// bslmf_integersequence.t.cpp                                        -*-C++-*-
#include <bslmf_integersequence.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <limits>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace bslmf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-function that represents a
// compile-time integer sequence, 'bslmf::IntegerSequence'.  Thus, we need to
// ensure that an integer sequences represent collections of integer values of
// the specified length.
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 2] bslmf::IntegerSequence<class T, T ...Ints>
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] Ensure local helper class works as expected.

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE

//=============================================================================
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#define TEST_EMPTY_INTEGER_SEQUENCE(T)                                   \
{                                                                        \
    using Obj1 = IntegerSequence<                         T>;            \
    ASSERT((bsl::is_same<Obj1::value_type,                T>::value));   \
    ASSERT(0 == Obj1::size());                                           \
    using Obj2 = IntegerSequence<          const          T>;            \
    ASSERT((bsl::is_same<Obj2::value_type, const          T>::value));   \
    ASSERT(0 == Obj2::size());                                           \
    using Obj3 = IntegerSequence<                volatile T>;            \
    ASSERT((bsl::is_same<Obj3::value_type,       volatile T>::value));   \
    ASSERT(0 == Obj3::size());                                           \
    using Obj4 = IntegerSequence<          const volatile T>;            \
    ASSERT((bsl::is_same<Obj4::value_type, const volatile T>::value));   \
    ASSERT(0 == Obj4::size());                                           \
}
// Test all cv-qualified combination on the specified 'T'.

namespace {

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

                              //================
                              // struct ItemUtil
                              //================

template <std::size_t INDEX, class T>
struct ItemUtil {
    // This 'struct' provides an namespace for utility mata-function operations
    // using parameter packs.
  private:
    // PRIVATE CLASS METHODS
    template <class ...IS>
    static
    constexpr T getNext(T, IS... s)
        // Recursively call 'value(s...)' function for an index value that
        // precedes the value of the specified template non-type parameter
        // 'INDEX'.
    {
        return ItemUtil<INDEX-1, T>::value(s...);
    }

  public:
    // CLASS METHODS
    template <class ...IS>
    static
    constexpr T value(IS... s)
        // Return the N-th item of the specified parameter pack 's...'.
    {
        static_assert(INDEX < sizeof...(IS), "");
        return getNext(s...);
    }
};

template <class T>
struct ItemUtil<0, T> {
    // This partial specialization of 'ItemUtil' provides an access to the
    // first item of an parameter pack.
  private:
    // PRIVATE CLASS METHODS
    template <class ...IS>
    static
    constexpr T getFirst(T v, IS...)
        // Return the first item 'v' of an parameter pack.
    {
        return v;
    }

    static
    constexpr T getFirst(T v)
        // Return the first item 'v' of an parameter pack having a single item.
    {
        return v;
    }

public:
    // CLASS METHODS
    template <class ...IS>
    static
    constexpr T value(IS ...s)
        // Return the first item the specified parameter pack.
    {
        return getFirst(s...);
    }
};

template <std::size_t N, class T, T... IS>
constexpr T getValue(IntegerSequence<T, IS...>)
    // Return the N-th item of the specified integer sequence.
{
    static_assert(N < sizeof...(IS), "");
    return ItemUtil<N, T>::value(IS...);
}

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Pass C-array as a parameter to a function with variadic template
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Suppose we want to initialize a C-Array of known size 'N' with data read
// from a data source using a library class that provides a variadic template
// interface that loads a data of variable length into the supplied parameter
// pack.
//
// First, define a class template 'DataReader',
//..
template <std::size_t t_N>
class DataReader {
  public:
//..
// Then, implement a method that loads the specified parameter pack 'args' with
// data read from a data source.
//..
    template <class ...t_T>
    void read(t_T*... args) const
    {
        static_assert(sizeof...(args) == t_N, "");
        read_impl(args...);
    }
//..
// Next, for the test purpose provide simple implementation of the recursive
// variadic 'read_impl' function that streams the number of the C-Array's
// element to 'stdout'.
//..
private:
    template <class t_U, class ...t_T>
    void read_impl(t_U*, t_T*... args) const
    {
        printf("read element #%i\n",
               static_cast<int>(t_N - 1 - sizeof...(args)));
        read_impl(args...);
    }
//..
// Then, implement the recursion break condition:
//..
    void read_impl() const
    {
    }
};
//..
// Next, define a helper function template 'readData' that expands the
// parameter pack of indices 't_I' and invokes the variadic template 'read'
// method of the specified 'reader' object.
//..
namespace {
template<class t_R, class t_T, std::size_t... t_I>
void readData(const t_R&  reader,
              t_T        *data,
              bslmf::IntegerSequence<std::size_t, t_I...>)
{
    reader.read(&data[t_I]...);
        // In pseudocode, this is equivalent to:
        // reader.read(&data[0],
        //             &data[1],
        //             &data[2],
        //             ...
        //             &data[t_N-1]);
}
}
//..

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                              "=============\n");

