// bslstl_pair.t.cpp                  -*-C++-*-

#include <bslstl_pair.h>

#include <bslalg_typetraits.h>
#include <bslalg_typetraitbitwisecopyable.h>
#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_issame.h>

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
// behaves differently when instantiated with types that use 'bslma_Allocator'.
// Testing consists mostly of testing all constructors with every combination
// of allocator-using/non-allocator-using template parameters.  The other
// combinatorial issue is the propagation of type traits from template
// parameters to 'bsl::pair' specializations.  Since this is a compile-time
// computation, it is only necessary to instantiate a representative
// combination of traits.
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
// [2] pair(bslma_Allocator *alloc);
// [2] pair(const T1& a, const T2& b);
// [2] pair(const T1& a, const T2& b, bslma_Allocator *alloc);
// [2] pair(const pair& rhs);
// [2] pair(const pair& rhs, bslma_Allocator *alloc);
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
//     pair(const pair<U1, U2>& rhs, bslma_Allocator *alloc);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [5] USAGE EXAMPLE
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

// Abbreviations for long trait names
typedef bslalg_TypeTraitBitwiseMoveable BitwiseMoveable;
typedef bslalg_TypeTraitBitwiseCopyable BitwiseCopyable;
typedef bslalg_TypeTraitHasTrivialDefaultConstructor TrivialConstructor;
typedef bslalg_TypeTraitBitwiseEqualityComparable    BitwiseComparable;
typedef bslalg_TypeTraitUsesBslmaAllocator UsesBslmaAllocator;

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
    char *myStrDup(const char *s, bslma_Allocator *alloc)
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
        // Simple string class that uses a 'bslma_Allocator' allocator.

        bslma_Allocator *d_allocator;
        char            *d_data;

      public:
        BSLALG_DECLARE_NESTED_TRAITS(my_String,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        explicit my_String(bslma_Allocator *alloc = 0);
            // Construct an empty string using the optionally-specified
            // allocator 'alloc'.

        my_String(const char* s, bslma_Allocator *alloc = 0);
            // Construct a string with contents specified in 's' using the
            // optionally-specified allocator 'alloc'.

        my_String(const my_String& rhs, bslma_Allocator *alloc = 0);
            // Construct a copy of the specified 'rhs' string using the
            // optionally-specified allocator 'alloc'.

        ~my_String();
            // Destroy this string.

        my_String& operator=(const my_String& rhs);
            // Copy specified 'rhs' string value to this string.

        const char* c_str() const;
            // Return the null-terminated character array for this string.

        bslma_Allocator *allocator() const;
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

    my_String::my_String(bslma_Allocator *alloc)
    : d_allocator(bslma_Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup("", d_allocator);
    }

    my_String::my_String(const char *s, bslma_Allocator *alloc)
    : d_allocator(bslma_Default::allocator(alloc)), d_data(0)
    {
        d_data = myStrDup(s, d_allocator);
    }

    my_String::my_String(const my_String& rhs, bslma_Allocator *alloc)
    : d_allocator(bslma_Default::allocator(alloc)), d_data(0)
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

    bslma_Allocator *my_String::allocator() const
    {
        return d_allocator;
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
        bslma_TestAllocator alloc;
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
    static bslma_TestAllocator *allocator();
        // Return pointer to singleton test allocator.

    my_NoAllocString();
    my_NoAllocString(const char* s);
    my_NoAllocString(const my_NoAllocString& rhs);
        // Construct a string the normal way.

    explicit my_NoAllocString(bslma_Allocator *alloc);
    my_NoAllocString(const char* s, bslma_Allocator *alloc);
    my_NoAllocString(const my_NoAllocString& rhs, bslma_Allocator *alloc);
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

bslma_TestAllocator *my_NoAllocString::allocator()
{
    static bslma_TestAllocator singleton(veryVerbose);
    return &singleton;
}

my_NoAllocString::my_NoAllocString()
: d_data(0)
{
    d_data = myStrDup("", allocator());
}

my_NoAllocString::my_NoAllocString(bslma_Allocator * /*alloc*/)
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

my_NoAllocString::my_NoAllocString(const char *s, bslma_Allocator * /*alloc*/)
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
                                   bslma_Allocator         * /*alloc*/)
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

                             // =================
                             // struct traits_...
                             // =================
struct traits_MoveAbandonBslma :
    BitwiseMoveable,
    UsesBslmaAllocator
{
};

struct traits_CopyTrivial :
    BitwiseCopyable,
    TrivialConstructor
{
};

struct traits_EqualityTrivial :
    BitwiseCopyable,
    TrivialConstructor,
    BitwiseComparable
{
};

                           // =====================
                           // struct my_(Traits...)
                           // =====================

struct my_MoveAbandonBslma
{
    BSLALG_DECLARE_NESTED_TRAITS(my_MoveAbandonBslma, traits_MoveAbandonBslma);
};

struct my_CopyTrivial
{
    BSLALG_DECLARE_NESTED_TRAITS(my_CopyTrivial, traits_CopyTrivial);
};

struct my_EqualityTrivial
{
    BSLALG_DECLARE_NESTED_TRAITS(my_EqualityTrivial, traits_EqualityTrivial);
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
      case 5: {
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
        //   'pair<const char*, int>'.
        // - Construct 'pair<my_String, int>' from
        //   'pair<const char*, int>' using an allocator.
        // - Construct 'pair<my_NoAllocString, my_String>' from
        //   'pair<const char*, const char*>' using an allocator.
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
        //                 bslma_Allocator            *alloc);
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nCONVERSION CONSTRUCTOR TEST"
                                 "\n===========================\n");

        bslma_TestAllocator ta1(veryVerbose);
        bslma_TestAllocator ta2(veryVerbose);
        bslma_TestAllocator& ta3 = *my_NoAllocString::allocator();

        bslma_DefaultAllocatorGuard allocGuard(&ta2);

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

      } break;;
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
        //   * bslalg_TypeTraitBitwiseMoveable
        //   * bslalg_TypeTraitBitwiseCopyable (implies BitwiseMoveable)
        //   * bslalg_TypeTraitTrivialConstructor
        // - Verify that the resulting instantiation has the trait
        //   'bslalg_TypeTraitUsesBslmaAllocator' if and only BOTH types have
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
        ASSERT(! (bslalg_HasTrait<Pair0, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair0, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair0, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair0, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair0, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma,"
                        "             my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_MoveAbandonBslma> Pair1;
        ASSERT(  (bslalg_HasTrait<Pair1, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair1, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair1, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair1, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair1, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_CopyTrivial> Pair2;
        ASSERT(  (bslalg_HasTrait<Pair2, BitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair2, BitwiseCopyable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair2, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair2, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair2, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_MoveAbandonBslma> Pair3;
        ASSERT(  (bslalg_HasTrait<Pair3, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair3, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair3, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair3, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair3, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_CopyTrivial>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_CopyTrivial> Pair4;
        ASSERT(  (bslalg_HasTrait<Pair4, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair4, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair4, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair4, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair4, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, my_NoTraits>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, my_NoTraits> Pair5;
        ASSERT(! (bslalg_HasTrait<Pair5, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair5, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair5, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair5, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair5, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_CopyTrivial, my_NoTraits>\n");
        }
        typedef bsl::pair<my_CopyTrivial, my_NoTraits> Pair6;
        ASSERT(! (bslalg_HasTrait<Pair6, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair6, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair6, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair6, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair6, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_String, my_NoTraits>\n");
        }
        typedef bsl::pair<my_String, my_NoTraits> Pair7;
        ASSERT(! (bslalg_HasTrait<Pair7, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair7, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair7, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair7, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair7, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_MoveAbandonBslma, int>\n");
        }
        typedef bsl::pair<my_MoveAbandonBslma, int> Pair8;
        ASSERT(  (bslalg_HasTrait<Pair8, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair8, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair8, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair8, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair8, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, my_CopyTrivial>\n");
        }
        typedef bsl::pair<int, my_CopyTrivial> Pair9;
        ASSERT(  (bslalg_HasTrait<Pair9, BitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair9, BitwiseCopyable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair9, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair9, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair9, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<my_String, my_MoveAbandonBslma>\n");
        }
        typedef bsl::pair<my_String, my_MoveAbandonBslma> Pair10;
        ASSERT(! (bslalg_HasTrait<Pair10, BitwiseMoveable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair10, BitwiseCopyable>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair10, TrivialConstructor>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair10, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair10, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<char, int>\n");
        }
        typedef bsl::pair<char, int> Pair11;
        ASSERT(  (bslalg_HasTrait<Pair11, BitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair11, BitwiseCopyable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair11, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair11, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair11, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, char>\n");
        }
        typedef bsl::pair<int, char> Pair12;
        ASSERT(  (bslalg_HasTrait<Pair12, BitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair12, BitwiseCopyable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair12, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair12, UsesBslmaAllocator>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair12, BitwiseComparable>::VALUE));

        if (verbose) {
            std::printf("Testing traits of "
                        "bsl::pair<int, int>\n");
        }
        typedef bsl::pair<int, int> Pair13;
        ASSERT(  (bslalg_HasTrait<Pair13, BitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair13, BitwiseCopyable>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair13, TrivialConstructor>::VALUE));
        ASSERT(! (bslalg_HasTrait<Pair13, UsesBslmaAllocator>::VALUE));
        ASSERT(  (bslalg_HasTrait<Pair13, BitwiseComparable>::VALUE));

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
        //    the 'bslalg_TypeTraitUsesBslmaAllocator' trait, then the
        //    'pair' also has that trait.
        // 3. If 'pair' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        //    trait, then each constructor can be passed a 'bslma_Allocator'
        //    pointer and that pointer is passed through to the member(s) that
        //    take it.
        // 4. Assignment works as designed.
        // 5. Operators ==, !=, <, >, <=, and >= work as designed.
        //
        // Plan:
        // - Select a small set of interesting types:
        //   'short'            - Fundamental type
        //   'my_String'        - Uses 'bslma_Allocator' in constructor
        //   'my_NoAllocString' - Does not use 'bslma_Allocator' in constructor
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
        //     pair(bslma_Allocator *alloc);
        //     pair(const T1& a, const T2& b);
        //     pair(const T1& a, const T2& b, bslma_Allocator *alloc);
        //     pair(const pair& rhs);
        //     pair(const pair& rhs, bslma_Allocator *alloc);
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

        bslma_TestAllocator ta1(veryVerbose);
        bslma_TestAllocator ta2(veryVerbose);
        bslma_TestAllocator& ta3 = *my_NoAllocString::allocator();

        bslma_DefaultAllocatorGuard allocGuard(&ta2);

        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());

        if (verbose) std::printf("testing bsl::pair<short, short>\n");
        {
            typedef bsl::pair<short, short> Obj;
            ASSERT((bslmf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<short, Obj::second_type>::VALUE));

            const short NULL_FIRST   = 0;
            const short NULL_SECOND  = 0;
            const short VALUE_FIRST  = 3;
            const short VALUE_SECOND = 4;

            ASSERT(  (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
        // End test bslstl_Pair<short, short>

        if (verbose) std::printf("testing bsl::pair<short, my_String>\n");
        {
            typedef bsl::pair<short, my_String> Obj;
            ASSERT((bslmf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_String, Obj::second_type>::VALUE));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<short, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const short        NULL_FIRST   = 0;
            const char  *const NULL_SECOND  = "";
            const short        VALUE_FIRST  = 4;
            const char  *const VALUE_SECOND = "Hello";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<short, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_String, Obj::second_type>::VALUE));

            const char *const NULL_FIRST   = "";
            const char *const NULL_SECOND  = "";
            const char *const VALUE_FIRST  = "Hello";
            const char *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
        // End testing bslstl_Pair<my_String, my_NoAllocString>

        if (verbose) {
            std::printf("testing bsl::pair<my_NoAllocString, short>\n");
        }
        {
            typedef bsl::pair<my_NoAllocString, short> Obj;
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<short, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const short        NULL_SECOND  = 0;
            const char  *const VALUE_FIRST  = "Hello";
            const short        VALUE_SECOND = 4;

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_String, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<my_NoAllocString, Obj::second_type>::VALUE));

            const char  *const NULL_FIRST   = "";
            const char  *const NULL_SECOND  = "";
            const char  *const VALUE_FIRST  = "Hello";
            const char  *const VALUE_SECOND = "Goodbye";

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
        //   'bslma_Allocator'.
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

        bslma_TestAllocator ta1(veryVerbose);
        bslma_TestAllocator ta2(veryVerbose);

        bslma_DefaultAllocatorGuard allocGuard(&ta2);

        {
            typedef bsl::pair<my_String, short> Obj;
            ASSERT((bslmf_IsSame<my_String, Obj::first_type>::VALUE));
            ASSERT((bslmf_IsSame<short, Obj::second_type>::VALUE));

            ASSERT(! (bslalg_HasTrait<Obj, BitwiseMoveable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, BitwiseCopyable>::VALUE));
            ASSERT(! (bslalg_HasTrait<Obj, TrivialConstructor>::VALUE));
            ASSERT(  (bslalg_HasTrait<Obj, UsesBslmaAllocator>::VALUE));

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
