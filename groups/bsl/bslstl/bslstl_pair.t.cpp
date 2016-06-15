// bslstl_pair.t.cpp                                                  -*-C++-*-
#include <bslstl_pair.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_bsltestutil.h>

#include <bsltf_movablealloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_simpletesttype.h>
#include <bsltf_templatetestfacility.h>

#include <stddef.h>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strcmp'

#include <algorithm>    // 'std::swap'

using namespace BloombergLP;
using namespace bsl;

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
//
// TBD: we need to add test cases for a bunch of new methods that were added
//      for the cpp11 project -- have added them to the plan below but have
//      not yet created the test cases.
//-----------------------------------------------------------------------------
// [ 2] typedef T1 first_type;
// [ 2] typedef T2 second_type;
// [ 2] T1 first;
// [ 2] T1 second;
// [ 2] pair();
// [ 2] pair(AllocatorPtr basicAllocator);
// [ 2] pair(const T1& a, const T2& b);
// [ 2] pair(const T1& a, const T2& b, AllocatorPtr basicAllocator);
// [11] template <class U1, class U2> pair(U1&& a, U2&& b);
// [11] template <class U1, class U2> pair(U1&& a, U2&& b, AllocatorPtr a);
// [11] template <class U1, class U2> pair(const U1&, const U2&);
// [11] template <class U1, class U2> pair(const U1&, const U2&, AllocatorPtr);
// [11] template <class U1, class U2> pair(U1&, const U2&);
// [11] template <class U1, class U2> pair(U1&, const U2&, AllocatorPtr);
// [11] template <class U1, class U2> pair(const U1&, U2&);
// [11] template <class U1, class U2> pair(const U1&, U2&, AllocatorPtr);
// [11] template <class U1, class U2> pair(U1&, U2&);
// [11] template <class U1, class U2> pair(U1&, U2&, AllocatorPtr);
// [14] pair(piecewise_construct_t, tuple aArgs, tuple bArgs)
// [14] pair(piecewise_construct_t, tuple aArgs, tuple bArgs, basicAllocator)
// [ 2] pair(const pair& original);
// [ 2] pair(const pair& original, AllocatorPtr basicAllocator);
// [10] pair(pair&& original)
// [10] pair(pair&& original, AllocatorPtr basicAllocator)
// [ 4] pair(const pair<U1, U2>& rhs);
// [ 4] pair(const pair<U1, U2>& rhs, AllocatorPtr basicAllocator);
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other)
// [ 9] template <class U1, class U2> pair(pair<U1, U2>&& other, AllocatorPtr)
// [  ] pair(const native_std::pair<U1, U2>& rhs);
// [  ] pair(const native_std::pair<U1, U2>&, BloombergLP::bslma::Allocator *);
// [ 2] ~pair();
// [ 2] pair& operator=(const pair& rhs);
// [13] pair& operator=(pair&& rhs);
// [ 8] pair& operator=(const pair<U1, U2>& rhs)
// [12] pair& operator=(pair<U1, U2>&& rhs)
// [  ] pair& operator=(const native_std::pair<U1, U2>& rhs);
// [ 2] bool operator==(const pair& x, const pair& y);
// [ 2] bool operator!=(const pair& x, const pair& y);
// [ 2] bool operator<(const pair& x, const pair& y);
// [ 2] bool operator>(const pair& x, const pair& y);
// [ 2] bool operator<=(const pair& x, const pair& y);
// [ 2] bool operator>=(const pair& x, const pair& y);
// [ 5] void pair::swap(pair& rhs);
// [ 5] void swap(pair& lhs, pair& rhs);
// [ 6] hashAppend(HASHALG& hashAlg, const pair<T1,T2>&  input);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] Type Traits
// [ 7] Concern: Can create a pointer-to-member for 'first' and 'second'
// [ 8] Concern: Can assign to a 'pair' of references

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// Pragmas to silence format warnings, should be cleaned up before final commit
// BDE_VERIFY pragma: -AC01  // bde_verify does not recognise generic allocator
// BDE_VERIFY pragma: -AL01  // Strict aliasing concerns should be addressed
// BDE_VERIFY pragma: -AT01  // bde_verify does not recognise generic allocator
// BDE_VERIFY pragma: -CC01  // C style casts
// BDE_VERIFY pragma: -FD01  // Lots of functions need a clear contract
// BDE_VERIFY pragma: -IND01 // Indent issues
// BDE_VERIFY pragma: -IND03 // Text-alignment issues
// BDE_VERIFY pragma: -IND04 // Text-alignment issues

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static bool             verbose = false;
static bool         veryVerbose = false;
static bool     veryVeryVerbose = false;
static bool veryVeryVeryVerbose = false;

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
// into memory allocated from a supplied allocator.  The allocator can conform
// to either the 'bslma::Allocator' protocol or the STL allocator concept:
//..
    template <class ALLOC>
    char *myStrDup(const char *s, ALLOC *basicAllocator)
        // Copy the specified null-terminated string 's' into memory allocated
        // from the specified '*basicAllocator'
    {
        char *result = (char*) basicAllocator->allocate(strlen(s) + 1);
        return strcpy(result, s);
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

        my_String(const char* s, bslma::Allocator *alloc = 0);      // IMPLICIT
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
        return 0 == strcmp(lhs.c_str(), rhs.c_str());
    }

    bool operator==(const my_String& lhs, const char *rhs)
    {
        return 0 == strcmp(rhs, lhs.c_str());
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
        return strcmp(lhs.c_str(), rhs.c_str()) < 0;
    }

    my_String::my_String(bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data(myStrDup("", d_allocator_p))
    {
    }

    my_String::my_String(const char *s, bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data(myStrDup(s, d_allocator_p))
    {
    }

    my_String::my_String(const my_String& rhs, bslma::Allocator *alloc)
        : d_allocator_p(bslma::Default::allocator(alloc))
        , d_data(myStrDup(rhs.d_data, d_allocator_p))
    {
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

                           // =======================
                           // class my_AllocArgString
                           // =======================

template <class ALLOC>
class my_AllocArgString
{
    // Another simple string class that uses a user-supplied STL-style
    // allocator that is provided to the constructor as the second argument,
    // following an 'bsl::allocator_arg_t' tag argument.  The behavior is
    // undefined unless 'ALLOC::value_type' is identical to 'char'.

    ALLOC  d_alloc;
    char  *d_data;

  public:
    typedef ALLOC allocator_type;

    my_AllocArgString();
    my_AllocArgString(const char* s);                               // IMPLICIT
    my_AllocArgString(const my_AllocArgString& rhs);
        // Construct a string without supplying an allocator.

    my_AllocArgString(bsl::allocator_arg_t, const ALLOC& a);
    my_AllocArgString(bsl::allocator_arg_t, const ALLOC& a, const char* s);
    my_AllocArgString(bsl::allocator_arg_t,
                      const ALLOC&             a,
                      const my_AllocArgString& rhs);
        // Construct a string using the specified 'a' allocator, following the
        // 'allocator_arg_t' construction protocol.

    ~my_AllocArgString();
        // Destroy this string.

    my_AllocArgString& operator=(const my_AllocArgString& rhs);
        // Copy the specified 'rhs' to this string.

    size_t length() const;
        // Return the length of this string, excluding the null terminator.

    const char* c_str() const;
        // Return the null-terminated character array for this string. Never
        // returns a null pointer.

    allocator_type get_allocator() const;
        // Return the allocator used to construct this object.

    bslma::Allocator* allocator() const;
        // Return the bslma mechanism within the STL allocator.
};

template <class ALLOC>
bool operator==(const my_AllocArgString<ALLOC>& lhs,
                const my_AllocArgString<ALLOC>& rhs)
{
    return 0 == strcmp(lhs.c_str(), rhs.c_str());
}

template <class ALLOC>
bool operator==(const my_AllocArgString<ALLOC>& lhs, const char *rhs)
{
    return 0 == strcmp(rhs, lhs.c_str());
}

template <class ALLOC>
bool operator==(const char *lhs, const my_AllocArgString<ALLOC>& rhs)
{
    return rhs == lhs;
}

template <class ALLOC>
bool operator!=(const my_AllocArgString<ALLOC>& lhs,
                const my_AllocArgString<ALLOC>& rhs)
{
    return ! (lhs == rhs);
}

template <class ALLOC>
bool operator!=(const my_AllocArgString<ALLOC>& lhs, const char *rhs)
{
    return ! (lhs == rhs);
}

template <class ALLOC>
bool operator!=(const char *lhs, const my_AllocArgString<ALLOC>& rhs)
{
    return ! (rhs == lhs);
}

template <class ALLOC>
bool operator<(const my_AllocArgString<ALLOC>& lhs,
               const my_AllocArgString<ALLOC>& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString()
    : d_alloc(), d_data(myStrDup("", &d_alloc))
{
    // class invariant: 'd_data' is not null
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(const char *s)
    : d_alloc(), d_data(myStrDup(s, &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(
    const my_AllocArgString& rhs)
    : d_alloc(), d_data(myStrDup(rhs.c_str(), &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(bsl::allocator_arg_t,
                                                    const ALLOC& a)
    : d_alloc(a), d_data(myStrDup("", &d_alloc))
{
    // class invariant: 'd_data' is not null
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(bsl::allocator_arg_t,
                                                    const ALLOC&  a,
                                                    const char   *s)
    : d_alloc(a), d_data(myStrDup(s, &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::my_AllocArgString(
                                    bsl::allocator_arg_t,
                                    const ALLOC&                        a,
                                    const my_AllocArgString<ALLOC>& rhs)
    : d_alloc(a), d_data(myStrDup(rhs.c_str(), &d_alloc))
{
}

template <class ALLOC>
my_AllocArgString<ALLOC>::~my_AllocArgString()
{
    d_alloc.deallocate(d_data, length() + 1);
}

template <class ALLOC>
my_AllocArgString<ALLOC>&
my_AllocArgString<ALLOC>::operator=(const my_AllocArgString<ALLOC>& rhs)
{
    if (this != &rhs) {
        d_alloc.deallocate(d_data, length() + 1);
        d_data = myStrDup(rhs.c_str(), &d_alloc);
    }
    return *this;
}

template <class ALLOC>
bslma::Allocator *my_AllocArgString<ALLOC>::allocator() const
{
    return d_alloc.mechanism();
}

template <class ALLOC>
const char *my_AllocArgString<ALLOC>::c_str() const
{
    return d_data;
}

template <class ALLOC>
ALLOC my_AllocArgString<ALLOC>::get_allocator() const
{
    return d_alloc;
}

template <class ALLOC>
size_t my_AllocArgString<ALLOC>::length() const
{
    return strlen(d_data);
}

namespace BloombergLP {
namespace bslmf {
template <class ALLOC>
struct UsesAllocatorArgT<my_AllocArgString<ALLOC> > : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <class ALLOC>
struct UsesBslmaAllocator<my_AllocArgString<ALLOC> > :
        bslmf::IsConvertible<Allocator*, ALLOC>::type {};
}  // close namespace bslma
}  // close enterprise namespace

                           // =====================
                           // class my_STLCharAlloc
                           // =====================

class my_STLCharAlloc
{
    // STL-conforming allocator for 'char' elements.  Cannot be rebound to
    // allocate other types.

    bslma::Allocator *d_bslmaAlloc_p;

  public:
    typedef char    value_type;
    typedef char   *pointer_type;
    typedef size_t  size_type;

    static bslma::TestAllocator *defaultMechanism();

    my_STLCharAlloc();
    explicit my_STLCharAlloc(bslma::Allocator *bslmaAlloc_p);
        // Not convertible from 'bslma::Allocator*'.

    //! my_STLCharAlloc(const my_STLCharAlloc&) = default;
    //! ~my_STLCharAlloc() = default;
    //! my_STLCharAlloc& operator=(const my_STLCharAlloc&) = default;

    char *allocate(size_type n);
    void deallocate(char *p, size_type n);

    bslma::Allocator *mechanism() const;
};

bslma::TestAllocator *my_STLCharAlloc::defaultMechanism()
{
    static bslma::TestAllocator singleton(veryVeryVerbose);
    return &singleton;
}

inline
my_STLCharAlloc::my_STLCharAlloc()
    : d_bslmaAlloc_p(defaultMechanism())
{
}

inline
my_STLCharAlloc::my_STLCharAlloc(bslma::Allocator *bslmaAlloc_p)
    : d_bslmaAlloc_p(bslmaAlloc_p)
{
}

inline
char *my_STLCharAlloc::allocate(size_type n)
{
    return static_cast<char*>(d_bslmaAlloc_p->allocate(n));
}

inline
void my_STLCharAlloc::deallocate(char* p, size_type /* n */)
{
    d_bslmaAlloc_p->deallocate(p);
}

inline
bslma::Allocator *my_STLCharAlloc::mechanism() const {
    return d_bslmaAlloc_p;
}

typedef my_AllocArgString<my_STLCharAlloc>       my_STLAllocArgStr;
typedef my_AllocArgString<bsl::allocator<char> > my_BslmaAllocArgStr;

                           // ======================
                           // class my_NoAllocString
                           // ======================

class my_NoAllocString : public my_AllocArgString<bsl::allocator<char> >
{
    // Another simple string class that does not use a user-supplied
    // allocator.  All memory is allocated from a shared test allocator.  Will
    // compile and run if an attempt is made to use a constructor that takes
    // an allocator argument, but will report an assert failure.

    typedef my_AllocArgString<bsl::allocator<char> > Base;

  public:
    my_NoAllocString();
    my_NoAllocString(const char* s);                                // IMPLICIT
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

    //! ~my_NoAllocString() = default;
    //! my_NoAllocString& operator=(const my_NoAllocString& rhs) = default;
};

my_NoAllocString::my_NoAllocString()
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism())
{
}

my_NoAllocString::my_NoAllocString(bslma::Allocator * /*alloc*/)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism())
{
    ASSERT("Shouldn't get here" && 0);
}

my_NoAllocString::my_NoAllocString(const char *s)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), s)
{
}

my_NoAllocString::my_NoAllocString(const char *s, bslma::Allocator * /*alloc*/)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), s)
{
    ASSERT("Shouldn't get here" && 0);
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString& rhs)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), rhs)
{
}

my_NoAllocString::my_NoAllocString(const my_NoAllocString&  rhs,
                                   bslma::Allocator         * /*alloc*/)
    : Base(bsl::allocator_arg, my_STLCharAlloc::defaultMechanism(), rhs)
{
    ASSERT("Shouldn't get here" && 0);
}

                           // ============
                           // class Values
                           // ============

template <class TYPE, bool = bsl::is_integral<TYPE>::value>
struct Values
{
    // Namespace for sample values of non-integral 'TYPE'

    typedef TYPE Type;

    static Type null() { return ""; }
    static Type first() { return "Hello"; }
    static Type second() { return "World"; };
};

template <class TYPE>
struct Values<TYPE, true>
{
    // Namespace for sample values of integral 'TYPE'

    typedef TYPE Type;

    static Type null() { return 0; }
    static Type first() { return 1; }
    static Type second() { return 2; };
};


                           // =====================
                           // struct my_(Traits...)
                           // =====================

struct my_NonTrivialBaseClass {
    // C++11 compilers will detect trivial classes, including empty classes, by
    // default, so we establish a simple non-trivial class that can be used as
    // the base class for our testing types, which will have trivial traits
    // only if explicitly marked as trivial for the relevant 'bsl' trait.

    my_NonTrivialBaseClass(){}
    my_NonTrivialBaseClass(const my_NonTrivialBaseClass&){}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct my_MoveAbandonBslma : my_NonTrivialBaseClass {
};

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseMoveable<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslmf

namespace bslma {
template <>
struct UsesBslmaAllocator<my_MoveAbandonBslma> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

struct my_CopyTrivial : my_NonTrivialBaseClass {};

namespace bsl {
template <>
struct is_trivially_copyable<my_CopyTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_CopyTrivial>
     : bsl::true_type {};
}  // close namespace bsl

struct my_EqualityTrivial : my_NonTrivialBaseClass {};

namespace bsl {
template <>
struct is_trivially_copyable<my_EqualityTrivial> : bsl::true_type {};
template <>
struct is_trivially_default_constructible<my_EqualityTrivial>
     : bsl::true_type {};
}  // close namespace bsl

struct my_NoTraits : my_NonTrivialBaseClass {};

namespace BloombergLP {
namespace bslmf {
template <>
struct IsBitwiseEqualityComparable<my_EqualityTrivial> : bsl::true_type {};

// Empty classes are bitwise moveable by default.  Specialize for 'my_NoTraits'
// to make it NOT bitwise moveable.
template<>
struct IsBitwiseMoveable<my_NoTraits> : bsl::false_type {};

}  // close namespace bslmf
}  // close enterprise namespace



//=============================================================================
//              HELPER CLASS TO TEST FORWARDING MANAGEDPTR TO CONSTRUCTORS
//-----------------------------------------------------------------------------

template <class TARGET>
class ManagedWrapper {
  private:
    void *d_ptr;

  public:
    ManagedWrapper(bslma::ManagedPtr<TARGET> source)                // IMPLICIT
        : d_ptr(source.get())
    {
    }

};

//=============================================================================
//              HELPER CLASSES AND FUNCTIONS FOR TESTING SWAP
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

    void swap(TypeWithSwap& a, TypeWithSwap& b) {
        a.swap(b);
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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    // Nothrow moves needed so that std::swap doesn't get SFINAEd out.
    TypeWithoutSwap(const TypeWithoutSwap&) noexcept = default;
    TypeWithoutSwap& operator=(const TypeWithoutSwap&) noexcept = default;
#endif

    bool operator==(const TypeWithoutSwap& rhs) const {
        return data == rhs.data;
    }

    void assertSwapCalled() const {
    }
};


template <class T1, class T2>
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


                           // =================================
                           // Hash<my_String> (hashAppend test)
                           // =================================

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const my_String& input)
{
    using bslh::hashAppend;
    hashAlg(input.c_str(), strlen(input.c_str()));
}

template <class STRING>
void testBslmaStringConversionCtor()
{
    bslma::TestAllocator ta1(veryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVerbose);
    bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

    bslma::DefaultAllocatorGuard allocGuard(&ta2);

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
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

    if (veryVeryVerbose) printf("\tpair<STRING,int>, explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1, &ta1), &P2 = p2;
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

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(2 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, explicit allocator\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(2 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVerbose) printf("Conversion from native 'std::pair'\n");

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
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

    if (veryVeryVerbose) printf("\tpair<STRING,int>, explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1, &ta1), &P2 = p2;
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

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(1 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta2 == P2.first.allocator());
        ASSERT(&ta2 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(1 <= ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, explicit allocator\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1, &ta1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta1 == P2.first.allocator());
        ASSERT(&ta1 == P2.second.allocator());
        ASSERT(2 <= ta1.numBlocksInUse());
        ASSERT(0 == ta2.numBlocksInUse());
        ASSERT(0 == ta3.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
}

template <class STRING>
void testNonBslmaStringConversionCtor()
{
    bslma::TestAllocator ta1(veryVeryVerbose);  // explicit allocator
    bslma::TestAllocator ta2(veryVeryVerbose);  // global default allocator
    // Default mechanism for 'my_STLCharAlloc'
    bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

    bslma::DefaultAllocatorGuard allocGuard(&ta2);

    bslma::TestAllocator *ta4_p;
    {
        STRING dummy;  // Default-constructed string with defaulted allocator
        // Default allocator when not supplied:
        ta4_p = dynamic_cast<bslma::TestAllocator *>(dummy.allocator());
    }
    bslma::TestAllocator& ta4 = *ta4_p; // 'STRING's default allocator

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        bsl::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        bsl::pair<const char*, const char*> p1("Hello", "World"), &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(2 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVerbose) printf("Conversion from native 'std::pair'\n");

    if (veryVeryVerbose) printf("\tpair<STRING,int>, no explicit allocator\n");
    {
        native_std::pair<const char*, short> p1("Hello", (short) 5), &P1 = p1;
        bsl::pair<STRING, int> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT(5 == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<int,STRING>, no explicit allocator\n");
    {
        bsl::pair<short, const char*> p1((short) 5, "Hello"), &P1 = p1;
        bsl::pair<int, STRING> p2(P1), &P2 = p2;
        ASSERT(5 == P2.first);
        ASSERT("Hello" == P2.second);
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(1 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());

    if (veryVeryVerbose) printf("\tpair<STRING,STRING>, no explicit alloc\n");
    {
        native_std::pair<const char*, const char*> p1("Hello", "World"),
            &P1 = p1;
        bsl::pair<STRING, STRING> p2(P1), &P2 = p2;
        ASSERT("Hello" == P2.first);
        ASSERT("World" == P2.second);
        ASSERT(&ta4 == P2.first.allocator());
        ASSERT(&ta4 == P2.second.allocator());
        ASSERT(0 == ta1.numBlocksInUse());
        ASSERT(&ta4 == &ta2 || 0 == ta2.numBlocksInUse());
        ASSERT(&ta4 == &ta3 || 0 == ta3.numBlocksInUse());
        ASSERT(2 <= ta4.numBlocksInUse());
    }

    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
    ASSERT(0 == ta4.numBlocksInUse());
}

template <class TYPE, bool = bslma::UsesBslmaAllocator<TYPE>::value>
struct AllocatorMatcher
{
    static bool match(const TYPE&, bslma::Allocator *) { return true; }
        // Return true (for types that don't take a bslma::Allocator).
};

template <class TYPE>
struct AllocatorMatcher<TYPE, true>
{
    static bool match(const TYPE& v, bslma::Allocator *a)
        // Return true if the specified 'v' was constructed with the specified
        // allocator 'a'; otherwise false.
        { return v.allocator() == a; }
};

template <class TYPE>
inline
bool matchAllocator(const TYPE& v, bslma::Allocator *a)
    // Return true if the specified 'v' was constructed with the specified
    // allocator 'a'; otherwise false.  If 'TYPE' is not constructed with a
    // 'bslma::Allocator', then always return true.
{
    return AllocatorMatcher<TYPE>::match(v, a);
}

template <class T1, class T2>
void testFunctionality(bsl::false_type /* UsesBslmaAllocator */)
    // Test functionality of 'bsl::pair<T1,T2>', using only constructors that
    // don't taken a 'bslma::Allocator*' argument.
{
    typedef bsl::pair<T1, T2> Obj;
    ASSERT((bsl::is_same<T1, typename Obj::first_type>::value));
    ASSERT((bsl::is_same<T2, typename Obj::second_type>::value));

    const T1 NULL_FIRST   = Values<T1>::null();
    const T2 NULL_SECOND  = Values<T2>::null();
    const T1 VALUE_FIRST  = Values<T1>::first();
    const T2 VALUE_SECOND = Values<T2>::second();

    // Test traits
    ASSERT(bslmf::IsBitwiseMoveable<Obj>::value ==
           (bslmf::IsBitwiseMoveable<T1>::value &&
            bslmf::IsBitwiseMoveable<T2>::value));
    ASSERT(bsl::is_trivially_copyable<Obj>::value ==
           (bsl::is_trivially_copyable<T1>::value &&
            bsl::is_trivially_copyable<T2>::value));
    ASSERT(bsl::is_trivially_default_constructible<Obj>::value ==
           (bsl::is_trivially_default_constructible<T1>::value &&
            bsl::is_trivially_default_constructible<T2>::value));
    ASSERT(bslma::UsesBslmaAllocator<Obj>::value ==
           (bslma::UsesBslmaAllocator<T1>::value ||
            bslma::UsesBslmaAllocator<T2>::value));

    bslma::TestAllocator ta0(veryVeryVerbose);

    bslma::DefaultAllocatorGuard allocGuard(&ta0);

    ASSERT(0 == ta0.numBlocksInUse());

    {
        // Default construct using default allocator
        Obj p1; const Obj& P1 = p1;
        ASSERT(NULL_FIRST  == P1.first);
        ASSERT(NULL_SECOND == P1.second);
        ASSERT(matchAllocator(P1.first, &ta0));
        ASSERT(matchAllocator(P1.second, &ta0));

        // Construct with two values and default allocator
        Obj p2(VALUE_FIRST, VALUE_SECOND); const Obj& P2 = p2;
        ASSERT(VALUE_FIRST  == P2.first);
        ASSERT(VALUE_SECOND == P2.second);
        ASSERT(matchAllocator(P2.first, &ta0));
        ASSERT(matchAllocator(P2.second, &ta0));

        // Copy construct
        Obj p3(p2); const Obj& P3 = p3;
        ASSERT(VALUE_FIRST == P3.first);
        ASSERT(VALUE_SECOND == P3.second);
        ASSERT(matchAllocator(P3.first, &ta0));
        ASSERT(matchAllocator(P3.second, &ta0));

        // Test use of default allocator
        int minAllocations = 0;  // Allocations from default allocator
        if (bslma::UsesBslmaAllocator<T1>::value)
            minAllocations += 3;
        if (bslma::UsesBslmaAllocator<T2>::value)
            minAllocations += 3;
        ASSERT(minAllocations <= ta0.numBlocksInUse());

        // Test equality and relational operators
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

        // Test assignment
        p1 = P2;
        ASSERT(P1 == P2);
        ASSERT(matchAllocator(P1.first, &ta0));
        ASSERT(matchAllocator(P1.second, &ta0));
        ASSERT(minAllocations <= ta0.numBlocksInUse());
    }

    // Test the any memory allocated was deallocated
    ASSERT(0 == ta0.numBlocksInUse());
}

template <class T1, class T2>
void testFunctionality(bsl::true_type /* UsesBslmaAllocator */)
    // Test functionality of 'bsl::pair<T1,T2>', with and without
    // explicitly-supplied 'bslma::Allocator*' constructor arguments.
{
    // Test without explicit allocator
    testFunctionality<T1,T2>(false_type());

    typedef bsl::pair<T1, T2> Obj;

    const T1 NULL_FIRST   = Values<T1>::null();
    const T2 NULL_SECOND  = Values<T2>::null();
    const T1 VALUE_FIRST  = Values<T1>::first();
    const T2 VALUE_SECOND = Values<T2>::second();

    bslma::TestAllocator ta0(veryVeryVerbose);
    bslma::TestAllocator ta1(veryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVerbose);
    bslma::TestAllocator ta3(veryVeryVerbose);

    bslma::DefaultAllocatorGuard allocGuard(&ta0);

    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());

    {
        // Compute number of allocations per constructor from supplied
        // allocator.
        int numAllocations = 0;  // Allocations from default allocator
        if (bslma::UsesBslmaAllocator<T1>::value)
            numAllocations += 1;
        if (bslma::UsesBslmaAllocator<T2>::value)
            numAllocations += 1;

        // Test default construction with allocator
        Obj p4(&ta1); const Obj& P4 = p4;
        ASSERT(NULL_FIRST  == P4.first);
        ASSERT(NULL_SECOND == P4.second);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta1.numBlocksInUse());

        Obj p5(VALUE_FIRST, VALUE_SECOND, &ta2); const Obj& P5 = p5;
        ASSERT(VALUE_FIRST  == P5.first);
        ASSERT(VALUE_SECOND == P5.second);
        ASSERT(matchAllocator(P5.first, &ta2));
        ASSERT(matchAllocator(P5.second, &ta2));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta2.numBlocksInUse());

        Obj p6(P5, &ta3); const Obj& P6 = p6;
        ASSERT(VALUE_FIRST == P6.first);
        ASSERT(VALUE_SECOND == P6.second);
        ASSERT(matchAllocator(P6.first, &ta3));
        ASSERT(matchAllocator(P6.second, &ta3));
        ASSERT(0 == ta0.numBlocksInUse());
        ASSERT(numAllocations <= ta3.numBlocksInUse());

        p4 = P5;
        ASSERT(P4 == P5);
        ASSERT(matchAllocator(P4.first, &ta1));
        ASSERT(matchAllocator(P4.second, &ta1));
        ASSERT(0 == ta0.numBlocksInUse());
    }

    ASSERT(0 == ta0.numBlocksInUse());
    ASSERT(0 == ta1.numBlocksInUse());
    ASSERT(0 == ta2.numBlocksInUse());
    ASSERT(0 == ta3.numBlocksInUse());
}

class Base { };
class Derived : public Base { };

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 15: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        //   pair(piecewise_construct_t, tuple aArgs, tuple bArgs);
        //   pair(piecewise_construct_t, tuple aArgs, tuple bArgs, alloc);
        // --------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_LIBRARYFEATURES_SUPPORT_PIECEWISE_CONSTRUCT)
        typedef bsl::pair<int, bsltf::MovableAllocTestType>        ObjA1;

        typedef bsltf::TemplateTestFacility TstFacility;
        {
            ObjA1 a1(std::piecewise_construct,
                     std::forward_as_tuple(2),
                     std::forward_as_tuple(4));
            ASSERT(2 == a1.first);
            ASSERT(4 == TstFacility::getIdentifier(a1.second));
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocatorRaw(&da);
            bslma::TestAllocatorMonitor dam(&da);

            ObjA1 a1(std::piecewise_construct,
                     std::forward_as_tuple(2),
                     std::forward_as_tuple(4),
                     &oa);
            ASSERT(2 == a1.first);
            ASSERT(4 == TstFacility::getIdentifier(a1.second));
            ASSERT(&oa == a1.second.allocator());

            ASSERT(dam.isTotalSame());
        }
        {
            typedef bsl::pair<int, int> Obj;
            typedef bsl::allocator<Obj> A;

            bsls::ObjectBuffer<Obj> buffer;
            Obj *p = (Obj *) buffer.buffer();

            bslma::TestAllocator testAlloc;
            A m(&testAlloc);

            bsl::allocator_traits<A>::construct(m, p,
                                                std::piecewise_construct,
                                                std::forward_as_tuple(1),
                                                std::forward_as_tuple(2));
            const Obj& X = buffer.object();
            ASSERTV(X.first , 1 == X.first );
            ASSERTV(X.second, 2 == X.second);
        }
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        //   pair& operator=(pair&& rhs);
        // --------------------------------------------------------------------

        typedef bsl::pair<int, bsltf::MovableAllocTestType>        ObjA1;
        typedef bsl::pair<bsltf::MovableAllocTestType, int>        ObjA2;

        typedef bsltf::MoveState            MovState;
        typedef bslmf::MovableRefUtil       MovUtil;
        typedef bsltf::TemplateTestFacility TstFacility;

        bsltf::MoveState::Enum miState, mfState;
        {
            bsltf::MovableAllocTestType t;
            ObjA1 s1(1, t);
            mfState = TstFacility::getMovedFromState(s1.second);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 d1(0, t);
            miState = TstFacility::getMovedIntoState(d1.second);
            ASSERT(MovState::e_NOT_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            d1 = MovUtil::move(s1);

            ASSERT(1 == d1.first);
            mfState = TstFacility::getMovedFromState(s1.second);
            miState = TstFacility::getMovedIntoState(d1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
        }
        {
            bsltf::MovableAllocTestType t;
            ObjA2 s2(t, 1);
            mfState = TstFacility::getMovedFromState(s2.first);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA2 d2(t, 0);
            miState = TstFacility::getMovedIntoState(d2.first);
            ASSERT(MovState::e_NOT_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            d2 = MovUtil::move(s2);

            ASSERT(1 == d2.second);
            mfState = TstFacility::getMovedFromState(s2.first);
            miState = TstFacility::getMovedIntoState(d2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        //   template <class U1, class U2> pair& operator=(pair<U1, U2>&& rhs);
        // --------------------------------------------------------------------

        typedef bsl::pair<Base *,    bsltf::MovableAllocTestType> ObjA1;
        typedef bsl::pair<Derived *, bsltf::MovableAllocTestType> ObjB1;

        typedef bsl::pair<bsltf::MovableAllocTestType, Base *>    ObjA2;
        typedef bsl::pair<bsltf::MovableAllocTestType, Derived *> ObjB2;

        typedef bsl::pair<const bsltf::MoveOnlyAllocTestType, int> ObjA3;
        typedef bsl::pair<bsltf::MoveOnlyAllocTestType, int>       ObjB3;

        typedef bsltf::MoveState            MovState;
        typedef bslmf::MovableRefUtil       MovUtil;
        typedef bsltf::TemplateTestFacility TstFacility;

        bsltf::MoveState::Enum miState, mfState;
        {
            bsltf::MovableAllocTestType t;
            ObjB1 b1((Derived *) 0, t);
            mfState = TstFacility::getMovedFromState(b1.second);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 a1((Base *) 0, t);
            miState = TstFacility::getMovedIntoState(a1.second);
            ASSERT(MovState::e_NOT_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            a1 = MovUtil::move(b1);

            mfState = TstFacility::getMovedFromState(b1.second);
            miState = TstFacility::getMovedIntoState(a1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
        }
        {
            bsltf::MovableAllocTestType t;
            ObjB2 b2(t, (Derived *) 0);
            mfState = TstFacility::getMovedFromState(b2.first);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA2 a2(t, (Base *) 0);
            miState = TstFacility::getMovedIntoState(a2.first);
            ASSERT(MovState::e_NOT_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            a2 = MovUtil::move(b2);

            mfState = TstFacility::getMovedFromState(b2.first);
            miState = TstFacility::getMovedIntoState(a2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
        }
        {
            bsltf::MoveOnlyAllocTestType t(3);
            mfState = TstFacility::getMovedFromState(t);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            bsltf::MoveOnlyAllocTestType t2(3);
            ObjB3 b3(MovUtil::move(t2), 3);

            ObjA3 a3(MovUtil::move(t), 3);
            mfState = TstFacility::getMovedFromState(t);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            miState = TstFacility::getMovedIntoState(a3.first);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            // The following does not (and should not) compile because the
            // dest pair has a 'first' type that is 'const' so you can't
            // assign to it.  Left here because of the educational value.
            // a3 = MovUtil::move(b3);

            // mfState = TstFacility::getMovedFromState(b3.first);
            // miState = TstFacility::getMovedIntoState(a3.first);
            // ASSERT(MovState::e_MOVED == mfState
            //    || MovState::e_UNKNOWN == mfState);
            // ASSERT(MovState::e_MOVED == miState
            //    || MovState::e_UNKNOWN == miState);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        // template <class U1, class U2> pair(U1&& a, U2&& b);
        // template <class U1, class U2> pair(U1&& a, U2&& b, AllocatorPtr a);
        // --------------------------------------------------------------------
        typedef bsl::pair<Base *,    bsltf::MovableAllocTestType>  ObjA1;
        typedef bsl::pair<bsltf::MovableAllocTestType, Base *>     ObjA2;
        typedef bsl::pair<const bsltf::MoveOnlyAllocTestType, int> ObjA3;
        typedef bsl::pair<int, const bsltf::MoveOnlyAllocTestType> ObjA4;

        typedef bsltf::MoveState            MovState;
        typedef bslmf::MovableRefUtil       MovUtil;
        typedef bsltf::TemplateTestFacility TstFacility;

        bsltf::MoveState::Enum miState, mfState;

        // Without allocator
        {
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA1 a1(d, m);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a1.second);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_NOT_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA2 a2(m, d);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a2.first);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_NOT_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA1 a1(d, MovUtil::move(m));

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a1.second);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA2 a2(MovUtil::move(m), d);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a2.first);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
            }
            {
                bsltf::MoveOnlyAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA3 a3(MovUtil::move(m), 1);

                // The following should fail to compile.
                // ObjA3 a3(m, 1);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a3.first);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
                ASSERT(1 == TstFacility::getIdentifier(a3.first));
            }
            {
                bsltf::MoveOnlyAllocTestType m(1);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA4 a4(1, MovUtil::move(m));

                // The following should fail to compile.
                // ObjA4 a4(1, m);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a4.second);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
                ASSERT(1 == TstFacility::getIdentifier(a4.first));
            }
        }

        // With allocator
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocatorRaw(&da);
            bslma::TestAllocatorMonitor dam(&da);

            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA1 a1(d, m, &oa);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a1.second);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_NOT_MOVED == miState
                    || MovState::e_UNKNOWN == miState);

                ASSERT(&oa == a1.second.allocator());
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA2 a2(m, d, &oa);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a2.first);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_NOT_MOVED == miState
                    || MovState::e_UNKNOWN == miState);

                ASSERT(&oa == a2.first.allocator());
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA1 a1(d, MovUtil::move(m), &oa);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a1.second);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);

                ASSERT(&oa == a1.second.allocator());
            }
            {
                Derived *d = (Derived *) 0;

                bsltf::MovableAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA2 a2(MovUtil::move(m), d, &oa);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a2.first);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);
                ASSERT(&oa == a2.first.allocator());
            }
            {
                bsltf::MoveOnlyAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA3 a3(MovUtil::move(m), 1, &oa);

                // The following should fail to compile.
                // ObjA3 a3(m, 1);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a3.first);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);

                ASSERT(&oa == a3.first.allocator());
                ASSERT(1 == TstFacility::getIdentifier(a3.first));
            }
            {
                bsltf::MoveOnlyAllocTestType m(1, &oa);
                mfState = TstFacility::getMovedFromState(m);
                ASSERT(MovState::e_NOT_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);

                ObjA4 a4(1, MovUtil::move(m), &oa);

                // The following should fail to compile.
                // ObjA4 a4(1, m);

                mfState = TstFacility::getMovedFromState(m);
                miState = TstFacility::getMovedIntoState(a4.second);
                ASSERT(MovState::e_MOVED == mfState
                    || MovState::e_UNKNOWN == mfState);
                ASSERT(MovState::e_MOVED == miState
                    || MovState::e_UNKNOWN == miState);

                ASSERT(&oa == a4.second.allocator());
                ASSERT(1 == TstFacility::getIdentifier(a4.second));
            }
            // TBD: clearly the following is just a breathing test to ensure
            // that the lvalue references to const and non-const values are
            // begin processed correctly.
            {
                typedef ManagedWrapper<bsltf::SimpleTestType>  WrappedType;
                typedef bsl::pair<WrappedType, int>            ManagedType;
                bslma::ManagedPtr<bsltf::SimpleTestType> mp;
                ManagedType mt(mp, 0);
            }
            {
                typedef ManagedWrapper<bsltf::SimpleTestType>  WrappedType;
                typedef bsl::pair<int, WrappedType>            ManagedType;
                bslma::ManagedPtr<bsltf::SimpleTestType> mp;
                ManagedType mt(0, mp);
            }
            {
                typedef ManagedWrapper<bsltf::SimpleTestType>  WrappedType;
                typedef bsl::pair<WrappedType, WrappedType>    ManagedType;
                bslma::ManagedPtr<bsltf::SimpleTestType> mp1;
                bslma::ManagedPtr<bsltf::SimpleTestType> mp2;
                ManagedType mt(mp1, mp2);
            }
            ASSERT(dam.isTotalSame());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        //   pair(pair&&)
        //   pair(pair&&, AllocatorPtr)
        // --------------------------------------------------------------------
        typedef bsl::pair<int, bsltf::MovableAllocTestType>        ObjA1;
        typedef bsl::pair<bsltf::MovableAllocTestType, int>        ObjA2;
        typedef bsl::pair<const bsltf::MoveOnlyAllocTestType, int> ObjA3;

        typedef bsltf::TemplateTestFacility TstFacility;
        typedef bsltf::MoveState           MovState;

        bsltf::MoveState::Enum miState, mfState;
        // Without allocator
        {
            bsltf::MovableAllocTestType t1(1);
            bsltf::MovableAllocTestType t2(2);
            ObjA1 s1(1, t1);
            ObjA2 s2(t2, 2);

            mfState = TstFacility::getMovedFromState(t1);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            mfState = TstFacility::getMovedFromState(t2);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 d1(bslmf::MovableRefUtil::move(s1));
            mfState = TstFacility::getMovedFromState(s1.second);
            miState = TstFacility::getMovedIntoState(d1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
            ASSERT(1 == s1.first); ASSERT(1 == d1.first);
            ASSERT(1 == TstFacility::getIdentifier(d1.second));

            ObjA2 d2(bslmf::MovableRefUtil::move(s2));
            mfState = TstFacility::getMovedFromState(s2.first);
            miState = TstFacility::getMovedIntoState(d2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
            ASSERT(2 == s2.second); ASSERT(2 == d2.second);
            ASSERT(2 == TstFacility::getIdentifier(d2.first));

            bsltf::MoveOnlyAllocTestType m(3);
            ObjA3 s3(bslmf::MovableRefUtil::move(m), 3);
            mfState = TstFacility::getMovedFromState(m);
            miState = TstFacility::getMovedIntoState(s3.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
            ASSERT(3 == s3.second);
            ASSERT(3 == TstFacility::getIdentifier(s3.first));

            // The following does not (and should not) compile because the
            // source pair has a 'first' type that is 'const' so you can't
            // move from it.  Left here because of the educational value.

            // ObjA3 d3(bslmf::MovableRefUtil::move(s3));
            // mfState = TstFacility::getMovedFromState(s3.first);
            // miState = TstFacility::getMovedIntoState(d3.first);
            // ASSERT(MovState::e_MOVED == mfState
            //     || MovState::e_UNKNOWN == mfState);
            // ASSERT(MovState::e_MOVED == miState
            //     || MovState::e_UNKNOWN == miState);
            // ASSERT(3 == s3.second); ASSERT(3 == d3.second);
            // ASSERT(3 == TstFacility::getIdentifier(d3.first));
        }
        // With allocator
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocatorRaw(&da);
            bslma::TestAllocatorMonitor dam(&da);

            bsltf::MovableAllocTestType t1(1, &oa);
            bsltf::MovableAllocTestType t2(2, &oa);
            ObjA1 s1(1, t1, &oa);
            ObjA2 s2(t2, 2, &oa);

            mfState = TstFacility::getMovedFromState(t1);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            mfState = TstFacility::getMovedFromState(t2);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 d1(bslmf::MovableRefUtil::move(s1), &oa);
            mfState = TstFacility::getMovedFromState(s1.second);
            miState = TstFacility::getMovedIntoState(d1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
            ASSERT(1 == s1.first); ASSERT(1 == d1.first);
            ASSERT(1 == TstFacility::getIdentifier(d1.second));
            ASSERT(&oa == d1.second.allocator());

            ObjA2 d2(bslmf::MovableRefUtil::move(s2), &oa);
            mfState = TstFacility::getMovedFromState(s2.first);
            miState = TstFacility::getMovedIntoState(d2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
            ASSERT(2 == s2.second); ASSERT(2 == d2.second);
            ASSERT(2 == TstFacility::getIdentifier(d2.first));
            ASSERT(&oa == d2.first.allocator());

            ASSERT(dam.isTotalSame());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BREATHING TEST for
        //
        //   template <class U1, class U2> pair(pair<U1, U2>&&)
        //   template <class U1, class U2> pair(pair<U1, U2>&&, AllocatorPtr)
        // --------------------------------------------------------------------

        typedef bsl::pair<Base *,    bsltf::MovableAllocTestType>  ObjA1;
        typedef bsl::pair<Derived *, bsltf::MovableAllocTestType>  ObjB1;

        typedef bsl::pair<bsltf::MovableAllocTestType, Base *>     ObjA2;
        typedef bsl::pair<bsltf::MovableAllocTestType, Derived *>  ObjB2;

        typedef bsl::pair<const bsltf::MoveOnlyAllocTestType, int> ObjA3;
        typedef bsl::pair<bsltf::MoveOnlyAllocTestType, int>       ObjB3;

        typedef bsl::pair<int, const bsltf::MoveOnlyAllocTestType> ObjA4;
        typedef bsl::pair<int, bsltf::MoveOnlyAllocTestType>       ObjB4;

        typedef bsltf::TemplateTestFacility TstFacility;
        typedef bsltf::MoveState           MovState;

        bsltf::MoveState::Enum miState, mfState;
        // Without allocator
        {
            bsltf::MovableAllocTestType t;
            ObjB1 b1((Derived *) 0, t);
            ObjB2 b2(t, (Derived *) 0);

            mfState = TstFacility::getMovedFromState(t);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 a1(bslmf::MovableRefUtil::move(b1));
            mfState = TstFacility::getMovedFromState(b1.second);
            miState = TstFacility::getMovedIntoState(a1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ObjA2 a2(bslmf::MovableRefUtil::move(b2));
            mfState = TstFacility::getMovedFromState(b2.first);
            miState = TstFacility::getMovedIntoState(a2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            bsltf::MoveOnlyAllocTestType m1(1);
            ObjB3 b3(bslmf::MovableRefUtil::move(m1), 1);
            mfState = TstFacility::getMovedFromState(m1);
            miState = TstFacility::getMovedIntoState(b3.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ObjA3 a3(bslmf::MovableRefUtil::move(b3));
            ASSERT(1 == a3.second);
            ASSERT(1 == TstFacility::getIdentifier(a3.first));
            mfState = TstFacility::getMovedFromState(b3.first);
            miState = TstFacility::getMovedIntoState(a3.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            bsltf::MoveOnlyAllocTestType m2(2);
            ObjB4 b4(2, bslmf::MovableRefUtil::move(m2));
            mfState = TstFacility::getMovedFromState(m2);
            miState = TstFacility::getMovedIntoState(b4.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ObjA4 a4(bslmf::MovableRefUtil::move(b4));
            ASSERT(2 == a4.first);
            ASSERT(2 == TstFacility::getIdentifier(a4.second));
            mfState = TstFacility::getMovedFromState(b4.second);
            miState = TstFacility::getMovedIntoState(a4.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);
        }
        // With allocator
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocatorRaw(&da);
            bslma::TestAllocatorMonitor dam(&da);

            bsltf::MovableAllocTestType t(&oa);
            ObjB1 b1((Derived *) 0, t, &oa);
            ObjB2 b2(t, (Derived *) 0, &oa);

            mfState = TstFacility::getMovedFromState(t);
            ASSERT(MovState::e_NOT_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);

            ObjA1 a1(bslmf::MovableRefUtil::move(b1), &oa);
            mfState = TstFacility::getMovedFromState(b1.second);
            miState = TstFacility::getMovedIntoState(a1.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ObjA2 a2(bslmf::MovableRefUtil::move(b2), &oa);
            mfState = TstFacility::getMovedFromState(b2.first);
            miState = TstFacility::getMovedIntoState(a2.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            bsltf::MoveOnlyAllocTestType m1(1, &oa);
            ObjB3 b3(bslmf::MovableRefUtil::move(m1), 1, &oa);

            ObjA3 a3(bslmf::MovableRefUtil::move(b3), &oa);
            ASSERT(1 == a3.second);
            mfState = TstFacility::getMovedFromState(b3.first);
            miState = TstFacility::getMovedIntoState(a3.first);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            bsltf::MoveOnlyAllocTestType m2(2, &oa);
            ObjB4 b4(2, bslmf::MovableRefUtil::move(m2), &oa);
            mfState = TstFacility::getMovedFromState(m2);
            miState = TstFacility::getMovedIntoState(b4.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ObjA4 a4(bslmf::MovableRefUtil::move(b4));
            ASSERT(2 == a4.first);
            ASSERT(2 == TstFacility::getIdentifier(a4.second));
            mfState = TstFacility::getMovedFromState(b4.second);
            miState = TstFacility::getMovedIntoState(a4.second);
            ASSERT(MovState::e_MOVED == mfState
                || MovState::e_UNKNOWN == mfState);
            ASSERT(MovState::e_MOVED == miState
                || MovState::e_UNKNOWN == miState);

            ASSERT(dam.isTotalSame());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT TO 'pair' OF REFERENCES
        //
        // Concerns:
        //: 1 The assignment operator for a 'pair' of references is well-formed
        //:   and assigns to the original objects referred to by the 'pair'.
        //
        // Plan:
        //: 1 Create a target 'pair' holding:
        //:   a) a reference in 'first' and a value for 'second'
        //:   b) a reference in 'second' and a value for 'first'
        //:   c) a reference in both 'first' and 'second'
        //:
        //: 2 Create a value pair of values corresponding to the types used by
        //:   the target pairs above.
        //:
        //: 3 Assign from the value pair to the target pair, and confirm:
        //:   a) the address of any reference members of the target pair does
        //:      not change.
        //:   b) the value of any object referred to by the target pair has
        //:      changed
        //:   c) the values held by the value-pair have not changed
        //:
        //: 4 Repeat step (3) with a reference pair, holding two references
        //:   rather than two values.
        //
        // Testing:
        //   pair& operator=(const pair<U1, U2>& rhs)
        //   Concern: Can assign to a 'pair' of references
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT TO 'pair' OF REFERENCES"
                            "\n==========================================\n");

        // data for pairs

        int    i1 = 13;
        int    i2 = 42;
        int    i3 = 91;
        double d1 = 3.14159;
        double d2 = 2.71828;
        double d3 = 1.61803;

        const int    ORIGINAL_I1 = i1;
        const int    ORIGINAL_I2 = i2;
        const int    ORIGINAL_I3 = i3;
        const double ORIGINAL_D1 = d1;
        const double ORIGINAL_D2 = d2;
        const double ORIGINAL_D3 = d3;

        // Construct the pairs to assign-from

        bsl::pair<int, double> value(i1, d1);
        bsl::pair<int, double> reference(i2, d2);

        // test assign from 'value'

        {
            bsl::pair<int&, double > target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I1, i3, ORIGINAL_I1 == i3);
            ASSERTV(ORIGINAL_D3, d3, ORIGINAL_D3 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int,  double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&d3,  pd, &d3 == pd);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I3, i3, ORIGINAL_I3 == i3);
            ASSERTV(ORIGINAL_D1, d3, ORIGINAL_D1 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int&, double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);
            ASSERTV(&d3,  pd, &d3 == pd);

            target = value;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);

            ASSERTV(ORIGINAL_I1, i1, ORIGINAL_I1 == i1);
            ASSERTV(ORIGINAL_D1, d1, ORIGINAL_D1 == d1);
            ASSERTV(ORIGINAL_I1, i3, ORIGINAL_I1 == i3);
            ASSERTV(ORIGINAL_D1, d3, ORIGINAL_D1 == d3);

            target = target;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I1, target.first,  ORIGINAL_I1 == target.first );
            ASSERTV(ORIGINAL_D1, target.second, ORIGINAL_D1 == target.second);

            ASSERTV(ORIGINAL_I1, value.first,  ORIGINAL_I1 == value.first );
            ASSERTV(ORIGINAL_D1, value.second, ORIGINAL_D1 == value.second);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;


        // test assign from 'reference'

        {
            bsl::pair<int&, double > target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I2, i3, ORIGINAL_I2 == i3);
            ASSERTV(ORIGINAL_D3, d3, ORIGINAL_D3 == d3);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int,  double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&d3,  pd, &d3 == pd);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I3, i3, ORIGINAL_I3 == i3);
            ASSERTV(ORIGINAL_D2, d3, ORIGINAL_D2 == d3);
        }

        i1 = ORIGINAL_I1;
        i2 = ORIGINAL_I2;
        i3 = ORIGINAL_I3;
        d1 = ORIGINAL_D1;
        d2 = ORIGINAL_D2;
        d3 = ORIGINAL_D3;

        {
            bsl::pair<int&, double&> target(i3, d3);

            int    *pi = &target.first;
            double *pd = &target.second;

            ASSERTV(&i3,  pi, &i3 == pi);
            ASSERTV(&d3,  pd, &d3 == pd);

            target = reference;

            ASSERTV(&target.first,  pi, &target.first  == pi);
            ASSERTV(&target.second, pd, &target.second == pd);

            ASSERTV(ORIGINAL_I2, target.first,  ORIGINAL_I2 == target.first );
            ASSERTV(ORIGINAL_D2, target.second, ORIGINAL_D2 == target.second);

            ASSERTV(ORIGINAL_I2,   reference.first,
                    ORIGINAL_I2 == reference.first );
            ASSERTV(ORIGINAL_D2,   reference.second,
                    ORIGINAL_D2 == reference.second);

            ASSERTV(ORIGINAL_I2, i2, ORIGINAL_I2 == i2);
            ASSERTV(ORIGINAL_D2, d2, ORIGINAL_D2 == d2);
            ASSERTV(ORIGINAL_I2, i3, ORIGINAL_I2 == i3);
            ASSERTV(ORIGINAL_D2, d3, ORIGINAL_D2 == d3);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST FORMING A POINTER-TO-DATA-MEMBER
        //
        // Concerns:
        //: 1 We can use pointer to members to access both 'first' and 'second'
        //
        // Plan:
        //: 1 Create pointer to member to both 'first' and 'second' and check
        //:   that the behavior is as expected
        //
        // Testing:
        //   Concern: Can create a pointer-to-member for 'first' and 'second'
        // --------------------------------------------------------------------
        if (verbose) printf("\nTEST FORMING A POINTER-TO-DATA-MEMBER"
                            "\n=====================================\n");

        {
            typedef bsl::pair<int,const char*> PairType;

            int         PairType::*pfirst  = &PairType::first;
            const char *PairType::*psecond = &PairType::second;

            PairType p(10, "test7");

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }

        {
            typedef bsl::pair<char, long double> PairType;

            char        PairType::*pfirst  = &PairType::first;
            long double PairType::*psecond = &PairType::second;

            PairType p('c', 3.14l);

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }

        {
            typedef bsl::pair<long double, char> PairType;

            long double PairType::*pfirst  = &PairType::first;
            char        PairType::*psecond = &PairType::second;

            PairType p(2.78l, 'd');

            ASSERTV(&p.first,  &(p.*pfirst),  p.first   == (p.*pfirst));
            ASSERTV(&p.second, &(p.*psecond), p.second  == (p.*psecond));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //
        // Concerns:
        //: 1 Hashes different inputs differently
        //:
        //: 2 Hashes equal inputs identically
        //:
        //: 3 Works for const and non-const pairs, members
        //:
        //: 4 'hashAppend' for 'pair' correctly uses 'hashAppend' implemented
        //:   for the pair's template parameter types.
        //
        // Plan:
        //: 1 Create pairs, some equal and some not, some const, some not.
        //:   Hash them all. Compare hashes, identifying those that should be
        //:   equal and those that should not.  Call with different mixes of
        //:   constness, to verify that all compile. (C-1..3)
        //:
        //: 2 Create a 'hashAppend' for 'my_String', create a set of pairs
        //:   using 'my_String' values, and verify that pairs having the same
        //:   'my_String' value produce the same hash code. (C-4)
        //
        // Testing:
        //   hashAppend(HASHALG& hashAlg, const pair<T1,T2>&  input);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        typedef ::BloombergLP::bslh::Hash<> Hasher;
        typedef Hasher::result_type         HashType;

        {
            const char *ptr1 = "hello";
            const char *ptr2 = "goodbye";

            bsl::pair<int,const char*> p1(1,   ptr1);  // P-1
            bsl::pair<int,const char*> p2(1,   ptr1);
            bsl::pair<int,const char*> p3(100, ptr1);
            bsl::pair<int,const char*> p4(1,   ptr2);
            bsl::pair<int,const char*> p5(1,   ptr2);
            bsl::pair<int,const char*> p6(100, ptr2);

            bsl::pair<const int, const char *>      p7(1, ptr1);  // P-4
            const bsl::pair<int,const char * const> p8(1, ptr1);

            Hasher hasher;  // P-2
            HashType a1 = hasher(p1), a2 = hasher(p2), a3 = hasher(p3),
                     a4 = hasher(p4), a5 = hasher(p5), a6 = hasher(p6),
                     a7 = hasher(p7), a8 = hasher(p8);

            if (veryVerbose) {
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p1.first, p1.second, (unsigned long long) a1);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p2.first, p2.second, (unsigned long long) a2);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p3.first, p3.second, (unsigned long long) a3);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p4.first, p4.second, (unsigned long long) a4);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p5.first, p5.second, (unsigned long long) a5);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p6.first, p6.second, (unsigned long long) a6);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p7.first, p7.second, (unsigned long long) a7);
                printf("\tHash of (%d,\"%s\") is %llx\n",
                       p8.first, p8.second, (unsigned long long) a8);
            }

            // P-3

            ASSERT(a1 == a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);
            if (veryVerbose) {
                printf(
                  "\tp1=p2: %d, p1/p3: %d, p1/p4: %d, p1/p5: %d, p1/p6: %d\n",
                  int(a1 == a2), int(a1 != a3), int(a1 != a4),
                  int(a1 != a5), int(a1 != a6));
            }
            ASSERT(a2 != a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);
            if (veryVerbose) {
                printf(
                   "\tp2/p3: %d, p2/p4: %d, p2/p5: %d, p2/p6: %d\n",
                   int(a2 != a3), int(a2 != a4), int(a2 != a5), int(a2 != a6));
            }
            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);
            if (veryVerbose) {
                printf(
                   "\tp3/p4: %d, p3/p5: %d, p3/p6: %d\n",
                   int(a3 != a4), int(a3 != a5), int(a3 != a6));
            }
            ASSERT(a4 == a5);
            ASSERT(a4 != a6);
            if (veryVerbose) {
                printf(
                   "\tp4=p5: %d, p4/p6: %d\n",
                   int(a4 == a5), int(a4 != a6));
            }
            ASSERT(a5 != a6);
            if (veryVerbose) {
                printf(
                    "\tp5/p6: %d\n",
                    int(a5 != a6));
            }

            ASSERT(a7 == a8);
            ASSERT(a1 == a8);
            if (veryVerbose) {
                printf(
                    "\tp7=p8: %d, p1=p8: %d\n",
                    int(a7 == a8), int(a1 == a8));
            }
        }
        if (verbose) {
            printf("\tTesting hash on a pair with a user-defined type");
        }
        {
            bsl::pair<int, my_String> p1(1,   "stringA");  // P-1
            bsl::pair<int, my_String> p2(1,   "stringA");
            bsl::pair<int, my_String> p3(100, "stringA");
            bsl::pair<int, my_String> p4(1,   "stringB");
            bsl::pair<int, my_String> p5(1,   "stringB");
            bsl::pair<int, my_String> p6(100, "stringB");

            Hasher hasher;  // P-2
            HashType a1 = hasher(p1), a2 = hasher(p2), a3 = hasher(p3),
                     a4 = hasher(p4), a5 = hasher(p5), a6 = hasher(p6);

            ASSERT(a1 == a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);

            ASSERT(a2 != a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);

            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);

            ASSERT(a4 == a5);
            ASSERT(a4 != a6);

            ASSERT(a5 != a6);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
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
        //   void pair::swap(pair& rhs);
        //   void swap(pair& lhs, pair& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        swapTestHelper<TypeWithSwapNamespace::TypeWithSwap, TypeWithoutSwap>();
        swapTestHelper<TypeWithoutSwap, TypeWithSwapNamespace::TypeWithSwap>();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION CONSTRUCTORS
        //
        // Concerns:
        // - Can construct a 'bsl::pair' from a different instantiation of
        //   'pair' where each type is convertible.
        // - If either type uses a 'bslma::Allocator', then an allocator can
        //   be passed to the conversion constructor and is used to construct
        //   that member.
        // - If neither type uses a 'bslma::allocator', then any allocator
        //   argument is ignored.
        //
        // Plan:
        // - Construct 'pair<int, double>' from 'pair<char, int>'
        // - For each 'STRING' type in the list, 'my_String',
        //   'my_BslmaAllocArgStr', 'my_STLAllocArgStr', 'my_NoAllocStr':
        //   * Construct 'pair<STRING, int>' from 'pair<const char*, short>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<STRING, int>' from 'pair<const char*, short>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Construct 'pair<int, STRING>' from 'pair<short, const char*>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<int, STRING>' from 'pair<short, const char*>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Construct 'pair<STRING, STRING>' from 'pair<char*,const char*>',
        //     without supplying an allocator on construction.
        //   * Construct 'pair<STRING, STRING>' from 'pair<char*,const char*>',
        //     supplying a 'bslma::Allocator*' on construction.
        //   * Repeat the above except constructing 'bsl::pair from
        //     'native_std::pair'.
        // - When an allocator is not supplied on construction, verify that
        //   the correct default is used by the STRING in the constructed pair.
        // - When 'STRING' does not use a 'bslma::Allocator' and an allocator
        //   is supllied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the appropriate default allocator.
        // - When 'STRING' does use a 'bslma::Allocator' and an allocator
        //   is supllied on construction, verify that the 'STRING' value
        //   in the constructed pair uses the supplied allocator.
        // - Verify that there are no memory leaks.
        //
        // Testing:
        //   pair(const pair<U1, U2>& rhs);
        //   pair(const pair<U1, U2>& rhs, AllocatorPtr basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION CONSTRUCTORS"
                            "\n===============================\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);
        bslma::TestAllocator& ta3 = *my_STLCharAlloc::defaultMechanism();

        {
            bslma::DefaultAllocatorGuard allocGuard(&ta2);

            bsl::pair<char, int> p1('A', 8), &P1 = p1;
            bsl::pair<int, double> p2(P1), &P2 = p2;
            ASSERT('A' == P2.first);
            ASSERT(8 == P2.second);

            ASSERT(0 == ta1.numBlocksInUse());
            ASSERT(0 == ta2.numBlocksInUse());
            ASSERT(0 == ta3.numBlocksInUse());
        }

        if (veryVerbose) printf("Convert to 'my_String'\n");
        testBslmaStringConversionCtor<my_String>();
        if (veryVerbose) printf("Convert to 'my_BslmaAllocArgStr'\n");
        testBslmaStringConversionCtor<my_BslmaAllocArgStr>();
        if (veryVerbose) printf("Convert to 'my_STLAllocArgStr'\n");
        testNonBslmaStringConversionCtor<my_STLAllocArgStr>();
        if (veryVerbose) printf("Convert to 'my_NoAllocString'\n");
        testNonBslmaStringConversionCtor<my_NoAllocString>();

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
        //   Type Traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTRAITS TEST"
                            "\n===========\n");

        if (verbose) {
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
            printf("Testing traits of "
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
        //: 1 Can construct a 'pair' using the default constructor, constructor
        //:   with two arguments and copy constructor with no allocator.
        //: 2 If and only if one or both members of the 'pair' have the
        //:   'bslma::UsesBslmaAllocator' trait, then the 'pair' also has that
        //:   trait.
        //: 3 If 'pair' has the 'bslma::UsesBslmaAllocator' trait, then each
        //:   constructor can be passed a 'bslma::Allocator' pointer and that
        //:   pointer is passed through to the member(s) that take it.
        //: 4 Assignment works as designed.
        //: 5 Operators ==, !=, <, >, <=, and >= work as designed.
        //
        // Plan:
        // - Select a small set of interesting types:
        //   'short'              - Fundamental type
        //   'my_String'          - Uses 'bslma::Allocator' in constructor
        //   'my_BslmaAllocArgStr - Uses 'bslma::Allocator' in constructor by
        //                          means of the 'allocator_arg' idiom.
        //   'my_STLAllocArgStr   - Uses an STL-style allocator in constructor
        //                          by means of the 'allocator_arg' idiom.
        //   'my_NoAllocString'   - Doesn't use 'bslma::Allocator' in
        //                          constructor.
        // - Instantiate 'pair' with each combination (25 total) of the
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
        //   typedef T1 first_type;
        //   typedef T2 second_type;
        //   T1 first;
        //   T1 second;
        //   pair();
        //   pair(AllocatorPtr basicAllocator);
        //   pair(const T1& a, const T2& b);
        //   pair(const T1& a, const T2& b, AllocatorPtr basicAllocator);
        //   pair(const pair& original);
        //   pair(const pair& original, AllocatorPtr basicAllocator);
        //   ~pair();
        //   pair& operator=(const pair& rhs);
        //   bool operator==(const pair& x, const pair& y);
        //   bool operator!=(const pair& x, const pair& y);
        //   bool operator<(const pair& x, const pair& y);
        //   bool operator>(const pair& x, const pair& y);
        //   bool operator<=(const pair& x, const pair& y);
        //   bool operator>=(const pair& x, const pair& y);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTIONALITY TEST"
                            "\n==================\n");

        ASSERT(! bslma::UsesBslmaAllocator<short              >::value);
        ASSERT(  bslma::UsesBslmaAllocator<my_String          >::value);
        ASSERT(  bslma::UsesBslmaAllocator<my_BslmaAllocArgStr>::value);
        ASSERT(! bslma::UsesBslmaAllocator<my_STLAllocArgStr  >::value);
        ASSERT(! bslma::UsesBslmaAllocator<my_NoAllocString   >::value);

#define TEST(T1, T2) do {                                               \
            if (veryVerbose) printf("Testing pair<%s,%s>\n", #T1, #T2); \
            testFunctionality<T1, T2>(                                  \
                bslma::UsesBslmaAllocator<bsl::pair<T1,T2> >());        \
        } while (false)

        TEST(short              , short              );
        TEST(short              , my_String          );
        TEST(short              , my_BslmaAllocArgStr);
        TEST(short              , my_STLAllocArgStr  );
        TEST(short              , my_NoAllocString   );

        TEST(my_String          , short              );
        TEST(my_String          , my_String          );
        TEST(my_String          , my_BslmaAllocArgStr);
        TEST(my_String          , my_STLAllocArgStr  );
        TEST(my_String          , my_NoAllocString   );

        TEST(my_BslmaAllocArgStr, short              );
        TEST(my_BslmaAllocArgStr, my_String          );
        TEST(my_BslmaAllocArgStr, my_BslmaAllocArgStr);
        TEST(my_BslmaAllocArgStr, my_STLAllocArgStr  );
        TEST(my_BslmaAllocArgStr, my_NoAllocString   );

        TEST(my_STLAllocArgStr  , short              );
        TEST(my_STLAllocArgStr  , my_String          );
        TEST(my_STLAllocArgStr  , my_BslmaAllocArgStr);
        TEST(my_STLAllocArgStr  , my_STLAllocArgStr  );
        TEST(my_STLAllocArgStr  , my_NoAllocString   );

        TEST(my_NoAllocString   , short              );
        TEST(my_NoAllocString   , my_String          );
        TEST(my_NoAllocString   , my_BslmaAllocArgStr);
        TEST(my_NoAllocString   , my_STLAllocArgStr  );
        TEST(my_NoAllocString   , my_NoAllocString   );

#undef TEST

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Instantiate 'pair' with a simple string class that uses
        //:    'bslma::Allocator'.
        //: 2 Construct a objects using the test allocator.
        //: 3 Verify that object members have the correct value.
        //: 4 Verify that that the correct allocator was used.
        //: 5 Verify that there are no memory leaks.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);

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
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
