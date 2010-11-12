// bdestl_pair.t.cpp                  -*-C++-*-

#include <bdetst_stlpair.h>

#include <bdema_testallocator.h>
#include <bdema_defaultallocatorguard.h>
#include <bdemf_issame.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Because 'std::pair' is a simple struct containing two data members, there
// are no real boundary conditions to test.  The main issue to be tested is
// that the four variants of the class have working constructors.  The class
// behaves idifferently when instantiated with types that use
// 'bdema_Allocator'.  Testing constists mostly of testing all constructors
// with every combination of allocator-using/non-allocator-using template
// parameters.  The other combinatorial issue is the propagation of type
// traits from template parameters to 'std::pair' specializations.  Since
// this is a compile-time computation, it is only necessary to instantiate
// a representative combination of traits.
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
// [2] pair(bdema_Allocator *alloc);
// [2] pair(const T1& a, const T2& b);
// [2] pair(const T1& a, const T2& b, bdema_Allocator *alloc);
// [2] pair(const std::pair& rhs);
// [2] pair(const std::pair& rhs, bdema_Allocator *alloc);
// [2] ~pair();
// [2] std::pair& operator=(const std::pair& rhs);
// [2] bool operator==(const std::pair& x, const std::pair& y);
// [2] bool operator!=(const std::pair& x, const std::pair& y);
// [2] bool operator<(const std::pair& x, const std::pair& y);
// [2] bool operator>(const std::pair& x, const std::pair& y);
// [2] bool operator<=(const std::pair& x, const std::pair& y);
// [2] bool operator>=(const std::pair& x, const std::pair& y);
// [3] Type Traits
// [4] template <typename U1, typename U2>
//     pair(const std::pair<U1, U2>& rhs);
// [4] template <typename U1, typename U2>
//     pair(const std::pair<U1, U2>& rhs, bdema_Allocator *alloc);
// [5] template <typename T1, typename T2>
//     pair<T1, T2> make_pair(const T1& a, const T2& b);
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

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

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

// Abreviations for long trait names
typedef bdealg_TypeTraitBitwiseMoveable BitwiseMoveable;
typedef bdealg_TypeTraitBitwiseCopyable BitwiseCopyable;
typedef bdealg_TypeTraitHasTrivialDefaultConstructor TrivialConstructor;
typedef bdealg_TypeTraitCanAbandonMemory CanAbandonMemory;
typedef bdealg_TypeTraitUsesBdemaAllocator UsesBdemaAllocator;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// A 'std::pair' is a very simple object when used without allocators.  Our
// usage example concentrates on the use of allocators with 'std::pair'.
// First, we create a utility function that copies a null-terminated string
// into memory allocated from a supplied allocator:
//..
    char *myStrDup(const char *s, bdema_Allocator *alloc)
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
        // Simple string class that uses a 'bdema_Allocator' allocator.

        bdema_Allocator *d_allocator;
        char            *d_data;

      public:
        BDEALG_DECLARE_NESTED_TRAITS(my_String,
                                     bdealg_TypeTraitUsesBdemaAllocator);

        explicit my_String(bdema_Allocator *alloc = 0);
            // Construct an empty string using the optionally-specified
            // allocator 'alloc'.

        my_String(const char* s, bdema_Allocator *alloc = 0);
            // Construct a string with contents specified in 's' using the
            // optionally-specified allocator 'alloc'.

        my_String(const my_String& rhs, bdema_Allocator *alloc = 0);
            // Construct a copy of the specified 'rhs' string using the
            // optionally-specified allocator 'alloc'.

        ~my_String();
            // Destroy this string.

        my_String& operator=(const my_String& rhs);
            // Copy specified 'rhs' string value to this string.

        const char* c_str() const;
            // Return the null-terminated character array for this string.

        bdema_Allocator *allocator() const;
            // Return the allocator used to construct this string or, if no
            // allocator was specified at construction, the default allocator
            // at the time of construction.
    };

    bool operator==(const my_String& str1, const my_String& str2)
    {
        return 0 == std::strcmp(str1.c_str(), str2.c_str());
    }

    bool operator==(const my_String& str, const char *p)
    {
        return 0 == std::strcmp(p, str.c_str());
    }

    bool operator==(const char *p, const my_String& str)
    {
        return str == p;
    }

    bool operator!=(const my_String& str1, const my_String& str2)
    {
        return ! (str1 == str2);
    }

    bool operator!=(const my_String& str, const char *p)
    {
        return ! (str == p);
    }

    bool operator!=(const char *p, const my_String& str)
    {
        return ! (str == p);
    }

    bool operator<(const my_String& str1, const my_String& str2)
    {
        return std::strcmp(str1.c_str(), str2.c_str()) < 0;
    }

    my_String::my_String(bdema_Allocator *alloc)
    : d_allocator(bdema_Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup("", d_allocator);
    }

    my_String::my_String(const char *s, bdema_Allocator *alloc)
    : d_allocator(bdema_Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup(s, d_allocator);
    }

    my_String::my_String(const my_String& rhs, bdema_Allocator *alloc)
    : d_allocator(bdema_Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup(rhs.d_data, d_allocator);
    }

    my_String::~my_String()
    {
        d_allocator->deallocate(d_data);
    }

    my_String& my_String::operator=(const my_String& rhs)
    {
        if (this != &rhs) {
            d_allocator->deallocate(d_data);
            d_data = myStrDup(rhs.d_data, d_allocator);
        }
        return *this;
    }

    const char *my_String::c_str() const
    {
        return d_data;
    }

    bdema_Allocator *my_String::allocator() const
    {
        return d_allocator;
    }
