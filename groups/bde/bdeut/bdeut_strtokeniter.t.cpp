// bdeut_strtokeniter.t.cpp     -*-C++-*-

#include <bdeut_strtokeniter.h>

#include <bdeut_stringref.h>

#include <bslma_allocator.h>        // for testing only
#include <bslma_testallocator.h>    // for testing only

#include <bsls_assert.h>

#include <bsl_cstdlib.h>                  // atoi()
#include <bsl_cstring.h>                  // strcmp
#include <bsl_iostream.h>
#include <bsl_strstream.h>                // ostrstream

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
//-----------------------------------------------------------------------------
// 'bdeut_StrTokenIter' public interface:
// [ 3] bdeut_StrTokenIter(const char*, const char*, ba* = 0);
// [ 4] bdeut_StrTokenIter(const char*, const char*, const char*, ba* = 0);
// [  ] ~bdeut_StrTokenIter();
// [ 5] void operator++();
// [ 6] void reset(const char *input);
// [ 1] operator const void *() const;
// [ 1] const char *operator()() const;
// [ 1] const char *delimiter() const;
// [ 1] const char *previousDelimiter() const;
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

typedef bdeut_StrTokenIter Obj;
typedef bdeut_StringRef    Ref;

//=============================================================================
//                  PARANOID ALLOCATOR FOR MANUAL TESTING
//-----------------------------------------------------------------------------

class donotusethis_FatalAllocator : public bslma_Allocator {
    // This class provides an allocator that emits a message and terminates on
    // 'allocate'.  It is for manual testing only and is of no programmatic use
    // in nightly builds

  private:
    // NOT IMPLEMENTED
    donotusethis_FatalAllocator(const donotusethis_FatalAllocator&);
    donotusethis_FatalAllocator& operator=(const donotusethis_FatalAllocator&);

  public:
    // CREATORS
    donotusethis_FatalAllocator() {}
        // Create a fatalAllocator.

    ~donotusethis_FatalAllocator() {}
        // Destroy this allocator.

    // MANIPULATORS
    void *allocate(size_type size)
        // Print a message to 'stdout' and terminate.
    {
        cout << "Attempting to allocate using the Fatal Allocator" << endl;
        BSLS_ASSERT(0);
        return (void *) this;
    }

