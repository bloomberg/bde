// bslim_printer.t.cpp                                                -*-C++-*-
#include <bslim_printer.h>

#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <cstdlib>
#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bslim;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a mechanism class having accessor
// methods and method templates, each of which is tested over a number of test
// cases:
//: 1 Test initial indentation for different values of 'level' and
//:   'spacesPerLevel', and the output when a 'name' is supplied.
//: 2 Test the output of each method and method template (and its
//:   specializations, if any), for positive and negative values of
//:   'spacesPerLevel'.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] Printer(bsl::ostream *stream, int level, int spacesPerLevel);
//
// ACCESSORS
// [ 2] absLevel() const;
// [ 5] end() const;
// [10] template<class TYPE>
//         void print(const TYPE& data, const char *name) const;
// [16] template <class TYPE, class PRINT_FUNCTOR>
//         void printForeign(const TYPE&           data,
//                           const PRINT_FUNCTOR&  printFunctionObject,
//                           const char           *name) const;
// [15] void printHexAddr(const void *address, const char *name) const;
// [13] template <class TYPE>
//      void printOrNull(const TYPE& address, const char *name) const;
// [ 2] spacesPerLevel() const;
// [ 4] start() const;
// [ 2] suppressInitialIndentFlag() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [20] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_ cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef Printer Obj;

//=============================================================================
//                        GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct HasPrint {
    // Class having a 'print' method

    // DATA
    int d_data;
    mutable int d_level;
    mutable int d_spacesPerLevel;

    // CREATORS
    HasPrint(int data) : d_data(data) {}

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    {
        d_level = level;
        d_spacesPerLevel = spacesPerLevel;
        Obj printer(&stream, level, spacesPerLevel);
        printer.print(d_data, "HasPrint:data");
        return stream;
    }
};

class NoPrint {
    // Class without a 'print' method

    // DATA
    int d_data;

  public:
    // CREATORS
    NoPrint(int data) : d_data(data) {}

    // ACCESSORS
    int get() const { return d_data; }
};

struct NoPrintUtil {
    // Utility class for class 'NoPrint'

    static bsl::ostream& print(bsl::ostream&  stream,
                               const NoPrint& obj,
                               int            level          = 0,
                               int            spacesPerLevel = 4)
    {
        Obj printer(&stream, level, spacesPerLevel);
        printer.print(obj.get(), 0);
        return stream;
    }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// EXAMPLE 1
class bdet_Datetime {
  public:
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    { return stream; }
};

class RecordAttributes {
    // This class provides a container for a fixed set of attributes.

    // DATA
    bdet_Datetime    d_timestamp;    // creation date and time
    int              d_processID;    // process id of creator

  public:
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
    // ...
};

bsl::ostream& RecordAttributes::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_timestamp, "timestamp");
    printer.print(d_processID, "process ID");
    printer.end();

    return stream;
}

// EXAMPLE 2
//
class BlockList {
    // This class implements a low-level memory manager that allocates and
    // manages a sequence of memory blocks.

    // TYPES
    struct Block {
        // This 'struct' overlays the beginning of each managed block of
        // allocated memory, implementing a doubly-linked list of managed
        // blocks, and thereby enabling constant-time deletions from, as
        // well as additions to, the list of blocks.

        Block                               *d_next_p;       // next
                                                             // pointer

        Block                              **d_addrPrevNext; // enable
                                                             // delete

        bsls_AlignmentUtil::MaxAlignedType   d_memory;       // force
                                                             // alignment
    };

    // DATA
    Block           *d_head_p;      // address of first block of memory
                                    // (or 0)

    bslma_Allocator *d_allocator_p; // memory allocator; held, but not
                                    // owned

  public:
    // ACCESSORS
    // ...
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

bsl::ostream& BlockList::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    Block *it = d_head_p;
    while (it)
    {
        printer.printHexAddr(it, 0);
        it = it->d_next_p;
    }

    printer.end();

    return stream;
}

//  EXAMPLE 3
//
class ThirdPartyClass {
    // ...
  public:
    // ACCESSORS
    int getAttribute1() const { return 0; }
    int getAttribute2() const { return 0; }
};

struct ThirdPartyClassUtil {
    static bsl::ostream& print(bsl::ostream&          stream,
                               const ThirdPartyClass& obj,
                               int                    level          = 0,
                               int                    spacesPerLevel = 4);
};

