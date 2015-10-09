// bslalg_arraydestructionprimitives.t.cpp                            -*-C++-*-

#include <bslalg_arraydestructionprimitives.h>

#include <bslalg_scalarprimitives.h>             // for testing only

#include <bslmf_istriviallycopyable.h>           // for testing only
#include <bslma_allocator.h>                     // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bsls_alignmentutil.h>                  // for testing only
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>                   // for testing only
#include <bsls_assert.h>                         // for testing only
#include <bsls_asserttest.h>                     // for testing only
#include <bsls_types.h>                          // for testing only

#include <ctype.h>      // 'isalpha'
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'strlen'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides a single algorithm to destroy ranges.
// The main concern (besides that the objects are actually destroyed) is that
// the destructor calls are elided if the basic object type is bit-wise
// copyable.
//
// In order to facilitate the generation of test object instances, we make a
// text object have the value semantics of a 'char', and generate an array of
// test objects from a string specification via a generating function
// parameterized by the actual test object type.  This lets us reuse the same
// test code for bitwise-copyable/moveable test types as well as those that do
// not have those traits.
//-----------------------------------------------------------------------------
// bslalg::ArrayPrimitives public interface:
// [ 2] void destroy(T *dstB, T *dstE);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

static bool g_veryVerbose = false;

typedef bslalg::ArrayDestructionPrimitives  Obj;

// TYPES
class TestType;
class TestTypeNoAlloc;
class BitwiseCopyableTestType;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // does not use 'bslma' allocators

typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

