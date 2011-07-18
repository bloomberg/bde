// bdema_managedptr.t.cpp                                             -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_lingeroptions_t_cpp,"$Id$ $CSID$")

#include <bdema_managedptr.h>

#include <bsl_climits.h>     // 'INT_MIN', 'INT_MAX'
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bslalg_hastrait.h>

#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, complex-constrained in-core
// value-semantic class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'set'
//: o 'clear'
//
// Basic Accessors:
//: o 'object'
//: o 'factory'
//: o 'deleter'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not implemented for this class.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
// ----------------------------------------------------------------------------
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] bdema_ManagedPtrDeleter()
// [ 3] bdema_ManagedPtrDeleter(void *object, void *factory, Deleter deleter)
// [ 7] bdema_ManagedPtrDeleter(const bdema_ManagedPtrDeleter& original)
// [ 2] ~bdema_ManagedPtrDeleter()
//
// MANIPULATORS
// [ 2] set(void *object, void *factory, Deleter deleter)
// [11] clear()
//
// ACCESSORS
// [12] void deleteManagedObject() const
// [ 4] void (*)(void *, void *) deleter() const
// [ 4] void *factory() const
// [ 4] void *object() const
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const
//
// FREE OPERATORS
// [ 6] bool operator==(const bdema_ManagedPtrDeleter& lhs, rhs)
// [ 6] bool operator!=(const bdema_ManagedPtrDeleter& lhs, rhs)
// [ 5] operator<<(ostream& s, const bdema_ManagedPtrDeleter& d)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 4] CONCERN: All accessor methods are declared 'const'.
// [ 8] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdema_ManagedPtrDeleter Obj;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bslalg_HasTrait<Obj, bslalg_TypeTraitBitwiseMoveable>::VALUE));

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used by test cases 3, 7, 8, 9, and 10

