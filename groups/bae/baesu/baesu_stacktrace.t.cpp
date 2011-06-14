// baesu_stacktrace.t.cpp                                             -*-C++-*-
#include <baesu_stacktrace.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
// TEST PLAN
//
//-----------------------------------------------------------------------------

//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
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
// STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

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

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

//=============================================================================
// GLOBAL HELPER #DEFINES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER TYPES, CLASSES, and CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baesu_StackTrace               Obj;
typedef baesu_StackTraceFrame          Frame;

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//=============================================================================

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 4;

    cout << "TEST CASE " << test << endl;

    bslma_TestAllocator ta("ta", veryVeryVeryVerbose);

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    bslma_TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        // First, we set up a test allocator as default allocator.  A
        // 'baesu_StackTrace' object, by default, gets all its memory from an
        // owned instance of 'bdema_HeapBypassAllocator'.  To demonstrate this
        // we start by setting the default allocator to a test allocator so we
        // can verify later that it was unused:

        bslma_TestAllocator da;
        bslma_DefaultAllocatorGuard guard(&da);

        // Next, create a stack trace.  Note that when we don't specify an
        // allocator (recommended), the default allocator is not used --
        // rather, a heap bypass allocator owned by the stack trace object is
        // used.  The heap bypass allocator is recommended because this
        // component is usually used for obtaining debug information, and the
        // possibility of heap corruption can't be ruled out.  The heap bypass
        // allocator obtains its memory directly from virtual memory rather
        // than going through the heap, avoiding potential complications due to
        // heap corruption.

        baesu_StackTrace stackTrace;
        ASSERT(0 == stackTrace.length());

        // Then, we 'resize' the stack-trace object to contain two
        // default-constructed frames, and take references to each of the two
        // new frames.

        stackTrace.resize(2);
        ASSERT(2 == stackTrace.length());
        baesu_StackTraceFrame& frame0 = stackTrace[0];
        baesu_StackTraceFrame& frame1 = stackTrace[1];

        // Next, we set the values of the fields of the two new frames.

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

        // Then, we verify the frames have the values we expect.

        ASSERT((void *) 0x12ab == frame0.address());
        ASSERT("/a/b/c/baesu_stacktrace.t.dbg_exc_mt" ==
                                                     frame0.libraryFileName());
        ASSERT(5 == frame0.lineNumber());
        ASSERT(116 == frame0.offsetFromSymbol());
        ASSERT("/a/b/c/sourceFile.cpp" == frame0.sourceFileName());
        ASSERT("_woof_1a" == frame0.mangledSymbolName());
        ASSERT("woof" == frame0.symbolName());

        ASSERT((void *) 0x34cd == frame1.address());
        ASSERT("/lib/libd.a" == frame1.libraryFileName());
        ASSERT(15 == frame1.lineNumber());
        ASSERT(228 == frame1.offsetFromSymbol());
        ASSERT("/a/b/c/secondSourceFile.cpp" == frame1.sourceFileName());
        ASSERT("_arf_1a" == frame1.mangledSymbolName());
        ASSERT("arf" == frame1.symbolName());

        // Next, we output the stack trace object.

        stackTrace.print(cout, 1, 2);

        // Finally, we observe the default allocator was never used.

        ASSERT(0 == da.numAllocations());
      }  break;
      case 3: {
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
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // It's very important this component not use the default allocator.

    ASSERT(0 == defaultAllocator.numAllocations());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
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
