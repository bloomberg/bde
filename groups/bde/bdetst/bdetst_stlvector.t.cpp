// bdetst_stlvector.t.cpp    -*-C++-*-

#include <bdetst_stlvector.h>

#include <iostream>
#include <exception>

using namespace BloombergLP;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing 
// value-semantic containers.  We have chosen as *primary* *manipulators* the 
// 'insert' and 'clear' methods to be used by the generator functions
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
// [ 2] vector<T,A>(const A& a = A()); 
// [ 5] vector<T,A>(size_type n, const T& val = T(), const A& a = A());
// [--] template<class InputIter>
//      vector<T,A>(InputIter first, InputIter last, const A& a = A());
// [ 4] vector<T,A>(const vector<T,A>& orig);
// [ 2] ~vector<T,A>();
// [ 6] reference operator[](size_type pos);
// [ 6] const_reference operator[](size_type pos) const;
// [ 6] reference at(size_type pos);
// [ 6] const_reference at(size_type pos) const;
// [ 7] bool operator==(const basic_string<C,T,A>&, 
//                      const basic_string<C,T,A>&);
// [ 7] bool operator!=(const basic_string<C,T,A>&, 
//                      const basic_string<C,T,A>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [18] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void insert(T::iterator, size_type, const T&); //no aliasing
//
// [ 3] int ggg(T *object, const char *spec, int vF = 1);
// [ 3] T& gg(T *object, const char *spec);
// [ 8] T g(const char *spec);

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
#define P_(X) cout << #X " = " << (X) << ", "<<std::flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//
//      Structure and Class used as part of Test objects
//-----------------------------------------------------------------------------

struct SampleStruct {
    int x;
    SampleStruct() : x(0) { }
    SampleStruct(int i) : x(i) { }
    friend ostream& operator<<(ostream& os, const SampleStruct &v);
};

ostream& 
operator<<(ostream& os, const SampleStruct &v) { return os << v.x; }

class SampleClass {
    int x;
    bdema_Allocator *d_allocator_p;
  public:
    SampleClass(int value, bdema_Allocator *basicAllocator = 0);
    SampleClass(const SampleClass& original,
                bdema_Allocator *basicAllocator = 0);
    SampleClass(bdema_Allocator *basicAllocator = 0);
    ~SampleClass();

    SampleClass& operator=(const SampleClass&);

    friend int operator==(const SampleClass& lhs, const SampleClass& rhs);
    friend ostream& operator<<(ostream& os, const SampleClass&);
};

SampleClass::SampleClass(int value, bdema_Allocator *basicAllocator)
    : x(value)
    , d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

SampleClass::SampleClass(const SampleClass& original,
                           bdema_Allocator *basicAllocator)
: x(original.x)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

