// balm_metricformat.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricformat.h>

#include <balm_category.h>
#include <balm_metricdescription.h>

#include <bdlf_bind.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// balm::MetricFormatSpec
// CLASS METHODS
// [ 9] static bsl::ostream& formatValue(bsl::ostream&,
//                                       double ,
//                                       const balm::MetricFormatSpec& );
// CREATORS
// [ 3] balm::MetricFormatSpec();
// [ 4] balm::MetricFormatSpec(float , const char *);
// [ 6] balm::MetricFormatSpec(const balm::MetricFormatSpec& );
// [ 3] ~balm::MetricFormatSpec();
// MANIPULATORS
// [ 7] balm::MetricFormatSpec& operator=(const balm::MetricFormatSpec& );
// [ 3] void setScale(float );
// [ 3] void setFormat(const char *);
// ACCESSORS
// [ 3] float scale() const;
// [ 3] const char *format() const;
// [ 8] bsl::ostream& print(bsl::ostream& ) const;
// FREE OPERATORS
// [ 5] operator==(balm::Metricformat&, balm::Metricformat&);
// [ 5] operator!=(balm::Metricformat&, balm::Metricformat&);
// [ 9] bsl::ostream& operator<<(bsl::ostream& , const balm::Metricformat& );
//
// balm::MetricFormat
// CREATORS
// [10] balm::MetricFormat(bslma::Allocator *);
// [13] balm::MetricFormat(const balm::MetricFormat& , bslma::Allocator *);
// [10] ~balm::MetricFormat();
// MANIPULATORS
// [14] balm::MetricFormat& operator=(const balm::MetricFormat& );
// [10] void setFormatSpec(balm::PublicationType::Value  ,
//                         const balm::MetricFormatSpec&  );
// [15] void clearFormatSpecs();
// [16] void clearFormatSpec(PublicationType::Value publicationType);
// ACCESSORS
// [10] MetricFormatSpec *formatSpec(balm::PublicationType::Value);
// [  ] bsl::ostream& print(bsl::ostream& , int, int ) const;
// FREE OPERATORS
// [12] operator==(balm::Metricformat&, balm::Metricformat&);
// [12] operator!=(balm::Metricformat&, balm::Metricformat&);
// [ 9] bsl::ostream& operator<<(bsl::ostream& , const balm::Metricformat& );
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] HELPERS: gg
// [ 2] HELPERS: CombinationIterator
// [17] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::PublicationType   Type;
typedef balm::MetricFormatSpec  Spec;
typedef balm::MetricFormat      Obj;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                         // =========================
                         // class CombinationIterator
                         // =========================

template <class T>
class CombinationIterator {
    // This class provides an iterator over the set of possible combinations
    // of elements.  A 'CombinationIterator' object is supplied a vector of
    // values of parameterized type 'T' at construction.  The behavior is
    // undefined unless the supplied vector contains unique elements and has a
    // size between 1 and 31 (inclusive).  The 'current' method returns a
    // reference to a vector containing the current combination of elements.
    // The 'next' method increments the iterator to the next possible
    // combination of elements.  Note that the sequence of combinations always
    // begins with the empty set and ends with the set containing all values.

    // DATA
    bsl::vector<T> d_values;               // sequence of values to combine
    bsl::vector<T> d_currentCombination;   // current combination
    int            d_bits;                 // current bit mask of 'd_values'
    int            d_maxBits;              // max bit mask of 'd_values'

    // PRIVATE MANIPULATORS
    void createCurrentCombination();
        // Populate 'd_currentCombination' with those elements of 'd_values'
        // whose corresponding bit in the 'd_bits' bit-mask is is 1.

  public:

    // CREATORS
    CombinationIterator(const T          *values,
                        int               numValues,
                        bslma::Allocator *allocator);
    CombinationIterator(const bsl::vector<T>&  values,
                        bslma::Allocator      *allocator);
        // Create an iterator through all possible combinations of the
        // specified 'values', and initialize it with the first
        // combination of values (the empty set of values), use the specified
        // 'allocator' to supply memory.  The behavior is undefined unless '
        // the number of values is less than 32 and each element of 'values'
        // appears only once.

    bool next();
        // If this iterator is not at the end of the sequence of combinations,
        // iterator to the next combination of value and return 'true',
        // otherwise return 'false'.

    // ACCESSORS
    const bsl::vector<T>& current() const;
        // Return a reference to the non-modifiable combination of
        // values that the iterator currently is positioned at.

    bool includesElement(int index) const;
        // Return 'true' if the 'current()' combination contains the value at
        // the specified 'index' in the sequence supplied at construction.

    int position() const;
        // Return the current position in the iteration.

};

                         // -------------------------
                         // class CombinationIterator
                         // -------------------------

