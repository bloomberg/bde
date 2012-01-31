// bslim_printer.t.cpp                                                -*-C++-*-
#include <bslim_printer.h>

#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdio.h>

#include <stdlib.h>    // 'atoi'
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
// [11] template<class TYPE>
//         void printAttribute(const char *name, const TYPE& data) const;
// [16] template <class TYPE, class PRINT_FUNCTOR>
//         void printForeign(const TYPE&           data,
//                           const PRINT_FUNCTOR&  printFunctionObject,
//                           const char           *name) const;
// [15] void printHexAddr(const void *address, const char *name) const;
// [13] template <class TYPE>
//      void printOrNull(const TYPE& address, const char *name) const;
// [11] template<class TYPE>
//         void printValue(const TYPE& data) const;
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

struct TestEnumNoStreaming {
   // A test enum with no streaming operator.
   enum Enum {
       VALUE_A,
       VALUE_B
   };
};

struct TestEnumWithStreaming {
    // A test enum with a streaming operator.
    enum Enum {
        VALUE_A,
        VALUE_B
    };
};

bsl::ostream& operator<<(bsl::ostream&               stream,
                         TestEnumWithStreaming::Enum value)
{
    const char *ascii;
    switch (value) {
      case TestEnumWithStreaming::VALUE_A:
        ascii = "VALUE_A";
        break;
      case TestEnumWithStreaming::VALUE_B:
        ascii = "VALUE_B";
        break;
      default:
        BSLS_ASSERT(false);
    }
    stream << ascii;
    return stream;
}

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

