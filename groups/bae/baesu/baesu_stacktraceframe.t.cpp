// baesu_stacktraceframe.t.cpp                                        -*-C++-*-

#include <baesu_stacktraceframe.h>

#include <baesu_objectfileformat.h>

#include <bdema_sequentialallocator.h>
#include <bdeu_string.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>    // atoi
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::ostream;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDescription'
//: o 'setDstInEffectFlag'
//: o 'setUtcOffsetInSeconds'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'description'
//: o 'dstInEffectFlag'
//: o 'utcOffsetInSeconds'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy assignment
//:   o swap
// ----------------------------------------------------------------------------
// CLASS METHODS
// [11] static bool isValidUtcOffsetInSeconds(int value);
//
// CREATORS
// [ 2] baetzo_LocalTimeDescriptor(bslma_Allocator *bA = 0);
// [ 3] baetzo_LocalTimeDescriptor(int o, bool f, const SRef& d, *bA = 0);
// [ 7] baetzo_LocalTimeDescriptor(const baetzo_LTDescriptor& o, *bA = 0);
// [ 2] ~baetzo_LocalTimeDescriptor();
//
// MANIPULATORS
// [ 9] operator=(const baetzo_LocalTimeDescriptor& rhs);
// [ 2] setDescription(const StringRef& value);
// [ 2] setDstInEffectFlag(bool value);
// [ 2] setUtcOffsetInSeconds(int value);
//
// [ 8] void swap(baetzo_LocalTimeDescriptor& other);
//
// ACCESSORS
// [ 4] bslma_Allocator *allocator() const;
// [ 4] const string& description() const;
// [ 4] bool dstInEffectFlag() const;
// [ 4] int utcOffsetInSeconds() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baetzo_LocalTimeDescriptor& lhs, rhs);
// [ 6] bool operator!=(const baetzo_LocalTimeDescriptor& lhs, rhs);
// [ 5] operator<<(ostream& s, const baetzo_LocalTimeDescriptor& d);
//
// FREE FUNCTIONS
// [ 8] void swap(baetzo_LocalTimeDescriptor& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 8] CONCERN: Precondition violations are detected when enabled.
// [10] Reserved for 'bslx' streaming.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baesu_StackTraceFrame Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL FUNCTIONS USED FOR TESTING
// ============================================================================

