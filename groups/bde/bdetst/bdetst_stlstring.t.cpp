// Bdetst_stlstring.t.cpp    -*-C++-*-

#include <bdetst_stlstring.h>

#include <iostream>
#include <exception>
#include <algorithm>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing 
// value-semantic containers.  We have chosen as *primary* *manipulators* the 
// 'append' and 'erase' methods to be used by the generator functions
// 'g' and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that each 
// manipulator must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bdema_testallocator' 
// component.  Exception neutrality involving streaming is verified using 
// 'bdex_testinstream' (and 'bdex_testoutstream').  
//
// Note that places where test drivers in this family are likely to require 
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 2] basic_string<C,T,A>(const A& a = A()); 
// [ 5] basic_string<C,T,A>(const basic_string<C,T,A>& original, 
//                          size_type pos = 0, size_type numChars = npos,
//                          const A& a = A());
// [ 4] basic_string<C,T,A>(const C *original, size_type numChars, 
//                          const A& a = A());
// [ 4] basic_string<C,T,A>(const C *original, const A& a = A());
// [ 4] basic_string<C,T,A>(size_type n, C c, const A& a = A());
// [--] template<class InputIter>
//      basic_string<C,T,A>(InputIter begin, InputIter end, const A& a = A());
// [ 2] ~basic_string<C,T,A>();
// [ 6] reference operator[](size_type pos);
// [ 6] const_reference operator[](size_type pos) const;
// [ 6] reference at(size_type pos);
// [ 6] const_reference at(size_type pos) const;
// [ 7] bool operator==(const basic_string<C,T,A>&, 
//                      const basic_string<C,T,A>&);
// [ 7] bool operator!=(const basic_string<C,T,A>&, 
//                      const basic_string<C,T,A>&);
// [11] size_type find(const string& s2, size_type pos1);
// [11] size_type find(const char* nts, size_type pos1);
// [11] size_type find(const char* buf, size_type pos1,
//                     size_type bufsize);
// [11] size_type find(char c, size_type pos1);
//
// [11] size_type rfind(const string& s2, size_type pos1);
// [11] size_type rfind(const char* nts, size_type pos1);
// [11] size_type rfind(const char* buf, size_type pos1,
//                      size_type bufsize);
// [11] size_type rfind(char c, size_type pos1);
//
// [11] size_type find_first_of(const string& s2, size_type pos1);
// [11] size_type find_first_of(const char* nts, size_type pos1);
// [11] size_type find_first_of(const char* buf, size_type pos1,
//                              size_type bufsize);
// [11] size_type find_first_of(char c, size_type pos1);
//
// [11] size_type find_last_of(const string& s2, size_type pos1);
// [11] size_type find_last_of(const char* nts, size_type pos1);
// [11] size_type find_last_of(const char* buf, size_type pos1,
//                             size_type bufsize);
// [11] size_type find_last_of(char c, size_type pos1);
//
// [11] size_type find_first_not_of(const string& s2, size_type pos1);
// [11] size_type find_first_not_of(const char* nts, size_type pos1);
// [11] size_type find_first_not_of(const char* buf, size_type pos1, 
//                                  size_type bufsize);
// [11] size_type find_first_not_of(char c, size_type pos1);
//
// [11] size_type find_last_not_of(const string& s2, size_type pos1);
// [11] size_type find_last_not_of(const char* nts, size_type pos1);
// [11] size_type find_last_not_of(const char* buf, size_type pos1, 
//                                 size_type bufsize);
// [11] size_type find_last_not_of(char c, size_type pos1);

//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [18] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void append(const C *character, size_type numChars); 
//
// [ 3] int ggg(basic_string<C,T,A> *object, const char *spec, int vF = 1);
// [ 3] basic_string<C,T,A>& gg(basic_string<C,T,A>* object, const char *spec);
// [--] basic_string<C,T,A>   g(const char *spec);

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
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BDEMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}                                       
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef string Obj;
typedef char Element;

const Element VALUES[] = { 'a', 'b', 'c', 'd', 'e' }; // avoid DEFAULT_VALUE

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4;  // conjunction with 'g' and 'gg'.

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

enum {
        PTR_SIZE = sizeof(char *),
        INT_SIZE = sizeof(int),
        STRUCT_SIZE = sizeof (string),
        INITIAL_CAPACITY_FOR_DEFAULT_OBJECT = 7 // The capacity of a default 
                                                // constructed Obj object,  
                                                // according to our 
                                                // implementation. Appending 
                                                // more characters 
                                                // than this value to a default
                                                // object causes a resize.
 };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int verifyInvariants(const Obj *object)
{
    int ret = (int)strlen(object->c_str()) == object->size();
    if (!ret) {
        P((int)strlen(object->c_str()));
        P(object->size());
    }
    return ret;
}

void
stretch(Obj *object, int size, const Element& elem)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    object->append(size, elem);
    ASSERT(object->size() >= size);
}


void
stretchRemoveAll(Obj *object, int size, const Element& elem)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
   // Function used for perturbation of internal state. 
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size, elem);
    object->erase();
    ASSERT(0 == object->size());
}


//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) string values to be
// appended to the 'basic_string' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'erase' method).
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
// ""           Has no effect; leaves the object unaffected.
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
    // manipulator 'erase'.  Optionally specify a zero 'verboseFlag' to 
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this 
    // function is used to implement 'gg' as well as allow for verification of 
    // syntax error detection. 
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->append(&VALUES[spec[i] - 'A'],1);
        }
        else if ('~' == spec[i]) {
            object->erase();
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
    Obj object((bdema_Allocator *)0);
    return gg(&object, spec);
}

std::string::size_type findExpected(const char             *hitsMask,
                                    std::string::size_type  startPos,
                                    char                    hit)
    // Return the positon within 'hitsMask' where the specified 'hit' is
    // found, searching forward from 'startPos'.  Return 'npos' if not found.
{
    std::size_t hitsLen = std::strlen(hitsMask);
    if (startPos > hitsLen) {
        startPos = hitsLen;
    }

    const char* exp = strchr(hitsMask + startPos, hit);
    return (0 == exp ? std::string::npos : exp - hitsMask);
}

