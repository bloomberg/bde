// bdeimp_bitwisecopy.t.cpp         -*-C++-*-

#include <bdeimp_bitwisecopy.h>

#include <bsls_alignmentutil.h>  // for testing only
#include <bsls_platform.h>       // for testing only

#include <bsl_cstdlib.h>         // atoi()
#include <bsl_cstring.h>         // memset(), memcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>
#include <bsl_new.h>             // placement syntax

#ifdef BSLS_PLATFORM__OS_UNIX
#include <sys/times.h>  // times()
#include <bsl_climits.h>      // times()
#include <bsl_ctime.h>        // CLK_TCK
#endif

#ifdef BSLS_PLATFORM__OS_LINUX
    #ifndef CLK_TCK
        #define CLK_TCK CLOCKS_PER_SEC
    #endif
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a static member function
// (pure procedure) that performs a bitwise copy on an object.
// The bitwise copy function takes advantage of the compile time knowledge
// of the object's length and alignment.
// The copy function is tested with various structs containing arrays of
// different lengths and different fundamental types.
// A performance test can be optionally performed when run on a UNIX platform.
//----------------------------------------------------------------------------
// [ 1 ]  bdeimp_BitwiseCopy<T>::copy(d, s);
// [ 2 ]  Performance test
// [ 3 ]  USAGE
//============================================================================


//============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//----------------------------------------------------------------------------

