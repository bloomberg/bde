// bslstl_span.t.cpp                                                  -*-C++-*-
#include <bslstl_span.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bslstl_array.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 2] span() noexcept;
// [ 2] span(pointer, size_type);
// [ 2] span(pointer, pointer);
// [ 2] span(element_type (&arr)[SIZE]) noexcept;
// [ 2] span(const span &) noexcept;
// [ 2] span(span<T_OTHER_TYPE>) noexcept;
// [ 2] operator=(const span &) noexcept;
// [ 3] span(array<value_type, SIZE>&) noexcept;
// [ 3] span(const array<value_type, SIZE>&) noexcept;
// [ 3] span(std::array<value_type, SIZE>&) noexcept;
// [ 3] span(const std::array<value_type, SIZE>&) noexcept;
// [ 3] span(CONTAINER& c);
// [ 3] span(const CONTAINER& c);
// [ 4] reference front();
// [ 4] reference back();
// [ 4] reference operator[](size_type);
// [ 4] reference at[](size_type);
// [ 4] bool empty() noexcept;
// [ 4] size_type extent;
// [ 4] size_type size() noexcept;
// [ 4] size_type size_bytes() noexcept;
// [ 4] pointer data() noexcept;
// [ 5] template <size_t COUNT> first();
// [ 5] template <size_t COUNT> last();
// [ 5] first(size_t count);
// [ 5] last(size_t count);
// [ 5] template <size_t COUNT, size_t OFFSET> subspan();
// [ 5] subspan(size_t count, size_t offset);
// [ 6] iterator begin() noexcept;
// [ 6] iterator end() noexcept;
// [ 6] reverse_iterator rbegin() noexcept;
// [ 6] reverse_iterator rend() noexcept;
// [ 7] void swap(span a, span b) noexcept;
// [ 7] span<const byte> as_bytes(span) noexcept;
// [ 7] span<byte> as_writable_bytes(span) noexcept;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CLASS TEMPLATE DEDUCTION GUIDES
// [ 9] USAGE EXAMPLE 1
// [10] USAGE EXAMPLE 2

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// We want to test the negative `noexcept`-ness of some functions in span, but
// only if we're not using `std::span`, because all three implementations of
// std::span add `noexcept` to `front`, `back`, `first`, `last`, and `subspan`,
// as well as the `(pointer, pointer)` and `(pointer, size)` constructors.
#ifndef BSLSTL_SPAN_IS_ALIASED
    #define NOEXCEPT_TEST_ONLY_BSL_SPAN                                       1
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#define ASSERT_NOEXCEPT(...)       ASSERT( noexcept(__VA_ARGS__))
#define ASSERT_NOT_NOEXCEPT(...)   ASSERT(!noexcept(__VA_ARGS__))
#else
#define ASSERT_NOEXCEPT(...)
#define ASSERT_NOT_NOEXCEPT(...)
#endif

// We want to test `span::at` only if we're using our own implementation or we
// have a C++26 compliant standard library.
#if !defined(BSLSTL_SPAN_IS_ALIASED) ||                                      \
     defined(BSLS_LIBRARYFEATURES_HAS_CPP26_BASELINE_LIBRARY)
    #define TEST_BSL_SPAN_AT                                                  1
#endif

using namespace BloombergLP;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// First, we create a template function that takes a generic container.  This
// function inspects each of the (numeric) values in the container, and if the
// low bit is set, flips it.  This has the effect of turning odd values into
// even values.
// ```

/// Make every value in the specified container `c` even.
template <class CONTAINER>
void MakeEven(CONTAINER &c)
{
    for (typename CONTAINER::iterator it = c.begin(); it != c.end(); ++it) {
        if (*it & 1) {
            *it ^= 1;
        }
    }
}
// ```


// First, we create the vector.  Then we define our function that returns a
// `slice` from the vector.

/// Return a span into the specified `vec`, starting at the specified
/// `first` index, and continuing up to (but not including) the specified
/// `last` index.
bsl::span<const int> slice(const bsl::vector<int>& vec,
                           size_t                  first,
                           size_t                  last)
{
    return bsl::span<const int>(vec.data() + first, last-first);
}

/// Test the basic constructors and assignment operators for bsl::span
void TestBasicConstructors()
{
    int arr [10];

    for (int i = 0; i < 10; ++i) {
        arr[i]  = i;
    }

    // default constructors
    {
        bsl::span<int, 0> defS;
        bsl::span<int>    defD;

        ASSERT_NOEXCEPT(bsl::span<int, 0>());
        ASSERT_NOEXCEPT(bsl::span<int   >());

        ASSERT(NULL == defS.data());
        ASSERT(0    == defS.size());
        ASSERT(NULL == defD.data());
        ASSERT(0    == defD.size());
    }

// MSVC erroneously reports the two constructors (pointer, size) and (pointer,
// pointer) as noexcept.  The trigger appears to be declaring them as
// `constexpr`.  I reported to MS on 10-May-2024. They replied (on 13-May):
//
// The short answer is: compile with /permissive-.
//
// The longer answer: MSVC in permissive mode has an extension that allows the
// compiler to adhere to a language rule prior to DR
// https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1351 and we
// can't change it due to back-compat.

    // pointer, size
    {
        bsl::span<int, 5> psS(&arr[5], 5);
        bsl::span<int>    psD(&arr[3], 3);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN) &&                                   \
      !(defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION <= 1999))
        ASSERT_NOT_NOEXCEPT(bsl::span<int, 5>(&arr[5], 5));
        ASSERT_NOT_NOEXCEPT(bsl::span<int   >(&arr[5], 5));
#endif

        ASSERT(&arr[5] == psS.data());
        ASSERT(5       == psS.size());
        ASSERT(&arr[3] == psD.data());
        ASSERT(3       == psD.size());
    }

    // pointer, pointer
    {
        bsl::span<int, 5> ppS(&arr[5], &arr[10]);
        bsl::span<int>    ppD(&arr[3], &arr[6]);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN) &&                                   \
      !(defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION <= 1999))
        ASSERT_NOT_NOEXCEPT(bsl::span<int, 5>(&arr[5], &arr[10]));
        ASSERT_NOT_NOEXCEPT(bsl::span<int   >(&arr[3], &arr[ 6]));
