// bslstl_containerbase.t.cpp                                         -*-C++-*-

#include <bslstl_containerbase.h>
#include <bslstl_allocator.h>      // for testing only
#include <bslma_default.h>         // for testing only

#include <cstdio>
#include <cstdlib>

#include <new>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

template <class T, class ALLOC>
class my_FixedSizeArray : public bslstl::ContainerBase<ALLOC>
{
    typedef bslstl::ContainerBase<ALLOC> Base;

    int    d_length;
    T     *d_array;

  public:
    typedef ALLOC  allocator_type;
    typedef T      value_type;

    // CREATORS

    my_FixedSizeArray(int length, const ALLOC& alloc = ALLOC());

    my_FixedSizeArray(const my_FixedSizeArray& rhs,
                      const ALLOC&             alloc = ALLOC());

    ~my_FixedSizeArray();

    // MANIPULATORS
    T& operator[](int i);

    // ACCESSORS
    const T& operator[](int i) const;

    int length() const;
};

template<class T, class ALLOC>
my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(int          length,
                                              const ALLOC& alloc)
    : Base(alloc), d_length(length)
{
    d_array = this->allocator().allocate(d_length); // sizeof(T)*d_length bytes

    // Default construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        this->allocator().construct(&d_array[i], T());
    }
}

template<class T, class ALLOC>
my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(
    const my_FixedSizeArray& rhs,
    const ALLOC&             alloc)
    : Base(alloc), d_length(rhs.d_length)
{
    d_array = this->allocator().allocate(d_length); // sizeof(T)*d_length bytes

    // copy construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        this->allocator().construct(&d_array[i], rhs.d_array[i]);
    }
}

template<class T, class ALLOC>
my_FixedSizeArray<T,ALLOC>::~my_FixedSizeArray()
{
    // Call destructor for each element
    for (int i = 0; i < d_length; ++i) {
        this->allocator().destroy(&d_array[i]);
    }

    // Return memory to allocator.
    this->allocator().deallocate(d_array, d_length);
}

template<class T, class ALLOC>
inline T& my_FixedSizeArray<T,ALLOC>::operator[](int i)
{
    return d_array[i];
}

template<class T, class ALLOC>
inline
const T& my_FixedSizeArray<T,ALLOC>::operator[](int i) const
{
    return d_array[i];
}

template<class T, class ALLOC>
inline int my_FixedSizeArray<T,ALLOC>::length() const
{
    return d_length;
}

template<class T, class ALLOC>
bool operator==(const my_FixedSizeArray<T,ALLOC>& lhs,
                const my_FixedSizeArray<T,ALLOC>& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;
    }
    for (int i = 0; i < lhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        my_FixedSizeArray<int, bsl::allocator<int> > a1(10);
        ASSERT(a1.get_allocator() == bsl::allocator<int>());
        ASSERT(a1.get_allocator() == bslma::Default::defaultAllocator());

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