std::string::size_type rfindExpected(const char             *hitsMask,
                                    std::string::size_type  startPos,
                                    char                    hit)
    // Return the positon within 'hitsMask' where the specified 'hit' is
    // found, searching backwards from 'startPos'.  Return 'npos' if not found.
{
    std::size_t hitsLen = std::strlen(hitsMask);
    if (startPos > hitsLen) {
        startPos = hitsLen;
    }

    for (int i = (int) startPos; i >= 0; --i) {
        if (hitsMask[i] == hit) {
            return i;
        }
    }

    return std::string::npos;
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
#if defined(BDES_PLATFORM__CMP_EPC)
    // The EPC C++ compiler on DGUX has a bug in the constructor for
    // ostrstream that take abuffer and a length.  The constructor will
    // destroy the last byte of the variable immediately preceeding the
    // buffer used to hold the stream.  If the parameter specifying the
    // buffer passed to the constructor is 1 less than the size of the buffer
    // stack integrity is maintained.
    const int epcBug = 1;
#else
    const int epcBug = 0;
#endif


    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);
    bdema_StrAllocator strAllocator0;
    bdema_StrAllocator strAllocator1;

    char *initialAllocation1 = (char *)strAllocator1.allocate(1);
    *initialAllocation1 = '\0';

    bdema_Allocator *ALLOCATOR[] = { 
      // TBD - Check if 0 should work      
      //                                          0,
                                     &testAllocator,
                                     &strAllocator0,
                                     &strAllocator1 
                                   };
    const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS:
        //
        // Concerns:
        //  1.  Search functions work correctly in the presence of single or
        //      multiple hits.
        //  2.  Search functions return npos if no hits.
        //  3.  Search functions return npos if hit is outside of search
        //      range.
        //  4.  Forward search functions return npos if search start position
        //      is npos or past end of string.
        //  5.  Reverse search functions search entire string if search start
        //      position is npos or past end of string.
        //  6.  Search functions find results at start, end, or middle of
        //      string. 
        //  7.  Partial hits are treated as non-hits.
        //  8.  Search functions work correctly if match string has length 0,
        //      1, or > 1.
        //
        // Plan:
        //  Construct a test vector with different input and search patterns
        //  such that the input strings span the following situations:
        //   1. Empty string
        //   2. One-character string
        //   3. Larger strings
        //   4. Strings with repeated patterns
        //  And the search patterns span the following situations:
        //   1. Empty search strings
        //   2. One-character search strings
        //   3. Two-character search strings
        //   4. Patterns that match the input string once
        //   5. Patterns that match the input string more than once.
        //   6. Patterns that don't match the input string.
        //   7. Patterns that match the entire input string.
        //  Perform searches using all of the 'find_xxx' functions in
        // 'std::string' using the following start positions:
        //   1. All input string positions from 0 to length();
        //   2. Input position past the end of the string (length() + 1)
        //   3. npos.
        //  For each search, confirm the result using the appropriate result
        //  from the input vector.
        //
        // Testing:
        //      size_type find(const string& s2, size_type pos1);
        //      size_type find(const char* nts, size_type pos1);
        //      size_type find(const char* buf, size_type pos1,
        //                     size_type bufsize);
        //      size_type find(char c, size_type pos1);
        //
        //      size_type rfind(const string& s2, size_type pos1);
        //      size_type rfind(const char* nts, size_type pos1);
        //      size_type rfind(const char* buf, size_type pos1,
        //                      size_type bufsize);
        //      size_type rfind(char c, size_type pos1);
        //
        //      size_type find_first_of(const string& s2, size_type pos1);
        //      size_type find_first_of(const char* nts, size_type pos1);
        //      size_type find_first_of(const char* buf, size_type pos1,
        //                              size_type bufsize);
        //      size_type find_first_of(char c, size_type pos1);
        //
        //      size_type find_last_of(const string& s2, size_type pos1);
        //      size_type find_last_of(const char* nts, size_type pos1);
        //      size_type find_last_of(const char* buf, size_type pos1,
        //                             size_type bufsize);
        //      size_type find_last_of(char c, size_type pos1);
        //
        //      size_type find_first_not_of(const string& s2, size_type pos1);
        //      size_type find_first_not_of(const char* nts, size_type pos1);
        //      size_type find_first_not_of(const char* buf, size_type pos1, 
        //                                  size_type bufsize);
        //      size_type find_first_not_of(char c, size_type pos1);
        //
        //      size_type find_last_not_of(const string& s2, size_type pos1);
        //      size_type find_last_not_of(const char* nts, size_type pos1);
        //      size_type find_last_not_of(const char* buf, size_type pos1, 
        //                                 size_type bufsize);
        //      size_type find_last_not_of(char c, size_type pos1);

        struct TestData {
            int         d_line;
            const char *d_input;
            const char *d_pattern;
            const char *d_findHits;
            const char *d_findOfHits;
        };

        static const TestData DATA[] = {
            // Line     Input    Pattern         Find            Find Of
            // ----     -----    -------         ----            -------
            { __LINE__, "",      "",             "X",            "" },
            { __LINE__, "a",     "",             "XX",           "." },
            { __LINE__, "",      "a",            "",             "" },

            { __LINE__, "a",     "a",            "X",            "X" },
            { __LINE__, "a",     "b",            ".",            "." },
            { __LINE__, "a",     "ab",           ".",            "X" },

            { __LINE__, "ab",    "a",            "X.",           "X." },
            { __LINE__, "ab",    "b",            ".X",           ".X" },
            { __LINE__, "ab",    "ab",           "X.",           "XX" },
            { __LINE__, "ab",    "x",            "..",           ".." },
            { __LINE__, "ab",    "ax",           "..",           "X." },
            { __LINE__, "ab",    "xb",           "..",           ".X" },
            { __LINE__, "ab",    "abc",          "..",           "XX" },

            { __LINE__, "abcd",  "a",            "X...",         "X..." },
            { __LINE__, "abcd",  "b",            ".X..",         ".X.." },
            { __LINE__, "abcd",  "d",            "...X",         "...X" },
            { __LINE__, "abcd",  "x",            "....",         "...." },
            { __LINE__, "abcd",  "ab",           "X...",         "XX.." },
            { __LINE__, "abcd",  "bc",           ".X..",         ".XX." },
            { __LINE__, "abcd",  "cd",           "..X.",         "..XX" },

            { __LINE__, "abcab", "a",            "X..X.",        "X..X." },
            { __LINE__, "abcab", "ab",           "X..X.",        "XX.XX" },
            { __LINE__, "abcab", "bc",           ".X...",        ".XX.X" },
            { __LINE__, "abcab", "abc",          "X....",        "XXXXX" },
        };

        static const std::size_t DATA_SIZE = sizeof(DATA) / sizeof(DATA[0]);

        typedef std::string::size_type size_type;
        static const size_type npos = std::string::npos;

        for (int i = 0; i < DATA_SIZE; ++i) {
            const TestData& testData = DATA[i];

            const int LINE = testData.d_line;

            const std::string s(testData.d_input);
            const size_type sLen = s.length();

            const std::string patStr(testData.d_pattern);
            const char *patNTS = testData.d_pattern; // null-terminated
            const size_type patLen = patStr.length();
            char patBufx[10];
            std::strcpy(patBufx, testData.d_pattern);
            patBufx[patLen] = 'x'; // not null-terminated
            const char *const patBuf = patBufx;
            const char patChar = testData.d_pattern[0];

            const char *const findHits     = testData.d_findHits;
            const char *const findOfHits   = testData.d_findOfHits;

            const size_type lastIteration = sLen + 2;
            for (size_type j = 0; j <= lastIteration; ++j) {

                const size_type startPos = (j == lastIteration ? npos : j);

                size_type r = npos, exp = npos;

#define FIND_ASSERT(c) LOOP4_ASSERT(LINE, startPos, r, exp, c)

                exp = findExpected(findHits, startPos, 'X');
                r = s.find(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.find(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.find(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.find(patChar, startPos);    FIND_ASSERT(r == exp);
                }

                exp = rfindExpected(findHits, startPos, 'X');
                r = s.rfind(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.rfind(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.rfind(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.rfind(patChar, startPos);    FIND_ASSERT(r == exp);
                }

                exp = findExpected(findOfHits, startPos, 'X');
                r = s.find_first_of(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.find_first_of(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.find_first_of(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.find_first_of(patChar, startPos);    FIND_ASSERT(r == exp);
                }

                exp = rfindExpected(findOfHits, startPos, 'X');
                r = s.find_last_of(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.find_last_of(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.find_last_of(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.find_last_of(patChar, startPos);    FIND_ASSERT(r == exp);
                }

                exp = findExpected(findOfHits, startPos, '.');
                r = s.find_first_not_of(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.find_first_not_of(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.find_first_not_of(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.find_first_not_of(patChar, startPos);    FIND_ASSERT(r == exp);
                }

                exp = rfindExpected(findOfHits, startPos, '.');
                r = s.find_last_not_of(patStr, startPos);         FIND_ASSERT(r == exp);
                r = s.find_last_not_of(patNTS, startPos);         FIND_ASSERT(r == exp);
                r = s.find_last_not_of(patBuf, startPos, patLen); FIND_ASSERT(r == exp);
                if (1 == patLen) {
                    r = s.find_last_not_of(patChar, startPos);    FIND_ASSERT(r == exp);
                }

            } // End for j
        } // End for i

      } break;

      // TBD: problem when exception is thrown during assignment
      case 10: {
#if 0
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //       other instance regardless of how either value is represented
        //       internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //       even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, 3, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        // Testing:
        //   basic_string<C,T,A>& operator=(char rhs);
        //   basic_string<C,T,A>& operator=(const char *rhs);
        //   basic_string<C,T,A>& operator=(const basic_string<C,T,A>& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            static const char *SPECS[] = { // len: 0-7, 10-12, 15-17, 30-32
                "", "A", "BC", "CDE", "DEAB", "EABCD", "ABCDEA", "ABCDEAB",
                "ABCDEABCDE",  "ABCDEABCDEA", "ABCDEABCDEAB",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
                "ABCDEABCDEABCDEAABCDEABCDEABCD",
                "ABCDEABCDEABCDEAABCDEABCDEABCDE",
                "ABCDEABCDEABCDEAABCDEABCDEABCDEA",
            0}; // Null string required as last element.

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

                    for (int uj = 0; uj < NUM_ALLOCATOR; ++uj) {
                        for (int vj = 0; vj < NUM_ALLOCATOR; ++vj) {

                          BEGIN_BDEMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mU(ALLOCATOR[uj]);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                    //----------^
                    Obj mV(ALLOCATOR[vj]);
                    const Obj& V = mV; gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_ALLOCATOR * NUM_ALLOCATOR;
                    if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                        cout << "\t| "; P_(uj); P_(vj); P_(U); P(V);
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, UU == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, Z==(V==U));

                    testAllocator.setAllocationLimit(AL);
                    mU = V; // test assignment here

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj,  V == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, verifyInvariants(&V));
                    //----------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == U);
                            LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj,
                                         verifyInvariants(&U));
                          } END_BDEMA_EXCEPTION_TEST

                          BEGIN_BDEMA_EXCEPTION_TEST {
                            Obj mU(ALLOCATOR[uj]);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                    //----------^
                    Obj mV(ALLOCATOR[vj]);
                    const Obj& V = mV; gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_ALLOCATOR * NUM_ALLOCATOR;
                    if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                        cout << "\t| "; P_(uj); P_(vj); P_(U); P(V);
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, UU == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, Z==(V==U));

                    mU = V.c_str(); // test assignment here

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj,  V == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, verifyInvariants(&V));
                    //----------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == U);
                            LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj,
                                         verifyInvariants(&U));
                          } END_BDEMA_EXCEPTION_TEST

                          BEGIN_BDEMA_EXCEPTION_TEST {
                            Obj mU(ALLOCATOR[uj]);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                    //----------^
                    Obj mV(ALLOCATOR[vj]);
                    const Obj& V = mV; gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_ALLOCATOR * NUM_ALLOCATOR;
                    if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                        cout << "\t| "; P_(uj); P_(vj); P_(U); P(V);
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, UU == U);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, Z==(V==U));

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, verifyInvariants(&U));
                    mU = V[0]; // test assignment here
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, verifyInvariants(&U));

                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, VV == V);
                    LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj, verifyInvariants(&V));
                    //----------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, uj, V_SPEC, vj,
                                         verifyInvariants(&U));
                          } END_BDEMA_EXCEPTION_TEST

                        }
                    }
                    LOOP2_ASSERT(ui, vi, verifyInvariants(&VV));
                }
                LOOP_ASSERT(ui, verifyInvariants(&UU));
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0-7, 10-12, 15-17, 30-32
                "", "A", "BC", "CDE", "DEAB", "EABCD", "ABCDEA", "ABCDEAB",
                "ABCDEABCDE",  "ABCDEABCDEA", "ABCDEABCDEAB",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
                "ABCDEABCDEABCDEAABCDEABCDEABCD",
                "ABCDEABCDEABCDEAABCDEABCDEABCDE",
                "ABCDEABCDEABCDEAABCDEABCDEABCDEA",
            0}; // Null string required as last element.

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

                for (int tj = 0; tj < NUM_ALLOCATOR; ++tj) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mY(ALLOCATOR[tj]);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(tj); P(Y); }

                    LOOP2_ASSERT(SPEC, tj, Y == Y);
                    LOOP2_ASSERT(SPEC, tj, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, tj, Y == Y);
                    LOOP2_ASSERT(SPEC, tj, X == Y);
                    LOOP2_ASSERT(SPEC, tj, verifyInvariants(&Y));
                  } END_BDEMA_EXCEPTION_TEST

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mY(ALLOCATOR[tj]);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(tj); P(Y); }

                    LOOP2_ASSERT(SPEC, tj, Y == Y);
                    LOOP2_ASSERT(SPEC, tj, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y.c_str(); // test assignment here

                    LOOP2_ASSERT(SPEC, tj, Y == Y);
                    LOOP2_ASSERT(SPEC, tj, X == Y);
                    LOOP2_ASSERT(SPEC, tj, verifyInvariants(&Y));
                  } END_BDEMA_EXCEPTION_TEST
                }

                LOOP_ASSERT(SPEC, verifyInvariants(&X));
            }
        }
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, use 'sizeof' to confirm
        //   that the (temporary) returned by 'g' differs in size from that
        //   returned by 'gg'.
        //
        // Testing:
        //   std::string g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0}; // Null string required as last element.

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
            LOOP_ASSERT(ti, verifyInvariants(&X));
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
            ASSERT(verifyInvariants(&x));
        }
      } break;
      case 8: {
        // ---------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        // Concerns:
        //
        //   1. Objects constructed with similar values are returned as equal.
        //   2. Objects constructed such that they have same (logical) value but
        //      different internal representation (due to the lack or presence 
        //      of an allocator) should be returned as equal.
        //   3. Unequal objects are always return false. 
        //
        // Plan:
        //   For 1 and 3, Specify a set A of unique allocators including no 
        //   allocator. Specify a set S of unique object values having various 
        //   minor or subtle differences, ordered by non-decreasing length.
        //   Verify the correctness of 'operator==' and 'operator!=' (returning
        //   either true or false) using all elements (u, ua, v, va) of the 
        //   cross product S X A X S X A.
        //   
        //
        //   For 2 create two objects using all elements in S one at a time. 
        //   For the second object change its internal representation by 
        //   extending it by different amounts in the set E, followed by erasing 
        //   its contents using 'erase'. Then recreate the original value and 
        //   verify that the second object still return equal to the first.
        //
        // Testing:
        //   operator==(const basic_string<C,T,A>&, const basic_string<C,T,A>&);
        //   operator!=(const basic_string<C,T,A>&, const basic_string<C,T,A>&);
        // ---------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

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
            "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
            "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
            "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
            "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
            "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA",
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            "AAAABAAAAAAAAAAAAAAAAAAAAAAAAAA",
            "AAAAABAAAAAAAAAAAAAAAAAAAAAAAAA",
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            "AAAABAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            "AAAAABAAAAAAAAAAAAAAAAAAAAAAAAAA",
            0}; // Null string required as last element.


        if (verbose) cout << "\nCompare each pair of similar and different"
                          << " values (u, ua, v, va) in S X A X S X A"
                          << " without perturbation." << endl;
        {

            // Create first object
            for (int si = 0; SPECS[si]; ++si) {
                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                    const char *const U_SPEC = SPECS[si];
                    const int curLen = (int)strlen(U_SPEC);

                    Obj mU(U_SPEC, ALLOCATOR[ai]); const Obj& U = mU;
                    LOOP_ASSERT(si, curLen == U.size()); // same lengths

                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;

                    if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                    // Create second object
                    for (int sj = 0; SPECS[sj]; ++sj) {
                        for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(V_SPEC, ALLOCATOR[aj]);
                            const Obj& V = mV;

                            if (veryVerbose) {
                                cout << "  ";
                                P_(sj); P_(V_SPEC); P(V);
                            }

                            const int isSame = si == sj;
                            LOOP2_ASSERT(si, sj,  isSame == (U == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V));
                            LOOP2_ASSERT(si, sj, verifyInvariants(&V));
                        }
                    }
                    LOOP_ASSERT(si, verifyInvariants(&U));
                }
            }
        }


        if (verbose) { cout <<
            "\nCompare each pair of similar values (u, ua, v, va) in "
                            << "S X A X S X A after perturbing." << endl; }
        
        {
            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15
            };

            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            // Create first object
            for (int si = 0; SPECS[si]; ++si) {
                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                    const char *const U_SPEC = SPECS[si];
                    const int curLen = (int)strlen(U_SPEC);

                    Obj mU(ALLOCATOR[ai]); const Obj& U = mU;
                    gg(&mU, U_SPEC);
                    LOOP_ASSERT(si, curLen == U.size()); // same lengths

                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;

                    if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                    // Create second object
                    for (int sj = 0; SPECS[sj]; ++sj) {
                        for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {
                            //Perform perturbation
                            for (int e = 0; e < NUM_EXTEND; ++e) {

                                const char *const V_SPEC = SPECS[sj];
                                Obj mV(ALLOCATOR[aj]); const Obj& V = mV;
                                gg(&mV, V_SPEC); 

                                stretchRemoveAll(&mV, EXTEND[e], 
                                                 VALUES[e % NUM_VALUES]);
                                gg(&mV, V_SPEC);

                                if (veryVerbose) {
                                    cout << "  ";
                                    P_(sj); P_(V_SPEC); P(V);
                                }

                                const int isSame = si == sj;
                                LOOP2_ASSERT(si, sj,  isSame == (U == V));
                                LOOP2_ASSERT(si, sj, !isSame == (U != V));
                                LOOP2_ASSERT(si, sj, verifyInvariants(&V));
                            }
                        }
                    }
                    LOOP_ASSERT(si, verifyInvariants(&U));
                }
            }

        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   operator<<(ostream&, const basic_string<C,T,A>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream) with allocator."
                          << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec                 output format       // ADJUST
                //----  -------------------  -----------------------------
                { L_,   "",                  ""                         },
                { L_,   "A",                 "a"                        },
                { L_,   "AB",                "ab"                       },
                { L_,   "ABC",               "abc"                      },
                { L_,   "ABCD",              "abcd"                     },
                { L_,   "ABCDE",             "abcde"                    },
                { L_,   "ABCDEA",            "abcdea"                   },
                { L_,   "ABCDEAB",           "abcdeab"                  },
                { L_,   "ABCDEABC",          "abcdeabc"                 },
                { L_,   "ABCDEABCD",         "abcdeabcd"                },
                { L_,   "ABCDEABCDE",        "abcdeabcde"               },
                { L_,   "ABCDEABCDEA",       "abcdeabcdea"              },
                { L_,   "ABCDEABCDEAB",      "abcdeabcdeab"             },
                { L_,   "ABCDEABCDEABC",     "abcdeabcdeabc"            },
                { L_,   "ABCDEABCDEABCD",    "abcdeabcdeabcd"           },
                { L_,   "ABCDEABCDEABCDE",   "abcdeabcdeabcde"          },
                { L_,   "ABCDEABCDEABCDEA",  "abcdeabcdeabcdea"         },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDE",
                                     "abcdeabcdeabcdeaabcdeabcdeabcde" },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDEA",
                                     "abcdeabcdeabcdeaabcdeabcdeabcdea" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = 0x7f;  // Value 1 used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int)strlen(SPEC);

                char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  out1 << X         << ends;
                ostrstream out2(buf2, SIZE);  out2 << X.c_str() << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
                LOOP_ASSERT(ti, verifyInvariants(&X));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESSORS:
        // Concerns:
        //   1. The returned value for operator[] and function at() is correct 
        //      as long as pos < size().
        //   2. The const version of operator[] returns CharT() if 
        //      pos == size().
        //   3. The at() function throws out_of_range exception if 
        //      pos >= size().
        //   4. Changing the internal representation to get the same (logical)
        //      final value, should not change the result of the element 
        //      accessor functions.
        //   5. The internal memory management is correctly hooked up so that 
	//      changes made to the state of the object via these accessors 
	//      do change the state of the object.
        //
        // Plan:
        //   For concerns 1 and 2 do the following:
        //   Specify a set S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  
	//
	//   For concern 4, repeat the above test on an existing object y after 
        //   perturbing y so as to achieve an internal representation
        //   of w that is potentially different from that of x.
        //
        //   For concern 3, check that function at() throws a out_of_range 
        //   exception when pos >= size().
	//
	//   For concern 5, For each value w in S, create an object x using 
        //   'gg'. Create another empty object y and make it 'resize' its size 
	//   equal to the size of x. Now using the element accessor functions 
	//   recreate the value of x in y. Verify that x == y. 
	//   Note - Using untested resize(int).
	//
        // Testing:
        //   reference operator[](int pos);
        //   const_reference operator[](int pos) const;
        //   reference at(int pos);
        //   const_reference at(int pos) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        const int SZ = 32;
        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec_p;           // specification string
            int         d_length;           // expected length
            Element     d_elements[SZ];     // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",                  0, { 0 }                   },
            { L_,   "A",                 1, { VA }                  },
            { L_,   "B",                 1, { VB }                  },
            { L_,   "AB",                2, { VA, VB }              },
            { L_,   "BC",                2, { VB, VC }              },
            { L_,   "BCA",               3, { VB, VC, VA }          },
            { L_,   "CAB",               3, { VC, VA, VB }          },
            { L_,   "CDAB",              4, { VC, VD, VA, VB }      },
            { L_,   "DABC",              4, { VD, VA, VB, VC }      },
            { L_,   "ABCDE",             5, { VA, VB, VC, VD, VE }  },
            { L_,   "EDCBA",             5, { VE, VD, VC, VB, VA }  },
            { L_,   "ABCDEA",            6, { VA, VB, VC, VD, VE,
                                              VA }                  },
            { L_,   "ABCDEAB",           7, { VA, VB, VC, VD, VE,
                                              VA, VB }              },
            { L_,   "BACDEABC",          8, { VB, VA, VC, VD, VE,
                                              VA, VB, VC }          },
            { L_,   "CBADEABCD",         9, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD }      },
            { L_,   "CBADEABCDAB",      11, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB }                  },
            { L_,   "CBADEABCDABC",     12, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC }              },
            { L_,   "CBADEABCDABCDE",   14, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC, VD, VE }      },
            { L_,   "CBADEABCDABCDEA",  15, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC, VD, VE, VA }  },
            { L_,   "CBADEABCDABCDEAB", 16, { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC, VD, VE, VA,
                                              VB }                  },
            { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA", 31,
                                            { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC, VD, VE, VA,
                                              VB, VC, VB, VA, VD,
                                              VE, VA, VB, VC, VD,
                                              VA, VB, VC, VD, VE,
                                              VA }                  },
            { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB", 32,
                                            { VC, VB, VA, VD, VE,
                                              VA, VB, VC, VD, VA,
                                              VB, VC, VD, VE, VA,
                                              VB, VC, VB, VA, VD,
                                              VE, VA, VB, VC, VD,
                                              VA, VB, VC, VD, VE,
                                              VA, VB }              },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15
        };

        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        (void) EXTEND[0];       // Silence "unused" warning

        if (verbose) cout << "\nTesting const and non-const versions of "
                          << "operator[] and function at() where pos < size()."
                          << endl;
        {

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                    Obj mX(ALLOCATOR[ai]);

                    const Obj& X = gg(&mX, SPEC);    
                    LOOP2_ASSERT(ti, ai, curLen == X.size()); // same lengths

                    if (verbose) cout << "\tOn objects of length "
                                      << curLen << ':' << endl;

                    if (verbose) cout << "\t\tSpec = \"" << SPEC << '"'
                                      << endl;

                    if (veryVerbose) { cout << "\t\t\t"; P(X); }

                    int i;
                    for (i = 0; i < LENGTH; ++i) {
                        LOOP3_ASSERT(LINE, ai, i, e[i] == mX[i]);
                        LOOP3_ASSERT(LINE, ai, i, e[i] == X[i]);
                        LOOP3_ASSERT(LINE, ai, i, e[i] == mX.at(i));
                        LOOP3_ASSERT(LINE, ai, i, e[i] == X.at(i));
                    }
                     
                    // STD (21.3.4): The behaviour of the non-const version of
                    // operator[] is undefined for pos == size(). The const  
                    // version of operator[] returns CharT(). Both versions of 
                    // at() throw out_of_range exceptions, so they are tested 
		    // below.

                    LOOP2_ASSERT(LINE, ai, Element() == X[LENGTH]);

                    for (; i < SZ; ++i) {
                        LOOP3_ASSERT(LINE, ai, i, 0 == e[i]);
                    }

                    // Check for perturbation, Concern 4

                    Obj mY(ALLOCATOR[ai]); 

                    const Obj& Y = gg(&mY, SPEC);

                    {                      
                        for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                            stretchRemoveAll(&mY, ei, VALUES[ei % NUM_VALUES]);
                            gg(&mY, SPEC);

                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                            int j;
                            for (j = 0; j < LENGTH; ++j) {
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == mY[j]);
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == Y[j]);
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == mY.at(j));
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == Y.at(j));
                            }
                     
                            // STD (21.3.4): The behaviour of the non-const  
                            // version of operator[] is undefined for pos == 
                            // size(). The const version of operator[] returns  
                            // CharT(). Both versions of at() throw out_of_range
                            // exceptions, so they are tested below.

                            LOOP3_ASSERT(LINE, ai, ei, Element() == Y[LENGTH]);

                            for (; j < SZ; ++j) {
                                LOOP4_ASSERT(LINE, ai, j, ei, 0 == e[j]);
                            }
                        }
                    }
                    LOOP2_ASSERT(LINE, ai, verifyInvariants(&X));
                }
            }
        }


        if (verbose) cout << "\nTesting non-const versions of operator[] and "
                          << "function at() modify state of object correctly."
                          << endl;
        {

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                    Obj mX(ALLOCATOR[ai]);

                    const Obj& X = gg(&mX, SPEC);
                    LOOP2_ASSERT(ti, ai, curLen == X.size()); // same lengths

                    if (verbose) cout << "\tOn objects of length "
                                      << curLen << ':' << endl;

                    if (verbose) cout << "\t\tSpec = \"" << SPEC << '"'
                                      << endl;
                    if (veryVerbose) { cout << "\t\t\t"; P(X); }

                    Obj mY(ALLOCATOR[ai]); const Obj& Y = mY;
                    Obj mZ(ALLOCATOR[ai]); const Obj& Z = mZ;

                    mY.resize(curLen);
                    mZ.resize(curLen);

                    // Change state of Y and Z so its same as X

                    for (int j = 0; j < curLen; j++) {
                        mY[j] = e[j];
			mZ.at(j) = e[j];
                    }

                    if (veryVerbose) {
		      cout << "\t\tNew object1: "; P(Y);
                      cout << "\t\tNew object2: "; P(Z);
                    }

                    LOOP2_ASSERT(ti, ai, Y == X);
		    LOOP2_ASSERT(ti, ai, Z == X);
		}
	    }
	}

