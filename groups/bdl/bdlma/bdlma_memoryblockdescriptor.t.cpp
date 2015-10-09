// bdlma_memoryblockdescriptor.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlma_memoryblockdescriptor.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>  // for testing only
#include <bslma_testallocator.h>          // for testing only

#include <bsls_alignment.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>   // 'atoi'
#include <bsl_cstring.h>   // 'memcpy'

#include <bsl_c_stdio.h>   // 'snprintf' (<cstdio> doesn't declare 'snprintf')

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>             // 'pipe', 'close' and 'dup'.
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bdlma::MemoryBlockDescriptor' is a simple value object holding to
// properties that can be written and accessed freely, i.e., there are no
// constraints placed up their values.
//
// The basic manipulators for a memory block descriptor are the default
// constructor and the 'ptr' and 'size' operations.
//-----------------------------------------------------------------------------
// [ 3] bdlma::MemoryBlockDescriptor();
// [ 3] bdlma::MemoryBlockDescriptor(void *, bsl::size_t);
// [ 5] bdlma::MemoryBlockDescriptor(const bdlma::MemoryBlockDescriptor&);
// [ 2] ~bdlma::MemoryBlockDescriptor();
// [ 6] bdlma::MemoryBlockDescriptor& operator=(rhs);
// [ 7] bool isNull() const;
// [ 2] void *address() const;
// [ 2] size_type size() const;
// [ 8] bsl::ostream& print() const;
// [ 4] operator==(const bdlma::MemoryBlockDescriptor&,
//                 const bdlma::MemoryBlockDescriptor&);
// [ 4] operator!=(const bdlma::MemoryBlockDescriptor&,
//                 const bdlma::MemoryBlockDescriptor&);
// [ 8] operator<<(bsl::ostream&, const bdlma::MemoryBlockDescriptor&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdlma::MemoryBlockDescriptor Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define PRINT(X)        \
bsl::printf(#X);        \
bsl::printf(" = ");     \
X.print();              \
bsl::printf("\n");

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator         talloc;
    bslma::DefaultAllocatorGuard guard(&talloc);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

        if (verbose) cout << endl << "USAGE EXAMPLE"
                          << endl << "============="
                          << endl;

///Usage
///-----
// This example demonstrates how to create and test the state of a
// 'bdlma::MemoryBlockDescriptor'.
//..
     char buffer[100];

     bdlma::MemoryBlockDescriptor a(buffer, sizeof buffer);
                                 ASSERT(!a.isNull());
                                 ASSERT(buffer         == a.address());
                                 ASSERT(sizeof buffer  == a.size());

     bdlma::MemoryBlockDescriptor b;
                                 ASSERT( b.isNull());
                                 ASSERT(0              == b.address());
                                 ASSERT(0              == b.size());
                                 ASSERT(a              != b);

     b = a;
                                 ASSERT(!b.isNull());
                                 ASSERT(buffer         == b.address());
                                 ASSERT(sizeof buffer  == b.size());
                                 ASSERT(a              == b);

//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const bdlma::MemoryBlockDescriptor&);
        //   ostream& print(ostream&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;

// The following code uses pipe() and fork(), so only works on Unix.
#ifdef BSLS_PLATFORM_OS_UNIX

        struct {
            void           *d_address;
            Obj::size_type  d_size;
        } VALUES [] = {
            {           0,    0 },
            { (void *) 37,    0 },
            { (void *) 37,   32 },
            { (void *)500,   32 }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(VALUES[i].d_address, VALUES[i].d_size); const Obj& X = x;

            const int BUF_SZ = 1000;
            char mExp[BUF_SZ], buf[BUF_SZ];

            const char XX = (char) 0xFF;  // Value for an unset char.
            memset(mExp, XX, BUF_SZ);
            memset(buf, XX, BUF_SZ);

            snprintf(mExp, BUF_SZ, "[%p, %d]",
                     VALUES[i].d_address, static_cast<int>(VALUES[i].d_size));

            // Because 'bdema' is a low-level utility, 'MemoryBlockDescriptor'
            // does not have a function to print to ostream, and thus cannot
            // print to a 'strstream'.  The print() member function always
            // prints to 'stdout'.  The code below forks a process and captures
            // stdout to a memory buffer
            const char *EXP = mExp;
            int pipes[2];
            int sz;
            pipe(pipes);
            if (fork()) {
                // Parent process.  Read pipe[0] into memory
                sz = static_cast<int>(read(pipes[0], buf, BUF_SZ));
                if (sz >= 0) { buf[sz] = '\0'; }
            }
            else {
                // Child process, print to pipes[1].
                close(1);
                dup(pipes[1]);

                // This call print() function sends its output to the pipe,
                // which is in turn read into 'buf' by the parent process.
                X.print();

                exit(0);
            }

            if (veryVerbose) {
                cout << "\nEXPECTED FORMAT:"       << endl << EXP  << endl
                     << "\nACTUAL FORMAT (print):" << endl << buf << endl;
           }
            const int EXP_SZ = static_cast<int>(strlen(EXP));
            ASSERT(EXP_SZ < BUF_SZ);           // Check buffer is large enough.
            ASSERT(sz < BUF_SZ);               // Check buffer is large enough.
            ASSERT(0 < sz);                    // Check something was printed
            ASSERT(XX == buf[BUF_SZ - 1]);     // Check for overrun.

            ASSERT(0 == memcmp(buf, EXP, BUF_SZ));
        }
#endif

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ACCESSOR
        //
        // Concerns:
        //   isNull() properly reflects whether the descriptor points to null
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct a set of objects from the set
        //   and validate that the accessor returns the correct value.
        //
        // Testing:
        //   bool isNull() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting 'isNull'." << endl;

        int addr1;

        struct {
            void            *d_address;
            Obj::size_type   d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1, 500 },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(VALUES[i].d_address, VALUES[i].d_size); const Obj& X = x;

            bool isNull = VALUES[i].d_address == 0;

            ASSERT(isNull == X.isNull());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdlma::MemoryBlockDescriptor& operator=(rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator" << endl;

        int addr1;
        int addr2;
        int addr3;
        struct {
            void            *d_address;
            Obj::size_type   d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1,  90 },
            { &addr2,  43 },
            { &addr3, 123 },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj u(VALUES[i].d_address, VALUES[i].d_size); const Obj& U = u;
                Obj v(VALUES[j].d_address, VALUES[j].d_size); const Obj& V = v;
                Obj w(v); const Obj& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(VALUES[i].d_address, VALUES[i].d_size); const Obj& U = u;
            Obj w(U);  const Obj& W = w;              // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bdlma::MemoryBlockDescriptor(
        //                       const bdlma::MemoryBlockDescriptor& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor" << endl;

        int addr1;
        int addr2;
        int addr3;

        struct {
            void            *d_address;
            Obj::size_type   d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1,  90 },
            { &addr2,  43 },
            { &addr3, 123 },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj w(VALUES[i].d_address, VALUES[i].d_size); const Obj& W = w;
            Obj x(VALUES[i].d_address, VALUES[i].d_size); const Obj& X = x;

            Obj y(X); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const bdlma::MemoryBlockDescriptor&,
        //                   const bdlma::MemoryBlockDescriptor&);
        //   bool operator!=(const bdlma::MemoryBlockDescriptor&,
        //                   const bdlma::MemoryBlockDescriptor&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality." << endl;

        int addr1;
        int addr2;
        int addr3;

        struct {
            void            *d_address;
            Obj::size_type   d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1,  90 },
            { &addr2,  43 },
            { &addr3, 123 },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(VALUES[i].d_address, VALUES[i].d_size); const Obj& U = u;
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(VALUES[j].d_address, VALUES[j].d_size); const Obj& V = v;

                bool isEqual = i == j;
                ASSERT(isEqual  == (U == V));
                ASSERT(!isEqual == (U != V))
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Test the two varieties of constructors
        //
        // Plan:
        //   Verify the default constructor.
        //
        //   Next, for a sequence of independent test values, use the
        //   alternative constructor to create and object with a specific value
        //   and verify the values using a basic accessor.
        //
        // Testing:
        //   bdlma::MemoryBlockDescriptor()
        //   bdlma::MemoryBlockDescriptor(void *address, size_type size)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting constructors." << endl;

        {
            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            Obj x; const Obj& X = x;
            ASSERT(0 == X.address());
            ASSERT(0 == X.size());
        }
        if (veryVerbose) cout << "\tTesting secondary constructor." << endl;

        int addr1;
        int addr2;
        int addr3;
        struct {
            void            *d_address;
            Obj::size_type   d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1,  90 },
            { &addr2,  43 },
            { &addr3, 123 }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            void           *address = VALUES[i].d_address;
            Obj::size_type  size    = VALUES[i].d_size;

            Obj x(address, size); const Obj& X = x;

            ASSERT(address == X.address());
            ASSERT(size    == X.size());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   void *address() const;
        //   size_type size() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'ptr' and 'size'." << endl;

        int addr1;
        int addr2;
        int addr3;

        struct {
            void           *d_address;
            Obj::size_type  d_size;
        } VALUES [] = {
            {      0,   0 },
            { &addr1,   0 },
            { &addr1,   1 },
            { &addr1,  90 },
            { &addr2,  43 },
            { &addr3, 123 }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            void           *address = VALUES[i].d_address;
            Obj::size_type  size    = VALUES[i].d_size;

            Obj x; const Obj& X = x;
            x.setAddressAndSize(address, size);

            ASSERT(address == X.address());
            ASSERT(size    == X.size());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 5], and
        //   assignment operator without [7, 8] and with [9] aliasing.  Use the
        //   basic accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while 'U'
        //   denotes the valid, but "unknown", default object value.
        //
        //: 1.  Create an object x1 (init. to VA).  { x1:VA }
        //:
        //: 2.  Create an object x2 (copy from x1).  { x1:VA x2:VA }
        //:
        //: 3.  Set x1 to VB.  { x1:VB x2:VA }
        //:
        //: 4.  Create an object x3 (default ctor).  { x1:VB x2:VA x3:U }
        //:
        //: 5.  Create an object x4 (copy from x3).  { x1:VB x2:VA x3:U x4:U }
        //:
        //: 6.  Set x3 to 'VC'. { x1:VB x2:VA x3:VC x4:U }
        //:
        //: 7.  Assign x2 = x1.  { x1:VB x2:VB x3:VC x4:U }
        //:
        //: 8.  Assign x2 = x3.  { x1:VB x2:VC x3:VC x4:U }
        //:
        //: 9.  Assign x1 = x1 (aliasing).  { x1:VB x2:VB x3:VC x4:U }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Verify: bdlma::MemoryBlockDescriptor"
                          << endl << "==========================="
                          << endl;

        char VA[1];
        char VB[5];
        char VC[10];
        void *AA = VA; const int SA = sizeof(VA);  // address & size for 'VA'
        void *AB = VB; const int SB = sizeof(VB);  // address & size for 'VB'
        void *AC = VC; const int SC = sizeof(VC);  // address & size for 'VC'

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(AA, SA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  PRINT(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(AA == mX1.address());
        ASSERT(AA == X1.address());
        ASSERT(SA == mX1.size());
        ASSERT(SA == X1.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  PRINT(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(AA == mX2.address());
        ASSERT(AA == X2.address());
        ASSERT(SA == mX2.size());
        ASSERT(SA == X2.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setAddressAndSize(AB, SB);
        if (verbose) { cout << '\t';  PRINT(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(AB == mX1.address());
        ASSERT(AB == X1.address());
        ASSERT(SB == mX1.size());
        ASSERT(SB == X1.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  PRINT(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(0 == mX3.address());
        ASSERT(0 == X3.address());
        ASSERT(0 == mX3.size());
        ASSERT(0 == X3.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  PRINT(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == mX4.address());
        ASSERT(0 == X4.address());
        ASSERT(0 == mX4.size());
        ASSERT(0 == X4.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setAddressAndSize(AC, SC);

        if (verbose) { cout << '\t';  PRINT(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(AC == mX3.address());
        ASSERT(AC == X3.address());
        ASSERT(SC == mX3.size());
        ASSERT(SC == X3.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t';  PRINT(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(AB == mX2.address());
        ASSERT(AB == X2.address());
        ASSERT(SB == mX2.size());
        ASSERT(SB == X2.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t';  PRINT(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(AC == mX2.address());
        ASSERT(AC == X2.address());
        ASSERT(SC == mX2.size());
        ASSERT(SC == X2.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { cout << '\t';  PRINT(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(AB == mX1.address());
        ASSERT(AB == X1.address());
        ASSERT(SB == mX1.size());
        ASSERT(SB == X1.size());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
