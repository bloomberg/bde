// bsltf_streamutil.t.cpp                                             -*-C++-*-
#include <bsltf_streamutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <bslma_testallocator.h>

#include <bsltf_templatetestfacility.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// No 'using' statements: this entire test driver is outside the enterprise
// namespace, to test the fact that ADL will find these 'operator<<' operators
// from outside the 'bsltf' namespace.  (even though they're in the
// 'BloombergLP::bsltf' namespace).

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// FREE OPERATORS
// [ 3] STREAM& op<<(STREAM&, const TemplateTestFacility::MethodPtr);
// [ 3] STREAM& op<<(STREAM&, const AllocBitwiseMoveableTestType&);
// [ 3] STREAM& op<<(STREAM&, const AllocTestType&);
// [ 3] STREAM& op<<(STREAM&, const BitwiseCopyableTestType&);
// [ 3] STREAM& op<<(STREAM&, const BitwiseMoveableTestType&);
// [ 3] STREAM& op<<(STREAM&, const EnumeratedTestType::Enum&);
// [ 3] STREAM& op<<(STREAM&, const MovableAllocTestType&);
// [ 3] STREAM& op<<(STREAM&, const MovableTestType&);
// [ 3] STREAM& op<<(STREAM&, const MoveOnlyAllocTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonAssignableTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonCopyConstructibleTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonDefaultConstructibleTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonEqualComparableTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonOptionalAllocTestType&);
// [ 3] STREAM& op<<(STREAM&, const NonTypicalOverloadsTestType&);
// [ 3] STREAM& op<<(STREAM&, const SimpleTestType&);
// [ 3] STREAM& op<<(STREAM&, const UnionTestType&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
// [ 4] USAGE EXAMPLE

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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef BloombergLP::bsltf::TemplateTestFacility TTF;

namespace {
namespace u {

class TestStreamer {
    // All of the operators provided by this component can stream to any
    // 'STREAM' type that supports 'operator<<(int)', so that's the only
    // streaming operation this 'class' supports.  The most recently streamed
    // value can be examined with the 'value' accessor.

    // DATA
    int d_valueStreamed;

  private:
    // NOT IMPLEMENTED
    TestStreamer(const TestStreamer&);               // = delete
    TestStreamer& operator=(const TestStreamer&);    // = delete

  public:
    // CREATOR
    TestStreamer()
        // Construct this object in the reset state.
    {
        reset();
    }

    // MANIPULATOR
    TestStreamer& operator<<(int rhs)
        // Stream the specified 'rhs' this object.
    {
        d_valueStreamed = rhs;
        return *this;
    }

    void reset()
        // Set the value of this object to 'INT_MIN'.
    {
        d_valueStreamed = INT_MIN;
    }

    // ACCESSOR
    int value() const
        // Return the value most recently streamed to this object since either
        // the last reset or creation, or 'INT_MIN' if it hasn't been streamed
        // to since then.
    {
        return d_valueStreamed;
    }
};

int clip(int x)
    // Return the value in the range '[ 0, 127 ]' that is closest to the
    // specified 'x'.
{
    return x < 0 ? 0
                 : 127 < x
                 ? 127
                 : x;
}

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                                    USAGE
//-----------------------------------------------------------------------------

namespace Usage {

class StdioStream {
    // This 'class' allows one to do transplate stream-style output of 'int's
    // and 'const char *' strings to 'fprintf'-style output.

    // DATA
    FILE *d_out_p;

  public:
    // CREATOR
    explicit StdioStream(FILE *out_p)
    : d_out_p(out_p)
        // Create a 'StdioStream' that will direct its output to the specified
        // 'out_p'.  The behavior is undefined if '0 == out_p'.
    {
        BSLS_ASSERT(out_p);
    }

    // MANIPULATORS
    StdioStream& operator<<(int rhs)
        // Output the specified 'rhs' to 'd_out_p'.
    {
        fprintf(d_out_p, "%d", rhs);

        return *this;
    }