//..
// Our main program creates a mapping from strings to integers.  Each node of
// the mapping consists of a 'std::pair<my_String, int>'.  The program
// allocates memory from a test allocator in order to ensure that there are no
// leaks:
//..
    int usageExample()
    {
        typedef std::pair<my_String, int> Node;

        Node *mapping[3];
        bdema_TestAllocator alloc;
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
        alloc.deleteObject(mapping[0]);
        alloc.deleteObject(mapping[1]);
        alloc.deleteObject(mapping[2]);

        ASSERT(0 == alloc.numBlocksInUse());

        return 0;
    }
//..

class my_NoAllocString
{
    // Another simple string class that does not use a user-supplied
    // allocator.  All memory is allocated from a shared test allocator.  Will
    // compile and run if an attempt is made to use a constructor that takes
    // an allocator argument, but will report an assert failure.

    char *d_data;

  public:
    static bdema_TestAllocator *allocator();
        // Return pointer to singleton test allocator.

    my_NoAllocString();
    my_NoAllocString(const char* s);
    my_NoAllocString(const my_NoAllocString& rhs);
        // Construct an string the normal way.

    explicit my_NoAllocString(bdema_Allocator *alloc);
    my_NoAllocString(const char* s, bdema_Allocator *alloc);
    my_NoAllocString(const my_NoAllocString& rhs, bdema_Allocator *alloc);
        // Attempt to construct a string and specify a user-supplied
        // allocator.  Reports an assert failure and ignores 'alloc', but
        // otherwise compiles and runs.

    ~my_NoAllocString();
        // Destroy this string.

    my_NoAllocString& operator=(const my_NoAllocString& rhs);
        // Copy 'rhs' to this string.

    const char* c_str() const;
        // Return the null-terminated character array for this string.
};

bool operator==(const my_NoAllocString& str1, const my_NoAllocString& str2)
{
    return 0 == std::strcmp(str1.c_str(), str2.c_str());
}

bool operator==(const my_NoAllocString& str, const char *p)
{
    return 0 == std::strcmp(p, str.c_str());
}

bool operator==(const char *p, const my_NoAllocString& str)
{
    return str == p;
}

