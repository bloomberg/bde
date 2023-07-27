// bslstl_syncbuf.t.cpp                                               -*-C++-*-
#include <bslstl_syncbuf.h>

#include <bslstl_stringbuf.h>

#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_stdstatefulallocator.h>

#include <iostream>  // 'std::cout'
#include <ostream>

#include <stddef.h>  // '::size_t'
#include <stdio.h>
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements 'std::basic_streambuf' that accumulates
// the output and atomically transfers it to the wrapped 'basic_streambuf' on
// the destructor or the 'emit' member function call.  All tests are performend
// for 'basic_syncbuf<char>' and 'basic_syncbuf<wchar_t>' instances.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] basic_syncbuf(ALLOCATOR = {});
// [ 2] basic_syncbuf(streambuf_type *wrapped, ALLOCATOR = {});
// [ 4] basic_syncbuf(basic_syncbuf&& original);
// [ 4] basic_syncbuf(basic_syncbuf&& original, ALLOCATOR);
// [ 2] ~basic_syncbuf();
//
// MANIPULATORS
// [ 5] basic_syncbuf& operator=(basic_syncbuf&& original);
// [ 2] bool emit();
// [ 2] void set_emit_on_sync(bool yes);
// [ 6] void swap(basic_syncbuf& other);
//
// ACCESSORS
// [ 3] allocator_type get_allocator() const;
// [ 3] streambuf_type *get_wrapped() const;
//
// FREE FUNCTIONS
// [ 6] void swap(basic_syncbuf& a, basic_syncbuf& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class CHAR>
class SyncBufTest {
    // "Type-parametrized namespace" for the tests.

    // PRIVATE TYPES
    typedef bsl::basic_syncbuf<CHAR> syncbuf;
    typedef bsl::basic_stringbuf<CHAR> stringbuf;
    typedef bsl::basic_string<CHAR> string;
    typedef syncbuf Obj;

    // PRIVATE CLASS FUNCTIONS
    template <size_t Len>
    static string make_string(const CHAR (&chars)[Len])
        // Create a 'string' from the specified 'chars'.
    {
        return string(chars, Len);
    }

    static string make_string(CHAR ch)
        // Create a 'string' from the specified 'ch'.
    {
        return string(size_t(1), ch);
    }
  public:
    static void testPrimaryManipulators()
        // TESTING PRIMARY MANIPULATORS
    {
        if (veryVerbose) printf("\tdefault constructor\n");
        {
            Obj buf;
            ASSERT(!buf.get_wrapped());
        }

        if (veryVerbose) printf("\tconstructor with wrapped buffer\n");
        {
            stringbuf wrapped;

            Obj buf(&wrapped);
            ASSERT(buf.get_wrapped() == &wrapped);
        }

        if (veryVerbose) printf("\tconstructors with allocator\n");
        {
            bsl::allocator<char> alloc;

            Obj buf(alloc);
            ASSERT(!buf.get_wrapped());
            ASSERT(buf.get_allocator() == alloc);
        }
        {
            bsl::allocator<char> alloc;
            stringbuf wrapped;

            Obj buf(&wrapped, alloc);
            ASSERT(buf.get_wrapped() == &wrapped);
            ASSERT(buf.get_allocator() == alloc);
        }

        if (veryVerbose) printf("\t'set_emit_on_sync'\n");
        {
            Obj buf;
            buf.set_emit_on_sync(true);
        }

        static const CHAR chars[] = { 'a', 'b' };

        if (veryVerbose) printf("\t'emit'\n");
        {
            stringbuf wrapped;

            Obj buf(&wrapped);
            buf.sputc(chars[0]);
            ASSERT(wrapped.str().empty());

            buf.sputc(chars[1]);
            ASSERT(wrapped.str().empty());

            buf.emit();
            ASSERT(wrapped.str() == make_string(chars));
        }

        if (veryVerbose) printf("\tdestructor\n");
        {
            stringbuf wrapped;
            {
                Obj buf(&wrapped);

                buf.sputc(chars[0]);
                ASSERT(wrapped.str().empty());

                buf.sputc(chars[1]);
                ASSERT(wrapped.str().empty());

                // 'emit' is called by the destructor
            }
            ASSERT(wrapped.str() == make_string(chars));
        }
    }