#ifdef BDE_BUILD_TARGET_EXC

        if (verbose) cout << "\tTesting for out_of_range exceptions thrown"
                          << " by at() when pos >= size()." << endl;

        {

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                    int exceptions, trials;

                    const int NUM_TRIALS = 2;
                    // Check exception behaviour for non-const version of at()
                    // Checking the behaviour for pos == size() and 
                    // pos > size().
                    for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                                   ; ++trials) {

                        try {
                            Obj mX(ALLOCATOR[ai]);
                            gg(&mX, SPEC);
                            mX.at(LENGTH + trials);
                        } catch (out_of_range) {
                            ++exceptions;
                            continue;
                        }   
                    }

                    ASSERT(exceptions == trials);

                    
                    // Check exception behaviour for const version of at() 
                    for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                                   ; ++trials) {
                    
                        try {
                            Obj mX(ALLOCATOR[ai]);
                            const Obj& X = gg(&mX, SPEC);
                            X.at(LENGTH + trials);
                        } catch (out_of_range) {
                            ++exceptions;
                            continue;
                        }   
                    }

                    ASSERT(exceptions == trials);
                }
            }
        }

#endif // BDE_BUILD_TARGET_EXC

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-VALUE COPY CONSTRUCTOR:
        //   We have the following concerns:
        //    1. The initial value is correct.
        //        a. Initial size of new object is minimum of numCharacters 
        //           and mS.size() - pos.
        //    2. The container throws appropriate exception on invalid 
        //       parameters.
        //        a. out_of_range exception if pos > mS.size().  
        //    3. The constructor is exception neutral w.r.t. memory allocation.
        //    4. The internal memory management system is hooked up properly 
        //       so that *all* internally allocated memory draws from a 
        //       user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For creating objects to pass as arguments to the copy constructor 
        //   we will use the generator function gg(Obj *mS, const char *spec),
        //   after creating mS using the (tested) default constructor.
        //
        //   Perform the tests for concerns 1 and 2 with and without an 
	//   allocator.
        //
        //   For 1(1a) we will construct objects mS of varying lengths and 
        //   pass to the copy ctor with 0 <= pos <= mS.size() and 
        //   0 <= numChars <= mS.size() - pos + 1. (Note passing a value of 
        //   numChars that is greater than mS.size() - pos). Verify that new 
        //   created object has correct size (min(numChars, mS.size() - pos)).
        //   
        //   For 2(2a) we will construct objects mS of varying lengths and 
        //   pass to the copy ctor with pos > mS.size(). Verify that an  
        //   out_of_range exception is thrown.
        //
        //   For 3 we will construct objects mS of varying lengths and 
        //   pass to the copy ctor with 0 <= pos <= mS.size() and 
        //   0 <= numChars <= mS.size() - pos + 1. We will check the presence
        //   of exceptions during memory allocations using a 
        //   'bdema_TestAllocator' and varying its *allocation* *limit*. Verify
        //   that there are no *numBlocksInUse()* after exception has been 
        //   handled.
        //
        //   For 4 we create an object entirely in static memory 
        //   (using a 'bdema_BufferAllocator') and never destroyed. 
        //   Additionally supply the bdema_BufferAllocator as the allocator 
        //   to the object, so any internal allocation should also be in static
        //   memory. If they are not then we will get a leak in Purify.
        //
        //   Use 'operator==' to verify the value and 'operator[]' to get 
        //   individual characters. 
        //
        // Testing:
        //   basic_string<Ch,Tr,A>(const basic_string<C,T,A>&  mS,
        //                         size_type          pos,
        //                         size_type          numChars,
        //                         const A&           a = A());
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Value Copy Constructor" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
            "\nTesting initial-value copy ctor with string and sub-string "
                          << "initial value." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_saSpec;   // source string
                int         d_len;      // size of the source string
            } DATA[] = {
                //line  s-array                               s-len  
                //----  ---------------                       ----- 
                { L_,   "",                                     0    },
                { L_,   "A",                                    1    },
                { L_,   "AB",                                   2    },
                { L_,   "ABC",                                  3    },
                { L_,   "ABCD",                                 4    },
                { L_,   "ABCDE",                                5    },
                { L_,   "ABCDEA",                               6    },
                { L_,   "ABCDEAB",                              7    },
                { L_,   "ABCDEABC",                             8    },
                { L_,   "ABCDEABCD",                            9    },
                { L_,   "ABCDEABCDE",                          10    },
                { L_,   "ABCDEABCDEA",                         11    },
                { L_,   "ABCDEABCDEAB",                        12    },
                { L_,   "ABCDEABCDEABC",                       13    },
                { L_,   "ABCDEABCDEABCD",                      14    },
                { L_,   "ABCDEABCDEABCDE",                     15    },
                { L_,   "ABCDEABCDEABCDEA",                    16    },
                { L_,   "ABCDEABCDEABCDEAB",                   17    },
                { L_,   "ABCDEABCDEABCDEABCDEABCD",            24    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDE",     31    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDEA",    32    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDEAB",   33    },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout << "\tTesting without allocators and "
                              << "without exceptions." <<endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN + 1; ++pos) {

                        for(int nChars = 0; nChars < SLEN - pos + 2; ++nChars) {
                            const int rlen = min(nChars, SLEN - pos);
                            
                            if (verbose)
                                cout << "\tCreating object of length: " 
                                     << rlen << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars); 
                            }
                                
                            Obj mX(S, pos, nChars);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: "; P(X);
                            }
                          
                            if (veryVerbose) {
                                if(nChars > rlen) { 
                                    cout <<"\t\tINVALID INPUT: ";    
                                    P_(nChars); P_(rlen); P(X.size());
                                }
                            }
  
                            LOOP_ASSERT(ti, X.size() == rlen);
                            LOOP_ASSERT(ti, verifyInvariants(&X));
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i=0;i < rlen;++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[rlen]);

                        } 
                    } 
                } 
            } 


            if (verbose) cout << "\tTesting with allocator and "
                              << "without checking for out_of_range "
                              << "exceptions." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN + 1; ++pos) {

                        for(int nChars = 0; nChars < SLEN - pos + 2;++nChars) {
                            const int rlen = min(nChars, SLEN - pos);
                    
                            if (verbose)
                                cout << "\tCreating object of length: " 
                                     << rlen << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars);
                            }
                             
                            const int BB = testAllocator.numBlocksTotal();   
                            const int B = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tBEFORE: ";
                                P_(BB); P_(B); P(nChars);
                            }

                            Obj mX(S, pos, nChars, &testAllocator);
                            const Obj &X = mX;

                            const int AA = testAllocator.numBlocksTotal();   
                            const int A = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tAFTER : ";
                                P_(AA); P(A);
                            } 
         
                            LOOP_ASSERT(ti, B + 1 == A);
                            LOOP_ASSERT(ti, BB + 1 == AA);
                            
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: "; P(X);
                            }
                          
                            if (veryVerbose) {
                                if(nChars > rlen) { 
                                    cout <<"\t\tINVALID INPUT: ";    
                                    P_(nChars); P_(rlen); P(X.size());
                                }
                            }
  
                            LOOP_ASSERT(ti, X.size() == rlen);
                            LOOP_ASSERT(ti, verifyInvariants(&X));
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i=0;i < rlen;++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[rlen]);
                        } 
                    } 
                } 
            } 


            if (verbose) cout << "\tTesting with allocators and checking "
                              << "for allocation exceptions." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN + 1; ++pos) {

                        for(int nChars = 0; nChars < SLEN - pos + 2; ++nChars) {
                            const int rlen = min(nChars, SLEN - pos);
                            
                            if (verbose)
                                cout << "\tCreating object of length: " 
                                     << rlen << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars);
                            }
                            
                          BEGIN_BDEMA_EXCEPTION_TEST {   
                            Obj mX(S, pos, nChars, &testAllocator);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: ";
                                P(X);
                            }
                          
                            if (veryVerbose) {
                                if(nChars > rlen) { 
                                    cout <<"\t\tINVALID INPUT: ";    
                                    P_(nChars); P_(rlen); P(X.size());
                                }
                            }
  
                            LOOP_ASSERT(ti, X.size() == rlen);
                            LOOP_ASSERT(ti, verifyInvariants(&X));
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i=0; i < rlen; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[rlen]);
                          } END_BDEMA_EXCEPTION_TEST
  
                            LOOP_ASSERT(ti, 
                                        0 == testAllocator.numBlocksInUse());

                        } 
                    } 
                } 
            } 

            if (verbose) cout << "\tIn place using a buffer allocator."
                              << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;

                    char memory[1024 * 1024];
                    bdema_BufferAllocator a(memory, sizeof memory);

                    {
                        Obj mS;
                        const Obj& S = mS;
                        gg(&mS, S_SPEC);

                        for(int pos = 0; pos < SLEN + 1; ++pos) {

                            for(int nChars = 0;nChars < SLEN - pos + 2;
                                                                  ++nChars) {
                    
                                    Obj *doNotDelete =
                                        new(a.allocate(sizeof(Obj)))
                                    Obj(S, pos, nChars, &a);

                                    Obj &mX = *doNotDelete;  const Obj &X = mX;
                                    if (veryVerbose) { 
                                        cout << "\t\t"; P(X);
                                    }

                                    LOOP_ASSERT(ti, verifyInvariants(&X));
                                    LOOP_ASSERT(ti, verifyInvariants(&S));
                            }
                        }
                    }

                }
       
                // No destructor is called; will produce memory leak in
                // purify if internal allocators are not hooked up
                // properly.
            } 
       