# define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                    \
    if (!(X)) { cout << #I << ": " << I << "\t"  \
                     << #J << ": " << J << "\n";  aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                             \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
                     << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                           \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
        aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                         \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\t"  \
                     << #M << ": " << M << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) {                                       \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\t"  \
                     << #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
        aSsErT(1, #X, __LINE__); } }

//============================================================================
#define P(X)  cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X)  cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                            // current Line number
//----------------------------------------------------------------------------

static int memendcmp (void *buffer, int cursor, int bufferSize, char fill)
    // Return 1 if any character in the specified 'buffer' of the specified
    // 'bufferSize', beginning at the specified 'cursor', does *not* match
    // the specified 'fill' character, and 0 otherwise.
{
    for (int i = cursor; i < bufferSize; ++i) {
        if (((char *) buffer)[i] != fill) return 1;
    }

    return 0;
}

int avoidopt (int *i, void *ptr, void *ptr2) {
    // This function is called to avoid optimization of the test loops
    // by the compiler.
    return ptr == ptr2 ? *i : 0;
}

template <class T, int iter>
inline void performtest(const char *name, char *testsource, char *testdest)
{
#ifdef BSLS_PLATFORM__OS_UNIX
        struct tms startTime;
        struct tms endmemcpyTime;
        struct tms endTime;
        T& TS = *(T *) testsource;
        T& TD = *(T *) testdest;
        times(&startTime);
        for (volatile int i = 0; i < iter; ++i) {
            memcpy(&TD, &TS, sizeof(TS));
            avoidopt((int *) &i, &TD,  &TS);
            memcpy(&TD, &TS, sizeof(TS));
            avoidopt((int *) &i, &TD,  &TS);
            memcpy(&TD, &TS, sizeof(TS));
            avoidopt((int *) &i, &TD,  &TS);
            memcpy(&TD, &TS, sizeof(TS));
            avoidopt((int *) &i, &TD,  &TS);
            memcpy(&TD, &TS, sizeof(TS));
            avoidopt((int *) &i, &TD,  &TS);
        }
        times(&endmemcpyTime);
        for (volatile int i = 0; i < iter; ++i) {
            bdeimp_BitwiseCopy<T>::copy(&TD, &TS);
            avoidopt((int *) &i, &TD,  &TS);
            bdeimp_BitwiseCopy<T>::copy(&TD, &TS);
            avoidopt((int *) &i, &TD,  &TS);
            bdeimp_BitwiseCopy<T>::copy(&TD, &TS);
            avoidopt((int *) &i, &TD,  &TS);
            bdeimp_BitwiseCopy<T>::copy(&TD, &TS);
            avoidopt((int *) &i, &TD,  &TS);
            bdeimp_BitwiseCopy<T>::copy(&TD, &TS);
            avoidopt((int *) &i, &TD,  &TS);
        }
        times(&endTime);
        cout << "data: " << name;
        cout << " utime: memcpy " <<
             float(endmemcpyTime.tms_utime - startTime.tms_utime) / CLK_TCK
             << " bitwise copy: "<<
             float(endTime.tms_utime - endmemcpyTime.tms_utime) / CLK_TCK
             << ". "  << iter << " iterations" <<  endl;
#else
        cout << "data: "
             << name << " test not performed on this platform" << endl;
#endif
}

//============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//----------------------------------------------------------------------------

struct testMixed1 {
    int     d_int;
    void   *d_ptr;
    int     d_int2;
};

struct testMixed2 {
    char    d_char;
    short   d_short;
    int     d_int;
};

struct testDouble3 {
    double d_double[3];
};

struct testInt {
    int    d_int;
};

struct testInt4 {
    int    d_int[4];
};

struct testInt7 {
    int    d_int[7];
};

struct testInt8 {
    int    d_int[8];
};

struct testInt10 {
    int    d_int[10];
};

struct testInt20 {
    int    d_int[20];
};

struct testInt30 {
    int    d_int[30];
};

struct testInt43 {
    int    d_int[43];
};

struct testInt83 {
    int    d_int[83];
};

struct testInt143 {
    int    d_int[143];
};

struct testInt1001 {
    int    d_int[1001];
};

struct testInt2003 {
    int    d_int[2003];
};

struct testChar {
    char    d_char;
};

struct testChar4 {
    char    d_char[4];
};

struct testChar8 {
    char    d_char[8];
};

struct testChar10 {
    char    d_char[10];
};

struct testChar43 {
    char    d_char[43];
};

struct testChar143 {
    char    d_char[143];
};

struct testChar1001 {
    char    d_char[1001];
};

struct testChar2003 {
    char    d_char[2003];
};

// USAGE Example

struct MyObject {
    char d_char;
    int d_int;
};


template <class TYPE>
class my_Array {
   // A fixed size templatized array.

   TYPE *d_array_p;      // dynamically allocated array (d_length elements)
   int d_length;         // length of this array (in elements)

 public:
   // CREATORS
   my_Array(int length);

   ~my_Array();

   // MANIPULATORS
   void swap(int index1, int index2);
       // Swap the two elements at the specified 'index1' and
       // 'index2'

   TYPE& operator[](int index);
       // return a reference to the element at the specified 'index'.

};

// CREATORS

template <class TYPE>
my_Array<TYPE>::my_Array(int length) : d_length(length)
{
   d_array_p = (TYPE *) new char[sizeof(TYPE) * length];
   for (int i = 0; i < length; ++i) {
       new (&d_array_p[i]) TYPE();
   }
}

template <class TYPE>
my_Array<TYPE>::~my_Array()
{
   for (int i = 0; i < d_length; ++i) {
       d_array_p[i].~TYPE();
   }
   delete [](char *)d_array_p;
}

template <class TYPE>
TYPE& my_Array<TYPE>::operator[](int index)
{
   return *(d_array_p + index);
}

template <class TYPE>
inline
void my_Array<TYPE>::swap(int index1, int index2)
{
    bdeimp_BitwiseCopy<TYPE>::swap(d_array_p + index1, d_array_p + index2);
}


struct Example {
   // This is the example 'struct' used as the template parameter
   // for the 'my_Array'.
   int d_data;

   // CREATORS
   Example(int value = 0) : d_data(value) {};

 private:
   // Copy constructor and assignment operator are not implemented.
   Example(const Example&);
   Example& operator=(const Example&);
};


//============================================================================
//                                MAIN PROGRAM
//----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;


    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   The streaming is converted from cout to strstream.  Additional
        //   code has been included in the container class.
        //
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        if (verbose) cout << "Usage Example 1" << endl;

        MyObject srcObj = { 'a', 10 };          ASSERT('a' == srcObj.d_char);
                                                ASSERT( 10 == srcObj.d_int);
        MyObject dstObj = { 'm', -1 };          ASSERT('m' == dstObj.d_char);
                                                ASSERT( -1 == dstObj.d_int);
        const MyObject *srcAddr = &srcObj;
        MyObject       *dstAddr = &dstObj;

        bdeimp_BitwiseCopy<MyObject>::copy(dstAddr, srcAddr);
                                                ASSERT('a' == srcObj.d_char);
                                                ASSERT( 10 == srcObj.d_int);
                                                ASSERT('a' == dstObj.d_char);
                                                ASSERT( 10 == dstObj.d_int);

        dstObj.d_char = 'm';
        dstObj.d_int  = -1;

        bdeimp_BitwiseCopy<MyObject>::swap(&srcObj, &dstObj);
                                                ASSERT('m' == srcObj.d_char);
                                                ASSERT( -1 == srcObj.d_int);
                                                ASSERT('a' == dstObj.d_char);
                                                ASSERT( 10 == dstObj.d_int);
        bdeimp_BitwiseCopy<MyObject>::swap(&srcObj, &dstObj);
                                                ASSERT('a' == srcObj.d_char);
                                                ASSERT( 10 == srcObj.d_int);
                                                ASSERT('m' == dstObj.d_char);
                                                ASSERT( -1 == dstObj.d_int);

        if (verbose) cout << "Usage Example 2" << endl;

        my_Array<Example> array(10);

        int i;
        for (i = 0; i < 10; ++i) array[i].d_data = 33 - 2 * i;

        char buf1[100];
        ostrstream out1(buf1, sizeof buf1);

        out1 << "before swap: ";
        for (i = 0; i < 10; ++i) out1 << array[i].d_data << ' ';
        out1 << '\n' << ends;
        if (verbose) cout << buf1;

        const char *BEFORE ="before swap: 33 31 29 27 25 23 21 19 17 15 \n";
        ASSERT(strcmp(buf1, BEFORE) == 0);

        array.swap(0, 2);

        char buf2[100];
        ostrstream out2(buf2, sizeof buf2);

        out2 << "after swap:  ";
        for (i = 0; i < 10; ++i) out2 << array[i].d_data << ' ';
        out2 << '\n' << ends;
        if (verbose) cout << buf2;

        const char *EXPECTED ="after swap:  29 31 33 27 25 23 21 19 17 15 \n";
        ASSERT(strcmp(buf2, EXPECTED) == 0);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PERFORMANCE MEASUREMENT
        //   This test measures performance of the copy function.  The
        //   performance test is only run when in verbose mode.  The second
        //   program parameter should be 'perf' to run the performance test.
        //   The third program parameter (verbose parameter) is used to specify
        //   the test to perform.  -1 means run all tests.  0 means don't run
        //   any tests.  A positive integer indicates the test number.
        //   The default is don't run any performance tests.
        //   This test is only performed on UNIX platforms.
        //
        // Testing:
        //   PERFORMANCE
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_UNIX
        union Align {
          char                               d_bytes[100000];
          bsls_AlignmentUtil::MaxAlignedType d_align;   // only used for
                                                        // alignment
        } testsource, testdest;

        struct tms startTime;
        struct tms endTime;

        // Usage: <Program> 2 perf <test-number>
        int performanceTest = (argc > 3 && strcmp(argv[2], "perf") == 0)
            ? atoi(argv[3]) : 0;

        if (performanceTest != 0) {
            int startTest = performanceTest;
            int endTest = performanceTest;
            if (startTest < 0) {
                startTest = 1;
                endTest = 1000;
            }
            if (endTest == 0) endTest = -1;
            for (int perfTest = startTest; perfTest <= endTest; ++perfTest) {
            cout << "Performance test: " << perfTest << endl;
            switch (perfTest) {
              case 20: {
                performtest<testInt,20000000>("Int1", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt4,20000000>("Int4", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt8,20000000>("Int8", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt43,20000000>("Int43", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt143,2000000>("Int143", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt1001,200000>("Int1001", (char *)&testsource,
                  (char *)&testdest);
                performtest<testInt2003,200000>("Int2003", (char *)&testsource,
                  (char *)&testdest);

                performtest<testChar,20000000>("Char1", (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar4,20000000>("Char4", (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar8,20000000>("Char8", (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar43,20000000>("Char43", (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar143,2000000>("Char143",
                  (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar1001,200000>("Char1001",
                  (char *)&testsource,
                  (char *)&testdest);
                performtest<testChar2003,200000>("Char2003",
                  (char *)&testsource,
                  (char *)&testdest);
              } break;

              case 19: {
                testChar143& TS = *(testChar143 *) &testsource;
                testChar143& TD = *(testChar143 *) &testdest;
                cout << "data: char[143] algorithm: memcpy\n";

                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 18: {
                testChar43& TS = *(testChar43 *) &testsource;
                testChar43& TD = *(testChar43 *) &testdest;
                cout << "data: char[43] algorithm: memcpy\n";

                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 17: {
                testChar10& TS = *(testChar10 *) (((char *) &testsource) + 1);
                testChar10& TD = *(testChar10 *) &testdest;
                cout << "data: char[10] not int aligned algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 16: {
                testChar10& TS = *(testChar10 *) &testsource;
                testChar10& TD = *(testChar10 *) &testdest;
                times(&startTime);
                cout << "data: char[10] algorithm: memcpy\n";
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 15: {
                testChar4& TS = *(testChar4 *) (((char *) &testsource) + 1);
                testChar4& TD = *(testChar4 *) &testdest;
                cout << "data: char[4] not int aligned algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 14: {
                testChar4& TS = *(testChar4 *) &testsource;
                testChar4& TD = *(testChar4 *) &testdest;
                cout << "data: char[4] algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 13: {
                testInt143& TS = *(testInt143 *) &testsource;
                testInt143& TD = *(testInt143 *) &testdest;
                cout << "data: int[143] algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 200000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 200000 iterations" <<  endl;
              } break;
              case 12: {
                testInt10& TS = *(testInt10 *) &testsource;
                testInt10& TD = *(testInt10 *) &testdest;
                cout << "data: int[10] algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 11: {
                testInt4& TS = *(testInt4 *) &testsource;
                testInt4& TD = *(testInt4 *) &testdest;
                cout << "data: int[4] algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 10: {
                testInt& TS = *(testInt *) &testsource;
                testInt& TD = *(testInt *) &testdest;
                cout << "data: int[1] algorithm: memcpy\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                    memcpy(&TD, &TS, sizeof(TS));
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 9: {
                testChar143& TS = *(testChar143 *) &testsource;
                testChar143& TD = *(testChar143 *) &testdest;
                cout << "data: char[143] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testChar143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 8: {
                testChar43& TS = *(testChar43 *) &testsource;
                testChar43& TD = *(testChar43 *) &testdest;
                cout << "data: char[43] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testChar43>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar43>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar43>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar43>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar43>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 7: {
                testChar10& TS = *(testChar10 *) &testsource;
                testChar10& TD = *(testChar10 *) &testdest;
                cout << "data: char[10] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 6: {
                testChar4& TS = *(testChar4 *) &testsource;
                testChar4& TD = *(testChar4 *) &testdest;
                cout << "data: char[4] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 5: {
                testInt143& TS = *(testInt143 *) &testsource;
                testInt143& TD = *(testInt143 *) &testdest;
                cout << "data: int[143] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 200000; ++i) {
                    bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 200000 iterations" <<  endl;
              } break;
              case 4: {
                testInt10& TS = *(testInt10 *) &testsource;
                testInt10& TD = *(testInt10 *) &testdest;
                cout << "data: int[10] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 3: {
                testInt4& TS = *(testInt4 *) &testsource;
                testInt4& TD = *(testInt4 *) &testdest;
                cout << "data: int[4] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 2: {
                testInt& TS = *(testInt *) &testsource;
                testInt& TD = *(testInt *) &testdest;
                cout << "data: int[1] algorithm: bitwiseCopy function\n";
                times(&startTime);
                for (volatile int i = 0; i < 20000000; ++i) {
                    bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              case 1: {
                times(&startTime);
                int TS, TD;
                cout << "control\n";
                for (volatile int i = 0; i < 20000000; ++i) {
                    avoidopt((int *) &i, &TD,  &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                    avoidopt((int *) &i, &TD,  &TS);
                }
                times(&endTime);
                cout << "user time: " <<
                     float(endTime.tms_utime - startTime.tms_utime) / CLK_TCK
                     << " sec. 20000000 iterations" <<  endl;
              } break;
              default: {
                    perfTest = endTest + 1;
              } break;
              }
          }
          }
#else
          if (verbose)
                cout << "No performance test performed on this platform"
                     << endl;
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Concerns:
        //   1) The function copies the data correctly for both char and
        //      int aligned data.
        //   2) The function copies the data correctly for short and long
        //      structures.
        //   3) Too much data should not be copied.
        //   4) The original data should be left unchanged.
        //   5) The function operates correctly for structs that are not
        //      physically int aligned.
        //
        // Plan:
        //   Invoke the function for a variety of structs including those
        //   that are not physically int aligned.
        //   Pad the source struct with a fix value.
        //   Verify after the function is invoked that:
        //    a) the original is unchanged by comparing to the control copy.
        //    b) the padding beyond the original is unchanged.
        //    c) the destination contains a copy of the original.
        //    d) the padding beyond the copy is unchanged.
        //
        // Testing:
        //    bdeimp_BitwiseCopy<T>::copy(d, s);
        //
        // --------------------------------------------------------------------
        union Align {
          char                               d_bytes[4096];
          bsls_AlignmentUtil::MaxAlignedType d_align;   // only used for
                                                        // alignment
        } testsource, testdest;

        if (verbose) cout << endl
                          << "testing copy of int struct. int aligned" << endl;
        {
            testInt TC;            // control
            testInt& TS = *(testInt *) &testsource;
            testInt& TD = *(testInt *) (sizeof(int) + (char *) &testdest);
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_int = 1; TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest) - sizeof(int),
                (char)0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource),
                (char)0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of int[4] struct." << endl;
        {
            testInt4 TC;            // control
            testInt4& TS = *(testInt4 *) &testsource;
            testInt4& TD = *(testInt4 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_int[0] = 1;
            TC.d_int[1] = -1;
            TC.d_int[2] = 100;
            TC.d_int[3] = 5;
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt4>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of int[7] struct." << endl;
        {
            testInt7 TC;            // control
            testInt7& TS = *(testInt7 *) &testsource;
            testInt7& TD = *(testInt7 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_int[0] = 1;
            TC.d_int[1] = -1;
            TC.d_int[2] = 100;
            TC.d_int[3] = 5;
            TC.d_int[4] = 3;
            TC.d_int[5] = 2;
            TC.d_int[6] = 9;
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt7>::copy(&TD, &TS);

            ASSERT (memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT (memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of int[8] struct." << endl;
        {
            testInt8 TC;            // control
            testInt8& TS = *(testInt8 *) &testsource;
            testInt8& TD = *(testInt8 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_int[0] = 1;
            TC.d_int[1] = -1;
            TC.d_int[2] = 100;
            TC.d_int[3] = 5;
            TC.d_int[4] = 3;
            TC.d_int[5] = 2;
            TC.d_int[6] = 9;
            TC.d_int[7] = 10;
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt8>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of int[10] struct." << endl;
        {
            testInt10 TC;            // control
            testInt10& TS = *(testInt10 *) &testsource;
            testInt10& TD = *(testInt10 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_int[0] = 1;
            TC.d_int[1] = -1;
            TC.d_int[2] = 100;
            TC.d_int[3] = 5;
            TC.d_int[4] = 3;
            TC.d_int[5] = 2;
            TC.d_int[6] = 9;
            TC.d_int[7] = 10;
            TC.d_int[8] = 0;
            TC.d_int[9] = 11;
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt10>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of int[143] struct." << endl;
        {
            testInt143 TC;            // control
            testInt143& TS = *(testInt143 *) &testsource;
            testInt143& TD = *(testInt143 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            for (unsigned i = 0; i < sizeof(TC.d_int) / sizeof(int); ++i) {
               TC.d_int[i] = i + 1;
            }
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testInt143>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                          << "testing copy of double[3] struct." << endl;
        {
            testDouble3 TC;            // control
            testDouble3& TS = *(testDouble3 *) &testsource;
            testDouble3& TD = *(testDouble3 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_double[0] = 99.0;
            TC.d_double[1] = -1.1;
            TC.d_double[2] = 1000.99;

            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testDouble3>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                     << "testing copy of char struct. int aligned" << endl;
        {
            testChar TC;            // control
            testChar& TS = *(testChar *) &testsource;
            testChar& TD = *(testChar *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char = 'a'; TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                     << "testing copy of char struct. char aligned" << endl;
        {
            testChar TC;            // control
            testChar& TS = *(testChar *) &testsource;
            testChar& TD = *(testChar *) (1 + (char *) &testdest);
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char = 'a'; TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest) - 1, 0x7E)
                    == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F)
                    == 0);
        }

        if (verbose) cout << endl
                     << "testing copy of char[4] struct. int aligned" << endl;
        {
            testChar4 TC;            // control
            testChar4& TS = *(testChar4 *) &testsource;
            testChar4& TD = *(testChar4 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char[0] = 'a';
            TC.d_char[1] = 'b';
            TC.d_char[2] = 'f';
            TC.d_char[3] = 'g';
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                     << "testing copy of char[4] struct. char aligned" << endl;
        {
            testChar4 TC;            // control
            testChar4& TS = *(testChar4 *) &testsource;
            testChar4& TD = *(testChar4 *) (1 + (char *) &testdest);
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char[0] = 'a';
            TC.d_char[1] = 'b';
            TC.d_char[2] = 'f';
            TC.d_char[3] = 'g';
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar4>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD),
                sizeof(testdest) - 1, 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS),
                sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
                     << "testing copy of char[10] struct. int aligned" << endl;
        {
            testChar10 TC;            // control
            testChar10& TS = *(testChar10 *) &testsource;
            testChar10& TD = *(testChar10 *) &testdest;
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char[0] = 'a';
            TC.d_char[1] = 'b';
            TC.d_char[2] = 'f';
            TC.d_char[3] = 'g';
            TC.d_char[4] = 'h';
            TC.d_char[5] = 'r';
            TC.d_char[6] = 't';
            TC.d_char[7] = 'u';
            TC.d_char[8] = 'v';
            TC.d_char[9] = 'w';
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD),
                sizeof(testdest), 0x7E) == 0);
            ASSERT(memendcmp (&TS, sizeof(TS),
                sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl
             << "testing copy of char[10] struct. char aligned" << endl;
        {
            testChar10 TC;            // control
            testChar10& TS = *(testChar10 *) &testsource;
            testChar10& TD = *(testChar10 *) (1 + (char *) &testdest);
            memset(&testsource, 0x7F, sizeof(testsource));
            memset(&testdest, 0x7E, sizeof(testdest));

            TC.d_char[0] = 'a';
            TC.d_char[1] = 'b';
            TC.d_char[2] = 'f';
            TC.d_char[3] = 'g';
            TC.d_char[4] = 'h';
            TC.d_char[5] = 'r';
            TC.d_char[6] = 't';
            TC.d_char[7] = 'u';
            TC.d_char[8] = 'v';
            TC.d_char[9] = 'w';
            TS = TC;

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) != 0);

            bdeimp_BitwiseCopy<testChar10>::copy(&TD, &TS);

            ASSERT(memcmp(&TS, &TC, sizeof(TS)) == 0);
            ASSERT(memcmp(&TS, &TD, sizeof(TS)) == 0);

            // validate end of buffers
            ASSERT(memendcmp (&TD, sizeof(TD), sizeof(testdest) - 1, 0x7E)
                    == 0);
            ASSERT(memendcmp (&TS, sizeof(TS), sizeof(testsource), 0x7F) == 0);
        }

        if (verbose) cout << endl;
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
