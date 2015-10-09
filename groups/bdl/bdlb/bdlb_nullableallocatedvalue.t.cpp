// bdlb_nullableallocatedvalue.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullableallocatedvalue.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bsl_algorithm.h>  // 'swap'
#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD
//
// Primary Manipulators and Basic Accessors
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Primary Manipulators:
//
// Basic Accessors:
//
//-----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

typedef bdlb::NullableAllocatedValue<int> NullableInt;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableInt>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableInt>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableInt>::value);

typedef bdlb::NullableAllocatedValue<bsl::string> NullableString;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableString>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableString>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableString>::value);

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate use of this component.
//
// Suppose we want to create a linked list of nodes that contain integers:
//..
    struct LinkedListNode {
        int                                          d_value;
        bdlb::NullableAllocatedValue<LinkedListNode> d_next;
    };
//..
// Note that 'bdlb::NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdlb::NullableValue' requires that the template parameter 'TYPE' be
// a complete type when the class is instantiated.
//
// We can now traverse a linked list and add a new value at the end using the
// following code:
//..
    void addValueAtEnd(LinkedListNode *node, int value)
    {
        while (!node->d_next.isNull()) {
            node = &node->d_next.value();
        }

        node->d_next.makeValue();
        node = &node->d_next.value();
        node->d_value = value;
    }