#ifdef BDE_BUILD_TARGET_EXC

            if (verbose) cout << "\tTesting without allocators and checking " 
                              << "out_of_range exceptions."<< endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    try {
                         
                        Obj mX(S, SLEN + 1, 1); 

                        if(veryVerbose) {
                            cout << "ERROR: Should throw exception";
                        }
                        ASSERT(0);

                    } catch (out_of_range) {
                        if (veryVerbose) {
                            cout << "\t\tInside out_of_range exception, ";
                            P(ti);
                        }    
                    }
                }
            } 

            if (verbose) cout << "\tTesting with allocators and checking " 
                              << "out_of_range exceptions."<< endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const char *S_SPEC = DATA[ti].d_saSpec;
                    const int   SLEN   = DATA[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    const int BB = testAllocator.numBlocksTotal();   
                    const int B = testAllocator.numBlocksInUse();   

                    if (veryVerbose) {
                        cout << "\t\tBEFORE: ";
                        P_(BB);P(B);
                    }
   

                    try {
                         
                        Obj mX(S, SLEN + 1, 1, &testAllocator); 

                        if(veryVerbose) {
                            cout << "ERROR: Should throw exception";
                        }
                        ASSERT(0);

                    } catch (out_of_range) {
                        if (veryVerbose) {
                            cout << "\t\tInside out_of_range exception,"<<endl;
                            P(ti);
                        }    
                    }

                    const int AA = testAllocator.numBlocksTotal();   
                    const int A = testAllocator.numBlocksInUse();   

                    if (veryVerbose) {
                        cout << "\t\tAFTER : ";
                        P_(AA);P(A);
                    }

                    LOOP_ASSERT(ti, BB + 0 == AA);
                    LOOP_ASSERT(ti,  B + 0 ==  A);

                    LOOP_ASSERT(ti, testAllocator.numBlocksInUse() == 0);
                }
            } 
