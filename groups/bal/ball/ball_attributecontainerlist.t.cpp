// ball_attributecontainerlist.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_attributecontainerlist.h>

#include <ball_attributecontainer.h>
#include <ball_attribute.h>

#include <bdlb_print.h>
#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// ball::AttributeContainerListIterator
// CREATORS
// [ 3] ball::AttributeContainerListIterator();
// [ 5] ball::AttributeContainerListIterator(
//                               const ball::AttributeContainerListIterator& );
// [ 3] ball::AttributeContainerListIterator(
//                                        ball::AttributeContainerList_Node *);
// MANIPULATORS
// [ 6] ball::AttributeContainerListIterator& operator=(
//                              const ball::AttributeContainerListIterator& );
// [ 3] ball::AttributeContainerListIterator operator++(int);
// [ 7] ball::AttributeContainerListIterator operator++();
// ACCESSORS
// [ 3] const ball::AttributeContainer *const *operator->() const;
// [ 3] const ball::AttributeContainer *operator*() const;
// [ 3] bool valid() const;
// FREE OPERATORS
// [ 4] bool operator==(const ball::AttributeContainerListIterator& ,
//                      const ball::AttributeContainerListIterator& );
// [ 4] bool operator!=(const ball::AttributeContainerListIterator& ,
//                      const ball::AttributeContainerListIterator& );
// ball::AttributeContainerList
// CREATORS
// [ 8] ball::AttributeContainerList(bslma::Allocator *);
// [10] ball::AttributeContainerList(const ball::AttributeContainerList&  ,
//                                  bslma::Allocator                   *);
// [ 8] ball::AttributeContainerList();
// MANIPULATORS
// [11] ball::AttributeContainerList& operator=(
//                                       const ball::AttributeContainerList& );
// [ 8] iterator pushFront(const ball::AttributeContainer *);
// [ 8] void remove(const iterator& );
// [14] void removeAll();
// [15] void removeAllAndRelease();
// ACCESSORS
// [ 8] iterator begin() const;
// [ 8] iterator end() const;
// [ 8] int numContainers() const;
// [13] bool hasValue(const ball::Attribute& ) const;
// [12] bsl::ostream& print(bsl::ostream& , int, int) const;
// FREE OPERATORS
// [ 9] bool operator==(const ball::AttributeContainerList& ,
//                      const ball::AttributeContainerList& );
// [ 9] bool operator!=(const ball::AttributeContainerList& ,
//                      const ball::AttributeContainerList& );
// [12] bsl::ostream& operator<<(bsl::ostream&                      ,
//                               const ball::AttributeContainerList& );
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST 1
// [ 2] BREATHING TEST 2
// [  ] free list
// [17] USAGE EXAMPLE

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::AttributeContainerList         Obj;
typedef ball::AttributeContainerListIterator ObjIter;
typedef ball::AttributeContainerList_Node    Node;
int verbose;
int veryVerbose;
int veryVeryVerbose;

bslma::TestAllocator testAllocator;

