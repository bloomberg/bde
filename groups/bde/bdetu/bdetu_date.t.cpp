// bdetu_date.t.cpp           -*-C++-*-

#include <bdetu_date.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a static member function (pure
// procedure) that computes a hash value for 'date's.  The general plan is that
// the method is tested against a set of tabulated test vectors.
//-----------------------------------------------------------------------------
// [ 1] static int hash(int value, int size);  CONCERN: value
// [ 2] static int hash(int value, int size);  CONCERN: distribution
// [ 3] static bool isValidYYYYMMDD(int dataValue);
// [ 4] static bdet_Date fromYyyyMmDd(int dataValue);       CONCERN: value
// [ 4] static bdet_Date convertFromYYYYMMDDRaw(int dataValue); CONCERN: value
// [ 4] static bdet_Date convertFromYYYYMMDD(int dataValue);CONCERN: value
// [ 5] static int toYyyyMmDd(bdet_Date value);             CONCERN: value
// [ 5] static int convertToYYYYMMDD(bdet_Date value);      CONCERN: value
//-----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdetu_Date Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage examples provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage examples provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
            if (verbose) bsl::cout << "\nUsage Example 1" << endl;

            const int date   = 20091106;
            bdet_Date result = bdetu_Date::convertFromYYYYMMDDRaw(date);

            if (veryVerbose) {
                bsl::cout << result << bsl::endl;
            }

            const int badDate = 20091131;
            if (bdetu_Date::convertFromYYYYMMDD(&result, badDate)) {
              if (veryVerbose) {
                bsl::cout << "Bad date: "           << badDate << bsl::endl;
                bsl::cout << "'result' unchanged: " << result  << bsl::endl;
              }
            }
        }

        {
            if (verbose) bsl::cout << "\nUsage Example 2" << endl;

            bdet_Date date(2009, 11, 06);
            int dateYYYYMMDD = bdetu_Date::convertToYYYYMMDD(date);

            if (veryVerbose) {
                bsl::cout << "Date in YYYYMMDD: " << dateYYYYMMDD << bsl::endl;
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING toYyyyMmDd METHOD (VALUE) :
        //   Verify the conversion method performs as expected over a range of
        //   inputs.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'toYyyyMmDd'" << endl
                 << "====================" << endl;

        int resultOld;
        int resultNew;
        resultOld = Obj::toYyyyMmDd(bdet_Date());
        resultNew = Obj::convertToYYYYMMDD(bdet_Date());
        ASSERT(10101 == resultOld);
        ASSERT(10101 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2006, 10, 18));
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2006, 10, 18));
        ASSERT(20061018 == resultOld);
        ASSERT(20061018 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2004, 9, 1));
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2004, 9, 1));
        ASSERT(20040901 == resultOld);
        ASSERT(20040901 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2004, 9, 1) - 1);
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2004, 9, 1) - 1);
        ASSERT(20040831 == resultOld);
        ASSERT(20040831 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2004, 12, 31) + 1);
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2004, 12, 31) + 1);
        ASSERT(20050101 == resultOld);
        ASSERT(20050101 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2004, 2, 28) + 1);
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2004, 2, 28) + 1);
        ASSERT(20040229 == resultOld);
        ASSERT(20040229 == resultNew);

        resultOld = Obj::toYyyyMmDd(bdet_Date(2003, 2, 28) + 1);
        resultNew = Obj::convertToYYYYMMDD(bdet_Date(2003, 2, 28) + 1);
        ASSERT(20030301 == resultOld);
        ASSERT(20030301 == resultNew);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING fromYyyyMmDd, convertFromYYYYMMDD METHOD (VALUE) :
        //   Verify the conversion method performs as expected over a range of
        //   inputs.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'fromYyyyMmDd, convertFromYYYYMMDD'" << endl
                 << "===========================================" << endl;

        {
            bdet_Date resultOld;
            bdet_Date resultNew;

            resultOld = Obj::convertFromYYYYMMDDRaw(20061018);
            resultNew = Obj::convertFromYYYYMMDD(20061018);
            ASSERT(bdet_Date(2006, 10, 18) == resultOld);
            ASSERT(bdet_Date(2006, 10, 18) == resultNew);

            resultOld = Obj::convertFromYYYYMMDDRaw(20040901);
            resultNew = Obj::convertFromYYYYMMDD(20040901);
            ASSERT(bdet_Date(2004, 9, 1) == resultOld);
            ASSERT(bdet_Date(2004, 9, 1) == resultNew);

            resultOld = Obj::convertFromYYYYMMDDRaw(19990131);
            resultNew = Obj::convertFromYYYYMMDD(19990131);
            ASSERT(bdet_Date(1999, 1, 31) == resultOld);
            ASSERT(bdet_Date(1999, 1, 31) == resultNew);

            resultOld = Obj::convertFromYYYYMMDDRaw(10101);
            resultNew = Obj::convertFromYYYYMMDD(10101);
            ASSERT(bdet_Date(1, 1, 1) == resultOld);
            ASSERT(bdet_Date(1, 1, 1) == resultNew);
        }

        {
            bdet_Date resultOld;
            bdet_Date resultNew;

            resultOld = Obj::fromYyyyMmDd(20061018);
            resultNew = Obj::convertFromYYYYMMDDRaw(20061018);
            ASSERT(bdet_Date(2006, 10, 18) == resultOld);
            ASSERT(bdet_Date(2006, 10, 18) == resultNew);

            resultOld = Obj::fromYyyyMmDd(20040901);
            resultNew = Obj::convertFromYYYYMMDDRaw(20040901);
            ASSERT(bdet_Date(2004, 9, 1) == resultOld);
            ASSERT(bdet_Date(2004, 9, 1) == resultNew);

            resultOld = Obj::fromYyyyMmDd(19990131);
            resultNew = Obj::convertFromYYYYMMDDRaw(19990131);
            ASSERT(bdet_Date(1999, 1, 31) == resultOld);
            ASSERT(bdet_Date(1999, 1, 31) == resultNew);

            resultOld = Obj::fromYyyyMmDd(10101);
            resultNew = Obj::convertFromYYYYMMDDRaw(10101);
            ASSERT(bdet_Date(1, 1, 1) == resultOld);
            ASSERT(bdet_Date(1, 1, 1) == resultNew);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'isValidYYYYMMDD'
        //
        // Concerns:
        //   That the method under test reports the correct result for both
        //   valid and invalid inputs.
        //
        // Plan:
        //   Use the table-driven approach to test representative valid and
        //   invalid inputs and assert that the function under test returns
        //   the expected result.
        //
        // Testing:
        //   static bool isValidYYYYMMDD(int dataValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'isValidYYYYMMDD'" << endl
                          << "=========================" << endl;

        static const struct {
            int  d_lineNum;    // source line number
            int  d_dateValue;  // date value ('YYYYMMDD' format)
            bool d_expected;   // expected result

        } DATA[] = {
            //lin   input value   expected
            //---   -----------   --------
            { L_,       10101,    true     },
            { L_,       10102,    true     },
            { L_,       10201,    true     },
            { L_,       20101,    true     },
            { L_,    19540917,    true     },
            { L_,    99980101,    true     },
            { L_,    99981231,    true     },
            { L_,    99991130,    true     },
            { L_,    99991230,    true     },
            { L_,    99991231,    true     },

            { L_,           0,    false    },
            { L_,        1231,    false    },
            { L_,    19540017,    false    },
            { L_,    19541317,    false    },
            { L_,    19540900,    false    },
            { L_,    19540931,    false    },
            { L_,    99991232,    false    },
            { L_,    99991331,    false    },
            { L_,   100000917,    false    },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE = DATA[ti].d_lineNum;
            const int  DATE = DATA[ti].d_dateValue;
            const bool EXP  = DATA[ti].d_expected;

            if (veryVerbose) { P_(DATE);  P(EXP); }

            LOOP2_ASSERT(LINE, DATE, EXP == Obj::isValidYYYYMMDD(DATE));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HASH OPERATOR (DISTRIBUTION) :
        //   Verify the hash function generates a good distribution of return
        //   values over a wide range of 'size' values.  Specifically, verify
        //   that for all x such that 0 <= x < size, x * 2^k for a fixed 'k' is
        //   perfectly distributed (every hash value is returned exactly once).
        //
        // Plan:
        //   Perform a large number of hashes and verifying the distribution.
        //
        // Testing:
        //   static int hash(int value, int size);  CONCERN: distribution
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'hash'" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(const bdet_Date& value,"
                             "int size)' "
                          << "distribution." << endl;
        {
            { // test distribution for shifted bits
                const int size = 257;
                int *hit = new int[size];
                int i;
                for (i = 0; i < size; ++i) {
                    hit[i] = 0;
                }
                for (int k = 0; k <= 13; ++k) { // keep dates in valid range
                    for (i = 0; i < size; ++i) {
                        bdet_Date date(1, 1, 1);
                        date += i << k;
                        ++hit[Obj::hash(date, size)];
                    }
                    for (i = 0; i < size; ++i) {
                        LOOP2_ASSERT(k, i, k + 1 == hit[i]);
                    }
                }
                delete [] hit;
            }
            { // test distribution for varying sizes
                static const int DATA[] = { 1, 5, 23, 257, 65537 };
                static const int NUM_DATA = sizeof DATA / sizeof *DATA;
                int i;
                for (int m = 1; m <= 4; m++) {
                    for (int si = 0; si < NUM_DATA; ++si) {
                        int size = DATA[si];
                        int *hit = new int[size];
                        for (i = 0; i < size; ++i) {
                            hit[i] = 0;
                        }
                        for (i = 0; i < m*size; ++i) {
                            bdet_Date date(1, 1, 1);
                            date += i;
                            ++hit[Obj::hash(date, size)];
                        }
                        for (i = 0; i < size; ++i) {
                            LOOP4_ASSERT(m, size, i, hit[i], m == hit[i]);
                        }
                        delete [] hit;
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HASH OPERATOR (VALUE):
        //   Verify the hash return value is constant across all platforms for
        //    a given input.
        //
        // Plan:
        //   Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   static int hash(int value, int size);  CONCERN: value
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash'" << endl
                 << "==============" << endl;

        if (verbose) cout << "\nTesting 'hash(int value, int size)' "
                          << "return value." << endl;
        {
            static const struct {
                int d_lineNum;     // source line number
                int d_year;        // value to hash
                int d_month;       // value to hash
                int d_day;         // value to hash
                int d_size;        // size of target hash table
                int d_exp;         // expected return value
            } DATA[] = {
                //line  year  month  day   size    exp
                //----  ----  -----  ---  ------  ------
                { L_,      1,     1,   1,    257,      0 },
                { L_,      1,     1,  10,    257,      9 },
                { L_,      1,     1,  31,    257,     30 },
                { L_,      1,     2,   1,    257,     31 },
                { L_,      1,     2,  17,    257,     47 },
                { L_,      1,     2,  28,    257,     58 },
                { L_,      1,    12,   1,    257,     77 },
                { L_,      1,    12,   9,    257,     85 },
                { L_,      1,    12,  31,    257,    107 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int SIZE  = DATA[ti].d_size;
                const int EXP   = DATA[ti].d_exp;

                bdet_Date date(YEAR, MONTH, DAY);

                if (veryVerbose) {
                    P_(date);
                    P_(SIZE);
                    P(EXP);
                    P(Obj::hash(date, SIZE));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, EXP == Obj::hash(date, SIZE));
            }
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