// STATIC DATA
const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma::TestAllocator *Z;  // initialized at the start of main()

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char             *d_data_p;
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }

    explicit TestType(char c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    explicit TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        if (&original != this) {
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }
    }

    ~TestType()
    {
        ++numDestructorCalls;
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
        char *newData = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p  = newData;
        *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c)
    {
        *d_data_p = c;
    }

    // ACCESSORS
    char datum() const
    {
        return *d_data_p;
    }

    void print() const
    {
        if (d_data_p) {
        ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <> struct UsesBslmaAllocator<TestType> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

bool operator==(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

                       // =====================
                       // class TestTypeNoAlloc
                       // =====================

class TestTypeNoAlloc {
    // This test type has footprint and interface identical to 'TestType'.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.  It does not allocate, and thus could have the
    // bit-wise copyable trait, but we defer this to the
    // 'BitwiseCopyableTestType'.

    // DATA
    union {
        char                                    d_char;
        char                                    d_fill[sizeof(TestType)];
        bsls::AlignmentFromType<TestType>::Type d_align;
    } d_u;

  public:
    // CREATORS
    TestTypeNoAlloc()
    {
        d_u.d_char = '?';
        ++numDefaultCtorCalls;
    }

    explicit TestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        ++numCharCtorCalls;
    }

    TestTypeNoAlloc(const TestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~TestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    TestTypeNoAlloc& operator=(const TestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
    char datum() const
    {
        return d_u.d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }
};

bool operator==(const TestTypeNoAlloc& lhs,
                const TestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

                       // =============================
                       // class BitwiseCopyableTestType
                       // =============================

class BitwiseCopyableTestType : public TestTypeNoAlloc {
    // This test type is identical to 'TestTypeNoAlloc' except that it has the
    // bit-wise copyable trait.  All members are inherited.

  public:
    // CREATORS
    BitwiseCopyableTestType()
    : TestTypeNoAlloc()
    {
    }

    explicit BitwiseCopyableTestType(char c)
    : TestTypeNoAlloc(c)
    {
        ++numCharCtorCalls;
    }

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : TestTypeNoAlloc(original.datum())
    {
    }
};

// TRAITS
namespace bsl {
template <> struct is_trivially_copyable<BitwiseCopyableTestType>
    : true_type {};
}  // close namespace bsl

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
class CleanupGuard {
    // This proctor is responsible to create, in an array specified at
    // construction, a sequence according to some specification.  Upon
    // destruction, it destroys elements in that array according to the current
    // specifications.  For '0 <= i < strlen(spec)', 'array[i]' is destroyed if
    // and only if '1 == isalpha(spec[i])' and in addition, if a reference to
    // an end pointer is specified at construction, if 'i < *specEnd - spec'.
    // If a tests succeeds, the specifications can be changed to allow for
    // different (un)initialized elements.

    // DATA
    TYPE        *d_array_p;
    const char  *d_spec_p;
    TYPE       **d_endPtr_p;
    TYPE        *d_initialEndPtr_p;
    size_t       d_length;

  public:
    // CREATORS
    CleanupGuard(TYPE *array, const char *spec, TYPE**endPtr = 0)
    : d_array_p(array)
    , d_spec_p(spec)
    , d_endPtr_p(endPtr)
    , d_initialEndPtr_p(endPtr ? *endPtr : 0)
    , d_length(strlen(spec))
    {
    }

    ~CleanupGuard()
    {
        for (int i = 0; d_spec_p[i] && i < static_cast<int>(d_length); ++i) {
            char c = d_spec_p[i];
            if (isalpha(c)) {
                if (d_endPtr_p && *d_endPtr_p - d_array_p <= i &&
                                           i < d_initialEndPtr_p - d_array_p) {
                    continue; // those elements have already been moved
                }
                Obj::destroy(d_array_p + i, d_array_p + i + 1);
            }
            else {
                LOOP_ASSERT(i, '_' == c);
            }
        }
    }

    // MANIPULATORS
    void setLength(int length)
    {
        d_length = length;
    }

    void release(const char *newSpec)
    {
        d_spec_p = newSpec;
        d_length = strlen(newSpec);
        d_endPtr_p = 0;
    }
};

template <class TYPE>
void cleanup(TYPE *array, const char *spec)
    // Destroy elements in the specified 'array' according to the specified
    // 'spec'.  For '0 <= i < strlen(spec)', 'array[i]' is destroyed if and
    // only if '1 == isalpha(spec[i])'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP_ASSERT(i, array[i].datum() == c);
            Obj::destroy(array + i, array + i + 1);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

template <class TYPE>
void verify(TYPE *array, const char *spec)
    // Verify that elements in the specified 'array' have values according to
    // the specified 'spec'.
{
    for (int i = 0; spec[i]; ++i) {
        char c = spec[i];
        if (isalpha(c)) {
            LOOP3_ASSERT(i, array[i].datum(), c, array[i].datum() == c);
        }
        else {
            LOOP_ASSERT(i, '_' == c);
        }
    }
}

void fillWithJunk(void *buf, int size)
{
    const int MAX_VALUE = 127;

    char *p = reinterpret_cast<char*>(buf);

    for (int i = 0; i < size; ++i) {
        p[i] = (char)((i % MAX_VALUE) + 1);
    }
}

//=============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure an array according to a custom language.  Letters
// [a .. z, A .. Z] correspond to arbitrary (but unique) char values used to
// initialize elements of an array of 'T' objects.  An underscore ('_')
// indicates that an element should be left uninitialized.
//
// LANGUAGE SPECIFICATION
// ----------------------
//
// <SPEC>   ::=  <EMPTY> | <LIST>
//
// <EMPTY>  ::=
//
// <LIST>   ::=  <ITEM>  | <ITEM><LIST>
//
// <ITEM>   ::=  <VALUE> | <UNSET>
//
// <VALUE>  ::=  'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' |
//               'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' |
//               's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' |
//               'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//               'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//               'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
//
// <UNSET>  ::=  '_'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Leaves the array unaffected.
// "a"          ...
//-----------------------------------------------------------------------------

template <class TYPE>
int ggg(TYPE *array, const char *spec, int verboseFlag = 1)
    // Configure the specified 'array' of objects of the parameterized 'TYPE'
    // (assumed to be uninitialized) according to the specified 'spec'.
    // Optionally specify a zero 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
    //
    // Note that this generator is used in exception tests, and thus need to be
    // exception-safe.
{
    CleanupGuard<TYPE> guard(array, spec);
    guard.setLength(0);

    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i, ++array) {
        char c = spec[i];
        guard.setLength(i);
        if (isalpha(c)) {
            bslalg::ScalarPrimitives::construct(array, c, Z);
        }
        else if ('_' == c) {
            continue;
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') in spec \"%s\""
                       " at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
    }
    guard.setLength(0);
    return SUCCESS;
}

template <class TYPE>
TYPE& gg(TYPE *array, const char *spec)
    // Return a reference to the modifiable first element of the specified
    // 'array' after the value of 'array' has been adjusted according to the
    // specified 'spec'.
{
    ASSERT(ggg(array, spec) < 0);
    return *array;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR CASE 2
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;   // source line number
    const char *d_spec;      // specification string
    int         d_begin;     // start of [begin, end) range
    int         d_ne;        // number of elements (ne = end - begin).
    const char *d_expected;  // expected result array
} DATA_2[] = {
    // Order test data by increasing 'ne'.

    //line spec         begin    ne      expected
    //---- ----         -----    --      --------
    { L_,  "___",       1,       0,      "___"             },  // 0
    { L_,  "_b_",       1,       0,      "_b_"             },
    { L_,  "abc",       1,       0,      "abc"             },

    { L_,  "_b_",       1,       1,      "___"             },  // 1
    { L_,  "abc",       1,       1,      "a_c"             },

    { L_,  "_bc_",      1,       2,      "____"            },  // 2
    { L_,  "abcd",      1,       2,      "a__d"            },

    { L_,  "abcde",     1,       3,      "a___e"           },  // 3

    { L_,  "abcdef",    1,       4,      "a____f"          },  // 4

    { L_,  "abcdefg",   1,       5,      "a_____g"         },  // 5
};
const int NUM_DATA_2 = sizeof DATA_2 / sizeof *DATA_2;

template <class TYPE>
void testDestroy(bool bitwiseCopyableFlag)
{
    const int MAX_SIZE = 16;
    static union {
        char                                d_raw[MAX_SIZE * sizeof(T)];
        bsls::AlignmentUtil::MaxAlignedType d_align;
    } u;
    T *buf = (T*)&u.d_raw[0];

    for (int ti = 0; ti < NUM_DATA_2; ++ti) {
        const int         LINE  = DATA_2[ti].d_lineNum;
        const char *const SPEC  = DATA_2[ti].d_spec;
        const int         BEGIN = DATA_2[ti].d_begin;
        const int         NE    = DATA_2[ti].d_ne;
        const char *const EXP   = DATA_2[ti].d_expected;
        ASSERT(MAX_SIZE >= (int)strlen(SPEC));

        if (g_veryVerbose) {
            printf("LINE = %d, SPEC = %s, "
                   "BEGIN = %d, NE = %d, EXP = %s\n",
                   LINE, SPEC, BEGIN, NE, EXP);
        }
        gg(buf, SPEC);  verify(buf, SPEC);

        const int NUM_DESTRUCTIONS = numDestructorCalls;

        Obj::destroy(&buf[BEGIN], &buf[BEGIN + NE]);

        if (bitwiseCopyableFlag) {
            ASSERT(NUM_DESTRUCTIONS == numDestructorCalls);
        }
        verify(buf, EXP);
        cleanup(buf, EXP);
    }
}

// ============================================================================
//                                  USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.  Note that this
// component is for use by the 'bslstl' package.  Other clients should use the
// STL algorithms (in header '<algorithm>' and '<memory>').
//
///Example 1: Destroy Arrays of 'int' and 'Integer' Wrapper Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use 'bslalg::ArrayDestructionPrimitives' to destroy
// both an array of integer scalars and an array of 'MyInteger' objects.
// Calling the 'destory' method on an array of integers is a no-op while
// calling the 'destroy' method on an array of objects of 'MyInteger' class
// invokes the destructor of each of the objects in the array.
//
// First, we define a 'MyInteger' class that contains an integer value:
//..
    class MyInteger {
        // This class represents an integer value.
//
        int d_intValue;  // integer value
//
      public:
        // CREATORS
        MyInteger();
            // Create a 'MyInteger' object having integer value '0'.
//
        explicit MyInteger(int value);
            // Create a 'MyInteger' object having the specified 'value'.
//
        ~MyInteger();
            // Destroy this object.
//
        // ACCESSORS
        int getValue() const;
            // Return the integer value contained in this object.
    };
//..

// CREATORS
MyInteger::MyInteger()
:d_intValue(0)
{
}

MyInteger::MyInteger(int value)
:d_intValue(value)
{
}

MyInteger::~MyInteger()
{
}

// ACCESSORS
int MyInteger::getValue() const
{
    return d_intValue;
}

}  // close namespace UsageExample

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

    g_veryVerbose = veryVerbose;

    (void)veryVeryVerbose;      // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");
        using namespace UsageExample;

//..
// Then, we create an array of of objects, 'myIntegers', of type 'MyInteger'
// (note that we 'bsls::ObjectBuffer' to allow us to safely invoke the
// destructor explicitly):
//..
    bsls::ObjectBuffer<MyInteger> arrayBuffer[5];
    MyInteger *myIntegers = &arrayBuffer[0].object();
    for (int i = 0;i < 5; ++i) {
        new (myIntegers + i) MyInteger(i);
    }
//..
// Now, we define a primitive integer array:
//..
    int scalarIntegers[] = { 0, 1, 2, 3, 4 };
//..
// Finally, we use the uniform 'bslalg::ArrayDestructionPrimitives:destroy'
// method to destroy both 'myIntegers' and 'scalarIntegers':
//..
    bslalg::ArrayDestructionPrimitives::destroy(myIntegers, myIntegers + 5);
    bslalg::ArrayDestructionPrimitives::destroy(scalarIntegers,
                                                scalarIntegers + 5);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING destroy
        //
        // Concerns:
        //: 1. The 'destroy' acts as a uniform interface to destroy arrays of
        //:    objects of different types as expected.
        //
        // Plan:
        //: 1. Construct arrays of objects of types that have different type
        //:    traits declared.  Call the 'destroy' method on them and verify
        //:    they are destroyed as expected.  (C-1)
        //
        // Testing:
        //   void destroy(T *b, T *e, *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'destroy'\n");

        if (verbose) printf("\n\t...with TestTypeNoAlloc.\n");
        testDestroy<TNA>(false);

        if (verbose) printf("\n\t...with TestType.\n");
        testDestroy<T>(false);

        if (verbose) printf("\n\t...with BitwiseCopyableTestType.\n");
        testDestroy<BCT>(false);


        if(verbose) printf("\nNegative testing\n");
        {
            bsls::AssertFailureHandlerGuard g(
                    bsls::AssertTest::failTestDriver);

            int * null = 0;
            ASSERT_SAFE_PASS(Obj::destroy(null, null));
            int simpleArray[] = { 0, 1, 2, 3, 4 };
            int * begin = simpleArray;
            int * end = begin;
            ASSERT_SAFE_FAIL(Obj::destroy(null, begin));
            ASSERT_SAFE_FAIL(Obj::destroy(begin, null));
            ASSERT_SAFE_PASS(Obj::destroy(begin, begin));

            ++begin; ++begin;  // Advance begin by two to form an invalid range
            ++end;
            ASSERT_SAFE_FAIL(Obj::destroy(begin, end));
            ++end;
            ASSERT(begin == end);
            ASSERT_SAFE_PASS(Obj::destroy(begin, end));
            ++end;
            ASSERT_SAFE_PASS(Obj::destroy(begin, end));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1. The 'destroy' algorithm works as intended.
        //
        // Plan:
        //: 1. Construct objects in a range and use 'destroy' to destroy
        //     them.  Make sure all memory is deallocated.  (C-1)
        //
        // Testing:
        //   This test exercises the component but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta(veryVerbose);

        numDestructorCalls = 0;
        {
            const int NUM_OBJECTS = 8;
            const T   VALUE('a');

            bsls::ObjectBuffer<T> array[NUM_OBJECTS];
            T *buffer = &array[0].object();

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                bslalg::ScalarPrimitives::copyConstruct(buffer + i,
                                                        VALUE,
                                                        &ta);
            }

            Obj::destroy(buffer, buffer + NUM_OBJECTS);

            ASSERT(NUM_OBJECTS == numDestructorCalls);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());

        numDestructorCalls = 0;
        {
            const int NUM_OBJECTS = 8;
            const TNA VALUE('a');

            bsls::ObjectBuffer<TNA> array[NUM_OBJECTS];
            TNA *buffer = &array[0].object();

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                bslalg::ScalarPrimitives::copyConstruct(buffer + i,
                                                        VALUE,
                                                        &ta);
            }

            Obj::destroy(buffer, buffer + NUM_OBJECTS);

            ASSERT(NUM_OBJECTS == numDestructorCalls);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());

        {
            const int NUM_OBJECTS = 8;
            const BCT VALUE('a');

            bsls::ObjectBuffer<BCT> array[NUM_OBJECTS];
            BCT *buffer = &array[0].object();

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                bslalg::ScalarPrimitives::copyConstruct(buffer + i,
                                                        VALUE,
                                                        &ta);
            }

            Obj::destroy(buffer, buffer + NUM_OBJECTS);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());

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
