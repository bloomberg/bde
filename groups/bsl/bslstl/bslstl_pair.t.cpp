// bslstl_pair.t.cpp                                                  -*-C++-*-

#include <bslstl_pair.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslma_usesbslmaallocator.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_issame.h>

#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Because 'bsl::pair' is a simple struct containing two data members, there
// are no real boundary conditions to test.  The main issue to be tested is
// that the four variants of the class have working constructors.  The class
// behaves differently when instantiated with types that use
// 'bslma::Allocator'.  Testing consists mostly of testing all constructors
// with every combination of allocator-using/non-allocator-using template
// parameters.  The other combinatorial issue is the propagation of type traits
// from template parameters to 'bsl::pair' specializations.  Since this is a
// compile-time computation, it is only necessary to instantiate a
// representative combination of traits.
//
// The test driver is rounded out by invoking each comparison operator for
// each possible result to ensure that they work correctly.  There is no need
// to do more exhaustive tests on the relationship operators because they are
// simple pass-through operations and the only thing we are protecting against
// are typos.
//-----------------------------------------------------------------------------
// [2] typedef T1 first_type;
// [2] typedef T2 second_type;
// [2] T1 first;
// [2] T1 second;
// [2] pair();
// [2] pair(bslma::Allocator *alloc);
// [2] pair(const T1& a, const T2& b);
// [2] pair(const T1& a, const T2& b, bslma::Allocator *alloc);
// [2] pair(const pair& rhs);
// [2] pair(const pair& rhs, bslma::Allocator *alloc);
// [2] ~pair();
// [2] pair& operator=(const pair& rhs);
// [2] bool operator==(const pair& x, const pair& y);
// [2] bool operator!=(const pair& x, const pair& y);
// [2] bool operator<(const pair& x, const pair& y);
// [2] bool operator>(const pair& x, const pair& y);
// [2] bool operator<=(const pair& x, const pair& y);
// [2] bool operator>=(const pair& x, const pair& y);
// [3] Type Traits
// [4] template <typename U1, typename U2>
//     pair(const pair<U1, U2>& rhs);
// [4] template <typename U1, typename U2>
//     pair(const pair<U1, U2>& rhs, bslma::Allocator *alloc);
// [5] void pair::swap(pair& rhs);
// [5] void swap(pair& lhs, pair& rhs);
// [7] Pointer to member test
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {
    // Namespace, because of the error on AIX/xlC:
    // "Static declarations are not considered for a function call if the
    // function is not qualified."

void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) std::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ std::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
// static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                // ===========================================
                // class my_String (supplied by Usage example)
                // ===========================================

///Usage
///-----
// A 'bsl::pair' is a very simple object when used without allocators.  Our
// usage example concentrates on the use of allocators with 'bsl::pair'.
// First, we create a utility function that copies a null-terminated string
// into memory allocated from a supplied allocator:
//..
    char *myStrDup(const char *s, bslma::Allocator *alloc)
        // Copy the specified null-terminated string 's' into memory allocated
        // from 'alloc'
    {
        char *result = (char*) alloc->allocate(std::strlen(s) + 1);
        return std::strcpy(result, s);
    }
