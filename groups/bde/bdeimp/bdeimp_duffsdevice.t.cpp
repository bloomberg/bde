// bdeimp_duffsdevice.t.cpp          -*-C++-*-

#include <bdeimp_duffsdevice.h>

#include <bsls_platformutil.h>       // for testing only
#include <bsls_timeutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>                  // memcpy()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a suite of static member functions
// (pure procedures) that perform common non-primitive operations on
// arrays.  The general plan is that each method is tested against a set of
// tabulated test vectors in all test modes on various fundamental types.
//-----------------------------------------------------------------------------
// [ 1] int  areEqual(const T *array1, const T *array2, int numElements);
// [ 1] int  areEqualRaw(const T *array1, const T *array2, int numElements);
// [ 2] void assign(T *dstArray, const T *srcArray, int numElements);
// [ 2] void assignRaw(T *dstArray, const T *srcArray, int numElements);
// [ 3] void initialize(T *array, T value, int numElements);
// [ 3] void initializeRaw(T *array, T value, int numElements);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [-1] BENCHMARKING

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        {
            int a1[32];
            bdeimp_DuffsDevice<int>::initialize(a1, 7, 32);

            int a2[32];
            bdeimp_DuffsDevice<int>::assign(a2, a1, 32);

            ASSERT(bdeimp_DuffsDevice<int>::areEqual(a1, a2, 32));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZE OPERATOR:
        // We are concerned that, for an array of any length, 'initialize' must
        // set the value of every element regardless of the element type.
        //
        // Plan:
        //   Specify a set of test vectors which define all relevant test
        //   patterns as well as the expected result.  Iterate over the test
        //   vectors and ensure the expected result is obtained through use of
        //   'areEqual'.  Repeat the process with various element types.
        //
        // Testing:
        //   void initialize(T *array, T value, int numElements);
        //   void initializeRaw(T *array, T value, int numElements);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'initialize'" << endl
                 << "====================" << endl;

        if (verbose) cout << "\nTesting 'initialize(array, value, ne)' "
                          << "for 'int'."
                          << endl;
        {
            const int SZ = 32;
            static struct {
                int d_lineNum;    // source line number
                int d_dst_p[SZ];  // destination array
                int d_value;      // initialization value
                int d_ne;         // number of elements
                int d_exp_p[SZ];  // source array (not const; Windows)
            } DATA[] = {
                //line  dst                   val.  ne  exp
                //----  --------------------  ----  --  --------------------
                { L_,   { 1 },                 0,    0, { 0 }                },
                { L_,   { 1 },                 0,    1, { 0 }                },
                { L_,   { 1 },                 2,    1, { 2 }                },
                { L_,   { 2 },                 0,    1, { 0 }                },
                { L_,   { 1, 2, 3 },           0,    1, { 0, 2, 3 }          },
                { L_,   { 1, 2, 3 },           0,    2, { 0, 0, 3 }          },
                { L_,   { 1, 2, 3 },           0,    3, { 0, 0, 0 }          },
                { L_,   { 1, 2, 3 },           4,    1, { 4, 2, 3 }          },
                { L_,   { 1, 2, 3 },           4,    2, { 4, 4, 3 }          },
                { L_,   { 1, 2, 3 },           4,    3, { 4, 4, 4 }          },
                { L_,   { 1, 2, 3, 4 },        0,    4, { 0, 0, 0, 0 }       },
                { L_,   { 1, 2, 3, 4, 5 },     0,    5, { 0, 0, 0, 0, 0 }    },
                { L_,   { 1, 2, 3, 4, 5, 6 },  0,    6, { 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                                               0,    7,
                                                     { 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8 },
                                               0,    8,
                                                  { 0, 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                                               0,    9,
                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                                               0,    15,
                                           { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                                               0,    16,
                                         { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                                               0,    17,
                                       { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE   = DATA[ti].d_lineNum;
                int       *DST    = DATA[ti].d_dst_p;
                const int  VALUE  = DATA[ti].d_value;
                const int  NE     = DATA[ti].d_ne;
                const int *EXP    = DATA[ti].d_exp_p;

                int dest[SZ];
                memcpy(dest, DST, SZ * sizeof(int));

                bdeimp_DuffsDevice<int>::initialize(dest, VALUE, NE);
                LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<int>::areEqual(dest,
                                                                 EXP,
                                                                 NE));
                LOOP_ASSERT(LINE, dest[NE] == DST[NE]);

                if (NE > 0) {
                     memcpy(dest, DST, SZ * sizeof(int));
                     bdeimp_DuffsDevice<int>::initializeRaw(dest, VALUE, NE);
                     LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<int>::areEqualRaw(dest,
                                                                    EXP,
                                                                    NE));
                     LOOP_ASSERT(LINE, dest[NE] == DST[NE]);
                }
            }
        }

        if (verbose) cout << "\nTesting 'initialize(array, value, ne)' "
                          << "for 'double'."
                          << endl;
        {
            const int SZ = 32;
            static struct {
                int    d_lineNum;    // source line number
                double d_dst_p[SZ];  // destination array
                double d_value;      // initialization value
                int    d_ne;         // number of elements
                double d_exp_p[SZ];  // source array (not const; Windows)
            } DATA[] = {
                //line  dst                   val.  ne  exp
                //----  --------------------  ----  --  --------------------
                { L_,   { 1 },                 0,    0, { 0 }                },
                { L_,   { 1 },                 0,    1, { 0 }                },
                { L_,   { 1 },                 2,    1, { 2 }                },
                { L_,   { 2 },                 0,    1, { 0 }                },
                { L_,   { 1, 2, 3 },           0,    1, { 0, 2, 3 }          },
                { L_,   { 1, 2, 3 },           0,    2, { 0, 0, 3 }          },
                { L_,   { 1, 2, 3 },           0,    3, { 0, 0, 0 }          },
                { L_,   { 1, 2, 3 },           4,    1, { 4, 2, 3 }          },
                { L_,   { 1, 2, 3 },           4,    2, { 4, 4, 3 }          },
                { L_,   { 1, 2, 3 },           4,    3, { 4, 4, 4 }          },
                { L_,   { 1, 2, 3, 4 },        0,    4, { 0, 0, 0, 0 }       },
                { L_,   { 1, 2, 3, 4, 5 },     0,    5, { 0, 0, 0, 0, 0 }    },
                { L_,   { 1, 2, 3, 4, 5, 6 },  0,    6, { 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                                               0,    7,
                                                     { 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8 },
                                               0,    8,
                                                  { 0, 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                                               0,    9,
                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                                               0,    15,
                                           { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                                               0,    16,
                                         { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                                               0,    17,
                                       { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int     LINE   = DATA[ti].d_lineNum;
                double       *DST    = DATA[ti].d_dst_p;
                const double  VALUE  = DATA[ti].d_value;
                const int     NE     = DATA[ti].d_ne;
                const double *EXP    = DATA[ti].d_exp_p;

                double dest[SZ];
                memcpy(dest, DST, SZ * sizeof(double));

                bdeimp_DuffsDevice<double>::initialize(dest, VALUE, NE);
                LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<double>::areEqual(dest,
                                                                 EXP,
                                                                 NE));
                LOOP_ASSERT(LINE, dest[NE] == DST[NE]);

                if (NE > 0) {
                     memcpy(dest, DST, SZ * sizeof(double));
                     bdeimp_DuffsDevice<double>::initializeRaw(dest,VALUE,NE);
                     LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<double>::areEqualRaw(dest,
                                                                    EXP,
                                                                    NE));
                     LOOP_ASSERT(LINE, dest[NE] == DST[NE]);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ASSIGN OPERATOR:
        // We are concerned that, for arrays of any length, 'assign' must set
        // the value of every element of the 'dstArray' to the value of the
        // 'srcArray' regardless of the element type.
        //
        // Plan:
        //   Specify a set of test vectors which define all relevant test
        //   patterns.  Iterate over the test vectors and ensure the expected
        //   result is obtained through use of 'areEqual'.  Repeat the process
        //   with various element types.
        //
        // Testing:
        //   void assign(T *dstArray, const T *srcArray, int numElements);
        //   void assignRaw(T *dstArray, const T *srcArray, int numElements);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'assign'" << endl
                 << "================" << endl;

        if (verbose) cout << "\nTesting 'assign(dstArray, srcArray, ne)' "
                          << "for 'int'."
                          << endl;
        {
            const int SZ = 32;
            static struct {
                int d_lineNum;          // source line number
                int d_dst_p[SZ];        // destination array
                int d_src_p[SZ];        // source array (not const; Windows)
                int d_ne;               // number of elements
            } DATA[] = {
                //line  dst                   src                   ne
                //----  --------------------  --------------------  --
                { L_,   { -1 },               { 1 },                 0 },
                { L_,   { -1 },               { 2 },                 0 },
                { L_,   { 1, -1 },            { 1 },                 1 },
                { L_,   { 1, -1 },            { 2 },                 1 },
                { L_,   { 2, -1 },            { 1 },                 1 },
                { L_,   { 0, 1, -1 },         { 1, 2 },              2 },
                { L_,   { 0, 1, 2, -1 },      { 1, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 4, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 4, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 2, 4 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 3, 2 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 2, 3, 1 },           3 },
                { L_,   { 0, 1, 2, 3, -1 },   { 1, 2, 3, 4 },        4 },
                { L_,   { 0, 1, 2, 3, 4, -1 },{ 1, 2, 3, 4, 5 },     5 },
                { L_,   { 0, 1, 2, 3, 4, 5, -1 },
                                              { 1, 2, 3, 4, 5, 6 },  6 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, -1 },
                                           { 1, 2, 3, 4, 5, 6, 7 },  7 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, 7, -1 },
                                        { 1, 2, 3, 4, 5, 6, 7, 8 },  8 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, 7, 8, -1 },
                                     { 1, 2, 3, 4, 5, 6, 7, 8, 9 },  9 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,-1 },
                           { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 }, 15 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 }, 16 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1 },
                     { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 }, 17 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE   = DATA[ti].d_lineNum;
                int       *DST    = DATA[ti].d_dst_p;
                const int *SRC    = DATA[ti].d_src_p;
                const int  NE     = DATA[ti].d_ne;
                const int  curLen = NE;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                int dest[SZ];

                if (NE > 0) {
                    memcpy(dest, DST, SZ * sizeof(int));
                    bdeimp_DuffsDevice<int>::assignRaw(dest, SRC, NE);
                    LOOP_ASSERT(LINE, 1 ==
                        bdeimp_DuffsDevice<int>::areEqualRaw(dest, SRC, NE));
                    LOOP_ASSERT(LINE, -1 == dest[NE]);
                }

                memcpy(dest, DST, SZ * sizeof(int));
                bdeimp_DuffsDevice<int>::assign(dest, SRC, NE);
                LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<int>::areEqual(dest, SRC, NE));
                LOOP_ASSERT(LINE, -1 == dest[NE]);
            }
        }

        if (verbose) cout << "\nTesting 'assign(dstArray, srcArray, ne)' "
                          << "for 'double'."
                          << endl;
        {
            const int SZ = 32;
            static struct {
                int    d_lineNum;       // source line number
                double d_dst_p[SZ];     // destination array
                double d_src_p[SZ];     // source array (not const; Windows)
                int    d_ne;            // number of elements
            } DATA[] = {
                //line  dst                   src                   ne
                //----  --------------------  --------------------  --
                { L_,   { -1 },               { 1 },                 0 },
                { L_,   { -1 },               { 1 },                 0 },
                { L_,   { 1, -1 },            { 1 },                 1 },
                { L_,   { 1, -1 },            { 2 },                 1 },
                { L_,   { 2, -1 },            { 1 },                 1 },
                { L_,   { 0, 1, -1 },         { 1, 2 },              2 },
                { L_,   { 0, 1, 2, -1 },      { 1, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 4, 2, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 4, 3 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 2, 4 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 1, 3, 2 },           3 },
                { L_,   { 1, 2, 3, -1 },      { 2, 3, 1 },           3 },
                { L_,   { 0, 1, 2, 3, -1 },   { 1, 2, 3, 4 },        4 },
                { L_,   { 0, 1, 2, 3, 4, -1 },{ 1, 2, 3, 4, 5 },     5 },
                { L_,   { 0, 1, 2, 3, 4, 5, -1 },
                                              { 1, 2, 3, 4, 5, 6 },  6 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, -1 },
                                           { 1, 2, 3, 4, 5, 6, 7 },  7 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, 7, -1 },
                                        { 1, 2, 3, 4, 5, 6, 7, 8 },  8 },
                { L_,   { 0, 1, 2, 3, 4, 5, 6, 7, 8, -1 },
                                     { 1, 2, 3, 4, 5, 6, 7, 8, 9 },  9 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,-1 },
                           { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 }, 15 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,-1 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 }, 16 },
                { L_,   { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,-1 },
                       { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 }, 17 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int     LINE   = DATA[ti].d_lineNum;
                double       *DST    = DATA[ti].d_dst_p;
                const double *SRC    = DATA[ti].d_src_p;
                const int     NE     = DATA[ti].d_ne;
                const int     curLen = NE;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                double dest[SZ];

                if (NE > 0) {
                    memcpy(dest, DST, SZ * sizeof(double));

                    bdeimp_DuffsDevice<double>::assignRaw(dest, SRC, NE);
                    LOOP_ASSERT(LINE, 1 ==
                                bdeimp_DuffsDevice<double>::areEqualRaw(dest,
                                                                        SRC,
                                                                        NE));
                    LOOP_ASSERT(LINE, -1 == dest[NE]);
                }

                memcpy(dest, DST, SZ * sizeof(double));

                bdeimp_DuffsDevice<double>::assign(dest, SRC, NE);
                LOOP_ASSERT(LINE, 1 ==
                            bdeimp_DuffsDevice<double>::areEqual(dest,
                                                                 SRC,
                                                                 NE));
                LOOP_ASSERT(LINE, -1 == dest[NE]);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING AREEQUAL OPERATOR:
        // We are concerned that, for arrays of any length, 'areEqual' must
        // verify the value of every element in the 'dstArray' has the same
        // value as the one in the 'srcArray' regardless of the element type.
        // We are also concerned that 'areEqual' tests the correct number
        // of elements.
        //
        // Plan:
        //   Specify a set of test vectors which define all relevant test
        //   patterns as well as the expected result.  Iterate over the test
        //   vectors and ensure the expected result is obtained.  Repeat the
        //   process with various element types.
        //
        // Testing:
        //   int areEqual(const T *array1, const T *array2, int numElements);
        //   int areEqualRaw(const T *array1, const T *array2,int numElements);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'areEqual'" << endl
                 << "==================" << endl;

        if (verbose) cout << "\nTesting 'areEqual(array1, array2, ne)' "
                          << "for 'int'."
                          << endl;
        {
            const int SZ = 32;
            static const struct {
                int d_lineNum;    // source line number
                int d_dst_p[SZ];  // destination array (not const; Windows)
                int d_src_p[SZ];  // source array (not const; Windows)
                int d_ne;         // number of elements
                int d_exp;        // expected return value
            } DATA[] = {
                //line  dst             src             ne  exp
                //----  --------------  --------------  --  ---
                { L_,   { 1 },          { 1 },           0,   1 },
                { L_,   { 1 },          { 2 },           0,   1 },
                { L_,   { 1 },          { 1 },           1,   1 },
                { L_,   { 1 },          { 2 },           1,   0 },
                { L_,   { 2 },          { 1 },           1,   0 },
                { L_,   { 1, 2 },       { 1, 2 },        1,   1 },
                { L_,   { 1, 3 },       { 1, 2 },        1,   1 },
                { L_,   { 1, 2 },       { 1, 2 },        2,   1 },
                { L_,   { 1, 2 },       { 3, 2 },        2,   0 },
                { L_,   { 1, 2 },       { 1, 3 },        2,   0 },
                { L_,   { 1, 2, 3 },    { 1, 2, 3 },     3,   1 },
                { L_,   { 1, 2, 3 },    { 4, 2, 3 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 4, 3 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 2, 4 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 3, 2 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 2, 3, 1 },     3,   0 },
                { L_,   { 1, 2, 3, 4 }, { 1, 2, 3, 4 },  4,   1 },
                { L_,   { 1, 2, 3, 4, 5 },
                                     { 1, 2, 3, 4, 5 },  5,   1 },
                { L_,   { 1, 2, 0, 4, 5 },
                                     { 1, 2, 3, 4, 5 },  5,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6 },
                                  { 1, 2, 3, 4, 5, 6 },  6,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6 },
                                  { 1, 2, 3, 4, 0, 6 },  6,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                               { 1, 2, 3, 4, 5, 6, 7 },  7,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                               { 0, 2, 3, 4, 5, 6, 7 },  7,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8 },
                            { 1, 2, 3, 4, 5, 6, 7, 8 },  8,   1 },
                { L_,   { 1, 0, 3, 4, 5, 6, 7, 8 },
                            { 1, 2, 3, 4, 5, 6, 7, 8 },  8,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                         { 1, 2, 3, 4, 5, 6, 7, 8, 9 },  9,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                         { 1, 2, 3, 4, 5, 6, 7, 8, 0 },  9,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                                                        15,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,0 },
                                                        15,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                                                        16,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 },
                                                        16,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                                                        17,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0 },
                                                        17,   0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int  LINE   = DATA[ti].d_lineNum;
                const int *DST    = DATA[ti].d_dst_p;
                const int *SRC    = DATA[ti].d_src_p;
                const int  NE     = DATA[ti].d_ne;
                const int  EXP    = DATA[ti].d_exp;
                const int  curLen = NE;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                LOOP_ASSERT(LINE, EXP ==
                            bdeimp_DuffsDevice<int>::areEqual(DST, SRC, NE));

                if (NE > 0) {
                    LOOP_ASSERT(LINE, EXP ==
                          bdeimp_DuffsDevice<int>::areEqualRaw(DST, SRC, NE));
                }
            }
        }

        if (verbose) cout << "\nTesting 'areEqual(array1, array2, ne)' "
                          << "for 'double'."
                          << endl;
        {
            const int SZ = 32;
            static const struct {
                int    d_lineNum;    // source line number
                double d_dst_p[SZ];  // destination array (not const; Windows)
                double d_src_p[SZ];  // source array (not const; Windows)
                int    d_ne;         // number of elements
                int    d_exp;        // expected return value
            } DATA[] = {
                //line  dst             src             ne  exp
                //----  --------------  --------------  --  ---
                { L_,   { 1 },          { 1 },           0,   1 },
                { L_,   { 1 },          { 2 },           0,   1 },
                { L_,   { 1 },          { 1 },           1,   1 },
                { L_,   { 1 },          { 2 },           1,   0 },
                { L_,   { 2 },          { 1 },           1,   0 },
                { L_,   { 1, 2 },       { 1, 2 },        1,   1 },
                { L_,   { 1, 3 },       { 1, 2 },        1,   1 },
                { L_,   { 1, 2 },       { 1, 2 },        2,   1 },
                { L_,   { 1, 2 },       { 3, 2 },        2,   0 },
                { L_,   { 1, 2 },       { 1, 3 },        2,   0 },
                { L_,   { 1, 2, 3 },    { 1, 2, 3 },     3,   1 },
                { L_,   { 1, 2, 3 },    { 4, 2, 3 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 4, 3 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 2, 4 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 1, 3, 2 },     3,   0 },
                { L_,   { 1, 2, 3 },    { 2, 3, 1 },     3,   0 },
                { L_,   { 1, 2, 3, 4 }, { 1, 2, 3, 4 },  4,   1 },
                { L_,   { 1, 2, 3, 4, 5 },
                                     { 1, 2, 3, 4, 5 },  5,   1 },
                { L_,   { 1, 2, 0, 4, 5 },
                                     { 1, 2, 3, 4, 5 },  5,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6 },
                                  { 1, 2, 3, 4, 5, 6 },  6,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6 },
                                  { 1, 2, 3, 4, 0, 6 },  6,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                               { 1, 2, 3, 4, 5, 6, 7 },  7,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7 },
                               { 0, 2, 3, 4, 5, 6, 7 },  7,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8 },
                            { 1, 2, 3, 4, 5, 6, 7, 8 },  8,   1 },
                { L_,   { 1, 0, 3, 4, 5, 6, 7, 8 },
                            { 1, 2, 3, 4, 5, 6, 7, 8 },  8,   0 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                         { 1, 2, 3, 4, 5, 6, 7, 8, 9 },  9,   1 },
                { L_,   { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
                         { 1, 2, 3, 4, 5, 6, 7, 8, 0 },  9,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                                                        15,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,0 },
                                                        15,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                                                        16,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 },
                                                        16,   0 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                                                        17,   1 },
                { L_,   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 },
                        { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0 },
                                                        17,   0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int     LINE   = DATA[ti].d_lineNum;
                const double *DST    = DATA[ti].d_dst_p;
                const double *SRC    = DATA[ti].d_src_p;
                const int     NE     = DATA[ti].d_ne;
                const int     EXP    = DATA[ti].d_exp;
                const int     curLen = NE;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                LOOP_ASSERT(LINE, EXP ==
                            bdeimp_DuffsDevice<double>::areEqual(DST,
                                                                 SRC,
                                                                 NE));

                if (NE > 0) {
                    LOOP_ASSERT(LINE, EXP ==
                            bdeimp_DuffsDevice<double>::areEqualRaw(DST,
                                                                    SRC,
                                                                    NE));
                }
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING DUFFS DEVICE BENCHMARKING
        // We are concerned that Duff's device is consistently outperformed by
        // 'bsl::memset', 'bsl::memcpy', and 'bsl::memmove'.
        //
        // Plan:
        //   Specify a set of test vectors which define all relevant test
        //   patterns as well as the expected result.  Iterate over the test
        //   vectors and ensure the expected result is obtained, and that the
        //   block using Duff's device is slower than the corresponding block
        //   using 'bsl::memset', 'bsl::memcpy', or 'bsl::memmove', as measured
        //   by 'bsls_TimeUtil::getTimer'.
        //
        // Testing:
        //   'bdeimp_DuffsDevice::initialize' versus 'bsl::memset'
        //   'bdeimp_DuffsDevice::assign' versus 'bsl::memcpy'
        //   'bdeimp_DuffsDevice::assign' versus 'bsl::memmove'
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'bdeimp_DuffsDevice' Benchmarking" << endl
                 << "=========================================" << endl;

        // We do not test zero-length operations since Duff's device is better
        // optimized than 'bsl::mem*'.

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout
              << "\nTesting 'bdeimp_DuffsDevice::initialize' vs 'bsl::memset'."
              << "\n----------------------------------------------------------"
              << endl;
        }
        {
            const struct {
                int d_line;      // source line number
                int d_value;     // initialization value
                int d_length;    // size of array to be initialized
            } DATA[] = {
                //Line  Value  Length
                //----  -----  ------
//              { L_,   0x01,   0,           },
                { L_,   0x23,   1,           },
                { L_,   0x45,   2,           },
                { L_,   0x67,   4,           },
                { L_,   0x89,   8,           },
                { L_,   0xab,   16,          },
                { L_,   0xcd,   32,          },
                { L_,   0xef,   64,          },
                { L_,   0xf0,   128,         },
                { L_,   0x1e,   256,         },
                { L_,   0xd2,   512,         },
                { L_,   0x3c,   1024,        },
                { L_,   0xb4,   2    * 1024, },
                { L_,   0x5a,   4    * 1024, },
                { L_,   0x00,   8    * 1024, },
                { L_,   0xff,   128  * 1024, },
                { L_,   0x69,   1024 * 1024, },

                // White-box: test for each case in Duff's device.
                { L_,   0x11,   8 * 512 + 1, },
                { L_,   0x22,   8 * 512 + 2, },
                { L_,   0x33,   8 * 512 + 3, },
                { L_,   0x44,   8 * 512 + 4, },
                { L_,   0x55,   8 * 512 + 5, },
                { L_,   0x66,   8 * 512 + 6, },
                { L_,   0x77,   8 * 512 + 7, },
                { L_,   0x88,   8 * 512 + 8, },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            char *dd = new char[1024*1024];  // array for Duff's device
            char *ms = new char[1024*1024];  // array for 'bsl::memset'

            bsls_PlatformUtil::Int64 ddStart, ddStop;
            bsls_PlatformUtil::Int64 msStart, msStop;

            for (int i = 0; i < DATA_SIZE; ++i) {
                int LINE   = DATA[i].d_line;
                int VALUE  = DATA[i].d_value;
                int LENGTH = DATA[i].d_length;

                ddStart = bsls_TimeUtil::getTimer();
                bdeimp_DuffsDevice<char>::initialize(dd, VALUE, LENGTH);
                ddStop  = bsls_TimeUtil::getTimer();

                msStart = bsls_TimeUtil::getTimer();
                bsl::memset(ms, VALUE, LENGTH);
                msStop  = bsls_TimeUtil::getTimer();

                bsls_PlatformUtil::Int64 ddTime = ddStop - ddStart;
                bsls_PlatformUtil::Int64 msTime = msStop - msStart;
                LOOP2_ASSERT(i, LINE, msTime <= ddTime);

                if (verbose) {
                    P_(i); P_(LINE); P_(LENGTH); P_(msTime); P(ddTime);
                }
            }

            delete[] dd;  delete[] ms;
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\nTesting 'bdeimp_DuffsDevice::assign' vs 'bsl::memcpy'."
                 << "\n------------------------------------------------------"
                 << endl;
        }
        {
            const struct {
                int d_line;      // source line number
                int d_value;     // initialization value
                int d_length;    // size of array to be initialized
            } DATA[] = {
                //Line  Value  Length
                //----  -----  ------
//              { L_,   0x01,   0,           },
                { L_,   0x23,   1,           },
                { L_,   0x45,   2,           },
                { L_,   0x67,   4,           },
                { L_,   0x89,   8,           },
                { L_,   0xab,   16,          },
                { L_,   0xcd,   32,          },
                { L_,   0xef,   64,          },
                { L_,   0xf0,   128,         },
                { L_,   0x1e,   256,         },
                { L_,   0xd2,   512,         },
                { L_,   0x3c,   1024,        },
                { L_,   0xb4,   2    * 1024, },
                { L_,   0x5a,   4    * 1024, },
                { L_,   0x00,   8    * 1024, },
                { L_,   0xff,   128  * 1024, },
                { L_,   0x69,   1024 * 1024, },

                // White-box: test for each case in Duff's device.
                { L_,   0x11,   8 * 512 + 1, },
                { L_,   0x22,   8 * 512 + 2, },
                { L_,   0x33,   8 * 512 + 3, },
                { L_,   0x44,   8 * 512 + 4, },
                { L_,   0x55,   8 * 512 + 5, },
                { L_,   0x66,   8 * 512 + 6, },
                { L_,   0x77,   8 * 512 + 7, },
                { L_,   0x88,   8 * 512 + 8, },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            char *dd = new char[1024*1024];  // array for Duff's device
            char *mc = new char[1024*1024];  // array for 'bsl::memcpy'

            bsls_PlatformUtil::Int64 ddStart, ddStop;
            bsls_PlatformUtil::Int64 mcStart, mcStop;

            for (int i = 0; i < DATA_SIZE; ++i) {
                int LINE   = DATA[i].d_line;
                int VALUE  = DATA[i].d_value;
                int LENGTH = DATA[i].d_length;

                bsl::memset(dd, 0x0d, LENGTH);
                bsl::memset(mc, VALUE, LENGTH);
                ddStart = bsls_TimeUtil::getTimer();
                bdeimp_DuffsDevice<char>::assign(dd, mc, LENGTH);
                ddStop  = bsls_TimeUtil::getTimer();

                bsl::memset(dd, VALUE, LENGTH);
                bsl::memset(mc, 0x0d, LENGTH);
                mcStart = bsls_TimeUtil::getTimer();
                bsl::memcpy(mc, dd, LENGTH);
                mcStop  = bsls_TimeUtil::getTimer();

                bsls_PlatformUtil::Int64 ddTime = ddStop - ddStart;
                bsls_PlatformUtil::Int64 mcTime = mcStop - mcStart;
                LOOP2_ASSERT(i, LINE, mcTime <= ddTime);

                if (verbose) {
                    P_(i); P_(LINE); P_(LENGTH); P_(mcTime); P(ddTime);
                }
            }

            delete[] dd;  delete[] mc;
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\nTesting 'bdeimp_DuffsDevice::assign' vs 'bsl::memmove'."
                 << "\n-------------------------------------------------------"
                 << endl;
        }
        {
            const struct {
                int d_line;      // source line number
                int d_value;     // initialization value
                int d_length;    // size of array to be initialized
            } DATA[] = {
                //Line  Value  Length
                //----  -----  ------
//              { L_,   0xab,   16,          },
                { L_,   0xcd,   32,          },
                { L_,   0xef,   64,          },
                { L_,   0xf0,   128,         },
                { L_,   0x1e,   256,         },
                { L_,   0xd2,   512,         },
                { L_,   0x3c,   1024,        },
                { L_,   0xb4,   2    * 1024, },
                { L_,   0x5a,   4    * 1024, },
                { L_,   0x00,   8    * 1024, },
                { L_,   0xff,   128  * 1024, },
                { L_,   0x69,   1024 * 1024, },

                // White-box: test for each case in Duff's device.
                { L_,   0x11,   8 * 512 + 1, },
                { L_,   0x22,   8 * 512 + 2, },
                { L_,   0x33,   8 * 512 + 3, },
                { L_,   0x44,   8 * 512 + 4, },
                { L_,   0x55,   8 * 512 + 5, },
                { L_,   0x66,   8 * 512 + 6, },
                { L_,   0x77,   8 * 512 + 7, },
                { L_,   0x88,   8 * 512 + 8, },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            char *dd = new char[1024*1024];  // array for Duff's device
            char *mm = new char[1024*1024];  // array for 'bsl::memmove'

            bsls_PlatformUtil::Int64 ddStart, ddStop;
            bsls_PlatformUtil::Int64 mmStart, mmStop;

            enum { DUFF_SIZE = 8 };  // Size of Duff's device (imp detail)
            for (int i = 0; i < DATA_SIZE; ++i) {
                int LINE   = DATA[i].d_line;
                int VALUE  = DATA[i].d_value;
                int LENGTH = DATA[i].d_length;

                for (int j = 0; j < DUFF_SIZE; ++j) {
                    int HALF_LENGTH = LENGTH / 2;

                    bsl::memset(dd, VALUE, HALF_LENGTH);
                    bsl::memset(dd + HALF_LENGTH, 0x0d, HALF_LENGTH);
                    ddStart = bsls_TimeUtil::getTimer();
                    bdeimp_DuffsDevice<char>::assign(dd + j,
                                                     dd + j + HALF_LENGTH,
                                                     HALF_LENGTH - j);
                    ddStop  = bsls_TimeUtil::getTimer();

                    bsl::memset(mm, VALUE, HALF_LENGTH);
                    bsl::memset(mm + HALF_LENGTH, 0x0d, HALF_LENGTH);
                    mmStart = bsls_TimeUtil::getTimer();
                    bsl::memmove(mm + j,
                                 mm + j + HALF_LENGTH,
                                 HALF_LENGTH - j);
                    mmStop  = bsls_TimeUtil::getTimer();

                    bsls_PlatformUtil::Int64 ddTime = ddStop - ddStart;
                    bsls_PlatformUtil::Int64 mmTime = mmStop - mmStart;
                    LOOP3_ASSERT(i, j, LINE, mmTime <= ddTime);

                    if (verbose) {
                        P_(i); P_(j); P_(LINE);
                        P_(LENGTH); P_(mmTime); P(ddTime);
                    }
                }
            }

            delete[] dd;  delete[] mm;
        }
      }  break;
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
