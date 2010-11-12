// bdem_schemaenumerationutil.t.cpp         -*-C++-*-

#include <bdem_schemaenumerationutil.h>

#include <bdem_schema.h>

#include <bslma_testallocator.h>        // testing only
#include <bdetu_unset.h>

#include <bsl_c_string.h>
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                   TEST PLAN
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 6] bool isConformant(int,                   const bdem_EnumerationDef&)
// [ 7] bool isConformant(const string&,         const bdem_EnumerationDef&)
// [ 8] bool isConformant(const vector<int>&,    const bdem_EnumerationDef&)
// [ 9] bool isConformant(const vector<string>&, const bdem_EnumerationDef&)
//-----------------------------------------------------------------------------
// [ 1] VERIFY CONSTANTS
// [ 2] TEST APPARATUS: ggInt()
// [ 2] TEST APPARATUS: ggString()
// [ 2] TEST APPARATUS: ggEnum()
// [ 3] SCALAR BREATHING TEST
// [ 4] INT VECTOR BREATHING TEST
// [ 5] STRING VECTOR BREATHING TEST
// [10] USAGE EXAMPLE

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
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

static int verbose         = 0;
static int veryVerbose     = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                     GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

static const int unsetInt       = bdetu_Unset<int>::unsetValue();
static const string unsetString = bdetu_Unset<string>::unsetValue();
static const vector<int>    unsetIntArray;
static const vector<string> unsetStringArray;

typedef bdem_SchemaEnumerationUtil Util;

const vector<int>& ggInt(
    vector<int> *intVec,
    const char *string)
{
    const char *pc = string;

    while (1) {
        while (*pc && isspace(*pc)) {
            ++pc;
        }
        if (!*pc) {
            break;
        }

        int val;
        if ('*' == *pc) {
            val = unsetInt;
        }
        else {
            ASSERT(1 == sscanf(pc, "%d", &val));
        }

        while (*pc && !isspace(*pc)) {
            ++pc;
        }

        intVec->push_back(val);
    }

    return *intVec;
}

const vector<string>& ggString(
    vector<string> *stringVec,
    const char *string)
{
    // This routine deliberately leaks tiny amounts of memory
    char *pc = strdup(string);

    bool more = *pc;

    while (more) {
        while (*pc && isspace(*pc)) {
            ++pc;
        }
        if (!*pc) {
            break;
        }

        char *name = pc;

        while (*pc && !isspace(*pc)) {
            ++pc;
        }

        more = *pc;
        *pc = 0;
        ++pc;

        if (!strcmp("*", name)) {
            *name = 0;                // "*" means "" == unsetString
        }

        stringVec->push_back(name);
    }

    return *stringVec;
}