//..
// We create a simple string class that holds strings allocated from a
// supplied allocator.  It uses 'myStrDup' (above) in its implementation:
//..
    class my_String {
        // Simple string class that uses a 'bslma::Allocator' allocator.

        bslma::Allocator *d_allocator_p;
        char             *d_data;

      public:
        BSLMF_NESTED_TRAIT_DECLARATION(my_String, bslma::UsesBslmaAllocator);

        explicit my_String(bslma::Allocator *alloc = 0);
            // Construct an empty string using the optionally-specified
            // allocator 'alloc'.

        my_String(const char* s, bslma::Allocator *alloc = 0);
            // Construct a string with contents specified in 's' using the
            // optionally-specified allocator 'alloc'.

        my_String(const my_String& rhs, bslma::Allocator *alloc = 0);
            // Construct a copy of the specified 'rhs' string using the
            // optionally-specified allocator 'alloc'.

        ~my_String();
            // Destroy this string.

        my_String& operator=(const my_String& rhs);
            // Copy specified 'rhs' string value to this string.

        const char* c_str() const;
            // Return the null-terminated character array for this string.

        bslma::Allocator *allocator() const;
            // Return the allocator used to construct this string or, if no
            // allocator was specified at construction, the default allocator
            // at the time of construction.
    };

    bool operator==(const my_String& lhs, const my_String& rhs)
    {
        return 0 == std::strcmp(lhs.c_str(), rhs.c_str());
    }

    bool operator==(const my_String& lhs, const char *rhs)
    {
        return 0 == std::strcmp(rhs, lhs.c_str());
    }

    bool operator==(const char *lhs, const my_String& rhs)
    {
        return rhs == lhs;
    }

    bool operator!=(const my_String& lhs, const my_String& rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator!=(const my_String& lhs, const char *rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator!=(const char *lhs, const my_String& rhs)
    {
        return ! (lhs == rhs);
    }

    bool operator<(const my_String& lhs, const my_String& rhs)
    {
        return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
    }

    my_String::my_String(bslma::Allocator *alloc)
    : d_allocator_p(bslma::Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup("", d_allocator_p);
    }

    my_String::my_String(const char *s, bslma::Allocator *alloc)
    : d_allocator_p(bslma::Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup(s, d_allocator_p);
    }

    my_String::my_String(const my_String& rhs, bslma::Allocator *alloc)
    : d_allocator_p(bslma::Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup(rhs.d_data, d_allocator_p);
    }

    my_String::~my_String()
    {
        d_allocator_p->deallocate(d_data);
    }

    my_String& my_String::operator=(const my_String& rhs)
    {
        if (this != &rhs) {
            d_allocator_p->deallocate(d_data);
            d_data = myStrDup(rhs.d_data, d_allocator_p);
        }
        return *this;
    }

    const char *my_String::c_str() const
    {
        return d_data;
    }

    bslma::Allocator *my_String::allocator() const
    {
        return d_allocator_p;
    }
//..
// Our main program creates a mapping from strings to integers.  Each node of
// the mapping consists of a 'bsl::pair<my_String, int>'.  The program
// allocates memory from a test allocator in order to ensure that there are no
// leaks:
//..
    int usageExample()
    {
        typedef bsl::pair<my_String, int> Node;

        Node *mapping[3];
        bslma::TestAllocator alloc;
//..
// When constructing a 'Node', an allocator is supplied in addition to
// parameters for the 'first' and 'second' data members.
//..
        {
            mapping[0] = new(alloc) Node("One", 1, &alloc);
            mapping[1] = new(alloc) Node("Three", 3, &alloc);
            mapping[2] = new(alloc) Node("Two", 2, &alloc);
            // Temporaries get destroyed here, even on broken compilers.
        }

        ASSERT("One" == mapping[0]->first);
        ASSERT(1 == mapping[0]->second);
        ASSERT("Three" == mapping[1]->first);
        ASSERT(3 == mapping[1]->second);
        ASSERT("Two" == mapping[2]->first);
        ASSERT(2 == mapping[2]->second);

        ASSERT(6 == alloc.numBlocksInUse());
//..
// Clean up at end.
//..
        alloc.deleteObjectRaw(mapping[0]);
        alloc.deleteObjectRaw(mapping[1]);
        alloc.deleteObjectRaw(mapping[2]);

        ASSERT(0 == alloc.numBlocksInUse());

        return 0;
    }
//..

                           // ======================
                           // class my_NoAllocString
                           // ======================

class my_NoAllocString
{
    // Another simple string class that does not use a user-supplied
    // allocator.  All memory is allocated from a shared test allocator.  Will
    // compile and run if an attempt is made to use a constructor that takes
    // an allocator argument, but will report an assert failure.

    char *d_data;

  public:
    static bslma::TestAllocator *allocator();
        // Return pointer to singleton test allocator.

    my_NoAllocString();
    my_NoAllocString(const char* s);
    my_NoAllocString(const my_NoAllocString& rhs);
        // Construct a string the normal way.

    explicit my_NoAllocString(bslma::Allocator *alloc);
    my_NoAllocString(const char* s, bslma::Allocator *alloc);
    my_NoAllocString(const my_NoAllocString& rhs, bslma::Allocator *alloc);
        // Attempt to construct a string and specify a user-supplied
        // allocator.  Reports an assert failure and ignores 'alloc', but
        // otherwise compiles and runs.  These functions would be called if
        // bslstl_pair attempted to construct a 'my_NoAllocString'
        // incorrectly.

    ~my_NoAllocString();
        // Destroy this string.

    my_NoAllocString& operator=(const my_NoAllocString& rhs);
        // Copy 'rhs' to this string.

    const char* c_str() const;
        // Return the null-terminated character array for this string.
};

bool operator==(const my_NoAllocString& lhs, const my_NoAllocString& rhs)
{
    return 0 == std::strcmp(lhs.c_str(), rhs.c_str());
}

bool operator==(const my_NoAllocString& lhs, const char *rhs)
{
    return 0 == std::strcmp(rhs, lhs.c_str());
}

bool operator==(const char *lhs, const my_NoAllocString& rhs)
{
    return rhs == lhs;
}

bool operator!=(const my_NoAllocString& lhs, const my_NoAllocString& rhs)
{
    return ! (lhs == rhs);
}

bool operator!=(const my_NoAllocString& lhs, const char *rhs)
{
    return ! (lhs == rhs);
}

bool operator!=(const char *lhs, const my_NoAllocString& rhs)
{
    return ! (rhs == lhs);
}

bool operator<(const my_NoAllocString& lhs, const my_NoAllocString& rhs)
{
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bslma::TestAllocator *my_NoAllocString::allocator()
{
    static bslma::TestAllocator singleton(veryVerbose);
    return &singleton;
}

my_NoAllocString::my_NoAllocString()
: d_data(0)
{
    d_data = myStrDup("", allocator());
}

my_NoAllocString::my_NoAllocString(bslma::Allocator * /*alloc*/)
: d_data(0)
{
    ASSERT("Shouldn't get here" && 0);
    d_data = myStrDup("", allocator());
}

my_NoAllocString::my_NoAllocString(const char *s)
: d_data(0)
{
    d_data = myStrDup(s, allocator());
}

my_NoAllocString::my_NoAllocString(const char *s, bslma::Allocator * /*alloc*/)
: d_data(0)
{
    ASSERT("Shouldn't get here" && 0);
    d_data = myStrDup(s, allocator());
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString& rhs)
: d_data(0)
{
    d_data = myStrDup(rhs.d_data, allocator());
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString&  rhs,
                                   bslma::Allocator         * /*alloc*/)
: d_data(0)
{
    ASSERT("Shouldn't get here" && 0);
    d_data = myStrDup(rhs.d_data, allocator());
}

my_NoAllocString::~my_NoAllocString()
{
    allocator()->deallocate(d_data);
}

my_NoAllocString& my_NoAllocString::operator=(const my_NoAllocString& rhs)
{
    if (this != &rhs) {
        allocator()->deallocate(d_data);
        d_data = myStrDup(rhs.d_data, allocator());
    }
    return *this;
}

const char *my_NoAllocString::c_str() const
{
    return d_data;
}

                           // =====================
                           // struct my_(Traits...)
                           // =====================

struct my_MoveAbandonBslma {};

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseMoveable<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <>
struct UsesBslmaAllocator<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslma
}  // close namespace BloombergLP

struct my_CopyTrivial {};

namespace bsl {
template <>
struct is_trivially_copyable<my_CopyTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_CopyTrivial>
     : bsl::true_type {};
}  // close namespace bsl

struct my_EqualityTrivial {};

namespace bsl {
template <>
struct is_trivially_copyable<my_EqualityTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_EqualityTrivial>
     : bsl::true_type {};
}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseEqualityComparable<my_EqualityTrivial> : bsl::true_type {};
}  // close namespace bslmf
}  // close namespace BloombergLP