#endif 
	}

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-VALUE CONSTRUCTORS:
        //   We have the following concerns:
        //    1. The initial value is correct.
        //    2. The container throws appropriate exception on invalid 
        //       parameters.
        //        a. length_error exception if numChars == npos.  
        //    3. The constructor is exception neutral w.r.t. memory allocation.
        //    4. The internal memory management system is hooked up properly 
        //       so that *all* internally allocated memory draws from a 
        //       user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   To get a const CharT *original, we create objects of type 
        //   basic_string<CharT> and call the data() function on 
        //   them. 
        // 
        //   We will use the generator function gg(Obj *mS, const char *spec),
        //   after creating mS using the default constructor.
        //
        //   Perform the tests for case 1 and 2 with and without an allocator.
        //
        //   For 1(1a) we will construct objects mS of varying lengths and 
        //   pass mS.data() to the ctor with 0 <= pos < mS.size() and 
        //   0 <= numChars <= mS.size() - pos + 1. (Note passing a value of 
        //   numChars that is greater than mS.size() - pos but less than 
        //   Obj::npos). Verify that new created object has correct size 
        //   numChars.
        //   
        //   For 2(2a) we will construct objects mS of varying lengths and 
        //   pass to the ctor with numChars == npos . Verify that an  
        //   length_error exception is thrown.
        //
        //   For 3 we will construct objects mS of varying lengths and 
        //   pass to the copy ctor with 0 <= pos < mS.size() and 
        //   0 <= numChars <= mS.size() - pos + 1. We will check the presence
        //   of exceptions during memory allocations using a 
        //   'bdema_TestAllocator' and varying its *allocation* *limit*. Verify
        //   that there are no *numBlocksInUse()* after exception has been 
        //   handled.
        //
        //   For 4 we create an object entirely in static memory 
        //   (using a 'bdema_BufferAllocator') and never destroyed. 
        //   Additionally supply the bdema_BufferAllocator as the allocator 
        //   to the object, so any internal allocation should also in static
        //   memory. If they are not then we will get a leak in Purify.
        //
        //   For the constructor taking a single character, perform the above
        //   tests using a single character instead of a const CharT *.
        //
        //   Use 'operator==' to verify the value and 'operator[]' to get 
        //   individual characters. 
        //
        // Testing:
        //   basic_string<Ch,Tr,A>(const charT*       original,
        //                         size_type          numChars,
        //                         const A&           a = A());
        //   basic_string<Ch,Tr,A>(const charT*       original,
        //                         const A&           a = A());
        //   basic_string<Ch,Tr,A>(size_type          numChars,
        //                         charT              c,
        //                         const A&           a = A());
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Value Constructor" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
            "\nTesting initial-value ctor with string and sub-string "
                          << "initial value." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_saSpec;   // source string
                int         d_len;      // length of the source string
            } DATA1[] = {
                //line  s-array                               s-len  
                //----  ---------------                       ----- 
                { L_,   "",                                     0    },
                { L_,   "A",                                    1    },
                { L_,   "AB",                                   2    },
                { L_,   "ABC",                                  3    },
                { L_,   "ABCD",                                 4    },
                { L_,   "ABCDE",                                5    },
                { L_,   "ABCDEA",                               6    },
                { L_,   "ABCDEAB",                              7    },
                { L_,   "ABCDEABC",                             8    },
                { L_,   "ABCDEABCD",                            9    },
                { L_,   "ABCDEABCDE",                          10    },
                { L_,   "ABCDEABCDEA",                         11    },
                { L_,   "ABCDEABCDEAB",                        12    },
                { L_,   "ABCDEABCDEABC",                       13    },
                { L_,   "ABCDEABCDEABCD",                      14    },
                { L_,   "ABCDEABCDEABCDE",                     15    },
                { L_,   "ABCDEABCDEABCDEA",                    16    },
                { L_,   "ABCDEABCDEABCDEAB",                   17    },
                { L_,   "ABCDEABCDEABCDEABCDEABCD",            24    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDE",     31    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDEA",    32    },
                { L_,   "ABCDEABCDEABCDEAABCDEABCDEABCDEAB",   33    },
            };

            const int NUM_DATA = sizeof DATA1 / sizeof *DATA1;

            if (verbose) cout << "\tTesting without allocators and "
                              << "without exceptions, constructors with "
                              << "and without length specified." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA1[ti].d_lineNum;
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;

                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN; ++pos) {
 
                        for(int nChars = 0; nChars < SLEN - pos + 2; ++nChars) {

                            if (verbose) 
                                cout << "\tCreating object of length:" 
                                     << nChars << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars);
                            }
                                
                            Obj mX(S.data() + pos, nChars);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: ";
                                P_(X.size()); P(X);
                            }
                          
                            LOOP_ASSERT(ti, X.size() == nChars);
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i = 0; i < nChars; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nChars]);

                        } 
                       
                        if (verbose) 
                            cout << "\tCreating object of length:"
                                 << " UNSPECIFIED." << endl;
                        {     
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P(pos);
                            }
                                
                            Obj mX(S.data() + pos);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: ";
                                P_(X.size()); P(X);
                            }

                            const int nSize = SLEN - pos;
                            
                            LOOP_ASSERT(ti, X.size() == nSize);
                            LOOP_ASSERT(ti, verifyInvariants(&X));
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i = 0; i < nSize; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nSize]);
        
                        } 
                    } 
                } 
            } 

            if (verbose) cout << "\tTesting with allocator and without " 
                              << "length error exceptions for objects," 
                              << " with and without length specified." << endl;

            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA1[ti].d_lineNum;
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN; ++pos) {
  
                        for(int nChars = 0; nChars < SLEN - pos + 2; ++nChars) {
                    
                            if (verbose)
                                cout << "\tCreating object of length: " 
                                     << nChars << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars);
                            }
                             
                            const int BB = testAllocator.numBlocksTotal();   
                            const int B = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tBEFORE: ";
                                P_(BB); P_(B); P(nChars);
                            }

                            Obj mX(S.data() + pos, nChars, &testAllocator);
                            const Obj &X = mX;

                            const int AA = testAllocator.numBlocksTotal();   
                            const int A = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tAFTER : "; P_(AA); P(A);
                            } 
         
                            LOOP_ASSERT(ti, B + 1 == A);
                            LOOP_ASSERT(ti, BB + 1 == AA);
                            
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: "; P(X);
                            }
                          
  
                            LOOP_ASSERT(ti, X.size() == nChars);
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i = 0; i < nChars; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nChars]);

                        } 
                        
                        if (verbose) 
                            cout << "\tCreating object of length:"
                                 << " UNSPECIFIED." << endl;
                        {     
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P(pos);
                            }
                               
                            const int BB = testAllocator.numBlocksTotal();   
                            const int B = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tBEFORE: ";
                                P_(BB); P(B);
                            }
 
                            Obj mX(S.data() + pos, &testAllocator);
                            const Obj &X = mX;
                    
                            const int AA = testAllocator.numBlocksTotal();   
                            const int A = testAllocator.numBlocksInUse();   

                            if (veryVerbose) {
                                cout << "\t\tAFTER : ";
                                P_(AA); P(A);
                            } 
         
                            LOOP_ASSERT(ti, B + 1 == A);
                            LOOP_ASSERT(ti, BB + 1 == AA);

                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: ";
                                P_(X.size());P(X);
                            }

                            const int nSize = SLEN - pos;
                            
                            LOOP_ASSERT(ti, X.size() == nSize);
                            LOOP_ASSERT(ti, verifyInvariants(&S));
                            LOOP_ASSERT(ti, verifyInvariants(&X));

                            for(int i = 0; i < nSize; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nSize]);
                        }
                    } 
   
                } 
            } 


            if (verbose) cout << "\tTesting with allocators and checking "
                              << "for allocation exceptions." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA1[ti].d_lineNum;
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    for(int pos = 0; pos < SLEN; ++pos) {
 
                        for(int nChars = 0; nChars < SLEN - pos + 2; ++nChars) {

                            if (verbose)
                                cout << "\tCreating object of length: " 
                                     << nChars << endl;
                             
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P_(pos); P(nChars);
                            }
                            
                          BEGIN_BDEMA_EXCEPTION_TEST {   
                            Obj mX(S.data() + pos, nChars, &testAllocator);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: "; P(X);
                            }
  
                            LOOP_ASSERT(ti, X.size() == nChars);
                            LOOP_ASSERT(ti, verifyInvariants(&S));

                            for(int i = 0; i < nChars; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nChars]);
                          } END_BDEMA_EXCEPTION_TEST
  
                            LOOP_ASSERT(ti, 
                                        0 == testAllocator.numBlocksInUse());

                        } 

                        if (verbose) 
                            cout << "\tCreating object of length:"
                                 << " UNSPECIFIED." << endl;
                        {     
                            if (veryVerbose) {
                                cout << "\t\tPARAM OBJECT: ";
                                P_(S); P(pos);
                            }
                                
                          BEGIN_BDEMA_EXCEPTION_TEST {   
                            Obj mX(S.data() + pos, &testAllocator);
                            const Obj &X = mX;
                    
                            if (veryVerbose) {
                                cout << "\t\tNEW OBJECT: ";
                                P_(X.size()); P(X);
                            }

                            const int nSize = SLEN - pos;
                            
                            LOOP_ASSERT(ti, X.size() == nSize);
                            LOOP_ASSERT(ti, verifyInvariants(&S));
                            LOOP_ASSERT(ti, verifyInvariants(&X));

                            for(int i = 0; i < nSize; ++i)
                                LOOP_ASSERT(ti, X[i] == S[pos + i]);
                            LOOP_ASSERT(ti, 0 == X[nSize]);
                          } END_BDEMA_EXCEPTION_TEST
                            LOOP_ASSERT(ti, 
                                        0 == testAllocator.numBlocksInUse());
                        }
                    } 
                } 
            } 

            if (verbose) cout << "\tIn place using a buffer allocator, "
                              << "length specified." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;

                    char memory[1024 * 1024];
                    bdema_BufferAllocator a(memory, sizeof memory);

                    {
                        Obj mS;
                        const Obj& S = mS;
                        gg(&mS, S_SPEC);

                        for(int pos = 0; pos < SLEN; ++pos) {

                            for(int nChars = 0; nChars < SLEN - pos + 2;
                                                                ++nChars) {
                                Obj *doNotDelete =
                                    new(a.allocate(sizeof(Obj)))
                                Obj(S.data() + pos, nChars, &a);
                                Obj &mX = *doNotDelete;  const Obj &X = mX;
                                if (veryVerbose) {
                                    cout << "\t\t"; P(X);
                                }
                                
                                LOOP_ASSERT(ti, verifyInvariants(&S));
                            }
                        }
                    }

                }
       
                // No destructor is called; will produce memory leak in
                // purify if internal allocators are not hooked up
                // properly.
            } 

            if (verbose) cout << "\tIn place using a buffer allocator, "
                              << "length unspecified." << endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;

                    char memory[1024 * 1024];
                    bdema_BufferAllocator a(memory, sizeof memory);

                    {
                        Obj mS;
                        const Obj& S = mS;
                        gg(&mS, S_SPEC);

                        for(int pos = 0; pos < SLEN; ++pos) {
                            Obj *doNotDelete =
                                new(a.allocate(sizeof(Obj)))
                            Obj(S.data() + pos, &a);
                            Obj &mX = *doNotDelete;  const Obj &X = mX;
                            if (veryVerbose) P(X);
                              
                            LOOP_ASSERT(ti, verifyInvariants(&X));
                            LOOP_ASSERT(ti, verifyInvariants(&S));
                        }
                    }

                }
       
                // No destructor is called; will produce memory leak in
                // purify if internal allocators are not hooked up
                // properly.
            } 