// Finally, define a 'data' C-Array and 'reader' variables and pass them to the
// 'readData' function as parameters.
//..
        constexpr int      k_SIZE = 5;
        DataReader<k_SIZE> reader;
        int                data[k_SIZE] = {0};

        readData(reader,
                 data,
                 bslmf::IntegerSequence<size_t, 0, 1, 2, 3, 4>());
//..
// Note that using a direct call to the 'bslmf::IntegerSequence' constructor
// looks a bit clumsy here.  The better approach is to use alias template
// 'bslmf::MakeIntegerSequence', that creates a collection of increasing
// integer values, having the specified N-value length.  The usage example in
// that component shows this method more clearly.  But we can not afford its
// presence here to avoid a cycle/levelization violation.
//
// The streaming operator produces output in the following format on 'stdout':
//..
// read element #0
// read element #1
// read element #2
// read element #3
// read element #4
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IntegerSequence'
        //
        // Concerns:
        //: 1 That the 'value_type' is the same as the template parameter of
        //:   the template 'struct' 'bslmf::IntegerSequence'.
        //:
        //: 2 That the length of an integer sequence equals to the size of the
        //:   template parameter pack.
        //:
        //: 3 That 'size()' can be used in constant expressions.
        //:
        //: 4 That 'T' can be any integer type.
        //:
        //: 5 That repeated values are supported, i.e.
        //:  'bslmf::IntegerSequence<int, 0, 0, 0, 0, 0>'.
        //:
        //: 6 That both positive and negative if any values of the parameter
        //:   type 'T' are supported, i.e. 'IntegerSequence<int, -1, 0, 1>'.
        //:
        //: 7 That a super-long sequences (having 1024 paramnetrs) are
        //:   supported.
        //
        // Plan:
        //: 1 Define a number of integer sequences of various integer types,
        //:   having distinct lengths and ensure that these sequences represent
        //:   collections of integer values having the specified length and
        //:   values.
        //:
        //: 2 Ensure that 'noexcept(size()' is 'true'. (C-3)
        //:
        //: 3 Test at least one super-long integer sequence having length equal
        //:   to 1023.
        //
        // Testing:
        //   bslmf::IntegerSequence<class T, T... Ints>
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bslmf::IntegerSequence'\n"
                              "========================\n");

        { // C-3
            using Obj = IntegerSequence<int>;
            ASSERTV(L_, noexcept(Obj::size()));
        }

        { // C-4
            TEST_EMPTY_INTEGER_SEQUENCE(         bool     );
            TEST_EMPTY_INTEGER_SEQUENCE(         char     );
            TEST_EMPTY_INTEGER_SEQUENCE(  signed char     );
            TEST_EMPTY_INTEGER_SEQUENCE(unsigned char     );
#if defined BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
            TEST_EMPTY_INTEGER_SEQUENCE(         char8_t  );
#endif
            TEST_EMPTY_INTEGER_SEQUENCE(         wchar_t  );
#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
            TEST_EMPTY_INTEGER_SEQUENCE(         char16_t );
            TEST_EMPTY_INTEGER_SEQUENCE(         char32_t );
#endif
            TEST_EMPTY_INTEGER_SEQUENCE(         short    );
            TEST_EMPTY_INTEGER_SEQUENCE(unsigned short    );
            TEST_EMPTY_INTEGER_SEQUENCE(         int      );
            TEST_EMPTY_INTEGER_SEQUENCE(unsigned int      );
            TEST_EMPTY_INTEGER_SEQUENCE(         long     );
            TEST_EMPTY_INTEGER_SEQUENCE(unsigned long     );
            TEST_EMPTY_INTEGER_SEQUENCE(         long long);
            TEST_EMPTY_INTEGER_SEQUENCE(unsigned long long);
        }

        { // C-1,2,5,6
            using Type = int;

            using Obj0 = IntegerSequence<Type>;
            ASSERTV(L_, (bsl::is_same<Type, Obj0::value_type>::value));
            ASSERTV(L_, 0 == Obj0::size());

            using Obj1 = IntegerSequence<Type, 0>;
            ASSERTV(L_, (bsl::is_same<Type, Obj1::value_type>::value));
            ASSERTV(L_, 1 == Obj1::size());
            ASSERTV(L_, 0 == getValue<0>(Obj1()));

            using Obj2 = IntegerSequence<Type, 0, 0>;
            ASSERTV(L_, (bsl::is_same<Type, Obj2::value_type>::value));
            ASSERTV(L_, 2 == Obj2::size());
            ASSERTV(L_, 0 == getValue<0>(Obj2()));
            ASSERTV(L_, 0 == getValue<1>(Obj2()));

            using Obj3 = IntegerSequence<Type, 0, 0, 0>;
            ASSERTV(L_, (bsl::is_same<Type, Obj3::value_type>::value));
            ASSERTV(L_, 3 == Obj3::size());
            ASSERTV(L_, 0 == getValue<0>(Obj3()));
            ASSERTV(L_, 0 == getValue<1>(Obj3()));
            ASSERTV(L_, 0 == getValue<2>(Obj3()));

            using Obj4 = IntegerSequence<Type, 0, 0, 0, 0>;
            ASSERTV(L_, (bsl::is_same<Type, Obj4::value_type>::value));
            ASSERTV(L_, 4 == Obj4::size());
            ASSERTV(L_, 0 == getValue<0>(Obj4()));
            ASSERTV(L_, 0 == getValue<1>(Obj4()));
            ASSERTV(L_, 0 == getValue<2>(Obj4()));
            ASSERTV(L_, 0 == getValue<3>(Obj4()));

            using Obj5 = IntegerSequence<Type, -2, -1, 0, 1, 2>;
            ASSERTV(L_, (bsl::is_same<Type, Obj5::value_type>::value));
            ASSERTV(L_,  5 == Obj5::size());
            ASSERTV(L_, -2 == getValue<0>(Obj5()));
            ASSERTV(L_, -1 == getValue<1>(Obj5()));
            ASSERTV(L_,  0 == getValue<2>(Obj5()));
            ASSERTV(L_,  1 == getValue<3>(Obj5()));
            ASSERTV(L_,  2 == getValue<4>(Obj5()));

            const Type k_MAX = std::numeric_limits<Type>::max();
            using Obj6 = IntegerSequence<Type, k_MAX, 11, 0, 0, -11, -k_MAX>;
            ASSERTV(L_, (bsl::is_same<Type, Obj6::value_type>::value));
            ASSERTV(L_,      6 == Obj6::size());
            ASSERTV(L_,  k_MAX == getValue<0>(Obj6()));
            ASSERTV(L_,     11 == getValue<1>(Obj6()));
            ASSERTV(L_,      0 == getValue<2>(Obj6()));
            ASSERTV(L_,      0 == getValue<3>(Obj6()));
            ASSERTV(L_,    -11 == getValue<4>(Obj6()));
            ASSERTV(L_, -k_MAX == getValue<5>(Obj6()));
        }
        {  // C-7
           using Obj  = IntegerSequence<int, 0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  100,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  200,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  300,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  400,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  500,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  600,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  700,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  800,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,  900,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   30,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   40,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   50,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   60,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   70,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   80,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   90,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0, 1000,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   10,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0,   20,
                                             0, 0,                      1023>;
            ASSERTV(L_, (bsl::is_same<int, Obj::value_type>::value));
            ASSERTV(L_, 1023 == Obj::size());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   Ensure that the 'ItemUtil' and 'getValue' meta-functions work as
        //   expected.
        //
        // Concerns:
        //: 1 That the 'ItemUtil<N>' meta-function returns the value of an item
        //:   of the specified parameter pack at the specified location 'N'.
        //:
        //: 2 That the 'getValue<N>' meta-function returns the value of an item
        //:   of the specified integer sequence at the specified location 'N'.
        //
        // Plan:
        //: 1 Ensure that 'ItemUtil' returns expected values of all items in
        //:   the parameter pack.
        //:
        //: 2 Ensure that 'getValue' invokes 'ItemUtil<N>' meta-function and
        //:   returns expected values of all items in an integer sequence.
        //
        // Testing:
        //   template <size_t N, class T>
        //       template <class IS> T ItemUtil::value(IS... s);
        //   template <size_t N, class T, T... IS>
        //       T getValue(IntegerSequence<T, IS...>);
        // --------------------------------------------------------------------

          if (verbose) printf("\nHELPER CLASS TEST\n"
                                "=================\n");

          if (veryVerbose) { T_ printf("Test 'struct ItemUtil\n"); }

          using Type = int;
  // --------------------------------------------------------------------------
  // C-1                    Index:            0  1  2  3  4  5  6  7  8   9
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 3 == ItemUtil<0, Type>::value(3                            )));
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 6 == ItemUtil<0, Type>::value(6, 0                         )));
  ASSERTV(L_, ( 9 == ItemUtil<1, Type>::value(0, 9                         )));
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 1 == ItemUtil<0, Type>::value(1, 0, 0                      )));
  ASSERTV(L_, ( 4 == ItemUtil<1, Type>::value(0, 4, 0                      )));
  ASSERTV(L_, ( 8 == ItemUtil<2, Type>::value(0, 0, 8                      )));
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 9 == ItemUtil<0, Type>::value(9, 0, 0, 0, 0, 0, 0, 0       )));
  ASSERTV(L_, ( 6 == ItemUtil<1, Type>::value(0, 6, 0, 0, 0, 0, 0, 0       )));
  ASSERTV(L_, ( 5 == ItemUtil<2, Type>::value(0, 0, 5, 0, 0, 0, 0, 0       )));
  ASSERTV(L_, ( 1 == ItemUtil<3, Type>::value(0, 0, 0, 1, 0, 0, 0, 0       )));
  ASSERTV(L_, ( 2 == ItemUtil<4, Type>::value(0, 0, 0, 0, 2, 0, 0, 0       )));
  ASSERTV(L_, ( 8 == ItemUtil<5, Type>::value(0, 0, 0, 0, 0, 8, 0, 0       )));
  ASSERTV(L_, ( 3 == ItemUtil<6, Type>::value(0, 0, 0, 0, 0, 0, 3, 0       )));
  ASSERTV(L_, ( 4 == ItemUtil<7, Type>::value(0, 0, 0, 0, 0, 0, 0, 4       )));
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 9 == ItemUtil<0, Type>::value(9, 0, 0, 0, 0, 0, 0, 0, 0    )));
  ASSERTV(L_, ( 6 == ItemUtil<1, Type>::value(0, 6, 0, 0, 0, 0, 0, 0, 0    )));
  ASSERTV(L_, ( 5 == ItemUtil<2, Type>::value(0, 0, 5, 0, 0, 0, 0, 0, 0    )));
  ASSERTV(L_, ( 1 == ItemUtil<3, Type>::value(0, 0, 0, 1, 0, 0, 0, 0, 0    )));
  ASSERTV(L_, ( 2 == ItemUtil<4, Type>::value(0, 0, 0, 0, 2, 0, 0, 0, 0    )));
  ASSERTV(L_, ( 8 == ItemUtil<5, Type>::value(0, 0, 0, 0, 0, 8, 0, 0, 0    )));
  ASSERTV(L_, ( 3 == ItemUtil<6, Type>::value(0, 0, 0, 0, 0, 0, 3, 0, 0    )));
  ASSERTV(L_, ( 4 == ItemUtil<7, Type>::value(0, 0, 0, 0, 0, 0, 0, 4, 0    )));
  ASSERTV(L_, ( 7 == ItemUtil<8, Type>::value(0, 0, 0, 0, 0, 0, 0, 0, 7    )));
  // --------------------------------------------------------------------------
  ASSERTV(L_, ( 9 == ItemUtil<0, Type>::value(9, 0, 0, 0, 0, 0, 0, 0, 0,  0)));
  ASSERTV(L_, ( 6 == ItemUtil<1, Type>::value(0, 6, 0, 0, 0, 0, 0, 0, 0,  0)));
  ASSERTV(L_, ( 5 == ItemUtil<2, Type>::value(0, 0, 5, 0, 0, 0, 0, 0, 0,  0)));
  ASSERTV(L_, ( 1 == ItemUtil<3, Type>::value(0, 0, 0, 1, 0, 0, 0, 0, 0,  0)));
  ASSERTV(L_, ( 2 == ItemUtil<4, Type>::value(0, 0, 0, 0, 2, 0, 0, 0, 0,  0)));
  ASSERTV(L_, ( 8 == ItemUtil<5, Type>::value(0, 0, 0, 0, 0, 8, 0, 0, 0,  0)));
  ASSERTV(L_, ( 3 == ItemUtil<6, Type>::value(0, 0, 0, 0, 0, 0, 3, 0, 0,  0)));
  ASSERTV(L_, ( 4 == ItemUtil<7, Type>::value(0, 0, 0, 0, 0, 0, 0, 4, 0,  0)));
  ASSERTV(L_, ( 7 == ItemUtil<8, Type>::value(0, 0, 0, 0, 0, 0, 0, 0, 7,  0)));
  ASSERTV(L_, (-1 == ItemUtil<9, Type>::value(0, 0, 0, 0, 0, 0, 0, 0, 0, -1)));

          if (veryVerbose) { T_ printf("Test 'getValue() helper function\n"); }

          // ------------------------------------------------------------------
          // C-2              Index:         0  1  2  3  4  5  6
          // ------------------------------------------------------------------
          using Obj = IntegerSequence<Type, 7, 6, 2, 1, 5, 3, 9>;
          ASSERTV(L_, getValue<0>(Obj()) ==  7                   );
          ASSERTV(L_, getValue<1>(Obj()) ==     6                );
          ASSERTV(L_, getValue<2>(Obj()) ==        2             );
          ASSERTV(L_, getValue<3>(Obj()) ==           1          );
          ASSERTV(L_, getValue<4>(Obj()) ==              5       );
          ASSERTV(L_, getValue<5>(Obj()) ==                 3    );
          ASSERTV(L_, getValue<6>(Obj()) ==                    9 );

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

#else

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    ASSERT(true); // remove unused warning for 'aSsErT'

    printf("Cannot test 'bslmf::IntegerSequence' in pre-C++11 mode.\n");
    return -1;
}

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