SampleClass::SampleClass(bdema_Allocator *basicAllocator)
: x(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

SampleClass::~SampleClass()
{
}

inline
SampleClass& SampleClass::operator=(const SampleClass& item)
{
    x = item.x;
    return *this;
}

ostream& operator<<(ostream& os, const SampleClass& rhs)
{
    return os << rhs.x;
}

inline
int operator==(const SampleClass& lhs, const SampleClass& rhs)
{
    return lhs.x == rhs.x;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef std::vector<SampleClass> Obj;
typedef SampleClass Element;

const Element VALUES[] = { 1, 2, 3, 4, 5 }; // avoid DEFAULT_VALUE

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4;  // conjunction with 'g' and 'gg'.

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

enum {
        PTR_SIZE = sizeof(int *),
        INT_SIZE = sizeof(int),
        LARGE_SIZE_VALUE = 10  // Declare a large value for insertions into the
                               // vector. Note this value will cause multiple
                               // resizes during insertion into the vector.
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void
stretch(Obj *object, int size, const Element& elem)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    object->insert(object->begin(), size, elem);
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
    object->clear();
    ASSERT(0 == object->size());
}

ostream&
operator<<(ostream& out, const Obj& object)
{
    Obj::const_iterator iter;
    for(iter = object.begin(); iter != object.end(); ++iter)
        out << *iter << " ";
    return out;
}

// int verifyInvariants(const Obj *object)
// {
//     return (int)strlen(object->c_str()) == object->length();
// }

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) string values to be
// appended to the 'basic_string' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'clear' method).
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
    // using only the primary manipulator function 'insert' and white-box 
    // manipulator 'clear'.  Optionally specify a zero 'verboseFlag' to 
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this 
    // function is used to implement 'gg' as well as allow for verification of 
    // syntax error detection. 
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->insert(object->end(),1,VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->clear();
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
 
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    bdema_Allocator *ALLOCATOR[] = { 
      // TBD - Check if 0 should work      
      //                                          0,
                                     &testAllocator,
                                   };
    const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // ---------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        // Concerns:
        //
        //   1. Objects constructed with similar values are returned as equal.
        //   2. Objects constructed such that they have same (logical) value but
        //      different internal representation (due to the lack or presence 
        //      of an allocator) should be returned as equal.
        //   3. Unequal objects are always returned as false. 
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
        //   its contents using 'clear'. Then recreate the original value and 
        //   verify that the second object still return equal to the first.
        //
        // Testing:
        //   operator==(const vector<T,A>&, const vector<T,A>&);
        //   operator!=(const vector<T,A>&, const vector<T,A>&);
        // --------------------------------------------------------------------

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
            0}; // Null string required as last element.


        if (verbose) cout << "\nCompare each pair of similar and different"
                          << " values (u, ua, v, va) in S X A X S X A"
                          << " without perturbation." << endl;
        {
            
            int oldLen = -1;

            // Create first object
            for (int si = 0; SPECS[si]; ++si) {
                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                    const char *const U_SPEC = SPECS[si];
                    const int curLen = (int)strlen(U_SPEC);

                    Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
                    LOOP2_ASSERT(si, ai, curLen == U.size()); // same lengths

                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;

                    if (curLen != oldLen) {
                        LOOP_ASSERT(U_SPEC, oldLen <= curLen);//non-decreasing
                        oldLen = curLen;
                    }

                    if (veryVerbose) { cout << "\t\t";
                        P_(si); P_(U_SPEC); P(U); }

                    // Create second object
                    for (int sj = 0; SPECS[sj]; ++sj) {
                        for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(ALLOCATOR[aj]);
                            const Obj& V = gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                cout << "\t\t";
                                P_(sj); P_(V_SPEC); P(V);
                            }

                            const int isSame = si == sj;
                            LOOP2_ASSERT(si, sj,  isSame == (U == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V));
                        }
                    }
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

            int oldLen = -1;

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

                    if (curLen != oldLen) {
                        LOOP_ASSERT(U_SPEC, oldLen <= curLen);//non-decreasing
                        oldLen = curLen;
                    }

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
                            }
                        }
                    }
                }
            }
        }

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESSORS:
        // Concerns:
        //   1. The returned value for operator[] and function at() is correct 
        //      as long as pos < size().
        //   2. The at() function throws out_of_range exception if 
        //      pos >= size().
        //   3. Changing the internal representation to get the same (logical)
        //      final value, should not change the result of the element 
        //      accesor functions.
        //   4. The internal memory management is correctly hooked up so that 
	//      changes made to the state of the object via these accessors 
	//      do change the state of the object.
        //
        // Plan:
        //   For 1 and 3 do the following:
        //   Specify a set S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        //   For 2, check that function at() throws a out_of_range exception
        //   when pos >= size().
	//
	//   For 4, For each value w in S, create a object x with w using
        //   'gg'. Create another empty object y and make it 'resize' capacity 
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

        if (verbose) cout << "\nTesting const and non-const versions of "
                          << "operator[] and function at() where pos < size()."
                          << endl;
        {
            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen       = LENGTH;

                for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                    Obj mX(ALLOCATOR[ai]);

                    const Obj& X = gg(&mX, SPEC);    // canonical organization

                    LOOP2_ASSERT(ti, ai, curLen == X.size()); // same lengths

                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;

                    if (curLen != oldLen) {
                        LOOP2_ASSERT(LINE, ai, oldLen <= curLen);
                              // non-decreasing
                        oldLen = curLen;
                    }

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
                     
                    for (; i < SZ; ++i) {
                        LOOP3_ASSERT(LINE, ai, i, 0 == e[i]);
                    }

                    // Check for perturbation.
                    static const int EXTEND[] = {
                        0, 1, 2, 3, 4, 5, 7, 8, 9, 15
                    };

                    const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

                    Obj mY(ALLOCATOR[ai]); 

                    const Obj& Y = gg(&mY, SPEC);

                    {                             // Perform the perturbation
                        for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                            stretchRemoveAll(&mY, EXTEND[ei],
                                             VALUES[ei % NUM_VALUES]);
                            gg(&mY, SPEC);

                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                            int j;
                            for (j = 0; j < LENGTH; ++j) {
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == mY[j]);
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == Y[j]);
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == mY.at(j));
                                LOOP4_ASSERT(LINE, ai, j, ei, e[j] == Y.at(j));
                            }
                     
                            for (; j < SZ; ++j) {
                                LOOP4_ASSERT(LINE, ai, j, ei, 0 == e[j]);
                            }
                        }
                    }
                }
            }
        }


        if (verbose) cout << "\nTesting non-const versions of operator[] and "
                          << "function at() modify state of object correctly."
                          << endl;
        {

            int oldLen = -1;
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

                    if (curLen != oldLen) {
                        LOOP2_ASSERT(LINE, ai, oldLen <= curLen);
                              // non-decreasing
                        oldLen = curLen;
                    }

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
                        } catch (std::out_of_range) {
                            ++exceptions;
                            if (veryVerbose) { 
			        cout << "In out_of_range exception.";
                                P(trials);
                            }
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
                        } catch (std::out_of_range) {
                            ++exceptions;
                            if (veryVerbose) { 
			        cout << "In out_of_range exception." ; 
                                P(trials);
                            }
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
        // TESTING INITIAL-LENGTH CONSTRUCTORS:
        //   We have the following concerns:
        //    1. The initial value is correct.
        //    2. The initial capacity is correctly set up.
        //    3. The constructor is exception neutral w.r.t. memory allocation.
        //    4. The internal memory management system is hooked up properly
        //       so that *all* internally allocated memory draws from a
        //       user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For the constructor we will create objects of varying sizes with
        //   different 'value' as argument. Test first with the default value
        //   for type T, and then test with different values. Perform the above
        //   tests:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //   and use basic accessors to verify
        //      - size
        //      - capacity
        //      - element value at each index position { 0 .. length - 1 }.
        //
        // Testing:
        //   vector<T,A>(size_type n, const T& value = T(), 
        //               const Allocator& a = A());
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Length Constructor" << endl
                          << "==================================" << endl;

        {

            static const struct {
                int         d_lineNum;          // source line number
                unsigned    d_length;           // expected length
                int         d_cap;              // expected capacity
            } DATA[] = {
                //line  length  capacity     
                //----  ------  --------    
                { L_,        0,        0,    },
                { L_,        1,        1,    },
                { L_,        2,        2,    },
                { L_,        3,        4,    },
                { L_,        4,        4,    },
                { L_,        5,        8,    },
                { L_,        6,        8,    },
                { L_,        7,        8,    },
                { L_,        8,        8,    },
                { L_,        9,       16,    },
                { L_,       11,       16,    },
                { L_,       12,       16,    },
                { L_,       14,       16,    },
                { L_,       15,       16,    },
                { L_,       16,       16,    },
                { L_,       17,       32,    },
                { L_,       31,       32,    },
                { L_,       32,       32,    },
                { L_,       33,       64,    },
                { L_,       65,      128,    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout << "\nTesting initial-length ctor with  "
                                 "(default) initial value." << endl;

            if (verbose) cout << "\tWithout passing in an allocator, "
                                 "using default value." << endl;
            {
                const int DEFAULT_VALUE = 0;

                int oldLen = -1;
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    
                    if (verbose) { cout << "\tCreating object of "; P(curLen);}

                    Obj mX(curLen);  const Obj& X = mX;
                
                    if (veryVerbose) { cout << "\t\t" ; P_(X);
                        int Cap = X.capacity(); P(Cap);
                    }

                    LOOP2_ASSERT(LINE, ti, curLen == X.size());
                    LOOP2_ASSERT(LINE, ti, curLen == X.capacity());       

                    for (int i = 0; i < curLen; ++i) {
                        LOOP3_ASSERT(LINE, ti, i, DEFAULT_VALUE == X[i]);
                    }
                }
            }

            if (verbose) cout << "\tWithout passing in an allocator, "
                                 "using non-default values." << endl;

            {

                int oldLen = -1;
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    const Element argValue = VALUES[ti % NUM_VALUES];

                    if (verbose) { 
                        cout << "\tCreating object of "; P_(curLen);
                        cout << " using Value : " << argValue << endl;
                    }
                               
                    Obj mX1(curLen, argValue);  const Obj& X1 = mX1;
                
                    if (veryVerbose) { cout << "\t\t" ; P_(X1);
                        int Cap = mX1.capacity(); P(Cap);
                    }

                    LOOP2_ASSERT(LINE, ti, curLen == X1.size());
                    LOOP2_ASSERT(LINE, ti, curLen == mX1.capacity());
                
                    for (int j = 0; j < curLen; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, argValue == X1[j]);
                    }
                }
            }

            if (verbose) cout << "\tPassing in an allocator, "
                                 "using default value." << endl;
            {
                const int DEFAULT_VALUE = 0;

                int oldLen = -1;
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    
                    if (verbose) { cout << "\tCreating object of "; P(curLen);}

                    const int BB = testAllocator.numBlocksTotal();   
                    const int  B = testAllocator.numBlocksInUse();

                    Obj mX(curLen, DEFAULT_VALUE, &testAllocator); 
                    const Obj& X = mX;
                
                    const int AA = testAllocator.numBlocksTotal(); 
                    const int  A = testAllocator.numBlocksInUse(); 
                    const int numBytes = testAllocator.lastAllocateNumBytes();

                    if (veryVerbose) { cout << "\t\t" ; P_(X);
                        int Cap = X.capacity(); P(Cap);    
                    }

                    LOOP2_ASSERT(LINE, ti, curLen == X.size());
                    LOOP2_ASSERT(LINE, ti, curLen == X.capacity());
                
                    for (int i = 0; i < curLen; ++i) {
                        LOOP3_ASSERT(LINE, ti, i, DEFAULT_VALUE == X[i]);
                    }

                    if (curLen == 0) {
                        LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(LINE, ti, BB + 1 == AA);  
                        LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                    }
                    LOOP2_ASSERT(LINE, ti, 
                                 numBytes == X.capacity() * sizeof(Element));
                }
            }


            if (verbose) cout << "\tPassing in an allocator, "
                                 "using non-default value." << endl;
            {

                int oldLen = -1;
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    const Element argValue = VALUES[ti % NUM_VALUES];
                    

                    if (verbose) { 
                        cout << "\tCreating object of "; P_(curLen);
                        cout << " using Value : " << argValue << endl;
                    }

                    const int BB = testAllocator.numBlocksTotal();  
                    const int  B = testAllocator.numBlocksInUse();   

                    Obj mX(curLen, argValue, &testAllocator); 
                    const Obj& X = mX;
                
                    const int AA = testAllocator.numBlocksTotal();         
                    const int  A = testAllocator.numBlocksInUse();        

                    if (veryVerbose) { cout << "\t\t" ; P_(X);
                        int Cap = X.capacity(); P(Cap);
                    }
                    
                    LOOP2_ASSERT(LINE, ti, curLen == X.size());
                    LOOP2_ASSERT(LINE, ti, curLen == X.capacity());
                
                    for (int j = 0; j < curLen; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, argValue == X[j]);
                    }

                    if (curLen == 0) {
                        LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(LINE, ti, BB + 1 == AA);        
                        LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
                    }
                }
            }


            if (verbose) cout << "\tWith passing an allocator and checking for"
                                 " allocation exceptions using default value."
                              << endl;
            {
                const int DEFAULT_VALUE = 0;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;

                    if (verbose) { cout << "\tCreating object of "; P(curLen);}
   
                    const int BB = testAllocator.numBlocksTotal();
                    const int  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) { cout << "\t\tBefore: "; P_(BB); P(B);}

                  BEGIN_BDEMA_EXCEPTION_TEST {
                                       
                    Obj mX(curLen, DEFAULT_VALUE, &testAllocator);  
                    const Obj& X = mX;
                
                    if (veryVerbose) { cout << "\t\t" ; P_(X);
                        int Cap = X.capacity(); P(Cap);
                    }

                    LOOP2_ASSERT(LINE, ti, curLen == X.size());
                    LOOP2_ASSERT(LINE, ti, curLen == X.capacity());
                
                    for (int i = 0; i < curLen; ++i) {
                        LOOP3_ASSERT(LINE, ti, i, DEFAULT_VALUE == X[i]);
                    }
                  } END_BDEMA_EXCEPTION_TEST

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                if (veryVerbose) { cout << "\t\tAfter : "; P_(AA); P(A);}
                    
                if (curLen == 0) {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 == AA);        
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }

                LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
            }

            if (verbose) cout << "\tWith passing an allocator and checking for"
                              " allocation exceptions using non-default value."
                              << endl;
            {

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    const Element argValue = VALUES[ti % NUM_VALUES];

                    if (verbose) { 
                        cout << "\tCreating object of "; P_(curLen);
                        cout << " using VALUE : " << argValue << endl;    
                    }

                    const int BB = testAllocator.numBlocksTotal();
                    const int  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) { cout << "\t\tBefore: "; P_(BB); P(B);}

                  BEGIN_BDEMA_EXCEPTION_TEST {
                                     
                    Obj mX(curLen, argValue, &testAllocator);  
                    const Obj& X = mX;
                
                    if (veryVerbose) { cout << "\t\t" ; P_(X);
                        int Cap = X.capacity(); P(Cap);
                    }

                    LOOP2_ASSERT(LINE, ti, curLen == X.size());
                    LOOP2_ASSERT(LINE, ti, curLen == X.capacity());
                
                    for (int i = 0; i < curLen; ++i) {
                        LOOP3_ASSERT(LINE, ti, i, argValue == X[i]);
                    }
                  } END_BDEMA_EXCEPTION_TEST

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) { cout << "\t\tAFTER : "; P_(AA); P(A);}
                    
                    if (curLen == 0) {
                        LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(LINE, ti, BB + 1 == AA);        
                        LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                    }

                    LOOP2_ASSERT(LINE, ti, 
                                 0 == testAllocator.numBlocksInUse());
                    }
                }
            }


            if (verbose) cout << "\tWith passing a bdema_BufferAllocator and "
                                 "finding memory leaks using default value."
                              << endl;
            {
                const int DEFAULT_VALUE = 0;
                char memory[8192];
                bdema_BufferAllocator a(memory, sizeof memory);

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;

                    if (verbose) { cout << "\tCreating object of "; P(curLen);}

                    void *doNotDelete = new(a.allocate(sizeof(Obj)))
                                        Obj(curLen, DEFAULT_VALUE, &a);

                    // No destructor is called; will produce memory leak in 
                    // purify if internal allocators are not hooked properly.

                }
            }
 
            if (verbose) cout << "\tWith passing a bdema_BufferAllocator and "
                               "finding memory leaks using non-default value."
                              << endl;
            {
                char memory[8192];
                bdema_BufferAllocator a(memory, sizeof memory);


                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int LINE         = DATA[ti].d_lineNum;
                    const unsigned LENGTH  = DATA[ti].d_length;
                    const int CAPACITY     = DATA[ti].d_cap;
                    const unsigned curLen  = LENGTH;
                    const Element argValue = VALUES[ti % NUM_VALUES];

                    if (verbose) {
                        cout << "\tCreating object of "; P_(curLen);
                        cout << " using VALUE : " << argValue << endl;    
                    }

                    void *doNotDelete = new(a.allocate(sizeof(Obj)))
                                        Obj(curLen, argValue, &a);

                    // No destructor is called; will produce memory leak in 
                    // purify if internal allocators are not hooked properly.

                }
            }
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        // We have the following concerns:
        //   1. The new object's value is the same as that of the original
        //        object (relying on the equality operators) and created with
        //        the correct capacity.
        //   2. All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3. The value of the original object is left unaffected.
        //   4. Subsequent changes in or destruction of the source object have
        //        no effect on the copy-constructed object.
        //   5. Subsequent changes (inserts) on the created object have no 
        //        effect on the original and change the capacity of the new 
        //        object correctly.
        //   6. The object has its internal memory management system hooked up
        //        properly so that *all* internally allocated memory draws
        //        from a user-supplied allocator whenever one is specified.
        //   7. The function is exception neutral w.r.t. memory allocation.
        // Plan:
        //   Specify a set S of object values with substantial and varied 
        //   differences, ordered by increasing length, to be used in the 
        //   following tests.
        //
        //   For concerns 1 - 4, for each value in S, initialize objects w and
        //   x, copy construct y from x and use 'operator==' to verify that 
        //   both  x and y subsequently have the same value as w. Let x go out
        //   of scope and again verify that w == y.  
        //
        //   For concern 5, for each value in S initialize objects w and x, 
        //   and copy construct y from x. Change the state of y, by using the 
        //   *primary* *manipulator* 'insert'. Using the 'operator!=' verify 
        //   that y differs from x and w, and verify that the capacity of y 
        //   changes correctly. 
        //
        //   To address concern 6, we will perform tests performed for concern 
        //   1:
        //     - While passing a testAllocator as a parameter to the new object
        //       and ascertaining that the new object gets its memory from the 
        //       provided testAllocator. Also perform test for concerns 2 and 5.
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferAllocator') and never destroyed.
        //
        //   To address concern 7, perform tests for concern 1 performed 
        //   in the presence of exceptions during memory allocations using a
        //   'bdema_TestAllocator' and varying its *allocation* *limit*.
        //
        // Testing:
        //   vector<T,A>(const vector<T,A>& original);
        //
        //
        // TBD When a new vector object Y is created from an old vector object
        //      X, then the standard states that Y should get its allocator by
        //      copying X's allocator (23.1, Point 8). The STLport vector 
        //      implementation does not follow this rule for bdema_Allocator
        //      based allocators. To verify this behaviour for non 
        //      bdema_Allocator, should test, copy constructor using one  
        //      and verify standard is followed.
        //        
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\n\tTesting Vector Copy constructor." << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { 
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };

            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int) strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW; gg(&mW, SPEC);
                const Obj& W = mW;

                LOOP_ASSERT(ti, curLen == W.size()); // same lengths
                if (veryVerbose) { cout << "\tControl Obj: "; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\tExtend By  : "; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj& mX = *pX;
         
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);
         
                    if (veryVerbose) { cout << "\t\tDynamic Obj: "; P(X); }

                    {                            // Testing concern 1
                        if (verbose) { cout << "\t\t\tRegular Case :" << endl;}

                        const Obj Y0(X);

                        if (veryVerbose) { 
                            cout << "\t\t\t\tObj : "; P_(Y0); 
                            int Cap = Y0.capacity(); P(Cap);
                        }

                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                        LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                            bdema_Default::defaultAllocator());

                        // STLPORT: STLPort Vector implementation on copy
                        // construction creates an object that has capacity 
                        // equal to the size of the original object.

                        LOOP2_ASSERT(SPEC, N, curLen == Y0.capacity());
                        
                    }


                    {                            // Testing concern 5

                        if (verbose) { 
                            cout << "\t\t\tInsert into created obj, "
                                    "without test allocator:" << endl;
                        }
    
                        Obj Y1(X);

                        if (veryVerbose) { 
                            cout << "\t\t\t\tBefore Insert: "; P(Y1);
                        }
                        
                        for (int i = 1; i < N+1; ++i) {
                            const int oldCap = Y1.capacity();
                            const int remSlots = Y1.capacity() - Y1.size();

                            stretch(&Y1, 1, VALUES[i % NUM_VALUES]);
                          
                            if (veryVerbose) { 
                                cout << "\t\t\t\tAfter Insert : "; 
                                int Cap = Y1.capacity(); P_(Cap); P_(i); P(Y1);
                            }
                            
                            LOOP3_ASSERT(SPEC, N, i, Y1.size() == curLen + i);
                            LOOP3_ASSERT(SPEC, N, i, W != Y1);
                            LOOP3_ASSERT(SPEC, N, i, X != Y1);

                            if (oldCap == 0) {
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y1.capacity() == 1);
                            }                                
                            else if (remSlots == 0) {
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y1.capacity() == 2 * oldCap);
                            }
                            else { 
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y1.capacity() == oldCap);
                            }
                        }
                    }

                    {                            // Testing concern 5 with
                                                 // test allocator.

                        if (verbose) { 
                            cout << "\t\t\tInsert into created obj, "
                                    "with test allocator:" << endl;
 
                        }

                        const int BB = testAllocator.numBlocksTotal();
                        const int  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { 
                            cout << "\t\t\t\tBefore Creation: "; P_(BB); P(B);
                        }

                        Obj Y11(X, &testAllocator);

                        const int AA = testAllocator.numBlocksTotal();
                        const int  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) { 
                            cout << "\t\t\t\tAfter Creation: "; P_(AA); P(A);
                            cout << "\t\t\t\tBefore Append: "; P(Y11);
                        }
                        
                        if (curLen == 0) {
                            LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                            LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                        }
                        else {
                            LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                            LOOP2_ASSERT(SPEC, N,  B + 1 ==  A);
                        }
                            
                        for (int i = 1; i < N+1; ++i) {
 
                            const int oldCap = Y11.capacity();
                            const int remSlots = Y11.capacity() - Y11.size();

                            const int CC = testAllocator.numBlocksTotal();
                            const int  C = testAllocator.numBlocksInUse();
                            
                            stretch(&Y11, 1, VALUES[i % NUM_VALUES]);
                          
                            const int DD = testAllocator.numBlocksTotal();
                            const int  D = testAllocator.numBlocksInUse();
                            
                            // Blocks allocated should increase only when 
                            // trying to add more than capacity. When adding
                            // the first element numBlocksInUse will increase
                            // by 1. In all other conditions numBlocksInUse 
                            // should remain the same. 

                            if (curLen == 0 && i == 1) {
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 1 ==  D);
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y11.capacity() == 1);
                            }
                            else if (remSlots == 0){
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y11.capacity() == 2 * oldCap);
                            }
                            else {
                                LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                                LOOP3_ASSERT(SPEC, N, i, 
                                             Y11.capacity() == oldCap);
                            }
                            
                            if (veryVerbose) { 
                                cout << "\t\t\t\tAfter Append : "; 
                                P_(i); P(Y11);
                            }

                            LOOP3_ASSERT(SPEC, N, i, Y11.size() == curLen + i);
                            LOOP3_ASSERT(SPEC, N, i, W != Y11);
                            LOOP3_ASSERT(SPEC, N, i, X != Y11);
                            LOOP3_ASSERT(SPEC, N, i,
                                Y11.get_allocator() == X.get_allocator());
                        }
                    }

                    {                            // Exception checking

                        const int BB = testAllocator.numBlocksTotal();
                        const int  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { 
                            cout << "\t\t\t\tBefore Creation: "; P_(BB); P(B);
                        }

                      BEGIN_BDEMA_EXCEPTION_TEST {     
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { 
                            cout << "\t\t\tException Case  : " << endl;
                            cout << "\t\t\t\tObj : "; P(Y2); 
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                        LOOP2_ASSERT(SPEC, N, 
                            Y2.get_allocator() == X.get_allocator());
                      } END_BDEMA_EXCEPTION_TEST

                        const int AA = testAllocator.numBlocksTotal();
                        const int  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) { 
                            cout << "\t\t\t\tAfter Creation: "; P_(AA); P(A);
                        }
                        
                        if (curLen == 0) {
                            LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                            LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                        }
                        else {
                            LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                            LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                        }
                    }

                    {                            // Buffer Allocator.
                        char memory[8192];
                        bdema_BufferAllocator a(memory, sizeof memory);
                        Obj *Y3 = new(a.allocate(sizeof(Obj))) Obj(&a);
                                                 
                        gg(Y3, SPEC);
                        const Obj Y4(*Y3, &a);
                        if (veryVerbose) { 
                            cout << "\t\t\tBuffer Case : " << endl; 
                            cout << "\t\t\t\tObj : ";P(Y4); 
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y4);
                        LOOP2_ASSERT(SPEC, N, W == *Y3);
                        LOOP2_ASSERT(SPEC, N, 
                            Y3->get_allocator() == Y4.get_allocator());

                    }

                    {                            // with 'original' destroyed
                        Obj Y5(X);
                        if (veryVerbose) { 
                            cout << "\t\t\tWith Original deleted: " << endl;
                            cout << "\t\t\t\tBefore Delete : "; P(Y5); 
                        }

                        delete pX;

                        LOOP2_ASSERT(SPEC, N, W == Y5);

                        for (int i = 1; i < N+1; ++i) {
                            stretch(&Y5, 1, VALUES[i % NUM_VALUES]);
                            if (veryVerbose) { 
                                cout << "\t\t\t\tAfter Append to new obj : "; 
                                P_(i);P(Y5);
                            }
                            LOOP3_ASSERT(SPEC, N, i, W != Y5);
                        }
                    }
                }
            }
        } 
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
        //   'clear').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'ggg' to set the state of a newly created object.  Verify that
        //   'ggg' returns the expected value corresponding to the location of
        //   the first invalid value of the 'spec'.  Repeat the test for a
        //   longer 'spec' generated by prepending a string ending in a '~'
        //   character (denoting 'clear').  Note that we are testing the
        //   parser only; the primary manipulators are already assumed to work.
        //
        // Testing:
        //   vector<T,A>& gg(vector<T,A> *object, const char *spec);
        //   int ggg(vector<T,A> *object, const char *spec, int vF = 1);
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
            }
        }

      } break ;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators 
        //      - insert                (black-box)
        //      - clear                 (white-box)
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
        //    3. 'insert'
        //        3a. produces the expected value.
        //        3b. increases capacity as needed.
        //        3c. maintains valid internal state.
        //        3d. is exception neutral with respect to memory allocation.  
        //    4. 'clear' 
        //        4a. produces the expected value (empty).
        //        4b. properly destroys each contained element value.
        //        4c. maintains valid internal state.
        //        4d. does not allocate memory.
        //    5. The size based parameters of the class reflect the platform.
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
        //   After the final insert operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object 
        //   and the 'bdema_TestAllocator' to confirm whether a resize has
        //   occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'insert' with 'clear'; this time, however, use the test 
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        // 
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation 
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'insert' 
        //          to create an instance of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'insert' to create an 
        //          instance of length i, use 'clear' to clear its value 
        //          and confirm (with 'length'), use insert to set the instance
        //          to a value of length j, verify the value, and allow the 
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'clear' is not 
        //   called; if only the second test produces an error, we know that
        //   'clear' is to blame.  We will rely on 'bdema_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d. 
        //
        //   To address concern 5, the values will be explicitly compared to
        //   the expected values.  This will be done first so as to ensure all
        //   other tests are reliable and may depend upon the class's
        //   constants.
        //
        // Testing:
        //   vector<T,A>(const A& a = A());
        //   ~vector<T,A>();
        //   BOOTSTRAP: 
        //   void insert(T::iterator,size_type,const T&); // no aliasing
        //   void clear();
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
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.size());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.size());
          } END_BDEMA_EXCEPTION_TEST
            ASSERT( 0 == testAllocator.numBlocksInUse());
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

        if (verbose) cout << "\nTesting 'insert' (bootstrap) without "
                          << "allocator." << endl;
        
        {
            const unsigned NUM_TRIALS = LARGE_SIZE_VALUE;
                        
 
            for (int li = 0; li < NUM_TRIALS; ++li) {
                if (verbose) cout << "\tOn an object of initial length "
                                  << li << "." << endl;
                Obj mX;  const Obj& X = mX;
               
                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }
                
                LOOP_ASSERT(li, li == X.size());

                if(veryVerbose){
                    cout <<"\t\tBEFORE:";
                    int Cap = X.capacity(); P_(Cap); P(X);
                }

                mX.insert(mX.end(), 1, VALUES[li % NUM_VALUES]);

                if(veryVerbose){
                    cout <<"\t\tAFTER:";
                    int Cap = X.capacity(); P_(Cap); P(X); 
                }

                LOOP_ASSERT(li, li + 1 == X.size());
             
                for (int i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }

                LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'insert' (bootstrap) with allocator."
                          << endl;
        {
            const int NUM_TRIALS = LARGE_SIZE_VALUE;
            for (int li = 0; li < NUM_TRIALS; ++li) {
                if (verbose) cout << "\tOn an object of initial length "
                                  << li << "." << endl;
                Obj mX(&testAllocator);  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                LOOP_ASSERT(li, li == X.size());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tBEFORE: ";
                    P_(BB); P_(B);
                    int Cap = X.capacity(); P_(Cap); P(X);
                }

                mX.insert(mX.end(), 1, VALUES[li % NUM_VALUES]);

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\t AFTER: ";
                    P_(AA); P_(A);
                    int Cap = X.capacity(); P_(Cap); P(X);
                }

                //STLPort: Implementation of STLport Vector increases capacity 
                //         only if the current length is zero or a power of 2.

                if (li == 0) {
                    LOOP_ASSERT(li, BB + 1 == AA);
                    LOOP_ASSERT(li, B + 1 == A);
                }
                else if((li & (li - 1)) == 0) {
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
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'clear' without allocator."
                          << endl;
        {
            const int NUM_TRIALS = LARGE_SIZE_VALUE;

            for (int li = 0; li < NUM_TRIALS; ++li) {
                if (verbose) cout << "\tOn an object of initial length "
                                  << li << "." << endl;
                Obj mX;  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                if(veryVerbose){
                    cout <<"\t\tBEFORE";
                    int Cap = X.capacity(); P_(Cap); P(X);  
                }

                LOOP_ASSERT(li, li == X.size());

                mX.clear();

                if(veryVerbose){
                    cout <<"\t\tAFTER";
                    int Cap = X.capacity(); P_(Cap); P(X);  
                }

                LOOP_ASSERT(li, 0 == X.size());

                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                LOOP_ASSERT(li, li == X.size());

                if(veryVerbose){
                    cout <<"\t\tAFTER SECOND INSERT ";
                    int Cap = X.capacity(); P_(Cap); P(X);  
                }

            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'clear' with allocator." << endl;
        {
            const int NUM_TRIALS = LARGE_SIZE_VALUE;
            for (int li = 0; li < NUM_TRIALS; ++li) {
                if (verbose) cout << "\tOn an object of initial length "
                                  << li << "." << endl;
                Obj mX(&testAllocator);  const Obj& X = mX;

                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                LOOP_ASSERT(li, li == X.size());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\tBEFORE: ";
                    P_(BB);P_(B);
                    int Cap = X.capacity();P_(Cap);P(X);
                }

                mX.clear();

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    cout << "\t\t AFTER: ";
                    P_(AA); P_(A);
                    int Cap = X.capacity(); P_(Cap); P(X);
                }

                for (int i = 0; i < li; ++i) {
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                LOOP_ASSERT(li, li == X.size());

                const int CC = testAllocator.numBlocksTotal();
                const int C  = testAllocator.numBlocksInUse();

                if(veryVerbose){
                    cout << "\t\t AFTER SECOND INSERT: ";
                    P_(CC); P_(C);
                    int Cap = X.capacity(); P_(Cap); P(X);
                }

                LOOP_ASSERT(li, BB == AA);
                LOOP_ASSERT(li, BB == CC);

                LOOP_ASSERT(li, B  - 0 == A);
                LOOP_ASSERT(li, B  - 0 == C);
                LOOP_ASSERT(li, li == X.size());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << 
          "\nTesting the destructor and exception neutrality with allocator."
                          << endl;

        if (verbose) cout << "\tWith 'insert' only" << endl;
        {
            // For each lengths li up to some modest limit:
            //    1. create an instance
            //    2. insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope
            //    5. make sure that the destructor cleans up

            const int NUM_TRIALS = LARGE_SIZE_VALUE;
            for (int li = 0; li < NUM_TRIALS; ++li) { // i is the length
                if (verbose) cout << 
                    "\t\tOn an object of length " << li << '.' << endl;

              BEGIN_BDEMA_EXCEPTION_TEST {

                Obj mX(&testAllocator);  const Obj& X = mX;              // 1.
                for (int i = 0; i < li; ++i) {                           // 2.
                    mX.insert(mX.end(), 1, VALUES[i % NUM_VALUES]);
                }

                LOOP_ASSERT(li, li == X.size());                         // 3.
                for (int i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }

              } END_BDEMA_EXCEPTION_TEST                                 // 4.
              LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
            }
        }

        if (verbose) cout << "\tWith 'insert' and 'clear'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. insert V0 values up to a length of i
            //    3. verify initial length and contents
            //    4. clear contents from instance 
            //    5. verify length is 0
            //    6. insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope
            //    9. make sure that the destructor cleans up

            const int NUM_TRIALS = LARGE_SIZE_VALUE;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout << 
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout << 
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.insert(mX.end(), 1, V0);
                    }

                    LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, V0 == X[k]);
                    }

                    mX.clear();                                         // 4.
                    LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.insert(mX.end(), 1, VALUES[k % NUM_VALUES]);
                    }

                    LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }

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
        //      - primary manipulators: 'insert' and 'clear' methods
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

        std::allocator<int> zza(&testAllocator);
        std::vector<int, std::allocator<void*> > zz1, zz2(zza);

        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << 
            "\ta. Check initial state of x1." << endl;

        ASSERT(0 == X1.size());

        if(veryVerbose){
            int capacity = X1.capacity();
            cout <<"\t\t";
            P(capacity);
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
        mX1.insert(mX1.end(),1,VA);
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
        mX2.insert(mX2.end(),1,VA);
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
        mX2.insert(mX2.end(),1,VB);
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
        mX1.clear();
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

      } break;

      case -1: {
        // --------------------------------------------------------------------
        // TEST SAFE MODE
        // Perform undefined behavior and confirm that STLPort produces an
        // understandable error message and aborts.  Will not produce an error
        // if not compiled in safe mode (though there is a small chance it will
        // segv).
        // --------------------------------------------------------------------
        std::vector<int> x(5, 2);
        x[5] = 3; // Out-of-bounds
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