#ifdef BDE_BUILD_TARGET_EXC

            if (verbose) cout << "\tTesting without allocators and checking " 
                              << "length_error exceptions."<< endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA1[ti].d_lineNum;
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    try {
                         
                        Obj mX(S.data(), Obj::npos); 

                        if(veryVerbose) {
                            cout << "ERROR: Should throw exception";
                        }
                        LOOP_ASSERT(ti, 0);

                    } catch (length_error) {
                        if (veryVerbose) {
                            cout << "\t\tInside length_error exception, ";
                            P(ti);
                        }
                    }
                }
            } 


            if (verbose) cout << "\tTesting with allocators and checking " 
                              << "length_error exceptions."<< endl;
            {
                for (int ti = 0; ti < NUM_DATA ; ++ti) {
                    const int   LINE   = DATA1[ti].d_lineNum;
                    const char *S_SPEC = DATA1[ti].d_saSpec;
                    const int   SLEN   = DATA1[ti].d_len;
                    
                    Obj mS;
                    const Obj& S = mS;
                    gg(&mS, S_SPEC);

                    const int BB = testAllocator.numBlocksTotal();   
                    const int  B = testAllocator.numBlocksInUse();   

                    if (veryVerbose) {
                        cout << "\t\tBEFORE: ";
                        P_(BB); P(B);
                    }
   

                    try {
                         
                        Obj mX(S.data(), Obj::npos, &testAllocator); 

                        if(veryVerbose) {
                            cout << "ERROR: Should throw exception";
                        }
                        ASSERT(0);

                    } catch (length_error) {
                        if (veryVerbose) {
                            cout << "\t\tInside length_error exception, "; 
                            P(ti); 
                        }
                    }

                    const int AA = testAllocator.numBlocksTotal();   
                    const int A = testAllocator.numBlocksInUse();   

                    if (veryVerbose) {
                        cout << "\t\tAFTER : ";
                        P_(AA); P(A);
                    }

                    LOOP_ASSERT(ti, BB + 0 == AA);
                    LOOP_ASSERT(ti,  B + 0 ==  A);

                    LOOP_ASSERT(ti, testAllocator.numBlocksInUse() == 0);
                }
            } 
