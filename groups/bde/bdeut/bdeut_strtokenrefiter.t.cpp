// bdeut_strtokenrefiter.t.cpp                                        -*-C++-*-

#include <bdeut_strtokenrefiter.h>

#include <bdeut_stringref.h>

#include <bsl_cstdlib.h>                  // 'bsl::atoi()'
#include <bsl_cstring.h>                  // 'bsl::memcpy', 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_strstream.h>                // 'bsl::ostrstream'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
//-----------------------------------------------------------------------------
// [ 3] bdeut_StrTokenRefIter(const char*, const char*);
// [ 4] bdeut_StrTokenRefIter(const char*, const char*, const char*);
// [ 3] bdeut_StrTokenRefIter(const StringRef&, const char*);
// [ 4] bdeut_StrTokenRefIter(const StringRef&, const char*, const char*);
// [  ] ~bdeut_StrTokenRefIter();
// [ 5] void operator++();
// [ 6] void reset(const char *input);
// [ 6] void reset(const StringRef& input);
// [ 1] operator const void *() const;
// [ 1] bdeut_StringRef operator()() const;
// [ 1] bdeut_StringRef delimiter() const;
// [ 1] bdeut_StringRef previousDelimiter() const;
// [ 1] bool isHard() const;
// [ 1] bool isPreviousHard() const;
// [ 1] bool hasSoft() const;
// [ 1] bool hasPreviousSoft() const;
// ----------------------------------------------------------------------------
// [ 2] char *stringRepeat(int reps, const char *string)
// [ 7] USAGE EXAMPLE

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdeut_StrTokenRefIter Obj;
typedef bdeut_StringRef       Ref;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
void showIter(const Obj& I)
{
    bsl::cout << "Prev. @@" << I.previousDelimiter() << "@@" << bsl::endl;
    bsl::cout << "Token ##" << I()                   << "##" << bsl::endl;
    bsl::cout << "Delim %%" << I.delimiter()         << "%%" << bsl::endl;
}

static
char *stringRepeat(int reps, const char *string)
{
    int len = bsl::strlen(string);
    if (0 == reps || 0 == len) return 0;
    char *str = (char *) new char[reps * len + 1];
    for (int i = 0; i < reps; ++i) {
        bsl::memcpy(str + (i * len), string, len);
    }
    str[reps * len] = '\0';
    return str;
}

// usage example functions
static
void listTokens(const char *input, const char *delims, bool isVerbose)
{
    for (Obj mI(input, delims); mI; ++mI) {
        if (isVerbose) cout << mI() << ':';
    }
    if (isVerbose) cout << endl;
}

static
int parseDate(const char *dateString, bool isVerbose)
{
    int numFields = 0;
    for (Obj mI(dateString, " \t", "/"); mI; ++mI) {
        ++numFields;
        if (isVerbose) cout << mI() << ':';
    }
    if (isVerbose) cout << endl;
    return numFields;
}

static
void reconstruct(const char *input,
                 const char *softDelims,
                 const char *hardDelims)
{
    bsl::ostrstream ostr;
    Obj mI(input, softDelims, hardDelims);  const Obj& I = mI;
    ostr << I.previousDelimiter();   // only once per string
    for (; I; ++mI) {
        ostr << I();                 // current token
        ostr << I.delimiter();       // current (trailing) delimiter
    }
    ostr << ends;
    ASSERT(0 == bsl::strcmp(ostr.str(), input));
    ostr.rdbuf()->freeze(0);         // return ownership of str to ostr
}

static
int checkDateFormat(const char *input)
{
    enum { GOOD = 0, BAD = 1 };
    Obj mI(input, " \t", "/");  const Obj& I = mI;
                                     // generate iterator outside 'for' loop
    for (; I; ++mI) {
        if (!I().length()) return BAD;       // empty token
    }
    return I.isHard() ? BAD : GOOD;  // check last delimiter buffer only
}

