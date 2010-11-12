// bdeci_hashtable.t.cpp           -*-C++-*-

#include <bdeci_hashtable.h>
#include <bdeci_hashtableimputil.h>

#include <bslma_bufferallocator.h>              // for testing only
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_platformutil.h>                  // for testing only

#include <bdeimp_int64hash.h>
#include <bdeimp_inthash.h>
#include <bdeimp_strhash.h>

#include <bsl_iostream.h>
#include <bsl_new.h>         // placement syntax
#include <bsl_string.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

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
// component.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 2] bdeci_Hashtable(bslma_Allocator *ba = 0);
// [13] bdeci_Hashtable(const InitialCapacity& ne, *ba = 0);
// [ 7] bdeci_Hashtable(const bdeci_Hashtable& original, *ba = 0);
// [ 2] ~bdeci_Hashtable();
// [ 9] bdeci_Hashtable& operator=(const bdeci_Hashtable& rhs);
// [12] int add(double value);
// [12] int addUnique(double value);
// [15] void compact();
// [12] int removeEvery(double value);
// [12] int removeFirst(double value);
// [ 2] void removeAll();
// [13] void reserveCapacity(int ne);
// [16] int hash(double value) const;
// [ 4] int isMember() const;
// [ 4] int numElements() const;
// [ 4] int numOccurrences() const;
// [ 4] int numSlots() const;
// [11] ostream& print(ostream& stream, int level, int spl);
//
// [ 6] operator==(const bdeci_Hashtable&, const bdeci_Hashtable&);
// [ 6] operator!=(const bdeci_Hashtable&, const bdeci_Hashtable&);
// [ 5] operator<<(ostream&, const bdeci_Hashtable&);
//-----------------------------------------------------------------------------
// [ 4] bdeci_HashtableSlotIter(const bdeci_Hashtable&, int si);
// [ 4] ~bdeci_HashtableSlotIter();
// [ 4] operator++();
// [ 4] void setSlot(int slotIndex);
// [ 4] operator const void *() const;
// [ 4] double operator()() const;
//-----------------------------------------------------------------------------
// [14] bdeci_HashtableSlotManip(bdeci_Hashtable *, int si);
// [14] ~bdeci_HashtableSlotManip();
// [14] addRaw();
// [14] advance();
// [14] remove();
// [14] void setSlot(int slotIndex);
// [14] operator const void *() const;
// [14] double operator()() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void add(double value);
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] Element dataForSlot(int slot, int index, int numSlots);
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 3] int ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ 8] Obj   g(const char *spec);

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

//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct my_DoubleHash {
    // This 'struct' provides a namespace for a function used to implement a
    // hash table of 'double' values.

    // CLASS METHODS
    static int hash(double value, int modulus);
        // Return an integer in the range from zero to one less than the
        // specified 'modulus' corresponding to the specified 'value'.
        // The behavior is undefined unless '0 < modulus < 2^31'.  Note that
        // 'modulus' is expected to be a prime not close to an integral power
        // of 2.  Also note that specifying a 'modulus' of 1 will cause 0 to be
        // returned for every 'value'.
};

int my_DoubleHash::hash(double value, int modulus)
{
    bsls_PlatformUtil::Int64 *v = (bsls_PlatformUtil::Int64 *)&value;
    return bdeimp_Int64Hash::hash(*v, modulus);
}

typedef double Element;
typedef my_DoubleHash ElementHash;
typedef bdeci_Hashtable<Element, ElementHash> Obj;
typedef bdeci_HashtableSlotIter<Element, ElementHash> ObjIter;
typedef bdeci_HashtableSlotManip<Element, ElementHash> ObjManip;

const Element VALUES[] = { 1.5, 2.5, 3.5, 4.75, -5.25, 6.0, 7.0, 8.0, 9.0,
                           10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0 };

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4,  // conjunction with 'g' and 'gg'.
              &V5 = VALUES[5], &VF = V5,
              &V6 = VALUES[6], &VG = V6,
              &V7 = VALUES[7], &VH = V7,
              &V8 = VALUES[8], &VI = V8;

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

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
        object->add(100.0 + i);
    }
    ASSERT(object->numElements() >= size);
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
    ASSERT(0 == object->numElements());
}

int verifyPrint (const char *a, const char *b)
    // Return 1 if the specified string 'a' is a reordering of the values
    // listed in the specified string 'a', and otherwise 0.
{
    char *c = new char[strlen(b) + 1];
    memcpy(c, b, strlen(b) + 1);
    c[strlen(b) - 1] = ' ';

    int i;
    for (i = 0; a[i] != '\0'; ++i) {
        int j = i;
        while (a[i] != ' ' && a[i] != '\n' && a[i] != '\t' && a[i] != '\0') {
            ++i;
        }

        for (int k = 0; k < (int)strlen(c) - (i - j); ++k) {
            int acon = 1;
            for (int kk = 0; kk < i - j; ++kk) {
                if (a[j + kk] != c[k + kk]) {
                    acon = 0;
                }
            }
            if (acon) {
                for (int kk = 0; kk < i - j; ++kk) {
                    c[k + kk] = ' ';
                }
            }
        }

        if (a[i] == '\0') {
            --i;
        }
    }

    int rv = 1;
    for (i = 0; c[i] != '\0'; ++i) {
        if (c[i] != ' ' && c[i] != '\n' && c[i] != '\t') {
            rv = 0;
        }
    }

    delete [] c;
    return rv;
}

Element dataForSlot(int slot, int index, int numSlots)
    // Return by value a new element that will hash into the specified 'slot'
    // assuming a hashtable consisting of the specified 'numSlots' and is
    // unique relative to all other values of the specified 'index'.  The
    // behavior is undefines unless 0 <= slot, 0 <= index, 0 < numSlots, and
    // slot + index * numSlots < 2^31 - 1.
{
    bsls_PlatformUtil::Int64 value = slot + index * numSlots;
    Element *v = (Element *)&value;
    return *v;
}