// PRIVATE MANIPULATORS
template <class T>
void CombinationIterator<T>::createCurrentCombination()
{
    d_currentCombination.clear();
    for (int i = 0; i < d_values.size(); ++i) {
        if (d_bits & (1 << i)) {
            d_currentCombination.push_back(d_values[i]);
        }
    }
}

// CREATORS
template <class T>
CombinationIterator<T>::CombinationIterator(const T          *values,
                                            int               numValues,
                                            bslma::Allocator *allocator)
: d_values(values, values + numValues, allocator)
, d_currentCombination(allocator)
, d_maxBits( (1 << numValues) - 1 )
, d_bits(0)
{
    BSLS_ASSERT(d_values.size() > 0);
    BSLS_ASSERT(d_values.size() < 32);
    d_currentCombination.reserve(d_values.size());
    createCurrentCombination();
}

template <class T>
CombinationIterator<T>::CombinationIterator(const bsl::vector<T>&  values,
                                            bslma::Allocator      *allocator)
: d_values(values, allocator)
, d_currentCombination(allocator)
, d_maxBits( (1 << values.size()) - 1 )
, d_bits(0)
{
    BSLS_ASSERT(d_values.size() > 0);
    BSLS_ASSERT(d_values.size() < 32);
    d_currentCombination.reserve(d_values.size());
    createCurrentCombination();
}

// MANIPULATORS
template <class T>
inline
bool CombinationIterator<T>::next()
{
    if (d_bits >= d_maxBits) {
        return false;                                                 // RETURN
    }
    ++d_bits;
    createCurrentCombination();
    return true;
}

// ACCESSORS
template <class T>
inline
const typename bsl::vector<T>& CombinationIterator<T>::current() const
{
    return d_currentCombination;
}

template <class T>
inline
bool CombinationIterator<T>::includesElement(int index) const
{
    return d_bits & (1 << index);
}

template <class T>
inline
int CombinationIterator<T>::position() const
{
    return d_bits;
}