void testFunctionAddress(int)
{
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// EXAMPLE 1
class Datetime {
  public:
    bsl::ostream& print(bsl::ostream& stream,
                        int           = 0,
                        int           = 4) const
    { return stream; }
};

class RecordAttributes {
    // This class provides a container for a fixed set of attributes.

    // DATA
    Datetime         d_timestamp;    // creation date and time
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
    printer.printAttribute("timestamp", d_timestamp);
    printer.printAttribute("process ID", d_processID);
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
    printer.printAttribute("Attribute2", obj.getAttribute1());
    printer.printAttribute("Attribute2", obj.getAttribute2());
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
    printer.printAttribute("AttributeB", d_attributeB);
    printer.end();

    return stream;
}

// EXAMPLE 4
//
class DateTz {
    // This 'class' represents a date value explicitly in a local time
    // zone.  The offset of that time (in minutes) from GMT is also part of
    // the value of this class.

  private:
    // DATA
    int d_localDate;  // date in YYYYMMDD format, local to the timezone
                      // indicated by 'd_offset'

    int d_offset;     // offset from GMT (in minutes)

  public:
    // ...
    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
    // ...
};

bsl::ostream& DateTz::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
   if (stream.bad()) {
       return stream;                                                 // RETURN
   }

   bsl::ostringstream tmp;
   tmp << d_localDate;

   const char sign    = d_offset < 0 ? '-' : '+';
   const int  minutes = '-' == sign ? -d_offset : d_offset;
   const int  hours   = minutes / 60;

   // space usage: +-  hh  mm  nil
   const int SIZE = 1 + 2 + 2 + 1;
   char buf[SIZE];

   // Use at most 2 digits for 'hours'
   if (hours < 100) {
       bsl::sprintf(buf, "%c%02d%02d", sign, hours, minutes % 60);
   }
   else {
       bsl::sprintf(buf, "%cXX%02d", sign, minutes % 60);
   }

   tmp << buf;

   bslim::Printer printer(&stream, level, spacesPerLevel);
   printer.start(true);
   stream << tmp.str();
   printer.end(true);

   return stream << bsl::flush;
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
      case 19: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        // Tested above

      } break;
      case 18: {
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
        (void)data;  // used
        ostringstream out;

        bslma_TestAllocator sa("supplied", veryVeryVeryVerbose);
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

      } break;
      case 17: {
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
      case 16: {
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
      case 15: {
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
      case 14: {
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
        //:   pointer of type 'void *', 'const void *', 'const char *',
        //:  'int *' and 'HasPrint *' is the same as the expected output.
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
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: 'printOrNull' (non-null pointer types)
        //
        // Concerns: That 'printOrNull'
        //: 1 prints the address held by the pointer in hexadecimal format, but
        //:   does not attempt to dereference the pointer if the pointer type
        //:   is 'void *', or  'const void *'.
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
        //:   'void *', or 'const void *'.  For each set of values in the
        //:   table, ensure that the actual output of 'printOrNull' is the
        //:   same as the expected output.
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
                { L_,    2,    2, "      '%c'\n" },
                { L_,    2,   -2, " '%c'"        },
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
      case 12: {
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
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'printAttribute' and 'printValue'
        //
        // Concerns:
        //: 1 That 'printAttribute' and 'printValue' call 'print' correctly.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output when 'printAttribute' and
        //:   'printValue' are called.  For each set of values in the table,
        //:   ensure that the actual outputs are the same as the expected
        //:   outputs.
        //
        // Testing:
        //   template<class TYPE>
        //   void printAttribute(const char *name, const TYPE& data) const;
        //   template<class TYPE>
        //   void printValue(const TYPE& data) const;
        // --------------------------------------------------------------------
        if (verbose)
        cout << endl
             << "TESTING 'printAttribute' and 'printValue'" << endl
             << "=========================================" << endl;
        static const struct {
            int         d_lineNum;        // source line number
            int         d_level;          // indentation level
            int         d_spacesPerLevel; // spaces per indentation level
            bsl::string d_expected;       // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL EXPECTED OUTPUT
            //----  ----- --- ---------------
            { L_,    2,    3, "         data = %d\n         %d\n" },
            { L_,    2,   -3, " data = %d %d"                     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            int data = 448992;
            Obj p(&out, LEVEL, SPL);
            p.printAttribute("data", data); p.printValue(data);

            char buf[999];
            snprintf(buf, 999, DATA[i].d_expected.c_str(), data, data);
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
        //:   'void *', 'const void *', 'const char *', 'int *' and
        //:  'HasPrint *' is the same as the expected output.
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

                typedef void (*functionPtr)(int);
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream vOut, cvOut, vvOut, vcvOut, cOut, iOut, uOut,
                              fOut;

                void *data = 0;
                Obj pV(&vOut, LEVEL, SPL); pV.print((const void *)data, 0);
                Obj pCV(&cvOut, LEVEL, SPL); pCV.print((const void *)data, 0);
                Obj pC(&cOut, LEVEL, SPL); pC.print((const char *)data, 0);
                Obj pI(&iOut, LEVEL, SPL); pI.print((int *)data, 0);
                Obj pU(&uOut, LEVEL, SPL); pU.print((HasPrint *)data, 0);
                Obj pF(&fOut, LEVEL, SPL); pF.print((functionPtr)data, 0);
                const bsl::string& EXPECTED = DATA[i].d_expected;

                // void *
                {
                    const bsl::string& ACTUAL = vOut.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const void *
                {
                    const bsl::string& ACTUAL = cvOut.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // const char *
                {
                    const bsl::string& ACTUAL = cOut.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // int *
                {
                    const bsl::string& ACTUAL = iOut.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // HasPrint *
                {
                    const bsl::string& ACTUAL = uOut.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                // functionPtr *
                {
                    const bsl::string& ACTUAL = fOut.str();
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
        //:   is 'void *' or 'const void *'.
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
        //:   'void *', 'const void *'.  For each set of values in the table,
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


                const char *data = "testing char *";

                char buf[999];
                snprintf(buf, 999, DATA[i].d_expected.c_str(), data);
                const bsl::string EXPECTED(buf);

                {
                    ostringstream out;
                    Obj p(&out, LEVEL, SPL);
                    p.print(data, 0);
                    const bsl::string& ACTUAL = out.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                {
                    ostringstream out;
                    Obj p(&out, LEVEL, SPL);
                    p.print(const_cast<char *>(data), 0);
                    const bsl::string& ACTUAL = out.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
                {
                    ostringstream out;
                    Obj p(&out, LEVEL, SPL);
                    p.print("testing char *", 0);
                    const bsl::string& ACTUAL = out.str();
                    LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
                }
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
        {
            if (verbose) cout << "function pointers" << endl
                              << "-----------------" << endl;
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

                ostringstream out;
                void (*functionPtr)(int) = testFunctionAddress;
                Obj p(&out, LEVEL, SPL); p.print(functionPtr, 0);

                ostringstream ptr;
                ptr << hex << showbase
                    << reinterpret_cast<bsls_Types::UintPtr>(functionPtr);
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
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'print' (user-defined types and specializations)
        //
        // Concerns: That 'print'
        //: 1 calls the 'print' method of the user-defined type with the values
        //:   of '-(absLevel + 1)' and 'spacesPerLevel'.
        //: 2 prints a newline character after printing the user-defined type
        //:   if 'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //: 3 's specialization for 'bsl::string' prints strings in the same
        //:   format as 'const char *'.
        //: 4 's specialization for 'bsl::vector' prints strings in the same
        //:   format as 'const char *' and that output is preceded and followed
        //:   by the required brackets.
        //: 5 's specialization for 'bsl::vector' will accept types other than
        //:   'bsl::string'.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a
        //:   'HasPrint' object.  For each set of values in the table, ensure
        //:   that the actual output of 'print' is the same as the expected
        //:   output.  (C-1 and C-2)
        //: 2 Create a table with the same values as P-1 with the expected
        //:   output changed to that for 'const char *'.  As in P-1, the actual
        //:   output it compared to the expected output.  (C-3)
        //: 3 Create a table with the same values as P-2 with additional fields
        //:   to specify the expected brackets.  As in P-2, the actual output
        //:   is compared to the expected output.  (C-4)
        //: 4 Create a table with the same values as P-3 except that the
        //:   expected data is integer data in decimal format.  As in P-3, the
        //:   actual output is compared to the expect output.  (C-5)
        //
        // Testing:
        //   template<class TYPE>
        //   void print(const TYPE& data, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout
         << endl
         << "TESTING 'print' (user-defined types and specializations)" << endl
         << "========================================================" << endl;

        if (verbose) cout << endl
                          << "TESTING 'print' (user-defined type)" << endl
                          << "===================================" << endl;
        {
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

                int LEVEL_EXP = -(p.absLevel() + 1);
                int LEVEL_ACT = data.d_level;
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
        }

        if (verbose) cout << endl
                          << "TESTING 'print' (bsl::string)" << endl
                          << "=============================" << endl;

        {
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

                ostringstream  out;
                const char    *tData = "hello"; bsl::string data(tData);
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

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

        if (verbose) cout
                       << endl
                       << "TESTING 'print' (bsl::vector<bsl::string>)" << endl
                       << "==========================================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expPrologue;    // data prologue
                bsl::string d_expFormat;      // data format
                bsl::string d_expEpilogue;    // data epilogue
            } DATA[] = {
                //LINE  LEVEL SPL EXP PROLOGUE EXP FORMAT EXP EPLILOGUE
                //----  ----- --- ------------ ---------- -------------
                { L_,    2,    2, "      [\n",
                                               "        \"%s\"\n",
                                                          "      ]\n" },

                { L_,    2,   -2,
                                  " [",
                                               " \"%s\"",
                                                          " ]"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                bsl::vector<bsl::string> data;
                data.push_back(bsl::string("Hello"));
                data.push_back(bsl::string("world!"));
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                char buf[999], *ptr = &buf[0];
                int  len, size = sizeof(buf);
                len = strlen(DATA[i].d_expPrologue.c_str());
                ASSERT(len + 1 < size);
                strncpy(ptr,
                        DATA[i].d_expPrologue.c_str(),
                        len);
                ptr += len; size -= len;
                for (int j = 0; j < (int)data.size(); ++j) {
                    int n = snprintf(ptr,
                                     size,
                                     DATA[i].d_expFormat.c_str(),
                                     data[j].c_str());
                    ptr += n; size -= n;
                }
                len = strlen(DATA[i].d_expEpilogue.c_str());
                ASSERT(len + 1 < size);
                strncpy(ptr,
                        DATA[i].d_expEpilogue.c_str(),
                        len);
                ptr += len; size -= len;
                ASSERT(size > 0);
                *ptr = '\0';
                const bsl::string EXPECTED(buf);
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }

        if (verbose) cout << endl
                          << "TESTING 'print' (bsl::vector<int>)" << endl
                          << "==================================" << endl;

        {
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expPrologue;    // data prologue
                bsl::string d_expFormat;      // data format
                bsl::string d_expEpilogue;    // data epilogue
            } DATA[] = {
                //LINE  LEVEL SPL EXP PROLOGUE EXP FORMAT EXP EPLILOGUE
                //----  ----- --- ------------ ---------- -------------
                { L_,    2,    2, "      [\n",
                                               "        %d\n",
                                                          "      ]\n" },

                { L_,    2,   -2,
                                  " [",
                                               " %d",
                                                          " ]"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                bsl::vector<int> data;
                data.push_back(0);
                data.push_back(1);
                data.push_back(3);
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                char buf[999], *ptr = &buf[0];
                int  len, size = sizeof(buf);
                len = strlen(DATA[i].d_expPrologue.c_str());
                ASSERT(len + 1 < size);
                strncpy(ptr,
                        DATA[i].d_expPrologue.c_str(),
                        len);
                ptr += len; size -= len;
                for (int j = 0; j < (int)data.size(); ++j) {
                    int n = snprintf(ptr,
                                     size,
                                     DATA[i].d_expFormat.c_str(),
                                     data[j]);
                    ptr += n; size -= n;
                }
                len = strlen(DATA[i].d_expEpilogue.c_str());
                ASSERT(len + 1 < size);
                strncpy(ptr,
                        DATA[i].d_expEpilogue.c_str(),
                        len);
                ptr += len; size -= len;
                ASSERT(size > 0);
                *ptr = '\0';
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
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'print' (fundamental types)
        //
        // Concerns: That 'print'
        //: 1 prints fundamental types correctly.
        //: 2 prints 'char' types
        //:   - enclosed within single quotes when printable
        //:   - in hexadecimal format when non-printable
        //:   - as strings in case of special characters '\n', '\t' and '\0'.
        //: 3 prints 'bool' types as alphabetical strings.
        //: 4 prints a newline character after printing the fundamental type if
        //:   'spacesPerLevel >= 0', and does not print a newline character
        //:   otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with an 'int'.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'print' is the same as the expected output.
        //: 2 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a 'char'.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'print' is the same as the expected output.
        //: 3 Create a table having fields for line number, level, spaces per
        //:   level, and expected output of 'print' when called with a 'bool'.
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

        {
            if (verbose) cout << "int" << endl
                              << "---" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    3, "         %d\n" },
                { L_,    2,   -3, " %d"           }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                int data = 448992;
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
            if (verbose) cout << "char" << endl
                              << "----" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                char        d_arg;            // argument to be printed
                bsl::string d_expected;       // expected output format
                bool        d_isHex;          // flag for print as hex
            } DATA[] = {
                //LINE  LEVEL SPL ARG   EXPECTED OUTPUT  IS HEX
                //----  ----- --- ---   --------------   ------
                { L_,    2,    2, 'a' , "      'a'\n"  , false  },
                { L_,    2,   -2, '\'', " '\''"        , false  },
                { L_,    2,    2, '\n', "      '\\n'\n", false  },
                { L_,    2,    2, '\t', "      '\\t'\n", false  },
                { L_,    2,    2, 0,    "      '\\0'\n", false  },
                { L_,    2,    2, 16  , "      %s\n"   , true   },
                { L_,    2,    2, 31  , "      %s\n"   , true   },
                { L_,    2,    2, 32  , "      ' '\n"  , false  },
                { L_,    2,    2, 126 , "      '~'\n"  , false  },
                { L_,    2,    2, 127 , "      %s\n"   , true   }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;
                bool ISHEX = DATA[i].d_isHex;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                char data = DATA[i].d_arg;
                Obj p(&out, LEVEL, SPL); p.print(data, 0);

                bsl::string EXPECTED;
                if (ISHEX) {
                    stringstream exp;
                    exp << hex << showbase
                        << static_cast<bsls_Types::UintPtr>(data);
                    char buf[999];
                    snprintf(buf, 999, DATA[i].d_expected.c_str(),
                                                            exp.str().c_str());
                    EXPECTED = buf;
                }
                else {
                    EXPECTED = DATA[i].d_expected;
                }
                const bsl::string& ACTUAL = out.str();

                if (veryVeryVerbose) {
                    cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                         << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
                }
                LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
            }
        }
        {
            if (verbose) cout << "bool" << endl
                              << "----" << endl;
            static const struct {
                int         d_lineNum;        // source line number
                int         d_level;          // indentation level
                int         d_spacesPerLevel; // spaces per indentation level
                bsl::string d_expected;       // expected output format
            } DATA[] = {
                //LINE  LEVEL SPL EXPECTED OUTPUT
                //----  ----- --- --------------
                { L_,    2,    2, "      true\n" },
                { L_,    2,   -2, " true"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA;  ++i) {
                const int LINE  = DATA[i].d_lineNum;
                int LEVEL = DATA[i].d_level;
                int SPL   = DATA[i].d_spacesPerLevel;

                if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                ostringstream out;
                bool data = true;
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
            if (verbose) cout << "enum" << endl
                              << "----" << endl;

            // Test level & spaces per level
            {
                static const struct {
                    int         d_lineNum;        // source line number
                    int         d_level;          // indentation level
                    int         d_spacesPerLevel; // spaces per indentation lvl
                    bsl::string d_expected;       // expected output format
                } DATA[] = {
                    //LINE  LEVEL SPL EXPECTED OUTPUT
                    //----  ----- --- --------------
                    { L_,    2,    3, "         %s\n" },
                    { L_,    2,   -3, " %s"           }
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA;  ++i) {
                    const int LINE  = DATA[i].d_lineNum;
                    int LEVEL = DATA[i].d_level;
                    int SPL   = DATA[i].d_spacesPerLevel;

                    if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

                    ostringstream out;
                    TestEnumWithStreaming::Enum value =
                                               TestEnumWithStreaming::VALUE_B;
                    Obj p(&out, LEVEL, SPL); p.printValue(value);

                    char buf[999];
                    snprintf(buf, 999, DATA[i].d_expected.c_str(), "VALUE_B");
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
                // Test different enum values
                TestEnumWithStreaming::Enum asciiA =
                                                TestEnumWithStreaming::VALUE_A;
                TestEnumWithStreaming::Enum asciiB =
                                                TestEnumWithStreaming::VALUE_B;
                TestEnumNoStreaming::Enum nonAsciiA =
                                                  TestEnumNoStreaming::VALUE_A;
                TestEnumNoStreaming::Enum nonAsciiB =
                                                  TestEnumNoStreaming::VALUE_B;

                {
                    ostringstream out;
                    Obj p(&out, 0, -1); p.printValue(asciiA);
                    const bsl::string EXPECTED(" VALUE_A");
                    LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());
                }
                {
                    ostringstream out;
                    Obj p(&out, 0, -1); p.printValue(asciiB);
                    const bsl::string EXPECTED(" VALUE_B");
                    LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());
                }
                {
                    ostringstream out;
                    Obj p(&out, 0, -1); p.printValue(nonAsciiA);
                    const bsl::string EXPECTED(" 0");
                    LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());
                }
                {
                    ostringstream out;
                    Obj p(&out, 0, -1); p.printValue(nonAsciiB);
                    const bsl::string EXPECTED(" 1");
                    LOOP2_ASSERT(EXPECTED, out.str(), EXPECTED == out.str());
                }
            }
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
        // of 'level' and 'spacesPerLevel' and ']' if 'suppressBracket' is
        // 'false', and does not print the indentation and ']' otherwise.
        // 'end' should also print a newline if 'spacesPerLevel' >= 0.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, suppress bracket flag, and expected output of 'end'.  For
        //:   each set of values in the table, ensure that the actual output of
        //:   'end' is the same as the expected output.
        //
        // Testing:
        //   void end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'end'" << endl
                                  << "=============" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            bool        d_suppressBracket; // suppress bracket flag
            bsl::string d_expected;        // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL SUPPRESS BRACKET EXPECTED OUTPUT
            //----  ----- --- ---------------- ---------------
            { L_,    0,    0, false,           "]\n"          },
            { L_,    0,    2, false,           "]\n"          },
            { L_,    2,    0, false,           "]\n"          },
            { L_,    2,    3, false,           "      ]\n"    },
            { L_,    2,   -3, false,           " ]"           },
            { L_,   -2,    3, false,           "      ]\n"    },
            { L_,   -2,   -3, false,           " ]"           },
            { L_,    0,    0, true ,           "\n"           },
            { L_,    0,    2, true ,           "\n"           },
            { L_,    2,    0, true ,           "\n"           },
            { L_,    2,    3, true ,           "\n"           },
            { L_,    2,   -3, true ,           ""             },
            { L_,   -2,    3, true ,           "\n"           },
            { L_,   -2,   -3, true ,           ""             },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;
            bool suppressBracket = DATA[i].d_suppressBracket;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            Obj p(&out, LEVEL, SPL); p.end(suppressBracket);

            const bsl::string& EXPECTED = DATA[i].d_expected;
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'start'
        //
        // Concerns: 'start' prints the correct indentation for different
        // values of 'level' and 'spacesPerLevel', and then prints '[' if
        // 'suppressBracket' is 'false', and does not print '[' otherwise.
        //
        // Plan:
        //: 1 Create a table having fields for line number, level, spaces per
        //:   level, suppress bracket flag, and expected output of 'start'.
        //:   For each set of values in the table, ensure that the actual
        //:   output of 'start' is the same as the expected output.
        //
        // Testing:
        //   void start() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'start'" << endl
                                  << "===============" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            bool        d_suppressBracket; // suppress bracket flag
            bsl::string d_expected;        // expected output format
        } DATA[] = {
            //LINE  LEVEL SPL SUPPRESS BRACKET EXPECTED OUTPUT
            //----  ----- --- ---------------- ---------------
            { L_,    0,    0, false,           "[\n"          },
            { L_,    0,    2, false,           "[\n"          },
            { L_,    2,    0, false,           "[\n"          },
            { L_,    2,    3, false,           "      [\n"    },
            { L_,    2,   -3, false,           "      ["      },
            { L_,   -2,    3, false,           "[\n"          },
            { L_,   -2,   -3, false,           "["            },
            { L_,    0,    0, true ,           ""             },
            { L_,    0,    2, true ,           ""             },
            { L_,    2,    0, true ,           ""             },
            { L_,    2,    3, true ,           "      "       },
            { L_,    2,   -3, true ,           "      "       },
            { L_,   -2,    3, true ,           ""             },
            { L_,   -2,   -3, true ,           ""             }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA;  ++i) {
            const int LINE  = DATA[i].d_lineNum;
            int LEVEL = DATA[i].d_level;
            int SPL   = DATA[i].d_spacesPerLevel;
            bool suppressBracket = DATA[i].d_suppressBracket;

            if (veryVerbose) { T_ P_(LINE) P_(LEVEL) P(SPL) }

            ostringstream out;
            Obj p(&out, LEVEL, SPL); p.start(suppressBracket);

            const bsl::string& EXPECTED = DATA[i].d_expected;
            const bsl::string& ACTUAL = out.str();

            if (veryVeryVerbose) {
                cout << "\t\tEXPECTED:\n" << "\t\t" << EXPECTED << endl
                     << "\t\tACTUAL:\n" << "\t\t" << ACTUAL << endl;
            }
            LOOP3_ASSERT(LINE, EXPECTED, ACTUAL, EXPECTED == ACTUAL);
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
