// baesu_stacktrace.t.cpp                                             -*-C++-*-
#include <baesu_stacktrace.h>

#include <bslma_bufferallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single (value-semantic) container
// class.
//
// Primary Manipulators:
//: o 'append'
//: o 'removeAll'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'length'
//: o 'operator[]'
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
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CREATORS
// [  ] baesu_StackTrace(bslma_Allocator *bA = 0);
// [  ] baesu_StackTrace(const baesu_StackTrace& o, *bA = 0);
//
// MANIPULATORS
// [  ] baesu_StackTrace& operator=(const baesu_StackTrace& rhs);
// [  ] baesu_StackTraceFrame& operator[](int index);
// [  ] void append(const baesu_StackTraceFrame& value);
// [  ] void removeAll();
// [  ] void resize(int newLength);
// [  ] void swap(baesu_StackTrace& other);
//
// ACCESSORS
// [  ] const baesu_StackTraceFrame& operator[](int index) const;
// [  ] int length() const;
//
// [  ] bslma_Allocator *allocator() const;
// [  ] ostream& print(ostream& s, int level = 0, int sPL = 4) const;

// FREE OPERATORS
// [  ] bool operator==(const baesu_StackTrace& lhs, rhs);
// [  ] bool operator!=(const baesu_StackTrace& lhs, rhs);
// [  ] operator<<(ostream& s, const baesu_StackTrace& o);

// FREE FUNCTIONS
// [  ]void swap(baesu_StackTrace& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE

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
//
// [ 3] void stretch(baesu_StackTrace *object, int size);
// [ 3] void stretchRemoveAll(baesu_StackTrace *o, int size);
// [ 3] int ggg(baesu_StackTrace *o, const char *s, int vF = 1);
// [ 3] baesu_StackTrace& gg(baesu_StackTrace *o, const char *s);
// [ 8] baesu_StackTrace   g(const char *spec);

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baesu_StackTrace      Obj;
typedef baesu_StackTraceFrame Frame;
typedef Frame                 Element;

const Frame VALUES[]   = {

    Frame((void *)0x1234,
         "lib0.a",
           10,
         "_woof_0",
           50,
         "sourceFile0.cpp",
         "woof0"),

    Frame((void *)0xabcd,
         "lib1.a",
         110,
         "_woof_1",
         150,
         "/a/sourceFile1.cpp",
         "woof1"),

    Frame((void *)0xabcd,
         "lib2.a",
         210,
         "sourceFile2.cpp",
         250,
         "_woof_2",
         "woof2"),

    Frame((void *)0xcdef,
         "lib3.a",
         310,
         "/a/b/sourceFile3.cpp",
         350,
         "_woof_3",
         "woof3"),

    Frame((void *)0xef01,
         "lib4.a",
         410,
         "/a/b/c/sourceFile4.cpp",
         450,
         "_woof_4",
         "woof4"),

    Frame((void *)0x0123,
         "lib5.a",
         510,
         "/a/b/c/d/sourceFile5.cpp",
         550,
         "_woof_5",
         "woof5")
};

//const Frame VALUES[]   = { V0, V1, V2, V3, V4, V5 };  // avoid 'DEFAULT_VALUE'
const int   NUM_VALUES = sizeof VALUES / sizeof *VALUES;

const Element &V0 = VALUES[0],  &VA = V0, // 'V0', 'V1', ... are used in
              &V1 = VALUES[1],  &VB = V1, // conjunction with the 'VALUES'
              &V2 = VALUES[2],  &VC = V2, // array.
              &V3 = VALUES[3],  &VD = V3, // 'VA', 'VB', ... are used in
              &V4 = VALUES[4],  &VE = V4; // conjuction with 'g' and 'gg'.


// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bslalg_HasTrait<Obj,
                              bslalg_TypeTraitBitwiseMoveable>::VALUE));
BSLMF_ASSERT((bslalg_HasTrait<Obj,
                              bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

// JSL: Do we want to move this string to the component of bsl::string itself?
// JSL: e.g.,  #define BSLSTL_LONG_STRING ...   TBD!

#ifdef BSLS_PLATFORM__CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------
// JSL: REMOVE THIS after it is moved to the test allocator.
// JSL: change the name to 'bslma_TestAllocatorMonitor'.

class bslma_TestAllocatorMonitor {
    // TBD

    // DATA
    int                              d_lastInUse;
    int                              d_lastMax;
    int                              d_lastTotal;
    const bslma_TestAllocator *const d_allocator_p;

  public:
    // CREATORS
    bslma_TestAllocatorMonitor(const bslma_TestAllocator& basicAllocator);
        // TBD

    ~bslma_TestAllocatorMonitor();
        // TBD

    // ACCESSORS
    bool isInUseSame() const;
        // TBD

    bool isInUseUp() const;
        // TBD

    bool isMaxSame() const;
        // TBD

    bool isMaxUp() const;
        // TBD

    bool isTotalSame() const;
        // TBD

    bool isTotalUp() const;
        // TBD
};

// CREATORS
inline
bslma_TestAllocatorMonitor::bslma_TestAllocatorMonitor(
                                     const bslma_TestAllocator& basicAllocator)
: d_lastInUse(basicAllocator.numBlocksInUse())
, d_lastMax(basicAllocator.numBlocksMax())
, d_lastTotal(basicAllocator.numBlocksTotal())
, d_allocator_p(&basicAllocator)
{
}

inline
bslma_TestAllocatorMonitor::~bslma_TestAllocatorMonitor()
{
}

// ACCESSORS
inline
bool bslma_TestAllocatorMonitor::isInUseSame() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() == d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isInUseUp() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() != d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isMaxSame() const
{
    return d_allocator_p->numBlocksMax() == d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isMaxUp() const
{
    return d_allocator_p->numBlocksMax() != d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isTotalSame() const
{
    return d_allocator_p->numBlocksTotal() == d_lastTotal;
}

inline
bool bslma_TestAllocatorMonitor::isTotalUp() const
{
    return d_allocator_p->numBlocksTotal() != d_lastTotal;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void stretch(Obj *object, int size)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->append(V0);
    }
    ASSERT(object->length() >= size);
}

void stretchRemoveAll(Obj *object, int size)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size);
    object->removeAll();
    ASSERT(0 == object->length());
}

void outerP(const char *leader, const Obj &X)
    // This function is used to avert an uncaught exception on Windows during
    // bdema exception testing.  This can happen, e.g., in test cases with
    // large DATA sets.
{
    cout << leader; P(X);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters '[A .. E]' correspond to arbitrary (but unique) 'Element' values to
// be appended to the 'Obj' object.  A tilde ('~') indicates that the logical
// (but not necessarily physical) state of the object is to be set to its
// initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Append the value corresponding to A.
// "AA"         Append two values both corresponding to A.
// "ABC"        Append three values corresponding to A, B and C.
// "ABC~"       Append three values corresponding to A, B and C and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Append three values corresponding to A, B, and C; empty
//              the object; and append values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->append(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->removeAll();
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[i] << "') in spec \""
                     << spec << "\" at position " << i << '.' << endl;
            }
            return i;  // Discontinue processing this spec.
        }
   }
   return SUCCESS;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

class EqualityTester {
private:
        const Obj   *A;
        const Obj   *B;
public:
        EqualityTester(const Obj *a, const Obj *b);
        ~EqualityTester();
};

// CREATORS
inline EqualityTester::EqualityTester(const Obj *a, const Obj *b)
: A(a)
, B(b)
{
}

