// bslstl_osyncstream.t.cpp                                           -*-C++-*-
#include <bslstl_osyncstream.h>

#include <bslstl_ostream.h>
#include <bslstl_ostringstream.h>
#include <bslstl_stringbuf.h>

#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stddef.h>  // '::size_t'
#include <stdio.h>
#include <stdlib.h>  // 'atoi'
#include <iostream>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] basic_osyncstream(streambuf_type *wrapped, ALLOCATOR = {});
// [ 2] basic_osyncstream(basic_ostream<CHAR,TRAITS>& os, ALLOCATOR = {});
// [ 4] basic_osyncstream(basic_osyncstream&& original);
// [ 4] basic_osyncstream(basic_osyncstream&& original, ALLOCATOR);
// [ 2] ~basic_osyncstream();
//
// MANIPULATORS
// [ 5] basic_osyncstream &operator=(basic_osyncstream&& original);
// [ 2] void emit();
//
// ACCESSORS
// [ 3] allocator_type get_allocator() const;
// [ 3] streambuf_type *get_wrapped() const;
// [ 3] syncbuf_type *rdbuf() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE 1

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
class SyncStreamTest {
    // "Type-parametrized namespace" for the tests.

    typedef bsl::basic_osyncstream<CHAR> osyncstream;
    typedef bsl::basic_ostringstream<CHAR> ostringstream;
    typedef bsl::basic_stringbuf<CHAR> stringbuf;
    typedef bsl::basic_string<CHAR> string;
    typedef osyncstream Obj;

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
        if (veryVerbose) printf("\tconstructors with wrapped buffer\n");
        {
            ostringstream wrapped;

            Obj buf(wrapped);
            ASSERT(buf.get_wrapped() == wrapped.rdbuf());
        }
        {
            stringbuf wrapped;

            Obj os(&wrapped);
            ASSERT(os.get_wrapped() == &wrapped);
        }

        if (veryVerbose) printf("\tconstructors with allocator\n");
        {
            bsl::allocator<char> alloc;
            ostringstream wrapped;

            Obj os(wrapped);
            ASSERT(os.get_wrapped() == wrapped.rdbuf());
            ASSERT(os.get_allocator() == alloc);
        }
        {
            bsl::allocator<char> alloc;
            stringbuf wrapped;

            Obj os(&wrapped);
            ASSERT(os.get_wrapped() == &wrapped);
            ASSERT(os.get_allocator() == alloc);
        }

        static const CHAR chars[] = { 'a', 'b' };

        if (veryVerbose) printf("\t'emit'\n");
        {
            stringbuf wrapped;

            Obj os(&wrapped);
            os.put(chars[0]);
            string str = wrapped.str();
            ASSERTV(str.c_str(), str.empty());

            os.put(chars[1]);
            str = wrapped.str();
            ASSERTV(str.c_str(), str.empty());

            os.emit();
            str = wrapped.str();
            ASSERTV(str.c_str(), str == make_string(chars));
        }

        if (veryVerbose) printf("\tdestructor\n");
        {
            stringbuf wrapped;
            {
                Obj os(&wrapped);
                os.put(chars[0]);
                ASSERT(wrapped.str().empty());

                os.put(chars[1]);
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

        ASSERT(X.rdbuf() != 0);
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

            rhs.put(ch);

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

            rhs.put(ch);

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

        rhs.put(chars[1]);
        ASSERTV(wrapped.str().empty());

        stringbuf tmp_wrapped;
        Obj lhs(&tmp_wrapped);
        ASSERT(lhs.get_wrapped() == &tmp_wrapped);

        lhs.put(chars[0]);
        ASSERTV(tmp_wrapped.str().empty());

        lhs = std::move(rhs);  // 'lhs->rdbuf()->emit()' is implicitly called
        ASSERT(lhs.get_wrapped() == &wrapped);
        ASSERT(rhs.get_allocator().mechanism() == &alloc);
        ASSERT(!rhs.get_wrapped());
        ASSERT(tmp_wrapped.str() == make_string(chars[0]));

        rhs.emit();
        ASSERT(wrapped.str().empty());

        lhs.emit();
        ASSERT(wrapped.str() == make_string(chars[1]));
    }
#endif

    static void breathingTest()
        // BREATHING TEST
    {
        static const CHAR chars[] = { 'a', 'b' };
        ostringstream wrapped;
        {
            Obj os(wrapped);
            ASSERT(os.get_wrapped() == wrapped.rdbuf());
            ASSERT(wrapped.str().empty());

            os.put(chars[0]);
            ASSERT(wrapped.str().empty());
            os.put(chars[1]);
            ASSERT(wrapped.str().empty());
        }
        string str = wrapped.str();
        ASSERTV(str.c_str(), str == make_string(chars));
    }
};

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L  // C++11
namespace bsl {

struct thread {
    // Fake 'bsl::thread' for the "USAGE EXAMPLE" testing.

    // CREATORS
    template <class t_CALLABLE>
    explicit thread(t_CALLABLE callable)
        // Call the specified 'callable'.
    {
        callable();
    }