#endif

        ASSERT(&arr[5] == ppS.data());
        ASSERT(5       == ppS.size());
        ASSERT(&arr[3] == ppD.data());
        ASSERT(3       == ppD.size());
    }

    // from a C-style array
    {
        bsl::span<int, 10> arrS(arr);
        bsl::span<int>     arrD(arr);

        ASSERT_NOEXCEPT(bsl::span<int, 10>(arr));
        ASSERT_NOEXCEPT(bsl::span<int    >(arr));

        ASSERT(&arr[0] == arrS.data());
        ASSERT(10      == arrS.size());
        ASSERT(&arr[0] == arrD.data());
        ASSERT(10      == arrD.size());
    }

    // from a span of a compatible type
    {
        bsl::span<int, 5>       sS(&arr[5], 5);
        bsl::span<int>          sD(&arr[3], 3);
        bsl::span<const int, 5> sS1(sS);
        bsl::span<const int>    sD1a(sD);
        bsl::span<const int>    sD1b(sS);
        bsl::span<const int, 3> sS2a(sD);

#if !(BSLS_PLATFORM_OS_DARWIN                          \
      && BSLS_PLATFORM_CMP_CLANG                       \
      && BSLS_PLATFORM_CMP_VERSION<=180000)
        ASSERT_NOEXCEPT(bsl::span<const int, 5>(sS));
#endif
        ASSERT_NOEXCEPT(bsl::span<const int   >(sS));
        ASSERT_NOEXCEPT(bsl::span<const int, 3>(sD));
        ASSERT_NOEXCEPT(bsl::span<const int   >(sD));

        ASSERT(&arr[5] == sS1.data());
        ASSERT(5       == sS1.size());
        ASSERT(&arr[3] == sD1a.data());
        ASSERT(3       == sD1a.size());
        ASSERT(&arr[5] == sD1b.data());
        ASSERT(5       == sD1b.size());
        ASSERT(&arr[3] == sS2a.data());
        ASSERT(3       == sS2a.size());
    }

    // copy constructor and assignment
    {
        bsl::span<int, 5> psS(&arr[5], 5);
        bsl::span<int>    psD(&arr[3], 3);
        bsl::span<int, 5> psS1(psS);
        bsl::span<int>    psD1a(psD);
        bsl::span<int>    psD1b(psS);

        ASSERT_NOEXCEPT(bsl::span<int, 5>(psS));
        ASSERT_NOEXCEPT(bsl::span<int   >(psS));
        ASSERT_NOEXCEPT(bsl::span<int   >(psD));

        ASSERT(&arr[5] == psS1.data());
        ASSERT(5       == psS1.size());
        ASSERT(&arr[3] == psD1a.data());
        ASSERT(3       == psD1a.size());
        ASSERT(&arr[5] == psD1b.data());
        ASSERT(5       == psD1b.size());

        bsl::span<int, 5> psS2(&arr[3], 5);
        bsl::span<int>    psD2a;
        bsl::span<int>    psD2b;

        psS2  = psS;
        psD2a = psD;
        psD2b = psS;

        ASSERT_NOEXCEPT(psS2  = psS);
        ASSERT_NOEXCEPT(psD2a = psD);
        ASSERT_NOEXCEPT(psD2b = psS);

        ASSERT_NOEXCEPT(bsl::span<int, 5>(psS));
        ASSERT_NOEXCEPT(bsl::span<int   >(psD));
        ASSERT_NOEXCEPT(bsl::span<int, 5>(psS));
        ASSERT_NOEXCEPT(bsl::span<int   >(psD));

        ASSERT(&arr[5] == psS2.data());
        ASSERT(5       == psS2.size());
        ASSERT(&arr[3] == psD2a.data());
        ASSERT(3       == psD2a.size());
        ASSERT(&arr[5] == psD2b.data());
        ASSERT(5       == psD2b.size());
    }

    // constexpr copy constructor and assignment
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    {
        constexpr static int val1 = 5;

        constexpr bsl::span<const int, 1> psS(&val1, 1);
        constexpr bsl::span<const int>    psD(&val1, 1);
        constexpr bsl::span<const int, 1> psS1(psS);
        constexpr bsl::span<const int>    psD1(psD);

        static_assert(1     == psS1.size());
        static_assert(&val1 == psS1.data());
        static_assert(5     == psS1[0]);

        static_assert(1     == psD1.size());
        static_assert(&val1 == psD1.data());
        static_assert(5     == psD1[0]);
    }
#endif

}


/// Test the construction of a bsl::span from various containers
void TestContainerConstructors()
{
#ifndef BSLSTL_ARRAY_IS_ALIASED
    bsl::array<int, 10>        arr;
    const bsl::array<int, 10>& cArr = arr;

    for (int i = 0; i < 10; ++i) {
        arr[i] = 10 - i;
    }

    // from a bsl::array
    {
        bsl::span<int, 10> arrS(arr);
        bsl::span<int>     arrD(arr);

        ASSERT_NOEXCEPT(bsl::span<int, 10>(arr));
        ASSERT_NOEXCEPT(bsl::span<int>    (arr));

        ASSERT(arr.data() == arrS.data());
        ASSERT(10         == arrS.size());
        ASSERT(arr.data() == arrD.data());
        ASSERT(10         == arrD.size());
    }

    // from a const bsl::array
    {
        bsl::span<const int, 10> carrS(cArr);
        bsl::span<const int>     carrD(cArr);

        ASSERT_NOEXCEPT(bsl::span<const int, 10>(cArr));
        ASSERT_NOEXCEPT(bsl::span<const int>    (cArr));

        ASSERT(cArr.data() == carrS.data());
        ASSERT(10          == carrS.size());
        ASSERT(cArr.data() == carrD.data());
        ASSERT(10          == carrD.size());
    }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    std::array<int, 10>        sArr;
    const std::array<int, 10>& cSArr = sArr;

    for (int i = 0; i < 10; ++i) {
        sArr[i] = 10 - i;
    }

    // from a std::array
    {
        bsl::span<int, 10> arrS(sArr);
        bsl::span<int>     arrD(sArr);

        ASSERT_NOEXCEPT(bsl::span<int, 10>(sArr));
        ASSERT_NOEXCEPT(bsl::span<int>    (sArr));

        ASSERT(sArr.data() == arrS.data());
        ASSERT(10          == arrS.size());
        ASSERT(sArr.data() == arrD.data());
        ASSERT(10          == arrD.size());
    }

    // from a const std::array
    {
        bsl::span<const int, 10> carrS(cSArr);
        bsl::span<const int>     carrD(cSArr);

        ASSERT_NOEXCEPT(bsl::span<const int, 10>(cSArr));
        ASSERT_NOEXCEPT(bsl::span<const int>    (cSArr));

        ASSERT(cSArr.data() == carrS.data());
        ASSERT(10           == carrS.size());
        ASSERT(cSArr.data() == carrD.data());
        ASSERT(10           == carrD.size());
    }

#endif

    bsl::vector<int>           vec(30);
    const bsl::vector<int>&    cVec = vec;

    // from a bsl::vector (dynamic span only)
    {
        bsl::span<int> arrD(vec);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN)
        ASSERT_NOT_NOEXCEPT(bsl::span<int>(vec));
#endif

        ASSERT(vec.data() == arrD.data());
        ASSERT(30         == arrD.size());
    }

    // from a const bsl::vector (dynamic span only)
    {
        bsl::span<const int> carrD(cVec);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN)
        ASSERT_NOT_NOEXCEPT(bsl::span<const int>(cVec));
