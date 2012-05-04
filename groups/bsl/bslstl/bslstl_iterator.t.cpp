// bslstl_iterator.t.cpp                                              -*-C++-*-

#include <bslstl_iterator.h>
#include <bslstl_allocator.h>
#include <bslma_default.h>

#include <cstdio>
#include <cstdlib>

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
//--------------------------------------------------------------------------

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

namespace mine {

template <class T, class ALLOC>
class MyFixedSizeArray
{
    ALLOC  d_allocator;
    int    d_length;
    T     *d_array;

  public:
    typedef ALLOC  allocator_type;
    typedef T      value_type;

    typedef T                                      *iterator;
    typedef T const                                *const_iterator;
    typedef bsl::reverse_iterator<iterator>         reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>   const_reverse_iterator;

    // CREATORS

    MyFixedSizeArray(int length, const ALLOC& alloc = ALLOC());

    MyFixedSizeArray(const MyFixedSizeArray& rhs,
                     const ALLOC&            alloc = ALLOC());

    ~MyFixedSizeArray();

    // MANIPULATORS
    iterator begin();
    iterator end();
    reverse_iterator rbegin();
    reverse_iterator rend();

    T& operator[](int i);

    // ACCESSORS
    const_iterator begin() const;
    const_iterator end() const;
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    const T& operator[](int i) const;

    int length() const;

    const ALLOC& allocator() const;
    // This is here for illustrative purposes.  We should not
    // generally have an accessor to return the allocator.
};

template<class T, class ALLOC>
MyFixedSizeArray<T,ALLOC>::MyFixedSizeArray(int          length,
                                            const ALLOC& alloc)
    : d_allocator(alloc), d_length(length)
{
    d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

    // Default construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        d_allocator.construct(&d_array[i], T());
    }
}

template<class T, class ALLOC>
MyFixedSizeArray<T,ALLOC>::MyFixedSizeArray(const MyFixedSizeArray& rhs,
                                            const ALLOC&            alloc)
    : d_allocator(alloc), d_length(rhs.d_length)
{
    d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes

    // copy construct each element of the array:
    for (int i = 0; i < d_length; ++i) {
        d_allocator.construct(&d_array[i], rhs.d_array[i]);
    }
}

template<class T, class ALLOC>
MyFixedSizeArray<T,ALLOC>::~MyFixedSizeArray()
{
    // Call destructor for each element
    for (int i = 0; i < d_length; ++i) {
        d_allocator.destroy(&d_array[i]);
    }

    // Return memory to allocator.
    d_allocator.deallocate(d_array, d_length);
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::iterator
MyFixedSizeArray<T,ALLOC>::begin()
{
    return d_array;
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::iterator
MyFixedSizeArray<T,ALLOC>::end()
{
    return d_array + d_length;
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::reverse_iterator
MyFixedSizeArray<T,ALLOC>::rbegin()
{
    return reverse_iterator(end());
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::reverse_iterator
MyFixedSizeArray<T,ALLOC>::rend()
{
    return reverse_iterator(begin());
}

template<class T, class ALLOC>
inline T& MyFixedSizeArray<T,ALLOC>::operator[](int i)
{
    return d_array[i];
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::const_iterator
MyFixedSizeArray<T,ALLOC>::begin() const
{
    return d_array;
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::const_iterator
MyFixedSizeArray<T,ALLOC>::end() const
{
    return d_array + d_length;
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::const_reverse_iterator
MyFixedSizeArray<T,ALLOC>::rbegin() const
{
    return end();
}

template<class T, class ALLOC>
typename MyFixedSizeArray<T,ALLOC>::const_reverse_iterator
MyFixedSizeArray<T,ALLOC>::rend() const
{
    return begin();
}

template<class T, class ALLOC>
inline
const T& MyFixedSizeArray<T,ALLOC>::operator[](int i) const
{
    return d_array[i];
}

template<class T, class ALLOC>
inline int MyFixedSizeArray<T,ALLOC>::length() const
{
    return d_length;
}

template<class T, class ALLOC>
inline
const ALLOC& MyFixedSizeArray<T,ALLOC>::allocator() const
{
    return d_allocator;
}

template<class T, class ALLOC>
bool operator==(const MyFixedSizeArray<T,ALLOC>& lhs,
                const MyFixedSizeArray<T,ALLOC>& rhs)
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

}  // close namespace mine

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

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

        using namespace mine;

        MyFixedSizeArray<int, bsl::allocator<int> > a1(5);
                ASSERT(5 == a1.length());
                ASSERT(bslma::Default::defaultAllocator() == a1.allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }

        typedef
            MyFixedSizeArray<int, bsl::allocator<int> >::reverse_iterator
            revIter;
        revIter start = a1.rbegin();
        revIter finish = a1.rend();
        for (int i = a1.length(); start != finish; ++start, --i) {
            ASSERT(*start == i);
            ASSERT(i == (finish - start));
        }

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