    StdioStream& operator<<(const char *rhs)
        // Output the specified 'rhs' to 'd_out_p'.
    {
        fprintf(d_out_p, "%s", rhs);
        if (strchr(rhs, '\n')) {
            fflush(d_out_p);
        }

        return *this;
    }
};

}  // close namespace Usage

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3;    (void) veryVerbose;
    int veryVeryVerbose     = argc > 4;    (void) veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    printf("TEST %s case %s\n", __FILE__, argv[1]);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the usage of this component.
        //
        // Plan:
        //: 1 Define a simple 'StdioStream' class that translates streaming
        //:   int 'printf'-style output, and define an object 'cout' of that
        //:   class.
        //:
        //: 2 Define a 'SimpleTestType' object, assign it a few values, and
        //:   stream them to 'cout'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        Usage::StdioStream cout(stdout);

// We have defined a 'cout' object that can stream 'int's and 'const char *'
// null-terminated strings.  Note that the operators defined by this component
// require only that the left-hand argument be able to stream 'int's, so our
// 'cout' more than satisfies this requirement.
//
// Then, in 'main', we define a 'bsltf::SimpleTestType' object 'st':
//..
    BloombergLP::bsltf::SimpleTestType st;
//..
// Now, we do some output, assigning 'st' several different values and
// streaming them out:
//..
    cout << "Several values of 'st': ";

    for (int ii = 0; ii <= 100; ii += 20) {
        st.setData(ii);

        cout << (ii ? ", " : "") << st;
    }
    cout << "\n";
//..
// Finally, we observe the output, which is:
//..
//  Several values of 'st': 0, 20, 40, 60, 80, 100
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST STREAMING SUPPORTED BSLTF TYPES
        //
        // Concerns:
        //: 1 The output operator correctly streams the value of an object of
        //:   any test type in the 'bsltf' package into a specified stream.
        //
        // Plan:
        //: 1 For each 'bsltf' test type, create an object of such a type
        //:   having a unique value and stream its value into a
        //:   'std::ostringstream'.  Verify that the string output to the
        //:   stream has the expected value.  (C-1)
        //
        // Testing:
        //   STREAM& op<<(STREAM&, const TemplateTestFacility::MethodPtr&);
        //   STREAM& op<<(STREAM&, const AllocBitwiseMoveableTestType&);
        //   STREAM& op<<(STREAM&, const AllocTestType&);
        //   STREAM& op<<(STREAM&, const BitwiseCopyableTestType&);
        //   STREAM& op<<(STREAM&, const BitwiseMoveableTestType&);
        //   STREAM& op<<(STREAM&, const EnumeratedTestType::Enum&);
        //   STREAM& op<<(STREAM&, const MovableAllocTestType&);
        //   STREAM& op<<(STREAM&, const MovableTestType&);
        //   STREAM& op<<(STREAM&, const MoveOnlyAllocTestType&);
        //   STREAM& op<<(STREAM&, const NonAssignableTestType&);
        //   STREAM& op<<(STREAM&, const NonCopyConstructibleTestType&);
        //   STREAM& op<<(STREAM&, const NonDefaultConstructibleTestType&);
        //   STREAM& op<<(STREAM&, const NonEqualComparableTestType&);
        //   STREAM& op<<(STREAM&, const NonOptionalAllocTestType&);
        //   STREAM& op<<(STREAM&, const NonTypicalOverloadsTestType&);
        //   STREAM& op<<(STREAM&, const SimpleTestType&);
        //   STREAM& op<<(STREAM&, const UnionTestType&);
        // --------------------------------------------------------------------

        if (verbose) printf("TEST STREAMING SUPPORTED BSLTF TYPES\n"
                            "====================================\n");

        namespace bt = BloombergLP::bsltf;

        BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);