inline EqualityTester::~EqualityTester()
{
        ASSERT(*A == *B);
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
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 99: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Configuring a stack-trace value
/// - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate how to create a 'baesu_StackTrace'
// object, then both modify and access its value.
//
// First, we set up a test allocator as default allocator.  A
// 'baesu_StackTrace' object, by default, gets all its memory from an owned
// instance of 'bdema_HeapBypassAllocator'.  To demonstrate this we start by
// setting the default allocator to a test allocator so we can verify later
// that it was unused:
//..
    bslma_TestAllocator         da;
    bslma_DefaultAllocatorGuard guard(&da);
//..
// Next, we create a stack trace object.  Note that when we don't specify an
// allocator, the default allocator is not used -- rather, a heap bypass
// allocator owned by the stack trace object is used.  The heap bypass
// allocator is recommended because this component is often used to obtain
// debug information in instances where an error has occurred, and the
// possibility of heap corruption can't be ruled out.  The heap bypass
// allocator obtains its memory directly from virtual memory rather than going
// through the heap, avoiding potential complications due to heap corruption.
//..
    baesu_StackTrace stackTrace;
    ASSERT(0 == stackTrace.length());
//..
// Then, we 'resize' the stack-trace object to contain two default-constructed
// frames, and take references to each of the two new frames.
//..
    stackTrace.resize(2);
    ASSERT(2 == stackTrace.length());
    baesu_StackTraceFrame& frame0 = stackTrace[0];
    baesu_StackTraceFrame& frame1 = stackTrace[1];
//..
// Next, we set the values of the fields of the two new frames.
//..
    frame0.setAddress((void *) 0x12ab);
    frame0.setLibraryFileName("/a/b/c/baesu_stacktrace.t.dbg_exc_mt");
    frame0.setLineNumber(5);
    frame0.setOffsetFromSymbol(116);
    frame0.setSourceFileName("/a/b/c/sourceFile.cpp");
    frame0.setMangledSymbolName("_woof_1a");
    frame0.setSymbolName("woof");

    frame1.setAddress((void *) 0x34cd);
    frame1.setLibraryFileName("/lib/libd.a");
    frame1.setLineNumber(15);
    frame1.setOffsetFromSymbol(228);
    frame1.setSourceFileName("/a/b/c/secondSourceFile.cpp");
    frame1.setMangledSymbolName("_arf_1a");
    frame1.setSymbolName("arf");
//..
// Then, we verify the frames have the values we expect.
//..
    ASSERT((void *) 0x12ab               == frame0.address());
    ASSERT("/a/b/c/baesu_stacktrace.t.dbg_exc_mt"
                                         == frame0.libraryFileName());
    ASSERT(  5                           == frame0.lineNumber());
    ASSERT(116                           == frame0.offsetFromSymbol());
    ASSERT("/a/b/c/sourceFile.cpp"       == frame0.sourceFileName());
    ASSERT("_woof_1a"                    == frame0.mangledSymbolName());
    ASSERT("woof"                        == frame0.symbolName());

    ASSERT((void *) 0x34cd               == frame1.address());
    ASSERT("/lib/libd.a"                 == frame1.libraryFileName());
    ASSERT( 15                           == frame1.lineNumber());
    ASSERT(228                           == frame1.offsetFromSymbol());
    ASSERT("/a/b/c/secondSourceFile.cpp" == frame1.sourceFileName());
    ASSERT("_arf_1a"                     == frame1.mangledSymbolName());
    ASSERT("arf"                         == frame1.symbolName());
//..
// Next, we output the stack trace object.
//..
    stackTrace.print(cout, 1, 2);
//..
// Finally, we observe the default allocator was never used.
//..
    ASSERT(0 == da.numAllocations());
//..
// The above usage produces the following output:
//..
//  [
//    [
//      address = 0x12ab
//      library file name = "/a/b/c/baesu_stacktrace.t.dbg_exc_mt"
//      line number = 5
//      mangled symbol name = "_woof_1a"
//      offset from symbol = 116
//      source file name = "/a/b/c/sourceFile.cpp"
//      symbol name = "woof"
//    ]
//    [
//      address = 0x34cd
//      library file name = "/lib/libd.a"
//      line number = 15
//      mangled symbol name = "_arf_1a"
//      offset from symbol = 228
//      source file name = "/a/b/c/secondSourceFile.cpp"
//      symbol name = "arf"
//    ]
//  ]
//..

      }  break;
      case 102: {
#if 0
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
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..14)
        //:
        //:   1 Create three 'bslma_TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values, then
        //:     setting all of the attributes to their 'B' values.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   baetzo_LocalTimeDescriptor(bslma_Allocator *bA = 0);
        //   ~baetzo_LocalTimeDescriptor();
        //   setDescription(const StringRef& value);
        //   setDstInEffectFlag(bool value);
        //   setUtcOffsetInSeconds(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        Frame mFA(&ta);      const Frame& FA = mFA;
        mFA.setLineNumber(5);
        mFA.setSymbolName("woof");

        Frame mFB(&ta);      const Frame& FB = mFB;
        mFB.setOffsetFromSymbol(20);
        mFB.setLibraryFileName("arf");

        Frame mFC(&ta);      const Frame& FC = mFC;
        mFC.setMangledSymbolName("urp");
        mFC.setSourceFileName("um");

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'c'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma_TestAllocator da("default2",  veryVeryVeryVerbose);
            bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma_Default::setDefaultAllocatorRaw(&da);

            Obj                 *objPtr;
            bslma_TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              // Exemplar cases 'a' and 'b' were eliminated, as they use the
              // internal heap bypass allocator, which is not a test allocator.

              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma_TestAllocator&  oa = *objAllocatorPtr;
            bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Also invoke the object's 'allocator' accessor.

            LOOP5_ASSERT(CONFIG, &da, &sa, &oa, X.allocator(),
                                                         &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP_ASSERT(CONFIG, 0 == X.length());

            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    const int inUse = oa.numBytesInUse();

                    mX.resize(3);
                    mX[0] = FA;
                    mX[1] = FB;
                    mX[2] = FC;
                    LOOP_ASSERT(CONFIG, inUse < oa.numBytesInUse());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                LOOP_ASSERT(CONFIG, &oa == mX[0].allocator());

                LOOP_ASSERT(CONFIG, "woof" == X[0].symbolName());
                LOOP_ASSERT(CONFIG, ""     == X[0].sourceFileName());

                const int numA  = oa.numAllocations();
                const int inUse = oa.numBytesInUse();

                mX[0].setSymbolName("bark");

                LOOP_ASSERT(CONFIG, "bark" == X[0].symbolName());
                LOOP_ASSERT(CONFIG, ""     == X[0].sourceFileName());

                mX[0].setSymbolName("");

                LOOP_ASSERT(CONFIG, ""     == X[0].symbolName());
                LOOP_ASSERT(CONFIG, ""     == X[0].sourceFileName());

                LOOP_ASSERT(CONFIG, numA  == oa.numAllocations());
                LOOP_ASSERT(CONFIG, inUse == oa.numBytesInUse());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            LOOP3_ASSERT(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                         oa.numBlocksTotal() == oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            if (CONFIG != 'a') {
                LOOP_ASSERT(CONFIG, 0 == da.numBlocksTotal());
            }
        }
#endif

      } break;
      case 101: {
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
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "Creator Test\n"
                             "============\n";

        Frame mFA;      const Frame& FA = mFA;
        mFA.setLineNumber(5);
        mFA.setSymbolName("woof");

        Frame mFB;      const Frame& FB = mFB;
        mFB.setOffsetFromSymbol(20);
        mFB.setLibraryFileName("arf");

        Frame mFC;      const Frame& FC = mFC;
        mFC.setMangledSymbolName("urp");
        mFC.setSourceFileName("um");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(0 == W.length());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(0 == X.length());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.resize(3);
        mX[0] = FA;
        mX[1] = FB;
        mX[2] = FC;

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(3 == X.length());
        ASSERT(FA == X[0]);
        ASSERT(FB == X[1]);
        ASSERT(FC == X[2]);

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(X);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(3 == Y.length());
        ASSERT(FA == Y[0]);
        ASSERT(FB == Y[1]);
        ASSERT(FC == Y[2]);

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

        ASSERT(3 == Z.length());
        ASSERT(FA == Z[0]);
        ASSERT(FB == Z[1]);
        ASSERT(FC == Z[2]);

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.removeAll();

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(0 == Z.length());
        ASSERT(W == Z);

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

        ASSERT(3 == W.length());
        ASSERT(FA == W[0]);
        ASSERT(FB == W[1]);
        ASSERT(FC == W[2]);

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

        ASSERT(0 == W.length());

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

        ASSERT(3 == X.length());
        ASSERT(FA == X[0]);
        ASSERT(FB == X[1]);
        ASSERT(FC == X[2]);

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

        Obj mU(X, X.allocator());    const Obj& U = mU;
        ASSERT(3 == U.length());
        ASSERT(U == X);

        bsl::swap(mU[0], mU[2]);

        ASSERT(U != X);
        ASSERT(X[0] == FA);    ASSERT(X[1] == FB);    ASSERT(X[2] == FC);
        ASSERT(U[0] == FC);    ASSERT(U[1] == FB);    ASSERT(U[2] == FA);

        mU.swap(mX);
        ASSERT(3 == U.length());
        ASSERT(3 == X.length());
        ASSERT(U != X);
        ASSERT(U[0] == FA);    ASSERT(U[1] == FB);    ASSERT(U[2] == FC);
        ASSERT(X[0] == FC);    ASSERT(X[1] == FB);    ASSERT(X[2] == FA);

        swap(mU, mX);
        ASSERT(3 == U.length());
        ASSERT(3 == X.length());
        ASSERT(U != X);
        ASSERT(X[0] == FA);    ASSERT(X[1] == FB);    ASSERT(X[2] == FC);
        ASSERT(U[0] == FC);    ASSERT(U[1] == FB);    ASSERT(U[2] == FA);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // APPEND
        //
        // Concerns:
        //: 1 The source is left unaffected (apart from aliasing).
        //: 2 The subsequent existing of the source has no effect on the
        //:    result object (apart from aliasing).
        //: 3 The function is alias safe.
        //: 4 The function is exception neutral (w.r.t. allocation).
        //: 5 The function preserves object invariants.
        //: 6 The function is independent of internal representation.
        //
        // Note that all (contingent) reallocations occur strictly before the
        // essential implementation of each method.  Therefore, C-1, C-2, and
        // C-4 above are valid for objects in the "canonical state", but need
        // not be repeated when C-6 ("white-box test") is addressed.
        //
        // Plan:
        //: 1 Use the data tables and protocols conventionally used for testing
        //:   containers; however, as the only supported mentod is 'append"
        //:   for an item, most of these tests are removed.
        //: 2 Use the enumeration technique to a depth of 5 for both the normal
        //:  and alias cases.  Data is tabulated explicitly for the 'insert'
        //:  method that takes a range from a source array (or itself, for the
        //:  aliasing test); other methods are tested using a subset of the
        //:  full test vector table.  In particular, the 'append' methods use
        //:  data where the destination index equals the destination length
        //:  ('strlen(D_SPEC) == DI').  All methods using the entire source
        //:  object use test data where the source length equals the number of
        //:  elements ('strlen(S_SPEC) == NE'), while the "scalar" methods use
        //:  data where the number of elements equals 1 ('1 == NE').  In
        //:  addition, the 'remove' methods switch the 'd-array' and 'expected'
        //:  values from the 'insert' table.
        //: o In the "canonical state" (black-box) tests, we confirm that the
        //:   source is unmodified by the method call, and that its subsequent
        //:   destruction has no effect on the destination object.
        //: o In all cases we want to make sure that after the application of
        //:   the operation, the object is allowed to go out of scope directly
        //:   to enable the destructor to assert object invariants.
        //: o Each object constructed should be wrapped in separate BSLMA test
        //:   assert macros and use 'gg' as an optimization.
        //
        // Testing:
        //   void append(const baesu_StackTraceFrame& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "APPEND" << endl
                          << "======" << endl;
        if (verbose) cout << "\nWithout Aliasing" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to insert into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to insert from sa
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "A",     0,  0, ""      },
                { L_,   "",      0,  "A",     0,  1, "A"     },
                { L_,   "",      0,  "A",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "BA"    },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  1, "AB"    },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    0,  1, "A"     },
                { L_,   "",      0,  "AB",    0,  2, "AB"    },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    1,  1, "B"     },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CAB"   },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "ACB"   },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  1, "ABC"   },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "BA"    },
                { L_,   "A",     0,  "BC",    0,  2, "BCA"   },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "CA"    },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  1, "AB"    },
                { L_,   "A",     1,  "BC",    0,  2, "ABC"   },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  1, "AC"    },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   0,  1, "A"     },
                { L_,   "",      0,  "ABC",   0,  2, "AB"    },
                { L_,   "",      0,  "ABC",   0,  3, "ABC"   },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  1, "B"     },
                { L_,   "",      0,  "ABC",   1,  2, "BC"    },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  1, "C"     },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DABC"  },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CAB"   },
                { L_,   "AB",    0,  "CD",    0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DAB"   },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "ACB"   },
                { L_,   "AB",    1,  "CD",    0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "ADB"   },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  1, "ABC"   },
                { L_,   "AB",    2,  "CD",    0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  1, "ABD"   },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "BA"    },
                { L_,   "A",     0,  "BCD",   0,  2, "BCA"   },
                { L_,   "A",     0,  "BCD",   0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "CA"    },
                { L_,   "A",     0,  "BCD",   1,  2, "CDA"   },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "DA"    },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  1, "AB"    },
                { L_,   "A",     1,  "BCD",   0,  2, "ABC"   },
                { L_,   "A",     1,  "BCD",   0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  1, "AC"    },
                { L_,   "A",     1,  "BCD",   1,  2, "ACD"   },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  1, "AD"    },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  0,  1, "A"     },
                { L_,   "",      0,  "ABCD",  0,  2, "AB"    },
                { L_,   "",      0,  "ABCD",  0,  3, "ABC"   },
                { L_,   "",      0,  "ABCD",  0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  1, "B"     },
                { L_,   "",      0,  "ABCD",  1,  2, "BC"    },
                { L_,   "",      0,  "ABCD",  1,  3, "BCD"   },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  1, "C"     },
                { L_,   "",      0,  "ABCD",  2,  2, "CD"    },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  1, "D"     },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  0,  "E",     0,  1, "EABCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  1, "AEBCD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABECD" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCED" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  1, "ABCDE" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DABC"  },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEABC" },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EABC"  },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADEBC" },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEBC"  },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "DE",    0,  2, "ABDEC" },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABEC"  },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "DE",    0,  2, "ABCDE" },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  1, "ABCE"  },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CAB"   },
                { L_,   "AB",    0,  "CDE",   0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CDE",   0,  3, "CDEAB" },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DAB"   },
                { L_,   "AB",    0,  "CDE",   1,  2, "DEAB"  },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EAB"   },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "ACB"   },
                { L_,   "AB",    1,  "CDE",   0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CDE",   0,  3, "ACDEB" },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "ADB"   },
                { L_,   "AB",    1,  "CDE",   1,  2, "ADEB"  },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AEB"   },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  1, "ABC"   },
                { L_,   "AB",    2,  "CDE",   0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CDE",   0,  3, "ABCDE" },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  1, "ABD"   },
                { L_,   "AB",    2,  "CDE",   1,  2, "ABDE"  },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  1, "ABE"   },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "BA"    },
                { L_,   "A",     0,  "BCDE",  0,  2, "BCA"   },
                { L_,   "A",     0,  "BCDE",  0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "CA"    },
                { L_,   "A",     0,  "BCDE",  1,  2, "CDA"   },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "DA"    },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  1, "AB"    },
                { L_,   "A",     1,  "BCDE",  0,  2, "ABC"   },
                { L_,   "A",     1,  "BCDE",  0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  1, "AC"    },
                { L_,   "A",     1,  "BCDE",  1,  2, "ACD"   },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  1, "AD"    },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 0,  1, "A"     },
                { L_,   "",      0,  "ABCDE", 0,  2, "AB"    },
                { L_,   "",      0,  "ABCDE", 0,  3, "ABC"   },
                { L_,   "",      0,  "ABCDE", 0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCDE", 0,  5, "ABCDE" },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  1, "B"     },
                { L_,   "",      0,  "ABCDE", 1,  2, "BC"    },
                { L_,   "",      0,  "ABCDE", 1,  3, "BCD"   },
                { L_,   "",      0,  "ABCDE", 1,  4, "BCDE"  },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  1, "C"     },
                { L_,   "",      0,  "ABCDE", 2,  2, "CD"    },
                { L_,   "",      0,  "ABCDE", 2,  3, "CDE"   },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  1, "D"     },
                { L_,   "",      0,  "ABCDE", 3,  2, "DE"    },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  1, "E"     },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                const Obj DD(g(D_SPEC));          // control for destination
                const Obj SS(g(S_SPEC));          // control for source
                const Obj EE(g(E_SPEC));          // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.append(s[SI]);                // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout << "\nWith Aliasing" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,   0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,   0,  0,  "A"     },
                { L_,   "A",     0,   0,  1,  "AA"    },
                { L_,   "A",     0,   1,  0,  "A"     },

                { L_,   "A",     1,   0,  0,  "A"     },
                { L_,   "A",     1,   0,  1,  "AA"    },
                { L_,   "A",     1,   1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,   0,  0,  "AB"    },
                { L_,   "AB",    0,   0,  1,  "AAB"   },
                { L_,   "AB",    0,   0,  2,  "ABAB"  },
                { L_,   "AB",    0,   1,  0,  "AB"    },
                { L_,   "AB",    0,   1,  1,  "BAB"   },
                { L_,   "AB",    0,   2,  0,  "AB"    },

                { L_,   "AB",    1,   0,  0,  "AB"    },
                { L_,   "AB",    1,   0,  1,  "AAB"   },
                { L_,   "AB",    1,   0,  2,  "AABB"  },
                { L_,   "AB",    1,   1,  0,  "AB"    },
                { L_,   "AB",    1,   1,  1,  "ABB"   },
                { L_,   "AB",    1,   2,  0,  "AB"    },

                { L_,   "AB",    2,   0,  0,  "AB"    },
                { L_,   "AB",    2,   0,  1,  "ABA"   },
                { L_,   "AB",    2,   0,  2,  "ABAB"  },
                { L_,   "AB",    2,   1,  0,  "AB"    },
                { L_,   "AB",    2,   1,  1,  "ABB"   },
                { L_,   "AB",    2,   2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,   0,  0,  "ABC"     },
                { L_,   "ABC",   0,   0,  1,  "AABC"    },
                { L_,   "ABC",   0,   0,  2,  "ABABC"   },
                { L_,   "ABC",   0,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   0,   1,  0,  "ABC"     },
                { L_,   "ABC",   0,   1,  1,  "BABC"    },
                { L_,   "ABC",   0,   1,  2,  "BCABC"   },
                { L_,   "ABC",   0,   2,  0,  "ABC"     },
                { L_,   "ABC",   0,   2,  1,  "CABC"    },
                { L_,   "ABC",   0,   3,  0,  "ABC"     },

                { L_,   "ABC",   1,   0,  0,  "ABC"     },
                { L_,   "ABC",   1,   0,  1,  "AABC"    },
                { L_,   "ABC",   1,   0,  2,  "AABBC"   },
                { L_,   "ABC",   1,   0,  3,  "AABCBC"  },
                { L_,   "ABC",   1,   1,  0,  "ABC"     },
                { L_,   "ABC",   1,   1,  1,  "ABBC"    },
                { L_,   "ABC",   1,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   1,   2,  0,  "ABC"     },
                { L_,   "ABC",   1,   2,  1,  "ACBC"    },
                { L_,   "ABC",   1,   3,  0,  "ABC"     },

                { L_,   "ABC",   2,   0,  0,  "ABC"     },
                { L_,   "ABC",   2,   0,  1,  "ABAC"    },
                { L_,   "ABC",   2,   0,  2,  "ABABC"   },
                { L_,   "ABC",   2,   0,  3,  "ABABCC"  },
                { L_,   "ABC",   2,   1,  0,  "ABC"     },
                { L_,   "ABC",   2,   1,  1,  "ABBC"    },
                { L_,   "ABC",   2,   1,  2,  "ABBCC"   },
                { L_,   "ABC",   2,   2,  0,  "ABC"     },
                { L_,   "ABC",   2,   2,  1,  "ABCC"    },
                { L_,   "ABC",   2,   3,  0,  "ABC"     },

                { L_,   "ABC",   3,   0,  0,  "ABC"     },
                { L_,   "ABC",   3,   0,  1,  "ABCA"    },
                { L_,   "ABC",   3,   0,  2,  "ABCAB"   },
                { L_,   "ABC",   3,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   3,   1,  0,  "ABC"     },
                { L_,   "ABC",   3,   1,  1,  "ABCB"    },
                { L_,   "ABC",   3,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   3,   2,  0,  "ABC"     },
                { L_,   "ABC",   3,   2,  1,  "ABCC"    },
                { L_,   "ABC",   3,   3,  0,  "ABC"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  0,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  0,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  0,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  0,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   1,  1,  "BABCD"     },
                { L_,   "ABCD",  0,   1,  2,  "BCABCD"    },
                { L_,   "ABCD",  0,   1,  3,  "BCDABCD"   },
                { L_,   "ABCD",  0,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   2,  1,  "CABCD"     },
                { L_,   "ABCD",  0,   2,  2,  "CDABCD"    },
                { L_,   "ABCD",  0,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   3,  1,  "DABCD"     },
                { L_,   "ABCD",  0,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  1,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  1,   0,  2,  "AABBCD"    },
                { L_,   "ABCD",  1,   0,  3,  "AABCBCD"   },
                { L_,   "ABCD",  1,   0,  4,  "AABCDBCD"  },
                { L_,   "ABCD",  1,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  1,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  1,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  1,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   2,  1,  "ACBCD"     },
                { L_,   "ABCD",  1,   2,  2,  "ACDBCD"    },
                { L_,   "ABCD",  1,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   3,  1,  "ADBCD"     },
                { L_,   "ABCD",  1,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  2,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   0,  1,  "ABACD"     },
                { L_,   "ABCD",  2,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  2,   0,  3,  "ABABCCD"   },
                { L_,   "ABCD",  2,   0,  4,  "ABABCDCD"  },
                { L_,   "ABCD",  2,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  2,   1,  2,  "ABBCCD"    },
                { L_,   "ABCD",  2,   1,  3,  "ABBCDCD"   },
                { L_,   "ABCD",  2,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  2,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  2,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   3,  1,  "ABDCD"     },
                { L_,   "ABCD",  2,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  3,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   0,  1,  "ABCAD"     },
                { L_,   "ABCD",  3,   0,  2,  "ABCABD"    },
                { L_,   "ABCD",  3,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  3,   0,  4,  "ABCABCDD"  },
                { L_,   "ABCD",  3,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   1,  1,  "ABCBD"     },
                { L_,   "ABCD",  3,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  3,   1,  3,  "ABCBCDD"   },
                { L_,   "ABCD",  3,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  3,   2,  2,  "ABCCDD"    },
                { L_,   "ABCD",  3,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  3,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  4,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   0,  1,  "ABCDA"     },
                { L_,   "ABCD",  4,   0,  2,  "ABCDAB"    },
                { L_,   "ABCD",  4,   0,  3,  "ABCDABC"   },
                { L_,   "ABCD",  4,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  4,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   1,  1,  "ABCDB"     },
                { L_,   "ABCD",  4,   1,  2,  "ABCDBC"    },
                { L_,   "ABCD",  4,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  4,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   2,  1,  "ABCDC"     },
                { L_,   "ABCD",  4,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  4,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  4,   4,  0,  "ABCD"      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 0,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 0,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 0,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 0,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 0,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   1,  1,  "BABCDE"     },
                { L_,   "ABCDE", 0,   1,  2,  "BCABCDE"    },
                { L_,   "ABCDE", 0,   1,  3,  "BCDABCDE"   },
                { L_,   "ABCDE", 0,   1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE", 0,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   2,  1,  "CABCDE"     },
                { L_,   "ABCDE", 0,   2,  2,  "CDABCDE"    },
                { L_,   "ABCDE", 0,   2,  3,  "CDEABCDE"   },
                { L_,   "ABCDE", 0,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   3,  1,  "DABCDE"     },
                { L_,   "ABCDE", 0,   3,  2,  "DEABCDE"    },
                { L_,   "ABCDE", 0,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   4,  1,  "EABCDE"     },
                { L_,   "ABCDE", 0,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 1,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 1,   0,  2,  "AABBCDE"    },
                { L_,   "ABCDE", 1,   0,  3,  "AABCBCDE"   },
                { L_,   "ABCDE", 1,   0,  4,  "AABCDBCDE"  },
                { L_,   "ABCDE", 1,   0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE", 1,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 1,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 1,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 1,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 1,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   2,  1,  "ACBCDE"     },
                { L_,   "ABCDE", 1,   2,  2,  "ACDBCDE"    },
                { L_,   "ABCDE", 1,   2,  3,  "ACDEBCDE"   },
                { L_,   "ABCDE", 1,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   3,  1,  "ADBCDE"     },
                { L_,   "ABCDE", 1,   3,  2,  "ADEBCDE"    },
                { L_,   "ABCDE", 1,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   4,  1,  "AEBCDE"     },
                { L_,   "ABCDE", 1,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 2,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   0,  1,  "ABACDE"     },
                { L_,   "ABCDE", 2,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 2,   0,  3,  "ABABCCDE"   },
                { L_,   "ABCDE", 2,   0,  4,  "ABABCDCDE"  },
                { L_,   "ABCDE", 2,   0,  5,  "ABABCDECDE" },
                { L_,   "ABCDE", 2,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 2,   1,  2,  "ABBCCDE"    },
                { L_,   "ABCDE", 2,   1,  3,  "ABBCDCDE"   },
                { L_,   "ABCDE", 2,   1,  4,  "ABBCDECDE"  },
                { L_,   "ABCDE", 2,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 2,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 2,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 2,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   3,  1,  "ABDCDE"     },
                { L_,   "ABCDE", 2,   3,  2,  "ABDECDE"    },
                { L_,   "ABCDE", 2,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   4,  1,  "ABECDE"     },
                { L_,   "ABCDE", 2,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 3,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   0,  1,  "ABCADE"     },
                { L_,   "ABCDE", 3,   0,  2,  "ABCABDE"    },
                { L_,   "ABCDE", 3,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 3,   0,  4,  "ABCABCDDE"  },
                { L_,   "ABCDE", 3,   0,  5,  "ABCABCDEDE" },
                { L_,   "ABCDE", 3,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   1,  1,  "ABCBDE"     },
                { L_,   "ABCDE", 3,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 3,   1,  3,  "ABCBCDDE"   },
                { L_,   "ABCDE", 3,   1,  4,  "ABCBCDEDE"  },
                { L_,   "ABCDE", 3,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 3,   2,  2,  "ABCCDDE"    },
                { L_,   "ABCDE", 3,   2,  3,  "ABCCDEDE"   },
                { L_,   "ABCDE", 3,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 3,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 3,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   4,  1,  "ABCEDE"     },
                { L_,   "ABCDE", 3,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 4,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   0,  1,  "ABCDAE"     },
                { L_,   "ABCDE", 4,   0,  2,  "ABCDABE"    },
                { L_,   "ABCDE", 4,   0,  3,  "ABCDABCE"   },
                { L_,   "ABCDE", 4,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 4,   0,  5,  "ABCDABCDEE" },
                { L_,   "ABCDE", 4,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   1,  1,  "ABCDBE"     },
                { L_,   "ABCDE", 4,   1,  2,  "ABCDBCE"    },
                { L_,   "ABCDE", 4,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 4,   1,  4,  "ABCDBCDEE"  },
                { L_,   "ABCDE", 4,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   2,  1,  "ABCDCE"     },
                { L_,   "ABCDE", 4,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 4,   2,  3,  "ABCDCDEE"   },
                { L_,   "ABCDE", 4,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 4,   3,  2,  "ABCDDEE"    },
                { L_,   "ABCDE", 4,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 4,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 5,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   0,  1,  "ABCDEA"     },
                { L_,   "ABCDE", 5,   0,  2,  "ABCDEAB"    },
                { L_,   "ABCDE", 5,   0,  3,  "ABCDEABC"   },
                { L_,   "ABCDE", 5,   0,  4,  "ABCDEABCD"  },
                { L_,   "ABCDE", 5,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 5,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   1,  1,  "ABCDEB"     },
                { L_,   "ABCDE", 5,   1,  2,  "ABCDEBC"    },
                { L_,   "ABCDE", 5,   1,  3,  "ABCDEBCD"   },
                { L_,   "ABCDE", 5,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 5,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   2,  1,  "ABCDEC"     },
                { L_,   "ABCDE", 5,   2,  2,  "ABCDECD"    },
                { L_,   "ABCDE", 5,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 5,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   3,  1,  "ABCDED"     },
                { L_,   "ABCDE", 5,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 5,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 5,   5,  0,  "ABCDE"      },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                const Obj DD(g(X_SPEC));          // control for destination
                const Obj EE(g(E_SPEC));          // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);


                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

#if TBD
                LOOP_ASSERT(LINE,
                            NUM_BLOCKS == testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE,
                            NUM_BYTES  == testAllocator.numBytesInUse());
#endif
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RESIZE
        //
        // Concerns:
        //: 1 The resulting length is correct and the resulting element values
        //:   are correct when:
        //:   o 'new length <  initial length'
        //:   o 'new length == initial length'
        //:   o 'new length >  initial length'
        //:
        //: 2 We are also concerned that the test data include sufficient
        //:   differences in initial and final length that resizing is
        //:   guaranteed to occur.  Beyond that, no explicit "white box" test
        //:   is required.
        //
        // Plan:
        //: 1 Specify a set 'A' of lengths.  For each 'a1' in 'A' construct an
        //:   object 'x' of length 'a1' with each element in 'x' initialized to
        //:   an arbitrary but known value 'V'.  For each 'a2' in 'A' use the
        //:   'resize' method to set the length of 'x' and potentially remove
        //:   or set element values as per the method's contract.  Use the
        //:   basic accessors to verify the length and element values of the
        //:   modified object 'x'.
        //
        // Testing:
        //   void resize(int newLength);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESIZE" << endl
                          << "======" << endl;

        if (verbose) cout << "\nTesting 'resize(int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];

            const Element I_VALUE       = VA;
            const Element DEFAULT_VALUE = Element();

            static const char *SPECS[] = { 
                "",                  //  0
                "A",                 //  1
                "AA",                //  2
                "AAA",               //  3
                "AAAA",              //  4
                "AAAAA",             //  5

                "AAAAAAA",           //  7
                "AAAAAAAA",          //  8
                "AAAAAAAAA",         //  9

                "AAAAAAAAAAAAAAA",   // 15
                "AAAAAAAAAAAAAAAA",  // 16
                "AAAAAAAAAAAAAAAAA", // 17

                 0 // Null string required as last element.
            };

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            for (int i = 0; i < NUM_TESTS; ++i) {
                const int   a1   = lengths[i];
                const char *SPEC = SPECS[i]; LOOP_ASSERT(i,
                                                         a1 == strlen(SPEC));
                const Obj   o1   = g(SPEC);

                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(o1, &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.resize(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, DEFAULT_VALUE == X[k]);
                        }
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // INITIAL LENGTH CONSTRUCTORS
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for intial-length constructors.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INITIAL LENGTH CONSTRUCTORS" << endl
                          << "===========================" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1  The value represented by any instance can be assigned to any
        //:    other instance regardless of how either value is represented
        //:    internally.
        //: 2  The 'rhs' value must not be affected by the operation.
        //: 3  'rhs' going out of scope has no effect on the value of 'lhs'
        //:     after the assignment.
        //: 4  Aliasing '(x = x)': The assignment operator must always work --
        //:    even when the 'lhs' and 'rhs' are identically the same object.
        //: 5  The assignment operator must be neutral with respect to memory
        //:    allocation exceptions.
        //
        // Plan:
        //: 1 Specify a set 'S' of unique object values with substantial and
        //:   varied differences, ordered by increasing length.  For each value
        //:   in 'S', construct an object 'x' along with a sequence of
        //:   similarly constructed duplicates 'x1', 'x2', ..., 'xN'.  Attempt
        //:   to affect every aspect of white-box state by altering each 'xi'
        //:   in a unique way.  Let the union of all such objects be the set
        //:   'T'.
        //:
        //: 2 To address C-1, C-2, and C-5, construct tests 'u = v' for all
        //:   '(u, v)' in 'T X T'.  Using canonical controls 'UU' and 'VV',
        //:   assert before the assignment that 'UU == u', 'VV == v', and
        //:   'v == u' iff 'VV == UU'.  After the assignment, assert that
        //:   'VV == u', 'VV == v', and, for grins, that 'v == u'.  Let 'v' go
        //:   out of scope and confirm that 'VV == u'.  All of these tests are
        //:   performed within the 'bslma' exception testing apparatus.
        //:
        //: 3 As a separate exercise, we address C-4 and C-5 by constructing
        //:   tests 'y = y' for all 'y' in 'T'.  Using a canonical control 'X',
        //:   we will verify that 'X == y' before and after the assignment,
        //:   again within the 'bslma' exception testing apparatus.
        //
        // Testing:
        //   baesu_StackTrace& operator=(const baesu_StackTrace& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9,
                "",        "A",    "BC",     "CDE",    "DEAB",   "EABCD",
                "AEDCBAE",         "CBAEDCBA",         "EDCBAEDCB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator); stretchRemoveAll(&mU, U_N);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV; gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g':
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each 'SPEC' in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed 'OBJECT' configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, use 'sizeof' to confirm
        //   that the (temporary) returned by 'g' differs in size from that
        //   returned by 'gg'.
        //
        // Testing:
        //   baesu_StackTrace   g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'g'" << endl
                          << "======================" << endl;

        static const char *SPECS[] = {
            "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0}; // Null string required as last element.

        bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x(&testAllocator);                      // runtime tests
            Obj& r1 = gg(&x, spec);
            Obj& r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original object
        //:   (relying on the previously tested equality operators).
        //: 2 All internal representations of a given value can be used to
        //:   create a new object of equivalent value.
        //: 3 The value of the original object is left unaffected.
        //: 4 Subsequent changes in or destruction of the source object have no
        //:   effect on the copy-constructed object.
        //: 5 The function is exception neutral w.r.t. memory allocation.
        //: 6 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //
        // Plan:
        //: 1 To address C-1, C2, and C-3, specify a set 'S' of object values
        //:   with substantial and varied differences, ordered by increasing
        //:   length.  For each value in 'S', initialize objects 'w' and 'x',
        //:   copy construct y from 'x' and use 'operator==' to verify that
        //:   both 'x' and 'y' subsequently have the same value as 'w'.  Let
        //:   'x' go out of scope and again verify that 'w == x'.  Repeat this
        //:   test with 'x' having the same *logical* value, but perturbed so
        //:   as to have potentially different internal representations.
        //:
        //: 2 To address C-5, we will perform each of the above tests in the
        //:   presence of exceptions during memory allocations using a
        //:   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //: 3 To address C-6, we will repeat the above tests:
        //:   1 When passing in no allocator.
        //:   2 When passing in a null pointer: (bslma_Allocator *)0.
        //:   3 When passing in a test allocator (see concern 5).
        //:   4 Where the object is constructed entirely in static memory
        //:     (using a 'bslma_BufferAllocator') and never destroyed.
        //:   5 After the (dynamically allocated) source object is deleted and
        //:     its footprint erased (see concern 4).
        //
        // Testing:
         //  baesu_StackTrace(const baesu_StackTrace& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;
        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW(&testAllocator); gg(&mW, SPEC); const Obj& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj &mX = *pX;              stretchRemoveAll(&mX, N);
                    const Obj& X = mX;          gg(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {
                        if (veryVeryVerbose) { cout <<
                                                "\t\t\tNo Allocator" << endl; }
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {
                        if (veryVeryVerbose) { cout <<
                                              "\t\t\tNull Allocator" << endl; }
                        const Obj Y1(X, (bslma_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        if (veryVeryVerbose) { cout <<
                                              "\t\t\tTest Allocator" << endl; }
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    {                           
                        if (veryVeryVerbose) { cout <<
                                            "\t\t\tBuffer Allocator" << endl; }
                        char memory[1024 * 1024]; // TBD: find lower bound?
                        bslma_BufferAllocator a(memory, sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {
                        if (veryVeryVerbose) { cout <<
                                   "\t\t\tWith 'original' destroyed" << endl; }
                        const Obj Y2(X, &testAllocator);

                        // testAllocator will erase the footprint of pX
                        // preventing further reference to this object.

                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        //LOOP2_ASSERT(SPEC, N, W == X);// This work before!!
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // TESTING EQUALITY OPERATORS:
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value:
        //      - d_size
        //      - the (corresponding) amount of dynamically allocated memory
        //
        // Also check that:
        //: o The equality operator's signature and return type are standard.
        //: o The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 First, specify a set 'S' of unique object values having various
        //:   minor or subtle differences, ordered by non-decreasing length.
        //:   Verify the correctness of 'operator==' and 'operator!='
        //:   (returning either 'true' or 'false') using all elements '(u, v)'
        //:   of the cross product 'S X S'.
        //:
        //: 2 Next, specify a second set 'S' containing a representative
        //:   variety of (black-box) box values ordered by increasing (logical)
        //:   length.  For each value in S', construct an object 'x' along with
        //:   a sequence of similarly constructed duplicates 'x1', 'x2', ...,
        //:   'xN'.  Attempt to affect every aspect of white-box state by
        //:   altering each 'xi' in a unique way.  Verify correctness of
        //:   'operator==' and 'operator!=' by asserting that each element in
        //:   '{ x, x1, x2, ..., xN }' is equivalent to every other element.
        //:
        //: 3 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //
        // Testing:
        //   bool operator==(const baesu_StackTrace& lhs, rhs);
        //   bool operator!=(const baesu_StackTrace& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;


        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "A",      "B",
                "AA",     "AB",     "BB",     "BA",
                "AAA",    "BAA",    "ABA",    "AAB",
                "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
                "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
                "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
                "AAAAAAA",          "BAAAAAA",          "AAAAABA",
                "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
                "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
                "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
            0}; // Null string required as last element.

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)strlen(U_SPEC);

                Obj mU(&testAllocator); gg(&mU, U_SPEC); const Obj& U = mU;
                LOOP_ASSERT(ti, curLen == U.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Obj mV(&testAllocator); gg(&mV, V_SPEC); const Obj& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const int isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "AB",     "ABC",    "ABCD",   "ABCDE",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int u = 0; u < NUM_EXTEND; ++u) {
                    const int U_N = EXTEND[u];
                    Obj mU(&testAllocator);  stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;       gg(&mU, SPEC);

                    if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, U_N, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, U_N, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (X != U));

                    for (int v = 0; v < NUM_EXTEND; ++v) {
                        const int V_N = EXTEND[v];
                        Obj mV(&testAllocator);  stretchRemoveAll(&mV, V_N);
                        const Obj& V = mV;       gg(&mV, SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || veryVerbose && firstFew > 0) {
                            cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }

                        // compare every variation with every other one

                        LOOP3_ASSERT(SPEC, U_N, V_N, 1 == (U == V));
                        LOOP3_ASSERT(SPEC, U_N, V_N, 0 == (U != V));
                    }
                }
            }
        }

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

      } break;
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
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5, 7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
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
        //   operator<<(ostream& s, const baetzo_LocalTimeDescriptor& d);
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

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        Frame mFA(&ta);      const Frame& FA = mFA;
        mFA.setAddress((void *) 0x12ab);
        mFA.setLibraryFileName("/lib/libc.so");
        mFA.setLineNumber(5);
        mFA.setOffsetFromSymbol(116);
        mFA.setSourceFileName("/a/b/c/sourceFile.cpp");
        mFA.setMangledSymbolName("_woof_1a");
        mFA.setSymbolName("woof");

        Frame mFB(&ta);      const Frame& FB = mFB;
        mFB.setAddress((void *) 0x34cd);
        mFB.setLibraryFileName("/lib/libd.a");
        mFB.setLineNumber(15);
        mFB.setOffsetFromSymbol(228);
        mFB.setSourceFileName("/a/b/c/secondSourceFile.cpp");
        mFB.setMangledSymbolName("_arf_1a");
        mFB.setSymbolName("arf");

        Frame mFD(&ta);      const Frame& FD = mFD;

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_ggStr;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  ggStr EXP
        //---- - ---  ----- ---

        { L_,  0,  0, "a",  "["                                             NL
                            "["                                             NL
                            "address = 0x12ab"                              NL
                            "library file name = \"/lib/libc.so\""          NL
                            "line number = 5"                               NL
                            "mangled symbol name = \"_woof_1a\""            NL
                            "offset from symbol = 116"                      NL
                            "source file name = \"/a/b/c/sourceFile.cpp\""  NL
                            "symbol name = \"woof\""                        NL
                            "]"                                             NL
                            "]"                                             NL
                                                                            },

        { L_,  0,  0, "b",  "["                                             NL
                            "["                                             NL
                            "address = 0x34cd"                              NL
                            "library file name = \"/lib/libd.a\""           NL
                            "line number = 15"                              NL
                            "mangled symbol name = \"_arf_1a\""             NL
                            "offset from symbol = 228"                      NL
                            "source file name = \"/a/b/c/"
                                                  "secondSourceFile.cpp\""  NL
                            "symbol name = \"arf\""                         NL
                            "]"                                             NL
                            "]"                                             NL
                                                                            },

        { L_,  0,  0, "d",  "["                                             NL
                            "["                                             NL
                            "address = NULL"                                NL
                            "library file name = \"\""                      NL
                            "line number = -1"                              NL
                            "mangled symbol name = \"\""                    NL
#ifdef BSLS_PLATFORM__CPU_32_BIT
                            "offset from symbol = 4294967295"               NL
#else
                            "offset from symbol = 18446744073709551615"     NL
#endif
                            "source file name = \"\""                       NL
                            "symbol name = \"\""                            NL
                            "]"                                             NL
                            "]"                                             NL
                                                                            },

        { L_,  0,  0, "ab", "["                                             NL
                            "["                                             NL
                            "address = 0x12ab"                              NL
                            "library file name = \"/lib/libc.so\""          NL
                            "line number = 5"                               NL
                            "mangled symbol name = \"_woof_1a\""            NL
                            "offset from symbol = 116"                      NL
                            "source file name = \"/a/b/c/sourceFile.cpp\""  NL
                            "symbol name = \"woof\""                        NL
                            "]"                                             NL
                            "["                                             NL
                            "address = 0x34cd"                              NL
                            "library file name = \"/lib/libd.a\""           NL
                            "line number = 15"                              NL
                            "mangled symbol name = \"_arf_1a\""             NL
                            "offset from symbol = 228"                      NL
                            "source file name = \"/a/b/c/"
                                                  "secondSourceFile.cpp\""  NL
                            "symbol name = \"arf\""                         NL
                            "]"                                             NL
                            "]"                                             NL
                                                                            },

        { L_,  0,  2, "a",  "["                                             NL
                            "  ["                                           NL
                            "    address = 0x12ab"                          NL
                            "    library file name = \"/lib/libc.so\""      NL
                            "    line number = 5"                           NL
                            "    mangled symbol name = \"_woof_1a\""        NL
                            "    offset from symbol = 116"                  NL
                            "    source file name = "
                                               "\"/a/b/c/sourceFile.cpp\""  NL
                            "    symbol name = \"woof\""                    NL
                            "  ]"                                           NL
                            "]"                                             NL
                                                                            },

        { L_,  0,  2, "ab", "["                                             NL
                            "  ["                                           NL
                            "    address = 0x12ab"                          NL
                            "    library file name = \"/lib/libc.so\""      NL
                            "    line number = 5"                           NL
                            "    mangled symbol name = \"_woof_1a\""        NL
                            "    offset from symbol = 116"                  NL
                            "    source file name = "
                                               "\"/a/b/c/sourceFile.cpp\""  NL
                            "    symbol name = \"woof\""                    NL
                            "  ]"                                           NL
                            "  ["                                           NL
                            "    address = 0x34cd"                          NL
                            "    library file name = \"/lib/libd.a\""       NL
                            "    line number = 15"                          NL
                            "    mangled symbol name = \"_arf_1a\""         NL
                            "    offset from symbol = 228"                  NL
                            "    source file name = \"/a/b/c/"
                                                  "secondSourceFile.cpp\""  NL
                            "    symbol name = \"arf\""                     NL
                            "  ]"                                           NL
                            "]"                                             NL
                                                                            },

        { L_,  0, -1, "a",  "["                                             SP
                            "["                                             SP
                            "address = 0x12ab"                              SP
                            "library file name = \"/lib/libc.so\""          SP
                            "line number = 5"                               SP
                            "mangled symbol name = \"_woof_1a\""            SP
                            "offset from symbol = 116"                      SP
                            "source file name = \"/a/b/c/sourceFile.cpp\""  SP
                            "symbol name = \"woof\""                        SP
                            "]"                                             SP
                            "]"
                                                                            },

        { L_, -9, -9, "a",  "["                                             SP
                            "["                                             SP
                            "address = 0x12ab"                              SP
                            "library file name = \"/lib/libc.so\""          SP
                            "line number = 5"                               SP
                            "mangled symbol name = \"_woof_1a\""            SP
                            "offset from symbol = 116"                      SP
                            "source file name = \"/a/b/c/sourceFile.cpp\""  SP
                            "symbol name = \"woof\""                        SP
                            "]"                                             SP
                            "]"
                                                                            },

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
                const char       *GGSTR  = DATA[ti].d_ggStr;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P(GGSTR) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;         const Obj& X = mX;

                mX.resize(bsl::strlen(GGSTR));
                for (int xi = 0; xi < X.length(); ++xi) {
                    char c = GGSTR[xi];
                    switch (c) {
                      case 'a': {
                        mX[xi] = FA;
                      }  break;
                      case 'b': {
                        mX[xi] = FB;
                      }  break;
                      case 'd': {
                        mX[xi] = FD;
                      }  break;
                      default: {
                        LOOP3_ASSERT(LINE, GGSTR, c,
                                            0 && "unrecognized char in GGSTR");
                        continue;
                      }
                    }
                }

                ostringstream os(&ta);

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

                // Verify output is formatted as expected.  Note that the
                // 'str()' method of 'os' will return a string by value, which
                // will use the default allocator.

                bslma_DefaultAllocatorGuard tmpGuard(&ta);

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions:
        //: o 'length'
        //: o 'operator[]'
        // Also, we want to ensure that various internal state representations
        // for a given value produce identical results.
        //
        // Plan:
        //: 1 Specify a set 'S' of representative object values ordered by
        //:   increasing length.
        //: 2 For each value 'w' in 'S', initialize a newly constructed object
        //:   'x' with 'w' using 'gg' and verify that each basic accessor
        //:   returns the expected result.
        //: 3 Reinitialize and repeat the same test on an existing object 'y'
        //:   after perturbing 'y' so as to achieve an internal state
        //:   representation of 'w' that is potentially different from that of
        //:   'x'.
        //
        // Testing:
        //   int length() const;
        //   const double& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'length' & 'operator[]'" << endl;
        {
            const int SZ = 10;
            const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { }                     },
                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "AB",           2,      { VA, VB }              },
                { L_,   "BC",           2,      { VB, VC }              },
                { L_,   "BCA",          3,      { VB, VC, VA }          },
                { L_,   "CAB",          3,      { VC, VA, VB }          },
                { L_,   "CDAB",         4,      { VC, VD, VA, VB }      },
                { L_,   "DABC",         4,      { VD, VA, VB, VC }      },
                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "EDCBA",        5,      { VE, VD, VC, VB, VA }  },
                { L_,   "ABCDEAB",      7,      { VA, VB, VC, VD, VE,
                                                  VA, VB }              },
                { L_,   "BACDEABC",     8,      { VB, VA, VC, VD, VE,
                                                  VA, VB, VC }          },
                { L_,   "CBADEABCD",    9,      { VC, VB, VA, VD, VE,
                                                  VA, VB, VC, VD }      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50; stretch(&mY, EXTEND); ASSERT(mY.length());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_lineNum;
                const char *const    SPEC   = DATA[ti].d_spec_p;
                const int            LENGTH = DATA[ti].d_length;
                const Element *const e      = DATA[ti].d_elements;
                const int            curLen = LENGTH;

                Obj mX(&testAllocator);

                const Obj& X = gg(&mX, SPEC);   // canonical organization
                mY.removeAll();
                const Obj& Y = gg(&mY, SPEC);   // has extended capacity

                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) { cout << "\t\t\t"; P(X);
                                   cout << "\t\t\t"; P(Y); }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                int i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }
                for (; i < SZ; ++i) {
                    LOOP2_ASSERT(LINE, i, Element() == e[i]);
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTION 'gg'
        //   Test the primitive generator function, 'gg', and other helper
        //   functions using the (previously tested) primary manipulators.
        //
        // Concerns:
        //: 1 We have to verify:
        //:   1 that valid generator syntax produces expected results, and
        //:   2 that invalid syntax is detected and reported.
        //:
        //: 2 We want also to make trustworthy some additional test helper
        //:   functionality that we will use within the first 10 test cases:
        //:   o 'stretch': Tested separately to observe stretch occurs.
        //:   o 'stretchRemoveAll': Deliberately implemented using 'stretch'.
        //:
        //: 3 Finally, we want to make sure that we can rationalize the
        //:   internal memory management with respect to the primary
        //:   manipulators (i.e., precisely when new blocks are allocated and
        //:   deallocated).
        //:   o TBD: The we don't have access to the allocation strategy of
        //:     the underlying 'bsl::vector<Obj>'.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'gg' to set the state of a newly created object.  Verify that
        //:   'gg' returns a valid reference to the modified argument object
        //:   and, using basic accessors, that the value of the object is as
        //:   expected.  Repeat the test for a longer 'spec' generated by
        //:   prepending a string ending in a '~' character (denoting
        //:   'removeAll').  Note that we are testing the parser only; the
        //:   primary manipulators are already assumed to work.
        //:
        //: 2 To verify that the stretching functions work as expected (and to
        //:   cross-check that internal memory is being managed as intended),
        //:   create a depth-ordered enumeration of initial values and sizes by
        //:   which to extend the initial value.  Record as expected values the
        //:   total number of memory blocks allocated during the first and
        //:   second modifications of each object.  For each test vector,
        //:   construct two identical objects 'X' and 'Y' and bring each to the
        //:   initial state.  Assert that the memory allocation for the two
        //:   operations are identical and consistent with the first expected
        //:   value.  Next apply the 'stretch' and 'stretchRemoveAll' functions
        //:   to 'X' and 'Y' (respectively) and again compare the memory
        //:   allocation characteristics for the two functions.  Note that we
        //:   will track the *total* number of *blocks* allocated as well as
        //:   the *current* number of *bytes* in use -- this to measure
        //:   different aspects of operation while remaining insensitive to the
        //:   array 'Element' size.
        //
        // Testing:
        //   void stretch(baesu_StackTrace *object, int size);
        //   void stretchRemoveAll(baesu_StackTrace *o, int size);
        //   int ggg(baesu_StackTrace *o, const char *s, int vF = 1);
        //   baesu_StackTrace& gg(baesu_StackTrace *o, const char *s);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTION 'gg'" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            const struct {  // non-'static', else default allocator leaks
                            // and asserts on destruction

                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_length;        // expected length
                Element     d_elements[SZ];  // expected value
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { }                     },

                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "~",            0,      { }                     },

                { L_,   "CD",           2,      { VC, VD }              },
                { L_,   "E~",           0,      { }                     },
                { L_,   "~E",           1,      { VE }                  },
                { L_,   "~~",           0,      { }                     },

                { L_,   "ABC",          3,      { VA, VB, VC }          },
                { L_,   "~BC",          2,      { VB, VC }              },
                { L_,   "A~C",          1,      { VC }                  },
                { L_,   "AB~",          0,      { }                     },
                { L_,   "~~C",          1,      { VC }                  },
                { L_,   "~B~",          0,      { }                     },
                { L_,   "A~~",          0,      { }                     },
                { L_,   "~~~",          0,      { }                     },

                { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                { L_,   "~BCD",         3,      { VB, VC, VD }          },
                { L_,   "A~CD",         2,      { VC, VD }              },
                { L_,   "AB~D",         1,      { VD }                  },
                { L_,   "ABC~",         0,      { }                     },

                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                { L_,   "AB~DE",        2,      { VD, VE }              },
                { L_,   "ABCD~",        0,      { }                     },
                { L_,   "A~C~E",        1,      { VE }                  },
                { L_,   "~B~D~",        0,      { }                     },

                { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_lineNum;
                const char *const    SPEC   = DATA[ti].d_spec_p;
                const int            LENGTH = DATA[ti].d_length;
                const Element *const e      = DATA[ti].d_elements;
                const int            curLen = (int)strlen(SPEC);


                Obj mX(&testAllocator);
                const Obj& X = gg(&mX, SPEC);   // original spec

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&testAllocator);
                const Obj& Y = gg(&mY, buf);    // extended spec

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                for (int i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                //line  spec            index
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "F",             0,     },

                { L_,   "AE",           -1,     }, // control
                { L_,   "aE",            0,     },
                { L_,   "Ae",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "ABC",          -1,     }, // control
                { L_,   " BC",           0,     },
                { L_,   "A C",           1,     },
                { L_,   "AB ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "ABCDE",        -1,     }, // control
                { L_,   "aBCDE",         0,     },
                { L_,   "ABcDE",         2,     },
                { L_,   "ABCDe",         4,     },
                { L_,   "AbCdE",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("oa-validSpecs",
                                              veryVeryVeryVerbose);

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;
                const int         curLen = (int)strlen(SPEC);

                Obj mX(&testAllocator);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);
            }
        }

        if (verbose) cout <<
            "\nTesting 'stretch' and 'stretchRemoveAll'." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_size;          // amount to grow (also length)
                int         d_firstResize;   // total blocks allocated
                int         d_secondResize;  // total blocks allocated

                // Note: total blocks (first/second Resize) and whether or not
                // 'removeAll' deallocates memory depends on 'Element' type.

            } DATA[] = {
                //line  spec            size    firstResize     secondResize
                //----  -------------   ----    -----------     ------------
                { L_,   "",             0,      0,              0       },

                { L_,   "",             1,      0,              0       },
                { L_,   "A",            0,      0,              0       },

                { L_,   "",             2,      0,              1       },
                { L_,   "A",            1,      0,              1       },
                { L_,   "AB",           0,      1,              0       },

                { L_,   "",             3,      0,              2       },
                { L_,   "A",            2,      0,              2       },
                { L_,   "AB",           1,      1,              1       },
                { L_,   "ABC",          0,      2,              0       },

                { L_,   "",             4,      0,              2       },
                { L_,   "A",            3,      0,              2       },
                { L_,   "AB",           2,      1,              1       },
                { L_,   "ABC",          1,      2,              0       },
                { L_,   "ABCD",         0,      2,              0       },

                { L_,   "",             5,      0,              3       },
                { L_,   "A",            4,      0,              3       },
                { L_,   "AB",           3,      1,              2       },
                { L_,   "ABC",          2,      2,              1       },
                { L_,   "ABCD",         1,      2,              1       },
                { L_,   "ABCDE",        0,      3,              0       },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator testAllocator("oa-stretch",
                                              veryVeryVeryVerbose);

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = (int)strlen(SPEC);
                const int curDepth     = curLen + size;

                Obj mX(&testAllocator);  const Obj& X = mX;
                Obj mY(&testAllocator);  const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the 'gg' function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int  bytes1A = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int  bytes2A = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int  bytes3A = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int  bytes12A =  bytes2A -  bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int  bytes23A =  bytes3A -  bytes2A;

                    if (veryVerbose) { P_( bytes12A);  P_(bytes23A);
                                       P_(blocks12A);  P(blocks23A); }

                    LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                    LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

#if TDB
                    {Q(debug-first); P_(firstResize) P(blocks12A);}
                    LOOP_ASSERT(LINE, firstResize == blocks12A);
#endif

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE,  bytes12A ==  bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {

                    int blocks1B = testAllocator.numBlocksTotal();
                    int  bytes1B = testAllocator.numBytesInUse();

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int  bytes2B = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int  bytes3B = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int  bytes12B =  bytes2B -  bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int  bytes23B =  bytes3B -  bytes2B;

                    if (veryVerbose) { P_( bytes12B); P_(bytes23B);
                                       P_(blocks12B); P(blocks23B); }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

#if TBD
                    {Q(debug-second); P_(secondResize); P(blocks12B);}
                    LOOP_ASSERT(LINE, secondResize == blocks12B);
#endif

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE,  bytes12B >=  bytes23B); // Equal for
                                                               // POD; else '>'
                                                               // or '>=.
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS (BOOTSTRAP): The basic concern is that the
        //   default constructor, the destructor, and, under normal conditions
        //   (i.e., no aliasing), the primary manipulators:
        //: o 'append' (black-box)
        //: o 'removeAll' (white-box)
        // operate as expected.
        //
        // Concerns:
        //: 1 The default constructor
        //:   1 creates the correct initial value.
        //:   2 is exception neutral with respect to memory allocation.
        //:   3 has the internal memory management system hooked up properly so
        //:     that *all* internally allocated memory draws from the same
        //:     user-supplied allocator whenever one is specified.
        //:
        //: 2 The destructor properly deallocates all allocated memory to its
        //:   corresponding allocator from any attainable state.
        //:
        //: 3 The 'append' method:
        //:   1 produces the expected value.
        //:   2 increases capacity as needed.
        //:   3 maintains valid internal state.
        //:   4 is exception neutral with respect to memory allocation.
        //:
        //: 4 The 'removeAll' method:
        //:   1 produces the expected value (empty).
        //:   2 properly destroys each contained element value.
        //:   3 maintains valid internal state.
        //:   4 does not allocate memory.
        //
        // Plan:
        //: 1 To address C-1.1, C-1.2, and C-1.3 create an object using the
        //:   default constructor:
        //:   1 with and without passing in an allocator.
        //:   2 in the presence of exceptions during memory allocations using a
        //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:   3 where the object is constructed entirely in static memory
        //:     (using a 'bslma_BufferAllocator') and never destroyed.
        //:
        //: 2 To address C-3.1, C-3.2, and C-3.3, construct a series of
        //:   independent objects, ordered by increasing length.  In each test,
        //:   allow the object to leave scope without further modification, so
        //:   that the destructor asserts internal object invariants
        //:   appropriately.  After the final append operation in each test,
        //:   use the (untested) basic accessors to cross-check the value of
        //:   the object and the 'bslma_TestAllocator' to confirm whether a
        //:   resize has occurred.
        //:
        //: 3 To address C-4.1, C-4.2, and C-4.3, construct a similar test,
        //:   replacing 'append' with 'removeAll'; this time, however, use the
        //:   test allocator to record 'numBlocksInUse' rather than
        //:   'numBlocksTotal'.
        //:
        //: 2 To address C-2, C-3.4, and C-4.4, create a small "area" test that
        //:   exercises the construction and destruction of objects of various
        //:   lengths and capacities in the presence of memory allocation
        //:   exceptions.  Two separate tests will be performed.  Let 'S' be
        //:   the sequence of integers '[0 .. N - 1]':
        //:
        //:   1 For each 'i' in 'S', use the default constructor and 'append'
        //:     to create an instance of length 'i', confirm its value (using
        //:     basic accessors), and let it leave scope.
        //:
        //:   2 For each '(i, j)' in 'S X S', use 'append' to create an
        //:     instance of length 'i', use 'removeAll' to erase its value and
        //:     confirm (with 'length'), use append to set the instance to a
        //:     value of length 'j', verify the value, and allow the instance
        //:     to leave scope.
        //:
        //:   The first test acts as a "control" in that 'removeAll' is not
        //:   called; if only the second test produces an error, we know that
        //:   'removeAll' is to blame.  We will rely on 'bslma_TestAllocator'
        //:   and purify to address concern 2, and on the object invariant
        //:   assertions in the destructor to address C-3.4 and C-4.4.
        //
        // Testing:
        //   baesu_StackTrace(bslma_Allocator *bA = 0);
        //   void append(int item); // bootstrap:  no aliasing
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;



        bslma_TestAllocator testAllocator("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tTesting ctor with no parameters." << endl;
        {
            const Obj X;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\nTesting 'append' with default ctor." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X); }

            mX.append(V0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(1 == X.length());
            ASSERT(V0 == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            mX.append(V0);

            if (veryVerbose) { cout << "\t\t"; P(X); }
            mX.append(V1);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(2 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
        }

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bslma_BufferAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P_(BB); P(X); }
            mX.append(V0);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P_(AA); P(X); }
            ASSERT(BB + 1 == AA); // ADJUST-ED
            ASSERT(B  + 1 == A);  // ADJUST-ED
            ASSERT(1 == X.length());
            ASSERT(V0 == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V1);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(2 == X.length());
            ASSERT(BB + 1 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(V2);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 1 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(3 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P_(BB); P(X); }
            mX.append(V3);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P_(AA); P(X); }
            ASSERT(BB + 1 == AA); // ADJUST-ED
            ASSERT(B  + 1 == A);  // ADJUST-ED
            ASSERT(4 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
            ASSERT(V3 == X[3]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2); mX.append(V3);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P_(BB); P(X); }
            mX.append(V4);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P_(AA); P(X); }
            ASSERT(BB + 3 == AA); // ADJUST-ED
            ASSERT(B  + 1 == A);  // ADJUST-ED
            ASSERT(5 == X.length());
            ASSERT(V0 == X[0]);
            ASSERT(V1 == X[1]);
            ASSERT(V2 == X[2]);
            ASSERT(V3 == X[3]);
            ASSERT(V4 == X[4]);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);   // always
            ASSERT(B - 0 == A); // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0);
            ASSERT(1 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1);
            ASSERT(2 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(V0); mX.append(V1); mX.append(V2);
            ASSERT(3 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each length 'i' up to some modest limit:
            //: 1 create an instance
            //: 2 append '{ V0, V1, V2, V3, V4, V0, ... }'  up to length 'i'
            //: 3 verify initial length and contents
            //: 4 allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(VALUES[k % NUM_VALUES]);
                }

                LOOP_ASSERT(i, i == X.length());                        // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                  // 4.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'removeAll'" << endl;
        {
            // For each pair of lengths '(i, j)' up to some modest limit:
            //: 1 create an instance
            //: 2 append 'V0' values up to a length of 'i'
            //: 3 verify initial length and contents
            //: 4 removeAll contents from instance
            //: 5 verify length is 0
            //: 6 append '{ V0, V1, V2, V3, V4, V0, ... }'  up to length 'j'
            //: 7 verify new length and contents
            //: 8 allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(V0);
                    }

                    LOOP2_ASSERT(i, j, i == X.length());                // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, V0 == X[k]);
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(VALUES[k % NUM_VALUES]);
                    }

                    LOOP2_ASSERT(i, j, j == X.length());                // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
                }
            }
        }

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
        //   Create four instances of the array using both the default and copy
        //   constructors.  Exercise these objects using primary manipulators,
        //   basic accessors, equality operators, and the assignment operator.
        //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
        //   assignment operator [9&10] in situations where the internal data
        //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
        //   with assignment for a non-empty instance [11] and allow the result
        //   to leave scope, enabling the destructor to assert internal object
        //   invariants.  Display object values frequently in verbose mode:
        //
        //: 1 Create an object x1 (default ctor).      { x1: }
        //: 2 Create a second object x2 (copy from x1).{ x1: x2: }
        //: 3 Append an element value A to x1.         { x1:A x2: }
        //: 4 Append the same element value A to x2.   { x1:A x2:A }
        //: 5 Append another element value B to x2.    { x1:A x2:AB }
        //: 6 Remove all elements from x1.             { x1: x2:AB }
        //: 7 Create a third object x3 (default ctor). { x1: x2:AB x3: }
        //: 8 Create a fourth object x4 (copy from x2).{ x1: x2:AB x3: x4:AB }
        //: 9 Assign x2 = x1 (non-empty becomes empty).{ x1: x2: x3: x4:AB }
        //:10 Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        //:11 Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma_TestAllocator testAllocator(veryVeryVeryVerbose);

        Frame A;
        A.setAddress((void *) 0x12ab);
        A.setLibraryFileName("/a/b/c/baesu_stacktrace.t.dbg_exc_mt");
        A.setLineNumber(5);
        A.setOffsetFromSymbol(116);
        A.setSourceFileName("/a/b/c/sourceFile.cpp");
        A.setMangledSymbolName("_woof_1a");
        A.setSymbolName("woof");

        Frame B;
        B.setAddress((void *) 0x34cd);
        B.setLibraryFileName("/lib/libd.a");
        B.setLineNumber(15);
        B.setOffsetFromSymbol(228);
        B.setSourceFileName("/a/b/c/secondSourceFile.cpp");
        B.setMangledSymbolName("_arf_1a");
        B.setSymbolName("arf");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object 'x1' (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta) Check initial state of 'x1'." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
                          "\tb) Try equality operators: 'x1 <op> x1'." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                         "\n 2) Create a second object 'x2' (copy from 'x1';)."
                         "\t\t{ x1: x2: }" << endl;

        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta) Check the initial state of 'x2'." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
                    "\tb) Try equality operators: 'x2 <op> x1', 'x2'." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Append an element value 'A' to 'x1')."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.append(A);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta) Check new state of 'x1'." << endl;
        ASSERT(1 == X1.length());
        ASSERT(A == X1[0]);

        if (verbose) cout <<
                    "\tb) Try equality operators: 'x1 <op> x1', 'x2'." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Append the same element value 'A' to 'x2')."
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.append(A);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(A == X2[0]);

        if (verbose) cout <<
                    "\tb) Try equality operators: 'x2 <op> x1', 'x2'." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Append another element value 'B' to 'x2')."
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.append(B);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta) Check new state of 'x2'." << endl;
        ASSERT(2 == X2.length());
        ASSERT(A == X2[0]);
        ASSERT(B == X2[1]);

        if (verbose) cout <<
                    "\tb) Try equality operators: 'x2 <op> x1', 'x2'." << endl;
        ASSERT((X2 == X1) == 0);          ASSERT((X2 != X1) == 1);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6) Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta) Check new state of 'x1'." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: 'x1 <op> x1', 'x2'." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object 'x3' (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
              "\tb) Try equality operators: 'x3 <op> x1', 'x2', 'x3'." << endl;
        ASSERT((X3 == X1) == 1);          ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                            "\n 8) Create a fourth object 'x4' (copy of 'x2')."
                            "\t\t{ x1: x2:01 x3: x4:01 }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta) Check new state of 'x4'." << endl;

        ASSERT(2 == X4.length());
        ASSERT(A == X4[0]);
        ASSERT(B == X4[1]);

        if (verbose) cout <<
        "\tb) Try equality operators: 'x4 <op> x1', 'x2', 'x3', 'x4'." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);          ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);          ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                          "\n 9) Assign 'x2' = 'x1' (non-empty becomes empty)."
                          "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
        "\tb) Try equality operators: 'x2 <op> x1', 'x2', 'x3', 'x4'." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 1);          ASSERT((X2 != X3) == 0);
        ASSERT((X2 == X4) == 0);          ASSERT((X2 != X4) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                          "\n10) Assign 'x3' = 'x4' (empty becomes non-empty)."
                          "\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta) Check new state of 'x3'." << endl;
        ASSERT(2 == X3.length());
        ASSERT(A == X3[0]);
        ASSERT(B == X3[1]);

        if (verbose) cout <<
        "\tb) Try equality operators: 'x3 <op> x1', 'x2', 'x3', 'x4'." << endl;
        ASSERT((X3 == X1) == 0);          ASSERT((X3 != X1) == 1);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);
        ASSERT((X3 == X4) == 1);          ASSERT((X3 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign 'x4' = 'x4' (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta) Check new state of 'x4'." << endl;
        ASSERT(2 == X4.length());
        ASSERT(A == X4[0]);
        ASSERT(B == X4[1]);

        if (verbose) cout <<
        "\tb) Try equality operators: 'x4 <op> x1', 'x2', 'x3', 'x4'." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);          ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);          ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
// ----------------------------- END-OF-FILE ---------------------------------
