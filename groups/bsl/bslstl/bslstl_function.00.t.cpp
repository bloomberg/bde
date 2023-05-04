// bslstl_function.00.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                            U_ENABLE_DEPRECATIONS
//
// Set 'U_ENABLE_DEPRECATIONS' to 1 get warnings about uses of deprecated
// methods.  These warnings are quite voluminous.  Test case 24 will fail
// unless '0 == U_ENABLE_DEPRECATIONS' to make sure we don't ship with these
// warnings enabled.
// ----------------------------------------------------------------------------

#undef  U_ENABLE_DEPRECATIONS
#define U_ENABLE_DEPRECATIONS 0
#if U_ENABLE_DEPRECATIONS
# define BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING 1
# include <bsls_deprecatefeature.h>
#endif

#include <bslstl_function.h>

// This test driver is split into several executables to enable compilation on
// platforms/compilers where the complete test code (with its many template
// instantiations) ends up needing too much resources during compilation,
// making it impossible to compile the code, or extremely slow.
//
// Please consult the test plan to see which test driver executable (part) runs
// which test cases.  Note that this test driver is special in that it has a
// "part 10", which is self contained (not included here) with a test case that
// must be compiled separately to work (due to compiler issues).
//
// This test driver file contains all the 'bslstl_function' test code and it is
// split into parts using conditional compilation.  It is '#include'd in the
// individual test parts (1-9) after defining the
// 'BSLSTL_FUNCTION_TEST_PART_NUMBER' macro to the part number of the including
// file.  When 'BSLSTL_FUNCTION_TEST_PART_NUMBER' is defined and its value is
// between 1-9 only the test code necessary for that part will be compiled,
// including conditional compilation of some common code (to avoid "unused"
// warnings) and of course the test cases in the 'main' function.
// This method of splitting up a test driver to multiple test executables keeps
// all the test code in one place (file), but still allows us to build (or to
// build in reasonable time) the test driver.
//
// Note that to make sure that an IDE or smart editor does not gray out all the
// code when editing if 'BSLSTL_FUNCTION_TEST_PART_NUMBER' equals to zero *all*
// code is enabled (like a non-split test driver).  The code itself sets that
// value if the '__INTELLISENSE__' (Microsoft IDE specific predefined) or
// either one of the 'BSLSTL_FUNCTION_TEST_ENABLE_ALL', or 'BDE_TARGET_EDITOR'
// are defined.  The 'BDE_TARGET_EDITOR' macro is meant to affect all
// split test drivers, while 'BSLSTL_FUNCTION_TEST_ENABLE_ALL' will only make
// all effective test code active in this component test driver files.
//
// Set your smart editor or IDE to define 'BSLSTL_FUNCTION_TEST_ENABLE_ALL'
// when editing this file for the syntax highlighting to show all "interesting"
// code active (not grayed out, or otherwise show inactive).  If you prefer all
// split test drivers to be shown that way (all test code active) just define
// 'BDE_TARGET_EDITOR' for your syntax-highlighting tool.
//
// When no control macro is defined this file (conditionally) compiles into an
// empty test driver that return success for test case 1, and -1 (no such test
// case) for test case number 2.  Test case number 1 returns success (and
// prints when run 'verbose') so that pedantic test runners won't warn that "no
// tests were run".

#ifndef BSLSTL_FUNCTION_TEST_PART_NUMBER

// Test part number not defined means: editing or compiling part 00
# if defined(__INTELLISENSE__) || defined(BDE_TARGET_EDITOR)
  // Allow the IDE to see all code.
#   define BSLSTL_FUNCTION_TEST_ENABLE_ALL
# else // not editing and no part number defined: we need a mock main
#   define BSLSTL_FUNCTION_TEST_COMPILING_NO_CASES
# endif

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER >= 1  \
   && BSLSTL_FUNCTION_TEST_PART_NUMBER <= 9
// Number is defined and in range
#define BSLSTL_FUNCTION_TEST_NEED_ASSERT
#define BSLSTL_FUNCTION_TEST_COMPILING_A_PART

#else  // If a bad test number is defined
#error 'BSLSTL_FUNCTION_TEST_PART_NUMBER' must be 1-9, or not defined.
Sun_Studio_only_gives_a_warning_for_pound_error;
#endif // out-of-range BSLSTL_FUNCTION_TEST_PART_NUMBER

// These large list of macros are defined to make the '#ifdef' lines of
// conditional code more readable (by making them shorter and easier for humans
// to grasp at a glance), and to make them consistent as well.  Being able to
// keep those preprocessor conditional directives looking the same (consistent)
// makes scanning the code (by humans) less error-prone, and faster.  The
// '_OR_'  macros are defined for the case when a heavy test case has to be
// split between two executable files (to keep the '#ifdef' on one line for
// consistency).
#if   BSLSTL_FUNCTION_TEST_PART_NUMBER == 1
#define BSLSTL_FUNCTION_TEST_PART_01

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 2
#define BSLSTL_FUNCTION_TEST_PART_02
#define BSLSTL_FUNCTION_TEST_PART_02_OR_03

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 3
#define BSLSTL_FUNCTION_TEST_PART_03
#define BSLSTL_FUNCTION_TEST_PART_02_OR_03

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 4
#define BSLSTL_FUNCTION_TEST_PART_04

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 5
#define BSLSTL_FUNCTION_TEST_PART_05

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 6
#define BSLSTL_FUNCTION_TEST_PART_06
#define BSLSTL_FUNCTION_TEST_PART_06_OR_07

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 7
#define BSLSTL_FUNCTION_TEST_PART_07
#define BSLSTL_FUNCTION_TEST_PART_06_OR_07

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 8
#define BSLSTL_FUNCTION_TEST_PART_08

#elif BSLSTL_FUNCTION_TEST_PART_NUMBER == 9
#define BSLSTL_FUNCTION_TEST_PART_09

#elif defined(BSLSTL_FUNCTION_TEST_ENABLE_ALL)
// Make sure no "interesting" (test) code is grayed out while editing in an IDE
// or smart editor, or enable the '00' executable to run all tests.
#define BSLSTL_FUNCTION_TEST_NEED_ASSERT

#define BSLSTL_FUNCTION_TEST_PART_01
#define BSLSTL_FUNCTION_TEST_PART_02
#define BSLSTL_FUNCTION_TEST_PART_03
#define BSLSTL_FUNCTION_TEST_PART_02_OR_03
#define BSLSTL_FUNCTION_TEST_PART_04
#define BSLSTL_FUNCTION_TEST_PART_05
#define BSLSTL_FUNCTION_TEST_PART_06
#define BSLSTL_FUNCTION_TEST_PART_07
#define BSLSTL_FUNCTION_TEST_PART_06_OR_07
#define BSLSTL_FUNCTION_TEST_PART_08
#define BSLSTL_FUNCTION_TEST_PART_09
#define BSLSTL_FUNCTION_TEST_COMPILING_A_PART
#endif

#ifndef BSLSTL_FUNCTION_TEST_ENABLE_ALL
#define BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(testCaseNumber)   \
    printf("Test case %d is skipped in this executable.\n", testCaseNumber)
    // So we can define the message just once.
#endif


#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART
// When a test part is compiled we need to include the necessary headers.  This
// section includes headers used by all parts.

#include <bsla_maybeunused.h>

#include <bslalg_constructorproxy.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_isreferencewrapper.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_macrorepeat.h>
#include <bsls_nullptr.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <algorithm>   // for 'std::swap'
#include <functional>  // For 'std::plus'
#include <utility>     // For 'std::move'

#include <climits>  // for 'INT_MAX'
#include <cstddef>  // for 'std::size_t'
#include <cstring>  // for 'std::str{cpy|cat}', 'std::mem{cpy|set|move}'
#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

#ifdef BSLSTL_FUNCTION_TEST_PART_01
#include <bslmf_removepointer.h>
#endif
// **NOT** using '#else' on purpose, because in an IDE *both* macros are
// defined, so none of the code is grayed out.
#ifdef BSLSTL_FUNCTION_TEST_PART_09
#include <bslmf_issame.h>
#include <bslmf_usesallocator.h>
#endif
// **NOT** using '#else' on purpose, because in an IDE *both* macros are
// defined, so none of the code is grayed out.

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -AL01   // class needs allocator() method
#pragma bde_verify -AP02   // class needs d_allocator_p member
#pragma bde_verify -AQK01  // Need #include <c-include> for 'symbol'
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FE01   // Exception type not derived from std::exception
#pragma bde_verify -IND02  // Function parameters should be on a single line
#pragma bde_verify -MN01   // class data members must be private

#pragma bde_verify append dictionary src tbd unspecialized  // Doesn't work ??
#endif

#include <stdio.h>   // for 'puts', 'printf'
#include <stdlib.h>  // for 'atoi'

using namespace BloombergLP;
using namespace bsl;

// COMPILE-FAIL CONFIGURATION MACROS
// ---------------------------------
//  Uncomment the following macros to produce the defined number of compiler
//  errors, for a correct implementation.
//#define BSLSTL_FUNCTION_TEST_BAD_COMPARISON 6
//#define BSLSTL_FUNCTION_TEST_BAD_SWAPS      4


// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time and enable compiling by certain
// compilers (that easily run out of resources), the test driver has been
// split into parts.  Note that because this is one of the most complicated
// templated constructs in our library certain test cases also had to be split
// up to achieve reasonable compilation times:
//
// 'bslstl_function.00.t.cpp': (this file, does not execute code)
// 'bslstl_function.01.t.cpp': -1: FAIL-TO-COMPILE TESTS
//                              1: BREATHING TEST
//                              2: PRIMITIVE CONSTRUCTORS AND BASIC ACCESSORS
//                              3: 'TrackableValue' TEST INFRASTRUCTURE
//                              4: DEFAULT AND 'nullptr' CONSTRUCTORS
// 'bslstl_function.02.t.cpp':  5: CONSTRUCT FROM CALLABLE OBJECT: PART 1
// 'bslstl_function.03.t.cpp':  5: CONSTRUCT FROM CALLABLE OBJECT: PART 2
//                              6: COPY CONSTRUCTORS
// 'bslstl_function.04.t.cpp':  7: MOVE CONSTRUCTORS
// 'bslstl_function.05.t.cpp':  8: SWAP
//                              9: COPY AND MOVE ASSIGNMENT
//                             10: ASSIGNMENT FROM 'nullptr'
// 'bslstl_function.06.t.cpp': 11: ASSIGNMENT FROM FUNCTOR: PART 1
// 'bslstl_function.07.t.cpp': 11: ASSIGNMENT FROM FUNCTOR: PART 2
// 'bslstl_function.08.t.cpp': 12: COMPARISON TO NULLPTR
//                             13: EMPTY FUNCTION INVOCATION
//                             14: POINTER TO FUNCTION INVOCATION
//                             15: POINTER TO MEMBER FUNCTION INVOCATION
// 'bslstl_function.09.t.cpp': 16: POINTER TO MEMBER DATA INVOCATION
//                             17: FUNCTION OBJECT INVOCATION
//                             18: TYPES AND TRAITS
//                             19: SUNCC BUG FIX
//                             20: DRQS94831150 BUG FIX
//                             21: CONSTRUCTOR SFINAE
//                             22: CONVERSION TO 'bdef_Function'
//                             23: CLASS TEMPLATE DEDUCTION GUIDES
//                             24: USAGE EXAMPLES
// 'bslstl_function.10.t.cpp': Tests that must be in a separate file
//
// The 'bsl::function' class template is an in-core value-semantic class that
// generalizes the notion of a callable object.  A 'function' object wraps a
// pointer to function, pointer to member function, pointer to member data,
// function object, or 'bsl::reference_wrapper' to one of the preceeding.  It
// can also be "empty", i.e., wrap no object.  The salient attributes of a
// 'function' object are whether its empty and, if not, the value of the
// callable object that it wraps.  Because the callable object (target) is
// type-erased on construction, the run-time type of the target is a salient
// attribute as well.  A 'function' object also has an allocator, which is not
// a salient attribute.
//
// A 'function' object can be modified via assignment.  There are no
// "primitive" manipulators; the entire value of the 'function' is modified as
// a single operation.  The *primitive* *accessors* are the conversion to
// 'bool', which indicates whether the 'function' is empty, the 'target_type'
// method, which returns a 'type_info' reference for type-erased target object,
// and the 'target' method, which returns a pointer to the target.  This test
// driver tests each of the standard allocator-aware, value-semantic operations
// except that there are no equality-comparison operators and, as an in-core
// type only, there are no output or streaming operations.  Additionally, it
// tests invocation of the 'function' object.
//
// The biggest complication in testing is verifying the large number of
// combinations of callable types and invocation prototypes (0 to 20 arguments,
// with and without return types).  When the callable type is a functor, it
// might fit in the small-object buffer or not, use an allocator or not, have a
// nothrow move constructor or not, and bit bitwise movable or not.  Black-box
// testing consists of constructing 'function' objects with a variety of
// callable types and testing that invocation works correctly and that
// 'operator bool' and the other accessors return the expected values.
//
// White-box testing looks at the memory allocation pattern.  The callable
// object is type erased and potentially stored in allocated memory.  The
// implementation uses the small-object optimization to store the type-erased
// callable object in the footprint of the 'function' object itself, when
// possible.  In order to ensure that the 'function' move constructor (and
// swap) never throws, a callable object with a throwing move constructor is
// never stored in the small object buffer unless wrapped in a
// 'bslmf::NothrowMoveWrapper'.
// ----------------------------------------------------------------------------
// TRAITS
// [18] bslma::UsesBslmaAllocator<function<PROTOTYPE> >
// [18] bslmf::UsesAllocatorArgT<function<PROTOTYPE> >
// [18] bsl::uses_allocator<function<PROTOTYPE>, bsl::allocator<char> >
// [18] bsl::is_nothrow_move_constructible<function<PROTOTYPE> >
//
// TYPES
// [18] allocator_type
// [18] result_type
// [18] argument_type
// [18] first_argument_type
// [18] second_argument_type
//
// CREATORS
// [ 4] function() noexcept;
// [ 4] function(nullptr_t) noexcept;
// [ 4] function(allocator_arg_t, const allocator_type& a);
// [ 4] function(allocator_arg_t, const allocator_type& a, nullptr_t);
// [ 5] function(FUNC f);
// [ 5] function(allocator_arg_t, const allocator_type& a, FUNC func);
// [ 6] function(const function& other);
// [ 6] function(allocator_arg_t, const allocator_type& a,
//               const function& other);
// [ 7] function(function&& other);
// [ 7] function(allocator_arg_t, const allocator_type& a,
//               function&& other);
// [ 5] ~function();
//
// MANIPULATORS
// [ 9] function& operator=(const function& rhs);
// [ 9] function& operator=(function&& rhs);
// [10] function& operator=(nullptr_t) noexcept;
// [11] function& operator=(FUNC&& rhs);
// [11] function& operator=(bsl::reference_wrapper<FUNC> rhs);
// [13] RET operator()(ARGS...) const; // No target
// [14] RET operator()(ARGS...) const; // function pointer target
// [15] RET operator()(ARGS...) const; // member-function pointer target
// [16] RET operator()(ARGS...) const; // data-member pointer target
// [17] RET operator()(ARGS...) const; // User-defined functor target
// [ 8] void swap(function& other) noexcept;
// [ 2] T      * target<T>();
//
// ACCESSORS
// [ 2] operator bool() const noexcept;
// [ 4] allocator_type get_allocator() const;
// [ 2] const std::type_info& target_type() const noexcept;
// [ 2] T const* target<T>() const;
//
// INTERNAL DEPRECATED METHODS
// [22] operator BloombergLP::bdef_Function<PROTOTYPE *>&();
// [22] const operator BloombergLP::bdef_Function<PROTOTYPE *>&() const;
// [ 4] BloombergLP::bslma::Allocator *allocator() const noexcept;
// [ 5] bool isInplace() const noexcept;
//
// FREE FUNCTIONS
// [12] bool operator==(const function<FUNC>& f, nullptr_t) noexcept;
// [12] bool operator==(nullptr_t, const function<FUNC>& f) noexcept;
// [12] bool operator!=(const function<FUNC>& f, nullptr_t) noexcept;
// [12] bool operator!=(nullptr_t, const function<FUNC>& f) noexcept;
// [ 8] void swap(function<FUNC>& a, function<FUNC>& b) noexcept;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [25] USAGE EXAMPLE
// [ 3] 'TrackableValue' TEST INFRASTRUCTURE
// [ 2] PRIMITIVE CONSTRUCTORS (BOOTSTRAP)
// [23] CLASS TEMPLATE DEDUCTION GUIDES
// [24] 0 == U_ENABLE_DEPRECATIONS
// [ 6] CONCERN: Construction from 'bdef_Function' does not double-wrap
// [ 9] CONCERN: Assignment from 'bdef_Function' does not double-wrap
// [19] CONCERN: Workaround for SunCC bug
// [20] CONCERN: Workaround for MSVC compiler bug (DRQS 94831150)
// [21] CONCERN: Constructor SFINAE prevents overload resolution ambiguities.
// [-1] CONCERN: 'a == b' does not compile for 'function' types.
// [-1] CONCERN: 'swap' does not compile for different 'function' types.
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

#ifdef BSLSTL_FUNCTION_TEST_NEED_ASSERT
void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}
#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT) && \
    defined(BDE_BUILD_TARGET_EXC)
# define ASSERT_NOEXCEPT(RESULT, EXPRESSION) \
         ASSERT(RESULT == noexcept(EXPRESSION))
#else
# define ASSERT_NOEXCEPT(RESULT, EXPRESSION)
#endif  // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

// ============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bool         verbose = false;
bool     veryVerbose = false;
bool veryVeryVerbose = false;

// ============================================================================
//                          EXCEPTION TEST MACROS
// ----------------------------------------------------------------------------

#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART

class ExceptionLimit
{
    // This class acts as a counter that, when decremented to zero, causes an
    // exception to be thrown.  It is used in exception testing to verify the
    // behavior of the test when an exception is thrown on, e.g., a move or
    // copy operation, after some number of successful operations.  The
    // exception-throwing behavior is disabled by setting the counter to -1.

    int         d_counter;
    const char *d_name_p;

  public:
    ExceptionLimit(const char *name)                                // IMPLICIT
        : d_counter(-1), d_name_p(name)
    { }

    ExceptionLimit& operator=(int newCount) {
        d_counter = newCount;
        return *this;
    }

    ExceptionLimit& operator--() {
        // Decrement 'counter'.  Throw 'this' on transition from zero to
        // negative.
#ifdef BDE_BUILD_TARGET_EXC
        if (d_counter >= 0 && --d_counter < 0) {
            throw *this;
        }
#endif
        return *this;
    }

    int value() const { return d_counter; }
    const char *what() const BSLS_NOTHROW_SPEC { return d_name_p; }
};

// These macros are used to test for exception neutrality in the case where
// either an optionally-specified allocator throws an exception or an
// optionally-specified exception limit is reached.  If both the allocator and
// the exception limit are non-null, the exception test is run twice, once
// testing the allocator limit and a second time testing the other exception
// limit.  If an allocator is specified, it must be a 'bslma::TestAllocator*'.
//
// The simple usage of these macros is as follows (curly braces required):
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(&alloc, &copyMoveLimit) {
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;
//          verify-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..
// If there are local variables that need to be constructed each time through
// the exception loop with the exception trigger turned off, they should be
// defined between the 'EXCEPTION_TEST_BEGIN' and 'EXCEPTION_TEST_TRY':
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(&alloc, 0) { // Allocator limit only
//      Obj localVariable(alloc);     // Won't throw a test exception
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;    // Might throw a test exception
//          verify-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..
// Sometimes, we need to test postconditions on failure.  For example, if
// assignment fails with an exception, both the lhs and rhs should be
// unchanged.  For these cases, the usage is expanded to include
// 'EXCEPTION_TEST_CATCH':
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(alloc, 0) {
//      Obj localVariable(alloc);
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;
//          verify-success-post-conditions;
//      } EXCEPTION_TEST_CATCH {
//          verify-failure-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..

#ifdef BDE_BUILD_TARGET_EXC

inline
void dumpExTest(const char *s,
                int         bslmaExceptionCounter,
                const char *exLimitName,
                int         exLimitCounter)
{
    if (! veryVeryVerbose) return;                                    // RETURN

    if (bslmaExceptionCounter >= 0) {
        printf("\t***   %s: alloc limit = %d ***\n", s, bslmaExceptionCounter);
    }
    else if (exLimitCounter >= 0) {
        printf("\t***   %s: %s = %d ***\n", s, exLimitName, exLimitCounter);
    }
}

// Run an exception test repeatedly using up to two exception counters: If a
// test allocator is specified, then its built-in exception limit is used.  If
// the address an exception-limit is specified, then it is used to set the
// exception limit.  If both are specified, The test is performed first by
// running through the allocator exception limit, then by running through the
// specified 'exceptionLimit'.  If both are null, then the test is run without
// forcing any exceptions.
#define EXCEPTION_TEST_BEGIN(testAllocator, exceptionLimit) do {              \
    if (veryVeryVerbose) printf("\t*** EXCEPTION_TEST_BEGIN ***\n");          \
    bslma::TestAllocator *testAlloc = (testAllocator);                        \
    ExceptionLimit *exLimit = (exceptionLimit);                               \
    int bslmaExceptionCounter = testAlloc ? 0 : -1;                           \
    int exLimitCounter = (exLimit && ! testAlloc) ? 0 : -1;                   \
    const char *const limitName = exLimit ? exLimit->what() : "(ignored)";    \
    do {                                                                      \
        bool exceptionCaught = false;                                         \
        if (testAlloc) testAlloc->setAllocationLimit(-1);                     \
        if (exLimit) *exLimit = -1;

#define EXCEPTION_TEST_TRY                                                    \
        if (testAlloc) testAlloc->setAllocationLimit(bslmaExceptionCounter);  \
        if (exLimit) *exLimit = exLimitCounter;                               \
        try

#define EXCEPTION_TEST_CATCH                                                  \
        catch (...) {                                                         \
            dumpExTest("EXCEPTION CAUGHT",                                    \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            exceptionCaught = true;                                           \
            if (testAlloc) testAlloc->setAllocationLimit(-1);                 \
            if (exLimit) *exLimit = -1;                                       \
        }                                                                     \
        if (exceptionCaught) try

#define EXCEPTION_TEST_ENDTRY                                                 \
        catch (...) {                                                         \
            dumpExTest("EXCEPTION CAUGHT",                                    \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            exceptionCaught = true;                                           \
        }                                                                     \
        if (exceptionCaught) {                                                \
            if (bslmaExceptionCounter >= 0) {                                 \
                ++bslmaExceptionCounter;                                      \
            } else if (exLimit) {                                             \
                ++exLimitCounter;                                             \
            }                                                                 \
        } else {                                                              \
            dumpExTest("SUCCEEDED WITH NO EXCEPTION",                         \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            if (exLimitCounter >= 0 || 0 == exLimit) {                     \
                break;                                                        \
            }                                                                 \
            bslmaExceptionCounter = -1;                                       \
            exLimitCounter = 0;                                               \
        }

#define EXCEPTION_TEST_END                                                    \
    } while (true);                                                           \
    if (testAlloc) testAlloc->setAllocationLimit(-1);                         \
    if (exLimit) *exLimit = -1;                                               \
    if (veryVeryVerbose) printf("\t*** EXCEPTION_TEST_END ***\n");            \
} while (false)

#else // if ! BDE_BUILD_TARGET_EXC

#define EXCEPTION_TEST_BEGIN(testAllocator, exceptionLimit) do { \
    (void) (testAllocator); (void) (exceptionLimit);
#define EXCEPTION_TEST_TRY do { if (true)
#define EXCEPTION_TEST_CATCH else
#define EXCEPTION_TEST_ENDTRY } while (false)
#define EXCEPTION_TEST_END } while (false)

#endif // BDE_BUILD_TARGET_EXC


// ============================================================================
//                      GLOBAL DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#define CPP_20 1
#else
#define CPP_20 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define MSVC 1
#else
#define MSVC 0
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1900
#define MSVC_2015 1
#else
#define MSVC_2015 0
#endif

namespace {

// Size type used by test allocator.
typedef bsls::Types::Int64 AllocSizeType;

// Use this test allocator when another allocator is not specified.
bslma::TestAllocator defaultTestAllocator("defaultTestAllocator");

#define NTWRAP(r)   bslalg::NothrowMovableUtil::wrap(r)
#define NTUNWRAP(r) bslalg::NothrowMovableUtil::unwrap(r)

                // --------------------
                // Class template RWrap
                // --------------------

template <class TYPE>
class RWrap {
    // This class represents an ersatz 'bsl::reference_wrapper', that is
    // understood to be a reference wrapper for the purpose of detecting
    // invocability during overload resolution when constructing
    // 'bsl::function' objects.

  public:
    // ACCESSORS
    operator TYPE&() const BSLS_KEYWORD_NOEXCEPT;
        // This function is declared but not defined.

    TYPE& get() const BSLS_KEYWORD_NOEXCEPT;
        // This function is declared but not defined.
};

} // close unnamed namespace

namespace BloombergLP {
namespace bslmf {

template <class TYPE>
struct IsReferenceWrapper<RWrap<TYPE> > : bsl::true_type {
};

}  // close namespace bslmf
}  // close enterprise namespace

namespace {

                // -----------------------
                // Class template SmartPtr
                // -----------------------

template <class TYPE>
class SmartPtr
{
    // A simple class with the interface of a smart pointer.

    TYPE *d_obj_p;

  public:
    typedef TYPE value_type;

    SmartPtr(TYPE *p = 0) : d_obj_p(p) { }                          // IMPLICIT

    TYPE& operator*() const  { return *d_obj_p; }
    TYPE* operator->() const { return  d_obj_p; }
};

#define INT_ARGN(n) int arg ## n
#define ARGN(n) arg ## n

#define SUMMING_FUNC(n)                                       \
    BSLA_MAYBE_UNUSED                                         \
    int sum ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {      \
        return BSLS_MACROREPEAT_SEP(n, ARGN, +) + 0x4000;     \
    }

// Increment '*val' by one.  Used to test a void return type.
BSLA_MAYBE_UNUSED
void increment(int *val)
{
    ++*val;
}

// Do nothing, but provide a function with a default argument, for testing
// constructibility from such a function.  See test case 5 ("CONSTRUCT FROM
// CALLABLE OBJECT") for more information about the compiler defects for which
// this function is used to test 'bsl::function's workaround.
BSLA_MAYBE_UNUSED
void functionWithDefaultArgument(int = 0)
{
}

// Create 11 functions with 0 to 13 integer arguments, returning the sum of the
// arguments + 0x4000.
SUMMING_FUNC(0)
BSLS_MACROREPEAT(13, SUMMING_FUNC)

                // ----------------------
                // Class ConvertibleToInt
                // ----------------------

class ConvertibleToInt
{
    // Class of objects implicitly convertible to 'int'

    int d_value;

  public:
    explicit ConvertibleToInt(int v) : d_value(v) { }

    operator int() const { return d_value; }
};

                // ----------------
                // Class IntWrapper
                // ----------------

class IntWrapper
{
    // Simple wrapper around an 'int' that supplies member functions (whose
    // address can be taken) for testing 'bsl::function'.  This wrapper also
    // provides a call operator that returns the value

    int d_value;

  public:
    IntWrapper(int i = 0) : d_value(i) { }                          // IMPLICIT
    IntWrapper(ConvertibleToInt i) : d_value(i) { }                 // IMPLICIT

#define ADD_FUNC(n)                                                  \
    int add ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) const {       \
        return value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);           \
    }

#define INCREMENT_FUNC(n)                                            \
    int increment ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {       \
        return d_value += BSLS_MACROREPEAT_SEP(n, ARGN, +);          \
    }

#define VOID_INCREMENT_FUNC(n)                                       \
    void voidIncrement ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {  \
        d_value += BSLS_MACROREPEAT_SEP(n, ARGN, +);                 \
    }

    int add0() const { return d_value; }
    BSLS_MACROREPEAT(12, ADD_FUNC)
        // Const function with 0 to 12 arguments.  Return value() plus the sum
        // of all arguments.

    int increment0() { return d_value; }
    BSLS_MACROREPEAT(12, INCREMENT_FUNC)
        // Mutable function with 0 to 12 arguments.  Increment the value by the
        // sum of all arguments and return the new value.  'increment0()' is a
        // no-op.

    void incrementBy1() { ++d_value; }

    int sub1(int arg) { return value() - arg; }

    int value() const { return d_value; }

    int operator()(int increment = 0) const { return d_value + increment; }
        // Return the current value added to the optionally specified
        // 'increment' (default 0).

    void voidIncrement0() { }
    BSLS_MACROREPEAT(12, VOID_INCREMENT_FUNC)
        // Mutable function with 0 to 12 arguments.  Increment the value by the
        // sum of all arguments.  'voidIncrement0()' is a no-op.
};

inline bool operator==(const IntWrapper& a, const IntWrapper& b)
{
    return a.value() == b.value();
}

BSLA_MAYBE_UNUSED
inline bool operator!=(const IntWrapper& a, const IntWrapper& b)
{
    return a.value() != b.value();
}

                // -----------------------
                // Class IntWrapperDerived
                // -----------------------

class IntWrapperDerived : public IntWrapper
{
    // Derived class of 'IntWrapper'

  public:
    IntWrapperDerived(int v) : IntWrapper(v) { }                    // IMPLICIT
};

                // ----------------
                // Struct IntHolder
                // ----------------

struct IntHolder {
    // PUBLIC DATA
    int d_value;

    // CREATORS
    IntHolder(int value)                                            // IMPLICIT
    : d_value(value)
    {
    }

    // ACCESSORS
    int value() const { return d_value; }
};

                // ---------------------
                // Struct ConstIntHolder
                // ---------------------

struct ConstIntHolder {
    // PUBLIC DATA
    const int d_value;

    // CREATORS
    ConstIntHolder(int value)                                       // IMPLICIT
    : d_value(value)
    {
    }

    ConstIntHolder(const ConstIntHolder& other)
        // Create a new 'ConstIntHolder' object with the value of the specified
        // 'other'.  Note that We need to provide this copy constructor because
        // compilers warn that its automatic generation is deprecated as we
        // have defined a copy assignment operator.  (And the code has to
        // compile under C++03, which does not have '= default'.)
    : d_value(other.d_value)
    {
    }

    // MANIPULATORS
    ConstIntHolder& operator=(const ConstIntHolder& rhs)
        // Assign the value of the specified 'rhs' to this object and return a
        // reference providing modifiable access to this object.  The behavior
        // is undefined unless 'rhs' has the same value of this object upon
        // invocation.  Note that this contract may seem silly, tautological,
        // and unusably restrictive, but it provides just enough capability to
        // support the syntactic needs of this test in a well-defined manner.
    {
        ///Implementation Note
        ///-------------------
        // The assignment operator for class types having a 'const' data member
        // is not implicitly defined.  Infrastructure in this test requires
        // that these "int holder" types be copy-assignable.  The copy
        // assignment operation for a 'ConstIntHolder' can only be well-defined
        // if the 'rhs' has the same value as 'this' so that assignment does
        // not end up changing 'd_value'.

        ASSERT(rhs.d_value == d_value);
        return *this;
    }

    // ACCESSORS
    int value() const { return d_value; }
};

                // -----------------------
                // Struct IntHolderDerived
                // -----------------------

struct IntHolderDerived : IntHolder {
    // CREATORS
    IntHolderDerived(int value)                                     // IMPLICIT
    : IntHolder(value)
    {
    }
};

                // --------------------------------
                // Struct IntHolderVirtuallyDerived
                // --------------------------------

struct IntHolderVirtuallyDerived_Base1 : virtual IntHolder {
    // CREATORS
    IntHolderVirtuallyDerived_Base1(int value)                      // IMPLICIT
    : IntHolder(value)
    {
    }
};

struct IntHolderVirtuallyDerived_Base2 : virtual IntHolder {
    // CREATORS
    IntHolderVirtuallyDerived_Base2(int value)                      // IMPLICIT
    : IntHolder(value)
    {
    }
};

struct IntHolderVirtuallyDerived : IntHolderVirtuallyDerived_Base1,
                                   IntHolderVirtuallyDerived_Base2 {
    // CREATORS
    IntHolderVirtuallyDerived(int value)                            // IMPLICIT
    : IntHolder(value)
    , IntHolderVirtuallyDerived_Base1(value)
    , IntHolderVirtuallyDerived_Base2(value)
    {
    }
};

#ifdef BSLSTL_FUNCTION_TEST_PART_08
int       *getAddress(           int& r) { return &r; }
const int *getConstAddress(const int& r) { return &r; }
#endif  // BSLSTL_FUNCTION_TEST_PART_08

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class DEST, class SRC>
inline DEST&& upcastMovableRef(SRC&& ref) {
    return bslmf::MovableRefUtil::move(ref);
}
#else
template <class DEST, class SRC>
inline bslmf::MovableRef<DEST> upcastMovableRef(bslmf::MovableRef<SRC> ref)
    // Convert from 'bslmf::MovableRef<SRC>' to 'bslmf::MovableRef<DEST>',
    // where 'DEST' is a base class of 'SRC'.
{
    DEST& dRef = bslmf::MovableRefUtil::access(ref); // Upcast
    return bslmf::MovableRefUtil::move(dRef);
}
#endif

// Prototype and common function type used in most tests
typedef int                  PROTO(const IntWrapper&, int);
typedef bsl::function<PROTO> Obj;

// An object of type 'InnerFunction' is a 'bsl::function' that is
// compatible-with, but has a different type than, 'Obj'.  If stored in an
// 'Obj', the target of the 'InnerFunction' is effectively wrapped twice, e.g.,
// 'Obj(InnerFunction(&simpleFunc))' produces a pointer to 'simpleFunc' wrapped
// inside a 'InnerFunction', wrapped inside an 'Obj'.  This type is used to
// test that such double wrapping does not cause any issues.  To simplify this
// test driver, objects of type 'InnerFunction' are always either empty or have
// a target of type 'int(*)(const IntWrapper& iw, int v)'.
typedef int PROTOTYPE2(const IntWrapper&, const int&);
typedef bsl::function<PROTOTYPE2> InnerFunction;

template <class TYPE>
bool eqTarget(const TYPE& lhs, const TYPE& rhs)
    // Return true if the specified 'lhs' compares equal to the specified 'rhs'
    // using 'operator=='.  This function is overloaded for types that don't
    // have 'operator=='.
{
    return lhs == rhs;
}

template <class TYPE>
bool eqTarget(const TYPE& lhs, const bslalg::NothrowMovableWrapper<TYPE>& rhs)
    // 'eqTarget` overload for case where 'rhs' is wrapped.
{
    return eqTarget(lhs, NTUNWRAP(rhs));
}

template <class TYPE>
bool eqTarget(const bslalg::NothrowMovableWrapper<TYPE>& lhs, const TYPE& rhs)
    // 'eqTarget` overload for case where 'lhs' is wrapped.
{
    return eqTarget(NTUNWRAP(lhs), rhs);
}

template <class TYPE>
bool eqTarget(const bslalg::NothrowMovableWrapper<TYPE>& lhs,
              const bslalg::NothrowMovableWrapper<TYPE>& rhs)
    // 'eqTarget` overload for case where both 'lhs' and 'rhs' are wrapped.
{
    return eqTarget(NTUNWRAP(lhs), NTUNWRAP(rhs));
}

template <class PROTO>
bool eqTarget(const bsl::function<PROTO>& lhs, const bsl::function<PROTO>& rhs)
    // 'eqTarget' overload for case where the specified 'lhs' and 'rhs' are
    // instantiations of 'bsl::function'.  Since 'bsl::function' does not
    // support 'operator==', this function approximates equality comparison for
    // the limited cases of empty 'bsl::function' and 'bsl::function'
    // containing a target of type 'int(*)(const IntWrapper& iw, int v)', only.
    // Test cases are designed so that, if a 'bsl::function' is used as a
    // target type, it is always one of these two cases.
{
    typedef int (*TargetType)(const IntWrapper&, int);

    const TargetType *lhsTarget = lhs.template target<TargetType>();
    const TargetType *rhsTarget = rhs.template target<TargetType>();

    ASSERT(lhsTarget || !lhs);  // Holds known target type or else empty.
    ASSERT(rhsTarget || !rhs);  // Holds known target type or else empty.

    // Return true if 'lhs' and 'rhs' have the same target or both are empty.
    return (lhsTarget && rhsTarget) ? *lhsTarget == *rhsTarget : !lhs == !rhs;
}

template <class FUNC>
bool eqTarget(const bsl::reference_wrapper<FUNC>& lhs, const FUNC& rhs)
    // 'eqTarget' overload for case where the specified 'lhs' is a reference
    // wrapped function and 'rhs' is a function.  It checks the function
    // wrapped by 'lhs' is the same as 'rhs'.  This specialization facilitates
    // the comparison done by 'testAssignFromFunctorImp' between
    // '*function(reference_wrapper(rhsIn)).target<reference_wrapper<FUNC>>()'
    // and 'rhsIn' during reference wrapping tests.
{
    return eqTarget(lhs.get(), rhs);
}

#ifdef BSLSTL_FUNCTION_TEST_PART_08
class CountCopies
{
    // Counts the number of times an object has been copy-constructed or
    // move-constructed.

    int d_numCopies;

  public:
    CountCopies() : d_numCopies(0) { }
    CountCopies(const CountCopies& other) : d_numCopies(other.d_numCopies+1) {}

    // Move constructor does not bump count
    CountCopies(bslmf::MovableRef<CountCopies> other)
        : d_numCopies(bslmf::MovableRefUtil::access(other).d_numCopies) {}

    CountCopies& operator=(const CountCopies& rhs)
        { d_numCopies = rhs.d_numCopies + 1; return *this; }

    int numCopies() const { return d_numCopies; }
};

// Return the number of times the specified 'cc' object has been copied.  Note
// that, since 'cc' is passed by value, there is a copy or move on every call.
// Therefore, the return value should never be less than 1.
inline
int numCopies(CountCopies cc) { return cc.numCopies(); }
#endif  // BSLSTL_FUNCTION_TEST_PART_08

// Whitebox: Small object optimization buffer size
static const std::size_t k_SMALL_OBJECT_BUFFER_SIZE =
    sizeof(bslstl::Function_SmallObjectOptimization::InplaceBuffer);

// Simple functions
int simpleFunc(const IntWrapper& iw, int v)
{
    return iw.value() + v;
}

BSLA_MAYBE_UNUSED
int simpleFunc2(const IntWrapper& iw, int v)
{
    return iw.value() - v;
}

#ifdef BSLSTL_FUNCTION_TEST_PART_01
#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
void voidFunc() {}
#endif
#endif  // BSLSTL_FUNCTION_TEST_PART_01

class CountingBase
{
    // Base class to keep count of the number of objects in existence
    static int s_count;

  public:
    CountingBase() { ++s_count; }
    CountingBase(const CountingBase&) { ++s_count; }
    CountingBase(bslmf::MovableRef<CountingBase>) { ++s_count; }
    ~CountingBase() { --s_count; ASSERT(s_count >= 0); }

    static int count() { return s_count; }
};

int CountingBase::s_count = 0;

class FunctorMonitor
{
    // An instance of this class can be used to check for a change in the
    // number of functor objects that have been created or destroyed during a
    // specific operation and ensure that every object created is destroyed in
    // the specific scope.  If the number of functor object in existence at
    // destruction is different than the number at construction, then a
    // diagnostic is printed and the test will fail.

    int d_line;     // source line number where 'FunctorMonitor' was created
    int d_snapshot; // Number of 'CountingBase' objects in existence at the
                    // time when this 'FunctorMonitor' was created.

  public:
    explicit FunctorMonitor(int line) { reset(line); }

    ~FunctorMonitor() {
        if (! isSameCount()) {
            printf("CountingBase::count(): %d\td_snapshot : %d\n",
                   CountingBase::count(), d_snapshot);
            aSsErT(1,"isSameCount() at destruction of FunctorMonitor", d_line);
        }
    }

    // MANIPULATORS
    void reset(int line) {
        d_line     = line;
        d_snapshot = CountingBase::count();
    }

    // ACCESSORS
    bool isSameCount() const { return CountingBase::count() == d_snapshot; }
};

// Limits the number of copies and moves before one of those operations throws.
ExceptionLimit copyMoveLimit("copy/move limit");

                // --------------------
                // Class TrackableValue
                // --------------------

class TrackableValue {
    // This class tracks a value through a series of move and copy operations
    // and has an easily-testable moved-from state.  An instance stores an
    // unsigned integer value and a pair of bits, one indicating if the value
    // was copied, another if it was moved (or neither or both).  When
    // assigned a value at construction, via assignment, or via a mutating
    // operation (such as +=), the move and copy bits are cleared, indicating
    // that the new value has been neither moved nor copied.  When assigned a
    // new value via copy construction or copy assignment, the copy bit is
    // set and the move bit is cleared.  When assigned a new value via move
    // construction, move assignment, or swap, the copy bit is transferred
    // from the original value and the move bit is set.  Thus a value that is
    // copied then moved will have both bits set but a value that is moved
    // then copy has only the copy bit set.  The copy and move bits are not
    // salient attributes of the value and are thus not used for testing
    // equality.

    // PRIVATE CONSTANTS
    enum {
        e_NUM_FLAGS       = 2,
        // Value is multiplied by the following value to make room for the
        // flag bits.  Multiply and divide are used to scale the value
        // because shift operations are not guaranteed to preserve sign.  The
        // compiler will almost certainly replace these multiplication and
        // divisions with more efficient arithmetic shift operations.
        e_VALUE_MULTIPLIER = 1 << e_NUM_FLAGS,
        e_FLAGS_MASK       = e_VALUE_MULTIPLIER - 1,
        e_VALUE_MASK       = ~e_FLAGS_MASK,
        e_COPIED_FLAG      = 0x01,
        e_MOVED_FLAG       = 0x02,
        e_MOVED_FROM_VAL   = 0x11111111, // Value assigned to moved-from object

        // The moved from state has a value of e_MOVED_FROM_VAL, with neither
        // flag set.
        e_MOVED_FROM_STATE = e_MOVED_FROM_VAL * e_VALUE_MULTIPLIER
    };

    // PRIVATE DATA
    int d_valueAndFlags;

    TrackableValue& setValue(int  v,
                             bool movedFlag = false,
                             bool copiedFlag = false);
        // Set the value to the specified 'v'.  Optionally specify 'movedFlag'
        // for setting the 'isMoved()' flag and optionally specify 'copiedFlag'
        // for setting the 'isCopied()' flag; otherwise each flag defaults to
        // false.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TrackableValue, bslmf::IsBitwiseMoveable);

    // Constants
    enum {
        e_MIN            = INT_MIN / e_VALUE_MULTIPLIER,
        e_MAX            = INT_MAX / e_VALUE_MULTIPLIER
    };

    // CREATORS
    TrackableValue(int v = 0)                                       // IMPLICIT
        // Create an object.  Set the value to the optionally specified 'v'
        // (default 0) and set 'isMoved()' and 'isCopied()' to false.
        { setValue(v); }

    TrackableValue(const TrackableValue& original)
        // Create an object with the value copied from the specified
        // 'original'.  Set 'isMoved()' to false and 'isCopied()' to true.
        { setValue(original.value(), false, true); }

    TrackableValue(bslmf::MovableRef<TrackableValue> original)
                                                          BSLS_KEYWORD_NOEXCEPT
        // Create an object with the value moved from the specified 'original'.
        // Set 'isMoved()' to true and 'isCopied()' to 'original.isCopied()',
        // then set 'original' to the moved-from state.
        { *this = bslmf::MovableRefUtil::move(original); }

    //! ~TrackableValue() = default;

    // MANIPULATORS
    TrackableValue& operator=(int v)
        // Set value to the specified 'v' and set 'isMoved()' and 'isCopied()'
        // to false; then return '*this'.
        { return setValue(v); }

    TrackableValue& operator=(const TrackableValue& rhs)
        // Set value to the specified 'rhs.value()' and set 'isMoved()' to
        // false and 'isCopied()' to true; then return '*this'.
        { return setValue(rhs.value(), false, true); }

    TrackableValue& operator=(bslmf::MovableRef<TrackableValue> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Move value from the specified 'rhs', set 'isMoved()' to true, and
        // set 'isCopied()' to 'rhs.isCopied()', then assign 'rhs' the value
        // 'e_MOVED_FROM_VAL' and return '*this'.

    void setIsCopiedRaw(bool copiedFlag);
    void setIsMovedRaw(bool movedFlag);
    void setValueRaw(int v);
        // Set the constituent parts of this object to the specified
        // 'copiedFlag', specified 'movedFlag', or specified 'v', without
        // modifying the other parts.  It is up to the caller to ensure that
        // the flags are set consistently.

    void swap(TrackableValue& other);
        // Exchange the values AND 'isCopied()' flags of '*this' and the
        // specified 'other' object, then set the 'isMoved()' flag of both
        // objects to true.

    void resetMoveCopiedFlags()
        // Set 'isMoved()' and 'isCopied() to false.
        { d_valueAndFlags &= e_VALUE_MASK; }

    // ACCESSORS
    bool isCopied()    const { return d_valueAndFlags & e_COPIED_FLAG; }
    bool isMoved()     const { return d_valueAndFlags & e_MOVED_FLAG; }
    bool isMovedFrom() const { return d_valueAndFlags == e_MOVED_FROM_STATE; }
    int  value()       const { return d_valueAndFlags / e_VALUE_MULTIPLIER; }
};

// TrackableValue FREE FUNCTIONS
inline bool operator==(const TrackableValue& a, const TrackableValue& b) {
    return a.value() == b.value();
}

inline bool operator!=(const TrackableValue& a, const TrackableValue& b) {
    return a.value() != b.value();
}

inline void swap(TrackableValue& a, TrackableValue& b) { a.swap(b); }

// IMPLEMENTATION of class TrackableValue
// --------------------------------------

inline
TrackableValue& TrackableValue::setValue(int  v,
                                         bool movedFlag,
                                         bool copiedFlag) {
    ASSERT(e_MIN <= v && v <= e_MAX);
    d_valueAndFlags = v * e_VALUE_MULTIPLIER;
    if (movedFlag)  d_valueAndFlags |= e_MOVED_FLAG;
    if (copiedFlag) d_valueAndFlags |= e_COPIED_FLAG;
    return *this;
}

inline
TrackableValue&
TrackableValue::operator=(bslmf::MovableRef<TrackableValue> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    TrackableValue& rhsRef = bslmf::MovableRefUtil::access(rhs);
    d_valueAndFlags = rhsRef.d_valueAndFlags | e_MOVED_FLAG;
    rhsRef.d_valueAndFlags = e_MOVED_FROM_STATE;
    return *this;
}

inline
void TrackableValue::setIsCopiedRaw(bool copiedFlag) {
    d_valueAndFlags &= (e_VALUE_MASK | e_MOVED_FLAG);
    if (copiedFlag) d_valueAndFlags |= e_COPIED_FLAG;
}

inline
void TrackableValue::setIsMovedRaw(bool movedFlag) {
    d_valueAndFlags &= (e_VALUE_MASK | e_COPIED_FLAG);
    if (movedFlag) d_valueAndFlags |= e_MOVED_FLAG;
}

inline
void TrackableValue::setValueRaw(int v) {
    ASSERT(e_MIN <= v && v <= e_MAX);
    d_valueAndFlags &= e_FLAGS_MASK;
    d_valueAndFlags |= v * e_VALUE_MULTIPLIER;
}

inline
void TrackableValue::swap(TrackableValue& other) {
    // Don't use std::swap<int> because don't want to #include <algorithm>
    int tmp = d_valueAndFlags;
    d_valueAndFlags = other.d_valueAndFlags;
    other.d_valueAndFlags = tmp;
    d_valueAndFlags       |= e_MOVED_FLAG;
    other.d_valueAndFlags |= e_MOVED_FLAG;
}

                // --------------------------
                // Trait template IsTrackable
                // --------------------------

template <class TYPE>
struct IsTrackable : bsl::false_type {
    // This trait evaluates to 'bsl::true_type' for opt-in types that have the
    // following member functions:
    //..
    //  bool isCopied() const;
    //  bool isMoved() const;
    //  bool isMovedFrom() const;
    //  void resetMoveCopiedFlags();
    //..
    // Specializing this trait to derive from 'true_type'
    // will turn the trait on for a type.
};

template <>
struct IsTrackable<TrackableValue> : bsl::true_type { };

                // --------------------------------------
                // Value tracking free function templates
                // --------------------------------------

template <class TYPE>
inline bool isCopied(const TYPE& obj, bool exp);
    // Return the value of 'isCopied()' for the specified 'obj' if
    // 'IsTrackable<TYPE>::value' is true; otherwise return the specified 'exp'
    // value.

template <class TYPE>
inline bool isMoved(const TYPE& obj, bool exp);
    // Return the value of 'isMoved()' for the specified 'obj' if
    // 'IsTrackable<TYPE>::value' is true; otherwise return the specified 'exp'
    // value.

template <class TYPE>
inline bool isMovedFrom(const TYPE& obj, bool exp);
    // Return the value of 'isMovedFrom()' for the specified 'obj' if
    // 'IsTrackable<TYPE>::value' is true; otherwise return the specified 'exp'
    // value.

template <class TYPE>
inline void resetMoveCopiedFlags(TYPE *objp);
    // Call 'obj.resetMoveCopiedFlags()' for the specified 'obj' if
    // 'IsTrackable<TYPE>::value' is true; otherwise do nothing.

// IMPLEMENTATION of value tracking free function templates
// --------------------------------------------------------

template <class TYPE>
inline bool isCopiedImp(const TYPE&, bool exp, bsl::false_type)
{
    return exp;
}

template <class TYPE>
inline bool isCopiedImp(const TYPE& obj, bool, bsl::true_type)
{
    return obj.isCopied();
}

template <class TYPE>
inline bool isCopied(const TYPE& obj, bool exp)
{
    return isCopiedImp(obj, exp, IsTrackable<TYPE>());
}

template <class TYPE>
inline bool isMovedImp(const TYPE&, bool exp, bsl::false_type)
{
    return exp;
}

template <class TYPE>
inline bool isMovedImp(const TYPE& obj, bool, bsl::true_type)
{
    return obj.isMoved();
}

template <class TYPE>
inline bool isMoved(const TYPE& obj, bool exp)
{
    return isMovedImp(obj, exp, IsTrackable<TYPE>());
}

template <class TYPE>
inline bool isMovedFromImp(const TYPE&, bool exp, bsl::false_type)
{
    return exp;
}

template <class TYPE>
inline bool isMovedFromImp(const TYPE& obj, bool, bsl::true_type)
{
    return obj.isMovedFrom();
}

template <class TYPE>
inline bool isMovedFrom(const TYPE& obj, bool exp)
    // Return the value of 'isMovedFrom()' for the specified 'obj' if 'TYPE'
    // has such a member function; otherwise return the specified 'exp' value.
{
    return isMovedFromImp(obj, exp, IsTrackable<TYPE>());
}

template <class TYPE>
inline void resetMoveCopiedFlagsImp(TYPE *, bsl::false_type)
{
}

template <class TYPE>
inline void resetMoveCopiedFlagsImp(TYPE *objp, bsl::true_type)
{
    objp->resetMoveCopiedFlags();
}

template <class TYPE>
inline void resetMoveCopiedFlags(TYPE *objp)
    // Return the value of 'obj.resetMoveCopiedFlags()' if the specified 'obj'
    // has such a member function; otherwise return the specified 'exp' value.
{
    resetMoveCopiedFlagsImp(objp, IsTrackable<TYPE>());
}

                // --------------------------
                // Class template TestFunctor
                // --------------------------

// Define a test functor that can be customized via a set of qualities.

enum FunctorQualities {
    // Bits for encoding the qualities of a functor.

    e_EMPTY_FUNCTOR       = 0x00,  // Empty class
    e_SMALL_FUNCTOR       = 0x01,  // Minimum non-empty size
    e_MEDIUM_FUNCTOR      = 0x02,  // Just fits in small-object buffer
    e_LARGE_FUNCTOR       = 0x03,  // Does not fit in small-object buffer
    e_SIZE_MASK           = 0x03,

    e_HAS_ALLOCATOR       = 0x04,
    e_IS_BITWISE_MOVABLE  = 0x08,
    e_IS_NOTHROW_MOVABLE  = 0x10
};

template <bool HAS_ALLOCATOR>
class TestFunctorMembers;

template <>
class TestFunctorMembers<false> {
    // Base class olding data member and dummy allocator mechanisms for a test
    // functor having no allocator.

  protected:
    struct Alloc {
        // Private non-allocator type
        bsl::true_type operator==(const Alloc&) const
            { return bsl::true_type(); }
        bsl::false_type operator!=(const Alloc&) const
            { return bsl::false_type(); }
    };

    TrackableValue    d_value;

    Alloc privateAllocator() const { return Alloc(); }
    void setPrivateAllocator(const Alloc&) { }
        // These setters/getters are no-ops and exist only to allow generic
        // code to compile.

  public:
    bool verifyAllocator(const bsl::allocator<char>&) const { return true; }
};

template <>
class TestFunctorMembers<true> {
    // Base class holding data member, allocator, and allocator-related public
    // member definitions for a test functor that has an allocator.

  protected:
    typedef bsl::allocator<char> Alloc;

    bslma::Allocator *d_allocator_p;
    TrackableValue    d_value;

    // Internal getters/setters for derived-class use only.
    Alloc privateAllocator() const { return d_allocator_p; }
    void setPrivateAllocator(const Alloc& a) { d_allocator_p = a.mechanism(); }

  public:
    typedef bsl::allocator<char> allocator_type;

    bslma::Allocator     *allocator()     const { return d_allocator_p; }
    bsl::allocator<char>  get_allocator() const { return d_allocator_p; }
        // Return the current allocator (old and new style, respectively).

    bool verifyAllocator(const bsl::allocator<char>& exp) const
        { return exp == d_allocator_p; }
};

template <bool HAS_ALLOCATOR, int SIZE_QUALITY>
class TestFunctorBase;

template <>
class TestFunctorBase<false /* HAS_ALLOCATOR */, e_EMPTY_FUNCTOR> {
    // Base class for empty (stateless) functor without allocator.

  protected:
    struct Alloc {
        // Private non-allocator type
        bsl::true_type operator==(const Alloc&) const
            { return bsl::true_type(); }
        bsl::false_type operator!=(const Alloc&) const
            { return bsl::false_type(); }
    };

    // CLASS DATA
    static TrackableValue d_value;  // Value is not instance-specific

    Alloc privateAllocator() const { return Alloc(); }
    void setPrivateAllocator(const Alloc&) { }
        // These setters/getters are no-ops and exist only to allow generic
        // code to compile.

  public:
    bool verifyAllocator(const bsl::allocator<char>&) const { return true; }
};

TrackableValue TestFunctorBase<false, e_EMPTY_FUNCTOR>::d_value;

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_SMALL_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a minimum-sized stateful functor.
};

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_MEDIUM_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a functor along with padding such
    // that just fits in the small-object buffer.

    typedef TestFunctorMembers<HAS_ALLOCATOR>               Members;
    typedef typename bsls::AlignmentFromType<Members>::Type MembersAlign;

    union {
        MembersAlign d_align;
        char         d_padding[k_SMALL_OBJECT_BUFFER_SIZE - sizeof(Members)];
    };
};

template <bool HAS_ALLOCATOR>
class TestFunctorBase<HAS_ALLOCATOR, e_LARGE_FUNCTOR>
    : public TestFunctorMembers<HAS_ALLOCATOR> {
    // Base class holding data members for a functor along with enough padding
    // such that it does not fit in the small-object buffer.

    typedef TestFunctorMembers<HAS_ALLOCATOR>               Members;
    typedef typename bsls::AlignmentFromType<Members>::Type MembersAlign;

    union {
        MembersAlign d_align;
        char         d_padding[1+k_SMALL_OBJECT_BUFFER_SIZE - sizeof(Members)];
    };
};

template <int QUALITIES>
class TestFunctor
    : public TestFunctorBase<bool(QUALITIES & e_HAS_ALLOCATOR),
                             QUALITIES & e_SIZE_MASK>
    , public CountingBase {

    // PRIVATE CONSTANTS
    enum { k_HAS_ALLOCATOR = bool(QUALITIES & e_HAS_ALLOCATOR) };

    // PRIVATE MEMBER FUNCTIONS
    int encode(int value) const
        // If 'QUALITIES & e_BITWISE_MOVABLE' is false, return an encoding of
        // the specified 'value' such that, it the result is stored in
        // 'd_value', then an accidentally bitwise move to a different copy of
        // 'TestFunctor' can be detected; otherwise return 'value' unchanged.
        // This encoding is its own inverse; i.e., 'encode(encode(v)) == v'.
    {
        if (QUALITIES & e_IS_BITWISE_MOVABLE) {
            return value;                                             // RETURN
        }

        std::size_t self = (std::size_t) this;  // Should be uintptr_t
        self /= sizeof(TrackableValue);         // Discard low-order zero bits.
        // Mask off high bits.
        self &= static_cast<std::size_t>(TrackableValue::e_MAX);
        return int(self ^ value);
    }

    typedef TestFunctorBase<bool(QUALITIES & e_HAS_ALLOCATOR),
                            QUALITIES & e_SIZE_MASK>           Base;

    typedef typename Base::Alloc                               Alloc;

  public:
    // General, customized functor for testing.  The specified 'QUALITIES'
    // parameter indicates size, allocator usage, and movability and is
    // encoded as the inclusive-or of the appropriate enumerations in
    // 'FunctorQualities'.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
                                      bslma::UsesBslmaAllocator,
                                      k_HAS_ALLOCATOR);

    BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
                                      bsl::is_nothrow_move_constructible,
                                      bool(QUALITIES & e_IS_NOTHROW_MOVABLE));

    // BSLMF_NESTED_TRAIT_DECLARATION_IF(TestFunctor,
    //                                 bslmf::IsBitwiseMoveable,
    //                                 bool(QUALITIES & e_IS_BITWISE_MOVABLE));
        // Defined out-of-line, below

    // CONSTANTS
    enum { k_IS_STATELESS = (e_EMPTY_FUNCTOR == (QUALITIES & e_SIZE_MASK)) };

    // CREATORS
    explicit TestFunctor(int value = 0, const Alloc& alloc = Alloc()) {
        this->setPrivateAllocator(alloc);
        this->setValue(value);
    }

    TestFunctor(const TestFunctor& original,
                const Alloc&       alloc = Alloc())
        // Copy construct from the specified 'original' using the
        // optionally-specified 'alloc' allocator (if any).
        : CountingBase(original)
    {
        this->setPrivateAllocator(alloc);
        operator=(original);
    }

    TestFunctor(bslmf::MovableRef<TestFunctor> original)            // IMPLICIT
        // Copy value from 'original', set 'isMoved()' to false and
        // 'isCopied()' to true.
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(bool(QUALITIES &
                                                 e_IS_NOTHROW_MOVABLE))
        // Move construct from the specified 'original' using the allocator
        // from 'original'.
    {
        TestFunctor& originalRef = original;
        this->setPrivateAllocator(originalRef.privateAllocator());
        operator=(bslmf::MovableRefUtil::move(originalRef));
    }

    TestFunctor(bslmf::MovableRef<TestFunctor> original, const Alloc& alloc)
        // Extended move construct from the specified 'original' using the the
        // specified 'alloc'.  If 'original.get_allocator() == alloc', the
        // behavior is identical to move construction, otherwise it is
        // identical to extended copy construction.
    {
        TestFunctor& originalRef = original;
        this->setPrivateAllocator(alloc);
        operator=(bslmf::MovableRefUtil::move(originalRef));
    }

    ~TestFunctor() { memset(static_cast<void*>(this), 0xbb, sizeof(*this)); }

    // MANIPULATORS
    TestFunctor& operator=(const TestFunctor& rhs) {
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // TBD: Conditionally suppress this decrement in C++03 to work
        // around limitation whereby 'destructiveMove' uses copy instead of
        // move.
        if (! (QUALITIES & e_IS_NOTHROW_MOVABLE))
#endif
        --copyMoveLimit;

        TrackableValue temp(encode(rhs.value()));
        this->d_value = temp;  // Invoke copy assignment on 'TrackableValue'
        return *this;
    }

    TestFunctor& operator=(bslmf::MovableRef<TestFunctor> rhs) {
        TestFunctor& rhsRef = rhs;
        if (this->privateAllocator() == rhsRef.privateAllocator()) {
            if (! (QUALITIES & e_IS_NOTHROW_MOVABLE)) {
                --copyMoveLimit;
            }
            int value = rhsRef.value();
            // Invoke move assignment on 'TrackableValue'
            this->d_value = bslmf::MovableRefUtil::move(rhsRef.d_value);
            // Re-encode value:
            this->d_value.setValueRaw(encode(value));
        }
        else {
            // Copy-assign if different allocators
            operator=(rhsRef);
        }
        return *this;
    }

    // Generation macro for parenthesis operators
#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        int ret = (value() + iw.value() +                                     \
                   BSLS_MACROREPEAT_SEP(n, ARGN, +));                         \
        setValue(ret);                                                        \
        return ret;                                                           \
    }

    // Invocation operator with 0 to 13 arguments.  The first argument (if any)
    // is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return value(); }
    int operator()(const IntWrapper& iw)
        { int ret = value() + iw.value(); setValue(ret); return ret; }
    BSLS_MACROREPEAT(12, OP_PAREN)

#undef OP_PAREN

    void setValue(int value) { this->d_value = encode(value); }

    // Invocation operator that sets the functor's value and returns void.
    // To ensure unambiguous overloading resolution, the argument is passed as
    // a null-terminated string, not as an integer.
    void operator()(const char* s) { setValue(atoi(s)); }

    void resetMoveCopiedFlags() { this->d_value.resetMoveCopiedFlags(); }

    // ACCESSORS
    bool isCopied()    const { return this->d_value.isCopied(); }
    bool isMoved()     const { return this->d_value.isMoved(); }
    bool isMovedFrom() const { return this->d_value.isMovedFrom(); }
    int  value()       const {
        return k_IS_STATELESS ? 0 : encode(this->d_value.value());
    }

    // HIDDEN FRIENDS
    friend bool operator==(const TestFunctor& a, const TestFunctor& b)
        { return a.value() == b.value(); }
    friend bool operator!=(const TestFunctor& a, const TestFunctor& b)
        { return a.value() != b.value(); }
};

}  // close unnamed namespace

namespace BloombergLP {
namespace bslmf {

template <int QUALITIES>
struct IsBitwiseMoveable<TestFunctor<QUALITIES> >
    : bsl::integral_constant<bool, bool(QUALITIES & e_IS_BITWISE_MOVABLE)>
{
    // Override all heuristics for 'IsBitwiseMoveable' (particularly the empty
    // object heuristic) and just use the specified quality.
};

}  // close namespace bslmf
}  // close enterprise namespace

namespace {

template <int QUALITIES>
struct IsTrackable<TestFunctor<QUALITIES> > :
        bsl::integral_constant<bool, !TestFunctor<QUALITIES>::k_IS_STATELESS> {
    // A 'TestFunctor' is trackable if it has state.
};

typedef TestFunctor<e_EMPTY_FUNCTOR  | e_IS_BITWISE_MOVABLE |
                    e_IS_NOTHROW_MOVABLE>                    EmptyFunctor;
typedef TestFunctor<e_SMALL_FUNCTOR  | e_IS_BITWISE_MOVABLE> SmallFunctor;
typedef TestFunctor<e_MEDIUM_FUNCTOR | e_IS_BITWISE_MOVABLE> MediumFunctor;
typedef TestFunctor<e_LARGE_FUNCTOR  | e_IS_BITWISE_MOVABLE> LargeFunctor;
typedef TestFunctor<e_SMALL_FUNCTOR  | e_IS_NOTHROW_MOVABLE> NTSmallFunctor;

BSLMF_ASSERT(sizeof(EmptyFunctor)  == 1                    );
BSLMF_ASSERT(sizeof(SmallFunctor)  <  k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(MediumFunctor) == k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(LargeFunctor)  >  k_SMALL_OBJECT_BUFFER_SIZE);

typedef TestFunctor<e_EMPTY_FUNCTOR> ThrowingEmptyFunctor;
// Test that being empty doesn't imply nothrow move or bitwise copyable:
BSLMF_ASSERT(!bsl::is_nothrow_move_constructible<ThrowingEmptyFunctor>::value);
BSLMF_ASSERT(!bslmf::IsBitwiseMoveable<ThrowingEmptyFunctor>::value);

typedef TestFunctor<e_SMALL_FUNCTOR                   > ThrowingSmallFunctor;
typedef TestFunctor<e_SMALL_FUNCTOR  | e_HAS_ALLOCATOR> SmallFunctorWithAlloc;
typedef TestFunctor<e_MEDIUM_FUNCTOR | e_HAS_ALLOCATOR> MediumFunctorWithAlloc;

typedef TestFunctor<e_SMALL_FUNCTOR | e_HAS_ALLOCATOR | e_IS_BITWISE_MOVABLE>
    BMSmallFunctorWithAlloc;

typedef TestFunctor<e_SMALL_FUNCTOR | e_HAS_ALLOCATOR | e_IS_NOTHROW_MOVABLE>
    NTSmallFunctorWithAlloc;

typedef TestFunctor<e_LARGE_FUNCTOR | e_HAS_ALLOCATOR | e_IS_BITWISE_MOVABLE>
    LargeFunctorWithAlloc;

BSLMF_ASSERT(sizeof(SmallFunctorWithAlloc)  <  k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(MediumFunctorWithAlloc) == k_SMALL_OBJECT_BUFFER_SIZE);
BSLMF_ASSERT(sizeof(LargeFunctorWithAlloc)  >  k_SMALL_OBJECT_BUFFER_SIZE);

class MutatingFunctor {
    // This class provides a functor having a non-const-qualified
    // 'operator()', that returns a different 'int' on subsequent calls.

    // PRIVATE DATA
    int d_data;

  public:
    MutatingFunctor() : d_data() {}

    int operator()() { return ++d_data; }
};

class FunctorWithoutDedicatedMove {
    // This class provides a functor having no dedicated move constructor or
    // move-assignment operator.  Move operations degenerate to copies.  This
    // type is not trackable because it does not instrument move operations.

    int d_data;

  public:
    explicit FunctorWithoutDedicatedMove(int v) : d_data(v) { }

    FunctorWithoutDedicatedMove(const FunctorWithoutDedicatedMove& original)
        : d_data(original.value()) { }

    // Dedicated move constructor is suppressed.

    FunctorWithoutDedicatedMove&
    operator=(const FunctorWithoutDedicatedMove& rhs)
        { d_data = rhs.value(); return *this; }

    // Dedicated move-assignment operator is suppressed.

    int operator()(const IntWrapper& iw, int i)
        { int ret = value() + iw.value() + i; setValue(ret); return ret; }

    void setValue(int v) { d_data = v; }

    int value() const { return d_data; }

    BSLA_MAYBE_UNUSED
    friend bool operator==(const FunctorWithoutDedicatedMove& a,
                           const FunctorWithoutDedicatedMove& b)
        { return a.value() == b.value(); }

    BSLA_MAYBE_UNUSED
    friend bool operator!=(const FunctorWithoutDedicatedMove& a,
                           const FunctorWithoutDedicatedMove& b)
        { return a.value() != b.value(); }
};

class FunctorWithFunctionCtor {
    // This class provides a functor that is constructible from a
    // 'bsl::function' object and has no dedicated move constructor or
    // move-assignment operator (move operations degenerate to copies).  In
    // C++03, constructing from a 'MovableRef<FunctorWithFunctionCtor>' is
    // ambiguous because 'MovableRef<FunctorWithFunctionCtor>' is convertible
    // to both 'bsl::function' and 'const FunctorWithFunctionCtor&'.  This type
    // is used to test that 'bsl::function' does not implicitly attempt move
    // construction of the target object in C++03.

    Obj d_func;

  public:
    explicit
    FunctorWithFunctionCtor(const Obj& f)
        : d_func(f) { }

    FunctorWithFunctionCtor(const FunctorWithFunctionCtor& original)
        : d_func(original.d_func) { }

    // Dedicated move constructor is suppressed.

    FunctorWithFunctionCtor&
    operator=(const FunctorWithFunctionCtor& rhs)
        { d_func = rhs.d_func; return *this; }

    // Dedicated move-assignment operator is suppressed.

    int operator()(const IntWrapper& iw, int i) { return d_func(iw, i); }

    BSLA_MAYBE_UNUSED
    friend bool operator==(const FunctorWithFunctionCtor& a,
                           const FunctorWithFunctionCtor& b)
    {
        const Obj& funcA = a.d_func;
        const Obj& funcB = b.d_func;

        if (! funcA) {
            return ! funcB;
        }
        else if (! funcB) {
            return false;
        }

        // Assume 'a' and 'b' both hold simple pointers to functions.
        PROTO *const *targetA = funcA.target<PROTO *>();
        PROTO *const *targetB = funcB.target<PROTO *>();
        return targetA && targetB && *targetA == *targetB;
    }

    BSLA_MAYBE_UNUSED
    friend bool operator!=(const FunctorWithFunctionCtor& a,
                           const FunctorWithFunctionCtor& b)
        { return ! (a == b); }
};

template <class PROTOTYPE>
struct HypotheticalFunctor;

template <class RET>
struct HypotheticalFunctor<RET()> {
    // ACCESSORS
    RET operator()() const;
};

template <class RET, class ARG0>
struct HypotheticalFunctor<RET(ARG0)> {
    // ACCESSORS
    RET operator()(ARG0) const;
};

template <class RET, class ARG0, class ARG1>
struct HypotheticalFunctor<RET(ARG0, ARG1)> {
    // ACCESSORS
    RET operator()(ARG0, ARG1) const;
};

template <class RET, class ARG0, class ARG1, class ARG2>
struct HypotheticalFunctor<RET(ARG0, ARG1, ARG2)> {
    // ACCESSORS
    RET operator()(ARG0, ARG1, ARG2) const;
};

inline bool isConstPtr(void *) { return false; }
inline bool isConstPtr(const void *) { return true; }

template <class TYPE>
inline bool isNullImp(const TYPE& p, bsl::true_type /* is pointer */) {
    return 0 == p;
}

template <class TYPE>
inline bool isNullImp(const TYPE&, bsl::false_type /* is pointer */) {
    return false;
}

template <class TYPE>
inline bool isNull(const TYPE& p)
    // Return true if 'TYPE' has the notion of a null value and the specified
    // 'p' argument has that null value.
{
    static const bool IS_POINTER =
        bsl::is_pointer<TYPE>::value ||
        bslmf::IsFunctionPointer<TYPE>::value ||
        bslmf::IsMemberFunctionPointer<TYPE>::value;

    return isNullImp(p, bsl::integral_constant<bool, IS_POINTER>());
}

inline bool isNull(const bsl::nullptr_t&)
    // This overload of 'isNull' returns true for 'nullptr_t'.
{
    return true;
}

template <class PROTOTYPE>
inline bool isNull(const bsl::function<PROTOTYPE>& f)
    // This overload of 'isNull' returns true if the specified 'f' is an empty
    // 'bsl::function'; else return false.
{
    return ! f;
}

template <class TYPE>
class ArgGeneratorBase {
    // Wrap and make available an object of the specified 'TYPE' for passing
    // into functions and constructors in the test driver.  'TYPE' is
    // constrained to be constructible from 'int' and have a 'value()' method
    // that returns an 'int'.  ('TrackableValue' and 'TestFunctor<>' meet these
    // criteria.)

    typedef typename bsl::remove_const<TYPE>::type MutableT;
    MutableT d_value;

  public:
    // TYPES
    typedef TYPE WrappedType;

    // CONSTANTS
    enum { INIT_VALUE = 0x2001 };

    // CREATORS
    ArgGeneratorBase() : d_value(INIT_VALUE) { }
        // Create an object of wrapping a 'TYPE' object with a known initial
        // value.

    // MANIPULATORS
    WrappedType& reset() { return (d_value = MutableT(INIT_VALUE)); }
        // Reset the wrapped object to its initial value and return a
        // modifiable reference to the wrapped object.

    // ACCESSORS
    int value() const { return d_value.value(); }
        // Return the value of the wrapped object.
};

template <class TYPE>
struct ArgGenerator : ArgGeneratorBase<TYPE> {
    // Wrap and make available an object of the specified 'TYPE' for passing
    // into functions and constructors in the test driver.  'TYPE' is
    // constrained to be constructible from 'int' and have a 'value()' method
    // that returns an 'int'.  ('TrackableValue' and 'TestFunctor<>' meet these
    // criteria.)  The expected usage is to construct an object of this type,
    // 'x', and pass 'x.obj()' to a function or constructor.  After the call,
    // verify that 'x.check(v)' returns true, where 'v' is the value that the
    // call is expected to have written into 'x'.
    //
    // This primary template is used when 'TYPE' is an rvalue not a reference
    // or pointer type.

    bool check(int /* exp */) const
        // Return true if the wrapped object has its initial value.  Normally,
        // this function would compare the wrapped object to the passed
        // argument, but since, for this specialization, 'obj()' returns an
        // rvalue, 'somecall(x.obj())' cannot modify 'x', so 'x' would be
        // expected to retain its initial value.
        { return this->value() == ArgGeneratorBase<TYPE>::INIT_VALUE; }

    TYPE obj() { return this->reset(); }
        // Reset the wrapped object to its initial value and return a copy of
        // the wrapped object, typically for use as an argument in a function
        // call.
};

template <class TYPE>
struct ArgGenerator<TYPE&> : ArgGeneratorBase<TYPE> {
    // Specialization of 'ArgGenerator' for lvalue references to the specified
    // 'TYPE'.

    bool check(int exp) const { return this->value() == exp; }
        // Return true if the wrapped object's value compares equal to the
        // specified 'exp'.

    TYPE& obj() { return this->reset(); }
        // Reset the wrapped object to its initial value and return a
        // modifiable lvalue reference to the wrapped object, typically for use
        // as an argument in a function call.
};

template <class TYPE>
struct ArgGenerator<BSLMF_MOVABLEREF_DEDUCE(TYPE)> : ArgGeneratorBase<TYPE> {
    // Specialization of 'ArgGenerator' for movable references to the specified
    // 'TYPE'.

    bool check(int exp) const { return this->value() == exp; }
        // Return true if the wrapped object's value compares equal to the
        // specified 'exp'.

    bslmf::MovableRef<TYPE> obj()
        // Reset the wrapped object to its initial value and return a
        // modifiable, movable reference to the wrapped object, typically for
        // use as an argument in a function call.
        { return bslmf::MovableRefUtil::move(this->reset()); }
};

template <class TYPE>
struct ArgGenerator<TYPE *> : ArgGeneratorBase<TYPE> {
    // Specialization of 'ArgGenerator' for pointers to the specified 'TYPE'.

    bool check(int exp) const { return this->value() == exp; }
        // Return true if the wrapped object's value compares equal to the
        // specified 'exp'.

    TYPE *obj() { return &this->reset(); }
        // Reset the wrapped object to its initial value and return a
        // modifiable pointer to the wrapped object, typically for use as an
        // argument in a function call.
};

template <class TYPE>
struct ArgGenerator<SmartPtr<TYPE> > : ArgGeneratorBase<TYPE> {
    // Specialization of 'ArgGenerator' for 'SmartPtr' to the specified 'TYPE'.

    bool check(int exp) const { return this->value() == exp; }
        // Return true if the wrapped object's value compares equal to the
        // specified 'exp'.

    SmartPtr<TYPE> obj() { return SmartPtr<TYPE>(&this->reset()); }
        // Reset the wrapped object to its initial value and return a smart
        // pointer to the wrapped object, typically for use as an argument in a
        // function call.  The smart pointer is returned by value, but provides
        // modifiable access to the wrapped object.
};

template <class TYPE>
struct ArgGenerator<BSLMF_MOVABLEREF_DEDUCE(SmartPtr<TYPE>)>
: ArgGeneratorBase<TYPE> {
    // Specialization of 'ArgGenerator' for movable references of modifiable
    // smart pointers smart pointers to the specified 'TYPE'.

  private:
    SmartPtr<TYPE> d_obj;
        // an object of type 'SmartPtr<TYPE>' such that the 'obj' member
        // function can return a 'bslmf::MovableRef<SmartPtr<TYPE> >' object
        // without creating an rvalue reference to a temporary

  public:
    bool check(int exp) const { return this->value() == exp; }
        // Return true if the wrapped object's value compares equal to the
        // specified 'exp'.

    bslmf::MovableRef<SmartPtr<TYPE> > obj()
        // Reset the wrapped object to its initial value and return a movable
        // reference of a smart pointer to the wrapped object, typically for
        // use as an argument in a function call.
    {
        d_obj = SmartPtr<TYPE>(&this->reset());
        return bslmf::MovableRefUtil::move(d_obj);
    }
};

// Special marker for moved-from comparisons
bsls::ObjectBuffer<Obj> movedFromMarkerBuf;
const Obj&              movedFromMarker = movedFromMarkerBuf.object();

class ConvertibleToObj {
  public:
    // CREATORS
    ConvertibleToObj();
        // This constructor is declared but not defined.

    // ACCESSORS
    operator Obj() const;
        // This implicit conversion operator is declared but not defined.
};

}  // close unnamed namespace

namespace BloombergLP {

// Forward reference to higher-level component, to test conversions.
template <class FUNCPTR> class bdef_Function;

template <class PROTOTYPE>
class bdef_Function<PROTOTYPE *> : public bsl::function<PROTOTYPE> {
    // Minimal rendering of 'BloombergLP::bdef_Function' to test conversions.

  public:
    template <class FUNC>
    bdef_Function(const FUNC& func)                                 // IMPLICIT
        : bsl::function<PROTOTYPE>(func) { }
};

namespace bslstl {

template <class PROTOTYPE>
struct Function_IsReferenceCompatible<BloombergLP::bdef_Function<PROTOTYPE *>,
                                      bsl::function<PROTOTYPE> >
    : bsl::true_type {
    // Specialization of trait for 'bdef_Function' being convertible to
    // 'bsl::function'.
};

}  // Close package namespace
}  // Close enterprise namespace

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

#ifdef BSLSTL_FUNCTION_TEST_PART_09
namespace {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Polymorphic Invocation
///- - - - - - - - - - - - - - - - -
// In this example, we create a single 'bsl::function' object, then assign it
// to callable objects of different types at run time.
//
// First, we define a simple function that returns the XOR of its two integer
// arguments:
//..
    int intXor(int a, int b) { return a ^ b; }
//..
// Next, we create a 'bsl::function' that takes two integers and returns an
// integer.  Because we have not initialized the object with a target, it
// starts out as empty and evaluates to false in a Boolean context:
//..
    void usageExample1()
    {
        bsl::function<int(int, int)> funcObject;
        ASSERT(! funcObject);
//..
// Next, we use assignment to give it the value of (a pointer to) 'intXor' and
// test that we can invoke it to get the expected result:
//..
        funcObject = intXor;
        ASSERT(funcObject);
        ASSERT(5 == funcObject(6, 3));
//..
// Next, we assign an instance of 'std::plus<int>' functor to 'funcObject',
// which then holds a copy of it, and again test that we get the expected
// result when we invoke 'funcObject'.
//..
        funcObject = std::plus<int>();
        ASSERT(funcObject);
        ASSERT(9 == funcObject(6, 3));
//..
// Then, if we are using C++11 or later, we assign it to a lambda expression
// that multiplies its arguments:
//..
  #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        funcObject = [](int a, int b) { return a * b; };
        ASSERT(funcObject);
        ASSERT(18 == funcObject(6, 3));
  #endif
//..
// Finally, we assign 'funcObject' to 'nullptr', which makes it empty again:
//..
        funcObject = bsl::nullptr_t();
        ASSERT(! funcObject);
    }
//..
//
///Example 2: Use in Generic a Algorithm
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to define an algorithm that performs a mutating operation on
// every element of an array of integers.  The inputs are pointers to the first
// and last element to transform, a pointer to the first element into which the
// to write the output, and an operation that takes an integer in and produces
// an integer return value.  Although the pointer arguments have known type
// ('int *'), the type of the transformation operation can be anything that can
// be called with an integral argument and produces an integral return value.
// We do not want to accept this operation as a template argument, however
// (perhaps because our algorithm is sufficiently complex and/or proprietary
// that we want to keep it out of header files).  We solve these disparate
// requirements by passing the operation as a 'bsl::function' object, whose
// type is known at compile time but which can be set to an arbitrary
// operation at run time:
//..
    void myAlgorithm(const int                      *begin,
                     const int                      *end,
                     int                            *output,
                     const bsl::function<int(int)>&  op);
        // Apply my special algorithm to the elements in the contiguous address
        // range from the specified 'begin' pointer up to but not including the
        // specified 'end' pointer, writing the result to the contiguous range
        // starting at the specified 'output' pointer.  The specified 'op'
        // function is applied to each element before it is fed into the
        // algorithm.
//..
// For the purpose of illustration, 'myAlgorithm' is a simple loop that
// invokes the specified 'op' on each element in the input range and writes it
// directly to the output:
//..
    void myAlgorithm(const int                      *begin,
                     const int                      *end,
                     int                            *output,
                     const bsl::function<int(int)>&  op)
    {
        for (; begin != end; ++begin) {
            *output++ = op(*begin);
        }
    }
//..
// Next, we define input and output arrays to be used throughout the rest of
// this example:
//..
    static const std::size_t DATA_SIZE = 5;
    static const int         testInput[DATA_SIZE] = { 4, 3, -2, 9, -7 };
    static int               testOutput[DATA_SIZE];
//..
// Next, we define a function that simply negates its argument:
//..
    long negate(long v) { return -v; }
        // Return the arithmetic negation of the specified 'v' integer.
//..
// Then, we test our algorithm using our negation function:
//..
    bool testNegation()
        // Test the use of the 'negation' function with 'myAlgorithm'.
    {
        myAlgorithm(testInput, testInput + DATA_SIZE, testOutput, negate);

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            if (-testInput[i] != testOutput[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }
//..
// Note that the prototype for 'negate' is not identical to the prototype used
// to instantiate the 'op' argument in 'myAlgorithm'.  All that is required is
// that each argument to 'op' be convertible to the corresponding argument in
// the function and that the return type of the function be convertible to the
// return type of 'op'.
//
// Next, we get a bit more sophisticated and define an operation that produces
// a running sum over its inputs.  A running sum requires holding on to state,
// so we define a functor class for this purpose:
//..
    class RunningSum {
        // Keep a running total of all of the inputs provided to 'operator()'.

        // DATA
        int d_sum;

      public:
        // CREATORS
        explicit RunningSum(int initial = 0) : d_sum(initial) { }
            // Create a 'RunningSum' with initial value set to the specified
            // 'initial' argument.

        // MANIPULATORS
        int operator()(int v)
            // Add the specified 'v' to the running sum and return the running
            // sum.
            { return d_sum += v; }
    };
//..
// Then, we test 'myAlgorithm' with 'RunningSum':
//..
    bool testRunningSum()
        // Test the user of 'RunningSum' with 'myAlgorithm'.
    {
        myAlgorithm(testInput, testInput+DATA_SIZE, testOutput, RunningSum());

        int sum = 0;
        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            sum += testInput[i];
            if (sum != testOutput[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }
//..
// Note that 'RunningSum::operator()' is a mutating operation and that, within
// 'myAlgorithm', 'op' is const.  Even though 'bsl::function' owns a copy of
// its target, logical constness does not apply, as per the standard.
//
// Finally, we run our tests and validate the results:
//..
    void usageExample2()
    {
        ASSERT(testNegation());
        ASSERT(testRunningSum());
    }
//..
//
///Example 3: A Parallel Work queue
///- - - - - - - - - - - - - - - -
// In this example, we'll simulate a simple library whereby worker threads take
// work items from a queue and execute them asynchronously.  This simulation is
// single-threaded, but keeps metrics on how much work each worker accomplished
// so that we can get a rough idea of how much parallelism was expressed by the
// program.
//
// We start by defining a work item type to be stored in our work queue.  This
// type is simply a 'bsl::function' taking a 'WorkQueue' pointer argument and
// returning 'void'.
//..
    class WorkQueue;  // Forward declaration

    typedef bsl::function<void(WorkQueue *)> WorkItem;
//..
// Next, we define a work queue class.  For simplicity, we'll implement our
// queue as a fixed-sized circular buffer and (because this is a
// single-threaded simulation), ignore synchronization concerns.
//..
    class WorkQueue {
        // A FIFO queue of tasks to be executed.

        // PRIVATE CONSTANTS
        static const int k_MAX_ITEMS = 16;

        // DATA
        int      d_numItems;
        int      d_head;
        WorkItem d_items[k_MAX_ITEMS];

      public:
        // CREATORS
        WorkQueue()
            // Create an empty work queue.
            : d_numItems(0), d_head(0) { }

        // MANIPULATORS
        void dequeue(WorkItem *result)
            // Move the work item at the head of the queue into the specified
            // 'result' and remove it from the queue.  The behavior is
            // undefined if this queue is empty.
        {
            ASSERT(d_numItems > 0);
            *result = bslmf::MovableRefUtil::move(d_items[d_head]);
            d_head = (d_head + 1) % k_MAX_ITEMS;  // circular
            --d_numItems;
        }

        void enqueue(bslmf::MovableRef<WorkItem> item)
            // Enqueue the specified 'item' work item onto the tail of the
            // queue.  The work is moved from 'item'.
        {
            int tail = (d_head + d_numItems++) % k_MAX_ITEMS; // circular
            ASSERT(d_numItems <= k_MAX_ITEMS);
            d_items[tail] = bslmf::MovableRefUtil::move(item);
        }

        // ACCESSORS
        bool isEmpty() const
            // Return true if there are no items in the queue; otherwise return
            // false.
            { return 0 == d_numItems; }

        int size() const
            // Return the number of items currently in the queue.
            { return d_numItems; }
    };
//..
// Next, we'll create a worker class that represents the state of a worker
// thread:
//..
    class Worker {
        // A simulated worker thread.

        // DATA
        bool d_isIdle;             // True if the worker is idle

      public:
        // CREATORS
        Worker()
            // Create an idle worker.
            : d_isIdle(true) { }

        // MANIPULATORS
        void run(WorkQueue *queue);
            // Dequeue a task from the specified 'queue' and execute it
            // (asynchronously, in theory).  The behavior is undefined unless
            // this worker is idle before the call to 'run'.

        // ACCESSORS
        bool isIdle() const
            // Return whether this worker is idle.  An idle worker is one that
            // can except work.
            { return d_isIdle; }
    };
//..
// Next, we implement the 'run' function, which removes a 'bsl::function'
// object from the work queue and then executes it, passing the work queue as
// the sole argument:
//..
    void Worker::run(WorkQueue *queue)
    {
        if (queue->isEmpty()) {
            // No work to do
            return;                                                   // RETURN
        }

        WorkItem task;
        queue->dequeue(&task);

        d_isIdle = false;  // We're about to do work.
        task(queue);       // Do the work.
        d_isIdle = true;   // We're idle again.
    }
//..
// Now, we implement a simple scheduler containing a work queue and an array of
// four workers, which are run in a round-robin fashion:
//..
    class Scheduler {
        // Parallel work scheduler.

        // PRIVATE CONSTANTS
        static const int k_NUM_WORKERS = 4;

        // DATA
        WorkQueue d_workQueue;
        Worker    d_workers[k_NUM_WORKERS];

      public:
        // CREATORS
        explicit Scheduler(bslmf::MovableRef<WorkItem> initialTask)
            // Create a scheduler and enqueue the specified 'initialTask'.
        {
            d_workQueue.enqueue(bslmf::MovableRefUtil::move(initialTask));
        }

        // MANIPULATORS
        void run();
            // Execute the tasks in the work queue (theoretically in parallel)
            // until the queue is empty.
    };
//..
// Next, we implement the scheduler's 'run' method: which does a round-robin
// scheduling of the workers, allowing each to pull work off of the queue and
// run it.  As tasks are run, they may enqueue more work.  The scheduler
// returns when there are no more tasks in the queue.
//..
    void Scheduler::run()
    {
        while (! d_workQueue.isEmpty()) {
            for (int i = 0; i < k_NUM_WORKERS; ++i) {
                if (d_workers[i].isIdle()) {
                    d_workers[i].run(&d_workQueue);
                }
            }
        }
    }
//..
// Next, we create a job for the parallel system to execute.  A popular
// illustration of parallel execution is the quicksort algorithm, which is a
// recursive algorithm whereby the input array is partitioned into a low and
// high half and quicksort is recursively applied, in parallel, to the two
// halves.  We define a class that encapsulates an invocation of quicksort on
// an input range:
//..
    template <class TYPE>
    class QuickSortTask {
        // A functor class to execute parallel quicksort on a contiguous range
        // of elements of specified 'TYPE' supplied at construction.

        // DATA
        TYPE *d_begin_p;
        TYPE *d_end_p;

        // PRIVATE CLASS METHODS
        static TYPE* partition(TYPE *begin, TYPE *end);
            // Partition the contiguous range specified by '[begin, end)' and
            // return an iterator, 'mid', such that every element in the range
            // '[begin, mid)' is less than '*mid' and every element in the
            // range '[mid + 1, end)' is not less than '*mid'.  The behavior is
            // undefined unless 'begin < end'.

      public:
        // CREATORS
        QuickSortTask(TYPE *begin, TYPE *end)
            // Create a task to sort the contiguous range from the item at the
            // specified 'begin' location up to but not included the item at
            // the specified 'end' location.
            : d_begin_p(begin), d_end_p(end) { }

        // MANIPULATORS
        void operator()(WorkQueue *queue);
            // Preform the sort in parallel using the specified 'queue' to
            // enqueue parallel work.
    };
//..
// Next we implement the 'partition' method, using a variation of the Lomuto
// partition scheme:
//..
    template <class TYPE>
    TYPE* QuickSortTask<TYPE>::partition(TYPE *begin, TYPE *end)
    {
        using std::swap;

        swap(begin[(end - begin) / 2], end[-1]); // Put pivot at end
        TYPE& pivot = *--end;
        TYPE *divider = begin;
        for (; begin != end; ++begin) {
            if (*begin < pivot) {
                swap(*divider, *begin);
                ++divider;
            }
        }
        swap(*divider, pivot);  // Put pivot in the middle
        return divider;
    }
//..
// Then we define the call operator for our task type, which performs the
// quicksort:
//..
    template <class TYPE>
    void QuickSortTask<TYPE>::operator()(WorkQueue *queue)
    {
        if (d_end_p - d_begin_p < 2) {
            // Zero or one element. End recursion.
            return;                                                   // RETURN
        }

        // Partition returns end iterator for low partition == begin iterator
        // for high partition.
        TYPE *mid = partition(d_begin_p, d_end_p);

        // Asynchronously sort the two partitions
        WorkItem sortLoPart(QuickSortTask(d_begin_p, mid));
        WorkItem sortHiPart(QuickSortTask(mid + 1, d_end_p));
        queue->enqueue(bslmf::MovableRefUtil::move(sortLoPart));
        queue->enqueue(bslmf::MovableRefUtil::move(sortHiPart));
    }
//..
// Finally, we use our scheduler and our 'QuickSortTask' to sort an array
// initially containing the integers between 1 and 31 in random order:
//..
    void usageExample3()
    {
        short data[] = {
            23, 12, 2, 28, 1, 10, 5, 13, 15, 8, 19, 14, 31, 29, 9, 11, 24, 3,
            30, 7, 17, 27, 20, 21, 18, 4, 22, 25, 16, 6, 26
        };

        static const int DATA_SIZE = sizeof(data) / sizeof(data[0]);
        ASSERT(31 == DATA_SIZE);

        WorkItem  initialTask(QuickSortTask<short>(data, data + DATA_SIZE));
        Scheduler sched(bslmf::MovableRefUtil::move(initialTask));
        sched.run();

        // Validate results
        for (int i = 0; i < DATA_SIZE; ++i) {
            ASSERT(i + 1 == data[i]);
        }
    }
//..

}  // close unnamed namespace
#endif  // BSLSTL_FUNCTION_TEST_PART_09

// ============================================================================
//                              TEST FUNCTIONS
// ----------------------------------------------------------------------------

template <class TYPE, class RET, class ARG>
void testPtrToMemFunc(const char *prototypeStr)
    // Test invocation of pointer to member function wrapper.  The 'TYPE',
    // 'RET', and 'ARG' type are selected by the caller so that'
    // 'function<RET(ARG...)>' can be constructed and invoked (according to the
    // invocation rules) with non-const member functions
    // 'IntWrapper::increment[0-12]'.
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);
    const ARG a10(0x0400);
    const ARG a11(0x0800);
    const ARG a12(0x1000);

    ArgGenerator<TYPE> gen;

    bsl::function<RET(TYPE)> f1(&IntWrapper::increment0);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    bsl::function<RET(TYPE, ARG)> f2(&IntWrapper::increment1);
    ASSERT(0x2003 == f2(gen.obj(), a1));
    ASSERT(gen.check(0x2003));

    bsl::function<RET(TYPE, ARG, ARG)> f3(&IntWrapper::increment2);
    ASSERT(0x2007 == f3(gen.obj(), a1, a2));
    ASSERT(gen.check(0x2007));

    bsl::function<RET(TYPE, ARG, ARG, ARG)> f4(&IntWrapper::increment3);
    ASSERT(0x200f == f4(gen.obj(), a1, a2, a3));
    ASSERT(gen.check(0x200f));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG)> f5(&IntWrapper::increment4);
    ASSERT(0x201f == f5(gen.obj(), a1, a2, a3, a4));
    ASSERT(gen.check(0x201f));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG,
                      ARG)> f6(&IntWrapper::increment5);
    ASSERT(0x203f == f6(gen.obj(), a1, a2, a3, a4, a5));
    ASSERT(gen.check(0x203f));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f7(&IntWrapper::increment6);
    ASSERT(0x207f == f7(gen.obj(), a1, a2, a3, a4, a5, a6));
    ASSERT(gen.check(0x207f));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f8(&IntWrapper::increment7);
    ASSERT(0x20ff == f8(gen.obj(), a1, a2, a3, a4, a5, a6, a7));
    ASSERT(gen.check(0x20ff));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> f9(&IntWrapper::increment8);
    ASSERT(0x21ff == f9(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(gen.check(0x21ff));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(&IntWrapper::increment9);
    ASSERT(0x23ff == f10(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(gen.check(0x23ff));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG, ARG)> f11(&IntWrapper::increment10);
    ASSERT(0x27ff == f11(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10));
    ASSERT(gen.check(0x27ff));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG, ARG, ARG)> f12(&IntWrapper::increment11);
    ASSERT(0x2fff ==
           f12(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11));
    ASSERT(gen.check(0x2fff));

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG, ARG, ARG)> f13(&IntWrapper::increment12);
    ASSERT(0x3fff ==
           f13(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12));
    ASSERT(gen.check(0x3fff));

    // Test using bslmf::NothrowMovableWrapper
    bsl::function<RET(TYPE,ARG,ARG)> ntf3(NTWRAP(&IntWrapper::increment2));
    ASSERT(0x2007 == ntf3(gen.obj(), a1, a2));
    ASSERT(gen.check(0x2007));
}

template <class TYPE, class RET, class ARG>
void testPtrToConstMemFunc(const char *prototypeStr)
    // Test invocation of pointer to const member function wrapper.  Tests
    // using const member functions 'IntWrapper::add[0-12]'.  To save compile
    // time, since 'testPtrToMemFunc' already tests every possible
    // argument-list length, we test only a small number of possible
    // argument-list lengths (specifically 0, 1, and 12 arguments) here.
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);
    const ARG a10(0x0400);
    const ARG a11(0x0800);
    const ARG a12(0x1000);

    ArgGenerator<TYPE> gen;

    bsl::function<RET(TYPE)> f1(&IntWrapper::add0);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    bsl::function<RET(TYPE, ARG)> f2(&IntWrapper::add1);
    ASSERT(0x2003 == f2(gen.obj(), a1));
    ASSERT(gen.check(0x2001));

    // No need to test 3 through 12 arguments.  That mechanism has already been
    // tested via the 'testPtrToMemFunc' function.

    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(&IntWrapper::add9);
    ASSERT(0x23ff == f10(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(gen.check(0x2001));

    // Test using bslalg::NothrowMovableWrapper
    bsl::function<RET(TYPE, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> ntf13(NTWRAP(&IntWrapper::add12));
    ASSERT(0x3fff ==
           ntf13(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,a12));
    ASSERT(gen.check(0x2001));
}

template <class TYPE, class RET>
void testPtrToMemData(const char *prototypeStr)
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    ArgGenerator<TYPE> gen;

    // Test with 'void' return type.
    bsl::function<void(TYPE)> f0(&IntHolder::d_value);
    f0(gen.obj());
    ASSERT(gen.check(0x2001));

    // Test with specified return type 'RET'.
    bsl::function<RET(TYPE)> f1(&IntHolder::d_value);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    // Test with a "nothrow" wrapper.
    bsl::function<RET(TYPE)> f2(NTWRAP(&IntHolder::d_value));
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));
}

template <class TYPE, class RET>
void testPtrToConstMemData(const char *prototypeStr)
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    ArgGenerator<TYPE> gen;

    // Test with 'void' return type.
    bsl::function<void(TYPE)> f0(&ConstIntHolder::d_value);
    f0(gen.obj());
    ASSERT(gen.check(0x2001));

    // Test with specified return type 'RET'.
    bsl::function<RET(TYPE)> f1(&ConstIntHolder::d_value);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    // Test with a "nothrow" wrapper.
    bsl::function<RET(TYPE)> f2(NTWRAP(&ConstIntHolder::d_value));
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));
}

template <class FUNCTOR, class OBJ>
bool checkValue(const OBJ& obj, int exp)
    // Return true if the functor wrapped within the specified 'obj' has a
    // value that matches the specified 'exp' value and false otherwise.  The
    // behavior is undefined unless 'obj.target<FUNCTOR>()' is non-null.
{
    if (FUNCTOR::k_IS_STATELESS) {
        return true;  // No state to check                            // RETURN
    }

    const FUNCTOR& target = *obj.template target<FUNCTOR>();
    return exp == target.value();
}

template <class FUNCTOR, class RET, class ARG>
void testWithFunctor(const char *prototypeStr)
    // Test invocation of a 'bsl::function' wrapping a functor object.
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const bool isStateless = FUNCTOR::k_IS_STATELESS;
    const int  initState = isStateless ? 0 : 0x3000;

    FUNCTOR ftor(initState);

    const ARG a0(0x0001);
    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);

    bsl::function<RET()> f0(ftor);
    ASSERT(initState + 0x0000 == f0());
    ASSERT(checkValue<FUNCTOR>(f0, initState + 0x0000));

    bsl::function<RET(ARG)> f1(ftor);
    ASSERT(initState + 0x0001 == f1(a0));
    ASSERT(checkValue<FUNCTOR>(f1, initState + 0x0001));

    bsl::function<RET(ARG, ARG)> f2(ftor);
    ASSERT(initState + 0x0003 == f2(a0, a1));
    ASSERT(checkValue<FUNCTOR>(f2, initState + 0x0003));

    bsl::function<RET(ARG, ARG, ARG)> f3(ftor);
    ASSERT(initState + 0x0007 == f3(a0, a1, a2));
    ASSERT(checkValue<FUNCTOR>(f3, initState + 0x0007));

    bsl::function<RET(ARG, ARG, ARG, ARG)> f4(ftor);
    ASSERT(initState + 0x000f == f4(a0, a1, a2, a3));
    ASSERT(checkValue<FUNCTOR>(f4, initState + 0x000f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG)> f5(ftor);
    ASSERT(initState + 0x001f == f5(a0, a1, a2, a3, a4));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG)> f6(ftor);
    ASSERT(initState + 0x003f == f6(a0, a1, a2, a3, a4, a5));
    ASSERT(checkValue<FUNCTOR>(f6, initState + 0x003f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f7(ftor);
    ASSERT(initState + 0x007f == f7(a0, a1, a2, a3, a4, a5, a6));
    ASSERT(checkValue<FUNCTOR>(f7, initState + 0x007f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f8(ftor);
    ASSERT(initState + 0x00ff == f8(a0, a1, a2, a3, a4, a5, a6, a7));
    ASSERT(checkValue<FUNCTOR>(f8, initState + 0x00ff));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> f9(ftor);
    ASSERT(initState + 0x01ff == f9(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(checkValue<FUNCTOR>(f9, initState + 0x01ff));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(ftor);
    ASSERT(initState + 0x03ff == f10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(checkValue<FUNCTOR>(f10, initState + 0x03ff));

    // Test invocation of const 'bsl::function' & verify side-effects
    const bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                            ARG, ARG, ARG)> cf10(ftor);
    ASSERT(initState + 0x03ff == cf10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(checkValue<FUNCTOR>(cf10, initState + 0x03ff));

    // Test 'bslalg::NothrowMovableWrapper'
    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> ntf9(NTWRAP(ftor));
    ASSERT(initState + 0x01ff == ntf9(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(checkValue<FUNCTOR>(ntf9, initState + 0x01ff));

    // None of the above invocations should have changed the original of
    // 'ftor'
    ASSERT(initState == ftor.value());

    // Discard return value of functor that returns non-void
    bsl::function<void(ARG)> fvi(ftor);
    fvi(a0);
    ASSERT(checkValue<FUNCTOR>(fvi, initState + 0x0001));

    // Invoke a functor with a void return value
    bsl::function<void(const char *)> fvr(ftor);
    fvr("1234");
    ASSERT(checkValue<FUNCTOR>(fvr, 1234)); // Wrapped copy is modified
    ASSERT(ftor.value() == initState);      // Original is unchanged
}

template <class FUNC>
inline
bool allocPropagationCheckImp(const Obj& f, bsl::true_type /*usesBslmaAlloc*/)
    // Return true (good) if the allocator for the specified 'f' function
    // object is stored as the allocator in the wrapped functor of type 'FUNC'
    // within 'f'; otherwise return false (failed to propagate allocator
    // correctly).  The behavior is undefined unless 'f' wraps an
    // allocator-aware object of type 'FUNC' which supplies an 'allocator()'
    // member that returns a stored allocator pointer.  This overload is used
    // when 'bslma::UsesBslmaAllocator<FUNC>' derives from 'true_type'.
{
    if (! f) {
        // Empty 'bsl::functor' has no target to propagate allocator to
        return true;                                                  // RETURN
    }

    const FUNC *target = f.target<FUNC>();
    return f.get_allocator() == target->get_allocator();
}

template <class FUNC>
inline
bool allocPropagationCheckImp(const Obj&, bsl::false_type /*usesBslmaAlloc*/)
    // Return true (good).  This overload is used when
    // 'bslma::UsesBslmaAllocator<FUNC>' derives from 'false_type', meaning
    // that 'FUNC' does not use an allocator.
{
    return true;
}

template <class FUNC>
inline
bool allocPropagationCheck(const Obj& f)
    // Return true if 'FUNC' does not use a 'bslma::Allocator*' or, if it
    // does, 'f.target<FUNC>()->allocator()' returns the same allocator as
    // 'f.allocator()' for the specified 'f' function object; otherwise
    // return false.
{
    return allocPropagationCheckImp<FUNC>(f,
                                          bslma::UsesBslmaAllocator<FUNC>());
}

#ifdef BSLSTL_FUNCTION_TEST_PART_01
struct UnusedFunctor {
    int operator()(const IntWrapper&, int);  // Declared but not defined
};
#endif  // BSLSTL_FUNCTION_TEST_PART_01

#ifdef BSLSTL_FUNCTION_TEST_PART_01
template <class FUNC>
void testBasicAccessors(FUNC func)
    // Test basic constructors and accessors.  Note that pass-by-value 'func'
    // is needed to force decay of function types.
{
    Obj f(func); const Obj& F = f;
    ASSERT(F ? true : false);  // Evaluate in boolean context
    ASSERT(typeid(FUNC) == F.target_type());
    ASSERT(F.target<FUNC>());
    ASSERT(F.target<FUNC>() && func == *F.target<FUNC>());
    ASSERT(f.target<FUNC>());
    ASSERT(f.target<FUNC>() && func == *f.target<FUNC>());
    ASSERT(0 == F.target<UnusedFunctor>());  // Wrong target-type
    ASSERT(0 == F.target<int>());            // non-callable type
#if ! defined(BSLS_PLATFORM_CMP_IBM)
    // IBM compiler has trouble forming pointer types from function types
    ASSERT(0 == F.target<PROTO>());          // Function (not pointer) type
    if (bsl::is_pointer<FUNC>::value) {
        // Function (not pointer) type should fail, even if it's the correct
        // function type.
        typedef typename bsl::remove_pointer<FUNC>::type NonPtrType;
        ASSERT(0 == F.target<NonPtrType>());
    }
#endif
    ASSERT(  isConstPtr(F.target<FUNC>()));
    ASSERT(! isConstPtr(f.target<FUNC>()));
    ASSERT_NOEXCEPT(false, Obj(func));

    Obj fw(NTWRAP(func)); const Obj& FW = fw;
    ASSERT(FW ? true : false);  // Evaluate in boolean context
    ASSERT(typeid(FUNC) == FW.target_type());
    ASSERT(FW.target<FUNC>() && func == *FW.target<FUNC>());
    ASSERT(fw.target<FUNC>() && func == *fw.target<FUNC>());
    ASSERT_NOEXCEPT(false, Obj(NTWRAP(func)));
}
#endif  // BSLSTL_FUNCTION_TEST_PART_01

char *strSurround(char *inout, const char *prefix, const char *suffix)
    // Replace the null-terminated string at the specified 'inout' address
    // with the concatenation of 'prefix', 'inout', and 'suffix'.
{
    std::size_t origLen = strlen(inout);
    std::size_t prefixLen = strlen(prefix);

    strcpy(inout + prefixLen + origLen, suffix);
    memmove(inout + prefixLen, inout, origLen);
    memcpy(inout, prefix, prefixLen);
    return inout;
}

//'bslstl::Function_TestRefUtil' Concept
///-------------------------------------
// The 'bslstl::Function_TestRefUtil' concept defines a contract that a utility
// 'struct' type must follow to meet the requirements of the test functions
// later introduced in this file.  A class type (the utility itself is not a
// template, it has members that are templates) that conforms to the
// 'bslstl::Function_TestRefUtil' concept must provide the following members:
//..
// 'IS_RVALUE': compile time constant 'static bool' or convertible to 'bool'
//              'true' if the 'RefUtil' is for r-value reference, 'false' for
//              l-value reference utilities.
//
// 'IS_CONST':  compile time constant 'static bool' or convertible to 'bool'
//              'true' if the 'RefUtil' reference is to 'const', 'false' if
//              the reference is to a mutable object.
//..
// There can be four concrete implementations of this utility concept:
// non-const l-value, const l-value, non-const r-value, and const r-value.  All
// four of those types are defined right below this concept definition.
//..
// 'OutType<ARG>::type': a metafunction that provides an inner type that is the
//                       kind of reference the utility represents.
//
// 'RET notDefined<ARG>()': a class method template without definition that
//                          returns the kind of reference type (created from
//                          the template 'ARG') that the utility represents
//
//  'RET xform(ARG&)': a class method template that "transforms" a non-const
//                     l-value reference argument to the kind of reference the
//                     utility represents
//
//  'xformName(argName)': Transform the name string in-place to a variation
//                        that best describes the reference type.  Note that
//                        the argument string must have enough space.
//..

struct LvalueRefUtil {
    enum { IS_RVALUE = false, IS_CONST = false };

    template <class ARG>
    struct OutType { typedef ARG& type; };

    template <class ARG>
    static ARG& declare();
        // This function is declared but not defined.

    template <class ARG>
    static ARG& xform(ARG& arg) { return arg; }

    static char* xformName(char *argName) { return argName; }
};

struct ConstLvalueRefUtil {
    enum { IS_RVALUE = false, IS_CONST = true };

    template <class ARG>
    struct OutType { typedef const ARG& type; };

    template <class ARG>
    static const ARG& declare();
        // This function is declared but not defined.

    template <class ARG>
    static const ARG& xform(ARG& arg) { return arg; }

    static char* xformName(char *argName) { return argName; }
};

struct RvalueRefUtil {
    enum { IS_RVALUE = true, IS_CONST = false };

    template <class ARG>
    struct OutType { typedef bslmf::MovableRef<ARG> type; };

    template <class ARG>
    static bslmf::MovableRef<ARG> declare();
        // This function is declared but not defined.

    template <class ARG>
    static bslmf::MovableRef<ARG> xform(ARG& arg)
        { return bslmf::MovableRefUtil::move(arg); }

    static char* xformName(char* argName)
        { return strSurround(argName, "move(", ")"); }
};

struct ConstRvalueRefUtil {
    enum { IS_RVALUE = true, IS_CONST = true };

    template <class ARG>
    struct OutType { typedef bslmf::MovableRef<const ARG> type; };

    template <class ARG>
    static bslmf::MovableRef<const ARG> declare();
        // This function is declared but not defined.

    template <class ARG>
    static bslmf::MovableRef<const ARG> xform(const ARG& arg)
        { return bslmf::MovableRefUtil::move(arg); }

    static char* xformName(char* argName)
        { return strSurround(argName, "move(", ")"); }
};

struct ConstructObjUtil {
    template <class FUNC>
    static Obj& construct(void                                    *buffer,
                          BSLS_COMPILERFEATURES_FORWARD_REF(FUNC)  func,
                          bslma::TestAllocator                    *) {
        return *::new(buffer) Obj(BSLS_COMPILERFEATURES_FORWARD(FUNC, func));
    }

    static char* xformName(char* funcName)
        { return strSurround(funcName, "Obj(", ")"); }
};

struct ConstructObjWithAllocUtil {
    template <class FUNC>
    static Obj& construct(void                                    *buffer,
                          BSLS_COMPILERFEATURES_FORWARD_REF(FUNC)  func,
                          bslma::TestAllocator                    *ta) {
        return *::new(buffer) Obj(bsl::allocator_arg, ta,
                                  BSLS_COMPILERFEATURES_FORWARD(FUNC, func));
    }

    static char* xformName(char* funcName)
        { return strSurround(funcName, "Obj(allocator_arg, alloc, ", ")"); }
};

inline bool convertTest(const Obj&)
    // Return true if the argument passed is convertible to 'bsl::function'
    // (i.e., 'Obj'); otherwise false.
{
    return true;
}

inline bool convertTest(...)
    // Return true if the argument passed is convertible to 'bsl::function'
    // (i.e., 'Obj'); otherwise false.
{
    return false;
}

template <class TYPE>
class OverloadTest {
    // Class that is convertible from 'TYPE' and from 'bsl::function'.
    int d_value;

  public:
    OverloadTest(const TYPE&) : d_value(1) { }
    OverloadTest(const Obj&) : d_value(2) { }
        // Return 1 if the argument passed is convertible to 'TYPE' or 2 if the
        // argument passed is convertible 'bsl::function'.  This function is
        // used to test that 'MovableRef<TYPE>' is a better match for 'TYPE'
        // than for 'bsl::function'; if not, then an ambiguity will be detected
        // at compile time.

    int value() const { return d_value; }

    friend bool operator==(int lhs, const OverloadTest& rhs)
        { return rhs.value() == lhs; }
};

template <class CONSTRUCT_UTIL, class REF_UTIL, class FUNC>
void testConstructFromCallableObjImp(FUNC                  func,
                                     const char           *funcName,
                                     bslma::TestAllocator *sa,
                                     bslma::TestAllocator *ta,
                                     bool                  skipExcTest,
                                     bool                  expInplace)
    // Pass-by-value required to force decay of 'FUNC'.
{
    typedef typename
        bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type TargetTp;

    // Build description string.
    char desc[160];
    CONSTRUCT_UTIL::xformName(REF_UTIL::xformName(strcpy(desc, funcName)));
    if (sa != ta) {
        strcat(desc, " (diff alloc)");
    }

    // Note that the functor constructor of 'bsl::function' forwards the
    // functor to the copy or move constructor used to place it into the
    // 'd_objbuf' object buffer.  If the supplied functor has the lvalue value
    // category, the functor will be copied.  Otherwise, the functor has an
    // rvalue value category.  If the (rvalue) functor is allocator-aware and
    // its allocator is the same as the 'bsl::function's allocator, the
    // 'bsl::function' will move-construct it into the buffer, and will
    // copy-construct it into the buffer otherwise.  If the (rvalue) functor is
    // not allocator-aware, the 'bsl::function' will move-construct it into the
    // buffer.

    // Set flags for first operation.
    const bool expCopy = !REF_UTIL::IS_RVALUE ||
                          REF_UTIL::IS_CONST  ||
                         (bslma::UsesBslmaAllocator<FUNC>::value && sa != ta);
    const bool expMove = !expCopy;

    AllocSizeType expBlocksUsed = expInplace ? 0 : 1;

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);
    bslma::TestAllocatorMonitor taMonitor(ta);
    FunctorMonitor              funcMonitor(L_);

    if (veryVerbose) printf("Construct %s\n", desc);
    EXCEPTION_TEST_BEGIN(ta, &copyMoveLimit) {
        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                ta->setAllocationLimit(-1);
                copyMoveLimit = -1;
            }
            funcMonitor.reset(L_);
            bslalg::ConstructorProxy<FUNC> funcCopyProxy(func, sa);
            FUNC&                          funcCopy = funcCopyProxy.object();
            resetMoveCopiedFlags(&NTUNWRAP(funcCopy));
            bsls::ObjectBuffer<Obj> fBuf;
            Obj&                    f = CONSTRUCT_UTIL::construct(
                fBuf.buffer(), REF_UTIL::xform(funcCopy), ta);     // CONSTRUCT
            ASSERTV(desc, isNull(NTUNWRAP(func)) == !f);
            ASSERTV(desc, ta == f.get_allocator());
            if (f) {
                ASSERTV(desc, typeid(TargetTp) == f.target_type());
                TargetTp *target_p = f.target<TargetTp>();
                ASSERTV(desc, target_p);
                if (target_p) {
                    // Test that target is moved-to from 'funcCopy'
                    ASSERTV(desc,
                            expMove ==
                                isMovedFrom(NTUNWRAP(funcCopy), expMove));
                    ASSERTV(desc, expMove == isMoved(*target_p, expMove));
                    ASSERTV(desc, expCopy == isCopied(*target_p, expCopy));
                    ASSERTV(desc, eqTarget(*target_p, func));
                    ASSERTV(desc, allocPropagationCheck<TargetTp>(f));
                    ASSERTV(desc, 0x4005 == f(IntWrapper(0x4000), 5));
                }
            }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERTV(desc, expInplace == f.isInplace());
#endif
            ASSERTV(desc, expBlocksUsed == taMonitor.numBlocksInUseChange());
            if (ta != &defaultTestAllocator) {
                ASSERTV(desc, defaultAllocMonitor.isInUseSame());
            }
            f.~Obj();                                                // DESTROY
        } EXCEPTION_TEST_CATCH {
            // Exception neutral: All memory has been released.
            ASSERTV(desc, taMonitor.isInUseSame());
            ASSERTV(desc, defaultAllocMonitor.isInUseSame());
            ASSERTV(desc, funcMonitor.isSameCount());
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    ASSERTV(desc, taMonitor.isInUseSame());
    ASSERTV(desc, defaultAllocMonitor.isInUseSame());
}

template <class FUNC>
void testConstructFromCallableObj(const FUNC&  func,
                                  const char  *funcName,
                                  bool         skipExcTest,
                                  bool         expInplace)
{
    bslma::TestAllocator  ta1("ta1", veryVeryVerbose);
    bslma::TestAllocator  ta2("ta2", veryVeryVerbose);
    bslma::TestAllocator& da = defaultTestAllocator;

#define CALL_IMP(CONSTRUCT, LRVAL, TA1, TA2)                                 \
    testConstructFromCallableObjImp<CONSTRUCT##Util, LRVAL##RefUtil>(        \
                                    func, funcName, &TA1, &TA2,              \
                                    skipExcTest, expInplace);

    CALL_IMP(ConstructObj         , Lvalue     , da , da );
    CALL_IMP(ConstructObj         , Lvalue     , ta1, da );
    CALL_IMP(ConstructObj         , Rvalue     , da , da );
    CALL_IMP(ConstructObj         , Rvalue     , ta1, da );
    CALL_IMP(ConstructObj         , ConstLvalue, da , da );
    CALL_IMP(ConstructObj         , ConstLvalue, ta1, da );
    CALL_IMP(ConstructObj         , ConstRvalue, da , da );
    CALL_IMP(ConstructObj         , ConstRvalue, ta1, da );
    CALL_IMP(ConstructObjWithAlloc, Lvalue     , ta1, ta1);
    CALL_IMP(ConstructObjWithAlloc, Lvalue     , ta1, ta2);
    CALL_IMP(ConstructObjWithAlloc, Rvalue     , ta1, ta1);
    CALL_IMP(ConstructObjWithAlloc, Rvalue     , ta1, ta2);
    CALL_IMP(ConstructObjWithAlloc, ConstLvalue, da , da );
    CALL_IMP(ConstructObjWithAlloc, ConstLvalue, ta1, da );
    CALL_IMP(ConstructObjWithAlloc, ConstRvalue, da , da );
    CALL_IMP(ConstructObjWithAlloc, ConstRvalue, ta1, da );

    using bslmf::MovableRefUtil;

    if (veryVerbose) printf("Conversion test\n");
    typename bsl::decay<FUNC>::type funcCopy(func);
    Obj f1 = funcCopy;
    ASSERT(convertTest(funcCopy));
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    // Known limitation in C++03
    Obj f2 = MovableRefUtil::move(funcCopy);
    ASSERT(convertTest(MovableRefUtil::move(funcCopy)));
#endif
    Obj f3(MovableRefUtil::move(funcCopy));
    ASSERT(convertTest(Obj(MovableRefUtil::move(funcCopy))));

    ASSERT(1 == OverloadTest<FUNC>(funcCopy));
    ASSERT(1 == OverloadTest<FUNC>(MovableRefUtil::move(funcCopy)));
#if defined(BSLS_PLATFORM_CMP_IBM)
    // IBM has trouble prioritizing rvalue-to-const-ref over user-defined
    // conversions.
    const Obj& cref = Obj(MovableRefUtil::move(funcCopy));
    ASSERT(2 == OverloadTest<FUNC>(cref));
#else
    ASSERT(2 == OverloadTest<FUNC>(Obj(MovableRefUtil::move(funcCopy))));
#endif

#undef CALL_IMP
}

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE

template <class VOID, class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructibleImp : bsl::false_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::false_type', and for which template argument
    // substitution will succeed, if the expression
    //..
    //  bsl::function<PROTOTYPE>(func);
    //..
    // is *not* well-formed, where 'func' is an expression of 'FUNC' type
    // qualified according to the 'REF_UTIL', as obtained by
    // 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructibleImp<
    typename bslmf::VoidType<decltype(
        bsl::function<PROTOTYPE>(REF_UTIL::template declare<FUNC>()))>::type,
    PROTOTYPE,
    REF_UTIL,
    FUNC> : bsl::true_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::true_type', and for which template argument
    // substitution will succeed, if the expression
    //..
    //  bsl::function<PROTOTYPE>(func);
    //..
    // is well-formed, where 'func' is an expression of 'FUNC' type qualified
    // according to the 'REF_UTIL', as obtained by 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructible
: TestIsConstructibleImp<void, PROTOTYPE, REF_UTIL, FUNC> {
    // This 'struct' template implements a boolean metafunction that publicly
    // derives from 'bsl::true_type' if a 'bsl::function<PROTOTYPE>' is
    // constructible with an object of 'FUNC' type qualified according to the
    // 'REF_UTIL'.  Otherwise, this 'struct' template publicly derives from
    // 'bsl::false_type'.
};

struct TestIsConstructibleUtil {
    // This utility 'struct' provides a suite for a set of functions used in
    // testing whether a 'bsl::function' specialization is constructible with
    // an object of a particular type.

    template <class PROTOTYPE, class REF_UTIL, class FUNC>
    static bool check()
        // Return 'true' if a 'bsl::function<PROTOTYPE>' is constructible with
        // an object of 'FUNC' type qualified according to the 'REF_UTIL', and
        // return 'false' otherwise.
        { return TestIsConstructible<PROTOTYPE, REF_UTIL, FUNC>(); }

    static char *xformName(char *funcName)
        { return strSurround(funcName, "Obj(", ")"); }
};

template <class VOID, class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructibleWithAllocImp : bsl::false_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::false_type', and for which template argument
    // substitution will succeed, if the expression
    //..
    //  bsl::function<PROTOTYPE>(bsl::allocator_arg,
    //                           bsl::allocator<char>(),
    //                           func);
    //..
    // is *not* well-formed, where 'func' is an expression of 'FUNC' type
    // qualified according to the 'REF_UTIL', as obtained by
    // 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructibleWithAllocImp<
    typename bslmf::VoidType<decltype(
        bsl::function<PROTOTYPE>(bsl::allocator_arg,
                                 bsl::allocator<char>(),
                                 REF_UTIL::template declare<FUNC>()))>::type,
    PROTOTYPE,
    REF_UTIL,
    FUNC> : bsl::true_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::true_type', and for which template argument
    // substitution will succeed, if the expression
    //..
    //  bsl::function<PROTOTYPE>(bsl::allocator_arg,
    //                           bsl::allocator<char>(),
    //                           func);
    //..
    // is well-formed, where 'func' is an expression of 'FUNC' type qualified
    // according to the 'REF_UTIL', as obtained by 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsConstructibleWithAlloc
: TestIsConstructibleWithAllocImp<void, PROTOTYPE, REF_UTIL, FUNC> {
    // This 'struct' template implements a boolean metafunction that publicly
    // derives from 'bsl::true_type' if a 'bsl::function<PROTOTYPE>' is
    // constructible with an object of 'FUNC' type qualified according to the
    // 'REF_UTIL' and an allocator.  Otherwise, this 'struct' template publicly
    // derives from 'bsl::false_type'.
};

struct TestIsConstructibleWithAllocUtil {
    // This utility 'struct' provides a suite for a set of functions used in
    // testing whether a 'bsl::function' specialization is constructible with
    // an object of a particular type and no allocator.

    template <class PROTOTYPE, class REF_UTIL, class FUNC>
    static bool check()
        // Return 'true' if a 'bsl::function<PROTOTYPE>' is constructible with
        // an object of 'FUNC' type qualified according to the 'REF_UTIL'
        // and an allocator, and return 'false' otherwise.
        { return TestIsConstructibleWithAlloc<PROTOTYPE, REF_UTIL, FUNC>(); }

    static char *xformName(char *funcName)
        { return strSurround(funcName, "Obj(allocator_arg, alloc, ", ")"); }
};

template <class PROTOTYPE,
          class FUNC,
          class TEST_IS_CONSTRUCTIBLE_UTIL,
          class REF_UTIL>
void testIsConstructibleImp(int         line,
                            const char *funcName,
                            bool        isConstructible)
    // Test the constructibility of a 'bsl::function<PROTOTYPE>' with an object
    // of 'FUNC' type qualified according to the qualifiers of the ('const'
    // and/or 'lvalue' or 'rvalue') 'REF_UTIL'.  If
    // 'TEST_IS_CONSTRUCTIBLE_UTIL' is 'TestIsConstructibleUtil', test the
    // constructibility of a 'bsl::function<PROTOTYPE>' with a so-qualified
    // 'FUNC' type without an allocator.  If 'TEST_IS_CONSTRUCTIBLE_UTIL' is
    // 'TestIsConstructibleWithAllocUtil', test the constructibility of a
    // 'bsl::function<PROTOTYPE>' with a so-qualified 'FUNC' type with a
    // specified allocator.  If 'isConstructible' is 'true', test that a
    // 'bsl::function<PROTOTYPE>' is so constructible, and test that it is not
    // so constructible otherwise.  If the test fails for any reason, log an
    // error message indicating the 'line' and 'funcName'.
{
    char desc[160];
    TEST_IS_CONSTRUCTIBLE_UTIL::xformName(
                                  REF_UTIL::xformName(strcpy(desc, funcName)));

    ASSERTV(line,
            desc,
            (isConstructible ==
             TEST_IS_CONSTRUCTIBLE_UTIL::
                 template check<PROTOTYPE, REF_UTIL, FUNC>()));
}

template <class PROTOTYPE, class FUNC>
void testIsConstructible(int          line,
                         const char  *funcName,
                         bool         isConstructible)
    // Test the constructibility of a 'bsl::function<PROTOTYPE>' with an object
    // of 'FUNC' type, with various 'const'-, 'volatile'-, and
    // reference-qualifications applied to the type of the object.  Test
    // construction with and without a specified allocator.  If
    // 'isConstructible' is 'true', test that a 'bsl::function<PROTOTYPE>' is
    // so constructible, and test that it is not so constructible otherwise.
    // If the test fails for any reason, log an error message indicating the
    // 'line' and 'funcName'.
{
#define CALL_IMP(TEST_IS_CONSTRUCTIBLE, LRVAL)                                \
    testIsConstructibleImp<PROTOTYPE,                                         \
                           FUNC,                                              \
                           TEST_IS_CONSTRUCTIBLE##Util,                       \
                           LRVAL##RefUtil>(line, funcName, isConstructible)

#if !MSVC_2015
    // MSVC 2015 and earlier have a brittle implementation of expression sfinae
    // and does not implement 2-phase name lookup correctly.  These 2 issues
    // combine to make the check for Lvalue-Callability in the single-argument
    // "functor" constructor of 'bsl::function' impossible to use in expression
    // sfinae.  For some reason, likely due to differences in overload
    // resolution, the "extended functor" constructor (the one that takes an
    // allocator) *can* be used in expression sfinae.

    CALL_IMP(TestIsConstructible         , Lvalue     );
    CALL_IMP(TestIsConstructible         , Rvalue     );
    CALL_IMP(TestIsConstructible         , ConstLvalue);
    CALL_IMP(TestIsConstructible         , ConstRvalue);
#endif

    CALL_IMP(TestIsConstructibleWithAlloc, Lvalue     );
    CALL_IMP(TestIsConstructibleWithAlloc, Rvalue     );
    CALL_IMP(TestIsConstructibleWithAlloc, ConstLvalue);
    CALL_IMP(TestIsConstructibleWithAlloc, ConstRvalue);

#undef CALL_IMP
}

#endif // defined(BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE)

template <class FUNC>
void testCopyCtorWithAlloc(FUNC        func,
                           const Obj&  original,
                           bool        extended)
{
    // Get the real functor type (in case it's wrapped).
    typedef typename
        bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type TargetTp;

    AllocSizeType expBlocksUsed, numBytesUsed;

    bslma::TestAllocator copyTa;

    // Re-create 'original' using '&copyTa' so that we can measure memory
    // usage.
    {
        Obj original2(bsl::allocator_arg, &copyTa, func);
        expBlocksUsed = copyTa.numBlocksInUse();
        numBytesUsed = copyTa.numBytesInUse();
    }
    ASSERT(copyTa.numBlocksInUse() == 0);
    ASSERT(copyTa.numBytesInUse()  == 0);

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);
    FunctorMonitor              funcMonitor(L_);

    EXCEPTION_TEST_BEGIN(&copyTa, &copyMoveLimit) {
        EXCEPTION_TEST_TRY {
            // We want to select one of two constructors at run time, so
            // instead of declaring a 'function' object directly, we create a
            // buffer that can hold a 'function' object and construct the
            // 'function' later, using the desired constructor.
            bsls::ObjectBuffer<Obj> copyBuf;

            funcMonitor.reset(L_);
            if (extended) {
                // Use extended copy constructor.
                ::new(copyBuf.buffer()) Obj(bsl::allocator_arg, &copyTa,
                                            original);
            }
            else {
                // Choose normal copy constructor with no allocator, but
                // install 'copyTa' as the allocator indirectly by setting the
                // default allocator for the duration of the constructor.
                bslma::DefaultAllocatorGuard guard(&copyTa);
                ::new(copyBuf.buffer()) Obj(original);
            }

            // 'copyBuf' now holds the copy-constructed 'function'.
            Obj& copy = copyBuf.object();

            ASSERT(copy.target_type() == original.target_type());
            ASSERT(&copyTa == copy.get_allocator());
            ASSERT(allocPropagationCheck<TargetTp>(copy));
            ASSERT(! copy == ! original);

            if (copy) {

                // Check for faithful copy of functor.
                // Address of original and copy must be different.
                ASSERT(eqTarget(*copy.target<TargetTp>(),
                                *original.target<TargetTp>()));
                ASSERT(copy.target<TargetTp>() !=
                       original.target<TargetTp>());

                // Invoke
                ASSERT(copy(IntWrapper(0x40), 9) ==
                       original(IntWrapper(0x40), 9));

                // Invocation performed identical operations on original and on
                // copy.  Check that equality relationship was not disturbed.
                ASSERT(eqTarget(*copy.target<TargetTp>(),
                                *original.target<TargetTp>()));
            }

            ASSERT(copyTa.numBlocksInUse() == expBlocksUsed);
            ASSERT(copyTa.numBytesInUse()  == numBytesUsed);
            ASSERT(defaultAllocMonitor.isInUseSame());

            copy.~Obj();
        }
        EXCEPTION_TEST_CATCH {
            // Exception neutral: All memory has been released.
            ASSERT(0 == copyTa.numBlocksInUse());
            ASSERT(defaultAllocMonitor.isInUseSame());
            ASSERT(funcMonitor.isSameCount());
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    ASSERT(copyTa.numBlocksInUse() == 0);
    ASSERT(copyTa.numBytesInUse()  == 0);
    ASSERT(defaultAllocMonitor.isInUseSame());
}

template <class FUNC>
void testCopyCtor(FUNC func)
{
    // Construct the original 'function'
    bslma::TestAllocator originalTa;
    Obj                  original(bsl::allocator_arg, &originalTa, func);

    // Snapshot allocator to ensure that nothing else is allocated from here.
    bslma::TestAllocatorMonitor originalAllocMonitor(&originalTa);

    // Test with normal and extended copy constructor
    testCopyCtorWithAlloc(func, original, /* extended */ false);
    testCopyCtorWithAlloc(func, original, /* extended */ true );
}

template <class FUNC>
void testMoveCtorWithSameAlloc(FUNC func, bool extended)
{
    if (veryVeryVerbose) {
        if (extended) {
            printf("\tExtended move: source and dest using same alloc\n");
        }
        else {
            printf("\tNormal move: dest copies source alloc.\n");
        }
    }

    typedef typename
        bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type TargetTp;

    bool isEmpty = isNull(NTUNWRAP(func));
    bool usesSmallObjectOptimization = false;

    // Construct a 'FUNC' object in a moved-from state.
    FUNC movedFromFunc(func);
    {
        FUNC movedToFunc(bslmf::MovableRefUtil::move(movedFromFunc));
        ASSERT(eqTarget(movedToFunc, func));
    }

    bslma::TestAllocator ta;

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);

    // For source and destination using the same allocator, create a pair of
    // objects with the identical memory footprint of the post-move (possibly
    // empty) source and post-move destination and measure the total memory
    // usage.
    AllocSizeType destNumBlocksUsed, destNumBytesUsed;
    {
        Obj   postMoveDest(bsl::allocator_arg, &ta, func);
        char *ctarget =
            reinterpret_cast<char*>(postMoveDest.target<TargetTp>());
        if (reinterpret_cast<char*>(&postMoveDest) <= ctarget &&
            ctarget < reinterpret_cast<char*>(&postMoveDest + 1)) {
            // Target functor falls within the footprint of the function
            // object.  Thus, the small object optimization is in use.  After
            // the move, the source 'function' wraps a 'TargetTp' object in a
            // moved-from state.
            usesSmallObjectOptimization = true;
            Obj postMoveSource(bsl::allocator_arg, &ta, movedFromFunc);
            destNumBlocksUsed = ta.numBlocksInUse();
            destNumBytesUsed = ta.numBytesInUse();
        }
        else {
            // Functor is allocated out-of-place, so ownership of it moves
            // rather than being move-construted.  The result of the move is
            // that the source is empty and the destination hold the functor.
            destNumBlocksUsed = ta.numBlocksInUse();
            destNumBytesUsed = ta.numBytesInUse();
        }
    }

    ASSERT(ta.numBlocksInUse() == 0);
    ASSERT(ta.numBytesInUse()  == 0);
    ASSERT(defaultAllocMonitor.isInUseSame());

    defaultAllocMonitor.reset();

    {
        // We want to select one of two constructors at run time, so instead
        // of declaring a 'function' object directly, we create a buffer that
        // can hold a 'function' object and construct the 'function' later
        // using the desired constructor.
        bsls::ObjectBuffer<Obj> destBuf;

        Obj source(bsl::allocator_arg, &ta, func);
        ASSERT(isEmpty == ! source);
        TargetTp *sourceTarget = isEmpty ? 0 : source.target<TargetTp>();

        if (extended) {
            // Use extended move constructor.
            ::new(&destBuf) Obj(bsl::allocator_arg, &ta,
                                bslmf::MovableRefUtil::move(source));
        }
        else {
            // Use normal move constructor
            ::new(&destBuf) Obj(bslmf::MovableRefUtil::move(source));
        }

        // 'destBuf' now holds the move-constructed 'function'.
        Obj& dest = destBuf.object();

        ASSERT(! source);
        ASSERT(&ta == source.get_allocator());
        ASSERT(&ta == dest.get_allocator());
        ASSERT(allocPropagationCheck<TargetTp>(dest));
        if (! usesSmallObjectOptimization) {
            // 'bsl::function' was moved by pointer so the address of the
            // target functor will not have changed.
            ASSERT(dest.target<TargetTp>() == sourceTarget);
        }

        ASSERT(isEmpty == ! dest);

        if (dest) {

            // Check for faithful move of functor
            ASSERT(dest.target_type() == typeid(TargetTp));
            ASSERT(eqTarget(*dest.target<TargetTp>(), func));

            // Invoke
            Obj temp(func);
            ASSERT(dest(IntWrapper(0x40), 7) == temp(IntWrapper(0x40), 7));

            // Invocation performed identical operations on func and on 'dest'.
            // Check that equality relationship was not disturbed.
            ASSERT(eqTarget(*dest.target<TargetTp>(),
                            *temp.target<TargetTp>()));
        }

        ASSERT(ta.numBlocksInUse() == destNumBlocksUsed);
        ASSERT(ta.numBytesInUse()  == destNumBytesUsed);
        ASSERT(defaultAllocMonitor.isInUseSame());

        dest.~Obj();
    }
    ASSERT(ta.numBlocksInUse() == 0);
    ASSERT(ta.numBytesInUse()  == 0);
    ASSERT(defaultAllocMonitor.isInUseSame());
}

template <class FUNC>
void testMoveCtorWithDifferentAlloc(FUNC funcArg, bool skipExcTest)
{
    if (veryVeryVerbose)
        printf("\tExtended move with different allocators.\n");

    typedef typename
        bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type TargetTp;

    // Copy argument
    const FUNC func = funcArg;

    bool isEmpty = isNull(NTUNWRAP(func));

    bslma::TestAllocator sourceTa;
    bslma::TestAllocator destTa;

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);

    // Create a pair of objects identical to the post-move source and
    // post-move destination and measure the total memory usage.
    AllocSizeType sourceNumBlocksUsed, sourceNumBytesUsed;
    AllocSizeType destNumBlocksUsed, destNumBytesUsed;
    {
        // Since allocator is different, move operation is identical to copy.
        Obj postMoveSource(bsl::allocator_arg, &sourceTa, func);
        sourceNumBlocksUsed = sourceTa.numBlocksInUse();
        sourceNumBytesUsed = sourceTa.numBytesInUse();

        Obj postMoveDest(bsl::allocator_arg, &destTa, func);
        destNumBlocksUsed = destTa.numBlocksInUse();
        destNumBytesUsed = destTa.numBytesInUse();
    }
    ASSERT(destTa.numBlocksInUse() == 0);
    ASSERT(destTa.numBytesInUse()  == 0);
    ASSERT(defaultAllocMonitor.isInUseSame());

    defaultAllocMonitor.reset();

    {
        // Create 'source'.
        Obj            source(bsl::allocator_arg, &sourceTa, func);
        FunctorMonitor funcMonitor(L_);

        EXCEPTION_TEST_BEGIN(&destTa, &copyMoveLimit) {
            EXCEPTION_TEST_TRY {
                if (skipExcTest) {
                    destTa.setAllocationLimit(-1);
                    copyMoveLimit = -1;
                }

                funcMonitor.reset(L_);

                // move-construct 'dest' using extended move constructor.
                Obj dest(bsl::allocator_arg, &destTa,
                         bslmf::MovableRefUtil::move(source));

                ASSERT(&sourceTa == source.get_allocator());

                ASSERT(isEmpty == ! dest);
                ASSERT(&destTa == dest.get_allocator());

                ASSERT(allocPropagationCheck<TargetTp>(dest));

                if (dest) {

                    // Check for faithful move of functor
                    ASSERT(dest.target_type() == typeid(TargetTp));
                    ASSERT(eqTarget(*dest.target<TargetTp>(), func));

                    // Invoke
                    Obj temp(func);
                    ASSERT(dest(IntWrapper(0x40), 7) ==
                           temp(IntWrapper(0x40), 7));

                    // Invocation performed identical operations on 'func' and
                    // on 'dest'.  Check that equality relationship was not
                    // disturbed.
                    ASSERT(eqTarget(*dest.target<TargetTp>(),
                                    *temp.target<TargetTp>()));
                }

                ASSERT(sourceTa.numBlocksInUse() == sourceNumBlocksUsed);
                ASSERT(sourceTa.numBytesInUse()  == sourceNumBytesUsed);
                ASSERT(destTa.numBlocksInUse() == destNumBlocksUsed);
                ASSERT(destTa.numBytesInUse()  == destNumBytesUsed);
                ASSERT(defaultAllocMonitor.isInUseSame());
            }
            EXCEPTION_TEST_CATCH {
                ASSERT(destTa.numBlocksInUse() == 0);
                ASSERT(destTa.numBytesInUse()  == 0);
                ASSERT(funcMonitor.isSameCount());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;
    }
    ASSERT(sourceTa.numBlocksInUse() == 0);
    ASSERT(sourceTa.numBytesInUse()  == 0);
    ASSERT(destTa.numBlocksInUse() == 0);
    ASSERT(destTa.numBytesInUse()  == 0);
    ASSERT(defaultAllocMonitor.isInUseSame());
}

template <class FUNC>
void testMoveCtor(const FUNC& func, bool skipExcTest)
{
    // Test normal move constructor.
    testMoveCtorWithSameAlloc(func, false);

    // Test extended move constructor with same allocator for source and
    // destination.
    testMoveCtorWithSameAlloc(func, true);

    // Test with different allocators for source and destination
    testMoveCtorWithDifferentAlloc(func, skipExcTest);
}

template <class FUNC>
bool AreEqualFunctions(const Obj& inA, const Obj& inB)
    // Given a known callable type specified by 'FUNC', return true if the
    // specified 'inA' wraps an object of type 'FUNC' with the same value as
    // the object wrapped by the specified 'inB' (or they are both empty);
    // otherwise return false.  As a special case, if 'inB' is the special
    // object 'movedFromMarker', then return 'true' if 'inA' is empty or wraps
    // a callable object of type 'FUNC' that holds the moved-from value of
    // 'FUNC'; otherwise return false.  This function also asserts that 'inA'
    // and 'inB' have correct allocator propagation from the 'function' object
    // to the wrapped functor.
{
    if (inA) ASSERT(allocPropagationCheck<FUNC>(inA));
    if (inB) ASSERT(allocPropagationCheck<FUNC>(inB));

    if (&inA == &inB) {
        return true;                                                  // RETURN
    }

    if (&inB == &movedFromMarker) {
        if (! inA) {
            return true;  // Empty is a valid moved-from state        // RETURN
        }

        const FUNC *targetA = inA.target<FUNC>();
        if (! targetA) {
            return false; // inA does not wrap a 'FUNC'               // RETURN
        }

        // return true only if inA's target self-reports as moved-from
        return isMovedFrom(*targetA, false);                          // RETURN
    }

    if (! inA || ! inB) {
        // One or both are empty
        if (static_cast<bool>(inA) == static_cast<bool>(inB)) {
            return true;  // Both are empty                           // RETURN
        }
        else {
            return false; // One is empty and the other is not        // RETURN
        }
    }

    if (typeid(FUNC) != inA.target_type() ||
        typeid(FUNC) != inB.target_type()) {
            // One or both wrap callable objects of the wrong type.

        return false;                                                 // RETURN
    }

    // Get the targets
    const FUNC *targetA = inA.target<FUNC>();
    const FUNC *targetB = inB.target<FUNC>();

    // Neither 'function' is empty, so neither should have a NULL target.
    ASSERT(targetA && targetB);
    ASSERT(targetA != targetB);  // Different objects have different targets

    return eqTarget(*targetA, *targetB);
}

#ifdef BSLSTL_FUNCTION_TEST_PART_05
void testSwap(const Obj& inA,
              const Obj& inB,
              bool (*    areEqualA_p)(const Obj&, const Obj&),
              bool (*    areEqualB_p)(const Obj&, const Obj&),
              int        lineA,
              int        lineB,
              bool       skipExcTest)
    // Test 'function::swap'.
{
    bslma::TestAllocator testAlloc;
    bsl::allocator<char> alloc(&testAlloc);

    // Make copies of inA and inB.
    Obj a( bsl::allocator_arg, alloc, inA);
    Obj a2(bsl::allocator_arg, alloc, inA);
    Obj b( bsl::allocator_arg, alloc, inB);
    Obj b2(bsl::allocator_arg, alloc, inB);

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);
    bslma::TestAllocatorMonitor testAllocMonitor(&testAlloc);

    // swap() should not call any potentially-throwing operations; set
    // allocation limit and move limit to detect such operations unless
    // 'skipExcTest' is true.
    testAlloc.setAllocationLimit(skipExcTest ? -1 : 0);
    copyMoveLimit = skipExcTest ? -1 : 0;
    a.swap(b);
    copyMoveLimit = -1;
    testAlloc.setAllocationLimit(-1);
    LOOP2_ASSERT(lineA, lineB, areEqualA_p(a2, b));
    LOOP2_ASSERT(lineA, lineB, areEqualB_p(b2, a));
    LOOP2_ASSERT(lineA, lineB, alloc == a.get_allocator());
    LOOP2_ASSERT(lineA, lineB, alloc == b.get_allocator());
    LOOP2_ASSERT(lineA, lineB, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAllocMonitor.isInUseSame());
    if (a2) {
        LOOP2_ASSERT(lineA, lineB, a2(1, 2) == b(1, 2));
    }
    if (b2) {
        LOOP2_ASSERT(lineA, lineB, b2(1, 2) == a(1, 2));
    }

    // Swap back using namespace-scope swap.  swap() should not call any
    // potentially-throwing operations; set allocation limit and move limit to
    // detect such operations unless 'skipExcTest' is true.
    testAlloc.setAllocationLimit(skipExcTest ? -1 : 0);
    copyMoveLimit = skipExcTest ? -1 : 0;
    swap(b, a);
    copyMoveLimit = -1;
    testAlloc.setAllocationLimit(-1);
    LOOP2_ASSERT(lineA, lineB, areEqualA_p(a2, a));
    LOOP2_ASSERT(lineA, lineB, areEqualB_p(b2, b));
    LOOP2_ASSERT(lineA, lineB, alloc == a.get_allocator());
    LOOP2_ASSERT(lineA, lineB, alloc == b.get_allocator());
    LOOP2_ASSERT(lineA, lineB, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAllocMonitor.isInUseSame());
    if (a2) {
        LOOP2_ASSERT(lineA, lineB, a2(2, 3) == a(2, 3));
    }
    if (b2) {
        LOOP2_ASSERT(lineA, lineB, b2(2, 3) == b(2, 3));
    }
}

void testAssignment(const Obj& inA,
                    const Obj& inB,
                    bool (*    areEqualA_p)(const Obj&, const Obj&),
                    bool (*    areEqualB_p)(const Obj&, const Obj&),
                    int        lineA,
                    int        lineB,
                    bool       skipExcTest)
    // Test copy and move assignment
{
    (void) skipExcTest; // Avoid unused arg warning in C++03 mode

    bslma::TestAllocator testAlloc1;
    bslma::TestAllocator testAlloc2;

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);
    bslma::TestAllocatorMonitor testAlloc1Monitor(&testAlloc1);
    bslma::TestAllocatorMonitor testAlloc2Monitor(&testAlloc2);

    // Test copy assignment
    EXCEPTION_TEST_BEGIN(&testAlloc1, &copyMoveLimit) {

        FunctorMonitor funcMonitor(L_);

        // Make copies of inA and inB.
        AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
        Obj           a(bsl::allocator_arg, &testAlloc1, inA);
        AllocSizeType aBytesBefore =
            testAlloc1.numBytesInUse() - preA1Bytes;
        Obj           b(bsl::allocator_arg, &testAlloc2, inB);

        // 'exp' should look like 'a' after the assignment
        preA1Bytes = testAlloc1.numBytesInUse();
        Obj           exp(bsl::allocator_arg, &testAlloc1, inB);
        AllocSizeType expBytes = testAlloc1.numBytesInUse() - preA1Bytes;

        preA1Bytes = testAlloc1.numBytesInUse();
        AllocSizeType preB2Bytes = testAlloc2.numBytesInUse();
        AllocSizeType preB2Total = testAlloc2.numBytesTotal();
        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                testAlloc1.setAllocationLimit(-1);
                copyMoveLimit = -1;
            }
            a = b;  ///////// COPY ASSIGNMENT //////////
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB)); // b is unchanged
            LOOP2_ASSERT(lineA, lineB, &testAlloc1 == a.get_allocator());
            LOOP2_ASSERT(lineA, lineB, &testAlloc2 == b.get_allocator());
            // Verify that memory allocator usage in a's allocator is the same
            // as destroying a and recreating it with b's functor.
            LOOP2_ASSERT(lineA, lineB,
                         postA1Bytes == preA1Bytes - aBytesBefore + expBytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
            if (a) {
                LOOP2_ASSERT(lineA, lineB, a(1, 2) == exp(1, 2));
            }
            if (b) {
                LOOP2_ASSERT(lineA, lineB, b(1, 2) == inB(1, 2));
            }
        }
        EXCEPTION_TEST_CATCH {
            // a and b are unchanged.
            LOOP2_ASSERT(lineA, lineB, areEqualA_p(a, inA));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB));

            // No memory was leaked
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    LOOP2_ASSERT(lineA, lineB, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc2Monitor.isInUseSame());

    // Test move assignment with unequal allocators
    defaultAllocMonitor.reset(&defaultTestAllocator);
    testAlloc1Monitor.reset(&testAlloc1);
    testAlloc2Monitor.reset(&testAlloc2);

    EXCEPTION_TEST_BEGIN(&testAlloc1, &copyMoveLimit) {

        FunctorMonitor funcMonitor(L_);

        // Make copies of inA and inB.
        AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
        Obj           a( bsl::allocator_arg, &testAlloc1, inA);
        AllocSizeType aBytesBefore =
            testAlloc1.numBytesInUse() - preA1Bytes;
        Obj           b( bsl::allocator_arg, &testAlloc2, inB);

        // 'exp' should look like 'a' after the assignment
        preA1Bytes = testAlloc1.numBytesInUse();
        Obj           exp(bsl::allocator_arg, &testAlloc1, inB);
        AllocSizeType expBytes = testAlloc1.numBytesInUse() - preA1Bytes;

        preA1Bytes = testAlloc1.numBytesInUse();
        AllocSizeType preB2Bytes = testAlloc2.numBytesInUse();
        AllocSizeType preB2Total = testAlloc2.numBytesTotal();
        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                testAlloc1.setAllocationLimit(-1);
                copyMoveLimit = -1;
            }
            a = bslmf::MovableRefUtil::move(b);  ///// MOVE ASSIGNMENT //////
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
            // 'b' should be either in the moved-from state or unchanged.
            LOOP2_ASSERT(lineA, lineB, (areEqualB_p(b, movedFromMarker) ||
                                        areEqualB_p(b, inB)));
            LOOP2_ASSERT(lineA, lineB,
                         &testAlloc1 == a.get_allocator());
            LOOP2_ASSERT(lineA, lineB,
                         &testAlloc2 == b.get_allocator());
            // Verify that memory allocator usage in a's allocator is the same
            // as destroying a and recreating it with b's functor.
            LOOP2_ASSERT(lineA, lineB,
                         postA1Bytes == preA1Bytes - aBytesBefore + expBytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
            if (a) {
                LOOP2_ASSERT(lineA, lineB, a(1, 2) == exp(1, 2));
            }
        }
        EXCEPTION_TEST_CATCH {
            // a and b are unchanged.
            LOOP2_ASSERT(lineA, lineB, areEqualA_p(a, inA));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB));

            // No memory was leaked
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    LOOP2_ASSERT(lineA, lineB, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc2Monitor.isInUseSame());

    // Test move assignment with equal allocators
    defaultAllocMonitor.reset(&defaultTestAllocator);
    testAlloc1Monitor.reset(&testAlloc1);

    EXCEPTION_TEST_BEGIN(&testAlloc1, &copyMoveLimit) {

        FunctorMonitor funcMonitor(L_);

        // Make copies of inA and inB.
        Obj           b( bsl::allocator_arg, &testAlloc1, inB);
        AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
        Obj           a( bsl::allocator_arg, &testAlloc1, inA);

        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                testAlloc1.setAllocationLimit(-1);
                copyMoveLimit = -1;
            }
            // Move assignment with equal allocators is the same as swap
            AllocSizeType preA1Total = testAlloc1.numBytesTotal();
            a = bslmf::MovableRefUtil::move(b);  ///// move ASSIGNMENT ////
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postA1Total = testAlloc1.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
            LOOP2_ASSERT(lineA, lineB, &testAlloc1 == a.get_allocator());
            LOOP2_ASSERT(lineA, lineB, &testAlloc1 == b.get_allocator());
            // Verify that previous memory for 'a' has been deallocated.
            LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
            // Verify that no additional memory was allocated.
            LOOP2_ASSERT(lineA, lineB, postA1Total == preA1Total);
            if (a) {
                LOOP2_ASSERT(lineA, lineB, a(1, 2) == inB(1, 2));
            }
            if (b) {
                LOOP2_ASSERT(lineA, lineB, b(1, 2) == inA(1, 2));
            }
        }
        EXCEPTION_TEST_CATCH {
            LOOP2_ASSERT(lineA, lineB,
                         0 && "Exception should not be thrown");
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP2_ASSERT(lineA, lineB, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());
}

void testAssignNullptr(const Obj& func, int line)
{
    bslma::TestAllocator testAlloc;
    bsl::allocator<char> alloc(&testAlloc);

    FunctorMonitor funcMonitor(L_);

    // Make copy of function with specified allocator
    Obj funcCopy(bsl::allocator_arg, alloc, func);

    AllocSizeType expTotalBlocks = testAlloc.numBlocksTotal();

    testAlloc.setAllocationLimit(0);    // Disable new allocations
    copyMoveLimit = 0;                      // Disable throwing-functor moves
    copyMoveLimit = 0;                      // Disable throwing-functor copies
    funcCopy = 0; ///////// Assignment from null pointer literal ////////
    copyMoveLimit = -1;
    copyMoveLimit = -1;
    testAlloc.setAllocationLimit(-1);

    LOOP_ASSERT(line, ! funcCopy);
    LOOP_ASSERT(line, alloc == funcCopy.get_allocator());
    LOOP_ASSERT(line, funcMonitor.isSameCount());
    // Test memory usage:
    //  * No new memory was allocated (total did not increase)
    //  * Memory use dropped to zero
    LOOP_ASSERT(line, expTotalBlocks == testAlloc.numBlocksTotal());
    LOOP_ASSERT(line, 0              == testAlloc.numBlocksInUse());
}
#endif  // BSLSTL_FUNCTION_TEST_PART_05

template <class FUNC, bool REFERENCE_WRAPPING>
inline
typename bsl::enable_if<!REFERENCE_WRAPPING, bool>::type
isCallable(const Obj& obj)
    // Check whether it is safe to call, for the specified parameter 'obj',
    // 'obj()' without the risk of a null pointer dereference as part of the
    // testing done in 'testAssignFromFunctorImp'.  If we are not doing
    // reference wrapping, then it is sufficient to rely on 'bsl::function's
    // 'operator bool'.
{
    return static_cast<bool>(obj);
}

template <class FUNC, bool REFERENCE_WRAPPING>
inline
typename bsl::enable_if<REFERENCE_WRAPPING &&
                            !(bsl::is_pointer<FUNC>::value ||
                              bsl::is_member_pointer<FUNC>::value),
                        bool>::type
isCallable(const Obj& obj)
    // Check whether it is safe to call, for the specified parameter 'obj',
    // 'obj()' without the risk of a null pointer dereference as part of the
    // testing done in 'testAssignFromFunctorImp'.  If we have a function
    // holding a reference wrapped non-pointer (such as a reference wrapped
    // functor object) then it is always safe.
{
    // If we are reference wrapping then the target should always be a pointer
    // to a non-null reference wrapper.  If that is not the case then report
    // the error and return false.
    ASSERT(obj);
    if (!obj) {
        return false;                                                 // RETURN
    }

    // As FUNC is not a pointer it can never be null, so return true.
    return true;
}

template <class FUNC, bool REFERENCE_WRAPPING>
inline
typename bsl::enable_if<REFERENCE_WRAPPING &&
                            (bsl::is_pointer<FUNC>::value ||
                             bsl::is_member_pointer<FUNC>::value),
                        bool>::type
isCallable(const Obj& obj)
    // Check whether it is safe to call, for the specified parameter 'obj',
    // 'obj()' without the risk of a null pointer dereference as part of the
    // testing done in 'testAssignFromFunctorImp'.  If we have a function
    // holding a reference wrapped pointer to a function (or a member function)
    // then we need to check whether that function pointer (or member function
    // pointer) is itself null.
{
    // If we are reference wrapping then the target should always be a pointer
    // to a non-null reference wrapper.  If that is not the case then report
    // the error and return false.
    ASSERT(obj);
    if (!obj) {
        return false;                                                 // RETURN
    }

    // The target is a reference wrapper of a pointer.  We do not expect the
    // 'target()' function to return null in this case, so if it does we report
    // the error and return false.
    ASSERT(obj.target<bsl::reference_wrapper<FUNC> >());
    if (!obj.target<bsl::reference_wrapper<FUNC> >()) {
        return false;                                                 // RETURN
    }

    return obj.target<bsl::reference_wrapper<FUNC> >()->get();
}

template <class FUNC, bool REFERENCE_WRAPPING>
void testAssignFromFunctorImp(const Obj&   lhsIn,
                              const FUNC&  rhsIn,
                              const char  *lhsFuncName,
                              const char  *rhsFuncName,
                              bool         skipExcTest)

{
    (void) skipExcTest; // Avoid unused var warning in C++03 mode.

    bslma::TestAllocator ta;
    bsl::allocator<char> alloc(&ta);

    bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);
    bslma::TestAllocatorMonitor testAllocMonitor(&ta);
    FunctorMonitor              funcMonitor(L_);

    // Determine whether we are dealing with a null functor or not.  This will
    // impact which tests we should carry out after assignment.
    bool rhsInIsNonNull = ! isNull(NTUNWRAP(rhsIn));

    typedef typename bsl::conditional<REFERENCE_WRAPPING,
                                      bsl::reference_wrapper<FUNC>,
                                      FUNC&>::type
        ReferenceType;

    typedef typename bsl::conditional<REFERENCE_WRAPPING,
                                      bsl::reference_wrapper<FUNC>,
                                      FUNC>::type
        RHSType;

    // Get the real functor type (in case it's wrapped).  Note that
    // 'bsl::function' will unwrap a 'NothrowMovableWrapper' but not if it is
    // the target of a 'reference_wrapper'.
    typedef typename bsl::conditional<
            REFERENCE_WRAPPING,
            bsl::reference_wrapper<FUNC>,
            typename bslalg::NothrowMovableUtil::UnwrappedType<FUNC>::type
        >::type
        TargetTp;

    // Test copy-assignment from non-const functor
    EXCEPTION_TEST_BEGIN(&ta, &copyMoveLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        AllocSizeType preBytes = ta.numBytesInUse();
        Obj           lhs(bsl::allocator_arg, alloc, lhsIn);
        AllocSizeType lhsBytesBefore = ta.numBytesInUse() - preBytes;

        // Create a 'ReferenceType' variable from a non-const version of
        // 'rhsIn'.  This variable is used *only* to construct the expected
        // value 'exp' and will *not* be used to modify 'rhsIn'.
        //
        // For the non-reference-wrapped test, this is equivalent to:
        // 'FUNC& rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));'
        //
        // For the reference-wrapped test, this is equivalent to:
        // 'bsl::reference_wrapper<FUNC>'
        // 'rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));'
        //
        // Note that, as this will only be used in the construction of 'exp',
        // the constness (or lack thereof) is irrelevant.
        ReferenceType rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));

        // Make copy of 'rhsIn'.  The copy is a non-const lvalue, but its value
        // should be unchanged by the assignment.
        //
        // In the non-reference-wrapped case, this is equivalent to the
        // following code that results in 'RHS' being a const reference to a
        // copy of 'rhsIn':
        //
        //       'FUNC         rhs(rhsIn);'
        //       'FUNC&        referenceToRhs(rhs);'
        //       'const FUNC&  RHS = referenceToRhs;'
        //
        // In the reference-wrapped case, this is equivalent to the following
        // code that results in RHS being a const reference to a reference
        // wrapped copy of 'rhsIn':
        //
        //       'FUNC                                 rhs(rhsIn);'
        //       'bsl::reference_wrapper<FUNC>         referenceToRhs(rhs);'
        //       'const bsl::reference_wrapper<FUNC>&  RHS = referenceToRhs;'
        //
        // Note that 'referenceToRhs' is used only in the construction of
        // 'RHS'.

        FUNC              rhs(rhsIn);
        ReferenceType     referenceToRhs(rhs);
        const RHSType&    RHS = referenceToRhs;

        // 'exp' is what 'lhs' should look like after the assignment.
        preBytes = ta.numBytesInUse();
        Obj           exp(bsl::allocator_arg, alloc, rhsInReferenceNonConst);
        AllocSizeType expBytes = ta.numBytesInUse() - preBytes;

        const bool expIsCallable = isCallable<FUNC, REFERENCE_WRAPPING>(exp);

        preBytes = ta.numBytesInUse();
        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                ta.setAllocationLimit(-1);
                copyMoveLimit = -1;
            }

            // Check noexceptness of the copy if applicable
 #ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
            if (REFERENCE_WRAPPING) {
                LOOP2_ASSERT(
                    lhsFuncName, rhsFuncName,
                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs = RHS));
            }
 #endif

            ///////// COPY-ASSIGNMENT FROM FUNC //////////
            lhs = RHS;

            // The number of bytes used by the lhs after the assignment is
            // equal to the number of bytes used before the assignment plus
            // the delta caused by the assignment.  Note that the delta might
            // be negative.
            AllocSizeType lhsBytesAfter = (lhsBytesBefore +
                                           ta.numBytesInUse() -
                                           preBytes);

            const bool lhsIsCallable =
                                   isCallable<FUNC, REFERENCE_WRAPPING>(lhs);

            // Make sure the emptiness is as expected:
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (REFERENCE_WRAPPING || rhsInIsNonNull) ==
                             static_cast<bool>(lhs));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == lhsIsCallable));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == expIsCallable));

            if (rhsInIsNonNull || REFERENCE_WRAPPING) {
                // Non-empty expected result
                LOOP2_ASSERT(lhsFuncName,
                             rhsFuncName,
                             lhs.target_type() == typeid(TargetTp));
                LOOP2_ASSERT(lhsFuncName,
                             rhsFuncName,
                             lhs.target<TargetTp>());
                if (lhs.target<TargetTp>()) {
                    LOOP2_ASSERT(lhsFuncName,
                                 rhsFuncName,
                                 eqTarget(*lhs.target<TargetTp>(), rhsIn));
                }
            }

            LOOP2_ASSERT(lhsFuncName, rhsFuncName, eqTarget(RHS, rhsIn));
            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         alloc == lhs.get_allocator());
            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         lhsBytesAfter == expBytes);

            if (lhsIsCallable && expIsCallable) {
                LOOP2_ASSERT(lhsFuncName, rhsFuncName, lhs(1, 2) == exp(1, 2));
            }

            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         allocPropagationCheck<TargetTp>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            LOOP2_ASSERT(lhsFuncName, rhsFuncName, eqTarget(rhs, rhsIn));
            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 testAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 funcMonitor.isSameCount());

    // Test copy-assignment from const functor
    EXCEPTION_TEST_BEGIN(&ta, &copyMoveLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        Obj lhs(bsl::allocator_arg, alloc, lhsIn);

        // Make copy of 'rhsIn'.  The copy is a non-const lvalue, but its value
        // should be unchanged by the assignment.
        //
        // In the non-reference-wrapped case, this is equivalent to the
        // following code that results in 'RHS' being a const reference to a
        // copy of 'rhsIn':
        //
        //       'FUNC         rhs(rhsIn);'
        //       'FUNC&        referenceToRhs(rhs);'
        //       'const FUNC&  RHS = referenceToRhs;'
        //
        // In the reference-wrapped case, this is equivalent to the following
        // code that results in RHS being a const reference to a reference
        // wrapped copy of 'rhsIn':
        //
        //       'FUNC                                 rhs(rhsIn);'
        //       'bsl::reference_wrapper<FUNC>         referenceToRhs(rhs);'
        //       'const bsl::reference_wrapper<FUNC>&  RHS = referenceToRhs;'
        //
        // Note that 'referenceToRhs' is used only in the construction of
        // 'RHS'.

        FUNC              rhs(rhsIn);
        ReferenceType     referenceToRhs(rhs);
        const RHSType&    RHS = referenceToRhs;


        // 'exp' is what 'lsh' should look like after the assignment.
        Obj exp(bsl::allocator_arg, alloc, rhsIn);

        // Note that, in this particular test, 'exp' is not constructed from a
        // reference wrapped function, so pass 'false' to the 'isCallable'
        // template parameter.
        const bool expIsCallable = isCallable<FUNC, false>(exp);

        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                ta.setAllocationLimit(-1);
                copyMoveLimit = -1;
            }

            // Check noexceptness of the copy if applicable
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
            if (REFERENCE_WRAPPING) {
                LOOP2_ASSERT(
                    lhsFuncName, rhsFuncName,
                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs = RHS));
            }
#endif

            // Prove that assignment can be called with const rhs.
            lhs = RHS;  // Assignment from const rhs

            const bool lhsIsCallable =
                                   isCallable<FUNC, REFERENCE_WRAPPING>(lhs);

            // Basic test that assignment produced the expected emptiness.
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (REFERENCE_WRAPPING || rhsInIsNonNull) ==
                             static_cast<bool>(lhs));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == lhsIsCallable));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == expIsCallable));

            // If we expected non-empty, do a basic check.
            if (lhsIsCallable && expIsCallable) {
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs(1, 2) == exp(1, 2));
            }

            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         allocPropagationCheck<TargetTp>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            LOOP2_ASSERT(lhsFuncName, rhsFuncName, eqTarget(rhs, rhsIn));
            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 testAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                 funcMonitor.isSameCount());

    FunctorMonitor funcMonitor2(L_);

    // Test move-assignment from functor
    EXCEPTION_TEST_BEGIN(&ta, skipExcTest ? 0 : &copyMoveLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        AllocSizeType preBytes = ta.numBytesInUse();
        Obj           lhs(bsl::allocator_arg, alloc, lhsIn);
        AllocSizeType lhsBytesBefore = ta.numBytesInUse() - preBytes;

        // Create a 'ReferenceType' variable from a non-const version of
        // 'rhsIn'.  This variable is used *only* to construct the expected
        // value 'exp' and will *not* be used to modify 'rhsIn'.
        //
        // For the non-reference-wrapped test, this is equivalent to:
        // FUNC& rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));
        //
        // For the reference-wrapped test, this is equivalent to:
        // 'bsl::reference_wrapper<FUNC>'
        // 'rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));'
        ReferenceType rhsInReferenceNonConst(const_cast<FUNC&>(rhsIn));

        // Copy 'rhsIn' so as to not change 'rhsIn'
        bslalg::ConstructorProxy<FUNC> rhsProxy(rhsIn, &ta);
        FUNC&                          rhs = rhsProxy.object();
        ReferenceType                  referenceToRhs(rhs);

        // 'exp' is what 'lhs' should look like after the assignment
        preBytes = ta.numBytesInUse();
        Obj           exp(bsl::allocator_arg, alloc, rhsInReferenceNonConst);
        AllocSizeType expBytes = ta.numBytesInUse() - preBytes;

        const bool expIsCallable = isCallable<FUNC, REFERENCE_WRAPPING>(exp);

        preBytes = ta.numBytesInUse();
        EXCEPTION_TEST_TRY {

            // Check noexceptness of the assignment if applicable
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
            if (REFERENCE_WRAPPING) {
                LOOP2_ASSERT(
                    lhsFuncName, rhsFuncName,
                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                        lhs = bslmf::MovableRefUtil::move(referenceToRhs)));
            }
#endif

            ///// MOVE-ASSIGNMENT FROM FUNC /////
            lhs = bslmf::MovableRefUtil::move(referenceToRhs);

            const bool lhsIsCallable =
                                   isCallable<FUNC, REFERENCE_WRAPPING>(lhs);

            // Basic test that assignment produced the expected emptiness.
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (REFERENCE_WRAPPING || rhsInIsNonNull) ==
                             static_cast<bool>(lhs));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == lhsIsCallable));
            LOOP2_ASSERT(lhsFuncName,
                         rhsFuncName,
                         (rhsInIsNonNull == expIsCallable));

            // The number of bytes used by the lhs after the assignment is
            // equal to the number of bytes used before the assignment plus the
            // delta caused by the assignment.  Note that the delta might be
            // negative.
            AllocSizeType lhsBytesAfter = (lhsBytesBefore +
                                           ta.numBytesInUse() -
                                           preBytes);
            if (rhsInIsNonNull || REFERENCE_WRAPPING) {
                // Non-empty expected result
                LOOP2_ASSERT(lhsFuncName, rhsFuncName, lhs);
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs.target_type() == typeid(TargetTp));
                TargetTp *target_p = lhs.target<TargetTp>();
                LOOP3_ASSERT(lhsFuncName, rhsFuncName, target_p,
                             target_p);
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             target_p && eqTarget(*target_p, rhsIn));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                // TBD: In C++03, there is a limitation in 'destructiveMove'
                // such that the target is not moved-into.
                LOOP4_ASSERT(
                    lhsFuncName,
                    rhsFuncName,
                    target_p,
                    REFERENCE_WRAPPING,
                    target_p &&
                        (REFERENCE_WRAPPING !=
                        isMoved(*target_p, !REFERENCE_WRAPPING)));
#endif
                LOOP5_ASSERT(
                    lhsFuncName,
                    rhsFuncName,
                    target_p,
                    REFERENCE_WRAPPING,
                    isMovedFrom(rhs, !REFERENCE_WRAPPING),
                    (REFERENCE_WRAPPING !=
                        isMovedFrom(rhs, !REFERENCE_WRAPPING)));
            }

            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         alloc == lhs.get_allocator());
            LOOP2_ASSERT(lhsFuncName, rhsFuncName, lhsBytesAfter == expBytes);

            if (lhsIsCallable && expIsCallable) {
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs(1, 2) == exp(1, 2));
            }

            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         allocPropagationCheck<TargetTp>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            // The LFTS does not require that 'rhs' be unchanged and, in fact,
            // the "Effects" clause would imply that the 'rhs' may change even
            // if the 'function' constructor throws.
            // LOOP2_ASSERT(lhsFuncName, rhsFuncName, RHS == rhsIn);
            LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP2_ASSERT(lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP2_ASSERT(lhsFuncName, rhsFuncName, defaultAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName, testAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lhsFuncName, rhsFuncName, funcMonitor2.isSameCount());
}

template <class FUNC>
void testAssignFromFunctor(const Obj&   lhsIn,
                           const FUNC&  rhsIn,
                           const char  *lhsFuncName,
                           const char  *rhsFuncName,
                           bool         skipExcTest)
    // This function is called as part of case 11 for the non reference_wrapped
    // case.  The specified 'lhsIn' and 'rhsIn' are passed to
    // 'testAssignFromFunctoImp' to be used therein for the creation of
    // function objects for the purpose of an assignment test.  The specified
    // 'lhsFuncName' and 'rhsFuncName' are also passed through for use in the
    // generation of diagnostic messages.  The specified 'skipExcTest' states
    // whether the exception testing should be skipped or not.
{
    testAssignFromFunctorImp<FUNC, false>(lhsIn,
                                          rhsIn,
                                          lhsFuncName,
                                          rhsFuncName,
                                          skipExcTest);
}

template <class FUNC>
typename bsl::enable_if<bsl::is_pointer<FUNC>::value, void>::type
testAssignFromFunctorRefWrap(const Obj&   lhsIn,
                             const FUNC&  rhsIn,
                             const char  *lhsFuncName,
                             const char  *rhsFuncName)
    // This function is called as part of case 11 for the reference_wrapped
    // case where FUNC is a pointer to a function.  The specified 'lhsIn' and
    // 'rhsIn' are passed to 'testAssignFromFunctorImp' wherein they will be
    // reference wrapped, prior to said reference wrappers being used for the
    // creation of function objects for the purpose of an assignment test.  The
    // specified 'lhsFuncName' and 'rhsFuncName' are also passed through for
    // use in the generation of diagnostic messages.  No exception testing will
    // be performed in this scenario.
{
    testAssignFromFunctorImp<FUNC, true>(lhsIn,
                                         rhsIn,
                                         lhsFuncName,
                                         rhsFuncName,
                                         true);
}

template <class FUNC>
typename bsl::enable_if<!bsl::is_pointer<FUNC>::value, void>::type
testAssignFromFunctorRefWrap(BSLA_MAYBE_UNUSED const Obj&   lhsIn,
                             BSLA_MAYBE_UNUSED const FUNC&  rhsIn,
                             BSLA_MAYBE_UNUSED const char  *lhsFuncName,
                             BSLA_MAYBE_UNUSED const char  *rhsFuncName)
    // This function is called as part of case 11 for the reference_wrapped
    // case where FUNC is not a pointer to a function.  This scenario is not
    // supported in C++03, but for C++11 or greater the behaviour is identical
    // to the reference_wrapped case where FUNC is a pointer.  The specified
    // 'lhsIn', 'rhsIn', 'lhsFuncName' and 'rhsFuncName' are used identically
    // to the reference wrapped case where FUNC is a pointer.
{
#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    testAssignFromFunctorImp<FUNC, true>(lhsIn,
                                         rhsIn,
                                         lhsFuncName,
                                         rhsFuncName,
                                         true);
#else
    if (veryVerbose)
        printf("Skipping testAssignFromFunctor test for lhs %s, rhs %s as "
               "this is not supported in C++03\n",
               lhsFuncName,
               rhsFuncName);
#endif
}

#ifdef BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE
template <class VOID, class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsAssignableImp : bsl::false_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::false_type', and for which template argument
    // substitution will succeed, if the expression 'f = func' is *not*
    // well-formed, where 'f' is a non-'const' lvalue expression of
    // 'bsl::function<PROTOTYPE>' type and 'func' is an expression of 'FUNC'
    // type qualified according to the 'REF_UTIL', as obtained by
    // 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsAssignableImp<
    typename bslmf::VoidType<decltype(
        LvalueRefUtil::declare<bsl::function<PROTOTYPE> >() =
            REF_UTIL::template declare<FUNC>())>::type,
    PROTOTYPE,
    REF_UTIL,
    FUNC> : bsl::true_type {
    // This 'struct' template implements a partial boolean metafunction that
    // publicly derives from 'bsl::true_type', and for which template argument
    // substitution will succeed, if the expression 'f = func' is well-formed,
    // where 'f' is a non-'const' lvalue expression of
    // 'bsl::function<PROTOTYPE>' type and 'func' is an expression of 'FUNC'
    // type qualified according to the 'REF_UTIL', as obtained by
    // 'REF_UTIL::declare<FUNC>()'.
};

template <class PROTOTYPE, class REF_UTIL, class FUNC>
struct TestIsAssignable
: TestIsAssignableImp<void, PROTOTYPE, REF_UTIL, FUNC> {
    // This 'struct' template implements a boolean metafunction that publicly
    // derives from 'bsl::true_type' if a 'bsl::function<PROTOTYPE>' is
    // assignable from an object of 'FUNC' type qualified according to the
    // 'REF_UTIL'.  Otherwise, this 'struct' template publicly derives from
    // 'bsl::false_type'.
};

struct TestIsAssignableUtil {
    // This utility 'struct' provides a suite for a set of functions used in
    // testing whether a 'bsl::function' specialization is assignable from
    // an object of a particular type.

    template <class PROTOTYPE, class REF_UTIL, class FUNC>
    static bool check()
        // Return 'true' if a 'bsl::function<PROTOTYPE>' is assignable from
        // an object of 'FUNC' type qualified according to the 'REF_UTIL', and
        // return 'false' otherwise.
        { return TestIsAssignable<PROTOTYPE, REF_UTIL, FUNC>(); }

    static char *xformName(char *funcName)
        { return strSurround(funcName, "Obj obj = ", ";"); }
};

template <class PROTOTYPE,
          class FUNC,
          class TEST_IS_ASSIGNABLE_UTIL,
          class REF_UTIL>
void testIsAssignableImp(int line, const char *funcName, bool isAssignable)
    // Test the assignability of a 'bsl::function<PROTOTYPE>' from an object of
    // 'FUNC' type qualified according to the qualifiers of the ('const' and/or
    // 'lvalue' or 'rvalue') 'REF_UTIL'.  Use the 'check' function template of
    // the 'TEST_IS_ASSIGNABLE_UTIL' template to determine whether a
    // 'bsl::function<PROTOTYPE>' is so assignable.  If 'isAssignable' is
    // 'true', test that a 'bsl::function<PROTOTYPE>' is so assignable, and
    // test that it is not so assignable otherwise.  If the test fails for any
    // reason, log an error message indicating the 'line' and 'funcName'.
{
    char desc[160];
    TEST_IS_ASSIGNABLE_UTIL::xformName(
                                  REF_UTIL::xformName(strcpy(desc, funcName)));

    ASSERTV(line,
            desc,
            (isAssignable == TEST_IS_ASSIGNABLE_UTIL::
                                 template check<PROTOTYPE, REF_UTIL, FUNC>()));
}

template <class PROTOTYPE, class FUNC>
void testIsAssignable(int line, const char *funcName, bool isAssignable)
    // Test the assignability of a 'bsl::function<PROTOTYPE>' with an object of
    // 'FUNC' type, with various 'const'-, 'volatile'-, and
    // reference-qualifications applied to the type of the object.  If
    // 'isAssignable' is 'true', test that a 'bsl::function<PROTOTYPE>' is so
    // assignable, and test that it is not so assignable otherwise.  If the
    // test fails for any reason, log an error message indicating the 'line'
    // and 'funcName'.
{
#define CALL_IMP(TEST_IS_ASSIGNABLE, LRVAL)                                   \
    testIsAssignableImp<PROTOTYPE,                                            \
                        FUNC,                                                 \
                        TEST_IS_ASSIGNABLE##Util,                             \
                        LRVAL##RefUtil>(line, funcName, isAssignable)

    CALL_IMP(TestIsAssignable, Lvalue     );
    CALL_IMP(TestIsAssignable, Rvalue     );
    CALL_IMP(TestIsAssignable, ConstLvalue);
    CALL_IMP(TestIsAssignable, ConstRvalue);

#undef CALL_IMP
}
#endif // defined(BSLSTL_FUNCTION_INVOKERUTIL_SUPPORT_IS_FUNC_INVOCABLE)

class MyPredicate {
    // This class provides a type invocable with the prototype 'bool (int)'.

  public:
    // CREATORS
    MyPredicate()
    {
    }

    // ACCESSORS
    bool operator()(int) const
    {
        return false;
    }
};

class MyPredicateWrapper {
    // This class provides a type that is implicitly convertible from a
    // 'MyPredicate'.

  public:
    // CREATORS
    MyPredicateWrapper(MyPredicate)                                 // IMPLICIT
    {
    }
};

struct MyDetectionUtil {
    // This utility 'struct' provides a suite of overload sets, 'isInt',
    // 'isMyPredicateWrapper', and 'isPredicateFunction', that each accept
    // various kinds of invocable types.  These overload sets are intended to
    // test that 'bsl::function' avoids creating overload set resolution
    // ambiguity problems by culling itself from overload resolution when the
    // compiler attempts to construct a 'bsl::function' temporary from a
    // callable type that is not compatible with the prototype of the
    // 'bsl::function'.

    // CLASS METHODS
    static bool isInt(int)
    {
        return true;
    }

    static bool isInt(const bsl::function<void()>&)
    {
        return false;
    }

    static bool isMyPredicateWrapper(const MyPredicateWrapper&)
    {
        return true;
    }

    static bool isMyPredicateWrapper(const bsl::function<void()>&)
    {
        return false;
    }

    static bool isPredicateFunction(const bsl::function<void()>&)
    {
        return false;
    }

    static bool isPredicateFunction(const bsl::function<bool(int)>&)
    {
        return true;
    }
};

// Functions for testing the workaround to the SunCC compiler bug (case 19)
template <class RET_TYPE>
void sun1(const bsl::function<RET_TYPE()>&)
{
}

template <class TYPE>
void sun2(const TYPE&)
{
}

// The following code tests a regression of DRQS94831150.  Without the fix, it
// failed to compile on Windows cl-18.00 and earlier.
namespace {

struct OuterClass {
    // A class containing a nested class.

    struct NestedClass {
        // A nested POD class.

        int d_data;     // data member used to test for correct return value
    };

    struct smallFunctor {
        // Small (empty) functor to return 'NestedClass' by value.

        NestedClass operator()(int value)
            // Return a 'NestedClass' whose 'd_data' member has the specified
            // 'value'.
        {
            NestedClass result = { value };
            return result;
        }
    };

    struct largeFunctor {
        // Small (empty) functor to return 'NestedClass' by value.

        // PUBLIC DATA
        int d_padding[20];      // padding to ensure the 'function' small
                                // object optimization does not kick in.

        // Manipulators
        NestedClass operator()(int value)
            // Return a 'NestedClass' whose 'd_data' member has the specified
            // 'value'.
        {
            NestedClass result = { value };
            return result;
        }
    };

    // PUBLIC DATA
    bsl::function<NestedClass(int)>              d_f1;
    bsl::function<NestedClass(int)>              d_f2;
    bsl::function<NestedClass(int)>              d_f3;
        // 'function' taking one 'int' argument and returning a 'NestedClass'.
    bsl::function<NestedClass(OuterClass&, int)> d_f4;
    bsl::function<NestedClass(OuterClass&, int)> d_f5;
        // 'function' taking argument of class 'OuterClass' and an 'int'
        // argument, and returning a 'NestedClass'.

    OuterClass()
        // Constructor initializes 'function' members from each of the
        // callable types above.
        : d_f1(&OuterClass::staticFunc)
        , d_f2(smallFunctor())
        , d_f3(largeFunctor())
        , d_f4(&OuterClass::memberFunc)
        , d_f5()
    {
        d_f5 = &OuterClass::memberFunc;
    }

    NestedClass memberFunc(int value);
        // Return a 'NestedClass' whose 'd_data' member has the specified
        // 'value'.  The address of this method is a pointer to
        // member-function.  Note that this function returning a nested POD
        // class by value is the trigger for the MSVC compiler bug.

    static NestedClass staticFunc(int value);
        // Return a 'NestedClass' whose 'd_data' member has the specified
        // 'value'.  The address of this method is an ordinary pointer to
        // function.  Note that this function returning a nested POD class by
        // value is the trigger for the MSVC compiler bug.
};

OuterClass::NestedClass OuterClass::memberFunc(int value)
{
    NestedClass result = { value };
    return result;
}

OuterClass::NestedClass OuterClass::staticFunc(int value)
{
    NestedClass result = { value };
    return result;
}

}  // close unnamed namespace


#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::function' cannot be deduced from the constructor parameters.
    //..
    // function()
    // function(nullptr_t);
    // function(allocator_arg_t, ALLOCATOR)
    // function(allocator_arg_t, ALLOCATOR, nullptr_t)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    struct Callable {
        float operator()(long)
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableConst {
        float operator()(long) const
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableNoexcept {
        float operator()(long) noexcept
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableConstNoexcept {
        float operator()(long) const noexcept
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableLV {
        float operator()(long) &
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableLVConst {
        float operator()(long) const &
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableLVNoexcept {
        float operator()(long) & noexcept
            // returns 0
        {
            return 0.f;
        }
    };

    struct CallableLVConstNoexcept {
        float operator()(long) const & noexcept
            // returns 0
        {
            return 0.f;
        }
    };

    struct NotCallable {
        float Named(long)
            // returns 0
        {
            return 0.f;
        }
    };

    struct MultipleCallable {
        float operator()(long) const
            // returns 0
        {
            return 0.f;
        }

        float operator()(double) const
            // returns 0
        {
            return 0.f;
        }
    };

    struct TemplateCallable {
        template <class VALUE>
        float operator ()(VALUE)
            // returns 0
        {
            return 0.f;
        }
    };

    void C_Variadic (const char *, ...) {}
        // a C-style variadic function that does nothing.

    using CallableType = float(long);
        // stripped type of the CallableXXX::operator()

    void SimpleConstructors ()
        // Test that constructing a 'bsl::function' from various combinations
        // of arguments deduces the correct type.
        //..
        // function(const function&  f)            -> decltype(f)
        // function(allocator_arg_t, ALLOCATOR, const function&  f)
        //                                                       -> decltype(f)
        // function(      function&& f)            -> decltype(f)
        // function(allocator_arg_t, ALLOCATOR,       function&& f)
        //                                                       -> decltype(f)
        //
        // function(func)
        // function(allocator_arg_t, ALLOCATOR, func)
        //..
    {
        using LambaType = double(short);
            // stripped type of the lb1 and lb2's callable method

        bsl::allocator<char>  ba;
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;
        auto                  lb1 = [](short)          -> double {return 0.0;};
        auto                  lb2 = [](short) noexcept -> double {return 0.0;};

        using RET1 = int;
        using ARG1A = float;
        bsl::function<RET1(ARG1A)> f1;
        bsl::function              f1a(f1);
        ASSERT_SAME_TYPE(decltype(f1a), bsl::function<RET1(ARG1A)>);

        using RET2 = long;
        using ARG2A = long long;
        using ARG2B = unsigned char;
        bsl::function<RET2(ARG2A, ARG2B)> f2;
        bsl::function                     f2a(bsl::allocator_arg, ba, f2);
        bsl::function                     f2b(bsl::allocator_arg, a1, f2);
        bsl::function                     f2c(bsl::allocator_arg, a2, f2);
        ASSERT_SAME_TYPE(decltype(f2a), bsl::function<RET2(ARG2A, ARG2B)>);
        ASSERT_SAME_TYPE(decltype(f2b), bsl::function<RET2(ARG2A, ARG2B)>);
        ASSERT_SAME_TYPE(decltype(f2c), bsl::function<RET2(ARG2A, ARG2B)>);

        using RET3 = double;
        using ARG3A = float;
        using ARG3B = int;
        bsl::function<RET3(ARG3A, ARG3B)> f3;
        bsl::function                     f3a(std::move(f3));
        ASSERT_SAME_TYPE(decltype(f3a), bsl::function<RET3(ARG3A, ARG3B)>);

        using RET4 = bsl::allocator<double>;
        using ARG4A = float;
        bsl::function<RET4(ARG4A)> f4;
        bsl::function              f4a(bsl::allocator_arg, ba, std::move(f4));
        bsl::function              f4b(bsl::allocator_arg, a1, std::move(f4));
        bsl::function              f4c(bsl::allocator_arg, a2, std::move(f4));
        ASSERT_SAME_TYPE(decltype(f4a), bsl::function<RET4(ARG4A)>);
        ASSERT_SAME_TYPE(decltype(f4b), bsl::function<RET4(ARG4A)>);
        ASSERT_SAME_TYPE(decltype(f4c), bsl::function<RET4(ARG4A)>);

        using RET5 = float;
        using ARG5A = short;
        using ARG5B = signed long;
        RET5(*proc5)(ARG5A, ARG5B) = nullptr;

        bsl::function               f5a(proc5);
        bsl::function               f5b(Callable{});
        bsl::function               f5c(CallableConst{});
        bsl::function               f5d(CallableNoexcept{});
        bsl::function               f5e(CallableConstNoexcept{});
        bsl::function               f5f(CallableLV{});
        bsl::function               f5g(CallableLVConst{});
        bsl::function               f5h(CallableLVNoexcept{});
        bsl::function               f5i(CallableLVConstNoexcept{});
        bsl::function               f5j(lb1);
        bsl::function               f5k(lb2);
        ASSERT_SAME_TYPE(decltype(f5a), bsl::function<RET5(ARG5A, ARG5B)>);
        ASSERT_SAME_TYPE(decltype(f5b), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5c), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5d), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5e), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5f), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5g), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5h), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5i), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f5j), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f5k), bsl::function<LambaType>);

        using RET6 = float;
        using ARG6A = short;
        using ARG6B = signed long;
        RET6(*proc6)(ARG6A, ARG6B) = nullptr;

        bsl::function f6a1(bsl::allocator_arg, ba, proc6);
        bsl::function f6a2(bsl::allocator_arg, a1, proc6);
        bsl::function f6a3(bsl::allocator_arg, a2, proc6);
        bsl::function f6b1(bsl::allocator_arg, ba, Callable{});
        bsl::function f6b2(bsl::allocator_arg, a1, Callable{});
        bsl::function f6b3(bsl::allocator_arg, a2, Callable{});
        bsl::function f6c1(bsl::allocator_arg, ba, CallableConst{});
        bsl::function f6c2(bsl::allocator_arg, a1, CallableConst{});
        bsl::function f6c3(bsl::allocator_arg, a2, CallableConst{});
        bsl::function f6d1(bsl::allocator_arg, ba, CallableNoexcept{});
        bsl::function f6d2(bsl::allocator_arg, a1, CallableNoexcept{});
        bsl::function f6d3(bsl::allocator_arg, a2, CallableNoexcept{});
        bsl::function f6e1(bsl::allocator_arg, ba, CallableConstNoexcept{});
        bsl::function f6e2(bsl::allocator_arg, a1, CallableConstNoexcept{});
        bsl::function f6e3(bsl::allocator_arg, a2, CallableConstNoexcept{});
        bsl::function f6f1(bsl::allocator_arg, ba, CallableLV{});
        bsl::function f6f2(bsl::allocator_arg, a1, CallableLV{});
        bsl::function f6f3(bsl::allocator_arg, a2, CallableLV{});
        bsl::function f6g1(bsl::allocator_arg, ba, CallableLVConst{});
        bsl::function f6g2(bsl::allocator_arg, a1, CallableLVConst{});
        bsl::function f6g3(bsl::allocator_arg, a2, CallableLVConst{});
        bsl::function f6h1(bsl::allocator_arg, ba, CallableLVNoexcept{});
        bsl::function f6h2(bsl::allocator_arg, a1, CallableLVNoexcept{});
        bsl::function f6h3(bsl::allocator_arg, a2, CallableLVNoexcept{});
        bsl::function f6i1(bsl::allocator_arg, ba, CallableLVConstNoexcept{});
        bsl::function f6i2(bsl::allocator_arg, a1, CallableLVConstNoexcept{});
        bsl::function f6i3(bsl::allocator_arg, a2, CallableLVConstNoexcept{});
        bsl::function f6j1(bsl::allocator_arg, ba, lb1);
        bsl::function f6j2(bsl::allocator_arg, a1, lb1);
        bsl::function f6j3(bsl::allocator_arg, a2, lb1);
        bsl::function f6k1(bsl::allocator_arg, ba, lb2);
        bsl::function f6k2(bsl::allocator_arg, a1, lb2);
        bsl::function f6k3(bsl::allocator_arg, a2, lb2);

        ASSERT_SAME_TYPE(decltype(f6a1), bsl::function<RET6(ARG6A, ARG6B)>);
        ASSERT_SAME_TYPE(decltype(f6a2), bsl::function<RET6(ARG6A, ARG6B)>);
        ASSERT_SAME_TYPE(decltype(f6a3), bsl::function<RET6(ARG6A, ARG6B)>);
        ASSERT_SAME_TYPE(decltype(f6b1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6b2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6b3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6c1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6c2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6c3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6d1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6d2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6d3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6e1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6e2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6e3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6f1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6f2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6f3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6g1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6g2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6g3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6h1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6h2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6h3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6i1), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6i2), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6i3), bsl::function<CallableType>);
        ASSERT_SAME_TYPE(decltype(f6j1), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f6j2), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f6j3), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f6k1), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f6k2), bsl::function<LambaType>);
        ASSERT_SAME_TYPE(decltype(f6k3), bsl::function<LambaType>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_FUNCTION_COMPILE_FAIL_C_VARIADIC
#ifdef  BSLSTL_FUNCTION_COMPILE_FAIL_C_VARIADIC
        bsl::function f97(C_Variadic);
        // This should fail to compile (no unique signature to detect)
#endif

// #define BSLSTL_FUNCTION_COMPILE_FAIL_MULTIPLE_CALL_OPERATOR
#ifdef  BSLSTL_FUNCTION_COMPILE_FAIL_MULTIPLE_CALL_OPERATOR
        bsl::function f98(MultipleCallable{});
        // This should fail to compile (no unique signature to detect)
#endif

// #define BSLSTL_FUNCTION_COMPILE_FAIL_TEMPLATE_CALL_OPERATOR
#ifdef  BSLSTL_FUNCTION_COMPILE_FAIL_TEMPLATE_CALL_OPERATOR
        bsl::function f98(TemplateCallable{});
        // This should fail to compile (no unique signature to detect)
#endif

// #define BSLSTL_FUNCTION_COMPILE_FAIL_NO_CALL_OPERATOR
#ifdef  BSLSTL_FUNCTION_COMPILE_FAIL_NO_CALL_OPERATOR
        bsl::function f99(NotCallable{});
        // This should fail to compile (no operator () to find)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
                     verbose = argc > 2;

#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART
                 veryVerbose = argc > 3;
             veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    // Use a test allocator as the global allocator.  The global allocator is
    // used only for global singleton objects, so it's use within this test
    // would always be an error.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    // Use a test allocator for the default allocator.  This allocator is used
    // when no other allocator is supplied.
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultTestAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultTestAllocator == bslma::Default::defaultAllocator());

    // Top-level monitor to make sure that every functor constructor is
    // matched with a destructor.
    FunctorMonitor topFuncMonitor(L_);
#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART
    case 25: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples in the component-level documentation
        //:   compile and run as expected.
        //
        // Plan:
        //: 1 Copy the usage example code from the header file into the test
        //:   driver and execute it here.
        //
        // Testing:
        //      USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLES"
                            "\n======================\n");
#ifdef BSLSTL_FUNCTION_TEST_PART_09
        usageExample1();
        usageExample2();
        usageExample3();
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING IF 'U_ENABLE_DEPRECATIONS' IS DISABLED
        //
        // Concern:
        //: 1 That we don't ship with 'U_ENABLE_DEPRECATIONS' set.
        //
        // Plan:
        //: 1 Assert that the 'U_ENABLE_DEPRECATIONS' macro is defined as '0'.
        //
        // Testing:
        //   0 == U_ENABLE_DEPRECATIONS
        // --------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING IF 'U_ENABLE_DEPRECATIONS' IS DISABLED"
              "\n==============================================\n");

        ASSERT(0 == U_ENABLE_DEPRECATIONS);
      } break;
      case 23: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Construction from iterators deduces the value type from the value
        //:   type of the iterator.
        //
        //: 2 Construction with a 'bslma::Allocator *' deduces the correct
        //:   specialization of 'bsl::allocator' for the type of the allocator.
        //
        // Plan:
        //: 1 Create a vector by invoking the constructor without supplying the
        //:   template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test;
        (void) test; // This variable only exists for ease of IDE navigation.
#else
        puts("CTAD is not supported by this compiler or its configuration.");
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO 'bdef_Function'
        //
        // Concerns:
        //: 1 An reference of type 'bsl::function<PROTOTYPE>&' is convertible
        //:   to a reference of type 'bdef_Function<PROTOTYPE *>&' referring to
        //:   the same object.
        //:
        //: 2 A const 'bsl::function' object is convertible to a const
        //:   reference to 'bdef_Function'.
        //
        // Plan:
        //: 1 Since 'bdef_Function' is not defined (to avoid circular
        //:   dependencies), define 'BloombergLP::bdef_Function<PROTOTYPE *>'
        //:   to be derived from 'bsl::function<PROTOTYPE>'.
        //:
        //: 2 Create an object of type 'bsl::function<int(const char *)>'.
        //:   Initialize a reference to 'bdef_Function<int(*)(const char *)>'
        //:   from that object.  Verify that the addresses of the original
        //:   object and the implicitly-converted reference are the same.
        //:   (C-1)
        //:
        //: 3 Repeat step 2 using const references.
        //
        // Testing:
        //  operator BloombergLP::bdef_Function<PROTOTYPE *>&();
        //  const operator BloombergLP::bdef_Function<PROTOTYPE *>&() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION TO 'bdef_Function'"
                            "\n=====================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        using BloombergLP::bdef_Function;

        bsl::function<int(const char *)>           original;
        bsl::function<int(const char *)> const&    ORIGINAL = original;
        bdef_Function<int(*)(const char *)>&       converted = original;
        bdef_Function<int(*)(const char *)> const& CONVERTED = ORIGINAL;

        ASSERT(&converted == &ORIGINAL);
        ASSERT(&CONVERTED == &ORIGINAL);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR SFINAE DURING OVERLOAD RESOLUTION
        //
        // Concerns:
        //: 1 The SFINAE constraints on the functor constructor of
        //:   'bsl::function', which ensure that the constructor only
        //:   participates in overload resolution if the supplied functor is
        //:   invocable according to the prototype of the 'bsl::function',
        //:   allow one to create an overload set for a function, for example,
        //..
        //  void registerCallback(const bsl::function<void(   )>& callback);
        //  void registerCallback(const bsl::function<void(int)>& callback);
        //..
        //:   such that one can invoke functions like 'registerCallback' and
        //:   have no ambiguity errors because the overloads that take
        //:   incompatible 'bsl::function's for the supplied argument are
        //:   culled during overload resolution.  For example, the following
        //:   would not be an ambiguous call:
        //..
        //  registerCallback([](int status)
        //  {
        //      bsl::cout << "status: " << status << "\n";
        //  });
        //..
        //:   because the lambda can bind to an argument of type
        //:   'const bsl::function<void(int)>&' but not
        //:   'const bsl::function<void()>&'.
        //
        // Plan:
        //: 1 Create a suite of overload sets in which each overload accepts
        //:   1 argument of an invocable type or 'bsl::function'
        //:   specialization.
        //:
        //: 2 Invoke each overload set with invocable types and 'bsl::function'
        //:   objects with various signatures, and observe that no ambiguity
        //:   occurs, and an overload is chosen that is compatible with the
        //:   supplied invocable / 'bsl::function'.
        //
        // Testing:
        //  bsl::function<PROTOTYPE>(FORWARD(FUNC) func);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING CONSTRUCTOR SFINAE DURING OVERLOAD RESOLUTION"
                "\n=====================================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

        const MyPredicate           myPredicate;
        const bsl::function<void()> myFunction;

        ASSERT( MyDetectionUtil::isInt(0));
        ASSERT(!MyDetectionUtil::isInt(myFunction));
        ASSERT( MyDetectionUtil::isMyPredicateWrapper(myPredicate));
        ASSERT(!MyDetectionUtil::isMyPredicateWrapper(myFunction));

        const bsl::function<bool(int)> myPredicateFunction;

        ASSERT( MyDetectionUtil::isPredicateFunction(myPredicate));
        ASSERT(!MyDetectionUtil::isPredicateFunction(myFunction));
        ASSERT( MyDetectionUtil::isPredicateFunction(myPredicateFunction));
#else  // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
        puts("Not supported by the compiler or its configuration.");
#endif

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING DRQS94831150 BUG FIX
        //
        // Concerns:
        //: 1 The MSVC 2013 and earlier MSVC compilers on Windows failed to
        //:   compile an earlier version of the 'bsl::function' constructor
        //:   when the template arguments belong to a class that is not yet
        //:   closed.  The compilation error was an incorrect diagnosis of a
        //:   mismatched calling convention when converting pointers and
        //:   appeared to be related to the return type being a POD class.
        //:   The concern of this test case is that this bug does not manifest
        //:   with the current implementation of 'bsl::function', and that
        //:   calling such a stored function does not corrupt the result.
        //
        // Plan:
        //: 1 Create a class, 'OuterClass', that has the following members:
        //:    o A nested POD-class type, 'NestedClass',
        //:    o A static member function returning 'NestedClass',
        //:    o A non-static member function returning 'NestedClass',
        //:    o A small (empty) functor whose invocation function returns
        //:      'NestedClass',
        //:    o A large functor (not suitable for small-object optimization)
        //:      whose invocation function returns 'NestedClass',
        //:    o Four member variables of type 'bsl::function'
        //:
        //: 2 Add a constructor to 'OuterClass' that initializes its four
        //:   member variables with each of the four callable types.
        //:
        //: 3 Create an instance of 'OuterClass' to force compilation of the
        //:   constructor.
        //:
        //: 4 Call each functor and verify that the returned 'NestedClass' has
        //:   the correct value.
        //:
        //: 5 For the function pointer and pointer-to-member cases, confirm
        //:   that the expected type is held in the 'target' object.
        //
        // Testing
        //  CONCERN: Workaround for MSVC compiler bug (DRQS94831150)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING DRQS94831150 BUG FIX"
                            "\n============================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09
        OuterClass testObj;

        bsl::function<OuterClass::NestedClass(OuterClass&, int)> mem_fun(
                                                      &OuterClass::memberFunc);
        ASSERT(99 == mem_fun(testObj, 99).d_data);

        ASSERT(42 == testObj.d_f1(42).d_data);
        ASSERT(13 == testObj.d_f2(13).d_data);
        ASSERT(69 == testObj.d_f3(69).d_data);
        int x = testObj.d_f4(testObj, 37).d_data;
        ASSERTV(x, 37 == x);
        ASSERT(37 == testObj.d_f4(testObj, 37).d_data);
        int y = testObj.d_f5(testObj, 73).d_data;
        ASSERTV(y, 73 == y);
        ASSERT(73 == testObj.d_f5(testObj, 73).d_data);

        typedef OuterClass::NestedClass (*SimpleFuncPtr_t)(int);

        SimpleFuncPtr_t *f1 = testObj.d_f1.target<SimpleFuncPtr_t>();
        ASSERT(f1);
        if (f1) {
            ASSERTV((void*)&OuterClass::staticFunc, (void*)f1,
                           &OuterClass::staticFunc   ==   *f1);
        }

        typedef OuterClass::NestedClass (OuterClass::*SimpleMemFuncPtr_t)(int);

        SimpleMemFuncPtr_t *f4 = testObj.d_f4.target<SimpleMemFuncPtr_t>();
        ASSERT(f4);
        if (f4) {
            ASSERTV(&OuterClass::memberFunc == *f4);
        }

        typedef OuterClass::NestedClass (OuterClass::*SimpleMemFuncPtr_t)(int);

        SimpleMemFuncPtr_t *f5 = testObj.d_f5.target<SimpleMemFuncPtr_t>();
        ASSERT(f5);
        if (f5) {
            ASSERTV(&OuterClass::memberFunc == *f5);
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING SUNCC BUG FIX
        //
        // Concerns:
        //: 1 The SunCC compiler fails to deduce an argument of type pointer to
        //:   function template specialization taking argument of type 'const
        //:   bsl::function<T()>&' if 'bsl::function' uses partial template
        //:   specialization. The concern is to ensure that this bug does not
        //:   manifest with the current implementation of 'bsl::function'.
        //
        // Plan:
        //: 1 Create a function template 'sun1' taking an argument of type
        //:   'const bsl::function<T()>&'.
        //:
        //: 2 Create a function template 'sun2' taking an argument of type
        //:   'const T&'.
        //:
        //: 3 Call 'sun2(&sun1<int>)' and verify that it compiles and runs.
        //
        // Testing:
        //  CONCERN: Workaround for SunCC bug
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SUNCC BUG FIX"
                            "\n=====================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09
        sun2(&sun1<int>);
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING TYPES AND TRAITS
        //
        // Concerns:
        //: 1 For an instantiation of 'function' F,
        //:   'bslma::UsesBslmaAllocator<F>::value' is true.
        //:
        //: 2 For an instantiation of 'function' F,
        //:   'bslmf::UsesAllocatorArgT<F>::value' is true.
        //:
        //: 3 For an instantiation of 'function' F,
        //:   'bsl::uses_allocator<F, bsl::allocator<char> >::value' is true.
        //:
        //: 4 For an instantiation of 'function' F,
        //:   'bsl::is_nothrow_move_constructible<F>::value' is true.
        //:
        //: 5 For an instantiation, 'function<RET(ARGS...)>', the nested
        //:   'allocator_type' is 'bsl::allocator<char>'.
        //:
        //: 6 For an instantiation, 'function<RET(ARGS...)>', the nested
        //:   'result_type' is 'RET'.
        //:
        //: 7 For an instantiation, 'function<RET(ARG)>', the nested
        //:   'argument_type' is 'ARG'.
        //:
        //: 8 For an instantiation, 'function<RET(ARG1, ARG2)>', the nested
        //:   'first_argument_type' is 'ARG1' and the nested
        //:   'second_argument_type' is 'ARG2'.
        //
        // Plan:
        //: 1 For concern 1, verify that
        //:   'bslma::UsesBslmaAllocator<bsl::function<void()> >::value'
        //:   verify is 'true'.
        //:
        //: 2 For concern 2, verify that
        //:   'bslmf::UsesAllocatorArgT<bsl::function<void()> >::value'
        //:   is 'true'.
        //:
        //: 3 For concern 3, verify that
        //:   'bsl::uses_allocator<bsl::function<void()>,bsl::allocator<char>>'
        //:   is 'true'.
        //:
        //: 4 For concern 4, verify that
        //:   'bsl::is_nothrow_move_constructible<bsl::function<void()> >
        //:   ::value' is 'true'.
        //:
        //: 5 For concerns 5-8, instantiate 'function' with various prototypes
        //:   and verify that the expected nested typedefs exist and match
        //:   their expected type.
        //
        // Testing:
        //  bslma::UsesBslmaAllocator<function<PROTOTYPE> >
        //  bslmf::UsesAllocatorArgT<function<PROTOTYPE> >
        //  bsl::uses_allocator<function<PROTOTYPE>, bsl::allocator<char> >
        //  bsl::is_nothrow_move_constructible<function<PROTOTYPE> >
        //  allocator_type
        //  result_type
        //  argument_type
        //  first_argument_type
        //  second_argument_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPES AND TRAITS"
                            "\n========================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09
        typedef bsl::allocator<char>                bslAllocator;

        typedef bsl::function<void()>               Obj1;
        typedef bsl::function<float(const double&)> Obj2;
        typedef bsl::function<int(double, char&)>   Obj3;

        if (veryVerbose) printf("Testing traits\n");

        ASSERT(bslma::UsesBslmaAllocator<Obj1>::value);
        ASSERT(bslma::UsesBslmaAllocator<Obj2>::value);
        ASSERT(bslma::UsesBslmaAllocator<Obj3>::value);

        ASSERT(bslmf::UsesAllocatorArgT<Obj1>::value);
        ASSERT(bslmf::UsesAllocatorArgT<Obj2>::value);
        ASSERT(bslmf::UsesAllocatorArgT<Obj3>::value);

        ASSERT((bsl::uses_allocator<Obj1, bslAllocator>::value));
        ASSERT((bsl::uses_allocator<Obj2, bslAllocator>::value));
        ASSERT((bsl::uses_allocator<Obj3, bslAllocator>::value));

        ASSERT(bsl::is_nothrow_move_constructible<Obj1>::value);
        ASSERT(bsl::is_nothrow_move_constructible<Obj2>::value);
        ASSERT(bsl::is_nothrow_move_constructible<Obj3>::value);

        if (veryVerbose) printf("Testing nested types\n");

        ASSERT((bsl::is_same<bslAllocator, Obj1::allocator_type>::value));
        ASSERT((bsl::is_same<bslAllocator, Obj2::allocator_type>::value));
        ASSERT((bsl::is_same<bslAllocator, Obj3::allocator_type>::value));

        ASSERT((bsl::is_same<void,  Obj1::result_type>::value));
        ASSERT((bsl::is_same<float, Obj2::result_type>::value));
        ASSERT((bsl::is_same<int,   Obj3::result_type>::value));

        ASSERT((bsl::is_same<const double&, Obj2::argument_type>::value));

        ASSERT((bsl::is_same<double, Obj3::first_argument_type>::value));
        ASSERT((bsl::is_same<char&,  Obj3::second_argument_type>::value));
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // FUNCTION OBJECT INVOCATION
        //  Functors are the remaining kind of object that can be stored in a
        //  'bsl::function'.  This test will resolve all remaining concerns for
        //  calling 'operator()', completing its testing.
        //
        // Concerns:
        //: 1 A 'bsl::function' object that is constructed with a functor (aka
        //:   function object) can be invoked as if it were a copy of that
        //:   functor.
        //:
        //: 2 Invocation works for zero to ten arguments and yields the
        //:   expected return value and side-effects on the functor.
        //:
        //: 3 Functions can return 'void'.
        //:
        //: 4 If the return value of the 'bsl::function' is 'void', then the
        //:   return value of function-pointer invocation is discarded,
        //:   even if the return type of the pointer-to-function is non-void.
        //:
        //: 5 The prototype for a function pointer need not be an exact match
        //:   for the parameter type of a 'bsl::function' type -- So long as
        //:   each formal argument to the 'bsl::function' is implicitly
        //:   convertible to the corresponding argument to the function pointer
        //:   and the return type of invocation through the function pointer
        //:   is implicitly convertible to the return type of the
        //:   'bsl::function'.
        //:
        //: 6 Invocation works correctly whether the functor is empty, fits
        //:   within the small-object optimization, or is allocated on the
        //:   heap.
        //:
        //: 7 Invocation works correctly 'bsl::function' objects wrapped within
        //:   other 'bsl::function' object.
        //:
        //: 8 Side effects are observed even if the 'bsl::function' is
        //:   const.  This surprising fact comes from the idea that a
        //:   'function' object is an abstraction of a pointer to a
        //:   function.  Moreover, type erasure means that, at compile time, it
        //:   is not possible to determine whether the callable object
        //:   cares whether or not it is const.
        //:
        //: 9 When the constructor's functor argument is wrapped using a
        //:   'bslalg::NothrowMovableWrapper', invocation procedes as though
        //:   the wrapper were not present.
        //:
        //: 10 The target can be a C++11 lambda expression.
        //
        // Plan:
        //: 1 Create a set of functor class with ten overloads of
        //:   'operator()', taking 0 to 13 arguments.  The first argument (for
        //:   all but the zero-argument case) is of type 'IntWrapper' and the
        //:   remaining arguments are of type 'int'.  These invocation
        //:   operators add all of the arguments to the integer state member
        //:   in the functor and returns the result.
        //:
        //: 2 For concerns 1 and 2, implement a test function template,
        //:   'testWithFunctor' that constructs constructs one instance of the
        //:   specified functor type and creates 13 instances of
        //:   'bsl::function' instantiated for the specified object type,
        //:   specified return type, and 0 to 12 arguments of the specified
        //:   argument type.  The test function constructs each instance of
        //:   'bsl::function' with a copy of the functor and then invokes
        //:   it, verifying that the return value and side-effects are as
        //:   expected.
        //:
        //: 3 For concern 3, add to the functor class another 'operator()'
        //:   Taking a 'const char *' argument and returning void.  Verify that
        //:   a 'bsl::function' object this invoker can be invoked and has the
        //:   expected size-effect.
        //:
        //: 4 For concern 4, create a 'bsl::function' with parameter
        //:   'void(int)' and verify that it can be used to wrap the functor
        //:   invoked with a single argument (discarding the return value).
        //:
        //: 5 For concern 5, instantiate 'testWithFunctor' with using a class
        //:   'ConvertibleToInt' instead of 'int' for the argument types
        //:   and using 'IntWrapper' instead of 'int' for the return type.
        //:
        //: 6 For concern 6, repeat each of the above steps with stateless,
        //:   small, and large functor classes by instantiating
        //:   'testWithFunctor' with 'EmptyFunctor', 'SmallFunctor', and
        //:   'LargeFunctor'.  It is not necessary to test with 'MediumFunctor'
        //:   as that does not test anything not already tested by
        //:   'SmallFunctor'.
        //:
        //: 7 For concern 7, create an 'bsl::function' nested within another
        //:   'bsl::function' with a different, but compatible, prototype.
        //:   Verify that invoking the outer function produces the same result
        //:   and invoking the inner function.
        //:
        //: 8 For concern 9, augment step 2 with a const 'bsl::function'
        //:   object. It is necessary to test only one set of arguments in
        //:   order to have confidence in the result.
        //:
        //: 9 For concern 9, augment step 2, wrapping the constructor argument
        //:   in a 'bslalg::NothrowMovableWrapper'.  It is necessary to test
        //:   only one set of arguments in order to have confidence in the
        //:   result.
        //:
        //: 10 For concern 10, create a 'bsl::function' that wraps a C++11
        //:   lambda expression, with and without captured arguments.  Test
        //:   that the target info are as expected and that the lambda can be
        //:   invoked through the 'bsl::function'.  Note that it is not
        //:   necessary to repeat all of the previous tests, as a lambda is
        //:   just a special case of a callable class object.
        //
        // Testing:
        //  RET operator()(ARGS...) const; // User-defined functor target
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION OBJECT INVOCATION"
                            "\n==========================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09
        if (veryVerbose) printf("Plan step 2\n");
        testWithFunctor<SmallFunctor, int, int>("SmallFunctor int(int...)");

        if (veryVerbose) printf("Plan step 5\n");
        testWithFunctor<SmallFunctor, IntWrapper, ConvertibleToInt>(
            "SmallFunctor IntWrapper(ConvertibleToInt...)");

        if (veryVerbose) printf("Plan step 6\n");
        testWithFunctor<EmptyFunctor, int, int>("EmptyFunctor int(int...)");
        testWithFunctor<EmptyFunctor, IntWrapper, ConvertibleToInt>(
            "EmptyFunctor IntWrapper(ConvertibleToInt...)");
        testWithFunctor<LargeFunctor, int, int>("LargeFunctor int(int...)");
        testWithFunctor<LargeFunctor, IntWrapper, ConvertibleToInt>(
            "LargeFunctor IntWrapper(ConvertibleToInt...)");

        InnerFunction innerFunc(&simpleFunc);
        ASSERT(3 == innerFunc(1, 2));
        Obj doubleWrappedFunc(innerFunc);
        ASSERT(3 == doubleWrappedFunc(1, 2));

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        // C++11 -- assume lambda support.

        // Test Lambda with no capture
        auto lambda1 = [](int x){ return 2 * x; };
        bsl::function<int(int)> f1(lambda1);
        ASSERT(typeid(lambda1) == f1.target_type());
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        ASSERT(f1.target<decltype(lambda1)>());
#endif
        ASSERT(4 == f1(2));

        // Test Lambda with capture by copy
        int multiplier = 3;
        auto lambda2 = [multiplier](int x){ return multiplier * x; };
        bsl::function<int(int)> f2(lambda2);
        ASSERT(typeid(lambda2) == f2.target_type());
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
        ASSERT(f2.target<decltype(lambda2)>());
#endif
        ASSERT(6 == f2(2));
#endif // C++11

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // POINTER TO MEMBER DATA INVOCATION
        //
        //  All of the concerns refer to an object 'f' of type
        //  'bsl::function<RET(T)>', for a specified return type 'RET' and
        //  class 'T'.  f's constructor argument is a pointer 'dp' to data
        //  member of type 'DT FT::*' for a specified data type 'DT' and class
        //  type 'FT'.  The invocation argument is an 'obj' of type 'T'.
        //
        // Concerns:
        //: 1 If 'T' is the same as 'FT&', invoking 'f(obj)' yields the same
        //:   return value as 'obj.*dp'.
        //:
        //: 2 If 'T' is the same as 'FT', invoking 'f(obj)' yields the same
        //:   return value as 'obj.*dp', and will access the data member of
        //:   the copy of 'obj' made when it was passed by value, not 'obj'
        //:   itself.
        //:
        //: 3 If 'T' is the same as 'FT*', or "smart pointer" to 'FT', invoking
        //:   'f(obj)' yields the same result as '(*obj).*dp'.
        //:
        //: 4 The return type 'RET' need not match the data-member type 'DT'
        //:   so long as 'RET' is implicitly convertible to 'DT' or a
        //:   reference-to 'DT'.
        //:
        //: 5 If 'dp' is a pointer to const data member, then 'T' can be
        //:   rvalue of, reference to, pointer to, or smart-pointer to either a
        //:   const or non-const type.  All of the above concerns apply to both
        //:   const and non-const data members.
        //:
        //: 6 Concerns 1 and 2 also apply if 'T' is an rvalue of, reference to,
        //:   pointer to, or smart-pointer to a type derived or
        //:   virtually-derived from 'FT'.
        //:
        //: 7 If 'RET' is 'void', then the return value of the access to 'dp'
        //:   is discarded.
        //:
        //: 8 If 'RET' is a reference to 'DT', the address of the reference
        //:   returned by 'f(obj)' is equal to the address of 'obj.*dp'.
        //:
        //: 9 When 'dp' is wrapped using a 'bslalg::NothrowMovableWrapper',
        //:   invocation proceeds as though the wrapper were not present.
        //:
        //: 10 In C++11 and later, if 'T' is an rvalue-reference qualified
        //:    type, 'RET' may be any type that is explicitly convertible from
        //:    an xvalue expression of 'DT' type.  Note that these are the same
        //:    semantics as when 'T' is a non-reference-qualified ("object")
        //:    type.  Note that the standard 'std::function' only permits 'RET'
        //:    types that are implicitly convertible from such 'DT'-typed
        //:    expressions.  'bsl::function' extends the set of allowable 'RET'
        //:    types for backwards compatibility.
        //:
        //: 11 In C++11 and later, if 'T' is an rvalue-reference qualified
        //:    type, or a non-reference-qualified ("object") type, 'RET' may be
        //:    any rvalue-reference qualified type that is explicitly
        //:    convertible from an (xvalue) expression of 'DT' type.  Note that
        //:    the standard 'std::function' only permits 'RET' types that are
        //:    implicitly convertible from such 'DT'-typed expressions.
        //:    'bsl::function' extends the set of allowable 'RET' types for
        //:    backwards compatibility.
        //
        // Plan:
        //: 1 Create a class 'IntHolder' that holds an 'int' value as a public
        //:   data member named 'd_value', and a class 'ConstIntHolder' that
        //:   holds a 'const int' value as a public data member.
        //:
        //: 2 For concern 1, implement a test function template
        //:   'testPtrToMemData' that instantiates a 'bsl::function'
        //:   instantiated for a specified object type and specified return
        //:   type, given the address of 'IntHolder::d_value'.  Then, invoke
        //:   'testPtrToMemData' with object type 'IntHolder&' and verify that
        //:   the return value is as expected.
        //:
        //: 3 For concern 2, ensure that 'testPtrtoMemData' checks for no
        //:   change to 'obj' if 'T' is not a pointer or reference type.
        //:   Invoke 'testPtrToMemData' with object type 'IntHolder'.
        //:
        //: 4 For concern 3, invoke 'testPtrToMemData' with object types
        //:   'IntHolder*', and 'SmartPtr<IntHolder>'.
        //:
        //: 5 For concern 4, invoke 'testPtrToMemData' with 'RET' types
        //:   'DT' and 'const DT&', as well as 'DT&' when 'T' is not a
        //:   const-qualified reference type.
        //:
        //: 6 For concern 5, implement a test function template,
        //:   'testPtrToConstMemData', that works similarly to
        //:   'testPtrToMemData' except that it wraps the const data member
        //:   'ConstIntHolder::d_value' instead of the non-const data member
        //:   'IntHolder::d_value'.  Invoke 'testPtrToConstMemData' with object
        //:   types 'ConstIntHolder', 'ConstIntHolder&', 'ConstIntHolder*', and
        //:   'SmartPtr<ConstIntHolder>', as well as 'const' versions of the
        //:   preceding, and corresponding 'const' and/or reference-qualified
        //:   versions of the return type.
        //:
        //: 7 For concern 6, create classes 'IntHolderDerived' and
        //:   'IntHolderVirtuallyDerived', respectively derived and virtually
        //:   derived from 'IntHolder'.  Invoke 'testPtrToMemData' with object
        //:   types 'IntHolderDerived', 'IntHolderDerived&',
        //:   'IntHolderDerived*', and 'SmartPtr<IntHolderDerived', as well as
        //:   'const' versions of the preceding, and corresponding 'const'
        //:   and/or reference-qualified versions of the return type.
        //:
        //: 8 For concern 7, add a check in 'testPtrToMemData' and
        //:   'testPtrToConstMemData' that a 'bsl::function' specialization
        //:   with a 'void' 'RET' is constructible from pointers to data
        //:   members, and that invoking such functions has no effect (outside
        //:   of, perhaps, dereferencing the pointer to the data member).
        //:
        //: 9 For concern 8, where both 'T' and 'RET' types are reference
        //:   types, take the address of the result of 'f(obj)' and verify that
        //:   it is equal to the address of the corresponding data member of
        //:   'obj', which is '&obj.*dp'.
        //:
        //: 10 For concern 9, add a check in 'testPtrToMemData' and
        //:    'testPtrToConstMemData' that a 'bsl::function' object
        //:    constructed using a pointer to a data member wrapped by a
        //:    'bslalg::NothrowMovableWrapper' has the same behavior as a
        //:    'bsl:function' object constructed with the non-wrapped pointer
        //:    to data member.
        //:
        //: 11 For concerns 10 and 11, use 'testPtrToMemData' and
        //:    'testPtrToConstMemData' with combinations of rvalue-reference
        //:    qualified 'T' and/or 'RET' types, and check that a
        //:    'bsl::function' with such a prototype is constructible from a
        //:    pointer to an (optionally const) data member if the criteria in
        //:    those concerns are met.
        //
        // Testing:
        //  RET operator()(ARGS...) const; // data-member pointer target
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO MEMBER DATA INVOCATION"
                            "\n=================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_09

#define TEST(c, d)       testPtrToMemData<c, d>(#d "(" #c ")")
#define TEST_CONST(c, d) testPtrToConstMemData<c, d>(#d "(" #c ")")

        if (veryVerbose) printf("Plan step 3\n");
        TEST(IntHolder&, int       );
        TEST(IntHolder&, int&      );
        TEST(IntHolder&, const int&);
        TEST(IntHolder , int       );

        if (veryVerbose) printf("Plan step 4\n");
        TEST(IntHolder *        , int       );
        TEST(IntHolder *        , int&      );
        TEST(IntHolder *        , const int&);
        TEST(SmartPtr<IntHolder>, int       );
        TEST(SmartPtr<IntHolder>, int&      );
        TEST(SmartPtr<IntHolder>, const int&);

        if (veryVerbose) printf("Plan step 5\n");
        TEST(const IntHolder&         , int       );
        TEST(const IntHolder&         , const int&);
        TEST(const IntHolder          , int       );
        TEST(const IntHolder *        , int       );
        TEST(const IntHolder *        , const int&);
        TEST(SmartPtr<const IntHolder>, int       );
        TEST(SmartPtr<const IntHolder>, const int&);

        if (veryVerbose) printf("Plan step 6\n");
        TEST_CONST(ConstIntHolder&               , int       );
        TEST_CONST(ConstIntHolder&               , const int&);
        TEST_CONST(ConstIntHolder                , int       );
        TEST_CONST(ConstIntHolder *              , int       );
        TEST_CONST(ConstIntHolder *              , const int&);
        TEST_CONST(SmartPtr<ConstIntHolder>      , int       );
        TEST_CONST(SmartPtr<ConstIntHolder>      , const int&);
        TEST_CONST(const ConstIntHolder&         , int       );
        TEST_CONST(const ConstIntHolder&         , const int&);
        TEST_CONST(const ConstIntHolder          , int       );
        TEST_CONST(const ConstIntHolder *        , int       );
        TEST_CONST(const ConstIntHolder *        , const int&);
        TEST_CONST(SmartPtr<const ConstIntHolder>, int       );
        TEST_CONST(SmartPtr<const ConstIntHolder>, const int&);

        if (veryVerbose) printf("Plan step 7\n");
        TEST(IntHolderDerived&                        , int       );
        TEST(IntHolderDerived&                        , int&      );
        TEST(IntHolderDerived&                        , const int&);
        TEST(IntHolderDerived                         , int       );
        TEST(IntHolderDerived *                       , int       );
        TEST(IntHolderDerived *                       , int&      );
        TEST(IntHolderDerived *                       , const int&);
        TEST(SmartPtr<IntHolderDerived>               , int       );
        TEST(SmartPtr<IntHolderDerived>               , int&      );
        TEST(SmartPtr<IntHolderDerived>               , const int&);
        TEST(const IntHolderDerived&                  , int       );
        TEST(const IntHolderDerived&                  , const int&);
        TEST(const IntHolderDerived                   , int       );
        TEST(const IntHolderDerived *                 , int       );
        TEST(const IntHolderDerived *                 , const int&);
        TEST(SmartPtr<const IntHolderDerived>         , int       );
        TEST(SmartPtr<const IntHolderDerived>         , const int&);
        TEST(IntHolderVirtuallyDerived&               , int       );
        TEST(IntHolderVirtuallyDerived&               , int&      );
        TEST(IntHolderVirtuallyDerived&               , const int&);
        TEST(IntHolderVirtuallyDerived                , int       );
        TEST(IntHolderVirtuallyDerived *              , int       );
        TEST(IntHolderVirtuallyDerived *              , int&      );
        TEST(IntHolderVirtuallyDerived *              , const int&);
        TEST(SmartPtr<IntHolderVirtuallyDerived>      , int       );
        TEST(SmartPtr<IntHolderVirtuallyDerived>      , int&      );
        TEST(SmartPtr<IntHolderVirtuallyDerived>      , const int&);
        TEST(const IntHolderVirtuallyDerived&         , int       );
        TEST(const IntHolderVirtuallyDerived&         , const int&);
        TEST(const IntHolderVirtuallyDerived          , int       );
        TEST(const IntHolderVirtuallyDerived *        , int       );
        TEST(const IntHolderVirtuallyDerived *        , const int&);
        TEST(SmartPtr<const IntHolderVirtuallyDerived>, int       );
        TEST(SmartPtr<const IntHolderVirtuallyDerived>, const int&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        if (veryVerbose) printf("Plan step 10");
        TEST(IntHolder&&                      , int        );
        TEST(IntHolder&&                      , int&&      );
        TEST(IntHolder&&                      , const int  );
        TEST(IntHolder&&                      , const int& );
        TEST(IntHolder&&                      , const int&&);
        TEST(const IntHolder&&                , int        );
        TEST(const IntHolder&&                , const int  );
        TEST(const IntHolder&&                , const int& );
        TEST(const IntHolder&&                , const int&&);
        TEST(IntHolderDerived&&               , int        );
        TEST(IntHolderDerived&&               , int&&      );
        TEST(IntHolderDerived&&               , const int  );
        TEST(IntHolderDerived&&               , const int& );
        TEST(IntHolderDerived&&               , const int&&);
        TEST(const IntHolderDerived&&         , int        );
        TEST(const IntHolderDerived&&         , const int  );
        TEST(const IntHolderDerived&&         , const int& );
        TEST(const IntHolderDerived&&         , const int&&);
        TEST(IntHolderVirtuallyDerived&&      , int        );
        TEST(IntHolderVirtuallyDerived&&      , int&&      );
        TEST(IntHolderVirtuallyDerived&&      , const int  );
        TEST(IntHolderVirtuallyDerived&&      , const int& );
        TEST(IntHolderVirtuallyDerived&&      , const int&&);
        TEST(const IntHolderVirtuallyDerived&&, int        );
        TEST(const IntHolderVirtuallyDerived&&, const int  );
        TEST(const IntHolderVirtuallyDerived&&, const int& );
        TEST(const IntHolderVirtuallyDerived&&, const int&&);
        TEST(SmartPtr<IntHolder>&&            , int        );
        TEST(SmartPtr<IntHolder>&&            , int&       );
        TEST(SmartPtr<IntHolder>&&            , int&&      );
        TEST(SmartPtr<IntHolder>&&            , const int  );
        TEST(SmartPtr<IntHolder>&&            , const int& );
        TEST(SmartPtr<IntHolder>&&            , const int&&);
        TEST(SmartPtr<const IntHolder>&&      , int        );
        TEST(SmartPtr<const IntHolder>&&      , const int  );
        TEST(SmartPtr<const IntHolder>&&      , const int& );
        TEST(SmartPtr<const IntHolder>&&      , const int&&);

        {
            bsl::function<int(IntHolder &&)> function(&IntHolder::d_value);
            IntHolder                        intHolder(0x2001);
            ASSERT(0x2001 == function(static_cast<IntHolder&&>(intHolder)));
        }

        {
            bsl::function<int && (IntHolder &&)> function(&IntHolder::d_value);
            IntHolder                            intHolder(0x2001);
            ASSERT(0x2001 == function(static_cast<IntHolder&&>(intHolder)));
        }

        {
            bsl::function<const int&(IntHolder &&)> function(
                &IntHolder::d_value);
            IntHolder intHolder(0x2001);
            ASSERT(0x2001 == function(static_cast<IntHolder&&>(intHolder)));
            ASSERT(&function(static_cast<IntHolder&&>(intHolder)) ==
                   &intHolder.d_value);
        }

        {
            bsl::function<const int && (IntHolder &&)> function(
                &IntHolder::d_value);
            IntHolder intHolder(0x2001);
            ASSERT(0x2001 == function(static_cast<IntHolder&&>(intHolder)));
        }

#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#undef TEST_CONST
#undef TEST

        if (veryVerbose) printf("Plan step 9\n");
        bsl::function<int& (IntHolder&)> ftr(&IntHolder::d_value);
        IntHolder ihr(0x2001);
        ftr(ihr) = 0x2002;
        ASSERT(0x2002 == ihr.d_value);
        ASSERT(&ftr(ihr) == &ihr.d_value);

        bsl::function<int& (IntHolder *)> ftp(&IntHolder::d_value);
        IntHolder ihp(0x2001);
        ftp(&ihp) = 0x2002;
        ASSERT(0x2002 == ihp.d_value);
        ASSERT(&ftp(&ihp) == &ihp.d_value);

        bsl::function<int& (SmartPtr<IntHolder>)> ftsp(&IntHolder::d_value);
        IntHolder ihsp(0x2001);
        ftsp(&ihsp) = 0x2002;
        ASSERT(0x2002 == ihsp.d_value);
        ASSERT(&ftsp(&ihsp) == &ihsp.d_value);

        bsl::function<int& (IntHolder&)> ftrd(&IntHolder::d_value);
        IntHolderDerived ihrd(0x2001);
        ftrd(ihrd) = 0x2002;
        ASSERT(0x2002 == ihrd.d_value);
        ASSERT(&ftrd(ihrd) == &ihrd.d_value);

        bsl::function<int& (IntHolder *)> ftpd(&IntHolder::d_value);
        IntHolderDerived ihpd(0x2001);
        ftpd(&ihpd) = 0x2002;
        ASSERT(0x2002 == ihpd.d_value);
        ASSERT(&ftpd(&ihpd) == &ihpd.d_value);

        bsl::function<int& (SmartPtr<IntHolder>)> ftspd(&IntHolder::d_value);
        IntHolderDerived ihspd(0x2001);
        ftspd(&ihspd) = 0x2002;
        ASSERT(0x2002 == ihspd.d_value);
        ASSERT(&ftspd(&ihspd) == &ihspd.d_value);

        bsl::function<const int& (IntHolder&)> ftcr(&IntHolder::d_value);
        IntHolder ihcr(0x2001);
        ASSERT(&ftcr(ihcr) == &ihcr.d_value);

        bsl::function<const int& (IntHolder *)> ftcp(&IntHolder::d_value);
        IntHolder ihcp(0x2001);
        ASSERT(&ftcp(&ihcp) == &ihcp.d_value);

        bsl::function<const int&(SmartPtr<IntHolder>)> ftcsp(
            &IntHolder::d_value);
        IntHolder ihcsp(0x2001);
        ASSERT(&ftcsp(&ihcsp) == &ihcsp.d_value);

        bsl::function<const int&(const IntHolder&)> ftccr(&IntHolder::d_value);
        IntHolder ihccr(0x2001);
        ASSERT(&ftccr(ihccr) == &ihccr.d_value);

        bsl::function<const int&(const IntHolder *)> ftccp(
            &IntHolder::d_value);
        IntHolder ihccp(0x2001);
        ASSERT(&ftccp(&ihccp) == &ihccp.d_value);

        bsl::function<const int&(SmartPtr<const IntHolder>)> ftccsp(
            &IntHolder::d_value);
        IntHolder ihccsp(0x2001);
        ASSERT(&ftccsp(&ihccsp) == &ihccsp.d_value);
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_09
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // POINTER TO MEMBER FUNCTION INVOCATION
        //
        //  All of the following concerns refer to an object 'f' of type
        //  'bsl::function<RET(T, ARGS...)>' for a specified return type
        //  'RET', class type 'T', and 0 or more additional argument types
        //  'ARGS...'.  f's constructor argument is a pointer 'fp' to member
        //  function of type 'FRET (FT::*)(FARGS...)' for a specified return
        //  type 'FRET', class type 'FT', and 0 or more argument types
        //  'FARGS...'.  The invocation arguments are 'obj' of type 'T' and
        //  'args...' of types matching 'ARGS...'.
        //
        // Concerns:
        //: 1 Invocation works for zero to nine arguments, 'args...' in
        //:   addition to the 'obj' argument and yields the expected return
        //:   value.
        //:
        //: 2 If 'T' is the same as 'FT&', invoking 'f(obj, args...)'
        //:   yields the same return value and side-effect as invoking
        //:   '(obj.*fp)(args...)'.
        //:
        //: 3 If 'T' is the same as 'FT', invoking 'f(obj, args...)'
        //:   yields the same return value as invoking '(obj.*fp)(args...)'
        //:   and will have no side effect on the (pass-by-value) 'obj'.
        //:
        //: 4 If 'T' is the same as 'FT*' or "smart pointer" to 'FT', invoking
        //:   'f(obj, args...)'  yields the same results as invoking
        //:   '((*obj).*fp)(args...)'.
        //:
        //: 5 The template argument types 'ARGS...' need not match the
        //:   member-function arguments 'FARGS...' exactly, so long as the
        //:   argument lists are the same length and each type in 'ARGS' is
        //:   implicitly convertible to the corresponding argument in
        //:   'FARGS'.
        //:
        //: 6 The return type 'RET' need not match the member-function return
        //:   type 'FRET' so long as 'RET' is implicitly convertible to
        //:   'FRET'.
        //:
        //: 7 If 'fp' is a pointer to const member function, then 'T' can be
        //:   rvalue of, reference to, pointer to, or smart-pointer to either
        //:   a const or a non-const type.  All of the above concerns apply to
        //:   both const and non-const member functions.
        //:
        //: 8 Concerns 1 and 2 also apply if 'T' is an rvalue of, reference to,
        //:   pointer to, or smart-pointer to type derived from 'FT'.
        //:
        //: 9 If 'RET' is 'void', then the return value of 'pf' is discarded,
        //:   even if 'FRET' is non-void.
        //:
        //: 10 When the 'fp' is wrapped using 'bslalg::NothrowMovableWrapper',
        //:   invocation proceeds as though the wrapper were not present.
        //
        // Plan:
        //: 1 Create a class 'IntWrapper' that holds an 'int' value and has
        //:   const member functions 'add0' to 'add9' and non-const member
        //:   functions 'increment0' to 'increment9' and 'voidIncrement0' to
        //:   'voidIncrement9' each taking 0 to 12 'int' arguments.  The
        //:   'sum[0-12]' functions return the 'int' sum of the arguments + the
        //:   wrapper's value.  The 'increment[0-12]' functions increment the
        //:   wrapper's value by the sum of the arguments and returns the
        //:   'int' result.  The 'voidIncrement[0-12]' functions increment the
        //:   wrapper's value by the sum of the arguments and return nothing.
        //:
        //: 2 For concern 1, implement a test function template
        //:   'testPtrToMemFunc' that creates 13 instances of 'bsl::function'
        //:   instantiated for the specified object type, specified return
        //:   type, and 0 to 12 arguments of the specified argument type.  The
        //:   test function constructs each instance with a pointer to the
        //:   corresponding 'increment[0-12]' member function of 'IntWrapper'
        //:   and then invokes it, verifying that the return value and
        //:   side-effects are as expected.
        //:
        //: 3 For concern 2, invoke 'testPtrToMemFunc' with object type
        //:   'IntWrapper&'.
        //:
        //: 4 For concern 3, ensure that 'testPtrToMemFunc' checks for no
        //:   change to 'obj' if 'T' is not a pointer or reference type.
        //:   Invoke 'testPtrToMemFunc' with object type 'IntWrapper'.
        //:
        //: 5 For concern 4, invoke 'testPtrToMemFunc' with object types
        //:   'IntWrapper*', and 'SmartPtr<IntWrapper>'.
        //:
        //: 6 For concerns 5 & 6, repeat steps 3, 4, and 5 except using a class
        //:   'ConvertibleToInt' instead of 'int' for the arguments in 'ARGS'
        //:   and using 'IntWrapper' instead of 'RET'.
        //:
        //: 7 For concern 7, implement a test function template,
        //:   'testPtrToConstMemFunc' that works similarly to
        //:   'testPtrToMemFunc' except that it wraps the const member
        //:   functions 'sum[0-12]' instead of the non-const member functions
        //:   'increment[0-12]'.  To save compile time, since concern 1 has
        //:   already been tested, we need to test only a small number of
        //:   possible argument-list lengths (e.g. 0, 1, and 12 arguments).
        //:   Invoke 'testPtrToConstMemFunc' with object types 'IntWrapper',
        //:   'IntWrapper&', 'IntWrapper*', and 'SmartPtr<IntWrapper>', as
        //:   well as 'const' versions of the preceding.
        //:
        //: 8 For concern 8, create a class, 'IntWrapperDerived' derived from
        //:   'IntWrapper'.  Invoke 'testPtrToConstMemFunc' with object types
        //:   'IntWrapperDerived', 'IntWrapperDerived&', 'IntWrapperDerived*',
        //:   and 'SmartPtr<IntWrapperDerived>', as well as 'const' and
        //:   versions of the preceding.
        //:
        //: 9 For concern 9, create a 'bsl::function' with prototype
        //:   'void(IntWrapper, int)' and use it to invoke
        //:   'IntWrapper::increment1', thus discarding the return value.
        //:   Repeat this test but wrapping 'IntWrapper::voidIncrement1',
        //:   showing that a 'void' function can be invoked.
        //:
        //: 10 For concern 10, add tests to 'testPtrToMemFunc' and
        //:   'testPtrToConstMemFunc' as well as to the variants in step 9
        //:   whereby the 'function' object is constructed using a
        //:   pointer-to-member-function wrapped by a
        //:   'bslalg::NothrowMovableWrapper' and verify that the behavior does
        //:   not change.
        //
        // Testing:
        //  RET operator()(ARGS...) const; // member-function pointer target
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO MEMBER FUNCTION INVOCATION"
                            "\n=====================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_08
        if (veryVerbose) printf("Plan step 3\n");
        testPtrToMemFunc<IntWrapper&, int, int>("int(IntWrapper&, int...)");

        if (veryVerbose) printf("Plan step 4\n");
        testPtrToMemFunc<IntWrapper, int, int>("int(IntWrapper, int...)");

        if (veryVerbose) printf("Plan step 5\n");
        testPtrToMemFunc<IntWrapper*, int, int>("int(IntWrapper*, int...)");
        testPtrToMemFunc<SmartPtr<IntWrapper>, int, int>(
            "int(SmartPtr<IntWrapper>, int...)");

        if (veryVerbose) printf("Plan step 6\n");
        testPtrToMemFunc<IntWrapper, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper&, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper&, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper*, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper*, ConvertibleToInt...");
        testPtrToMemFunc<SmartPtr<IntWrapper>, IntWrapper, ConvertibleToInt>(
            "IntWrapper(SmartPtr<IntWrapper>, ConvertibleToInt...");

        if (veryVerbose) printf("Plan step 7\n");
        testPtrToConstMemFunc<IntWrapper, int, int>(
            "int(IntWrapper, int...)");
        testPtrToConstMemFunc<IntWrapper&, int, int>(
            "int(IntWrapper&, int...)");
        testPtrToConstMemFunc<IntWrapper*, int, int>(
            "int(IntWrapper*, int...)");
        testPtrToConstMemFunc<SmartPtr<IntWrapper>, int, int>(
            "int(SmartPtr<IntWrapper>, int...)");
        testPtrToConstMemFunc<const IntWrapper, int, int>(
            "int(const IntWrapper, int...)");
        testPtrToConstMemFunc<const IntWrapper&, int, int>(
            "int(const IntWrapper&, int...)");
        testPtrToConstMemFunc<const IntWrapper*, int, int>(
            "int(const IntWrapper*, int...)");
        testPtrToConstMemFunc<SmartPtr<const IntWrapper>, int, int>(
            "int(SmartPtr<const IntWrapper>, int...)");

        if (veryVerbose) printf("Plan step 8\n");
        testPtrToConstMemFunc<IntWrapperDerived, int, int>(
            "int(IntWrapperDerived, int...)");
        testPtrToConstMemFunc<IntWrapperDerived&, int, int>(
            "int(IntWrapperDerived&, int...)");
        testPtrToConstMemFunc<IntWrapperDerived*, int, int>(
            "int(IntWrapperDerived*, int...)");
        testPtrToConstMemFunc<SmartPtr<IntWrapperDerived>, int, int>(
            "int(SmartPtr<IntWrapperDerived>, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived, int, int>(
            "int(const IntWrapperDerived, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived&, int, int>(
            "int(const IntWrapperDerived&, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived*, int, int>(
            "int(const IntWrapperDerived*, int...)");
        testPtrToConstMemFunc<SmartPtr<const IntWrapperDerived>, int, int>(
            "int(SmartPtr<const IntWrapperDerived>, int...)");

        if (veryVerbose) printf("Plan step 9\n");
        IntWrapper iw(0x3001);

        bsl::function<void(IntWrapper, int)> ft(&IntWrapper::increment1);
        ft(iw, 1);                     // No return type to test
        ASSERT(0x3001 == iw.value());  // Passed by value. Original unchanged.

        bsl::function<void(IntWrapper&, int)> ftr(&IntWrapper::increment1);
        ftr(iw, 2);                    // No return type to test
        ASSERT(0x3003 == iw.value());

        bsl::function<void(IntWrapper*, int)> ftp(&IntWrapper::increment1);
        ftp(&iw, 4);                   // No return type to test
        ASSERT(0x3007 == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>,
                           int)> ftsp(&IntWrapper::increment1);
        ftsp(&iw, 8);                  // No return type to test
        ASSERT(0x300f == iw.value());

        bsl::function<void(IntWrapper*, int)>
            ntftp(NTWRAP(&IntWrapper::increment1));
        ntftp(&iw, 0x10);                   // No return type to test
        ASSERT(0x301f == iw.value());

        bsl::function<void(IntWrapper, int)> vt(&IntWrapper::voidIncrement1);
        vt(iw, 0x10);                  // No return type to test
        ASSERT(0x301f == iw.value());  // Passed by value. Original unchanged.

        bsl::function<void(IntWrapper&, int)> vtr(&IntWrapper::voidIncrement1);
        vtr(iw, 0x20);                 // No return type to test
        ASSERT(0x303f == iw.value());

        bsl::function<void(IntWrapper*, int)> vtp(&IntWrapper::voidIncrement1);
        vtp(&iw, 0x40);                // No return type to test
        ASSERT(0x307f == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>,
                           int)> vtsp(&IntWrapper::voidIncrement1);
        vtsp(&iw, 0x80);               // No return type to test
        ASSERT(0x30ff == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>, int)>
            ntvtsp(NTWRAP(&IntWrapper::voidIncrement1));
        vtsp(&iw, 0x100);               // No return type to test
        ASSERT(0x31ff == iw.value());

        MutatingFunctor                  incrementing;
        const bsl::function<long long()> mutator(incrementing);
        ASSERT(1 == mutator());
        ASSERT(2 == mutator());
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_08
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // POINTER TO FUNCTION INVOCATION
        //
        // Concerns:
        //: 1 A 'bsl::function' object that is constructed with a pointer to a
        //:   non-member function can be invoked as if it were that function.
        //:
        //: 2 Invocation works for zero to ten arguments and yields the
        //:   expected return value.
        //:
        //: 3 The prototype for a function pointer need not be an exact match
        //:   for the parameter type of a 'bsl::function' type -- So long as
        //:   each formal argument to the 'bsl::function' is implicitly
        //:   convertible to the corresponding argument to the function pointer
        //:   and the return type of invocation through the function pointer
        //:   is implicitly convertible to the return type of the
        //:   'bsl::function'.
        //:
        //: 4 Functions can return 'void'.
        //:
        //: 5 If the return value of the 'bsl::function' is 'void', then the
        //:   return value of function-pointer invocation is discarded,
        //:   even if the return type of the pointer-to-function is non-void.
        //:
        //: 6 Arguments that are supposed to be passed by reference *are*
        //:   passed by reference all the way through the invocation
        //:   interface.
        //:
        //: 7 Arguments that are supposed to be passed by value are copied
        //:   exactly once when passed through the invocation interface.
        //:
        //: 8 When the function pointer argument is wrapped using a
        //:   'bslalg::NothrowMovableWrapper', invocation procedes as though
        //:   the wrapper were not present.
        //
        // Plan:
        //: 1 Create a set of functions, 'sum0' to 'sum10' taking 0 to 13
        //:   arguments.  The first argument (for all but 'sum0') is of type
        //:   'IntWrapper' and the remaining arguments are of type 'int'.  The
        //:   return value is an 'int' comprising the sum of the arguments +
        //:   '0x4000'.
        //:
        //: 2 For concerns 1 and 2, create and invoke 'bsl::function's
        //:   wrapping pointers to each of the functions 'sum0' to 'sum10'.
        //:   Verify that the return from the invocations matches the expected
        //:   results.
        //:
        //: 3 For concern 3, repeat step 2 except instantiate the
        //:   'bsl::function' objects with prototypes with arguments of type
        //:   'ConvertibleToInt' and return type 'IntWrapper'.
        //:
        //: 4 For concern 4, create a global function, 'increment' that
        //:   increments its argument (passed by address) and returns void.
        //:   Verify that a 'bsl::function' object wrapping a pointer to
        //:   'increment' can be invoked and has the expected size-effect.
        //:
        //: 5 For concern 5, create a 'bsl::function' with parameter
        //:   'void(int)' and verify that it can be used to wrap 'sum1'
        //:   (discarding the return value).
        //:
        //: 6 For concern 6, implement a set of functions, 'getAddress' and
        //:   'getConstAddress' that return the address of their argument,
        //:   which is passed by reference and passed by const reference,
        //:   respectively. Wrap pointers to these functions in
        //:   'bsl::function' objects with the same signature and verify that
        //:   they return the address of their arguments.
        //:
        //: 7 For concern 7, implement a class 'CountCopies' whose copy
        //:   constructor increments a counter, so that you can keep track of
        //:   how many copies-of-copies get made. Implement a function
        //:   'numCopies' that takes a 'CountCopies' object by value and
        //:   returns the number of times it was copied.  Verify that, when
        //:   invoked through a 'bsl::function' wrapper, the argument is
        //:   copied only once.
        //:
        //: 8 For concern 8, repeat a few test cases from the steps 2 through
        //:   7, wrapping the functoin-pointer argument in a nothrow wrapper.
        //:   Note that it is not necessary to test every combination of 0 to
        //:   13 arguments in order to have confidence that the constructor
        //:   argument is being correctly unwrapped.
        //
        // Testing:
        //  RET operator()(ARGS...) const; // function pointer target
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO FUNCTION INVOCATION"
                            "\n==============================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_08
        if (veryVerbose) printf("Plan step 2 with function references\n");
        {
            const bsl::function<int()> f0(sum0);
            ASSERT(0x4000 == f0());

            const bsl::function<int(int)> f1(sum1);
            ASSERT(0x4001 == f1(1));

            const bsl::function<int(int, int)> f2(sum2);
            ASSERT(0x4003 == f2(1, 2));

            const bsl::function<int(int, int, int)> f3(sum3);
            ASSERT(0x4007 == f3(1, 2, 4));

            const bsl::function<int(int, int, int, int)> f4(sum4);
            ASSERT(0x400f == f4(1, 2, 4, 8));

            const bsl::function<int(int, int, int, int, int)> f5(sum5);
            ASSERT(0x401f == f5(1, 2, 4, 8, 0x10));

            const bsl::function<int(int, int, int, int, int, int)> f6(sum6);
            ASSERT(0x403f == f6(1, 2, 4, 8, 0x10, 0x20));

            const bsl::function<int(int, int, int, int, int, int, int)> f7(
                                                                         sum7);
            ASSERT(0x407f == f7(1, 2, 4, 8, 0x10, 0x20, 0x40));

            const bsl::function<int(int, int, int, int, int, int, int, int)>
                                                                      f8(sum8);
            ASSERT(0x40ff == f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));

            const bsl::function<int(int, int, int, int, int, int, int, int,
                                    int)> f9(sum9);
            ASSERT(0x41ff == f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            const bsl::function<int(int, int, int, int, int, int, int, int,
                                    int, int)> f10(sum10);
            ASSERT(0x43ff == f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVerbose) printf("Repeat step 2 with function pointers\n");
        {
            const bsl::function<int()> f0(&sum0);
            ASSERT(0x4000 == f0());

            const bsl::function<int(int)> f1(&sum1);
            ASSERT(0x4001 == f1(1));

            const bsl::function<int(int, int)> f2(&sum2);
            ASSERT(0x4003 == f2(1, 2));

            const bsl::function<int(int, int, int)> f3(&sum3);
            ASSERT(0x4007 == f3(1, 2, 4));

            const bsl::function<int(int, int, int, int)> f4(&sum4);
            ASSERT(0x400f == f4(1, 2, 4, 8));

            const bsl::function<int(int, int, int, int, int)> f5(&sum5);
            ASSERT(0x401f == f5(1, 2, 4, 8, 0x10));

            const bsl::function<int(int, int, int, int, int, int)> f6(&sum6);
            ASSERT(0x403f == f6(1, 2, 4, 8, 0x10, 0x20));

            const bsl::function<int(int, int, int, int, int, int, int)> f7(
                                                                        &sum7);
            ASSERT(0x407f == f7(1, 2, 4, 8, 0x10, 0x20, 0x40));

            const bsl::function<int(int, int, int, int, int, int, int, int)>
                                                                     f8(&sum8);
            ASSERT(0x40ff == f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));

            const bsl::function<int(int, int, int, int, int, int, int, int,
                                    int)> f9(&sum9);
            ASSERT(0x41ff == f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            const bsl::function<int(int, int, int, int, int, int, int, int,
                                    int, int)> f10(&sum10);
            ASSERT(0x43ff == f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVerbose) printf("Plan step 3 with function references\n");
        {
            typedef IntWrapper       Ret;
            typedef ConvertibleToInt Arg;

            const Arg a1(0x0001);
            const Arg a2(0x0002);
            const Arg a3(0x0004);
            const Arg a4(0x0008);
            const Arg a5(0x0010);
            const Arg a6(0x0020);
            const Arg a7(0x0040);
            const Arg a8(0x0080);
            const Arg a9(0x0100);
            const Arg a10(0x0200);
            const Arg a11(0x0400);
            const Arg a12(0x0800);
            const Arg a13(0x1000);

            const bsl::function<Ret()> f0(sum0);
            ASSERT(0x4000 == f0());

            const bsl::function<Ret(Arg)> f1(sum1);
            ASSERT(0x4001 == f1(a1));

            const bsl::function<Ret(Arg, Arg)> f2(sum2);
            ASSERT(0x4003 == f2(a1, a2));

            const bsl::function<Ret(Arg, Arg, Arg)> f3(sum3);
            ASSERT(0x4007 == f3(a1, a2, a3));

            const bsl::function<Ret(Arg, Arg, Arg, Arg)> f4(sum4);
            ASSERT(0x400f == f4(a1, a2, a3, a4));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg)> f5(sum5);
            ASSERT(0x401f == f5(a1, a2, a3, a4, a5));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg)> f6(sum6);
            ASSERT(0x403f == f6(a1, a2, a3, a4, a5, a6));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg)> f7(
                                                                         sum7);
            ASSERT(0x407f == f7(a1, a2, a3, a4, a5, a6, a7));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg)>
                                                                      f8(sum8);
            ASSERT(0x40ff == f8(a1, a2, a3, a4, a5, a6, a7, a8));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg)> f9(sum9);
            ASSERT(0x41ff == f9(a1, a2, a3, a4, a5, a6, a7, a8, a9));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg)> f10(sum10);
            ASSERT(0x43ff == f10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg)> f11(sum11);
            ASSERT(0x47ff ==
                   f11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg, Arg)> f12(sum12);
            ASSERT(0x4fff ==
                   f12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg, Arg, Arg)> f13(sum13);
            ASSERT(0x5fff ==
                   f13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,a13));
        }

        if (veryVerbose) printf("Repeat step 3 with function pointers\n");
        {
            typedef IntWrapper       Ret;
            typedef ConvertibleToInt Arg;

            const Arg a1(0x0001);
            const Arg a2(0x0002);
            const Arg a3(0x0004);
            const Arg a4(0x0008);
            const Arg a5(0x0010);
            const Arg a6(0x0020);
            const Arg a7(0x0040);
            const Arg a8(0x0080);
            const Arg a9(0x0100);
            const Arg a10(0x0200);
            const Arg a11(0x0400);
            const Arg a12(0x0800);
            const Arg a13(0x1000);

            const bsl::function<Ret()> f0(&sum0);
            ASSERT(0x4000 == f0());

            const bsl::function<Ret(Arg)> f1(&sum1);
            ASSERT(0x4001 == f1(a1));

            const bsl::function<Ret(Arg, Arg)> f2(&sum2);
            ASSERT(0x4003 == f2(a1, a2));

            const bsl::function<Ret(Arg, Arg, Arg)> f3(&sum3);
            ASSERT(0x4007 == f3(a1, a2, a3));

            const bsl::function<Ret(Arg, Arg, Arg, Arg)> f4(&sum4);
            ASSERT(0x400f == f4(a1, a2, a3, a4));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg)> f5(&sum5);
            ASSERT(0x401f == f5(a1, a2, a3, a4, a5));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg)> f6(&sum6);
            ASSERT(0x403f == f6(a1, a2, a3, a4, a5, a6));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg)> f7(
                                                                        &sum7);
            ASSERT(0x407f == f7(a1, a2, a3, a4, a5, a6, a7));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg)>
                                                                     f8(&sum8);
            ASSERT(0x40ff == f8(a1, a2, a3, a4, a5, a6, a7, a8));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg)> f9(&sum9);
            ASSERT(0x41ff == f9(a1, a2, a3, a4, a5, a6, a7, a8, a9));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg)> f10(&sum10);
            ASSERT(0x43ff == f10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg)> f11(&sum11);
            ASSERT(0x47ff ==
                   f11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg, Arg)> f12(&sum12);
            ASSERT(0x4fff ==
                   f12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12));

            const bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                                    Arg, Arg, Arg, Arg, Arg)> f13(&sum13);
            ASSERT(0x5fff ==
                   f13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,a13));
        }

        // Test void return type
        if (veryVerbose) printf("Plan step 4\n");
        bsl::function<void(int*)> fvoid(&increment);
        int                       v = 1;
        fvoid(&v);
        ASSERT(2 == v);

        // Test discarding of return value
        if (veryVerbose) printf("Plan step 5\n");
        bsl::function<void(int)> fdiscard(&sum1);
        fdiscard(3);

        // Test pass-by-reference
        if (veryVerbose) printf("Plan step 6\n");
        bsl::function<int*(int&)> ga(getAddress);
        ASSERT(&v == ga(v));
        bsl::function<const int*(const int&)> gca(getConstAddress);
        ASSERT(&v == gca(v));
        gca(v);

        // Test pass-by-value
        if (veryVerbose) printf("Plan step 7\n");
        bsl::function<int(CountCopies)> nc(numCopies);
        CountCopies                     cc;
        ASSERT(1 == numCopies(cc));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT(1 == nc(cc));
        ASSERT(0 == numCopies(CountCopies()));
        ASSERT(0 == nc(CountCopies()));
#else
        ASSERT(2 == nc(cc));
        ASSERT(0 == numCopies(CountCopies()));
        ASSERT(1 == nc(CountCopies()));
#endif
        ASSERT(0 == cc.numCopies());

        // Test 'bslalg::NothrowMovableWrapper'
        if (veryVerbose) printf("Plan step 8\n");
        {
            // Test normal function
            const bsl::function<int(int)> f1(NTWRAP(&sum1));
            ASSERT(0x4001 == f1(1));

            // Test function with argument conversion
            typedef IntWrapper       Ret;
            typedef ConvertibleToInt Arg;

            const Arg a1(0x0001);
            const Arg a2(0x0002);

            const bsl::function<Ret(Arg, Arg)> f2(NTWRAP(&sum2));
            ASSERT(0x4003 == f2(a1, a2));

            // Test void return type
            const bsl::function<void(int*)> fvoid(NTWRAP(&increment));
            int                             v = 1;
            fvoid(&v);
            ASSERT(2 == v);

            // Test discarding of return value
            const bsl::function<void(int)> fdiscard(NTWRAP(&sum1));
            fdiscard(3);

            // Test pass-by-reference
            const bsl::function<int*(int&)> ga(NTWRAP(&getAddress));
            ASSERT(&v == ga(v));
            const bsl::function<const int*(const int&)> gca(NTWRAP(
                                                            &getConstAddress));
            ASSERT(&v == gca(v));
            gca(v);

            // Test pass-by-value
            const bsl::function<int(CountCopies)> nc(NTWRAP(&numCopies));
            CountCopies                           cc;
            ASSERT(1 == numCopies(cc));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(1 == nc(cc));
            ASSERT(0 == numCopies(CountCopies()));
            ASSERT(0 == nc(CountCopies()));
#else
            ASSERT(2 == nc(cc));
            ASSERT(0 == numCopies(CountCopies()));
            ASSERT(1 == nc(CountCopies()));
#endif
            ASSERT(0 == cc.numCopies());
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_08
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // EMPTY FUNCTION INVOCATION
        //
        // Concerns:
        //: 1 Invoking an empty function causes 'bsl::bad_function_call' to be
        //:   thrown.
        //:
        //: 2 The above is true for functions with zero to ten arguments.
        //:
        //: 3 It is irrelevant whether the empty 'function' was constructed
        //:   using the default constructor, the 'nullptr_t' constructor, or
        //:   constructor taking a pointer type.
        //
        // Plan:
        //: 1 For concerns 1, default-construct 'bsl::function' objects.
        //:   Invoke each object with suitable arguments and catch any
        //:   exceptions.  Verify that 'bad_function_call' is thrown in each
        //:   case.
        //:
        //: 2 For concern 2, repeat step 1 with different template parameters
        //:   for the constructed 'function' object, having zero to ten
        //:   function arguments.
        //:
        //: 3 For concern 3, repeat step 1 with different constructors.
        //
        // Testing:
        //  RET operator()(ARGS...) const; // No target
        // --------------------------------------------------------------------

        if (verbose) printf("\nEMPTY FUNCTION INVOCATION"
                            "\n=========================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_08

#ifdef BDE_BUILD_TARGET_EXC

        // Test 0 to 13 arguments.  Extra cases are added for testing
        // non-default constructors.
        for (int numArgs = 0; numArgs <= 13 + 4; ++numArgs) {

            if (veryVerbose) printf("with %d arguments\n", numArgs);

            int constructed = -1;

            try {
                switch (numArgs) {
                  case 0: {
                    const bsl::function<int()> f0;
                    constructed = 0;
                    f0();
                  } break;

                  case 1: {
                    const bsl::function<int(int)> f1;
                    constructed = 1;
                    f1(1);
                  } break;

                  case 2: {
                    const bsl::function<int(int, int)> f2;
                    constructed = 2;
                    f2(1, 2);
                  } break;

                  case 3: {
                    const bsl::function<int(int, int, int)> f3;
                    constructed = 3;
                    f3(1, 2, 4);
                  } break;

                  case 4: {
                    const bsl::function<int(int, int, int, int)> f4;
                    constructed = 4;
                    f4(1, 2, 4, 8);
                  } break;

                  case 5: {
                    const bsl::function<int(int, int, int, int, int)> f5;
                    constructed = 5;
                    f5(1, 2, 4, 8, 0x10);
                  } break;

                  case 6: {
                    const bsl::function<int(int, int, int, int, int, int)> f6;
                    constructed = 6;
                    f6(1, 2, 4, 8, 0x10, 0x20);
                  } break;

                  case 7: {
                    const bsl::function<int(int, int, int, int, int, int, int)>
                                                                            f7;
                    constructed = 7;
                    f7(1, 2, 4, 8, 0x10, 0x20, 0x40);
                  } break;

                  case 8: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int)> f8;
                    constructed = 8;
                    f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80);
                  } break;

                  case 9: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int, int)> f9;
                    constructed = 9;
                    f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100);
                  } break;

                  case 10: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int, int, int)> f10;
                    constructed = 10;
                    f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200);
                  } break;

                  case 11: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int, int, int, int)> f11;
                    constructed = 11;
                    f11(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200,
                        0x400);
                  } break;

                  case 12: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int, int, int, int, int)> f12;
                    constructed = 12;
                    f12(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200,
                        0x400, 0x800);
                  } break;

                  case 13: {
                    const bsl::function<int(int, int, int, int, int, int, int,
                                            int, int, int, int, int, int)> f13;
                    constructed = 13;
                    f13(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200,
                        0x400, 0x800, 0x1000);
                  } break;

                  case 14: {
                    // Test empty 'function' constructed from 'nullptr_t'
                    bsl::nullptr_t np = 0;
                    const bsl::function<int(int)> f(np);
                    constructed = 14;
                    f(14);
                  } break;

                  case 15: {
                    // Test empty 'function' constructed from null pointer to
                    // function.
                    int (*pf)(int) = 0;
                    const bsl::function<int(int)> f(pf);
                    constructed = 15;
                    f(15);
                  } break;

                  case 16: {
                    // Test empty 'function' constructed from null pointer to
                    // member function.
                    int (IntWrapper::*pmf)(int) = 0;
                    const bsl::function<int(IntWrapper, int)> f(pmf);
                    constructed = 16;
                    IntWrapper iw(0);
                    f(iw, 16);
                  } break;

                  case 17: {
                    // Test empty 'function' constructed from null pointer to
                    // member data.
                    int IntWrapper::*pmd = 0;
                    const bsl::function<int(IntWrapper)> f(pmd);
                    constructed = 17;
                    f(17);
                  } break;
                } // end switch

                ASSERTV(numArgs, 0 && "Exception should have been thrown");
            }
            catch (const bsl::bad_function_call& ex) {
                ASSERTV(numArgs, ex.what(), 0 != ex.what());
            }
            catch (...) {
                ASSERTV(numArgs, 0 && "Incorrect exception caught");
            }
            ASSERTV(numArgs, numArgs == constructed);
        } // end for
#endif //  BDE_BUILD_TARGET_EXC

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_08
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON TO NULLPTR
        //
        // Concerns:
        //: 1 If 'function' f is empty, then 'f == nullptr' is true,
        //:   'nullptr == f' is true, 'f != nullptr' is false and
        //:   'nullptr != f' is false.
        //:
        //: 2 If 'function' f is not empty, then 'f == nullptr' is false,
        //:   'nullptr == f' is false, 'f != nullptr' is true and
        //:   'nullptr != f' is true.
        //
        // Plan:
        //: 1 Construct an empty 'function' object 'e' and a non-empty
        //:   function object 'f'.
        //:
        //: 2 For concern 1, verify that  'e == nullptr' is true,
        //:   'nullptr == e' is true, 'e != nullptr' is false and
        //:   'nullptr != e' is false.
        //:
        //: 3 For concern 2, verify that 'f == nullptr' is false,
        //:   'nullptr == f' is false, 'f != nullptr' is true and
        //:   'nullptr != f' is true.
        //
        // Testing:
        //  bool operator==(const function<FUNC>& f, nullptr_t) noexcept;
        //  bool operator==(nullptr_t, const function<FUNC>& f) noexcept;
        //  bool operator!=(const function<FUNC>& f, nullptr_t) noexcept;
        //  bool operator!=(nullptr_t, const function<FUNC>& f) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON TO NULLPTR"
                            "\n=============================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_08
        Obj e;
        Obj f(&simpleFunc);

        ASSERT(  e == 0 );
        ASSERT(  0 == e );
        ASSERT(!(e != 0));
        ASSERT(!(0 != e));

        ASSERT(!(f == 0));
        ASSERT(!(0 == f));
        ASSERT(  f != 0 );
        ASSERT(  0 != f );

        // Just for grins, let's make sure that everything becomes reversed if
        // we swap 'e' and 'f'.
        e.swap(f);

        ASSERT(!(e == 0));
        ASSERT(!(0 == e));
        ASSERT(  e != 0 );
        ASSERT(  0 != e );

        ASSERT(  f == 0 );
        ASSERT(  0 == f );
        ASSERT(!(f != 0));
        ASSERT(!(0 != f));

        if (verbose) printf("confirm null pointer comparison is 'noexcept'\n");
        {
            Obj a;

            ASSERT_NOEXCEPT(true, a == 0);
            ASSERT_NOEXCEPT(true, 0 == a);

            ASSERT_NOEXCEPT(true, a != 0);
            ASSERT_NOEXCEPT(true, 0 != a);
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_08
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ASSIGNMENT FROM FUNCTOR
        //
        // Concerns:
        //: 1 Assigning a 'function' object the value of a functor,
        //:   pointer-to-function, or pointer-to-member-function results in
        //:   the lhs having the same value as if it were constructed with
        //:   that functor, pointer-to-functor or pointer-to-member.
        //:
        //: 2 The functor previously wrapped by the lhs is destroyed.
        //:
        //: 3 If the rhs is an rvalue, the target of the 'function' is move-
        //:   constructed (using the extended move constructor) from the rhs
        //:   with no intervening copies.
        //:
        //: 4 After the assignment, the allocator of the lhs is unchanged.
        //:
        //: 5 If assignment is from a functor that takes an allocator, the
        //:   copy of that functor in the lhs uses the same allocator.
        //:
        //: 6 The change in memory allocation is the same as if the lhs were
        //:   destroyed then re-constructed with its original allocator and
        //:   with the specified functor.
        //:
        //: 7 The above concerns apply to the entire range of functor types
        //:   for the lhs and functor types for the rhs,
        //:   including for functors in nothrow wrappers.
        //:
        //: 8 If an exception is thrown, both lhs and rhs are unchanged.
        //:
        //: 9 In C++11, if assignment is from a functor that is not
        //:   Lvalue-Callable with the arguments and return type of the
        //:   signature of the lhs, the assignment operator does not
        //:   participate in overload resolution.
        //
        // Plan:
        //: 1 For concern 1, assign from functor to a 'function' object and
        //:   verify that the 'target_type' and 'target' of the 'function'
        //:   matches the functor.
        //:
        //: 2 For concern 2, use an original functor type that tracks number
        //:   instances in existence.  Verify that the assignment from a
        //:   different functor type reducers the number of such functors in
        //:   existence.
        //:
        //: 3 For concern 3, verify, that the 'target_type' and 'target<FUNC>'
        //:   of the 'lhs' return the expected values. For each 'FUNC' type
        //:   tested, also test assignment from 'bslmf::MovableRef<FUNC>' and
        //:   verify (when possible) that 'target<FUNC>' is moved from the
        //:   reference argument with no intervening copies, using the
        //:   extended move constructor with the original lhs allocator.  The
        //:   last is facilitated by using functor types based on
        //:   'TrackableValue', which allows tracking moves through multiple
        //:   levels of function call.
        //:
        //: 4 For concern 4, check the allocator of the lhs after assignment
        //:   and verify that it is equivalent to the allocator before the
        //:   assignment.
        //:
        //: 5 For concern 5, verify that the lhs after the assignment uses
        //:   allocator propagation.
        //:
        //: 6 For concern 6, measure the memory use for constructing the lhs
        //:   and the memory use for constructing a 'function' from the rhs.
        //:   After assigning from the rhs to the lhs, verify that the memory
        //:   change is the difference between these memory values.
        //:
        //: 7 For concern 7, construct an array of 'function' objects created
        //:   with different functor types.  Package the steps above into a
        //:   template function 'testAssignFromFunctor', which is
        //:   parameterized by functor type.  The test function will make a
        //:   copy of the input 'function' using the a test allocator and use
        //:   that as the lhs for the steps above.  Instantiate
        //:   'testAssignFromFunctor' with each of our test functor types.
        //:   Call each instantiation with each functor in the data array.
        //:
        //: 8 For concern 8, test assignments in within the exception-test
        //:   framework and verify that, on exception, both operands retain
        //:   their original values.
        //:
        //: 9 For concern 9, in C++11, test assignments from a wide variety of
        //:   functors, and verify that the assignment operator only
        //:   participates in overload resolution for functors that are
        //:   Lvalue-Callable with the return type and arguments of the
        //:   signature of the lhs.
        //
        // Testing:
        //  function& operator=(FUNC&& rhs);
        //  function& operator=(bsl::reference_wrapper<FUNC> rhs);
        // --------------------------------------------------------------------

        static const char* title[] = {
#if defined(BSLSTL_FUNCTION_TEST_ENABLE_ALL) || \
    !defined(BSLSTL_FUNCTION_TEST_PART_06_OR_07)
            "",
            ""
#elif defined(BSLSTL_FUNCTION_TEST_PART_06)
            ": PART 1",
            "========"
#elif defined(BSLSTL_FUNCTION_TEST_PART_07)
            ": PART 2",
            "========"
#endif
        };

        if (verbose)printf("\nASSIGNMENT FROM FUNCTOR%s"
                           "\n=======================%s\n",
                           title[0], title[1]);

#ifdef BSLSTL_FUNCTION_TEST_PART_06_OR_07

#ifdef BSLSTL_FUNCTION_TEST_ENABLE_ALL
#define BSLSTL_FUNCTION_TEST_CASE11_PART_1
#define BSLSTL_FUNCTION_TEST_CASE11_PART_2

#elif defined(BSLSTL_FUNCTION_TEST_PART_06)
#define BSLSTL_FUNCTION_TEST_CASE11_PART_1

#elif defined(BSLSTL_FUNCTION_TEST_PART_07)
#define BSLSTL_FUNCTION_TEST_CASE11_PART_2
#endif

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for lhs dimension of test

            int         d_line;           // Line number
            Obj         d_function;       // function object to assign
            const char *d_funcName;       // function object name
            bool        d_ntWrapped;      // Obj is NTWRAP()'d
        };

#define TEST_ITEM_PART1(F, V)                                                 \
        { L_, Obj(NTWRAP(F(V))), "NTWRAP(" #F "(" #V "))", true }
// In the first executable we (only) test the 'NOTWRAP'ped (no throw) left hand
// side arguments.

#define TEST_ITEM_PART2(F, V) { L_, Obj(F(V)), #F "(" #V ")", false }
// In the second executable we test the plain left hand side arguments (and the
// rest of the test code after this section).

#ifdef BSLSTL_FUNCTION_TEST_ENABLE_ALL
#define TEST_ITEM(F, V)                                                       \
    TEST_ITEM_PART1(F, V),                                                    \
    TEST_ITEM_PART2(F, V)

#elif defined(BSLSTL_FUNCTION_TEST_CASE11_PART_1)
#define TEST_ITEM(F, V)                                                       \
    TEST_ITEM_PART1(F, V)

#elif defined(BSLSTL_FUNCTION_TEST_CASE11_PART_2)
#define TEST_ITEM(F, V)                                                       \
    TEST_ITEM_PART2(F, V)
#endif

        const TestData DATA[] = {
            TEST_ITEM(SimpleFuncPtr_t                , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t                , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t             , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t             , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor                   , 0                 ),
            TEST_ITEM(SmallFunctor                   , 0x2000            ),
            TEST_ITEM(MediumFunctor                  , 0x4000            ),
            TEST_ITEM(LargeFunctor                   , 0x6000            ),
            TEST_ITEM(NTSmallFunctor                 , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor           , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc          , 0x2000            ),
            TEST_ITEM(BMSmallFunctorWithAlloc        , 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc        , 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc          , 0x1000            ),
            TEST_ITEM(InnerFunction                  , &simpleFunc       )
        };

        const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);
#undef TEST_ITEM
#undef TEST_ITEM_PART2
#undef TEST_ITEM_PART1

        for (int i = 0; i < NUM_DATA; ++i) {
            // const int line         = DATA[i].d_line;
            const Obj&   lhs          = DATA[i].d_function;
            const char  *funcName     = DATA[i].d_funcName;
            bool         lhsNtWrapped = DATA[i].d_ntWrapped;

#define TEST(f) do {                                                          \
            if (veryVerbose) printf("Assign %s = %s%s\n", funcName, #f,       \
                                    lhsNtWrapped ? " (nothrow)" : "");        \
            testAssignFromFunctor(lhs, f, funcName, #f, lhsNtWrapped);        \
            if (veryVerbose) printf("Assign %s = NTWRAP(%s) (nothrow)\n",     \
                                    funcName, #f);                            \
            testAssignFromFunctor(lhs, NTWRAP(f), funcName,                   \
                                  "NTWRAP(" #f ")", true);                    \
            if (veryVerbose) printf("Assign %s = ref(%s) (nothrow)\n",        \
                                    funcName, #f);                            \
            testAssignFromFunctorRefWrap(lhs, f, funcName, "ref(" #f ")");    \
        } while (false)

            TEST(nullFuncPtr                                  );
            TEST(nullMemFuncPtr                               );
            TEST(&simpleFunc                                  );
            TEST(&IntWrapper::add1                            );
            TEST(EmptyFunctor()                               );
            TEST(SmallFunctor(0x2000)                         );
            TEST(MediumFunctor(0x4000)                        );
            TEST(LargeFunctor(0x6000)                         );
            TEST(NTSmallFunctor(0x3000)                       );
            TEST(ThrowingSmallFunctor(0x7000)                 );
            TEST(ThrowingEmptyFunctor()                       );
            TEST(SmallFunctorWithAlloc(0x2000)                );
            TEST(BMSmallFunctorWithAlloc(0x2000)              );
            TEST(NTSmallFunctorWithAlloc(0x2000)              );
            TEST(LargeFunctorWithAlloc(0x2000)                );
            TEST(InnerFunction(&simpleFunc2)                  );

        } // end for (each array item)
#undef TEST

#ifdef BSLSTL_FUNCTION_TEST_CASE11_PART_2

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

#define TEST(p, f, c) testIsAssignable<p, f>(__LINE__, #f, c)

        typedef IntWrapper                        IW;
        typedef SmartPtr<IntWrapper>              SIW;
        typedef SmartPtr<const IntWrapper>        ScIW;
        typedef RWrap<IntWrapper>                 RIW;
        typedef RWrap<const IntWrapper>           RcIW;

        typedef IntWrapperDerived                 IWD;
        typedef SmartPtr<IntWrapperDerived>       SIWD;
        typedef SmartPtr<const IntWrapperDerived> ScIWD;
        typedef RWrap<IntWrapperDerived>          RIWD;
        typedef RWrap<const IntWrapperDerived>    RcIWD;

        //                                                 Constructible?
        //  Proto Functor Type                                           \.
        //  ===== ==================================================== =======

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // const- and/or reference-qualified functions, pointer to functions,
        // and pointers to member functions that vary in arity and return-type
        // compatibility.
        TEST(PROTO, int     ()                                        , false);
        TEST(PROTO, int     (IW)                                      , false);
        TEST(PROTO, int     (IW, int)                                 , true );
        TEST(PROTO, int     (IW, int, int)                            , false);
        TEST(PROTO, void    ()                                        , false);
        TEST(PROTO, void    (IW)                                      , false);
        TEST(PROTO, void    (IW, int)                                 , false);
        TEST(PROTO, void    (IW, int, int)                            , false);
        TEST(PROTO, int     ()                                        , false);
        TEST(PROTO, int     (IWD)                                     , false);
        TEST(PROTO, int     (IWD, int)                                , false);
        TEST(PROTO, int     (IWD, int, int)                           , false);
        TEST(PROTO, int  (*)()                                        , false);
        TEST(PROTO, int  (*)(IW)                                      , false);
        TEST(PROTO, int  (*)(IW, int)                                 , true );
        TEST(PROTO, int  (*)(IW, int, int)                            , false);
        TEST(PROTO, void (*)()                                        , false);
        TEST(PROTO, void (*)(IW)                                      , false);
        TEST(PROTO, void (*)(IW, int)                                 , false);
        TEST(PROTO, void (*)(IW, int, int)                            , false);
        TEST(PROTO, int  (IW::*)()                                    , false);
        TEST(PROTO, int  (IW::*)(int)                                 , false);
        TEST(PROTO, int  (IW::*)(int, int)                            , false);
        TEST(PROTO, void (IW::*)()                                    , false);
        TEST(PROTO, void (IW::*)(int)                                 , false);
        TEST(PROTO, void (IW::*)(int, int)                            , false);
        TEST(PROTO, int  (IW::*)(int) &                               , false);
        TEST(PROTO, void (IW::*)(int) &                               , false);
        TEST(PROTO, int  (IW::*)(int) const                           , true );
        TEST(PROTO, void (IW::*)(int) const                           , false);
        TEST(PROTO, int  (IW::*)(int) const&                          , true );
        TEST(PROTO, void (IW::*)(int) const&                          , false);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(PROTO, int  (IW::*)(int) &&                              , false);
        TEST(PROTO, void (IW::*)(int) &&                              , false);
        TEST(PROTO, int  (IW::*)(int) const&&                         , false);
        TEST(PROTO, void (IW::*)(int) const&&                         , false);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // pointers to member objects.
        TEST(PROTO, int IW::*                                         , false);

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // optionally const- and/or reference-qualified function objects,
        // having function-call operators that vary in arity and return type
        // compatibility.
        TEST(PROTO,       HypotheticalFunctor<int ()>                 , false);
        TEST(PROTO,       HypotheticalFunctor<int (IW)>               , false);
        TEST(PROTO,       HypotheticalFunctor<int (IW, int)>          , true );
        TEST(PROTO,       HypotheticalFunctor<int (IW, int, int)>     , false);
        TEST(PROTO,       HypotheticalFunctor<void()>                 , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW)>               , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW, int)>          , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW, int, int)>     , false);
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>          , true );
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>&         , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(PROTO,       HypotheticalFunctor<int (IW, int)>&&        , true );
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>&&        , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        //                                           Constructible?
        //  Prototype                    Functor Type              \.
        //  ============================ ======================= =======

        // 'bsl::function' objects having prototypes with arguments and return
        // values implicitly convertible to and/or from those of the function
        // object from which they're constructed.
        TEST(      int   (int          ), int   (int  )         , true );
        TEST(      int   (int          ), int   (float)         , true );
        TEST(      int   (int          ), float (int  )         , true );
        TEST(      int   (int          ), float (float)         , true );
        TEST(      int   (float        ), int   (int  )         , true );
        TEST(      int   (float        ), int   (float)         , true );
        TEST(      int   (float        ), float (int  )         , true );
        TEST(      int   (float        ), float (float)         , true );
        TEST(      float (int          ), int   (int  )         , true );
        TEST(      float (int          ), int   (float)         , true );
        TEST(      float (int          ), float (int  )         , true );
        TEST(      float (int          ), float (float)         , true );
        TEST(      float (float        ), int   (int  )         , true );
        TEST(      float (float        ), int   (float)         , true );
        TEST(      float (float        ), float (int  )         , true );
        TEST(      float (float        ), float (float)         , true );

        // 'bsl::function' objects having prototypes with const- and/or
        // reference-qualified arguments and results, constructed with pointers
        // to member objects.
        TEST(      void  (        IW   ), int IW::*             , true );
        TEST(      void  (        IW&  ), int IW::*             , true );
        TEST(      void  (const   IW   ), int IW::*             , true );
        TEST(      void  (const   IW&  ), int IW::*             , true );
        TEST(      int   (        IW   ), int IW::*             , true );
        TEST(      int   (        IW&  ), int IW::*             , true );
        TEST(      int   (const   IW   ), int IW::*             , true );
        TEST(      int   (const   IW&  ), int IW::*             , true );
        TEST(      int&  (        IW   ), int IW::*             , false);
        TEST(      int&  (        IW&  ), int IW::*             , true );
        TEST(      int&  (const   IW   ), int IW::*             , false);
        TEST(      int&  (const   IW&  ), int IW::*             , false);
        TEST(const int&  (        IW   ), int IW::*             , true );
        TEST(const int&  (        IW&  ), int IW::*             , true );
        TEST(const int&  (const   IW   ), int IW::*             , true );
        TEST(const int&  (const   IW&  ), int IW::*             , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(      void  (        IW&& ), int IW::*             , true );
        TEST(      void  (const   IW&& ), int IW::*             , true );
        TEST(      int   (        IW&& ), int IW::*             , true );
        TEST(      int   (const   IW&& ), int IW::*             , true );
        TEST(      int&  (        IW&& ), int IW::*             , false);
        TEST(      int&  (const   IW&& ), int IW::*             , false);
        TEST(const int&  (        IW&& ), int IW::*             , true );
        TEST(const int&  (const   IW&& ), int IW::*             , true );
        TEST(      int&& (        IW   ), int IW::*             , true );
        TEST(      int&& (        IW&  ), int IW::*             , true );
        TEST(      int&& (        IW&& ), int IW::*             , true );
        TEST(      int&& (const   IW   ), int IW::*             , true );
        TEST(      int&& (const   IW&  ), int IW::*             , false);
        TEST(      int&& (const   IW&& ), int IW::*             , false);
        TEST(const int&& (        IW   ), int IW::*             , true );
        TEST(const int&& (        IW&  ), int IW::*             , true );
        TEST(const int&& (        IW&& ), int IW::*             , true );
        TEST(const int&& (const   IW   ), int IW::*             , true );
        TEST(const int&& (const   IW&  ), int IW::*             , true );
        TEST(const int&& (const   IW&& ), int IW::*             , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with smart pointer to, and
        // reference-wrapped class types, constructed with pointers to member
        // objects.
        TEST(       int  (        IW   ), int  IW::*            , true );
        TEST(       int  (        IW&  ), int  IW::*            , true );
        TEST(       int  (const   IW   ), int  IW::*            , true );
        TEST(       int  (const   IW&  ), int  IW::*            , true );
        TEST(       int  (       RIW   ), int  IW::*            , true );
        TEST(       int  (       RIW&  ), int  IW::*            , true );
        TEST(       int  (const  RIW   ), int  IW::*            , true );
        TEST(       int  (const  RIW&  ), int  IW::*            , true );
        TEST(       int  (      RcIW   ), int  IW::*            , true );
        TEST(       int  (      RcIW&  ), int  IW::*            , true );
        TEST(       int  (const RcIW   ), int  IW::*            , true );
        TEST(       int  (const RcIW&  ), int  IW::*            , true );
        TEST(       int  (       SIW   ), int  IW::*            , true );
        TEST(       int  (       SIW&  ), int  IW::*            , true );
        TEST(       int  (const  SIW   ), int  IW::*            , true );
        TEST(       int  (const  SIW&  ), int  IW::*            , true );
        TEST(       int  (      ScIW   ), int  IW::*            , true );
        TEST(       int  (      ScIW&  ), int  IW::*            , true );
        TEST(       int  (const ScIW   ), int  IW::*            , true );
        TEST(       int  (const ScIW&  ), int  IW::*            , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IW&& ), int  IW::*            , true );
        TEST(       int  (const   IW&& ), int  IW::*            , true );
        TEST(       int  (       RIW&& ), int  IW::*            , true );
        TEST(       int  (const  RIW&& ), int  IW::*            , true );
        TEST(       int  (      RcIW&& ), int  IW::*            , true );
        TEST(       int  (const RcIW&& ), int  IW::*            , true );
        TEST(       int  (       SIW&& ), int  IW::*            , true );
        TEST(       int  (const  SIW&& ), int  IW::*            , true );
        TEST(       int  (      ScIW&& ), int  IW::*            , true );
        TEST(       int  (const ScIW&& ), int  IW::*            , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with smart pointer to, and
        // reference-wrapped class types, constructed with pointers to member
        // objects of a base type.
        TEST(       int  (        IWD  ), int  IW::*            , true );
        TEST(       int  (        IWD& ), int  IW::*            , true );
        TEST(       int  (const   IWD  ), int  IW::*            , true );
        TEST(       int  (const   IWD& ), int  IW::*            , true );
        TEST(       int  (       RIWD  ), int  IW::*            , true );
        TEST(       int  (       RIWD& ), int  IW::*            , true );
        TEST(       int  (const  RIWD  ), int  IW::*            , true );
        TEST(       int  (const  RIWD& ), int  IW::*            , true );
        TEST(       int  (      RcIWD  ), int  IW::*            , true );
        TEST(       int  (      RcIWD& ), int  IW::*            , true );
        TEST(       int  (const RcIWD  ), int  IW::*            , true );
        TEST(       int  (const RcIWD& ), int  IW::*            , true );
        TEST(       int  (       SIWD  ), int  IW::*            , true );
        TEST(       int  (       SIWD& ), int  IW::*            , true );
        TEST(       int  (const  SIWD  ), int  IW::*            , true );
        TEST(       int  (const  SIWD& ), int  IW::*            , true );
        TEST(       int  (      ScIWD  ), int  IW::*            , true );
        TEST(       int  (      ScIWD& ), int  IW::*            , true );
        TEST(       int  (const ScIWD  ), int  IW::*            , true );
        TEST(       int  (const ScIWD& ), int  IW::*            , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IWD&&), int  IW::*            , true );
        TEST(       int  (const   IWD&&), int  IW::*            , true );
        TEST(       int  (       RIWD&&), int  IW::*            , true );
        TEST(       int  (const  RIWD&&), int  IW::*            , true );
        TEST(       int  (      RcIWD&&), int  IW::*            , true );
        TEST(       int  (const RcIWD&&), int  IW::*            , true );
        TEST(       int  (       SIWD&&), int  IW::*            , true );
        TEST(       int  (const  SIWD&&), int  IW::*            , true );
        TEST(       int  (      ScIWD&&), int  IW::*            , true );
        TEST(       int  (const ScIWD&&), int  IW::*            , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects with prototypes having smart pointer to, and
        // reference-wrapped, class types, constructed with pointers to member
        // functions.
        TEST(       int  (        IW   ), int (IW::*)()         , true );
        TEST(       int  (       RIW   ), int (IW::*)()         , true );
        TEST(       int  (      RcIW   ), int (IW::*)()         , false);
        TEST(       int  (       SIW   ), int (IW::*)()         , true );
        TEST(       int  (      ScIW   ), int (IW::*)()         , false);
        TEST(       int  (        IW   ), int (IW::*)() &       , false);
        TEST(       int  (       RIW   ), int (IW::*)() &       , true );
        TEST(       int  (      RcIW   ), int (IW::*)() &       , false);
        TEST(       int  (       SIW   ), int (IW::*)() &       , true );
        TEST(       int  (      ScIW   ), int (IW::*)() &       , false);
        TEST(       int  (        IW   ), int (IW::*)() const   , true );
        TEST(       int  (       RIW   ), int (IW::*)() const   , true );
        TEST(       int  (      RcIW   ), int (IW::*)() const   , true );
        TEST(       int  (       SIW   ), int (IW::*)() const   , true );
        TEST(       int  (      ScIW   ), int (IW::*)() const   , true );
        TEST(       int  (        IW   ), int (IW::*)() const&  , MSVC ||
                                                                  CPP_20);
        TEST(       int  (       RIW   ), int (IW::*)() const&  , true );
        TEST(       int  (      RcIW   ), int (IW::*)() const&  , true );
        TEST(       int  (       SIW   ), int (IW::*)() const&  , true );
        TEST(       int  (      ScIW   ), int (IW::*)() const&  , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IW   ), int (IW::*)() &&      , true );
        TEST(       int  (       RIW   ), int (IW::*)() &&      , false);
        TEST(       int  (      RcIW   ), int (IW::*)() &&      , false);
        TEST(       int  (       SIW   ), int (IW::*)() &&      , false);
        TEST(       int  (      ScIW   ), int (IW::*)() &&      , false);
        TEST(       int  (        IW   ), int (IW::*)() const&& , true );
        TEST(       int  (       RIW   ), int (IW::*)() const&& , false);
        TEST(       int  (      RcIW   ), int (IW::*)() const&& , false);
        TEST(       int  (       SIW   ), int (IW::*)() const&& , false);
        TEST(       int  (      ScIW   ), int (IW::*)() const&& , false);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects with prototypes having smart pointer to, and
        // reference-wrapped, class types, constructed with pointers to member
        // functions of base types.
        TEST(       int  (        IWD  ), int (IW::*)()         , true     );
        TEST(       int  (       RIWD  ), int (IW::*)()         , true     );
        TEST(       int  (      RcIWD  ), int (IW::*)()         , false    );
        TEST(       int  (       SIWD  ), int (IW::*)()         , true     );
        TEST(       int  (      ScIWD  ), int (IW::*)()         , false    );
        TEST(       int  (        IWD  ), int (IW::*)() &       , MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() &       , true     );
        TEST(       int  (      RcIWD  ), int (IW::*)() &       , false    );
        TEST(       int  (       SIWD  ), int (IW::*)() &       , true     );
        TEST(       int  (      ScIWD  ), int (IW::*)() &       , false    );
        TEST(       int  (        IWD  ), int (IW::*)() const   , true     );
        TEST(       int  (       RIWD  ), int (IW::*)() const   , true     );
        TEST(       int  (      RcIWD  ), int (IW::*)() const   , true     );
        TEST(       int  (       SIWD  ), int (IW::*)() const   , true     );
        TEST(       int  (      ScIWD  ), int (IW::*)() const   , true     );
        TEST(       int  (        IWD  ), int (IW::*)() const&  , MSVC ||
                                                                  CPP_20   );
        TEST(       int  (       RIWD  ), int (IW::*)() const&  , true     );
        TEST(       int  (      RcIWD  ), int (IW::*)() const&  , true     );
        TEST(       int  (       SIWD  ), int (IW::*)() const&  , true     );
        TEST(       int  (      ScIWD  ), int (IW::*)() const&  , true     );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IWD  ), int (IW::*)() &&      ,!MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() &&      , false    );
        TEST(       int  (      RcIWD  ), int (IW::*)() &&      , false    );
        TEST(       int  (       SIWD  ), int (IW::*)() &&      , false    );
        TEST(       int  (      ScIWD  ), int (IW::*)() &&      , false    );
        TEST(       int  (        IWD  ), int (IW::*)() const&& ,!MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() const&& , false    );
        TEST(       int  (      RcIWD  ), int (IW::*)() const&& , false    );
        TEST(       int  (       SIWD  ), int (IW::*)() const&& , false    );
        TEST(       int  (      ScIWD  ), int (IW::*)() const&& , false    );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with const- and/or
        // reference-qualified 'IntWrapper' return values, constructed with
        // lvalue-callable objects that return compatible types.
        TEST(      IW    (             ),       IW    ()        , true );
        TEST(      IW    (             ),       IW&   ()        , true );
        TEST(      IW    (             ), const IW    ()        , true );
        TEST(      IW    (             ), const IW&   ()        , true );
        TEST(      IW&   (             ),       IW    ()        , MSVC );
        TEST(      IW&   (             ),       IW&   ()        , true );
        TEST(      IW&   (             ), const IW    ()        , false);
        TEST(      IW&   (             ), const IW&   ()        , false);
        TEST(const IW    (             ),       IW    ()        , true );
        TEST(const IW    (             ),       IW&   ()        , true );
        TEST(const IW    (             ), const IW    ()        , true );
        TEST(const IW    (             ), const IW&   ()        , true );
        TEST(const IW&   (             ),       IW    ()        , true );
        TEST(const IW&   (             ),       IW&   ()        , true );
        TEST(const IW&   (             ), const IW    ()        , true );
        TEST(const IW&   (             ), const IW&   ()        , true );
        TEST(      IWD   (             ),       IW    ()        , false);
        TEST(      IW    (             ),       IWD   ()        , true );
        TEST(      IW    (             ),       IWD&  ()        , true );
        TEST(      IW    (             ), const IWD   ()        , true );
        TEST(      IW    (             ), const IWD&  ()        , true );
        TEST(      IW&   (             ),       IWD   ()        , MSVC );
        TEST(      IW&   (             ),       IWD&  ()        , true );
        TEST(      IW&   (             ), const IWD   ()        , false);
        TEST(      IW&   (             ), const IWD&  ()        , false);
        TEST(const IW    (             ),       IWD   ()        , true );
        TEST(const IW    (             ),       IWD&  ()        , true );
        TEST(const IW    (             ), const IWD   ()        , true );
        TEST(const IW    (             ), const IWD&  ()        , true );
        TEST(const IW&   (             ),       IWD   ()        , true );
        TEST(const IW&   (             ),       IWD&  ()        , true );
        TEST(const IW&   (             ), const IWD   ()        , true );
        TEST(const IW&   (             ), const IWD&  ()        , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(      IW    (             ),       IW&&  ()        , true );
        TEST(      IW    (             ), const IW&&  ()        , true );
        TEST(      IW&   (             ),       IW&&  ()        , MSVC );
        TEST(      IW&   (             ), const IW&&  ()        , false);
        TEST(      IW&   (             ),       IW&&  ()        , MSVC );
        TEST(      IW&   (             ), const IW&&  ()        , false);
        TEST(const IW    (             ),       IW&&  ()        , true );
        TEST(const IW    (             ), const IW&&  ()        , true );
        TEST(const IW&   (             ),       IW&&  ()        , true );
        TEST(const IW&   (             ), const IW&&  ()        , true );
        TEST(      IW&&  (             ),       IW    ()        , true );
        TEST(      IW&&  (             ),       IW&   ()        , true );
        TEST(      IW&&  (             ), const IW    ()        , false);
        TEST(      IW&&  (             ), const IW&   ()        , false);
        TEST(const IW&&  (             ),       IW    ()        , true );
        TEST(const IW&&  (             ),       IW&   ()        , true );
        TEST(const IW&&  (             ), const IW    ()        , true );
        TEST(const IW&&  (             ), const IW&   ()        , true );
        TEST(      IW    (             ),       IWD&& ()        , true );
        TEST(      IW    (             ), const IWD&& ()        , true );
        TEST(      IW&   (             ),       IWD&& ()        , MSVC );
        TEST(      IW&   (             ), const IWD&& ()        , false);
        TEST(const IW    (             ),       IWD&& ()        , true );
        TEST(const IW    (             ), const IWD&& ()        , true );
        TEST(const IW&   (             ),       IWD&& ()        , true );
        TEST(const IW&   (             ), const IWD&& ()        , true );
        TEST(      IW&&  (             ),       IWD   ()        , true );
        TEST(      IW&&  (             ),       IWD&  ()        , true );
        TEST(      IW&&  (             ), const IWD   ()        , false);
        TEST(      IW&&  (             ), const IWD&  ()        , false);
        TEST(const IW&&  (             ),       IWD   ()        , true );
        TEST(const IW&&  (             ),       IWD&  ()        , true );
        TEST(const IW&&  (             ), const IWD   ()        , true );
        TEST(const IW&&  (             ), const IWD&  ()        , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Verify that 'bsl::function<PROTO>' is assignable from types that
        // are implicitly convertible to 'bsl::function<PROTO>'.  Note this
        // should should be the case for all 'PROTO' types that are function
        // types without const, volatile, or reference qualifiers.  Further
        // note that this "test vector" verifies that {DRQS 138769521} has been
        // addressed.
        TEST(PROTO, ConvertibleToObj, true);

#undef TEST
#endif  // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

#endif  // BSLSTL_FUNCTION_TEST_CASE11_PART_2

// Clean up local macros
#ifdef BSLSTL_FUNCTION_TEST_CASE11_PART_1
#undef BSLSTL_FUNCTION_TEST_CASE11_PART_1
#else
#undef BSLSTL_FUNCTION_TEST_CASE11_PART_2
#endif

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_06_OR_07
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ASSIGNMENT FROM 'nullptr'
        //
        // Concerns:
        //: 1 Assigning a 'function' object the value 'nullptr' results in an
        //:   empty 'function'.
        //:
        //: 2 The allocator of the assigned 'function' does not change.
        //:
        //: 3 No memory is allocated by the assignment, though memory might be
        //:   freed. The number of blocks used after the assignment should
        //:   be zero.
        //:
        //: 4 No potentially-throwing operations are invoked.
        //:
        //: 5 The above concerns apply to 'function' objects constructed with
        //:   every category of wrapped functor, including functors in a
        //:   nothrow wrapper.
        //
        // Plan:
        //: 1 For concern 1, create a 'function', 'f', and assign 'f =
        //:   nullptr'. Verify that 'f.empty()' is true after the assignment.
        //:
        //: 2 For concern 2, verify that the allocator is the same before and
        //:   after the assignment.
        //:
        //: 3 For concern 3, test the blocks used from the allocator
        //:   before and after the assignment.  Verify that the total used
        //:   does not increase and that the number of blocks used after the
        //:   assignment matches the number of blocks used for an empty
        //:   function using the same allocator.
        //:
        //: 4 For concern 4, construct the initial 'function' using a functor
        //:   that throws on move or copy and set the allocator to throw on
        //:   the first allocation request.  Verify that the assignment works
        //:   without throwing.
        //:
        //: 5 For concern 5, encapsulate the above steps into a function
        //:   template, 'testAssignNullptr', taking a 'function' argument.
        //:   Create an array of 'function' objects, each of which is
        //:   constructed with a functor from a different category (small,
        //:   large, throwing, non-throwing, etc.).  Invoke
        //:   'testAssignNullptr' for each element of the array of 'function'.
        //
        // Testing:
        //   function& operator=(nullptr_t) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSIGNMENT FROM 'nullptr'"
                            "\n=========================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_05
        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int         d_line;           // Line number
            Obj         d_function;       // function object to swap
            const char *d_funcName;       // function object name
        };

#define TEST_ITEM(F, V)                 \
        { L_, Obj(F(V)), #F "(" #V ")" }

#define NTTST_ITM(F, V)                                    \
        { L_, Obj(NTWRAP(F(V))), "NTWRAP(" #F "(" #V "))" }

        const TestData DATA[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x2000            ),
            TEST_ITEM(MediumFunctor          , 0x4000            ),
            TEST_ITEM(LargeFunctor           , 0x6000            ),
            TEST_ITEM(NTSmallFunctor         , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(BMSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x1000            ),
            TEST_ITEM(InnerFunction          , &simpleFunc       ),

            NTTST_ITM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            NTTST_ITM(EmptyFunctor           , 0                 ),
            NTTST_ITM(SmallFunctor           , 0x2000            ),
            NTTST_ITM(LargeFunctor           , 0x6000            ),
            NTTST_ITM(NTSmallFunctor         , 0x3000            ),
            NTTST_ITM(ThrowingSmallFunctor   , 0x7000            ),
            NTTST_ITM(SmallFunctorWithAlloc  , 0x2000            ),
        };

#undef TEST_ITEM
#undef NTTST_ITM

        const int NUM_DATA = sizeof(DATA) / sizeof(TestData);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   line     = DATA[i].d_line;
            const Obj&  func     = DATA[i].d_function;
            const char *funcName = DATA[i].d_funcName;

            if (veryVerbose) printf("Assign %s = nullptr\n", funcName);

            testAssignNullptr(func, line);

        } // end for (each function in DATA array)

        if (verbose) printf("confirm assigning null pointers is 'noexcept'\n");
        {
            Obj a;
            ASSERT_NOEXCEPT(true, a = 0);
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_05
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY AND MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 The rhs of an assignment wraps a functor equal to the lhs before
        //:   the assignment for both copy assignment and move assignment.
        //:
        //: 2 The allocator of the lhs is not changed by the assignment and
        //:   its wrapped functor's allocator (if any) continues to reflect
        //:   proper allocator propagation.
        //:
        //: 3 The rhs of a copy assignment is not changed.
        //:
        //: 4 For move assignment, if the lhs and rhs allocators compare
        //:   equal, no memory is allocated and the rhs is swapped with the
        //:   lhs.
        //:
        //: 5 If the lhs and rhs allocators do not compare equal, the memory
        //:   allocation behavior of move assignment is identical to that of
        //:   copy assignment and the rhs is either unchanged or is changed to
        //:   the moved-from state.
        //:
        //: 6 For move assignment, if the lhs and rhs allocators compare
        //:   equal, then no exception is thrown, even if the wrapped functor
        //:   has a throwing move constructor.
        //:
        //: 7 Memory formerly belonging to the lhs is released by both copy
        //:   and move assignment.
        //:
        //: 8 The above concerns apply for each of the different types of
        //:   wrapped functors.
        //:
        //: 9 If an exception is thrown during an assignment, both operands
        //:   of the assignment are unchanged.
        //:
        //: 10 The above concerns apply if the functor used to construct
        //:   either operand of the assignment was wrapped in a nothrow
        //:   wrapper.
        //:
        //: 11 If the argument to the assignment operator is a reference to a
        //:   'bdef_Function' for the same prototype, the 'bdef_Function' is
        //:   treated as a 'bsl::function' and the copy assignment operator is
        //:   invoked, yielding a result with identical target rather than
        //:   wrapping the 'bdef_Function' itself as a target.
        //
        // Plan:
        //: 1 For concern 1, construct a pair of 'function' objects 'a' and
        //:   'b'.  Copy-construct 'a1' and 'b1' from 'a' and 'b' respectively
        //:   and assign 'a1 = b1'.  Verify that the target type and target
        //:   value for 'a1' after the assignment matches the target type and
        //:   target of 'b'.  Make another pair of copies, 'a2' and 'b2' and
        //:   repeat the test for move assignment.
        //:
        //: 2 For concern 2, verify that allocators of 'a1' and 'a2' after the
        //:   assignments in step 1 match the allocator used to construct them
        //:   and that the wrapped functors' allocators (if any) continue to
        //:   reflect proper allocator propagation.
        //:
        //: 3 For concern 3, verify that the target type and target of 'b1'
        //:   matches the target type and target of 'b'.
        //:
        //: 4 For concern 4, check the allocators of 'a2' and 'b2' before the
        //:   assignment and, if they are equal, verify that no allocations
        //:   or deallocations are performed using either that allocator or
        //:   the global allocator as a result of the move assignment.
        //:
        //: 5 For concern 5, check the allocators of 'a2' and 'b2' before the
        //:   assignment and, if they are not equal, verify that copying 'b2'
        //:   to 'a2' by move assignment results in a memory footprint
        //:   equivalent to destroying 'a1' and move-constructing 'b' using
        //:   'a1's allocator.  Verify that the function wrapped 'b1' is put
        //:   into either in a moved-from state or left unchanged.
        //:
        //: 6 For concern 6, when 'a2' and 'b2' have the same allocator, turn
        //:   on instrumentation in the allocators and the functors that would
        //:   cause them to throw exceptions.  Verify that the move assignment
        //:   succeeds even with this instrumentation active.
        //:
        //: 7 For concern 7, track the amount of
        //:   memory used to construct 'a1', 'b1', 'a2', and 'b2'.  Verify
        //:   that, after the assignment, the memory used matches the memory
        //:   that would be used by two copies of 'b1' and one or two copies
        //:   of 'b2' (depending on whether or not 'a2' and 'b2' have the same
        //:   allocator).
        //:
        //: 8 For concern 8, package the above steps into a function
        //:   template, 'testAssign', which is instantiated on two allocator
        //:   types and takes two 'function' object arguments as well as
        //:   comparison functions for each functor type.  The comparison
        //:   function are used to test if the targets compare equal and to
        //:   assert that they have proper allocator-propagation.  In
        //:   'testAssign', 'a' is copied into 'a1' and 'a2' using the first
        //:   allocator and 'b' is copied into 'b1' and 'b2' using the second
        //:   allocator argument.  Create two arrays where each array element
        //:   contains a function object and a pointer to a function that can
        //:   compare that function for equality.  Each function object is
        //:   constructed with a different callable object and the comparison
        //:   function is instantiated with the that callable type.  Loop
        //:   through the 4-way cross product of the two arrays and two sets
        //:   of allocator types and call 'testAssign' to perform the test on
        //:   each combination.
        //:
        //: 9 For concern 9, test assignments in within the exception-test
        //:   framework and verify that, on exception, both operands retain
        //:   their original values.
        //:
        //: 10 For concern 10, add functors with nothrow wrappers to the array
        //:   in step 8, above. Suppress exception tests for move assignment
        //:   when nothrow wrappers are used, since throwing an exception from
        //:   a wrapper would terminate the program.
        //:
        //: 11 For concern 11, construct a 'bdef_Function' from a functor and
        //:   assign it to a 'bsl::function' with the same prototype.  Verify
        //:   that the target of the assigned-to object is a copy of the
        //:   functor.
        //
        // Testing
        //  function& operator=(const function& rhs);
        //  function& operator=(function&& rhs);
        //  CONCERN: Assignment from 'bdef_Function' does not double-wrap
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY AND MOVE ASSIGNMENT"
                            "\n========================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_05
        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;
        typedef bool (* const AreEqualFuncPtr_t)(const Obj&, const Obj&);

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int                d_line;           // Line number
            Obj                d_function;       // function object to swap
            const char        *d_funcName;       // function object name
            AreEqualFuncPtr_t  d_areEqualFunc_p; // comparison function
            bool               d_skipExcTest;    // Skip exception test on move
        };

#define TEST_ITEM(F, V)                                                       \
        { L_, Obj(F(V)), #F "(" #V ")", &AreEqualFunctions<F>, false },       \
        { L_, Obj(NTWRAP(F(V))), "NTWRAP(" #F "(" #V "))",                    \
                &AreEqualFunctions<F>, true }

        const TestData DATA_A[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x200             ),
            TEST_ITEM(MediumFunctor          , 0x400             ),
            TEST_ITEM(LargeFunctor           , 0x600             ),
            TEST_ITEM(NTSmallFunctor         , 0x300             ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x700             ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x200             ),
            TEST_ITEM(BMSmallFunctorWithAlloc, 0x200             ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x200             ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x100             ),
            TEST_ITEM(InnerFunction          , &simpleFunc       )
        };

        const int NUM_DATA_A = sizeof(DATA_A) / sizeof(TestData);

        const TestData DATA_B[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc2       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::sub1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x30              ),
            TEST_ITEM(MediumFunctor          , 0x50              ),
            TEST_ITEM(LargeFunctor           , 0x70              ),
            TEST_ITEM(NTSmallFunctor         , 0x40              ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x60              ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x10              ),
            TEST_ITEM(BMSmallFunctorWithAlloc, 0x10              ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x10              ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x20              ),
            TEST_ITEM(InnerFunction          , &simpleFunc2      )
        };
#undef TEST_ITEM

        const int NUM_DATA_B = sizeof(DATA_B) / sizeof(TestData);

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int          lineA     = DATA_A[i].d_line;
            const Obj&         funcA     = DATA_A[i].d_function;
            const char        *funcAName = DATA_A[i].d_funcName;
            AreEqualFuncPtr_t  areEqualA = DATA_A[i].d_areEqualFunc_p;

            for (int j = 0; j < NUM_DATA_B; ++j) {
                const int          lineB       = DATA_B[j].d_line;
                const Obj&         funcB       = DATA_B[j].d_function;
                const char        *funcBName   = DATA_B[j].d_funcName;
                AreEqualFuncPtr_t  areEqualB   = DATA_B[j].d_areEqualFunc_p;
                const bool         skipExcTest = (DATA_A[i].d_skipExcTest ||
                                                  DATA_B[j].d_skipExcTest);

                if (veryVerbose) printf("Assign %s = %s\n",
                                        funcAName, funcBName);

                if (veryVeryVerbose) printf("\tFunctor types = %s, %s\n",
                                            funcAName, funcBName);
                testAssignment(funcA, funcB,
                               areEqualA, areEqualB,
                               lineA, lineB,
                               skipExcTest);

            } // End for (each item in 'DATA_B')
        } // End for (each item in 'DATA_A')

        SmallFunctor                       functor(0x30);
        BloombergLP::bdef_Function<PROTO*> original(functor);
        bsl::function<PROTO>               copyObj;

        // Test copy assign from 'bdef_Function'
        copyObj = original;
        SmallFunctor *target = copyObj.target<SmallFunctor>();
        ASSERT(target && *target == functor);

        // Test copy assign from 'bdef_Function&&'
        copyObj = 0;
        copyObj = bslmf::MovableRefUtil::move(original);
        target = copyObj.target<SmallFunctor>();
        ASSERT(target && *target == functor);
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_05
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP
        //
        // Concerns:
        //: 1 Swapping two 'function' objects has the same affect as
        //:   constructing the same objects but with the constructor arguments
        //:   to one substituted for the constructor arguments to the other.
        //:
        //: 2 Memory consumption, both from allocators and from the global
        //:   heap, is unchanged by the swap operation.
        //:
        //: 3 The above concerns apply for each of the different types of
        //:   wrapped functors.
        //:
        //: 4 The above concerns apply to 'function's constructed with
        //:   non-default allocator arguments.  Note that the allocators to
        //:   both objects must compare equal in order for them to be swapped.
        //:
        //: 5 The above concerns apply to 'function's constructed with nothrow-
        //:   wrapped targets.
        //:
        //: 6 Unless one or both of the 'function' targets was constructed with
        //:   'nothrow' wrappers, 'swap' is guaranteed not to throw.  If this
        //:   concern sounds counterintuitive, it is because the nothrow
        //:   wrapper actually "pretends" that the target won't throw, even if
        //:   it might actually throw on move. If the target is *known* to
        //:   possibly throw, then 'bsl::function' ensures that it will never
        //:   be moved, but rather referenced only through a pointer that is
        //:   moved.
        //:
        //: 7 The namespace-scope function, 'bsl::swap' invokes
        //:   'bsl::function<F>::swap' when invoked with two 'function'
        //:   objects.
        //
        // Plan:
        //: 1 For concern 1, create two different 'function' objects, 'a', 'b',
        //:   wrapping callable objects of type 'FA' and 'FB', respectively.
        //:   Construct another pair of function objects 'a2' and 'b2', using
        //:   the same arguments as 'a' and b', respectively.  Swap 'a' with
        //:   'b'.  Verify that, after the swap, 'a.target_type() ==
        //:   b2.target_type()', '*a.target<FB>() == *b2.target<FB>()',
        //:   'b.target_type() == a2.target_type()', '*b.target<FB>() ==
        //:   *a2.target<FB>()'. Also verify that the allocators of both
        //:   objects are compare equal to their original values and that the
        //:   wrapped functors' allocators (if any) continue to reflect proper
        //:   allocator propagation.  (Since the allocators of 'a' and 'b' were
        //:   the same before the swap, it is unimportant whether the
        //:   allocators are swapped or not.)
        //:
        //: 2 For concern 2, check the memory in use by the allocator and by
        //:   the global heap after constructing 'a', 'b', 'a2', and 'b2' and
        //:   verify that the amount of memory in use after the swap is the
        //:   same as before the swap.
        //:
        //: 3 For concerns 3 and 4, package the above steps into a function
        //:   template, 'testSwap', which takes two 'function' object arguments
        //:   as well as comparison functions for each functor type. The
        //:   comparison function are used to test if the targets compare equal
        //:   and to assert that they have proper allocator-propagation.
        //:   'testSwap' copies the input arguments using a local allocator
        //:   argument before swapping them.  Create two arrays where each
        //:   array element contains a function object and a pointer to a
        //:   function that can compare that function for equality.  Each
        //:   function object is constructed with a different callable object
        //:   and the comparison function is instantiated with the type of that
        //:   object.  Loop through the cross product of the two arrays and
        //:   call 'testSwap' to perform the test.
        //:
        //: 4 For concern 5, add a nothrow-wrapped version of each 'function'
        //:   in the arrays described in step 3.
        //:
        //: 5 For concern 6, test 'swap' within the exception-test framework.
        //:   Suppress exception tests when nothrow wrappers are used, since
        //:   throwing an exception from a wrapper would terminate the program.
        //:
        //: 6 For concern 7, reverse the call to member 'swap' in step 1 by
        //:   using free function 'bsl::swap'.
        //
        // Testing
        //  void swap(function& other) noexcept;
        //  void swap(function<FUNC>& a, function<FUNC>& b) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nSWAP"
                            "\n====\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_05
        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;
        typedef bool (* const AreEqualFuncPtr_t)(const Obj&, const Obj&);

        // Null functors
        int             ( *nullFuncPtr)   (const IntWrapper&, int)  = 0;
        int ( IntWrapper::*nullMemFuncPtr)(int              ) const = 0;

        struct TestData {
            // Data for one dimension of test

            int                d_line;           // Line number
            Obj                d_function;       // function object to swap
            const char        *d_funcName;       // function object name
            AreEqualFuncPtr_t  d_areEqualFunc_p; // comparison function
            bool               d_ntWrapped;      // functor is NTWRAP()'d
        };

#define TEST_ITEM(F, V)                                                 \
        { L_, Obj(F(V)), #F "(" #V ")", &AreEqualFunctions<F>, false }, \
        { L_, Obj(NTWRAP(F(V))), "NTWRAP(" #F "(" #V "))",              \
                &AreEqualFunctions<F>, true }

        const TestData DATA_A[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x2000            ),
            TEST_ITEM(MediumFunctor          , 0x4000            ),
            TEST_ITEM(LargeFunctor           , 0x6000            ),
            TEST_ITEM(NTSmallFunctor         , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x1000            ),
            TEST_ITEM(BMSmallFunctorWithAlloc, 0x1000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x1000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(InnerFunction          , &simpleFunc       )
        };

        const int NUM_DATA_A = sizeof(DATA_A) / sizeof(TestData);

        const TestData DATA_B[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc2       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::sub1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x3000            ),
            TEST_ITEM(MediumFunctor          , 0x5000            ),
            TEST_ITEM(LargeFunctor           , 0x7000            ),
            TEST_ITEM(NTSmallFunctor         , 0x4000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x6000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(BMSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x1000            ),
            TEST_ITEM(InnerFunction          , &simpleFunc2      )
        };
#undef TEST_ITEM

        const int NUM_DATA_B = sizeof(DATA_B) / sizeof(TestData);

        for (int i = 0; i < NUM_DATA_A; ++i) {
            const int          lineA     = DATA_A[i].d_line;
            const Obj&         funcA     = DATA_A[i].d_function;
            const char        *funcAName = DATA_A[i].d_funcName;
            AreEqualFuncPtr_t  areEqualA = DATA_A[i].d_areEqualFunc_p;

            for (int j = 0; j < NUM_DATA_B; ++j) {
                const int          lineB       = DATA_B[j].d_line;
                const Obj&         funcB       = DATA_B[j].d_function;
                const char        *funcBName   = DATA_B[j].d_funcName;
                AreEqualFuncPtr_t  areEqualB   = DATA_B[j].d_areEqualFunc_p;
                const bool         skipExcTest = (DATA_A[i].d_ntWrapped ||
                                                  DATA_B[j].d_ntWrapped);

                if (veryVerbose) printf("swap(%s, %s)\n",funcAName,funcBName);

                testSwap(funcA, funcB, areEqualA, areEqualB, lineA, lineB,
                         skipExcTest);

            } // end for (each item in 'DATA_B')
        } // end for (each item in 'DATA_A')

        if (verbose) printf("confirming swap operations are 'noexcept'\n");
        {
            Obj a, b;
            ASSERT_NOEXCEPT(true, a.swap(b));
            ASSERT_NOEXCEPT(true, bsl::swap(a, b));
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_05
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        //
        // Concerns:
        //: 1 Moving an empty 'function' object (with either the move
        //:   constructor or allocator-extended move constructor) yields an
        //:   empty destination 'function' object.
        //:
        //: 2 Moving a non-empty 'function' yields a non-empty destination
        //:   'function' and an empty source object.
        //:
        //: 3 The 'target_type' and 'target' attributes of the source
        //:   before the move match the corresponding attributes of the
        //:   destination after the move.
        //:
        //: 4 If the source 'function' could be invoked before the move, then
        //:   the destination can be invoked and will yield the same results.
        //:
        //: 5 If the move constructor is invoked (without an allocator), then
        //:   the source and destination after the move will use the original
        //:   source allocator or a copy of it.
        //:
        //: 6 If the allocator-extended move constructor is invoked
        //:   then the destination will use the specified allocator.
        //:
        //: 7 If 'FUNC' takes a 'bslma::Allocator*', then the wrapped functor
        //:   will use the same allocator as the 'function' object (i.e., the
        //:   allocator is propagated).
        //:
        //: 8 The net memory consumption of the source and destination after
        //:   the move is equal to the memory consumption of the source before
        //:   the move.
        //:
        //: 9 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes with or without throwing move constructors.
        //:
        //: 10 If, in the case of the extended move constructor, the original
        //:   and copy use different allocators, the behavior (including
        //:   memory consumption) is as though the copy constructor were
        //:   invoked.
        //:
        //: 11 If the functor move-constructor or the allocator throws an
        //:   exception, then no resources are leaked.
        //:
        //: 12 The above concerns apply if the original 'function' object is
        //:   constructed with a nothrow wrapper.  Specifically, if the
        //:   nothrow wrapper results in an otherwise ineligible functor
        //:   becoming eligible for the small object optimization, then the
        //:   copy will also use the small object optimization.
        //
        // Plan:
        //: 1 For concern 1 move an empty 'function' using both the move
        //:   constructor and extended move constructor and verify that the
        //:   destination in each case is an empty 'function'.
        //:
        //: 2 For concern 2 move a non-empty function and verify that the
        //:   destination is not empty and that the source becomes empty.
        //:
        //: 3 For concern 3, for all 'function' objects moved in this test, //
        //:   create a copy of the source object before the move then preform
        //:   the move construction.  Verify that the 'target_type' of the
        //:   copy of the source compares equal to the 'target_type' of the
        //:   destination.  For non-empty 'function' objects, also verify that
        //:   the 'target' attributes of the copy of the source and the
        //:   destination point to objects that compare equal to each other.
        //:
        //: 4 For concern 4, invoke each non-empty destination and verify that
        //:   the result is the same as invoking a copy of the source before
        //:   the move.
        //:
        //: 5 For concern 5, move-construct a 'function' and verify that
        //:   'allocator()' invoked on both the source and the destination
        //:   after the move returns same the value as invoking 'allocator()'
        //:   on the source before the move or a clone of that value.
        //:
        //: 6 For concern 6, use the extended move constructor and verify that
        //:   the allocator for the destination matches the allocator passed
        //:   into the constructor and that the source allocator is unchanged
        //:   before and after the move operation.
        //:
        //: 7 For concern 7, perform the above steps using a small and a large
        //:   'FUNC' type that take a 'bslma::Allocator*' as well as with
        //:   functors that don't take a 'bslma::Allocator*'.  In the former
        //:   case, verify that the functor wrapped within the
        //:   newly-constructed 'function' object uses the same allocator as
        //:   the newly-created object itself.
        //:
        //: 8 For concern 8, measure the memory used to construct the source
        //:   object and the additional memory consumed in move-constructing
        //:   the destination object.  If the source allocator fits in
        //:   the small-object buffer, verify that the net memory consumption
        //:   did not change during the move; otherwise, verify that the net
        //:   memory consumption increased by one block.
        //:
        //: 9 For concerns 9 and 10, package all of the previous plan steps
        //:   into a function template 'testMoveCtor', instantiated with a
        //:   functor and allocator.  This test template will create an
        //:   original 'function' object using the passed-in functor and
        //:   allocator and -- using a fresh copy each time -- move it with
        //:   the move constructor and with several invocations of the
        //:   extended move constructor, passing in allocators of all of the
        //:   types described in concern 10.  Invoke 'testMoveCtor' with many
        //:   combinations of functor and allocator types so that every
        //:   category combination is represented.
        //:
        //: 10 For concern 11, performed the above steps within the exception
        //:   test framework and verify that, on exception, memory allocation
        //:   does not change and no functor objects are leaked.
        //:
        //: 11 For concern 12, repeat the above steps with a representative
        //:   set of 'FUNC' types. The test for memory allocation (step 8)
        //:   will have the effect of proving that the original and the
        //:   moved-to object either both use the small-object optimization or
        //:   neither uses it.
        //
        // Testing:
        //  function(function&& other);
        //  function(allocator_arg_t, const allocator_type& a,
        //           function&& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTORS"
                            "\n=================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_04
        static const std::size_t k_MAX_ALIGNMENT =
            bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

        // Test that small-object buffer meets the minimum size constraints.
        ASSERTV(k_SMALL_OBJECT_BUFFER_SIZE,
                sizeof(void*), k_MAX_ALIGNMENT,
                k_SMALL_OBJECT_BUFFER_SIZE >= 6 * sizeof(void*));

        // Test our (platform-specific) assumptions about the small-object
        // buffer size.  For historical reasons and backward compatibility, the
        // size of the small object buffer should be the size of 6 pointers on
        // most platforms.
        static const std::size_t k_EXP_SMALL_OBJECT_BUFFER_SIZE =
#if BSLS_PLATFORM_OS_DARWIN && BSLS_PLATFORM_CPU_32_BIT
            // Small object buffer size is rounded up to is 16 bytes on 32-bit
            // OSX builds using Clang.
            8 * sizeof(void*);
#else
            // Small object buffer size is the size of 6 pointers on most
            // platforms.
            6 * sizeof(void*);
#endif
        ASSERTV(k_SMALL_OBJECT_BUFFER_SIZE,
                sizeof(void*), k_MAX_ALIGNMENT,
                k_SMALL_OBJECT_BUFFER_SIZE == k_EXP_SMALL_OBJECT_BUFFER_SIZE);

        // Expected size is 3 pointers larger than buffer, rounded up to max
        // alignment.
        static const std::size_t k_EXP_SIZE =
            (k_SMALL_OBJECT_BUFFER_SIZE + 3 * sizeof(void*)
             + k_MAX_ALIGNMENT - 1) & ~(k_MAX_ALIGNMENT - 1);
        ASSERTV(sizeof(Obj), k_EXP_SIZE,
                sizeof(void*),
                k_SMALL_OBJECT_BUFFER_SIZE,
                k_MAX_ALIGNMENT,
                sizeof(Obj) == k_EXP_SIZE);

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int ( IntWrapper::*nullMemFuncPtr)(int) const = 0;

#define TEST(f, skipExcTest) do {                         \
            if (veryVerbose) printf("FUNC is %s\n", #f);  \
            testMoveCtor(f, skipExcTest);                 \
      } while (false)

        bslma::TestAllocator xa;

        // For testing 'bsl::function' wrapped in 'bsl::function'.
        InnerFunction simpleInnerFunction(bsl::allocator_arg,&xa, &simpleFunc);

        //   Callable object                        skipExceptionTest
        //   =====================================  =================
        TEST(nullFuncPtr                          , false);
        TEST(&simpleFunc                          , false);
        TEST(nullMemFuncPtr                       , false);
        TEST(&IntWrapper::add1                    , false);
        TEST(EmptyFunctor()                       , false);
        TEST(SmallFunctor(0x2000)                 , false);
        TEST(MediumFunctor(0x4000)                , false);
        TEST(LargeFunctor(0x6000)                 , false);
        TEST(NTSmallFunctor(0x3000)               , false);
        TEST(ThrowingSmallFunctor(0x7000)         , false);
        TEST(ThrowingEmptyFunctor()               , false);
        TEST(SmallFunctorWithAlloc(0, &xa)        , false);
        TEST(BMSmallFunctorWithAlloc(0, &xa)      , false);
        TEST(NTSmallFunctorWithAlloc(0, &xa)      , false);
        TEST(LargeFunctorWithAlloc(0, &xa)        , false);
        TEST(simpleInnerFunction                  , false);
        TEST(NTWRAP(nullFuncPtr)                  , true );
        TEST(NTWRAP(simpleFunc)                   , true );
        TEST(NTWRAP(EmptyFunctor())               , true );
        TEST(NTWRAP(SmallFunctor(0x2000))         , true );
        TEST(NTWRAP(MediumFunctor(0x4000))        , true );
        TEST(NTWRAP(LargeFunctor(0x6000))         , true );
        TEST(NTWRAP(NTSmallFunctor(0x3000))       , true );
        TEST(NTWRAP(ThrowingSmallFunctor(0x7000)) , true );
        TEST(NTWRAP(ThrowingEmptyFunctor())       , true );
        TEST(NTWRAP(SmallFunctorWithAlloc(0, &xa)), true );
        TEST(NTWRAP(LargeFunctorWithAlloc(0, &xa)), true );
        TEST(NTWRAP(simpleInnerFunction)          , true );
#undef TEST

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_04
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 Copying an empty 'function' object (with either the copy
        //:   constructor or allocator-extended copy constructor) yields an
        //:   empty 'function' object.
        //:
        //: 2 Copying a non-empty 'function' yields a non-empty 'function'
        //:   object.
        //:
        //: 3 The 'target_type' and 'target' attributes of the original
        //:   'function' match the corresponding attributes of the copy.
        //:
        //: 4 If the original 'function' can be invoked, then its copy can
        //:   also be invoked and will yield the same results.
        //:
        //: 5 If the copy constructor is invoked (without an allocator), then
        //:   the copy will use the value of
        //:   'bslma::Default::defaultAllocator()' at the time of the copy.
        //:
        //: 6 If the allocator-extended copy constructor is invoked
        //:   then the copy will use the specified allocator.
        //:
        //: 7 If 'FUNC' takes a 'bslma::Allocator*', then the 'function'
        //:   allocator is propagated to the wrapped functor.
        //:
        //: 8 The memory allocated by this constructor is the same as if the
        //:   copy were created like the original, except using the specified
        //:   allocator (or default allocator if none specified).
        //:
        //: 9 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes with or without throwing move constructors.
        //:
        //: 10 If the functor copy-constructor or the allocator throws an
        //:   exception, then no resources are leaked.
        //:
        //: 11 The above concerns apply if the original 'function' object is
        //:   constructed with a nothrow wrapper.  Specifically, if the
        //:   nothrow wrapper results in an otherwise ineligible functor
        //:   becoming eligible for the small object optimization, then the
        //:   copy will also use the small object optimization.
        //:
        //: 12 If the argument to the constructor is a reference to a
        //:   'bdef_Function' for the same prototype, the 'bdef_Function' is
        //:   treated as a 'bsl::function' and the copy constructor is invoked,
        //:   yielding a result with identical target rather than wrapping the
        //:   'bdef_Function' itself as a target.
        //
        // Plan:
        //: 1 For concern 1 copy an empty 'function' using both the copy
        //:   constructor and extended copy constructor and verify that the
        //:   result in each case is an empty 'function'.
        //:
        //: 2 For concern 2 copy a non-empty function and verify that the
        //:   result is not empty.
        //:
        //: 3 For concern 3, for all 'function' objects copied in this test,
        //:   verify that the 'target_type' of the original compares equal to
        //:   the 'target_type' of the copy.  For non-empty 'function'
        //:   objects, also verify that the 'target' attributes of the
        //:   original and the copy point to objects that compare equal to
        //:   each other.
        //:
        //: 4 For concern 4, invoke each non-empty copy and verify that the
        //:   result is the same as invoking the original.
        //:
        //: 5 For concern 5, copy-construct a 'function' and verify that
        //:   invoking 'allocator()' on the copy returns
        //:   'bslma::Default::defaultAllocator()'.  Change the default
        //:   allocator temporarily and verify that 'allocator()' still
        //:   returns the default allocator at the time of the copy.
        //:
        //: 6 For concern 6, use the extended copy constructor and verify that
        //:   the allocator for the copy matches the allocator passed into the
        //:   constructor (as was done for the previous test case).
        //:
        //: 7 For concern 7, perform the above steps using a small and a large
        //:   'FUNC' type that take a 'bslma::Allocator*' as well as with
        //:   functors that don't take a 'bslma::Allocator*.  In the former
        //:   case, verify that the functor wrapped within the
        //:   newly-constructed 'function' object uses the same allocator as
        //:   the newly-created object itself.
        //:
        //: 8 For concern 8, construct a function object 'f1' using a specific
        //:   'func' argument and allocator 'a1'.  Construct a second function
        //:   object 'f1' using the same 'func' argument and an allocator
        //:   'a2'.  Using the extended copy constructor, create a copy of
        //:   'f1' using allocator 'a2'.  Verify that the memory allocations
        //:   during this construction match those in constructing 'f2'.
        //:
        //: 9 For concern 9, package all of the previous plan steps
        //:   into a function template 'testCopyCtor', instantiated with a
        //:   functor and allocator.  This test template will create an
        //:   original 'function' object using the passed-in functor and
        //:   allocator and copy it with the copy constructor and with several
        //:   invocations of the extended copy constructor.  Invoke
        //:   'testCopyCtor' with many combinations of functor
        //:   types so that every category is represented.
        //:
        //: 10 For concern 10, performed the above steps within the exception
        //:   test framework and verify that, on exception, memory allocation
        //:   does not change and no functor objects are leaked.
        //:
        //: 11 For concern 11, repeat the above steps with a representative
        //:   set of wrapped 'FUNC' types. The test for memory allocation
        //:   (step 8) will have the effect of proving that the original and
        //:   the moved-to object either both use the small-object
        //:   optimization or neither uses it.
        //:
        //: 12 For concern 12, construct a 'bdef_Function' from a functor and
        //:   use it to construct 'bsl::function' with the same prototype.
        //:   Verify that the target of the newly-constructed object is a copy
        //:   of the functor.
        //
        // Testing:
        //  function(const function& other);
        //  function(allocator_arg_t, const allocator_type& a,
        //           const function& other);
        //  CONCERN: Construction from 'bdef_Function' does not double-wrap
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_03
        bslma::TestAllocator xa;

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int ( IntWrapper::*nullMemFuncPtr)(int) const = 0;

        // For testing 'bsl::function' wrapped in 'bsl::function'.
        InnerFunction emptyInnerFunction;
        InnerFunction simpleInnerFunction(bsl::allocator_arg,&xa, &simpleFunc);

#define TEST(f) do {                                         \
            if (veryVerbose) printf("FUNC is %s\n", #f);     \
            testCopyCtor(f);                                 \
        } while (false)

        TEST(nullFuncPtr                          );
        TEST(nullMemFuncPtr                       );
        TEST(emptyInnerFunction                   );
        TEST(simpleFunc                           );
        TEST(&IntWrapper::add1                    );
        TEST(EmptyFunctor()                       );
        TEST(SmallFunctor(0x2000)                 );
        TEST(MediumFunctor(0x4000)                );
        TEST(LargeFunctor(0x6000)                 );
        TEST(NTSmallFunctor(0x3000)               );
        TEST(ThrowingSmallFunctor(0x7000)         );
        TEST(ThrowingEmptyFunctor()               );
        TEST(SmallFunctorWithAlloc(0, &xa)        );
        TEST(BMSmallFunctorWithAlloc(0, &xa)      );
        TEST(NTSmallFunctorWithAlloc(0, &xa)      );
        TEST(LargeFunctorWithAlloc(0, &xa)        );
        TEST(simpleInnerFunction                  );
        TEST(NTWRAP(nullFuncPtr)                  );
        TEST(NTWRAP(simpleFunc)                   );
        TEST(NTWRAP(emptyInnerFunction)           );
        TEST(NTWRAP(EmptyFunctor())               );
        TEST(NTWRAP(SmallFunctor(0x2000))         );
        TEST(NTWRAP(MediumFunctor(0x4000))        );
        TEST(NTWRAP(LargeFunctor(0x6000))         );
        TEST(NTWRAP(NTSmallFunctor(0x3000))       );
        TEST(NTWRAP(ThrowingSmallFunctor(0x7000)) );
        TEST(NTWRAP(ThrowingEmptyFunctor())       );
        TEST(NTWRAP(SmallFunctorWithAlloc(0, &xa)));
        TEST(NTWRAP(LargeFunctorWithAlloc(0, &xa)));
        TEST(NTWRAP(simpleInnerFunction)          );
#undef TEST

        typedef BloombergLP::bdef_Function<PROTO*>  bdefObj;
        SmallFunctor                                functor(0x30);
        SmallFunctor                               *target;

        // Test copy construct from 'bdef_Function'
        {
            bdefObj original(functor);
            Obj     copyObj(original);
            target = copyObj.target<SmallFunctor>();
            ASSERT(target && *target == functor);
        }
        {
            bdefObj original(functor);
            Obj    copyObj(bsl::allocator_arg, &xa, original);
            target = copyObj.target<SmallFunctor>();
            ASSERT(target && *target == functor);
        }
        {
            bdefObj original(functor);
            Obj     copyObj(bslmf::MovableRefUtil::move(original));
            target = copyObj.target<SmallFunctor>();
            ASSERT(target && *target == functor);
        }
        {
            bdefObj original(functor);
            Obj     copyObj(bsl::allocator_arg, &xa,
                            bslmf::MovableRefUtil::move(original));
            target = copyObj.target<SmallFunctor>();
            ASSERT(target && *target == functor);
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_03
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONSTRUCT FROM CALLABLE OBJECT
        //
        // Concerns:
        //  ('FUNC' is the callable type, 'func' is the callable argument,
        //  and 'alloc' is the supplied allocator argument, if any.)
        //
        //: 1 If 'func' is a null pointer-to-function or null
        //:   pointer-to-member-function, the constructor yields an empty
        //:   'function' object.
        //:
        //: 2 If 'func' is other than a null pointer, the constructor yields a
        //:   non-empty (callable) 'function'.  Note that invocation testing
        //:   is only to ensure that the function was constructed
        //:   successfully.  It is not necessary to thoroughly test all
        //:   argument-list combinations.
        //:
        //: 3 The 'target_type()' accessor will return 'type_id(FUNC)' and the
        //:   'target<FUNC>()' accessor will return a pointer to a functor
        //:   that compares equal to 'func'. If invoked with a movable
        //:   (rvalue) reference to 'FUNC', 'target<FUNC>()' returns a pointer
        //:   to an object that was moved from the argument, with no
        //:   intervening copies.
        //:
        //: 4 If no allocator is specified, the default allocator is used to
        //:   construct the 'function' object.
        //:
        //: 5 If an allocator is specified, it is used to construct the
        //:   'function' object.
        //:
        //: 6 The 'get_allocator' accessor will return the allocator used to
        //:   construct the 'function', converted to
        //:   'bsl::allocator<char>'. The 'allocator' accessor will return
        //:   'get_allocator().mechanism()'.
        //:
        //: 7 If 'FUNC' is eligible for the small object optimization, no
        //:   memory is allocated by this constructor (though the contained
        //:   target may, itself, allocate).
        //:
        //: 8 If 'FUNC' is not eligible for the small object optimization, one
        //:   block of memory of sufficient size to hold 'FUNC' is allocated
        //:   from the allocator by this constructor (though the contained
        //:   target may, itself, allocate more).
        //:
        //: 9 If memory is allocated, the destructor frees it.
        //:
        //: 10 If 'FUNC' takes a 'bslma::Allocator*', or 'bsl::allocator',
        //:    then the 'function' allocator is propagated to the wrapped
        //:    functor.
        //:
        //: 11 The above concerns apply to 'func' arguments of type pointer to
        //:    function, pointer to member function, or functor types of
        //:    various sizes and cv-qualified rvalue and lvalue references to
        //:    functor types of various sizes, with or without throwing move
        //:    constructors. TBD pointer-to-DATA-member
        //:
        //: 12 If memory allocation or functor construction fails with an
        //:    exception, then no resources are leaked.
        //:
        //: 13 If 'FUNC' is wrapped in a nothrow wrapper, all of the above
        //:    concerns hold. For concerns that hinge on the presence or
        //:    absence of the small object optimization, any sufficiently-small
        //:    'FUNC' is eligible for the optimization when wrapped in a
        //:    nothrow wrapper, even if it would otherwise throw.
        //:
        //: 14 An object or reference to a compatible 'FUNC' is implicitly
        //:    convertible to 'bsl::function'.  Regression test: If 'FUNC' is
        //:    constructible from 'function', no constructor ambiguity is
        //:    created as a result of an internal implicit move in C++03, even
        //:    if 'FUNC' does not have a dedicated move constructor.
        //:
        //: 15 If 'FUNC' is an empty 'function' with a different, but
        //:    compatible, prototype, then the constructed 'function' is empty;
        //:    it does not wrap the empty 'function' as a target.
        //:
        //: 16 When compiling with an IBM XL C++ compiler, 'bsl::function' is
        //:    constructible from a function having default arguments despite a
        //:    defect in said compiler that disallows forming 'typedef's to
        //:    function types deduced from functions that have default
        //:    arguments.
        //:
        //: 17 In C++11, if 'FUNC' is not Lvalue-Callable with the arguments
        //:    and return type of the prototype of the 'bsl::function', the
        //:    constructor does not participate in overload resolution.
        //
        // Plan:
        //: 1 For concern 1, construct 'function' objects using a null pointer
        //:   to function and a null pointer to member function and verify
        //:   that each returns false when converted to a Boolean value.
        //:
        //: 2 For concern 2, invoke every non-empty 'function' constructed by
        //:   this test and verify that it produces the expected results.
        //:
        //: 3 For concern 3, verify, for each 'function' constructed, that
        //:   'target_type' and 'target<FUNC>' return the expected values. For
        //:   each 'FUNC' type tested, also test 'bslmf::MovableRef<FUNC>' and
        //:   verify (when possible) that 'target<FUNC>' is moved from the
        //:   reference argument with no intervening copies.  The last is
        //:   facilitated by using functor types based on 'TrackableValue',
        //:   which allows tracking moves through multiple levels of function
        //:   call.
        //:
        //: 4 For concerns 4 and 6, construct a 'function' object without an
        //:   allocator argument.  Verify that 'allocator()' returns the
        //:   address of the default test allocator and that 'get_allocator()'
        //:   returns 'bsl::allocator<char>()'
        //:
        //: 5 For concerns 5 and 6, construct a 'function' object with the
        //:   address of a 'bslma:TestAllocator'.  Verify that 'allocator()'
        //:   returns the address of the test allocator and that
        //:   'get_allocator' returns 'bsl::allocator<char>(allocator())'.
        //:
        //: 6 For concern 7, test the results of steps 2-4 to verify that when
        //:   'func' is eligible for the small object optimization, no memory
        //:   is allocated from either the global allocator or the
        //:   allocator used to construct the 'function' object.  Also verify
        //:   that the deprecated 'isInplace' method returns true.
        //:
        //: 7 For concern 8, test the results of steps 2-4 to verify that when
        //:   'func' is not eligible the small object optimization, one block
        //:   of memory, of sufficient size to hold 'FUNC', is allocated from
        //:   the allocator used to construct the 'function' object.  Also
        //:   verify that the deprecated 'isInplace' method returns false.
        //:
        //: 8 For concern 9, check at the end of each step, when the
        //:   'function' object is destroyed, that all memory is returned to
        //:   the allocator from which it came.
        //:
        //: 9 For concern 10, perform the above steps using a small and a
        //:   large 'FUNC' type that take a 'bslma::Allocator*' as well as
        //:   with functors that don't take a 'bslma::Allocator*.  After
        //:   construction, verify that the wrapped functor uses the same
        //:   allocator as the 'function' object in the former case.
        //:
        //: 10 For concern 11, wrap the common parts of the above steps into a
        //:    function template, 'testConstructFromCallableObj', which takes
        //:    'FUNC' as a template parameter.  Instantiate this
        //:    template with each of the following callable types: pointer to
        //:    function, pointer to member function, and functor types of of
        //:    all varieties.
        //:
        //: 11 For concern 12, within 'testConstructFromCallableObj', construct
        //:    the 'function' within the exception test framework.  On
        //:    exception, verify that any allocated memory has been released
        //:    and that no copies of 'func' have been leaked.
        //:
        //: 12 For concern 13, invoke 'testConstructFromCallableObj' on an
        //:    interesting selection of functor types wrapped in a nothrow
        //:    wrapper. Verify that for all functors that fit in the small
        //:    object buffer, even if they have throwing move constructors, no
        //:    memory is allocated (i.e., the functor is stored in the small
        //:    object buffer).
        //:
        //: 13 For concern 14, add tests to 'testConstructFromCallableObj' that
        //:    require conversion from 'FUNC' to 'bsl::function'.  Invoke
        //:    'testConstructFromCallableObj' on a class that lacks dedicated
        //:    move operations.  Repeat for such a class that also is
        //:    constructible from 'function'.  Verify that a transformation
        //:    from 'MovableRef<FUNC>' to 'FUNC' is a better match for
        //:    overloading than a conversion from 'MovableRef<FUNC>' to
        //:    'bsl::function' (regression test from prior ambiguity).
        //:
        //: 14 For concern 15, construct a 'function' from an empty 'function'
        //:    having a different prototype.  Verify that the resulting object
        //:    is empty.
        //:
        //: 15 For concern 16, construct a 'bsl::function' from a function that
        //:    has at least 1 default argument.
        //:
        //: 16 For concern 17, In C++11, test constructing 'bsl::function'
        //:    objects with a wide variety of functors, and verify that the
        //:    constructor from that functor only participates in overload
        //:    resolution if the functor is Lvalue-Callable with the return
        //:    type and arguments of the prototype of the 'bsl::function'.
        //
        // Testing
        //  function(FUNC func);
        //  function(allocator_arg_t, const allocator_type& a, FUNC func);
        //  ~function();
        //  bool isInplace() const noexcept;
        // --------------------------------------------------------------------


        static const char* title[] = {
#if defined(BSLSTL_FUNCTION_TEST_ENABLE_ALL) || \
    !defined(BSLSTL_FUNCTION_TEST_PART_02_OR_03)
            "",
            ""
#elif defined(BSLSTL_FUNCTION_TEST_PART_02)
            ": PART 1",
            "========"
#elif defined(BSLSTL_FUNCTION_TEST_PART_03)
            ": PART 2",
            "========"
#endif
        };

        if (verbose) printf("\nCONSTRUCT FROM CALLABLE OBJECT%s"
                            "\n==============================%s\n",
                             title[0], title[1]);

#ifdef BSLSTL_FUNCTION_TEST_PART_02_OR_03

#ifdef BSLSTL_FUNCTION_TEST_ENABLE_ALL
#define BSLSTL_FUNCTION_TEST_CASE05_PART_1
#define BSLSTL_FUNCTION_TEST_CASE05_PART_2

#elif defined(BSLSTL_FUNCTION_TEST_PART_02)
#define BSLSTL_FUNCTION_TEST_CASE05_PART_1

#elif defined(BSLSTL_FUNCTION_TEST_PART_03)
#define BSLSTL_FUNCTION_TEST_CASE05_PART_2
#endif
        bslma::TestAllocator xa;

        // Null pointers
#ifdef BSLSTL_FUNCTION_TEST_CASE05_PART_1
        int (*nullFuncPtr)(IntWrapper, int)           = 0;
        int ( IntWrapper::*nullMemFuncPtr)(int) const = 0;
#endif  // BSLSTL_FUNCTION_TEST_CASE05_PART_1

        // For testing 'bsl::function' wrapped in 'bsl::function'.
        InnerFunction emptyInnerFunction;
        InnerFunction simpleInnerFunction(bsl::allocator_arg,&xa, &simpleFunc);

#define TEST(f, W, E) testConstructFromCallableObj(f, #f, W, E)

        //   Functor                                  isWrapped  expInplace
        //   ======================================   =========  ==========
#ifdef BSLSTL_FUNCTION_TEST_CASE05_PART_1
        TEST(bsl::nullptr_t()                       , false    , true );
        TEST(       nullFuncPtr                     , false    , true );
        TEST(NTWRAP(nullFuncPtr)                    , true     , true );
        TEST(       nullMemFuncPtr                  , false    , true );
        TEST(NTWRAP(nullMemFuncPtr)                 , true     , true );
        TEST(       emptyInnerFunction              , false    , true );
        TEST(NTWRAP(emptyInnerFunction)             , true     , true );
        TEST(       &simpleFunc                     , false    , true );
        TEST(NTWRAP(&simpleFunc)                    , true     , true );
        TEST(       &IntWrapper::add1               , false    , true );
        TEST(NTWRAP(&IntWrapper::add1)              , true     , true );
        TEST(       EmptyFunctor()                  , false    , true );
        TEST(NTWRAP(EmptyFunctor())                 , true     , true );
        TEST(       SmallFunctor(0)                 , false    , true );
        TEST(NTWRAP(SmallFunctor(0))                , true     , true );
        TEST(       MediumFunctor(0)                , false    , true );
        TEST(NTWRAP(MediumFunctor(0))               , true     , true );
        TEST(       LargeFunctor(0)                 , false    , false);
        TEST(NTWRAP(LargeFunctor(0))                , true     , false);
        TEST(       NTSmallFunctor(0)               , false    , true );
        TEST(NTWRAP(NTSmallFunctor(0))              , true     , true );
        TEST(       BMSmallFunctorWithAlloc(0, &xa) , false    , true );
        TEST(NTWRAP(BMSmallFunctorWithAlloc(0, &xa)), true     , true );
        TEST(       NTSmallFunctorWithAlloc(0, &xa) , false    , true );
        TEST(NTWRAP(NTSmallFunctorWithAlloc(0, &xa)), true     , true );
#endif  // BSLSTL_FUNCTION_TEST_CASE05_PART_1

// **NOT** using '#else' on purpose, *both* PART macros may be defined.
#ifdef BSLSTL_FUNCTION_TEST_CASE05_PART_2
        TEST(       ThrowingSmallFunctor(0)         , false    , false);
        TEST(NTWRAP(ThrowingSmallFunctor(0))        , true     , true );
        TEST(       ThrowingEmptyFunctor(0)         , false    , false);
        TEST(NTWRAP(ThrowingEmptyFunctor(0))        , true     , true );

        TEST(       SmallFunctorWithAlloc(  0, &xa) , false    , false);
        TEST(NTWRAP(SmallFunctorWithAlloc(  0, &xa)), true     , true );
        TEST(       LargeFunctorWithAlloc(  0, &xa) , false    , false);
        TEST(NTWRAP(LargeFunctorWithAlloc(  0, &xa)), true     , false);
        TEST(       simpleInnerFunction             , false    , false);
        TEST(NTWRAP(simpleInnerFunction)            , true     , false);

        TEST(       FunctorWithoutDedicatedMove(0)  , false    , false);
        TEST(NTWRAP(FunctorWithoutDedicatedMove(0)) , true     , true );

        Obj x(&simpleFunc);
        TEST(       FunctorWithFunctionCtor(x)      , false    , false);
        TEST(NTWRAP(FunctorWithFunctionCtor(x))     , true     , false);
#endif  // BSLSTL_FUNCTION_TEST_CASE05_PART_2
#undef TEST

#ifdef BSLSTL_FUNCTION_TEST_CASE05_PART_2

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
#define TEST(p, f, c) testIsConstructible<p, f>(__LINE__, #f, c)

        typedef IntWrapper                        IW;
        typedef SmartPtr<IntWrapper>              SIW;
        typedef SmartPtr<const IntWrapper>        ScIW;
        typedef RWrap<IntWrapper>                 RIW;
        typedef RWrap<const IntWrapper>           RcIW;

        typedef IntWrapperDerived                 IWD;
        typedef SmartPtr<IntWrapperDerived>       SIWD;
        typedef SmartPtr<const IntWrapperDerived> ScIWD;
        typedef RWrap<IntWrapperDerived>          RIWD;
        typedef RWrap<const IntWrapperDerived>    RcIWD;

        //                                                 Constructible?
        //  Proto Functor Type                                           \.
        //  ===== ==================================================== =======

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // const- and/or reference-qualified functions, pointer to functions,
        // and pointers to member functions that vary in arity and return-type
        // compatibility.

        TEST(PROTO, int     ()                                        , false);
        TEST(PROTO, int     (IW)                                      , false);
        TEST(PROTO, int     (IW, int)                                 , true );
        TEST(PROTO, int     (IW, int, int)                            , false);
        TEST(PROTO, void    ()                                        , false);
        TEST(PROTO, void    (IW)                                      , false);
        TEST(PROTO, void    (IW, int)                                 , false);
        TEST(PROTO, void    (IW, int, int)                            , false);
        TEST(PROTO, int     ()                                        , false);
        TEST(PROTO, int     (IWD)                                     , false);
        TEST(PROTO, int     (IWD, int)                                , false);
        TEST(PROTO, int     (IWD, int, int)                           , false);
        TEST(PROTO, int  (*)()                                        , false);
        TEST(PROTO, int  (*)(IW)                                      , false);
        TEST(PROTO, int  (*)(IW, int)                                 , true );
        TEST(PROTO, int  (*)(IW, int, int)                            , false);
        TEST(PROTO, void (*)()                                        , false);
        TEST(PROTO, void (*)(IW)                                      , false);
        TEST(PROTO, void (*)(IW, int)                                 , false);
        TEST(PROTO, void (*)(IW, int, int)                            , false);
        TEST(PROTO, int  (IW::*)()                                    , false);
        TEST(PROTO, int  (IW::*)(int)                                 , false);
        TEST(PROTO, int  (IW::*)(int, int)                            , false);
        TEST(PROTO, void (IW::*)()                                    , false);
        TEST(PROTO, void (IW::*)(int)                                 , false);
        TEST(PROTO, void (IW::*)(int, int)                            , false);
        TEST(PROTO, int  (IW::*)(int) &                               , false);
        TEST(PROTO, void (IW::*)(int) &                               , false);
        TEST(PROTO, int  (IW::*)(int) const                           , true );
        TEST(PROTO, void (IW::*)(int) const                           , false);
        TEST(PROTO, int  (IW::*)(int) const&                          , true );
        TEST(PROTO, void (IW::*)(int) const&                          , false);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(PROTO, int  (IW::*)(int) &&                              , false);
        TEST(PROTO, void (IW::*)(int) &&                              , false);
        TEST(PROTO, int  (IW::*)(int) const&&                         , false);
        TEST(PROTO, void (IW::*)(int) const&&                         , false);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // pointers to member objects.
        TEST(PROTO, int IW::*                                         , false);

        // 'bsl::function' objects having 'PROTO' prototype constructed with
        // optionally const- and/or reference-qualified function objects,
        // having function-call operators that vary in arity and return type
        // compatibility.
        TEST(PROTO,       HypotheticalFunctor<int ()>                 , false);
        TEST(PROTO,       HypotheticalFunctor<int (IW)>               , false);
        TEST(PROTO,       HypotheticalFunctor<int (IW, int)>          , true );
        TEST(PROTO,       HypotheticalFunctor<int (IW, int, int)>     , false);
        TEST(PROTO,       HypotheticalFunctor<void()>                 , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW)>               , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW, int)>          , false);
        TEST(PROTO,       HypotheticalFunctor<void(IW, int, int)>     , false);
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>          , true );
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>&         , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(PROTO,       HypotheticalFunctor<int (IW, int)>&&        , true );
        TEST(PROTO, const HypotheticalFunctor<int (IW, int)>&&        , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        //                                           Constructible?
        //  Prototype                    Functor Type              \.
        //  ============================ ======================= =======

        // 'bsl::function' objects having prototypes with arguments and return
        // values implicitly convertible to and/or from those of the function
        // object from which they're constructed.
        TEST(      int   (int          ), int   (int  )         , true );
        TEST(      int   (int          ), int   (float)         , true );
        TEST(      int   (int          ), float (int  )         , true );
        TEST(      int   (int          ), float (float)         , true );
        TEST(      int   (float        ), int   (int  )         , true );
        TEST(      int   (float        ), int   (float)         , true );
        TEST(      int   (float        ), float (int  )         , true );
        TEST(      int   (float        ), float (float)         , true );
        TEST(      float (int          ), int   (int  )         , true );
        TEST(      float (int          ), int   (float)         , true );
        TEST(      float (int          ), float (int  )         , true );
        TEST(      float (int          ), float (float)         , true );
        TEST(      float (float        ), int   (int  )         , true );
        TEST(      float (float        ), int   (float)         , true );
        TEST(      float (float        ), float (int  )         , true );
        TEST(      float (float        ), float (float)         , true );

        // 'bsl::function' objects having prototypes with const- and/or
        // reference-qualified arguments and results, constructed with pointers
        // to member objects.
        TEST(      void  (        IW   ), int IW::*             , true );
        TEST(      void  (        IW&  ), int IW::*             , true );
        TEST(      void  (const   IW   ), int IW::*             , true );
        TEST(      void  (const   IW&  ), int IW::*             , true );
        TEST(      int   (        IW   ), int IW::*             , true );
        TEST(      int   (        IW&  ), int IW::*             , true );
        TEST(      int   (const   IW   ), int IW::*             , true );
        TEST(      int   (const   IW&  ), int IW::*             , true );
        TEST(      int&  (        IW   ), int IW::*             , false);
        TEST(      int&  (        IW&  ), int IW::*             , true );
        TEST(      int&  (const   IW   ), int IW::*             , false);
        TEST(      int&  (const   IW&  ), int IW::*             , false);
        TEST(const int&  (        IW   ), int IW::*             , true );
        TEST(const int&  (        IW&  ), int IW::*             , true );
        TEST(const int&  (const   IW   ), int IW::*             , true );
        TEST(const int&  (const   IW&  ), int IW::*             , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(      void  (        IW&& ), int IW::*             , true );
        TEST(      void  (const   IW&& ), int IW::*             , true );
        TEST(      int   (        IW&& ), int IW::*             , true );
        TEST(      int   (const   IW&& ), int IW::*             , true );
        TEST(      int&  (        IW&& ), int IW::*             , false);
        TEST(      int&  (const   IW&& ), int IW::*             , false);
        TEST(const int&  (        IW&& ), int IW::*             , true );
        TEST(const int&  (const   IW&& ), int IW::*             , true );
        TEST(      int&& (        IW   ), int IW::*             , true );
        TEST(      int&& (        IW&  ), int IW::*             , true );
        TEST(      int&& (        IW&& ), int IW::*             , true );
        TEST(      int&& (const   IW   ), int IW::*             , true );
        TEST(      int&& (const   IW&  ), int IW::*             , false);
        TEST(      int&& (const   IW&& ), int IW::*             , false);
        TEST(const int&& (        IW   ), int IW::*             , true );
        TEST(const int&& (        IW&  ), int IW::*             , true );
        TEST(const int&& (        IW&& ), int IW::*             , true );
        TEST(const int&& (const   IW   ), int IW::*             , true );
        TEST(const int&& (const   IW&  ), int IW::*             , true );
        TEST(const int&& (const   IW&& ), int IW::*             , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with smart pointer to, and
        // reference-wrapped class types, constructed with pointers to member
        // objects.
        TEST(       int  (        IW   ), int  IW::*            , true );
        TEST(       int  (        IW&  ), int  IW::*            , true );
        TEST(       int  (const   IW   ), int  IW::*            , true );
        TEST(       int  (const   IW&  ), int  IW::*            , true );
        TEST(       int  (       RIW   ), int  IW::*            , true );
        TEST(       int  (       RIW&  ), int  IW::*            , true );
        TEST(       int  (const  RIW   ), int  IW::*            , true );
        TEST(       int  (const  RIW&  ), int  IW::*            , true );
        TEST(       int  (      RcIW   ), int  IW::*            , true );
        TEST(       int  (      RcIW&  ), int  IW::*            , true );
        TEST(       int  (const RcIW   ), int  IW::*            , true );
        TEST(       int  (const RcIW&  ), int  IW::*            , true );
        TEST(       int  (       SIW   ), int  IW::*            , true );
        TEST(       int  (       SIW&  ), int  IW::*            , true );
        TEST(       int  (const  SIW   ), int  IW::*            , true );
        TEST(       int  (const  SIW&  ), int  IW::*            , true );
        TEST(       int  (      ScIW   ), int  IW::*            , true );
        TEST(       int  (      ScIW&  ), int  IW::*            , true );
        TEST(       int  (const ScIW   ), int  IW::*            , true );
        TEST(       int  (const ScIW&  ), int  IW::*            , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IW&& ), int  IW::*            , true );
        TEST(       int  (const   IW&& ), int  IW::*            , true );
        TEST(       int  (       RIW&& ), int  IW::*            , true );
        TEST(       int  (const  RIW&& ), int  IW::*            , true );
        TEST(       int  (      RcIW&& ), int  IW::*            , true );
        TEST(       int  (const RcIW&& ), int  IW::*            , true );
        TEST(       int  (       SIW&& ), int  IW::*            , true );
        TEST(       int  (const  SIW&& ), int  IW::*            , true );
        TEST(       int  (      ScIW&& ), int  IW::*            , true );
        TEST(       int  (const ScIW&& ), int  IW::*            , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with smart pointer to, and
        // reference-wrapped class types, constructed with pointers to member
        // objects of a base type.
        TEST(       int  (        IWD  ), int  IW::*            , true );
        TEST(       int  (        IWD& ), int  IW::*            , true );
        TEST(       int  (const   IWD  ), int  IW::*            , true );
        TEST(       int  (const   IWD& ), int  IW::*            , true );
        TEST(       int  (       RIWD  ), int  IW::*            , true );
        TEST(       int  (       RIWD& ), int  IW::*            , true );
        TEST(       int  (const  RIWD  ), int  IW::*            , true );
        TEST(       int  (const  RIWD& ), int  IW::*            , true );
        TEST(       int  (      RcIWD  ), int  IW::*            , true );
        TEST(       int  (      RcIWD& ), int  IW::*            , true );
        TEST(       int  (const RcIWD  ), int  IW::*            , true );
        TEST(       int  (const RcIWD& ), int  IW::*            , true );
        TEST(       int  (       SIWD  ), int  IW::*            , true );
        TEST(       int  (       SIWD& ), int  IW::*            , true );
        TEST(       int  (const  SIWD  ), int  IW::*            , true );
        TEST(       int  (const  SIWD& ), int  IW::*            , true );
        TEST(       int  (      ScIWD  ), int  IW::*            , true );
        TEST(       int  (      ScIWD& ), int  IW::*            , true );
        TEST(       int  (const ScIWD  ), int  IW::*            , true );
        TEST(       int  (const ScIWD& ), int  IW::*            , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IWD&&), int  IW::*            , true );
        TEST(       int  (const   IWD&&), int  IW::*            , true );
        TEST(       int  (       RIWD&&), int  IW::*            , true );
        TEST(       int  (const  RIWD&&), int  IW::*            , true );
        TEST(       int  (      RcIWD&&), int  IW::*            , true );
        TEST(       int  (const RcIWD&&), int  IW::*            , true );
        TEST(       int  (       SIWD&&), int  IW::*            , true );
        TEST(       int  (const  SIWD&&), int  IW::*            , true );
        TEST(       int  (      ScIWD&&), int  IW::*            , true );
        TEST(       int  (const ScIWD&&), int  IW::*            , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects with prototypes having smart pointer to, and
        // reference-wrapped, class types, constructed with pointers to member
        // functions.
        TEST(       int  (        IW   ), int (IW::*)()         , true );
        TEST(       int  (       RIW   ), int (IW::*)()         , true );
        TEST(       int  (      RcIW   ), int (IW::*)()         , false);
        TEST(       int  (       SIW   ), int (IW::*)()         , true );
        TEST(       int  (      ScIW   ), int (IW::*)()         , false);
        TEST(       int  (        IW   ), int (IW::*)() &       , false);
        TEST(       int  (       RIW   ), int (IW::*)() &       , true );
        TEST(       int  (      RcIW   ), int (IW::*)() &       , false);
        TEST(       int  (       SIW   ), int (IW::*)() &       , true );
        TEST(       int  (      ScIW   ), int (IW::*)() &       , false);
        TEST(       int  (        IW   ), int (IW::*)() const   , true );
        TEST(       int  (       RIW   ), int (IW::*)() const   , true );
        TEST(       int  (      RcIW   ), int (IW::*)() const   , true );
        TEST(       int  (       SIW   ), int (IW::*)() const   , true );
        TEST(       int  (      ScIW   ), int (IW::*)() const   , true );
        TEST(       int  (        IW   ), int (IW::*)() const&  , MSVC ||
                                                                  CPP_20);
        TEST(       int  (       RIW   ), int (IW::*)() const&  , true );
        TEST(       int  (      RcIW   ), int (IW::*)() const&  , true );
        TEST(       int  (       SIW   ), int (IW::*)() const&  , true );
        TEST(       int  (      ScIW   ), int (IW::*)() const&  , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IW   ), int (IW::*)() &&      , true );
        TEST(       int  (       RIW   ), int (IW::*)() &&      , false);
        TEST(       int  (      RcIW   ), int (IW::*)() &&      , false);
        TEST(       int  (       SIW   ), int (IW::*)() &&      , false);
        TEST(       int  (      ScIW   ), int (IW::*)() &&      , false);
        TEST(       int  (        IW   ), int (IW::*)() const&& , true );
        TEST(       int  (       RIW   ), int (IW::*)() const&& , false);
        TEST(       int  (      RcIW   ), int (IW::*)() const&& , false);
        TEST(       int  (       SIW   ), int (IW::*)() const&& , false);
        TEST(       int  (      ScIW   ), int (IW::*)() const&& , false);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects with prototypes having smart pointer to, and
        // reference-wrapped, class types, constructed with pointers to member
        // functions of base types.
        TEST(       int  (        IWD  ), int (IW::*)()         , true );
        TEST(       int  (       RIWD  ), int (IW::*)()         , true );
        TEST(       int  (      RcIWD  ), int (IW::*)()         , false);
        TEST(       int  (       SIWD  ), int (IW::*)()         , true );
        TEST(       int  (      ScIWD  ), int (IW::*)()         , false);
        TEST(       int  (        IWD  ), int (IW::*)() &       , MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() &       , true );
        TEST(       int  (      RcIWD  ), int (IW::*)() &       , false);
        TEST(       int  (       SIWD  ), int (IW::*)() &       , true );
        TEST(       int  (      ScIWD  ), int (IW::*)() &       , false);
        TEST(       int  (        IWD  ), int (IW::*)() const   , true );
        TEST(       int  (       RIWD  ), int (IW::*)() const   , true );
        TEST(       int  (      RcIWD  ), int (IW::*)() const   , true );
        TEST(       int  (       SIWD  ), int (IW::*)() const   , true );
        TEST(       int  (      ScIWD  ), int (IW::*)() const   , true );
        TEST(       int  (        IWD  ), int (IW::*)() const&  , MSVC ||
                                                                  CPP_20);
        TEST(       int  (       RIWD  ), int (IW::*)() const&  , true );
        TEST(       int  (      RcIWD  ), int (IW::*)() const&  , true );
        TEST(       int  (       SIWD  ), int (IW::*)() const&  , true );
        TEST(       int  (      ScIWD  ), int (IW::*)() const&  , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(       int  (        IWD  ), int (IW::*)() &&      , !MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() &&      , false);
        TEST(       int  (      RcIWD  ), int (IW::*)() &&      , false);
        TEST(       int  (       SIWD  ), int (IW::*)() &&      , false);
        TEST(       int  (      ScIWD  ), int (IW::*)() &&      , false);
        TEST(       int  (        IWD  ), int (IW::*)() const&& , !MSVC_2015);
        TEST(       int  (       RIWD  ), int (IW::*)() const&& , false);
        TEST(       int  (      RcIWD  ), int (IW::*)() const&& , false);
        TEST(       int  (       SIWD  ), int (IW::*)() const&& , false);
        TEST(       int  (      ScIWD  ), int (IW::*)() const&& , false);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // 'bsl::function' objects having prototypes with const- and/or
        // reference-qualified 'IntWrapper' return values, constructed with
        // lvalue-callable objects that return compatible types.
        TEST(      IW    (             ),       IW    ()        , true );
        TEST(      IW    (             ),       IW&   ()        , true );
        TEST(      IW    (             ), const IW    ()        , true );
        TEST(      IW    (             ), const IW&   ()        , true );
        TEST(      IW&   (             ),       IW    ()        , MSVC );
        TEST(      IW&   (             ),       IW&   ()        , true );
        TEST(      IW&   (             ), const IW    ()        , false);
        TEST(      IW&   (             ), const IW&   ()        , false);
        TEST(const IW    (             ),       IW    ()        , true );
        TEST(const IW    (             ),       IW&   ()        , true );
        TEST(const IW    (             ), const IW    ()        , true );
        TEST(const IW    (             ), const IW&   ()        , true );
        TEST(const IW&   (             ),       IW    ()        , true );
        TEST(const IW&   (             ),       IW&   ()        , true );
        TEST(const IW&   (             ), const IW    ()        , true );
        TEST(const IW&   (             ), const IW&   ()        , true );
        TEST(      IWD   (             ),       IW    ()        , false);
        TEST(      IW    (             ),       IWD   ()        , true );
        TEST(      IW    (             ),       IWD&  ()        , true );
        TEST(      IW    (             ), const IWD   ()        , true );
        TEST(      IW    (             ), const IWD&  ()        , true );
        TEST(      IW&   (             ),       IWD   ()        , MSVC );
        TEST(      IW&   (             ),       IWD&  ()        , true );
        TEST(      IW&   (             ), const IWD   ()        , false);
        TEST(      IW&   (             ), const IWD&  ()        , false);
        TEST(const IW    (             ),       IWD   ()        , true );
        TEST(const IW    (             ),       IWD&  ()        , true );
        TEST(const IW    (             ), const IWD   ()        , true );
        TEST(const IW    (             ), const IWD&  ()        , true );
        TEST(const IW&   (             ),       IWD   ()        , true );
        TEST(const IW&   (             ),       IWD&  ()        , true );
        TEST(const IW&   (             ), const IWD   ()        , true );
        TEST(const IW&   (             ), const IWD&  ()        , true );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(      IW    (             ),       IW&&  ()        , true );
        TEST(      IW    (             ), const IW&&  ()        , true );
        TEST(      IW&   (             ),       IW&&  ()        , MSVC );
        TEST(      IW&   (             ), const IW&&  ()        , false);
        TEST(      IW&   (             ),       IW&&  ()        , MSVC );
        TEST(      IW&   (             ), const IW&&  ()        , false);
        TEST(const IW    (             ),       IW&&  ()        , true );
        TEST(const IW    (             ), const IW&&  ()        , true );
        TEST(const IW&   (             ),       IW&&  ()        , true );
        TEST(const IW&   (             ), const IW&&  ()        , true );
        TEST(      IW&&  (             ),       IW    ()        , true );
        TEST(      IW&&  (             ),       IW&   ()        , true );
        TEST(      IW&&  (             ), const IW    ()        , false);
        TEST(      IW&&  (             ), const IW&   ()        , false);
        TEST(const IW&&  (             ),       IW    ()        , true );
        TEST(const IW&&  (             ),       IW&   ()        , true );
        TEST(const IW&&  (             ), const IW    ()        , true );
        TEST(const IW&&  (             ), const IW&   ()        , true );
        TEST(      IW    (             ),       IWD&& ()        , true );
        TEST(      IW    (             ), const IWD&& ()        , true );
        TEST(      IW&   (             ),       IWD&& ()        , MSVC );
        TEST(      IW&   (             ), const IWD&& ()        , false);
        TEST(const IW    (             ),       IWD&& ()        , true );
        TEST(const IW    (             ), const IWD&& ()        , true );
        TEST(const IW&   (             ),       IWD&& ()        , true );
        TEST(const IW&   (             ), const IWD&& ()        , true );
        TEST(      IW&&  (             ),       IWD   ()        , true );
        TEST(      IW&&  (             ),       IWD&  ()        , true );
        TEST(      IW&&  (             ), const IWD   ()        , false);
        TEST(      IW&&  (             ), const IWD&  ()        , false);
        TEST(const IW&&  (             ),       IWD   ()        , true );
        TEST(const IW&&  (             ),       IWD&  ()        , true );
        TEST(const IW&&  (             ), const IWD   ()        , true );
        TEST(const IW&&  (             ), const IWD&  ()        , true );
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Verify that 'bsl::function<PROTO>' is constructible from types that
        // are implicitly convertible to 'bsl::function<PROTO>'.  Note this
        // should should be the case for all 'PROTO' types that are function
        // types without const, volatile, or reference qualifiers.  Further
        // note that this "test vector" verifies that {DRQS 138769521} has been
        // addressed.
        TEST(PROTO, ConvertibleToObj, true);
#undef TEST

#endif  // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

        // Concern 15: Construction from an empty 'bsl::function' argument of a
        // different (but compatible) 'bsl::function' instantiation, yields an
        // empty 'function' (it does not wrap the empty 'bsl::function').
        // Empty 'bsl::function' with different prototype.  This concern is
        // tested indirectly in the table-driven approach above, but a clear,
        // direct test seems called for.
        ASSERT(isNull(emptyInnerFunction));
        Obj emptyCopy(emptyInnerFunction);
        ASSERT(!emptyCopy);

        // Concern 17: Construction from a function that has a default argument
        // is possible despite defects in certain compilers.
        {
            bsl::function<void(int)> f(functionWithDefaultArgument);
            bsl::function<void(int)> g(&functionWithDefaultArgument);
        }
#endif  // BSLSTL_FUNCTION_TEST_CASE05_PART_2

#ifdef BSLSTL_FUNCTION_TEST_CASE05_PART_1
#undef BSLSTL_FUNCTION_TEST_CASE05_PART_1
#else
#undef BSLSTL_FUNCTION_TEST_CASE05_PART_2
#endif

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_02_OR_03
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT AND 'nullptr' CONSTRUCTORS
        //
        // Concerns:
        //: 1 Constructing a 'function' without specifying a callable object
        //:   argument or specifying a null pointer constant for the target
        //:   argument yields an empty 'function' object.
        //:
        //: 2 If no allocator is specified or if 'alloc' is a null pointer of
        //:   type convertible to 'bslma::Allocator *', then the
        //:   'get_allocator' accessor will return a default-constructed
        //:   'bsl::allocator<char>' and the 'allocator' accessor will return
        //:   a pointer to the default allocator.
        //:
        //: 3 If 'alloc' is constructed from a non-null pointer to a
        //:   'bslma::Allocator' object, then the 'get_allocator' accessor
        //:   will return that allocator and the 'allocator' accessor will
        //:   return a pointer to the 'bslma::Allocator' object
        //:
        //: 4 No memory is allocated by any of these constructors.
        //
        // Plan:
        //: 1 For concern 1 construct a 'function' object with each of these
        //:   constructors and verify that it converts to a Boolean false
        //:   value.
        //:
        //: 2 For concern 2, verify that the objects constructed with no
        //:   allocator or with a null pointer for the allocator return the
        //:   default allocator when queried using 'get_allocator' or
        //:   'allocator'.
        //:
        //: 3 For concern 3, construct a 'function' object with the address of
        //:   a 'bslma:TestAllocator'.  Verify that 'get_allocator' and
        //:   'allocator' return the expected results.
        //:
        //: 4 For concern 4, test that no memory is consumed by the specified
        //:   test allocator or default test allocator for any of these
        //:   constructor invocations.
        //
        // Testing
        //      function();
        //      function(nullptr_t);
        //      function(allocator_arg_t, const allocator_type& a);
        //      function(allocator_arg_t, const allocator_type& a, nullptr_t);
        //      allocator_type get_allocator() const;
        //      BloombergLP::bslma::Allocator *allocator() const noexcept;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING DEFAULT AND 'nullptr' CONSTRUCTORS"
                   "\n==========================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_01
        bslma::TestAllocatorMonitor defaultAllocMonitor(&defaultTestAllocator);

        if (veryVerbose) printf("with no allocator specified\n");
        {
            defaultAllocMonitor.reset();
            bsl::function<int(float)> f1;
            ASSERT(f1 ? false : true);  // Succeed if empty
            ASSERT(bsl::allocator<char>() == f1.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&defaultTestAllocator == f1.allocator());
#endif
            ASSERT(defaultAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(0);
            ASSERT(f2 ? false : true);  // Succeed if empty
            ASSERT(bsl::allocator<char>() == f2.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&defaultTestAllocator == f2.allocator());
#endif
            ASSERT(defaultAllocMonitor.isInUseSame());

            // Null allocator pointer behaves as though allocator were not
            // specified (Bloomberg extension)
            bslma::TestAllocator *nullTa_p = 0;

            bsl::function<int(float)> f3(bsl::allocator_arg, nullTa_p);
            ASSERT(! f3);
            ASSERT(bsl::allocator<char>() == f3.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&defaultTestAllocator == f3.allocator());
#endif
            ASSERT(defaultAllocMonitor.isInUseSame());

            bsl::function<int(float)> f4(bsl::allocator_arg, nullTa_p, 0);
            ASSERT(! f4);
            ASSERT(bsl::allocator<char>() == f4.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&defaultTestAllocator == f4.allocator());
#endif
            ASSERT(defaultAllocMonitor.isInUseSame());
        }

        bslma::TestAllocator ta;

        if (veryVerbose) printf("with bsl::allocator<T>\n");
        {
            defaultAllocMonitor.reset();
            bsl::allocator<void*> alloc(&ta);

            bsl::function<int(float)> f(bsl::allocator_arg, alloc);
            ASSERT(! f);
            ASSERT(alloc == f.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&ta == f.allocator());
#endif
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(defaultAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                         bsl::nullptr_t());
            ASSERT(! f2);
            ASSERT(alloc == f2.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&ta == f2.allocator());
#endif
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(defaultAllocMonitor.isInUseSame());
        }

        if (veryVerbose) printf("with bslma::Allocator*\n");
        {
            defaultAllocMonitor.reset();
            bsl::function<int(float)> f(bsl::allocator_arg, &ta);
            ASSERT(! f);
            ASSERT(bsl::allocator<char>(&ta) == f.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&ta == f.allocator());
#endif
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(defaultAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(bsl::allocator_arg, &ta, 0);
            ASSERT(! f2);
            ASSERT(bsl::allocator<char>(&ta) == f2.get_allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&ta == f2.allocator());
#endif
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(defaultAllocMonitor.isInUseSame());
        }

#ifdef DRQS_164241820_FIXED
        // This test is known to fail (see DRQS 164241820) but, being an
        // obscure corner case, it can be fixed at our leisure, at which point
        // we will re-enable the test.

        if (veryVerbose) {
            printf("Construct with compatible empty 'function' object\n");
        }
        defaultAllocMonitor.reset();
        {
            bsl::function<int()>  intFunc;
            bsl::function<void()> f(intFunc);
            const bsl::function<void()>& F = f;
            ASSERT(! F);
            ASSERT(defaultAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(0 == F.target<bsl::function<int()> >());
            ASSERT(0 == f.target<bsl::function<int()> >());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&defaultTestAllocator == f.allocator());
#endif
        }
        ASSERT(defaultAllocMonitor.isInUseSame());
#endif  // DRQS_164241820_FIXED

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_01
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'TrackableValue' TEST INFRASTRUCTURE
        //
        // Concerns:
        //: 1 A 'TrackableValue' constructed from an integer returns the
        //:   correct integer value, with tracking information showing that it
        //:   has not been copied into, has not been moved into, and has not
        //:   been moved from.
        //:
        //: 2 Copy-constructing a 'TrackableValue' produces a copy that is
        //:   identical to the original except that 'isCopied()' returns true
        //:   and 'isMoved()' returns false. The original is not modified in
        //:   any way.  Equality comparison shows that they are equal.
        //:
        //: 3 Move-constructing a 'TrackableValue' produces an object that has
        //:   the previous value and 'isCopied' state of the original, with
        //:   'isMoved' flag set to true.  The original is set to the
        //:   moved-from state with both the 'isCopied' and 'isMoved' flags
        //:   set to false.
        //:
        //: 4 A copy-constructed object of type 'TrackableValue' has the
        //:   'isMoved' flag false, even if the original was moved-to.
        //:
        //: 5 Copy-constructing or move-constructing a moved-from
        //:   'TrackableValue' yields an object that is not considered
        //:   moved-from ('isMovedFrom()' returns false.
        //:
        //: 6 Assigning a 'TrackableValue' from an integer produces identical
        //:   result as though the lhs were constructed from that integer.
        //:
        //: 7 Copy-assigning a 'TrackableValue' produces the same result as
        //:   as though the lhs were copy-constructed from the same rhs
        //:   object.
        //:
        //: 8 Move-assigning a 'TrackableValue' produces the same result as
        //:   as though the lhs were move-constructed from the same rhs
        //:   object.
        //:
        //: 9 Swapping 'TrackableValue's swaps both the values and the
        //:   'isCopied' flag.  The 'isMoved' flag is set true for both
        //:   objects after the swap.
        //:
        //: 10 The 'resetMoveCopiedFlags' method resets the move and copied
        //:   flags of a 'TrackableValue' to false.
        //:
        //: 11 The global 'isMoved', 'isCopied', and 'isMovedFrom' functions
        //:   have the same meaning for trackable types as the corresponding
        //:   member functions.  The second argument is ignored in this case.
        //:
        //: 12 The global 'isMoved', 'isCopied', and 'isMovedFrom' functions
        //:   are noops for non-trackable types.  The second argument is
        //:   returned in this case.
        //:
        //: 13 The global 'resetMoveCopiedFlags' has the same meaning for
        //:   trackable types as the corresponding member function and is a
        //:   no-op for non-trackable types.
        //
        // Plan:
        //: 1 For concern 1, create a 'TrackableValue' from an integer and
        //:   verify that 'value()' returns that integer and that 'isMoved()',
        //:   'isCopied()', and 'isMovedFrom()' all return false.
        //:
        //: 2 For concern 2, copy-construct a 'TrackableValue
        //:   and verify all of the copy's attributes are identical to the
        //:   original except that 'isCopied()' returns true.  Verify that the
        //:   original and copy compare equal and do not compare unequal.
        //:
        //: 3 For concern 3, move-construct a newly-created 'TrackableValue'
        //:   and verify all of the resulting object's attributes are
        //:   identical to the original except that 'isCopied()' returns true.
        //:   Verify that, after the move, the original is in a moved-from
        //:   state.  Repeat this test, but move-construct from a copy of a
        //:   newly-created object, and verify that the new moved-to object
        //:   has both the 'isMoved' and 'isCopied' flags set to true.
        //:
        //: 4 For concern 4, construct a copy of a moved-from object and verify
        //:   that the 'isMoved' flag is false for the copy.
        //:
        //: 5 For concern 5, move-construct a 'TrackableValue', then
        //:   make a copy of the moved-from object using copy
        //:   construction. Verify that 'isMovedFrom()' is false for the
        //:   copy. Repeat this step using move construction instead of copy
        //:   construction.
        //:
        //: 6 For concern 6, repeat step 1 except instead of constructing from
        //:   an integer, default-construct the 'TrackableValue' then assign
        //:   from the integer.
        //:
        //: 7 For concern 7, repeat steps 2 and 4 except instead of
        //:   copy-construction, use default-construction of the
        //:   'TrackableValue' then copy-assign from another 'TrackableValue
        //:   object.
        //:
        //: 8 For concern 8, repeat step 4 except instead of
        //:   move-construction, use default-construction the 'TrackableValue'
        //:   then move-assign from another 'TrackableValue object.
        //:
        //: 9 For concern 9, create two 'TrackableValue' objects, one by
        //:   direct construction and one by copy construction.  Swap them
        //:   using the 'TrackableValue::swap' member function and verify that
        //:   their values and 'isCopied' flags were swapped and that
        //:   'isMoved' returns true for both values.  Swap the values back
        //:   using non-member 'swap' and verify again that their values and
        //:   'isCopied' flags are swapped and that both have true values for
        //:   'isMoved'.
        //:
        //: 10 For concern 10, call 'resetMoveCopiedFlags' on 'TrackableValue'
        //:    objects with each combination of 'isMoved' and 'isCopied' flags
        //:    and verify that 'isMoved' and 'isCopied' return false
        //:    afterwards.
        //:
        //: 11 For concern 11, create 'TrackableValue' objects with each
        //:    combination of 'isMoved', 'isCopied', and 'isMovedFrom' flags.
        //:    Verify that the global functions have the same meaning as the
        //:    member functions regardless of the second argument.
        //:
        //: 12 For concern 12, create non-'TrackableValue' objects of various
        //:    types likely to be used in tests.  Verify that the global
        //:    'isMoved', 'isCopied', and 'isMovedFrom' functions return their
        //:    second argument.
        //:
        //: 13 For concern 13, call global 'resetMoveCopiedFlags' on the
        //:    objects created in steps 10-13 and verify that global 'isMoved'
        //:    and 'isCopied' functions return false for all of these objects
        //:    if the second argument is false.
        //
        // TESTING
        //   'TrackableValue' TEST INFRASTRUCTURE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'TrackableValue TEST INFRASTRUCTURE"
                            "\n===========================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_01
        // Construction from int
        {
            TrackableValue nine(9);
            ASSERT(9 == nine.value());
            ASSERT(! nine.isCopied());
            ASSERT(! nine.isMoved());
            ASSERT(! nine.isMovedFrom());

            nine.resetMoveCopiedFlags();
            ASSERT(! nine.isCopied());
            ASSERT(! nine.isMoved());
        }

        // Copy construction
        {
            TrackableValue ninea(9);
            TrackableValue nineb(ninea);
            ASSERT(9 == ninea.value());
            ASSERT(! ninea.isCopied());
            ASSERT(! ninea.isMoved());
            ASSERT(! ninea.isMovedFrom());
            ASSERT(9 == nineb.value());
            ASSERT(  nineb.isCopied());
            ASSERT(! nineb.isMoved());
            ASSERT(! nineb.isMovedFrom());
            ASSERT(   ninea == nineb);
            ASSERT(! (ninea != nineb));

            nineb.resetMoveCopiedFlags();
            ASSERT(! nineb.isCopied());
            ASSERT(! nineb.isMoved());
        }

        // Move construction
        {
            TrackableValue eighta(8);
            TrackableValue eightb(bslmf::MovableRefUtil::move(eighta));
            ASSERT(8 != eighta.value());
            ASSERT(TrackableValue(8) != eighta);
            ASSERT(! eighta.isMoved());
            ASSERT(! eighta.isCopied());
            ASSERT(  eighta.isMovedFrom());
            ASSERT(TrackableValue(8) == eightb);
            ASSERT(8 == eightb.value());
            ASSERT(  eightb.isMoved());
            ASSERT(! eightb.isCopied());
            ASSERT(! eightb.isMovedFrom());

            // Move-construction from a copied-to object
            TrackableValue eightc(eightb);
            ASSERT(! eightc.isMoved());
            ASSERT(  eightc.isCopied());
            ASSERT(! eightc.isMovedFrom());
            TrackableValue eightd(bslmf::MovableRefUtil::move(eightc));
            ASSERT(eightc.isMovedFrom());
            ASSERT(8 == eightd.value());
            ASSERT(eightd == eightb);
            ASSERT(  eightd.isMoved());
            ASSERT(  eightd.isCopied());
            ASSERT(! eightd.isMovedFrom());

            eightb.resetMoveCopiedFlags();
            ASSERT(! eightb.isCopied());
            ASSERT(! eightb.isMoved());
            eightd.resetMoveCopiedFlags();
            ASSERT(! eightd.isCopied());
            ASSERT(! eightd.isMoved());
        }

        // Copy-construction from a moved-to object
        {
            TrackableValue eighta(8);
            TrackableValue eightb(bslmf::MovableRefUtil::move(eighta));
            TrackableValue eightc(eightb);
            ASSERT(8 == eightc.value());
            ASSERT(eightc == eightb);
            ASSERT(! eightc.isMoved());  // Copy is not moved-to
            ASSERT(  eightc.isCopied());
            ASSERT(! eightc.isMovedFrom());

            eightc.resetMoveCopiedFlags();
            ASSERT(! eightc.isCopied());
            ASSERT(! eightc.isMoved());
        }

        // Copy assignment
        {
            // From clean (uncopied) object
            TrackableValue nine(9);
            TrackableValue lhs;
            ASSERT(0 == lhs.value());

            lhs = nine;  // COPY ASSIGNMENT
            ASSERT(9 == nine.value());
            ASSERT(! nine.isCopied());
            ASSERT(! nine.isMoved());
            ASSERT(! nine.isMovedFrom());
            ASSERT(9 == lhs.value());
            ASSERT(  lhs.isCopied());
            ASSERT(! lhs.isMoved());
            ASSERT(! lhs.isMovedFrom());
            ASSERT(nine == lhs);

            // From a moved-from object
            TrackableValue eighta(8);
            TrackableValue eightb(bslmf::MovableRefUtil::move(eighta));
            lhs = eightb;  // COPY ASSIGNMENT
            ASSERT(8 == lhs.value());
            ASSERT(lhs == eightb);
            ASSERT(! lhs.isMoved());  // Copy is not moved-to
            ASSERT(  lhs.isCopied());
            ASSERT(! lhs.isMovedFrom());
        }

        // Move assignment
        {
            // From clean rhs
            TrackableValue eighta(8);
            TrackableValue lhs;
            lhs = bslmf::MovableRefUtil::move(eighta); // MOVE ASSIGNMENT
            ASSERT(8 != eighta.value());
            ASSERT(TrackableValue(8) != eighta);
            ASSERT(! eighta.isMoved());
            ASSERT(! eighta.isCopied());
            ASSERT(  eighta.isMovedFrom());
            ASSERT(TrackableValue(8) == lhs);
            ASSERT(8 == lhs.value());
            ASSERT(  lhs.isMoved());
            ASSERT(! lhs.isCopied());
            ASSERT(! lhs.isMovedFrom());

            // Move-assignment from a copied-to object
            TrackableValue eightb(8);
            TrackableValue eightc(eightb);
            ASSERT(eightc.isCopied());
            lhs = bslmf::MovableRefUtil::move(eightc); // MOVE ASSIGNMENT
            ASSERT(eightc.isMovedFrom());
            ASSERT(8 == lhs.value());
            ASSERT(lhs == eightb);
            ASSERT(  lhs.isMoved());
            ASSERT(  lhs.isCopied());
            ASSERT(! lhs.isMovedFrom());

            MediumFunctor mfa(66);
            MediumFunctor mfb(mfa);
            MediumFunctor mflhs(0);
            ASSERT(! isCopied(mfa, true));
            ASSERT(  isCopied(mfb, false));
            mflhs = bslmf::MovableRefUtil::move(mfb); // MOVE ASSIGNMENT
            ASSERT(66 == mflhs.value());
            ASSERT(  isMoved(mflhs, false));
            ASSERT(  isCopied(mflhs, false));
            ASSERT(! isMovedFrom(mflhs, true));
            ASSERT(  isMovedFrom(mfb, false));
        }

        // Swap
        {
            TrackableValue twelve(12);
            TrackableValue a(11), b(twelve);
            // 'b' is copied-to, 'a' is not.
            ASSERT(! a.isCopied());
            ASSERT(  b.isCopied());
            ASSERT(! a.isMoved());
            ASSERT(! b.isMoved());
            ASSERT(! a.isMovedFrom());
            ASSERT(! b.isMovedFrom());
            a.swap(b);
            ASSERT(11    == b.value());
            ASSERT(12    == a.value());
            ASSERT(! b.isCopied());
            ASSERT(  a.isCopied());
            ASSERT(  b.isMoved());
            ASSERT(  a.isMoved());
            ASSERT(! b.isMovedFrom());
            ASSERT(! a.isMovedFrom());

            using namespace std;  // Bring 'std::swap' into scope.
            b = 11;  // Turn off the 'isMoved' flag
            ASSERT(! b.isCopied());
            ASSERT(! b.isMoved());
            swap(a, b);
            ASSERT(11    == a.value());
            ASSERT(12    == b.value());
            ASSERT(! a.isCopied());
            ASSERT(  b.isCopied());
            ASSERT(  a.isMoved());
            ASSERT(  b.isMoved());
            ASSERT(! a.isMovedFrom());
            ASSERT(! b.isMovedFrom());

            // Test 'setIsCopiedRaw', 'setIsMovedRaw', and 'setValueRaw'
            a.setIsCopiedRaw(true);
            ASSERT(11 == a.value());
            ASSERT(  a.isCopied());
            ASSERT(  a.isMoved());
            ASSERT(! a.isMovedFrom());
            a.setIsCopiedRaw(false);
            ASSERT(11 == a.value());
            ASSERT(! a.isCopied());
            ASSERT(  a.isMoved());
            ASSERT(! a.isMovedFrom());

            b.setIsMovedRaw(false);
            ASSERT(12 == b.value());
            ASSERT(  b.isCopied());
            ASSERT(! b.isMoved());
            ASSERT(! b.isMovedFrom());
            b.setIsMovedRaw(true);
            ASSERT(12 == b.value());
            ASSERT(  b.isCopied());
            ASSERT(  b.isMoved());
            ASSERT(! b.isMovedFrom());

            b.setValueRaw(13);
            ASSERT(13 == b.value());
            ASSERT(  b.isCopied());
            ASSERT(  b.isMoved());
            ASSERT(! b.isMovedFrom());
        }

        IntWrapper     w;
        TrackableValue a;
        TrackableValue b(a);
        TrackableValue c(bslmf::MovableRefUtil::move(a));

        // Non-member 'isMoved'
        ASSERT(! isMoved(4, false)); // int
        ASSERT(  isMoved(4, true));
        ASSERT(! isMoved(w, false)); // 'IntWrapper'
        ASSERT(  isMoved(w, true));
        ASSERT(! isMoved(&w, false)); // ptr-to-obj
        ASSERT(  isMoved(&w, true));
        ASSERT(! isMoved(&simpleFunc, false)); // ptr-to-func
        ASSERT(  isMoved(&simpleFunc, true));
        ASSERT(! isMoved(&IntWrapper::value, false)); // ptr-to-mem-func
        ASSERT(  isMoved(&IntWrapper::value, true));
        ASSERT(! isMoved(a, false));
        ASSERT(! isMoved(a, true));
        ASSERT(! isMoved(b, false));
        ASSERT(! isMoved(b, true));
        ASSERT(  isMoved(c, false));
        ASSERT(  isMoved(c, true));

        // Non-member 'isCopied'
        ASSERT(! isCopied(4, false)); // int
        ASSERT(  isCopied(4, true));
        ASSERT(! isCopied(w, false)); // 'IntWrapper'
        ASSERT(  isCopied(w, true));
        ASSERT(! isCopied(&w, false)); // ptr-to-obj
        ASSERT(  isCopied(&w, true));
        ASSERT(! isCopied(&simpleFunc, false)); // ptr-to-func
        ASSERT(  isCopied(&simpleFunc, true));
        ASSERT(! isCopied(&IntWrapper::value, false)); // ptr-to-mem-func
        ASSERT(  isCopied(&IntWrapper::value, true));
        ASSERT(! isCopied(a, false));
        ASSERT(! isCopied(a, true));
        ASSERT(  isCopied(b, false));
        ASSERT(  isCopied(b, true));
        ASSERT(! isCopied(c, false));
        ASSERT(! isCopied(c, true));

        // Non-member 'isMovedFrom'
        ASSERT(! isMovedFrom(4, false)); // int
        ASSERT(  isMovedFrom(4, true));
        ASSERT(! isMovedFrom(w, false)); // 'IntWrapper'
        ASSERT(  isMovedFrom(w, true));
        ASSERT(! isMovedFrom(&w, false)); // ptr-to-obj
        ASSERT(  isMovedFrom(&w, true));
        ASSERT(! isMovedFrom(&simpleFunc, false)); // ptr-to-func
        ASSERT(  isMovedFrom(&simpleFunc, true));
        ASSERT(! isMovedFrom(&IntWrapper::value, false)); // ptr-to-mem-func
        ASSERT(  isMovedFrom(&IntWrapper::value, true));
        ASSERT(  isMovedFrom(a, false));
        ASSERT(  isMovedFrom(a, true));
        ASSERT(! isMovedFrom(b, false));
        ASSERT(! isMovedFrom(b, true));
        ASSERT(! isMovedFrom(c, false));
        ASSERT(! isMovedFrom(c, true));

        // Non-member 'resetMoveCopiedFlags'
        resetMoveCopiedFlags(&w); // no-op
        ASSERT(! isMoved(&w, false));
        ASSERT(! isCopied(&w, false));
        resetMoveCopiedFlags(&c);
        ASSERT(! isMoved(c, false));
        ASSERT(! isCopied(c, false));
        resetMoveCopiedFlags(&b);
        ASSERT(! isMoved(b, false));
        ASSERT(! isCopied(b, false));
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_01
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE CONSTRUCTORS (BOOTSTRAP) AND BASIC ACCESSORS
        //   To bootstrap testing of 'function', we start by confirming that
        //   the constructors put the 'function' object in the correct state
        //   as observed using the basic accessors.  This test does not check
        //   memory allocation, allocator management, or invocation behavior.
        //
        // Concerns:
        //: 1 Default construction and construction using a null-pointer
        //:   literal (0 or 'nullptr_t()') create an empty 'function'
        //:   object. 'operator bool' will return false.
        //:
        //: 2 'target_type' returns 'typeid(void)' for empty function objects.
        //:
        //: 3 'target' returns a null pointer for empty function objects.
        //:
        //: 4 For C++11, the default and 'nullptr_t' constructors (and the
        //:   destructor) have a non-throwing exception specification. The
        //:   constructors taking a run-time pointer argument do not have a
        //:   non-throwing exception specification (i.e., they are allowed to
        //:   throw).
        //:
        //: 5 Construction with a non-null pointer to function, non-null
        //:   pointer to member function, non-null pointer to member data, or
        //:   functor object creates a non-empty 'function' object.
        //:   'operator bool' will return true.
        //:
        //: 6 For a non-empty 'function', the 'target_type' accessor returns
        //:   the 'type_info' of the callable type specified at construction.
        //:
        //: 7 For a non-empty 'function' constructed with non-null 'func'
        //:   argument of type 'FUNC', the 'target<FUNC>' method returns a
        //:   pointer of type 'FUNC*' to an object that compares equal to
        //:   'func'.  If the 'function' is const, then the returned pointer is
        //:   of type 'const FUNC*'.  If the template argument to 'target' is
        //:   other than 'FUNC', 'target<T>()' returns a null pointer to 'T'.
        //:
        //: 8 If constructed with a callable object wrapped using
        //:   'bslalg::NothrowMovableWrapper<FUNC>', 'target_type' and
        //:   'target<FUNC>' behave as though the argument were not wrapped,
        //:   i.e., returning the unwrapped type ('FUNC') and unwrapped
        //:   callable object.
        //:
        //: 9 (Non concern) The semantics and implementation of the
        //:   operations being tested here are independent of the function
        //:   prototype.  It is therefore not necessary to tests with multiple
        //:   different prototypes. (Many different prototypes are tested in
        //:   the invocation tests.)
        //
        // Plan:
        //: 1 For concern 1, default-construct a 'function' object and
        //:   construct a second 'function' object using a 'nullptr' argument.
        //:   In both cases, verify that the resulting 'function' objects
        //:   evaluate to false in a boolean context, i.e., the 'function'
        //:   objects are empty.
        //:
        //: 2 For concern 2, verify for each empty 'function' in step 1 that
        //:   the 'target_type' accessor returns 'typeid(void)'.
        //:
        //: 3 For concern 3, verify for each empty 'function' in step 1 that
        //:   the 'target<nullptr_t>' accessor returns a null pointer. In the
        //:   case of a const 'function', verify that the returned pointer is
        //:   a pointer-to-const.
        //:
        //: 4 For concern 4, use 'noexcept(Obj())', 'noexcept(Obj(0))',
        //:   'noexcept(Obj(func-ptr))' and 'noexcept(Obj(mem-func-ptr))' to
        //:   test the exception specification of the empty-function
        //:   constructors.  Verify that these expressions evaluate to true
        //:   for the 'Obj()' and 'Obj(0)' cases and false for the others.
        //:   To avoid testing in a pre-C++11 compiler, wrap these tests in
        //:   the 'ASSERT_NOEXCEPT' macro, which is a no-op for compilers that
        //:   don't support 'noexcept'.
        //:
        //: 5 For concern 5, construct 'function' objects with a non-null
        //:   pointer to function, a non-null pointer to member function, (TBD,
        //:   pointer-to-data-member) and a functor object.  Verify that the
        //:   resulting objects evaluate to true in a boolean context.
        //:
        //: 6 For concern 6, verify that the return value of 'target_type'
        //:   matches the expected 'type_info' for the objects created in step
        //:   5.
        //:
        //: 7 For concern 7, verify that, for the 'function' objects created in
        //:   step 5, the return value of 'target<FUNC>' is a non-null pointer
        //:   pointing to a value that compares equal to the callable argument
        //:   used to construct the 'function' object.  Verify that returned
        //:   value of 'target' is a pointer-to-const iff the 'function' is
        //:   const.  Also invoke 'target<T>' for 'T' different than 'FUNC' and
        //:   confirm that it returns a null pointer.
        //:
        //: 8 For concern 8, wrap each callable object from step 5 in a
        //:   'bslalg::NothrowMovableWrapper' and repeat steps 6 and 7,
        //:   verifying that the behavior is the same as if the wrapper were
        //:   not used.
        //
        // Testing:
        //  PRIMITIVE CONSTRUCTORS (BOOTSTRAP)
        //  operator bool() const;
        //  const typeinfo& target_type() const;
        //  T      * target<T>();
        //  T const* target<T>() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMITIVE CONSTRUCTORS (BOOTSTRAP) "
                            "AND BASIC ACCESSORS"
                            "\n==========================================="
                            "===================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_01
        // Steps 1-4 of the test plan:

        if (veryVerbose) printf("Construct with no arguments\n");
        {
            Obj f; const Obj& F = f;
            ASSERT(F ? false : true);  // Evaluate F in a boolean context
            ASSERT(typeid(void) == F.target_type());
            ASSERT(0 == F.target<bsl::nullptr_t>());
            ASSERT(0 == f.target<bsl::nullptr_t>());
            ASSERT(  isConstPtr(F.target<bsl::nullptr_t>()));
            ASSERT(! isConstPtr(f.target<bsl::nullptr_t>()));
            ASSERT_NOEXCEPT(true, Obj());
        }

        if (veryVerbose) printf("Construct with 'nullptr' literal argument\n");
        {
            Obj f(0); const Obj& F = f;
            ASSERT(F ? false : true);  // Evaluate F in a boolean context
            ASSERT(typeid(void) == F.target_type());
            ASSERT(0 == F.target<bsl::nullptr_t>());
            ASSERT(0 == f.target<bsl::nullptr_t>());
            ASSERT_NOEXCEPT(true, Obj(bsl::nullptr_t()));
        }

        // Steps 5-8 of the test plan:

        // The 'testBasicAccessors' function template implements steps 5 - 8 of
        // the test plan.  Each call to the 'TEST' macro test a different
        // callable type.
#define TEST(OBJ) do {                                                     \
            if (veryVerbose) printf("Construct with %s argument\n", #OBJ); \
            testBasicAccessors(OBJ);                                       \
        } while (false)

        TEST(simpleFunc       );  // Pointer to function
        TEST(&IntWrapper::add1);  // Pointer to member function
        TEST(EmptyFunctor()   );  // Functor
        TEST(SmallFunctor(1)  );  // Functor
        TEST(MediumFunctor(2) );  // Functor
        TEST(LargeFunctor(3)  );  // Functor
#undef TEST

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_01
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Construct 'function' objects wrapping a small variety of
        //:   functors.
        //:
        //: 2 Verify that default-constructed 'function' objects are empty
        //:   (evaluate to false in a boolean context) and that 'function'
        //:   objects constructed with a non-null callable argument are not
        //:   empty (evaluate to true in a boolean context).
        //:
        //: 3 Verify that 'target_type' and 'target' return the type of the
        //:   callable object and a pointer to the callable object,
        //:   respectively.
        //:
        //: 3 Invoke non-empty functors and verify expected results.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_01
        if (veryVerbose) printf("Wrap pointer-to-function 'int(*)()'\n");
        {
            typedef bsl::function<int()> Obj;
            Obj n;
            ASSERT(! n);
            ASSERT(typeid(void) == n.target_type());
            ASSERT(0 == n.target<int(*)()>());

            Obj f(sum0); const Obj& F = f;
            ASSERT(0x4000 == F());  // invoke
            ASSERT(typeid(&sum0) == F.target_type());
            ASSERT(&sum0 == *f.target<int(*)()>());
            ASSERT(&sum0 == *F.target<int(*)()>());
            ASSERT(0 == F.target<int(*)(int)>());

            int (*sum0Ptr)() = sum0;
            Obj g(bslmf::MovableRefUtil::move(sum0Ptr)); const Obj& G = g;
            ASSERT(0x4000 == G());  // invoke
            ASSERT(typeid(&sum0) == G.target_type());
            ASSERT(&sum0 == *g.target<int(*)()>());
            ASSERT(&sum0 == *G.target<int(*)()>());
            ASSERT(0 == G.target<int(*)(int)>());
        }

        if (veryVerbose)
            printf("Wrap pointer-to-function 'int(*)(int, int)'\n");
        {
            typedef bsl::function<int(int, int)> Obj;
            Obj                                  f(sum2);
            ASSERT(0x4003 == f(1, 2));
            ASSERT(typeid(&sum2) == f.target_type());
            ASSERT(&sum2 == *f.target<int(*)(int, int)>());

            int (*sum2Ptr)(int, int) = sum2;
            Obj g(bslmf::MovableRefUtil::move(sum2Ptr));
            ASSERT(0x4003 == g(1, 2));
            ASSERT(typeid(&sum2) == g.target_type());
            ASSERT(&sum2 == *g.target<int(*)(int, int)>());
        }

        if (veryVerbose) printf("Wrap pointer-to-member-function\n");
        {
            IntWrapper iw(0x4000), *iw_p = &iw; const IntWrapper& IW = iw;

            if (veryVerbose) printf("Wrap null void (IntWrapper::*)(int)\n");
            void (IntWrapper::*nullMember_p)(int) = 0;
            bsl::function<void(IntWrapper&, int)> nullf(nullMember_p);
            ASSERT(! nullf);

            if (veryVerbose) printf("Wrap void (IntWrapper::*)()\n");
            bsl::function<void(IntWrapper&)> f0(&IntWrapper::incrementBy1);
            ASSERT(f0);
            f0(iw);
            ASSERT(0x4001 == iw.value());

            if (veryVerbose) printf("Wrap void (IntWrapper::*)(int, int)\n");
            bsl::function<void(IntWrapper*, int, int)>
                f3(&IntWrapper::increment2);
            ASSERT(f3);
            f3(iw_p, 2, 4);
            ASSERT(0x4007 == iw_p->value());

            if (veryVerbose) printf("Wrap int (IntWrapper::*)(int) const\n");
            bsl::function<PROTO> fv(&IntWrapper::add1);
            ASSERT(fv);
            ASSERT(0x400f == fv(IW, 8));
        }

        if (veryVerbose) printf("Wrap functor\n");
        {
            IntWrapper iw1(0x4000); const IntWrapper& IW1 = iw1;

            bsl::function<int()> f0(IW1);
            ASSERT(f0);
            ASSERT(0x4000 == f0());
            ASSERT(typeid(IntWrapper) == f0.target_type());
            ASSERT(0x4000 == f0.target<IntWrapper>()->value());

            bsl::function<int(int)> f1(IW1);
            ASSERT(f1);
            ASSERT(0x4002 == f1(2));
            ASSERT(typeid(IntWrapper) == f1.target_type());
            ASSERT(0x4000 == f1.target<IntWrapper>()->value());

            bsl::function<int()> f2(IntWrapper(0x4000));
            ASSERT(f2);
            ASSERT(0x4000 == f2());
            ASSERT(typeid(IntWrapper) == f2.target_type());
            ASSERT(0x4000 == f2.target<IntWrapper>()->value());

            IntWrapper iw3(0x4000);
            bsl::function<int()> f3(bslmf::MovableRefUtil::move(iw3));
            ASSERT(f3);
            ASSERT(0x4000 == f3());
            ASSERT(typeid(IntWrapper) == f3.target_type());
            ASSERT(0x4000 == f3.target<IntWrapper>()->value());
        }

        if (veryVerbose)
            printf("Basic reference_wrapped function assignment\n");
        {
#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            bsl::function<void()> f1(voidFunc);
            bsl::function<void()> f2;
            f2 = bsl::ref(f1);
            ASSERT(typeid(bsl::reference_wrapper<bsl::function<void()> >) ==
                   f2.target_type());
#else
            if (veryVerbose)
                printf("Skipping basic reference_wrapped function assignment "
                       "as this is not supported in C++03\n");
#endif
        }
#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_01
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING FAIL-TO-COMPILE CONCERNS
        //   There are some constructs that might accidentally compile, but are
        //   not valid.  This test, guarded by a macro, confirms that such
        //   dangerous code does not accidentally compile.
        //
        // Concerns:
        //: 1 'a == b' should not compile where 'a' and 'b' are objects of
        //:   'bsl::function' type, whether the same or different
        //:   specializations.
        //:
        //: 2 'a.swap(b)' should not compile where 'a' and 'b' are objects of
        //:   different 'bsl::function' specializations.
        //:
        //: 3 'bsl::swap(a, b)' should not compile where 'a' and 'b' are
        //:   objects of different 'bsl::function' specializations.
        //
        // Plan:
        //: 1 Construct 'function' objects wrapping a small variety of
        //:   'function' instantiations.
        //:
        //: 2 For concern 1, define the macro
        //:   'BSLSTL_FUNCTION_TEST_BAD_COMPARISON' and invoke 'a == b' and 'a
        //:   != b', where the types of 'a' and 'b' are the same or different
        //:   specializations of 'function', and verify that the expressions
        //:   don't compile.
        //:
        //: 3 For concerns 2 and 3, define the macro
        //:   'BSLSTL_FUNCTION_TEST_BAD_SWAPS' and invoke 'a.swap(b)' and
        //:   'swap(a, b)', where the types of 'a' and 'b' are different
        //:   specializations of 'function', and verify that the expressions
        //:   don't compile.
        //
        // Testing:
        //  CONCERN: 'a == b' does not compile for 'function' types.
        //  CONCERN: 'swap' does not compile for different 'function' types.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FAIL-TO-COMPILE CONCERNS"
                            "\n================================\n");

#ifdef BSLSTL_FUNCTION_TEST_PART_01

#if !defined(BSLSTL_FUNCTION_TEST_BAD_COMPARISON)
        if (verbose) printf("\nTesting 'operator==' is not configured\n");
#else
        bsl::function<void()> a, b;
        ASSERT(a == b);  // this should not compile
        ASSERT(a != b);  // this should not compile

        bsl::function<int(int)> c;
        ASSERT(a == c);  // this should not compile
        ASSERT(a != c);  // this should not compile

        ASSERT(c == a);  // this should not compile
        ASSERT(c != a);  // this should not compile
#endif

#if !defined(BSLSTL_FUNCTION_TEST_BAD_SWAPS)
        if (verbose) printf("\nTesting 'swap' is not configured\n");
#else
        bsl::function<void()>   a;
        bsl::function<int(int)> b;

        a.swap(b);  // this should not compile
        b.swap(a);  // this should not compile

        bsl::swap(a, b);  // this should not compile
        bsl::swap(b, a);  // this should not compile
#endif

#else
        BSLSTL_FUNCTION_TEST_CASE_IS_NOT_IN_THIS_EXECUTABLE(test);
#endif  // BSLSTL_FUNCTION_TEST_PART_01
      } break;
#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

#ifdef BSLSTL_FUNCTION_TEST_COMPILING_NO_CASES
    // This '#ifdef' is necessary so compiling "ALL" works.
      case 1: {
          if (verbose)
              puts("\nPART 00 TEST EXECUTABLE CONTAINS NO TEST CASES"
                   "\n==============================================");
      } break;
#endif
      default: {
        fprintf(stderr, "WARNING: CASE '%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

#ifdef BSLSTL_FUNCTION_TEST_COMPILING_A_PART
    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // Verify that every test case destroys any functors that it creates.
    ASSERT(topFuncMonitor.isSameCount());
#endif  // BSLSTL_FUNCTION_TEST_COMPILING_A_PART

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