//=============================================================================
//                          CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                        // ==================
                        // class AttributeSet
                        // ==================

     // attributeset.h

    class AttributeSet : public ball::AttributeContainer {
        // A simple set based implementation of the 'ball::AttributeContainer'
        // protocol used for testing.

        struct AttributeComparator {
            bool operator()(const ball::Attribute& lhs,
                            const ball::Attribute& rhs) const
                // Return 'true' if the specified 'lhs' is ordered before the
                // specified 'rhs'.
            {
                int cmp = bsl::strcmp(lhs.name(), rhs.name());
                if (0 != cmp) {
                    return cmp < 0;                                   // RETURN
                }
                if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
                    return lhs.value().typeIndex() < rhs.value().typeIndex();
                                                                      // RETURN
                }
                switch (lhs.value().typeIndex()) {
                  case 0: // unset
                    return true;
                  case 1: // int
                    return lhs.value().the<int>() < rhs.value().the<int>();
                  case 2: // int64
                    return lhs.value().the<bsls::Types::Int64>()
                        <  rhs.value().the<bsls::Types::Int64>();
                 case 3: // string
                   return lhs.value().the<bsl::string>()
                       <  rhs.value().the<bsl::string>();
                }
                BSLS_ASSERT(false);
                return false;
            }
        };

        // DATA
        bsl::set<ball::Attribute, AttributeComparator> d_set;

        // ...

      public:

        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(AttributeSet,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        explicit
        AttributeSet(bslma::Allocator *basicAllocator = 0);
            // Create this attribute set.

        AttributeSet(const AttributeSet&  original,
                     bslma::Allocator    *basicAllocator = 0);
            // Create an attribute set with the same value as the specified
            // 'original'.

        virtual ~AttributeSet();
            // Destroy this attribute set.

        // MANIPULATORS
        void insert(const ball::Attribute& value);
            // Add the specified 'value' to this attribute set.

        bool remove(const ball::Attribute& value);
            // Remove the specified 'value' from this attribute set, return
            // 'true' if the attribute was found, and 'false' if 'value' was
            // not a member of this set.

        // ACCESSORS
        virtual bool hasValue(const ball::Attribute& value) const;
            // Return 'true' if the attribute having specified 'value' exists
            // in this object, and 'false' otherwise.

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream' at the
            // (absolute value of) the optionally specified indentation 'level'
            // and return a reference to 'stream'.
    };

                        // ------------------
                        // class AttributeSet
                        // ------------------

    // CREATORS
    inline
    AttributeSet::AttributeSet(bslma::Allocator *basicAllocator)
    : d_set(AttributeComparator(), basicAllocator)
    {
    }

    inline
    AttributeSet::AttributeSet(const AttributeSet&  original,
                               bslma::Allocator    *basicAllocator)
    : d_set(original.d_set, basicAllocator)
    {
    }

    // MANIPULATORS
    inline
    void AttributeSet::insert(const ball::Attribute& value)
    {
        d_set.insert(value);
    }

    inline
    bool AttributeSet::remove(const ball::Attribute& value)
    {
        return d_set.erase(value) > 0;
    }

    // attributeset.cpp

    // CREATORS
    AttributeSet::~AttributeSet()
    {
    }

    // ACCESSORS
    bool AttributeSet::hasValue(const ball::Attribute& value) const
    {
        return d_set.find(value) != d_set.end();
    }

    bsl::ostream& AttributeSet::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
    {
        char EL = (spacesPerLevel < 0) ? ' ' : '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[" << EL;

        bsl::set<ball::Attribute>::const_iterator it = d_set.begin();
        for (; it != d_set.end(); ++it) {
            it->print(stream, level+1, spacesPerLevel);
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]" << EL;
        return stream;
    }

                        // ========================
                        // class TestPrintContainer
                        // ========================

class TestPrintContainer : public ball::AttributeContainer {
    // An implementation of 'ball::AttributeContainer' intended to test the
    // 'print' method.

    char d_char;

  public:

    TestPrintContainer() : d_char('X') {}
        // Create a 'TestPrintContainer' that will print 'X' when 'print' is
        // invoked.

    TestPrintContainer(char c) : d_char(c) {}
        // Create a 'TestPrintContainer' that will print the specified 'c'
        // when 'print' is invoked.

    virtual ~TestPrintContainer() {}
        // Destroy this attribute set.

    // MANIPULATORS
    void setChar(char c) { d_char = c; }
        // Set to the character to be printed by this object to the specified
        // 'c'.

    // ACCESSORS
    virtual bool hasValue(const ball::Attribute& value) const {return true;}
        // Return 'true'.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.
};

bsl::ostream& TestPrintContainer::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << d_char << EL;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream;
}

//=============================================================================
//                          FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bool stringDiff(const bsl::string& expectedValue,
                const bsl::string& actualValue)
    // Return 'true' if the specified 'expectedValue' equals the specified
    // 'actualValue', otherwise return 'false' and output to the console
    // information about the lowest index where the two values differ.
{
    const char *exp = expectedValue.c_str();
    const char *act = actualValue.c_str();

    int index = 0;
    for (; *exp && (*exp == *act); ++exp, ++act, ++index) {
    }
    if (*exp == *act) {
        return true;                                                  // RETURN
    }
    bsl::cout << "expcetedValue[" << index << "] = " << (int)*exp
              << " (" << *exp << ")" << bsl::endl
              << "actualValue[" << index << "] = " << (int)*act
              << " (" << *act << ")" << bsl::endl;

    bsl::cout << (int)*exp << "==" << (int)*act << "?\n";
    return false;
}

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'ball::AttributeContainerList' object
// for testing.  They interpret a given 'spec' (from left to right) to
// configure list from a sequence of containers
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <CONTAINER> *
//
// <CONTAINER>  ::= ['0' - '9']   %% the container index, from the sequence of
//                                %% containers to add
//
// Spec String      Description
// -----------      -----------------------------------------------------------
// ""               Has no effect; leaves the object unaltered.
//
// "0"              Appends the container at the 0 index of supplied containers
//
// "420"            Appends the container at index 4, then 2, then 0 from the
//                  supplied sequence of containers
//-----------------------------------------------------------------------------