// FREE OPERATORS
void gg(balm::MetricFormat *format, const bsl::vector<Type::Value>& types)
{
    bsl::vector<Type::Value>::const_iterator tIt = types.begin();
    for ( ; tIt != types.end(); ++tIt) {
        format->setFormatSpec(*tIt, Spec((int)*tIt, "%x"));
    }
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    const Type::Value ALL_TYPES[] = {
        Type::e_UNSPECIFIED,
        Type::e_TOTAL,
        Type::e_COUNT,
        Type::e_MIN,
        Type::e_MAX,
        Type::e_AVG,
        Type::e_RATE,
        Type::e_RATE_COUNT
    };
    const int NUM_TYPES = sizeof ALL_TYPES / sizeof *ALL_TYPES;

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

//
///Usage
///-----
// The following example demonstrates how to create and configure a
// 'balm::MetricFormat'.  Note that clients of the 'balm' package can set the
// format for a metric through 'balm_configurationutil' or
// 'balm_metricregistry'.
//
// We start by creating a 'balm::MetricFormat' object:
//..
    bslma::Allocator  *allocator = bslma::Default::allocator(0);
    balm::MetricFormat  format(allocator);
//..
// Next we specify that average values should only be printed to two decimal
// places:
//..
    format.setFormatSpec(balm::PublicationType::e_AVG,
                         balm::MetricFormatSpec(1.0, "%.2f"));
//..
// Next we specify that rate values should be formatted as a percentage --
// i.e., multiplied by 100, and then displayed with a "%" character.
//..
    format.setFormatSpec(balm::PublicationType::e_RATE,
                         balm::MetricFormatSpec(100.0, "%.2f%%"));
//..
// We can verify that the correct format specifications have been set.
//..
    ASSERT(balm::MetricFormatSpec(1.0, "%.2f") ==
           *format.formatSpec(balm::PublicationType::e_AVG));
    ASSERT(balm::MetricFormatSpec(100.0, "%.2f%%") ==
           *format.formatSpec(balm::PublicationType::e_RATE));
    ASSERT(0 == format.formatSpec(balm::PublicationType::e_TOTAL));
//..
// We can use the 'balm::MetricFormatSpec::formatValue' utility function to
// format the value 0.055 to the console.  Note however, that there is no
// guarantee that every implementation of 'balm::Publisher' will format metrics
// in this way.
//..
    balm::MetricFormatSpec::formatValue(
            bsl::cout, .055, *format.formatSpec(balm::PublicationType::e_AVG));
    bsl::cout << bsl::endl;
    balm::MetricFormatSpec::formatValue(
           bsl::cout, .055, *format.formatSpec(balm::PublicationType::e_RATE));
    bsl::cout << bsl::endl;
//..
// The resulting console output will be:
//..
//  0.06
//  5.50%
//..
    } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATOR: clearFormatSpec
        //
        // Concerns:
        //   That 'clearFormatSpec' removes the indicated format spec from a
        //   'balm::MetricFormat'.
        //
        // Plan:
        //   Create a set 'S' of possible values for a 'balm::MetricFormat'.
        //   For each possible value in set 'S', initialize a
        //   'balm::MetricFormat' (using the 'gg' generator), and then call
        //   'clearFormatSpec' on each initialized format spec, and verify
        //   that all the format spec has been removed.  Finally, re-add
        //   format specs and verify they are added correctly.
        //
        // Testing:
        //   void clearFormatSpec(PublicationType::Value publicationType);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting manipulator: clearFormatSpec()"
                          << "[balm::MetricFormat]" << endl;

        CombinationIterator<Type::Value> iter(ALL_TYPES, NUM_TYPES, Z);
        do {  // while (iter.next())
            Obj mX(Z); const Obj& MX = mX;
            gg(&mX, iter.current());

            // Test clearFormatSpec.
            for (int i = 0; i < Type::k_LENGTH; ++i) {
                if (iter.includesElement(i)) {
                    ASSERT(Spec(i, "%x") == *MX.formatSpec((Type::Value)i));
                    mX.clearFormatSpec((Type::Value)i);
                }
                ASSERT(0 == MX.formatSpec((Type::Value)i));

                // Verify other format specs are not affected.
                for (int j = 0; j < Type::k_LENGTH; ++j) {
                    if (j <= i || !iter.includesElement(j)) {
                        ASSERT(0 == MX.formatSpec((Type::Value)j));
                    }
                    else {
                        ASSERT(Spec(j, "%x") ==
                               *MX.formatSpec((Type::Value)j));
                    }
                }
            }

            // Re-add format specs.
            for (int i = 0; i < Type::k_LENGTH; ++i) {
                ASSERT(0 == MX.formatSpec((Type::Value)i));
                mX.setFormatSpec((Type::Value)i, Spec(i, "%x"));
                ASSERT(Spec(i, "%x") == *MX.formatSpec((Type::Value)i));
            }
        } while (iter.next());
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATOR: clearFormatSpecs
        //
        // Concerns:
        //   That clear format specs clears all the format specs from a
        //   'balm::MetricFormat'.
        //
        // Plan:
        //   Create a set 'S' of possible values for a 'balm::MetricFormat'.
        //   For each possible value in set 'S', initialize a
        //   'balm::MetricFormat' (using the 'gg' generator), and then call
        //   'clearFormatSpecs', and verify that all the format specs have
        //   been removed.  Finally, re-add format specs and verify they are
        //   added correctly.
        //
        // Testing:
        //   void clearFormatSpecs();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting manipulator: clearFormatSpecs()"
                          << "[balm::MetricFormat]" << endl;

        CombinationIterator<Type::Value> iter(ALL_TYPES, NUM_TYPES, Z);
        do {  // while (iter.next())
            Obj mX(Z); const Obj& MX = mX;
            gg(&mX, iter.current());

            // Test clearFormatSpecs.
            mX.clearFormatSpecs();
            for (int i = 0; i < Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                ASSERT(0 == MX.formatSpec(type));
            }

            // Re-add format specs.
            mX.clearFormatSpecs();
            for (int i = 0; i < Type::k_LENGTH; ++i) {
                ASSERT(0 == MX.formatSpec((Type::Value)i));
                mX.setFormatSpec((Type::Value)i, Spec(i, "%x"));
                ASSERT(Spec(i, "%x") == *MX.formatSpec((Type::Value)i));
            }
        } while (iter.next());
      } break;
      case 15:{
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:  balm::MetricFormatSpec
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const balm::MetricFormatSpec&);
        //   ostream& print(ostream&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting output operators: "
                          << "balm::MetricFormatSpec" << endl;

        Obj mX1;  const Obj& X1 = mX1;
        mX1.setFormatSpec(Type::e_AVG, Spec(2.0, "%x"));
        mX1.setFormatSpec(Type::e_TOTAL, Spec(3.0, "%u"));

        bsl::ostringstream printBuf, operBuf;
        operBuf << X1;
        X1.print(printBuf, 1, 3);

        const char *EXP_1 =
            "[  UNSPECIFIED = NULL "
            " TOTAL = [ scale = 3 format = \"%u\" ] "
            " COUNT = NULL "
            " MIN = NULL "
            " MAX = NULL "
            " AVG = [ scale = 2 format = \"%x\" ] "
            " RATE = NULL "
            " RATE_COUNT = NULL  ] ";

        const char *EXP_2 =
            "   [\n"
            "      UNSPECIFIED = NULL\n"
            "      TOTAL = [ scale = 3 format = \"%u\" ]\n"
            "      COUNT = NULL\n"
            "      MIN = NULL\n"
            "      MAX = NULL\n"
            "      AVG = [ scale = 2 format = \"%x\" ]\n"
            "      RATE = NULL\n"
            "      RATE_COUNT = NULL\n"
            "    ]\n";

        bsl::string printVal(printBuf.str());
        bsl::string operVal(operBuf.str());

        if (veryVerbose) {
            P_(printVal); P_(operVal);
        }
        ASSERT(EXP_1 == operVal);
        ASSERT(EXP_2 == printVal);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR: balm::MetricFormat
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricFormat& operator=(
        //                                 const balm::MetricFormat& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator: "
                          << "balm::MetricFormat" << endl;

        CombinationIterator<Type::Value> uIter(ALL_TYPES, NUM_TYPES, Z);
        CombinationIterator<Type::Value> vIter(ALL_TYPES, NUM_TYPES, Z);

        do {  // while (uIter.next())
            do {  // while (vIter.next())
                Obj u(Z); const Obj& U = u;
                gg(&u, uIter.current());

                Obj v(Z); const Obj& V = v;
                gg(&v, vIter.current());

                Obj w(v); const Obj& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            } while (vIter.next());
        } while (uIter.next());

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        CombinationIterator<Type::Value> iter(ALL_TYPES, NUM_TYPES, Z);

        do {  // while (iter.next())
            Obj u(Z); const Obj& U = u;
            gg(&u, iter.current());

            Obj w(U);  const Obj& W = w;   // control
            u = u;
            ASSERT(W == U);
        } while (iter.next());

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR: balm::MetricFormat
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricFormat(const balm::MetricFormat& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor: "
                          << "balm::MetricFormat" << endl;

        CombinationIterator<Type::Value> iter(ALL_TYPES, NUM_TYPES, Z);

        do {  // while (iter.next())
            Obj w(Z); const Obj& W = w;
            gg(&w, iter.current());
            Obj x(Z); const Obj& X = x;
            gg(&x, iter.current());

            Obj y(X); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        } while (iter.next());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: balm::MetricFormatSpec
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        //   Use different addresses to the same 'balm::MetricDescription'
        //   value to verify comparisons are made by address (rather than by
        //   value).
        //
        // Testing:
        //   operator==(balm::Metricformat&, balm::Metricformat&);
        //   operator!=(balm::Metricformat&, balm::Metricformat&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality: balm::MetricFormat" << endl;

        CombinationIterator<Type::Value> uIter(ALL_TYPES, NUM_TYPES, Z);
        CombinationIterator<Type::Value> vIter(ALL_TYPES, NUM_TYPES, Z);

        do {  // while (uIter.next())
            Obj u(Z); const Obj& U = u;
            gg(&u, uIter.current());
            do {  // while (vIter.next())
                Obj v(Z); const Obj& V = v;
                gg(&v, vIter.current());

                bool isEqual = uIter.position() == vIter.position();
                ASSERT(isEqual  == (U == V));
                ASSERT(!isEqual == (U != V))
            } while (vIter.next());
        } while (uIter.next());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR: gg
        //
        // Concerns:
        //   That the generator function 'gg' correctly populates a
        //   'balm::MetricFormat' based on a vector of publication types.
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'gg' using the set
        //   'S' and the primary accessors.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting generator function 'gg': balm::MetricFormat"
                 << endl;

        CombinationIterator<Type::Value> cIter(ALL_TYPES, NUM_TYPES, Z);

        do {
            Obj mX(Z); const Obj& MX = mX;
            gg(&mX, cIter.current());

            for (int i = 0; i < Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                if (cIter.includesElement(i)) {
                    ASSERT(Spec(i, "%x") == *MX.formatSpec(type));
                }
                else {
                    ASSERT(0 == MX.formatSpec(type));
                }
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
        } while (cIter.next());
      } break;

      case 10: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //    balm::MetricFormatSpec
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        // balm::MetricFormat(bslma::Allocator *basicAllocator);
        // ~balm::MetricFormat();
        // void setFormatSpec(balm::PublicationType::Value  publicationType,
        //                    const balm::MetricFormatSpec& formatSpec);
        // MetricFormatSpec *formatSpec(balm::PublicationType::Value);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting primary manipulators and accessors (bootstrap):"
                 << " balm::MetricFormat" << endl;
        {
            if (veryVerbose) cout << "\tTest default constructor" << endl;

            Obj mX(Z); const Obj& MX = mX;
            for (int i = 0; i < Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                ASSERT(0 == MX.formatSpec(type));
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            if (veryVerbose)
                cout << "\tTesting primary manipulators and accessors "
                     << "(bootstrap)" << endl;

            CombinationIterator<Type::Value> cIter(ALL_TYPES, NUM_TYPES, Z);

            do {
                Obj mX(Z); const Obj& MX = mX;

                // Add the format spec's based on the combination of types.
                const bsl::vector<Type::Value>& types = cIter.current();
                bsl::vector<Type::Value>::const_iterator tIt = types.begin();
                for ( ; tIt != types.end(); ++tIt) {
                    mX.setFormatSpec(*tIt, Spec((int)*tIt, "%x"));
                }

                // Verify the format spec has been added.
                for (int i = 0; i < Type::k_LENGTH; ++i) {
                    Type::Value type = (Type::Value)i;
                    if (cIter.includesElement(i)) {
                        ASSERT(Spec(i, "%x") == *MX.formatSpec(type));
                    }
                    else {
                        ASSERT(0 == MX.formatSpec(type));
                    }
                }

                ASSERT(0 == defaultAllocator.numBytesInUse());
            } while (cIter.next());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: formatValue [balm::MetricFormatSpec]
        //
        // Concerns:
        //   The format value method formats data as expected.  Note that this
        //   method delegates to printf.
        //
        // Plan:
        //   For each of a small set of possible input values test that
        //   'formatValue' correctly formats the input value:
        //
        // Testing:
        //   static bsl::ostream& formatValue(bsl::ostream&                ,
        //                                    double ,
        //                                   const balm::MetricFormatSpec& );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting class method: formatValue "
                          << "[balm::MetricFormatSpec]" << endl;

        struct {
            float       d_scale;
            const char *d_spec;
            double      d_value;
            const char *d_expected;
        } VALUES [] = {
            {  0.0, ""     , 1.5, ""        },
            {  1.0, "%.4f" , 1.5, "1.5000"  },
            {  1.0, "%.0f" , 2.0, "2"       },
            {  1.0, "%.0f" , 1.1, "1"       },
            {  1.0, "%.0f" , 1.5, "2"       },
            {  1.0, "%.0f" , 1.7, "2"       },
            {  1.0, "%.0f" , 3.0, "3"       },
            {  2.0, "%.0f" , 3.0, "6"       },
            {  2.0, "%.1f" , 1.1, "2.2"     }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec mX(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& X = mX;
            bsl::ostringstream ostrm;
            Spec::formatValue(ostrm, VALUES[i].d_value, X);

            bsl::string result(ostrm.str());
            if (veryVerbose) {
                P_(result); P(VALUES[i].d_expected);
            }

            ASSERT(result == VALUES[i].d_expected);

        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:  balm::MetricFormatSpec
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const balm::MetricFormatSpec&);
        //   ostream& print(ostream&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting output operators: "
                          << "balm::MetricFormatSpec" << endl;

        struct {
             float       d_scale;
             const char *d_spec;
             const char *d_expected;
        } VALUES [] = {
            {  0.0, ""      , "[ scale = 0 format = \"\" ]"      },
            {  1.0, "%u"    , "[ scale = 1 format = \"%u\" ]"    },
            {  5.0, "%x"    , "[ scale = 5 format = \"%x\" ]"    },
            {  0.0, "%1.1f" , "[ scale = 0 format = \"%1.1f\" ]" }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec mX(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& MX = mX;

            bsl::ostringstream printBuf, operBuf;
            MX.print(printBuf);
            operBuf << MX;

            bsl::string EXP(VALUES[i].d_expected);
            bsl::string printVal(printBuf.str());
            bsl::string operVal(operBuf.str());

            if (veryVerbose) {
                P_(printVal); P_(operVal); P(EXP);
            }
            LOOP_ASSERT(i, EXP == printVal);
            LOOP_ASSERT(i, EXP == operVal);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR: balm::MetricFormatSpec
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricFormatSpec& operator=(
        //                                 const balm::MetricFormatSpec& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator: "
                          << "balm::MetricFormatSpec" << endl;

        struct {
             float       d_scale;
             const char *d_spec;
        } VALUES [] = {
            {  0.0, ""      },
            {  0.0, "%d"    },
            {  1.0, "%u"    },
            {  2.0, "%d"    },
            {  5.0, "%x"    },
            {  5.0, "%d"    },
            {  0.0, "%1.1f" },
            { 11.0, "%4.2f" },
            { 11.0, "%u"    },
            { 12.0, "%1.0f" },
            { -1.0, "%o"    },
            { -9.0, "%X"    },
            {  5.0, ""      },
            { 12.0, "%d"    }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                Spec u(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& U = u;
                Spec v(VALUES[j].d_scale, VALUES[j].d_spec); const Spec& V = v;
                Spec w(v); const Spec& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec u(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& U = u;
            Spec w(U);  const Spec& W = w;   // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR: balm::MetricFormatSpec
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricFormatSpec(const balm::MetricFormatSpec& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor: "
                          << "balm::MetricFormatSpec" << endl;

        struct {
             float       d_scale;
             const char *d_spec;
        } VALUES [] = {
            {  0.0, ""      },
            {  0.0, "%d"    },
            {  1.0, "%u"    },
            {  2.0, "%d"    },
            {  5.0, "%x"    },
            {  5.0, "%d"    },
            {  0.0, "%1.1f" },
            { 11.0, "%4.2f" },
            { 11.0, "%u"    },
            { 12.0, "%1.0f" },
            { -1.0, "%o"    },
            { -9.0, "%X"    },
            {  5.0, ""      },
            { 12.0, "%d"    }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec w(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& W = w;
            Spec x(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& X = x;

            Spec y(X); const Spec& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: balm::MetricFormatSpec
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        //   Use different addresses to the same 'balm::MetricDescription'
        //   value to verify comparisons are made by address (rather than by
        //   value).
        //
        // Testing:
        //   operator==(balm::Metricformat&, balm::Metricformat&);
        //   operator!=(balm::Metricformat&, balm::Metricformat&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality: balm::MetricFormatSpec"
                          << endl;
        struct {
             float       d_scale;
             const char *d_spec;
        } VALUES [] = {
            {  0.0, ""      },
            {  0.0, "%d"    },
            {  1.0, "%u"    },
            {  2.0, "%d"    },
            {  5.0, "%x"    },
            {  5.0, "%d"    },
            {  0.0, "%1.1f" },
            { 11.0, "%4.2f" },
            { 11.0, "%u"    },
            { 12.0, "%1.0f" },
            { -1.0, "%o"    },
            { -9.0, "%X"    },
            {  5.0, ""      },
            { 12.0, "%d"    }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec u(VALUES[i].d_scale, VALUES[i].d_spec); const Spec& U = u;
            for (int j = 0; j < NUM_VALUES; ++j) {
                Spec v(VALUES[j].d_scale, VALUES[j].d_spec); const Spec& V = v;

                bool isEqual = i == j;
                ASSERT(isEqual  == (U == V));
                ASSERT(!isEqual == (U != V))
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS:  balm::MetricFormatSpec
        //
        // Concerns:
        //   Test the two varieties of constructors
        //
        // Plan:
        //   Verify the default constructor.
        //
        //   Next, for a sequence of independent test values, use the
        //   alternative constructor to create and object with a specific value
        //   and verify the values using a basic accessor.
        //
        // Testing:
        //   balm::MetricFormatSpec(double, const char *)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting constructors: balm::MetricFormatSpec"
                          << endl;
        struct {
             float       d_scale;
             const char *d_spec;
        } VALUES [] = {
            {  0.0, ""      },
            {  0.0, "%d"    },
            {  1.0, "%u"    },
            {  2.0, "%d"    },
            {  5.0, "%x"    },
            {  5.0, "%d"    },
            {  0.0, "%1.1f" },
            { 11.0, "%4.2f" },
            { 11.0, "%u"    },
            { 12.0, "%1.0f" },
            { -1.0, "%o"    },
            { -9.0, "%X"    },
            {  5.0, ""      },
            { 12.0, "%d"    }
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Spec mX(VALUES[i].d_scale, VALUES[i].d_spec);
            const Spec& MX = mX;

            ASSERT(VALUES[i].d_scale == MX.scale());
            ASSERT(0 == bsl::strcmp(VALUES[i].d_spec, MX.format()));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //    balm::MetricFormatSpec
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        // balm::MetricFormatSpec();
        // ~balm::MetricFormatSpec();
        // void setScale(float scale);
        // void setFormat(const char *format);
        // float scale() const;
        // const char *format() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting primary manipulators and accessors (bootstrap):"
                 << " balm::MetricFormatSpec" << endl;

        {
            if (veryVerbose) cout << "\tTest default constructor" << endl;

            Spec mX; const Spec& MX = mX;
            ASSERT(1.0 == MX.scale());
            ASSERT(0   == bsl::strcmp("%f", MX.format()));
        }
        {
            if (veryVerbose)
                cout << "\tTesting primary manipulators and accessors "
                     << "(bootstrap)" << endl;

            struct {
                float       d_scale;
                const char *d_spec;
            } VALUES [] = {
                {  0.0, ""      },
                {  0.0, "%d"    },
                {  1.0, "%u"    },
                {  2.0, "%d"    },
                {  5.0, "%x"    },
                {  5.0, "%d"    },
                {  0.0, "%1.1f" },
                { 11.0, "%4.2f" },
                { 11.0, "%u"    },
                { 12.0, "%1.0f" },
                { -1.0, "%o"    },
                { -9.0, "%X"    },
                {  5.0, ""      },
                { 12.0, "%d"    }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Spec mX; const Spec& MX = mX;

                mX.setScale(VALUES[i].d_scale);
                mX.setFormat(VALUES[i].d_spec);

                ASSERT(VALUES[i].d_scale == MX.scale());
                ASSERT(0 == bsl::strcmp(VALUES[i].d_spec, MX.format()));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPERS: CombinationsIterator
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout  << endl
            << "TEST HELPERS: CombinationIterator\n"
            << "=================================\n";

        {
            if (veryVerbose)
                cout << "\tTesting CombinationIterator\n";

            const char *VALUES ="1234";
            const char *COM_0  = "";
            const char *COM_1  = "1";
            const char *COM_2  = "2";
            const char *COM_3  = "12";
            const char *COM_4  = "3";
            const char *COM_5  = "13";
            const char *COM_6  = "23";
            const char *COM_7  = "123";
            const char *COM_8  = "4";
            const char *COM_9  = "14";
            const char *COM_10 = "24";
            const char *COM_11 = "124";
            const char *COM_12 = "34";
            const char *COM_13 = "134";
            const char *COM_14 = "234";
            const char *COM_15 = "1234";
            const char *COMBINATIONS[] = {
                COM_0, COM_1, COM_2, COM_3, COM_4, COM_5, COM_6, COM_7, COM_8,
                COM_9, COM_10, COM_11, COM_12, COM_13, COM_14, COM_15 };

            bsl::vector<char> values(VALUES, VALUES + bsl::strlen(VALUES), Z);
            CombinationIterator<char> iter(values, Z);
            ASSERT(0 == defaultAllocator.numBytesInUse());
            int i = 0;
            do {
                bsl::string result1(Z), result2(Z);

                const bsl::vector<char>& combination = iter.current();
                for (int j = 0; j < combination.size();++j) {
                    result1 += combination[j];
                }

                for (int j = 0; j < bsl::strlen(VALUES); ++j) {
                    if (iter.includesElement(j)) {
                        result2 += VALUES[j];
                    }
                }
                if (veryVeryVerbose) {
                    P_(COMBINATIONS[i]); P_(result1); P(result2);
                }
                ASSERT(result1 == COMBINATIONS[i]);
                ASSERT(result2 == COMBINATIONS[i]);
                ++i;
            } while (iter.next());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1) Create an object x1.                       x1:
        //    2) Create a second object x2 (copy from x1).  x1: x2:
        //    3) Append an element value A to x1.           x1:A x2:
        //    4) Append the same element value A to x2.     x1:A x2:A
        //    5) Append another element value B to x2.      x1:A x2:AB
        //    6) Remove all elements from x1.               x1: x2:AB
        //    7) Create a third object x3.                  x1: x2:AB x3:
        //    8) Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9) Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10) Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11) Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            if (veryVerbose) {
                cout << "\tTesting 'balm::MetricFormatSpec'" << endl;
            }
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
            Spec mX1;  const Spec& X1 = mX1;
            mX1.setScale(5.0);
            mX1.setFormat("%u");

            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta: Check initial state of x1." << endl;
            ASSERT(5.0 == X1.scale());
            ASSERT(0   == bsl::strcmp(X1.format(), "%u"));

            if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
            Spec mX2(X1);  const Spec& X2 = mX2;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check the initial state of x2." << endl;
            ASSERT(5.0 == X2.scale());
            ASSERT(0   == bsl::strcmp(X2.format(), "%u"));

            if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                              << endl;

            ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

            mX1.setScale(2.0);
            mX1.setFormat("%d");

            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta. Check new state of x1." << endl;

            ASSERT(2.0 == X1.scale());
            ASSERT(0   == bsl::strcmp(X1.format(), "%d"));

            if (verbose) cout << "\tb. Try equality operators: x1 <op> x1, x2."
                              << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
            ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
            Spec mX3;  const Spec& X3 = mX3;
            if (verbose) { cout << '\t';  P(X3); }

            if (verbose) cout << "\ta. Check initial state of x3." << endl;
            ASSERT(1.0 == X3.scale());
            ASSERT(0   == bsl::strcmp(X3.format(), "%f"));

            if (verbose)
                   cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                        << endl;

            ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
            ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
            ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
            Spec mX4(X3);  const Spec& X4 = mX4;
            if (verbose) { cout << '\t';  P(X4); }

            if (verbose) cout << "\ta. Check initial state of x4." << endl;
            ASSERT(1.0 == X4.scale());
            ASSERT(0   == bsl::strcmp(X4.format(), "%f"));

            if (verbose) cout
                   << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                   << endl;

            ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
            ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
            ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
            ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
            mX3.setScale(3.0);
            mX3.setFormat("%x");

            if (verbose) { cout << '\t';  P(X3); }

            if (verbose) cout << "\ta. Check new state of x3." << endl;
            ASSERT(3.0 == X3.scale());
            ASSERT(0   == bsl::strcmp(X3.format(), "%x"));

            if (verbose) cout
                    << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                    << endl;

            ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
            ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
            ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
            ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
            mX2 = X1;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check new state of x2." << endl;
            ASSERT(2.0 == X2.scale());
            ASSERT(0   == bsl::strcmp(X2.format(), "%d"));

            if (verbose)
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
            ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
            ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
            mX2 = X3;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check new state of x2." << endl;
            ASSERT(3.0 == X2.scale());
            ASSERT(0   == bsl::strcmp(X2.format(), "%x"));

            if (verbose)
                cout <<"\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
            ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
            ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

            mX1 = X1;
            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta. Check new state of x1." << endl;
            ASSERT(2.0 == X1.scale());
            ASSERT(0   == bsl::strcmp(X1.format(), "%d"));

            if (verbose)
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
            ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
            ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
            ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));
        }

        {
            if (veryVerbose) {
                cout << "\tTesting 'balm::MetricFormatSpec'" << endl;
            }
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
            Obj mX1;  const Obj& X1 = mX1;
            mX1.setFormatSpec(Type::e_AVG, Spec(2.0, "%x"));
            mX1.setFormatSpec(Type::e_TOTAL, Spec(3.0, "%u"));

            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta. Check initial state of x1." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_AVG:
                    ASSERT(Spec(2.0, "%x") == *X1.formatSpec(type));
                    break;
                  case Type::e_TOTAL:
                    ASSERT(Spec(3.0, "%u") == *X1.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X1.formatSpec(type));
                }
            }
            if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
            Obj mX2(X1);  const Obj& X2 = mX2;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check the initial state of x2." << endl;
            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_AVG:
                    ASSERT(Spec(2.0, "%x") == *X2.formatSpec(type));
                    break;
                  case Type::e_TOTAL:
                    ASSERT(Spec(3.0, "%u") == *X2.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X2.formatSpec(type));
                }
            }

            if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                              << endl;

            ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

            mX1.setFormatSpec(Type::e_MAX, Spec(5.0, "%f"));
            mX1.clearFormatSpec(Type::e_TOTAL);

            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta. Check new state of x1." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_AVG:
                    ASSERT(Spec(2.0, "%x") == *X1.formatSpec(type));
                    break;
                  case Type::e_MAX:
                    ASSERT(Spec(5.0, "%f") == *X1.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X1.formatSpec(type));
                }
            }

            if (verbose) cout << "\tb. Try equality operators: x1 <op> x1, x2."
                              << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
            ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
            Obj mX3;  const Obj& X3 = mX3;
            if (verbose) { cout << '\t';  P(X3); }

            if (verbose) cout << "\ta. Check initial state of x3." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                ASSERT(0 == X3.formatSpec(type));
            }

            if (verbose)
                   cout << "\tb. Try equality operators: x3 <op> x1, x2, x3."
                        << endl;

            ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
            ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
            ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
            Obj mX4(X3);  const Obj& X4 = mX4;
            if (verbose) { cout << '\t';  P(X4); }

            if (verbose) cout << "\ta. Check initial state of x4." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                ASSERT(0 == X4.formatSpec(type));
            }

            if (verbose) cout
                   << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4."
                   << endl;

            ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
            ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
            ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
            ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;

            mX3.setFormatSpec(Type::e_MIN, Spec(-1.0, "%x"));

            if (verbose) { cout << '\t';  P(X3); }

            if (verbose) cout << "\ta. Check new state of x3." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_MIN:
                    ASSERT(Spec(-1.0, "%x") == *X3.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X3.formatSpec(type));
                }
            }

            if (verbose) cout
                    << "\tb. Try equality operators: x3 <op> x1, x2, x3, x4."
                    << endl;

            ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
            ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
            ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
            ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
            mX2 = X1;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check new state of x2." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_AVG:
                    ASSERT(Spec(2.0, "%x") == *X2.formatSpec(type));
                    break;
                  case Type::e_MAX:
                    ASSERT(Spec(5.0, "%f") == *X2.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X2.formatSpec(type));
                }
            }

            if (verbose)
                cout << "\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
            ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
            ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
            mX2 = X3;
            if (verbose) { cout << '\t';  P(X2); }

            if (verbose) cout << "\ta. Check new state of x2." << endl;

            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_MIN:
                    ASSERT(Spec(-1.0, "%x") == *X2.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X2.formatSpec(type));
                }
            }

            if (verbose)
                cout <<"\tb. Try equality operators: x2 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
            ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
            ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
            ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

            mX1 = X1;
            if (verbose) { cout << '\t';  P(X1); }

            if (verbose) cout << "\ta. Check new state of x1." << endl;
            for (int i = 0; i < (int)Type::k_LENGTH; ++i) {
                Type::Value type = (Type::Value)i;
                switch (type) {
                  case Type::e_AVG:
                    ASSERT(Spec(2.0, "%x") == *X1.formatSpec(type));
                    break;
                  case Type::e_MAX:
                    ASSERT(Spec(5.0, "%f") == *X1.formatSpec(type));
                    break;
                  default:
                    ASSERT(0 == X1.formatSpec(type));
                }
            }

            if (verbose)
                cout << "\tb. Try equality operators: x1 <op> x1, x2, x3, x4."
                     << endl;
            ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
            ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
            ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
            ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
