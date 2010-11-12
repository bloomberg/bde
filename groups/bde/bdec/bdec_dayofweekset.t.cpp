// bdec_dayofweekset.t.cpp         -*-C++-*-

#include <bdec_dayofweekset.h>

#include <bsls_platformutil.h>                  // for testing only

#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <bsl_new.h>          // placement syntax
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
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
// sorting the elements in expected test case results.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [10] int maxSupportedBdexVersion() const;
//
// CREATORS
// [ 2] bdec_DayOfWeekSet();
// [ 7] bdec_DayOfWeekSet(const bdec_DayOfWeekSet& original);
// [ 2] ~bdec_DayOfWeekSet();
//
// MANIPULATORS
// [ 9] bdec_DayOfWeekSet& operator=(const bdec_DayOfWeekSet& rhs);
// [14] bdec_DayOfWeekSet& operator|=(const bdec_DayOfWeekSet& rhs);
// [14] bdec_DayOfWeekSet& operator&=(const bdec_DayOfWeekSet& rhs);
// [14] bdec_DayOfWeekSet& operator^=(const bdec_DayOfWeekSet& rhs);
// [14] bdec_DayOfWeekSet& operator-=(const bdec_DayOfWeekSet& rhs);
// [12] void add(int value);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [12] int remove(int value);
// [ 2] void removeAll();
//
// ACCESSORS
// [13] bool areMembers(const bdec_DayOfWeekSet& set) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] iterator begin() const;
// [ 4] iterator end() const;
// [ 4] bool isMember(bdet_Month::Month value) const;
// [ 4] int length() const;
// [ 4] reverse_iterator rbegin() const;
// [ 4] reverse_iterator rend() const;
//
// FREE OPERATORS
// [14] bdec_DayOfWeekSet  operator~(const bdec_DayOfWeekSet& set);
// [14] operator|(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [14] operator&(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [14] operator^(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [14] operator-(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [ 6] operator==(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [ 6] operator!=(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
// [ 5] operator<<(ostream&, const bdec_DayOfWeekSet&);
//--------------------------------------------------------------------------
// [ 4] bdec_DayOfWeekSet_Iter();
// [ 4] bdec_DayOfWeekSet_Iter(int data, int index);
// [ 4] bdec_DayOfWeekSet_Iter(const bdec_DayOfWeekSet_Iter& original);
// [ 4] ~bdec_DayOfWeekSet_Iter();
// [ 4] bdec_DayOfWeekSet_Iter& operator=(c bdec_DayOfWeekSet_Iter&);
// [ 4] bdec_DayOfWeekSet_Iter& operator++();
// [ 4] bdec_DayOfWeekSet_Iter& operator--();
// [ 4] const bdet_DayOfWeek::Day& operator*() const;
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP: void add(int value);
// [15] USAGE EXAMPLE
// [ 3] bdec_DayOfWeekSet& gg(bdec_DayOfWeekSet* obj, const char *spec);
// [ 3] int ggg(bdec_DayOfWeekSet *object, const char *spec, int vF = 1);
// [ 3] bool arePrintedValuesEquivalent (const char *a, const char *b);
// [ 8] bdec_DayOfWeekSet g(const char *spec);
//==========================================================================

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdec_DayOfWeekSet       Obj;
typedef bdet_DayOfWeek::Day     Element;
typedef bdec_DayOfWeekSet_Iter  Iterator;

const Element VALUES[] =
{
    bdet_DayOfWeek::BDET_SUN,
    bdet_DayOfWeek::BDET_TUE,
    bdet_DayOfWeek::BDET_THU,
    bdet_DayOfWeek::BDET_MON,
    bdet_DayOfWeek::BDET_FRI,
    bdet_DayOfWeek::BDET_SAT,
    bdet_DayOfWeek::BDET_WED
};

const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
              &V1 = VALUES[1], &VB = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VE = V4,  // conjunction with 'g' and 'gg'.
              &V5 = VALUES[5], &VF = V5,
              &V6 = VALUES[6], &VG = V6;

const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

enum {
  SU = 1 << bdet_DayOfWeek::BDET_SUN,
  MO = 1 << bdet_DayOfWeek::BDET_MON,
  TU = 1 << bdet_DayOfWeek::BDET_TUE,
  WE = 1 << bdet_DayOfWeek::BDET_WED,
  TH = 1 << bdet_DayOfWeek::BDET_THU,
  FR = 1 << bdet_DayOfWeek::BDET_FRI,
  SA = 1 << bdet_DayOfWeek::BDET_SAT
};

const int BIT_MASK[] = {
    0, // filler
    SU,
    MO,
    TU,
    WE,
    TH,
    FR,
    SA,
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bool arePrintedValuesEquivalent(const char *a, const char *b)
    // Return 'true' if the specified string 'a' is a reordering of the values
    // listed in the specified string 'b', and 'false' otherwise.
{
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
                return false;
            }

            c[i + len] = oldValue;
            i += len;
        }
    }

    for (i = 0; c[i] != '\0'; ++i) {
        if (c[i] != ' ') {
            delete [] d;
            delete [] c;
            return false;
        }
    }

    for (i = 0; d[i] != '\0'; ++i) {
        if (d[i] != ' ') {
            delete [] d;
            delete [] c;
            return false;
        }
    }

    delete [] d;
    delete [] c;
    return true;
}

static Element nextElement(const Element *array,
                           int            arrayLen,
                           Element        element)
    // Returns the smallest element that is greater than the specified
    // 'element' from the specified 'array' (of the specified 'arrayLen').
    // Return 'element' if there are no elements in 'array' that are greater
    // than 'element'.
{
    Element ret = element;

    for (int i = 0; i < arrayLen; ++i) {
        if (element < array[i]) {
            if (ret == element) {
                if (ret < array[i]) {
                    ret = array[i];
                }
            }
            else if (ret > array[i]) {
                ret = array[i];
            }
        }
    }

    return ret;
}

static int sortElementsAndRemoveDuplicates(Element       *dest,
                                           int            destLen,
                                           const Element *src,
                                           int            srcLen)
    // Load into the specified 'dest' array (of the specified 'destLen') the
    // elements from the specified 'src' array (of the specified 'srcLen'), in
    // ascending order and removing duplicated elements.  Return the number of
    // elements added.  The behavior is undefined unless srcLen <= destLen.
{
    ASSERT(srcLen <= destLen);

    Element last   = Element(0);
    int     dstPos = 0;

    for (Element cur = nextElement(src, srcLen, last);
         cur != last;
         last = cur, cur = nextElement(src, srcLen, last)) {
        dest[dstPos++] = cur;
    }

    return dstPos;
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. G] correspond to arbitrary (but unique) DayOfWeek values to be
// added to the 'bdec_DayOfWeekSet' object.  A tilde ('~') indicates that
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
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G'
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
//              remove all the elements (set set length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Add three values corresponding to A, B, and C; empty
//              the object; and add values corresponding to D and E.
//
//-----------------------------------------------------------------------------