#endif

        ASSERT(cVec.data() == carrD.data());
        ASSERT(30          == carrD.size());
    }

    bsl::string           str("ABCDE");
    const bsl::string&    cStr = str;
    // from a bsl::string (dynamic span only)
    {
        bsl::span<char> strD(str);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN)
        ASSERT_NOT_NOEXCEPT(bsl::span<char      >(str));
        ASSERT_NOT_NOEXCEPT(bsl::span<const char>(str));
#endif

        ASSERT(str.data() == strD.data());
        ASSERT(5          == strD.size());

        bsl::span<const char> cstrD(str);
        ASSERT(str.data() == cstrD.data());
        ASSERT(5          == cstrD.size());
    }

    // from a const bsl::string (dynamic span only)
    {
        bsl::span<const char> cstrD(cStr);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN)
        ASSERT_NOT_NOEXCEPT(bsl::span<const char>(cStr));
#endif

        ASSERT(str.data() == cstrD.data());
        ASSERT(5          == cstrD.size());
    }

    bsl::string_view      sv(str);
    // from a string_view (dynamic span only)
    {
        bsl::span<const char> cstrD(sv);

#if defined(NOEXCEPT_TEST_ONLY_BSL_SPAN)
        ASSERT_NOT_NOEXCEPT(bsl::span<const char>(sv));
#endif

        ASSERT(sv.data() == cstrD.data());
        ASSERT(5         == cstrD.size());
    }
}

#ifdef TEST_BSL_SPAN_AT
/// Return `true` if accessing the `idx`th element of the span `s` throws an
/// `out_of_range` exception
template <class TYPE, size_t EXTENT>
bool doesAtThrow(bsl::span<TYPE, EXTENT> s, size_t idx)
{
    try {
        (void) s.at(idx);
    }
    catch (const std::out_of_range&) {
        return true;
    }
    return false;
}
#endif


/// Test the accessors of bsl::span.
void TestAccessors()
{
    int arr [10];

    for (int i = 0; i < 10; ++i) {
        arr[i]  = i;
    }

    bsl::span<      int, 10> sSpan (&arr[0], 10);
    bsl::span<const int,  4> csSpan(&arr[5], 4);
    bsl::span<      int>     dSpan (&arr[0], 10);
    bsl::span<const int>     cdSpan(&arr[5], 4);
    bsl::span<int, 0>        zsSpan;
    bsl::span<int>           zdSpan;

    // extent
    ASSERT(10                  ==  sSpan.extent);
    ASSERT(4                   == csSpan.extent);
    ASSERT(bsl::dynamic_extent ==  dSpan.extent);
    ASSERT(bsl::dynamic_extent == cdSpan.extent);
    ASSERT(0                   == zsSpan.extent);
    ASSERT(bsl::dynamic_extent == zdSpan.extent);

    // size
    ASSERT_NOEXCEPT( sSpan.size());
    ASSERT_NOEXCEPT(csSpan.size());
    ASSERT_NOEXCEPT( dSpan.size());
    ASSERT_NOEXCEPT(cdSpan.size());
    ASSERT_NOEXCEPT(zsSpan.size());
    ASSERT_NOEXCEPT(zdSpan.size());

    ASSERT(10 ==  sSpan.size());
    ASSERT(4  == csSpan.size());
    ASSERT(10 ==  dSpan.size());
    ASSERT(4  == cdSpan.size());
    ASSERT(0  == zsSpan.size());
    ASSERT(0  == zdSpan.size());

    // size_bytes
    ASSERT_NOEXCEPT( sSpan.size_bytes());
    ASSERT_NOEXCEPT(csSpan.size_bytes());
    ASSERT_NOEXCEPT( dSpan.size_bytes());
    ASSERT_NOEXCEPT(cdSpan.size_bytes());
    ASSERT_NOEXCEPT(zsSpan.size_bytes());
    ASSERT_NOEXCEPT(zdSpan.size_bytes());

    ASSERT(10 * sizeof(int) ==  sSpan.size_bytes());
    ASSERT(4  * sizeof(int) == csSpan.size_bytes());
    ASSERT(10 * sizeof(int) ==  dSpan.size_bytes());
    ASSERT(4  * sizeof(int) == cdSpan.size_bytes());
    ASSERT(0  * sizeof(int) == zsSpan.size_bytes());
    ASSERT(0  * sizeof(int) == zdSpan.size_bytes());

    // empty
    ASSERT_NOEXCEPT( sSpan.empty());
    ASSERT_NOEXCEPT(csSpan.empty());
    ASSERT_NOEXCEPT( dSpan.empty());
    ASSERT_NOEXCEPT(cdSpan.empty());
    ASSERT_NOEXCEPT(zsSpan.empty());
    ASSERT_NOEXCEPT(zdSpan.empty());

    ASSERT(! sSpan.empty());
    ASSERT(!csSpan.empty());
    ASSERT(! dSpan.empty());
    ASSERT(!cdSpan.empty());
    ASSERT( zsSpan.empty());
    ASSERT( zdSpan.empty());

    // data
    ASSERT_NOEXCEPT( sSpan.data());
    ASSERT_NOEXCEPT(csSpan.data());
    ASSERT_NOEXCEPT( dSpan.data());
    ASSERT_NOEXCEPT(cdSpan.data());
    ASSERT_NOEXCEPT(zsSpan.data());
    ASSERT_NOEXCEPT(zdSpan.data());

    ASSERT(&arr[0] == sSpan.data());
    ASSERT(&arr[5] == csSpan.data());
    ASSERT(&arr[0] == dSpan.data());
    ASSERT(&arr[5] == cdSpan.data());

    // front
#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.front());
    ASSERT_NOT_NOEXCEPT(csSpan.front());
    ASSERT_NOT_NOEXCEPT( dSpan.front());
    ASSERT_NOT_NOEXCEPT(cdSpan.front());
#endif

    ASSERT(0 == sSpan.front());
    ASSERT(5 == csSpan.front());
    ASSERT(0 == dSpan.front());
    ASSERT(5 == cdSpan.front());

    // back
#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.back());
    ASSERT_NOT_NOEXCEPT(csSpan.back());
    ASSERT_NOT_NOEXCEPT( dSpan.back());
    ASSERT_NOT_NOEXCEPT(cdSpan.back());
#endif

    ASSERT(9 == sSpan.back());
    ASSERT(8 == csSpan.back());
    ASSERT(9 == dSpan.back());
    ASSERT(8 == cdSpan.back());

    // operator[]
#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan[0]);
    ASSERT_NOT_NOEXCEPT(csSpan[0]);
    ASSERT_NOT_NOEXCEPT( dSpan[0]);
    ASSERT_NOT_NOEXCEPT(cdSpan[0]);
#endif

    ASSERT(7 == sSpan[7]);
    ASSERT(6 == csSpan[1]);
    ASSERT(7 == dSpan[7]);
    ASSERT(6 == cdSpan[1]);

#ifdef TEST_BSL_SPAN_AT
    // at()
    ASSERT(7 == sSpan.at(7));
    ASSERT(6 == csSpan.at(1));
    ASSERT(7 == dSpan.at(7));
    ASSERT(6 == cdSpan.at(1));

    ASSERT(!doesAtThrow(sSpan,   0));
    ASSERT(!doesAtThrow(sSpan,   9));
    ASSERT( doesAtThrow(sSpan,  10));
    ASSERT( doesAtThrow(sSpan, 100));

    ASSERT(!doesAtThrow(csSpan,   0));
    ASSERT(!doesAtThrow(csSpan,   3));
    ASSERT( doesAtThrow(csSpan,   4));
    ASSERT( doesAtThrow(csSpan, 100));

    ASSERT(!doesAtThrow(dSpan,   0));
    ASSERT(!doesAtThrow(dSpan,   9));
    ASSERT( doesAtThrow(dSpan,  10));
    ASSERT( doesAtThrow(dSpan, 100));

    ASSERT(!doesAtThrow(cdSpan,   0));
    ASSERT(!doesAtThrow(cdSpan,   3));
    ASSERT( doesAtThrow(cdSpan,   4));
    ASSERT( doesAtThrow(cdSpan, 100));

    ASSERT( doesAtThrow(zsSpan,   0));
    ASSERT( doesAtThrow(zdSpan,   0));
#endif
}