struct my_NoTraits {};

//=============================================================================
//                HELPER CLASSES AND FUNCTIONS FOR TESTING SWAP
//-----------------------------------------------------------------------------

                           // ===================
                           // struct TypeWithSwap
                           // ===================

namespace TypeWithSwapNamespace {

    struct TypeWithSwap {
        int data;
        bool swapCalled;

        explicit TypeWithSwap(int d)
        : data(d)
        , swapCalled(false)
        {}

        bool operator==(const TypeWithSwap& rhs) const {
            return data == rhs.data;
        }

        void swap(TypeWithSwap& other) {
            std::swap(data, other.data);

            // set the flag indicating that this function has been called
            other.swapCalled = swapCalled = true;
        }

        void assertSwapCalled() const {
            ASSERT(swapCalled);
        }
    };

    void swap(TypeWithSwap& lhs, TypeWithSwap& rhs) {
        lhs.swap(rhs);
    }
}  // close namespace TypeWithSwapNamespace

                           // ======================
                           // struct TypeWithoutSwap
                           // ======================

struct TypeWithoutSwap {
    int data;

    explicit TypeWithoutSwap(int d)
    : data(d)
    {}

    bool operator==(const TypeWithoutSwap& rhs) const {
        return data == rhs.data;
    }

    void assertSwapCalled() const {
    }
};