//..

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class Swappable {

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'Swappable' objects created
                                      // since last reset

    // DATA
    int d_value;

    // FRIENDS
    friend bool operator==(const Swappable&, const Swappable&);
    friend void swap(Swappable&, Swappable&);

  private:
    // NOT IMPLEMENTED
    Swappable();

  public:
    // CLASS METHODS
    static int numObjectsCreated()
    {
        return s_numObjectsCreated;
    }

    static void reset()
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    Swappable(int v)
    : d_value(v)
    {
        ++Swappable::s_numObjectsCreated;
    }

    Swappable(const Swappable& original)
    : d_value(original.d_value)
    {
        ++Swappable::s_numObjectsCreated;
    }

    // MANIPULATORS
    void swap(Swappable&)
    {
        BSLS_ASSERT_OPT(0);
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.d_value == rhs.d_value;
}

bool operator!=(const Swappable& lhs, const Swappable& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool Swappable::s_swapCalledFlag    = false;
int  Swappable::s_numObjectsCreated = 0;

void swap(Swappable& a, Swappable& b)
{
    Swappable::s_swapCalledFlag = true;

    bsl::swap(a.d_value, b.d_value);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    bslma::TestAllocator *ALLOC = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        LinkedListNode node;
        node.d_value = 3;
        ASSERT(node.d_next.isNull());

        addValueAtEnd(&node, 5);
        ASSERT(!node.d_next.isNull());
        ASSERT(node.d_next.value().d_next.isNull());
        ASSERT(3 == node.d_value);
        ASSERT(5 == node.d_next.value().d_value);

        addValueAtEnd(&node, 53);
        ASSERT(!node.d_next.isNull());
        ASSERT(!node.d_next.value().d_next.isNull());
        ASSERT(node.d_next.value().d_next.value().d_next.isNull());
        ASSERT( 3 == node.d_value);
        ASSERT( 5 == node.d_next.value().d_value);
        ASSERT(53 == node.d_next.value().d_next.value().d_value);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //: 1 Swap of two null objects leaves both objects null.
        //:
        //: 2 Swap of a null object and a non-null object, or of two non-null
        //:   objects, swaps the underlying pointers to the out-of-place
        //:   objects (i.e., no temporary objects are incurred to effect the
        //:   swap).
        //
        // Plan:
        //: 1 Create a class, 'Swappable', with a 'swap' free function
        //:   instrumented to track swap calls and creators instrumented to
        //:   track the number of 'Swappable' objects created.  Instantiate
        //:   'bdlb::NullableAllocatedValue' with 'Swappable' and execute
        //:   operations needed to verify the concerns.  (C-1..2)
        //
        // Testing:
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& other);
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (veryVerbose) cout << "\tTesting member 'swap'." << endl;
        {
            // Swap two null objects.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> nullObj1;
            bdlb::NullableAllocatedValue<Swappable> nullObj2;

            ASSERT(nullObj1.isNull());
            ASSERT(nullObj2.isNull());

            nullObj1.swap(nullObj2);

            ASSERT(nullObj1.isNull());
            ASSERT(nullObj2.isNull());

            ASSERT(    !Swappable::swapCalled());
            ASSERT(0 == Swappable::numObjectsCreated());
        }

        {
            // Swap a null object and a non-null object.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> nonNullObj(Swappable(10));
            bdlb::NullableAllocatedValue<Swappable> nonNullObjCopy(nonNullObj);
            bdlb::NullableAllocatedValue<Swappable> nullObj;

            ASSERT(!nonNullObj.isNull());
            ASSERT(!nonNullObjCopy.isNull());
            ASSERT( nullObj.isNull());

            ASSERT(nonNullObj == nonNullObjCopy);
            ASSERT(nullObj    != nonNullObjCopy);

            const int numObjects = Swappable::numObjectsCreated();

            nonNullObj.swap(nullObj);

            ASSERT( nonNullObj.isNull());
            ASSERT(!nonNullObjCopy.isNull());
            ASSERT(!nullObj.isNull());

            ASSERT(nonNullObj != nonNullObjCopy);
            ASSERT(nullObj    == nonNullObjCopy);

            ASSERT(             !Swappable::swapCalled());
            ASSERT(numObjects == Swappable::numObjectsCreated());
        }

        {
            // Swap two non-null objects.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> obj1(Swappable(10));
            bdlb::NullableAllocatedValue<Swappable> obj2(Swappable(20));

            bdlb::NullableAllocatedValue<Swappable> obj1Copy(obj1);
            bdlb::NullableAllocatedValue<Swappable> obj2Copy(obj2);

            ASSERT(!obj1.isNull());
            ASSERT(!obj2.isNull());
            ASSERT(!obj1Copy.isNull());
            ASSERT(!obj2Copy.isNull());

            ASSERT(obj1 == obj1Copy);
            ASSERT(obj2 == obj2Copy);
            ASSERT(obj1 != obj2);

            const int numObjects = Swappable::numObjectsCreated();

            obj1.swap(obj2);

            ASSERT(!obj1.isNull());
            ASSERT(!obj2.isNull());
            ASSERT(!obj1Copy.isNull());
            ASSERT(!obj2Copy.isNull());

            ASSERT(obj1 == obj2Copy);
            ASSERT(obj2 == obj1Copy);
            ASSERT(obj1 != obj2);

            ASSERT(             !Swappable::swapCalled());
            ASSERT(numObjects == Swappable::numObjectsCreated());
        }

        if (veryVerbose) cout << "\tTesting free 'swap'." << endl;
        {
            // Swap two null objects.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> nullObj1;
            bdlb::NullableAllocatedValue<Swappable> nullObj2;

            ASSERT(nullObj1.isNull());
            ASSERT(nullObj2.isNull());

            using bsl::swap;
            swap(nullObj1, nullObj2);

            ASSERT(nullObj1.isNull());
            ASSERT(nullObj2.isNull());

            ASSERT(    !Swappable::swapCalled());
            ASSERT(0 == Swappable::numObjectsCreated());
        }

        {
            // Swap a null object and a non-null object.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> nonNullObj(Swappable(10));
            bdlb::NullableAllocatedValue<Swappable> nonNullObjCopy(nonNullObj);
            bdlb::NullableAllocatedValue<Swappable> nullObj;

            ASSERT(!nonNullObj.isNull());
            ASSERT(!nonNullObjCopy.isNull());
            ASSERT( nullObj.isNull());

            ASSERT(nonNullObj == nonNullObjCopy);
            ASSERT(nullObj    != nonNullObjCopy);

            const int numObjects = Swappable::numObjectsCreated();

            using bsl::swap;
            swap(nonNullObj, nullObj);

            ASSERT( nonNullObj.isNull());
            ASSERT(!nonNullObjCopy.isNull());
            ASSERT(!nullObj.isNull());

            ASSERT(nonNullObj != nonNullObjCopy);
            ASSERT(nullObj    == nonNullObjCopy);

            ASSERT(             !Swappable::swapCalled());
            ASSERT(numObjects == Swappable::numObjectsCreated());
        }

        {
            // Swap two non-null objects.

            Swappable::reset();

            bdlb::NullableAllocatedValue<Swappable> obj1(Swappable(10));
            bdlb::NullableAllocatedValue<Swappable> obj2(Swappable(20));

            bdlb::NullableAllocatedValue<Swappable> obj1Copy(obj1);
            bdlb::NullableAllocatedValue<Swappable> obj2Copy(obj2);

            ASSERT(!obj1.isNull());
            ASSERT(!obj2.isNull());
            ASSERT(!obj1Copy.isNull());
            ASSERT(!obj2Copy.isNull());

            ASSERT(obj1 == obj1Copy);
            ASSERT(obj2 == obj2Copy);
            ASSERT(obj1 != obj2);

            const int numObjects = Swappable::numObjectsCreated();

            using bsl::swap;
            swap(obj1, obj2);

            ASSERT(!obj1.isNull());
            ASSERT(!obj2.isNull());
            ASSERT(!obj1Copy.isNull());
            ASSERT(!obj2Copy.isNull());

            ASSERT(obj1 == obj2Copy);
            ASSERT(obj2 == obj1Copy);
            ASSERT(obj1 != obj2);

            ASSERT(             !Swappable::swapCalled());
            ASSERT(numObjects == Swappable::numObjectsCreated());
        }
      } break;
      case 8: {
        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20140601;

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Use 'bsl::string' for 'TYPE'.
        //
        //   Specify a set of unique values.  Construct and initialize all
        //   combinations (u, v) in the cross product.  Copy construct a
        //   control w from v, assign v to u, and assert that w == u and
        //   w == v.  Then test aliasing by copy constructing a control w from
        //   each u, assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue(const bdlb::NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nTesting assignment u = V." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], ALLOC);  const Obj& V = mV;

                    Obj mW(V, ALLOC);  const Obj& W = mW;

                    mU = V;

                    ASSERTV(U, W, U == W);
                    ASSERTV(V, W, V == W);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;
                Obj mW(U,     ALLOC);  const Obj& W = mW;

                mU = U;

                ASSERTV(U, W, U == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //   Any value must be copy constructible without affecting the
        //   argument.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   Specify a set whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identical objects W and X using tested methods.  Then
        //   copy construct Y from X and use the equality operator to assert
        //   that both X and Y have the same value as W.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue(const bdlb::NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

        if (verbose) cout << "\nUsing bdlb::NullableAllocatedValue<"
                          << "bsl::string>." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X, ALLOC);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //   The '==' operator must return 'false' for objects that are very
        //   similar but still different, but must return 'true' for objects
        //   that are exactly the same.  Likewise, 'operator!=' must return
        //   'true' for objects that are very similar but still different, but
        //   must return 'false' for objects that are exactly the same.
        //
        // Plan:
        //   Use 'int' for 'TYPE'.  Construct a set of objects containing
        //   similar but different values.  Loop through the cross product of
        //   the test data.  For each tuple, use the '==' and '!=' operators
        //   to check their return value for correctness.
        //
        // Testing:
        //   bool operator==(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator!=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        const int NUM_VALUES = 3;

        Obj objArray[NUM_VALUES];

        objArray[1].makeValue(123);
        objArray[2].makeValue(234);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const Obj& U = objArray[i];

            if (veryVerbose) { T_ P_(i) P(U) }

            for (int j = 0; j < NUM_VALUES; ++j) {
                const Obj& V = objArray[j];

                if (veryVeryVerbose) { T_ T_ P_(j) P(V) }

                const bool isSame = (i == j);
                ASSERTV(U, V,  isSame == (U == V));
                ASSERTV(U, V, !isSame == (U != V));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   The print method and output (<<) operator work as expected.
        //
        // Plan:
        //   Conduct the test using 'int' for 'TYPE'.
        //
        //   For a set of values, check that the 'print' function and the
        //   output (<<) operator work as expected.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //   bsl::ostream& operator<<bsl::ostream&, const b_NV<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Print Method & Output (<<) Operator"
                          << "\n==========================================="
                          << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        const ValueType VALUE1          = 123;
        const char      NULL_RESULT[]   = "NULL";
        const char      VALUE1_RESULT[] = "123";

        if (verbose) cout << "\nTesting 'print' Method." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting Output (<<) Operator." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   Verify that the primary manipulators and basic accessors work as
        //   expected.
        //
        // Concerns:
        //   * The default constructor creates a null object.
        //   * 'makeValue()' sets the value to the default value for 'TYPE'.
        //   * 'makeValue(const TYPE&)' sets the value appropriately.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   First, verify the default constructor by testing that the
        //   resulting object is null.
        //
        //   Next, verify that the 'makeValue' function works by making a value
        //   equal to the value passed into 'makeValue'.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue();
        //   ~bdlb::NullableAllocatedValue();
        //   TYPE& makeValue();
        //   BOOTSTRAP: TYPE& makeValue(const TYPE&);
        //   bool isNull() const;
        //   const TYPE& value() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators & Basic Accessors"
                          << "\n=============================================="
                          << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>'."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX;  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(3);  // set some random value
                mX.makeValue();   // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;
                const ValueType VALUE2 = 456;

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE2 == X.value());
            }
        }

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<"
                          << "bsl::string>'." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX(ALLOC);  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue("3");  // set some random value
                mX.makeValue();     // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";
                const ValueType VALUE2 = "456";

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE2 == X.value());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST USING 'int'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isNull'.
        //
        // Plan:
        //   Create four test objects using the default, value, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        // possible values
        const ValueType VA(123);
        const ValueType VB(234);
        const ValueType VC(345);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST USING 'bsl::string'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isSet'.
        //
        // Plan:
        //   Create four test objects using the default, value, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        typedef bsl::string                             ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        // possible values
        const ValueType VA("The");
        const ValueType VB("Breathing");
        const ValueType VC("Test");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