/// Test the various ways of making a smaller span from an original
void TestSubspan()
{
    int          arr [10];
    const size_t DYN = bsl::dynamic_extent;

    for (int i = 0; i < 10; ++i) {
        arr[i]  = i;
    }

    bsl::span<      int, 10> sSpan (&arr[0], 10);
    bsl::span<const int,  4> csSpan(&arr[5], 4);
    bsl::span<      int>     dSpan (&arr[0], 10);
    bsl::span<const int>     cdSpan(&arr[5], 4);

    // first
    bsl::span<int, 4>        sFirstA  = sSpan.first<4>();
    bsl::span<const int, 2>  csFirstA = csSpan.first<2>();
    bsl::span<int, 4>        dFirstA  = dSpan.first<4>();
    bsl::span<const int, 2>  cdFirstA = cdSpan.first<2>();

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.first<4>());
    ASSERT_NOT_NOEXCEPT(csSpan.first<2>());
    ASSERT_NOT_NOEXCEPT( dSpan.first<4>());
    ASSERT_NOT_NOEXCEPT(cdSpan.first<2>());
#endif

    ASSERT(4 == sFirstA.size());
    ASSERT(2 == csFirstA.size());
    ASSERT(4 == dFirstA.size());
    ASSERT(2 == cdFirstA.size());

    ASSERT(&arr[0] == sFirstA.data());
    ASSERT(&arr[5] == csFirstA.data());
    ASSERT(&arr[0] == dFirstA.data());
    ASSERT(&arr[5] == cdFirstA.data());

    bsl::span<int>        sFirstB  = sSpan.first(4);
    bsl::span<const int>  csFirstB = csSpan.first(2);
    bsl::span<int>        dFirstB  = dSpan.first(4);
    bsl::span<const int>  cdFirstB = cdSpan.first(2);

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.first(4));
    ASSERT_NOT_NOEXCEPT(csSpan.first(2));
    ASSERT_NOT_NOEXCEPT( dSpan.first(4));
    ASSERT_NOT_NOEXCEPT(cdSpan.first(2));
#endif

    ASSERT(4 == sFirstB.size());
    ASSERT(2 == csFirstB.size());
    ASSERT(4 == dFirstB.size());
    ASSERT(2 == cdFirstB.size());

    ASSERT(&arr[0] == sFirstB.data());
    ASSERT(&arr[5] == csFirstB.data());
    ASSERT(&arr[0] == dFirstB.data());
    ASSERT(&arr[5] == cdFirstB.data());

    // last
    bsl::span<int, 4>        sLastA  = sSpan.last<4>();
    bsl::span<const int, 2>  csLastA = csSpan.last<2>();
    bsl::span<int, 4>        dLastA  = dSpan.last<4>();
    bsl::span<const int, 2>  cdLastA = cdSpan.last<2>();

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.last<4>());
    ASSERT_NOT_NOEXCEPT(csSpan.last<2>());
    ASSERT_NOT_NOEXCEPT( dSpan.last<4>());
    ASSERT_NOT_NOEXCEPT(cdSpan.last<2>());
#endif

    ASSERT(4 == sLastA.size());
    ASSERT(2 == csLastA.size());
    ASSERT(4 == dLastA.size());
    ASSERT(2 == cdLastA.size());

    ASSERT(&arr[6] == sLastA.data());
    ASSERT(&arr[7] == csLastA.data());
    ASSERT(&arr[6] == dLastA.data());
    ASSERT(&arr[7] == cdLastA.data());

    bsl::span<int>        sLastB  = sSpan.last(4);
    bsl::span<const int>  csLastB = csSpan.last(2);
    bsl::span<int>        dLastB  = dSpan.last(4);
    bsl::span<const int>  cdLastB = cdSpan.last(2);

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.last(4));
    ASSERT_NOT_NOEXCEPT(csSpan.last(2));
    ASSERT_NOT_NOEXCEPT( dSpan.last(4));
    ASSERT_NOT_NOEXCEPT(cdSpan.last(2));