#endif 

	}

        if (verbose) cout << "\n\tTesting the single character constructor "
                          << "without allocator and without checking for "
                          << "length_error exception." << endl;      
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
             
            for (int i = 0; i < NUM_TRIALS; ++i) {
                
                Obj mX(i, VALUES[i % NUM_VALUES]); const Obj& X = mX;        

                if(veryVerbose){
                    cout << "\t\t";
                    P_(i); int Cap = X.capacity(); P_(Cap); P(X);
                }

                LOOP_ASSERT(i, i == X.size()); 
                
                for (int j = 0; j < i; ++j) 
                    LOOP_ASSERT(i, X[j] == VALUES[i % NUM_VALUES]); 
                LOOP_ASSERT(i, 0 == X[i]);                 
            } 

        }


        if (verbose) cout << "\n\tTesting the single character constructor "
                          << "with allocator and without checking for "
                          << "length_error exception." << endl;      
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
             
            for (int i = 0; i < NUM_TRIALS; ++i) {

                const int BB = testAllocator.numBlocksTotal();   
                const int  B = testAllocator.numBlocksInUse();   

                if (veryVerbose) {
                    cout << "\t\tBEFORE: ";
                    P_(BB); P(B);
                }

                Obj mX(i, VALUES[i % NUM_VALUES], &testAllocator); 
                const Obj& X = mX;               

                const int AA = testAllocator.numBlocksTotal();   
                const int  A = testAllocator.numBlocksInUse();   

                if(veryVerbose){
                    cout << "\t\tAFTER : "; P_(AA); P(A); cout << "\t\t";
                    P_(i); int Cap = X.capacity(); P_(Cap); P(X);
                }

                LOOP_ASSERT(i, BB + 1 == AA); 
                LOOP_ASSERT(i,  B + 1 ==  A); 
                LOOP_ASSERT(i, i == X.size()); 
                
                for (int j = 0; j < i; ++j) 
                    LOOP_ASSERT(i, X[j] == VALUES[i % NUM_VALUES]); 
                LOOP_ASSERT(i, 0 == X[i]);                 
            }
        } 


        if (verbose) cout << "\n\tTesting the single character constructor "
                          << "with allocator and checking for "
                          << "allocation exceptions." << endl;      
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
             
            for (int i = 0; i < NUM_TRIALS; ++i) {

              BEGIN_BDEMA_EXCEPTION_TEST {   
                
                Obj mX(i, VALUES[i % NUM_VALUES], &testAllocator); 
                const Obj& X = mX;               

              } END_BDEMA_EXCEPTION_TEST

                LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());
            }
        } 

        if (verbose) cout << "\tIn place using a buffer allocator, "
                          << "for single character constructor." << endl;
        {
            char memory[1024 * 1024];
            bdema_BufferAllocator a(memory, sizeof memory);

            {
                const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
             
                for (int i = 0; i < NUM_TRIALS; ++i) {
                
                    Obj *doNotDelete =
                                new(a.allocate(sizeof(Obj)))
                    Obj(i, VALUES[i % NUM_VALUES], &a);

                    Obj &mX = *doNotDelete;  const Obj &X = mX;

                    if (veryVerbose) {
                        cout << "\t\t"; P(X);
                    }

                    LOOP_ASSERT(i, verifyInvariants(&X));
                }

            }

            // No destructor is called; will produce memory leak in
            // purify if internal allocators are not hooked up
            // properly.
        } 


#ifdef BDE_BUILD_TARGET_EXC

        if (verbose) cout << "\n\tTesting the single character constructor "
                          << "without allocator and checking for "
                          << "length_error exception." << endl;      
        {
            try { 

                Obj mX(Obj::npos, VALUES[0]); 
                const Obj& X = mX;               
                ASSERT(0);

            } catch(length_error) {
                if(veryVerbose) {
                    cout << "\t\tInside length error exception.";
                }
            } 
        }


        if (verbose) cout << "\n\tTesting the single character constructor "
                          << "with allocator and checking for "
                          << "length_error exception." << endl;      
        {
            const int BB = testAllocator.numBlocksTotal(); 
            const int  B = testAllocator.numBlocksInUse(); 
             
            if (veryVerbose) {
                cout << "\t\tBEFORE: "; P_(BB); P(B);
            }

            try { 

                Obj mX(Obj::npos, VALUES[0], &testAllocator); 
                const Obj& X = mX;               
                ASSERT(0);

            } catch(length_error) {
                if(veryVerbose) {
                    cout << "\t\tInside length error exception." << endl;
                }
            } 

            const int AA = testAllocator.numBlocksTotal(); 
            const int  A = testAllocator.numBlocksInUse(); 
             
            if (veryVerbose) {
                cout << "\t\tAFTER : "; P_(AA);P(A);
            }

            ASSERT(AA == BB);
            ASSERT( A ==  B); 
            ASSERT( 0 == testAllocator.numBlocksInUse());
        }