static const struct {
    int           d_line;           // source line number
    void         *d_object;
    void         *d_factory;
    Obj::Deleter  d_deleter;
} DEFAULT_DATA[] = {
    //LINE  OBJECT    FACTORY   DELETER
    //----  ------    -------   -------

    // default (must be first)
    { L_,        0,         0,        0},

    //// 'timeout'
    //{ L_,         1,  false },
    //{ L_,   INT_MAX,  false },

    //// 'useLingeringFlag'
    //{ L_,         0,   true },

    //// other
    //{ L_,         1,   true },
    //{ L_,   INT_MAX,   true },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
///Usage
///-----
//// The following snippets of code illustrate how to set and get linger options
//// on a socket having the specified 'socketHandle'.
////..
//    bdema_ManagedPtrDeleter lingerOptions;
////
//    // Set the lingering option with a timeout of 2 seconds
//    lingerOptions.setUseLingeringFlag(true);
//    lingerOptions.setTimeout(2);
////..
//// We can then use set these linger options on any socket handle using
//// 'bteso_SocketOptions'.
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ACCESSOR 'deleteManagedObject'
        ////   Ensure that the method correctly set the object to its default
        ////   value.
        ////
        //// Concerns:
        ////: 1 The method correctly set the object to its default value.
        ////
        //// Plan:
        ////: 1 Using the table-driven technique:
        ////:   1 Specify a set of (unique) valid object values (one per row) in
        ////:     terms of their individual attributes, including (a) first, the
        ////:     default value, and (b) boundary values corresponding to every
        ////:     range of values that each individual attribute can
        ////:     independently attain.
        ////:
        ////: 2 For each row (representing a distinct attribute value, 'V') in
        ////:   the table of P-1, verify that the method sets the object to its
        ////:   default state.
        ////
        // Testing:
        //   deleteManagedObject()
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "MANIPULATOR 'reset'" << endl
                 << "===================" << endl;

        if (verbose) cout <<
                            "\nUse a table of distinct object values." << endl;

        //const int NUM_DATA                     = DEFAULT_NUM_DATA;
        //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const int          LINE    = DEFAULT_DATA[ti].d_line;
            void        *const OBJECT  = DEFAULT_DATA[ti].d_object;
            void        *const FACTORY = DEFAULT_DATA[ti].d_factory;
            const Obj::Deleter DELETER = DEFAULT_DATA[ti].d_deleter;

            Obj mX(OBJECT, FACTORY, DELETER); const Obj& X = mX;

            //if (veryVerbose) { T_ P_(LINE) P(X) }

            //mX.clear();

            //LOOP2_ASSERT(LINE, X, Obj() == X);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'clear'
        //   Ensure that the method correctly set the object to its default
        //   value.
        //
        // Concerns:
        //: 1 The method correctly set the object to its default value.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 2 For each row (representing a distinct attribute value, 'V') in
        //:   the table of P-1, verify that the method sets the object to its
        //:   default state.
        //
        // Testing:
        //   clear()
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "MANIPULATOR 'reset'" << endl
                 << "===================" << endl;

        if (verbose) cout <<
                            "\nUse a table of distinct object values." << endl;

        //const int NUM_DATA                     = DEFAULT_NUM_DATA;
        //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const int          LINE    = DEFAULT_DATA[ti].d_line;
            void        *const OBJECT  = DEFAULT_DATA[ti].d_object;
            void        *const FACTORY = DEFAULT_DATA[ti].d_factory;
            const Obj::Deleter DELETER = DEFAULT_DATA[ti].d_deleter;

            Obj mX(OBJECT, FACTORY, DELETER); const Obj& X = mX;

            if (veryVerbose) { T_ P_(LINE) P(X) }

            mX.clear();

            LOOP2_ASSERT(LINE, X, Obj() == X);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   bdex streaming is not implemented for this type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING (not implemented)" << endl
                          << "================================" << endl;

        // scalar and array object values for various stream tests

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:     having the value 'W'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as
        //:     that of 'mX'.  (C-3)
        //:
        //:   5 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:     1 The target object, 'mX', now has the same value as that of
        //:       'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, the source object, 'Z', is a reference to the target
        //:   object, 'mX', and both 'mX' and 'ZZ' are initialized to have the
        //:   value 'V'.  For each row (representing a distinct object value,
        //:   'V') in the table described in P-2:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.  (C-5)
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //
        // Testing:
        //   operator=(const bdema_ManagedPtrDeleter& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
                            "\nUse a table of distinct object values." << endl;
        //const int NUM_DATA                     = DEFAULT_NUM_DATA;
        //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const int          LINE1    = DEFAULT_DATA[ti].d_line;
            void        *const OBJECT1  = DEFAULT_DATA[ti].d_object;
            void        *const FACTORY1 = DEFAULT_DATA[ti].d_factory;
            const Obj::Deleter DELETER1 = DEFAULT_DATA[ti].d_deleter;

            const Obj  Z(OBJECT1, FACTORY1, DELETER1);
            const Obj ZZ(OBJECT1, FACTORY1, DELETER1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const int          LINE2    = DEFAULT_DATA[ti].d_line;
                void        *const OBJECT2  = DEFAULT_DATA[ti].d_object;
                void        *const FACTORY2 = DEFAULT_DATA[ti].d_factory;
                const Obj::Deleter DELETER2 = DEFAULT_DATA[ti].d_deleter;

                Obj mX(OBJECT2, FACTORY2, DELETER2);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                LOOP4_ASSERT(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            // self-assignment

            {
                      Obj mX(OBJECT1, FACTORY1, DELETER1);
                const Obj ZZ(OBJECT1, FACTORY1, DELETER1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);
                LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can independently
        //:     attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Create an objects using the copy constructor on 'Z' from P-2.1.
        //:
        //:   3 Use the copy constructor to create an object 'X',
        //:     supplying it the 'const' object 'Z'.  (C-2)
        //:
        //:   4 Use the equality-comparison operator to verify that:
        //:     (C-1, 3)
        //:
        //:     1 The newly constructed object, 'X', has the same value as
        //:       that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   bdema_ManagedPtrDeleter(const bdema_ManagedPtrDeleter& o);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
                            "\nUse a table of distinct object values." << endl;

        //const int NUM_DATA                     = DEFAULT_NUM_DATA;
        //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const int          LINE    = DEFAULT_DATA[ti].d_line;
            void        *const OBJECT  = DEFAULT_DATA[ti].d_object;
            void        *const FACTORY = DEFAULT_DATA[ti].d_factory;
            const Obj::Deleter DELETER = DEFAULT_DATA[ti].d_deleter;

            const Obj  Z(OBJECT, FACTORY, DELETER);
            const Obj ZZ(OBJECT, FACTORY, DELETER);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX = Z;  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Verify the value of the object.

            LOOP3_ASSERT(LINE, Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);
        }  // end foreach row

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..8)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..3, 6..8)
        //:
        //:       1 Create an object 'X' having the value 'R1'.
        //:
        //:       2 Create an object 'Y' having the value 'R2'.
        //:
        //:       3 Verify the commutativity property and expected return value
        //:         for both '==' and '!='.  (C-1..3, 6..8)
        //
        // Testing:
        //   bool operator==(const bdema_ManagedPtrDeleter& lhs, rhs);
        //   bool operator!=(const bdema_ManagedPtrDeleter& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef void        *T1;        // 'object'
        typedef void        *T2;        // 'factory'
        typedef Obj::Deleter T3;        // 'deleter'

        // Attribute 1 Values: 'timeout'

        //const T1 A1 = 1;               // baseline
        //const T1 B1 = INT_MAX;

        //// Attribute 2 Values: 'useLingeringFlag'

        //const T2 A2 = false;           // baseline
        //const T2 B2 = true;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int           d_line;           // source line number
            void         *d_object;
            void         *d_factory;
            Obj::Deleter  d_deleter;
        } DATA[] = {
        //LINE  OBJECT    FACTORY   DELETER
        //----  ------    -------   -------

        // default (must be first)
        { L_,        0,         0,        0},

        //// 'timeout'
        //{ L_,         1,  false },
        //{ L_,   INT_MAX,  false },

        //// 'useLingeringFlag'
        //{ L_,         0,   true },

        //// other
        //{ L_,         1,   true },
        //{ L_,   INT_MAX,   true },

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  TIMEOUT  FLAG
        //----  -------  ----

        //////////{ L_,        A1,   A2 },       // baseline

        //////////{ L_,        B1,   A2 },
        //////////{ L_,        A1,   B2 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE1    = DEFAULT_DATA[ti].d_line;
            void        *const OBJECT1  = DEFAULT_DATA[ti].d_object;
            void        *const FACTORY1 = DEFAULT_DATA[ti].d_factory;
            const Obj::Deleter DELETER1 = DEFAULT_DATA[ti].d_deleter;

            if (veryVerbose) { 
                T_ P_(LINE1) P_(OBJECT1) P_(FACTORY1) P_(DELETER1) 
            }

            // Ensure an object compares correctly with itself (alias test).
            {
                const Obj X(OBJECT1, FACTORY1, DELETER1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int          LINE2    = DEFAULT_DATA[ti].d_line;
                void        *const OBJECT2  = DEFAULT_DATA[ti].d_object;
                void        *const FACTORY2 = DEFAULT_DATA[ti].d_factory;
                const Obj::Deleter DELETER2 = DEFAULT_DATA[ti].d_deleter;

                if (veryVerbose) { 
                    T_ T_ P_(LINE2) P_(OBJECT2) P_(FACTORY2) P_(DELETER2) 
                }

                const bool EXP = ti == tj;  // expected for equality comparison

                const Obj X(OBJECT1, FACTORY1, DELETER1);
                const Obj Y(OBJECT2, FACTORY2, DELETER2);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify value, and commutativity.

                LOOP4_ASSERT(LINE1, LINE2, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(LINE1, LINE2, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(LINE1, LINE2, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 6)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5, 7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are,
        //:       arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:       invoked instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 7)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const bdema_ManagedPtrDeleter& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            int         d_timeout;
            bool        d_useLingeringFlag;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  EXP
        //---- - ---  ---   ----  ---

        { L_,  0,  0,  89,  true, "["                                        NL
                                  "timeout = 89"                             NL
                                  "useLingeringFlag = true"                  NL
                                  "]"                                        NL
                                                                             },

        { L_,  0,  1,  89,  true, "["                                        NL
                                  " timeout = 89"                            NL
                                  " useLingeringFlag = true"                 NL
                                  "]"                                        NL
                                                                             },

        { L_,  0, -1,  89,  true, "["                                        SP
                                  "timeout = 89"                             SP
                                  "useLingeringFlag = true"                  SP
                                  "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  EXP
        //---- - ---  ---   ----  ---

        { L_,  3,  0,  89,  true, "["                                        NL
                                  "timeout = 89"                             NL
                                  "useLingeringFlag = true"                  NL
                                  "]"                                        NL
                                                                             },

        { L_,  3,  2,  89,  true, "      ["                                  NL
                                  "        timeout = 89"                     NL
                                  "        useLingeringFlag = true"          NL
                                  "      ]"                                  NL
                                                                             },

        { L_,  3, -2,  89,  true, "      ["                                  SP
                                  "timeout = 89"                             SP
                                  "useLingeringFlag = true"                  SP
                                  "]"
                                                                             },

        { L_, -3,  0,  89,  true, "["                                        NL
                                  "timeout = 89"                             NL
                                  "useLingeringFlag = true"                  NL
                                  "]"                                        NL
                                                                             },

        { L_, -3,  2,  89,  true, "["                                        NL
                                  "        timeout = 89"                     NL
                                  "        useLingeringFlag = true"          NL
                                  "      ]"                                  NL
                                                                             },

        { L_, -3, -2,  89,  true, "["                                        SP
                                  "timeout = 89"                             SP
                                  "useLingeringFlag = true"                  SP
                                  "]"
                                                                             },
        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  EXP
        //---- - ---  ---   ----  ---

        { L_,  2,  3,   7, false, "      ["                                  NL
                                  "         timeout = 7"                     NL
                                  "         useLingeringFlag = false"        NL
                                  "      ]"                                  NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  EXP
        //---- - ---  ---   ----  ---

        { L_, -9, -9,  89,  true, "[ 89 true ]"                              },

        { L_, -9, -9,   7, false, "[ 7 false ]"                              },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const int         OFF    = DATA[ti].d_timeout;
                const bool        FLAG   = DATA[ti].d_useLingeringFlag;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(OFF) P_(FLAG)}

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                //const Obj X(OFF, FLAG);

                //ostringstream os;

                //if (-9 == L && -9 == SPL) {

                //    // Verify supplied stream is returned by reference.

                //    LOOP_ASSERT(LINE, &os == &(os << X));

                //    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                //}
                //else {

                //    // Verify supplied stream is returned by reference.

                //    LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));

                //    if (veryVeryVerbose) { T_ T_ Q(print) }
                //}

                //// Verify output is formatted as expected.

                //if (veryVeryVerbose) { P(os.str()) }

                //LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):  (C-1)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //
        // Testing:
        //   bool factory() const;
        //   int object() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        //// Attribute Types

        //typedef int  T1;     // 'timeout'
        //typedef bool T2;     // 'useLingeringFlag'

        //if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        //// -----------------------------------------------------
        //// 'D' values: These are the default-constructed values.
        //// -----------------------------------------------------

        //const T1 D1   = 0;        // 'timeout'
        //const T2 D2   = false;    // 'useLingeringFlag'

        //// -------------------------------------------------------
        //// 'A' values: Boundary values.
        //// -------------------------------------------------------

        //const T1 A1   = INT_MIN;
        //const T2 A2   = true;

        //if (verbose) cout << "\nCreate an object." << endl;

        //Obj mX;  const Obj& X = mX;

        //if (verbose) cout <<
        //        "\nVerify all basic accessors report expected values." << endl;
        //{
        //    const T1& timeout = X.object();
        //    LOOP2_ASSERT(D1, timeout, D1 == timeout);

        //    const T2& useLingeringFlag = X.factory();
        //    LOOP2_ASSERT(D2, useLingeringFlag, D2 == useLingeringFlag);
        //}

        //if (verbose) cout <<
        //    "\nApply primary manipulators and verify expected values." << endl;

        //if (veryVerbose) { T_ Q(timeout) }
        //{
        //    mX.setTimeout(A1);

        //    const T1& timeout = X.object();
        //    LOOP2_ASSERT(A1, timeout, A1 == timeout);
        //}

        //if (veryVerbose) { T_ Q(useLingeringFlag) }
        //{
        //    mX.setUseLingeringFlag(A2);

        //    const T2& useLingeringFlag = X.factory();
        //    LOOP2_ASSERT(A2, useLingeringFlag, A2 == useLingeringFlag);
        //}
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..2)
        //:
        //:   1 Use the value constructor to create an object having the value
        //:     'V' supplying all the arguments as 'const'. (C-2)
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     that all of the attributes of each object have their expected
        //:     values.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   bdema_ManagedPtrDeleter(int o, bool f);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        //if (verbose) cout <<
        //                    "\nUse a table of distinct object values." << endl;
        //const int NUM_DATA                     = DEFAULT_NUM_DATA;
        //const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        //for (int ti = 0; ti < NUM_DATA; ++ti) {
        //    const int         LINE    = DATA[ti].d_line;
        //    const int         TIMEOUT = DATA[ti].d_timeout;
        //    const bool        FLAG    = DATA[ti].d_useLingeringFlag;

        //    if (veryVerbose) { T_ P_(TIMEOUT) P_(FLAG) }

        //    Obj mX(TIMEOUT, FLAG);  const Obj& X = mX;

        //    if (veryVerbose) { T_ T_ P(X) }

        //    // Use untested functionality to help ensure the first row
        //    // of the table contains the default-constructed value.

        //    static bool firstFlag = true;
        //    if (firstFlag) {
        //        LOOP3_ASSERT(LINE, Obj(), X, Obj() == X)
        //        firstFlag = false;
        //    }

        //    // -------------------------------------
        //    // Verify the object's attribute values.
        //    // -------------------------------------

        //    LOOP3_ASSERT(LINE, TIMEOUT, X.object(), TIMEOUT == X.object());

        //    LOOP3_ASSERT(LINE, FLAG, X.factory(),
        //                 FLAG == X.factory());
        //}

        //if (verbose) cout << "\nNegative Testing." << endl;
        //{
        //    bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

        //    if (veryVerbose) cout << "\t'timeout'" << endl;
        //    {
        //        ASSERT_SAFE_PASS(Obj( 0, false));
        //        ASSERT_SAFE_FAIL(Obj(-1, false));
        //    }
        //}
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Any argument can be 'const'.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: 'D', 'A',
        //:   and 'B'.  'D' values corresponding to the default-constructed
        //:   object, 'A' and 'B' values are chosen to be distinct boundary
        //:   values where possible.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values, then
        //:   setting all of the attributes to their 'B' values.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   bdema_ManagedPtrDeleter();
        //   ~bdema_ManagedPtrDeleter();
        //   setTimeout(int value);
        //   setUseLingeringFlag(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        // Attribute 1 Values: 'object'

        const void        *D1 = 0;      // 'object'
        const void        *D2 = 0;      // 'factory'
        const Obj::Deleter D3 = 0;      // 'deleter'

        // 'A' values.

//        const int  A1   = 1;
//        const bool A2   = true;

//        const T1 A1 = 60;
//        const T2 A2 = true;
//        const T2 A3 = true;

        // 'B' values.

//        const int    B1 = INT_MAX;
//        const bool   B2 = false;

        {
            Obj mX;  const Obj& X = mX;

            //// -----------------------------------------------------
            //// Verify that each attribute is independently settable.
            //// -----------------------------------------------------

            //// 'timeout'
            //{
            //    mX.setTimeout(A1);
            //    ASSERT(A1 == X.object());
            //    ASSERT(D2 == X.factory());
            //    ASSERT(D3 == X.deleter());

            //    mX.setTimeout(B1);
            //    ASSERT(B1 == X.object());
            //    ASSERT(D2 == X.factory());

            //    mX.setTimeout(D1);
            //    ASSERT(D1 == X.object());
            //    ASSERT(D2 == X.factory());
            //    ASSERT(D3 == X.deleter());
            //}

            //// 'useLingeringFlag'
            //{
            //    mX.setUseLingeringFlag(A2);
            //    ASSERT(D1 == X.object());
            //    ASSERT(A2 == X.factory());
            //    ASSERT(D3 == X.deleter());

            //    mX.setUseLingeringFlag(B2);
            //    ASSERT(D1 == X.object());
            //    ASSERT(B2 == X.factory());
            //    ASSERT(D3 == X.deleter());

            //    mX.setUseLingeringFlag(D2);
            //    ASSERT(D1 == X.object());
            //    ASSERT(D2 == X.factory());
            //    ASSERT(D3 == X.deleter());
            //}

            //// Corroborate attribute independence.
            //{
            //    // Set all attributes to their 'A' values.

            //    mX.setTimeout(A1);
            //    mX.setUseLingeringFlag(A2);

            //    ASSERT(A1 == X.object());
            //    ASSERT(A2 == X.factory());

            //    // Set all attributes to their 'B' values.

            //    mX.setTimeout(B1);

            //    ASSERT(B1 == X.object());
            //    ASSERT(A2 == X.factory());

            //    mX.setUseLingeringFlag(B2);

            //    ASSERT(B1 == X.object());
            //    ASSERT(B2 == X.factory());

            //}
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\ttimeout" << endl;
            {
                //ASSERT_SAFE_PASS(obj.setTimeout( 0));
                //ASSERT_SAFE_FAIL(obj.setTimeout(-1));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute Types

        typedef void        *T1;        // 'object'
        typedef void        *T2;        // 'factory'
        typedef Obj::Deleter T3;        // 'deleter'

        // Attribute 1 Values: 'object'

        const T1 D1 = 0;               // default value
 //       const T1 A1 = 60;

        // Attribute 2 Values: 'factory'

        const T2 D2 = 0;           // default value
//        const T2 A2 = true;

        // Attribute 3 Values: 'deleter'

        const T2 D3 = 0;           // default value
//        const T2 A3 = true;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.object());
        ASSERT(D2 == W.factory());
        ASSERT(D3 == W.deleter());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.object());
        ASSERT(D2 == X.factory());
        ASSERT(D3 == X.deleter());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
        //                     "\t\t{ w:D x:A         }" << endl;

        //mX.setTimeout(A1);
        //mX.setUseLingeringFlag(A2);

        //if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(X) }

        //ASSERT(A1 == X.object());
        //ASSERT(A2 == X.factory());

        //if (veryVerbose) cout <<
        //     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        //ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        //ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
        //                     "\t\t{ w:D x:A y:A     }" << endl;

        //Obj mY(A1, A2);  const Obj& Y = mY;

        //if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(Y) }

        //ASSERT(A1 == Y.object());
        //ASSERT(A2 == Y.factory());

        //if (veryVerbose) cout <<
        //     "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        //ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        //ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        //ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
        //                     "\t\t{ w:D x:A y:A z:A }" << endl;

        //Obj mZ(Y);  const Obj& Z = mZ;

        //if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(Z) }

        //ASSERT(A1 == Z.object());
        //ASSERT(A2 == Z.factory());

        //if (veryVerbose) cout <<
        //   "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        //ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        //ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        //ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        //ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
        //                     "\t\t\t{ w:D x:A y:A z:D }" << endl;

        //mZ.setTimeout(D1);
        //mZ.setUseLingeringFlag(D2);

        //if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(Z) }

        //ASSERT(D1 == Z.object());
        //ASSERT(D2 == Z.factory());

        //if (veryVerbose) cout <<
        //   "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        //ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        //ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        //ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        //ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 7. Assign 'w' from 'x'."
        //                     "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        //mW = X;

        //if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(W) }

        //ASSERT(A1 == W.object());
        //ASSERT(A2 == W.factory());

        //if (veryVerbose) cout <<
        //   "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        //ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        //ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        //ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        //ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 8. Assign 'w' from 'z'."
        //                     "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        //mW = Z;

        //if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(W) }

        //ASSERT(D1 == W.object());
        //ASSERT(D2 == W.factory());

        //if (veryVerbose) cout <<
        //   "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        //ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        //ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        //ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        //ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        //// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        //if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
        //                     "\t\t\t{ w:D x:A y:A z:D }" << endl;
        //mX = X;

        //if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        //if (veryVeryVerbose) { T_ T_ P(X) }

        //ASSERT(A1 == X.object());
        //ASSERT(A2 == X.factory());

        //if (veryVerbose) cout <<
        //   "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        //ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        //ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        //ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        //ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