#endif

    ASSERT(4 == sLastB.size());
    ASSERT(2 == csLastB.size());
    ASSERT(4 == dLastB.size());
    ASSERT(2 == cdLastB.size());

    ASSERT(&arr[6] == sLastB.data());
    ASSERT(&arr[7] == csLastB.data());
    ASSERT(&arr[6] == dLastB.data());
    ASSERT(&arr[7] == cdLastB.data());

    // subspan
    bsl::span<int, 4>         sSubA1  = sSpan.subspan<2, 4>();
    bsl::span<const int, 2>   csSubA1 = csSpan.subspan<1, 2>();
    bsl::span<int, 4>         dSubA1  = dSpan.subspan<2, 4>();
    bsl::span<const int, 2>   cdSubA1 = cdSpan.subspan<1, 2>();

    bsl::span<int>            sSubA2  = sSpan.subspan<4, DYN>();
    bsl::span<const int>      csSubA2 = cdSpan.subspan<2, DYN>();
    bsl::span<int>            dSubA2  = sSpan.subspan<4, DYN>();
    bsl::span<const int>      cdSubA2 = cdSpan.subspan<2, DYN>();

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.subspan<2, 4>());
    ASSERT_NOT_NOEXCEPT(csSpan.subspan<1, 2>());
    ASSERT_NOT_NOEXCEPT( dSpan.subspan<2, 4>());
    ASSERT_NOT_NOEXCEPT(cdSpan.subspan<1, 2>());

    ASSERT_NOT_NOEXCEPT( sSpan.subspan<2, DYN>());
    ASSERT_NOT_NOEXCEPT(csSpan.subspan<1, DYN>());
    ASSERT_NOT_NOEXCEPT( dSpan.subspan<2, DYN>());
    ASSERT_NOT_NOEXCEPT(cdSpan.subspan<1, DYN>());
#endif

    ASSERT(4 == sSubA1.size());
    ASSERT(2 == csSubA1.size());
    ASSERT(4 == dSubA1.size());
    ASSERT(2 == cdSubA1.size());

    ASSERT(&arr[2] == sSubA1.data());
    ASSERT(&arr[6] == csSubA1.data());
    ASSERT(&arr[2] == dSubA1.data());
    ASSERT(&arr[6] == cdSubA1.data());

    ASSERT(6 == sSubA2.size());
    ASSERT(2 == csSubA2.size());
    ASSERT(6 == dSubA2.size());
    ASSERT(2 == cdSubA2.size());

    ASSERT(&arr[4] == sSubA2.data());
    ASSERT(&arr[7] == csSubA2.data());
    ASSERT(&arr[4] == dSubA2.data());
    ASSERT(&arr[7] == cdSubA2.data());

    bsl::span<int>       sSubB1  = sSpan.subspan(2, 4);
    bsl::span<const int> csSubB1 = csSpan.subspan(1, 2);
    bsl::span<int>       dSubB1  = dSpan.subspan(2, 4);
    bsl::span<const int> cdSubB1 = cdSpan.subspan(1, 2);

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.subspan(2, 4));
    ASSERT_NOT_NOEXCEPT(csSpan.subspan(1, 2));
    ASSERT_NOT_NOEXCEPT( dSpan.subspan(2, 4));
    ASSERT_NOT_NOEXCEPT(cdSpan.subspan(1, 2));
#endif

    ASSERT(4 == sSubB1.size());
    ASSERT(2 == csSubB1.size());
    ASSERT(4 == dSubB1.size());
    ASSERT(2 == cdSubB1.size());

    ASSERT(&arr[2] == sSubB1.data());
    ASSERT(&arr[6] == csSubB1.data());
    ASSERT(&arr[2] == dSubB1.data());
    ASSERT(&arr[6] == cdSubB1.data());

    bsl::span<int>       sSubB2  = sSpan.subspan(4, DYN);
    bsl::span<const int> csSubB2 = csSpan.subspan(3, DYN);
    bsl::span<int>       dSubB2  = dSpan.subspan(4, DYN);
    bsl::span<const int> cdSubB2 = cdSpan.subspan(3, DYN);

#ifdef NOEXCEPT_TEST_ONLY_BSL_SPAN
    ASSERT_NOT_NOEXCEPT( sSpan.subspan(4, DYN));
    ASSERT_NOT_NOEXCEPT(csSpan.subspan(3, DYN));
    ASSERT_NOT_NOEXCEPT( dSpan.subspan(4, DYN));
    ASSERT_NOT_NOEXCEPT(cdSpan.subspan(3, DYN));
#endif

    ASSERT(6 == sSubB2.size());
    ASSERT(1 == csSubB2.size());
    ASSERT(6 == dSubB2.size());
    ASSERT(1 == cdSubB2.size());

    ASSERT(&arr[4] == sSubB2.data());
    ASSERT(&arr[8] == csSubB2.data());
    ASSERT(&arr[4] == dSubB2.data());
    ASSERT(&arr[8] == cdSubB2.data());

    bsl::span<int>       sSubB3  = sSpan.subspan(3);
    bsl::span<const int> csSubB3 = csSpan.subspan(2);
    bsl::span<int>       dSubB3  = dSpan.subspan(3);
    bsl::span<const int> cdSubB3 = cdSpan.subspan(2);

    ASSERT(7 == sSubB3.size());
    ASSERT(2 == csSubB3.size());
    ASSERT(7 == dSubB3.size());
    ASSERT(2 == cdSubB3.size());

    ASSERT(&arr[3] == sSubB3.data());
    ASSERT(&arr[7] == csSubB3.data());
    ASSERT(&arr[3] == dSubB3.data());
    ASSERT(&arr[7] == cdSubB3.data());
}