    void deallocate(void *address) { }
        // Do nothing.
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static char *stringRepeat(int reps, const char *string)
{
    int len = strlen(string);
    if (0 == reps || 0 == len) return 0;
    char *str = (char *) new char[reps * len + 1];
    for (int i = 0; i < reps; ++i) {
        bsl::memcpy(str + (i*len), string, len);
    }
    str[reps * len] = '\0';
    return str;
}

// usage example functions
static void listTokens(const char *input, const char *delims, bool isVerbose)
{
    for (Obj mI(input, delims); mI; ++mI) {
        if (isVerbose) cout << mI() << ':';
    }
    if (isVerbose) cout << endl;
}

static int parseDate(const char *dateString, bool isVerbose)
{
    int numFields = 0;
    for (Obj mI(dateString, " \t", "/"); mI; ++mI) {
        ++numFields;
        if (isVerbose) cout << mI() << ':';
    }
    if (isVerbose) cout << endl;
    return numFields;
}

static void reconstruct(const char *input,
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

static int checkDateFormat(const char *input)
{
    enum { GOOD = 0, BAD = 1 };
    Obj mI(input, " \t", "/");  const Obj& I = mI;
                                     // generate iterator outside 'for' loop
    for (; I; ++mI) {
        if (!*I()) return BAD;       // empty token
    }
    return I.isHard() ? BAD : GOOD;  // check last delimiter buffer only
}

static int restrictiveCheckDateFormat(const char *input)
{
    enum { GOOD = 0, BAD = 1 };
    Obj mI(input, " \t", "/");  const Obj& I = mI;
                                     // generate iterator outside 'for' loop
    if (*I.previousDelimiter()) return BAD;
    for (; I; ++mI) {
        if (!*I()) return BAD;       // empty token
    }
    return *I.delimiter() ? BAD : GOOD;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

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
        bdeut_StrTokenIter it("", softDelims, hardDelims);

        if (verbose) cout << "softDelims: " << *softDelims << endl;
        if (verbose) cout << "hardDelims: " << *hardDelims << endl;

        {
            if (verbose) cout << "\t\tTesting simple example" << endl;

            it.reset("\t These/are /  /\nthe   good-old  / \tdays!! \n");
            ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "\t "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(0 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), "These"));
            ASSERT(0 == strcmp(it.delimiter(), "/"));
            ASSERT(0 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "/"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), "are"));
            ASSERT(0 == strcmp(it.delimiter(), " /  "));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), " /  "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), ""));
            ASSERT(0 == strcmp(it.delimiter(), "/\n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "/\n"));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), "the"));
            ASSERT(0 == strcmp(it.delimiter(), "   "));
            ASSERT(1 == it.hasSoft());
            ASSERT(0 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "   "));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(0 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), "good-old"));
            ASSERT(0 == strcmp(it.delimiter(), "  / \t"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(),"  / \t"));
            ASSERT(1 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), "days"));
            ASSERT(0 == strcmp(it.delimiter(), "!"));
            ASSERT(0 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "!"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), ""));
            ASSERT(0 == strcmp(it.delimiter(), "! \n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());

            ++it;  ASSERT(!it);
            ASSERT(0 == strcmp(it.previousDelimiter(), "!"));
            ASSERT(0 == it.hasPreviousSoft());
            ASSERT(1 == it.isPreviousHard());
            ASSERT(0 == strcmp(it(), ""));
            ASSERT(0 == strcmp(it.delimiter(), "! \n"));
            ASSERT(1 == it.hasSoft());
            ASSERT(1 == it.isHard());
        }

        {
            if (verbose) cout << "\t\tTesting listTokens example" << endl;
            listTokens("\n Now\t\tis the time... ", softDelims, verbose);
        }

        {
            if (verbose) cout << "\t\tTesting parseDate example" << endl;
            ASSERT(3 == parseDate("3/8/59", verbose));
            ASSERT(3 == parseDate("3  8/59", verbose));
            ASSERT(3 == parseDate("3 / 8 / 59", verbose));
            ASSERT(3 == parseDate("\t\t  3\t\t8  /\t59 \t\t  ", verbose));
            ASSERT(3 == parseDate("3/8/59/", verbose));
            ASSERT(4 == parseDate("3//8/59", verbose));
            ASSERT(4 == parseDate("/3/8/59", verbose));
            ASSERT(4 == parseDate("3 8 / / 59", verbose));
            ASSERT(4 == parseDate("3 8 59//", verbose));
        }

        {
            if (verbose) cout << "\t\tTesting reconstruct example" << endl;
            reconstruct("?This is // only :! atest\n\n", softDelims,
                                                         hardDelims);
        }

        {
            if (verbose) cout << "\t\tTesting checkDateFormat example" << endl;
            ASSERT(0 == checkDateFormat("3/8/59"));
            ASSERT(1 == checkDateFormat("3/8/59/"));
        }

        {
            if (verbose) cout <<
                "\t\tTesting restrictiveCheckDateFormat example" << endl;
            ASSERT(0 == restrictiveCheckDateFormat("3/8/59"));  // ok
            ASSERT(1 == restrictiveCheckDateFormat(" 3/8/59")); // leading
            ASSERT(1 == restrictiveCheckDateFormat("/3/8/59")); // leading
            ASSERT(1 == restrictiveCheckDateFormat("3/8/59/")); // trailing
            ASSERT(1 == restrictiveCheckDateFormat("3/8/59 ")); // trailing
        }

        {
            if (verbose) cout << "\t\tTesting nested iterator example" << endl;
            const char *string = "The quick brown fox\n"
                                 "jumped over the\n"
                                 "small lazy dog.\n";

            for (bdeut_StrTokenIter it1(string, "\n"); it1; ++it1) {
                if (verbose) cout << '@';
                for (bdeut_StrTokenIter it2(it1(), " "); it2; ++it2) {
                    if (verbose) cout << it2() << ':';
                }
            }
            if (verbose) cout << endl;
        }
        if (verbose) cout << "Done. (usage Examples)" << endl;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Testing:
        //    void reset(const char *input);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting void reset(const char *string)"
                          << "\n======================================"
                          << endl;

        Ref mR;  const Ref& R = mR;  // reusable string ref

        const char *softDelims = " ";
        const char *hardDelims = "@";
        Obj mI("", softDelims, hardDelims);  const Obj& I = mI;
        ASSERT(!I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());

        if (verbose) cout << "softDelims: " << *softDelims << endl;
        if (verbose) cout << "hardDelims: " << *hardDelims << endl;

        if (verbose) cout << "\t\tTesting previousDelimiter()" << endl;
        mI.reset("  ");  ASSERT(!I);
        ASSERT(0 == strcmp(I.previousDelimiter(), "  "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());

        mI.reset(" @");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset(" x ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), " "));
        ASSERT(1 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("  x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), "  "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset(" xx");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "xx"));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(2 == R.length());
        ASSERT(0 == strncmp("xx", R.data(), R.length()));

        mI.reset(" @x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset(" @ ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset(" @@");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), " "));
        ASSERT(1 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        if (verbose) cout << "\t\tTesting token" << endl;
        mI.reset("x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("xx");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "xx"));
        ASSERT(0 == strcmp(I.delimiter(), ""));
        ASSERT(0 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(2 == R.length());
        ASSERT(0 == strncmp("xx", R.data(), R.length()));

        mI.reset("x x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), " "));
        ASSERT(1 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("xx ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "xx"));
        ASSERT(0 == strcmp(I.delimiter(), " "));
        ASSERT(1 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(2 == R.length());
        ASSERT(0 == strncmp("xx", R.data(), R.length()));

        mI.reset("x  ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "  "));
        ASSERT(1 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x@ ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "@ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x@x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x@@");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x  x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "  "));
        ASSERT(1 == I.hasSoft());
        ASSERT(0 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x @x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), " @"));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x@ x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "@ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x@@x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x @ x");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), " @ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        mI.reset("x @ @");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), "x"));
        ASSERT(0 == strcmp(I.delimiter(), " @ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(1 == R.length());
        ASSERT(0 == strncmp("x", R.data(), R.length()));

        if (verbose) cout << "\t\tTesting delimiter" << endl;
        mI.reset("@");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@ ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@@");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@  ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@  "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@ @");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@ "));
        ASSERT(1 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@x ");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@xx");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        mI.reset("@x@");  ASSERT(I);
        ASSERT(0 == strcmp(I.previousDelimiter(), ""));
        ASSERT(0 == I.hasPreviousSoft());
        ASSERT(0 == I.isPreviousHard());
        ASSERT(0 == strcmp(I(), ""));
        ASSERT(0 == strcmp(I.delimiter(), "@"));
        ASSERT(0 == I.hasSoft());
        ASSERT(1 == I.isHard());
        mR = I.tokenRef();
        ASSERT(0 == R.length());

        {
            if (verbose) cout << "\t\tTesting reset in middle of string"
                              <<endl;
            const char *str= ", .Now ,, is .? the ??time ..for ,men \n";
            const char *str2= ",To :?: come   ,,to";

            const char *softDelim = " ,.\t\n";
            const char *hardDelim = ":?";
            Obj mI("", softDelim, hardDelim);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());

            {
                if (verbose) cout << "\t\tTesting reset with null" <<endl;
                mI.reset(str);  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "Now"));
                ASSERT(0 == strcmp(I.delimiter(), " ,, "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
                mR = I.tokenRef();
                ASSERT(3 == R.length());
                ASSERT(0 == strncmp("Now", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), " ,, "));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "is"));
                ASSERT(0 == strcmp(I.delimiter(), " .? "));
                ASSERT(1 == I.hasSoft());
                ASSERT(1 == I.isHard());
                mR = I.tokenRef();
                ASSERT(2 == R.length());
                ASSERT(0 == strncmp("is", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), " .? "));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "the"));
                ASSERT(0 == strcmp(I.delimiter(), " ?"));
                ASSERT(1 == I.hasSoft());
                ASSERT(1 == I.isHard());
                mR = I.tokenRef();
                ASSERT(3 == R.length());
                ASSERT(0 == strncmp("the", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), " ?"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), ""));
                ASSERT(0 == strcmp(I.delimiter(), "?"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
                mR = I.tokenRef();
                ASSERT(0 == R.length());

                mI.reset(0);  ASSERT(!I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), ""));
                ASSERT(0 == strcmp(I.delimiter(), ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }
            {
                if (verbose) cout << "\t\tTesting reset with empty" <<endl;
                mI.reset(str);  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "Now"));
                ASSERT(0 == strcmp(I.delimiter(), " ,, "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
                mR = I.tokenRef();
                ASSERT(3 == R.length());
                ASSERT(0 == strncmp("Now", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ++mI;  ASSERT(I);
                ++mI;  ASSERT(I);

                mI.reset("");  ASSERT(!I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ""));
                ASSERT(0 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), ""));
                ASSERT(0 == strcmp(I.delimiter(), ""));
                ASSERT(0 == I.hasSoft());
                ASSERT(0 == I.isHard());
            }
            {
                if (verbose) cout << "\t\tTesting reset with null" <<endl;
                mI.reset(str);  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "Now"));
                ASSERT(0 == strcmp(I.delimiter(), " ,, "));
                ASSERT(1 == I.hasSoft());
                ASSERT(0 == I.isHard());
                mR = I.tokenRef();
                ASSERT(3 == R.length());
                ASSERT(0 == strncmp("Now", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ++mI;  ASSERT(I);
                ++mI;  ASSERT(I);

                mI.reset(str2);  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), ","));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(0 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), "To"));
                ASSERT(0 == strcmp(I.delimiter(), " :"));
                ASSERT(1 == I.hasSoft());
                ASSERT(1 == I.isHard());
                mR = I.tokenRef();
                ASSERT(2 == R.length());
                ASSERT(0 == strncmp("To", R.data(), R.length()));

                ++mI;  ASSERT(I);
                ASSERT(0 == strcmp(I.previousDelimiter(), " :"));
                ASSERT(1 == I.hasPreviousSoft());
                ASSERT(1 == I.isPreviousHard());
                ASSERT(0 == strcmp(I(), ""));
                ASSERT(0 == strcmp(I.delimiter(), "?"));
                ASSERT(0 == I.hasSoft());
                ASSERT(1 == I.isHard());
                mR = I.tokenRef();
                ASSERT(0 == R.length());
            }
        }

        if (verbose) cout << "Done. (reset())" << endl;

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
        for (i = 0; i < 256; ++i) {
            all[i] = i + 1;
        }

        Ref mR;  const Ref& R = mR;  // reusable string ref

        {
            if (verbose) cout <<
                "\t\tTesting case where all chars are soft delimiters" << endl;
            Obj mI(all, all);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), all));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout <<
                "\t\tTesting case where all chars are hard delimiters" << endl;
            Obj mI(all, all, all);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            for (i = 0; i < 254; ++i) {
                LOOP_ASSERT(i, 0 == strcmp(I(), ""));
                mR = I.tokenRef();
                LOOP_ASSERT(i, 0 == R.length());

                LOOP_ASSERT(i, all[i] == *(I.delimiter()));
                LOOP_ASSERT(i, 1 == I.isHard());
                ++mI;  LOOP_ASSERT(i, I);
                LOOP_ASSERT(i, all[i] == *(I.previousDelimiter()));
                LOOP_ASSERT(i, 1 == I.isPreviousHard());
            }
            ++mI;  ASSERT(!I);
            ASSERT(all[253] == *(I.previousDelimiter()));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(all[254] == *(I.delimiter()));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
        }

        {
            if (verbose) cout << "Testing case where no chars are delimiters"
                              << endl;
            Obj mI(all, 0);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), all));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(all) == R.length());
            ASSERT(0           == strncmp(all, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), all));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "Testing nested iterators" << endl;
            const char *string = "abcdefghijklmnopqrstuvwxyz"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "abcdefghijklmnopqrstuvwxyz\n";
            char *finalString = stringRepeat(10, string);
            for (Obj mI(finalString, "\n"); mI; ++mI) {
                for (Obj czIt(mI(), "z"); czIt; ++czIt) {
                    ASSERT(0 == strcmp(czIt(), "abcdefghijklmnopqrstuvwxy"));
                }
            }
            delete [] finalString;
        }

        {
            if (verbose) cout << "Testing long strings" << endl;
            const char *string = "abcdefghijklmnopqrstuvwxyz";
            char *str = stringRepeat(10000, string);
            for (Obj czIt(str, "z"); czIt; ++czIt) {
                ASSERT(0 == strcmp(czIt(), "abcdefghijklmnopqrstuvwxy"));
            }
            delete [] str;
        }

        {
            if (verbose) cout << "Testing duplicate delimiters" << endl;
            const char *string = "abcdefghijklmnopqrstuvwxyz";
            char *str = stringRepeat(5, string);
            for (Obj czIt(str, "zzzzzzzzz"); czIt; ++czIt) {
                ASSERT(0 == strcmp(czIt(), "abcdefghijklmnopqrstuvwxy"));
            }
            delete [] str;
        }

        const char *str = "The   lazy   ,,, ,dog";
        {
            if (verbose) cout << "\t\tTesting soft delimiter separators"
                              << endl;
            Obj mI(str, " ,");  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I.delimiter(), "   ,,, ,"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));
        }

        {
            if (verbose) cout << "\t\tTesting hard delimiter separators"
                              << endl;
            Obj mI(str, " ", ",");  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I.delimiter(), "   ,"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));
        }

        {
            if (verbose) cout << "\t\tTesting mix of soft and " <<
                                 "hard delimiter separators" << endl;
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

            ASSERT(0 == strcmp(I.previousDelimiter(), " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I1.previousDelimiter(), " "));
            ASSERT(1 == I1.hasPreviousSoft());
            ASSERT(0 == I1.isPreviousHard());
            ASSERT(0 == strcmp(I2.previousDelimiter(), ":"));
            ASSERT(0 == I2.hasPreviousSoft());
            ASSERT(1 == I2.isPreviousHard());

            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I1(), "lazy"));
            ASSERT(0 == strcmp(I2(), ""));

            ASSERT(0 == strcmp(I.delimiter(), ",: "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            ASSERT(0 == strcmp(I1.delimiter(), ":, "));
            ASSERT(1 == I1.hasSoft());
            ASSERT(1 == I1.isHard());
            ASSERT(0 == strcmp(I2.delimiter(), ":"));
            ASSERT(0 == I2.hasSoft());
            ASSERT(1 == I2.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));
            mR = I1.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));
            mR = I2.tokenRef();
            ASSERT(0 == R.length());
        }
        if (verbose) cout << "Done. (operator++)" << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Testing:
        //    bdeut_StrTokenIter(const char*, const char*, const char*, ba*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting four-argument constructor"
                          << "\n================================="  << endl;

        bslma_TestAllocator ta(veryVeryVerbose);
        bslma_Allocator *ba = &ta;

        Ref mR;  const Ref& R = mR;  // reusable string ref

        const char *softDelims = " ,.\t\n";
        const char *hardDelims = ":?";
        const char *str        = ", .The   \tlazy::fox ?\n";

        if (verbose) cout <<  "\t\tString\t| softDelims\t| hardDelims" << endl;
        if (verbose) cout <<  "\t\t------\t| ----------\t| ----------" << endl;
        {
            if (verbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            Obj mI(0, 0, 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     0\t\t|     \"\"" << endl;
            Obj mI(0, 0, "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     0\t\t| hardDelims" << endl;
            Obj mI(0, 0, hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            Obj mI(0, "", 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     \"\"\t|     \"\"" << endl;
            Obj mI(0, "", "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     \"\"\t| hardDelims" << endl;
            Obj mI(0, "", hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            Obj mI(0, softDelims, 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t| softDelims\t|     \"\"" << endl;
            Obj mI(0, softDelims, "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t| softDelims\t| hardDelims" << endl;
            Obj mI(0, softDelims, hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            Obj mI("", 0, 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     0\t\t|     \"\"" << endl;
            Obj mI("", 0, "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     0\t\t| hardDelims" << endl;
            Obj mI("", 0, hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            Obj mI("", "", 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     \"\"\t|     \"\"" << endl;
            Obj mI("", "", "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout <<"\t\t  \"\"\t|     \"\"\t| hardDelims" <<endl;
            Obj mI("", "", hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t| softDelims\t|     0" << endl;
            Obj mI("", softDelims, 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t| softDelims\t|     \"\""
                              << endl;
            Obj mI("", softDelims, "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout <<"\t\t  \"\"\t| softDelims\t| hardDelims"
                              << endl;
            Obj mI("", softDelims, hardDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            Obj mI(str, 0, 0, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     0\t\t|     \"\"" << endl;
            Obj mI(str, 0, "", ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     0\t\t| hardDelims" << endl;
            Obj mI(str, 0, hardDelims, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ", .The   \tlazy"));
            ASSERT(0 == strcmp(I.delimiter(), ":"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(14 == R.length());
            ASSERT(0  == strncmp(", .The   \tlazy", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ":"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ":"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());
        }

        {
            if (verbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            Obj mI(str, "", 0, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     \"\"\t|     \"\"" << endl;
            Obj mI(str, "", "", ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     \"\"\t| hardDelims" << endl;
            Obj mI(str, "", hardDelims, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ", .The   \tlazy"));
            ASSERT(0 == strcmp(I.delimiter(), ":"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(14 == R.length());
            ASSERT(0  == strncmp(", .The   \tlazy", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ":"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ":"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());
        }

        {
            if (verbose) cout << "\t\t  str\t| softDelims\t|     0" << endl;
            Obj mI(str, softDelims, 0, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   \t"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   \t"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy::fox"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(9 == R.length());
            ASSERT(0 == strncmp("lazy::fox", R.data(), R.length()));
        }

        {
            if (verbose) cout << "\t\t  str\t| softDelims\t|     \"\"" << endl;
            Obj mI(str, softDelims, "", ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   \t"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   \t"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy::fox"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(9 == R.length());
            ASSERT(0 == strncmp("lazy::fox", R.data(), R.length()));
        }

        {
            if (verbose) cout << "\t\t  str\t| softDelims\t| hardDelims"
                              << endl;
            Obj mI(str, softDelims, hardDelims, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   \t"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   \t"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I.delimiter(), ":"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));
        }

        char all[256];
        for (int i = 0; i < 256; ++i) {
            all[i] = i+1;
        }

        {
            if (verbose) cout <<
                "\t\tTesting case where all chars are hard delimiters" << endl;
            Obj mI(all, all, all, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(all[0] == *(I.delimiter()));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing:
        //    bdeut_StrTokenIter(const char*, const char*, ba*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting three-argument constructor"
                          << "\n=================================="  << endl;

        bslma_TestAllocator ta(veryVeryVerbose);
        bslma_Allocator *ba = &ta;

        Ref mR;  const Ref& R = mR;  // reusable string ref

        const char *softDelims = " ,.\t\n";
        const char *hardDelims = ":?";
        const char *str        = ", .The   \tlazy::fox ?\n";

        if (verbose) cout <<  "\t\tString\t| softDelims\t| hardDelims" << endl;
        if (verbose) cout <<  "\t\t------\t| ----------\t| ----------" << endl;
        {
            if (verbose) cout << "\t\t  0\t|     0\t\t|     0" << endl;
            Obj mI(0, 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t|     \"\"\t|     0" << endl;
            Obj mI(0, "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  0\t| softDelims\t|     0" << endl;
            Obj mI(0, softDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     0\t\t|     0" << endl;
            Obj mI("", 0, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t|     \"\"\t|     0" << endl;
            Obj mI("", "", ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  \"\"\t| softDelims\t|     0" << endl;
            Obj mI("", softDelims, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     0\t\t|     0" << endl;
            Obj mI(str, 0, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t|     \"\"\t|     0" << endl;
            Obj mI(str, "", ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(str) == R.length());
            ASSERT(0           == strncmp(str, R.data(), R.length()));

            ++mI;  ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), str));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout << "\t\t  str\t| softDelims\t|     0" << endl;
            Obj mI(str, softDelims, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), "   \t"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "   \t"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy::fox"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(9 == R.length());
            ASSERT(0 == strncmp("lazy::fox", R.data(), R.length()));
        }

        char all[256];
        for (int i = 0; i < 256; ++i) {
            all[i] = i+1;
        }

        {
            if (verbose) cout <<
                "\t\tTesting case where all chars are soft delimiters" << endl;
            Obj mI(all, all, ba);  const Obj& I = mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), all));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
        }

        {
            if (verbose) cout <<
                "\t\tTesting case where no chars are delimiters" << endl;
            Obj mI(all, 0, ba);  const Obj& I = mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), all));
            ASSERT(0 == strcmp(I.delimiter(), ""));
            ASSERT(0 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(strlen(all) == R.length());
            ASSERT(0           == strncmp(all, R.data(), R.length()));
        }

        if (verbose) cout << "Done. (CTOR)" << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing:
        //    char *stringRepeat(int reps, const char *string);
        // --------------------------------------------------------------------

        if (verbose)
          cout << "\nTesting char *stringRepeat(int reps, const char *string)"
               << "\n========================================================"
               << endl;

        int i;
        char all[128];
        for (i = 0; i < 128; ++i) {
            all[i] = i+1;
        }
        all[127] = '\0';

        {
            char *str = stringRepeat(2, all);
            if (verbose) cout << str << endl;
            for (i = 0; i < 127; ++i) {
                LOOP_ASSERT(i, *(str + i) == all[i % 128]);
            }
            ASSERT('\0' == all[127]);
            for (i = 128; i < 254; ++i) {
                LOOP_ASSERT(i, str[i] == all[(i+1) % 128]);
            }
            delete [] str;
        }

        if (verbose) cout <<"Done. (char *stringRepeat(...))" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Breathing test
        //
        // Plan:
        //   o create a StrTokenIter(const char*, const char*, const char*)
        //   o verify basic correctness.
        //
        // Testing:
        //   bdeut_StrTokenIter(const char*, const char*, const char*, ba*);
        //   void operator++();
        //   operator const void *() const;
        //   const char *operator()() const;
        //   bool isHard() const;
        //   bool isPreviousHard() const;
        //   bool hasSoft() const;
        //   bool hasPreviousSoft() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBreathing Test"
                          << "\n==============" << endl;

        bslma_TestAllocator ta(veryVeryVerbose);
        bslma_Allocator *ba = &ta;

        const char *string = ", .Now ,, is .? the ??time ..for all good,men \n"
                             "to,come,to,the    aid,of    their,country.\n";

        const char *softDelim = " ,.\t\n";
        const char *hardDelim = ":?";

        Ref mR;  const Ref& R = mR;  // reusable string ref

        if (verbose) cout << "\tTesting with TestAllocator" << endl;

        {
            Obj mI(string, softDelim, hardDelim, ba);  const Obj& I = mI;
            ASSERT(I);

            if (verbose) cout << I() << endl;
            if (verbose) cout << I.delimiter() << endl;
            if (verbose) cout << I.previousDelimiter() << endl;

            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "Now"));
            ASSERT(0 == strcmp(I.delimiter(), " ,, "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("Now", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ,, "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "is"));
            ASSERT(0 == strcmp(I.delimiter(), " .? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("is", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " .? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "the"));
            ASSERT(0 == strcmp(I.delimiter(), " ?"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("the", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ?"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "time"));
            ASSERT(0 == strcmp(I.delimiter(), " .."));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("time", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " .."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "for"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("for", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "all"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("all", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "good"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("good", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "men"));
            ASSERT(0 == strcmp(I.delimiter(), " \n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("men", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " \n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "to"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("to", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "come"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("come", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "to"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("to", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "the"));
            ASSERT(0 == strcmp(I.delimiter(), "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("the", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "aid"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("aid", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "of"));
            ASSERT(0 == strcmp(I.delimiter(), "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("of", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "their"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(5 == R.length());
            ASSERT(0 == strncmp("their", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "country"));
            ASSERT(0 == strcmp(I.delimiter(), ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(7 == R.length());
            ASSERT(0 == strncmp("country", R.data(), R.length()));

            ++mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "country"));
            ASSERT(0 == strcmp(I.delimiter(), ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("??The,.\nlazy ? ?    ? ");  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), ",.\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ",.\n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I.delimiter(), " ? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
        }

        if (verbose) cout << "\tTesting with default allocator (= 0)" << endl;

        {
            Obj mI(string, softDelim, hardDelim);  const Obj& I = mI;
            ASSERT(I);

            if (verbose) cout << I() << endl;
            if (verbose) cout << I.delimiter() << endl;
            if (verbose) cout << I.previousDelimiter() << endl;

            ASSERT(0 == strcmp(I.previousDelimiter(), ", ."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "Now"));
            ASSERT(0 == strcmp(I.delimiter(), " ,, "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("Now", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ,, "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "is"));
            ASSERT(0 == strcmp(I.delimiter(), " .? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("is", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " .? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "the"));
            ASSERT(0 == strcmp(I.delimiter(), " ?"));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("the", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ?"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "time"));
            ASSERT(0 == strcmp(I.delimiter(), " .."));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("time", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " .."));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "for"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("for", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "all"));
            ASSERT(0 == strcmp(I.delimiter(), " "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("all", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "good"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("good", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "men"));
            ASSERT(0 == strcmp(I.delimiter(), " \n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("men", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " \n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "to"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("to", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "come"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("come", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "to"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("to", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "the"));
            ASSERT(0 == strcmp(I.delimiter(), "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("the", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "aid"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("aid", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "of"));
            ASSERT(0 == strcmp(I.delimiter(), "    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(2 == R.length());
            ASSERT(0 == strncmp("of", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "their"));
            ASSERT(0 == strcmp(I.delimiter(), ","));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(5 == R.length());
            ASSERT(0 == strncmp("their", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "country"));
            ASSERT(0 == strcmp(I.delimiter(), ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(7 == R.length());
            ASSERT(0 == strncmp("country", R.data(), R.length()));

            ++mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ","));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "country"));
            ASSERT(0 == strcmp(I.delimiter(), ".\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());

            mI.reset("??The,.\nlazy ? ?    ? ");  ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ""));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?"));
            ASSERT(0 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?"));
            ASSERT(0 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "The"));
            ASSERT(0 == strcmp(I.delimiter(), ",.\n"));
            ASSERT(1 == I.hasSoft());
            ASSERT(0 == I.isHard());
            mR = I.tokenRef();
            ASSERT(3 == R.length());
            ASSERT(0 == strncmp("The", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), ",.\n"));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(0 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), "lazy"));
            ASSERT(0 == strcmp(I.delimiter(), " ? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(4 == R.length());
            ASSERT(0 == strncmp("lazy", R.data(), R.length()));

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), " ? "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "?    "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "? "));
            ASSERT(1 == I.hasSoft());
            ASSERT(1 == I.isHard());
            mR = I.tokenRef();
            ASSERT(0 == R.length());

            ++mI;
            ASSERT(!I);
            ASSERT(0 == strcmp(I.previousDelimiter(), "?    "));
            ASSERT(1 == I.hasPreviousSoft());
            ASSERT(1 == I.isPreviousHard());
            ASSERT(0 == strcmp(I(), ""));
            ASSERT(0 == strcmp(I.delimiter(), "? "));
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
