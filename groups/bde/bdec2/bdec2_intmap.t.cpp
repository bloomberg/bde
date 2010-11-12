// bdec2_intmap.t.cpp         -*-C++-*-

#include <bdec2_intmap.h>

#include <bdema_bufferedsequentialallocator.h>   // for testing only
#include <bdex_testoutstream.h>                  // for testing only
#include <bdex_testinstream.h>                   // for testing only
#include <bdex_testinstreamexception.h>          // for testing only

#include <bslma_defaultallocatorguard.h>         // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bsls_platformutil.h>                   // for testing only

#include <bsl_new.h>         // placement syntax
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'add' and 'removeAll' methods to be used by the generator functions
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
// [ 2] bdec2_IntMap(bslma_Allocator *ba = 0);
// [  ] bdec2_IntMap(Hint, bslma_Allocator *ba = 0);
// [13] bdec2_IntMap(const InitialCapacity& ne, *ba = 0);
// [  ] bdec2_IntMap(const InitialCapacity& ne, Hint, *ba = 0);
// [ 7] bdec2_IntMap(const bdec2_IntMap& original, *ba = 0);
// [  ] bdec2_IntMap(const bdec2_IntMap& original, Hint, *ba = 0);
// [16] bdec2_IntMap(keys, values, int ne, *ba = 0);
// [  ] bdec2_IntMap(keys, values, int ne, Hint, *ba = 0);
// [ 2] ~bdec2_IntMap();
// [ 9] bdec2_IntMap& operator=(const bdec2_IntMap& rhs);
// [12] VALUE *add(int key, const VALUE& value);
// [ 4] VALUE *lookup(int key);
// [12] int remove(int key);
// [ 2] void removeAll();
// [13] void reserveCapacity(int ne);
// [12] VALUE *set(int key, const VALUE& value);
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [ 4] int isMember() const;
// [ 4] int length() const;
// [ 4] const VALUE *lookup(int key) const;
// [ 4] const int *lookupKey(int key) const;
// [11] ostream& print(ostream& stream, int level, int spl);
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
//
// [ 6] operator==(const bdec2_IntMap&, const bdec2_IntMap&);
// [ 6] operator!=(const bdec2_IntMap&, const bdec2_IntMap&);
// [ 5] operator<<(ostream&, const bdec2_IntMap&);
// [10] operator>>(bdex_InStream&, bdec2_IntMap&);
// [10] operator<<(bdex_OutStream&, const bdec2_IntMap&);
//-----------------------------------------------------------------------------
// [14] bdec2_IntMapIter(const bdec2_IntMap&);
// [14] ~bdec2_IntMapIter();
// [14] operator++();
// [14] operator const void *() const;
// [14] Int operator()() const;
//-----------------------------------------------------------------------------
// [15] bdec2_IntMapManip(bdec2_IntMap *);
// [15] ~bdec2_IntMapManip();
// [15] advance();
// [15] remove();
// [15] operator const void *() const;
// [15] Int operator()() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [ 2] BOOTSTRAP: bsl::string *add(int key, const char *value);
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] int arePrintedValuesEquivalent (const char *a, const char *b);
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 3] int ggg(bdec2_IntMap *object, const char *spec, int vF = 1);
// [ 3] bdec2_IntMap& gg(bdec2_IntMap* object, const char *spec);
// [ 8] bdec2_IntMap   g(const char *spec);

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdec2_IntMap<bsl::string> Obj;
typedef bdec2_IntMapIter<bsl::string> ObjIter;
typedef bdec2_IntMapManip<bsl::string> ObjManip;
typedef int KeyElement;
typedef const char *ValueElement;

const KeyElement KEYS[] = { 1, 2, 3, 4, -5, 6, 7, 8, 9,
                              10, 11, 12, 13, 14, 15, 16, 17 };

const KeyElement &K0 = KEYS[0], &KA = K0,  // K0, K1, ... are used in
                 &K1 = KEYS[1], &KB = K1,  // conjunction with KEYS array.
                 &K2 = KEYS[2], &KC = K2,
                 &K3 = KEYS[3], &KD = K3,  // KA, KB, ... are used in
                 &K4 = KEYS[4], &KE = K4,  // conjunction with 'g' and 'gg'.
                 &K5 = KEYS[5], &KF = K5,
                 &K6 = KEYS[6], &KG = K6,
                 &K7 = KEYS[7], &KH = K7,
                 &K8 = KEYS[8], &KI = K8;

const int NUM_KEYS = sizeof KEYS / sizeof *KEYS;

const ValueElement VALUES[] = { "a", "ab", "abc", "abcdef", "abcdefghij",
                                "abanfbdafbkadf", "afa;jfhg;jaehg;kjehgajkea",
                                "adfgafd", "sadf", "asdf", "asddf", "b", "c",
                                "d", "e", "f", "g" };

const ValueElement &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
                   &V1 = VALUES[1], &VB = V1,  // conjunction with VALUES.
                   &V2 = VALUES[2], &VC = V2,
                   &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
                   &V4 = VALUES[4], &VE = V4,  // conjunction with 'g' & 'gg'.
                   &V5 = VALUES[5], &VF = V5,
                   &V6 = VALUES[6], &VG = V6,
                   &V7 = VALUES[7], &VH = V7,
                   &V8 = VALUES[8], &VI = V8;

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

const bsl::string S0(V0), &SA = S0,
                  S1(V1), &SB = S1,
                  S2(V2), &SC = S2,
                  S3(V3), &SD = S3,
                  S4(V4), &SE = S4,
                  S5(V5), &SF = S5,
                  S6(V6), &SG = S6,
                  S7(V7), &SH = S7,
                  S8(V8), &SI = S8;

struct Assertions {
    char assertion[NUM_KEYS == NUM_VALUES];
};

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
        object->add(100 + i, "");
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