/// Verify that the iterators work
void TestIterators()
{
    int arr [10];
    int idx;

    for (int i = 0; i < 10; ++i) {
        arr[i]  = i;
    }

    bsl::span<      int, 10> sSpan (&arr[0], 10);
    bsl::span<const int,  4> csSpan(&arr[5], 4);
    bsl::span<      int>     dSpan (&arr[0], 10);
    bsl::span<const int>     cdSpan(&arr[5], 4);

    ASSERT_NOEXCEPT( sSpan.begin());
    ASSERT_NOEXCEPT(csSpan.begin());
    ASSERT_NOEXCEPT( dSpan.begin());
    ASSERT_NOEXCEPT(cdSpan.begin());

    ASSERT_NOEXCEPT( sSpan.end());
    ASSERT_NOEXCEPT(csSpan.end());
    ASSERT_NOEXCEPT( dSpan.end());
    ASSERT_NOEXCEPT(cdSpan.end());

    ASSERT_NOEXCEPT( sSpan.rbegin());
    ASSERT_NOEXCEPT(csSpan.rbegin());
    ASSERT_NOEXCEPT( dSpan.rbegin());
    ASSERT_NOEXCEPT(cdSpan.rbegin());

    ASSERT_NOEXCEPT( sSpan.rend());
    ASSERT_NOEXCEPT(csSpan.rend());
    ASSERT_NOEXCEPT( dSpan.rend());
    ASSERT_NOEXCEPT(cdSpan.rend());

    // Forward iterators
    idx = 0;
    for (bsl::span<int, 10>::iterator iter = sSpan.begin();
                                      iter != sSpan.end();
                                      ++iter, ++idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 5;
    for (bsl::span<const int, 4>::iterator iter = csSpan.begin();
                                           iter != csSpan.end();
                                           ++iter, ++idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 0;
    for (bsl::span<int>::iterator iter = dSpan.begin();
                                  iter != dSpan.end();
                                  ++iter, ++idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 5;
    for (bsl::span<const int>::iterator iter = cdSpan.begin();
                                        iter != cdSpan.end();
                                        ++iter, ++idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    // Reverse iterators
    idx = 9;
    for (bsl::span<int, 10>::reverse_iterator iter = sSpan.rbegin();
                                              iter != sSpan.rend();
                                              ++iter, --idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 8;
    for (bsl::span<const int, 4>::reverse_iterator iter = csSpan.rbegin();
                                                   iter != csSpan.rend();
                                                   ++iter, --idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 9;
    for (bsl::span<int>::reverse_iterator iter = dSpan.rbegin();
                                          iter != dSpan.rend();
                                          ++iter, --idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    idx = 8;
    for (bsl::span<const int>::reverse_iterator iter = cdSpan.rbegin();
                                                iter != cdSpan.rend();
                                                ++iter, --idx) {
        ASSERT(idx == *iter);
        ASSERT(&arr[idx] == &*iter);
    }

    // Modifiable iterators
    idx = 0;
    for (bsl::span<int, 10>::iterator iter = sSpan.begin();
                                      iter != sSpan.end();
                                      ++iter, ++idx) {
        int newValue = 123 + idx;
        *iter = newValue;
        ASSERT(newValue == *iter);
        ASSERT(newValue == arr[idx]);
    }

    idx = 0;
    for (bsl::span<int>::iterator iter = dSpan.begin();
                                  iter != dSpan.end();
                                  ++iter, ++idx) {
        int newValue = 456 + idx;
        *iter = newValue;
        ASSERT(newValue == *iter);
        ASSERT(newValue == arr[idx]);
    }


    // Modifiable reverse iterators
    idx = 9;
    for (bsl::span<int, 10>::reverse_iterator iter = sSpan.rbegin();
                                              iter != sSpan.rend();
                                              ++iter, --idx) {
        int newValue = 789 + idx;
        *iter = newValue;
        ASSERT(newValue == *iter);
        ASSERT(newValue == arr[idx]);
    }

    idx = 9;
    for (bsl::span<int>::reverse_iterator iter = dSpan.rbegin();
                                          iter != dSpan.rend();
                                          ++iter, --idx) {
        int newValue = 246 + idx;
        *iter = newValue;
        ASSERT(newValue == *iter);
        ASSERT(newValue == arr[idx]);
    }

}

/// Test the free functions swap, as_bytes, and as_writable_bytes
void TestFreeFunctions ()
{
    int arr [10];

    for (int i = 0; i < 10; ++i) {
        arr[i]  = i;
    }

    bsl::span<int, 5> sSpan1 (&arr[0], 5);
    bsl::span<int, 5> sSpan2 (&arr[1], 5);
    bsl::span<int>    dSpan1 (&arr[2], 4);
    bsl::span<int>    dSpan2 (&arr[3], 2);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    auto sBytes1 = bsl::as_bytes(sSpan1);
    auto sBytes2 = bsl::as_writable_bytes(sSpan2);
    auto dBytes1 = bsl::as_bytes(dSpan1);
    auto dBytes2 = bsl::as_writable_bytes(dSpan2);

    ASSERT_NOEXCEPT(bsl::as_bytes(sSpan1));
    ASSERT_NOEXCEPT(bsl::as_writable_bytes(sSpan2));
    ASSERT_NOEXCEPT(bsl::as_bytes(dSpan1));
    ASSERT_NOEXCEPT(bsl::as_writable_bytes(dSpan2));

    BSLMF_ASSERT((bsl::is_same_v<decltype(sBytes1),
                                  bsl::span<const std::byte, 5*sizeof(int)>>));
    BSLMF_ASSERT((bsl::is_same_v<decltype(sBytes2),
                                  bsl::span<      std::byte, 5*sizeof(int)>>));
    BSLMF_ASSERT((bsl::is_same_v<decltype(dBytes1),
                                                 bsl::span<const std::byte>>));
    BSLMF_ASSERT((bsl::is_same_v<decltype(dBytes2),
                                                 bsl::span<      std::byte>>));

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SAL01:  //  Possible strict-aliasing violation
    ASSERT(sBytes1.data() == (const std::byte *) sSpan1.data());
    ASSERT(sBytes1.size() == sizeof(int) * sSpan1.size());
    ASSERT(sBytes2.data() == (std::byte *) sSpan2.data());
    ASSERT(sBytes2.size() == sizeof(int) * sSpan2.size());
// BDE_VERIFY pragma: pop
#endif

    ASSERT(&arr[0] == sSpan1.data());
    ASSERT(5       == sSpan1.size());
    ASSERT(&arr[1] == sSpan2.data());
    ASSERT(5       == sSpan2.size());
    ASSERT(&arr[2] == dSpan1.data());
    ASSERT(4       == dSpan1.size());
    ASSERT(&arr[3] == dSpan2.data());
    ASSERT(2       == dSpan2.size());

    bsl::swap(sSpan1, sSpan2);
    bsl::swap(dSpan1, dSpan2);

    ASSERT_NOEXCEPT(swap(sSpan1, sSpan2));
    ASSERT_NOEXCEPT(swap(dSpan1, dSpan2));

    ASSERT(&arr[1] == sSpan1.data());
    ASSERT(5       == sSpan1.size());
    ASSERT(&arr[0] == sSpan2.data());
    ASSERT(5       == sSpan2.size());
    ASSERT(&arr[3] == dSpan1.data());
    ASSERT(2       == dSpan1.size());
    ASSERT(&arr[2] == dSpan2.data());
    ASSERT(4       == dSpan2.size());
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
/// This struct provides a namespace for functions testing deduction guides.
/// The tests are compile-time only; it is not necessary that these routines
/// be called at run-time.  Note that the following constructors do not have
/// associated deduction guides because the template parameters for
/// `bsl::span` cannot be deduced from the constructor parameters.
/// ```
/// span()
/// span(array<T_OTHER_TYPE, SIZE> &)
/// span(const array<T_OTHER_TYPE, SIZE> &)
/// span(span<T_OTHER_TYPE, SIZE>)
/// ```
struct TestDeductionGuides {

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    /// Test that constructing a `bsl::span` from various combinations of
    /// arguments deduces the correct type.
    /// ```
    /// span(const span&  s)        -> decltype(s)
    /// span(TYPE (&)[SIZE])        -> span<TYPE, SIZE>
    /// span(      bsl::array<TYPE, SIZE>)  -> span<TYPE, SIZE>
    /// span(const bsl::array<TYPE, SIZE>)  -> span<const TYPE, SIZE>
    /// span(      std::array<TYPE, SIZE>)  -> span<TYPE, SIZE>
    /// span(const std::array<TYPE, SIZE>)  -> span<const TYPE, SIZE>
    /// span(      bsl::vector<TYPE, SIZE>) -> span<TYPE>
    /// span(const bsl::vector<TYPE, SIZE>) -> span<const TYPE>
    /// span(            CONTAINER &) -> span<CONTAINER::value_type>
    /// span(const const CONTAINER &) -> span<const CONTAINER::value_type>
    /// ```
    static void SimpleConstructors ()
    {

        typedef int T1;
        bsl::span<T1, 0> sSpan1;
        bsl::span<T1>    dSpan1;
        bsl::span        sSpan1a(sSpan1);
        bsl::span        dSpan1a(dSpan1);
        ASSERT_SAME_TYPE(decltype(sSpan1a), bsl::span<T1, 0>);
        ASSERT_SAME_TYPE(decltype(dSpan1a), bsl::span<T1,
                                                         bsl::dynamic_extent>);

        typedef double T2;
        T2        arr2[3] = {0.0, 1.0, 2.0};
        bsl::span span2a(arr2);
        ASSERT_SAME_TYPE(decltype(span2a), bsl::span<T2, 3>);


#ifndef BSLSTL_ARRAY_IS_ALIASED
        typedef float T3;
        bsl::array<T3, 2>        arr3;
        const bsl::array<T3, 2>& cArr3 = arr3;
        bsl::span                span3a(arr3);
        bsl::span                span3b(cArr3);
        ASSERT_SAME_TYPE(decltype(span3a), bsl::span<T3, 2>);
        ASSERT_SAME_TYPE(decltype(span3b), bsl::span<const T3, 2>);
#endif

        typedef long double T4;
        std::array<T4, 2>        arr4;
        const std::array<T4, 2>& cArr4 = arr4;
        bsl::span                span4a(arr4);
        bsl::span                span4b(cArr4);
        ASSERT_SAME_TYPE(decltype(span4a), bsl::span<T4, 2>);
        ASSERT_SAME_TYPE(decltype(span4b), bsl::span<const T4, 2>);

        typedef long T5;
        bsl::vector<T5>        vec5;
        const bsl::vector<T5>& cVec5 = vec5;
        bsl::span              span5a(vec5);
        bsl::span              span5b(cVec5);
        ASSERT_SAME_TYPE(decltype(span5a), bsl::span<T5, bsl::dynamic_extent>);
        ASSERT_SAME_TYPE(decltype(span5b), bsl::span<const T5,
                                                         bsl::dynamic_extent>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
//  bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.
    BloombergLP::bslma::TestAllocator
                                globalAllocator("global", veryVeryVeryVerbose);
    BloombergLP::bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   Return a span from a function, and use it in a range-based for
        //   loop.
        //
        // Concerns:
        // 1. The usage example compiles, links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example into test driver and verify
        //    functionality with some sample values. (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 2\n"
                            "===============\n");

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        bsl::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        // We can now iterate over the elements in the slice using the span:
        bsl::span<const int> sp = slice(v, 4, 7);
        int                  val = 4;
        for (int x: sp) {
            ASSERT(x == val++);
        }

        // Note that we can use the return value directly and avoid declaring
        // the variable `sp`:
        val = 2;
        for (int x: slice(v, 2, 8)) {
            ASSERT(x == val++);
        }
#endif

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //   Create a span from a local array, and demonstrate that it can
        //   be accessed using the span, and the span can be passed to another
        //   function.
        //
        // Concerns:
        // 1. The usage example compiles, links, and runs as shown.
        //
        //
        // Plan:
        // 1. Incorporate usage example into test driver and verify
        //    functionality with some sample values. (C-1,2)
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 1\n"
                            "===============\n");

        int            arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        bsl::span<int> sp(arr + 3, 4);   // 4 elements, starting at 3.
        for (int i = 0; i < 10; ++i) {
            ASSERT(arr[i] == i);
        }

        ASSERT(sp[0] == 3);
        ASSERT(sp[1] == 4);
        ASSERT(sp[2] == 5);
        ASSERT(sp[3] == 6);

        MakeEven(sp);

        ASSERT(sp[0] == 2); // Has been changed
        ASSERT(sp[1] == 4);
        ASSERT(sp[2] == 4); // Has been changed
        ASSERT(sp[3] == 6);

        ASSERT(arr[0] == 0); // Not part of the span
        ASSERT(arr[1] == 1); // Not part of the span
        ASSERT(arr[2] == 2); // Not part of the span
        ASSERT(arr[3] == 2); // Has been changed
        ASSERT(arr[4] == 4);
        ASSERT(arr[5] == 4); // Has been changed
        ASSERT(arr[6] == 6);
        ASSERT(arr[7] == 7); // Not part of the span
        ASSERT(arr[8] == 8); // Not part of the span
        ASSERT(arr[9] == 9); // Not part of the span
      } break;
      case 8: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        // 1. Construction from arrays deduces the value type from the value
        //    type of the array.
        //
        // 2. Construction from a container deduces the value type from the
        //    value type of the container.
        //
        // Plan:
        // 1. Create a span by invoking the constructor without supplying the
        //    template arguments explicitly.
        //
        // 2. Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test;
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // FREE FUNCTIONS
        //   This case tests the functionality of span's free functions `swap`,
        //   `as_bytes` and `as_writable_bytes`.
        //
        // Concerns:
        // 1. The functions exist, are callable, and return sane values.
        //
        // Plan:
        // 1. Create a variety of span objects.
        //
        // 2. Call the free functions with the spans from step #1.
        //
        // 3. Verify the results of the functions under test.
        //
        // Testing:
        //   void swap(span a, span b) noexcept;
        //   span<const byte> as_bytes(span) noexcept;
        //   span<byte> as_writable_bytes(span) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nFREE FUNCTIONS"
                            "\n==============\n");

        TestFreeFunctions();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ITERATORS
        //   This case tests the functionality of span's iterators.
        //
        // Concerns:
        // 1. Span should provide a full set of iterators; forward, reverse,
        //    const, and const reverse.
        //
        // Plan:
        // 1. Create a variety of span objects.
        //
        // 2. Traverse the contents of the memory referred to by the spans from
        //    step #1, using all the different kinds of iterators.
        //
        // 3. Verify that the iterators refer to the correct underlying element
        //    referred to by the span.
        //
        // Testing:
        //   iterator begin() noexcept;
        //   iterator end() noexcept;
        //   reverse_iterator rbegin() noexcept;
        //   reverse_iterator rend() noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nITERATORS"
                            "\n=========\n");

        TestIterators();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // SUBSPAN
        //   This case tests the functions that create a span that refers to a
        //   subset of an existing span.
        //
        // Concerns:
        // 1. Each of the functions under tests returns a new span that refers
        //    to a subset of the data referenced by the original span.
        //
        // Plan:
        // 1. Create a variety of span objects.
        //
        // 2. Create new spans using the functions `first`, `last`, and
        //    `subspan`.
        //
        // 3. Verify that the new spans refer to the correct underlying data.
        //
        // Testing:
        //   template <size_t COUNT> first();
        //   template <size_t COUNT> last();
        //   first(size_t count);
        //   last(size_t count);
        //   template <size_t COUNT, size_t OFFSET> subspan();
        //   subspan(size_t count, size_t offset);
        // --------------------------------------------------------------------

        if (verbose) printf("\nSUBSPAN"
                            "\n=======\n");

        TestSubspan();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ACCESSORS
        //   This case tests methods that provide access to the elements of the
        //   span.
        //
        // Concerns:
        // 1. The functions under test provide access to the correct elements.
        //
        // Plan:
        // 1. Create a variety of span objects.
        //
        // 2. Verify that `operator[]`, `front`, and `back` provide references
        //    to the correct element of the underlying span.
        //
        // 3. Verify that `size` and `data` return the size of the span, and a
        //    pointer to the first element of the span, respectively.
        //
        // Testing:
        //   reference front();
        //   reference back();
        //   reference operator[](size_type);
        //   reference at(size_type);
        //   bool empty() noexcept;
        //   size_type extent;
        //   size_type size() noexcept;
        //   size_type size_bytes() noexcept;
        //   pointer data() noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nACCESSORS"
                            "\n=========\n");

        TestAccessors();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTION FROM A CONTAINER
        //   This case tests construction of a span from a container
        //
        // Concerns:
        // 1. That a span can be constructed from a bsl::array, a std::array,
        //    and a container that provides contiguous storage.
        //
        // Plan:
        // 1. Attempt to create a span from a bsl::array, a std::array, and
        //    from a bsl::vector.
        //
        // 2. Verify that the resulting spans refer to the memory held by the
        //    container that they were created from.
        //
        // Testing:
        //   span(array<value_type, SIZE>&) noexcept;
        //   span(const array<value_type, SIZE>&) noexcept;
        //   span(std::array<value_type, SIZE>&) noexcept;
        //   span(const std::array<value_type, SIZE>&) noexcept;
        //   span(CONTAINER& c);
        //   span(const CONTAINER& c);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTION FROM A CONTAINER"
                            "\n=============================\n");

        TestContainerConstructors();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC CONSTRUCTORS
        //   This case tests construction of a span.
        //
        // Concerns:
        // 1. That a span can be constructed from various combinations of
        //    parameters.
        //
        // Plan:
        // 1. Attempt to create a span from various combinations of parameters.
        //    bsl::vector.
        //
        // 2. Verify that the resulting spans refer to the memory referred to
        //    the paramaters passed to the constructor.
        //
        // Testing:
        //   span() noexcept;
        //   span(pointer, size_type);
        //   span(pointer, pointer);
        //   span(element_type (&arr)[SIZE]) noexcept;
        //   span(const span &) noexcept;
        //   span(span<T_OTHER_TYPE>) noexcept;
        //   operator=(const span &) noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC CONSTRUCTORS"
                            "\n==================\n");

        TestBasicConstructors();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create a span referring to a the elements of a C-style array.
        //
        // 2. Ensure that the span refers to the memory managed by the array.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int               arr[5] = {0, 1, 2, 3, 4};
        bsl::span<int, 5> ss0(arr);
        bsl::span<int>    sd0 (&arr[0], 5);
        ASSERT(5 == ss0.size());
        ASSERT(5 == sd0.size());
        ASSERT(0 == ss0.front());
        ASSERT(4 == sd0.back());

        ASSERT(2 == ss0[2]);
        sd0[2] = 7;
        ASSERT(7 == ss0[2]);

        ASSERT(bsl::dynamic_extent == size_t(-1));

        ASSERT((bsl::is_trivially_copyable<bsl::span<int>    >::value));
        ASSERT((bsl::is_trivially_copyable<bsl::span<int, 6> >::value));
        ASSERT((bsl::is_trivially_copyable<bsl::span<bsl::string>    >::value));
        ASSERT((bsl::is_trivially_copyable<bsl::span<bsl::string, 6> >::value));
        ASSERT((bslmf::IsTriviallyCopyableCheck<bsl::span<int> >::value));
        ASSERT((bslmf::IsTriviallyCopyableCheck<bsl::span<int, 6> >::value));
        ASSERT((bslmf::IsTriviallyCopyableCheck<bsl::span<bsl::string> >::value));
        ASSERT((bslmf::IsTriviallyCopyableCheck<bsl::span<bsl::string, 6> >::value));

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