const bdem_EnumerationDef& ggEnum(
    bdem_EnumerationDef *eDef,
    const char *string)
{
    // This routine deliberately leaks tiny amounts of memory
    char *pc = strdup(string);

    while (*pc) {
        while (*pc && isspace(*pc)) {
            ++pc;
        }
        if (!*pc) {
            break;
        }

        const char *name = pc;

        while (*pc && !isspace(*pc)) {
            ++pc;
        }

        ASSERT(*pc);
        *pc = 0;
        ++pc;
        while (*pc && isspace(*pc)) {
            ++pc;
        }
        ASSERT(*pc);

        int val;
        ASSERT(1 == sscanf(pc, "%d", &val));

        while (*pc && !isspace(*pc)) {
            ++pc;
        }

        eDef->addEnumerator(name, val);
    }

    return *eDef;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test        = argc > 1 ? atoi(argv[1]) : 0;
    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'isConformant' methods of
        //   this component.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Usage Example" << endl
                                  << "=============" << endl;

        // Usage
        // -----
        // Here we demonstrate the circumstances in which ints, strings, int
        // arrays and string arrays are conformant to an enumeration and those
        // circumstances in which they are not:

        vector<int> intArray;          intArray.push_back(5);
        vector<string> stringArray;    stringArray.push_back("a");

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        bdem_EnumerationDef& eDef = *mS.createEnumeration();
        const bdem_EnumerationDef& EDef = eDef;

        // unset values are conformant to ANY enumeration
        ASSERT(1 == Util::isConformant(unsetInt,            EDef));
        ASSERT(1 == Util::isConformant(unsetString,         EDef));
        ASSERT(1 == Util::isConformant(unsetString.c_str(), EDef));
        ASSERT(1 == Util::isConformant(unsetIntArray,       EDef));
        ASSERT(1 == Util::isConformant(unsetStringArray,    EDef));

        // no set value is conformant to an empty enumeration
        ASSERT(0 == Util::isConformant(5,           EDef));
        ASSERT(0 == Util::isConformant(string("a"), EDef));
        ASSERT(0 == Util::isConformant("a",         EDef));
        ASSERT(0 == Util::isConformant(intArray,    EDef));
        ASSERT(0 == Util::isConformant(stringArray, EDef));

        eDef.addEnumerator("a", 5);

        // if the value is contained in one of the string-int pairs, it's
        // conformant
        ASSERT(1 == Util::isConformant(5,           EDef));
        ASSERT(1 == Util::isConformant(string("a"), EDef));
        ASSERT(1 == Util::isConformant("a",         EDef));
        ASSERT(1 == Util::isConformant(intArray,    EDef));
        ASSERT(1 == Util::isConformant(stringArray, EDef));

        eDef.addEnumerator("b",  7);
        eDef.addEnumerator("c", 10);
        eDef.addEnumerator("f", 15);

        intArray.push_back(10);
        stringArray.push_back("f");

        // another conformant example
        ASSERT(1 == Util::isConformant(10,          EDef));
        ASSERT(1 == Util::isConformant(string("f"), EDef));
        ASSERT(1 == Util::isConformant("f",         EDef));
        ASSERT(1 == Util::isConformant(intArray,    EDef));
        ASSERT(1 == Util::isConformant(stringArray, EDef));

        intArray.push_back(8);
        stringArray.push_back("e");

        // values not contained in the enumeration are not conformant
        ASSERT(0 == Util::isConformant(8,           EDef));
        ASSERT(0 == Util::isConformant(string("e"), EDef));
        ASSERT(0 == Util::isConformant("e",         EDef));
        ASSERT(0 == Util::isConformant(intArray,    EDef));
        ASSERT(0 == Util::isConformant(stringArray, EDef));

        // again, unset values are conformant to ANY enumeration
        ASSERT(1 == Util::isConformant(unsetInt,            EDef));
        ASSERT(1 == Util::isConformant(unsetString,         EDef));
        ASSERT(1 == Util::isConformant(unsetString.c_str(), EDef));
        ASSERT(1 == Util::isConformant(unsetIntArray,       EDef));
        ASSERT(1 == Util::isConformant(unsetStringArray,    EDef));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TABLE DRIVEN STRING VECTOR TEST
        //
        // Concerns:
        //   Need a wide variety of tests of integer vectors, which will be
        //   facilitated by table-driven code.
        //
        // Plan:
        //   Use 'ggString()' to create a variety of string vectors, and
        //   'ggEnum()' to create a variety of enumerations and verify that
        //   the conformance between them is what we expect.
        //
        //   Note that we verify that none of the strings vectors in the
        //   table are conformant with an empty enumerationDef, and that an
        //   empty string vector or one containing only instances of the
        //   unset string is conformant with all the enumerationDef's in the
        //   table.
        //
        // Tactics:
        //   Table-driven, multiple gg()-type functions.
        //
        // Testing:
        //   Util::isConformant(const vector<string>&, const enumerationDef&)
        // --------------------------------------------------------------------

        if (verbose) cout << "Table-driven String Vector Test\n"
                             "===============================\n";

        struct {
            int         d_line;
            const char *d_ggStrVecStr;
            const char *d_ggEnumStr;
            int         d_conformant;
        } DATA[] = {
            // note '*' means unsetString
            { L_, "c",                "a 2", 0 },
            { L_, "a",                "a 2", 1 },
            { L_, "* a",              "a 2", 1 },
            { L_, "* c",              "a 2", 0 },
            { L_, "j k l",            "a 2", 0 },
            { L_, "z",                "a 1 b 2 c 3", 0 },
            { L_, "a a b",            "a 1 b 2 c 3", 1 },
            { L_, "a b c",            "a 1 b 2 c 3", 1 },
            { L_, "c b a b c",        "a 1 b 2 c 3", 1 },
            { L_, "a b c e",          "a 1 b 2 c 3", 0 },
            { L_, "a a a a a",        "a 1 b 2 c 3", 1 },
            { L_, "a a a * a a",      "a 1 b 2 c 3", 1 },
            { L_, "aa bb cc",         "a 1 b 2 c 3", 0 },
            { L_, "jj a b",           "a 1 b 2 c 3", 0 },
            { L_, "a b c",            "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "arf",              "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "arf a",            "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "woof meow",        "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "meow woof",        "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "meow * woof *",    "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "growl arf",        "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "putt",             "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "boo hoo",               "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "boo * hoo *",           "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "hoo bow",               "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "do well",               "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, "do well *",             "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, "boo wow bow wow hoo",   "boo 83 hoo 23 bow 9 wow 35", 1 }
        };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        for (int i = 0; i < DATA_LEN; ++i) {
            const int LINE             = DATA[i].d_line;
            const char *GG_STR_VEC_STR = DATA[i].d_ggStrVecStr;
            const char *GG_ENUM_STR    = DATA[i].d_ggEnumStr;
            const int CONF             = DATA[i].d_conformant;

            vector<string> stringVec;
            const vector<string>& StringVec = ggString(&stringVec,
                                                               GG_STR_VEC_STR);

            {
                // There are no string vectors that are either empty or
                // containing only unsetString in the table, therefore none of
                // the vectors are conformant with an empty enumerationDef.
                bdem_EnumerationDef& emptyEDef = *mS.createEnumeration();
                const bdem_EnumerationDef& EmptyEDef = emptyEDef;

                ASSERT(0 == Util::isConformant(StringVec, EmptyEDef));
            }

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = ggEnum(&eDef, GG_ENUM_STR);
            ASSERT(EDef.numEnumerators() > 0);

            {
                // An empty vector is conformant with any enumerationDef.
                vector<string> vec2;
                ASSERT(1 == Util::isConformant(vec2, EDef));

                // As is a vector containing only unset strings.
                for (int i = 0; i < 10; ++i) {
                    vec2.push_back(unsetString);
                    ASSERT(1 == Util::isConformant(vec2, EDef));
                }
            }

            LOOP_ASSERT(LINE, CONF == Util::isConformant(StringVec, EDef));
        }

        {
            // An empty vector is conformant with an empty EnumerationDef.
            vector<string> vec3;
            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            ASSERT(1 == Util::isConformant(vec3, eDef));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TABLE DRIVEN INTEGER VECTOR TEST
        //
        // Concerns:
        //   Need a wide variety of tests of integer vectors, which will be
        //   facilitated by table-driven code.
        //
        // Plan:
        //   Use 'ggInt()' to create a variety of int vectors, and 'ggEnum()'
        //   to create a variety of enumerations and verify that the
        //   conformance between them is what we expect.
        //
        //   Note that we verify that none of the int vectors in the
        //   table are conformant with an empty enumerationDef, and that an
        //   empty int vector or one containing only instances of the
        //   unset int is conformant with all the enumerationDef's in the
        //   table.
        //
        // Tactics:
        //   Table-driven, multiple gg()-type functions.
        //
        // Testing:
        //   Util::isConformant(const vector<int>&, const enumerationDef&)
        // --------------------------------------------------------------------

        if (verbose) cout << "Table-driven Integer Vector Test\n"
                             "================================\n";

        struct {
            int         d_line;
            const char *d_ggIntVecStr;
            const char *d_ggEnumStr;
            int         d_conformant;
        } DATA[] = {
            // note '*' means unsetInt
            { L_, "0",             "a 2", 0 },
            { L_, "2",             "a 2", 1 },
            { L_, "* 2",           "a 2", 1 },
            { L_, "* 4",           "a 2", 0 },
            { L_, "42",            "a 2", 0 },
            { L_, "0",            "a 1 b 2 c 3", 0 },
            { L_, "1 1 2",        "a 1 b 2 c 3", 1 },
            { L_, "1 2 3",        "a 1 b 2 c 3", 1 },
            { L_, "3 2 1 2 3",    "a 1 b 2 c 3", 1 },
            { L_, "1 2 3 5",      "a 1 b 2 c 3", 0 },
            { L_, "1 1 1 1 1",    "a 1 b 2 c 3", 1 },
            { L_, "1 1 * 1 1 1",  "a 1 b 2 c 3", 1 },
            { L_, "83 94 23",     "a 1 b 2 c 3", 0 },
            { L_, "13 1 2",       "a 1 b 2 c 3", 0 },
            { L_, "13 1 2 *",     "a 1 b 2 c 3", 0 },
            { L_, "* 13 1 2",     "a 1 b 2 c 3", 0 },
            { L_, "1 2 3",        "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "102374",       "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "102374 5",     "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "8374 37476",   "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "37476 8374",   "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "37476 8374 *", "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "45 57 93",     "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "0",            "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "83 23",        "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "83 23 * * *",  "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "23 9",         "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "77 6",         "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, "83 35 9 35 23","boo 83 hoo 23 bow 9 wow 35", 1 }
        };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        for (int i = 0; i < DATA_LEN; ++i) {
            const int LINE             = DATA[i].d_line;
            const char *GG_INT_VEC_STR = DATA[i].d_ggIntVecStr;
            const char *GG_ENUM_STR    = DATA[i].d_ggEnumStr;
            const int CONF             = DATA[i].d_conformant;

            vector<int> intVec;
            const vector<int>& IntVec = ggInt(&intVec, GG_INT_VEC_STR);

            {
                // There are no integer vectors that are either empty or
                // containing only INT_MIN in the table, therefore none of
                // the vectors are conformant with an empty enumerationDef.
                bdem_EnumerationDef& emptyEDef = *mS.createEnumeration();
                const bdem_EnumerationDef& EmptyEDef = emptyEDef;

                ASSERT(0 == Util::isConformant(IntVec, EmptyEDef));
            }

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = ggEnum(&eDef, GG_ENUM_STR);
            ASSERT(EDef.numEnumerators() > 0);

            {
                // An empty vector is conformant with any enumerationDef.
                vector<int> vec2;
                ASSERT(1 == Util::isConformant(vec2, EDef));

                // As is a vector containing only unset ints.
                for (int i = 0; i < 10; ++i) {
                    vec2.push_back(unsetInt);
                    ASSERT(1 == Util::isConformant(vec2, EDef));
                }
            }

            LOOP_ASSERT(LINE, CONF == Util::isConformant(IntVec, EDef));
        }

        {
            // An empty vector is conformant with an empty EnumerationDef.
            vector<int> vec3;
            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            ASSERT(1 == Util::isConformant(vec3, eDef));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TABLE DRIVEN STRING SCALAR TEST
        //
        // Concerns:
        //   Need a wide variety of tests of scalar strings, which will be
        //   facilitated by table-driven code.
        //
        // Plan:
        //   Use a table to generate a large number of combinations of
        //   string scalars & enumerationDef's.
        //
        //   None of the string scalars in the table are the unset string,
        //   so we can verify that none of the string scalars in the table
        //   are conformant with an empty enumerationDef.
        //       We also verify that the unset string is conformant with
        //   all the enumerationDef's in the table.
        //
        // Tactics:
        //   Table driven.
        //
        // Testing:
        //   Util::isConformant(string, const EnumerationDef&)
        // --------------------------------------------------------------------

        if (verbose) cout << "Table-driven String Scalar Test\n"
                             "===============================\n";

        struct {
            int         d_line;
            const char *d_stringVal;
            const char *d_ggEnumStr;
            int         d_conformant;
        } DATA[] = {
            { L_, " ",        "a 2", 0 },
            { L_, "z",        "a 2", 0 },
            { L_, "a",        "a 2", 1 },
            { L_, "snickers", "a 2", 0 },
            { L_, " ",        "a 1 b 2 c 3", 0 },
            { L_, "a",        "a 1 b 2 c 3", 1 },
            { L_, "b",        "a 1 b 2 c 3", 1 },
            { L_, "c",        "a 1 b 2 c 3", 1 },
            { L_, "babyruth", "a 1 b 2 c 3", 0 },
            { L_, " ",        "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "hoop",     "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "flow",     "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "moan",     "arf 102374 woof 37476 meow 8374", 0 },
            { L_, "arf",      "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "woof",     "arf 102374 woof 37476 meow 8374", 1 },
            { L_, "meow",     "arf 102374 woof 37476 meow 8374", 1 },
            { L_, " ",        "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, "front",    "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, "bow",      "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, "back",     "boo 83 hoo 23 bow 9 wow 35", 0 }
        };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        for (int i = 0; i < DATA_LEN; ++i) {
            const int LINE          = DATA[i].d_line;
            const string STRING_VAL = DATA[i].d_stringVal;
            const char *GG_ENUM_STR = DATA[i].d_ggEnumStr;
            const int CONF          = DATA[i].d_conformant;

            {
                // The unset string is not in the table, therefore all string
                // values in the table will not be conformant to an empty
                // enumerationDef.
                bdem_EnumerationDef& emptyEDef = *mS.createEnumeration();
                const bdem_EnumerationDef& EmptyEDef = emptyEDef;

                ASSERT(0 == Util::isConformant(STRING_VAL, EmptyEDef));
            }

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = ggEnum(&eDef, GG_ENUM_STR);

            ASSERT(EDef.numEnumerators() > 0);
            ASSERT(1 == Util::isConformant(unsetString, EDef));

            LOOP_ASSERT(LINE, CONF == Util::isConformant(STRING_VAL, EDef));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TABLE DRIVEN INTEGER SCALAR TEST
        //
        // Concerns:
        //   Need a wide variety of tests of scalar ints, which will be
        //   facilitated by table-driven code.
        //
        // Plan:
        //   Use a table to generate a large number of combinations of
        //   int scalars & enumerationDef's.
        //
        //   None of the int scalars in the table are the unset int, so we can
        //   verify that none of the string scalars in the table are
        //   conformant with an empty enumerationDef.
        //       We also verify that the unset int is conformant with all the
        //   enumerationDef's in the table.
        //
        // Tactics:
        //   Table driven.
        //
        // Testing:
        //   Util::isConformant(int, const EnumerationDef&)
        // --------------------------------------------------------------------

        if (verbose) cout << "Table-driven Integer Scalar Test\n"
                             "================================\n";

        struct {
            int         d_line;
            int         d_intVal;
            const char *d_ggEnumStr;
            int         d_conformant;
        } DATA[] = {
            { L_, 0,        "a 2", 0 },
            { L_, 10,       "a 2", 0 },
            { L_, -1,       "a 2", 0 },
            { L_, INT_MAX,  "a 2", 0 },
            { L_, -INT_MAX, "a 2", 0 },
            { L_, 2,        "a 2", 1 },
            { L_, 0,        "a 1 b 2 c 3", 0 },
            { L_, 1,        "a 1 b 2 c 3", 1 },
            { L_, 2,        "a 1 b 2 c 3", 1 },
            { L_, 3,        "a 1 b 2 c 3", 1 },
            { L_, 73847,    "a 1 b 2 c 3", 0 },
            { L_, 0,        "arf 102374 woof 37476 meow 8374", 0 },
            { L_, 12,       "arf 102374 woof 37476 meow 8374", 0 },
            { L_, 22,       "arf 102374 woof 37476 meow 8374", 0 },
            { L_, 55,       "arf 102374 woof 37476 meow 8374", 0 },
            { L_, 102374,   "arf 102374 woof 37476 meow 8374", 1 },
            { L_, 37476,    "arf 102374 woof 37476 meow 8374", 1 },
            { L_, 8374,     "arf 102374 woof 37476 meow 8374", 1 },
            { L_, 7,        "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, 8374,     "boo 83 hoo 23 bow 9 wow 35", 0 },
            { L_, 9,        "boo 83 hoo 23 bow 9 wow 35", 1 },
            { L_, 0,        "boo 83 hoo 23 bow 9 wow 35", 0 }
        };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        for (int i = 0; i < DATA_LEN; ++i) {
            const int LINE          = DATA[i].d_line;
            const int INT_VAL       = DATA[i].d_intVal;
            const char *GG_ENUM_STR = DATA[i].d_ggEnumStr;
            const int CONF          = DATA[i].d_conformant;

            {
                // INT_MIN is not in the table, therefore all integer values
                // in the table will not be conformant to an empty
                // enumerationDef.
                bdem_EnumerationDef& emptyEDef = *mS.createEnumeration();
                const bdem_EnumerationDef& EmptyEDef = emptyEDef;

                ASSERT(0 == Util::isConformant(INT_VAL, EmptyEDef));
            }

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = ggEnum(&eDef, GG_ENUM_STR);

            ASSERT(EDef.numEnumerators() > 0);
            ASSERT(1 == Util::isConformant(unsetInt, EDef));

            LOOP_ASSERT(LINE, CONF == Util::isConformant(INT_VAL, EDef));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STRING VECTORS BREATHING TEST
        //
        // Concerns:
        //   That we properly test the conformance of string vectors.
        //
        // Plan:
        //   Test a reasonable combination of string vector / enumeration
        //   inputs, very simple tests without gg() functions.
        //
        // Testing:
        //   Util::isConformant(const vector<string>&, const Enumeration&);
        // --------------------------------------------------------------------

        if (verbose) cout << "Breathing test of string vectors\n"
                             "================================\n";

        vector<string> stringArrayA;    stringArrayA.push_back("a");
        vector<string> stringArrayABC;
        {
            stringArrayABC.push_back("a");
            stringArrayABC.push_back("b");
            stringArrayABC.push_back("c");
        }

        vector<string> stringArrayF;    stringArrayF.push_back("f");

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        bdem_EnumerationDef& eDef = *mS.createEnumeration();
        const bdem_EnumerationDef& EDef = eDef;

        ASSERT(1 == Util::isConformant(unsetStringArray, EDef));
        ASSERT(0 == Util::isConformant(stringArrayA,     EDef));
        ASSERT(0 == Util::isConformant(stringArrayABC,   EDef));
        ASSERT(0 == Util::isConformant(stringArrayF,     EDef));

        eDef.addEnumerator("a", 5);
        eDef.addEnumerator("b", 6);
        eDef.addEnumerator("c", 7);

        ASSERT(1 == Util::isConformant(unsetStringArray, EDef));
        ASSERT(1 == Util::isConformant(stringArrayA,     EDef));
        ASSERT(1 == Util::isConformant(stringArrayABC,   EDef));
        ASSERT(0 == Util::isConformant(stringArrayF,     EDef));
      } break;
      case 4: {
        //=====================================================================
        // INT VECTORS BREATHING TEST
        //
        // Concerns:
        //   That we properly test the conformance of int vectors.
        //
        // Plan:
        //   Test a reasonable combination of int vector / enumeration inputs,
        //   very simple tests without gg() functions.
        //
        // Testing (superficially):
        //   Util::isConformant(const vector<int>&, const EnumerationDef&)
        //=====================================================================

        if (verbose) cout << "Breathing test of int vectors\n"
                             "=============================\n";

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        {
            vector<int> intArray5;
            {
                intArray5.push_back(5);
                intArray5.push_back(6);
                intArray5.push_back(7);
            }
            vector<int> intArray8;          intArray8.push_back(8);

            bdem_EnumerationDef& eDef = *mS.createEnumeration();

            ASSERT(1 == Util::isConformant(unsetIntArray, eDef));
            ASSERT(0 == Util::isConformant(intArray5,     eDef));
            ASSERT(0 == Util::isConformant(intArray8,     eDef));

            eDef.addEnumerator("a", 5);
            eDef.addEnumerator("b", 6);
            eDef.addEnumerator("c", 7);

            const bdem_EnumerationDef& EDef = eDef;

            ASSERT(1 == Util::isConformant(unsetIntArray, EDef));
            ASSERT(1 == Util::isConformant(intArray5,     EDef));
            ASSERT(0 == Util::isConformant(intArray8,     EDef));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SCALARS BREATHING TEST
        //
        // Concerns:
        //   That we properly test the conformance of scalar ints and strings.
        //
        // Plan:
        //   - test scalars for conformity to a null EnumerationDef
        //   - add an enumerator to the EnumerationDef and observe how that
        //     changes the conformity results
        //   - build a partially populated vector of string names of integer
        //     indexes of the names, put enumerators into an EnumerationDef
        //     for every int that is named in the vector, observe the
        //     the conformities match properly
        //
        // Testing (superficially):
        //   Util::isConformant(int,    const EnumerationDef&)
        //   Util::isConformant(string, const EnumerationDef&)
        // --------------------------------------------------------------------

        if (verbose) cout << "Breating test of Scalars\n"
                             "========================\n";

        bdem_Schema mS(Z);     const bdem_Schema& S = mS;

        {
            if (veryVerbose) cout << "Testing Scalar Ints\n"
                                     "===================\n";

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = eDef;

            ASSERT(1 == Util::isConformant(unsetInt, EDef));
            ASSERT(0 == Util::isConformant(5,        EDef));
            ASSERT(0 == Util::isConformant(8,        EDef));

            eDef.addEnumerator("a", 5);

            ASSERT(1 == Util::isConformant(unsetInt, EDef));
            ASSERT(1 == Util::isConformant(5,        EDef));
            ASSERT(0 == Util::isConformant(8,        EDef));
        }

        {
            if (veryVerbose) cout << "Testing Scalar Strings\n"
                                     "======================\n";

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            const bdem_EnumerationDef& EDef = eDef;

            ASSERT(1 == Util::isConformant(unsetString, EDef));
            ASSERT(0 == Util::isConformant("a",         EDef));
            ASSERT(0 == Util::isConformant("f",         EDef));

            eDef.addEnumerator("a", 5);

            ASSERT(1 == Util::isConformant(unsetString, EDef));
            ASSERT(1 == Util::isConformant("a",         EDef));
            ASSERT(0 == Util::isConformant("f",         EDef));
        }

        {
            if (veryVerbose) cout << "Testing Scalar Ints & Strings\n"
                                     "=============================\n";

            const char *nameVec[100];
            memset(nameVec, 0, sizeof(nameVec));
            nameVec[18] = "eighteen";
            nameVec[27] = "twenty seven";
            nameVec[76] = "seventy six";
            nameVec[81] = "eighty one";

            const char *bogusNames[] = { "woof", "meow", "arf", "ruff" };
            enum { NUM_BOGUS_NAMES = sizeof bogusNames / sizeof *bogusNames };

            bdem_EnumerationDef& eDef = *mS.createEnumeration();
            for (int i = 0; i < 100; ++i) {
                if (nameVec[i]) {
                    eDef.addEnumerator(nameVec[i], i);
                }
            }
            const bdem_EnumerationDef& EDef = eDef;
            ASSERT(4 == EDef.numEnumerators());

            for (int i = 0; i < 100; ++i) {
                ASSERT(!!nameVec[i] == Util::isConformant(i, EDef));
                if (nameVec[i]) {
                    ASSERT(1 == Util::isConformant(nameVec[i], EDef));
                }
            }
            for (int i = 0; i < NUM_BOGUS_NAMES; ++i) {
                ASSERT(0 == Util::isConformant(bogusNames[i], EDef));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GG()
        //
        // Concerns:
        //   That the gg() function works properly.
        //
        // Plan:
        //   Construct some enumerations using gg() and verify that they are
        //   built as intended.
        // --------------------------------------------------------------------

        if (verbose) cout << "gg() Test\n"
                             "=========\n";

        {
            if (veryVerbose) cout << "ggInt() Test\n"
                                     "============\n";

            {
                vector<int> empty;
                const vector <int>& EMPTY = ggInt(&empty, "");

                ASSERT(&empty == &EMPTY);
                ASSERT(EMPTY.size() == 0);
            }

            {
                vector<int> v479;
                const vector<int>& V479 = ggInt(&v479, "4 7 9");

                ASSERT(&v479 == &V479);
                ASSERT(V479.size() == 3);
                ASSERT(4 == V479[0]);
                ASSERT(7 == V479[1]);
                ASSERT(9 == V479[2]);
            }

            {
                vector<int> vBig;
                const vector<int>& VBig = ggInt(&vBig, "36475 * -347238 *");

                ASSERT(&vBig == &VBig);
                ASSERT(VBig.size() == 4);
                ASSERT(36475    == VBig[0]);
                ASSERT(unsetInt == VBig[1]);
                ASSERT(-347238  == VBig[2]);
                ASSERT(unsetInt == VBig[3]);
            }
        }

        {
            if (veryVerbose) cout << "ggString() Test\n"
                                     "===============\n";

            {
                vector<string> empty;
                const vector<string>& EMPTY = ggString(&empty, "");

                ASSERT(&empty == &EMPTY);
                ASSERT(EMPTY.size() == 0);
            }

            {
                vector<string> vAbc;
                const vector<string>& VAbc = ggString(&vAbc, "a * c");

                ASSERT(&vAbc == &VAbc);
                ASSERT(VAbc.size() == 3);
                ASSERT("a"         == VAbc[0]);
                ASSERT(unsetString == VAbc[1]);
                ASSERT("c"         == VAbc[2]);
            }

            {
                vector<string> vBigStrs;
                const vector<string>& VBigStrs = ggString(&vBigStrs,
                                                "Eat noodles at Chef Chau's.");
                ASSERT(&vBigStrs == &VBigStrs);
                ASSERT(5 == VBigStrs.size());
                ASSERT("Eat"     == VBigStrs[0]);
                ASSERT("noodles" == VBigStrs[1]);
                ASSERT("at"      == VBigStrs[2]);
                ASSERT("Chef"    == VBigStrs[3]);
                ASSERT("Chau's." == VBigStrs[4]);
            }
        }

        {
            if (veryVerbose) cout << "ggEnum() Test\n"
                                     "=============\n";

            bdem_Schema mS(Z);     const bdem_Schema& S = mS;

            {
                bdem_EnumerationDef& empty = *mS.createEnumeration();
                const bdem_EnumerationDef& EMPTY = ggEnum(&empty, "");

                ASSERT(&empty == &EMPTY);
                ASSERT(0 == EMPTY.numEnumerators());
            }

            {
                bdem_EnumerationDef& eAbc = *mS.createEnumeration();
                const bdem_EnumerationDef& EAbc = ggEnum(&eAbc, "a 1 b 2 c 3");

                ASSERT(&eAbc == &EAbc);
                ASSERT(3 == EAbc.numEnumerators());

                ASSERT(!strcmp("a", EAbc.lookupName(1)));
                ASSERT(!strcmp("b", EAbc.lookupName(2)));
                ASSERT(!strcmp("c", EAbc.lookupName(3)));

                ASSERT(!strcmp("a", EAbc.lookupName("a")));
                ASSERT(!strcmp("b", EAbc.lookupName("b")));
                ASSERT(!strcmp("c", EAbc.lookupName("c")));
            }

            {
                bdem_EnumerationDef& eBig = *mS.createEnumeration();
                const bdem_EnumerationDef& EBig = ggEnum(&eBig,
                        "Macy's 230000 WalMart 1000000 CircuitCity -100000");

                ASSERT(&eBig == &EBig);
                ASSERT(3 == EBig.numEnumerators());

                ASSERT(!strcmp("Macy's",      EBig.lookupName(230000)));
                ASSERT(!strcmp("WalMart",     EBig.lookupName(1000000)));
                ASSERT(!strcmp("CircuitCity", EBig.lookupName(-100000)));

                ASSERT(!strcmp("Macy's",      EBig.lookupName("Macy's")));
                ASSERT(!strcmp("WalMart",     EBig.lookupName("WalMart")));
                ASSERT(!strcmp("CircuitCity", EBig.lookupName("CircuitCity")));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTANTS TEST
        //
        // Concerns:
        //   That an unset 'int' is equal to 'INT_MIN', since code in this
        //   component depends on that.
        // --------------------------------------------------------------------

        if (verbose) cout << "Constants Test\n"
                             "==============\n";

        // Util::isConformant(int, const bdem_Enumeration&) uses INT_MIN for
        // efficiency when it really means 'bdetu_Unset<int>::unsetValue()',
        // verify here that they have the same value.
        ASSERT(bdetu_Unset<int>::unsetValue() == INT_MIN);

        // Util::isConformant(string, const bdem_Enumeration&) uses ""
        // for the unset string while it really means
        // 'bdetu_Unset<string>::unsetValue()', verify here that they have the
        // same value.
        ASSERT(bdetu_Unset<string>::unsetValue() == "");
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    else {
        if (verbose) cout << "(success)" << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