static
int restrictiveCheckDateFormat(const char *input)
{
    enum { GOOD = 0, BAD = 1 };
    Obj mI(input, " \t", "/");  const Obj& I = mI;
                                     // generate iterator outside 'for' loop
    if (I.previousDelimiter().length()) return BAD;
    for (; I; ++mI) {
        if (!I().length()) return BAD;       // empty token
    }
    return I.delimiter().length() ? BAD : GOOD;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose modes, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        const char *softDelims = " \t\n";
        const char *hardDelims = "/!";

        bdeut_StrTokenRefIter it("", softDelims, hardDelims);

        if (veryVerbose) cout << "softDelims: " << *softDelims << endl;
        if (veryVerbose) cout << "hardDelims: " << *hardDelims << endl;

        if (veryVerbose) cout << "\t\tTesting simple example" << endl;
        {
            it.reset("\t These/are /  /\nthe   good-old  / \tdays!! \n");
            ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "\t "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(0 == it.isPreviousHard());
            ASSERT(1 == (it() == "These"));
            ASSERT(1 == (it.delimiter() == "/"));
            ASSERT(0 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "/"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(1 == (it() == "are"));
            ASSERT(1 == (it.delimiter() == " /  "));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == " /  "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(1 == (it() == ""));
            ASSERT(1 == (it.delimiter() == "/\n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "/\n"));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(1 == (it() == "the"));
            ASSERT(1 == (it.delimiter() == "   "));
            ASSERT(1 == it.hasSoft());
            ASSERT(0 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "   "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(0 == it.isPreviousHard());
            ASSERT(1 == (it() == "good-old"));
            ASSERT(1 == (it.delimiter() == "  / \t"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "  / \t"));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(1 == (it() == "days"));
            ASSERT(1 == (it.delimiter() == "!"));
            ASSERT(0 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(1 == (it.previousDelimiter() == "!"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(1 == (it() == ""));
            ASSERT(1 == (it.delimiter() == "! \n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(!it);
            ASSERT(1 == (it.previousDelimiter() == "!"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            // ASSERT(1 == (it() == ""));
            ASSERT(1 == (it.delimiter() == "! \n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());
        }

        if (veryVerbose) cout << "\t\tTesting 'listTokens' example" << endl;
        {
            listTokens("\n Now\t\tis the time... ", softDelims, veryVerbose);
        }

        if (veryVerbose) cout << "\t\tTesting 'parseDate' example" << endl;
        {
            ASSERT(3 == parseDate("3/8/59", veryVerbose));
            ASSERT(3 == parseDate("3  8/59", veryVerbose));
            ASSERT(3 == parseDate("3 / 8 / 59", veryVerbose));
            ASSERT(3 == parseDate("\t\t  3\t\t8  /\t59 \t\t  ", veryVerbose));
            ASSERT(3 == parseDate("3/8/59/", veryVerbose));
            ASSERT(4 == parseDate("3//8/59", veryVerbose));
            ASSERT(4 == parseDate("/3/8/59", veryVerbose));
            ASSERT(4 == parseDate("3 8 / / 59", veryVerbose));
            ASSERT(4 == parseDate("3 8 59//", veryVerbose));
        }

        if (veryVerbose) cout << "\t\tTesting 'reconstruct' example" << endl;
        {
            reconstruct("?This is // only :! atest\n\n", softDelims,
                                                         hardDelims);
        }

        if (veryVerbose) cout << "\t\tTesting 'checkDateFormat' example"
                              << endl;
        {
            ASSERT(0 == checkDateFormat("3/8/59"));
            ASSERT(1 == checkDateFormat("3/8/59/"));
        }

        if (veryVerbose) cout <<
                    "\t\tTesting 'restrictiveCheckDateFormat' example" << endl;
        {
            ASSERT(0 == restrictiveCheckDateFormat("3/8/59"));  // ok
            ASSERT(1 == restrictiveCheckDateFormat(" 3/8/59")); // leading
            ASSERT(1 == restrictiveCheckDateFormat("/3/8/59")); // leading
            ASSERT(1 == restrictiveCheckDateFormat("3/8/59/")); // trailing
            ASSERT(1 == restrictiveCheckDateFormat("3/8/59 ")); // trailing
        }

        if (veryVerbose) cout << "\t\tTesting nested iterator example" << endl;
        {
            const char *string = "The quick brown fox\n"
                                 "jumped over the\n"
                                 "small lazy dog.\n";

            for (bdeut_StrTokenRefIter it1(string, "\n"); it1; ++it1) {
                if (veryVerbose) cout << '@';
                for (bdeut_StrTokenRefIter it2(it1(), " "); it2; ++it2) {
                    if (veryVerbose) cout << it2() << ':';
                }
            }
            if (veryVerbose) cout << endl;
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Testing:
        //   void reset(const char *input);
        //   void reset(const StringRef& input);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'reset' methods"
                          << "\n=======================" << endl;

        const char *softDelims = " ";
        const char *hardDelims = "@";

        if (veryVerbose) cout << "softDelims: " << *softDelims << endl;
        if (veryVerbose) cout << "hardDelims: " << *hardDelims << endl;

        if (veryVerbose) cout << "\tTesting 'void reset(const char *input)'."
                              << endl;
        {
            Obj mI("", softDelims, hardDelims);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset(" ");  ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting 'previousDelimiter()'"
                                  << endl;
            mI.reset("  ");  ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == "  "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset(" @");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset(" x ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("  x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "  "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset(" xx");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset(" @x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset(" @ ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset(" @@");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting token" << endl;
            mI.reset("x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("xx");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("x x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("xx ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("x  ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("x@ ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x@x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x@@");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x  x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("x @x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x@ x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x@@x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x @ x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("x @ @");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting delimiter" << endl;
            mI.reset("@");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@ ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@@");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@  ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@ x");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@ @");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@x ");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@xx");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            mI.reset("@x@");  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting reset in middle of string"
                                  << endl;
            {
                const char *str  = ", .Now ,, is .? the ??time ..for ,men \n";
                const char *str2 = ",To :?: come   ,,to";

                const char *softDelim = " ,.\t\n";
                const char *hardDelim = ":?";

                Obj mI("", softDelim, hardDelim);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                if (veryVerbose) cout << "\t\tTesting reset with null" << endl;
                {
                    mI.reset(str);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " ,, "));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "is"));
                    ASSERT(1 == (I.delimiter() == " .? "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " .? "));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == "the"));
                    ASSERT(1 == (I.delimiter() == " ?"));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " ?"));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == "?"));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    mI.reset(0);  ASSERT(!I);
                    ASSERT(1 == (I.previousDelimiter() == ""));
                    ASSERT(0 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    // ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == ""));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(0 == I.isHard());
                }

                if (veryVerbose) cout << "\t\tTesting reset with empty"
                                      << endl;
                {
                    mI.reset(str);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);

                    mI.reset("");  ASSERT(!I);
                    ASSERT(1 == (I.previousDelimiter() == ""));
                    ASSERT(0 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    // ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == ""));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(0 == I.isHard());
                }

                if (veryVerbose) cout << "\t\tTesting reset with null"
                                      << endl;
                {
                    mI.reset(str);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);

                    mI.reset(str2);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ","));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "To"));
                    ASSERT(1 == (I.delimiter() == " :"));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " :"));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == "?"));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(1 == I.isHard());
                }
            }
        }

        if (veryVerbose)
            cout << "\tTesting 'void reset(const StringRef& input)'." << endl;
        {
            char buffer[128];
            bsl::memset(buffer, '#', sizeof buffer);

            Ref mR(buffer, 0);  const Ref& R = mR;

            Obj mI(R, softDelims, hardDelims);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " ", 1);
            mR.assign(buffer, 1);

            mI.reset(R);  ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting 'previousDelimiter()'"
                                  << endl;

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "  ", 2);
            mR.assign(buffer, 2);

            mI.reset(R);  ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == "  "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());


            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " @", 2);
            mR.assign(buffer, 2);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());


            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " x ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());


            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "  x", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "  "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());


            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " xx", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " @x", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " ", 1);
            mR.assign(buffer, 1);

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " @ ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, " @@", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting token" << endl;

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x", 1);
            mR.assign(buffer, 1);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "xx", 2);
            mR.assign(buffer, 2);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x x", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "xx ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "xx"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x  ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x@ ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x@x", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x@@", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x  x", 4);
            mR.assign(buffer, 4);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x @x", 4);
            mR.assign(buffer, 4);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x@ x", 4);
            mR.assign(buffer, 4);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x@@x", 4);
            mR.assign(buffer, 4);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x @ x", 5);
            mR.assign(buffer, 5);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "x @ @", 5);
            mR.assign(buffer, 5);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "x"));
            ASSERT(1 == (I.delimiter() == " @ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting delimiter" << endl;

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@", 1);
            mR.assign(buffer, 1);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@ ", 2);
            mR.assign(buffer, 2);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@@", 2);
            mR.assign(buffer, 2);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@  ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@  "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@ x", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@ @", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@ "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@x ", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@xx", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, "@x@", 3);
            mR.assign(buffer, 3);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "@"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            if (veryVerbose) cout << "\t\tTesting reset in middle of string"
                                  << endl;
            {
                const char *str  = ", .Now ,, is .? the ??time ..for ,men \n";
                const char *str2 = ",To :?: come   ,,to";

                const char *softDelim = " ,.\t\n";
                const char *hardDelim = ":?";

                Obj mI("", softDelim, hardDelim);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                if (veryVerbose) cout << "\t\tTesting reset with null" << endl;
                {
                    bsl::memset(buffer, '#', sizeof buffer);
                    bsl::strncpy(buffer, str, bsl::strlen(str));
                    mR.assign(buffer, bsl::strlen(str));

                    mI.reset(R);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " ,, "));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "is"));
                    ASSERT(1 == (I.delimiter() == " .? "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " .? "));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == "the"));
                    ASSERT(1 == (I.delimiter() == " ?"));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " ?"));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == "?"));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    mR.assign(0);

                    mI.reset(R);  ASSERT(!I);
                    ASSERT(1 == (I.previousDelimiter() == ""));
                    ASSERT(0 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    // ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == ""));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(0 == I.isHard());
                }

                if (veryVerbose) cout << "\t\tTesting reset with empty"
                                      << endl;
                {
                    bsl::memset(buffer, '#', sizeof buffer);
                    bsl::strncpy(buffer, str, bsl::strlen(str));
                    mR.assign(buffer, bsl::strlen(str));

                    mI.reset(R);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);

                    bsl::memset(buffer, '#', sizeof buffer);
                    mR.assign(buffer, 0);

                    mI.reset(R);  ASSERT(!I);
                    ASSERT(1 == (I.previousDelimiter() == ""));
                    ASSERT(0 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    // ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == ""));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(0 == I.isHard());
                }

                if (veryVerbose) cout << "\t\tTesting reset with null" << endl;
                {
                    bsl::memset(buffer, '#', sizeof buffer);
                    bsl::strncpy(buffer, str, bsl::strlen(str));
                    mR.assign(buffer, bsl::strlen(str));

                    mI.reset(R);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ", ."));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "Now"));
                    ASSERT(1 == (I.delimiter() == " ,, "));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(0 == I.isHard());

                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);
                    ++mI;  ASSERT(I);

                    bsl::memset(buffer, '#', sizeof buffer);
                    bsl::strncpy(buffer, str2, bsl::strlen(str2));
                    mR.assign(buffer, bsl::strlen(str2));

                    mI.reset(R);  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == ","));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(0 == I.isPreviousHard());
                    ASSERT(1 == (I() == "To"));
                    ASSERT(1 == (I.delimiter() == " :"));
                    ASSERT(1 == I.hasSoft());
                    ASSERT(1 == I.isHard());

                    ++mI;  ASSERT(I);
                    ASSERT(1 == (I.previousDelimiter() == " :"));
                    ASSERT(1 == I.hasPreviousSoft());
                    ASSERT(1 == I.isPreviousHard());
                    ASSERT(1 == (I() == ""));
                    ASSERT(1 == (I.delimiter() == "?"));
                    ASSERT(0 == I.hasSoft());
                    ASSERT(1 == I.isHard());
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Testing:
        //    void operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting void operator++()"
                          << "\n=========================" << endl;

        char all[256];
        int i;  // loop index
        for (i = 0; i < 255; ++i) {
            all[i] = i + 1;
        }
        all[255] = 0;

        if (veryVerbose) cout <<
                "\t\tTesting case where all chars are soft delimiters" << endl;
        {
            Obj mI(all, all);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == all));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        if (veryVerbose) cout <<
                "\t\tTesting case where all chars are hard delimiters" << endl;
        {
            Obj mI(all, 0, all);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            for (i = 0; i < 254; ++i) {
                LOOP_ASSERT(i, 1 == (I() == ""));

                LOOP_ASSERT(i, all[i] == I.delimiter()[0]);
                LOOP_ASSERT(i, 1 == I.isHard());
                ++mI;  LOOP_ASSERT(i, I);
                LOOP_ASSERT(i, all[i] == I.previousDelimiter()[0]);
                LOOP_ASSERT(i, 1 == I.isPreviousHard());
            }
            ++mI;  ASSERT(!I);
            ASSERT(all[253] == I.previousDelimiter()[0]);
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(all[254] == I.delimiter()[0]);
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
        }

        if (veryVerbose) cout << "Testing case where no chars are delimiters"
                              << endl;
        {
            Obj mI(all, 0);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == all));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;  ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == all));
            ASSERT(1 == (I.delimiter() == ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        if (veryVerbose) cout << "Testing nested iterators" << endl;
        {
            const char *string = "abcdefghijklmnopqrstuvwxyz"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "abcdefghijklmnopqrstuvwxyz\n";
            char *finalString = stringRepeat(10, string);
            for (Obj mI(finalString, "\n"); mI; ++mI) {
                for (Obj czIt(mI(), "z"); czIt; ++czIt) {
                    ASSERT(1 == (czIt() == "abcdefghijklmnopqrstuvwxy"));
                }
            }
            delete [] finalString;
        }

        if (veryVerbose) cout << "Testing long strings" << endl;
        {
            const char *string = "abcdefghijklmnopqrstuvwxyz";
            char *str = stringRepeat(10000, string);
            for (Obj czIt(str, "z"); czIt; ++czIt) {
                ASSERT(1 == (czIt() == "abcdefghijklmnopqrstuvwxy"));
            }
            delete [] str;
        }

        if (veryVerbose) cout << "Testing duplicate delimiters" << endl;
        {
            const char *string = "abcdefghijklmnopqrstuvwxyz";
            char *str = stringRepeat(5, string);
            for (Obj czIt(str, "zzzzzzzzz"); czIt; ++czIt) {
                ASSERT(1 == (czIt() == "abcdefghijklmnopqrstuvwxy"));
            }
            delete [] str;
        }

        const char *str = "The   lazy   ,,, ,dog";

        if (veryVerbose) cout << "\t\tTesting soft delimiter separators"
                              << endl;
        {
            Obj mI(str, " ,");  const Obj& I = mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "The"));
            ASSERT(1 == (I.delimiter() == "   "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "   "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "lazy"));
            ASSERT(1 == (I.delimiter() == "   ,,, ,"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        if (veryVerbose) cout << "\t\tTesting hard delimiter separators"
                              << endl;
        {
            Obj mI(str, " ", ",");  const Obj& I = mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "The"));
            ASSERT(1 == (I.delimiter() == "   "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "   "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "lazy"));
            ASSERT(1 == (I.delimiter() == "   ,"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
        }

        if (veryVerbose) cout << "\t\tTesting mix of soft and " <<
                                 "hard delimiter separators"    << endl;
        {
            const char *str  = "The lazy,: dog";
            const char *str1 = "The lazy:, dog";
            const char *str2 = "The::lazy:, dog";
            const char *soft = " ,";
            const char *hard = ":";

            Obj mI (str , soft, hard);  const Obj& I  = mI;
            Obj mI1(str1, soft, hard);  const Obj& I1 = mI1;
            Obj mI2(str2, soft, hard);  const Obj& I2 = mI2;

            ASSERT(I);   ++mI;   ASSERT(I);
            ASSERT(I1);  ++mI1;  ASSERT(I1);
            ASSERT(I2);  ++mI2;  ASSERT(I2);

            ASSERT(1 == (I.previousDelimiter()  == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I1.previousDelimiter() == " "));
            ASSERT(1 == I1.hasPreviousSoft());
            ASSERT(0 == I1.isPreviousHard());
            ASSERT(1 == (I2.previousDelimiter() == ":"));
            ASSERT(0 == I2.hasPreviousSoft());
            ASSERT(1 == I2.isPreviousHard());

            ASSERT(1 == (I()  == "lazy"));
            ASSERT(1 == (I1() == "lazy"));
            ASSERT(1 == (I2() == ""));

            ASSERT(1 == (I.delimiter()  == ",: "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            ASSERT(1 == (I1.delimiter() == ":, "));
            ASSERT(1 == I1.hasSoft());
            ASSERT(1 == I1.isHard());
            ASSERT(1 == (I2.delimiter() == ":"));
            ASSERT(0 == I2.hasSoft());
            ASSERT(1 == I2.isHard());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Testing:
        //   bdeut_StrTokenRefIter(const char*, const char*, const char*);
        //   bdeut_StrTokenRefIter(const StringRef&, const char*, const char*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting three-argument constructors"
                          << "\n==================================="  << endl;

        const char *softDelims = " ,.\t\n";
        const char *hardDelims = ":?";
        const char *str        = ", .The   \tlazy::fox ?\n";

        if (veryVerbose) cout <<  "\t\tString\t| softDelims\t| hardDelims"
                              << endl;
        if (veryVerbose) cout <<  "\t\t------\t| ----------\t| ----------"
                              << endl;

        if (veryVerbose) cout <<
            "\tTesting 'bdeut_StrTokenRefIter(const char*, "
                                             "const char*, const char*)'."
                              << endl;
        {
            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            {
                Obj mI(0, 0, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     \"\"" << endl;
            {
                Obj mI(0, 0, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI(0, 0, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            {
                Obj mI(0, "", 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     \"\"" << endl;
            {
                Obj mI(0, "", "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI(0, "", hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            {
                Obj mI(0, softDelims, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI(0, softDelims, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI(0, softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            {
                Obj mI("", 0, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     \"\""
                                  << endl;
            {
                Obj mI("", 0, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI("", 0, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            {
                Obj mI("", "", 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     \"\""
                                  << endl;
            {
                Obj mI("", "", "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<"\t\t  \"\"\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI("", "", hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI("", softDelims, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI("", softDelims, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<"\t\t  \"\"\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI("", softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            {
                Obj mI(str, 0, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     \"\""
                                  << endl;
            {
                Obj mI(str, 0, "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI(str, 0, hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ", .The   \tlazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ":"));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            {
                Obj mI(str, "", 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     \"\""
                                  << endl;
            {
                Obj mI(str, "", "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI(str, "", hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ", .The   \tlazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ":"));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(str, softDelims, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI(str, softDelims, "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI(str, softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            char all[256];
            for (int i = 0; i < 255; ++i) {
                all[i] = i + 1;
            }
            all[255] = 0;

            if (veryVerbose) cout <<
                "\t\tTesting case where all chars are hard delimiters" << endl;
            {
                Obj mI(all, 0, all);  const Obj& I = mI;
                ASSERT(I);

                if (veryVerbose) showIter(I);

                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == "\01"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }
        }

        if (veryVerbose) cout <<
            "\tTesting 'bdeut_StrTokenRefIter(const StringRef&, "
                                             "const char*, const char*)'."
                              << endl;
        {
            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(0), 0, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     \"\"" << endl;
            {
                Obj mI(Ref(0), 0, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(0), 0, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(0), "", 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     \"\"" << endl;
            {
                Obj mI(Ref(0), "", "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(0), "", hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            {
                Obj mI(Ref(0), softDelims, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(0), softDelims, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(0), softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(""), 0, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(""), 0, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(""), 0, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(""), "", 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(""), "", "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<"\t\t  \"\"\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(""), "", hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(Ref(""), softDelims, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(""), softDelims, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<"\t\t  \"\"\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(""), softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(str), 0, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(str), 0, "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(str), 0, hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ", .The   \tlazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ":"));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(str), "", 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(str), "", "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(str), "", hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ", .The   \tlazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ":"));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(Ref(str), softDelims, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     \"\""
                                  << endl;
            {
                Obj mI(Ref(str), softDelims, "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t| hardDelims"
                                  << endl;
            {
                Obj mI(Ref(str), softDelims, hardDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy"));
                ASSERT(1 == (I.delimiter() == ":"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }

            char all[256];
            for (int i = 0; i < 255; ++i) {
                all[i] = i + 1;
            }
            all[255] = 0;

            if (veryVerbose) cout <<
                "\t\tTesting case where all chars are hard delimiters" << endl;
            {
                Obj mI(Ref(all), 0, all);  const Obj& I = mI;
                ASSERT(I);

                if (veryVerbose) showIter(I);

                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == "\01"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing:
        //   bdeut_StrTokenRefIter(const char*, const char*);
        //   bdeut_StrTokenRefIter(const StringRef&, const char*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting two-argument constructors"
                          << "\n================================="  << endl;

        const char *softDelims = " ,.\t\n";
        const char *str        = ", .The   \tlazy::fox ?\n";

        if (veryVerbose) cout <<  "\t\tString\t| softDelims\t| hardDelims"
                              << endl;
        if (veryVerbose) cout <<  "\t\t------\t| ----------\t| ----------"
                              << endl;

        if (veryVerbose) cout <<
            "\tTesting 'bdeut_StrTokenRefIter(const char*, const char*)'."
                              << endl;
        {
            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            {
                Obj mI(0, 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            {
                Obj mI(0, "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            {
                Obj mI(0, softDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            {
                Obj mI("", 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            {
                Obj mI("", "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI("", softDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            {
                Obj mI(str, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            {
                Obj mI(str, "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(str, softDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            char all[256];
            for (int i = 0; i < 255; ++i) {
                all[i] = i + 1;
            }
            all[255] = 0;

            if (veryVerbose) cout <<
                "\t\tTesting case where all chars are soft delimiters" << endl;
            {
                Obj mI(all, all);  const Obj& I = mI;
                ASSERT(!I);

                if (veryVerbose) showIter(I);

                ASSERT(1 == (I.previousDelimiter() == all));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<
                      "\t\tTesting case where no chars are delimiters" << endl;
            {
                Obj mI(all, 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == all));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }
        }

        if (veryVerbose) cout <<
            "\tTesting 'bdeut_StrTokenRefIter(const StringRef&, const char*)'."
                              << endl;
        {
            if (veryVerbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(0), 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(0), "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            {
                Obj mI(Ref(0), softDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(""), 0);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(""), "");  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  \"\"\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(Ref(""), softDelims);  const Obj& I = mI;
                ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            {
                Obj mI(Ref(str), 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            {
                Obj mI(Ref(str), "");  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(!I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == str));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout << "\t\t  str\t| softDelims\t|     0"
                                  << endl;
            {
                Obj mI(Ref(str), softDelims);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "The"));
                ASSERT(1 == (I.delimiter() == "   \t"));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());

                ++mI;  ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == "   \t"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == "lazy::fox"));
                ASSERT(1 == (I.delimiter() == " "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            char all[256];
            for (int i = 0; i < 255; ++i) {
                all[i] = i + 1;
            }
            all[255] = 0;

            if (veryVerbose) cout <<
                "\t\tTesting case where all chars are soft delimiters" << endl;
            {
                Obj mI(Ref(all), all);  const Obj& I = mI;
                ASSERT(!I);

                if (veryVerbose) showIter(I);

                ASSERT(1 == (I.previousDelimiter() == all));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                // ASSERT(1 == (I() == ""));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }

            if (veryVerbose) cout <<
                      "\t\tTesting case where no chars are delimiters" << endl;
            {
                Obj mI(Ref(all), 0);  const Obj& I = mI;
                ASSERT(I);
                ASSERT(1 == (I.previousDelimiter() == ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(1 == (I() == all));
                ASSERT(1 == (I.delimiter() == ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing:
        //    char *stringRepeat(int reps, const char *string);
        // --------------------------------------------------------------------

        if (verbose)
        cout << "\nTesting 'char *stringRepeat(int reps, const char *string)'"
             << "\n=========================================================="
             << endl;

        int i;
        char all[128];
        for (i = 0; i < 128; ++i) {
            all[i] = i + 1;
        }
        all[127] = '\0';

        {
            char *str = stringRepeat(2, all);
            if (veryVerbose) cout << str << endl;
            for (i = 0; i < 127; ++i) {
                LOOP_ASSERT(i, *(str + i) == all[i % 128]);
            }
            ASSERT('\0' == all[127]);
            for (i = 128; i < 254; ++i) {
                LOOP_ASSERT(i, str[i] == all[(i + 1) % 128]);
            }
            delete [] str;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Breathing test
        //
        // Plan:
        //   Create a 'StrTokenRefIter(const char*, const char*, const char*)'
        //   and verify basic correctness.  Repeat for the three-argument
        //   constructor that takes a 'const bdeut_StringRef&'.
        //
        // Testing:
        //   operator const void *() const;
        //   bdeut_StringRef operator()() const;
        //   bdeut_StringRef delimiter() const;
        //   bdeut_StringRef previousDelimiter() const;
        //   bool isHard() const;
        //   bool isPreviousHard() const;
        //   bool hasSoft() const;
        //   bool hasPreviousSoft() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBreathing Test"
                          << "\n==============" << endl;

        const char *string = ", .Now ,, is .? the ??time ..for all good,men \n"
                             "to,come,to,the    aid,of    their,country.\n";

        const char *softDelim = " ,.\t\n";
        const char *hardDelim = ":?";

        const char *resetString = "??The,.\nlazy ? ?    ? ";

        {
            Obj mI(string, softDelim, hardDelim);  const Obj& I = mI;
            ASSERT(I);

            if (veryVerbose) showIter(I);

            ASSERT(1 == (I.previousDelimiter() == ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "Now"));
            ASSERT(1 == (I.delimiter() == " ,, "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ,, "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "is"));
            ASSERT(1 == (I.delimiter() == " .? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " .? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "the"));
            ASSERT(1 == (I.delimiter() == " ?"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ?"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "time"));
            ASSERT(1 == (I.delimiter() == " .."));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " .."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "for"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "all"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "good"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "men"));
            ASSERT(1 == (I.delimiter() == " \n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " \n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "to"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "come"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "to"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "the"));
            ASSERT(1 == (I.delimiter() == "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "aid"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "of"));
            ASSERT(1 == (I.delimiter() == "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "their"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "country"));
            ASSERT(1 == (I.delimiter() == ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == "country"));
            ASSERT(1 == (I.delimiter() == ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset(resetString);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "The"));
            ASSERT(1 == (I.delimiter() == ",.\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ",.\n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "lazy"));
            ASSERT(1 == (I.delimiter() == " ? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
        }

        {
            const int len = bsl::strlen(string);
            char buffer[256];  ASSERT((int)sizeof buffer > len);
            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, string, len);

            Ref mR(buffer, len);  const Ref& R = mR;

            Obj mI(R, softDelim, hardDelim);  const Obj& I = mI;
            ASSERT(I);

            if (veryVerbose) showIter(I);

            ASSERT(1 == (I.previousDelimiter() == ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "Now"));
            ASSERT(1 == (I.delimiter() == " ,, "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ,, "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "is"));
            ASSERT(1 == (I.delimiter() == " .? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " .? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "the"));
            ASSERT(1 == (I.delimiter() == " ?"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ?"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "time"));
            ASSERT(1 == (I.delimiter() == " .."));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " .."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "for"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "all"));
            ASSERT(1 == (I.delimiter() == " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "good"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "men"));
            ASSERT(1 == (I.delimiter() == " \n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " \n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "to"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "come"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "to"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "the"));
            ASSERT(1 == (I.delimiter() == "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "aid"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "of"));
            ASSERT(1 == (I.delimiter() == "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "their"));
            ASSERT(1 == (I.delimiter() == ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "country"));
            ASSERT(1 == (I.delimiter() == ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            // ASSERT(1 == (I() == "country"));
            ASSERT(1 == (I.delimiter() == ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            const int resetLen = bsl::strlen(resetString);
            ASSERT((int)sizeof buffer > resetLen);
            bsl::memset(buffer, '#', sizeof buffer);
            bsl::strncpy(buffer, resetString, resetLen);
            mR.assign(buffer, resetLen);

            mI.reset(R);  ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == "The"));
            ASSERT(1 == (I.delimiter() == ",.\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == ",.\n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(1 == (I() == "lazy"));
            ASSERT(1 == (I.delimiter() == " ? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == " ? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "?    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(I);
            ASSERT(1 == (I.previousDelimiter() == "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());

            ++mI;
            ASSERT(!I);
            ASSERT(1 == (I.previousDelimiter() == "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            // ASSERT(1 == (I() == ""));
            ASSERT(1 == (I.delimiter() == "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
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
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