template <class T>
static Obj& gg(Obj                   *obj,
               const char            *spec,
               const bsl::vector<T>&  containers)
{
    for (const char *c = spec; *c; ++c) {
        int index = *c - '0';
        ASSERT(0 <= index && index < 10);
        obj->pushFront(&containers[index]);
    };
    return *obj;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::Allocator *Z = &testAllocator;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and adjust the line lengths.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

///Usage
///-----
// In the following example we demonstrate how to create a
// 'ball::AttributeContainerList' object, how to add and remove elements from
// the list, and how to walk the list of attribute container addresses.
//
// We start by creating three attribute sets that we will use to populate our
// attribute container list.  Note that this example uses the 'AttributeSet'
// implementation of the 'ball::AttributeContainer' protocol defined in the
// 'ball_attributecontainer' component documentation.
//..
    AttributeSet s1, s2, s3;
    s1.insert(ball::Attribute("Set1", 1));
    s2.insert(ball::Attribute("Set2", 2));
    s3.insert(ball::Attribute("Set3", 3));
//
//..
// We can now create our 'ball::AttributeContainerList' and add the three
// attribute container addresses to the list:
//..
    ball::AttributeContainerList exampleList;
    ball::AttributeContainerList::iterator s1Iter = exampleList.pushFront(&s1);
    ball::AttributeContainerList::iterator s2Iter = exampleList.pushFront(&s2);
    ball::AttributeContainerList::iterator s3Iter = exampleList.pushFront(&s3);
//..
// We can use the 'hasValue()' operation to test which attribute value are
// contained within the list of containers:
//..
    ASSERT(exampleList.hasValue(ball::Attribute("Set1", 1)));
    ASSERT(exampleList.hasValue(ball::Attribute("Set2", 2)));
    ASSERT(exampleList.hasValue(ball::Attribute("Set3", 3)));

    ASSERT(!exampleList.hasValue(ball::Attribute("Set1", 2)));
    ASSERT(!exampleList.hasValue(ball::Attribute("Set2", 1)));
    ASSERT(!exampleList.hasValue(ball::Attribute("Set4", 1)));
//..
// Finally, we can use the iterators to walk the list of efficiently remove
// elements from the list:
//..
    exampleList.remove(s3Iter);

    bsl::cout << exampleList << bsl::endl;
//..
// The resulting output will be the following:
//..
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING: free list
        //
        // Concerns:
        //   That memory removed from a container list are added to the
        //   container list's free list.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize a object based on that value.  Remove the elements
        //   using 'remove()' and verify that the allocated memory is not
        //   free, re-add the object and verify new memory is not allocated.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: free list\n"
                          << "==================\n";

        bslma::TestAllocator containerAllocator;
        bsl::vector<AttributeSet> containers(&containerAllocator);
        containers.resize(10);

        const char *VALUES[] = { "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "0123456789",
                                 "01234567890123456789",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };

        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        for (int i = 0; i < NUM_VALUES; ++i) {

            // Create a control object 'W'.
            Obj w(&containerAllocator); const Obj& W = w;
            gg(&w, VALUES[i], containers);

            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj x(Z); const Obj& X = x;
            gg(&x, VALUES[i], containers);

            ASSERT(0 != testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            int bytesInUse = testAllocator.numBytesInUse();

            x.removeAll();

            ASSERT(0          == X.numContainers());
            ASSERT(X.begin()  == X.end());
            ASSERT(bytesInUse == testAllocator.numBytesInUse());
            ASSERT(0          == defaultAllocator.numBytesInUse());

            x = w;

            ASSERT(W          == X);
            ASSERT(bytesInUse == testAllocator.numBytesInUse());
            ASSERT(0          == defaultAllocator.numBytesInUse());

            // remove 3 elements, then remove all the rest.s
            int j = 0;
            while (0 != X.numContainers() && j++ < 3) {
                x.remove(X.begin());
            }
            x.removeAll();

            ASSERT(X.begin()  == X.end());
            ASSERT(bytesInUse == testAllocator.numBytesInUse());
            ASSERT(0          == defaultAllocator.numBytesInUse());

            x = w;

            ASSERT(W          == X);
            ASSERT(bytesInUse == testAllocator.numBytesInUse());
            ASSERT(0          == defaultAllocator.numBytesInUse());

        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING: removeAllAndRelease()
        //
        // Concerns:
        //   'removeAllAndRelease()' removes all the contained elements and
        //   releases the allocated memory.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize a object based on that value and call remove.  Verify
        //   the object is empty and memory has been released.
        //
        // Testing:
        //   void removeAllAndRelease();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: removeAllAndRelease()\n"
                          << "==============================\n";

        bslma::TestAllocator containerAllocator;
        bsl::vector<AttributeSet> containers(&containerAllocator);
        containers.resize(10);

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "0123456789",
                                 "01234567890123456789",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj x(Z); const Obj& X = x;
            gg(&x, VALUES[i], containers);

            if (*VALUES[i] != 0) {
                ASSERT(0 != testAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            int bytesInUse = testAllocator.numBytesInUse();

            x.removeAllAndRelease();

            ASSERT(0         == X.numContainers());
            ASSERT(X.begin() == X.end());
            ASSERT(0         == testAllocator.numBytesInUse());
            ASSERT(0         == defaultAllocator.numBytesInUse());

            gg(&x, VALUES[i], containers);
            ASSERT(bsl::strlen(VALUES[i]) == X.numContainers());
            ASSERT(0                      == defaultAllocator.numBytesInUse());
            if (*VALUES[i] != 0) {
                ASSERT(0 != testAllocator.numBytesInUse());
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING: removeAll()
        //
        // Concerns:
        //   'removeAll()' removes all the contained elements but does not
        //   release the allocated memory.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize a object based on that value and call remove.  Verify
        //   the object is empty but no memory has been released.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: removeAll()\n"
                          << "====================\n";

        bslma::TestAllocator containerAllocator;
        bsl::vector<AttributeSet> containers(&containerAllocator);
        containers.resize(10);

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "0123456789",
                                 "01234567890123456789",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj x(Z); const Obj& X = x;
            gg(&x, VALUES[i], containers);

            if (*VALUES[i] != 0) {
                ASSERT(0 != testAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            int bytesInUse = testAllocator.numBytesInUse();

            x.removeAll();

            ASSERT(0          == X.numContainers());
            ASSERT(X.begin()  == X.end());
            ASSERT(bytesInUse == testAllocator.numBytesInUse());
            ASSERT(0          == defaultAllocator.numBytesInUse());

            gg(&x, VALUES[i], containers);
            ASSERT(bsl::strlen(VALUES[i]) == X.numContainers());
            ASSERT(bytesInUse             == testAllocator.numBytesInUse());
            ASSERT(0                      == defaultAllocator.numBytesInUse());

        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING: hasValue()
        //
        // Concerns:
        //   'hasValue()' returns the logical or of calling 'hasValue()' on
        //   each of the containers in the list.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize a object based on that value and test if it contains
        //   the attribute values indicated by the test element.
        //
        // Testing:
        //   bool hasValue(const ball::Attribute& value) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: hasValue()\n"
                          << "====================\n";

        // Create a series of attribute containers, each contain 1 unique
        // attribute of the form ("X", N), where N is the index of the
        // container.
        bsl::vector<AttributeSet> containers(Z);
        for (int i = 0; i < 10; ++i) {
            AttributeSet s;
            s.insert(ball::Attribute("X", i));
            containers.push_back(s);
        }

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "0123456789",
                                 "01234567890123456789",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(Z); const Obj& X = x;
            gg(&x, VALUES[i], containers);

            // Note that each number, N, in the specification should map to a
            // container in the list containing the attribute ("X", N).
            bsl::string spec(VALUES[i]);
            for (int j = 0; j < 10; ++j) {
                // Find if 'j' appears in the specification (and therefore
                // hasValue will return 'true').
                bool hasValue = spec.find('0' + j) != bsl::string::npos;
                LOOP2_ASSERT(i, j,
                             hasValue == X.hasValue(ball::Attribute("X", j)));
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT (<<) OPERATOR:
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   operator<<(ostream&, const balm::MetricRecord&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'print'\n"
                          << "================\n";

        bsl::vector<TestPrintContainer> containers(Z);
        for (int i = 0; i < 10; ++i) {
            containers.push_back(TestPrintContainer('a' + i));
        }

        {
            bsl::ostringstream buffer;
            struct {
                const char *d_spec;
                const char *d_exptdVal;
            } VALUES [] = {
                { "",      "[ ] " } ,
                { "0",     "[  [   a  ] ] " },
                { "10",    "[  [   a  ]  [   b  ] ] "}
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u(Z); const Obj& U = u;
                gg(&u, VALUES[i].d_spec, containers);

                bsl::ostringstream buf1, buf2;
                buf2 << U;
                U.print(buf1, 0, -1);
                LOOP2_ASSERT(i, buf1.str(),
                             VALUES[i].d_exptdVal == buf1.str());
                LOOP2_ASSERT(i, buf2.str(),
                             VALUES[i].d_exptdVal == buf2.str());

                if (veryVerbose) {
                    stringDiff(VALUES[i].d_exptdVal, buf1.str());
                    stringDiff(VALUES[i].d_exptdVal, buf2.str());
                }
            }
        }
        {
            bsl::ostringstream buffer;
            struct {
                const char *d_spec;
                const char *d_exptdVal;
            } VALUES [] = {
                { "",      "   [\n"
                           "   ]\n" } ,
                { "0",     "   [\n"
                           "      [\n"
                           "         a\n"
                           "      ]\n"
                           "   ]\n" } ,
                { "01",    "   [\n"
                           "      [\n"
                           "         b\n"
                           "      ]\n"
                           "      [\n"
                           "         a\n"
                           "      ]\n"
                           "   ]\n" } ,
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u(Z); const Obj& U = u;
                gg(&u, VALUES[i].d_spec, containers);

                bsl::ostringstream buf;
                U.print(buf, 1, 3);
                LOOP2_ASSERT(i, buf.str(), VALUES[i].d_exptdVal == buf.str());

                if (veryVerbose) {
                    stringDiff(VALUES[i].d_exptdVal, buf.str());
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR: ball::AttributeContainerList
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
        //   ball::AttributeContainerList& operator=(
        //                     const ball::AttributeContainerList&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST ASSIGNMENT: iterator\n"
                          << "=========================\n";

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };

        const int NUM_ELEMENTS = 10;
        bsl::vector<AttributeSet> containers;
        containers.resize(NUM_ELEMENTS);

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            for (int j = 0; j < NUM_ELEMENTS; ++j) {
                Obj u(Z); const Obj& U = u;
                gg(&u, VALUES[i], containers);

                Obj v(Z);    const Obj& V = v;
                gg(&v, VALUES[j], containers);
                Obj w(V, Z); const Obj& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            Obj u(Z); const Obj& U = u;
            gg(&u, VALUES[i], containers);

            Obj w(U);    const Obj& W = w;
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
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
        //   ball::AttributeContainerList(const AttributeContainerList& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST COPY C'TOR: iterator\n"
                          << "=========================\n";

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        bsl::vector<AttributeSet> containers(Z);
        containers.resize(10);

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(Z); const Obj& X = x;
            gg(&x, VALUES[i], containers);

            Obj w(Z); const Obj& W = w;
            gg(&w, VALUES[i], containers);

            Obj y(X, Z); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: ball::AttributeContainerList
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
        // Testing:
        //   bool operator==(const ball::AttributeContainerList&,
        //                   const ball::AttributeContainerList&);
        //   bool operator!=(const ball::AttributeContainerList&,
        //                   const ball::AttributeContainerList&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST EQUALITY: ball::AttributeContainerList\n"
                          << "==========================================\n";

        const char *VALUES[] = { "",
                                 "0",
                                 "2",
                                 "9",
                                 "01",
                                 "03",
                                 "05",
                                 "11",
                                 "19",
                                 "010",
                                 "011",
                                 "020",
                                 "022",
                                 "1111111111111111",
                                 "11111111111111111",
                                 "10000000000000001",
                                 "21212121212121212",
                                 "12121212121212121" };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        bsl::vector<AttributeSet> containers(Z);
        containers.resize(10);

        if (verbose) cout << "\nTesting equality." << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(Z); const Obj& U = u;
            gg(&u, VALUES[i], containers);

            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(Z); const Obj& V = v;
                gg(&v, VALUES[j], containers);

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V))
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //                                       ball::AttributeContainerList
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //
        //   ball::AttributeContainerList(bslma::Allocator *);
        //   ~ball::AttributeContainerList();
        //   iterator pushFront(const ball::AttributeContainer *container);
        //   void remove(const iterator& element);
        //   iterator begin() const;
        //   iterator end() const;
        //   int numContainers() const;
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "BASIC MANIPULATORS AND ACCESSORS: bae_AttributeContainerList\n"
            << "============================================================"
            << endl;

        const int NUM_ELEMENTS = 10;
        bsl::vector<AttributeSet> containers(Z);
        bsl::vector<Obj::iterator> iterators(Z);
        containers.resize(NUM_ELEMENTS);

        Obj mX(Z); const Obj& MX = mX;
        ASSERT(0 == MX.numContainers());
        ASSERT(MX.begin() == MX.end());
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            iterators.push_back(mX.pushFront(&containers[i]));
            ASSERT(iterators.back() == MX.begin());
            ASSERT(i + 1 == MX.numContainers());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());

        Obj::iterator it = MX.begin();
        for (int i = NUM_ELEMENTS - 1; i >= 0; ++it, --i) {
            ASSERT(&containers[i] == *it);
        }
        ASSERT(it == MX.end());

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            Obj::iterator *itEl = &iterators[iterators.size()/2];
            mX.remove(*itEl);
            iterators.erase(itEl);
            ASSERT(NUM_ELEMENTS - i -1 == MX.numContainers());
            Obj::iterator it = MX.begin();
            for (int j = MX.numContainers() - 1; j >= 0; --j, ++it) {
                ASSERT(*it == *iterators[j]);
            }
            ASSERT(it == MX.end());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING: operator++(); (post-fix increment)
        //
        // Concerns:
        //   The post-fix increment operator increments the iterator and
        //   returns the previous iterator position.
        //
        // Plan:
        //   For a sequence of independent test values, create a iterator,
        //   increment the iterator using the postfix increment operation and
        //   verify the updated iterator value equals that of an iterator
        //   incremented using the prefix increment operation, also verify the
        //   returned value was the original value of the iterator.
        //
        // Testing:
        //   ball::AttributeContainerListIterator operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST: post-fix increment\n"
                          << "========================\n";

        const int NUM_ELEMENTS = 10;
        bsl::vector<AttributeSet> containers;
        bsl::vector<Node> nodes;
        containers.resize(NUM_ELEMENTS);
        nodes.resize(NUM_ELEMENTS);

        // Construct a linked list of nodes by hand to test the iterator.
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            Node& node = nodes[i];
            node.d_value_p = &containers[i];
            node.d_next_p  = (i < NUM_ELEMENTS - 1) ?
                             node.d_next_p = &nodes[i+1] : 0;
            node.d_prevNextAddr_p = (i > 0) ?
                             &nodes[i - 1].d_next_p : 0;
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            ObjIter u(&nodes[i]); const ObjIter& U = u;
            ObjIter v(&nodes[i]); const ObjIter& V = v;
            ObjIter w(&nodes[i]); const ObjIter& W = w;

            ObjIter x = u++; const ObjIter& X = x;
            ++v;
            ASSERT(W == X);
            ASSERT(V == U);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
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
        //   ball::AttributeContainerListIterator& operator=(
        //                    const ball::AttributeContainerListIterator& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST ASSIGNMENT: iterator\n"
                          << "=========================\n";

        const int NUM_ELEMENTS = 10;
        bsl::vector<AttributeSet> containers;
        bsl::vector<Node> nodes;
        containers.resize(NUM_ELEMENTS);
        nodes.resize(NUM_ELEMENTS);

        // Construct a linked list of nodes by hand to test the iterator.
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            Node& node = nodes[i];
            node.d_value_p = &containers[i];
            node.d_next_p  = (i < NUM_ELEMENTS - 1) ?
                             node.d_next_p = &nodes[i+1] : 0;
            node.d_prevNextAddr_p = (i > 0) ?
                             &nodes[i - 1].d_next_p : 0;
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            for (int j = 0; j < NUM_ELEMENTS; ++j) {
                ObjIter u(&nodes[i]); const ObjIter& U = u;
                ObjIter v(&nodes[j]); const ObjIter& V = v;
                ObjIter w(v); const ObjIter& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            ObjIter u(&nodes[i]); const ObjIter& U = u;
            ObjIter w(U);  const ObjIter& W = w;  // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:  ball::AttributeContainerListIterator
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
        //   ball::AttributeContainerListIterator(const
        //                             ball::AttributeContainerListIterator& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST COPY C'TOR: iterator\n"
                          << "=========================\n";

            const int NUM_ELEMENTS = 10;
            bsl::vector<AttributeSet> containers;
            bsl::vector<Node> nodes;
            containers.resize(NUM_ELEMENTS);
            nodes.resize(NUM_ELEMENTS);

            // Construct a linked list of nodes by hand to test the iterator.
            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                Node& node = nodes[i];
                node.d_value_p = &containers[i];
                node.d_next_p  = (i < NUM_ELEMENTS - 1) ?
                                 node.d_next_p = &nodes[i+1] : 0;
                node.d_prevNextAddr_p = (i > 0) ?
                                 &nodes[i - 1].d_next_p : 0;
            }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            ObjIter x(&nodes[i]); const ObjIter& X = x;
            ObjIter w(&nodes[i]); const ObjIter& W = w;
            ObjIter y(X); const ObjIter& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR: ball::AttributeContainerListIterator
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
        // Testing:
        //   bool operator==(const ball::AttributeContainerListIterator&,
        //                   const ball::AttributeContainerListIterator&);
        //   bool operator!=(const ball::AttributeContainerListIterator&,
        //                   const ball::AttributeContainerListIterator&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST EQUALITY: iterator\n"
                          << "=======================\n";

        const int NUM_ELEMENTS = 10;
        bsl::vector<AttributeSet> containers;
        bsl::vector<Node> nodes;
        containers.resize(NUM_ELEMENTS);
        nodes.resize(NUM_ELEMENTS);

        // Construct a linked list of nodes by hand to test the iterator.
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            Node& node = nodes[i];
            node.d_value_p = &containers[i];
            node.d_next_p  = (i < NUM_ELEMENTS - 1) ?
                             node.d_next_p = &nodes[i+1] : 0;
            node.d_prevNextAddr_p = (i > 0) ?
                                    &nodes[i - 1].d_next_p : 0;
        }

        {
            ObjIter a; ObjIter b;
            ASSERT(a == b);
            ASSERT(!(a != b));
        }

        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            ObjIter u(&nodes[i]); const ObjIter& U = u;
            for (int j = 0; j < NUM_ELEMENTS; ++j) {
                ObjIter v(&nodes[j]); const ObjIter& V = v;

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V))
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //                                 ball::AttributeContainerListIterator
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //
        //   ball::AttributeContainerListIterator()
        //   ball::AttributeContainerListIterator(
        //                           ball::AttributeContainerList_Node *);
        //   ball::AttributeContainerListIterator operator++(int);
        //   const ball::AttributeContainer *operator*() const;
        //   const ball::AttributeContainer *const *operator->() const;
        //   bool isValid();
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "BASIC MANIPULATORS AND ACCESSORS: iterator\n"
            << "=========================================="
            << endl;

        {
            if (veryVerbose) {
                cout << "\tTest default constructor" << endl;
                ObjIter mX; const ObjIter& MX = mX;
                ASSERT(!MX.valid());
            }
            if (veryVerbose) {
                cout << "\tTest basic iterator properties" << endl;
            }

            const int NUM_ELEMENTS = 10;
            bsl::vector<AttributeSet> containers;
            bsl::vector<Node> nodes;
            containers.resize(NUM_ELEMENTS);
            nodes.resize(NUM_ELEMENTS);

            // Construct a linked list of nodes by hand to test the iterator.
            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                Node& node = nodes[i];
                node.d_value_p = &containers[i];
                node.d_next_p  = (i < NUM_ELEMENTS - 1) ?
                                 node.d_next_p = &nodes[i+1] : 0;
                node.d_prevNextAddr_p = (i > 0) ?
                                 &nodes[i - 1].d_next_p : 0;
            }

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                ObjIter it(&nodes[i]);
                for (int j = i; j < NUM_ELEMENTS; ++j) {
                    ASSERT(it.valid());
                    ASSERT(&containers[j] == *it);
                    ObjIter it2 = ++it;
                    if (j < NUM_ELEMENTS - 1) {
                        ASSERT(*it2 == *it);
                    }
                }
                ASSERT(!it.valid());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

          if (verbose) cout << endl
                            << "BREATHING TEST 2" << endl
                            << "================" << endl;

          bslma::TestAllocator t1, t2;
          Obj mX(&t1); const Obj& MX = mX;
          Obj mY(&t2); const Obj& MY = mY;

          const int NUM_VALUES = 10;
          ball::AttributeContainer *containers[NUM_VALUES];

          bsl::vector<Obj::iterator> mXIters, mYIters;
          for (int i = 0; i < 10; ++i) {
              // Note that this is an invalid address.
              containers[i] = (ball::AttributeContainer *)(i + 1);
              mXIters.push_back(mX.pushFront(containers[i]));
              mYIters.push_back(mY.pushFront(containers[i]));

              ASSERT(MX == MY);
              Obj::iterator xI = MX.begin();
              Obj::iterator yI = MY.begin();
              for (int j = i ; j >= 0; --j, ++xI, ++yI) {
                  ASSERT(xI != yI);
                  ASSERT(containers[j] == *xI);
                  ASSERT(containers[j] == *yI);
              }
              ASSERT(i + 1 == MX.numContainers());
              ASSERT(i + 1 == MY.numContainers());
          }

          Obj mZ(mX, Z); const Obj& MZ = mZ;
          ASSERT(NUM_VALUES == MZ.numContainers());
          ASSERT(MZ == MX);
          ASSERT(MY == MX);

          mZ.removeAll();

          mZ = MX;
          mZ = MY;

          for (int i = 0; i < 5; ++i) {
              mX.remove(mXIters[i * 2]);
              mY.remove(mYIters[i * 2]);
              ASSERT(10 - i - 1 == MX.numContainers());
              ASSERT(10 - i - 1 == MY.numContainers());
          }

          Obj::iterator xI = MX.begin();
          Obj::iterator yI = MY.begin();
          for (int i = 4; i >= 0; --i, ++xI, ++yI) {
              ASSERT(containers[i * 2 + 1] == *xI);
              ASSERT(containers[i * 2 + 1] == *yI);
          }

          mX.removeAllAndRelease();
          mY.removeAll();

          ASSERT(0 == MX.numContainers());
          ASSERT(0 == MY.numContainers());
          ASSERT(MX == MY);

          ASSERT(0 == t1.numBytesInUse());
          ASSERT(0 < t2.numBytesInUse());

          AttributeSet a(Z), b(Z), c(Z);
          a.insert(ball::Attribute("A", 1));
          b.insert(ball::Attribute("A", 2));
          c.insert(ball::Attribute("C", 1));
          ASSERT( a.hasValue(ball::Attribute("A", 1)));
          ASSERT(!a.hasValue(ball::Attribute("A", 2)));
          ASSERT(!a.hasValue(ball::Attribute("C", 1)));
          ASSERT(!b.hasValue(ball::Attribute("A", 1)));
          ASSERT( b.hasValue(ball::Attribute("A", 2)));
          ASSERT(!b.hasValue(ball::Attribute("C", 1)));
          ASSERT(!c.hasValue(ball::Attribute("A", 1)));
          ASSERT(!c.hasValue(ball::Attribute("A", 2)));
          ASSERT( c.hasValue(ball::Attribute("C", 1)));

          ASSERT(!MX.hasValue(ball::Attribute("A", 1)));
          ASSERT(!MX.hasValue(ball::Attribute("A", 2)));
          ASSERT(!MX.hasValue(ball::Attribute("C", 1)));

          mX.pushFront(&a);

          ASSERT( MX.hasValue(ball::Attribute("A", 1)));
          ASSERT(!MX.hasValue(ball::Attribute("A", 2)));
          ASSERT(!MX.hasValue(ball::Attribute("C", 1)));

          mX.pushFront(&a);

          ASSERT( MX.hasValue(ball::Attribute("A", 1)));
          ASSERT(!MX.hasValue(ball::Attribute("A", 2)));
          ASSERT(!MX.hasValue(ball::Attribute("C", 1)));

          mX.pushFront(&b);

          ASSERT( MX.hasValue(ball::Attribute("A", 1)));
          ASSERT( MX.hasValue(ball::Attribute("A", 2)));
          ASSERT(!MX.hasValue(ball::Attribute("C", 1)));

          mX.pushFront(&c);

          ASSERT( MX.hasValue(ball::Attribute("A", 1)));
          ASSERT( MX.hasValue(ball::Attribute("A", 2)));
          ASSERT( MX.hasValue(ball::Attribute("C", 1)));

          mX.removeAllAndRelease();

          ASSERT(!MX.hasValue(ball::Attribute("A", 1)));
          ASSERT(!MX.hasValue(ball::Attribute("A", 2)));
          ASSERT(!MX.hasValue(ball::Attribute("C", 1)));

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
        //    1. Create an object x1.                       x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1.           x1:A x2:
        //    4. Append the same element value A to x2.     x1:A x2:A
        //    5. Append another element value B to x2.      x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3.                  x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        AttributeSet a(Z), b(Z), c(Z);
        const AttributeSet *A = &a;
        const AttributeSet *B = &b;
        const AttributeSet *C = &c;

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(Z);  const Obj& X1 = mX1;
        mX1.pushFront(A);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(1 == X1.numContainers());
        ASSERT(A == *X1.begin());
        ASSERT(!X1.end().valid());
        ASSERT(!(++X1.begin()).valid());
        ASSERT(X1.end() == ++X1.begin());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(1 == X2.numContainers());
        ASSERT(A == *X2.begin());
        ASSERT(!X2.end().valid());
        ASSERT(!(++X2.begin()).valid());
        ASSERT(X2.end() == ++X2.begin());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.pushFront(B);

        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(2 == X1.numContainers());
        ASSERT(B == *X1.begin());
        ASSERT(A == *(++X1.begin()));
        ASSERT(!X1.end().valid());
        ASSERT(!(++(++X1.begin())).valid());
        ASSERT(++(++X1.begin()) == X1.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(0 == X3.numContainers());
        ASSERT(!X3.begin().valid());
        ASSERT(!X3.end().valid());
        ASSERT(X3.begin() == X3.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == X4.numContainers());
        ASSERT(!X4.begin().valid());
        ASSERT(!X4.end().valid());
        ASSERT(X4.begin() == X4.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.pushFront(A);
        mX3.pushFront(C);

        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(2 == X3.numContainers());
        ASSERT(C == *X3.begin());
        ASSERT(A == *(++X3.begin()));
        ASSERT(!X3.end().valid());
        ASSERT(!(++(++X3.begin())).valid());
        ASSERT(++(++X3.begin()) == X3.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.numContainers());
        ASSERT(B == *X2.begin());
        ASSERT(A == *(++X2.begin()));
        ASSERT(!X2.end().valid());
        ASSERT(!(++(++X2.begin())).valid());
        ASSERT(++(++X2.begin()) == X2.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.numContainers());
        ASSERT(C == *X2.begin());
        ASSERT(A == *(++X2.begin()));
        ASSERT(!X2.end().valid());
        ASSERT(!(++(++X2.begin())).valid());
        ASSERT(++(++X2.begin()) == X2.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(2 == X1.numContainers());
        ASSERT(B == *X1.begin());
        ASSERT(A == *(++X1.begin()));
        ASSERT(!X1.end().valid());
        ASSERT(!(++(++X1.begin())).valid());
        ASSERT(++(++X1.begin()) == X1.end());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
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

    ASSERT(0 == defaultAllocator.numBytesInUse());
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