bool operator!=(const my_NoAllocString& str1, const my_NoAllocString& str2)
{
    return ! (str1 == str2);
}

bool operator!=(const my_NoAllocString& str, const char *p)
{
    return ! (str == p);
}

bool operator!=(const char *p, const my_NoAllocString& str)
{
    return ! (str == p);
}

bool operator<(const my_NoAllocString& str1, const my_NoAllocString& str2)
{
    return std::strcmp(str1.c_str(), str2.c_str()) < 0;
}

bdema_TestAllocator *my_NoAllocString::allocator()
{
    static bdema_TestAllocator singleton(veryVerbose);
    return &singleton;
}

my_NoAllocString::my_NoAllocString()
: d_data(0)
{
    d_data = myStrDup("", allocator());
}

my_NoAllocString::my_NoAllocString(bdema_Allocator *alloc)
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

my_NoAllocString::my_NoAllocString(const char *s, bdema_Allocator *alloc)
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
                                   bdema_Allocator         *alloc)
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


struct traits_MoveAbandonBdema :
    BitwiseMoveable,
    CanAbandonMemory,
    UsesBdemaAllocator
{
};
    
struct traits_CopyTrivial :
    BitwiseCopyable,
    TrivialConstructor
{
};

struct my_MoveAbandonBdema
{
    BDEALG_DECLARE_NESTED_TRAITS(my_MoveAbandonBdema, traits_MoveAbandonBdema);
    my_MoveAbandonBdema() { }
    my_MoveAbandonBdema(const my_MoveAbandonBdema&, bdema_Allocator*) { }
};

struct my_CopyTrivial
{
    BDEALG_DECLARE_NESTED_TRAITS(my_CopyTrivial, traits_CopyTrivial);
};