template <typename T1, typename T2>
void swapTestHelper()
    // Test 'swap' method and free function for 'bsl::pair<T1, T2>'.
{
    typedef bsl::pair<T1, T2> test_pair;

    // construct two pairs
    test_pair orig_p1(T1(11), T2(12));
    test_pair orig_p2(T1(21), T2(22));

    // copy pairs so that originals remain unchanged
    test_pair p1(orig_p1);
    test_pair p2(orig_p2);

    // swap copies with the free function
    swap(p1, p2);

    // verify that 'swap' worked
    ASSERT(p1 == orig_p2);
    ASSERT(p2 == orig_p1);

    // verify that 'swap' member function was called
    p1.first.assertSwapCalled();
    p1.second.assertSwapCalled();
    p2.first.assertSwapCalled();
    p2.second.assertSwapCalled();

    // restore the original values
    p1 = orig_p1;
    p2 = orig_p2;

    // now use the 'swap' method
    p1.swap(p2);

    // verify that 'swap' worked
    ASSERT(p1 == orig_p2);
    ASSERT(p2 == orig_p1);

    // verify that 'swap' member function was called
    p1.first.assertSwapCalled();
    p1.second.assertSwapCalled();
    p2.first.assertSwapCalled();
    p2.second.assertSwapCalled();
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    // veryVeryVerbose = argc > 4;

    std::setbuf(stdout, 0);    // Use unbuffered output

    std::printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // Pointer to member
        //
        // Concerns:
        // - We can use pointer to members to access both 'first' and 'second'
        //
        // Plan
        // - Create pointer to member to both 'first' and 'second' and check
        //   that the behavior is as expected
        //
        // Testing:
        //     Usage Example
        // --------------------------------------------------------------------
        if (verbose) std::printf("\nPOINTER TO MEMBER TEST"
                                 "\n======================\n");

        int bsl::pair<int,const char*>::*pfirst
                                        = &bsl::pair<int,const char*>::first;
        const char* bsl::pair<int,const char*>::*psecond
                                        = &bsl::pair<int,const char*>::second;
        bsl::pair<int,const char*> p(10, "test7");
        ASSERT(p.first == (p.*pfirst));
        ASSERT(p.second == (p.*psecond));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // - The usage example in the header documentation compiles and runs.
        //
        // Plan
        // - Copy the usage example from the header documentation.
        // - Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //     Usage Example
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nUSAGE EXAMPLE"
                                 "\n=============\n");

        usageExample();

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // SWAP TEST
        //
        // Concerns:
        // - 'swap' free function correctly swaps the values of 'pair::first'
        //   and 'pair::second' data fields.
        // - 'swap' member function correctly swaps the values of 'pair::first'
        //   and 'pair::second' data fields.
        // - If there is a 'swap' free functions defined for either of
        //   'pair::first_type' or 'pair::second_type' types, 'pair::swap'
        //   should use it (if the ADL lookup of 'swap' works correctly).
        // - Otherwise 'pair::swap' should use the default 'std::swap'.
        //
        // Plan:
        // - Create two types: one with 'swap' method and free function
        //   (TypeWithSwap), and another without 'swap' (TypeWithoutSwap).
        // - Instantiate 'pair' with TypeWithSwap as T1 and TypeWithoutSwap as
        //   T2, and the other way around.
        // - Test both 'swap' method and free functions on the two
        //   instantiations of 'pair' described above.
        //
        // Testing:
        //     // free function
        //     template <typename T1, typename T2>
        //     void bsl::swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs);
        //
        //     // member function
        //     template <typename T1, typename T2>
        //     void pair<T1, T2>::swap(pair& rhs);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nSWAP TEST"
                                 "\n=========\n");

        swapTestHelper<TypeWithSwapNamespace::TypeWithSwap, TypeWithoutSwap>();
        swapTestHelper<TypeWithoutSwap, TypeWithSwapNamespace::TypeWithSwap>();
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // CONVERSION CONSTRUCTOR TEST
        //
        // Concerns:
        // - Can construct a 'bsl::pair' from a different instantiation of
        //   'pair' where each type is convertible.
        // - If either type uses an allocator, then an allocator can be passed
        //   to the conversion constructor and is used to construct that
        //   member.
        //
        // Plan:
        // - Construct 'pair<int, double>' from 'pair<char, int>'
        // - Construct 'pair<my_String, int>' from
        //             'pair<const char*, int>'.
        // - Construct 'pair<my_String, int>' from
        //             'pair<const char*, int>' using an allocator.
        // - Construct 'pair<my_NoAllocString, my_String>' from
        //             'pair<const char*, const char*>' using an allocator.
        // - When an allocator is used, verify that result has correct
        //   allocator.
        // - When an allocator is used, verify no memory leaks.
        //
        // Testing:
        //     template <typename U1, typename U2>
        //     pair(const pair<U1, U2>& rhs);
        //
        //     template <typename U1, typename U2>
        //     pair(const pair<U1, U2>&  rhs,
        //          bslma::Allocator    *alloc);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nCONVERSION CONSTRUCTOR TEST"
                                 "\n===========================\n");

        bslma::TestAllocator ta1(veryVerbose);
        bslma::TestAllocator ta2(veryVerbose);
        bslma::TestAllocator& ta3 = *my_NoAllocString::allocator();

        bslma::DefaultAllocatorGuard allocGuard(&ta2);

        {
            bsl::pair<char, int> p1(9, 8), &P1 = p1;
            bsl::pair<int, double> p2(P1), &P2 = p2;
            ASSERT(9 == P2.first);
            ASSERT(8 == P2.second);
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        {
            bsl::pair<const char*, int> p1("Hello", 5), &P1 = p1;
            bsl::pair<my_String, int> p2(P1), &P2 = p2;
            ASSERT("Hello" == P2.first);
            ASSERT(5 == P2.second);
            ASSERT(&ta2 == P2.first.allocator());
            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(1 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        {
            bsl::pair<const char*, int> p1("Hello", 5), &P1 = p1;
            bsl::pair<my_String, int> p2(P1, &ta1), &P2 = p2;
            ASSERT("Hello" == P2.first);
            ASSERT(5 == P2.second);
            ASSERT(&ta1 == P2.first.allocator());
            ASSERT(1 <= ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        {
            bsl::pair<const char*, const char*> p1("Hello", "Goodbye"),
                &P1 = p1;
            bsl::pair<my_NoAllocString, my_String> p2(P1, &ta1), &P2 = p2;
            ASSERT("Hello" == P2.first);
            ASSERT("Goodbye" == P2.second);
            ASSERT(&ta3 == P2.first.allocator());
            ASSERT(&ta1 == P2.second.allocator());
            ASSERT(1 <= ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(1 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TRAITS TEST
        //
        // Concerns:
        // - Instantiating a 'pair' on a pair of types produce a type
        //   with type traits correctly derived from those types' traits.
        //
        // Plan:
        // - Instantiate 'pair' with a number of types, each having one
        //   or more of the type traits in which we are interested.
        // - Verify that the resulting instantiation has the following traits
        //   only if and only if both types on which it is instantiated have
        //   those traits:
        //   * bslmf::IsBitwiseMoveable
        //   * bsl::is_trivially_copyable (implies IsBitwiseMoveable)
        //   * bsl::is_trivially_default_constructible
        // - Verify that the resulting instantiation has the trait
        //   'bslma::UsesBslmaAllocator' if and only BOTH types have
        //   this trait.
        //
        // Testing:
        //     Type Traits
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nTRAITS TEST"
                                 "\n===========\n");

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_NoTraits, my_NoTraits>\n");
        }
        typedef bsl::pair<my_NoTraits, my_NoTraits> Pair0;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair0>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair0>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair0>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair0>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair0>::value));
        ASSERT(  (bslmf::IsPair<                          Pair0>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma,"
                        "             my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_MoveAbandonBslma> Pair1;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair1>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair1>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair1>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair1>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair1>::value));
        ASSERT(  (bslmf::IsPair<                          Pair1>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_CopyTrivial> Pair2;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair2>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair2>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair2>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair2>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair2>::value));
        ASSERT(  (bslmf::IsPair<                          Pair2>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_MoveAbandonBslma> Pair3;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair3>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair3>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair3>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair3>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair3>::value));
        ASSERT(  (bslmf::IsPair<                          Pair3>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_CopyTrivial> Pair4;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair4>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair4>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair4>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair4>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair4>::value));
        ASSERT(  (bslmf::IsPair<                          Pair4>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_NoTraits>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_NoTraits> Pair5;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair5>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair5>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair5>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair5>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair5>::value));
        ASSERT(  (bslmf::IsPair<                          Pair5>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_NoTraits> Pair6;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair6>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair6>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair6>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair6>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair6>::value));
        ASSERT(  (bslmf::IsPair<                          Pair6>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_String, my_NoTraits>\n");
        }
        typedef bsl::pair<my_String, my_NoTraits> Pair7;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair7>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair7>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair7>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair7>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair7>::value));
        ASSERT(  (bslmf::IsPair<                          Pair7>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, int>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, int> Pair8;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair8>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair8>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair8>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair8>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair8>::value));
        ASSERT(  (bslmf::IsPair<                          Pair8>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, my_CopyTrivial>\n");
        }
        typedef bsl::pair<int, my_CopyTrivial> Pair9;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair9>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair9>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair9>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair9>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair9>::value));
        ASSERT(  (bslmf::IsPair<                          Pair9>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_String, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_String, my_MoveAbandonBslma> Pair10;
        ASSERT(! (bslmf::IsBitwiseMoveable<               Pair10>::value));
        ASSERT(! (bsl::is_trivially_copyable<             Pair10>::value));
        ASSERT(! (bsl::is_trivially_default_constructible<Pair10>::value));
        ASSERT(  (bslma::UsesBslmaAllocator<              Pair10>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair10>::value));
        ASSERT(  (bslmf::IsPair<                          Pair10>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<char, int>\n");
        }
        typedef bsl::pair<char, int> Pair11;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair11>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair11>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair11>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair11>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair11>::value));
        ASSERT(  (bslmf::IsPair<                          Pair11>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, char>\n");
        }
        typedef bsl::pair<int, char> Pair12;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair12>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair12>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair12>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair12>::value));
        ASSERT(! (bslmf::IsBitwiseEqualityComparable<     Pair12>::value));
        ASSERT(  (bslmf::IsPair<                          Pair12>::value));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, int>\n");
        }
        typedef bsl::pair<int, int> Pair13;
        ASSERT(  (bslmf::IsBitwiseMoveable<               Pair13>::value));
        ASSERT(  (bsl::is_trivially_copyable<             Pair13>::value));
        ASSERT(  (bsl::is_trivially_default_constructible<Pair13>::value));
        ASSERT(! (bslma::UsesBslmaAllocator<              Pair13>::value));
        ASSERT(  (bslmf::IsBitwiseEqualityComparable<     Pair13>::value));
        ASSERT(  (bslmf::IsPair<                          Pair13>::value));

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //
        // Concerns:
        // 1. Can construct a 'pair' using the default constructor,
        //    constructor with two arguments and copy constructor with no
        //    allocator.
        // 2. If and only if one or both members of the 'pair' have
        //    the 'bslma::UsesBslmaAllocator' trait, then the
        //    'pair' also has that trait.
        // 3. If 'pair' has the 'bslma::UsesBslmaAllocator'
        //    trait, then each constructor can be passed a 'bslma::Allocator'
        //    pointer and that pointer is passed through to the member(s) that
        //    take it.
        // 4. Assignment works as designed.
        // 5. Operators ==, !=, <, >, <=, and >= work as designed.
        //
        // Plan:
        // - Select a small set of interesting types:
        //   'short'            - Fundamental type
        //   'my_String'        - Uses 'bslma::Allocator' in constructor
        //   'my_NoAllocString' - Doesn't use 'bslma::Allocator' in constructor
        // - Instantiate 'pair' with each combination (9 total) of the
        //   above types.
        // - For each instantiation, do the following:
        //   * Verify that the 'first_type' and 'second_type' typedefs are the
        //     types specified in the template parameters.
        //   * Set the default allocator to a test allocator
        //   * Construct objects using the three non-allocator constructors.
        //   * Verify that the 'first' and 'second' members are correct values.
        //   * Test the relationship operators
        //   * Test assignment
        //   * Test that the default allocator was used as expected.
        //   * Test that the special 'my_NoAllocString' allocator was used or
        //     not used as expected.
        //   * Verify that all memory is returned to the allocator(s) when the
        //     objects go out of scope.
        // - For instantiations where at least one member is of type
        //   'my_String', also do the following:
        //   * Construct objects using the three constructors with allocators.
        //   * Verify that memory was used from the allocators as expected.
        //   * Verify that no memory was used from the default allocator.
        //   * Test assignment among the new objects
        //
        // Testing:
        //     typedef T1 first_type;
        //     typedef T2 second_type;
        //     T1 first;
        //     T1 second;
        //     pair();
        //     pair(bslma::Allocator *alloc);
        //     pair(const T1& a, const T2& b);
        //     pair(const T1& a, const T2& b, bslma::Allocator *alloc);
        //     pair(const pair& rhs);
        //     pair(const pair& rhs, bslma::Allocator *alloc);
        //     ~pair();
        //     pair& operator=(const pair& rhs);
        //     bool operator==(const pair& x, const pair& y);
        //     bool operator!=(const pair& x, const pair& y);
        //     bool operator<(const pair& x, const pair& y);
        //     bool operator>(const pair& x, const pair& y);
        //     bool operator<=(const pair& x, const pair& y);
        //     bool operator>=(const pair& x, const pair& y);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nFUNCTIONALITY TEST"
                                 "\n==================\n");

        bslma::TestAllocator ta1(veryVerbose);
        bslma::TestAllocator ta2(veryVerbose);
        bslma::TestAllocator& ta3 = *my_NoAllocString::allocator();

        bslma::DefaultAllocatorGuard allocGuard(&ta2);

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        if (verbose) std::printf("testing bsl::pair<short, short>\n");
        {
            typedef bsl::pair<short, short> Obj;
            ASSERT((bsl::is_same<short, Obj::first_type>::value));
            ASSERT((bsl::is_same<short, Obj::second_type>::value));

            const short NULL_FIRST   = 0;
            const short NULL_SECOND  = 0;
            const short VALUE_FIRST  = 3;
            const short VALUE_SECOND = 4;

            ASSERT(  (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(  (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(  (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(! (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }
        // End test bslstl::Pair<short, short>

        if (verbose) std::printf("testing bsl::pair<short, my_String>\n");
        {
            typedef bsl::pair<short, my_String> Obj;
            ASSERT((bsl::is_same<short, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_String, Obj::second_type>::value));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            ASSERT(&ta2 == P1.second.allocator());
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            ASSERT(&ta2 == P2.second.allocator());
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);
            ASSERT(&ta2 == P3.second.allocator());

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            ASSERT(&ta1 == P4.second.allocator());
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            ASSERT(&ta1 == P5.second.allocator());
            Obj p6(p2, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);
            ASSERT(&ta1 == P6.second.allocator());

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End test bsl::pair<short, my_String>

        if (verbose) {
            std::printf("testing bsl::pair<short, my_NoAllocString>\n");
        }
        {
            typedef bsl::pair<short, my_NoAllocString> Obj;
            ASSERT((bsl::is_same<short, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_NoAllocString, Obj::second_type>::value));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(! (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // end test bsl::pair<short, my_NoAllocString>

        if (verbose) std::printf("testing bsl::pair<my_String, short>\n");
        {
            typedef bsl::pair<my_String, short> Obj;
            ASSERT((bsl::is_same<my_String, Obj::first_type>::value));
            ASSERT((bsl::is_same<short, Obj::second_type>::value));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            ASSERT(&ta2 == P1.first.allocator());
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            ASSERT(&ta2 == P2.first.allocator());
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);
            ASSERT(&ta2 == P3.first.allocator());

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            ASSERT(&ta1 == P4.first.allocator());
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            ASSERT(&ta1 == P5.first.allocator());
            Obj p6(p2, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);
            ASSERT(&ta1 == P6.first.allocator());

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bsl::pair<my_String, short>

        if (verbose) {
            std::printf("testing bsl::pair<my_String, my_String>\n");
        }
        {
            typedef bsl::pair<my_String, my_String> Obj;
            ASSERT((bsl::is_same<my_String, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_String, Obj::second_type>::value));

            const char *const NULL_FIRST   = "";
            const char *const NULL_SECOND  = "";
            const char *const VALUE_FIRST  = "Hello";
            const char *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            ASSERT(&ta2 == P1.first.allocator() &&
                   &ta2 == P1.second.allocator());
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            ASSERT(&ta2 == P2.first.allocator() &&
                   &ta2 == P2.second.allocator());
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);
            ASSERT(&ta2 == P3.first.allocator() &&
                   &ta2 == P3.second.allocator());

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(6 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            ASSERT(&ta1 == P4.first.allocator() &&
                   &ta1 == P4.second.allocator());
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            ASSERT(&ta1 == P5.first.allocator() &&
                   &ta1 == P5.second.allocator());
            Obj p6(p2, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);
            ASSERT(&ta1 == P5.first.allocator() &&
                   &ta1 == P5.second.allocator());

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(6 <= ta1.numBlocksInUse());
            ASSERT(6 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bsl::pair<my_String, my_String>

        if (verbose) {
            std::printf("testing bsl::pair<my_String, my_NoAllocString>\n");
        }
        {
            typedef bsl::pair<my_String, my_NoAllocString> Obj;
            ASSERT((bsl::is_same<my_String, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_NoAllocString, Obj::second_type>::value));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            ASSERT(&ta2 == P1.first.allocator());
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            ASSERT(&ta2 == P2.first.allocator());
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);
            ASSERT(&ta2 == P3.first.allocator());

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            ASSERT(&ta1 == P4.first.allocator());
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            ASSERT(&ta1 == P5.first.allocator());
            Obj p6(p2, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);
            ASSERT(&ta1 == P6.first.allocator());

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(6 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bslstl::Pair<my_String, my_NoAllocString>

        if (verbose) {
            std::printf("testing bsl::pair<my_NoAllocString, short>\n");
        }
        {
            typedef bsl::pair<my_NoAllocString, short> Obj;
            ASSERT((bsl::is_same<my_NoAllocString, Obj::first_type>::value));
            ASSERT((bsl::is_same<short, Obj::second_type>::value));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(! (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bsl::pair<my_NoAllocString, short>

        if (verbose) {
            std::printf("testing bsl::pair<my_NoAllocString, my_String>\n");
        }
        {
            typedef bsl::pair<my_NoAllocString, my_String> Obj;
            ASSERT((bsl::is_same<my_NoAllocString, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_String, Obj::second_type>::value));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            ASSERT(&ta2 == P1.second.allocator());
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            ASSERT(&ta2 == P2.second.allocator());
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);
            ASSERT(&ta2 == P3.second.allocator());

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            ASSERT(&ta1 == P4.second.allocator());
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            ASSERT(&ta1 == P5.second.allocator());
            Obj p6(p2, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);
            ASSERT(&ta1 == P6.second.allocator());

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(6 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bsl::pair<my_NoAllocString, my_String>

        if (verbose) {
            std::printf("testing bsl::pair<my_NoAllocString, "
                        "my_NoAllocString>\n");
        }
        {
            typedef bsl::pair<my_NoAllocString, my_NoAllocString> Obj;
            ASSERT((bsl::is_same<my_NoAllocString, Obj::first_type>::value));
            ASSERT((bsl::is_same<my_NoAllocString, Obj::second_type>::value));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(! (bslma::UsesBslmaAllocator<              Obj>::value));

            Obj p1; const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(P2 == P3);
            ASSERT(P1 != P2);
            ASSERT(P1 < P2);
            ASSERT(P2 > P1);
            ASSERT(P1 <= P2);
            ASSERT(P2 >= P1);
            ASSERT(P3 <= P2);
            ASSERT(P2 >= P3);

            ASSERT(! (P1 == P2));
            ASSERT(! (P3 != P2));
            ASSERT(! (P2 < P1));
            ASSERT(! (P1 > P2));
            ASSERT(! (P2 < P3));
            ASSERT(! (P3 > P2));
            ASSERT(! (P2 <= P1));
            ASSERT(! (P1 >= P2));

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 <= ta2.numBlocksInUse());
            ASSERT(6 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bsl::pair<my_NoAllocString, my_NoAllocString>

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // - Can construct a 'pair' with allocator.
        //
        // Plan:
        // - Instantiate 'pair' with a simple string class that uses
        //   'bslma::Allocator'.
        // - Construct a objects using the test allocator.
        // - Verify that object members have the correct value.
        // - Verify that that the correct allocator was used.
        // - Verify that there are no memory leaks.
        //
        // Testing:
        //   Breathing test only.  Exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) std::printf("\nBREATHING TEST"
                                 "\n==============\n");

        bslma::TestAllocator ta1(veryVerbose);
        bslma::TestAllocator ta2(veryVerbose);

        bslma::DefaultAllocatorGuard allocGuard(&ta2);

        {
            typedef bsl::pair<my_String, short> Obj;
            ASSERT((bsl::is_same<my_String, Obj::first_type>::value));
            ASSERT((bsl::is_same<short, Obj::second_type>::value));

            ASSERT(! (bslmf::IsBitwiseMoveable<               Obj>::value));
            ASSERT(! (bsl::is_trivially_copyable<             Obj>::value));
            ASSERT(! (bsl::is_trivially_default_constructible<Obj>::value));
            ASSERT(  (bslma::UsesBslmaAllocator<              Obj>::value));

            const my_String NULL_FIRST("");
            const short     NULL_SECOND  = 0;
            const my_String VALUE_FIRST("Hello");
            const short     VALUE_SECOND = 4;

            Obj p1(&ta1); const Obj& P1 = p1;
            ASSERT(NULL_FIRST == P1.first && NULL_SECOND == P1.second);
            Obj p2(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P2 = p2;
            ASSERT(VALUE_FIRST == P2.first && VALUE_SECOND == P2.second);
            Obj p3(p2, &ta1); const Obj& P3 = p3;
            ASSERT(VALUE_FIRST == P3.first && VALUE_SECOND == P3.second);

            ASSERT(3 == ta1.numBlocksInUse());

            p1 = p2;
            ASSERT(P1 == P2);

            ASSERT(3 == ta1.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());

      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