    static void testBasicAccessors()
        // TESTING BASIC ACCESSORS
    {
        bsl::allocator<char> alloc;
        stringbuf wrapped;

        Obj mX(&wrapped, alloc); const Obj& X = mX;

        ASSERT(X.get_wrapped() == &wrapped);

        ASSERT(X.get_allocator() == alloc);
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    static void testMoveConstructor()
        // TESTING MOVE CONSTRUCTOR
    {
        const CHAR ch = 'a';

        if (veryVerbose) printf("\tmove constructor w/o allocator\n");
        {
            bsl::allocator<char> alloc;
            stringbuf wrapped;

            Obj rhs(&wrapped, alloc);
            ASSERT(rhs.get_wrapped() == &wrapped);
            ASSERT(rhs.get_allocator() == alloc);

            rhs.sputc(ch);

            Obj lhs(std::move(rhs));
            ASSERT(lhs.get_wrapped() == &wrapped);
            ASSERT(lhs.get_allocator() == alloc);
            ASSERT(!rhs.get_wrapped());
            ASSERT(wrapped.str().empty());

            rhs.emit();
            ASSERT(wrapped.str().empty());

            lhs.emit();
            ASSERT(wrapped.str() == make_string(ch));
        }

        if (veryVerbose) printf("\tmove constructor with allocator\n");
        {
            bslma::TestAllocator alloc1("a1");
            bslma::TestAllocator alloc2("a2");
            stringbuf wrapped;

            Obj rhs(&wrapped, &alloc1);
            ASSERT(rhs.get_wrapped() == &wrapped);
            ASSERT(rhs.get_allocator().mechanism() == &alloc1);

            rhs.sputc(ch);

            Obj lhs(std::move(rhs), &alloc2);
            ASSERT(lhs.get_wrapped() == &wrapped);
            ASSERT(lhs.get_allocator().mechanism() == &alloc2);
            ASSERT(!rhs.get_wrapped());
            ASSERT(wrapped.str().empty());

            rhs.emit();
            ASSERT(wrapped.str().empty());

            lhs.emit();
            ASSERT(wrapped.str() == make_string(ch));
        }
    }

    static void testMoveAssignment()
        // TESTING MOVE ASSIGNMENT
    {
        static const CHAR chars[] = { 'a', 'b' };

        bslma::TestAllocator alloc("alloc");
        stringbuf wrapped;

        Obj rhs(&wrapped, &alloc);
        ASSERT(rhs.get_wrapped() == &wrapped);
        ASSERT(rhs.get_allocator().mechanism() == &alloc);

        rhs.sputc(chars[1]);
        ASSERTV(wrapped.str().empty());

        stringbuf tmp_wrapped;
        Obj lhs(&tmp_wrapped);
        ASSERT(lhs.get_wrapped() == &tmp_wrapped);

        lhs.sputc(chars[0]);
        ASSERTV(tmp_wrapped.str().empty());

        lhs = std::move(rhs);  // 'lhs.emit()' is implicitly called here
        ASSERT(lhs.get_wrapped() == &wrapped);
        ASSERT(rhs.get_allocator().mechanism() == &alloc);
        ASSERT(!rhs.get_wrapped());
        ASSERT(tmp_wrapped.str() == make_string(chars[0]));

        rhs.emit();
        ASSERT(wrapped.str().empty());

        lhs.emit();
        ASSERT(wrapped.str() == make_string(chars[1]));
    }

    static void testSwap()
        // TESTING 'swap'
    {
        {
            bslma::TestAllocator alloc("alloc");
            stringbuf wrapped1, wrapped2;

            Obj a(&wrapped1, &alloc),
                b(&wrapped2, &alloc);

            ASSERT(a.get_wrapped() == &wrapped1);
            ASSERT(b.get_wrapped() == &wrapped2);
            ASSERT(a.get_allocator() == b.get_allocator());

            a.swap(b);

            ASSERT(a.get_wrapped() == &wrapped2);
            ASSERT(b.get_wrapped() == &wrapped1);

            swap(a, b);

            ASSERT(a.get_wrapped() == &wrapped1);
            ASSERT(b.get_wrapped() == &wrapped2);
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            typedef bsltf::StdStatefulAllocator<
                                       CHAR,
                                       true,  // ON_CONTAINER_COPY_CONSTRUCTION
                                       true,  // ON_CONTAINER_COPY_ASSIGNMENT
                                       true,  // ON_CONTAINER_SWAP
                                       true>  // ON_CONTAINER_MOVE_ASSIGNMENT
                                       PropagatingStdAlloc;

            typedef bsltf::StdStatefulAllocator<
                                       CHAR,
                                       true,  // ON_CONTAINER_COPY_CONSTRUCTION
                                       true,  // ON_CONTAINER_COPY_ASSIGNMENT
                                       false, // ON_CONTAINER_SWAP
                                       true>  // ON_CONTAINER_MOVE_ASSIGNMENT
                                       NonPropagatingStdAlloc;

            typedef bsl::basic_syncbuf<CHAR, bsl::char_traits<CHAR>,
                                       PropagatingStdAlloc>     PropagatingObj;
            typedef bsl::basic_syncbuf<CHAR, bsl::char_traits<CHAR>,
                                     NonPropagatingStdAlloc> NonPropagatingObj;

            bsls::AssertTestHandlerGuard guard;

            bslma::TestAllocator   ta1, ta2;
            PropagatingStdAlloc     pa1(&ta1),  pa2(&ta2);
            NonPropagatingStdAlloc npa1(&ta1), npa2(&ta2);

            // PROPAGATE_ON_CONTAINER_SWAP | EQUAL ALLOCATORS

            // true | true
            {
                PropagatingObj a(pa1), b(pa1);

                ASSERT_PASS(a.swap(b));
                ASSERT_PASS(swap(a,b));
            }

            // true | false
            {
                PropagatingObj a(pa1), b(pa2);

                ASSERT_PASS(a.swap(b));
                ASSERT_PASS(swap(a,b));
            }

            // false | true
            {
                NonPropagatingObj a(npa1), b(npa1);

                ASSERT_PASS(a.swap(b));
                ASSERT_PASS(swap(a,b));
            }

            // false | false
            {
                NonPropagatingObj a(npa1), b(npa2);

                ASSERT_FAIL(a.swap(b));
                ASSERT_FAIL(swap(a,b));
            }
        }
    }
#endif

    static void breathingTest()
        // BREATHING TEST
    {
        static const CHAR chars[] = { 'a', 'b' };
        stringbuf wrapped;
        {
            Obj buf(&wrapped);
            ASSERT(buf.get_wrapped() == &wrapped);
            ASSERT(wrapped.str().empty());

            buf.sputc(chars[0]);
            ASSERT(wrapped.str().empty());
            buf.sputc(chars[1]);
            ASSERT(wrapped.str().empty());
        }
        ASSERT(wrapped.str() == make_string(chars));
    }
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace bsl { using std::ostream; }

void writeSync(bsl::ostream& os)
    // Write atomically to the specified 'os' output stream.
{
    // Temporarily replace the underlying 'streambuf'
    bsl::syncbuf buf(os.rdbuf());
    os.rdbuf(&buf);

    // Write to the 'syncbuf'
    os << "Hello, ";
    os << "World!\n";

    // Restore the underlying 'streambuf'
    os.rdbuf(buf.get_wrapped());

    // The accumulated output will be atomically flushed/emitted here
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate the usage example from header into test driver,
        //:   remove leading comment characters. (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
// Now call the function:
//..
    writeSync(std::cout);
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //: 1 The 'swap' function exchanges the state of two 'syncbuf's.
        //:
        //: 2 Swap free function works the same way as the 'swap' member.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct two 'syncbuf' objects: 'a' and 'b'.  Call the member
        //:   'swap' function.  Verify that the state has been exchanged.  Call
        //:   the 'swap' function.  Verify that the state has been restored.
        //:
        //: 2 Perform negative testing to verify that asserts catch all the
        //:   undefined behavior in the contract.
        //
        // Testing:
        //   void swap(basic_syncbuf& other);
        //   void swap(basic_syncbuf& a, basic_syncbuf& b);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        SyncBufTest< char  >::testSwap();
        SyncBufTest<wchar_t>::testSwap();
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 Move assignment moves all the state from 'original', including
        //:   the accumulated and not flushed output.  'original' has no
        //:   associated wrapped buffer after the call.
        //:
        //: 2 'emit' is called for '*this' before any assignments.
        //
        // Plan:
        //: 1 Construct two 'syncbuf' objects: 'lhs' and 'rhs'.  Write a char
        //:   to each of the objects (different chars) but don't call 'emit'.
        //:   Move-assign - 'lhs = move(rhs)'.  Verify that:
        //:     o 'lhs.get_wrapped()' and 'lhs.get_allocator()' return what
        //:       'rhs.get_wrapped()' and 'rhs.get_allocator()' returned
        //:       before;
        //:     o 'rhs.get_wrapped()' returns 'nullptr';
        //:     o a char written to 'lhs' is flushed to the wrapped
        //:       'streambuf';
        //:     o 'rhs.emit()' has no effect;
        //:     o 'lhs.emit()' flushes a char written to 'rhs' before to the
        //:       wrapped 'streambuf'.
        //
        // Testing:
        //   basic_syncbuf& operator=(basic_syncbuf&& original);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        SyncBufTest< char  >::testMoveAssignment();
        SyncBufTest<wchar_t>::testMoveAssignment();
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 Move constructor moves all the state from 'original', including
        //:   the accumulated and not flushed output.  'original' has no
        //:   associated wrapped buffer after the call.
        //:
        //: 2 An allocator can be passed as a second argument.  Otherwise it
        //:   will be copied from 'original'.
        //
        // Plan:
        //: 1 Construct a 'syncbuf' object ('rhs').  Write a char but don't
        //:   call 'emit'.  Move-construct a new object ('lhs') from 'rhs'
        //:   (without explicit allocator).  Verify that:
        //:     o 'lhs.get_wrapped()' and 'lhs.get_allocator()' return what
        //:       'rhs.get_wrapped()' and 'rhs.get_allocator()' returned
        //:       before;
        //:     o 'rhs.get_wrapped()' returns 'nullptr';
        //:     o 'rhs.emit()' has no effect;
        //:     o 'lhs.emit()' flushes the buffered content to the wrapped
        //:       'streambuf'.
        //:
        //: 2 Repeat the test but now pass an allocator explicitly to the
        //:   move constructor.  Verify that 'lhs.get_allocator()' returns the
        //:   specified allocator instead of 'rhs.get_allocator()'.  Repeat the
        //:   other verification without changes.
        //
        // Testing:
        //   basic_syncbuf(basic_syncbuf&& original);
        //   basic_syncbuf(basic_syncbuf&& original, ALLOCATOR);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        SyncBufTest< char  >::testMoveConstructor();
        SyncBufTest<wchar_t>::testMoveConstructor();
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The accessors are callable with 'const' object.
        //:
        //: 2 The accessors return the values specified at construction time.
        //
        // Plan:
        //: 1 Construct a 'syncbuf' object.  Take a 'const' reference to it.
        //:   Call each of the accessors using the reference.  Compare the
        //:   returned values with the values passed to the constructor.
        //
        // Testing:
        //   allocator_type get_allocator() const;
        //   streambuf_type *get_wrapped() const;
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        SyncBufTest< char  >::testBasicAccessors();
        SyncBufTest<wchar_t>::testBasicAccessors();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 An object can be constructed using a pointer to 'streambuf' and
        //:   an allocator.  Either of the parameters (or both) can be omited -
        //:   default values will be used instead.
        //:
        //: 2 'set_emit_on_sync' is callable with one 'bool' argument and sets
        //:   the "emit-on-sync" flag to the specified value.
        //:
        //: 3 'emit' flushes the buffered content to the wrapped 'streambuf'.
        //:
        //: 4 Destuctor calls 'emit'.
        //
        // Plan:
        //: 1 Try each of four variants of the constructor calls.
        //:
        //: 2 Call the 'set_emit_on_sync' function.
        //:
        //: 3 Create a 'stringbuf' and set it as a wrapped 'streambuf'.  Write
        //:   chars to the 'syncbuf' and verify that nothing has been flushed
        //:   to the 'stringbuf'.  Call 'emit' and verify that everything has
        //:   been flushed.
        //:
        //: 4 Repeat the previous test but don't call 'emit', destroy the
        //:   'syncbuf' instead.
        //
        // Testing:
        //   basic_syncbuf(ALLOCATOR = {});
        //   basic_syncbuf(streambuf_type *wrapped, ALLOCATOR = {});
        //   ~basic_syncbuf();
        //   bool emit();
        //   void set_emit_on_sync(bool yes);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS"
                            "\n============================\n");

        SyncBufTest< char  >::testPrimaryManipulators();
        SyncBufTest<wchar_t>::testPrimaryManipulators();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform simple sanity tests.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        SyncBufTest< char  >::breathingTest();
        SyncBufTest<wchar_t>::breathingTest();
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