bsl::ostream&
ThirdPartyClassUtil::print(bsl::ostream&          stream,
                           const ThirdPartyClass& obj,
                           int                    level,
                           int                    spacesPerLevel)
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(obj.getAttribute1(), "Attribute1");
    printer.print(obj.getAttribute2(), "Attribute2");
    printer.end();

    return stream;
}

class MyClass {
    // ...
    // DATA
    ThirdPartyClass d_attributeA;
    int             d_attributeB;

  public:
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
    // ...
};

bsl::ostream& MyClass::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    if (stream.bad()) {
         return stream;                                               // RETURN
    }

    Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printForeign(d_attributeA,
                         &ThirdPartyClassUtil::print,
                         "AttributeA");
    printer.print(d_attributeB, "AttributeB");
    printer.end();

    return stream;
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns: That it compiles
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Usage Example 1" << endl
                                  << "===============" << endl;

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTION NEUTRALITY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING EXCEPTION NEUTRALITY" << endl
                                  << "============================" << endl;

        void *data = reinterpret_cast<void *>(0xdeadbeef);
        ostringstream out;

        bslma_TestAllocator sa("supplied", veryVeryVeryVerbose);
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'printForeign'
        //
        // Concerns: That 'printForeign'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //: 3 prints the expected output when 'name' is not supplied.
        //: 4 calls the foreign function with the values of '-(absLevel + 1)'
        //    and 'spacesPerLevel'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printForeign' when called with a
        //:   'NoPrint' object, and a 'name'.  For each set of values in the
        //:   table, ensure that the actual output of 'printOrNull' is the same
        //:   as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printForeign' when called with a
        //:   'NoPrint' object, but 'name' is not supplied.  For each set of
        //:   values in the table, ensure that the actual output of
        //:   'printOrNull' is the same as the expected output.
        //
        // Testing:
        //   template <class TYPE, class PRINT_FUNCTOR>
        //   void printForeign(const TYPE&           data,
        //                     const PRINT_FUNCTOR&  printFunctionObject,
        //                     const char           *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printForeign'" << endl
                                  << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, "data = %d\n"               },
                { L_,    0,    2, "  data =     %d\n"         },
                { L_,    2,    0, "data = %d\n"               },
                { L_,    2,    2, "      data =         %d\n" },
                { L_,    2,   -2, " data =  %d"               },
                { L_,   -2,    2, "      data =         %d\n" },
                { L_,   -2,   -2, " data =  %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 433001; NoPrint data(tData);
                Obj p(&out, LEVEL, SPL);
                p.printForeign(data, &NoPrintUtil::print, "data");

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "              %d\n" },
                { L_,    2,   -2, "  %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 433001; NoPrint data(tData);
                Obj p(&out, LEVEL, SPL);
                p.printForeign(data, &NoPrintUtil::print, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'printHexAddr'
        //
        // Concerns: That 'printHexAddr'
        //: 1 prints the input address in hexadecimal format when 'name' is not
        //:   supplied.
        //: 2 prints the string "NULL" when a null pointer is passed to it.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when 'name' is not
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printHexAddr' is the same as the expected
        //:   output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when a null pointer
        //:   is passed to it.  For each set of values in the table, ensure
        //:   that the actual output of 'printHexAddr' is the same as the
        //:   expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printHexAddr(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printHexAddr'" << endl
                                  << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    3, "         %s\n" },
                { L_,    2,   -3, " %s"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int *data = reinterpret_cast<int *>(0xdeadbeef);
                Obj p(&out, LEVEL, SPL); p.printHexAddr(data, 0);

                ostringstream ptr;
                ptr << hex << showbase
                    << reinterpret_cast<bsls_Types::UintPtr>(data);
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            ptr.str().c_str());
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      NULL\n" },
                { L_,    2,   -2, " NULL"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                void *data = 0;
                Obj p(&out, LEVEL, SPL); p.printHexAddr(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'printHexAddr' (indentation and name)
        //
        // Concerns: That 'printHexAddr'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printHexAddr' when a 'name' is
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printHexAddr' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printHexAddr(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printHexAddr' (indentation and name)" << endl
                 << "=============================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    0,    0, "data = %s\n"          },
            { L_,    0,    2, "  data = %s\n"        },
            { L_,    2,    0, "data = %s\n"          },
            { L_,    2,    3, "         data = %s\n" },
            { L_,    2,   -3, " data = %s"           },
            { L_,   -2,    3, "         data = %s\n" },
            { L_,   -2,   -3, " data = %s"           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int *data = reinterpret_cast<int *>(0xdeadbeef);
            Obj p(&out, LEVEL, SPL); p.printHexAddr(data, "data");

            ostringstream ptr;
            ptr << hex << showbase
                << reinterpret_cast<bsls_Types::UintPtr>(data);
            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), ptr.str().c_str());
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'printOrNull' (null pointers)
        //
        // Concerns:
        //: 1 That 'printOrNull' prints the string "NULL" when a null pointer
        //:   is passed to it.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   null pointer.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' when called with a null
        //:   pointer of type 'void *', 'const void *', 'volatile void *',
        //:   'volatile const void *', 'const char *', 'int *' and 'HasPrint *'
        //:   is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'printOrNull' (null pointers)" << endl
                          << "=====================================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      NULL\n" },
                { L_,    2,   -2, " NULL"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream vOut, cvOut, vvOut, vcvOut, cOut, iOut, uOut;

                void *data = 0;
                Obj pV(&vOut, LEVEL, SPL); pV.printOrNull(data, 0);
                Obj pCV(&cvOut, LEVEL, SPL);
                pCV.printOrNull((const void *)data, 0);
                Obj pVV(&vvOut, LEVEL, SPL);
                pVV.printOrNull((volatile void *)data, 0);
                Obj pVCV(&vcvOut, LEVEL, SPL);
                pVCV.printOrNull((volatile const void *)data, 0);
                Obj pC(&cOut, LEVEL, SPL);
                pC.printOrNull((const char *)data, 0);
                Obj pI(&iOut, LEVEL, SPL); pI.printOrNull((int *)data, 0);
                Obj pU(&uOut, LEVEL, SPL); pU.printOrNull((HasPrint *)data, 0);

                const bsl::string& EXPECTED = DATA[i].d_expected;

                // void *
                {
                    const bsl::string& ACTUAL = vOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile void *
                {
                    const bsl::string& ACTUAL = vvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile const void *
                {
                    const bsl::string& ACTUAL = vcvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const char *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // int *
                {
                    const bsl::string& ACTUAL = iOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // HasPrint *
                {
                    const bsl::string& ACTUAL = uOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: 'printOrNull' (non-null pointer types)
        //
        // Concerns: That 'printOrNull'
        //: 1 prints the address held by the pointer in hexadecimal format, but
        //:   does not attempt to dereference the pointer if the pointer type
        //:   is 'void *', 'const void *', 'volatile void *' or
        //:   'volatile const void *'.
        //: 2 dereferences the pointer and prints a character if the
        //:   pointer type is 'const char *'.
        //: 3 dereferences the pointer and prints the held value if the
        //:   pointer points to a fundamental type.
        //: 4 calls the 'print' method of the held object with the values of
        //:   '-(absLevel + 1)' and 'spacesPerLevel' if the pointer points to a
        //:   user-defined type.
        //: 5 prints a newline character after printing the data if
        //:   'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'void *', 'const void *', 'volatile void *' or
        //:   'volatile const void *'.  For each set of values in the table,
        //:   ensure that the actual output of 'printOrNull' is the same as the
        //:   expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'const char *'.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' is the same as the expected
        //:   output.
        //: 3 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'int *'.  For each set of values in the table, ensure that the
        //:   actual output of 'printOrNull' is the same as the expected
        //:   output.
        //: 4 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when called with a
        //:   'HasPrint *'.  For each set of values in the table, ensure that
        //:   the actual output of 'printOrNull' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printOrNull' (non-null pointer types)" << endl
                 << "==============================================" << endl;

        {
            if (verbose) cout << "void *" << endl
                              << "------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %s\n" },
                { L_,    2,   -2, " %s"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out, cOut, vOut, vcOut;
                void *data = reinterpret_cast<void *> (0xdeadbeef);
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);
                Obj pC(&cOut, LEVEL, SPL);
                pC.printOrNull((const void *)data, 0);
                Obj pV(&vOut, LEVEL, SPL);
                pV.printOrNull((volatile void *)data, 0);
                Obj pVC(&vcOut, LEVEL, SPL);
                pVC.printOrNull((volatile const void *)data, 0);

                ostringstream ptr;
                ptr << hex << showbase
                    << reinterpret_cast<bsls_Types::UintPtr>(data);
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            ptr.str().c_str());
                const bsl::string EXPECTED(buf);

                // void *
                {
                    const bsl::string& ACTUAL = out.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile void *
                {
                    const bsl::string& ACTUAL = vOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile const void *
                {
                    const bsl::string& ACTUAL = vcOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
        {
            if (verbose) cout << "const char *" << endl
                              << "------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %c\n" },
                { L_,    2,   -2, " %c"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                const char *data = "testing char *";
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "int *" << endl
                              << "-----" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %d\n" },
                { L_,    2,   -2, " %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int testData = 88932; int *data = &testData;
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "HasPrint *" << endl
                              << "----------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "              HasPrint:data = %d\n" },
                { L_,    2,   -2, "  HasPrint:data = %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 73326; HasPrint tClass(tData);
                HasPrint *data = &tClass;
                Obj p(&out, LEVEL, SPL); p.printOrNull(data, 0);

                int LEVEL_EXP = -(p.absLevel() + 1);
                int LEVEL_ACT = data->d_level;
                int SPL_EXP = SPL; int SPL_ACT = data->d_spacesPerLevel;

                LOOP2_ASSERT(LEVEL_EXP, LEVEL_ACT, LEVEL_EXP == LEVEL_ACT);
                LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'printOrNull' (indentation and name)
        //
        // Concerns: That 'printOrNull'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'printOrNull' when a 'name' is
        //:   supplied.  For each set of values in the table, ensure that the
        //:   actual output of 'printOrNull' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void printOrNull(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'printOrNull' (indentation and name)" << endl
                 << "============================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    0,    0, "data = %d\n"          },
            { L_,    0,    2, "  data = %d\n"        },
            { L_,    2,    0, "data = %d\n"          },
            { L_,    2,    3, "         data = %d\n" },
            { L_,    2,   -3, " data = %d"           },
            { L_,   -2,    3, "         data = %d\n" },
            { L_,   -2,   -3, " data = %d"           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int tData = 448992; int *data = &tData;
            Obj p(&out, LEVEL, SPL); p.printOrNull(data, "data");

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), *data);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'print' (null pointers)
        //
        // Concerns:
        //: 1 That 'print' prints the string "NULL" when a null pointer is
        //:   passed to it.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a null
        //:   pointer.  For each set of values in the table, ensure that the
        //:   actual output of 'print' when called with a null pointer of type
        //:   'void *', 'const void *', 'volatile void *',
        //:   'volatile const void *', 'const char *', 'int *' and 'HasPrint *'
        //:   is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "\nTESTING 'print' (null pointers)"
                          << "\n===============================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      NULL\n" },
                { L_,    2,   -2, " NULL"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream vOut, cvOut, vvOut, vcvOut, cOut, iOut, uOut;

                void *data = 0;
                Obj pV(&vOut, LEVEL, SPL); pV.print(data, 0);
                Obj pCV(&cvOut, LEVEL, SPL); pCV.print((const void *)data, 0);
                Obj pVV(&vvOut, LEVEL, SPL);
                pVV.print((volatile void *)data, 0);
                Obj pVCV(&vcvOut, LEVEL, SPL);
                pVCV.print((volatile const void *)data, 0);
                Obj pC(&cOut, LEVEL, SPL); pC.print((const char *)data, 0);
                Obj pI(&iOut, LEVEL, SPL); pI.print((int *)data, 0);
                Obj pU(&uOut, LEVEL, SPL); pU.print((HasPrint *)data, 0);

                const bsl::string& EXPECTED = DATA[i].d_expected;

                // void *
                {
                    const bsl::string& ACTUAL = vOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile void *
                {
                    const bsl::string& ACTUAL = vvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile const void *
                {
                    const bsl::string& ACTUAL = vcvOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const char *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // int *
                {
                    const bsl::string& ACTUAL = iOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // HasPrint *
                {
                    const bsl::string& ACTUAL = uOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'print' (non-null pointer types)
        //
        // Concerns: That 'print'
        //: 1 prints the address held by the pointer in hexadecimal format, but
        //:   does not attempt to dereference the pointer if the pointer type
        //:   is 'void *', 'const void *', 'volatile void *' or
        //:   'volatile const void *'.
        //: 2 prints the input as a null-terminated C-style string if the
        //:   pointer is of type 'const char *'.
        //: 3 prints the address held by the pointer type, and then
        //:   dereferences the pointer and prints the held value, if the
        //:   pointer points to a fundamental type.
        //: 4 prints the address held by the pointer type, and then calls the
        //:   'print' method of the held object with the values of
        //:   '-(absLevel + 1)' and 'spacesPerLevel', if the pointer points to
        //:   a user-defined type.
        //: 5 prints a newline character after printing the data if
        //:   'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'void *', 'const void *', 'volatile void *' or
        //:   'volatile const void *'.  For each set of values in the table,
        //:   ensure that the actual output of 'print' is the same as the
        //:   expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'const char *'.  For each set of values in the table, ensure that
        //:   the actual output of 'print' is the same as the expected output.
        //: 3 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'int *'.  For each set of values in the table, ensure that
        //:   the actual output of 'print' is the same as the expected output.
        //: 4 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'HasPrint *'.  For each set of values in the table, ensure that
        //:   the actual output of 'print' is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'print' (non-null pointer types)" << endl
                          << "========================================"
                          << endl;

        {
            if (verbose) cout << "void *" << endl
                              << "------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %s\n" },
                { L_,    2,   -2, " %s"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out, cOut, vOut, vcOut;
                void *data = reinterpret_cast<void *> (0xdeadbeef);
                Obj p(&out, LEVEL, SPL); p.print(data, 0);
                Obj pC(&cOut, LEVEL, SPL); pC.print((const void *)data, 0);
                Obj pV(&vOut, LEVEL, SPL); pV.print((volatile void *)data, 0);
                Obj pVC(&vcOut, LEVEL, SPL);
                pVC.print((volatile const void *)data, 0);

                stringstream exp;
                exp << hex << showbase
                    << reinterpret_cast<bsls_Types::UintPtr>(data);
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            exp.str().c_str());
                const bsl::string EXPECTED(buf);

                // void *
                {
                    if (veryVerbose) cout << "\nvoid *\n";
                    const bsl::string& ACTUAL = out.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    if (veryVerbose) cout << "\nconst void *\n";
                    const bsl::string& ACTUAL = cOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile void *
                {
                    if (veryVerbose) cout << "\nvolatile void *\n";
                    const bsl::string& ACTUAL = vOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // volatile const void *
                {
                    if (veryVerbose) cout << "\nvolatile const void *\n";
                    const bsl::string& ACTUAL = vcOut.str();
                    if (veryVeryVerbose) {
                        cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                             << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                    }
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
            }
        }
        {
            if (verbose) cout << "const char *" << endl
                              << "------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      \"%s\"\n" },
                { L_,    2,   -2, " \"%s\""        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                const char *data = "testing char *";
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "int *" << endl
                              << "-----" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %s %d\n" },
                { L_,    2,   -2, " %s %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int testData = 88932; int *data = &testData;
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                ostringstream ptr;
                ptr << hex << showbase << data;
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                     ptr.str().c_str(), *data);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "HasPrint *" << endl
                              << "----------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %s         HasPrint:data = %d\n" },
                { L_,    2,   -2, " %s  HasPrint:data = %d"               },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int tData = 73326;
                HasPrint tClass(tData); HasPrint *data = &tClass;
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                int LEVEL_EXP = -(p.absLevel() + 1);
                int LEVEL_ACT = data->d_level;
                int SPL_EXP = SPL; int SPL_ACT = data->d_spacesPerLevel;

                LOOP2_ASSERT(LEVEL_EXP, LEVEL_ACT, LEVEL_EXP == LEVEL_ACT);
                LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);

                ostringstream ptr;
                ptr << hex << showbase << data;
                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                     ptr.str().c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'print' (user-defined types)
        //
        // Concerns: That 'print'
        //: 1 calls the 'print' method of the user-defined type with the values
        //:   of '-(absLevel + 1)' and 'spacesPerLevel'.
        //: 2 prints a newline character after printing the user-defined type
        //:   if 'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'HasPrint' object.  For each set of values in the table, ensure
        //:   that the actual output of 'print' is the same as the expected
        //:   output.
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'print' (user-defined types)" << endl
                          << "====================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    2,    2, "              HasPrint:data = %d\n" },
            { L_,    2,   -2, "  HasPrint:data = %d"               },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int tData = 54321; HasPrint data(tData);
            Obj p(&out, LEVEL, SPL); p.print(data, 0);

            int LEVEL_EXP = -(p.absLevel() + 1); int LEVEL_ACT = data.d_level;
            int SPL_EXP = SPL; int SPL_ACT = data.d_spacesPerLevel;

            LOOP2_ASSERT(LEVEL_EXP, LEVEL_ACT, LEVEL_EXP == LEVEL_ACT);
            LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'print' (fundamental types)
        //
        // Concerns: That 'print'
        //: 1 prints fundamental types correctly.
        //: 2 prints a newline character after printing the fundamental type if
        //:   'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a 'char'.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'print' is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "\nTESTING 'print' (fundamental types)"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    2,    2, "      %c\n" },
            { L_,    2,   -2, " %c"        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            char data = 'z';
            Obj p(&out, LEVEL, SPL); p.print(data, 0);

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'print' (indentation and name)
        //
        // Concerns: That 'print'
        //: 1 prints the correct indentation for different values of 'level'
        //:   and 'spacesPerLevel'.
        //: 2 prints the 'name' correctly when 'name' is supplied.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when a 'name' is supplied.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'print' is the same as the expected output.
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'print' (indentation and name)" << endl
                          << "======================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- --------------
            { L_,    0,    0, "data = %d\n"          },
            { L_,    0,    2, "  data = %d\n"        },
            { L_,    2,    0, "data = %d\n"          },
            { L_,    2,    3, "         data = %d\n" },
            { L_,    2,   -3, " data = %d"           },
            { L_,   -2,    3, "         data = %d\n" },
            { L_,   -2,   -3, " data = %d"           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int data = 448992;
            Obj p(&out, LEVEL, SPL); p.print(data, "data");

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
            const bsl::string EXPECTED(buf);
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'end'
        //
        // Concerns: 'end' prints the correct indentation for different values
        // of 'level' and 'spacesPerLevel', and then prints ']'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'end'.  For each set of values in
        //:   the table, ensure that the actual output of 'end' is the same as
        //:   the expected output.
        //
        // Testing:
        //   void end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'end'" << endl
                                  << "=============" << endl;

        if (verbose) cout << "suppressBracket = false" << endl
                          << "-----------------------" << endl;
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, "]\n"       },
                { L_,    0,    2, "]\n"       },
                { L_,    2,    0, "]\n"       },
                { L_,    2,    3, "      ]\n" },
                { L_,    2,   -3, " ]"        },
                { L_,   -2,    3, "      ]\n" },
                { L_,   -2,   -3, " ]"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                Obj p(&out, LEVEL, SPL); p.end();

                const bsl::string& EXPECTED = DATA[i].d_expected;
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }

        if (verbose) cout << "suppressBracket = true" << endl
                          << "----------------------" << endl;
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, "\n" },
                { L_,    0,    2, "\n" },
                { L_,    2,    0, "\n" },
                { L_,    2,    3, "\n" },
                { L_,    2,   -3, ""   },
                { L_,   -2,    3, "\n" },
                { L_,   -2,   -3, ""   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                Obj p(&out, LEVEL, SPL); p.end(true);

                const bsl::string& EXPECTED = DATA[i].d_expected;
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'start'
        //
        // Concerns: 'start' prints the correct indentation for different
        // values of 'level' and 'spacesPerLevel', and then prints '['.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'start'.  For each set of
        //:   values in the table, ensure that the actual output of
        //:   'start' is the same as the expected output.
        //
        // Testing:
        //   void start() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'start'" << endl
                                  << "===============" << endl;

        if (verbose) cout << "suppressBracket = false" << endl
                          << "-----------------------" << endl;
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, "[\n"       },
                { L_,    0,    2, "[\n"       },
                { L_,    2,    0, "[\n"       },
                { L_,    2,    3, "      [\n" },
                { L_,    2,   -3, "      ["   },
                { L_,   -2,    3, "[\n"       },
                { L_,   -2,   -3, "["         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                Obj p(&out, LEVEL, SPL); p.start();

                const bsl::string& EXPECTED = DATA[i].d_expected;
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }

        if (verbose) cout << "suppressBracket = true" << endl
                          << "----------------------" << endl;
        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    0,    0, ""       },
                { L_,    0,    2, ""       },
                { L_,    2,    0, ""       },
                { L_,    2,    3, "      " },
                { L_,    2,   -3, "      " },
                { L_,   -2,    3, ""       },
                { L_,   -2,   -3, ""       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                Obj p(&out, LEVEL, SPL); p.start(true);

                const bsl::string& EXPECTED = DATA[i].d_expected;
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 'HasPrint::print' prints the expected output for different values
        //:    of level and spaces per level.
        //: 2 'NoPrintUtil::print' prints the expected output for different
        //:   values of level and spaces per level.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'HasPrint::print'.  For each set of
        //:   values in the table, ensure that the actual output of
        //:   'HasPrint::print' is the same as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'NoPrintUtil::print'.  For each set
        //:   of values in the table,  ensure that the actual output of
        //:   'NoPrintUtil::print' is the same as the expected output.
        //
        // Testing:
        //   bsl::ostream& HasPrint::print(bsl::ostream&, int, int) const;
        //   bsl::ostream& NoPrintUtil::print
        //                           (bsl::ostream&, const NoPrint&, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING TEST APPARATUS" << endl
                                  << "======================" << endl;

        {
            if (verbose) cout << "HasPrint::print" << endl
                              << "---------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      HasPrint:data = %d\n" },
                { L_,    2,   -2, " HasPrint:data = %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out; int tData = 73326; HasPrint data(tData);
                data.print(out, LEVEL, SPL);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "NoPrintUtil::print" << endl
                              << "------------------" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      %d\n" },
                { L_,    2,   -2, " %d"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out; int tData = 73326; NoPrint data(tData);
                NoPrintUtil::print(out, data, LEVEL, SPL);

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), tData);
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The constructor can create an object having any state required
        //:   for thorough testing.
        //: 2 Accessors return the corresponding state variable of the object.
        //: 3 Accessors are declared 'const'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, and spaces
        //:   per level.  For each set of values in the table, create a 'const
        //:   Printer' object, and then use the basic accessors to compare the
        //:   values set in the object with the input values.
        //
        // Testing:
        //   Printer(bsl::ostream *stream, int level, int spacesPerLevel);
        //   int absLevel() const;
        //   bool suppressInitialIndentFlag() const;
        //   int spacesPerLevel() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CONSTRUCTOR AND BASIC ACCESSORS" << endl
                          << "=======================================" << endl;

        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
        } DATA[] = {
            //LINE  LEVEL SPL
            //----  ----- ---
            { L_,    0,    0 },
            { L_,    2,    3 },
            { L_,    2,   -3 },
            { L_,   -2,    3 },
            { L_,   -2,   -3 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE   = DATA[i].d_lineNum;
            int LEVEL        = DATA[i].d_level;
            int SPL_EXP      = DATA[i].d_spacesPerLevel;
            int ABSLVL_EXP   = LEVEL > 0 ? LEVEL:-LEVEL;
            bool SIIFLAG_EXP = LEVEL < 0 ? true:false;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL_EXP) }

            ostringstream out; const Obj p(&out, LEVEL, SPL_EXP);
            int ABSLVL_ACT = p.absLevel();
            int SPL_ACT = p.spacesPerLevel();
            int SIIFLAG_ACT = p.suppressInitialIndentFlag();

            LOOP2_ASSERT(ABSLVL_EXP, ABSLVL_ACT, ABSLVL_EXP == ABSLVL_ACT);
            LOOP2_ASSERT(SPL_EXP, SPL_ACT, SPL_EXP == SPL_ACT);
            LOOP2_ASSERT(SIIFLAG_EXP, SIIFLAG_ACT, SIIFLAG_EXP == SIIFLAG_ACT);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to support comprehensive
        //:   testing.
        //
        // Plan: Create a 'Printer' object, and call its 'print' method with an
        // integer.  Ensure that the actual output is the same as the expected
        // output.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        ostringstream out;
        int data = 4892;
        Obj p(&out, 0, 4); p.print(data, 0);

        const bsl::string EXPECTED("    4892\n");
        const bsl::string& ACTUAL = out.str();

        if (veryVerbose) {
            cout << "\tEXPECTED:\n" << "\t" << EXPECTED << endl
                 << "\tACTUAL:\n" << "\t" << ACTUAL << endl;
        }
        LOOP2_ASSERT(EXPECTED, ACTUAL, EXPECTED == ACTUAL);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