#endif //BDE_BUILD_TARGET_EXC


      } break;



      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify (1) that valid
        //   generator syntax produces expected results and (2) that invalid
        //   syntax is detected and reported.
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'erase').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'ggg' to set the state of a newly created object.  Verify that
        //   'ggg' returns the expected value corresponding to the location of
        //   the first invalid value of the 'spec'.  Repeat the test for a
        //   longer 'spec' generated by prepending a string ending in a '~'
        //   character (denoting 'erase').  Note that we are testing the
        //   parser only; the primary manipulators are already assumed to work.
        //
        // Testing:
        //   basic_string<C,T,A>& gg(basic_string<C,T,A>* object, 
        //                           const char *spec);
        //   int ggg(basic_string<C,T,A> *object, const char *spec, 
        //           int vF = 1);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },

                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "~",            0,      { 0 }                   },

                { L_,   "CD",           2,      { VC, VD }              },
                { L_,   "E~",           0,      { 0 }                   },
                { L_,   "~E",           1,      { VE }                  },
                { L_,   "~~",           0,      { 0 }                   },

                { L_,   "ABC",          3,      { VA, VB, VC }          },
                { L_,   "~BC",          2,      { VB, VC }              },
                { L_,   "A~C",          1,      { VC }                  },
                { L_,   "AB~",          0,      { 0 }                   },
                { L_,   "~~C",          1,      { VC }                  },
                { L_,   "~B~",          0,      { 0 }                   },
                { L_,   "A~~",          0,      { 0 }                   },
                { L_,   "~~~",          0,      { 0 }                   },

                { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                { L_,   "~BCD",         3,      { VB, VC, VD }          },
                { L_,   "A~CD",         2,      { VC, VD }              },
                { L_,   "AB~D",         1,      { VD }                  },
                { L_,   "ABC~",         0,      { 0 }                   },

                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                { L_,   "AB~DE",        2,      { VD, VE }              },
                { L_,   "ABCD~",        0,      { 0 }                   },
                { L_,   "A~C~E",        1,      { VE }                  },
                { L_,   "~B~D~",        0,      { 0 }                   },

                { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = (int)strlen(SPEC);

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

                LOOP_ASSERT(LINE, LENGTH == X.size());
                LOOP_ASSERT(LINE, LENGTH == Y.size());
                for (int i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }

                LOOP_ASSERT(LINE, verifyInvariants(&X));
                LOOP_ASSERT(LINE, verifyInvariants(&Y));
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

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int INDEX        = DATA[ti].d_index;
                const int curLen       = (int)strlen(SPEC);

                Obj mX(&testAllocator);  const Obj& X = mX;

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
                LOOP_ASSERT(LINE, verifyInvariants(&X));
            }
        }

      } break ;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators 
        //      - append                (black-box)
        //      - erase                 (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default Constructor
        //        1a. creates the correct initial value.
        //        1b. is exception neutral with respect to memory allocation.
        //        1c. has the internal memory management system hooked up 
        //              properly so that *all* internally allocated memory 
        //              draws from the same user-supplied allocator whenever
        //              one is specified.
        //    2. The destructor properly deallocates all allocated memory to
        //            its corresponding allocator from any attainable state.
        //    3. 'append'
        //        3a. produces the expected value.
        //        3b. increases capacity as needed.
        //        3c. maintains valid internal state.
        //        3d. is exception neutral with respect to memory allocation.  
        //    4. 'erase' 
        //        4a. produces the expected value (empty).
        //        4b. properly destroys each contained element value.
        //        4c. maintains valid internal state.
        //        4d. does not allocate memory.
        //
        // Plan:
        //   To address concerns 1a - 1c, create an object using the default
        //   constructor: 
        //    - With and without passing in an allocator. 
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory 
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent 
        //   objects, ordered by increasing length.  In each test, allow the 
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately. 
        //   After the final append operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object 
        //   and the 'bdema_TestAllocator' to confirm whether a resize has
        //   occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'append' with 'erase'; this time, however, use the test 
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //   Additionally, append elements after the erase to original length 
        //   to confirm 4c.
        // 
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation 
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'append' 
        //          to create an instance of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'append' to create an 
        //          instance of length i, use 'erase' to erase its value 
        //          and confirm (with 'size'), use append to set the instance
        //          to a value of length j, verify the value, and allow the 
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'erase' is not 
        //   called; if only the second test produces an error, we know that
        //   'erase' is to blame.  We will rely on 'bdema_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d. 
        //
        // Testing:
        //   basic_string<C,T,A>(bdema_Allocator *ba);
        //   ~basic_string<C,T,A>();
        //   BOOTSTRAP: void append(const C *original,int size); // no aliasing
        //   void erase();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.size());
            ASSERT(verifyInvariants(&X));
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.size());
            ASSERT(verifyInvariants(&X));
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            if (veryVerbose) cout << 
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.size());
            ASSERT(verifyInvariants(&X));
          } END_BDEMA_EXCEPTION_TEST

            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap) without "
                          << "allocator." << endl;
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
                        
            for (int li = 0; li < NUM_TRIALS; ++li) {

                if (verbose) cout << "\tOn an object of initial length:"
                                  << li << "." << endl;
                Obj mX;  const Obj& X = mX;
               
                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }
                
                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                if(veryVerbose){
                    cout <<"\t\tBEFORE: ";
                    int Cap = X.capacity();
                    P_(Cap); P(X);
                }

                mX.append(&VALUES[li % NUM_VALUES], 1);

                if(veryVerbose){
                    cout <<"\t\tAFTER : ";
                    int Cap = X.capacity();
                    P_(Cap); P(X);
                }

                LOOP_ASSERT(li, li + 1 == X.size());
             
                for (int i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }

                LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
                LOOP_ASSERT(li, 0 == X[li + 1]);

                LOOP_ASSERT(li, verifyInvariants(&X));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap) with allocator."
                          << endl;
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;

            for (int li = 0; li < NUM_TRIALS; ++li) {

                if (verbose) cout << "\tOn an object of initial length:"
                                  << li << "." << endl;
                Obj mX(&testAllocator);  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }

                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X);
                }

                mX.append(&VALUES[li % NUM_VALUES], 1);

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tAFTER : "; P_(AA); P_(A); P(X);
                }

                if (li == INITIAL_CAPACITY_FOR_DEFAULT_OBJECT) {
                    LOOP_ASSERT(li, BB + 1 == AA);
                    LOOP_ASSERT(li, B + 0 == A);
                }
                else {
                    LOOP_ASSERT(li, BB + 0 == AA);
                    LOOP_ASSERT(li, B + 0 == A);
                }
           
                LOOP_ASSERT(li, li + 1 == X.size());
                for (int i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }
                LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
                LOOP_ASSERT(li, 0 == X[li + 1]);

                LOOP_ASSERT(li, verifyInvariants(&X));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'erase' without allocator."
                          << endl;
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;

            for (int li = 0; li < NUM_TRIALS; ++li) {
            
                if (verbose) cout << "\tOn an object of initial length:"
                                  << li << "." << endl;
                Obj mX;  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }

                if(veryVerbose){
                    cout <<"\t\tBEFORE: ";
                    int Cap = X.capacity();
                    P_(Cap); P(X);
                }

                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                mX.erase();

                if(veryVerbose){
                    cout <<"\t\tAFTER : ";
                    int Cap = X.capacity();
                    P_(Cap); P(X);
                }

                LOOP_ASSERT(li, 0 == X.size());
                LOOP_ASSERT(li, 0 == X[0]);

                LOOP_ASSERT(li, verifyInvariants(&X));

                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }

                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                if(veryVerbose){
                    cout <<"\t\tAFTER SECOND APPEND: ";
                    int Cap = X.capacity();
                    P_(Cap); P(X);
                }

                LOOP_ASSERT(li, verifyInvariants(&X));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'erase' with allocator." << endl;
        {
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;

            for (int li = 0; li < NUM_TRIALS; ++li) {

                if (verbose) cout << "\tOn an object of initial length:"
                                  << li << "." << endl;

                Obj mX(&testAllocator);  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }

                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tBEFORE: ";
                    P_(BB); P_(B); P(X);
                }

                mX.erase();

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                
                if (veryVerbose) {
                    cout << "\t\tAFTER : ";
                    P_(AA); P_(A); P(X);
                }

                for (int i = 0; i < li; ++i) {
                    mX.append(&VALUES[i % NUM_VALUES], 1);
                }

                const int CC = testAllocator.numBlocksTotal();
                const int C  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tAFTER SECOND APPEND: ";
                    P_(CC); P_(C); P(X);
                }

                LOOP_ASSERT(li, BB == AA);
                LOOP_ASSERT(li, BB == CC);

                LOOP_ASSERT(li, B  - 0 == A);
                LOOP_ASSERT(li, B  - 0 == C);

                LOOP_ASSERT(li, li == X.size());
                LOOP_ASSERT(li, 0 == X[li]);

                LOOP_ASSERT(li, verifyInvariants(&X));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting the destructor and exception "
                          << "neutrality with allocator." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. append { V0, V1, V2, V3, V4, V0, ... }  up to length i
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope
            //    5. make sure that destructor cleans up
            
            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;

            for (int i = 0; i < NUM_TRIALS; ++i) { 
                if (verbose) cout << 
                    "\t\tOn an object of length:" << i << '.' << endl;

              BEGIN_BDEMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(&VALUES[k % NUM_VALUES], 1);
                }

                LOOP_ASSERT(i, i == X.size());                          // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, VALUES[k % NUM_VALUES] == X[k]);
                }

                LOOP_ASSERT(i, verifyInvariants(&X));
              } END_BDEMA_EXCEPTION_TEST                                // 4.
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());      // 5.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'erase'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. append V0 values up to a length of i
            //    3. verify initial length and contents
            //    4. erase contents from instance 
            //    5. verify length is 0
            //    6. append { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope
            //    9. make sure that the destructor cleans up

            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;

            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout << 
                    "\t\tOn an object of initial length:" << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout << 
                        "\t\t\tAnd with final length:" << j << '.' << endl;

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(&V0, 1);
                    }

                    LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, V0 == X[k]);
                    }

                    mX.erase();                                         // 4.
                    LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(&VALUES[k % NUM_VALUES],1);
                    }

                    LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }

                    LOOP2_ASSERT(i, j, verifyInvariants(&X));
                  } END_BDEMA_EXCEPTION_TEST                            // 8.
                  LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
                }
     
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting the destructor and exception "
                          << "neutrality without allocator." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. append { V0, V1, V2, V3, V4, V0, ... }  up to length i
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope
            //    5. make sure that the destructor cleans up

            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
            for (int i = 0; i < NUM_TRIALS; ++i) { 
                if (verbose) cout << 
                    "\t\tOn an object of length:" << i << '.' << endl;

              BEGIN_BDEMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX;  const Obj& X = mX;                             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(&VALUES[k % NUM_VALUES], 1);
                }

                LOOP_ASSERT(i, i == X.size());                          // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, VALUES[k % NUM_VALUES] == X[k]);
                }

                LOOP_ASSERT(i, verifyInvariants(&X));
              } END_BDEMA_EXCEPTION_TEST                                // 4.
              
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());      // 5.
            }

        }

        if (verbose) cout << "\tWith 'append' and 'erase'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. append V0 values up to a length of i
            //    3. verify initial length and contents
            //    4. erase contents from instance 
            //    5. verify length is 0
            //    6. append { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope
            //    9. make sure that the destructor cleans up

            const int NUM_TRIALS = INITIAL_CAPACITY_FOR_DEFAULT_OBJECT + 2;
            for (int i = 0; i < NUM_TRIALS; ++i) { 
                if (verbose) cout << 
                    "\t\tOn an object of initial length:" << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout << 
                        "\t\t\tAnd with final length:" << j << '.' << endl;

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX;  const Obj& X = mX;                         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(&V0, 1);
                    }

                    LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, V0 == X[k]);
                    }

                    mX.erase();                                         // 4.
                    LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(&VALUES[k % NUM_VALUES],1);
                    }

                    LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }

                    LOOP2_ASSERT(i, j, verifyInvariants(&X));

                  } END_BDEMA_EXCEPTION_TEST                            // 8.
                  LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
                }

            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - primary manipulators: 'append' and 'erase' methods
        //      - basic accesors: 'size' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary manipulator [1&5], copy constructor [2&8], and 
        //   assignment operator [9&10] in situations where the internal data 
        //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
        //   assignment for a non-empty instance [11] and allow the result to
        //   leave scope, enabling the destructor to assert internal object 
        //   invariants.  Display object values frequently in verbose mode:
        //
        // 1. Create an object x1 (default ctor).       { x1: }
        // 2. Create a second object x2 (copy from x1). { x1: x2: }
        // 3. Append an element value A to x1).         { x1:A x2: }
        // 4. Append the same element value A to x2).   { x1:A x2:A }
        // 5. Append another element value B to x2).    { x1:A x2:AB }
        // 6. Remove all elements from x1.              { x1: x2:AB }
        // 7. Create a third object x3 (default ctor).  { x1: x2:AB x3: }
        // 8. Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
        // 9. Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
        // 10. Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        // 11. Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor)." 
                             "\t\t\t{ x1: }" << endl;

        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << 
            "\ta. Check initial state of x1." << endl;

        ASSERT(0 == X1.size());

        if(veryVerbose){
            int Cap = X1.capacity();
            cout <<"\t\t"; P(Cap);
        }

        if (verbose) cout << 
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;

        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << 
            "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.size());

        if (verbose) cout << 
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append an element value A to x1)."  
                            "\t\t\t{ x1:A x2: }" << endl;
        mX1.append(&VA, 1);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << 
            "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.size());
        ASSERT(VA == X1[0]);

        if (verbose) cout << 
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append the same element value A to x2)." 
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.append(&VA, 1);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << 
            "\ta. Check new state of x2." << endl;
        ASSERT(1 == X2.size());
        ASSERT(VA == X2[0]);

        if (verbose) cout << 
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Append another element value B to x2)." 
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.append(&VB, 1);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << 
            "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.size());
        ASSERT(VA == X2[0]);
        ASSERT(VB == X2[1]);

        if (verbose) cout << 
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Remove all elements from x1." 
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.erase();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << 
            "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.size());

        if (verbose) cout << 
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create a third object x3 (default ctor)." 
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << 
            "\ta. Check new state of x3." << endl;
        ASSERT(0 == X3.size());

        if (verbose) cout << 
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(X3 == X1 == 1);          ASSERT(X3 != X1 == 0);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create a forth object x4 (copy of x2)." 
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << 
            "\ta. Check new state of x4." << endl;
       
        ASSERT(2 == X4.size());
        ASSERT(VA == X4[0]);
        ASSERT(VB == X4[1]);

        if (verbose) cout << 
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 1);          ASSERT(X4 != X2 == 0);
        ASSERT(X4 == X3 == 0);          ASSERT(X4 != X3 == 1);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1 (non-empty becomes empty)." 
                             "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << 
            "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.size());

        if (verbose) cout << 
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        ASSERT(X2 == X3 == 1);          ASSERT(X2 != X3 == 0);
        ASSERT(X2 == X4 == 0);          ASSERT(X2 != X4 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x3 = x4 (empty becomes non-empty)." 
                             "\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << 
            "\ta. Check new state of x3." << endl;
        ASSERT(2 == X3.size());
        ASSERT(VA == X3[0]);
        ASSERT(VB == X3[1]);

        if (verbose) cout << 
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(X3 == X1 == 0);          ASSERT(X3 != X1 == 1);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);
        ASSERT(X3 == X4 == 1);          ASSERT(X3 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11. Assign x4 = x4 (aliasing)." 
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << 
            "\ta. Check new state of x4." << endl;
        ASSERT(2 == X4.size());
        ASSERT(VA == X4[0]);
        ASSERT(VB == X4[1]);

        if (verbose) cout << 
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 0);          ASSERT(X4 != X2 == 1);
        ASSERT(X4 == X3 == 1);          ASSERT(X4 != X3 == 0);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);


        ASSERT(verifyInvariants(&X1));
        ASSERT(verifyInvariants(&X2));
        ASSERT(verifyInvariants(&X3));
        ASSERT(verifyInvariants(&X4));
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