        {
            bt::AllocBitwiseMoveableTestType o1 =
                              TTF::create<bt::AllocBitwiseMoveableTestType>(1);

#if 0
            // 'TTF::create<bt::AllocEmplacableTestType>' is commented out
            // everywhere it is called in bsltf_templatetestfacility.t.cpp and
            // doesn't compile when I try to build it here.

            bt::AllocEmplacableTestType o2 =
                                   TTF::create<bt::AllocEmplacableTestType>(2);
#endif

            bt::AllocTestType o3 = TTF::create<bt::AllocTestType>(3);

            bt::BitwiseCopyableTestType o4 =
                                   TTF::create<bt::BitwiseCopyableTestType>(4);

            bt::BitwiseMoveableTestType o5 =
                                   TTF::create<bt::BitwiseMoveableTestType>(5);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            bt::EmplacableTestType o6 = TTF::create<bt::EmplacableTestType>(6);
#endif

            bt::EnumeratedTestType::Enum o7 =
                                  TTF::create<bt::EnumeratedTestType::Enum>(7);

            TTF::MethodPtr o8 = TTF::create<TTF::MethodPtr>(8);

            bt::MovableAllocTestType o9 =
                                      TTF::create<bt::MovableAllocTestType>(9);

            bt::MovableTestType o10 = TTF::create<bt::MovableTestType>(10);

            bt::MoveOnlyAllocTestType o11(11);

            bt::NonAssignableTestType o12 =
                                    TTF::create<bt::NonAssignableTestType>(12);

            bt::NonCopyConstructibleTestType o13(13);

            bt::NonDefaultConstructibleTestType o14 =
                          TTF::create<bt::NonDefaultConstructibleTestType>(14);

            bt::NonEqualComparableTestType o15 =
                               TTF::create<bt::NonEqualComparableTestType>(15);

            bt::NonOptionalAllocTestType o16(16, &ta);

            bt::NonTypicalOverloadsTestType o17 =
                              TTF::create<bt::NonTypicalOverloadsTestType>(17);

            bt::SimpleTestType o18 = TTF::create<bt::SimpleTestType>(18);

            bt::StdAllocTestType<bsl::allocator<int> > o19(19);

            bt::UnionTestType o20 = TTF::create<bt::UnionTestType>(20);

            u::TestStreamer ts, *ts_p;

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o1);
            ASSERT( 1 == ts.value());
            ASSERT(&ts == ts_p);

#if 0
            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o2);
            ASSERT( 2 == ts.value());
            ASSERT(&ts == ts_p);
#endif

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o3);
            ASSERT( 3 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o4);
            ASSERT( 4 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o5);
            ASSERT( 5 == ts.value());
            ASSERT(&ts == ts_p);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o6);
            ASSERT( 6 == ts.value());
            ASSERT(&ts == ts_p);