int arePrintedValuesEquivalent (const char *a, const char *b)
    // Return 1 if the specified string 'a' is a reordering of the values
    // listed in the specified string 'b', and otherwise 0.  Leading spaces on
    // each line are relevant.
{
    // Verify leading spaces are identical.

    {
        int i = 0;
        int j = 0;
        while (a[i] && b[j]) {

            // Ensure same number of leading spaces.

            while (a[i] == ' ' && b[j] == ' ') {
                ++i;
                ++j;
            }
            if (a[i] == ' ' || b[j] == ' ') {
                return 0;
            }

            // Advance to next line or end of string.

            while (a[i] && a[i] != '\n') {
                ++i;
            }
            if (a[i]) {
                ++i;
            }
            while (b[j] && b[j] != '\n') {
                ++j;
            }
            if (b[j]) {
                ++j;
            }
        }
        if (a[i] || b[j]) {
            return 0;
        }
    }


    // Create copies of the input strings.

    char *c = new char[strlen(a) + 1];
    char *d = new char[strlen(b) + 1];
    int i;
    memcpy(c, a, strlen(a) + 1);
    memcpy(d, b, strlen(b) + 1);

    // Remove the brackets, new lines, and tabs.

    for (i = 0; c[i] != '\0'; ++i) {
        if ('[' == c[i] || ']' == c[i] || '\n' == c[i] || '\t' == c[i]) {
            c[i] = ' ';
        }
    }
    for (i = 0; d[i] != '\0'; ++i) {
        if ('[' == d[i] || ']' == d[i] || '\n' == d[i] || '\t' == d[i]) {
            d[i] = ' ';
        }
    }

    // Find matches and replace matched items with spaces.

    i = 0;
    while (c[i] != '\0') {

        // Advance to next item, replacing whitespace with spaces.

        while (c[i] == ' ' && c[i] != '\0') {
            c[i] = ' ';
            ++i;
        }

        // Compute length of item.

        int len = 0;
        while (c[i + len] != ' ' && c[i + len] != '\0') {
            ++len;
        }

        if (len) {

            // Find the item in the other string.

            char oldValue = c[i + len];
            c[i + len] = '\0';
            char *s = strstr(d, &c[i]);

            // If found, erase it with spaces from both strings.
            // Otherwise, return not equivalent.

            if (s) {
                for (int j = 0; j < len; ++j) {
                    s[j] = ' ';
                    c[i + j] = ' ';
                }
            }
            else {
                delete [] d;
                delete [] c;
                return 0;
            }

            c[i + len] = oldValue;
            i += len;
        }
    }

    for (i = 0; c[i] != '\0'; ++i) {
        if (c[i] != ' ') {
            delete [] d;
            delete [] c;
            return 0;
        }
    }

    for (i = 0; d[i] != '\0'; ++i) {
        if (d[i] != ' ') {
            delete [] d;
            delete [] c;
            return 0;
        }
    }

    delete [] d;
    delete [] c;
    return 1;
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) Int values to be
// added to the 'bdec2_IntMap' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be map
// to its initial, empty state (via the 'removeAll' method).
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
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Add the value corresponding to A.
// "AA"         Add two values both corresponding to A.
// "ABC"        Add three values corresponding to A, B and C.
// "ABC~"       Add three values corresponding to A, B and C and then
//              remove all the elements (map map length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Add three values corresponding to A, B, and C; empty
//              the object; and add values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'add' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'I') {
            object->add(KEYS[spec[i] - 'A'], VALUES[spec[i] - 'A']);
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator taDefault(veryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&taDefault);
    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        {
            const int K1 = 100;
            const int K2 = 200;
            const int K3 = 300;

            const char *V1 = "a";
            const char *V2 = "ab";
            const char *V3 = "abc";

            bdec2_IntMap<bsl::string> s;    ASSERT( 0 == s.length());

            s.add(K1, V1);                  ASSERT( 1 == s.length());
                                            ASSERT( s.isMember(K1));
                                            ASSERT(!s.isMember(K2));
                                            ASSERT(!s.isMember(K3));

            s.add(K2, V2);                  ASSERT( 2 == s.length());
                                            ASSERT( s.isMember(K1));
                                            ASSERT( s.isMember(K2));
                                            ASSERT(!s.isMember(K3));

            s.add(K3, V3);                  ASSERT( 3 == s.length());
                                            ASSERT( s.isMember(K1));
                                            ASSERT( s.isMember(K2));
                                            ASSERT( s.isMember(K3));

            s.remove(K1);                   ASSERT( 2 == s.length());
                                            ASSERT(!s.isMember(K1));
                                            ASSERT( s.isMember(K2));
                                            ASSERT( s.isMember(K3));

            s.remove(K2);                   ASSERT( 1 == s.length());
                                            ASSERT(!s.isMember(K1));
                                            ASSERT(!s.isMember(K2));
                                            ASSERT( s.isMember(K3));

            s.remove(K3);                   ASSERT( 0 == s.length());

            // iterator

            for (bdec2_IntMapIter<bsl::string> it(s); it; ++it) {
                cout << it.key() << ' ' << it.value() << endl;
            }

            // manipulator

            bdec2_IntMapManip<bsl::string> manip(&s);
            while (manip) {
                if (K3 >= manip.key()) {// condition to keep current element
                    manip.advance();
                }
                else {                  // otherwise get rid of current element
                    manip.remove();
                }
            }
        }
      } break;
      case 16: {
        // TBD doc
        //bdec2_IntMap<bsl::string> x(KEYS, VALUES, NUM_VALUES);
        //ASSERT(x.length() == NUM_VALUES);
        //for (int i = 0; i < NUM_VALUES; ++i) {
        //    ASSERT(*x.lookup(KEYS[i]) == VALUES[i]);
        // }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING BDEC2_INTMAPMANIP:
        //   The basic concern is that the default constructor, the destructor,
        //   the manipulators:
        //      - advance()
        //      - remove()
        //    and the accessors:
        //      - operator const void *() const;
        //      - Int operator()() const;
        //   operate as expected.
        //
        // Plan:
        //   Create a map S of various lengths and internal representations.
        //   For each element X of S, create an empty map Y, walk the map
        //   using the iterator and populate the map Y with each found member
        //   (while verifing the value was not already present), and verify
        //   X == Y.  Also remove each element of X and use 'X.remove(value)'
        //   as an oracle to verify the resulting map.
        //
        // Testing:
        //   bdec2_IntMapManip(bdec2_IntMap *);
        //   ~bdec2_IntMapManip();
        //   advance();
        //   remove();
        //   operator const void *() const;
        //   Int operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing bdec2_IntMapManip" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nTesting bdec2_IntMapManip" << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of initial length "
                                  << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    gg(&mX, SPEC);
                    LOOP2_ASSERT(ti, ei, curLen == X.length()); // same lengths

                    Obj mY(&testAllocator);  const Obj& Y = mY;
                    for (ObjManip manip(&mX); manip; manip.advance()) {
                        LOOP_ASSERT(ti, mY.add(manip.key(), manip.value()));
                    }
                    LOOP_ASSERT(ti, X == Y);

                    // test remove
                    for (int k = 0; k < X.length(); ++k) {
                        Obj mA(X, &testAllocator);  const Obj& A = mA;
                        Obj mB(X, &testAllocator);  const Obj& B = mB;

                        int key;
                        int count = 0;
                        for (ObjManip manip(&mA); manip; ) {
                            if (k == count) {
                                key = manip.key();
                                manip.remove();
                            }
                            ++count;
                            if (manip) {
                                manip.advance();
                            }
                        }

                        int v = mB.remove(key);
                        LOOP2_ASSERT(ti, k, 1 == v);
                        LOOP2_ASSERT(ti, k, B == A);
                    }
                }
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING BDEC2_INTMAPITER:
        //   The basic concern is that the default constructor, the destructor,
        //   the manipulators 'operator++' and the accessors:
        //      - operator const void *() const;
        //      - Int operator()() const;
        //   operate as expected.
        //
        // Plan:
        //   Create a map S of various lengths and internal representations.
        //   For each element X of S, create an empty map Y, walk the map
        //   using the iterator and populate the map Y with each found member
        //   (while verifing the value was not already present), and verify
        //   X == Y.
        //
        // Testing:
        //   bdec2_IntMapIter(const bdec2_IntMap&);
        //   ~bdec2_IntMapIter();
        //   operator++();
        //   operator const void *() const;
        //   Int operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing bdec2_IntMapIter" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting bdec2_IntMapIter" << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of initial length "
                                  << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    gg(&mX, SPEC);
                    LOOP2_ASSERT(ti, ei, curLen == X.length()); // same lengths

                    Obj mY(&testAllocator);  const Obj& Y = mY;
                    for (ObjIter iter(X); iter; ++iter) {
                        LOOP_ASSERT(ti, mY.add(iter.key(), iter.value()));
                    }
                    LOOP_ASSERT(ti, X == Y);
                }
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHOD
        //   The concerns are as follows:
        //    1. capacity-reserving constructor:
        //       a. The initial value is correct (empty).
        //       b. The initial capacity is correct.
        //       c. The constructor is exception neutral w.r.t. allocation.
        //       d. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2. 'reserveCapacity' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, add as
        //   many values as the requested initial capacity, and use
        //   'bslma_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bslma_TestAllocator') cannot confirm correct
        //   capacity-reserving behavior, but can test for rudimentary correct
        //   object behavior via the destructor and Purify, and, in
        //   'veryVerbose' mode, via the print statements.
        //
        //   To test 'reserveCapacity', specify a table of initial object
        //   values and subsequent capacities to reserve.  Construct each
        //   tabulated value, call 'reserveCapacity' with the tabulated number
        //   of elements, and confirm that the test object has the same value
        //   as a separately constructed control object.  Then, add as many
        //   values as required to bring the test object's length to the
        //   specified number of elements, and use 'bslma_TestAllocator' to
        //   verify that no additional allocations have occurred.  Perform each
        //   test in the standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   bdec2_IntMap(const InitialCapacity& ne, *ba = 0);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Method" << endl
            << "=================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdec2_IntMap(capacity, ba)' Constructor" << endl;
        if (verbose) cout << "\twith a 'bslma_TestAllocator':" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BSLMA_EXCEPTION_TEST {
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE, &testAllocator);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                const int DEFAULT_BYTES = taDefault.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(K0 + i, "");
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(taDefault.numBytesInUse(),
                                   DEFAULT_BYTES == taDefault.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\twith no allocator (exercise only):" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(K0 + i, "");
                }
                if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[16384];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(K0 + i, "");
                }
                if (veryVerbose) P(X);
            }
             // No destructor is called; will produce memory leak in purify
             // if internal allocators are not hooked up properly.
        }

        if (verbose) cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_x;        // initial value
                int         d_ne;       // total number of elements to reserve
            } DATA[] = {
                //line  initialValue   numElements
                //----  ------------   -----------
                { L_,   "",              0            },
                { L_,   "",              1            },
                { L_,   "",              2            },
                { L_,   "",              3            },
                { L_,   "",              4            },
                { L_,   "",              5            },
                { L_,   "",              15           },
                { L_,   "",              16           },
                { L_,   "",              17           },

                { L_,   "A",             0            },
                { L_,   "A",             1            },
                { L_,   "A",             2            },
                { L_,   "A",             3            },
                { L_,   "A",             4            },
                { L_,   "A",             5            },
                { L_,   "A",             15           },
                { L_,   "A",             16           },
                { L_,   "A",             17           },

                { L_,   "AB",            0            },
                { L_,   "AB",            1            },
                { L_,   "AB",            2            },
                { L_,   "AB",            3            },
                { L_,   "AB",            4            },
                { L_,   "AB",            5            },
                { L_,   "AB",            15           },
                { L_,   "AB",            16           },
                { L_,   "AB",            17           },

                { L_,   "ABCDE",         0            },
                { L_,   "ABCDE",         1            },
                { L_,   "ABCDE",         2            },
                { L_,   "ABCDE",         3            },
                { L_,   "ABCDE",         4            },
                { L_,   "ABCDE",         5            },
                { L_,   "ABCDE",         15           },
                { L_,   "ABCDE",         16           },
                { L_,   "ABCDE",         17           },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_x;
                const int   NE   = DATA[ti].d_ne;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator);  const Obj &X = mX;
                mX.reserveCapacity(NE);
                LOOP_ASSERT(LINE, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.add(K0 + i, "");
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADD AND REMOVE
        //   Concerns:
        //     For the 'add' and 'remove' methods, the following properties
        //     must hold:
        //       1. The function is exception neutral (w.r.t. allocation).
        //       2. The function preserves object invariants.
        //       3. The function is independent of internal representation.
        //     Note that all (contingent) reallocations occur strictly before
        //     the essential implementation of each method.  Therefore,
        //     concern 1 above is valid for objects in the "canonical state",
        //     but need not be repeated when concern 3 ("white-box test") is
        //     addressed.
        //
        // Plan:
        //   Use the enumeration technique to create a map S of initial values.
        //   Perterb the internal representation of the map S by various
        //   amounts to obtain the map T of initial states.  For each element
        //   T, perform a handful of 'add' and 'remove' operations of elements
        //   from the universe of elements in VALUES to validate the return
        //   values while maintaining the intial value.  Finally, 'remove' all
        //   members from the universe while verifying the return value.  The
        //   method 'isMember' is used as an oracle for computing the return
        //   values of 'add' and 'remove'.
        //
        // Testing:
        //   VALUE *add(int key, const VALUE& value);
        //   int remove(int key);
        //   VALUE *set(int key, const VALUE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'add' and 'remove'" << endl
                << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting x.add(value) and x.remove(value)" << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            int k;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of initial length "
                                  << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    gg(&mX, SPEC);
                    LOOP2_ASSERT(ti, ei, curLen == X.length()); // same lengths

                    // test adding and removing while maintaining intial value
                    for (k = 0; k < NUM_KEYS; ++k) {
                        int rv;
                        int initialLength = X.length();
                        bsl::string *rp;

                        int erv = !X.isMember(KEYS[k]);
                        rp = mX.add(KEYS[k], VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k,
                                  erv ? 0 == (int)strcmp(rp->c_str(), VALUES[k])
                                      : 0 == rp);

                        rp = mX.add(KEYS[k], VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, 0 == rp);

                        rv = mX.remove(KEYS[k]);
                        LOOP3_ASSERT(ti, ei, k, 1 == rv);

                        rv = mX.remove(KEYS[k]);
                        LOOP3_ASSERT(ti, ei, k, 0 == rv);

                        rp = mX.add(KEYS[k], VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k,
                                      0 == (int)strcmp(rp->c_str(), VALUES[k]));

                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength + erv == X.length());

                        rp = mX.set(KEYS[k], VALUES[(k + 1) % NUM_VALUES]);
                        LOOP3_ASSERT(ti, ei, k, VALUES[(k + 1) % NUM_VALUES]
                                                                       == *rp);

                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength + erv == X.length());

                        rv = mX.remove(KEYS[k]);
                        LOOP3_ASSERT(ti, ei, k, 1 == rv);

                        rp = mX.set(KEYS[k], VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, VALUES[k] == *rp);

                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength + erv == X.length());
                    }

                    // test removing everything
                    for (k = 0; k < NUM_KEYS; ++k) {
                        int initialLength = X.length();
                        int erv = X.isMember(KEYS[k]);
                        int rv = mX.remove(KEYS[k]);
                        LOOP3_ASSERT(ti, ei, k, erv == rv);
                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength - erv == X.length());
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //   The print method formats the value of the object directly from
        //   the underlying state information according to supplied arguments.
        //   Ensure that the method formats properly for:
        //     - empty and non-empty values
        //     - negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        // Plan:
        //   For each of an enumerated map of object, 'level', and
        //   'spacesPerLevel' values, ordered by increasing object length, use
        //   'ostrstream' to 'print' that object's value, using the tabulated
        //   parameters, to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spacesPerLevel);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'print' method" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ------------------
                { L_,   "",        0,         0,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        0,         2,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        1,         1,        " ["          NL
                                                        " ]"          NL  },

                { L_,   "",        1,         2,        "  ["         NL
                                                        "  ]"         NL  },

                { L_,   "",       -1,         2,        "["           NL
                                                        "  ]"         NL  },

                { L_,   "A",       0,         0,        "["           NL
                                                        "(1,a)"       NL
                                                        "]"           NL  },

                { L_,   "A",      -2,         1,        "["           NL
                                                        "   (1,a)"    NL
                                                        "  ]"         NL  },

                { L_,   "BC",      1,         2,        "  ["         NL
                                                        "    (2,ab)"  NL
                                                        "    (3,abc)" NL
                                                        "  ]"         NL  },

                { L_,   "BC",      2,         1,        "  ["         NL
                                                        "   (2,ab)"   NL
                                                        "   (3,abc)"  NL
                                                        "  ]"         NL  },

                { L_,   "ABCDE",   1,         3, "   ["                  NL
                                                 "      (1,a)"           NL
                                                 "      (2,ab)"          NL
                                                 "      (3,abc)"         NL
                                                 "      (4,abcdef)"      NL
                                                 "      (-5,abcdefghij)" NL
                                                 "   ]"                  NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xFF;  // Value 1 used for an unmap char.
            const char Z2 = 0x00;  // Value 2 used to represent an unmap char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Premap buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Premap buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                               P_(IND); P(SPL); }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, arePrintedValuesEquivalent(buf1, FMT));
                LOOP_ASSERT(ti, arePrintedValuesEquivalent(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective member functions 'streamOut' and
        //      'streamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        //   Plan:
        //     First perform a trivial direct (breathing) test of the
        //     'outStream' and 'inStream' methods (to address concern 1).  Note
        //     that the rest of the testing will use the stream operators.
        //
        //     Next, specify a map S of unique object values with substantial
        //     and varied differences, ordered by increasing length.  For each
        //     value in S, construct an object x along with a sequence of
        //     similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //     affect every aspect of white-box state by altering each xi in
        //     a unique way.  Let the union of all such objects be the map T.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in T X T, stream-out the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in T, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fail every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        // Testing:
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   operator>>(bdex_InStream&, bdec2_IntMap&);
        //   operator<<(bdex_OutStream&, const bdec2_IntMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(g("ABC"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bdex_TestOutStream out;  out << X;

            const char *const OD  = out.data();
            const int         LOD = out.length();

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g("DE"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            in >> t;                         ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
            ASSERT(X == t);
        }

        if (verbose) cout <<
            "\nTesting stream operators ('<<' and '>>')." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5,
                "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);     stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;          gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out << U;   // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must remap stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = strlen(V_SPEC);

                        const Obj VV = g(V_SPEC);               // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          BEGIN_BSLMA_EXCEPTION_TEST {
                            testInStream.reset();
                            const int V_N = EXTEND[vj];
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;      gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testAllocator.setAllocationLimit(AL);
                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BSLMA_EXCEPTION_TEST

                          BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reset();
                            const int V_N = EXTEND[vj];
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;      gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BDEX_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bdex_TestInStream testInStream("", 0);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    testInStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    Obj t(&testAllocator);      gg(&t, SPEC);
                    stretchRemoveAll(&t, N);    gg(&t, SPEC);

                  // Ensure that reading from an empty or invalid input stream
                  // leaves the stream invalid and the target object unchanged.

                                        LOOP2_ASSERT(ti, ei, testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj X1 = g("CDCD"),   Y1 = g("BB"),    Z1 = g("ABCDE");
            const Obj X2 = g("ADE"),    Y2 = g("CABDE"), Z2 = g("B");
            const Obj X3 = g("DEEDDE"), Y3 = g("C"),     Z3 = g("DBED");

            bdex_TestOutStream out;
            out << Y1;  const int LOD1 = out.length();
            out << Y2;  const int LOD2 = out.length();
            out << Y3;  const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream testInStream(OD, i);
                bdex_TestInStream& in = testInStream;
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i, !in);
                                        if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
                else if (i < LOD2) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);
                                     if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
                else {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);
                                     if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);

            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W = g("");            // default value
        const Obj X = g("ABCDE");       // original value
        const Obj Y = g("DCB");         // new value

        /* TBD need these tests
        enum { A_LEN = 3 };
        KeyElement a[A_LEN];  a[0] = KD;  a[1] = KC;  a[2] = KB;
        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out << version << length;
            out << KD << VD;
            out << KC << VC;
            out << KB << VB;
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(in);
                        ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small
            const int length   = 3;

            bdex_TestOutStream out;
            out << version << length;
            out << KD << VD;
            out << KC << VC;
            out << KB << VB;
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); in.setSuppressVersionCheck(1);
                                           ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char version = 2; // too large
            const int length   = 3;

            bdex_TestOutStream out;
            out << version << length;
            out << KD << VD;
            out << KC << VC;
            out << KB << VB;
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad length." << endl;
        {
            const char version = 1;
            const int length   = -1; // too small

            bdex_TestOutStream out;
            out << version << length;
            out << KD << VD;
            out << KC << VC;
            out << KB << VB;
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tRepeated value." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out << version << length;
            out << KD << VD;
            out << KC << VC;
            out << KD << VD;
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
            // value of the map is unknown
        }
        */
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //         other instance regardless of how either value is represented
        //         internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //         even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        // Plan:
        //   Specify a map S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the map T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
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
        //   bdec2_IntMap& operator=(const bdec2_IntMap& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = strlen(U_SPEC);

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
                    const int vLen = strlen(V_SPEC);

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

                          BEGIN_BSLMA_EXCEPTION_TEST {
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
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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

                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 8: {
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
        //   bdec2_IntMap g(const char *spec);
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
        // TESTING COPY CONSTRUCTOR:
        //   We have the following concerns:
        //   1. The new object's value is the same as that of the original
        //       object (relying on the previously tested equality operators).
        //   2. All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3. The value of the original object is left unaffected.
        //   4. Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   5. The function is exception neutral w.r.t. memory allocation.
        //   6. The object has its internal memory management system hooked up
        //         properly so that *all* internally allocated memory draws
        //         from a user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   To address concerns 1 - 3, specify a map S of object values with
        //   substantial and varied differences, ordered by increasing length.
        //   For each value in S, initialize objects w and x, copy construct y
        //   from x and use 'operator==' to verify that both x and y
        //   subsequently have the same value as w.  Let x go out of scope and
        //   again verify that w == x.  Repeat this test with x having the same
        //   *logical* value, but perturbed so as to have potentially different
        //   internal representations.
        //
        //   To address concern 5, we will perform each of the above tests in
        //   the presence of exceptions during memory allocations using a
        //   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //
        //   To address concern 6, we will repeat the above tests:
        //     - When passing in no allocator.
        //     - When passing in a null pointer: (bslma_Allocator *)0.
        //     - When passing in a test allocator (see concern 5).
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferedSequentialAllocator') and never
        //       destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        // Testing:
        //   bdec2_IntMap(const bdec2_IntMap& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = strlen(SPEC);

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

                    {                                   // No allocator.
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bslma_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BEGIN_BSLMA_EXCEPTION_TEST {        // Test allocator.
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } END_BSLMA_EXCEPTION_TEST

                    {                                   // Buffer Allocator.
                        char memory[1024];
                        bdema_BufferedSequentialAllocator a(memory,
                                                            sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);

                        // testAllocator will erase the footprint of pX
                        // preventing further reference to this object.

                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                    }
                }
            }
        }
      } break;
    case 6: { // TBD same key, different value tests
        // --------------------------------------------------------------------
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
        //   Note also that both equality operators must return either 1 or 0,
        //   and neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //   First specify a map S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   1 or 0) using all elements (u, v) of the cross product S X S.
        //
        //   Next specify a second map S' containing a representative variety
        //   of (black-box) box values ordered by increasing (logical) length.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to every other element.
        //
        // Testing:
        //   operator==(const bdec2_IntMap&, const bdec2_IntMap&);
        //   operator!=(const bdec2_IntMap&, const bdec2_IntMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_lineNum;       // source line number
              const char *d_spec;  // string
            } DATA[] = {
                //line spec
                //---- ---------------------
                { L_,  ""                 },
                { L_,  "A"                },
                { L_,  "B"                },
                { L_,  "I"                },
                { L_,  "AB"               },
                { L_,  "BC"               },
                { L_,  "CD"               },
                { L_,  "ABC"              },
                { L_,  "BCD"              },
                { L_,  "CDE"              },
                { L_,  "DEF"              },
                { L_,  "ABCD"             },
                { L_,  "BCDE"             },
                { L_,  "CDEF"             },
                { L_,  "DEFG"             },
                { L_,  "EFGH"             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int curLen = strlen(DATA[i].d_spec);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(i, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator);  const Obj& X = mX;
                gg(&mX,DATA[i].d_spec);

                if (veryVerbose) { P_(i); P(DATA[i].d_spec); P(X); }

                for (int j = 0; j < NUM_DATA; ++j) {
                    Obj mY(&testAllocator);  const Obj& Y = mY;
                    gg(&mY,DATA[j].d_spec);

                    if (veryVerbose) {
                        cout << "  ";
                        P_(j);
                        P_(DATA[j].d_spec);
                        P(Y);
                    }

                    int r = 1;

                    for (int k = 0; k < NUM_KEYS; ++k) {
                        if (X.isMember(KEYS[k]) != Y.isMember(KEYS[k])) {
                            r = 0;
                        }
                    }

                    LOOP2_ASSERT(i, j,  r == (X == Y));
                    LOOP2_ASSERT(i, j,  1 == (X == X));
                    LOOP2_ASSERT(i, j,  1 == (Y == Y));
                    LOOP2_ASSERT(i, j, !r == (X != Y));
                    LOOP2_ASSERT(i, j,  0 == (X != X));
                    LOOP2_ASSERT(i, j,  0 == (Y != Y));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const struct {
                int d_lineNum;       // source line number
              const char *d_spec;  // string
            } DATA[] = {
                //line spec
                //---- ---------------------
                { L_,  ""                 },
                { L_,  "A"                },
                { L_,  "B"                },
                { L_,  "I"                },
                { L_,  "AB"               },
                { L_,  "BC"               },
                { L_,  "CD"               },
                { L_,  "ABC"              },
                { L_,  "BCD"              },
                { L_,  "CDE"              },
                { L_,  "DEF"              },
                { L_,  "ABCD"             },
                { L_,  "BCDE"             },
                { L_,  "CDEF"             },
                { L_,  "DEFG"             },
                { L_,  "EFGH"             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int curLen = strlen(DATA[i].d_spec);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(i, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                for (int ie = 0; ie < NUM_EXTEND; ++ie) {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ie]);
                    gg(&mX,DATA[i].d_spec);

                    for (int j = 0; j < NUM_DATA; ++j) {
                        for (int je = 0; je < NUM_EXTEND; ++je) {
                            Obj mY(&testAllocator);  const Obj& Y = mY;
                            stretchRemoveAll(&mY, EXTEND[je]);
                            gg(&mY,DATA[j].d_spec);

                            int r = 1;

                            for (int k = 0; k < NUM_KEYS; ++k) {
                                if (X.isMember(KEYS[k]) !=
                                    Y.isMember(KEYS[k])) {
                                  r = 0;
                                }
                            }

                            LOOP2_ASSERT(i, j,  r == (X == Y));
                            LOOP2_ASSERT(i, j,  1 == (X == X));
                            LOOP2_ASSERT(i, j,  1 == (Y == Y));
                            LOOP2_ASSERT(i, j, !r == (X != Y));
                            LOOP2_ASSERT(i, j,  0 == (X != X));
                            LOOP2_ASSERT(i, j,  0 == (Y != Y));
                        }
                    }
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative map of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.  Note
        //   that the output ordering is not guaranteed and the function
        //   'arePrintedValuesEquivalent' is used to validate equality of the
        //   output to the expected output.
        //
        // Testing:
        //   operator<<(ostream&, const bdec2_IntMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec            output format                // ADJUST
                //----  --------------  ---------------------------------
                { L_,   "",             "[ ]"                           },
                { L_,   "A",            "[ (1,a) ]"                     },
                { L_,   "BC",           "[ (2,ab) (3,abc) ]"            },
                { L_,   "ABCDE",
                  "[ (1,a) (2,ab) (3,abc) (4,abcdef) (-5,abcdefghij) ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = 0x7f;  // Value 1 used to represent an unmap char.
            const char Z2 = 0x00;  // Value 2 used to represent an unmap char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Premap buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Premap buf2 to Z2 values.

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
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, arePrintedValuesEquivalent(buf1, FMT));
                LOOP_ASSERT(ti, arePrintedValuesEquivalent(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

      } break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions
        //     - length() const
        //     - isMember(Int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.
        //
        // Plan:
        //   Specify a map S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        // Testing:
        //   int length() const;
        //   int isMember(Int value) const;
        //   VALUE *lookup(int key);
        //   const VALUE *lookup(int key) const;
        //   const int *lookupKey(int key) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting 'length' & 'isMember'" << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                KeyElement  d_elements[SZ];     // exected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },
                { L_,   "A",            1,      { KA }                  },
                { L_,   "B",            1,      { KB }                  },
                { L_,   "AA",           1,      { KA }                  },
                { L_,   "AB",           2,      { KA, KB }              },
                { L_,   "BC",           2,      { KB, KC }              },
                { L_,   "ABAB",         2,      { KA, KB }              },
                { L_,   "ABABAB",       2,      { KA, KB }              },
                { L_,   "ABC",          3,      { KA, KB, KC }          },
                { L_,   "ABCABC",       3,      { KA, KB, KC }          },
                { L_,   "ABCABCABC",    3,      { KA, KB, KC }          },
                { L_,   "BCA",          3,      { KB, KC, KA }          },
                { L_,   "CAB",          3,      { KC, KA, KB }          },
                { L_,   "CDAB",         4,      { KC, KD, KA, KB }      },
                { L_,   "DABC",         4,      { KD, KA, KB, KC }      },
                { L_,   "ABCDE",        5,      { KA, KB, KC, KD, KE }  },
                { L_,   "EDCBA",        5,      { KA, KB, KC, KD, KE }  },
                { L_,   "ABCDEAB",      5,      { KA, KB, KC, KD, KE }  },
                { L_,   "BACDEABC",     5,      { KA, KB, KC, KD, KE }  },
                { L_,   "CBADEABCD",    5,      { KA, KB, KC, KD, KE }  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE            = DATA[ti].d_lineNum;
                const char *const SPEC    = DATA[ti].d_spec_p;
                const int LENGTH          = DATA[ti].d_length;
                const KeyElement *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

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
                for (i = 0; i < NUM_KEYS; ++i) {
                    int ev = 0;
                    for (int j = 0; j < LENGTH; ++j) {
                        if (KEYS[i] == e[j]) {
                            ev = 1;
                        }
                    }
                    LOOP2_ASSERT(LINE, i, ev == X.isMember(KEYS[i]));
                    LOOP2_ASSERT(LINE, i, ev == Y.isMember(KEYS[i]));

                    {
                        bsl::string *xp = mX.lookup(KEYS[i]);
                        bsl::string *yp = mY.lookup(KEYS[i]);
                        LOOP2_ASSERT(LINE, i, ev == (0 != xp));
                        LOOP2_ASSERT(LINE, i, ev == (0 != yp));
                        LOOP2_ASSERT(LINE, i, !xp || VALUES[i] == *xp);
                        LOOP2_ASSERT(LINE, i, !yp || VALUES[i] == *yp);
                    }

                    {
                        const bsl::string *xp = X.lookup(KEYS[i]);
                        const bsl::string *yp = Y.lookup(KEYS[i]);
                        LOOP2_ASSERT(LINE, i, ev == (0 != xp));
                        LOOP2_ASSERT(LINE, i, ev == (0 != yp));
                        LOOP2_ASSERT(LINE, i, !xp || VALUES[i] == *xp);
                        LOOP2_ASSERT(LINE, i, !yp || VALUES[i] == *yp);
                    }

                    {
                        const int *xp = mX.lookupKey(KEYS[i]);
                        const int *yp = mY.lookupKey(KEYS[i]);
                        LOOP2_ASSERT(LINE, i, ev == (0 != xp));
                        LOOP2_ASSERT(LINE, i, ev == (0 != yp));
                        LOOP2_ASSERT(LINE, i, !xp || KEYS[i] == *xp);
                        LOOP2_ASSERT(LINE, i, !yp || KEYS[i] == *yp);
                    }

                    {
                        const int *xp = X.lookupKey(KEYS[i]);
                        const int *yp = Y.lookupKey(KEYS[i]);
                        LOOP2_ASSERT(LINE, i, ev == (0 != xp));
                        LOOP2_ASSERT(LINE, i, ev == (0 != yp));
                        LOOP2_ASSERT(LINE, i, !xp || KEYS[i] == *xp);
                        LOOP2_ASSERT(LINE, i, !yp || KEYS[i] == *yp);
                    }
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION gg
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify (1) that valid
        //   generator syntax produces expected results and (2) that invalid
        //   syntax is detected and reported.
        //
        //   We want also to make trustworthy some additional test helper
        //   functionality that we will use within the first 10 test cases:
        //    - 'stretch'          Tested separately to observe stretch occurs.
        //    - 'stretchRemoveAll' Deliberately implemented using 'stretch'.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to map the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'removeAll').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   To verify that the stretching functions work as expected (and to
        //   cross-check that internal memory is being managed as intended),
        //   create a depth-ordered enumeration of initial values and sizes by
        //   which to extend the initial value.  Record as expected values the
        //   total number of memory blocks allocated during the first and
        //   second modifications of each object.  For each test vector,
        //   construct two identical objects X and Y and bring each to the
        //   initial state.  Assert that the memory allocation for the two
        //   operations are identical and consistent with the first expected
        //   value.  Next apply the 'stretch' and 'stretchRemoveAll' functions
        //   to X and Y (respectively) and again compare the memory allocation
        //   characteristics for the two functions.  Note that we will track
        //   the *total* number of *blocks* allocated as well as the *current*
        //   number of *bytes* in use -- this to measure different aspects of
        //   operation while remaining insensitive to the map 'Element' size.
        //
        //   For 'arePrintedValuesEquivalent', an enumerated sequence of
        //   specifications and results is created and the method's return
        //   value is compared to the expected return value.
        //
        // Testing:
        //   bdec2_IntMap& gg(bdec2_IntMap* object, const char *spec);
        //   int ggg(bdec2_IntMap *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int            d_lineNum;          // source line number
                const char    *d_spec_p;           // specification string
                int            d_length;           // expected length
                KeyElement     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },

                { L_,   "A",            1,      { KA }                  },
                { L_,   "B",            1,      { KB }                  },
                { L_,   "~",            0,      { 0 }                   },

                { L_,   "CD",           2,      { KC, KD }              },
                { L_,   "E~",           0,      { 0 }                   },
                { L_,   "~E",           1,      { KE }                  },
                { L_,   "~~",           0,      { 0 }                   },

                { L_,   "ABC",          3,      { KA, KB, KC }          },
                { L_,   "~BC",          2,      { KB, KC }              },
                { L_,   "A~C",          1,      { KC }                  },
                { L_,   "AB~",          0,      { 0 }                   },
                { L_,   "~~C",          1,      { KC }                  },
                { L_,   "~B~",          0,      { 0 }                   },
                { L_,   "A~~",          0,      { 0 }                   },
                { L_,   "~~~",          0,      { 0 }                   },

                { L_,   "ABCD",         4,      { KA, KB, KC, KD }      },
                { L_,   "~BCD",         3,      { KB, KC, KD }          },
                { L_,   "A~CD",         2,      { KC, KD }              },
                { L_,   "AB~D",         1,      { KD }                  },
                { L_,   "ABC~",         0,      { 0 }                   },

                { L_,   "ABCDE",        5,      { KA, KB, KC, KD, KE }  },
                { L_,   "~BCDE",        4,      { KB, KC, KD, KE }      },
                { L_,   "AB~DE",        2,      { KD, KE }              },
                { L_,   "ABCD~",        0,      { 0 }                   },
                { L_,   "A~C~E",        1,      { KE }                  },
                { L_,   "~B~D~",        0,      { 0 }                   },

                { L_,   "~CBA~~ABCDE",  5,      { KA, KB, KC, KD, KE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { KE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE            = DATA[ti].d_lineNum;
                const char *const SPEC    = DATA[ti].d_spec_p;
                const int LENGTH          = DATA[ti].d_length;
                const KeyElement *const e = DATA[ti].d_elements;
                const int curLen = strlen(SPEC);

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
                    LOOP2_ASSERT(LINE, i, X.isMember(e[i]));
                    LOOP2_ASSERT(LINE, i, Y.isMember(e[i]));
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
                { L_,   "J",             0,     },

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
                const int curLen       = strlen(SPEC);

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

                { L_,   "",             1,      0,              1       },
                { L_,   "A",            0,      3,              0       },

                { L_,   "",             2,      0,              2       },
                { L_,   "A",            1,      3,              1       },
                { L_,   "AB",           0,      6,              0       },

                { L_,   "",             3,      0,              3       },
                { L_,   "A",            2,      3,              2       },
                { L_,   "AB",           1,      6,              1       },
                { L_,   "ABC",          0,      9,              0       },

                { L_,   "",             4,      0,              4       },
                { L_,   "A",            3,      3,              3       },
                { L_,   "AB",           2,      6,              2       },
                { L_,   "ABC",          1,      9,              1       },
                { L_,   "ABCD",         0,     12,              0       },

                { L_,   "",             5,      0,              4       },
                { L_,   "A",            4,      3,              3       },
                { L_,   "AB",           3,      6,              2       },
                { L_,   "ABC",          2,      9,              1       },
                { L_,   "ABCD",         1,     12,              0       },
                { L_,   "ABCDE",        0,     14,              0       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = strlen(SPEC);
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

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int bytes3A = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int bytes12A = bytes2A - bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int bytes23A = bytes3A - bytes2A;

                    if (veryVerbose) { P_(bytes12A);  P_(bytes23A);
                                       P_(blocks12A); P(blocks23A); }

                    LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                    LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

                    LOOP2_ASSERT(LINE, blocks12A, firstResize == blocks12A);

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE, bytes12A == bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {

                    int blocks1B = testAllocator.numBlocksTotal();
                    int bytes1B = testAllocator.numBytesInUse();

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int bytes3B = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int bytes12B = bytes2B - bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int bytes23B = bytes3B - bytes2B;

                    if (veryVerbose) { P_(bytes12B);  P_(bytes23B);
                                       P_(blocks12B); P(blocks23B); }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B >= bytes23B);
                }
            }
        }

        if (verbose) cout << "\nTesting 'arePrintedValuesEquivalent'." << endl;
        {
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_a;        // first source string
                const char *d_b;        // second source string
                int         d_rv;       // expected return value
            } DATA[] = {
                //line     A                       B                    rv
                //----  ----------------------  ----------------------  --
                { L_,   "[ ]",                  "[  ]",                  1 },
                { L_,   "[ ]",                  "[ 1.5 ]",               0 },
                { L_,   "[ ]",                  "[ 2.5 3.5 ]",           0 },
                { L_,   "[ ]",                  "[ 1.5 2.5 3.5 4.75 ]",  0 },
                { L_,   "[ 1.5 ]" ,             "[ ]",                   0 },
                { L_,   "[ 1.5 ]" ,             "[ 1.5 ]",               1 },
                { L_,   "[ 1.5 ]" ,             "[ 2.5 3.5 ]",           0 },
                { L_,   "[ 1.5 ]" ,             "[ 1.5 2.5 3.5 4.75 ]",  0 },
                { L_,   "[ 2.5 3.5 ]",          "[ ]",                   0 },
                { L_,   "[ 2.5 3.5 ]",          "[ 1.5 ]",               0 },
                { L_,   "[ 2.5 3.5 ]",          "[ 2.5 3.5 ]",           1 },
                { L_,   "[ 2.5 3.5 ]",          "[ 1.5 2.5 3.5 4.75 ]",  0 },
                { L_,   "[ 1.5 2.5 3.5 4.75 ]", "[ ]",                   0 },
                { L_,   "[ 1.5 2.5 3.5 4.75 ]", "[ 1.5 ]",               0 },
                { L_,   "[ 1.5 2.5 3.5 4.75 ]", "[ 2.5 3.5 ]",           0 },
                { L_,   "[ 1.5 2.5 3.5 4.75 ]", "[ 1.5 2.5 3.5 4.75 ]",  1 },

                { L_,   "  [ ]",                "[  ]",                  0 },
                { L_,   "  [ ]",                "  [  ]",                1 },
                { L_,   "[ 2.5" NL "3.5 ]",     "[ 2.5 3.5 ]",           0 },
                { L_,   "[ 2.5" NL "3.5 ]",     "[ 2.5" NL " 3.5 ]",     0 },
                { L_,   "[ 2.5" NL "3.5 ]",     "[ 2.5" NL "3.5 ]",      1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_lineNum;
                const char *const A = DATA[ti].d_a;
                const char *const B = DATA[ti].d_b;
                const int RV        = DATA[ti].d_rv;

                LOOP_ASSERT(LINE, RV == arePrintedValuesEquivalent(A, B));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions, the primary manipulators
        //      - add                   (black-box)
        //      - removeAll             (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default Constructor
        //        1a. creates the correct initial value.
        //        1b. is exception neutral with respect to memory allocation.
        //        1c. has the internal memory management system hooked up
        //              properly so that *all* internally allocated memory
        //              draws from the same user-supplied allocator whenever
        //              one is specified.
        //    2. The destructor properly deallocates all allocated memory to
        //         its corresponding allocator from any attainable state.
        //    3. 'add'
        //        3a. produces the expected value.
        //        3b. increases capacity as needed.
        //        3c. maintains valid internal state.
        //        3d. is exception neutral with respect to memory allocation.
        //    4. 'removeAll'
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
        //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferedSequentialAllocator') and never
        //        destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent
        //   objects, ordered by increasing length.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final add operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object
        //   and the 'bslma_TestAllocator' to confirm whether a resize has
        //   occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'add' with 'removeAll'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'add'
        //          to create an instance of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'add' to create an
        //          instance of length i, use 'removeAll' to erase its value
        //          and confirm (with 'length'), use add to map the instance
        //          to a value of length j, verify the value, and allow the
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bslma_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        //   Note that 'add' is not tested to ensure non-duplicate values
        //   during this bootstrap test.
        //
        // Testing:
        //   bdec2_IntMap(bslma_Allocator *ba);
        //   ~bdec2_IntMap();
        //   BOOTSTRAP: bsl::string *add(int key, const char *value);
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

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

        if (verbose) cout << "\t\tWith execeptions." << endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.add(K0, S0);
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(1 == X.length());
            ASSERT(X.isMember(K0));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, S0);

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.add(K1, S1);
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(2 == X.length());
            ASSERT(X.isMember(K0));
            ASSERT(X.isMember(K1));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, S0); mX.add(K1, S1);

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.add(K2, S2);
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(3 == X.length());
            ASSERT(X.isMember(K0));
            ASSERT(X.isMember(K1));
            ASSERT(X.isMember(K2));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, S0); mX.add(K1, S1); mX.add(K2, S2);

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.add(K3, S3);
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(4 == X.length());
            ASSERT(X.isMember(K0));
            ASSERT(X.isMember(K1));
            ASSERT(X.isMember(K2));
            ASSERT(X.isMember(K3));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, S0); mX.add(K1, S1); mX.add(K2, S2); mX.add(K3, S3);

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.add(K4, S4);
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(5 == X.length());
            ASSERT(X.isMember(K0));
            ASSERT(X.isMember(K1));
            ASSERT(X.isMember(K2));
            ASSERT(X.isMember(K3));
            ASSERT(X.isMember(K4));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.length());

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.removeAll();
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, V0);
            ASSERT(1 == X.length());

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.removeAll();
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, V0); mX.add(K1, V1);
            ASSERT(2 == X.length());

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.removeAll();
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(K0, V0); mX.add(K1, V1); mX.add(K2, V2);
            ASSERT(3 == X.length());

            const int DEFAULT_BEFORE = taDefault.numBytesInUse();
            mX.removeAll();
            ASSERT(taDefault.numBytesInUse() == DEFAULT_BEFORE)

            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'add' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. add { K0, K1, K2, K3, K4, K0, ... }  up to length i
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              BEGIN_BSLMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.add(KEYS[k % NUM_KEYS], VALUES[k % NUM_VALUES]);
                }

                const int len = i > NUM_KEYS ? NUM_KEYS : i;
                LOOP_ASSERT(i, len == X.length());                      // 3.
                for (k = 0; k < i; ++k) {
                  LOOP2_ASSERT(i, k, X.isMember(KEYS[k % NUM_KEYS]));
                }

              } END_BSLMA_EXCEPTION_TEST                                // 4.
            }
        }

        if (verbose) cout << "\tWith 'add' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. add values up to a length of i
            //    3. verify initial length and contents
            //    4. removeAll contents from instance
            //    5. verify length is 0
            //    6. add { K0, K1, K2, K3, K4, K0, ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.add(KEYS[k % NUM_KEYS], VALUES[k % NUM_VALUES]);
                        LOOP3_ASSERT(i, j, k, X.isMember(KEYS[k % NUM_KEYS]));
                    }

                    const int len = i > NUM_KEYS ? NUM_KEYS : i;
                    LOOP2_ASSERT(i, j, len == X.length());              // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, X.isMember(KEYS[k % NUM_KEYS]));
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.add(KEYS[k % NUM_KEYS], VALUES[k % NUM_VALUES]);
                        LOOP3_ASSERT(i, j, k, X.isMember(KEYS[k % NUM_KEYS]));
                    }

                    const int len2 = j > NUM_KEYS ? NUM_KEYS : j;
                    LOOP2_ASSERT(i, j, len2 == X.length());             // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, X.isMember(KEYS[k % NUM_KEYS]));
                    }
                  } END_BSLMA_EXCEPTION_TEST                            // 8.
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
        //      - the output operator: 'operator<<'
        //      - primary manipulators: 'add' and 'removeAll' methods
        //      - basic accesors: 'length' and 'operator[]'
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
        // 3. Add an element value A to x1).         { x1:A x2: }
        // 4. Add the same element value A to x2).   { x1:A x2:A }
        // 5. Add another element value B to x2).    { x1:A x2:AB }
        // 6. Remove all elements from x1.              { x1: x2:AB }
        // 7. Create a third object x3 (default ctor).  { x1: x2:AB x3: }
        // 8. Create a fourth object x4 (copy of x2).   { x1: x2:AB x3: x4:AB }
        // 9. Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
        // 10. Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        // 11. Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

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
        ASSERT(0 == X1.length());

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
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Add an element value A to x1)."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.add(KA, VA);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(X1.isMember(KA));

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Add the same element value A to x2)."
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.add(KA, VA);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(X2.isMember(KA));

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Add another element value B to x2)."
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.add(KB, VB);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(X2.isMember(KA));
        ASSERT(X2.isMember(KB));

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.length());

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
        ASSERT(0 == X3.length());

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

        LOOP_ASSERT(X4.length(), 2 == X4.length());
        ASSERT(X4.isMember(KA));
        ASSERT(X4.isMember(KB));
        ASSERT(0 == X4.isMember(KC));

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
        ASSERT(0 == X2.length());

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
        ASSERT(2 == X3.length());

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
        ASSERT(2 == X4.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 0);          ASSERT(X4 != X2 == 1);
        ASSERT(X4 == X3 == 1);          ASSERT(X4 != X3 == 0);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

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