int ggg(bdec_DayOfWeekSet *object, const char *spec, int verboseFlag = 1)
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
        if ('A' <= spec[i] && spec[i] <= 'G') {
            object->add(VALUES[spec[i] - 'A']);
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

bdec_DayOfWeekSet& gg(bdec_DayOfWeekSet *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdec_DayOfWeekSet g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdec_DayOfWeekSet object;
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

///Usage
///-----
// A 'bdec_DayOfWeekSet' is useful for recording recurring appointments,
// or special days (e.g., weekend days), in a calendar.  The following
// snippets of code illustrate how to create and use a 'bdec_DayOfWeek' set.
// We'll start by creating a couple of commonly useful sets.  First we
// define the 'bdec_dayOfWeekSet' 'weekendDays':
//..
    bdec_DayOfWeekSet weekendDays;  ASSERT(0 == weekendDays.length());
//..
// Notice that, this set is initially empty.  Next, let's add the days that
// characterize weekends:
//..
    weekendDays.add(bdet_DayOfWeek::BDET_SUN);
    ASSERT(1 == weekendDays.length());

    weekendDays.add(bdet_DayOfWeek::BDET_SAT);
    ASSERT(2 == weekendDays.length());
//..
// Observe that 'weekendDays' now contains precisely the days we expect it to
// contain:
//..
    ASSERT(true  == weekendDays.isMember(bdet_DayOfWeek::BDET_SUN));
    ASSERT(false == weekendDays.isMember(bdet_DayOfWeek::BDET_MON));
    ASSERT(false == weekendDays.isMember(bdet_DayOfWeek::BDET_TUE));
    ASSERT(false == weekendDays.isMember(bdet_DayOfWeek::BDET_WED));
    ASSERT(false == weekendDays.isMember(bdet_DayOfWeek::BDET_THU));
    ASSERT(false == weekendDays.isMember(bdet_DayOfWeek::BDET_FRI));
    ASSERT(true  == weekendDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// Now let's create the complementary 'bdec_DayOfWeekSet' 'weekDays' directly
// from 'weekendDays' via a combination of unary negation and copy
// construction:
//..
    bdec_DayOfWeekSet weekDays(~weekendDays);

    ASSERT(5 == weekDays.length());

    ASSERT(false == weekDays.isMember(bdet_DayOfWeek::BDET_SUN));
    ASSERT(true  == weekDays.isMember(bdet_DayOfWeek::BDET_MON));
    ASSERT(true  == weekDays.isMember(bdet_DayOfWeek::BDET_TUE));
    ASSERT(true  == weekDays.isMember(bdet_DayOfWeek::BDET_WED));
    ASSERT(true  == weekDays.isMember(bdet_DayOfWeek::BDET_THU));
    ASSERT(true  == weekDays.isMember(bdet_DayOfWeek::BDET_FRI));
    ASSERT(false == weekDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// If we want to create a set containing all of the days in the week, we could
// do so via unary negation of the default constructed value:
//..
    const bdec_DayOfWeekSet NO_DAYS;
    const bdec_DayOfWeekSet ALL_DAYS(~NO_DAYS);

    ASSERT(7 == ALL_DAYS.length());
//..
// Observe that neither 'weekDays' nor 'weekendDays' represent the same value
// as 'ALL_DAYS', but their union does:
//..
    ASSERT(ALL_DAYS != weekendDays);
    ASSERT(ALL_DAYS != weekDays);
    ASSERT(ALL_DAYS == (weekDays | weekendDays));
    ASSERT(ALL_DAYS == (weekDays ^ weekendDays));

    ASSERT(weekendDays == ALL_DAYS - weekDays);

    ASSERT(weekDays    == ALL_DAYS - weekendDays);

    ASSERT(weekDays    == ALL_DAYS - weekendDays);
//..
// Similarly, neither 'weekDays' nor 'weekendDays' represents the same value as
// 'NO_DAYS', but their intersection does:
//..
    ASSERT(NO_DAYS != weekendDays);
    ASSERT(NO_DAYS != weekDays);
    ASSERT(NO_DAYS == (weekDays & weekendDays));

    ASSERT(weekendDays == weekendDays - weekDays);

    ASSERT(weekDays    == weekDays - weekendDays);
//..
// The only days of the week that have an 'E' in them are 'TUESDAY' and
// 'WEDNESDAY'.  Let's create the corresponding set 'eDays':
//..
    bdec_DayOfWeekSet eDays;                 ASSERT(0 == eDays.length());
    eDays.add(bdet_DayOfWeek::BDET_TUE);     ASSERT(1 == eDays.length());
    eDays.add(bdet_DayOfWeek::BDET_WED);     ASSERT(2 == eDays.length());

    ASSERT(false == eDays.isMember(bdet_DayOfWeek::BDET_SUN));
    ASSERT(false == eDays.isMember(bdet_DayOfWeek::BDET_MON));
    ASSERT(true  == eDays.isMember(bdet_DayOfWeek::BDET_TUE));
    ASSERT(true  == eDays.isMember(bdet_DayOfWeek::BDET_WED));
    ASSERT(false == eDays.isMember(bdet_DayOfWeek::BDET_THU));
    ASSERT(false == eDays.isMember(bdet_DayOfWeek::BDET_FRI));
    ASSERT(false == eDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// The only days of the week that have an 'N' in them are 'MONDAY' and
// 'WEDNESDAY', and 'SUNDAY'.  Let's create the corresponding set 'nDays'
// starting with the value of 'eDays' by first removing 'TUESDAY',
// and then adding 'SUNDAY' and 'MONDAY':
//..
    bdec_DayOfWeekSet nDays(eDays);          ASSERT(2 == nDays.length());

    nDays.remove(bdet_DayOfWeek::BDET_TUE);  ASSERT(1 == nDays.length());

    nDays.add(bdet_DayOfWeek::BDET_SUN);     ASSERT(2 == nDays.length());
    nDays.add(bdet_DayOfWeek::BDET_MON);     ASSERT(3 == nDays.length());

    ASSERT(true  == nDays.isMember(bdet_DayOfWeek::BDET_SUN));
    ASSERT(true  == nDays.isMember(bdet_DayOfWeek::BDET_MON));
    ASSERT(false == nDays.isMember(bdet_DayOfWeek::BDET_TUE));
    ASSERT(true  == nDays.isMember(bdet_DayOfWeek::BDET_WED));
    ASSERT(false == nDays.isMember(bdet_DayOfWeek::BDET_THU));
    ASSERT(false == nDays.isMember(bdet_DayOfWeek::BDET_FRI));
    ASSERT(false == nDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// Observe that all 'eDays' are 'weekDays', but that's not true of 'nDays':
//..
    ASSERT(true  == weekDays.areMembers(eDays));
    ASSERT(false == weekDays.areMembers(nDays));
//..
///Iterator Usage
/// - - - - - - -
// Finally observe that iteration order is defined by increasing enumerated
// 'bdet_DayOfWeek::Day' value '[ SUN .. SAT ]'.  The following use of the
// *forward* (bi-directional) iterator:
//..
    for (bdec_DayOfWeekSet::iterator it  = ALL_DAYS.begin();
                                     it != ALL_DAYS.end();
                                   ++it) {
        bsl::cout << *it << bsl::endl;
    }
//..
// produces:
//..
//  SUN
//  MON
//  TUE
//  WED
//  THU
//  FRI
//  SAT
//..
// on standard output.  Similarly, the following use of the *reverse* iterator:
//..
    for (bdec_DayOfWeekSet::reverse_iterator it  = weekDays.rbegin();
                                             it != weekDays.rend();
                                           ++it) {
        bsl::cout << *it << bsl::endl;
    }
//..
// produces:
//..
//  FRI
//  THU
//  WED
//  TUE
//  MON
//..

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING SET ARITHMETIC OPERATORS:
        //
        // Concerns:
        //     For these operators, the following properties must hold:
        //       1) The return value is correct.
        //       2) The function is alias safe.
        //       3) The function preserves object invariants.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.
        //   Verify the correctness of the operators using all elements (u, v)
        //   of the cross product S X S.
        //
        // Testing:
        //   bdec_DayOfWeekSet& operator|=(const bdec_DayOfWeekSet& rhs);
        //   bdec_DayOfWeekSet& operator&=(const bdec_DayOfWeekSet& rhs);
        //   bdec_DayOfWeekSet& operator^=(const bdec_DayOfWeekSet& rhs);
        //   bdec_DayOfWeekSet& operator-=(const bdec_DayOfWeekSet& rhs);
        //   bdec_DayOfWeekSet  operator~(const bdec_DayOfWeekSet& set);
        //   operator|(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        //   operator&(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        //   operator^(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        //   operator-(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing set arithmetic operators."
                          << endl
                          << "================================="
                          << endl;

        if (verbose) cout <<
            "\nTesting set arithmetic operators." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_specA;    // source string for A
                const char *d_specB;    // source string for B
                const char *d_specU;    // expected spec for A | B
                const char *d_specI;    // expected spec for A & B
                const char *d_specE;    // expected spec for A ^ B
                const char *d_specS;    // expected spec for A - B
                const char *d_specC;    // expected spec for ~A
            } DATA[] = {
            //line   A      B      A|B       A&B    A^B     A-B        ~A
            //---- -----  -----  --------  ------  -----    ---         --
            { L_,  "",    "",    "",       "",     "",       "",    "ABCDEFG"},
            { L_,  "",    "A",   "A",      "",     "A",      "",    "ABCDEFG"},
            { L_,  "",    "B",   "B",      "",     "B",      "",    "ABCDEFG"},
            { L_,  "",    "AB",  "AB",     "",     "AB",     "",    "ABCDEFG"},
            { L_,  "",    "BC",  "BC",     "",     "BC",     "",    "ABCDEFG"},
            { L_,  "",    "CD",  "CD",     "",     "CD",     "",    "ABCDEFG"},
            { L_,  "",    "ABC", "ABC",    "",     "ABC",    "",    "ABCDEFG"},
            { L_,  "",    "BCD", "BCD",    "",     "BCD",    "",    "ABCDEFG"},
            { L_,  "",    "CDE", "CDE",    "",     "CDE",    "",    "ABCDEFG"},
            { L_,  "",    "DEF", "DEF",    "",     "DEF",    "",    "ABCDEFG"},

            { L_,  "A",   "",    "A",      "",     "A",      "A",   "BCDEFG" },
            { L_,  "A",   "A",   "A",      "A",    "",       "",    "BCDEFG" },
            { L_,  "A",   "B",   "AB",     "",     "AB",     "A",   "BCDEFG" },
            { L_,  "A",   "AB",  "AB",     "A",    "B",      "",    "BCDEFG" },
            { L_,  "A",   "BC",  "ABC",    "",     "ABC",    "A",   "BCDEFG" },
            { L_,  "A",   "CD",  "ACD",    "",     "ACD",    "A",   "BCDEFG" },
            { L_,  "A",   "ABC", "ABC",    "A",    "BC",     "",    "BCDEFG" },
            { L_,  "A",   "BCD", "ABCD",   "",     "ABCD",   "A",   "BCDEFG" },
            { L_,  "A",   "CDE", "ACDE",   "",     "ACDE",   "A",   "BCDEFG" },
            { L_,  "A",   "DEF", "ADEF",   "",     "ADEF",   "A",   "BCDEFG" },

            { L_,  "B",   "",    "B",      "",     "B",      "B",   "ACDEFG" },
            { L_,  "B",   "A",   "AB",     "",     "AB",     "B",   "ACDEFG" },
            { L_,  "B",   "B",   "B",      "B",    "",       "",    "ACDEFG" },
            { L_,  "B",   "AB",  "AB",     "B",    "A",      "",    "ACDEFG" },
            { L_,  "B",   "BC",  "BC",     "B",    "C",      "",    "ACDEFG" },
            { L_,  "B",   "CD",  "BCD",    "",     "BCD",    "B",   "ACDEFG" },
            { L_,  "B",   "ABC", "ABC",    "B",    "AC",     "",    "ACDEFG" },
            { L_,  "B",   "BCD", "BCD",    "B",    "CD",     "",    "ACDEFG" },
            { L_,  "B",   "CDE", "BCDE",   "",     "BCDE",   "B",   "ACDEFG" },
            { L_,  "B",   "DEF", "BDEF",   "",     "BDEF",   "B",   "ACDEFG" },

            { L_,  "AB",  "",    "AB",     "",     "AB",     "AB",  "CDEFG"  },
            { L_,  "AB",  "A",   "AB",     "A",    "B",      "B",   "CDEFG"  },
            { L_,  "AB",  "B",   "AB",     "B",    "A",      "A",   "CDEFG"  },
            { L_,  "AB",  "AB",  "AB",     "AB",   "",       "",    "CDEFG"  },
            { L_,  "AB",  "BC",  "ABC",    "B",    "AC",     "A",   "CDEFG"  },
            { L_,  "AB",  "CD",  "ABCD",   "",     "ABCD",   "AB",  "CDEFG"  },
            { L_,  "AB",  "ABC", "ABC",    "AB",   "C",      "",    "CDEFG"  },
            { L_,  "AB",  "BCD", "ABCD",   "B",    "ACD",    "A",   "CDEFG"  },
            { L_,  "AB",  "CDE", "ABCDE",  "",     "ABCDE",  "AB",  "CDEFG"  },
            { L_,  "AB",  "DEF", "ABDEF",  "",     "ABDEF",  "AB",  "CDEFG"  },

            { L_,  "BC",  "",    "BC",     "",     "BC",     "BC",  "ADEFG"  },
            { L_,  "BC",  "A",   "ABC",    "",     "ABC",    "BC",  "ADEFG"  },
            { L_,  "BC",  "B",   "BC",     "B",    "C",      "C",   "ADEFG"  },
            { L_,  "BC",  "AB",  "ABC",    "B",    "AC",     "C",   "ADEFG"  },
            { L_,  "BC",  "BC",  "BC",     "BC",   "",       "",    "ADEFG"  },
            { L_,  "BC",  "CD",  "BCD",    "C",    "BD",     "B",   "ADEFG"  },
            { L_,  "BC",  "ABC", "ABC",    "BC",   "A",      "",    "ADEFG"  },
            { L_,  "BC",  "BCD", "BCD",    "BC",   "D",      "",    "ADEFG"  },
            { L_,  "BC",  "CDE", "BCDE",   "C",    "BDE",    "B",   "ADEFG"  },
            { L_,  "BC",  "DEF", "BCDEF",  "",     "BCDEF",  "BC",  "ADEFG"  },

            { L_,  "CD",  "",    "CD",     "",     "CD",     "CD",  "ABEFG"  },
            { L_,  "CD",  "A",   "ACD",    "",     "ACD",    "CD",  "ABEFG"  },
            { L_,  "CD",  "B",   "BCD",    "",     "BCD",    "CD",  "ABEFG"  },
            { L_,  "CD",  "AB",  "ABCD",   "",     "ABCD",   "CD",  "ABEFG"  },
            { L_,  "CD",  "BC",  "BCD",    "C",    "BD",     "D",   "ABEFG"  },
            { L_,  "CD",  "CD",  "CD",     "CD",   "",       "",    "ABEFG"  },
            { L_,  "CD",  "ABC", "ABCD",   "C",    "ABD",    "D",   "ABEFG"  },
            { L_,  "CD",  "BCD", "BCD",    "CD",   "B",      "",    "ABEFG"  },
            { L_,  "CD",  "CDE", "CDE",    "CD",   "E",      "",    "ABEFG"  },
            { L_,  "CD",  "DEF", "CDEF",   "D",    "CEF",    "C",   "ABEFG"  },

            { L_,  "ABC", "",    "ABC",    "",     "ABC",    "ABC", "DEFG"   },
            { L_,  "ABC", "A",   "ABC",    "A",    "BC",     "BC",  "DEFG"   },
            { L_,  "ABC", "B",   "ABC",    "B",    "AC",     "AC",  "DEFG"   },
            { L_,  "ABC", "AB",  "ABC",    "AB",   "C",      "C",   "DEFG"   },
            { L_,  "ABC", "BC",  "ABC",    "BC",   "A",      "A",   "DEFG"   },
            { L_,  "ABC", "CD",  "ABCD",   "C",    "ABD",    "AB",  "DEFG"   },
            { L_,  "ABC", "ABC", "ABC",    "ABC",  "",       "",    "DEFG"   },
            { L_,  "ABC", "BCD", "ABCD",   "BC",   "AD",     "A",   "DEFG"   },
            { L_,  "ABC", "CDE", "ABCDE",  "C",    "ABDE",   "AB",  "DEFG"   },
            { L_,  "ABC", "DEF", "ABCDEF", "",     "ABCDEF", "ABC", "DEFG"   },

            { L_,  "BCD", "",    "BCD",    "",     "BCD",    "BCD", "AEFG"   },
            { L_,  "BCD", "A",   "ABCD",   "",     "ABCD",   "BCD", "AEFG"   },
            { L_,  "BCD", "B",   "BCD",    "B",    "CD",     "CD",  "AEFG"   },
            { L_,  "BCD", "AB",  "ABCD",   "B",    "ACD",    "CD",  "AEFG"   },
            { L_,  "BCD", "BC",  "BCD",    "BC",   "D",      "D",   "AEFG"   },
            { L_,  "BCD", "CD",  "BCD",    "CD",   "B",      "B",   "AEFG"   },
            { L_,  "BCD", "ABC", "ABCD",   "BC",   "AD",     "D",   "AEFG"   },
            { L_,  "BCD", "BCD", "BCD",    "BCD",  "",       "",    "AEFG"   },
            { L_,  "BCD", "CDE", "BCDE",   "CD",   "BE",     "B",   "AEFG"   },
            { L_,  "BCD", "DEF", "BCDEF",  "D",    "BCEF",   "BC",  "AEFG"   },

            { L_,  "CDE", "",    "CDE",    "",     "CDE",    "CDE", "ABFG"   },
            { L_,  "CDE", "A",   "ACDE",   "",     "ACDE",   "CDE", "ABFG"   },
            { L_,  "CDE", "B",   "BCDE",   "",     "BCDE",   "CDE", "ABFG"   },
            { L_,  "CDE", "AB",  "ABCDE",  "",     "ABCDE",  "CDE", "ABFG"   },
            { L_,  "CDE", "BC",  "BCDE",   "C",    "BDE",    "DE",  "ABFG"   },
            { L_,  "CDE", "CD",  "CDE",    "CD",   "E",      "E",   "ABFG"   },
            { L_,  "CDE", "ABC", "ABCDE",  "C",    "ABDE",   "DE",  "ABFG"   },
            { L_,  "CDE", "BCD", "BCDE",   "CD",   "BE",     "E",   "ABFG"   },
            { L_,  "CDE", "CDE", "CDE",    "CDE",  "",       "",    "ABFG"   },
            { L_,  "CDE", "DEF", "CDEF",   "DE",   "CF",     "C",   "ABFG"   },

            { L_,  "DEF", "",    "DEF",    "",     "DEF",    "DEF", "ABCG"   },
            { L_,  "DEF", "A",   "ADEF",   "",     "ADEF",   "DEF", "ABCG"   },
            { L_,  "DEF", "B",   "BDEF",   "",     "BDEF",   "DEF", "ABCG"   },
            { L_,  "DEF", "AB",  "ABDEF",  "",     "ABDEF",  "DEF", "ABCG"   },
            { L_,  "DEF", "BC",  "BCDEF",  "",     "BCDEF",  "DEF", "ABCG"   },
            { L_,  "DEF", "CD",  "CDEF",   "D",    "CEF",    "EF",  "ABCG"   },
            { L_,  "DEF", "ABC", "ABCDEF", "",     "ABCDEF", "DEF", "ABCG"   },
            { L_,  "DEF", "BCD", "BCDEF",  "D",    "BCEF",   "EF",  "ABCG"   },
            { L_,  "DEF", "CDE", "CDEF",   "DE",   "CF",     "F",   "ABCG"   },
            { L_,  "DEF", "DEF", "DEF",    "DEF",  "",       "",    "ABCG"   }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC_A = DATA[ti].d_specA;
                const char *SPEC_B = DATA[ti].d_specB;
                const char *SPEC_U = DATA[ti].d_specU;
                const char *SPEC_I = DATA[ti].d_specI;
                const char *SPEC_E = DATA[ti].d_specE;
                const char *SPEC_S = DATA[ti].d_specS;
                const char *SPEC_C = DATA[ti].d_specC;

                const int curLen = strlen(SPEC_A);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(ti, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                const Obj U = g(SPEC_U);
                const Obj I = g(SPEC_I);
                const Obj E = g(SPEC_E);
                const Obj S = g(SPEC_S);
                const Obj C = g(SPEC_C);

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC_A);
                const Obj XX(X);

                Obj mY;  const Obj& Y = mY;
                gg(&mY, SPEC_B);
                const Obj YY(Y);

                // testing 'operator|' and 'operator|='
                {
                    Obj mZ;  const Obj& Z = mZ;

                    mZ = X | Y;
                    LOOP_ASSERT(LINE, U == Z);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    mR |= Y;
                    LOOP_ASSERT(LINE, U == R);

                    LOOP_ASSERT(LINE, XX == X);
                    LOOP_ASSERT(LINE, YY == Y);

                    if (X == Y) {  // alias test
                        Obj mA;  const Obj& A = mA;
                        mA = X;

                        mA |= mA;
                        LOOP_ASSERT(LINE, U == A);
                    }
                }

                // testing 'operator&' and 'operator&='
                {
                    Obj mZ;  const Obj& Z = mZ;

                    mZ = X & Y;
                    LOOP_ASSERT(LINE, I == Z);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    mR &= Y;
                    LOOP_ASSERT(LINE, I == R);

                    LOOP_ASSERT(LINE, XX == X);
                    LOOP_ASSERT(LINE, YY == Y);

                    if (X == Y) {  // alias test
                        Obj mA;  const Obj& A = mA;
                        mA = X;

                        mA &= mA;
                        LOOP_ASSERT(LINE, I == A);
                    }
                }

                // testing 'operator^' and 'operator^='
                {
                    Obj mZ;  const Obj& Z = mZ;

                    mZ = X ^ Y;
                    LOOP_ASSERT(LINE, E == Z);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    mR ^= Y;
                    LOOP_ASSERT(LINE, E == R);

                    LOOP_ASSERT(LINE, XX == X);
                    LOOP_ASSERT(LINE, YY == Y);

                    if (X == Y) {  // alias test
                        Obj mA;  const Obj& A = mA;
                        mA = X;

                        mA ^= mA;
                        LOOP_ASSERT(LINE, E == A);
                    }
                }

                // testing 'operator-' and 'operator-='
                {
                    Obj mZ;  const Obj& Z = mZ;

                    mZ = X - Y;
                    LOOP_ASSERT(LINE, S == Z);

                    Obj mR;  const Obj& R = mR;
                    mR = X;

                    mR -= Y;
                    LOOP_ASSERT(LINE, S == R);

                    LOOP_ASSERT(LINE, XX == X);
                    LOOP_ASSERT(LINE, YY == Y);

                    if (X == Y) {  // alias test
                        Obj mA;  const Obj& A = mA;
                        mA = X;

                        mA -= mA;
                        LOOP_ASSERT(LINE, S == A);
                    }
                }

                // testing 'operator~'
                {
                    Obj mZ;  const Obj& Z = mZ;

                    mZ = ~X;
                    LOOP_ASSERT(LINE, C  == Z);
                    LOOP_ASSERT(LINE, XX == X);

                    Obj mA;  const Obj& A = mA;
                    mA = X;

                    mA = ~mA; // alias test
                    LOOP_ASSERT(LINE, C == A);
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'areMembers':
        //
        // Concerns:
        //   For this function, the following properties must hold:
        //     1) The return value is 0 or 1.
        //     2) The function does not modify the source objects.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   that the function returns the expected value.  Create a control
        //   object that checks for case 2.
        //
        // Testing:
        //   bool areMembers(const bdec_DayOfWeekSet& set) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing areMembers" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting 'areMembers'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_specA;    // source string for A
                const char *d_specB;    // source string for B
                int         d_LE;       // expected rv for B.areMembers(A)
                int         d_GE;       // expected rv for A.areMembers(B)
            } DATA[] = {
                //line   A       B     LE  GE
                //---- ------  ------  --  --
                { L_,  "",     "",      1,  1 },
                { L_,  "",     "A",     1,  0 },
                { L_,  "",     "B",     1,  0 },
                { L_,  "",     "AB",    1,  0 },
                { L_,  "",     "BC",    1,  0 },
                { L_,  "",     "CD",    1,  0 },
                { L_,  "",     "ABC",   1,  0 },
                { L_,  "",     "BCD",   1,  0 },
                { L_,  "",     "CDE",   1,  0 },
                { L_,  "",     "DEF",   1,  0 },
                { L_,  "",     "ABCD",  1,  0 },
                { L_,  "",     "BCDE",  1,  0 },
                { L_,  "",     "CDEF",  1,  0 },
                { L_,  "",     "DEFG",  1,  0 },

                { L_,  "A",    "A",     1,  1 },
                { L_,  "A",    "B",     0,  0 },
                { L_,  "A",    "AB",    1,  0 },
                { L_,  "A",    "BC",    0,  0 },
                { L_,  "A",    "CD",    0,  0 },
                { L_,  "A",    "ABC",   1,  0 },
                { L_,  "A",    "BCD",   0,  0 },
                { L_,  "A",    "CDE",   0,  0 },
                { L_,  "A",    "DEF",   0,  0 },
                { L_,  "A",    "ABCD",  1,  0 },
                { L_,  "A",    "BCDE",  0,  0 },
                { L_,  "A",    "CDEF",  0,  0 },
                { L_,  "A",    "DEFG",  0,  0 },

                { L_,  "B",    "B",     1,  1 },
                { L_,  "B",    "AB",    1,  0 },
                { L_,  "B",    "BC",    1,  0 },
                { L_,  "B",    "CD",    0,  0 },
                { L_,  "B",    "ABC",   1,  0 },
                { L_,  "B",    "BCD",   1,  0 },
                { L_,  "B",    "CDE",   0,  0 },
                { L_,  "B",    "DEF",   0,  0 },
                { L_,  "B",    "ABCD",  1,  0 },
                { L_,  "B",    "BCDE",  1,  0 },
                { L_,  "B",    "CDEF",  0,  0 },
                { L_,  "B",    "DEFG",  0,  0 },

                { L_,  "AB",   "AB",    1,  1 },
                { L_,  "AB",   "BC",    0,  0 },
                { L_,  "AB",   "CD",    0,  0 },
                { L_,  "AB",   "ABC",   1,  0 },
                { L_,  "AB",   "BCD",   0,  0 },
                { L_,  "AB",   "CDE",   0,  0 },
                { L_,  "AB",   "DEF",   0,  0 },
                { L_,  "AB",   "ABCD",  1,  0 },
                { L_,  "AB",   "BCDE",  0,  0 },
                { L_,  "AB",   "CDEF",  0,  0 },
                { L_,  "AB",   "DEFG",  0,  0 },

                { L_,  "BC",   "BC",    1,  1 },
                { L_,  "BC",   "CD",    0,  0 },
                { L_,  "BC",   "ABC",   1,  0 },
                { L_,  "BC",   "BCD",   1,  0 },
                { L_,  "BC",   "CDE",   0,  0 },
                { L_,  "BC",   "DEF",   0,  0 },
                { L_,  "BC",   "ABCD",  1,  0 },
                { L_,  "BC",   "BCDE",  1,  0 },
                { L_,  "BC",   "CDEF",  0,  0 },
                { L_,  "BC",   "DEFG",  0,  0 },

                { L_,  "CD",   "CD",    1,  1 },
                { L_,  "CD",   "ABC",   0,  0 },
                { L_,  "CD",   "BCD",   1,  0 },
                { L_,  "CD",   "CDE",   1,  0 },
                { L_,  "CD",   "DEF",   0,  0 },
                { L_,  "CD",   "ABCD",  1,  0 },
                { L_,  "CD",   "BCDE",  1,  0 },
                { L_,  "CD",   "CDEF",  1,  0 },
                { L_,  "CD",   "DEFG",  0,  0 },

                { L_,  "ABC",  "ABC",   1,  1 },
                { L_,  "ABC",  "BCD",   0,  0 },
                { L_,  "ABC",  "CDE",   0,  0 },
                { L_,  "ABC",  "DEF",   0,  0 },
                { L_,  "ABC",  "ABCD",  1,  0 },
                { L_,  "ABC",  "BCDE",  0,  0 },
                { L_,  "ABC",  "CDEF",  0,  0 },
                { L_,  "ABC",  "DEFG",  0,  0 },

                { L_,  "BCD",  "BCD",   1,  1 },
                { L_,  "BCD",  "CDE",   0,  0 },
                { L_,  "BCD",  "DEF",   0,  0 },
                { L_,  "BCD",  "ABCD",  1,  0 },
                { L_,  "BCD",  "BCDE",  1,  0 },
                { L_,  "BCD",  "CDEF",  0,  0 },
                { L_,  "BCD",  "DEFG",  0,  0 },

                { L_,  "CDE",  "CDE",   1,  1 },
                { L_,  "CDE",  "DEF",   0,  0 },
                { L_,  "CDE",  "ABCD",  0,  0 },
                { L_,  "CDE",  "BCDE",  1,  0 },
                { L_,  "CDE",  "CDEF",  1,  0 },
                { L_,  "CDE",  "DEFG",  0,  0 },

                { L_,  "DEF",  "DEF",   1,  1 },
                { L_,  "DEF",  "ABCD",  0,  0 },
                { L_,  "DEF",  "BCDE",  0,  0 },
                { L_,  "DEF",  "CDEF",  1,  0 },
                { L_,  "DEF",  "DEFG",  1,  0 },

                { L_,  "ABCD", "ABCD",  1,  1 },
                { L_,  "ABCD", "BCDE",  0,  0 },
                { L_,  "ABCD", "CDEF",  0,  0 },
                { L_,  "ABCD", "DEFG",  0,  0 },

                { L_,  "BCDE", "BCDE",  1,  1 },
                { L_,  "BCDE", "CDEF",  0,  0 },
                { L_,  "BCDE", "DEFG",  0,  0 },

                { L_,  "CDEF", "CDEF",  1,  1 },
                { L_,  "CDEF", "DEFG",  0,  0 },

                { L_,  "DEFG", "DEFG",  1,  1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC_A = DATA[ti].d_specA;
                const char *SPEC_B = DATA[ti].d_specB;
                const int   LE     = DATA[ti].d_LE;
                const int   GE     = DATA[ti].d_GE;

                const int curLen = strlen(SPEC_A);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC_A);
                const Obj XX(X);

                Obj mY;  const Obj& Y = mY;
                gg(&mY, SPEC_B);
                const Obj YY(Y);

                LOOP_ASSERT(LINE, LE == Y.areMembers(X));
                LOOP_ASSERT(LINE, GE == X.areMembers(Y));
                LOOP_ASSERT(LINE, 1  == X.areMembers(X));
                LOOP_ASSERT(LINE, 1  == Y.areMembers(Y));
                LOOP_ASSERT(LINE, XX == X);
                LOOP_ASSERT(LINE, YY == Y);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADD AND REMOVE
        //
        // Concerns:
        //     That the 'add' and 'remove' methods work correctly.
        //
        // Plan:
        //   Use the enumeration technique to create a set S of initial values.
        //   Perturb the internal representation of the set S by various
        //   amounts to obtain the set T of initial states.  For each element
        //   T, perform a handful of 'add' and 'remove' operations of elements
        //   from the universe of elements in VALUES to validate the return
        //   values and ensure the methods are alias safe.  Finally, 'remove'
        //   all members from the universe while verifying the return value.
        //   The method 'isMember' is used as an oracle for computing the
        //   return values of 'add' and 'remove'.
        //
        // Testing:
        //   void add(DayOfWeek value);
        //   int remove(DayOfWeek value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'add' and 'remove'" << endl
                << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting x.add(value) and x.remove(value)" << endl;
        {
            static const char *SPECS[] = { // len: 0 - 7
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "DEFGAB", "ABCDEFG",
            0}; // Null string required as last element.

            int oldLen = -1;
            int k;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of initial length "
                                  << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                Obj mX;  const Obj& X = mX;
                gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                // test adding and removing
                for (k = 0; k < NUM_VALUES; ++k) {
                    int rv;
                    int initialLength = X.length();

                    int erv = !X.isMember(VALUES[k]);
                    mX.add(VALUES[k]);
                    mX.add(VALUES[k]);

                    rv = mX.remove(VALUES[k]);
                    LOOP2_ASSERT(ti, k, 1 == rv);

                    rv = mX.remove(VALUES[k]);
                    LOOP2_ASSERT(ti, k, 0 == rv);

                    mX.add(VALUES[k]);
                    mX.add(VALUES[k]);

                    LOOP2_ASSERT(ti, k, initialLength + erv == X.length());
                }

                // test removing everything
                for (k = 0; k < NUM_VALUES; ++k) {
                    int initialLength = X.length();
                    int erv = X.isMember(VALUES[k]);
                    int rv  = mX.remove(VALUES[k]);
                    LOOP2_ASSERT(ti, k, erv == rv);
                    LOOP2_ASSERT(ti, k, initialLength - erv == X.length());
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //
        // Concerns:
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

                { L_,   "",       -2,         2,        "["           NL
                                                        "    ]"       NL  },

                { L_,   "",        1,        -2,        "  [ ]"           },
                { L_,   "",        2,        -2,        "    [ ]"         },
                { L_,   "",       -1,        -3,        "[ ]"             },
                { L_,   "",       -2,        -3,        "[ ]"             },

                { L_,  "A",        0,         0,        "["           NL
                                                        "SUN"         NL
                                                        "]"           NL  },

                { L_,  "A",        0,         2,        "["           NL
                                                        "  SUN"       NL
                                                        "]"           NL  },

                { L_,  "A",        1,         1,        " ["          NL
                                                        "  SUN"       NL
                                                        " ]"          NL  },

                { L_,  "A",        1,         2,        "  ["         NL
                                                        "    SUN"     NL
                                                        "  ]"         NL  },

                { L_,  "A",       -1,         2,        "["           NL
                                                        "    SUN"     NL
                                                        "  ]"         NL  },

                { L_,  "A",       -2,         2,        "["           NL
                                                        "      SUN"   NL
                                                        "    ]"       NL  },

                { L_,  "A",        1,        -2,        "  [ SUN ]"       },
                { L_,  "A",        2,        -2,        "    [ SUN ]"     },
                { L_,  "A",       -1,        -3,        "[ SUN ]"         },
                { L_,  "A",       -2,        -3,        "[ SUN ]"         },

                { L_, "BC",        0,         0,        "["           NL
                                                        "TUE THU"     NL
                                                        "]"           NL  },

                { L_, "BC",        0,         2,        "["           NL
                                                        "  TUE THU"   NL
                                                        "]"           NL  },

                { L_, "BC",        1,         1,        " ["          NL
                                                        "  TUE THU"   NL
                                                        " ]"          NL  },

                { L_, "BC",        1,         2,        "  ["         NL
                                                        "    TUE THU" NL
                                                        "  ]"         NL  },

                { L_, "BC",       -1,         2,        "["           NL
                                                        "    TUE THU" NL
                                                        "  ]"         NL  },

                { L_, "BC",       -2,         2,        "["             NL
                                                        "      TUE THU" NL
                                                        "    ]"         NL  },

                { L_, "BC",        1,        -2,        "  [ TUE THU ]"   },
                { L_, "BC",        2,        -2,        "    [ TUE THU ]" },
                { L_, "BC",       -1,        -3,        "[ TUE THU ]"     },
                { L_, "BC",       -2,        -3,        "[ TUE THU ]"     },

                { L_,   "ABCDE",   1,         3,        "   ["        NL
                                                        "      SUN"
                                                             " MON"
                                                             " TUE"
                                                             " THU"
                                                             " FRI"   NL
                                                        "   ]"        NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

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

                Obj mX;  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(LINE, curLen == X.length()); // same lengths

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
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, arePrintedValuesEquivalent(buf1, FMT));
                LOOP_ASSERT(LINE, arePrintedValuesEquivalent(buf2, FMT));

                LOOP2_ASSERT(LINE, SZ, 0 == strncmp(buf1, FMT, SZ - 1));
                LOOP2_ASSERT(LINE, SZ, 0 == strncmp(buf2, FMT, SZ - 1));

                LOOP_ASSERT(LINE,
                                 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LINE,
                                 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   1) 'maxSupportedBdexVersion' returns the correct version.
        //   2) Ensure that streaming works under the following conditions:
        //       VALID      - may contain any sequence of valid values.
        //       EMPTY      - valid, but contains no data.
        //       INVALID    - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED  - the data contains explicitly inconsistent fields.
        //
        //   Plan:
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct (breathing) test of the 'bdexStreamOut'
        //     and 'bdexStreamIn' methods.
        //
        //     Next, specify a set S of unique object values with substantial
        //     and varied differences, ordered by increasing length.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in S X S, stream-out the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in S, attempt to stream into (a temporary copy of) x
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
        //   int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(g("ABC"));
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bdex_TestOutStream out;  X.bdexStreamOut(out, 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());
            in.setSuppressVersionCheck(1);

            Obj t(g("DE"));

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            t.bdexStreamIn(in, 1);          ASSERT(in);  ASSERT(in.isEmpty());

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

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                Obj mU; const Obj& U = mU; gg(&mU, U_SPEC);

                bdex_TestOutStream out;

                // testing stream-out operator here
                bdex_OutStreamFunctions::streamOut(out, U, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream(OD, LOD);
                testInStream.setSuppressVersionCheck(1);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int         vLen   = strlen(V_SPEC);

                    const Obj VV = g(V_SPEC);               // control

                    if (veryVerbose) {
                        cout << "\t\t\tFor destination objects of length "
                                                    << vLen << ":\t";
                        P(V_SPEC);
                    }

                    const int Z = ui == vi; // flag indicating same values

                    BEGIN_BDEX_EXCEPTION_TEST { testInStream.reset();
                        LOOP2_ASSERT(U_SPEC, V_SPEC, testInStream);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, !testInStream.isEmpty());
                        Obj mV;
                        const Obj& V = mV;      gg(&mV, V_SPEC);

                        if (veryVerbose) { cout << "\t |"; P_(U); P(V); }

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, Z==(U==V));

                        // test stream-in operator here
                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mV,
                                                         VERSION);

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  U == V);

                    } END_BDEX_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 7
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "GFEDCB", "ABCDEFG",
            0}; // Null string required as last element.

            bdex_TestInStream testInStream("", 0);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX; gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

              BEGIN_BDEX_EXCEPTION_TEST { testInStream.reset();

                Obj t; gg(&t, SPEC);

              // Ensure that reading from an empty or invalid input stream
              // leaves the stream invalid and the target object unchanged.

                                    LOOP_ASSERT(ti, testInStream);
                                    LOOP_ASSERT(ti, X == t);

                bdex_InStreamFunctions::streamIn(testInStream, t, VERSION);
                LOOP_ASSERT(ti, !testInStream);
                LOOP_ASSERT(ti, X == t);

                bdex_InStreamFunctions::streamIn(testInStream, t, VERSION);
                LOOP_ASSERT(ti, !testInStream);
                LOOP_ASSERT(ti, X == t);

              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj X1 = g("CDCD"),   Y1 = g("BB"),    Z1 = g("ABCDE");
            const Obj X2 = g("ADE"),    Y2 = g("CABDE"), Z2 = g("B");
            const Obj X3 = g("DEEDDE"), Y3 = g("C"),     Z3 = g("DBED");

            bdex_TestOutStream out;
            bdex_OutStreamFunctions::streamOut(out, Y1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, Y2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, Y3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream testInStream(OD, i);
                bdex_TestInStream& in = testInStream;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                Obj t1(X1),
                    t2(X2),
                    t3(X3);

                if (i < LOD1) {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i)  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else if (i < LOD2) {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                     if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                }
                else {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                     if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
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

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            bdex_TestOutStream out;

            Obj z(Y);
            bdex_OutStreamFunctions::streamOut(out, z, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);
            ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION); ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            bdex_TestOutStream out;

            Obj z(Y);
            out.putUint8(*reinterpret_cast<unsigned char *>(&z));

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            bdex_InStreamFunctions::streamIn(in, t, -1);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char version = 5; // too large

            bdex_TestOutStream out;

            Obj z(Y);
            out.putUint8(*reinterpret_cast<unsigned char *>(&z));

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //       other instance.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //       even when the lhs and rhs are identically the same object.
        //
        // Plan:
        //
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  To address
        //   concerns 1 - 3, construct tests u = v for all (u, v) in S X S.
        //   Using canonical controls UU and VV, assert before the assignment
        //   that UU == u, VV == v, and v == u if and only if and only if
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.
        //
        //   As a separate exercise, we address 4 by constructing tests
        //   y = y for all y in S.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment.
        //
        // Testing:
        //   bdec_DayOfWeekSet& operator=(const bdec_DayOfWeekSet& rhs);
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
                "ABCDEFG",
            0}; // Null string required as last element.

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = strlen(U_SPEC);

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
                    const int         vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    Obj mU;
                    const Obj& U = mU; gg(&mU, U_SPEC);

                    {

                        Obj mV;
                        const Obj& V = mV; gg(&mV, V_SPEC);

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, Z==(V==U));

                        mU = V; // test assignment here

                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                    }

                    // 'mV' (and therefore 'V') now out of scope
                    LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 7
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "DEFGAB", "ABCDEFG",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                Obj mY; const Obj& Y = mY; gg(&mY, SPEC);

                if (veryVerbose) { cout << "\t\t"; P(Y); }

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);

                mY = Y; // test assignment here

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, and that
        //   'g' returns an object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Finally, use 'sizeof' to confirm that the
        //   (temporary) returned by 'g' differs in size from that returned by
        //   'gg'.
        //
        // Testing:
        //   bdec_DayOfWeekSet g(const char *spec);
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
            Obj mX;  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }

            LOOP_ASSERT(ti, X == g(spec));
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x;                      // runtime tests
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
        //
        // Concerns:
        //   We have the following concerns:
        //   1) The new object's value is the same as that of the original
        //      object (relying on the previously tested equality operators).
        //   2) The value of the original object is left unaffected.
        //   3) Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //
        // Plan:
        //   To address concerns 1 and 2, specify a set S of object values with
        //   substantial and varied differences, ordered by increasing length.
        //   For each value in S, initialize objects w and x, copy construct y
        //   from x and use 'operator==' to verify that both x and y
        //   subsequently have the same value as w.  Let x go out of scope and
        //   again verify that w == y.
        //
        // Testing:
        //   bdec_DayOfWeekSet(const bdec_DayOfWeekSet& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEFG",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC   = SPECS[ti];
                const int         curLen = strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW; gg(&mW, SPEC); const Obj& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                Obj *mX = new Obj; gg(mX, SPEC); const Obj& X = *mX;
                LOOP_ASSERT(ti, X == W);

                Obj mY(X);  const Obj &Y = mY;
                LOOP_ASSERT(ti, X == W);  LOOP_ASSERT(ti, Y == W);

                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }

                delete mX;
                LOOP_ASSERT(ti, Y == W);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
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
        // Testing:
        //   operator==(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        //   operator!=(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const struct {
              int         d_lineNum;  // source line number
              const char *d_spec;     // string
            } DATA[] = {
                //line spec
                //---- ---------------------
                { L_,  ""                 },
                { L_,  "A"                },
                { L_,  "B"                },
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
                { L_,  "EFGA"             },
                { L_,  "ABCDE"            },
                { L_,  "EFGABC"           },
                { L_,  "CDABEFG"          },
                { L_,  "ABCDEFG"          },
                { L_,  "GFEDCBA"          }
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

                Obj mX;  const Obj& X = mX;
                gg(&mX, DATA[i].d_spec);

                if (veryVerbose) { P_(i); P_(DATA[i].d_spec); P(X); }

                for (int j = 0; j < NUM_DATA; ++j) {
                    Obj mY;  const Obj& Y = mY;
                    gg(&mY, DATA[j].d_spec);

                    if (veryVerbose) {
                        cout << "  ";
                        P_(j);
                        P_(DATA[j].d_spec);
                        P(Y);
                    }

                    int r = 1;

                    for (int k = 0; k < NUM_VALUES; ++k) {
                        if (X.isMember(VALUES[k]) != Y.isMember(VALUES[k])) {
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
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
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
        //   'arePrintedValuesEquivalent' is used to validate equality of the
        //   output to the expected output.
        //
        // Testing:
        //   operator<<(ostream&, const bdec_DayOfWeekSet&);
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
                //line  spec            output format
                //----  --------------  ---------------------------------
                { L_,   "",             "[ ]"                           },
                { L_,   "A",            "[ SUN ]"                       },
                { L_,   "F",            "[ SAT ]"                       },
                { L_,   "BC",           "[ TUE THU ]"                   },
                { L_,   "AF",           "[ SUN SAT ]"                   },
                { L_,   "FA",           "[ SAT SUN ]"                   },
                { L_,   "ABCDE",        "[ SUN TUE THU MON FRI ]"       },
                { L_,   "ABCDEF",       "[ SUN TUE THU MON FRI SAT ]"   }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SIZE     = 1000; // Must be big enough to hold output
                                       // string.
            const char Z1 = 'a';  // Value 1 used to represent an unset char.
            const char Z2 = 'b';  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX;  const Obj& X = gg(&mX, SPEC);
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

                const int SZ   = strlen(FMT) + 1;
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
        // TESTING BASIC ACCESSORS
        //   This will test the basic accessors and also the
        //   bdec_DayOfWeekSet_Iter class.
        //
        // Concerns:
        //   1) The iterator copy constructor, initializing constructor, and
        //      assignment operators set the internal state of the iterator
        //      correctly.
        //   2) The set accessors return expected values.
        //   3) The increment and decrement operators on the iterators work as
        //      expected.
        //
        // Plan:
        //   1) Test iterator constructors and 'operator='.
        //      For each line in a set of test data, initialize a 'data'
        //      variable containing the the elements from the line.  Create the
        //      following 3 iterators:
        //          o X (default ctor)
        //          o Y (init ctor)
        //          o Z (copy of Y using copy ctor)
        //      Loop through the sorted array of elements, checking the value
        //      of '*Y' and '*Z' at each iteration.  At the mid-point, assign
        //      'Y' to 'X'.
        //      Loop through the sorted array of elements starting at the
        //      mid-point, checking the value of '*X' at each iteration.
        //   2) Test set accessors.
        //      For each line in a set of test data, initialize a set using the
        //      'gg' generator function.  Check that the length is as expected
        //      and that each element is a member of the set.  Test the forward
        //      and reverse iterators by creating a sorted array of elements
        //      and checking that the iterators contain the same elements as
        //      the sorted array, and in the same order.
        //
        // Testing:
        //   iterator begin() const;
        //   iterator end() const;
        //   bool isMember(bdet_Month::Month value) const;
        //   int length() const;
        //   reverse_iterator rbegin() const;
        //   reverse_iterator rend() const;
        //
        //   bdec_DayOfWeekSet_Iter();
        //   bdec_DayOfWeekSet_Iter(int data, int index);
        //   bdec_DayOfWeekSet_Iter(const bdec_DayOfWeekSet_Iter& original);
        //   ~bdec_DayOfWeekSet_Iter();
        //   bdec_DayOfWeekSet_Iter& operator=(const bdec_DayOfWeekSet_Iter&);
        //   bdec_DayOfWeekSet_Iter& operator++();
        //   bdec_DayOfWeekSet_Iter& operator--();
        //   const bdet_DayOfWeek::Day& operator*() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Basic Accessors"
                          << "\n=======================" << endl;

        const int SZ = 10;
        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec_p;           // specification string
            int         d_length;           // expected length
            Element     d_elements[SZ];     // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { VA }                          },
            { L_,   "A",            1,      { VA }                          },
            { L_,   "B",            1,      { VB }                          },
            { L_,   "AA",           1,      { VA }                          },
            { L_,   "AB",           2,      { VA, VB }                      },
            { L_,   "BC",           2,      { VB, VC }                      },
            { L_,   "AABB",         2,      { VA, VB }                      },
            { L_,   "ABAB",         2,      { VA, VB }                      },
            { L_,   "ABBA",         2,      { VA, VB }                      },
            { L_,   "BAAB",         2,      { VA, VB }                      },
            { L_,   "BABA",         2,      { VA, VB }                      },
            { L_,   "BBAA",         2,      { VA, VB }                      },
            { L_,   "ABABAB",       2,      { VA, VB }                      },
            { L_,   "ABC",          3,      { VA, VB, VC }                  },

            { L_,   "AABBCC",       3,      { VA, VB, VC }                  },
            { L_,   "AABCBC",       3,      { VA, VB, VC }                  },
            { L_,   "AABCCB",       3,      { VA, VB, VC }                  },
            { L_,   "AACBBC",       3,      { VA, VB, VC }                  },
            { L_,   "AACBCB",       3,      { VA, VB, VC }                  },
            { L_,   "AACCBB",       3,      { VA, VB, VC }                  },

            { L_,   "ABABCC",       3,      { VA, VB, VC }                  },
            { L_,   "ABACBC",       3,      { VA, VB, VC }                  },
            { L_,   "ABACCB",       3,      { VA, VB, VC }                  },
            { L_,   "ACABBC",       3,      { VA, VB, VC }                  },
            { L_,   "ACABCB",       3,      { VA, VB, VC }                  },
            { L_,   "ACACBB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBACC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCABC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCACB",       3,      { VA, VB, VC }                  },
            { L_,   "ACBABC",       3,      { VA, VB, VC }                  },
            { L_,   "ACBACB",       3,      { VA, VB, VC }                  },
            { L_,   "ACCABB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBCAC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCBAC",       3,      { VA, VB, VC }                  },
            { L_,   "ABCCAB",       3,      { VA, VB, VC }                  },
            { L_,   "ACBBAC",       3,      { VA, VB, VC }                  },
            { L_,   "ACBCAB",       3,      { VA, VB, VC }                  },
            { L_,   "ACCBAB",       3,      { VA, VB, VC }                  },

            { L_,   "ABBCCA",       3,      { VA, VB, VC }                  },
            { L_,   "ABCBCA",       3,      { VA, VB, VC }                  },
            { L_,   "ABCCBA",       3,      { VA, VB, VC }                  },
            { L_,   "ACBBCA",       3,      { VA, VB, VC }                  },
            { L_,   "ACBCBA",       3,      { VA, VB, VC }                  },
            { L_,   "ACCBBA",       3,      { VA, VB, VC }                  },

            { L_,   "BAABCC",       3,      { VA, VB, VC }                  },
            { L_,   "BAACBC",       3,      { VA, VB, VC }                  },
            { L_,   "BAACCB",       3,      { VA, VB, VC }                  },
            { L_,   "CAABBC",       3,      { VA, VB, VC }                  },
            { L_,   "CAABCB",       3,      { VA, VB, VC }                  },
            { L_,   "CAACBB",       3,      { VA, VB, VC }                  },

            { L_,   "BABACC",       3,      { VA, VB, VC }                  },
            { L_,   "BACABC",       3,      { VA, VB, VC }                  },
            { L_,   "BACACB",       3,      { VA, VB, VC }                  },
            { L_,   "CABABC",       3,      { VA, VB, VC }                  },
            { L_,   "CABACB",       3,      { VA, VB, VC }                  },
            { L_,   "CACABB",       3,      { VA, VB, VC }                  },

            { L_,   "BABCAC",       3,      { VA, VB, VC }                  },
            { L_,   "BACBAC",       3,      { VA, VB, VC }                  },
            { L_,   "BACCAB",       3,      { VA, VB, VC }                  },
            { L_,   "CABBAC",       3,      { VA, VB, VC }                  },
            { L_,   "CABCAB",       3,      { VA, VB, VC }                  },
            { L_,   "CACBAB",       3,      { VA, VB, VC }                  },

            { L_,   "BABCCA",       3,      { VA, VB, VC }                  },
            { L_,   "BACBCA",       3,      { VA, VB, VC }                  },
            { L_,   "BACCBA",       3,      { VA, VB, VC }                  },
            { L_,   "CABBCA",       3,      { VA, VB, VC }                  },
            { L_,   "CABCBA",       3,      { VA, VB, VC }                  },
            { L_,   "CACBBA",       3,      { VA, VB, VC }                  },

            { L_,   "BBAACC",       3,      { VA, VB, VC }                  },
            { L_,   "BCAABC",       3,      { VA, VB, VC }                  },
            { L_,   "BCAACB",       3,      { VA, VB, VC }                  },
            { L_,   "CBAABC",       3,      { VA, VB, VC }                  },
            { L_,   "CBAACB",       3,      { VA, VB, VC }                  },
            { L_,   "CCAABB",       3,      { VA, VB, VC }                  },

            { L_,   "BBACAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCABAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCACAB",       3,      { VA, VB, VC }                  },
            { L_,   "CBABAC",       3,      { VA, VB, VC }                  },
            { L_,   "CBACAB",       3,      { VA, VB, VC }                  },
            { L_,   "CCABAB",       3,      { VA, VB, VC }                  },

            { L_,   "BBACCA",       3,      { VA, VB, VC }                  },
            { L_,   "BCABCA",       3,      { VA, VB, VC }                  },
            { L_,   "BCACBA",       3,      { VA, VB, VC }                  },
            { L_,   "CBABCA",       3,      { VA, VB, VC }                  },
            { L_,   "CBACBA",       3,      { VA, VB, VC }                  },
            { L_,   "CCABBA",       3,      { VA, VB, VC }                  },

            { L_,   "BBCAAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCBAAC",       3,      { VA, VB, VC }                  },
            { L_,   "BCCAAB",       3,      { VA, VB, VC }                  },
            { L_,   "CBBAAC",       3,      { VA, VB, VC }                  },
            { L_,   "CBCAAB",       3,      { VA, VB, VC }                  },
            { L_,   "CCBAAB",       3,      { VA, VB, VC }                  },

            { L_,   "BBCACA",       3,      { VA, VB, VC }                  },
            { L_,   "BCBACA",       3,      { VA, VB, VC }                  },
            { L_,   "BCCABA",       3,      { VA, VB, VC }                  },
            { L_,   "CBBACA",       3,      { VA, VB, VC }                  },
            { L_,   "CBCABA",       3,      { VA, VB, VC }                  },
            { L_,   "CCBABA",       3,      { VA, VB, VC }                  },

            { L_,   "BBCCAA",       3,      { VA, VB, VC }                  },
            { L_,   "BCBCAA",       3,      { VA, VB, VC }                  },
            { L_,   "BCCBAA",       3,      { VA, VB, VC }                  },
            { L_,   "CBBCAA",       3,      { VA, VB, VC }                  },
            { L_,   "CBCBAA",       3,      { VA, VB, VC }                  },
            { L_,   "CCBBAA",       3,      { VA, VB, VC }                  },

            { L_,   "ABCABCABC",    3,      { VA, VB, VC }                  },
            { L_,   "BCA",          3,      { VB, VC, VA }                  },
            { L_,   "CAB",          3,      { VC, VA, VB }                  },
            { L_,   "CDAB",         4,      { VC, VD, VA, VB }              },
            { L_,   "DABC",         4,      { VD, VA, VB, VC }              },
            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }          },
            { L_,   "EDCBA",        5,      { VA, VB, VC, VD, VE }          },
            { L_,   "ABCDEAB",      5,      { VA, VB, VC, VD, VE }          },
            { L_,   "BACDEABC",     5,      { VA, VB, VC, VD, VE }          },
            { L_,   "CBADEABCD",    5,      { VA, VB, VC, VD, VE }          },

            { L_,   "ABFCDE",       6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "EDCFBA",       6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "ABCDFEAB",     6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "BACDEFABC",    6,      { VA, VB, VC, VD, VE, VF }      },
            { L_,   "CBADEAFBCD",   6,      { VA, VB, VC, VD, VE, VF }      },

            { L_,   "AGBFCDE",      7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "EDGCFBA",      7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "ABCDFEGAB",    7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "BACDEFGABC",   7,      { VA, VB, VC, VD, VE, VF, VG }  },
            { L_,   "CBAGDEAFBCD",  7,      { VA, VB, VC, VD, VE, VF, VG }  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        int ti;

        if (verbose) cout << "\nTesting iterator constructors and 'operator='."
                          << endl;
        for (ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const int LENGTH       = DATA[ti].d_length;
            const Element *const e = DATA[ti].d_elements;
            const int MID_POINT    = LENGTH / 2;

            int data = 0;
            int i;

            if (veryVerbose) {
                cout << "\tSet = [ ";
            }

            for (i = 0; i < LENGTH; ++i) {
                const Element element = e[i];

                data |= BIT_MASK[element];

                if (veryVerbose) {
                    cout << element << " ";
                }
            }

            if (veryVerbose) {
                cout << "]" << endl;
            }

            Iterator        mX;
            const Iterator& X = mX;
            Iterator        mY(data, 1);
            const Iterator& Y = mY;
            Iterator        mZ(Y);
            const Iterator& Z = mZ;

            Element       sortedElements[SZ];
            int           length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);
            LOOP2_ASSERT(LINE, length, LENGTH == length);

            for (i = 0; i < LENGTH; ++i, ++mY, ++mZ) {
                const Element element =  sortedElements[i];
                const Element eY      = *Y;
                const Element eZ      = *Z;

                if (i == MID_POINT) {
                    mX = mY;
                }

                LOOP3_ASSERT(LINE, i, eY, element == eY);
                LOOP3_ASSERT(LINE, i, eZ, element == eZ);
            }

            for (i = MID_POINT; i < LENGTH; ++i, ++mX) {
                const Element element =  sortedElements[i];
                const Element eX      = *X;

                LOOP3_ASSERT(LINE, i, eX, element == eX);
            }
        }

        if (verbose) cout << "\nTesting 'isMember', 'length', 'begin', 'end', "
                          << "'rbegin', 'rend', 'operator++', 'operator--', "
                          << "and 'operator*'." << endl;
        for (ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;
            const int LENGTH       = DATA[ti].d_length;
            const Element *const e = DATA[ti].d_elements;
            const int curLen = LENGTH;

            Obj mX;
            const Obj& X = gg(&mX, SPEC);

            LOOP_ASSERT(ti, curLen == X.length());

            if (curLen != oldLen) {
                if (verbose) cout << "\ton objects of length "
                                  << curLen << ':' << endl;
                LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
            if (veryVeryVerbose) { cout << "\t\t\t"; P(X); }

            LOOP_ASSERT(LINE, LENGTH == X.length());

            int i;
            for (i = 0; i < NUM_VALUES; ++i) {
                bool ev = false;
                for (int j = 0; j < LENGTH; ++j) {
                    if (VALUES[i] == e[j]) {
                        ev = true;
                    }
                }
                LOOP2_ASSERT(LINE, i, ev == X.isMember(VALUES[i]));
            }

            if (veryVerbose) cout << "\t\tTesting forward iterator." << endl;
            {
                Element       sortedElements[SZ];
                Obj::iterator testIterator;
                int           length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);

                LOOP2_ASSERT(LINE, length, LENGTH == length);

                for (i = 0, testIterator = X.begin();
                     i < LENGTH;
                     ++i, ++testIterator) {
                    LOOP_ASSERT(LINE, X.end() != testIterator);

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    LOOP3_ASSERT(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                LOOP_ASSERT(LINE, X.end() == testIterator);

                for (--i; i >= 0; --i) {
                    --testIterator;

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    LOOP3_ASSERT(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                LOOP_ASSERT(LINE, X.begin() == testIterator);
            }

            if (veryVerbose) cout << "\t\tTesting reverse iterator." << endl;
            {
                Element               sortedElements[SZ];
                Obj::reverse_iterator testIterator;
                int                   length = sortElementsAndRemoveDuplicates(
                                                                sortedElements,
                                                                SZ,
                                                                e,
                                                                LENGTH);

                LOOP2_ASSERT(LINE, length, LENGTH == length);

                for (i = LENGTH - 1, testIterator = X.rbegin();
                     i >= 0;
                     --i, ++testIterator) {
                    LOOP_ASSERT(LINE, X.rend() != testIterator);

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    LOOP3_ASSERT(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                LOOP_ASSERT(LINE, X.rend() == testIterator);

                for (++i; i < LENGTH; ++i) {
                    --testIterator;

                    if (veryVeryVerbose) {
                        cout << "\t\t\t";
                        P_(sortedElements[i]) P(*testIterator)
                    }

                    LOOP3_ASSERT(LINE, sortedElements[i],   *testIterator,
                                       sortedElements[i] == *testIterator);
                }

                LOOP_ASSERT(LINE, X.rbegin() == testIterator);
            }
        }

        if (verbose) cout << "\nEnd of Basic Accessors Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION gg
        //
        // Concerns:
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
        //   'removeAll').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   For 'arePrintedValuesEquivalent', an enumerated sequence of
        //   specifications and results is created and the method's return
        //   value is compared to the expected return value.
        //
        // Testing:
        //   bdec_DayOfWeekSet& gg(bdec_DayOfWeekSet* obj, const char *spec);
        //   int ggg(bdec_DayOfWeekSet *object, const char *spec, int vF = 1);
        //   bool arePrintedValuesEquivalent (const char *a, const char *b);
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
                { L_,   "A",            1,      { VA }                  },
                { L_,   "B",            1,      { VB }                  },
                { L_,   "F",            1,      { VF }                  },

                { L_,   "CD",           2,      { VC, VD }              },
                { L_,   "AF",           2,      { VA, VF }              },
                { L_,   "~E",           1,      { VE }                  },

                { L_,   "ABC",          3,      { VA, VB, VC }          },
                { L_,   "~BC",          2,      { VB, VC }              },
                { L_,   "A~C",          1,      { VC }                  },
                { L_,   "A~F",          1,      { VF }                  },
                { L_,   "~~C",          1,      { VC }                  },
                { L_,   "~~F",          1,      { VF }                  },

                { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
                { L_,   "~BCD",         3,      { VB, VC, VD }          },
                { L_,   "A~CD",         2,      { VC, VD }              },
                { L_,   "AB~D",         1,      { VD }                  },

                { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
                { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
                { L_,   "AB~DE",        2,      { VD, VE }              },
                { L_,   "A~C~E",        1,      { VE }                  },

                { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

                { L_,   "ABCDE~CDEC~E", 1,      { VE }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_lineNum;
                const char *const    SPEC   = DATA[ti].d_spec_p;
                const int            LENGTH = DATA[ti].d_length;
                const Element *const ELEMS  = DATA[ti].d_elements;
                const int            curLen = strlen(SPEC);

                Obj mX;
                const Obj& X = gg(&mX, SPEC);   // original spec

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY;
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
                    LOOP2_ASSERT(LINE, i, X.isMember(ELEMS[i]));
                    LOOP2_ASSERT(LINE, i, Y.isMember(ELEMS[i]));
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
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;
                const int         curLen = strlen(SPEC);

                Obj mX;

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

        if (verbose) cout << "\nTesting 'arePrintedValuesEquivalent'." << endl;
        {
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
        //
        // Concerns:
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators
        //      - add                   (black-box)
        //      - removeAll             (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1) The default constructor creates the correct initial value.
        //    2) The destructor works correctly.
        //    3) 'add'
        //        3a) produces the expected value.
        //        3b) maintains valid internal state.
        //    4) 'removeAll'
        //        4a) produces the expected value (empty).
        //        4b) maintains valid internal state.
        //
        // Plan:
        //   To address concern 1, create an object using the default
        //   constructor.
        //
        //   To address concern 3a, construct a series of independent
        //   objects, ordered by increasing length.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final add operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object.
        //
        //   To address concerns 4a, construct a similar test, replacing
        //   'add' with 'removeAll'; this time.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'add'
        //          to create an instance of length i, confirm its value (using
        //          basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'add' to create an
        //          instance of length i, use 'removeAll' to erase its value
        //          and confirm (with 'length'), use add to set the instance
        //          to a value of length j, verify the value, and allow the
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.
        //
        //   Note that 'add' is not tested to ensure non-duplicate values
        //   during this bootstrap test.
        //
        // Testing:
        //   bdec_DayOfWeekSet();
        //   ~bdec_DayOfWeekSet();
        //   BOOTSTRAP: void add(DayOfWeek value);
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor." << endl;
        {
            const Obj X;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;

            mX.add(V0);
            ASSERT(1 == X.length());
            ASSERT(X.isMember(V0));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0);

            mX.add(V1);
            ASSERT(2 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1);

            mX.add(V2);
            ASSERT(3 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);

            mX.add(V3);
            ASSERT(4 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2); mX.add(V3);

            mX.add(V4);
            ASSERT(5 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
        }

        {
            if (verbose) cout << "\tOn an object of initial length 5." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2); mX.add(V3); mX.add(V4);

            mX.add(V5);
            LOOP_ASSERT(X.length(), 6 == X.length());

            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
            ASSERT(X.isMember(V5));
        }

        {
            if (verbose) cout << "\tOn an object of initial length 6." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);
            mX.add(V3); mX.add(V4); mX.add(V5);

            mX.add(V6);
            ASSERT(7 == X.length());
            ASSERT(X.isMember(V0));
            ASSERT(X.isMember(V1));
            ASSERT(X.isMember(V2));
            ASSERT(X.isMember(V3));
            ASSERT(X.isMember(V4));
            ASSERT(X.isMember(V5));
            ASSERT(X.isMember(V6));
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0);
            ASSERT(1 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1);
            ASSERT(2 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX;  const Obj& X = mX;
            mX.add(V0); mX.add(V1); mX.add(V2);
            ASSERT(3 == X.length());

            mX.removeAll();
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting the destructor." << endl;

        if (verbose) cout << "\tWith 'add' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1) create an instance
            //    2) add { V0, V1, V2, V3, V4, V5, ... }  up to length i
            //    3) verify initial length and contents
            //    4) allow the instance to leave scope

            const int NUM_TRIALS = 15;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              {
                int k; // loop index

                Obj mX;  const Obj& X = mX;                             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.add(VALUES[k % NUM_VALUES]);
                }

                const int len = i > NUM_VALUES ? NUM_VALUES : i;
                LOOP_ASSERT(i, len == X.length());                      // 3.
                for (k = 0; k < i; ++k) {
                  LOOP2_ASSERT(i, k, X.isMember(VALUES[k % NUM_VALUES]));
                }

              }                                                         // 4.
            }
        }

        if (verbose) cout << "\tWith 'add' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1) create an instance
            //    2) add values up to a length of i
            //    3) verify initial length and contents
            //    4) removeAll contents from instance
            //    5) verify length is 0
            //    6) add { V0, V1, V2, V3, V4, V0, ... }  up to length j
            //    7) verify new length and contents
            //    8) allow the instance to leave scope

            const int NUM_TRIALS = 15;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  {
                    int k; // loop index

                    Obj mX;  const Obj& X = mX;                         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.add(VALUES[k % NUM_VALUES]);
                    }

                    const int len = i > NUM_VALUES ? NUM_VALUES : i;
                    LOOP2_ASSERT(i, j, len == X.length());              // 3.
                    for (k = 0; k < i; ++k) {
                      LOOP3_ASSERT(i, j, k,
                                   X.isMember(VALUES[k % NUM_VALUES]));
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.add(VALUES[k % NUM_VALUES]);
                    }

                    const int len2 = j > NUM_VALUES ? NUM_VALUES : j;
                    LOOP2_ASSERT(i, j, len2 == X.length());             // 7.
                    for (k = 0; k < j; ++k) {
                      LOOP3_ASSERT(i, j, k,
                                   X.isMember(VALUES[k % NUM_VALUES]));
                    }
                  }                                                     // 8.
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
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
        // 1) Create an object x1 (default ctor).       { x1: }
        // 2) Create a second object x2 (copy from x1). { x1: x2: }
        // 3) Add an element value A to x1.             { x1:A x2: }
        // 4) Add the same element value A to x2.       { x1:A x2:A }
        // 5) Add another element value B to x2.        { x1:A x2:AB }
        // 6) Remove all elements from x1.              { x1: x2:AB }
        // 7) Create a third object x3 (default ctor).  { x1: x2:AB x3: }
        // 8) Create a fourth object x4 (copy of x2).   { x1: x2:AB x3: x4:AB }
        // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
        // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   This test case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Add an element value A to x1."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.add(VA);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(X1.isMember(VA));

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Add the same element value A to x2."
                             "\t\t\t{ x1:A x2:A }" << endl;
        mX2.add(VA);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(X2.isMember(VA));

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Add another element value B to x2."
                             "\t\t\t{ x1:A x2:AB }" << endl;

        mX2.add(VB);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(X2.isMember(VA));
        ASSERT(X2.isMember(VB));

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6) Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(X3 == X1 == 1);          ASSERT(X3 != X1 == 0);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8) Create a forth object x4 (copy of x2)."
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(X4.isMember(VA));
        ASSERT(X4.isMember(VB));
        ASSERT(!X4.isMember(VC));

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 1);          ASSERT(X4 != X2 == 0);
        ASSERT(X4 == X3 == 0);          ASSERT(X4 != X3 == 1);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9) Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        ASSERT(X2 == X3 == 1);          ASSERT(X2 != X3 == 0);
        ASSERT(X2 == X4 == 0);          ASSERT(X2 != X4 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10) Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(2 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(X3 == X1 == 0);          ASSERT(X3 != X1 == 1);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);
        ASSERT(X3 == X4 == 1);          ASSERT(X3 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(2 == X4.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 0);          ASSERT(X4 != X2 == 1);
        ASSERT(X4 == X3 == 1);          ASSERT(X4 != X3 == 0);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying the iterator..." << endl;
        {
            if (verbose) cout << "\tX4 = {" << flush;
            for (Obj::iterator it = X4.begin(); it != X4.end(); ++it) {
                if (verbose) cout << ' ' << *it << flush;
            }
            if (verbose) cout << " }" << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying the reverse_iterator..." << endl;
        {
            if (verbose) cout << "\tX4 = {" << flush;
            for (Obj::reverse_iterator it = X4.rbegin(); it != X4.rend(); ++it)
            {
                if (verbose) cout << ' ' << *it << flush;
            }
            if (verbose) cout << " }" << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying the 'print' method..." << endl;
        if (verbose) {
            cout << endl;
            X4.print(cout);
            X4.print(cout,  0, 2);
            X4.print(cout,  1, 2);
            X4.print(cout,  2, 2);
            X4.print(cout,  0, -1);  cout << endl;
            X4.print(cout,  0, -2);  cout << endl;
            X4.print(cout,  0, -4);  cout << endl;
            X4.print(cout,  2, -4);  cout << endl;
            X4.print(cout,  4, -4);  cout << endl;
            X4.print(cout, -2, -4);  cout << endl;
            X4.print(cout, -4, -4);  cout << endl;
            X4.print(cout,  0, 2);
            cout << X4 << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying 'operator<<'..." << endl;
        if (verbose) {
            cout << endl;
            cout << X4 << endl;
            cout << X4 << X4 << X4 << endl;
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