struct StringHash {
    static int hash(const bsl::string& string, int size) {
        return bdeimp_StrHash::hash(string.c_str(), size);
    }
};

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) double values to be
// added to the 'bdeci_Hashtable' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
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
//              remove all the elements (set set numElements to 0).  Note that
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
    const char *codes = "ABCDEFGHI";
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        const char *code = strchr(codes, spec[i]);
        if (code) {
            object->add(VALUES[code - codes]);
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
    bslma_DefaultAllocatorGuard defaultGuard(&taDefault);
    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
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
            const int E1 = 100;
            const int E2 = 200;
            const int E3 = 300;
            const int E4 = 400;
            const int E5 = 500;

            bdeci_Hashtable<int, bdeimp_IntHash> ht;
                                            ASSERT( 0 == ht.numElements());

            ht.add(E1);                     ASSERT( 1 == ht.numElements());
                                            ASSERT( ht.isMember(E1));
                                            ASSERT(!ht.isMember(E2));
                                            ASSERT(!ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            ht.add(E2);                     ASSERT( 2 == ht.numElements());
                                            ASSERT( ht.isMember(E1));
                                            ASSERT( ht.isMember(E2));
                                            ASSERT(!ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            ht.add(E3);                     ASSERT( 3 == ht.numElements());
                                            ASSERT( ht.isMember(E1));
                                            ASSERT( ht.isMember(E2));
                                            ASSERT( ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            ht.removeFirst(E1);             ASSERT( 2 == ht.numElements());
                                            ASSERT(!ht.isMember(E1));
                                            ASSERT( ht.isMember(E2));
                                            ASSERT( ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            ht.removeFirst(E2);             ASSERT( 1 == ht.numElements());
                                            ASSERT(!ht.isMember(E1));
                                            ASSERT(!ht.isMember(E2));
                                            ASSERT( ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            ht.removeFirst(E3);             ASSERT( 0 == ht.numElements());
                                            ASSERT(!ht.isMember(E1));
                                            ASSERT(!ht.isMember(E2));
                                            ASSERT(!ht.isMember(E3));
                                            ASSERT(!ht.isMember(E4));
                                            ASSERT(!ht.isMember(E5));

            for (bdeci_HashtableSlotIter<int, bdeimp_IntHash> it(ht, 0);
                                                                   it; ++it) {
                cout << it() << endl;  // I.e., operate on current element.
            }

            bdeci_HashtableSlotManip<int, bdeimp_IntHash> manip(&ht, 0);
            while (manip) {
                if (E3 >= manip()) {
                    manip.advance();
                }
                else {
                    manip.remove();
                }
            }

            ht.reserveCapacity(2);
            manip.setSlot(ht.hash(E1));
            manip.addRaw(E1);
            manip.setSlot(ht.hash(E2));
            manip.addRaw(E2);
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING HASHING STRINGS
        //  Concern:
        //     We are concerned that if the objects stored allocate memory,
        //    that memory will come from the default allocator.
        //
        //  Plan:
        //     To test that the hashtable is properly handling memory
        //     allocation while storing objects, we'll store some strings and
        //     verify that the memory used to store them did not come from the
        //     default allocator.
        //
        //  Testing:
        //     add()
        // --------------------------------------------------------------------

        {
            bdeci_Hashtable<bsl::string, StringHash> table(&testAllocator);
            ASSERT(0 == taDefault.numBytesInUse());

            table.add(bsl::string("woof", &testAllocator));

            ASSERT(0 == taDefault.numBytesInUse());
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING HASH:
        //   We are concerned that, for an object of arbitrary state, 'hash'
        //   must return the slot index that the underlying hash function
        //   produces.
        //
        // Plan:
        //   Since only connection is being tested, a set of values are hashed
        //   and equivalence with the underlying hash function is verified for
        //   a variety of number of slots in the hashtable.
        //
        // Testing:
        //   int hash(double value) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'hash'" << endl
                          << "==============" << endl;

        if (verbose) cout <<
            "\nTesting 'hash'." << endl;
        {
            for (int ei = 0;
                 ei < 7 && ei < bdeci_HashtableImpUtil::BDECI_NUM_SIZES;
                 ++ei) {
                const int EXTEND = bdeci_HashtableImpUtil::lookup(ei);
                Obj mX(&testAllocator);  const Obj& X = mX;
                stretch(&mX, 2*EXTEND);
                LOOP_ASSERT(ei, 2*EXTEND == X.numElements());
                LOOP_ASSERT(ei, EXTEND == X.numSlots());

                for (int ti = 0; ti < NUM_VALUES; ++ti) {
                    LOOP2_ASSERT(ei,
                                 ti,
                                 ElementHash::hash(VALUES[ti],
                                                         EXTEND) ==
                                                           X.hash(VALUES[ti]));
                }
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING COMPACT:
        //   We are concerned that, for an object of arbitrary state, 'compact'
        //   reduces the number of slots used to the minimum number allowed by
        //   the load factor constraints.
        //
        // Plan:
        //   Produce a set of objects with varying internal representations,
        //   'compact' them, and verify the correct number of slots.  Also
        //   ensure the number of elements is unmodified.
        //
        // Testing:
        //   void compact();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'compact'" << endl
                          << "=================" << endl;

        if (verbose) cout <<
            "\nTesting 'compact'." << endl;
        {
            for (int ei = 0;
                 ei < 7 && ei < bdeci_HashtableImpUtil::BDECI_NUM_SIZES;
                 ++ei) {
                const int EXTEND = bdeci_HashtableImpUtil::lookup(ei);
                Obj mX(&testAllocator);  const Obj& X = mX;

                mX.add(VA);
                stretch(&mX, 2*EXTEND - 1);
                LOOP_ASSERT(ei, 2*EXTEND == X.numElements());
                LOOP_ASSERT(ei, EXTEND == X.numSlots());
                mX.add(VA);
                LOOP_ASSERT(ei, 2*EXTEND + 1 == X.numElements());
                LOOP_ASSERT(ei, EXTEND < X.numSlots());

                mX.removeFirst(VA);
                LOOP_ASSERT(ei, 2*EXTEND == X.numElements());
                LOOP_ASSERT(ei, bdeci_HashtableImpUtil::lookup(ei + 1) ==
                                                                 X.numSlots());
                mX.compact();
                LOOP_ASSERT(ei, 2*EXTEND == X.numElements());
                LOOP_ASSERT(ei, EXTEND == X.numSlots());

                mX.removeFirst(VA);
                LOOP_ASSERT(ei, 2*EXTEND - 1 == X.numElements());
                LOOP_ASSERT(ei, EXTEND == X.numSlots());
                mX.compact();
                LOOP_ASSERT(ei, 2*EXTEND - 1 == X.numElements());
                LOOP_ASSERT(ei, EXTEND == X.numSlots());

                mX.removeAll();
                LOOP_ASSERT(ei, 0 == X.numElements());
                LOOP_ASSERT(ei, 1 == X.numSlots());
                mX.compact();
                LOOP_ASSERT(ei, 0 == X.numElements());
                LOOP_ASSERT(ei, 1 == X.numSlots());
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING bdeci_HashtableSlotManip:
        //   The basic concern is that the default constructor, the destructor,
        //   the manipulators:
        //      - advance()
        //      - removeFirst()
        //    and the accessors:
        //      - operator const void *() const;
        //      - double operator()() const;
        //   operate as expected.
        //
        // Plan:
        //   Create a set S of various lengths and internal representations.
        //   For each element X of S, create an empty set Y, walk the set
        //   using the iterator and populate the set Y with each found member
        //   (while verifying the value was not already present), and verify
        //   X == Y.  Also remove each element of X and use
        //   'X.removeFirst(value)' as an oracle to verify the resulting set.
        //
        // Testing:
        //   bdeci_HashtableSlotManip(bdeci_Hashtable &, int si);
        //   ~bdeci_HashtableSlotManip();
        //   advance();
        //   remove();
        //   setSlot(int slotIndex);
        //   operator const void *() const;
        //   double operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing bdeci_HashtableSlotManip" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
            "\nTesting bdeci_HashtableSlotManip" << endl;
        {
            static const int EXTEND[] = {
                5, 23, 97, 389
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;
            int i;

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                Obj mX(&testAllocator);  const Obj& X = mX;
                for (i = 0; i < EXTEND[ei]; ++i) {
                    mX.add(VA);
                }
                LOOP_ASSERT(ei, EXTEND[ei] == X.numElements());
                mX.removeEvery(VA);
                const int numSlots = X.numSlots();

                {
                    // Insert elements in a pattern and verify manipulator.
                    // Note the elements are added in reverse order to the
                    // expected order from the manipulator.

                    const struct {
                        int d_lineNum;    // source line number
                        int d_slot;       // slot to place data
                        int d_index;      // index
                    } DATA[] = {
                        //line      slot      index
                        //----  ------------  -----
                        { L_,   numSlots - 1,     2 },
                        { L_,   numSlots - 1,     1 },
                        { L_,   numSlots - 1,     0 },
                        { L_,              2,     1 },
                        { L_,              2,     0 },
                        { L_,              0,     2 },
                        { L_,              0,     1 },
                        { L_,              0,     0 },
                    };
                    const int NUM_DATA = sizeof DATA / sizeof *DATA;

                    LOOP_ASSERT(ei, 0 == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());
                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.add(value);
                    }
                    LOOP_ASSERT(ei, NUM_DATA == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());

                    ObjManip manip2(&mX, 0);
                    for (int si = 0; si < numSlots; ++si) {
                        ObjManip manip(&mX, si);
                        manip2.setSlot(si);

                        for (i = NUM_DATA - 1; i >= 0; --i) {
                            if (si == DATA[i].d_slot) {
                                Element value = dataForSlot(DATA[i].d_slot,
                                                            DATA[i].d_index,
                                                            numSlots);
                                LOOP2_ASSERT(ei, i, manip);
                                LOOP2_ASSERT(ei, i, value == manip());
                                manip.advance();
                                LOOP2_ASSERT(ei, i, manip2);
                                LOOP2_ASSERT(ei, i, value == manip2());
                                manip2.advance();
                            }
                        }
                        LOOP_ASSERT(ei, !manip);
                        LOOP_ASSERT(ei, !manip2);
                    }

                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.removeFirst(value);
                    }
                    LOOP_ASSERT(ei, 0 == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());
                }

                {
                    // Insert elements in a pattern and verify manipulator.
                    // Note the elements are added in reverse order to the
                    // expected order from the manipulator.

                    const struct {
                        int d_lineNum;    // source line number
                        int d_slot;       // slot to place data
                        int d_index;      // index
                    } DATA[] = {
                        //line      slot      index
                        //----  ------------  -----
                        { L_,   numSlots - 1,     1 },
                        { L_,   numSlots - 1,     2 },
                        { L_,   numSlots - 1,     0 },
                        { L_,              2,     0 },
                        { L_,              2,     1 },
                        { L_,              0,     2 },
                        { L_,              0,     0 },
                        { L_,              0,     1 },
                    };
                    const int NUM_DATA = sizeof DATA / sizeof *DATA;

                    {
                        for (int i = 0; i < NUM_DATA; ++i) {
                            Element value = dataForSlot(DATA[i].d_slot,
                                                        DATA[i].d_index,
                                                        numSlots);
                            mX.add(value);
                        }
                    }
                    LOOP_ASSERT(ei, NUM_DATA == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());

                    ObjManip manip2(&mX, 0);
                    for (int si = 0; si < numSlots; ++si) {
                        ObjManip manip(&mX, si);
                        manip2.setSlot(si);

                        for (int i = NUM_DATA - 1; i >= 0; --i) {
                            if (si == DATA[i].d_slot) {
                                Element value = dataForSlot(DATA[i].d_slot,
                                                            DATA[i].d_index,
                                                            numSlots);
                                LOOP2_ASSERT(ei, i, manip);
                                LOOP2_ASSERT(ei, i, value == manip());
                                manip.advance();
                                LOOP2_ASSERT(ei, i, manip2);
                                LOOP2_ASSERT(ei, i, value == manip2());
                                manip2.advance();
                            }
                        }
                        LOOP_ASSERT(ei, !manip);
                        LOOP_ASSERT(ei, !manip2);
                    }

                    {
                        for (int i = 0; i < NUM_DATA; ++i) {
                            Element value = dataForSlot(DATA[i].d_slot,
                                                        DATA[i].d_index,
                                                        numSlots);
                            mX.removeFirst(value);
                        }
                    }
                    LOOP_ASSERT(ei, 0 == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());
                }

            }
        }
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
                    LOOP2_ASSERT(ti, ei, curLen == X.numElements());

                    // test remove
                    for (int k = 0; k < X.numElements(); ++k) {
                        Obj mA(X, &testAllocator);  const Obj& A = mA;
                        Obj mB(X, &testAllocator);  const Obj& B = mB;

                        double value;
                        int count = 0;
                        for (int si = 0; si < mA.numSlots(); ++si) {
                            for (ObjManip manip(&mA, si); manip; ) {
                                if (k == count) {
                                    value = manip();
                                    manip.remove();
                                }
                                ++count;
                                if (manip) {
                                    manip.advance();
                                }
                            }
                        }

                        int v = mB.removeFirst(value);
                        LOOP2_ASSERT(ti, k, 1 == v);
                        LOOP2_ASSERT(ti, k, B == A);
                    }
                }
            }
        }
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
                    LOOP2_ASSERT(ti, ei, curLen == X.numElements());
                    mX.reserveCapacity(X.numElements() + 1);

                    // test add
                    for (int k = 0; k < X.numElements(); ++k) {
                        Obj mA(X, &testAllocator);  const Obj& A = mA;
                        mA.reserveCapacity(A.numElements() + 1);

                        int count = 0;
                        for (int si = 0; si < mA.numSlots(); ++si) {
                            for (ObjManip manip(&mA, si); manip; ) {
                                if (k == count) {
                                    manip.addRaw(VA);
                                    LOOP2_ASSERT(ti, k, VA == manip());
                                    LOOP2_ASSERT(ti, k, X.numElements() + 1
                                                           == A.numElements());
                                    manip.remove();
                                    LOOP2_ASSERT(ti, k, X == A);
                                }
                                ++count;
                                if (manip) {
                                    manip.advance();
                                }
                            }
                        }
                    }
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
        //   bdeci_Hashtable(const InitialCapacity& ne, *ba = 0);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Method" << endl
            << "=================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdeci_Hashtable(capacity, ba)' Constructor"
                          << endl;
        if (verbose) cout << "\twith a 'bslma_TestAllocator':" << endl;
        {
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BSLMA_EXCEPTION_TEST {
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE, &testAllocator);  const Obj &X = mX;
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(VA);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(ne, NUM_BLOCKS == testAllocator.numBlocksTotal());
                LOOP_ASSERT(ne, NUM_BYTES  == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\twith no allocator (exercise only):" << endl;
        {
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE);  const Obj &X = mX;
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(VA);
                }
                if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[16384];
            bslma_BufferAllocator a(memory, sizeof memory);
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.add(VA);
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

            static const int EXTEND[] = {
                0, 1, 4, 5, 7, 17, 23, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            const Obj Empty;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_x;
                const int   NE   = DATA[ti].d_ne;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                const Obj W(g(SPEC), &testAllocator);

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(&testAllocator);
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    mX = W;
                    const Obj &X = mX;
                    mX.reserveCapacity(0);
                    LOOP_ASSERT(LINE, W == X);
                    mX.reserveCapacity(NE);
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    if (veryVerbose) P_(X);
                    for (int i = X.numElements(); i < NE; ++i) {
                        mX.add(VA);
                    }
                    if (veryVerbose) P(X);
                    LOOP_ASSERT(LINE,
                                NUM_BLOCKS == testAllocator.numBlocksTotal());
                    LOOP_ASSERT(LINE,
                                NUM_BYTES == testAllocator.numBytesInUse());
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADD, ADDUNIQUE, REMOVEFIRST, AND REMOVEEVERY
        //   Concerns:
        //     For the 'add', 'addUnique', 'removeFirst', and 'removeEvery'
        //     methods, the following properties must hold:
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
        //   Use the enumeration technique to create a set S of initial values.
        //   Perturb the internal representation of the set S by various
        //   amounts to obtain the set T of initial states.  For each element
        //   T, perform a handful of 'addUnique' and 'removeFirst' operations
        //   of elements from the universe of elements in VALUES to validate
        //   the return values while maintaining the initial value.  Finally,
        //   'removeFirst' all members from the universe while verifying the
        //   return value.  The method 'isMember' is used as an oracle for
        //   computing the return values of 'addUnique' and 'removeFirst'.
        //
        // Testing:
        //   int add(double value);
        //   int addUnique(double value);
        //   int removeFirst(double value);
        //   int removeEvery(double value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'add', 'addUnique', 'removeFirst', and "
                          << "'removeEvery'" << endl
                << "==============================================="
                          << "=============" << endl;

        if (verbose) cout << "\nTesting add(value)." << endl;
        {
            static const struct {
                int         d_lineNum;// source line number
                const char *d_spec_p; // specification string
                Element     d_value;  // value to remove
                const char *d_exp_p;  // specification string of expected value
                int   d_rv;           // expected return value
            } DATA[] = {
                //line  spec            value  exp             rv
                //----  --------------  -----  --------------  --
                { L_,   "",              VA,   "A",             1 },
                { L_,   "",              VB,   "B",             1 },
                { L_,   "A",             VA,   "AA",            1 },
                { L_,   "A",             VB,   "AB",            1 },
                { L_,   "B",             VA,   "BA",            1 },
                { L_,   "B",             VB,   "BB",            1 },
                { L_,   "AB",            VA,   "ABA",           1 },
                { L_,   "AB",            VB,   "ABB",           1 },
                { L_,   "BA",            VA,   "BAA",           1 },
                { L_,   "BA",            VB,   "BAB",           1 },
                { L_,   "ABC",           VA,   "ABCA",          1 },
                { L_,   "ABC",           VB,   "ABCB",          1 },
                { L_,   "ACB",           VA,   "ACBA",          1 },
                { L_,   "ACB",           VB,   "ACBB",          1 },
                { L_,   "CAB",           VA,   "CABA",          1 },
                { L_,   "CAB",           VB,   "CABB",          1 },
                { L_,   "CBA",           VA,   "CBAA",          1 },
                { L_,   "CBA",           VB,   "CBAB",          1 },
                { L_,   "AABB",          VA,   "AABBA",         1 },
                { L_,   "AABB",          VB,   "AABBB",         1 },
                { L_,   "ABAB",          VA,   "ABABA",         1 },
                { L_,   "ABAB",          VB,   "ABABB",         1 },
                { L_,   "BAAB",          VA,   "BAABA",         1 },
                { L_,   "BAAB",          VB,   "BAABB",         1 },
                { L_,   "ABBA",          VA,   "ABBAA",         1 },
                { L_,   "ABBA",          VB,   "ABBAB",         1 },
                { L_,   "BABA",          VA,   "BABAA",         1 },
                { L_,   "BABA",          VB,   "BABAB",         1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 5, 23, 97, 389
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            const Obj Empty;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                Element VALUE          = DATA[ti].d_value;
                const char *const EXP  = DATA[ti].d_exp_p;
                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    {
                      Obj mX(&testAllocator);  const Obj& X = mX;
                      stretchRemoveAll(&mX, strlen(SPEC) + EXTEND[ei]);
                      gg(&mX, SPEC);
                      mX.add(VALUE);

                      Obj mE(&testAllocator);  const Obj& E = mE;
                      stretchRemoveAll(&mE, strlen(SPEC) + EXTEND[ei]);
                      gg(&mE, EXP);
                      LOOP2_ASSERT(LINE, ei, E == X);
                    }
                    {
                      Obj mX(&testAllocator);  const Obj& X = mX;
                      stretch(&mX, strlen(SPEC) + EXTEND[ei]);
                      mX = Empty;
                      gg(&mX, SPEC);
                      mX.add(VALUE);

                      Obj mE(&testAllocator);  const Obj& E = mE;
                      stretch(&mE, strlen(SPEC) + EXTEND[ei]);
                      mE = Empty;
                      gg(&mE, EXP);
                      LOOP2_ASSERT(LINE, ei, E == X);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting x.addUnique(value) and x.removeFirst(value)" << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",         "ABCDEFGH",         "ABCDEFGHI",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            const Obj Empty;

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
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    mX = Empty; // set numSlots to 1
                    gg(&mX, SPEC);
                    LOOP2_ASSERT(ti, ei, curLen == X.numElements());

                    // test adding and removing
                    int k;
                    for (k = 0; k < NUM_VALUES; ++k) {
                        int rv;
                        int initialLength = X.numElements();

                        int erv = !X.isMember(VALUES[k]);
                        rv = 0 != mX.addUnique(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, erv == rv);

                        rv = 0 != mX.addUnique(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, 0 == rv);

                        rv = mX.removeFirst(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, 1 == rv);

                        rv = mX.removeFirst(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, 0 == rv);

                        rv = 0 != mX.addUnique(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, 1 == rv);

                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength + erv == X.numElements());
                    }

                    // test removing everything
                    for (k = 0; k < NUM_VALUES; ++k) {
                        int initialLength = X.numElements();
                        int erv = X.isMember(VALUES[k]);
                        int rv = mX.removeFirst(VALUES[k]);
                        LOOP3_ASSERT(ti, ei, k, erv == rv);
                        LOOP3_ASSERT(ti, ei, k,
                                     initialLength - erv == X.numElements());
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting removeEvery(value)." << endl;
        {
            static const struct {
                int         d_lineNum;// source line number
                const char *d_spec_p; // specification string
                Element     d_value;  // value to remove
                const char *d_exp_p;  // specification string of expected value
                int   d_rv;           // expected return value
            } DATA[] = {
                //line  spec            value  exp             rv
                //----  --------------  -----  --------------  --
                { L_,   "",              VA,   "",              0 },
                { L_,   "",              VB,   "",              0 },
                { L_,   "A",             VA,   "",              1 },
                { L_,   "A",             VB,   "A",             0 },
                { L_,   "B",             VA,   "B",             0 },
                { L_,   "B",             VB,   "",              1 },
                { L_,   "AB",            VA,   "B",             1 },
                { L_,   "AB",            VB,   "A",             1 },
                { L_,   "BA",            VA,   "B",             1 },
                { L_,   "BA",            VB,   "A",             1 },
                { L_,   "ABC",           VA,   "BC",            1 },
                { L_,   "ABC",           VB,   "AC",            1 },
                { L_,   "ACB",           VA,   "CB",            1 },
                { L_,   "ACB",           VB,   "AC",            1 },
                { L_,   "CAB",           VA,   "CB",            1 },
                { L_,   "CAB",           VB,   "CA",            1 },
                { L_,   "CBA",           VA,   "CB",            1 },
                { L_,   "CBA",           VB,   "CA",            1 },
                { L_,   "AABB",          VA,   "BB",            2 },
                { L_,   "AABB",          VB,   "AA",            2 },
                { L_,   "ABAB",          VA,   "BB",            2 },
                { L_,   "ABAB",          VB,   "AA",            2 },
                { L_,   "BAAB",          VA,   "BB",            2 },
                { L_,   "BAAB",          VB,   "AA",            2 },
                { L_,   "ABBA",          VA,   "BB",            2 },
                { L_,   "ABBA",          VB,   "AA",            2 },
                { L_,   "BABA",          VA,   "BB",            2 },
                { L_,   "BABA",          VB,   "AA",            2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 5, 23, 97, 389
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                Element VALUE          = DATA[ti].d_value;
                const char *const EXP  = DATA[ti].d_exp_p;
                const int         RV   = DATA[ti].d_rv;
                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    stretchRemoveAll(&mX, EXTEND[ei]);
                    int i;
                    for (i = 0; i < (int)strlen(SPEC); ++i) {
                        mX.add(VA);
                    }
                    for (i = 0; i < (int)strlen(SPEC); ++i) {
                        mX.removeFirst(VA);
                    }
                    gg(&mX, SPEC);
                    int rv = mX.removeEvery(VALUE);
                    LOOP2_ASSERT(LINE, ei, RV == rv);

                    Obj mE(&testAllocator);  const Obj& E = mE;
                    for (i = 0; i < (int)strlen(SPEC); ++i) {
                        mE.add(VA);
                    }
                    for (i = 0; i < (int)strlen(SPEC); ++i) {
                        mE.removeFirst(VA);
                    }
                    gg(&mE, EXP);

                    LOOP2_ASSERT(LINE, ei, E == X);
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
        //   For each of an enumerated set of object, 'level', and
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
                                                        "1.5"         NL
                                                        "]"           NL  },

                { L_,   "A",      -2,         1,        "["           NL
                                                        "   1.5"      NL
                                                        "  ]"         NL  },

                { L_,   "BC",      1,         2,        "  ["         NL
                                                        "    2.5"     NL
                                                        "    3.5"     NL
                                                        "  ]"         NL  },

                { L_,   "BC",      2,         1,        "  ["         NL
                                                        "   2.5"      NL
                                                        "   3.5"      NL
                                                        "  ]"         NL  },

                { L_,   "ABCDE",   1,         3,        "   ["        NL
                                                        "      1.5"   NL
                                                        "      2.5"   NL
                                                        "      3.5"   NL
                                                        "      4.75"  NL
                                                        "      -5.25" NL
                                                        "   ]"        NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000;       // Must be able to hold output string.
            const char Z1 = (char)0xFF;  // Value 1 used for an unset char.
            const char Z2 = 0x00;        // Value 2 used for an unset char.

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
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.numElements()); // same lengths

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
                LOOP_ASSERT(ti, verifyPrint(buf1, FMT));
                LOOP_ASSERT(ti, verifyPrint(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // There is no streaming functionality.  This case is left as a
        // place-holder.
        // --------------------------------------------------------------------
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
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
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
        //   bdeci_Hashtable& operator=(const bdeci_Hashtable& rhs);
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

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

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

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
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

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];

                    Obj mX(&testAllocator);
                    stretchRemoveAll(&mX, N);
                    gg(&mX, SPEC);
                    const Obj& X = mX;                          // control
                    LOOP_ASSERT(ti, curLen == X.numElements()); // same lengths

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
        //   Obj g(const char *spec);
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
        //   To address concerns 1 - 3, specify a set S of object values with
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
        //       (using a 'bslma_BufferAllocator') and never destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        // Testing:
        //   bdeci_Hashtable(const bdeci_Hashtable& original, *ba = 0);
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

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    // Create control object w.
                    Obj mW(&testAllocator);  stretchRemoveAll(&mW, N);
                    gg(&mW, SPEC);  const Obj& W = mW;
                    LOOP_ASSERT(ti, curLen == W.numElements()); // same lengths
                    if (veryVerbose) { cout << "\t"; P(W); }

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
                        bslma_BufferAllocator a(memory, sizeof memory);
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
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //
        //   Note also that both equality operators must return either 1 or 0,
        //   and neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   1 or 0) using all elements (u, v) of the cross product S X S.
        //
        //   Next specify a second set S' containing a representative variety
        //   of (black-box) box values ordered by increasing (logical) length.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to only itself.
        //
        // Testing:
        //   operator==(const bdeci_Hashtable&,
        //              const bdeci_Hashtable&);
        //   operator!=(const bdeci_Hashtable&,
        //              const bdeci_Hashtable&);
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

                    // Compute expected result.

                    int r = X.numElements() == Y.numElements() &&
                            X.numSlots() == Y.numSlots();
                    for (int slotIndex = 0;
                         r && slotIndex < X.numSlots();
                         ++slotIndex) {
                        ObjIter xi(X, slotIndex);
                        ObjIter yi(Y, slotIndex);
                        while (r && xi && yi) {
                          if (xi() != yi()) {
                            r = 0;
                          }
                          ++xi;
                          ++yi;
                        }
                        if (xi != yi) {
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

        if (verbose) cout << "\nCompare objects having "
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

                            // Compute expected result.

                            int r = X.numElements() == Y.numElements() &&
                                    X.numSlots() == Y.numSlots();
                            for (int slotIndex = 0;
                                 r && slotIndex < X.numSlots();
                                 ++slotIndex) {
                                ObjIter xi(X, slotIndex);
                                ObjIter yi(Y, slotIndex);
                                while (r && xi && yi) {
                                    if (xi() != yi()) {
                                        r = 0;
                                    }
                                    ++xi;
                                    ++yi;
                                }
                                if (xi != yi) {
                                    r = 0;
                                }
                            }

                            LOOP4_ASSERT(i, ie, j, je,  r == (X == Y));
                            LOOP4_ASSERT(i, ie, j, je,  1 == (X == X));
                            LOOP4_ASSERT(i, ie, j, je,  1 == (Y == Y));
                            LOOP4_ASSERT(i, ie, j, je, !r == (X != Y));
                            LOOP4_ASSERT(i, ie, j, je,  0 == (X != X));
                            LOOP4_ASSERT(i, ie, j, je,  0 == (Y != Y));
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
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.  Note
        //   that the output ordering is not guaranteed and the function
        //   'verifyPrint' is used to validate equality of the output to the
        //   expected output.
        //
        // Testing:
        //   operator<<(ostream&, const bdeci_Hashtable&);
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
                { L_,   "A",            "[ 1.5 ]"                       },
                { L_,   "AA",           "[ 1.5 1.5 ]"                   },
                { L_,   "BC",           "[ 2.5 3.5 ]"                   },
                { L_,   "ABCDE",        "[ 1.5 2.5 3.5 4.75 -5.25 ]"    },
                { L_,   "ABCDEABCDE",
                    "[ 1.5 2.5 3.5 4.75 -5.25 1.5 2.5 3.5 4.75 -5.25 ]" },
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
                const int curLen = strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.numElements()); // same lengths

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
                LOOP_ASSERT(ti, verifyPrint(buf1, FMT));
                LOOP_ASSERT(ti, verifyPrint(buf2, FMT));
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
        //     - numElements() const
        //     - numSlots() const
        //     - isMember() const
        //     - numOccurrences() const
        //     - bdeci_HashtableIter
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.
        //
        // Plan:
        //   Specify a set S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        // Testing:
        //   int isMember(double value) const;
        //   int numElements() const;
        //   int numOccurrences(double value) const;
        //   int numSlots() const;
        //
        //   bdeci_HashtableIter(const bdeci_Hashtable&m int si);
        //   ~bdeci_HashtableIter();
        //   operator++();
        //   setSlot(int slotIndex);
        //   operator const void *() const;
        //   double operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting 'numElements', 'numSlots', "
                          << "'isMember', and 'numOccurrences'" << endl;
        {
            const int SZ = 32;
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_numElements;   // expected number of elements
                int         d_numSlots;      // expected number of slots
                Element     d_elements[SZ];  // expected element values
            } DATA[] = {
                //line  spec            ne   ns  elements
                //----  --------------  ---  ---  ------------------------
                { L_,   "",              0,   1,  { 0 }                   },
                { L_,   "A",             1,   1,  { VA }                  },
                { L_,   "B",             1,   1,  { VB }                  },
                { L_,   "AB",            2,   1,  { VA, VB }              },
                { L_,   "BC",            2,   1,  { VB, VC }              },
                { L_,   "ABC",           3,   5,  { VA, VB, VC }          },
                { L_,   "ACB",           3,   5,  { VA, VB, VC }          },
                { L_,   "BAC",           3,   5,  { VA, VB, VC }          },
                { L_,   "BCA",           3,   5,  { VA, VB, VC }          },
                { L_,   "CAB",           3,   5,  { VA, VB, VC }          },
                { L_,   "CBA",           3,   5,  { VA, VB, VC }          },
                { L_,   "CDAB",          4,   5,  { VC, VD, VA, VB }      },
                { L_,   "DABC",          4,   5,  { VD, VA, VB, VC }      },
                { L_,   "ABCDE",         5,   5,  { VA, VB, VC, VD, VE }  },
                { L_,   "EDCBA",         5,   5,  { VE, VD, VC, VB, VA }  },
                { L_,   "ABCABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCACB",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCBAC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCBCA",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCCAB",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCCBA",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ACBABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "BACABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "BCAABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "CABABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "CBAABC",        6,   5,  { VA,VA,VB,VB,VC,VC }   },
                { L_,   "ABCDEAB",       7,   5,  { VA, VB, VC, VD, VE,
                                                    VA, VB }              },
                { L_,   "BACDEABC",      8,   5,  { VB, VA, VC, VD, VE,
                                                    VA, VB, VC }          },
                { L_,   "CBADEABCD",     9,   5,  { VC, VB, VA, VD, VE,
                                                    VA, VB, VC, VD }      },
                { L_,   "CBADEABCDE",   10,   5,  { VC, VB, VA, VD, VE,
                                                    VA, VB, VC, VD, VE }  },
                { L_,   "CBADEABCDEA",  11,  23,  { VC, VB, VA, VD, VE,
                                                    VA, VB, VC, VD, VE,
                                                    VA                 }  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50;
            stretch(&mY, EXTEND); ASSERT(EXTEND == mY.numElements());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NE           = DATA[ti].d_numElements;
                const int NS           = DATA[ti].d_numSlots;
                const Element *const e = DATA[ti].d_elements;
                const int curLen       = NE;

                Obj mX(&testAllocator);

                const Obj& X = gg(&mX, SPEC);   // canonical organization
                mY.removeAll();
                const Obj& Y = gg(&mY, SPEC);   // has extended capacity

                LOOP_ASSERT(ti, curLen == X.numElements()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) { cout << "\t\t\t"; P(X);
                                   cout << "\t\t\t"; P(Y); }

                LOOP_ASSERT(LINE, NE == X.numElements());
                LOOP_ASSERT(LINE, NS == X.numSlots());
                LOOP_ASSERT(LINE, NE == Y.numElements());
                int i;
                for (i = 0; i < NUM_VALUES; ++i) {
                    int ev = 0;
                    for (int j = 0; j < NE; ++j) {
                        if (VALUES[i] == e[j]) {
                            ev = 1;
                        }
                    }
                    LOOP2_ASSERT(LINE, i, ev == X.isMember(VALUES[i]));
                    LOOP2_ASSERT(LINE, i, ev == Y.isMember(VALUES[i]));
                }
                for (i = 0; i < NUM_VALUES; ++i) {
                    int ev = 0;
                    for (int j = 0; j < NE; ++j) {
                        if (VALUES[i] == e[j]) {
                            ++ev;
                        }
                    }
                    LOOP2_ASSERT(LINE, i, ev == X.numOccurrences(VALUES[i]));
                    LOOP2_ASSERT(LINE, i, ev == Y.numOccurrences(VALUES[i]));
                }
            }
        }

        if (verbose) cout <<
            "\nTesting bdeci_HashtableSlotIter" << endl;
        {
            static const int EXTEND[] = {
                5, 23, 97, 389
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;
           int i;

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                Obj mX(&testAllocator);  const Obj& X = mX;
                for (i = 0; i < EXTEND[ei]; ++i) {
                    mX.add(VA);
                }
                LOOP_ASSERT(ei, EXTEND[ei] == X.numElements());
                mX.removeEvery(VA);
                const int numSlots = X.numSlots();

                {
                    // Insert elements in a pattern and verify iterator.
                    // Note the elements are added in reverse order to the
                    // expected order from the iterator.

                    const struct {
                        int d_lineNum;    // source line number
                        int d_slot;       // slot to place data
                        int d_index;      // index
                    } DATA[] = {
                        //line      slot      index
                        //----  ------------  -----
                        { L_,   numSlots - 1,     2 },
                        { L_,   numSlots - 1,     1 },
                        { L_,   numSlots - 1,     0 },
                        { L_,              2,     1 },
                        { L_,              2,     0 },
                        { L_,              0,     2 },
                        { L_,              0,     1 },
                        { L_,              0,     0 },
                    };
                    const int NUM_DATA = sizeof DATA / sizeof *DATA;

                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.add(value);
                    }
                    LOOP_ASSERT(ei, NUM_DATA == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());

                    ObjIter iter2(X, 0);
                    for (int si = 0; si < numSlots; ++si) {
                        ObjIter iter(X, si);
                        iter2.setSlot(si);
                        for (i = NUM_DATA - 1; i >= 0; --i) {
                            if (si == DATA[i].d_slot) {
                              Element value = dataForSlot(DATA[i].d_slot,
                                                          DATA[i].d_index,
                                                          numSlots);
                              LOOP2_ASSERT(ei, i, iter);
                              LOOP2_ASSERT(ei, i, value == iter());
                              ++iter;
                              LOOP2_ASSERT(ei, i, iter2);
                              LOOP2_ASSERT(ei, i, value == iter2());
                              ++iter2;
                            }
                        }
                        LOOP_ASSERT(ei, !iter);
                        LOOP_ASSERT(ei, !iter2);
                    }

                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.removeFirst(value);
                    }
                    LOOP_ASSERT(ei, 0 == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());
                }

                {
                    // Insert elements in a pattern and verify iterator.
                    // Note the elements are added in reverse order to the
                    // expected order from the iterator.

                    const struct {
                        int d_lineNum;    // source line number
                        int d_slot;       // slot to place data
                        int d_index;      // index
                    } DATA[] = {
                        //line      slot      index
                        //----  ------------  -----
                        { L_,   numSlots - 1,     1 },
                        { L_,   numSlots - 1,     2 },
                        { L_,   numSlots - 1,     0 },
                        { L_,              2,     0 },
                        { L_,              2,     1 },
                        { L_,              0,     2 },
                        { L_,              0,     0 },
                        { L_,              0,     1 },
                    };
                    const int NUM_DATA = sizeof DATA / sizeof *DATA;

                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.add(value);
                    }
                    LOOP_ASSERT(ei, NUM_DATA == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());

                    ObjIter iter2(X, 0);
                    for (int si = 0; si < numSlots; ++si) {
                        ObjIter iter(X, si);
                        iter2.setSlot(si);
                        for (i = NUM_DATA - 1; i >= 0; --i) {
                            if (si == DATA[i].d_slot) {
                              Element value = dataForSlot(DATA[i].d_slot,
                                                          DATA[i].d_index,
                                                          numSlots);
                              LOOP2_ASSERT(ei, i, iter);
                              LOOP2_ASSERT(ei, i, value == iter());
                              ++iter;
                              LOOP2_ASSERT(ei, i, iter2);
                              LOOP2_ASSERT(ei, i, value == iter2());
                              ++iter2;
                            }
                        }
                        LOOP_ASSERT(ei, !iter);
                        LOOP_ASSERT(ei, !iter2);
                    }

                    for (i = 0; i < NUM_DATA; ++i) {
                        Element value = dataForSlot(DATA[i].d_slot,
                                                    DATA[i].d_index,
                                                    numSlots);
                        mX.removeFirst(value);
                    }
                    LOOP_ASSERT(ei, 0 == X.numElements());
                    LOOP_ASSERT(ei, numSlots == X.numSlots());
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
        //    - 'dataForSlot'      Provides data for a particular slot.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
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
        //   operation while remaining insensitive to the set 'Element' size.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        //   int ggg(Obj *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        //   Element dataForSlot(int slot, int index, int numSlots);
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

                LOOP_ASSERT(LINE, LENGTH == X.numElements());
                LOOP_ASSERT(LINE, LENGTH == Y.numElements());
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
                { L_,   "A",            0,      1,              0       },

                { L_,   "",             2,      0,              2       },
                { L_,   "A",            1,      1,              1       },
                { L_,   "AB",           0,      2,              0       },

                { L_,   "",             3,      0,              3       },
                { L_,   "A",            2,      1,              2       },
                { L_,   "AB",           1,      2,              1       },
                { L_,   "ABC",          0,      3,              0       },

                { L_,   "",             4,      0,              4       },
                { L_,   "A",            3,      1,              3       },
                { L_,   "AB",           2,      2,              2       },
                { L_,   "ABC",          1,      3,              1       },
                { L_,   "ABCD",         0,      4,              0       },

                { L_,   "",             5,      0,              4       },
                { L_,   "A",            4,      1,              3       },
                { L_,   "AB",           3,      2,              2       },
                { L_,   "ABC",          2,      3,              1       },
                { L_,   "ABCD",         1,      4,              0       },
                { L_,   "ABCDE",        0,      4,              0       },
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

                    LOOP_ASSERT(LINE, curLen == X.numElements());
                    LOOP_ASSERT(LINE, curLen == Y.numElements());

                    LOOP_ASSERT(LINE, firstResize == blocks12A);

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

                    LOOP_ASSERT(LINE, curDepth == X.numElements());
                    LOOP_ASSERT(LINE,        0 == Y.numElements());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B >= bytes23B);
                }
            }
        }

        if (verbose) cout << "\nTesting 'dataForSlot'." << endl;
        {
            const struct {
                int d_lineNum;    // source line number
                int d_slot;       // slot to place data
                int d_index;      // index
                int d_numSlot;    // number of slots
            } DATA[] = {
                //line  slot  index  numSlot
                //----  ----  -----  -------
                { L_,      0,     0,       1 },
                { L_,      0,     1,       1 },
                { L_,      0,     2,       1 },
                { L_,      0,     0,       2 },
                { L_,      0,     1,       2 },
                { L_,      1,     0,       2 },
                { L_,      1,     1,       2 },
                { L_,      0,     0,       5 },
                { L_,      0,     1,       5 },
                { L_,      1,     0,       5 },
                { L_,      1,     1,       5 },
                { L_,      2,     0,       5 },
                { L_,      2,     1,       5 },
                { L_,      3,     0,       5 },
                { L_,      3,     1,       5 },
                { L_,      4,     0,       5 },
                { L_,      4,     1,       5 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ui = 0; ui < NUM_DATA; ++ui) {
                const int U_LINE    = DATA[ui].d_lineNum;
                const int U_SLOT    = DATA[ui].d_slot;
                const int U_INDEX   = DATA[ui].d_index;
                const int U_NUMSLOT = DATA[ui].d_numSlot;

                LOOP_ASSERT(U_LINE, U_SLOT ==
                                ElementHash::hash(dataForSlot(U_SLOT,
                                                                    U_INDEX,
                                                                    U_NUMSLOT),
                                                        U_NUMSLOT));
                for (int vi = 0; vi < NUM_DATA; ++vi) {
                    const int V_LINE    = DATA[vi].d_lineNum;
                    const int V_SLOT    = DATA[vi].d_slot;
                    const int V_INDEX   = DATA[vi].d_index;
                    const int V_NUMSLOT = DATA[vi].d_numSlot;

                    const int rv = (U_NUMSLOT != V_NUMSLOT) ||
                                   ((ui == vi) ^
                                   (dataForSlot(U_SLOT, U_INDEX,  U_NUMSLOT) !=
                                    dataForSlot(V_SLOT, V_INDEX,  V_NUMSLOT)));
                    LOOP2_ASSERT(U_LINE, V_LINE, rv);
                }
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
        //        (using a 'bslma_BufferAllocator') and never destroyed.
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
        //          and confirm (with 'length'), use add to set the instance
        //          to a value of length j, verify the value, and allow the
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bslma_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        // Testing:
        //   bdeci_Hashtable(bslma_Allocator *ba);
        //   ~bdeci_Hashtable();
        //   BOOTSTRAP: void add(double value);
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
            ASSERT(0 == X.numElements());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.numElements());
        }

        if (verbose) cout << "\t\tWith execeptions." << endl;
        {
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.numElements());
          } END_BSLMA_EXCEPTION_TEST
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

        if (verbose) cout << "\nTesting 'add' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
            mX.add(V0);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
            ASSERT(BB + 1 == AA);
            ASSERT(B  + 1 == A);
            ASSERT(1 == X.numElements());
            ASSERT(X.isMember(V0));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
            mX.add(V1);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
            ASSERT(2 == X.numElements());
            ASSERT(BB + 1 == AA);
            ASSERT(B  + 1 == A);
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0); mX.add(V1);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
            mX.add(V2);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
            ASSERT(BB + 1 == AA);
            ASSERT(B  + 0 == A);
            ASSERT(3 == X.numElements());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
            mX.add(V3);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
            ASSERT(BB + 1 == AA);
            ASSERT(B  + 1 == A);
            ASSERT(4 == X.numElements());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2); mX.add(V3);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
            mX.add(V4);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
            ASSERT(BB + 0 == AA);
            ASSERT(B  + 0 == A);
            ASSERT(5 == X.numElements());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.numElements());

            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(B - 0 == A);
            ASSERT(0 == X.numElements());
            ASSERT(1 == X.numSlots());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0);
            ASSERT(1 == X.numElements());

            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(B - 0 == A);
            ASSERT(0 == X.numElements());
            ASSERT(1 == X.numSlots());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0); mX.add(V1);
            ASSERT(2 == X.numElements());

            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(B - 0 == A);
            ASSERT(0 == X.numElements());
            ASSERT(1 == X.numSlots());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);
            ASSERT(3 == X.numElements());

            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(B - 0 == A);
            ASSERT(0 == X.numElements());
            ASSERT(1 == X.numSlots());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'add' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. add { V0, V1, V2, V3, V4, V0, ... }  up to length i
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
                    mX.add(VALUES[k % NUM_VALUES]);
                }

                const int len = i > NUM_VALUES ? NUM_VALUES : i;
                LOOP_ASSERT(i, len == X.numElements());                 // 3.
                for (k = 0; k < i; ++k) {
                  LOOP2_ASSERT(i, k, X.isMember(VALUES[k % NUM_VALUES]));
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
            //    6. add { V0, V1, V2, V3, V4, V0, ... }  up to length j
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
                        mX.add(VALUES[k % NUM_VALUES]);
                    }

                    const int len = i > NUM_VALUES ? NUM_VALUES : i;
                    LOOP2_ASSERT(i, j, len == X.numElements());         // 3.
                    for (k = 0; k < i; ++k) {
                      LOOP3_ASSERT(i, j, k,
                                   X.isMember(VALUES[k % NUM_VALUES]));
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.numElements());           // 5.
                    LOOP2_ASSERT(i, j, 1 == X.numSlots());

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.add(VALUES[k % NUM_VALUES]);
                    }

                    const int len2 = j > NUM_VALUES ? NUM_VALUES : j;
                    LOOP2_ASSERT(i, j, len2 == X.numElements());        // 7.
                    for (k = 0; k < j; ++k) {
                      LOOP3_ASSERT(i, j, k,
                                   X.isMember(VALUES[k % NUM_VALUES]));
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
        //      - basic accessors: 'length' and 'operator[]'
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
        ASSERT(0 == X1.numElements());

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
        ASSERT(0 == X2.numElements());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Add an element value A to x1)."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.add(VA);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.numElements());
        ASSERT(X1.isMember(VA));

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Add the same element value A to x2)."
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.add(VA);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(1 == X2.numElements());
        ASSERT(X2.isMember(VA));

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Add another element value B to x2)."
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.add(VB);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.numElements());
        ASSERT(X2.isMember(VA));
        ASSERT(X2.isMember(VB));

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
        ASSERT(0 == X1.numElements());

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
        ASSERT(0 == X3.numElements());

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

        ASSERT(2 == X4.numElements());
        ASSERT(X4.isMember(VA));
        ASSERT(X4.isMember(VB));
        ASSERT(0 == X4.isMember(VC));

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
        ASSERT(0 == X2.numElements());

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
        ASSERT(2 == X3.numElements());

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
        ASSERT(2 == X4.numElements());

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