#endif

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o7);
            ASSERT( 7 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o8);
            ASSERT( 8 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o9);
            ASSERT( 9 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o10);
            ASSERT(10 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o11);
            ASSERT(11 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o12);
            ASSERT(12 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o13);
            ASSERT(13 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o14);
            ASSERT(14 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o15);
            ASSERT(15 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o16);
            ASSERT(16 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o17);
            ASSERT(17 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o18);
            ASSERT(18 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o19);
            ASSERT(19 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ts_p = &(ts << o20);
            ASSERT(20 == ts.value());
            ASSERT(&ts == ts_p);
        }

        for (int jj = -1; jj <= +1; ++jj) {
            for (int kk = 0; kk < 128; ++kk) {
                int ii = kk;

                u::TestStreamer ts, *ts_p = 0;

                bt::AllocBitwiseMoveableTestType o1 =
                             TTF::create<bt::AllocBitwiseMoveableTestType>(ii);

                ts_p = &(ts << o1);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::AllocTestType o2 = TTF::create<bt::AllocTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o2);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::BitwiseCopyableTestType o3 =
                                  TTF::create<bt::BitwiseCopyableTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o3);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::BitwiseMoveableTestType o4 =
                                  TTF::create<bt::BitwiseMoveableTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o4);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::EnumeratedTestType::Enum o5 =
                                 TTF::create<bt::EnumeratedTestType::Enum>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o5);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::MovableAllocTestType o6 =
                                     TTF::create<bt::MovableAllocTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o6);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::MovableTestType o7 = TTF::create<bt::MovableTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o7);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::MoveOnlyAllocTestType o8(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o8);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonAssignableTestType o9 =
                                    TTF::create<bt::NonAssignableTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o9);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonCopyConstructibleTestType o10(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o10);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonDefaultConstructibleTestType o11 =
                          TTF::create<bt::NonDefaultConstructibleTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o11);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonEqualComparableTestType o12 =
                               TTF::create<bt::NonEqualComparableTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o12);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonTypicalOverloadsTestType o13 =
                              TTF::create<bt::NonTypicalOverloadsTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o13);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::SimpleTestType o14 = TTF::create<bt::SimpleTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o14);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::UnionTestType o15 = TTF::create<bt::UnionTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o15);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                TTF::MethodPtr o16 = TTF::create<TTF::MethodPtr>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o16);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);

                ii = u::clip(ii + jj);
                bt::NonOptionalAllocTestType o17 =
                                 TTF::create<bt::NonOptionalAllocTestType>(ii);

                ts.reset();
                ts_p = 0;
                ts_p = &(ts << o17);
                ASSERT(ii == ts.value());
                ASSERT(&ts == ts_p);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 The 'TestStream' object always has the last 'int' value streamed
        //:   to it.
        //:
        //: 2 The 'operator<<(TestStream&, int)' returns a reference to the
        //:   first argument.
        //:
        //: 3 If never streamed to since creation, or if reset, the value is
        //:   'INT_MIN'.
        //
        // Plan:
        //: 1 Default construct a 'TestStreamer' object.
        //:   o Observe that its initial value is 'INT_MIN'.
        //:
        //:   o Stream an 'int' to it and observe that the value takes on that
        //:     'int's value.
        //:
        //:   o Observe that a reference to the 'TestStreamer' is returned.
        //:
        //: 2 Test 'u::clip', which, passed an integer, returns the nearest
        //:   integer in the range '[ 0 .. 127 ]'.
        //:
        //: 3 Iterate through a large number of values, using 'u::clip' to
        //:   pass many values in the range '[ 0 .. 127 ]' to
        //:   'operator<<(TestStream&, int)' and observing the results.
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) printf("TEST APPARATUS\n"
                            "==============\n");

        if (verbose) printf("Simple test of 'u::TestStreamer'\n");
        {
            u::TestStreamer ts, *ts_p = 0;
            ASSERT(INT_MIN == ts.value());

            ts_p = &(ts << 4);
            ASSERT(4 == ts.value());
            ASSERT(&ts == ts_p);

            ts.reset();
            ts_p = 0;
            ASSERT(INT_MIN == ts.value());
        }

        if (verbose) printf("Thorough test of 'u::clip'\n");
        {
            enum { k_MILLION = 1000 * 1000 };

            for (int ii = -k_MILLION; ii <= +k_MILLION; ++ii) {
                const int cc = u::clip(ii);

                if      (127 < ii) {
                    ASSERTV(cc, ii, 127 == cc);
                }
                else if ( ii < 0) {
                    ASSERTV(cc, ii,   0 == cc);
                }
                else {
                    ASSERTV(cc, ii,  cc == ii);
                }
            }
        }

        if (verbose) printf("Test 'u::TestStreamer' and 'u::clip' together\n");
        {
            for (int jj = -3; jj <= +3; ++jj) {
                for (int kk = 0; kk < 128; ++kk) {
                    int ii = kk;

                    u::TestStreamer ts, *ts_p = 0;
                    ASSERT(INT_MIN == ts.value());

                    for (int mm = 0, mmMax = 0 == jj ? 1 : 128; mm < mmMax;
                                                                        ++mm) {
                        ts_p = 0;
                        ts_p = &(ts << ii);
                        ASSERTV(ts.value(), ii, ts.value() == ii);
                        ASSERT(&ts == ts_p);

                        ii = u::clip(ii + jj);
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That our 'operator<<' can stream values to type which supports
        //:   'int' being streamed to it.
        //
        // Plan:
        //: 1 Construct an object of type 'bsltf::SimpleTestType'.
        //:
        //: 2 Construct an object of type 'u::TestStreamer', a type defined in
        //:   this file, which supports only 'operator<<(int)' and records such
        //:   actions.
        //:
        //: 3 Set the 'SimpleTestType' object to several values, and then, in
        //:   each case, stream the value to the 'TestStreamer' object, and
        //:   observe that the correct value was streamed.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        BloombergLP::bsltf::SimpleTestType stt;

        u::TestStreamer ts;
        ASSERT(INT_MIN == ts.value());

        stt.setData(5);
        ts << stt;
        ASSERTV(ts.value(),  5 == ts.value());

        stt.setData(97);
        ts << stt;
        ASSERTV(ts.value(), 97 == ts.value());

        stt.setData(25);
        ts << stt;
        ASSERTV(ts.value(), 25 == ts.value());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

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