struct my_NoTraits
{
};

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
        // TESTING make_pair
        //
        // Concerns:
        // - Calling std::make_pair on types with and without allocators 
        //   returns pair of correct type.
        // 
        // Plan:
        // - Call 'std::make_pair' with a number of types, each having one
        //   or more of the type traits in which we are interested.
        // - Setting the default allocator to a test allocator, verify that no
        //   memory is leaked.
        //
        // Testing:
        //     template <typename T1, typename T2>
        //     pair<T1, T2> make_pair(const T1& a, const T2& b);
        // --------------------------------------------------------------------
  
        if (verbose) std::printf("\nTESTING make_pair"
                                 "\n=================\n");

        bdema_TestAllocator ta;
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_NoTraits, my_NoTraits>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_NoTraits v1; my_NoTraits v2;
            typedef std::pair<my_NoTraits, my_NoTraits> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_MoveAbandonBdema, "
                        "my_MoveAbandonBdema>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_MoveAbandonBdema v2;
            typedef std::pair<my_MoveAbandonBdema, my_MoveAbandonBdema> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_CopyTrivial v2;
            typedef std::pair<my_CopyTrivial, my_CopyTrivial> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_CopyTrivial, my_MoveAbandonBdema>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_MoveAbandonBdema v2;
            typedef std::pair<my_CopyTrivial, my_MoveAbandonBdema> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_MoveAbandonBdema, my_CopyTrivial>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_CopyTrivial v2;
            typedef std::pair<my_MoveAbandonBdema, my_CopyTrivial> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_MoveAbandonBdema, my_NoTraits>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; my_NoTraits v2;
            typedef std::pair<my_MoveAbandonBdema, my_NoTraits> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_CopyTrivial v1; my_NoTraits v2;
            typedef std::pair<my_CopyTrivial, my_NoTraits> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_String, my_NoTraits>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_String v1; my_NoTraits v2;
            typedef std::pair<my_String, my_NoTraits> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_MoveAbandonBdema, int>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_MoveAbandonBdema v1; int v2;
            typedef std::pair<my_MoveAbandonBdema, int> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<int, my_CopyTrivial>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            int v1; my_CopyTrivial v2;
            typedef std::pair<int, my_CopyTrivial> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<my_String, my_MoveAbandonBdema>\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            my_String v1; my_MoveAbandonBdema v2;
            typedef std::pair<my_String, my_MoveAbandonBdema> Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) {
            std::printf("Testing make_pair returning "
                        "std::pair<pair<int, int>, pair<int, int> >\n");
        }
        {
            bdema_DefaultAllocatorGuard ag(&ta);
            std::pair<int, int> v1; std::pair<int, int> v2;
            typedef std::pair<std::pair<int, int>, std::pair<int, int> > Obj;
            Obj x = std::make_pair(v1, v2);
        }
        ASSERT(0 == ta.numBlocksInUse());

        typedef std::pair<int, int> IntPair;
        typedef std::pair<const IntPair, IntPair> IntPairPair;
        IntPair y = std::make_pair(2, 3);
        ASSERT(2 == y.first);
        ASSERT(3 == y.second);
        IntPairPair z(y, IntPair());
        ASSERT(2 == z.first.first);
        ASSERT(3 == z.first.second);

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // CONVERSION CONSTRUCTOR TEST
        //
        // Concerns:
        // - Can construct a 'std::pair' from a different instantiation of
        //   'std::pair' where each type is convertible.
        // - If either type uses an allocator, then an allocator can be passed
        //   to the conversion constructor and is used to construct that
        //   member.
        //
        // Plan:
        // - Construct 'std::pair<int, double>' from 'std::pair<char, int>'
        // - Construct 'std::pair<my_String, int>' from
        //   'std::pair<const char*, int>'.
        // - Construct 'std::pair<my_String, int>' from
        //   'std::pair<const char*, int>' using an allocator.
        // - Construct 'std::pair<my_NoAllocString, my_String>' from
        //   'std::pair<const char*, const char*>' using an allocator.
        // - When an allocator is used, verify that result has correct
        //   allocator.
        // - When an allocator is used, verify no memory leaks.
        //
        // Testing:
        //     template <typename U1, typename U2>
        //     std::pair(const std::pair<U1, U2>& rhs);
        //
        //     template <typename U1, typename U2>
        //     std::pair(const std::pair<U1, U2>&  rhs,
        //                 bdema_Allocator            *alloc);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nCONVERSION CONSTRUCTOR TEST"
                                 "\n===========================\n");

        bdema_TestAllocator ta1(veryVerbose);
        bdema_TestAllocator ta2(veryVerbose);
        bdema_TestAllocator& ta3 = *my_NoAllocString::allocator();

        bdema_DefaultAllocatorGuard allocGuard(&ta2);

        {
            std::pair<char, int> p1(9, 8), &P1 = p1;
            std::pair<int, double> p2(P1), &P2 = p2;
            ASSERT(9 == P2.first);
            ASSERT(8 == P2.second);
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        {
            std::pair<const char*, int> p1("Hello", 5), &P1 = p1;
            std::pair<my_String, int> p2(P1), &P2 = p2;
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
            std::pair<const char*, int> p1("Hello", 5), &P1 = p1;
            std::pair<my_String, int> p2(P1, &ta1), &P2 = p2;
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
            std::pair<const char*, const char*> p1("Hello", "Goodbye"),
                &P1 = p1;
            std::pair<my_NoAllocString, my_String> p2(P1, &ta1), &P2 = p2;
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
      } break;;

      case 3: {
        // --------------------------------------------------------------------
        // TRAITS TEST
        //
        // Concerns:
        // - Instantiating a 'std::pair' on a pair of types produce a type
        //   with type traits correctly derived from those types' traits.
        // 
        // Plan:
        // - Instantiate 'std::pair' with a number of types, each having one
        //   or more of the type traits in which we are interested.
        // - Verify that the resulting instantiation has the following traits
        //   only if and only if both types on which it is instantiated have
        //   those traits: 
        //   * bdealg_TypeTraitBitwiseMoveable
        //   * bdealg_TypeTraitBitwiseCopyable (implies BitwiseMoveable)
        //   * bdealg_TypeTraitTrivialConstructor
        //   * bdealg_TypeTraitCanAbandonMemory
        // - Verify that the resulting instantiation has the trait
        //   'bdealg_TypeTraitUsesBdemaAllocator' if and only BOTH types have
        //   this trait.
        //
        // Testing:
        //     Type Traits
        // --------------------------------------------------------------------
  
        if (verbose) std::printf("\nTRAITS TEST"
                                 "\n===========\n");

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_NoTraits, my_NoTraits>\n");
        }
        typedef std::pair<my_NoTraits, my_NoTraits> Pair0;
        ASSERT(! (bdealg_HasTrait<Pair0, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair0, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair0, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair0, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair0, UsesBdemaAllocator>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair0, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair0, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair0, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair0, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair0, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_MoveAbandonBdema, "
                        "my_MoveAbandonBdema>\n");
        }
        typedef std::pair<my_MoveAbandonBdema, my_MoveAbandonBdema> Pair1;
        ASSERT(  (bdealg_HasTrait<Pair1, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair1, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair1, TrivialConstructor>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair1, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair1, UsesBdemaAllocator>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair1, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair1, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair1, TrivialConstructor>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair1, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair1, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        typedef std::pair<my_CopyTrivial, my_CopyTrivial> Pair2;
        ASSERT(  (bdealg_HasTrait<Pair2, BitwiseMoveable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair2, BitwiseCopyable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair2, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair2, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair2, UsesBdemaAllocator>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair2, BitwiseMoveable>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair2, BitwiseCopyable>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair2, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair2, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair2, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_CopyTrivial, my_MoveAbandonBdema>\n");
        }
        typedef std::pair<my_CopyTrivial, my_MoveAbandonBdema> Pair3;
        ASSERT(  (bdealg_HasTrait<Pair3, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair3, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair3, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair3, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair3, UsesBdemaAllocator>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair3, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair3, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair3, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<const Pair3, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<const Pair3, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_MoveAbandonBdema, my_CopyTrivial>\n");
        }
        typedef std::pair<my_MoveAbandonBdema, my_CopyTrivial> Pair4;
        ASSERT(  (bdealg_HasTrait<Pair4, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair4, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair4, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair4, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair4, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_MoveAbandonBdema, my_NoTraits>\n");
        }
        typedef std::pair<my_MoveAbandonBdema, my_NoTraits> Pair5;
        ASSERT(! (bdealg_HasTrait<Pair5, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair5, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair5, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair5, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair5, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        typedef std::pair<my_CopyTrivial, my_NoTraits> Pair6;
        ASSERT(! (bdealg_HasTrait<Pair6, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair6, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair6, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair6, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair6, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_String, my_NoTraits>\n");
        }
        typedef std::pair<my_String, my_NoTraits> Pair7;
        ASSERT(! (bdealg_HasTrait<Pair7, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair7, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair7, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair7, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair7, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_MoveAbandonBdema, int>\n");
        }
        typedef std::pair<my_MoveAbandonBdema, int> Pair8;
        ASSERT(  (bdealg_HasTrait<Pair8, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair8, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair8, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair8, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair8, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<int, my_CopyTrivial>\n");
        }
        typedef std::pair<int, my_CopyTrivial> Pair9;
        ASSERT(  (bdealg_HasTrait<Pair9, BitwiseMoveable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair9, BitwiseCopyable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair9, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair9, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair9, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<my_String, my_MoveAbandonBdema>\n");
        }
        typedef std::pair<my_String, my_MoveAbandonBdema> Pair10;
        ASSERT(! (bdealg_HasTrait<Pair10, BitwiseMoveable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair10, BitwiseCopyable>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair10, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair10, CanAbandonMemory>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair10, UsesBdemaAllocator>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "std::pair<const pair<int, int>, pair<int, int> >\n");
        }
        typedef std::pair<int, int> IntPair;
        typedef std::pair<const IntPair, IntPair> Pair11;
        ASSERT(  (bdealg_HasTrait<Pair11, BitwiseMoveable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair11, BitwiseCopyable>::VALUE));
        ASSERT(  (bdealg_HasTrait<Pair11, TrivialConstructor>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair11, CanAbandonMemory>::VALUE));
        ASSERT(! (bdealg_HasTrait<Pair11, UsesBdemaAllocator>::VALUE));

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //
        // Concerns:
        // 1. Can construct a 'std::pair' using the default constructor,
        //    constructor with two arguments and copy constructor with no
        //    allocator. 
        // 2. If and only if one or both members of the 'std::pair' have
        //    the 'bdealg_TypeTraitUsesBdemaAllocator' trait, then the
        //    'std::pair' also has that trait.
        // 3. If 'std::pair' has the 'bdealg_TypeTraitUsesBdemaAllocator'
        //    trait, then each constructor can be passed a 'bdema_Allocator'
        //    pointer and that pointer is passed through to the member(s) that
        //    take it.
        // 4. Assignment works as designed.
        // 5. Operators ==, !=, <, >, <=, and >= work as designed.
        //
        // Plan:
        // - Select a small set of interesting types:
        //    'short'            - Fundamental type
        //    'my_String'        - Uses 'bdema_Allocator' in constructor
        //    'my_NoAllocString' - Doesn't use 'bdema_Allocator' in constructor
        // - Instantiate 'std::pair' with each combination (9 total) of the
        //   above types.
        // - For each instantiation, do the following:
        //   * Verify that the 'first_type' and 'second_type' typdefs are the
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
        //     pair(bdema_Allocator *alloc);
        //     pair(const T1& a, const T2& b);
        //     pair(const T1& a, const T2& b, bdema_Allocator *alloc);
        //     pair(const std::pair& rhs);
        //     pair(const std::pair& rhs, bdema_Allocator *alloc);
        //     ~pair();
        //     std::pair& operator=(const std::pair& rhs);
        //     bool operator==(const std::pair& x, const std::pair& y);
        //     bool operator!=(const std::pair& x, const std::pair& y);
        //     bool operator<(const std::pair& x, const std::pair& y);
        //     bool operator>(const std::pair& x, const std::pair& y);
        //     bool operator<=(const std::pair& x, const std::pair& y);
        //     bool operator>=(const std::pair& x, const std::pair& y);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nFUNCTIONALITY TEST"
                                 "\n==================\n");

        bdema_TestAllocator ta1(veryVerbose);
        bdema_TestAllocator ta2(veryVerbose);
        bdema_TestAllocator& ta3 = *my_NoAllocString::allocator();

        bdema_DefaultAllocatorGuard allocGuard(&ta2);

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        if (verbose) std::printf("testing std::pair<short, short>\n");
        {
            typedef std::pair<short, short> Obj;
            ASSERT((bdemf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<short, Obj::second_type>::VALUE));

            const short NULL_FIRST   = 0;
            const short NULL_SECOND  = 0;
            const short VALUE_FIRST  = 3;
            const short VALUE_SECOND = 4;

            ASSERT(  (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
        // End test bdest_Pair<short, short>

        if (verbose) std::printf("testing std::pair<short, my_String>\n");
        {
            typedef std::pair<short, my_String> Obj;
            ASSERT((bdemf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_String, Obj::second_type>::VALUE));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            Obj p6(p5, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End test std::pair<short, my_String>

        if (verbose) {
            std::printf("testing std::pair<short, my_NoAllocString>\n");
        }
        {
            typedef std::pair<short, my_NoAllocString> Obj;
            ASSERT((bdemf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
        // end test std::pair<short, my_NoAllocString>

        if (verbose) std::printf("testing std::pair<my_String, short>\n");
        {
            typedef std::pair<my_String, short> Obj;
            ASSERT((bdemf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<short, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            Obj p6(p5, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing std::pair<my_String, short>

        if (verbose) {
            std::printf("testing std::pair<my_String, my_String>\n");
        }
        {
            typedef std::pair<my_String, my_String> Obj;
            ASSERT((bdemf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_String, Obj::second_type>::VALUE));

            const char *const NULL_FIRST   = "";
            const char *const NULL_SECOND  = "";
            const char *const VALUE_FIRST  = "Hello";
            const char *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
            ASSERT(6 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            Obj p6(p5, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(6 <= ta1.numBlocksInUse());
            ASSERT(6 <= ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing std::pair<my_String, my_String>

        if (verbose) {
            std::printf("testing std::pair<my_String, my_NoAllocString>\n");
        }
        {
            typedef std::pair<my_String, my_NoAllocString> Obj;
            ASSERT((bdemf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            Obj p6(p5, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(6 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing bdest_Pair<my_String, my_NoAllocString>

        if (verbose) {
            std::printf("testing std::pair<my_NoAllocString, short>\n");
        }
        {
            typedef std::pair<my_NoAllocString, short> Obj;
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<short, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
        // End testing std::pair<my_NoAllocString, short>

        if (verbose) {
            std::printf("testing std::pair<my_NoAllocString, my_String>\n");
        }
        {
            typedef std::pair<my_NoAllocString, my_String> Obj;
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_String, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(3 <= ta3.numBlocksInUse());

            Obj p4(&ta1); const Obj& P4 = p4;
            ASSERT(NULL_FIRST == P4.first && NULL_SECOND == P4.second);
            Obj p5(VALUE_FIRST, VALUE_SECOND, &ta1); const Obj& P5 = p5;
            ASSERT(VALUE_FIRST == P5.first && VALUE_SECOND == P5.second);
            Obj p6(p5, &ta1); const Obj& P6 = p6;
            ASSERT(VALUE_FIRST == P6.first && VALUE_SECOND == P6.second);

            p4 = p5;
            ASSERT(P4 == P5);

            ASSERT(3 <= ta1.numBlocksInUse());
            ASSERT(3 <= ta2.numBlocksInUse());
            ASSERT(6 <= ta3.numBlocksInUse());
        }

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
        // End testing std::pair<my_NoAllocString, my_String>

        if (verbose) {
            std::printf("testing std::pair<my_NoAllocString, "
                        "my_NoAllocString>\n");
        }
        {
            typedef std::pair<my_NoAllocString, my_NoAllocString> Obj;
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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
        // End testing std::pair<my_NoAllocString, my_NoAllocString>

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // 
        // Concerns:
        // - Can construct a 'std::pair' with allocator.
        //
        // Plan:
        // - Instantiate 'std::pair' with a simple string class that uses
        //   'bdema_Allocator'.
        // - Construct a objects using the test allocator.
        // - Verify that object members have the correct value.
        // - Verify that that the correct allocator was used.
        // - Verify that there are no memory leaks.
        //
        // Testing:
        //   Breathing test only.  Excercises basic functionality.
        // --------------------------------------------------------------------        
        if (verbose) std::printf("\nBREATHING TEST"
                                 "\n==============\n");

        bdema_TestAllocator ta1(veryVerbose);
        bdema_TestAllocator ta2(veryVerbose);

        bdema_DefaultAllocatorGuard allocGuard(&ta2);

        {
            typedef std::pair<my_String, short> Obj;
            ASSERT((bdemf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bdemf_IsSame<short, Obj::second_type>::VALUE));

            ASSERT(! (bdealg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bdealg_HasTrait<Obj, CanAbandonMemory>::VALUE));
            ASSERT(  (bdealg_HasTrait<Obj, UsesBdemaAllocator>::VALUE));

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