    // MANIPULATORS
    void join()
        // No-Op.
    {
    }
};

}  // close namespace bsl

// The following example demonstrates concurrent printing of a standard STL
// container of values that can be "ostreamed".  The elements are separated by
// comma and all the sequence is enclosed in curly brackets.  This example
// requires C++11 at least.
//..
template <class t_CONTAINER>
void printContainer(bsl::ostream& stream, const t_CONTAINER& container)
    // Print elements of the specified 'container' to the specified
    // 'stream' in a multi-threaded environment without interleaving with
    // output from another threads.
{
    bsl::osyncstream out(stream);
    out << '{';
    bool first = true;
    for(auto& value : container) {
        if (first) {
            first = false;
        }
        else {
            out << ", ";
        }
        out << value;
    }
    out << '}';
} // all output is atomically transferred to 'stream' on 'out' destruction
//..
#endif

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
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example 1 provided in the component header file
        //:   compiles, links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example 1 from header into test driver, remove
        //:   leading comment characters. (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 1"
                            "\n===============\n");
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L  // C++11
// The following example demonstrates the use of a named variable within a
// block statement for streaming.
//..
    const int    container1[] = {1, 2, 3};
    const double container2[] = {4.0, 5.0, 6.0, 7.0};

    bsl::thread thread1{[&]{ printContainer(std::cout, container1); }};
    bsl::thread thread2{[&]{ printContainer(std::cout, container2); }};
    thread1.join();
    thread2.join();
//..
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
        //: 2 'rdbuf()->emit()' is called for '*this' before any assignments.
        //
        // Plan:
        //: 1 Construct two 'osyncstream' object: 'lhs' and 'rhs'.  Write a
        //:   char to each of the objects (different chars) but don't call
        //    'emit'.  Move-assign - 'lhs = move(rhs)'.  Verify that:
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
        //   basic_osyncstream &operator=(basic_osyncstream&& original);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        SyncStreamTest< char  >::testMoveAssignment();
        SyncStreamTest<wchar_t>::testMoveAssignment();
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 The move constructor moves all the state from 'original',
        //:   including the accumulated and not flushed output.  'original' has
        //:   no associated wrapped buffer after the call.
        //:
        //: 2 An allocator can be passed as a second argument.  Otherwise it
        //:   will be copied from 'original'.
        //
        // Plan:
        //: 1 Construct an 'osyncstream' object ('rhs').  Write a char but
        //:   don't call 'emit'.  Move-construct a new object ('lhs') from
        //:   'rhs' (without explicit allocator).  Verify that
        //:   'lhs.get_wrapped()' and 'lhs.get_allocator()' return what
        //:   'rhs.get_wrapped()' and 'rhs.get_allocator()' returned before.
        //:   Verify that 'rhs.get_wrapped()' returns 'nullptr'.  Verify that
        //:   'rhs.emit()' has no effect while 'lhs.emit()' flushes the
        //:   buffered content to the wrapped 'streambuf'.
        //:
        //: 2 Repeat the test but now pass an allocator explicitly to the
        //:   move constructor.  Verify that 'lhs.get_allocator()' returns the
        //:   specified allocator instead of 'rhs.get_allocator()'.  Repeat the
        //:   other verification without changes.
        //
        // Testing:
        //   basic_osyncstream(basic_osyncstream&& original);
        //   basic_osyncstream(basic_osyncstream&& original, ALLOCATOR);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        SyncStreamTest< char  >::testMoveConstructor();
        SyncStreamTest<wchar_t>::testMoveConstructor();
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The accessors are callable with 'const' object.
        //:
        //: 2 The accessors return the values specified at the construction
        //:   time.
        //
        // Plan:
        //: 1 Construct an 'osyncstream' object.  Take a 'const' reference to
        //:   it.  Call each of the accessors using the reference.  Compare the
        //:   returned values with the values passed to the constructor.  For
        //:   'rdbuf' just verify that it returns non-null because we have no
        //:   original object to compare.
        //
        // Testing:
        //   allocator_type get_allocator() const;
        //   streambuf_type *get_wrapped() const;
        //   syncbuf_type *rdbuf() const;
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        SyncStreamTest< char  >::testBasicAccessors();
        SyncStreamTest<wchar_t>::testBasicAccessors();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 An object can be constructed using either a pointer to
        //:   'streambuf' or a reference to 'ostream'.  Optionally  an
        //:   allocator can be specified for each constructor.
        //:
        //: 2 'emit' flushes the buffered content to the wrapped 'streambuf'.
        //:
        //: 3 Destuctor calls 'emit'.
        //
        // Plan:
        //: 1 Try each of four variants of the constructor calls.  Verify that
        //:   the getters return the passed values.
        //:
        //: 2 Create a 'stringbuf' and set it as a wrapped 'streambuf'.  Write
        //:   chars to the 'osyncstream' and verify that nothing has been
        //:   flushed to the 'stringbuf'.  Call 'emit' and verify that
        //:   everything has been flushed.
        //:
        //: 3 Repeat the previous test but don't call 'emit', destroy the
        //:   'osyncstream' instead.
        //
        // Testing:
        //   basic_osyncstream(streambuf_type *wrapped, ALLOCATOR = {});
        //   basic_osyncstream(basic_ostream<CHAR,TRAITS>& os, ALLOCATOR = {});
        //   ~basic_osyncstream();
        //   void emit();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS"
                            "\n============================\n");

        SyncStreamTest< char  >::testPrimaryManipulators();
        SyncStreamTest<wchar_t>::testPrimaryManipulators();
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

        SyncStreamTest< char  >::breathingTest();
        SyncStreamTest<wchar_t>::breathingTest();
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