void cloneStrings(Obj *sfn, bslma_Allocator *alloc)
{
    const char *pc;

    pc = sfn->libraryFileName();
    if (pc) {
        sfn->setLibraryFileName(bdeu_String::copy(pc, alloc));
    }

    pc = sfn->sourceFileName();
    if (pc) {
        sfn->setSourceFileName(bdeu_String::copy(pc, alloc));
    }

    pc = sfn->mangledSymbolName();
    if (pc) {
        sfn->setMangledSymbolName(bdeu_String::copy(pc, alloc));
    }

    pc = sfn->symbolName();
    if (pc) {
        sfn->setSymbolName(bdeu_String::copy(pc, alloc));
    }
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);
    bslma_TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // baesu_StackTraceFrame USAGE EXAMPLE
        //
        // Concerns:
        //   Demonstrate stack trace frame basic functionality.
        //
        // Plan:
        //   Create a couple of stack trace frames, manipulate their fields,
        //   verify equality and inequality.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdema_StackTraceFrame usage example\n"
                             "===================================\n";

        // Here we create two 'baesu_StackTraceFrame' objects, modify their
        // properties, and compare them.  First, we create the objects 'a' and
        // 'b':

        bslma_TestAllocator ta;
        bdema_SequentialAllocator sa(&ta);

        baesu_StackTraceFrame a, b;

        ASSERT(a == b);

        // Next, we verify all values are initialized by the constructor to
        // invalid values.

        ASSERT(!a.isAddressValid());
        ASSERT(!a.isLibraryFileNameValid());
        ASSERT(!a.isLineNumberValid());
        ASSERT(!a.isMangledSymbolNameValid());
        ASSERT(!a.isOffsetFromSymbolValid());
        ASSERT(!a.isSourceFileNameValid());
        ASSERT(!a.isSymbolNameValid());

        a.setLineNumber(5);
        ASSERT(a.isLineNumberValid());
        ASSERT(a != b);
        b.setLineNumber(5);
        ASSERT(b.isLineNumberValid());
        ASSERT(a == b);

        a.setAddress((void *) 0x12345678);
        ASSERT(a.isAddressValid());
        ASSERT(a != b);
        b.setAddress(a.address());
        ASSERT(b.isAddressValid());
        ASSERT(a == b);

        a.setLibraryFileName("/usr/lib/woof.so");
        ASSERT(a.isLibraryFileNameValid());
        ASSERT(a != b);
        b.setLibraryFileName(bdeu_String::copy(a.libraryFileName(), &sa));
        ASSERT(b.isLibraryFileNameValid());
        ASSERT(a == b);

        a.setSymbolName("woof");
        ASSERT(a.isSymbolNameValid());
        ASSERT(a != b);
        b.setSymbolName(bdeu_String::copy(a.symbolName(), &sa));
        ASSERT(a == b);
        ASSERT(b.isSymbolNameValid());

        a.setMangledSymbolName("woof");
        ASSERT(a.isMangledSymbolNameValid());
        ASSERT(a != b);
        b.setMangledSymbolName(bdeu_String::copy(a.mangledSymbolName(),
                                                 &sa));
        ASSERT(a == b);
        ASSERT(b.isMangledSymbolNameValid());

        a.setSourceFileName("woof.cpp");
        ASSERT(a.isSourceFileNameValid());
        ASSERT(a != b);
        b.setSourceFileName(bdeu_String::copy(a.sourceFileName(), &sa));
        ASSERT(b.isSourceFileNameValid());
        ASSERT(a == b);

        if (verbose) cout << a;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 No memory allocation occurs from the default allocator.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Using the default c'tor and manipulators, create two 'const'
        //:     'Obj', 'Z' and 'ZZ', each having the value driven by the table.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj',
        //:       'mX', using 'oa', and having the value 'W' 
        //:
        //:     2 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     4 Use the equality comparison operators to verify that:
        //:       (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //: 4 Verify that no memory is ever allocated from the default
        //:   allocator.  (C-3)
        //
        // Testing:
        //   operator=(const baetzo_LocalTimeDescriptor& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*OperatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            OperatorPtr operatorAssignment = &Obj::operator=;
            if (veryVeryVerbose) P(operatorAssignment);
        }

        // Attribute Types

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 D1 = 0;                // default value
        const T1 A1 = &D1;              // non-zero value
        const T1 B1 = &A1;

        // Attribute 2 Values: 'libraryFileName'

        const T2 D2 = 0;                // default value
        const T2 A2 = "/lib/lib1.so";
        const T2 B2 = "/lib/lib1.ko";

        // Attribute 3 Values: 'lineNumber'

        const T3 D3 = -1;               // default value
        const T3 A3 = 10;
        const T3 B3 = 50;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 D4 = Obj::invalidOffset();    // default value
        const T4 A4 = 100;
        const T4 B4 = 200;

        // Attribute 5 Values: 'sourceFileName'

        const T5 D5 = 0;                // default value
        const T5 A5 = "/a/b/c/d/mysource.cpp";
        const T5 B5 = "/b/c/d/e/yoursource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 D6 = 0;                // default value
        const T6 A6 = "mangledwoof";
        const T6 B6 = "woofmangled";

        // Attribute 7 Values: "symbolName"

        const T7 D7 = 0;                // default value
        const T7 A7 = "woof";
        const T7 B7 = "Woof";

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static struct {
            int         d_line;           // source line number
            T1          d_address;
            T2          d_libraryFileName;
            T3          d_lineNumber;
            T4          d_offsetFromSymbol;
            T5          d_sourceFileName;
            T6          d_mangledSymbolName;
            T7          d_symbolName;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  @ LIB  LN OFF SFN MSN  SN
        //--   --  --  --  --  --  --  --

        { L_,  D1, D2, D3, D4, D5, D6, D7 },
        { L_,  A1, A2, A3, A4, A5, A6, A7 },
        { L_,  B1, B2, B3, B4, B5, B6, B7 },

        { L_,  D1, D2, D3, D4, D5, D6, A7 },
        { L_,  D1, D2, D3, D4, D5, A6, D7 },
        { L_,  D1, D2, D3, D4, A5, D6, D7 },
        { L_,  D1, D2, D3, A4, D5, D6, D7 },
        { L_,  D1, D2, A3, D4, D5, D6, D7 },
        { L_,  D1, A2, D3, D4, D5, D6, D7 },
        { L_,  A1, D2, D3, D4, D5, D6, D7 },

        { L_,  D1, D2, D3, D4, D5, D6, B7 },
        { L_,  D1, D2, D3, D4, D5, B6, D7 },
        { L_,  D1, D2, D3, D4, B5, D6, D7 },
        { L_,  D1, D2, D3, B4, D5, D6, D7 },
        { L_,  D1, D2, B3, D4, D5, D6, D7 },
        { L_,  D1, B2, D3, D4, D5, D6, D7 },
        { L_,  B1, D2, D3, D4, D5, D6, D7 },

        { L_,  A1, A2, A3, A4, A5, A6, B7 },
        { L_,  A1, A2, A3, A4, A5, B6, A7 },
        { L_,  A1, A2, A3, A4, B5, A6, A7 },
        { L_,  A1, A2, A3, B4, A5, A6, A7 },
        { L_,  A1, A2, B3, A4, A5, A6, A7 },
        { L_,  A1, B2, A3, A4, A5, A6, A7 },
        { L_,  B1, A2, A3, A4, A5, A6, A7 },

        { L_,  B1, B2, B3, B4, B5, B6, A7 },
        { L_,  B1, B2, B3, B4, B5, A6, B7 },
        { L_,  B1, B2, B3, B4, A5, B6, B7 },
        { L_,  B1, B2, B3, A4, B5, B6, B7 },
        { L_,  B1, B2, A3, B4, B5, B6, B7 },
        { L_,  B1, A2, B3, B4, B5, B6, B7 },
        { L_,  A1, B2, B3, B4, B5, B6, B7 },

        { L_,  A1, B2, D3, A4, B5, D6, A7 },
        { L_,  B1, D2, A3, B4, D5, A6, B7 },
        { L_,  D1, A2, B3, D4, A5, B6, D7 },
        { L_,  D1, B2, A3, D4, B5, A6, D7 },
        { L_,  B1, A2, D3, B4, A5, D6, B7 },
        { L_,  A1, D2, B3, A4, D5, B6, A7 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1    = DATA[ti].d_line;
            const T1  ADDRESS1 = DATA[ti].d_address;
            const T2  LIB1     = DATA[ti].d_libraryFileName;
            const T3  LN1      = DATA[ti].d_lineNumber;
            const T4  OFFSET1  = DATA[ti].d_offsetFromSymbol;
            const T5  SFN1     = DATA[ti].d_sourceFileName;
            const T6  MSN1     = DATA[ti].d_mangledSymbolName;
            const T7  SN1      = DATA[ti].d_symbolName;

            if (veryVerbose) { T_ P_(LINE1) P_(ADDRESS1) P_(LIB1) P_(LN1)
                                         P_(OFFSET1) P_(SFN1) P_(MSN1) P(SN1) }

            Obj mZ;    const Obj& Z = mZ;
            mZ.setAddress(ADDRESS1);
            mZ.setLibraryFileName(LIB1);
            mZ.setLineNumber(LN1);
            mZ.setOffsetFromSymbol(OFFSET1);
            mZ.setSourceFileName(SFN1);
            mZ.setMangledSymbolName(MSN1);
            mZ.setSymbolName(SN1);

            Obj mZZ(Z);    const Obj& ZZ = mZZ;

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P_(ZZ) }

            LOOP3_ASSERT(LINE1, Z, ZZ, Z == ZZ);

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            mZ = mZ;    // aliasing

            LOOP3_ASSERT(LINE1, Z, ZZ, Z == ZZ);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2    = DATA[tj].d_line;
                const T1  ADDRESS2 = DATA[tj].d_address;
                const T2  LIB2     = DATA[tj].d_libraryFileName;
                const T3  LN2      = DATA[tj].d_lineNumber;
                const T4  OFFSET2  = DATA[tj].d_offsetFromSymbol;
                const T5  SFN2     = DATA[tj].d_sourceFileName;
                const T6  MSN2     = DATA[tj].d_mangledSymbolName;
                const T7  SN2      = DATA[tj].d_symbolName;

                if (veryVerbose) { T_ P_(LINE2) P_(ADDRESS2) P_(LIB2) P_(LN2)
                                         P_(OFFSET2) P_(SFN2) P_(MSN2) P(SN2) }

                Obj mX;    const Obj& X = mX;
                mX.setAddress(ADDRESS2);
                mX.setLibraryFileName(LIB2);
                mX.setLineNumber(LN2);
                mX.setOffsetFromSymbol(OFFSET2);
                mX.setSourceFileName(SFN2);
                mX.setMangledSymbolName(MSN2);
                mX.setSymbolName(SN2);

                if (veryVerbose) { T_ P_(LINE2) P_(X) }

                LOOP4_ASSERT(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);                        
                LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z ==   X);
                LOOP4_ASSERT(LINE1, LINE2,  Z,  ZZ,  Z ==  ZZ);
                LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);
            }
        }

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // n/a
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //:
        //: 4 No memory is ever allocated by the copy c'tor.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Copy-construct a new instance 'X' from 'Z'.
        //:
        //:   3 Verify 'X == Z'
        //:
        //:   4 Use 'ZZ' to verify 'Z' in unchanged.
        //:
        //: 3 Verify no memory allocation has occurred using the default
        //:   allocator.
        //
        // Testing:
        //   baetzo_LocalTimeDescriptor(const baetzo_LTDescriptor& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;


        // Attribute Types

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 D1 = 0;                // default value
        const T1 A1 = &D1;              // non-zero value
        const T1 B1 = &A1;

        // Attribute 2 Values: 'libraryFileName'

        const T2 D2 = 0;                // default value
        const T2 A2 = "/lib/lib1.so";
        const T2 B2 = "/lib/lib1.ko";

        // Attribute 3 Values: 'lineNumber'

        const T3 D3 = -1;               // default value
        const T3 A3 = 10;
        const T3 B3 = 50;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 D4 = Obj::invalidOffset();    // default value
        const T4 A4 = 100;
        const T4 B4 = 200;

        // Attribute 5 Values: 'sourceFileName'

        const T5 D5 = 0;                // default value
        const T5 A5 = "/a/b/c/d/mysource.cpp";
        const T5 B5 = "/b/c/d/e/yoursource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 D6 = 0;                // default value
        const T6 A6 = "mangledwoof";
        const T6 B6 = "woofmangled";

        // Attribute 7 Values: "symbolName"

        const T7 D7 = 0;                // default value
        const T7 A7 = "woof";
        const T7 B7 = "Woof";

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static struct {
            int         d_line;           // source line number
            T1          d_address;
            T2          d_libraryFileName;
            T3          d_lineNumber;
            T4          d_offsetFromSymbol;
            T5          d_sourceFileName;
            T6          d_mangledSymbolName;
            T7          d_symbolName;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  @ LIB  LN OFF SFN MSN  SN
        //--   --  --  --  --  --  --  --

        { L_,  D1, D2, D3, D4, D5, D6, D7 },
        { L_,  A1, A2, A3, A4, A5, A6, A7 },
        { L_,  B1, B2, B3, B4, B5, B6, B7 },

        { L_,  D1, D2, D3, D4, D5, D6, A7 },
        { L_,  D1, D2, D3, D4, D5, A6, D7 },
        { L_,  D1, D2, D3, D4, A5, D6, D7 },
        { L_,  D1, D2, D3, A4, D5, D6, D7 },
        { L_,  D1, D2, A3, D4, D5, D6, D7 },
        { L_,  D1, A2, D3, D4, D5, D6, D7 },
        { L_,  A1, D2, D3, D4, D5, D6, D7 },

        { L_,  D1, D2, D3, D4, D5, D6, B7 },
        { L_,  D1, D2, D3, D4, D5, B6, D7 },
        { L_,  D1, D2, D3, D4, B5, D6, D7 },
        { L_,  D1, D2, D3, B4, D5, D6, D7 },
        { L_,  D1, D2, B3, D4, D5, D6, D7 },
        { L_,  D1, B2, D3, D4, D5, D6, D7 },
        { L_,  B1, D2, D3, D4, D5, D6, D7 },

        { L_,  A1, A2, A3, A4, A5, A6, B7 },
        { L_,  A1, A2, A3, A4, A5, B6, A7 },
        { L_,  A1, A2, A3, A4, B5, A6, A7 },
        { L_,  A1, A2, A3, B4, A5, A6, A7 },
        { L_,  A1, A2, B3, A4, A5, A6, A7 },
        { L_,  A1, B2, A3, A4, A5, A6, A7 },
        { L_,  B1, A2, A3, A4, A5, A6, A7 },

        { L_,  B1, B2, B3, B4, B5, B6, A7 },
        { L_,  B1, B2, B3, B4, B5, A6, B7 },
        { L_,  B1, B2, B3, B4, A5, B6, B7 },
        { L_,  B1, B2, B3, A4, B5, B6, B7 },
        { L_,  B1, B2, A3, B4, B5, B6, B7 },
        { L_,  B1, A2, B3, B4, B5, B6, B7 },
        { L_,  A1, B2, B3, B4, B5, B6, B7 },

        { L_,  A1, B2, D3, A4, B5, D6, A7 },
        { L_,  B1, D2, A3, B4, D5, A6, B7 },
        { L_,  D1, A2, B3, D4, A5, B6, D7 },
        { L_,  D1, B2, A3, D4, B5, A6, D7 },
        { L_,  B1, A2, D3, B4, A5, D6, B7 },
        { L_,  A1, D2, B3, A4, D5, B6, A7 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE    = DATA[ti].d_line;
            const T1  ADDRESS = DATA[ti].d_address;
            const T2  LIB     = DATA[ti].d_libraryFileName;
            const T3  LN      = DATA[ti].d_lineNumber;
            const T4  OFFSET  = DATA[ti].d_offsetFromSymbol;
            const T5  SFN     = DATA[ti].d_sourceFileName;
            const T6  MSN     = DATA[ti].d_mangledSymbolName;
            const T7  SN      = DATA[ti].d_symbolName;

            if (veryVerbose) { T_ P_(LINE) P_(ADDRESS) P_(LIB) P_(LN)
                                         P_(OFFSET) P_(SFN) P_(MSN) P(SN) }

            Obj mZ;    const Obj& Z = mZ;
            mZ.setAddress(ADDRESS);
            mZ.setLibraryFileName(LIB);
            mZ.setLineNumber(LN);
            mZ.setOffsetFromSymbol(OFFSET);
            mZ.setSourceFileName(SFN);
            mZ.setMangledSymbolName(MSN);
            mZ.setSymbolName(SN);

            Obj mZZ;    const Obj& ZZ = mZZ;
            mZZ.setAddress(ADDRESS);
            mZZ.setLibraryFileName(LIB);
            mZZ.setLineNumber(LN);
            mZZ.setOffsetFromSymbol(OFFSET);
            mZZ.setSourceFileName(SFN);
            mZZ.setMangledSymbolName(MSN);
            mZZ.setSymbolName(SN);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            LOOP3_ASSERT(LINE, Z, ZZ, Z == ZZ);

            Obj mX(Z);    const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Verify the value of the object.

            LOOP3_ASSERT(LINE,  Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY COMPARISON OPERATORS
        //  Ensure that '==' and ''!  '=' are the operational definition of
        //  value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //:
        //:14 'const char *' attributes evaluate to equivalent if they point to
        //:   differently located but equivalent strings.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signature and
        //:   return type for the two homogeneous, free equality comparison
        //:   operators defined in this component.  (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma_TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so. 
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:   1 Create a single object 'X' and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 Create a single object 'Y' with all 'const char *' fields
        //:     duplicated to equivalent strings, verify that 'X == Y', ...
        //:
        //:   3 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For all combinations of the booleans 'CLONE_X' and 'CLONE_Y',
        //:
        //:       1 Create an object 'X' having the value 'R1'.
        //:
        //:       2 If 'CLONE_X', substitute equivalent copies of all the
        //:         'const char *' attributes of 'X'.
        //:
        //:       3 Create an object 'Y' having the value 'R2'.
        //:
        //:       4 If 'CLONE_Y', substitute equivalent copies of all the
        //:         'const char *' attributes of 'Y'.
        //:
        //:       5 Verify the commutativity property and expected return value
        //:         for both '==' and '!='.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory was ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const baesu_StackTraceFrame& lhs, rhs);
        //   bool operator!=(const baesu_StackTraceFrame& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*OperatorPtr)(const Obj&, const Obj&);

            // Verify that the signature and return type are standard.

            union {
                OperatorPtr  d_op;
                const void  *d_p;
            } u;
            if (verbose) {
                u.d_op = &operator==;
            }
            else {
                u.d_op = &operator!=;
            }

            if (veryVeryVerbose) P(u.d_p);
        }

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 D1 = 0;                // default value
        const T1 A1 = &D1;              // non-zero value
        const T1 B1 = &A1;

        // Attribute 2 Values: 'libraryFileName'

        const T2 D2 = 0;                // default value
        const T2 A2 = "/lib/lib1.so";
        const T2 B2 = "/lib/lib1.ko";

        // Attribute 3 Values: 'lineNumber'

        const T3 D3 = -1;               // default value
        const T3 A3 = 10;
        const T3 B3 = 50;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 D4 = Obj::invalidOffset();    // default value
        const T4 A4 = 100;
        const T4 B4 = 200;

        // Attribute 5 Values: 'sourceFileName'

        const T5 D5 = 0;                // default value
        const T5 A5 = "/a/b/c/d/mysource.cpp";
        const T5 B5 = "/b/c/d/e/yoursource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 D6 = 0;                // default value
        const T6 A6 = "mangledwoof";
        const T6 B6 = "woofmangled";

        // Attribute 7 Values: "symbolName"

        const T7 D7 = 0;                // default value
        const T7 A7 = "woof";
        const T7 B7 = "Woof";

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static struct {
            int         d_line;           // source line number
            T1          d_address;
            T2          d_libraryFileName;
            T3          d_lineNumber;
            T4          d_offsetFromSymbol;
            T5          d_sourceFileName;
            T6          d_mangledSymbolName;
            T7          d_symbolName;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  @ LIB  LN OFF SFN MSN  SN
        //--   --  --  --  --  --  --  --

        { L_,  D1, D2, D3, D4, D5, D6, D7 },
        { L_,  A1, A2, A3, A4, A5, A6, A7 },
        { L_,  B1, B2, B3, B4, B5, B6, B7 },

        { L_,  D1, D2, D3, D4, D5, D6, A7 },
        { L_,  D1, D2, D3, D4, D5, A6, D7 },
        { L_,  D1, D2, D3, D4, A5, D6, D7 },
        { L_,  D1, D2, D3, A4, D5, D6, D7 },
        { L_,  D1, D2, A3, D4, D5, D6, D7 },
        { L_,  D1, A2, D3, D4, D5, D6, D7 },
        { L_,  A1, D2, D3, D4, D5, D6, D7 },

        { L_,  D1, D2, D3, D4, D5, D6, B7 },
        { L_,  D1, D2, D3, D4, D5, B6, D7 },
        { L_,  D1, D2, D3, D4, B5, D6, D7 },
        { L_,  D1, D2, D3, B4, D5, D6, D7 },
        { L_,  D1, D2, B3, D4, D5, D6, D7 },
        { L_,  D1, B2, D3, D4, D5, D6, D7 },
        { L_,  B1, D2, D3, D4, D5, D6, D7 },

        { L_,  A1, A2, A3, A4, A5, A6, B7 },
        { L_,  A1, A2, A3, A4, A5, B6, A7 },
        { L_,  A1, A2, A3, A4, B5, A6, A7 },
        { L_,  A1, A2, A3, B4, A5, A6, A7 },
        { L_,  A1, A2, B3, A4, A5, A6, A7 },
        { L_,  A1, B2, A3, A4, A5, A6, A7 },
        { L_,  B1, A2, A3, A4, A5, A6, A7 },

        { L_,  B1, B2, B3, B4, B5, B6, A7 },
        { L_,  B1, B2, B3, B4, B5, A6, B7 },
        { L_,  B1, B2, B3, B4, A5, B6, B7 },
        { L_,  B1, B2, B3, A4, B5, B6, B7 },
        { L_,  B1, B2, A3, B4, B5, B6, B7 },
        { L_,  B1, A2, B3, B4, B5, B6, B7 },
        { L_,  A1, B2, B3, B4, B5, B6, B7 },

        { L_,  A1, B2, D3, A4, B5, D6, A7 },
        { L_,  B1, D2, A3, B4, D5, A6, B7 },
        { L_,  D1, A2, B3, D4, A5, B6, D7 },
        { L_,  D1, B2, A3, D4, B5, A6, D7 },
        { L_,  B1, A2, D3, B4, A5, D6, B7 },
        { L_,  A1, D2, B3, A4, D5, B6, A7 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1    = DATA[ti].d_line;
            const T1  ADDRESS1 = DATA[ti].d_address;
            const T2  LIB1     = DATA[ti].d_libraryFileName;
            const T3  LN1      = DATA[ti].d_lineNumber;
            const T4  OFFSET1  = DATA[ti].d_offsetFromSymbol;
            const T5  SFN1     = DATA[ti].d_sourceFileName;
            const T6  MSN1     = DATA[ti].d_mangledSymbolName;
            const T7  SN1      = DATA[ti].d_symbolName;

            if (veryVerbose) { T_ P_(LINE1) P_(ADDRESS1) P_(LIB1) P_(LN1)
                                         P_(OFFSET1) P_(SFN1) P_(MSN1) P(SN1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma_TestAllocator ta;
                bdema_SequentialAllocator sa(&ta);

                Obj mX;    const Obj& X = mX;
                mX.setAddress(ADDRESS1);
                mX.setLibraryFileName(LIB1);
                mX.setLineNumber(LN1);
                mX.setOffsetFromSymbol(OFFSET1);
                mX.setSourceFileName(SFN1);
                mX.setMangledSymbolName(MSN1);
                mX.setSymbolName(SN1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));

                Obj mY;    const Obj& Y = mY;
                mY.setAddress(ADDRESS1);
                mY.setLibraryFileName(LIB1);
                mY.setLineNumber(LN1);
                mY.setOffsetFromSymbol(OFFSET1);
                mY.setSourceFileName(SFN1);
                mY.setMangledSymbolName(MSN1);
                mY.setSymbolName(SN1);

                LOOP2_ASSERT(LINE1, Y,   Y == Y);
                LOOP2_ASSERT(LINE1, Y, !(Y != Y));

                cloneStrings(&mY, &sa);

                LOOP2_ASSERT(LINE1, Y,   Y == Y);
                LOOP2_ASSERT(LINE1, Y, !(Y != Y));

                LOOP3_ASSERT(LINE1, X, Y,   X == Y);
                LOOP3_ASSERT(LINE1, X, Y,   Y == X);
                LOOP3_ASSERT(LINE1, Y, Y, !(X != Y));
                LOOP3_ASSERT(LINE1, Y, Y, !(Y != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2    = DATA[tj].d_line;
                const T1  ADDRESS2 = DATA[tj].d_address;
                const T2  LIB2     = DATA[tj].d_libraryFileName;
                const T3  LN2      = DATA[tj].d_lineNumber;
                const T4  OFFSET2  = DATA[tj].d_offsetFromSymbol;
                const T5  SFN2     = DATA[tj].d_sourceFileName;
                const T6  MSN2     = DATA[tj].d_mangledSymbolName;
                const T7  SN2      = DATA[tj].d_symbolName;

                if (veryVerbose) { T_ P_(LINE2) P_(ADDRESS2) P_(LIB2) P_(LN2)
                                         P_(OFFSET2) P_(SFN2) P_(MSN2) P(SN2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                bslma_TestAllocator ta;
                bdema_SequentialAllocator sa(&ta);

                for (int cfg = 0; cfg < 4; ++cfg) {
                    const bool CLONE_X = cfg & 1;
                    const bool CLONE_Y = cfg & 2;

                    // Verify value, commutativity, and no memory allocation.

                    Obj mX;    const Obj& X = mX;
                    mX.setAddress(ADDRESS1);
                    mX.setLibraryFileName(LIB1);
                    mX.setLineNumber(LN1);
                    mX.setOffsetFromSymbol(OFFSET1);
                    mX.setSourceFileName(SFN1);
                    mX.setMangledSymbolName(MSN1);
                    mX.setSymbolName(SN1);

                    if (CLONE_X) {
                        cloneStrings(&mX, &sa);
                    }

                    Obj mY;    const Obj& Y = mY;
                    mY.setAddress(ADDRESS2);
                    mY.setLibraryFileName(LIB2);
                    mY.setLineNumber(LN2);
                    mY.setOffsetFromSymbol(OFFSET2);
                    mY.setSourceFileName(SFN2);
                    mY.setMangledSymbolName(MSN2);
                    mY.setSymbolName(SN2);

                    if (CLONE_Y) {
                        cloneStrings(&mY, &sa);
                    }

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CLONE_X) P_(CLONE_Y) P_(X) P_(Y) }

                    LOOP5_ASSERT(LINE1, LINE2, cfg, X, Y,  EXP == (X == Y));
                    LOOP5_ASSERT(LINE1, LINE2, cfg, Y, X,  EXP == (Y == X));

                    LOOP5_ASSERT(LINE1, LINE2, cfg, X, Y, !EXP == (X != Y));
                    LOOP5_ASSERT(LINE1, LINE2, cfg, Y, X, !EXP == (Y != X));
                }
            }

            ASSERT(0 == defaultAllocator.numBlocksTotal());
        }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The 'print' method signature and return type are standard.
        //:
        //: 4 The 'print' method returns the supplied 'ostream'.
        //:
        //: 5 The output 'operator<<' signature and return type are standard.
        //:
        //: 6 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' method and 'operator<<' defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structures.  (C-4, 6)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5, 7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A B } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are,
        //:       arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:       invoked instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 7)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const baesu_StackTraceFrame& stf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            union {
               funcPtr      d_print;
               operatorPtr  d_operatorOp;
               void        *d_p;
            } u;
            if (verbose) {
                u.d_print      = &Obj::print;
            }
            else {
                u.d_operatorOp = &operator<<;
            }

            // output contents of 'u' to prevent unused variable complaints

            if (veryVeryVerbose) P(u.d_p);
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 A1 = (T1) 0xabcdef01;
        const T1 B1 = (T1) 0x12345678;

        // Attribute 2 Values: 'libraryFileName'

        const T2 A2 = "/lib/lib1.so";
        const T2 B2 = "/a/b/c/myExec.tsk";

        // Attribute 3 Values: 'lineNumber'

        const T3 A3 = 10;
        const T3 B3 = 50;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 A4 = 100;
        const T4 B4 = 200;

        // Attribute 5 Values: 'sourceFileName'

        const T5 A5 = "/a/b/c/d/mysource.cpp";
        const T5 B5 = "/b/c/d/e/yoursource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 A6 = "mangledwoof";
        const T6 B6 = "woofmangled";

        // Attribute 7 Values: "symbolName"

        const T7 A7 = "woof";
        const T7 B7 = "Woof";

        enum ObjVersion { A, B, D };

        static const struct {
            int          d_line;           // source line number
            int          d_level;
            int          d_spacesPerLevel;

            ObjVersion   d_version;        // 'A', 'B', or 'C'

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  V  EXP
        //---- - ---  -  ---

        { L_,  0,  0, A, "["                                                 NL
                         "address = 0xabcdef01"                              NL
                         "library file name = \"/lib/lib1.so\""              NL
                         "line number = 10"                                  NL
                         "offset from symbol = 100"                          NL
                         "source file name = \"/a/b/c/d/mysource.cpp\""      NL
                         "mangled symbol name = \"mangledwoof\""             NL
                         "symbol name = \"woof\""                            NL
                         "]"                                                 NL
                                                                             },

        { L_,  0,  1, A, "["                                                 NL
                         " address = 0xabcdef01"                             NL
                         " library file name = \"/lib/lib1.so\""             NL
                         " line number = 10"                                 NL
                         " offset from symbol = 100"                         NL
                         " source file name = \"/a/b/c/d/mysource.cpp\""     NL
                         " mangled symbol name = \"mangledwoof\""            NL
                         " symbol name = \"woof\""                           NL
                         "]"                                                 NL
                                                                             },

        { L_,  0, -1, A, "["                                                 SP
                         "address = 0xabcdef01"                              SP
                         "library file name = \"/lib/lib1.so\""              SP
                         "line number = 10"                                  SP
                         "offset from symbol = 100"                          SP
                         "source file name = \"/a/b/c/d/mysource.cpp\""      SP
                         "mangled symbol name = \"mangledwoof\""             SP
                         "symbol name = \"woof\""                            SP
                         "]"                                                 },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  V  EXP
        //---- - ---  -  ---

        { L_,  3,  0, A, "["                                                 NL
                         "address = 0xabcdef01"                              NL
                         "library file name = \"/lib/lib1.so\""              NL
                         "line number = 10"                                  NL
                         "offset from symbol = 100"                          NL
                         "source file name = \"/a/b/c/d/mysource.cpp\""      NL
                         "mangled symbol name = \"mangledwoof\""             NL
                         "symbol name = \"woof\""                            NL
                         "]"                                                 NL
                                                                             },

        { L_,  3,  2, A, "      ["                                           NL
                         "        address = 0xabcdef01"                      NL
                         "        library file name = \"/lib/lib1.so\""      NL
                         "        line number = 10"                          NL
                         "        offset from symbol = 100"                  NL
                         "        source file name = \"/a/b/c/d/mysource.cpp\""
                                                                             NL
                         "        mangled symbol name = \"mangledwoof\""     NL
                         "        symbol name = \"woof\""                    NL
                         "      ]"                                           NL
                                                                             },

        { L_,  3, -2, A, "      ["                                           SP
                         "address = 0xabcdef01"                              SP
                         "library file name = \"/lib/lib1.so\""              SP
                         "line number = 10"                                  SP
                         "offset from symbol = 100"                          SP
                         "source file name = \"/a/b/c/d/mysource.cpp\""      SP
                         "mangled symbol name = \"mangledwoof\""             SP
                         "symbol name = \"woof\""                            SP
                         "]"                                                 },

        { L_, -3,  0, A, "["                                                 NL
                         "address = 0xabcdef01"                              NL
                         "library file name = \"/lib/lib1.so\""              NL
                         "line number = 10"                                  NL
                         "offset from symbol = 100"                          NL
                         "source file name = \"/a/b/c/d/mysource.cpp\""      NL
                         "mangled symbol name = \"mangledwoof\""             NL
                         "symbol name = \"woof\""                            NL
                         "]"                                                 NL
                                                                             },

        { L_, -3,  2, A, "["                                                 NL
                         "        address = 0xabcdef01"                      NL
                         "        library file name = \"/lib/lib1.so\""      NL
                         "        line number = 10"                          NL
                         "        offset from symbol = 100"                  NL
                         "        "
                             "source file name = \"/a/b/c/d/mysource.cpp\""  NL
                         "        mangled symbol name = \"mangledwoof\""     NL
                         "        symbol name = \"woof\""                    NL
                         "      ]"                                           NL
                                                                             },

        { L_, -3, -2, A, "["                                                 SP
                         "address = 0xabcdef01"                              SP
                         "library file name = \"/lib/lib1.so\""              SP
                         "line number = 10"                                  SP
                         "offset from symbol = 100"                          SP
                         "source file name = \"/a/b/c/d/mysource.cpp\""      SP
                         "mangled symbol name = \"mangledwoof\""             SP
                         "symbol name = \"woof\""                            SP
                         "]"                                                 },

        { L_,  3,  0, B, "["                                                 NL
                         "address = 0x12345678"                              NL
                         "library file name = \"/a/b/c/myExec.tsk\""         NL
                         "line number = 50"                                  NL
                         "offset from symbol = 200"                          NL
                         "source file name = \"/b/c/d/e/yoursource.cpp\""    NL
                         "mangled symbol name = \"woofmangled\""             NL
                         "symbol name = \"Woof\""                            NL
                         "]"                                                 NL
                                                                             },

        { L_,  3,  2, B, "      ["                                           NL
                         "        address = 0x12345678"                      NL
                         "        "
                              "library file name = \"/a/b/c/myExec.tsk\""    NL
                         "        line number = 50"                          NL
                         "        offset from symbol = 200"                  NL
                         "        "
                            "source file name = \"/b/c/d/e/yoursource.cpp\"" NL
                         "        mangled symbol name = \"woofmangled\""     NL
                         "        symbol name = \"Woof\""                    NL
                         "      ]"                                           NL
                                                                             },

        { L_,  3, -2, B, "      ["                                           SP
                         "address = 0x12345678"                              SP
                         "library file name = \"/a/b/c/myExec.tsk\""         SP
                         "line number = 50"                                  SP
                         "offset from symbol = 200"                          SP
                         "source file name = \"/b/c/d/e/yoursource.cpp\""    SP
                         "mangled symbol name = \"woofmangled\""             SP
                         "symbol name = \"Woof\""                            SP
                         "]"                                                 },

        { L_, -3,  0, B, "["                                                 NL
                         "address = 0x12345678"                              NL
                         "library file name = \"/a/b/c/myExec.tsk\""         NL
                         "line number = 50"                                  NL
                         "offset from symbol = 200"                          NL
                         "source file name = \"/b/c/d/e/yoursource.cpp\""    NL
                         "mangled symbol name = \"woofmangled\""             NL
                         "symbol name = \"Woof\""                            NL
                         "]"                                                 NL
                                                                             },

        { L_, -3,  2, B, "["                                                 NL
                         "        address = 0x12345678"                      NL
                         "        library file name = \"/a/b/c/myExec.tsk\"" NL
                         "        line number = 50"                          NL
                         "        offset from symbol = 200"                  NL
                         "        "
                         "source file name = \"/b/c/d/e/yoursource.cpp\""    NL
                         "        mangled symbol name = \"woofmangled\""     NL
                         "        symbol name = \"Woof\""                    NL
                         "      ]"                                           NL
                                                                             },

        { L_, -3, -2, B, "["                                                 SP
                         "address = 0x12345678"                              SP
                         "library file name = \"/a/b/c/myExec.tsk\""         SP
                         "line number = 50"                                  SP
                         "offset from symbol = 200"                          SP
                         "source file name = \"/b/c/d/e/yoursource.cpp\""    SP
                         "mangled symbol name = \"woofmangled\""             SP
                         "symbol name = \"Woof\""                            SP
                         "]"                                                 },

        { L_, -9, -9, A, "woof+0x64 at 0xabcdef01"
#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_ELF
                         " static"
#endif
#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS
                         " source:mysource.cpp:10 in lib1.so" },
#else
                         " source:mysource.cpp:10 in /lib/lib1.so" },
#endif

        { L_, -9, -9, B, "Woof+0xc8 at 0x12345678"
#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_ELF
                         " static"
#endif
                         " source:yoursource.cpp:50 in myExec.tsk" },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const ObjVersion  V      = DATA[ti].d_version;
                const char *const EXP    = DATA[ti].d_expected_p;

                char v = 0;
                Obj mX;         const Obj& X = mX;
                switch (V) {
                  case A: {
                    v = 'A';
                    mX.setAddress(A1);
                    mX.setLibraryFileName(A2);
                    mX.setLineNumber(A3);
                    mX.setOffsetFromSymbol(A4);
                    mX.setSourceFileName(A5);
                    mX.setMangledSymbolName(A6);
                    mX.setSymbolName(A7);
                  }  break;
                  case B: {
                    v = 'B';
                    mX.setAddress(B1);
                    mX.setLibraryFileName(B2);
                    mX.setLineNumber(B3);
                    mX.setOffsetFromSymbol(B4);
                    mX.setSourceFileName(B5);
                    mX.setMangledSymbolName(B6);
                    mX.setSymbolName(B7);
                  }  break;
                  case D: {
                    v = 'D';
                  }  break;
                  default: {
                    ASSERT(0);
                  }  break;
                }

                if (veryVerbose) { T_ P_(L) P_(SPL) P(v) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bsl::ostringstream os;

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        //   n/a -- since this object allocates no memory, there is nothing to
        //   be tested that hasn't already been covered in case 2.
        // --------------------------------------------------------------------

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // n/a -- this class has only a default c'tor
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 Each attribute is modifiable independently.
        //:
        //: 3 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 4 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, and
        //:   distinct ('A') and ('B') values other than ('D').
        //:
        //: 2 Use the individual (as yet unproven) salient attribute accessors
        //:   to verify the default-constructed value.  (C-1)
        //:
        //: 3 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's value,
        //:   passing a 'const' argument representing each of the three test
        //:   values, in turn (see P-1), first to 'Ai', then to 'Bi', and
        //:   finally back to 'Di'.  .  (C-5..6, 11..14)
        //:
        //: 4 Corroborate that attributes are modifiable independently by first
        //:   setting all of the attributes to their 'A' values, then setting
        //:   all of the attributes to their 'B' values.  (C-10)
        //:
        //: 5 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   baesu_StackTraceFrame()
        //   ~baesu_StackTraceFrame()
        //   setAddress(const void *)
        //   setLibraryFileName(const char *)
        //   setLineNumber(int)
        //   setOffsetFromSymbol(bsl::size_t)
        //   setSourceFileName(const char *)
        //   setMangledSymbolName(const char *)
        //   setSymbolName(const char *)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;


        // Attribute Types

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 D1 = 0;                // default value
        const T1 A1 = &D1;              // non-zero value
        const T1 B1 = &A1;

        // Attribute 2 Values: 'libraryFileName'

        const T2 D2 = 0;                // default value
        const T2 A2 = "/lib/lib1.so";
        const T2 B2 = "/lib/lib1.ko";

        // Attribute 3 Values: 'lineNumber'

        const T3 D3 = -1;               // default value
        const T3 A3 = 10;
        const T3 B3 = 50;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 D4 = Obj::invalidOffset();    // default value
        const T4 A4 = 100;
        const T4 B4 = 200;

        // Attribute 5 Values: 'sourceFileName'

        const T5 D5 = 0;                // default value
        const T5 A5 = "/a/b/c/d/mysource.cpp";
        const T5 B5 = "/b/c/d/e/yoursource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 D6 = 0;                // default value
        const T6 A6 = "mangledwoof";
        const T6 B6 = "woofmangled";

        // Attribute 7 Values: "symbolName"

        const T7 D7 = 0;                // default value
        const T7 A7 = "woof";
        const T7 B7 = "Woof";

        Obj mX;    const Obj& X = mX;

        // -------------------------------------
        // Verify the object's attribute values.
        // -------------------------------------

        LOOP2_ASSERT(D1, X.address(),
                     D1 == X.address());
        LOOP2_ASSERT(D2, X.libraryFileName(),
                     D2 == X.libraryFileName());
        LOOP2_ASSERT(D3, X.lineNumber(),
                     D3 == X.lineNumber());
        LOOP2_ASSERT(D4, X.offsetFromSymbol(),
                     D4 == X.offsetFromSymbol());
        LOOP2_ASSERT(D5, X.sourceFileName(),
                     D5 == X.sourceFileName());
        LOOP2_ASSERT(D6, X.mangledSymbolName(),
                     D6 == X.mangledSymbolName());
        LOOP2_ASSERT(D7, X.symbolName(),
                     D7 == X.symbolName());

        // -----------------------------------------------------
        // Verify that each attribute is independently settable.
        // -----------------------------------------------------

        // 'address'

        {
            mX.setAddress(A1);
            ASSERT(A1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setAddress(B1);
            ASSERT(B1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setAddress(D1);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'libraryFileName'

        {
            mX.setLibraryFileName(A2);
            ASSERT(D1 == X.address());
            ASSERT(A2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setLibraryFileName(B2);
            ASSERT(D1 == X.address());
            ASSERT(B2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setLibraryFileName(D2);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'lineNumber'

        {
            mX.setLineNumber(A3);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(A3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setLineNumber(B3);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(B3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setLineNumber(D3);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'offsetFromSymbol'

        {
            mX.setOffsetFromSymbol(A4);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(A4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setOffsetFromSymbol(B4);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(B4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setOffsetFromSymbol(D4);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'sourceFileName'

        {
            mX.setSourceFileName(A5);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(A5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setSourceFileName(B5);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(B5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setSourceFileName(D5);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'mangledSymbolName'

        {
            mX.setMangledSymbolName(A6);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(A6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setMangledSymbolName(B6);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(B6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());

            mX.setMangledSymbolName(D6);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // 'symbolName'

        {
            mX.setSymbolName(A7);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(A7 == X.symbolName());

            mX.setSymbolName(B7);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(B7 == X.symbolName());

            mX.setSymbolName(D7);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        // set all fields

        {
            mX.setAddress(A1);
            mX.setLibraryFileName(A2);
            mX.setLineNumber(A3);
            mX.setOffsetFromSymbol(A4);
            mX.setSourceFileName(A5);
            mX.setMangledSymbolName(A6);
            mX.setSymbolName(A7);
            ASSERT(A1 == X.address());
            ASSERT(A2 == X.libraryFileName());
            ASSERT(A3 == X.lineNumber());
            ASSERT(A4 == X.offsetFromSymbol());
            ASSERT(A5 == X.sourceFileName());
            ASSERT(A6 == X.mangledSymbolName());
            ASSERT(A7 == X.symbolName());

            mX.setAddress(B1);
            mX.setLibraryFileName(B2);
            mX.setLineNumber(B3);
            mX.setOffsetFromSymbol(B4);
            mX.setSourceFileName(B5);
            mX.setMangledSymbolName(B6);
            mX.setSymbolName(B7);
            ASSERT(B1 == X.address());
            ASSERT(B2 == X.libraryFileName());
            ASSERT(B3 == X.lineNumber());
            ASSERT(B4 == X.offsetFromSymbol());
            ASSERT(B5 == X.sourceFileName());
            ASSERT(B6 == X.mangledSymbolName());
            ASSERT(B7 == X.symbolName());

            mX.setAddress(D1);
            mX.setLibraryFileName(D2);
            mX.setLineNumber(D3);
            mX.setOffsetFromSymbol(D4);
            mX.setSourceFileName(D5);
            mX.setMangledSymbolName(D6);
            mX.setSymbolName(D7);
            ASSERT(D1 == X.address());
            ASSERT(D2 == X.libraryFileName());
            ASSERT(D3 == X.lineNumber());
            ASSERT(D4 == X.offsetFromSymbol());
            ASSERT(D5 == X.sourceFileName());
            ASSERT(D6 == X.mangledSymbolName());
            ASSERT(D7 == X.symbolName());
        }

        ASSERT(0 == defaultAllocator.numAllocations());
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
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (copy from 'x').      { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute Types

        typedef const void  *T1;        // 'address_p'
        typedef const char  *T2;        // 'libraryFileName_p'
        typedef int          T3;        // 'lineNumber'
        typedef bsl::size_t  T4;        // 'offsetFromSymbol'
        typedef const char  *T5;        // 'sourceFileName_p'
        typedef const char  *T6;        // 'mangledSymbolName_p'
        typedef const char  *T7;        // 'symbolName_p'

        // Attribute 1 Values: 'address'

        const T1 D1 = 0;                // default value
        const T1 A1 = &D1;              // non-zero value

        // Attribute 2 Values: 'libraryFileName'

        const T2 D2 = 0;                // default value
        const T2 A2 = "/lib/lib1.so";

        // Attribute 3 Values: 'lineNumber'

        const T3 D3 = -1;               // default value
        const T3 A3 = 10;

        // Attribute 4 Values: 'offsetFromSymbol'

        const T4 D4 = Obj::invalidOffset();    // default value
        const T4 A4 = 100;

        // Attribute 5 Values: 'sourceFileName'

        const T5 D5 = 0;                // default value
        const T5 A5 = "/a/b/c/d/mysource.cpp";

        // Attribute 6 Values: 'mangledSymbolName'

        const T6 D6 = 0;                // default value
        const T6 A6 = "mangledwoof";

        // Attribute 7 Values: "symbolName"

        const T7 D7 = 0;                // default value
        const T7 A7 = "woof";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.address());
        ASSERT(D2 == W.libraryFileName());
        ASSERT(D3 == W.lineNumber());
        ASSERT(D4 == W.offsetFromSymbol());
        ASSERT(D5 == W.sourceFileName());
        ASSERT(D6 == W.mangledSymbolName());
        ASSERT(D7 == W.symbolName());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.address());
        ASSERT(D2 == X.libraryFileName());
        ASSERT(D3 == X.lineNumber());
        ASSERT(D4 == X.offsetFromSymbol());
        ASSERT(D5 == X.sourceFileName());
        ASSERT(D6 == X.mangledSymbolName());
        ASSERT(D7 == X.symbolName());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setAddress(A1);
        mX.setLibraryFileName(A2);
        mX.setLineNumber(A3);
        mX.setOffsetFromSymbol(A4);
        mX.setSourceFileName(A5);
        mX.setMangledSymbolName(A6);
        mX.setSymbolName(A7);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.address());
        ASSERT(A2 == X.libraryFileName());
        ASSERT(A3 == X.lineNumber());
        ASSERT(A4 == X.offsetFromSymbol());
        ASSERT(A5 == X.sourceFileName());
        ASSERT(A6 == X.mangledSymbolName());
        ASSERT(A7 == X.symbolName());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (copy from 'Z')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(X);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.address());
        ASSERT(A2 == Y.libraryFileName());
        ASSERT(A3 == Y.lineNumber());
        ASSERT(A4 == Y.offsetFromSymbol());
        ASSERT(A5 == Y.sourceFileName());
        ASSERT(A6 == Y.mangledSymbolName());
        ASSERT(A7 == Y.symbolName());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.address());
        ASSERT(A2 == Z.libraryFileName());
        ASSERT(A3 == Z.lineNumber());
        ASSERT(A4 == Z.offsetFromSymbol());
        ASSERT(A5 == Z.sourceFileName());
        ASSERT(A6 == Z.mangledSymbolName());
        ASSERT(A7 == Z.symbolName());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setAddress(D1);
        mZ.setLibraryFileName(D2);
        mZ.setLineNumber(D3);
        mZ.setOffsetFromSymbol(D4);
        mZ.setSourceFileName(D5);
        mZ.setMangledSymbolName(D6);
        mZ.setSymbolName(D7);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.address());
        ASSERT(D2 == Z.libraryFileName());
        ASSERT(D3 == Z.lineNumber());
        ASSERT(D4 == Z.offsetFromSymbol());
        ASSERT(D5 == Z.sourceFileName());
        ASSERT(D6 == Z.mangledSymbolName());
        ASSERT(D7 == Z.symbolName());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.address());
        ASSERT(A2 == W.libraryFileName());
        ASSERT(A3 == W.lineNumber());
        ASSERT(A4 == W.offsetFromSymbol());
        ASSERT(A5 == W.sourceFileName());
        ASSERT(A6 == W.mangledSymbolName());
        ASSERT(A7 == W.symbolName());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.address());
        ASSERT(D2 == W.libraryFileName());
        ASSERT(D3 == W.lineNumber());
        ASSERT(D4 == W.offsetFromSymbol());
        ASSERT(D5 == W.sourceFileName());
        ASSERT(D6 == W.mangledSymbolName());
        ASSERT(D7 == W.symbolName());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.address());
        ASSERT(A2 == X.libraryFileName());
        ASSERT(A3 == X.lineNumber());
        ASSERT(A4 == X.offsetFromSymbol());
        ASSERT(A5 == X.sourceFileName());
        ASSERT(A6 == X.mangledSymbolName());
        ASSERT(A7 == X.symbolName());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
