// bslstl_stringview.t.cpp                                            -*-C++-*-
#include <bslstl_stringview.h>
#include <bslstl_stringref.h>

#include <bslstl_algorithm.h>  // bsl::count()
#include <bslstl_map.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_nameof.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <cstring>     // memcpy(), memcmp()
#include <iostream>
#include <stdexcept>   // std::out_of_range
#include <string>
#include <sstream>

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under testing is a value-semantic type whose interface and
// contract are dictated by the C++ standard.  This class is implemented in the
// form of a template, and thus its proper instantiation for several types is a
// concern.
//
// We will follow our standard 10-case approach to testing value-semantic
// types, with the primary manipulator and basic accessors tested fully
// together in case 2 and additional operators, accessors, and manipulators
// tested above case 7 (tests for the test apparatus and streaming are not
// applicable).  We have chosen as a *primary* *manipulator* the value
// constructor.  And *basic* *accessors* are the methods 'data' and 'length'.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] basic_string_view(const CHAR_TYPE *str, size_type numChars);
// [ 5] basic_string_view(const basic_string_view& original);
// [ 8] basic_string_view();
// [ 8] basic_string_view(const CHAR_TYPE *str);
//
// MANIPULATORS
// [ 6] swap(basic_string_view& rhs);
// [ 7] basic_string_view& operator=(const basic_string_view& rhs);
// [12] void remove_prefix(size_type numChars);
// [12] void remove_suffix(size_type numChars);
//
// ACCESSORS
// [ 2] size_type length() const;
// [ 2] const_pointer data() const;
// [ 9] size_type size() const;
// [ 9] size_type max_size() const;
// [ 9] bool empty() const
// [10] const_reference operator[](size_type position) const;
// [10] const_reference at(size_type position) const;
// [10] const_reference front() const;
// [10] const_reference back() const;
// [11] const_iterator  begin() const;
// [11] const_iterator cbegin() const;
// [11] const_iterator  end() const;
// [11] const_iterator cend() const;
// [11] const_reverse_iterator  rbegin() const;
// [11] const_reverse_iterator crbegin() const;
// [11] const_reverse_iterator  rend() const;
// [11] const_reverse_iterator crend() const;
// [13] size_type copy(CHAR_TYPE *str, size_type numCh, size_type pos) const;
// [14] size_type find(basic_string_view sv, size_type pos) const;
// [14] size_type find(const CHAR_TYPE *s, size_type pos, size_type n) const;
// [14] size_type find(const CHAR_TYPE *s, size_type pos) const;
// [14] size_type find(CHAR_TYPE c, size_type pos) const;
// [14] size_type rfind(basic_string_view sv, size_type pos) const;
// [14] size_type rfind(const CHAR_TYPE *s, size_type pos, size_type n) const;
// [14] size_type rfind(const CHAR_TYPE *s, size_type pos) const;
// [14] size_type rfind(CHAR_TYPE c, size_type pos) const;
// [15] size_type find_first_of(basic_string_view, size_type) const;
// [15] size_type find_first_of(const CHAR_TYPE*, size_type, size_type) const;
// [15] size_type find_first_of(const CHAR_TYPE*, size_type) const;
// [15] size_type find_first_of(CHAR_TYPE c, size_type pos) const;
// [15] size_type find_last_of(basic_string_view, size_type) const;
// [15] size_type find_last_of(const CHAR_TYPE*, size_type, size_type) const;
// [15] size_type find_last_of(const CHAR_TYPE*, size_type) const;
// [15] size_type find_last_of(CHAR_TYPE, size_type) const;
// [16] size_type find_first_not_of(basic_string_view, size_type) const;
// [16] size_type find_first_not_of(const TYPE*, size_type, size_type) const;
// [16] size_type find_first_not_of(const CHAR_TYPE*, size_type) const;
// [16] size_type find_first_not_of(CHAR_TYPE c, size_type pos) const;
// [16] size_type find_last_not_of(basic_string_view, size_type) const;
// [16] size_type find_last_not_of(const TYPE*, size_type, size_type) const;
// [16] size_type find_last_not_of(const CHAR_TYPE*, size_type) const;
// [16] size_type find_last_not_of(CHAR_TYPE, size_type) const;
// [17] basic_string_view substr(size_type p = 0, size_type n = npos) const;
// [18] int compare(basic_string_view other) const;
// [18] int compare(size_type p, size_type n, basic_string_view o) const;
// [18] int compare(size_t, size_t, basic_string_view, size_t, size_t) const;
// [18] int compare(const CHAR_TYPE *other) const;
// [18] int compare(size_type lP, size_type lNs, const CHAR_TYPE *o) const;
// [18] int compare(size_type, size_type, const CHAR_TYPE *, size_type) const;
// [21] bool starts_with(basic_string_view subview) const;
// [21] bool starts_with(CHAR_TYPE character) const;
// [21] bool starts_with(const CHAR_TYPE* characterString) const;
// [21] bool ends_with(basic_string_view subview) const;
// [21] bool ends_with(CHAR_TYPE character) const;
// [21] bool ends_with(const CHAR_TYPE* characterString) const;
//
// FREE OPERATORS
// [ 3] operator<<(std::basic_ostream& stream, basic_string_view view);
// [ 4] bool operator==(basic_string_view, basic_string_view);
// [ 4] bool operator!=(basic_string_view, basic_string_view);
// [20] bool operator<(basic_string_view lhs, basic_string_view rhs);
// [20] bool operator>(basic_string_view lhs, basic_string_view rhs);
// [20] bool operator<=(basic_string_view lhs, basic_string_view rhs);
// [20] bool operator>=(basic_string_view lhs, basic_string_view rhs);
// [19] void hashAppend(HASHALG& hashAlg, const basic_string_view& str);
// [22]  string_view operator ""_sv(const char*    str, std::size_t len);
// [22] wstring_view operator ""_sv(const wchar_t* str, std::size_t len);
//
// FREE FUNCTIONS
// [ 6] swap(basic_string_view& lhs, basic_string_view& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [25] USAGE EXAMPLE
// [24] TRAITS

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                             SWAP TEST HELPERS
// ----------------------------------------------------------------------------

namespace incorrect {

// We use a variadic function here rather than a function template with two
// arguments to avoid ambiguity with 'std::swap' when 'bsl::string_view' is an
// alias for 'std::string_view'.
void swap(...)
    // Fail.  In a successful test, this 'swap' should never be called.  It is
    // set up to be called (and fail) in the case where ADL fails to choose the
    // right 'swap' in 'invokeAdlSwap' below.
{
    ASSERT(0 && "incorrect swap called");
}

}  // close namespace incorrect

template <class TYPE>
void invokeAdlSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).
{
    using incorrect::swap;

    // A correct ADL will key off the types of '*a' and '*b', which will be of
    // our 'bsl' container type, to find the right 'bsl::swap' and not
    // 'incorrect::swap'.

    swap(*a, *b);
}

template <class TYPE>
void invokePatternSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by the recommended pattern for calling 'swap'.
{
    // Invoke 'swap' using the recommended pattern for 'bsl' clients.

    using bsl::swap;

    swap(*a, *b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' and 'invokePatternSwap' (above).

using namespace BloombergLP;
using bsls::NameOf;
using bsls::nameOfType;

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

static bslma::TestAllocator *globalAllocator_p;
static bslma::TestAllocator *defaultAllocator_p;
static bslma::TestAllocator *objectAllocator_p;

template <class TYPE>
class DummyTrait : public bsl::char_traits<TYPE>
    // This class is used to simulate 'invalid length of string' scenario.  We
    // juggle the calculated length of string instead of allocating enormous
    // amount of memory, filling it and passing it's address to the
    // 'string_view' constructor.
{
  public:
    // TYPES
    typedef typename bsl::basic_string_view<TYPE>::size_type size_type;

    // CLASS METHODS
    static size_type length(const TYPE*)
        // Unconditionally return the "not-a-position" value.
    {
        return bsl::basic_string_view<TYPE>::npos;
    }

    static int compare(const TYPE*, const TYPE*, std::size_t)
        // Do nothing.
    {
        return 0;
    }
};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Support function overloads for printing debug info, discovered via ADL.
namespace bsl {

template <class TRAITS, class ALLOC>
void debugprint(const bsl::basic_string<char, TRAITS, ALLOC>& v)
    // Print the contents of the specified string 'v' to 'stdout', then flush.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            printf("%c", v[i]);
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  numChars)
    // Return the position of the *first* occurrence of a character belonging
    // to the specified 'characterString' of the specified 'numChars' length,
    // if such a string can be found in the specified 'stringView' (on or
    // *after* the specified 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE> VIEW;
    typedef typename VIEW::size_type                   size_type;
    size_type min = VIEW::npos;
    for (size_type i = 0; i < numChars; ++i) {
        size_type result = stringView.find(characterString[i], position);
        if (result < min) {
            min = result;
        }
    }
    return min;
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              const bsl::basic_string_view<CHAR_TYPE>&              subview,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *first* occurrence of a character belonging
    // to the specified 'subview', if such an occurrence can be found in the
    // specified 'stringView' (on or *after* the optionally specified
    // 'position' if such a 'position' is specified), and return 'npos'
    // otherwise.
{
    return findFirstOf(stringView, subview.data(), position, subview.length());
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position)
    // Return the position of the *first* occurrence of a character belonging
    // to the specified 'characterString', if such an occurrence can be found
    // in the specified 'stringView' (on or *after* the optionally specified
    // 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE>::traits_type TRAITS;
    return findFirstOf(stringView,
                       characterString,
                       position,
                       TRAITS::length(characterString));
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              CHAR_TYPE                                             character,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *first* occurrence of the specified
    // 'character', if such an occurrence can be found in the specified
    // 'stringView' (on or *after* the optionally specified 'position' if such
    // a 'position' is specified), and return 'npos' otherwise.
{
    return stringView.find(character, position);
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  numChars)
    // Return the position of the *last* occurrence of a character belonging to
    // the specified 'characterString' of the specified 'numChars' length, if
    // such a string can be found in the specified 'stringView' (on or *after*
    // the specified 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE> VIEW;
    typedef typename VIEW::size_type                   size_type;

    size_type max = VIEW::npos;
    size_type i   = 0;

    // 'npos' is greater than any possible non-'npos' return value of the
    // 'rfind', so we try to find any symbol belonging to the specified
    // 'stringView', no matter what position it stays.

    while (VIEW::npos == max && i < numChars) {
        max = stringView.rfind(characterString[i], position);
        ++i;
    }

    // And now we can compare current non-'npos' maximum with the next result.

    for (; i < numChars; ++i) {
        size_type result = stringView.rfind(characterString[i], position);
        if (VIEW::npos != result && result > max ) {
            max = result;
        }
    }

    return max;
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              const bsl::basic_string_view<CHAR_TYPE>&              subview,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *last* occurrence of a character belonging to
    // the specified 'subview', if such an occurrence can be found in the
    // specified 'stringView' (on or *before* the optionally specified
    // 'position' if such a 'position' is specified), and return 'npos'
    // otherwise.
{
    return findLastOf(stringView, subview.data(), position, subview.length());
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position)
    // Return the position of the *last* occurrence of a character belonging to
    // the specified 'characterString', if such an occurrence can be found in
    // the specified 'stringView' (on or *after* the optionally specified
    // 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE>::traits_type TRAITS;
    return findLastOf(stringView,
                      characterString,
                      position,
                      TRAITS::length(characterString));
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              CHAR_TYPE                                             character,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *last* occurrence of the specified
    // 'character', if such an occurrence can be found in the specified
    // 'stringView' (on or *before* the optionally specified 'position' if such
    // a 'position' is specified), and return 'npos' otherwise.
{
    return stringView.rfind(character, position);
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstNotOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  numChars)
    // Return the position of the *first* occurrence of a character *not*
    // belonging to the specified 'characterString' of the specified 'numChars'
    // length, if such an occurrence can be found in the specified 'stringView'
    // (on or *after* the specified 'position'), and return 'npos' otherwise.
{
    // The idea is to find the position of the first occurrence of each
    // possible symbol of the 'CHAR_TYPE', not belonging to the specified
    // 'characterString'.  But it can take too much time to iterate through all
    // characters.  So, as this function is used for test purposes only and we
    // definitely know the content of the test strings and views, we can reduce
    // the number of characters being checked to 128 ([0..127]).

    typedef typename bsl::basic_string_view<CHAR_TYPE> VIEW;
    typedef typename VIEW::size_type                   size_type;

    const CHAR_TYPE LIMIT = CHAR_TYPE(127);
    CHAR_TYPE       charactersForSearch[LIMIT + 1];

    size_type numCharactersForSearch = 0;
    VIEW      characterStringView(characterString, numChars);

    for (CHAR_TYPE i = 0; ; ++i) {
        const CHAR_TYPE CHARACTER = i;
        if (VIEW::npos == characterStringView.find(CHARACTER)) {
            charactersForSearch[numCharactersForSearch] = CHARACTER;
            ++numCharactersForSearch;
        }

        // We need to predict the situation when char variable tries to get
        // value equal to 128.

        if (LIMIT == i) {
            break;
        }
    }

    return findFirstOf(stringView,
                       charactersForSearch,
                       position,
                       numCharactersForSearch);
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstNotOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              const bsl::basic_string_view<CHAR_TYPE>&              subview,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *first* occurrence of a character *not*
    // belonging to the specified 'subview', if such an occurrence can be found
    // in the specified 'stringView' (on or *after* the optionally specified
    // 'position' if such a 'position' is specified), and return 'npos'
    // otherwise.
{
    return findFirstNotOf(stringView,
                          subview.data(),
                          position,
                          subview.length());
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstNotOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position)
    // Return the position of the *first* occurrence of a character *not*
    // belonging to the specified 'characterString', if such an occurrence can
    // be found in the specified 'stringView' (on or *after* the optionally
    // specified 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE>::traits_type TRAITS;
    return findFirstNotOf(stringView,
                          characterString,
                          position,
                          TRAITS::length(characterString));
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findFirstNotOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              CHAR_TYPE                                             character,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *first* occurrence of a character
    // *different* from the specified 'character', if such an occurrence can
    // be found in  the specified 'stringView' (on or *after* the optionally
    // specified 'position' if such a 'position' is specified), and return
    // 'npos' otherwise.
{
    return findFirstNotOf(stringView, &character, position, 1);
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastNotOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  numChars)
    // Return the position of the *last* occurrence of a character *not*
    // belonging to the specified 'characterString' of the specified 'numChars'
    // length, if such an occurrence can be found in  the specified
    // 'stringView' (on or *after* the specified 'position'), and return 'npos'
    // otherwise.
{
    // The idea is to find the position of the first occurrence of each
    // possible symbol of the 'CHAR_TYPE', not belonging to the specified
    // 'characterString'.  But it takes too much time to iterate through all
    // characters.  So, as this function is used for test purposes only and we
    // definitely know the content of the test strings and views, we can reduce
    // the number of characters being checked to 128 ([0..127]).

    typedef typename bsl::basic_string_view<CHAR_TYPE> VIEW;
    typedef typename VIEW::size_type                   size_type;

    const CHAR_TYPE LIMIT = CHAR_TYPE(127);
    CHAR_TYPE       charactersForSearch[LIMIT + 1];

    size_type numCharactersForSearch = 0;
    VIEW      characterStringView(characterString, numChars);

    for (CHAR_TYPE i = 0; ; ++i) {
        const CHAR_TYPE CHARACTER = i;
        if (VIEW::npos == characterStringView.find(CHARACTER)) {
            charactersForSearch[numCharactersForSearch] = CHARACTER;
            ++numCharactersForSearch;
        }

        // We need to predict the situation when char variable tries to get
        // value equal to 128.

        if (LIMIT == i) {
            break;
        }
    }

    return findLastOf(stringView,
                      charactersForSearch,
                      position,
                      numCharactersForSearch);
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastNotOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              const bsl::basic_string_view<CHAR_TYPE>&              subview,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *last* occurrence of a character *not*
    // belonging to the specified 'subview', if such an occurrence can be found
    // in  the specified 'stringView' (on or *before* the optionally specified
    // 'position' if such a 'position' is specified), and return 'npos'
    // otherwise.
{
    return findLastNotOf(stringView,
                         subview.data(),
                         position,
                         subview.length());
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastNotOf(
        const bsl::basic_string_view<CHAR_TYPE>&               stringView,
        const CHAR_TYPE                                       *characterString,
        typename bsl::basic_string_view<CHAR_TYPE>::size_type  position)
    // Return the position of the *last* occurrence of a character *not*
    // belonging to the specified 'characterString', if such an occurrence can
    // be found in  the specified 'stringView' (on or *after* the optionally
    // specified 'position'), and return 'npos' otherwise.
{
    typedef typename bsl::basic_string_view<CHAR_TYPE>::traits_type TRAITS;
    return findLastNotOf(stringView,
                         characterString,
                         position,
                         TRAITS::length(characterString));
}

template <class CHAR_TYPE>
typename bsl::basic_string_view<CHAR_TYPE>::size_type findLastNotOf(
              const bsl::basic_string_view<CHAR_TYPE>&              stringView,
              CHAR_TYPE                                             character,
              typename bsl::basic_string_view<CHAR_TYPE>::size_type position)
    // Return the position of the *last* occurrence of a character *different*
    // from the specified 'character', if such an occurrence can be found in
    // the specified 'stringView' (on or *before* the optionally specified
    // 'position' if such a 'position' is specified), and return 'npos'
    // otherwise.
{
    return findLastNotOf(stringView, &character, position, 1);
}

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class TRAITS = bsl::char_traits<TYPE> >
struct TestDriver {
    // TYPES
    typedef bsl::basic_string_view<TYPE, TRAITS> Obj;
        // Type under testing.

    typedef typename Obj::size_type              size_type;
    typedef typename Obj::iterator               iterator;
    typedef typename Obj::const_iterator         const_iterator;
    typedef typename Obj::reverse_iterator       reverse_iterator;
    typedef typename Obj::const_reverse_iterator const_reverse_iterator;

    // CLASS DATA
    // We have to define 's_testStringLength' constant inside of the class
    // declaration to avoid MSVC build error "C2131: expression did not
    // evaluate to a constant".  And we have to initialize it with the literal
    // instead of using 'sizeof', because MSVC doesn't allow to use 'sizeof'
    // for constant static arrays, defined outside  of the class declaration:
    // "C2070: 'const char []': illegal sizeof operand".

    static const TYPE s_testString[];           // common test string
    static const int  s_testStringLength = 49;  // length of test string

    // TEST CASES
    static void testCase23();
        // Conversions between strings and string views.

    static void testCase22();
        // Test 'operator ""_sv'.

#if defined(BSLSTL_STRINGVIEW_ENABLE_CPP20_METHODS)
    static void testCase21();
        // Test 'starts_with' and 'ends_with'.
#endif

    static void testCase20();
        // Test comparison operators.

    static void testCase19();
        // Test 'hashAppend'.

    static void testCase18();
        // Test 'compare'.

    static void testCase17();
        // Test 'substr'.

    static void testCase16();
        // Test 'find_first_not_of' and 'find_last_not_of'.

    static void testCase15();
        // Test 'find_first_of' and 'find_last_of'.

    static void testCase14();
        // Test 'find' and 'rfind'.

    static void testCase13();
        // Test 'copy'.

    static void testCase12();
        // Test cutting methods ('remove_prefix' and 'remove_suffix').

    static void testCase11();
        // Test iterator accessors.

    static void testCase10();
        // Test element accessors.

    static void testCase9();
        // Test size accessors.

    static void testCase8();
        // Test constructors.

    static void testCase7();
        // Test copy-assignment operator.

    static void testCase6();
        // Test swap.

    static void testCase5();
        // Test copy constructor.

    static void testCase4();
        // Test equality comparison.

    static void testCase3();
        // Test print operations.

    static void testCase2();
        // Test primary manipulators.

};

                                // ----------
                                // CLASS DATA
                                // ----------

template <class TYPE, class TRAITS>
const TYPE TestDriver<TYPE, TRAITS>::s_testString [] =
                                                  {
                                                     0, 48, 49, 50, 51, 52, 53,
                                                    48,  0, 49, 50, 51, 52, 53,
                                                    48, 49,  0, 50, 51, 52, 53,
                                                    48, 49, 50,  0, 51, 52, 53,
                                                    48, 49, 50, 51,  0, 52, 53,
                                                    48, 49, 50, 51, 52,  0, 53,
                                                    48, 49, 50, 51, 52, 53,  0
                                                  };

                                // ----------
                                // TEST CASES
                                // ----------

template <class TYPE, class TRAITS>
void TestDriver<TYPE,TRAITS>::testCase23()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSION W.R.T. 'std::basic_string'
    //
    // Concerns:
    //: 1 That it is possible to construct a 'string_view' from a
    //:   'std::string'.
    //:
    //: 2 That the source is not modified.
    //:
    //: 3 That the contents of the constructed object match the original
    //:   string.
    //:
    //: 4 That a variety of string and string view types are constructible and
    //:   assignable from each other.
    //
    // Plan:
    //: 1 Using a few samples, create strings with those contents, create
    //:   string views from them, and verify that the results are consistent.
    //:
    //: 2 Construct a variety of string-like objects and verify that they
    //:   inter-convert properly with string views.
    // --------------------------------------------------------------------

    static const char *DATA[] = {
        "", "woof", "meow", "bow wow",
        "The rain in Spain falls mainly in the plain.",
        "By george, I think she's got it!" };
    enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

    for (int ti = 0; ti < k_NUM_DATA; ++ti) {
        const char *CHAR_STR = DATA[ti];
        const char *pc;
        TYPE        buffer[100], *pB;

        // Copy from 'char' buffer to 'TYPE' buffer.

        for (pB = buffer, pc = CHAR_STR; (*pB++ = *pc++); ) {
            ;  // do nothing
        }
        const size_t LEN = pB - 1 - buffer;
        ASSERT(0 == buffer[LEN]);

        pB = buffer;

        const std::basic_string<TYPE> str(pB);
        ASSERT(pB == str);
        const bsl::basic_string_view<TYPE> sv(str);
        ASSERT(pB == str);    // unchanged

        // Compare 'sv' with 'str', they should match.

        ASSERT(str.length() == sv.length());
        ASSERT(!sv.data()[sv.length()]);

        for (unsigned ii = 0; ii < LEN; ++ii) {
            ASSERT(sv[ii] == str[ii]);
            ASSERT(sv[ii] == buffer[ii]);
        }

        if (LEN < 4) {
            continue;
        }

        // Now, do it over again with an embedded zero in the string.

        buffer[2] = 0;

        const std::basic_string<TYPE> zStr(pB, LEN);
        ASSERT(LEN == zStr.length());
        ASSERT(zStr != str);
        ASSERT(zStr[2] == 0);

        const std::basic_string<TYPE> zStrB(pB, LEN);
        ASSERT(LEN == zStrB.length());
        ASSERT(zStrB != str);
        ASSERT(zStrB[2] == 0);
        ASSERT(zStr == zStrB);

        const bsl::basic_string_view<TYPE> zSv(zStr);
        ASSERT(LEN == zSv.length());
        ASSERT(zSv.data() == zStr.data());
        ASSERT(zSv[2] == 0);

        ASSERT(LEN == zStr.length());  // unchanged
        ASSERT(zStr == zStrB);         // unchanged

        for (unsigned ii = 0; ii < LEN; ++ii) {
            ASSERT(zSv[ii] == zStr[ii]);
            ASSERT(zSv[ii] == buffer[ii]);
        }

        if (veryVerbose) printf("\tbsl::string vs. string_view\n");
        {
            bsl::basic_string<TYPE> s(pB, pB + LEN);
            bsl::basic_string_view<TYPE> v(s);
            ASSERT(v.data() == s.data());
            ASSERT(s == v);
            ASSERT(v == s);
            v = s;
            ASSERT(v.data() == s.data());
            bsl::basic_string<TYPE> o(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o = v;
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o.assign(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
        }

        if (veryVerbose) printf("\tstd::string vs. string_view\n");
        {
            std::basic_string<TYPE> s(pB, pB + LEN);
            bsl::basic_string_view<TYPE> v(s);
            ASSERT(v.data() == s.data());
            ASSERT(s == v);
            ASSERT(v == s);
            v = s;
            ASSERT(v.data() == s.data());
            std::basic_string<TYPE> o(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            // Will not work with 'explicit string_view::operator std::string'
            // in our implementation.
            o = v;
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o.assign(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
#endif
        }

        if (veryVerbose) printf("\tstd::string vs. string_view\n");
        {
            std::basic_string<TYPE> s(pB, pB + LEN);
            bsl::basic_string_view<TYPE> v(s);
            ASSERT(v.data() == s.data());
            ASSERT(s == v);
            ASSERT(v == s);
            v = s;
            ASSERT(v.data() == s.data());
            std::basic_string<TYPE> o(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
            // Will not work with 'explicit string_view::operator std::string'
            // in our implementation.
            o = v;
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o.assign(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
#endif
        }

        if (veryVerbose) printf("\tStringRefImp vs. string_view\n");
        {
            bslstl::StringRefImp<TYPE> s(pB, pB + LEN);
            bsl::basic_string_view<TYPE> v(s);
            ASSERT(v.data() == s.data());
            ASSERT(s == v);
            ASSERT(v == s);
            v = s;
            ASSERT(v.data() == s.data());
            bslstl::StringRefImp<TYPE> o(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o = v;
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o.assign(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
        }

        if (veryVerbose) printf("\tStringRefData vs. string_view\n");
        {
            bslstl::StringRefData<TYPE> s(pB, pB + LEN);
            bsl::basic_string_view<TYPE> v(s);
            ASSERT(v.data() == s.data());
            ASSERT(s == v);
            ASSERT(v == s);
            v = s;
            ASSERT(v.data() == s.data());
            bslstl::StringRefData<TYPE> o(v);
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
            o = v;
            ASSERT(0 == memcmp(o.data(), v.data(), (LEN + 1) * sizeof(TYPE)));
        }
    }
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE,TRAITS>::testCase22()
{
    // --------------------------------------------------------------------
    // TESTING LITERALS
    //
    // Concerns:
    //: 1 That the 'operator ""_sv' correctly forwards arguments to the
    //:   constructor arguments to the constructor of the 'basic_string_view'
    //:   type.
    //:
    //: 2 That the length of the resultant string_view is determined by the
    //:   specified 'length' argument and does not depend on character values
    //:   of the input character array, for example null character.
    //:
    //: 3 That an access to 'operator ""_sv' can be gained using either
    //:   'bsl::literals', 'bsl::string_view_literals' or
    //:   'bsl::literals::string_view_literals' namespaces.
    //:
    //: 4 The 'operator ""_sv' allocates no memory.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 For a variety of strings of different sizes and different values
    //:   including embedded null character test that the resultant
    //:   string_views have expected values and lengths by comparing them with
    //:   the objects, created via explicit constructor calling.  (c-1..2)
    //:
    //: 3 Invoke the operator within the scope of different namespaces.  (C-3)
    //:
    //: 4 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-4)
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
    //:   (C-5)
    //
    // Testing:
    //    string_view operator ""_sv(const char*    str, std::size_t len);
    //   wstring_view operator ""_sv(const wchar_t* str, std::size_t len);
    // --------------------------------------------------------------------

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("for char type.\n");
    {
        if (verbose) printf("\tTesting basic behavior.\n");

        typedef bsl::basic_string_view<char> ObjC;

        { // C-1..3
            using namespace bsl::string_view_literals;

            struct {
                int                     d_line;
                ObjC                    d_str;
                const ObjC::value_type *d_expected_p;
                ObjC::size_type         d_len;
            } DATA[] = {
                // LINE  STR                     EXPECTED            LEN
                //-----  ----------------        -------------       ----
                { L_,              ""_sv,                  "",        0 },
                { L_,            "\0"_sv,                "\0",        1 },
                { L_,          "\0\0"_sv,              "\0\0",        2 },
                { L_,        "\0\0\0"_sv,            "\0\0\0",        3 },
                { L_,             "a"_sv,                 "a",        1 },
                { L_,            "ab"_sv,                "ab",        2 },
                { L_,           "abc"_sv,               "abc",        3 },
                { L_,        "abcdef"_sv,            "abcdef",        6 },
                { L_,      "\0abcdef"_sv,          "\0abcdef",        7 },
                { L_,      "a\0bcdef"_sv,          "a\0bcdef",        7 },
                { L_,      "ab\0cdef"_sv,          "ab\0cdef",        7 },
                { L_,      "abc\0def"_sv,          "abc\0def",        7 },
                { L_,      "abcd\0ef"_sv,          "abcd\0ef",        7 },
                { L_,      "abcde\0f"_sv,          "abcde\0f",        7 },
                { L_,      "abcdef\0"_sv,          "abcdef\0",        7 },
                { L_,    "\0abcdef\0"_sv,        "\0abcdef\0",        8 },
                { L_,    "a\0bcde\0f"_sv,        "a\0bcde\0f",        8 },
                { L_,    "ab\0cd\0ef"_sv,        "ab\0cd\0ef",        8 },
                { L_,    "abc\0\0def"_sv,        "abc\0\0def",        8 },
                { L_,    "\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_sv,
                         "\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_",     34 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int             LINE   = DATA[ti].d_line;
                const ObjC&           RESULT = DATA[ti].d_str;
                const ObjC::size_type LENGTH = DATA[ti].d_len;

                const ObjC            EXPECTED(DATA[ti].d_expected_p, LENGTH);

                LOOP_ASSERT(LINE,  EXPECTED == RESULT);
            }
        }

        { // c-3
            using namespace bsl::literals;
            ObjC mX = "test"_sv;
            (void) mX;
        }
        { // c-3
            using namespace bsl::string_view_literals;
            ObjC mX = "test"_sv;
            (void) mX;
        }
        { // c-3
            using namespace bsl::literals::string_view_literals;
            ObjC mX = "test"_sv;
            (void) mX;
        }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
        if (verbose) printf("\tNegative testing.\n");
        { // C-6
            using namespace bsl::string_view_literals;
            bsls::AssertTestHandlerGuard hG;

            const ObjC::value_type *STRING   = TestDriver<char>::s_testString;
            const ObjC::value_type *NULL_PTR = 0;

            ASSERT_SAFE_PASS(operator ""_sv(STRING, 0));
            ASSERT_SAFE_PASS(operator ""_sv(STRING, 5));

            ASSERT_SAFE_PASS(operator ""_sv(NULL_PTR, 0));
            ASSERT_SAFE_FAIL(operator ""_sv(NULL_PTR, 5));
        }
#endif
    }

    if (verbose) printf("for wchar_t type.\n");
    {
        if (verbose) printf("\tTesting basic behavior.\n");

        typedef bsl::wstring_view ObjW;

        { // C-1..3
            using namespace bsl::string_view_literals;

            struct {
                int                     d_line;
                ObjW                    d_str;
                const ObjW::value_type *d_expected_p;
                ObjW::size_type         d_len;
            } DATA[] = {
                // LINE  STR                       EXPECTED           LEN
                //-----  ----------------          --------------     ----
                { L_,              L""_sv,                   L"",      0 },
                { L_,            L"\0"_sv,                 L"\0",      1 },
                { L_,          L"\0\0"_sv,               L"\0\0",      2 },
                { L_,        L"\0\0\0"_sv,             L"\0\0\0",      3 },
                { L_,             L"a"_sv,                  L"a",      1 },
                { L_,            L"ab"_sv,                 L"ab",      2 },
                { L_,           L"abc"_sv,                L"abc",      3 },
                { L_,        L"abcdef"_sv,             L"abcdef",      6 },
                { L_,      L"\0abcdef"_sv,           L"\0abcdef",      7 },
                { L_,      L"a\0bcdef"_sv,           L"a\0bcdef",      7 },
                { L_,      L"ab\0cdef"_sv,           L"ab\0cdef",      7 },
                { L_,      L"abc\0def"_sv,           L"abc\0def",      7 },
                { L_,      L"abcd\0ef"_sv,           L"abcd\0ef",      7 },
                { L_,      L"abcde\0f"_sv,           L"abcde\0f",      7 },
                { L_,      L"abcdef\0"_sv,           L"abcdef\0",      7 },
                { L_,    L"\0abcdef\0"_sv,         L"\0abcdef\0",      8 },
                { L_,    L"a\0bcde\0f"_sv,         L"a\0bcde\0f",      8 },
                { L_,    L"ab\0cd\0ef"_sv,         L"ab\0cd\0ef",      8 },
                { L_,    L"abc\0\0def"_sv,         L"abc\0\0def",      8 },
                { L_,    L"\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_sv,
                         L"\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_",     34 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int             LINE   = DATA[ti].d_line;
                const ObjW&           RESULT = DATA[ti].d_str;
                const ObjW::size_type LENGTH = DATA[ti].d_len;

                const ObjW            EXPECTED(DATA[ti].d_expected_p, LENGTH);

                LOOP_ASSERT(LINE,  EXPECTED == RESULT);
            }
        }

        { // c-3
            using namespace bsl::literals;
            ObjW mX = L"test"_sv;
            (void) mX;
        }
        { // c-3
            using namespace bsl::string_view_literals;
            ObjW mX = L"test"_sv;
            (void) mX;
        }
        { // c-3
            using namespace bsl::literals::string_view_literals;
            ObjW mX = L"test"_sv;
            (void) mX;
        }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
        if (verbose) printf("\tNegative testing.\n");
        { // C-6
            using namespace bsl::string_view_literals;
            bsls::AssertTestHandlerGuard hG;

            const ObjW::value_type *STRING   =
                                             TestDriver<wchar_t>::s_testString;
            const ObjW::value_type *NULL_PTR = 0;

            ASSERT_SAFE_PASS(operator ""_sv(STRING, 0));
            ASSERT_SAFE_PASS(operator ""_sv(STRING, 5));

            ASSERT_SAFE_PASS(operator ""_sv(NULL_PTR, 0));
            ASSERT_SAFE_FAIL(operator ""_sv(NULL_PTR, 5));
        }
#endif
    }

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

#else
    if (verbose) { printf("Cannot test 'operator ""_sv' in pre-C++11 mode or"
                          "if the compiler does not support inline"
                          " namespaces.\n"); }
#endif

}

#if defined(BSLSTL_STRINGVIEW_ENABLE_CPP20_METHODS)
template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase21() {
    // ------------------------------------------------------------------------
    // TESTING 'STARTS_WITH' AND 'ENDS_WITH'
    //   We are going to use fully tested 'find' and 'rfind' methods as a
    //   control functions.
    //
    // Concerns:
    //: 1 An empty or zero-length object can be used as a parameter for search.
    //:
    //: 2 Search operations can be called for an empty or zero-length object.
    //:
    //: 3 The null symbol ('/0') is correctly handled whether it belongs to the
    //:   object or to the requested value for search.
    //:
    //: 4 The length of the value for search can exceed the length of the
    //:   object.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create an empty and a zero-length objects.  For each object call
    //:   each tested method, passing in turn an empty object, a zero-length
    //:   object, a null pointer as a parameter and verify the results.
    //:
    //: 2 Specify a set S of unique non-empty object values.  For each value
    //:   'x' in 'S' call each tested method, passing in turn an empty object,
    //:    a zero-length object, a null pointer as a parameter and verify the
    //:    results.  Call each tested method for an empty object and for a
    //:    zero-length object, passing 'x' as a parameter and verify the
    //:    results.  (C-1..2)
    //:
    //: 3 Specify a set S of unique non-empty object values both containing
    //:   null symbols and not.  For each value 'x' in 'S" call each tested
    //:   method, passing in turn each value 'y' in 'S', or underlying string
    //:   of 'y', or the first character of 'y' and verify the results using
    //:   'find' or 'rfind' methods accordingly.  (C-3..4)
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
    //
    // Testing:
    //   bool starts_with(basic_string_view subview) const;
    //   bool starts_with(CHAR_TYPE character) const;
    //   bool starts_with(const CHAR_TYPE* characterString) const;
    //   bool ends_with(basic_string_view subview) const;
    //   bool ends_with(CHAR_TYPE character) const;
    //   bool ends_with(const CHAR_TYPE* characterString) const;
    // ------------------------------------------------------------------------
    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING     = s_testString;
    const TYPE      *NULL_PTR   = 0;

    const size_type  NPOS       = Obj::npos;
    const size_type  MAX_LENGTH = 5;

    static const struct {
        int        d_lineNum;             // line number
        const TYPE d_string[MAX_LENGTH];  // string_view basis
        size_type  d_length;              // string_view length
    } DATA[] = {
        //LINE   STRING               CLOSURE    LENGTH
        //----   -------------------  -------    ------
        { L_,   { 0                   , 0    },  1     },
        { L_,   { 'a'                 , 0    },  1     },
        { L_,   { 'b'                 , 0    },  1     },
        { L_,   { 0  , 0              , 0    },  2     },
        { L_,   { 0  , 'a'            , 0    },  2     },
        { L_,   { 'a', 0              , 0    },  2     },
        { L_,   { 'a', 'a'            , 0    },  2     },
        { L_,   { 'a', 'b'            , 0    },  2     },
        { L_,   { 'b', 'a'            , 0    },  2     },
        { L_,   { 'b', 'b'            , 0    },  2     },
        { L_,   { 0  , 0  , 0         , 0    },  3     },
        { L_,   { 0  , 0  , 'a'       , 0    },  3     },
        { L_,   { 0  , 'a', 0         , 0    },  3     },
        { L_,   { 0  , 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 0  , 0         , 0    },  3     },
        { L_,   { 'a', 0  , 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 0         , 0    },  3     },
        { L_,   { 'a', 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 'b'       , 0    },  3     },
        { L_,   { 'a', 'b', 'a'       , 0    },  3     },
        { L_,   { 'a', 'b', 'b'       , 0    },  3     },
        { L_,   { 'b', 'a', 'a'       , 0    },  3     },
        { L_,   { 'b', 'a', 'b'       , 0    },  3     },
        { L_,   { 'b', 'b', 'a'       , 0    },  3     },
        { L_,   { 'b', 'b', 'b'       , 0    },  3     },
        { L_,   { 'd', 'e', 'f'       , 0    },  3     },
        { L_,   { 0  , 0  , 0  , 0    , 0    },  4     },
        { L_,   { 0  , 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 0    , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 0    , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 0    , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 0    , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 0    , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 0    , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 0    , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'k', 'l', 'm', 'n'  , 0    },  4     },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    // Testing empty object, zero-length object and null pointer.

    {
        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        Obj        mXZero(STRING, 0);
        const Obj& XZero = mXZero;

        ASSERTV(true == XEmpty.starts_with(XZero));
        ASSERTV(true == XZero.starts_with(XEmpty));

        ASSERTV(true == XEmpty.ends_with(XZero));
        ASSERTV(true == XZero.ends_with(XEmpty));

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            // string_view

            ASSERTV(LINE, false == XEmpty.starts_with(X));
            ASSERTV(LINE, false == XZero.starts_with(X));

            ASSERTV(LINE, false == XEmpty.ends_with(X));
            ASSERTV(LINE, false == XZero.ends_with(X));

            // string

            const bool ZERO_LENGTH_STR = (0 == TRAITS::length(STR));

            ASSERTV(LINE, ZERO_LENGTH_STR == XEmpty.starts_with(STR));
            ASSERTV(LINE, ZERO_LENGTH_STR == XZero.starts_with(STR));

            ASSERTV(LINE, ZERO_LENGTH_STR == XEmpty.ends_with(STR));
            ASSERTV(LINE, ZERO_LENGTH_STR == XZero.ends_with(STR));

            // character

            ASSERTV(LINE, false == XEmpty.starts_with(STR[0]));
            ASSERTV(LINE, false == XZero.starts_with(STR[0]));

            ASSERTV(LINE, false == XEmpty.ends_with(STR[0]));
            ASSERTV(LINE, false == XZero.ends_with(STR[0]));

            // reverse

            ASSERTV(LINE, true  == X.starts_with(XZero));
            ASSERTV(LINE, true  == X.starts_with(XEmpty));

            ASSERTV(LINE, true  == X.ends_with(XZero));
            ASSERTV(LINE, true  == X.ends_with(XEmpty));
        }
    }

    // Testing non-empty objects.

    for (size_type i = 0; i < NUM_DATA; ++i) {
        const int          LINE1   = DATA[i].d_lineNum;
        const TYPE * const STR1    = DATA[i].d_string;
        const size_type    LENGTH1 = DATA[i].d_length;

        Obj        mX1(STR1, LENGTH1);
        const Obj& X1 = mX1;

        for (size_type j = 0; j < NUM_DATA; ++j) {
            const int          LINE2   = DATA[j].d_lineNum;
            const TYPE * const STR2    = DATA[j].d_string;
            const size_type    LENGTH2 = DATA[j].d_length;

            Obj        mX2(STR2, LENGTH2);
            const Obj& X2 = mX2;

            // string_view

            const bool EXP_S_SV = (0 == X1.find(X2));
            const bool EXP_E_SV = (NPOS              != X1.rfind(X2) &&
                                   LENGTH1 - LENGTH2 == X1.rfind(X2));

            const bool RESULT_S_SV = X1.starts_with(X2);
            const bool RESULT_E_SV = X1.ends_with(X2);

            ASSERTV(LINE1, LINE2, EXP_S_SV, RESULT_S_SV,
                    EXP_S_SV == RESULT_S_SV);
            ASSERTV(LINE1, LINE2, EXP_E_SV, RESULT_E_SV,
                    EXP_E_SV == RESULT_E_SV);

            // string

            const bool EXP_S_S = (0 == X1.find(STR2));
            const bool EXP_E_S =
                           (NPOS                           != X1.rfind(STR2) &&
                            LENGTH1 - TRAITS::length(STR2) == X1.rfind(STR2));

            const bool RESULT_S_S = X1.starts_with(STR2);
            const bool RESULT_E_S = X1.ends_with(STR2);

            ASSERTV(LINE1, LINE2, EXP_S_S, RESULT_S_S,
                    EXP_S_S == RESULT_S_S);
            ASSERTV(LINE1, LINE2, EXP_E_S, RESULT_E_S,
                    EXP_E_S == RESULT_E_S);

            // character

            const bool EXP_S_CH = (0 == X1.find(STR2[0]));
            const bool EXP_E_CH = (LENGTH1 - 1 == X1.rfind(STR2[0]));

            const bool RESULT_S_CH = X1.starts_with(STR2[0]);
            const bool RESULT_E_CH = X1.ends_with(STR2[0]);

            ASSERTV(LINE1, LINE2, EXP_S_CH, RESULT_S_CH,
                    EXP_S_CH == RESULT_S_CH);
            ASSERTV(LINE1, LINE2, EXP_E_CH, RESULT_E_CH,
                    EXP_E_CH == RESULT_E_CH);
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;
        (void) hG;

        TYPE  CH  = 0;
        TYPE *STR = &CH;

        Obj        mX(STR);
        const Obj& X = mX;

        ASSERT_SAFE_PASS(X.starts_with(STR));
        ASSERT_SAFE_FAIL(X.starts_with(NULL_PTR));

        ASSERT_SAFE_PASS(X.ends_with(STR));
        ASSERT_SAFE_FAIL(X.ends_with(NULL_PTR));
    }
#endif
}
#endif

template <class TYPE, class TRAITS>
void TestDriver<TYPE,TRAITS>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON OPERATORS
    //   We are going to use fully tested 'compare' method as a control
    //   function for the 'operator<'.  Other operators are expected to be
    //   bound with the 'operator<', so the expected results for them can be
    //   logically evaluated from its result.
    //
    // Concerns:
    //: 1 The 'operator<' returns the correct result of lexicographic
    //:   comparison of two objects.
    //:
    //: 2 The 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   the 'operator<'.
    //:
    //: 3 The empty object is comparable.
    //:
    //: 4 The zero-length object is comparable.
    //
    // Plan:
    //: 1 Create an empty object, a zero-length object and compare them.
    //:
    //: 2 Specify a set 'S' of unique object values having various
    //:   minor or subtle differences.
    //:
    //: 3 Compare each element in the 'S' with empty and zero-length objects
    //:   and verify the results.  (C-3..4)
    //:
    //: 4 Using the 'compare' method verify the correctness of comparison
    //:   operators behavior using all elements of the cross product S X S.
    //:   (C-1..2)
    //
    // Testing:
    //   bool operator<(basic_string_view lhs, basic_string_view rhs);
    //   bool operator>(basic_string_view lhs, basic_string_view rhs);
    //   bool operator<=(basic_string_view lhs, basic_string_view rhs);
    //   bool operator>=(basic_string_view lhs, basic_string_view rhs);
    // ------------------------------------------------------------------------

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef bool (*OP)(Obj, Obj);

        OP op = &bsl::operator<;
        (void) op;
        op    = &bsl::operator<=;
        (void) op;
        op    = &bsl::operator>;
        (void) op;
        op    = &bsl::operator>=;
        (void) op;
    }
#endif

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE *STRING   = s_testString;
    const TYPE *NULL_PTR = 0;

    Obj        mXEmpty(NULL_PTR, 0);
    const Obj& XEmpty = mXEmpty;

    Obj        mXZero(STRING, 0);
    const Obj& XZero  = mXZero;

    ASSERTV(!(XEmpty <  XEmpty));
    ASSERTV(!(XEmpty >  XEmpty));
    ASSERTV( (XEmpty <= XEmpty));
    ASSERTV( (XEmpty >= XEmpty));

    ASSERTV(!(XZero  <  XZero ));
    ASSERTV(!(XZero  >  XZero ));
    ASSERTV( (XZero  <= XZero ));
    ASSERTV( (XZero  >= XZero ));

    ASSERTV(!(XEmpty <  XZero ));
    ASSERTV(!(XEmpty >  XZero ));
    ASSERTV( (XEmpty <= XZero ));
    ASSERTV( (XEmpty >= XZero ));

    const size_type MAX_LENGTH = 5;

    static const struct {
        int        d_lineNum;             // line number
        const TYPE d_string[MAX_LENGTH];  // string_view basis
        size_type  d_length;              // string_view length
    } DATA[] = {
        //LINE   STRING               CLOSURE    LENGTH
        //----   -------------------  -------    ------
        { L_,   { 0                   , 0    },  1     },
        { L_,   { 'a'                 , 0    },  1     },
        { L_,   { 'b'                 , 0    },  1     },
        { L_,   { 0  , 0              , 0    },  2     },
        { L_,   { 0  , 'a'            , 0    },  2     },
        { L_,   { 'a', 0              , 0    },  2     },
        { L_,   { 'a', 'a'            , 0    },  2     },
        { L_,   { 'a', 'b'            , 0    },  2     },
        { L_,   { 'b', 'a'            , 0    },  2     },
        { L_,   { 'b', 'b'            , 0    },  2     },
        { L_,   { 0  , 0  , 0         , 0    },  3     },
        { L_,   { 0  , 0  , 'a'       , 0    },  3     },
        { L_,   { 0  , 'a', 0         , 0    },  3     },
        { L_,   { 0  , 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 0  , 0         , 0    },  3     },
        { L_,   { 'a', 0  , 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 0         , 0    },  3     },
        { L_,   { 'a', 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 'b'       , 0    },  3     },
        { L_,   { 'a', 'b', 'a'       , 0    },  3     },
        { L_,   { 'a', 'b', 'b'       , 0    },  3     },
        { L_,   { 'b', 'a', 'a'       , 0    },  3     },
        { L_,   { 'b', 'a', 'b'       , 0    },  3     },
        { L_,   { 'b', 'b', 'a'       , 0    },  3     },
        { L_,   { 'b', 'b', 'b'       , 0    },  3     },
        { L_,   { 'd', 'e', 'f'       , 0    },  3     },
        { L_,   { 0  , 0  , 0  , 0    , 0    },  4     },
        { L_,   { 0  , 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 0    , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 0    , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 0    , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 0    , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 0    , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 0    , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 0    , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'k', 'l', 'm', 'n'  , 0    },  4     },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_type i = 0; i < NUM_DATA; ++i) {
        const int          LINE1   = DATA[i].d_lineNum;
        const TYPE * const STRING1 = DATA[i].d_string;
        const size_type    LENGTH1 = DATA[i].d_length;

        Obj        mX1(STRING1, LENGTH1);
        const Obj& X1 = mX1;

        ASSERTV(LINE1,    XEmpty <  X1 );
        ASSERTV(LINE1,  !(XEmpty >  X1));
        ASSERTV(LINE1,    XEmpty <= X1 );
        ASSERTV(LINE1,  !(XEmpty >= X1));

        ASSERTV(LINE1,    XZero <  X1 );
        ASSERTV(LINE1,  !(XZero >  X1));
        ASSERTV(LINE1,    XZero <= X1 );
        ASSERTV(LINE1,  !(XZero >= X1));

        for (size_type j = 0; j < NUM_DATA; ++j) {
            const int          LINE2   = DATA[j].d_lineNum;
            const TYPE * const STRING2 = DATA[j].d_string;
            const size_type    LENGTH2 = DATA[j].d_length;

            Obj        mX2(STRING2, LENGTH2);
            const Obj& X2 = mX2;

            const int  CMP_RESULT       = X1.compare(X2);
            const bool EXP_LESS         = ( 0 > CMP_RESULT);
            const bool EXP_GREATER      = ( 0 < CMP_RESULT);
            const bool EXP_LESS_EQUAL   = ( 0 == CMP_RESULT || EXP_LESS);
            const bool EXP_LESS_GREATER = ( 0 == CMP_RESULT || EXP_GREATER);

            ASSERTV(LINE1, LINE2, EXP_LESS         == (X1 <  X2));
            ASSERTV(LINE1, LINE2, EXP_GREATER      == (X1 >  X2));
            ASSERTV(LINE1, LINE2, EXP_LESS_EQUAL   == (X1 <= X2));
            ASSERTV(LINE1, LINE2, EXP_LESS_GREATER == (X1 >= X2));
        }
    }
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING HASHAPPEND
    //   Verify that the hashAppend function works properly and is picked up by
    //   'bslh::Hash'
    //
    // Concerns:
    //: 1 Objects constructed with the same values hash as equal.
    //:
    //: 2 Objects, having different internal presentations, but considered to
    //:   be equal, hash as equal.
    //:
    //: 3 Unequal objects hash as unequal (not required, but we can hope).
    //:
    //: 4 Empty objects can be hashed.
    //:
    //: 5 Zero-length objects can be hashed.
    //
    // Plan:
    //: 1 Create an empty object, a zero-length object and hash them.
    //:
    //: 2 Specify a set S of unique object values.
    //:
    //: 3 Verify the correctness of hash values matching using all elements of
    //:   the cross product S X S.  (C-1..3)
    //:
    //: 4 Compare hashes of objects from S with hashes from P-1 and verify the
    //:   results.  (C-4..5)
    //:
    //: 5 Create two objects, having the same lengths and referring to the
    //:   different strings, having the same values.  Hash them and verify that
    //:   hashes are equal.  (C-3)
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const basic_string_view& str);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;
    Hasher                              hasher;

    const TYPE *STRING   = s_testString;
    const TYPE *NULL_PTR = 0;

    Obj             mXEmpty(NULL_PTR, 0);
    const Obj&      XEmpty    = mXEmpty;
    const size_type hashEmpty = hasher(XEmpty);

    Obj             mXZero(STRING, 0);
    const Obj&      XZero          = mXZero;
    const size_type hashZero = hasher(XZero);

    const size_type MAX_LENGTH = 5;

    static const struct {
        int        d_lineNum;             // line number
        const TYPE d_string[MAX_LENGTH];  // string_view basis
        size_type  d_length;              // string_view length
    } DATA[] = {
        //LINE   STRING               CLOSURE    LENGTH
        //----   -------------------  -------    ------
        { L_,   { 0                   , 0    },  1     },
        { L_,   { 'a'                 , 0    },  1     },
        { L_,   { 'b'                 , 0    },  1     },
        { L_,   { 0  , 0              , 0    },  2     },
        { L_,   { 0  , 'a'            , 0    },  2     },
        { L_,   { 'a', 0              , 0    },  2     },
        { L_,   { 'a', 'a'            , 0    },  2     },
        { L_,   { 'a', 'b'            , 0    },  2     },
        { L_,   { 'b', 'a'            , 0    },  2     },
        { L_,   { 'b', 'b'            , 0    },  2     },
        { L_,   { 0  , 0  , 0         , 0    },  3     },
        { L_,   { 0  , 0  , 'a'       , 0    },  3     },
        { L_,   { 0  , 'a', 0         , 0    },  3     },
        { L_,   { 0  , 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 0  , 0         , 0    },  3     },
        { L_,   { 'a', 0  , 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 0         , 0    },  3     },
        { L_,   { 'a', 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 'b'       , 0    },  3     },
        { L_,   { 'a', 'b', 'a'       , 0    },  3     },
        { L_,   { 'a', 'b', 'b'       , 0    },  3     },
        { L_,   { 'b', 'a', 'a'       , 0    },  3     },
        { L_,   { 'b', 'a', 'b'       , 0    },  3     },
        { L_,   { 'b', 'b', 'a'       , 0    },  3     },
        { L_,   { 'b', 'b', 'b'       , 0    },  3     },
        { L_,   { 'd', 'e', 'f'       , 0    },  3     },
        { L_,   { 0  , 0  , 0  , 0    , 0    },  4     },
        { L_,   { 0  , 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 0    , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 0    , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 0    , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 0    , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 0    , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 0    , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 0    , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'k', 'l', 'm', 'n'  , 0    },  4     },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_type i = 0; i < NUM_DATA; ++i) {
        const int          LINE1   = DATA[i].d_lineNum;
        const TYPE * const STRING1 = DATA[i].d_string;
        const size_type    LENGTH1 = DATA[i].d_length;

        Obj            mX1(STRING1, LENGTH1);
        const Obj&     X1    = mX1;
        const HashType HASH1 = hasher(X1);

        ASSERTV(LINE1, hashEmpty, HASH1, hashEmpty != HASH1);
        ASSERTV(LINE1, hashZero,  HASH1, hashZero  != HASH1);

        for (size_type j = 0; j < NUM_DATA; ++j) {
            const int          LINE2   = DATA[j].d_lineNum;
            const TYPE * const STRING2 = DATA[j].d_string;
            const size_type    LENGTH2 = DATA[j].d_length;

            Obj            mX2(STRING2, LENGTH2);
            const Obj&     X2    = mX2;
            const HashType HASH2 = hasher(X2);

            ASSERTV(LINE1, LINE2, HASH1, HASH2, (i == j) == (HASH1 == HASH2));
        }
    }

    {
        const TYPE STRING1[MAX_LENGTH] = {'a', 'b', 'c', 'd', 0};
        const TYPE STRING2[MAX_LENGTH] = {'a', 'b', 'c', 'd', 0};

        Obj            mX1(STRING1);
        const Obj&     X1    = mX1;
        const HashType HASH1 = hasher(X1);

        Obj            mX2(STRING2);
        const Obj&     X2    = mX2;
        const HashType HASH2 = hasher(X2);

        ASSERTV(X1.data() != X2.data());
        ASSERTV(X1        == X2       );
        ASSERTV(HASH1     == HASH2    );
    }
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARE
    //   The idea is to test some method's overload thoroughly and then use it
    //   as a control function for other overloads. For non-empty objects this
    //   function is
    //   'compare(size_type, size_type, const CHAR_TYPE *, size_type)', whereas
    //   its behavior is checked with the similar 'std::basic_string::compare'
    //   method. For empty objects such function is
    //   'compare(basic_string_view other)', because all possible combinations
    //   of parameters for other overloads can be represented as 'string_view'
    //   object.
    //
    // Concerns:
    //: 1 Empty and zero-length objects can be passed for comparison.
    //:
    //: 2 The "compare" methods stick to the borders of the compared objects
    //:   (e.g. they check only the available number of characters, even if
    //:   user requests more).
    //:
    //: 3 The null symbol ('/0') is correctly handled whether it belongs to the
    //:   objects or to the strings for comparison.
    //:
    //: 4 The default parameter values are exactly as expected.
    //:
    //: 5 The 'compare' throws 'std::out_of_range' exception when passed an
    //:   out-of-bound position (e.g. 'position > length()').
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create an empty object, a zero-length object.
    //:
    //: 2 Using each 'compare' interface, compare empty object with itself (or
    //:   with the null pointer) and verify the results.
    //:
    //: 3 Specify a set S of unique non-empty object values.
    //:
    //: 4 Using 'compare(basic_string_view other)' interface, compare each
    //:   value in 'S' with empty and zero-length objects. Verify the
    //:   results.
    //:
    //: 5 Using interface tested in P-4 as a control function, verify the
    //:   return values of other interfaces comparing values in 'S' and
    //:   empty/zero-length objects.  (C-1)
    //:
    //: 6 For each object in 'S' create the 'bsl::basic_string' object 'str'
    //:   passing the same parameters on construction, that for object in 'S'
    //:   were passed earlier.
    //:
    //: 7 Using 'compare(size_type, size_type, const CHAR_TYPE *, size_type)'
    //:   interface, compare each pair (s1, s2) in the cross product S X S and
    //:   verify the results by calling the 'std::basic_string::compare' method
    //:   for the 'str' object with the same parameters.
    //:
    //: 8 Using interface tested in P-7 as a control function, verify the
    //:   return values of other interfaces comparing each pair (s1, s2) in the
    //:   cross product S X S.  (C-2..3)
    //:
    //: 9 Compare each pair (s1, s2) in the cross product S X S using interface
    //:   'compare(size_t, size_t, basic_string_view, size_t, size_t)', but
    //:   passing only first four parameters.  Verify the results by calling
    //:   the same interface with the same parameters and explicitly passing
    //:   'npos' value as the last parameter.  (C-4)
    //:
    //:10 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //:
    //:11 Using 'try-catch' block, verify for each appropriate interface that
    //:   'std::out_of_range' exception is thrown by the 'copy' when
    //:   'position > length()'.  (C-5)
    //
    // Testing:
    //   int compare(basic_string_view other) const;
    //   int compare(size_type p, size_type n, basic_string_view o) const;
    //   int compare(size_t, size_t, basic_string_view, size_t, size_t) const;
    //   int compare(const CHAR_TYPE *other) const;
    //   int compare(size_type lP, size_type lNs, const CHAR_TYPE *o) const;
    //   int compare(size_type, size_type, const CHAR_TYPE *, size_type) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const TYPE      *NULL_PTR      = 0;

    const size_type  NPOS          = Obj::npos;
    const size_type  MAX_LENGTH    = 5;

    static const struct {
        int        d_lineNum;             // line number
        const TYPE d_string[MAX_LENGTH];  // string_view basis
        size_type  d_length;              // string_view length
    } DATA[] = {
        //LINE   STRING               CLOSURE    LENGTH
        //----   -------------------  -------    ------
        { L_,   { 0                   , 0    },  1     },
        { L_,   { 'a'                 , 0    },  1     },
        { L_,   { 'b'                 , 0    },  1     },
        { L_,   { 0  , 0              , 0    },  2     },
        { L_,   { 0  , 'a'            , 0    },  2     },
        { L_,   { 'a', 0              , 0    },  2     },
        { L_,   { 'a', 'a'            , 0    },  2     },
        { L_,   { 'a', 'b'            , 0    },  2     },
        { L_,   { 'b', 'a'            , 0    },  2     },
        { L_,   { 'b', 'b'            , 0    },  2     },
        { L_,   { 0  , 0  , 0         , 0    },  3     },
        { L_,   { 0  , 0  , 'a'       , 0    },  3     },
        { L_,   { 0  , 'a', 0         , 0    },  3     },
        { L_,   { 0  , 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 0  , 0         , 0    },  3     },
        { L_,   { 'a', 0  , 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 0         , 0    },  3     },
        { L_,   { 'a', 'a', 'a'       , 0    },  3     },
        { L_,   { 'a', 'a', 'b'       , 0    },  3     },
        { L_,   { 'a', 'b', 'a'       , 0    },  3     },
        { L_,   { 'a', 'b', 'b'       , 0    },  3     },
        { L_,   { 'b', 'a', 'a'       , 0    },  3     },
        { L_,   { 'b', 'a', 'b'       , 0    },  3     },
        { L_,   { 'b', 'b', 'a'       , 0    },  3     },
        { L_,   { 'b', 'b', 'b'       , 0    },  3     },
        { L_,   { 'd', 'e', 'f'       , 0    },  3     },
        { L_,   { 0  , 0  , 0  , 0    , 0    },  4     },
        { L_,   { 0  , 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 0    , 0    },  4     },
        { L_,   { 0  , 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 0    , 0    },  4     },
        { L_,   { 0  , 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 0    , 0    },  4     },
        { L_,   { 0  , 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 0    , 0    },  4     },
        { L_,   { 'a', 0  , 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 0    , 0    },  4     },
        { L_,   { 'a', 0  , 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 0    , 0    },  4     },
        { L_,   { 'a', 'a', 0  , 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 0    , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'a', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'a', 'b', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'a', 'b'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'a'  , 0    },  4     },
        { L_,   { 'b', 'b', 'b', 'b'  , 0    },  4     },
        { L_,   { 'k', 'l', 'm', 'n'  , 0    },  4     },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting basic usage.\n");

    // Testing empty object.  Note that we cannot have the value of 'position'
    // parameter greater than the length of the object (zero for an empty
    // object).  Otherwise we will get an exception.

    if (verbose) printf("\t\tTesting empty object.\n");
    {
        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        // Comparing two empty objects or empty object with empty string.

        {
            ASSERTV( 0 == XEmpty.compare(XEmpty));

            for (size_type i = 0; i < MAX_LENGTH; ++i) {
                const size_type LHS_NUM_CHARS = i;

                ASSERTV( 0 == XEmpty.compare(0, LHS_NUM_CHARS, XEmpty));

                for (size_type j = 0; j < MAX_LENGTH; ++j) {
                    const size_type RHS_NUM_CHARS = j;

                    ASSERTV( 0 == XEmpty.compare(0,
                                                 LHS_NUM_CHARS,
                                                 XEmpty,
                                                 0,
                                                 RHS_NUM_CHARS));
                }

                ASSERTV( 0 == XEmpty.compare(0, LHS_NUM_CHARS, NULL_PTR, 0));
            }

            // ASSERTV( 0 == XEmpty.compare(NULL_PTR));                   // UB
            // ASSERTV( 0 == XEmpty.compare(0, 0, NULL_PTR));             // UB
        }

        // Testing control interface 'compare(basic_string_view other)'.  All
        // other interfaces can be tested further using previously tested
        // interfaces to verify return values.

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            Obj        mXZero(STRING, 0);
            const Obj& XZero = mXZero;

            ASSERTV(LINE, 0 >  XEmpty.compare(X     ));
            ASSERTV(LINE, 0 <  X.compare(     XEmpty));

            ASSERTV(LINE, 0 == XEmpty.compare(XZero ));
            ASSERTV(LINE, 0 == XZero.compare( XEmpty));
        }

        // int compare(size_type position, size_type n, basic_string_view o);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            Obj        mXZero(STRING, 0);
            const Obj& XZero = mXZero;

            // We can only use null values for 'position' and 'numChars',
            // because length of empty object is equal to zero.

            ASSERTV(LINE, 0 > XEmpty.compare(0, 0, X));

            // But for the non-empty objects we need to iterate through the
            // range of positions and through the range of numbers of requested
            // symbols.

            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type LHS_POSITION = j;

                for (size_type k = 0; k <= MAX_LENGTH; ++k) {
                    const size_type LHS_NUM_CHARS = k;

                    Obj        mXSub = X.substr(LHS_POSITION, LHS_NUM_CHARS);
                    const Obj& XSub = mXSub;

                    const int EXP    = XSub.compare(XEmpty);
                    const int RESULT = X.compare(LHS_POSITION,
                                                 LHS_NUM_CHARS,
                                                 XEmpty);

                    ASSERTV(LINE, LHS_POSITION, LHS_NUM_CHARS, EXP, RESULT,
                            EXP == RESULT);
                }
            }

            ASSERTV(LINE, 0 == XEmpty.compare(0, 0, XZero ));
            ASSERTV(LINE, 0 == XZero.compare( 0, 0, XEmpty));
        }

        // int compare(size_t, size_t, basic_string_view, size_t, size_t);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            Obj        mXZero(STRING, 0);
            const Obj& XZero = mXZero;

            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type POSITION = j;

                for (size_type k = 0; k <= MAX_LENGTH; ++k) {
                    const size_type NUM_CHARS = k;

                    Obj        mXSub = X.substr(POSITION, NUM_CHARS);
                    const Obj& XSub = mXSub;

                    // Empty is compared with non-empty.
                    {
                        const int EXP    = XEmpty.compare(XSub);
                        const int RESULT = XEmpty.compare(0,
                                                          0,
                                                          X,
                                                          POSITION,
                                                          NUM_CHARS);

                        ASSERTV(LINE, POSITION, NUM_CHARS, EXP, RESULT,
                                EXP == RESULT);
                    }

                    // Non-empty is compared with empty.
                    {
                        const int EXP    = XSub.compare(XEmpty);
                        const int RESULT = X.compare(POSITION,
                                                     NUM_CHARS,
                                                     XEmpty,
                                                     0,
                                                     0);

                        ASSERTV(LINE, POSITION, NUM_CHARS, EXP, RESULT,
                                EXP == RESULT);
                    }

                }
            }

            ASSERTV(LINE, 0 == XEmpty.compare(0, 0, XZero , 0, 0));
            ASSERTV(LINE, 0 == XZero.compare( 0, 0, XEmpty, 0, 0));
        }

        // int compare(const CHAR_TYPE *other);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE = DATA[i].d_lineNum;
            const TYPE * const STR  = DATA[i].d_string;

            Obj        mX(STR);
            const Obj& X = mX;

            const int EXP    = XEmpty.compare(X  );
            const int RESULT = XEmpty.compare(STR);

            ASSERTV(LINE, EXP, RESULT, EXP == RESULT);
        }

        // int compare(size_type lP, size_type lNs, const CHAR_TYPE *other);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE = DATA[i].d_lineNum;
            const TYPE * const STR  = DATA[i].d_string;

            Obj        mX(STR);
            const Obj& X = mX;

            const int EXP    = XEmpty.compare(0, 0, X  );
            const int RESULT = XEmpty.compare(0, 0, STR);

            ASSERTV(LINE, EXP, RESULT, EXP == RESULT);
        }

        // int compare(size_type, size_type, const CHAR_TYPE *, size_type);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            for (size_type j = 0; j <= LENGTH; ++j) {
                const size_type LHS_NUM_CHARS = j;

                Obj        mX(STR, LHS_NUM_CHARS);
                const Obj& X = mX;

                const int EXP    = XEmpty.compare(0, 0, X                    );
                const int RESULT = XEmpty.compare(0, 0, STRING, LHS_NUM_CHARS);

                ASSERTV(LINE, LHS_NUM_CHARS, EXP, RESULT, EXP == RESULT);
            }
        }
    }

    if (verbose) printf("\t\tTesting non-empty object.\n");
    {
        bsl::basic_string<TYPE> sampleString;
        sampleString.reserve(MAX_LENGTH);

        bsl::basic_string<TYPE> rhsSampleString;
        rhsSampleString.reserve(MAX_LENGTH);
        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            sampleString.clear();
            sampleString.append(STR, LENGTH);

            // Firstly iterate through available positions and lengths of LHS
            // (our object).  Position must be less than the length of the
            // object, while for the requested lengths we do not have such
            // restrictions.

            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type LHS_POSITION = j;

                for (size_type k = 0; k <= MAX_LENGTH; ++k) {
                    const size_type LHS_NUM_CHARS = k;

                    // Now create RHS.  As we are going to use
                    // 'int compare(size_type,
                    //              size_type,
                    //              const CHAR_TYPE *,
                    //              size_type)'
                    // interface as a control function for other interfaces, we
                    // need to iterate through the 'NUM_DATA' table to test it
                    // with all variants of string and through the available
                    // lengths for these strings.

                    for (size_type l = 0; l < NUM_DATA; ++l) {
                        const int          RHS_LINE   = DATA[l].d_lineNum;
                        const TYPE * const RHS_STRING = DATA[l].d_string;

                        for (size_type m = 0; m <= MAX_LENGTH; ++m) {
                            const size_type RHS_NUM_CHARS = m;

                            const int EXP    = sampleString.compare(
                                                                LHS_POSITION,
                                                                LHS_NUM_CHARS,
                                                                RHS_STRING,
                                                                RHS_NUM_CHARS);
                            const int RESULT = X.compare(LHS_POSITION,
                                                         LHS_NUM_CHARS,
                                                         RHS_STRING,
                                                         RHS_NUM_CHARS);

                            // Note, that we cannot directly compare 'EXP' and
                            // 'RESULT', because the contract of the
                            // 'bsl::string::compare' does not specify the
                            // returned value strictly.

                            if (0 >EXP) {
                                ASSERTV(LINE, LHS_POSITION, LHS_NUM_CHARS,
                                        RHS_LINE, RHS_NUM_CHARS, RESULT,
                                        0 > RESULT);
                            }
                            else if (0 == EXP) {
                                ASSERTV(LINE, LHS_POSITION, LHS_NUM_CHARS,
                                        RHS_LINE, RHS_NUM_CHARS, RESULT,
                                        0 == RESULT);
                            }
                            else {
                                ASSERTV(LINE, LHS_POSITION, LHS_NUM_CHARS,
                                        RHS_LINE, RHS_NUM_CHARS, RESULT,
                                        0 <  RESULT);
                            }

                        }
                    }
                }
            }

            // Now we can use fully tested interface to check return values of
            // other interfaces.  We need to iterate through the 'NUM_DATA'
            // table to get RHS string/string_view.

            for (size_type j = 0; j < NUM_DATA; ++j) {
                const int          RHS_LINE   = DATA[j].d_lineNum;
                const TYPE * const RHS_STRING = DATA[j].d_string;
                const size_type    RHS_LENGTH = DATA[j].d_length;

                Obj        mXRHS(RHS_STRING, RHS_LENGTH);
                const Obj& XRHS = mXRHS;

                // int compare(basic_string_view other)
                {
                    const int EXP    = X.compare(0,
                                                 LENGTH,
                                                 RHS_STRING,
                                                 RHS_LENGTH);
                    const int RESULT = X.compare(XRHS);

                    ASSERTV(LINE, RHS_LINE, EXP, RESULT, EXP == RESULT);
                }

                // int compare(const CHAR_TYPE *other)
                {
                    const int EXP    = X.compare(0,
                                                 LENGTH,
                                                 RHS_STRING,
                                                 TRAITS::length(RHS_STRING));
                    const int RESULT = X.compare(RHS_STRING);
                    ASSERTV(LINE, RHS_LINE, EXP, RESULT, EXP == RESULT);
                }

                for (size_type k = 0; k < LENGTH; ++k) {
                    const size_type LHS_POSITION = k;

                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type LHS_NUM_CHARS = l;

                        // compare(size_type, size_type, basic_string_view);
                        {
                            const int EXP    = X.compare(LHS_POSITION,
                                                         LHS_NUM_CHARS,
                                                         RHS_STRING,
                                                         RHS_LENGTH);
                            const int RESULT = X.compare(LHS_POSITION,
                                                         LHS_NUM_CHARS,
                                                         XRHS);
                            ASSERTV(LINE, RHS_LINE, EXP, RESULT,
                                    EXP == RESULT);
                        }

                        // compare(size_type, size_type, const CHAR_TYPE *);
                        {
                            const int EXP    = X.compare(
                                                   LHS_POSITION,
                                                   LHS_NUM_CHARS,
                                                   RHS_STRING,
                                                   TRAITS::length(RHS_STRING));
                            const int RESULT = X.compare(LHS_POSITION,
                                                         LHS_NUM_CHARS,
                                                         RHS_STRING);
                            ASSERTV(LINE, RHS_LINE, EXP, RESULT,
                                    EXP == RESULT);
                        }

                        // int compare(size_type         lhsPosition,
                        //             size_type         lhsNumChars,
                        //             basic_string_view other,
                        //             size_type         otherPosition
                        //             size_type         otherNumChars);

                        for (size_type m = 0; m < RHS_LENGTH; ++m) {
                            const size_type RHS_POSITION = m;

                            for (size_type n = 0; n <= MAX_LENGTH; ++n) {
                                const size_type RHS_NUM_CHARS = n;

                                // There is a subtle point here.  Control
                                // method  'compare(size_type,
                                //                  size_type,
                                //                  const CHAR_TYPE *,
                                //                  size_type)'
                                // does not limit the 'otherNumChars'
                                // parameter.  Whereas tested method limits its
                                // value to the number of available characters
                                // of the 'other' object.  So we have to
                                // evaluate the last parameter of the control
                                // method explicitly.

                                const size_type EXP_RHS_NUM_CHARS =
                                      RHS_NUM_CHARS > RHS_LENGTH - RHS_POSITION
                                    ? RHS_LENGTH - RHS_POSITION
                                    : RHS_NUM_CHARS;

                                const int EXP    = X.compare(
                                                     LHS_POSITION,
                                                     LHS_NUM_CHARS,
                                                     RHS_STRING + RHS_POSITION,
                                                     EXP_RHS_NUM_CHARS);

                                const int RESULT = X.compare(LHS_POSITION,
                                                             LHS_NUM_CHARS,
                                                             XRHS,
                                                             RHS_POSITION,
                                                             RHS_NUM_CHARS);

                                ASSERTV(LINE    , LHS_POSITION,
                                        RHS_LINE, RHS_POSITION,
                                        EXP, RESULT,
                                        EXP == RESULT);
                            }

                            // Additionally we want to check 'npos' value to
                            // test default values further.

                            {
                                const size_type EXP_RHS_NUM_CHARS =
                                                     RHS_LENGTH - RHS_POSITION;

                                const int EXP    = X.compare(
                                                     LHS_POSITION,
                                                     LHS_NUM_CHARS,
                                                     RHS_STRING + RHS_POSITION,
                                                     EXP_RHS_NUM_CHARS);

                                const int RESULT = X.compare(LHS_POSITION,
                                                             LHS_NUM_CHARS,
                                                             XRHS,
                                                             RHS_POSITION,
                                                             NPOS);

                                ASSERTV(LINE    , LHS_POSITION,
                                        RHS_LINE, RHS_POSITION,
                                        EXP, RESULT,
                                        EXP == RESULT);
                            }
                        }
                    }
                }
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj           mXEmpty;
        const Obj&    XEmpty = mXEmpty;
        Obj           mXNonEmpty(STRING, 0);
        const Obj&    XNonEmpty = mXNonEmpty;

        const size_type NPOS = Obj::npos;

        typedef typename bsl::basic_string_view<TYPE, DummyTrait<TYPE> >
                                                               DummyStringView;
        DummyStringView        mXDummy;
        const DummyStringView& XDummy = mXDummy;

        ASSERT_SAFE_PASS(XEmpty.compare(   STRING  ));
        ASSERT_SAFE_FAIL(XEmpty.compare(   NULL_PTR));
        ASSERT_SAFE_PASS(XNonEmpty.compare(STRING  ));
        ASSERT_SAFE_FAIL(XNonEmpty.compare(NULL_PTR));

        ASSERT_SAFE_PASS(XEmpty.compare(   0, 0, STRING            ));
        ASSERT_SAFE_FAIL(XEmpty.compare(   0, 0, NULL_PTR          ));
        ASSERT_SAFE_PASS(XNonEmpty.compare(0, 0, STRING            ));
        ASSERT_SAFE_FAIL(XNonEmpty.compare(0, 0, NULL_PTR          ));

        ASSERT_SAFE_PASS(XEmpty.compare(   0, 0, NULL_PTR, 0       ));
        ASSERT_SAFE_FAIL(XEmpty.compare(   0, 0, NULL_PTR, 1       ));
        ASSERT_SAFE_PASS(XNonEmpty.compare(0, 0, NULL_PTR, 0       ));
        ASSERT_SAFE_FAIL(XNonEmpty.compare(0, 0, NULL_PTR, 1       ));

        ASSERT_SAFE_PASS(XEmpty.compare(   0, 0, STRING            ));
        ASSERT_SAFE_PASS(XDummy.compare(   0, 0, STRING            ));

        ASSERT_SAFE_PASS(XNonEmpty.compare(0, 0, STRING  , NPOS - 1));
        ASSERT_SAFE_PASS(XNonEmpty.compare(0, 0, STRING  , NPOS    ));
    }
#endif

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tTesting exceptions.\n");
    {
        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        // int compare(size_type position, size_type n, basic_string_view o);

        bool outOfRangeCaught = false;

        try {
            (void) XEmpty.compare(1, 0, XEmpty);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);

        // int compare(size_t, size_t, basic_string_view, size_t, size_t);

        outOfRangeCaught = false;

        try {
            (void) XEmpty.compare(1, 0, XEmpty, 0 , 0);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);

        outOfRangeCaught = false;

        try {
            (void) XEmpty.compare(0, 0, XEmpty, 1 , 0);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);

        // int compare(size_type lP, size_type lNs, const CHAR_TYPE *other);

        outOfRangeCaught = false;

        try {
            (void) XEmpty.compare(1, 0, STRING);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);

        // int compare(size_type, size_type, const CHAR_TYPE *, size_type);

        outOfRangeCaught = false;

        try {
            (void) XEmpty.compare(1, 0, NULL_PTR, 0);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);
    }
#endif
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING SUBSTR
    //
    // Concerns:
    //: 1 The requested length of substring exceeding the length of object is
    //:   correctly handled.
    //:
    //: 2 Calling the 'substr' method for an empty object leads to undefined
    //:   behavior.
    //:
    //: 3 The default parameter values are exactly as expected.
    //:
    //: 4 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using a loop-based approach, construct a set of objects, 'O', on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   each address in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having each length in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'o' in the
    //:   set 'O':
    //:
    //:   1 Iterate through the range of positions and the range of the lengths
    //:     and call the 'substr' method for 'o'.  Verify the returned value
    //:     using basic accessors.  (C-1)
    //:
    //:   2 Call the 'substr' method omitting the last parameter and both
    //:     parameters.  Verify the returned values using basic accessors.
    //:     (C-3)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-2,4)
    //
    // Testing:
    //   basic_string_view substr(size_type p = 0, size_type n = npos) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    if (verbose) printf("\tTesting basic behavior.\n");

    // Note that an attempt to get a substring of an empty or zero-length
    // object leads to undefined behavior.  Such scenarios will be tested at
    // the end of this case.

    for (size_type i = 0; i <= STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;

            Obj        mX(START, LENGTH);
            const Obj& X = mX;

            // 'position' should be less than the length of the object.

            for (size_type l = 0; l <= LENGTH; ++l) {
                const size_type POSITION = l;

                // But for the 'numChars' we do not have such restrictions, so
                // we have to check values equal and greater than length of the
                // object.

                for (size_type k = 0; k <= STRING_LENGTH; ++k) {
                    const size_type  NUM_CHARS       = k;
                    const TYPE      *EXPECTED_DATA   = START + POSITION;
                    const size_type  EXPECTED_LENGTH =
                             LENGTH - POSITION > NUM_CHARS ? NUM_CHARS
                                                           : LENGTH - POSITION;

                    const Obj RESULT = X.substr(POSITION, NUM_CHARS);

                    ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                            EXPECTED_DATA   == RESULT.data());
                    ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                            EXPECTED_LENGTH == RESULT.length());
                }

                // Testing default 'numChars' value.
                {
                    const TYPE      *EXPECTED_DATA   = START + POSITION;
                    const size_type  EXPECTED_LENGTH = LENGTH - POSITION;

                    const Obj RESULT = X.substr(POSITION);

                    ASSERTV(OFFSET, LENGTH, POSITION,
                            EXPECTED_DATA   == RESULT.data());
                    ASSERTV(OFFSET, LENGTH, POSITION,
                            EXPECTED_LENGTH == RESULT.length());
                }
            }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED) || \
    (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 900000)
    // Earlier versions of the library do not default the first parameter.

            // Testing default 'position' value.  We can not test method for a
            // zero-length object, because it unconditionally leads to the
            // undefined behavior.

            if (0 != LENGTH) {
                const TYPE      *EXPECTED_DATA   = START;
                const size_type  EXPECTED_LENGTH = LENGTH;

                const Obj RESULT = X.substr();

                ASSERTV(OFFSET, LENGTH,
                        EXPECTED_DATA   == RESULT.data());
                ASSERTV(OFFSET, LENGTH,
                        EXPECTED_LENGTH == RESULT.length());
            }
#endif
        }
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        (void) XEmpty.substr(0);
        (void) XEmpty.substr(0, 10);

        bool caught = false;
        try {
            (void) XEmpty.substr(1);
        }
        catch (const std::out_of_range&) {
            caught = true;
        }
        ASSERT(caught && "XEmpty.substr(1) did not throw as expected");

        Obj         mXZero(STRING, 0);
        const Obj&  XZero = mXZero;

        (void) XZero.substr(0);

        caught = false;
        try {
            (void) XZero.substr(1);
        }
        catch (const std::out_of_range&) {
            caught = true;
        }
        ASSERT(caught && "XZero.substr(1) did not throw as expected");

        Obj        mX(STRING, 1);
        const Obj& X = mX;

        (void) X.substr(0);
        (void) X.substr(0, 10);
        (void) X.substr(1);
        (void) X.substr(1, 10);

        caught = false;
        try {
            (void) X.substr(2);
        }
        catch (const std::out_of_range&) {
            caught = true;
        }
        ASSERT(caught && "X.substr(2) did not throw as expected");
    }
#endif
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING FIND_FIRST_NOT_OF AND FIND_LAST_NOT_OF
    //   Fully tested 'find()' and 'rfind()' methods are used as a control
    //   functions.
    //
    // Concerns:
    //: 1 An empty object can be used as a parameter for search.
    //:
    //: 2 Search operations can be called for an empty object.
    //:
    //: 3 The null pointer can be used as a parameter for search.
    //:
    //: 4 The 'npos' requested position is accepted and correctly handled by
    //:   the 'find_first_not_of' and 'find_last_not_of'.
    //:
    //: 5 The null symbol ('/0') is correctly handled whether it belongs to the
    //:   object or to the requested value for search.
    //:
    //: 6 The default parameter values are exactly as expected.
    //:
    //: 7 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create an object with null parameters.  For this object:
    //:
    //:   1 Iterate positions in the range [0, M], where the 'M' is some
    //:     non-negative integer constant.  For each position:
    //:
    //:     1 Look for an empty object and verify the result.
    //:
    //:     2 Pass a null pointer as a parameter and verify the result.
    //:
    //:   2 Using a loop-based approach, construct a set of the pairs 'P1'
    //:     '(ADDR, LENGTH)', where
    //:     ADDR   is an address in the range '[S, S + N]', where 'S' is a
    //:            some string constant and 'N' is some non-negative integer
    //:            constant
    //:     LENGTH is an integer constant in the range '[0, N - position]',
    //:            where the 'position' is the size of the indent of the ADDR
    //:            from the beginning of 'S'
    //:     For each element 'p' in the set use 'ADDR1' and 'LENGTH1' as a
    //:     parameters to create a non-empty object 'o' and then call each
    //:     "find" method for the empty object from P-1, passing 'o' (or ADDR)
    //:     as a first parameter and iterating through the range of positions
    //:     and the range of the lengths.  Verify the results.  (C-2)
    //:
    //: 2 Construct another set of the pairs 'P2' '(ADDR, LENGTH)', where
    //:     ADDR   is an address referring to some string constant
    //:     LENGTH is an integer constant defining the length of the string
    //:            referred by the 'ADDR'
    //:   Each string referred by 'ADDR' from the 'P2' can contain null
    //:   symbols.  Various strings can contain the same symbols (or even
    //:   sequences of symbols).  Some strings contain the set of unique
    //:   symbols.  For each element 'p1' in the 'P2', containing 'ADDR1' and
    //:   'LENGTH1' values:
    //:
    //:   1 Create an object 'o1' referring to the 'ADDR1' and having 'LENGTH1'
    //:     symbols.
    //:
    //:   2 Call the appropriate overloads of 'find_first_not_of' and
    //:     'find_last_not_of' methods  for the 'o1' looking for a null pointer
    //:     and for an empty object.  Verify the results.  (C-1,3)
    //:
    //:   3 Iterate through the set 'P2' and for each element 'p2', containing
    //:     'ADDR2' and 'LENGTH2' values, create an object 'o2'  referring to
    //:     the 'ADDR2' and having 'LENGTH2' symbols.
    //:
    //:   4 Iterate through the range of positions and the range of the lengths
    //:     and call each of the 'find_first_not_of' and 'find_last_not_of'
    //:     overloads for 'o1', passing 'o2' (or ADDR2) as the first parameter.
    //:     Verify  the results using the control functions 'findFirstNotOf'
    //:     and 'findLastNotOf', based on fully tested 'find' and 'rfind'
    //:     methods.  (C-5)
    //:
    //:   6 Iterate through the range of positions and the range of the lengths
    //:     and call each of the 'find_first_not_of' and 'find_last_not_of'
    //:     overloads for 'o1' passing 'o2' (or ADDR2) as the first parameter
    //:     and the 'npos' value for the position parameter.  Verify the
    //:     results using the control functions 'findFirstNotOf' and
    //:     'findLastNotOf'.  (C-4)
    //:
    //:   7 Call each of the 'find_first_not_of' and 'find_last_not_of'
    //:     overloads for 'o1' passing 'o2' (or ADDR2) as the first parameter
    //:     and omitting default parameters.  Verify the results by calling the
    //:     same overloads with the explicitly passed parameters having the
    //:     appropriate values.  (C-6)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
    //
    // Testing:
    //   size_type find_first_not_of(basic_string_view, size_type) const;
    //   size_type find_first_not_of(const TYPE*, size_type, size_type) const;
    //   size_type find_first_not_of(const CHAR_TYPE*, size_type) const;
    //   size_type find_first_not_of(CHAR_TYPE c, size_type pos) const;
    //   size_type find_last_not_of(basic_string_view, size_type) const;
    //   size_type find_last_not_of(const TYPE*, size_type, size_type) const;
    //   size_type find_last_not_of(const CHAR_TYPE*, size_type) const;
    //   size_type find_last_not_of(CHAR_TYPE, size_type) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING   = s_testString;
    const TYPE      *NULL_PTR = 0;
    const size_type  NPOS     = Obj::npos;

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty object.
    {
        const size_type MAX_INDEX = 2;

        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        // Test search of an empty object in the empty object.

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;

            const size_type RESULT_F = XEmpty.find_first_not_of(XEmpty,
                                                                POSITION);
            const size_type RESULT_L = XEmpty.find_last_not_of( XEmpty,
                                                                POSITION);

            ASSERTV(POSITION, RESULT_F, NPOS == RESULT_F);
            ASSERTV(POSITION, RESULT_L, NPOS == RESULT_L);
        }

        // Test null pointer.  'numChars' can only be equal to 0, otherwise it
        // will be undefined behavior.

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;

            const size_type RESULT_F = XEmpty.find_first_not_of(NULL_PTR,
                                                                POSITION,
                                                                0);
            const size_type RESULT_L = XEmpty.find_last_not_of( NULL_PTR,
                                                                POSITION,
                                                                0);

            ASSERTV(POSITION, RESULT_F, NPOS == RESULT_F);
            ASSERTV(POSITION, RESULT_L, NPOS == RESULT_L);
        }

        // Test non-null pointers.  Note that zero-length objects will also be
        // tested within this section

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            for (size_type j = 0; j <= MAX_INDEX - i; ++j) {
                const size_type  OFFSET = i;
                const size_type  LENGTH = j;
                const TYPE      *START  = STRING + OFFSET;

                Obj         mX(START, LENGTH);
                const Obj&  X = mX;

                for (size_type k = 0; k <= MAX_INDEX; ++k) {
                    const size_type POSITION = k;

                    // Test search of an empty object.
                    {
                        size_type expF = 0;
                        size_type expL = 0;

                        if (POSITION < LENGTH) {
                            expF = POSITION;
                            expL = POSITION;
                        }
                        else {
                            expF = NPOS;
                            expL = LENGTH - 1;
                        }

                        const size_type EXP_F = expF;
                        const size_type EXP_L = expL;

                        const size_type RESULT_F = X.find_first_not_of(
                                                                     XEmpty,
                                                                     POSITION);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                     XEmpty,
                                                                     POSITION);

                        ASSERTV(OFFSET, LENGTH, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(OFFSET, LENGTH, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // Test search by an empty object.

                    // find_X_not_of(const CHAR_TYPE *, size_type, size_type)
                    for (size_type l = 0; l <= MAX_INDEX; ++l) {
                        const size_type NUM_CHARS = l;

                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                                NPOS == XEmpty.find_first_not_of(START,
                                                                 POSITION,
                                                                 NUM_CHARS));
                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                                NPOS == XEmpty.find_last_not_of( START,
                                                                 POSITION,
                                                                 NUM_CHARS));
                    }

                    // 'find_X_not_of(basic_string_view, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_not_of(X, POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_not_of( X, POSITION));
                    }

                    // 'find_X_not_of(const CHAR_TYPE *, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_not_of(START,
                                                                 POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_not_of( START,
                                                                 POSITION));
                    }

                    // 'find_X_not_of(CHAR_TYPE, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_not_of(*START,
                                                                 POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_not_of( *START,
                                                                 POSITION));
                    }
                }
            }
        }
    }

    // Testing non-empty objects.
    {
        const size_type MAX_LENGTH = 4;

        static const struct {
            int        d_lineNum;             // line number
            const TYPE d_string[MAX_LENGTH];  // string_view basis
            size_type  d_length;              // string_view length
        } DATA[] = {
            //LINE   STRING           CLOSURE    LENGTH
            //----   --------------   -------    ------
            { L_,   { 0               , 0    },  1     },
            { L_,   { 'a'             , 0    },  1     },
            { L_,   { 'b'             , 0    },  1     },
            { L_,   { 0  , 0          , 0    },  2     },
            { L_,   { 0  , 'a'        , 0    },  2     },
            { L_,   { 'a', 0          , 0    },  2     },
            { L_,   { 'a', 'a'        , 0    },  2     },
            { L_,   { 'a', 'b'        , 0    },  2     },
            { L_,   { 'b', 'a'        , 0    },  2     },
            { L_,   { 'b', 'b'        , 0    },  2     },
            { L_,   { 0  , 0  , 0     , 0    },  3     },
            { L_,   { 0  , 0  , 'a'   , 0    },  3     },
            { L_,   { 0  , 'a', 0     , 0    },  3     },
            { L_,   { 0  , 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 0  , 0     , 0    },  3     },
            { L_,   { 'a', 0  , 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 0     , 0    },  3     },
            { L_,   { 'a', 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 'b'   , 0    },  3     },
            { L_,   { 'a', 'b', 'a'   , 0    },  3     },
            { L_,   { 'a', 'b', 'b'   , 0    },  3     },
            { L_,   { 'b', 'a', 'a'   , 0    },  3     },
            { L_,   { 'b', 'a', 'b'   , 0    },  3     },
            { L_,   { 'b', 'b', 'a'   , 0    },  3     },
            { L_,   { 'b', 'b', 'b'   , 0    },  3     },
            { L_,   { 'd', 'e', 'f'   , 0    },  3     },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bsl::basic_string<TYPE> sampleString;
        sampleString.reserve(MAX_LENGTH);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            // Test null pointer.
            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type POSITION = j;
                const size_type RESULT_F = X.find_first_not_of(NULL_PTR,
                                                               POSITION,
                                                               0);
                const size_type RESULT_L = X.find_last_not_of( NULL_PTR,
                                                               POSITION,
                                                               0);
                const size_type EXP_F = POSITION;
                const size_type EXP_L = POSITION;

                ASSERTV(LINE, POSITION, EXP_F, RESULT_F, EXP_F == RESULT_F);
                ASSERTV(LINE, POSITION, EXP_L, RESULT_L, EXP_L == RESULT_L);
            }

            // Test non-null pointer.
            sampleString.clear();
            sampleString.append(STRING, LENGTH);

            for (size_type j = 0; j < NUM_DATA; ++j) {
                const int          LINE_P   =  DATA[j].d_lineNum;
                const TYPE * const STRING_P =  DATA[j].d_string;
                const size_type    LENGTH_P =  DATA[j].d_length;
                const TYPE         SYMBOL   = *STRING_P;

                Obj        mPATTERN(STRING_P, LENGTH_P);
                const Obj& PATTERN = mPATTERN;

                for (size_type k = 0; k < MAX_LENGTH; ++k) {
                    const size_type POSITION = k;

                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        const size_type EXP_F = findFirstNotOf(X,
                                                               STRING_P,
                                                               POSITION,
                                                               NUM_CHARS);

                        const size_type EXP_L = findLastNotOf( X,
                                                               STRING_P,
                                                               POSITION,
                                                               NUM_CHARS);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                    STRING_P,
                                                                    POSITION,
                                                                    NUM_CHARS);

                        const size_type RESULT_L = X.find_last_not_of(
                                                                    STRING_P,
                                                                    POSITION,
                                                                    NUM_CHARS);

                        ASSERTV(LINE, LINE_P, POSITION,
                                NUM_CHARS, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);

                        ASSERTV(LINE, LINE_P, POSITION,
                                NUM_CHARS, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // Interfaces without 'numChars'.

                    // 'find_X_not_of(basic_string_view, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               PATTERN,
                                                               POSITION);
                        const size_type EXP_L = findLastNotOf( X,
                                                               PATTERN,
                                                               POSITION);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                     PATTERN,
                                                                     POSITION);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                     PATTERN,
                                                                     POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_not_of(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               STRING_P,
                                                               POSITION);
                        const size_type EXP_L = findLastNotOf( X,
                                                               STRING_P,
                                                               POSITION);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                     STRING_P,
                                                                     POSITION);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                     STRING_P,
                                                                     POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_not_of(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               SYMBOL,
                                                               POSITION);
                        const size_type EXP_L = findLastNotOf( X,
                                                               SYMBOL,
                                                               POSITION);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                     SYMBOL,
                                                                     POSITION);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                     SYMBOL,
                                                                     POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }
                }

                // Also we want to check the scenarios, when the 'POSITION' is
                // equal to the 'npos' value (mostly for the further testing of
                // the default values).
                {
                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        const size_type EXP_F = findFirstNotOf(X,
                                                               STRING_P,
                                                               NPOS,
                                                               NUM_CHARS);
                        const size_type EXP_L = findLastNotOf( X,
                                                               STRING_P,
                                                               NPOS,
                                                               NUM_CHARS);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                    STRING_P,
                                                                    NPOS,
                                                                    NUM_CHARS);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                    STRING_P,
                                                                    NPOS,
                                                                    NUM_CHARS);

                        ASSERTV(LINE, LINE_P, NUM_CHARS, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);

                        ASSERTV(LINE, LINE_P, NUM_CHARS, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // Interfaces without 'numChars'.

                    // 'find_X_not_of(basic_string_view, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               PATTERN,
                                                               NPOS);
                        const size_type EXP_L = findLastNotOf( X,
                                                               PATTERN,
                                                               NPOS);

                        const size_type RESULT_F = X.find_first_not_of(PATTERN,
                                                                       NPOS);
                        const size_type RESULT_L = X.find_last_not_of( PATTERN,
                                                                       NPOS);

                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_not_of(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               STRING_P,
                                                               NPOS);
                        const size_type EXP_L = findLastNotOf( X,
                                                               STRING_P,
                                                               NPOS);

                        const size_type RESULT_F = X.find_first_not_of(
                                                                      STRING_P,
                                                                      NPOS);
                        const size_type RESULT_L = X.find_last_not_of(
                                                                      STRING_P,
                                                                      NPOS);
                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_not_of(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP_F = findFirstNotOf(X,
                                                               SYMBOL,
                                                               NPOS);
                        const size_type EXP_L = findLastNotOf( X,
                                                               SYMBOL,
                                                               NPOS);

                        const size_type RESULT_F = X.find_first_not_of(SYMBOL,
                                                                       NPOS);
                        const size_type RESULT_L = X.find_last_not_of(SYMBOL,
                                                                      NPOS);

                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }
                }

                // Now we can check the correctness of the default values.

                // 'find_first_not_of(basic_string_view, size_type)'
                {
                    const size_type EXP_F    = X.find_first_not_of(PATTERN, 0);
                    const size_type RESULT_F = X.find_first_not_of(PATTERN   );

                    const size_type EXP_L    = X.find_last_not_of( PATTERN,
                                                                   NPOS);
                    const size_type RESULT_L = X.find_last_not_of( PATTERN);

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }

                // 'find_first_not_of(const CHAR_TYPE *, size_type)'
                {
                    const size_type EXP_F    = X.find_first_not_of(STRING_P,
                                                                   0);
                    const size_type RESULT_F = X.find_first_not_of(STRING_P);

                    const size_type EXP_L    = X.find_last_not_of( STRING_P,
                                                                   NPOS);
                    const size_type RESULT_L = X.find_last_not_of( STRING_P);

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }

                // 'find_first_not_of(CHAR_TYPE, size_type)'
                {
                    const size_type EXP_F    = X.find_first_not_of(SYMBOL, 0);
                    const size_type RESULT_F = X.find_first_not_of(SYMBOL   );

                    const size_type EXP_L    = X.find_last_not_of( SYMBOL,
                                                                   NPOS);
                    const size_type RESULT_L = X.find_last_not_of( SYMBOL);

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const size_type STRING_LENGTH = s_testStringLength;
        Obj             mX(STRING, STRING_LENGTH);
        const Obj&      X = mX;

        ASSERT_SAFE_PASS(X.find_first_not_of(STRING  , 0, 1            ));
        ASSERT_SAFE_PASS(X.find_first_not_of(STRING  , 0, 0            ));
        ASSERT_SAFE_PASS(X.find_first_not_of(NULL_PTR, 0, 0            ));
        ASSERT_SAFE_FAIL(X.find_first_not_of(NULL_PTR, 0, 1            ));

        ASSERT_SAFE_PASS(X.find_first_not_of(STRING  , 0               ));
        ASSERT_SAFE_FAIL(X.find_first_not_of(NULL_PTR, 0               ));

        ASSERT_SAFE_PASS(X.find_first_not_of(STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.find_first_not_of(STRING  , 0, Obj::npos    ));

        ASSERT_SAFE_PASS(X.find_last_not_of( STRING  , 0, 1            ));
        ASSERT_SAFE_PASS(X.find_last_not_of( STRING  , 0, 0            ));
        ASSERT_SAFE_PASS(X.find_last_not_of( NULL_PTR, 0, 0            ));
        ASSERT_SAFE_FAIL(X.find_last_not_of( NULL_PTR, 0, 1            ));

        ASSERT_SAFE_PASS(X.find_last_not_of( STRING  , 0               ));
        ASSERT_SAFE_FAIL(X.find_last_not_of( NULL_PTR, 0               ));

        ASSERT_SAFE_PASS(X.find_last_not_of( STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.find_last_not_of( STRING  , 0, Obj::npos    ));
    }
#endif
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING FIND_FIRST_OF AND FIND_LAST_OF
    //   Fully tested 'find()' and 'rfind()' methods are used as a control
    //   functions.
    //
    // Concerns:
    //: 1 An empty object can be used as a parameter for search.
    //:
    //: 2 Search operations can be called for an empty object.
    //:
    //: 3 The null pointer can be used as a parameter for search.
    //:
    //: 4 The 'npos' requested position is accepted and correctly handled by
    //:   the 'find_first_of' and 'find_last_of'.
    //:
    //: 5 The null symbol ('/0') is correctly handled whether it belongs to the
    //:   object or to the requested value for search.
    //:
    //: 6 The default parameter values are exactly as expected.
    //:
    //: 7 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create an object with null parameters.  For this object:
    //:
    //:   1 Iterate positions in the range [0, M], where the 'M' is some
    //:     non-negative integer constant.  For each position:
    //:
    //:     1 Look for an empty object and verify the result.
    //:
    //:     2 Pass a null pointer as a parameter and verify the result.
    //:
    //:   2 Using a loop-based approach, construct a set of the pairs 'P1'
    //:     '(ADDR, LENGTH)', where
    //:     ADDR   is an address in the range '[S, S + N]', where 'S' is a
    //:            some string constant and 'N' is some non-negative integer
    //:            constant
    //:     LENGTH is an integer constant in the range '[0, N - position]',
    //:            where the 'position' is the size of the indent of the ADDR
    //:            from the beginning of 'S'
    //:     For each element 'p' in the set use 'ADDR1' and 'LENGTH1' as a
    //:     parameters to create a non-empty object 'o' and then call each
    //:     "find" method for the empty object from P-1, passing 'o' (or ADDR)
    //:     as a first parameter and iterating through the range of positions
    //:     and the range of the lengths.  Verify the results.  (C-2)
    //:
    //: 2 Construct another set of the pairs 'P2' '(ADDR, LENGTH)', where
    //:     ADDR   is an address referring to some string constant
    //:     LENGTH is an integer constant defining the length of the string
    //:            referred by the 'ADDR'
    //:   Each string referred by 'ADDR' from the 'P2' can contain null
    //:   symbols.  Various strings can contain the same symbols (or even
    //:   sequences of symbols).  Some strings contain the set of unique
    //:   symbols.  For each element 'p1' in the 'P2', containing 'ADDR1' and
    //:   'LENGTH1' values:
    //:
    //:   1 Create an object 'o1' referring to the 'ADDR1' and having 'LENGTH1'
    //:     symbols.
    //:
    //:   2 Call the appropriate overloads of 'find_first_of' and
    //:     'find_last_of' methods  for the 'o1' looking for a null pointer
    //:     and for an empty object.  Verify the results.  (C-1,3)
    //:
    //:   3 Iterate through the set 'P2' and for each element 'p2', containing
    //:     'ADDR2' and 'LENGTH2' values, create an object 'o2'  referring to
    //:     the 'ADDR2' and having 'LENGTH2' symbols.
    //:
    //:   4 Iterate through the range of positions and the range of the lengths
    //:     and call each of the 'find_first_of' and 'find_last_of' overloads
    //:     for 'o1', passing 'o2' (or ADDR2) as the first parameter.  Verify
    //:     the results using the control functions 'findFirstOf' and
    //:     'findLastOf', based on fully tested 'find' and 'rfind' methods.
    //:     (C-5)
    //:
    //:   6 Iterate through the range of positions and the range of the lengths
    //:     and call each of the 'find_first_of' and 'find_last_of' overloads
    //:     for 'o1' passing 'o2' (or ADDR2) as the first parameter and the
    //:     'npos' value for the position parameter.  Verify the results using
    //:     the control functions 'findFirstOf' and 'findLastOf'. (C-4)
    //:
    //:   7 Call each of the 'find_first_of' and 'find_last_of' overloads for
    //:     'o1' passing 'o2' (or ADDR2) as the first parameter and omitting
    //:     default parameters.  Verify the results by calling the same
    //:     overloads with the explicitly passed parameters having the
    //:     appropriate values.  (C-6)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
    //
    // Testing:
    //   size_type find_first_of(basic_string_view, size_type) const;
    //   size_type find_first_of(const CHAR_TYPE*, size_type, size_type) const;
    //   size_type find_first_of(const CHAR_TYPE*, size_type) const;
    //   size_type find_first_of(CHAR_TYPE c, size_type pos) const;
    //   size_type find_last_of(basic_string_view, size_type) const;
    //   size_type find_last_of(const CHAR_TYPE*, size_type, size_type) const;
    //   size_type find_last_of(const CHAR_TYPE*, size_type) const;
    //   size_type find_last_of(CHAR_TYPE, size_type) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING   = s_testString;
    const TYPE      *NULL_PTR = 0;
    const size_type  NPOS     = Obj::npos;

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty object.
    {
        const size_type MAX_INDEX = 2;

        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        // Test search of an empty object in the empty object.

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;

            const size_type RESULT_F = XEmpty.find_first_of(XEmpty, POSITION);
            const size_type RESULT_L = XEmpty.find_last_of( XEmpty, POSITION);

            ASSERTV(POSITION, RESULT_F, NPOS == RESULT_F);
            ASSERTV(POSITION, RESULT_L, NPOS == RESULT_L);
        }

        // Test null pointer.  'numChars' can only be equal to 0, otherwise it
        // will be undefined behavior.

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;

            const size_type RESULT_F = XEmpty.find_first_of(NULL_PTR,
                                                            POSITION,
                                                            0);
            const size_type RESULT_L = XEmpty.find_last_of( NULL_PTR,
                                                            POSITION,
                                                            0);

            ASSERTV(POSITION, RESULT_F, NPOS == RESULT_F);
            ASSERTV(POSITION, RESULT_L, NPOS == RESULT_L);
        }

        // Test non-null pointers.  Note that zero-length objects will also be
        // tested within this section

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            for (size_type j = 0; j <= MAX_INDEX - i; ++j) {
                const size_type  OFFSET = i;
                const size_type  LENGTH = j;
                const TYPE      *START  = STRING + OFFSET;

                Obj         mX(START, LENGTH);
                const Obj&  X = mX;

                for (size_type k = 0; k <= MAX_INDEX; ++k) {
                    const size_type POSITION = k;

                    // Test search of an empty object.
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == X.find_first_of(XEmpty, POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == X.find_last_of( XEmpty, POSITION));
                    }

                    // Test search by an empty object.

                    // find_X_of(const CHAR_TYPE *, size_type, size_type)
                    for (size_type l = 0; l <= MAX_INDEX; ++l) {
                        const size_type NUM_CHARS = l;

                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                                NPOS == XEmpty.find_first_of(START,
                                                             POSITION,
                                                             NUM_CHARS));
                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS,
                                NPOS == XEmpty.find_last_of( START,
                                                             POSITION,
                                                             NUM_CHARS));
                    }

                    // 'find_X_of(basic_string_view, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_of(X, POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_of( X, POSITION));
                    }

                    // 'find_X_of(const CHAR_TYPE *, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_of(START, POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_of( START, POSITION));
                    }

                    // 'find_X_of(CHAR_TYPE, size_type)'
                    {
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_first_of(*START,
                                                              POSITION));
                        ASSERTV(OFFSET, LENGTH, POSITION,
                                NPOS == XEmpty.find_last_of( *START,
                                                              POSITION));
                    }
                }
            }
        }
    }

    // Testing non-empty objects.
    {
        const size_type MAX_LENGTH = 4;

        static const struct {
            int        d_lineNum;             // line number
            const TYPE d_string[MAX_LENGTH];  // string_view basis
            size_type  d_length;              // string_view length
        } DATA[] = {
            //LINE   STRING           CLOSURE    LENGTH
            //----   --------------   -------    ------
            { L_,   { 0               , 0    },  1     },
            { L_,   { 'a'             , 0    },  1     },
            { L_,   { 'b'             , 0    },  1     },
            { L_,   { 0  , 0          , 0    },  2     },
            { L_,   { 0  , 'a'        , 0    },  2     },
            { L_,   { 'a', 0          , 0    },  2     },
            { L_,   { 'a', 'a'        , 0    },  2     },
            { L_,   { 'a', 'b'        , 0    },  2     },
            { L_,   { 'b', 'a'        , 0    },  2     },
            { L_,   { 'b', 'b'        , 0    },  2     },
            { L_,   { 0  , 0  , 0     , 0    },  3     },
            { L_,   { 0  , 0  , 'a'   , 0    },  3     },
            { L_,   { 0  , 'a', 0     , 0    },  3     },
            { L_,   { 0  , 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 0  , 0     , 0    },  3     },
            { L_,   { 'a', 0  , 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 0     , 0    },  3     },
            { L_,   { 'a', 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 'b'   , 0    },  3     },
            { L_,   { 'a', 'b', 'a'   , 0    },  3     },
            { L_,   { 'a', 'b', 'b'   , 0    },  3     },
            { L_,   { 'b', 'a', 'a'   , 0    },  3     },
            { L_,   { 'b', 'a', 'b'   , 0    },  3     },
            { L_,   { 'b', 'b', 'a'   , 0    },  3     },
            { L_,   { 'b', 'b', 'b'   , 0    },  3     },
            { L_,   { 'd', 'e', 'f'   , 0    },  3     },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bsl::basic_string<TYPE> sampleString;
        sampleString.reserve(MAX_LENGTH);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            // Test null pointer.
            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type POSITION = j;
                const size_type RESULT_F = X.find_first_of(NULL_PTR,
                                                           POSITION,
                                                           0);
                const size_type RESULT_L = X.find_last_of( NULL_PTR,
                                                           POSITION,
                                                           0);
                const size_type EXP_F = NPOS;
                const size_type EXP_L = NPOS;

                ASSERTV(LINE, POSITION, EXP_F, RESULT_F, EXP_F == RESULT_F);
                ASSERTV(LINE, POSITION, EXP_L, RESULT_L, EXP_L == RESULT_L);
            }

            // Test non-null pointer.
            sampleString.clear();
            sampleString.append(STRING, LENGTH);

            for (size_type j = 0; j < NUM_DATA; ++j) {
                const int          LINE_P   =  DATA[j].d_lineNum;
                const TYPE * const STRING_P =  DATA[j].d_string;
                const size_type    LENGTH_P =  DATA[j].d_length;
                const TYPE         SYMBOL   = *STRING_P;

                Obj        mPATTERN(STRING_P, LENGTH_P);
                const Obj& PATTERN = mPATTERN;

                for (size_type k = 0; k < MAX_LENGTH; ++k) {
                    const size_type POSITION = k;

                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        const size_type EXP_F = findFirstOf(X,
                                                            STRING_P,
                                                            POSITION,
                                                            NUM_CHARS);

                        const size_type EXP_L = findLastOf( X,
                                                            STRING_P,
                                                            POSITION,
                                                            NUM_CHARS);

                        const size_type RESULT_F = X.find_first_of(STRING_P,
                                                                   POSITION,
                                                                   NUM_CHARS);

                        const size_type RESULT_L = X.find_last_of(STRING_P,
                                                                  POSITION,
                                                                  NUM_CHARS);

                        ASSERTV(LINE, LINE_P, POSITION,
                                NUM_CHARS, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);

                        ASSERTV(LINE, LINE_P, POSITION,
                                NUM_CHARS, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // Interfaces without 'numChars'.

                    // 'find_X_of(basic_string_view, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            PATTERN,
                                                            POSITION);
                        const size_type EXP_L = findLastOf( X,
                                                            PATTERN,
                                                            POSITION);

                        const size_type RESULT_F = X.find_first_of(PATTERN,
                                                                   POSITION);
                        const size_type RESULT_L = X.find_last_of( PATTERN,
                                                                   POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_of(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            STRING_P,
                                                            POSITION);
                        const size_type EXP_L = findLastOf( X,
                                                            STRING_P,
                                                            POSITION);

                        const size_type RESULT_F = X.find_first_of(STRING_P,
                                                                   POSITION);
                        const size_type RESULT_L = X.find_last_of( STRING_P,
                                                                   POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_of(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            SYMBOL,
                                                            POSITION);
                        const size_type EXP_L = findLastOf( X,
                                                            SYMBOL,
                                                            POSITION);

                        const size_type RESULT_F = X.find_first_of(SYMBOL,
                                                                   POSITION);
                        const size_type RESULT_L = X.find_last_of(SYMBOL,
                                                                  POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }
                }

                // Also we want to check the scenarios, when the 'POSITION' is
                // equal to the 'npos' value (mostly for the further testing of
                // the default values).
                {
                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        const size_type EXP_F = findFirstOf(X,
                                                            STRING_P,
                                                            NPOS,
                                                            NUM_CHARS);

                        const size_type EXP_L = findLastOf( X,
                                                            STRING_P,
                                                            NPOS,
                                                            NUM_CHARS);

                        const size_type RESULT_F = X.find_first_of(STRING_P,
                                                                   NPOS,
                                                                   NUM_CHARS);

                        const size_type RESULT_L = X.find_last_of(STRING_P,
                                                                  NPOS,
                                                                  NUM_CHARS);

                        ASSERTV(LINE, LINE_P, NUM_CHARS, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);

                        ASSERTV(LINE, LINE_P, NUM_CHARS, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // Interfaces without 'numChars'.

                    // 'find_X_of(basic_string_view, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            PATTERN,
                                                            NPOS);
                        const size_type EXP_L = findLastOf( X,
                                                            PATTERN,
                                                            NPOS);

                        const size_type RESULT_F = X.find_first_of(PATTERN,
                                                                   NPOS);
                        const size_type RESULT_L = X.find_last_of( PATTERN,
                                                                   NPOS);

                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_of(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            STRING_P,
                                                            NPOS);
                        const size_type EXP_L = findLastOf( X,
                                                            STRING_P,
                                                            NPOS);

                        const size_type RESULT_F = X.find_first_of(STRING_P,
                                                                   NPOS);
                        const size_type RESULT_L = X.find_last_of( STRING_P,
                                                                   NPOS);

                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }

                    // 'find_X_of(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP_F = findFirstOf(X,
                                                            SYMBOL,
                                                            NPOS);
                        const size_type EXP_L = findLastOf( X,
                                                            SYMBOL,
                                                            NPOS);

                        const size_type RESULT_F = X.find_first_of(SYMBOL,
                                                                   NPOS);
                        const size_type RESULT_L = X.find_last_of(SYMBOL,
                                                                  NPOS);

                        ASSERTV(LINE, LINE_P, EXP_F, RESULT_F,
                                EXP_F == RESULT_F);
                        ASSERTV(LINE, LINE_P, EXP_L, RESULT_L,
                                EXP_L == RESULT_L);
                    }
                }

                // Now we can check the correctness of the default values.

                // 'find_first_of(basic_string_view, size_type)'
                {
                    const size_type EXP_F    = X.find_first_of(PATTERN, 0   );
                    const size_type RESULT_F = X.find_first_of(PATTERN      );

                    const size_type EXP_L    = X.find_last_of( PATTERN, NPOS);
                    const size_type RESULT_L = X.find_last_of( PATTERN      );

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }

                // 'find_first_of(const CHAR_TYPE *, size_type)'
                {
                    const size_type EXP_F    = X.find_first_of(STRING_P, 0   );
                    const size_type RESULT_F = X.find_first_of(STRING_P      );

                    const size_type EXP_L    = X.find_last_of( STRING_P, NPOS);
                    const size_type RESULT_L = X.find_last_of( STRING_P      );

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }

                // 'find_first_of(CHAR_TYPE, size_type)'
                {
                    const size_type EXP_F    = X.find_first_of(SYMBOL, 0   );
                    const size_type RESULT_F = X.find_first_of(SYMBOL      );

                    const size_type EXP_L    = X.find_last_of( SYMBOL, NPOS);
                    const size_type RESULT_L = X.find_last_of( SYMBOL      );

                    ASSERTV(LINE, LINE_P, EXP_F, RESULT_F, EXP_F == RESULT_F);
                    ASSERTV(LINE, LINE_P, EXP_L, RESULT_L, EXP_L == RESULT_L);
                }
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const size_type STRING_LENGTH = s_testStringLength;
        Obj             mX(STRING, STRING_LENGTH);
        const Obj&      X = mX;

        ASSERT_SAFE_PASS(X.find_first_of(STRING  , 0, 1            ));
        ASSERT_SAFE_PASS(X.find_first_of(STRING  , 0, 0            ));
        ASSERT_SAFE_PASS(X.find_first_of(NULL_PTR, 0, 0            ));
        ASSERT_SAFE_FAIL(X.find_first_of(NULL_PTR, 0, 1            ));

        ASSERT_SAFE_PASS(X.find_first_of(STRING  , 0               ));
        ASSERT_SAFE_FAIL(X.find_first_of(NULL_PTR, 0               ));

        ASSERT_SAFE_PASS(X.find_first_of(STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.find_first_of(STRING  , 0, Obj::npos    ));

        ASSERT_SAFE_PASS(X.find_last_of( STRING   , 0, 1           ));
        ASSERT_SAFE_PASS(X.find_last_of( STRING   , 0, 0           ));
        ASSERT_SAFE_PASS(X.find_last_of( NULL_PTR , 0, 0           ));
        ASSERT_SAFE_FAIL(X.find_last_of( NULL_PTR , 0, 1           ));

        ASSERT_SAFE_PASS(X.find_last_of( STRING   , 0              ));
        ASSERT_SAFE_FAIL(X.find_last_of( NULL_PTR , 0              ));

        ASSERT_SAFE_PASS(X.find_last_of( STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.find_last_of( STRING  , 0, Obj::npos    ));
    }
#endif
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING FIND AND RFIND
    //   To avoid filling/reading of enormously large tables we are going to
    //   use 'bsl::basic_string::find()' and 'bsl::basic_string::rfind()'
    //   methods as a test functions, as they do completely the same work.
    //
    // Concerns:
    //: 1 An empty object can be used as a parameter for search.
    //:
    //: 2 Search operations can be called for an empty object.
    //:
    //: 3 The null pointer can be used as a parameter for search.
    //:
    //: 4 The 'find' returns 'npos' if the position does not belong to the
    //:   object.
    //:
    //: 5 The 'find' returns only the valid range matches (e.g. the returned
    //:   value that is not equal to the 'npos', points to the first of
    //:   'numChars' symbols (or to the whole string/string_view/character),
    //:   that are undoubtedly belong to the object.
    //:
    //: 6 The 'find' accepts the request for a zero-length string/string_view
    //:   or zero characters (if the requested 'position' lies in the valid
    //:   range) and returns this 'position'.
    //:
    //: 7 The 'rfind' accepts the request for a zero-length string/string_view
    //:   or zero characters (even if the requested 'position' does not lie in
    //:   the valid range) and returns length of the object or the 'position'
    //:   whichever is less.
    //:
    //: 8 The 'npos' requested position is accepted and correctly handled by
    //:   the 'find' and 'rfind'.
    //:
    //: 9 The null symbol ('/0') is correctly handled whether it belongs to the
    //:   object or to the requested value for search.
    //:
    //:10 The default parameter values are exactly as expected.
    //:
    //:11 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create an object with null parameters.  For this object:
    //:
    //:   1 Iterate positions in the range [0, M], where the 'M' is some
    //:     non-negative integer constant.  For each position:
    //:
    //:     1 Look for an empty object and verify the result.
    //:
    //:     2 Look for a null pointer and verify the result.
    //:
    //:   2 Using a loop-based approach, construct a set of the pairs 'P1'
    //:     '(ADDR, LENGTH)', where
    //:     ADDR   is an address in the range '[S, S + N]', where 'S' is some
    //:            string constant and 'N' is some non-negative integer
    //:            constant
    //:     LENGTH is an integer constant in the range '[0, N - position]',
    //:            where the 'position' is the size of the indent of the ADDR
    //:            from the beginning of 'S'
    //:     For each element 'p1' in the set use 'ADDR1' and 'LENGTH1' as
    //:     parameters to create a non-empty object and then call each 'find'
    //:     and 'rfind' overload for the empty object from P-1 iterating
    //:     through the range of positions and the range of the lengths. Verify
    //:     the results.  (C-2)
    //:
    //: 2 Construct another set of the pairs 'P2' '(ADDR, LENGTH)', where
    //:     ADDR   is an address referring to some string constant
    //:     LENGTH is a non-negative integer constant defining the length of
    //:            the string referred by the 'ADDR'
    //:   Each string referred by 'ADDR' from 'P2' can contain null symbols.
    //:   Various strings can contain the same symbols (or even sequences of
    //:   symbols).  Some strings contain the set of unique symbols.  For each
    //:   element 'p1' in 'P2', containing 'ADDR1' and 'LENGTH1' values:
    //:
    //:   1 Create an object 'o1' referring to the 'ADDR1' and having 'LENGTH1'
    //:     symbols.  Also create a 'bsl::basic_string' object, 's1', passing
    //:     the 'ADDR1' and the 'LENGTH2' as a parameters.  So informally we
    //:     can say, that 'o1' and 's1' have the same values.
    //:
    //:   2 Call the appropriate overloads of the 'find' and 'rfind' methods
    //:     for the 'o1' looking for the null pointer and empty object.  Verify
    //:     the results. (C-1,3)
    //:
    //:   3 Iterate through the 'P2' set and for each element 'p2', containing
    //:     'ADDR2' and 'LENGTH2' values, create an object 'o2' referring to
    //:     the 'ADDR2' and having 'LENGTH2' symbols.
    //:
    //:   4 Iterate through the range of positions and the range of the lengths
    //:     and call the 'find' and 'rfind' methods for 'o1'.  Here we need to
    //:     call only overloads, that take three parameters (string, position
    //:     and the number of characters) and pass 'ADDR2' as the first
    //:     parameter.  These overloads are the base ones and we are going to
    //:     use them to verify the return values of other overloads.  For now
    //:     verify the results using the corresponding methods of 's1' and
    //:     passing the same parameters.
    //:
    //:   5 Iterate through the range of positions and call remaining
    //:     overloads of the 'find' and 'rfind' methods using the 'ADDR2' as a
    //:     parameter and verifying the returned values by the results of calls
    //:     of overloads from the P-2-4.  (C-4..7, 9)
    //:
    //:   6 Iterate through the range of positions and the range of the lengths
    //:     and call the 'find' and 'rfind' methods for 'o1' passing 'npos'
    //:     value as the position parameter and verify the results.  (C-8)
    //:
    //:   7 Call the 'find' and 'rfind' methods for 'o1' using default
    //:     parameters and verify the results by calling the same overload
    //:     with the explicitly passed parameters having the appropriate
    //:     values.  (C-10)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-11)
    //
    // Testing:
    //   size_type find(basic_string_view sv, size_type pos) const;
    //   size_type find(const CHAR_TYPE *s, size_type pos, size_type n) const;
    //   size_type find(const CHAR_TYPE *s, size_type pos) const;
    //   size_type find(CHAR_TYPE c, size_type pos) const;
    //   size_type rfind(basic_string_view sv, size_type pos) const;
    //   size_type rfind(const CHAR_TYPE *s, size_type pos, size_type n) const;
    //   size_type rfind(const CHAR_TYPE *s, size_type pos) const;
    //   size_type rfind(CHAR_TYPE c, size_type pos) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING   = s_testString;
    const TYPE      *NULL_PTR = 0;
    const size_type  NPOS     = Obj::npos;

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty object.
    {
        const size_type MAX_INDEX = 2;

        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        // Test search of an empty object in the empty object.
        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;
            const size_type EXP      = POSITION ? NPOS : POSITION;
            const size_type EXP_R    = 0;

            const size_type RESULT   = XEmpty.find( XEmpty, POSITION);
            const size_type RESULT_R = XEmpty.rfind(XEmpty, POSITION);

            ASSERTV(POSITION, EXP,   RESULT,   EXP   == RESULT);
            ASSERTV(POSITION, EXP_R, RESULT_R, EXP_R == RESULT_R);
        }

        // Test null pointer.  'numChars' can only be equal to 0, otherwise it
        // will be undefined behavior.

        for (size_type i = 0; i < MAX_INDEX; ++i) {
            const size_type POSITION = i;
            const size_type EXP      = POSITION ? NPOS : POSITION;
            const size_type EXP_R    = 0;

            const size_type RESULT   = XEmpty.find( NULL_PTR, POSITION, 0);
            const size_type RESULT_R = XEmpty.rfind(NULL_PTR, POSITION, 0);

            ASSERTV(POSITION, EXP,   RESULT,   EXP   == RESULT);
            ASSERTV(POSITION, EXP_R, RESULT_R, EXP_R == RESULT_R);
        }

        // Test non-null pointer.
        for (size_type i = 0; i < MAX_INDEX; ++i) {
            for (size_type j = 0; j <= MAX_INDEX - i; ++j) {
                const size_type  OFFSET = i;
                const size_type  LENGTH = j;
                const TYPE      *START  = STRING + OFFSET;

                Obj         mX(START, LENGTH);
                const Obj&  X = mX;

                for (size_type k = 0; k <= MAX_INDEX; ++k) {
                    const size_type POSITION = k;

                    // Test search of an empty object.
                    {
                        const size_type EXP   =
                                         POSITION > LENGTH ? NPOS   : POSITION;
                        const size_type EXP_R =
                                         POSITION > LENGTH ? LENGTH : POSITION;

                        const size_type RESULT   = X.find( XEmpty, POSITION);
                        const size_type RESULT_R = X.rfind(XEmpty, POSITION);

                        ASSERTV(OFFSET, LENGTH, POSITION, EXP,   RESULT,
                                EXP == RESULT);
                        ASSERTV(OFFSET, LENGTH, POSITION, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // Test search by an empty object.

                    // [r]find(const CHAR_TYPE *, size_type, size_type)
                    for (size_type l = 0; l <= MAX_INDEX; ++l) {
                        const size_type NUM_CHARS = l;
                        const size_type EXP =
                                  (0 == POSITION && 0 == NUM_CHARS) ? 0 : NPOS;
                        const size_type EXP_R =     0 == NUM_CHARS ?  0 : NPOS;

                        const size_type RESULT =   XEmpty.find( START,
                                                                POSITION,
                                                                NUM_CHARS);
                        const size_type RESULT_R = XEmpty.rfind(START,
                                                                POSITION,
                                                                NUM_CHARS);

                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT,
                                EXP == RESULT);
                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // '[r]find(basic_string_view, size_type)'
                    {
                        const size_type NUM_CHARS = X.length();
                        const size_type EXP =
                                  (0 == POSITION && 0 == NUM_CHARS) ? 0 : NPOS;
                        const size_type EXP_R =     0 == NUM_CHARS  ? 0 : NPOS;

                        const size_type RESULT   = XEmpty.find( X, POSITION);
                        const size_type RESULT_R = XEmpty.rfind(X, POSITION);

                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT,
                                EXP == RESULT);
                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT_R,
                                EXP_R == RESULT_R);

                    }

                    // '[r]find(const CHAR_TYPE *, size_type)'
                    {
                        const size_type NUM_CHARS = TRAITS::length(START);
                        const size_type EXP =
                                  (0 == POSITION && 0 == NUM_CHARS) ? 0 : NPOS;
                        const size_type EXP_R =     0 == NUM_CHARS  ? 0 : NPOS;

                        const size_type RESULT   = XEmpty.find( START,
                                                                POSITION);
                        const size_type RESULT_R = XEmpty.rfind(START,
                                                                POSITION);

                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT,
                                EXP == RESULT);
                        ASSERTV(OFFSET, LENGTH, POSITION, NUM_CHARS, RESULT_R,
                                EXP_R == RESULT_R);

                    }

                    // '[r]find(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP   = NPOS;
                        const size_type EXP_R = NPOS;

                        const size_type RESULT   = XEmpty.find( *START,
                                                                POSITION);
                        const size_type RESULT_R = XEmpty.rfind(*START,
                                                                POSITION);

                        ASSERTV(OFFSET, LENGTH, POSITION, RESULT,
                                EXP == RESULT);
                        ASSERTV(OFFSET, LENGTH, POSITION, RESULT_R,
                                EXP_R == RESULT_R);

                    }
                }
            }
        }
    }

    {
        const size_type MAX_LENGTH = 4;

        static const struct {
            int        d_lineNum;             // line number
            const TYPE d_string[MAX_LENGTH];  // string_view basis
            size_type  d_length;              // string_view length
        } DATA[] = {
            //LINE   STRING           CLOSURE    LENGTH
            //----   --------------   -------    ------
            { L_,   { 0               , 0    },  1     },
            { L_,   { 'a'             , 0    },  1     },
            { L_,   { 'b'             , 0    },  1     },
            { L_,   { 0  , 0          , 0    },  2     },
            { L_,   { 0  , 'a'        , 0    },  2     },
            { L_,   { 'a', 0          , 0    },  2     },
            { L_,   { 'a', 'a'        , 0    },  2     },
            { L_,   { 'a', 'b'        , 0    },  2     },
            { L_,   { 'b', 'a'        , 0    },  2     },
            { L_,   { 'b', 'b'        , 0    },  2     },
            { L_,   { 0  , 0  , 0     , 0    },  3     },
            { L_,   { 0  , 0  , 'a'   , 0    },  3     },
            { L_,   { 0  , 'a', 0     , 0    },  3     },
            { L_,   { 0  , 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 0  , 0     , 0    },  3     },
            { L_,   { 'a', 0  , 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 0     , 0    },  3     },
            { L_,   { 'a', 'a', 'a'   , 0    },  3     },
            { L_,   { 'a', 'a', 'b'   , 0    },  3     },
            { L_,   { 'a', 'b', 'a'   , 0    },  3     },
            { L_,   { 'a', 'b', 'b'   , 0    },  3     },
            { L_,   { 'b', 'a', 'a'   , 0    },  3     },
            { L_,   { 'b', 'a', 'b'   , 0    },  3     },
            { L_,   { 'b', 'b', 'a'   , 0    },  3     },
            { L_,   { 'b', 'b', 'b'   , 0    },  3     },
            { L_,   { 'd', 'e', 'f'   , 0    },  3     },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bsl::basic_string<TYPE> sampleString;
        sampleString.reserve(MAX_LENGTH);

        for (size_type i = 0; i < NUM_DATA; ++i) {
            const int          LINE   = DATA[i].d_lineNum;
            const TYPE * const STR    = DATA[i].d_string;
            const size_type    LENGTH = DATA[i].d_length;

            Obj        mX(STR, LENGTH);
            const Obj& X = mX;

            // Test null pointer.
            for (size_type j = 0; j < LENGTH; ++j) {
                const size_type POSITION = j;

                const size_type EXP      = POSITION;
                const size_type EXP_R    = POSITION;

                const size_type RESULT   = X.find( NULL_PTR, POSITION, 0);
                const size_type RESULT_R = X.rfind(NULL_PTR, POSITION, 0);

                ASSERTV(LINE, POSITION, EXP,   RESULT,   EXP   == RESULT  );
                ASSERTV(LINE, POSITION, EXP_R, RESULT_R, EXP_R == RESULT_R);
            }

            // Test non-null pointer.
            sampleString.clear();
            sampleString.append(STR, LENGTH);

            for (size_type j = 0; j < NUM_DATA; ++j) {
                const int          LINE_P   =  DATA[j].d_lineNum;
                const TYPE * const STRING_P =  DATA[j].d_string;
                const size_type    LENGTH_P =  DATA[j].d_length;
                const TYPE         SYMBOL   = *STRING_P;

                Obj        mPattern(STRING_P, LENGTH_P);
                const Obj& PATTERN = mPattern;

                for (size_type k = 0; k < MAX_LENGTH; ++k) {
                    const size_type POSITION = k;

                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        size_type       exp  = sampleString.find(STRING_P,
                                                                 POSITION,
                                                                 NUM_CHARS);
                        size_type       expR = sampleString.rfind(STRING_P,
                                                                  POSITION,
                                                                  NUM_CHARS);
                        const size_type EXP =
                                           bsl::basic_string<TYPE>::npos == exp
                                           ? NPOS : exp;

                        const size_type EXP_R =
                                          bsl::basic_string<TYPE>::npos == expR
                                          ? NPOS : expR;

                        const size_type RESULT = X.find(STRING_P,
                                                        POSITION,
                                                        NUM_CHARS);

                        const size_type RESULT_R = X.rfind(STRING_P,
                                                           POSITION,
                                                           NUM_CHARS);

                        ASSERTV(LINE, LINE_P, POSITION, NUM_CHARS, EXP, RESULT,
                                EXP == RESULT);

                        ASSERTV(LINE, LINE_P, POSITION,
                                NUM_CHARS, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);

                        // To be sure, we want to explicitly check some
                        // concerns, even if it will be some excessive testing.

                        // 'find'
                        if (POSITION > LENGTH) {                       // (C-4)
                            ASSERTV(LINE, LINE_P, POSITION, NUM_CHARS, RESULT,
                                    NPOS == RESULT);
                        }
                        else if (NUM_CHARS > LENGTH - POSITION) {      // (C-5)
                            ASSERTV(LINE, LINE_P, POSITION, NUM_CHARS, RESULT,
                                    NPOS == RESULT);
                        }
                        else if (0 == NUM_CHARS) {                     // (C-6)
                            ASSERTV(LINE, LINE_P, POSITION, NUM_CHARS, RESULT,
                                    POSITION == RESULT);
                        }

                        // 'rfind'
                        if (0 == NUM_CHARS) {                          // (C-7)
                            const size_type MIN = POSITION > LENGTH
                                                ? LENGTH
                                                : POSITION;

                            ASSERTV(LINE, LINE_P, POSITION,
                                    NUM_CHARS, RESULT_R,
                                    MIN == RESULT_R);
                        }
                    }

                    // Now we can use fully tested
                    // 'find(const TYPE *, size_type, size_type)' and
                    // 'rfind(const TYPE *, size_type, size_type)'to verify
                    // values returned by other overloads.

                    // '[r]find(basic_string_view, size_type)'
                    {
                        const size_type EXP   = X.find( STRING_P,
                                                        POSITION,
                                                        LENGTH_P);
                        const size_type EXP_R = X.rfind(STRING_P,
                                                        POSITION,
                                                        LENGTH_P);

                        const size_type RESULT   = X.find( PATTERN,
                                                           POSITION);
                        const size_type RESULT_R = X.rfind(PATTERN,
                                                           POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP, RESULT,
                                EXP == RESULT);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // '[r]find(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP   = X.find(
                                                     STRING_P,
                                                     POSITION,
                                                     TRAITS::length(STRING_P));
                        const size_type EXP_R = X.rfind(
                                                     STRING_P,
                                                     POSITION,
                                                     TRAITS::length(STRING_P));

                        const size_type RESULT   = X.find( STRING_P,
                                                           POSITION);
                        const size_type RESULT_R = X.rfind(STRING_P,
                                                           POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP, RESULT,
                                EXP == RESULT);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // '[r]find(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP   = X.find( STRING_P,
                                                        POSITION,
                                                        1);
                        const size_type EXP_R = X.rfind(STRING_P,
                                                        POSITION,
                                                        1);

                        const size_type RESULT   = X.find(SYMBOL,
                                                          POSITION);
                        const size_type RESULT_R = X.rfind(SYMBOL,
                                                           POSITION);

                        ASSERTV(LINE, LINE_P, POSITION, EXP, RESULT,
                                EXP == RESULT);
                        ASSERTV(LINE, LINE_P, POSITION, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }
                }

                // Also we want to check the scenarios, when the 'POSITION' is
                // equal to the 'npos' value (mostly for the further testing of
                // the default values).
                {
                    for (size_type l = 0; l <= MAX_LENGTH; ++l) {
                        const size_type NUM_CHARS = l;

                        size_type       exp  = sampleString.find(STRING_P,
                                                                 NPOS,
                                                                 NUM_CHARS);
                        size_type       expR = sampleString.rfind(STRING_P,
                                                                  NPOS,
                                                                  NUM_CHARS);
                        const size_type EXP =
                                           bsl::basic_string<TYPE>::npos == exp
                                           ? NPOS : exp;

                        const size_type EXP_R =
                                          bsl::basic_string<TYPE>::npos == expR
                                          ? NPOS : expR;

                        const size_type RESULT = X.find(STRING_P,
                                                        NPOS,
                                                        NUM_CHARS);

                        const size_type RESULT_R = X.rfind(STRING_P,
                                                           NPOS,
                                                           NUM_CHARS);

                        ASSERTV(LINE, LINE_P, NPOS, NUM_CHARS, EXP, RESULT,
                                EXP == RESULT);

                        ASSERTV(LINE, LINE_P, NPOS,
                                NUM_CHARS, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // Now we can use fully tested
                    // 'find(const TYPE *, npos, size_type)' and
                    // 'rfind(const TYPE *, npos, size_type)'to verify values
                    // returned by other overloads.

                    // '[r]find(basic_string_view, size_type)'
                    {
                        const size_type EXP   = X.find( STRING_P,
                                                        NPOS,
                                                        LENGTH_P);
                        const size_type EXP_R = X.rfind(STRING_P,
                                                        NPOS,
                                                        LENGTH_P);

                        const size_type RESULT   = X.find( PATTERN,
                                                           NPOS);
                        const size_type RESULT_R = X.rfind(PATTERN,
                                                           NPOS);

                        ASSERTV(LINE, LINE_P, EXP,   RESULT,
                                EXP   == RESULT);
                        ASSERTV(LINE, LINE_P, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // '[r]find(const CHAR_TYPE *, size_type)'
                    {
                        const size_type EXP   = X.find(
                                                     STRING_P,
                                                     NPOS,
                                                     TRAITS::length(STRING_P));
                        const size_type EXP_R = X.rfind(
                                                     STRING_P,
                                                     NPOS,
                                                     TRAITS::length(STRING_P));

                        const size_type RESULT   = X.find( STRING_P,
                                                           NPOS);
                        const size_type RESULT_R = X.rfind(STRING_P,
                                                           NPOS);

                        ASSERTV(LINE, LINE_P, EXP,   RESULT,
                                EXP   == RESULT);
                        ASSERTV(LINE, LINE_P, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }

                    // '[r]find(CHAR_TYPE, size_type)'
                    {
                        const size_type EXP   = X.find( STRING_P,
                                                        NPOS,
                                                        1);
                        const size_type EXP_R = X.rfind(STRING_P,
                                                        NPOS,
                                                        1);

                        const size_type RESULT   = X.find(SYMBOL,
                                                          NPOS);
                        const size_type RESULT_R = X.rfind(SYMBOL,
                                                           NPOS);

                        ASSERTV(LINE, LINE_P, EXP,   RESULT,
                                EXP   == RESULT);
                        ASSERTV(LINE, LINE_P, EXP_R, RESULT_R,
                                EXP_R == RESULT_R);
                    }
                }

                // Now we can check the correctness of the default values.

                // 'find(basic_string_view, size_type)'
                {
                    const size_type EXP      = X.find( PATTERN, 0   );
                    const size_type RESULT   = X.find( PATTERN      );

                    const size_type EXP_R    = X.rfind(PATTERN, NPOS);
                    const size_type RESULT_R = X.rfind(PATTERN      );

                    ASSERTV(LINE, LINE_P, EXP,   RESULT,   EXP   == RESULT  );
                    ASSERTV(LINE, LINE_P, EXP_R, RESULT_R, EXP_R == RESULT_R);
                }

                // 'find(const CHAR_TYPE *, size_type)'
                {
                    const size_type EXP      = X.find( STRING_P, 0   );
                    const size_type RESULT   = X.find( STRING_P      );

                    const size_type EXP_R    = X.rfind(STRING_P, NPOS);
                    const size_type RESULT_R = X.rfind(STRING_P      );

                    ASSERTV(LINE, LINE_P, EXP,   RESULT,   EXP   == RESULT  );
                    ASSERTV(LINE, LINE_P, EXP_R, RESULT_R, EXP_R == RESULT_R);
                }

                // 'find(CHAR_TYPE, size_type)'
                {
                    const size_type EXP      = X.find( SYMBOL, 0   );
                    const size_type RESULT   = X.find( SYMBOL      );

                    const size_type EXP_R    = X.rfind(SYMBOL, NPOS);
                    const size_type RESULT_R = X.rfind(SYMBOL      );

                    ASSERTV(LINE, LINE_P, EXP,   RESULT,   EXP   == RESULT  );
                    ASSERTV(LINE, LINE_P, EXP_R, RESULT_R, EXP_R == RESULT_R);
                }
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const size_type STRING_LENGTH = s_testStringLength;
        Obj             mX(STRING, STRING_LENGTH);
        const Obj&      X = mX;

        ASSERT_SAFE_PASS(X.find( STRING  , 0, 1            ));
        ASSERT_SAFE_PASS(X.find( STRING  , 0, 0            ));
        ASSERT_SAFE_PASS(X.find( NULL_PTR, 0, 0            ));
        ASSERT_SAFE_FAIL(X.find( NULL_PTR, 0, 1            ));

        ASSERT_SAFE_PASS(X.find( STRING  , 0               ));
        ASSERT_SAFE_FAIL(X.find( NULL_PTR, 0               ));

        ASSERT_SAFE_PASS(X.find( STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.find( STRING  , 0, Obj::npos    ));

        ASSERT_SAFE_PASS(X.rfind(STRING  , 0, 1            ));
        ASSERT_SAFE_PASS(X.rfind(STRING  , 0, 0            ));
        ASSERT_SAFE_PASS(X.rfind(NULL_PTR, 0, 0            ));
        ASSERT_SAFE_FAIL(X.rfind(NULL_PTR, 0, 1            ));

        ASSERT_SAFE_PASS(X.rfind(STRING  , 0               ));
        ASSERT_SAFE_FAIL(X.rfind(NULL_PTR, 0               ));

        ASSERT_SAFE_PASS(X.rfind(STRING  , 0, Obj::npos - 1));
        ASSERT_SAFE_PASS(X.rfind(STRING  , 0, Obj::npos    ));
    }
#endif
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING COPY
    //
    // Concerns:
    //: 1 The 'copy' returns the number of copied characters.
    //:
    //: 2 The 'copy' sticks to the borders of the underlying string (e.g. it
    //:   copies only the available number of characters, even if user requests
    //:   more).
    //:
    //: 3 The 'copy' can be called for an empty object.
    //:
    //: 4 Default parameters assume expected values.
    //:
    //: 5 The position of the symbol next to the last one
    //:   ('position == length()') is valid and correctly handled.
    //:
    //: 6 The 'copy' throws 'std::out_of_range' exception when passed an
    //:   out-of-bound position (e.g. 'position > length()').
    //:
    //: 7 No additional memory is allocated.
    //:
    //: 8 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create some destination buffer.
    //:
    //: 3 Create an object with null parameters, copy zero symbols and non-zero
    //:   symbols from it and verify that the destination buffer and the object
    //:   itself remain unchanged.  (C-3)
    //:
    //: 4 Using a loop-based approach, construct a set of objects, 'O', on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   each address in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having each length in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'o' in the
    //:   set 'O'
    //:
    //:   1 Construct a set 'C' of numbers of characters to copy in the range
    //:     '[0, N]' (so they are less, equal or greater than length of 'o'.
    //:     For each element 'c' of the 'C':
    //:
    //:     1 Construct a set 'P' of numbers of positions to start copying in
    //:       the range '[0, o.length()]' (so 'copy' should not throw an
    //:       exception).  For each element 'p' of the 'P'
    //:
    //:       1 Fill destination buffer with symbols not belonging to 'S'.
    //:
    //:       2 Call 'copy' for the 'o', passing destination buffer address,
    //:         'c' and 'p' as a parameters.  Verify the returned value and the
    //:         contents of the destination buffer.  (C-1..2)
    //:
    //:     2 Fill destination buffer with symbols not belonging to 'S' again.
    //:
    //:     3 Call 'copy' for the 'o', passing destination buffer address and
    //:       'c' as a parameters (default value is used for 'p').  Verify the
    //:       returned value and the contents of the destination buffer.  (C-4)
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-8)
    //:
    //: 5 Using 'try-catch' block, verify that 'std::out_of_range' exception is
    //:   thrown by the 'copy' when 'position > length()'.  (C-6)
    //:
    //: 6 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-7)
    //
    // Testing:
    //   size_type copy(CHAR_TYPE *str, size_type numCh, size_type pos) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;
    TYPE             TARGET[STRING_LENGTH];

    if (verbose)
        printf("\tTesting basic behavior.\n");

    // Testing empty objects copying.
    {
        const TYPE *NULL_PTR        = 0;
        TYPE        BUFFER_EMPTY[2] = {'a', 'a'};

        Obj        mXEmpty(NULL_PTR, 0);
        const Obj& XEmpty = mXEmpty;

        mXEmpty.copy(BUFFER_EMPTY, 0, 0);

        ASSERTV(0   == XEmpty.data()  );
        ASSERTV(0   == XEmpty.length());
        ASSERT ('a' == BUFFER_EMPTY[0]);
        ASSERT ('a' == BUFFER_EMPTY[1]);

        mXEmpty.copy(BUFFER_EMPTY, 1, 0);

        ASSERTV(0   == XEmpty.data()  );
        ASSERTV(0   == XEmpty.length());
        ASSERT ('a' == BUFFER_EMPTY[0]);
        ASSERT ('a' == BUFFER_EMPTY[1]);
    }

    // Testing non-empty objects copying.
    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;

            Obj         mX(START, LENGTH);
            const Obj&  X = mX;

            for (size_type k = 0; k <= STRING_LENGTH; ++k) {
                const size_type NUM_CHARS = k;

                // Testing call with explicit parameters.

                for (size_type l = 0; l <= LENGTH; ++l) {
                    const size_type POSITION  = l;
                    const size_type EXPECTED  = NUM_CHARS < LENGTH - POSITION
                                              ? NUM_CHARS : LENGTH - POSITION;

                    std::fill(TARGET, TARGET + STRING_LENGTH, 'a');

                    size_type numFilled = bsl::count(TARGET,
                                                     TARGET + STRING_LENGTH,
                                                     'a');
                    ASSERTV(i, j, k, l, numFilled, STRING_LENGTH == numFilled);

                    size_type numCopied = X.copy(TARGET, NUM_CHARS, POSITION);

                    numFilled = bsl::count(TARGET,
                                           TARGET + STRING_LENGTH,
                                           'a');

                    ASSERTV(i, j, k, l, numCopied, EXPECTED == numCopied);
                    ASSERTV(i, j, k, l, START, TARGET,
                            0 == memcmp(START + POSITION,
                                        TARGET,
                                        EXPECTED * sizeof(TYPE)));

                    ASSERTV(i, j, k, l, numFilled,
                            STRING_LENGTH - EXPECTED == numFilled);

                }

                // Testing call with default parameter.

                {
                    const size_type EXPECTED  = NUM_CHARS < LENGTH
                                              ? NUM_CHARS : LENGTH;

                    std::fill(TARGET, TARGET + STRING_LENGTH, 'a');

                    size_type numFilled = bsl::count(TARGET,
                                                     TARGET + STRING_LENGTH,
                                                     'a');
                    ASSERTV(i, j, k, numFilled, STRING_LENGTH == numFilled);

                    size_type numCopied = X.copy(TARGET, NUM_CHARS);

                    numFilled = bsl::count(TARGET,
                                           TARGET + STRING_LENGTH,
                                           'a');

                    ASSERTV(i, j, k, numCopied, EXPECTED == numCopied);
                    ASSERTV(i, j, k, START, TARGET,
                            0 == memcmp(START,
                                        TARGET,
                                        EXPECTED * sizeof(TYPE)));

                    ASSERTV(i, j, k, numFilled,
                            STRING_LENGTH - EXPECTED == numFilled);
                }
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj        mXN(STRING, STRING_LENGTH);
        const Obj& XN = mXN;

        // Testing null destination string.

        ASSERT_SAFE_PASS(XN.copy(TARGET, 1));
        ASSERT_SAFE_FAIL(XN.copy(0,      1));

        // Testing ranges overlapping.

        const size_type  BUFFER_SIZE    = 6;
        TYPE             BUFFER[BUFFER_SIZE];
        const size_type  START_POSITION = 2;
        const size_type  VIEW_SIZE      = 2;
        TYPE            *BEGIN          = BUFFER + START_POSITION;
        TYPE            *END            = BEGIN + VIEW_SIZE;

        // |<----- BUFFER ----->|
        // []  []  []  []  []  []
        //         |view|

        Obj        mXR(BEGIN, VIEW_SIZE);
        const Obj& XR = mXR;

        ASSERT_SAFE_PASS(XR.copy(BEGIN - 2, 0));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 2, 1));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 2, 2));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 2, 3));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 1, 0));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 1, 1));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 1, 2));
        ASSERT_SAFE_PASS(XR.copy(BEGIN - 1, 3));
        ASSERT_SAFE_PASS(XR.copy(BEGIN    , 0));
        ASSERT_SAFE_FAIL(XR.copy(BEGIN    , 1));
        ASSERT_SAFE_FAIL(XR.copy(BEGIN    , 2));
        ASSERT_SAFE_FAIL(XR.copy(BEGIN    , 3));
        ASSERT_SAFE_PASS(XR.copy(BEGIN + 1, 0));
        ASSERT_SAFE_PASS(XR.copy(BEGIN + 1, 1));
        ASSERT_SAFE_FAIL(XR.copy(BEGIN + 1, 2));
        ASSERT_SAFE_FAIL(XR.copy(BEGIN + 1, 3));
        ASSERT_SAFE_PASS(XR.copy(END      , 0));
        ASSERT_SAFE_PASS(XR.copy(END      , 1));
        ASSERT_SAFE_PASS(XR.copy(END      , 2));
        ASSERT_SAFE_PASS(XR.copy(END      , 3));
        ASSERT_SAFE_PASS(XR.copy(END   + 1, 0));
        ASSERT_SAFE_PASS(XR.copy(END   + 1, 1));
        ASSERT_SAFE_PASS(XR.copy(END   + 2, 0));
    }
#endif

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tTesting exceptions.\n");
    {
        const size_type LENGTH = 3;

        Obj        mX(STRING, LENGTH);
        const Obj& X = mX;

        bool outOfRangeCaught = false;

        try {
            X.copy(TARGET, LENGTH, LENGTH + 1);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }
        ASSERT(outOfRangeCaught);
    }
#endif

    // Check that no additional memory has been allocated.
    ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING CUTTING METHODS
    //
    // Concerns:
    //: 1 Empty suffix and prefix can be removed from an empty object.
    //:
    //: 2 The 'remove_prefix' method shifts the beginning of the underlying
    //:   string and decreases the size of the 'string_view' by the length of
    //:   removed prefix.
    //:
    //: 3 The 'remove_suffix' method leaves the beginning of the underlying
    //:   string unaffected, but decreases the size of the 'string_view' by the
    //:   length of removed suffix.
    //:
    //: 4 No additional memory is allocated.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an object with null parameters, remove empty suffix and prefix
    //:   and verify object's value by accessors.  (C-1)
    //:
    //: 3 Using a loop-based approach, construct a set of the pairs 'P'
    //:   '(ADDR, LENGTH)', where
    //:   ADDR   is an address in the range '[S, S + N]', where 'S' is a
    //:           some string constant and 'N' is some non-negative integer
    //:           constant
    //:   LENGTH is an integer constant in the range '[0, N - position]', where
    //:          the 'position' is the size of the indent of the ADDR from the
    //:          beginning of 'S'
    //:   For each element 'p' in the set:
    //:
    //:   1 Construct a set 'S' of sizes of prefixes/suffixes in the range
    //:     '[0, LENGTH]'.  For each element 's' in the 'S':
    //:
    //:     1 Construct a pair of objects (o1, o2), referring to the ADDR and
    //:       having LENGTH symbols.
    //:
    //:     2 Call 'remove_prefix' for the 'o1', passing 's' as a
    //:       parameter.  Verify the value of 'o1' using basic accessors.
    //:       (C-2)
    //:
    //:     3 Call 'remove_suffix' for the 'o2', passing 's' as a
    //:       parameter.  Verify the value of 'o2' using basic accessors.
    //:       (C-3)
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
    //:   (C-5)
    //:
    //: 5 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-4)
    //
    // Testing:
    //   void remove_prefix(size_type numChars);
    //   void remove_suffix(size_type numChars);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty objects cutting.
    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        mXEmpty.remove_prefix(0);

        ASSERTV(0 == XEmpty.data()  );
        ASSERTV(0 == XEmpty.length());

        mXEmpty.remove_suffix(0);

        ASSERTV(0 == XEmpty.data()  );
        ASSERTV(0 == XEmpty.length());
    }

    // Testing non-empty objects cutting.
    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;

            for (size_type k = 0; k <= LENGTH; ++k) {
                const size_type  NUM_CHARS        = k;
                const size_type  EXPECTED_LENGTH  = LENGTH - NUM_CHARS;
                const TYPE      *EXPECTED_P_START = START + NUM_CHARS;
                const TYPE      *EXPECTED_S_START = START;

                Obj          mXP(START, LENGTH);
                const Obj&   XP = mXP;
                Obj          mXS(START, LENGTH);
                const Obj&   XS = mXS;

                ASSERTV(i, j, k, XP.data(),   START  == XP.data());
                ASSERTV(i, j, k, XP.length(), LENGTH == XP.length());
                ASSERTV(i, j, k, XS.data(),   START  == XS.data());
                ASSERTV(i, j, k, XS.length(), LENGTH == XS.length());

                mXP.remove_prefix(NUM_CHARS);
                mXS.remove_suffix(NUM_CHARS);

                ASSERTV(i, j, k, EXPECTED_P_START, XP.data(),
                        EXPECTED_P_START == XP.data());
                ASSERTV(i, j, k, EXPECTED_LENGTH,  XP.length(),
                        EXPECTED_LENGTH  == XP.length());
                ASSERTV(i, j, k, START, EXPECTED_S_START, XS.data(),
                        EXPECTED_S_START == XS.data());
                ASSERTV(i, j, k, EXPECTED_LENGTH,  XS.length(),
                        EXPECTED_LENGTH  == XS.length());
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const TYPE *NULL_PTR = 0;

        // Testing 'remove_prefix()'.
        Obj mXEmpty(NULL_PTR, 0);

        ASSERT_SAFE_PASS(mXEmpty.remove_prefix(0));
        ASSERT_SAFE_FAIL(mXEmpty.remove_prefix(1));

        Obj mXP1(STRING, 1);
        Obj mXP2(STRING, 1);
        Obj mXP3(STRING, 1);

        ASSERT_SAFE_PASS(mXP1.remove_prefix(0));
        ASSERT_SAFE_PASS(mXP2.remove_prefix(1));
        ASSERT_SAFE_FAIL(mXP3.remove_prefix(2));

        // Testing 'remove_suffix()'.

        ASSERT_SAFE_PASS(mXEmpty.remove_suffix(0));
        ASSERT_SAFE_FAIL(mXEmpty.remove_suffix(1));

        Obj mXS1(STRING, 1);
        Obj mXS2(STRING, 1);
        Obj mXS3(STRING, 1);

        ASSERT_SAFE_PASS(mXS1.remove_suffix(0));
        ASSERT_SAFE_PASS(mXS2.remove_suffix(1));
        ASSERT_SAFE_FAIL(mXS3.remove_suffix(2));
    }
#endif

    // Check that no additional memory has been allocated.
    ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATOR ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor returns the expected type of iterator.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 The range '[[c,r]begin(), [c,r]end())' contains all elements of the
    //:   underlying string.
    //:
    //: 5 Each accessor returns the past-the-end iterator for an empty object.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an empty object.  Verify the values, returned by accessors.
    //:   As we are not able to dereference past-the-end iterators,  we can
    //:   only compare them each other.  (C-5)
    //:
    //: 3 Using a loop-based approach, construct a set of objects, 'O', on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'o' in the
    //:   'O' set iterate through all elements in 'o' from the '[c,r]begin' to
    //:   the '[c,r]end' and verify values, iterators referring to.
    //:   (C-1..2, 4)
    //:
    //: 4 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   const_iterator  begin() const;
    //   const_iterator cbegin() const;
    //   const_iterator  end() const;
    //   const_iterator cend() const;
    //   const_reverse_iterator  rbegin() const;
    //   const_reverse_iterator crbegin() const;
    //   const_reverse_iterator  rend() const;
    //   const_reverse_iterator crend() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    typedef typename Obj::const_iterator         CIter;
    typedef typename Obj::const_reverse_iterator CRIter;

    // Testing empty object.
    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        ASSERTV(XEmpty.begin()   == XEmpty.cbegin() );
        ASSERTV(XEmpty.begin()   == XEmpty.end()    );
        ASSERTV(XEmpty.cbegin()  == XEmpty.cend()   );
        ASSERTV(XEmpty.rbegin()  == XEmpty.crbegin());
        ASSERTV(XEmpty.rbegin()  == XEmpty.rend()   );
        ASSERTV(XEmpty.crbegin() == XEmpty.crend()  );
    }

    // Testing non-empty objects.
    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;

            Obj         mX(START, LENGTH);
            const Obj&  X = mX;

            ASSERT(X.begin()  == X.cbegin());
            ASSERT(X.end()    == X.cend());
            ASSERT(X.rbegin() == X.crbegin());
            ASSERT(X.rend()   == X.crend());

            size_type counter = 0;
            for (CIter iter = mX.begin();
                 iter != mX.end();
                 ++iter, ++counter) {
                ASSERTV(counter, START[counter] == *iter);
            }
            ASSERTV(counter, LENGTH == counter);

            counter = 0;

            for (CIter citer = X.cbegin();
                 citer != X.cend();
                 ++citer, ++counter) {
                ASSERTV(counter, START[counter] == *citer);
            }
            ASSERTV(counter, LENGTH == counter);

            counter = LENGTH;

            for (CRIter riter = X.rbegin();
                 riter != X.rend();
                 ++riter, --counter) {
                const size_type POSITION = counter - 1;
                ASSERTV(counter, START[POSITION] == *riter);
            }
            ASSERTV(counter, 0 == counter);

            counter = LENGTH;

            for (CRIter criter = X.crbegin();
                 criter != X.crend();
                 ++criter, --counter) {
                const size_type POSITION = counter - 1;
                ASSERTV(counter, START[POSITION] == *criter);
            }
            ASSERTV(counter, 0 == counter);

        }
    }

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase10()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENT ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 QoI: Asserted precondition violations are detected when enabled.
    //:
    //: 5 The 'at' throws 'std::out_of_range' exception when passed an
    //:   out-of-bound position (e.g. 'position >= length()').
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Using a loop-based approach, dynamically construct a set of objects
    //:   on the constant string 'S' (that can include null symbol), pointing
    //:   to the addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'o' in the
    //:   set:
    //:
    //:   1 Call 'front' and 'back' accessors and verify returned values.
    //:
    //:   2 For each position in the range '[0, o.length()]' call 'operator[]'
    //:     and 'at' accessors and verify returned values.  (C-1..2)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
    //:   (C-4)
    //:
    //: 4 Using 'try-catch' block, verify that 'std::out_of_range' exception is
    //:   thrown by the 'at' when 'position >= length()'.  (C-5)
    //:
    //: 5 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   const_reference operator[](size_type position) const;
    //   const_reference at(size_type position) const;
    //   const_reference front() const;
    //   const_reference back() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    if (verbose) printf("\tTesting basic behavior.\n");

    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;
            const TYPE       FRONT  = *START;

            Obj         mX(START, LENGTH);
            const Obj&  X = mX;

            if (!X.empty()) {
                const TYPE BACK   = *(START + LENGTH - 1);
                ASSERTV(i, j, FRONT, X.front(), FRONT == X.front());
                ASSERTV(i, j, BACK,  X.back(),  BACK  == X.back() );
            }

            for (size_type k = 0; k < j; ++k) {
                const size_type POSITION = k;
                const TYPE      EXPECTED = *(START + POSITION);

                ASSERTV(i, j, k, EXPECTED, X[POSITION],
                        EXPECTED == X[POSITION]  );
                ASSERTV(i, j, k, EXPECTED, X.at(POSITION),
                        EXPECTED == X.at(POSITION));
            }
        }
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const size_type LENGTH = 3;

        Obj           mXEmpty;
        const Obj&    XEmpty = mXEmpty;
        Obj           mXNonEmpty(STRING, LENGTH);
        const Obj&    XNonEmpty = mXNonEmpty;

        // Testing 'front()' and 'back()'.

        ASSERT_SAFE_PASS(XNonEmpty.front());
        ASSERT_SAFE_PASS(XNonEmpty.back());
        ASSERT_SAFE_FAIL(XEmpty.front());
        ASSERT_SAFE_FAIL(XEmpty.back());

        // Testing 'operator[]'.

        ASSERT_SAFE_PASS(XNonEmpty[1]);
        ASSERT_SAFE_PASS(XNonEmpty[2]);
        ASSERT_SAFE_FAIL(XNonEmpty[3]);
        ASSERT_SAFE_FAIL(XEmpty[0]);
    }
#endif

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tTesting exceptions.\n");
    {
        const size_type LENGTH = 3;

        Obj        mX(STRING, LENGTH);
        const Obj& X = mX;

        bool outOfRangeCaught = false;

        try {
            (void) X.at(LENGTH);
        }
        catch (const std::out_of_range&) {
            outOfRangeCaught = true;
        }

        ASSERT(outOfRangeCaught);
    }
#endif

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING SIZE ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 The accessors correctly return values for an empty object.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an empty object.  Verify the values, returned by accessors.
    //:   (C-4)
    //:
    //: 3 Using a loop-based approach, construct a set of objects, 'O', on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'o' in the
    //:   set 'O' call accessors and verify return values.  (C-1..2)
    //:
    //: 4 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   size_type size() const;
    //   size_type max_size() const;
    //   bool empty() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    // libc++ defines string_view::max_size using numeric_limits<size_type> see
    // https://github.com/llvm-mirror/libcxx/blob/master/include/string_view
    const size_type  MAX_MAX_SIZE =
#if !defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
        (Obj::npos - 1) / sizeof(TYPE);
#else
        std::numeric_limits<typename Obj::size_type>::max();
#endif
    // Testing empty object.
    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        ASSERTV(XEmpty.size(),      0            == XEmpty.size()     );
        ASSERTV(XEmpty.max_size(),  MAX_MAX_SIZE >= XEmpty.max_size() );
        ASSERTV(XEmpty.empty(),     true         == XEmpty.empty()    );
        ASSERTV(bsl::empty(XEmpty), true         == bsl::empty(XEmpty));
    }

    // Testing non-empty objects.
    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET             = i;
            const size_type  LENGTH             = j;
            const TYPE      *START              = STRING + OFFSET;
            bool             EXPECTED_EMPTINESS = LENGTH ? false : true;

            Obj         mX(START, LENGTH);
            const Obj&  X = mX;

            ASSERTV(i, j, X.size(),      LENGTH             == X.size()     );
            ASSERTV(i, j, X.max_size(),  MAX_MAX_SIZE       >= X.max_size() );
            ASSERTV(i, j, X.empty(),     EXPECTED_EMPTINESS == X.empty()    );
            ASSERTV(i, j, bsl::empty(X), EXPECTED_EMPTINESS == bsl::empty(X));
        }
    }

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase8()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS
    //
    // Concerns:
    //: 1 The default constructor setup null values for all fields.
    //:
    //: 2 The value constructor correctly handles null symbols in the passed
    //:   string.
    //:
    //: 3 QoI: Asserted precondition violations are detected when enabled.
    //:
    //: 4 The constructors allocate no memory from any allocator.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an object using default constructor and verify its value by
    //:   accessors.  (C-1)
    //:
    //: 3 Create an object using value constructor.  Verify object's value by
    //:   accessors.
    //:
    //: 4 Using a loop-based approach, construct a set of objects on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant and verify their values by accessors.
    //:   (C-2)
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-3)
    //:
    //: 6 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-4)
    //
    // Testing:
    //   basic_string_view();
    //   basic_string_view(const CHAR_TYPE *str);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
    bslma::TestAllocator da("default",   veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing default constructor.

    {
        Obj        *objPtr = new(fa) Obj();
        Obj&        mX     = *objPtr;
        const Obj&  X      = mX;

        ASSERTV(fa.numBytesInUse(), sizeof(Obj) == fa.numBytesInUse());

        ASSERTV(X.data(),   0 == X.data());
        ASSERTV(X.length(), 0 == X.length());

        fa.deleteObject(objPtr);
    }

    // Testing value constructor.

    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        const size_type  OFFSET          = i;
        const TYPE      *START           = STRING + OFFSET;
        const size_type  EXPECTED_LENGTH = TRAITS::length(START);

        Obj        *objPtr = new (fa) Obj(START);
        Obj&        mX     = *objPtr;
        const Obj&  X      = mX;

        ASSERTV(i, fa.numBytesInUse(), sizeof(Obj) == fa.numBytesInUse());

        ASSERTV(i, START,  X.data(), START == X.data()  );
        ASSERTV(i, EXPECTED_LENGTH,   X.length(),
                   EXPECTED_LENGTH == X.length());

        fa.deleteObject(objPtr);
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;
        (void) hG;

        // Since the constructor relies on the trait's function when
        // calculating the length of the passed string, we use dummy trait,
        // whose method returns the same value (npos) for any string instead of
        // allocating an enormous amount of memory.

        typedef typename bsl::basic_string_view<TYPE, DummyTrait<TYPE> >
                                                               DummyStringView;

        ASSERT_SAFE_PASS((Obj(            STRING)));
        ASSERT_SAFE_FAIL((Obj(                 0)));
        ASSERT_SAFE_FAIL((DummyStringView(STRING)));
    }
#endif

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable
    //:   target object to that of any source object.
    //:
    //: 2 The signature and return type are standard.
    //:
    //: 3 The reference returned is to the target object (i.e., '*this').
    //:
    //: 4 The value of the source object is not modified.
    //:
    //: 5 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //: 6 Empty objects can be assigned/modified by copy-assignment operator.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-2)
    //:
    //: 2 Create an empty object.  Assign this object to itself using
    //:   copy-assignment operator.  Verify object's value using basic
    //:   accessors ('data' and 'length').  (C-4)
    //:
    //: 3 Using a loop-based approach, construct a set of objects, 'O1', on the
    //:   constant string 'S1' (that can include null symbol), referring to the
    //:   addresses in the range '[S1, S1 + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position1]', where the 'position1' is the size of the indent
    //:   of the address from the beginning of 'S1'.  For each object 'o1' in
    //:   the 'O1' set:
    //:
    //:   1 Using a loop-based approach, construct a set of objects, 'O2' on
    //:     the constant string 'S2' (that can include null symbol), pointing
    //:     to the addresses in the range '[S2, S2 + N]', where the 'N' is the
    //:     integer constant from P-3, and having lengths in the range
    //:     '[0, N - position2]', where the 'position2' is the size of the
    //:     indent of the address from the beginning of 'S2'.  For each object
    //:     'o2' in the 'O2' set:
    //:
    //:     1 Assign the 'o1' to itself and verify the RESULTs using basic
    //:       accessors.  (C-5)
    //:
    //:     1 Assign the 'o2' to the 'o1' and verify the results using basic
    //:       accessors.
    //:
    //:     2 Assign empty object to the 'o1' and verify the results using
    //:       basic accessors.
    //:
    //:     3 Assign the 'o2' to the empty object and verify the results using
    //:       basic accessors.  (C-1, 3, 6)
    //
    // Testing:
    //   basic_string_view& operator=(basic_string_view rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        operatorPtr operatorAssignment = &Obj::operator=;
        (void)operatorAssignment;  // quash potential compiler warning
    }

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty object self-assignment.
    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        Obj *mREmpty = &(mXEmpty = XEmpty);

        ASSERTV(&XEmpty == mREmpty        );
        ASSERTV(0       == XEmpty.data()  );
        ASSERTV(0       == XEmpty.length());
    }

    const TYPE      *STRING1       = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;
    TYPE             STRING2[STRING_LENGTH];

    memcpy(STRING2, STRING1, STRING_LENGTH * sizeof(TYPE));

    ASSERT(STRING1 != STRING2);
    ASSERT(0       == memcmp(STRING1, STRING2, STRING_LENGTH * sizeof(TYPE)));

    for (size_type i1 = 0; i1 < STRING_LENGTH; ++i1) {
        for (size_type j1 = 0; j1 <= STRING_LENGTH - i1; ++j1) {
            const size_type  OFFSET1 = i1;
            const size_type  LENGTH1 = j1;
            const TYPE      *START1  = STRING1 + OFFSET1;

            for (size_type i2 = 0; i2 < STRING_LENGTH; ++i2) {
                for (size_type j2 = 0; j2 <= STRING_LENGTH - i2; ++j2) {
                    const size_type  OFFSET2 = i2;
                    const size_type  LENGTH2 = j2;
                    const TYPE      *START2  = STRING2 + OFFSET2;

                    // Target object creation.

                    Obj        mX(START1, LENGTH1);
                    const Obj& X = mX;

                    // Testing self-assignment.

                    ASSERTV(X.data(),   START1  == X.data()  );
                    ASSERTV(X.length(), LENGTH1 == X.length());

                    Obj *mR = &(mX = X);
                    ASSERTV(            &X      == mR        );
                    ASSERTV(X.data(),   START1  == X.data()  );
                    ASSERTV(X.length(), LENGTH1 == X.length());

                    // Source object creation.

                    Obj        mY(START2, LENGTH2);
                    const Obj& Y = mY;

                    ASSERTV(X.data(),   START1  == X.data()  );
                    ASSERTV(X.length(), LENGTH1 == X.length());
                    ASSERTV(Y.data(),   START2  == Y.data());
                    ASSERTV(Y.length(), LENGTH2 == Y.length());

                    // Assignment.

                    mR = &(mX = Y);

                    ASSERTV(            &X      == mR        );
                    ASSERTV(X.data(),   START2  == X.data()  );
                    ASSERTV(X.length(), LENGTH2 == X.length());
                    ASSERTV(Y.data(),   START2  == Y.data());
                    ASSERTV(Y.length(), LENGTH2 == Y.length());
                }
            }

            // Testing empty object assignment / modification.
            const TYPE *NULL_PTR = 0;

            Obj        mXEmpty(NULL_PTR, 0);
            const Obj& XEmpty = mXEmpty;
            Obj        mX(START1, LENGTH1);
            const Obj& X = mX;
            Obj        mY(START1, LENGTH1);
            const Obj& Y = mY;

            mX = mXEmpty;

            ASSERTV(X.data(),        NULL_PTR == X.data()       );
            ASSERTV(X.length(),      0        == X.length()     );
            ASSERTV(XEmpty.data(),   NULL_PTR == XEmpty.data()  );
            ASSERTV(XEmpty.length(), 0        == XEmpty.length());

            mXEmpty = mY;

            ASSERTV(Y.data(),        START1   == Y.data()       );
            ASSERTV(Y.length(),      LENGTH1  == Y.length()     );
            ASSERTV(XEmpty.data(),   START1   == XEmpty.data()  );
            ASSERTV(XEmpty.length(), LENGTH1  == XEmpty.length());
        }
    }
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING SWAP
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not affect
    //:   the value of the object (alias-safety).
    //:
    //: 4 Empty objects can be swapped.
    //:
    //: 5 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 6 No memory allocation occurs as a result of swap operation (e.g.,
    //:   there is no dynamically created temporary strings).
    //
    // Plan:
    //: 1 Use the respective addresses of 'swap' member and 'swap' free
    //:   function to initialize function pointers having the appropriate
    //:   signatures and return types.
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Create an empty object.  Swap this object with itself using both
    //:   member function and free function.  Verify object's value using basic
    //:   accessors ('data' and 'length').  (C-4)
    //:
    //: 4 Using a loop-based approach, construct a set of objects, 'O1', on the
    //:   constant string 'S1' (that can include null symbol), referring to the
    //:   addresses in the range '[S1, S1 + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position1]', where the 'position1' is the size of the indent
    //:   of the address from the beginning of 'S1'.  For each object 'o1' in
    //:   the 'O1' set:
    //:
    //:   1 Swap the 'o1' with itself using both member function and free
    //:     function and verify it's value using basic accessors ('data' and
    //:     'length').  (C-3)
    //:
    //:   2 Using a loop-based approach, construct a set of objects, 'O2' on
    //:     the constant string 'S2' (that can include null symbol), pointing
    //:     to the addresses in the range '[S2, S2 + N]', where the 'N' is the
    //:     integer constant from P-4, and having lengths in the range
    //:     '[0, N - position2]', where the 'position2' is the size of the
    //:     indent of the address from the beginning of 'S2'.  For each object
    //:     'o2' in the 'O2' set:
    //:
    //:     1 Swap the 'o1' with the 'o2' using both member function and free
    //:       function and verify their values using basic accessors ('data'
    //:       and 'length').  (C-1)
    //:
    //:   1 Swap the 'o1' with empty object using both member function and free
    //:     function and verify theirs values using basic accessors ('data' and
    //:     'length').  (C-4)
    //:
    //: 5 Create two objects having different values, swap them using the
    //:   'invokeAdlSwap' helper function template and verify that the values
    //:   have been exchanged.  (C-5)
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-6)
    //
    // Testing:
    //   swap(basic_string_view& rhs);                         // method
    //   swap(basic_string_view& lhs, basic_string_view& rhs); // free function
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (verbose) printf("\tTesting basic behavior.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING1       = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;
    TYPE             STRING2[STRING_LENGTH];

    memcpy(STRING2, STRING1, STRING_LENGTH * sizeof(TYPE));

    ASSERT(STRING1 != STRING2);
    ASSERT(0       == memcmp(STRING1, STRING2, STRING_LENGTH * sizeof(TYPE)));

    // Testing empty objects swap.
    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        mXEmpty.swap(mXEmpty);

        ASSERTV(0 == XEmpty.data()  );
        ASSERTV(0 == XEmpty.length());

        swap(mXEmpty, mXEmpty);

        ASSERTV(0 == XEmpty.data()  );
        ASSERTV(0 == XEmpty.length());
    }

    for (size_type i1 = 0; i1 < STRING_LENGTH; ++i1) {
        for (size_type j1 = 0; j1 <= STRING_LENGTH - i1; ++j1) {
            const size_type  OFFSET1 = i1;
            const size_type  LENGTH1 = j1;
            const TYPE      *START1  = STRING1 + OFFSET1;

            Obj        mX1(START1, LENGTH1);
            const Obj& X1 = mX1;

            for (size_type i2 = 0; i2 < STRING_LENGTH; ++i2) {
                for (size_type j2 = 0; j2 <= STRING_LENGTH - i2; ++j2) {
                    const size_type  OFFSET2 = i2;
                    const size_type  LENGTH2 = j2;
                    const TYPE      *START2  = STRING2 + OFFSET2;

                    Obj        mX2(START2, LENGTH2);
                    const Obj& X2 = mX2;

                    ASSERTV(X1.data(),   START1  == X1.data()  );
                    ASSERTV(X1.length(), LENGTH1 == X1.length());
                    ASSERTV(X2.data(),   START2  == X2.data()  );
                    ASSERTV(X2.length(), LENGTH2 == X2.length());

                    // Testing method.

                    mX1.swap(mX2);

                    ASSERTV(X1.data(),   START2  == X1.data()  );
                    ASSERTV(X1.length(), LENGTH2 == X1.length());
                    ASSERTV(X2.data(),   START1  == X2.data()  );
                    ASSERTV(X2.length(), LENGTH1 == X2.length());

                    // Testing free function.

                    swap(mX1, mX2);

                    ASSERTV(X1.data(),   START1  == X1.data()  );
                    ASSERTV(X1.length(), LENGTH1 == X1.length());
                    ASSERTV(X2.data(),   START2  == X2.data()  );
                    ASSERTV(X2.length(), LENGTH2 == X2.length());
                }
            }
            const TYPE *NULL_PTR = 0;
            Obj         mXEmpty(NULL_PTR, 0);
            const Obj&  XEmpty = mXEmpty;

            mX1.swap(mXEmpty);

            ASSERTV(X1.data(),       0       == X1.data()      );
            ASSERTV(X1.length(),     0       == X1.length()    );
            ASSERTV(XEmpty.data(),   START1  == XEmpty.data()  );
            ASSERTV(XEmpty.length(), LENGTH1 == XEmpty.length());

            swap(mX1, mXEmpty);

            ASSERTV(X1.data(),       START1  == X1.data()      );
            ASSERTV(X1.length(),     LENGTH1 == X1.length()    );
            ASSERTV(XEmpty.data(),   0       == XEmpty.data()  );
            ASSERTV(XEmpty.length(), 0       == XEmpty.length());
        }
    }

    if (verbose) printf(
         "\tTesting that free 'swap' function is discoverable through ADL.\n");
    {
        Obj        mX1(STRING1, STRING_LENGTH);
        const Obj& X1 = mX1;
        Obj        mX2(STRING2, 0);
        const Obj& X2 = mX2;

        ASSERTV(X1.data(),   STRING1       == X1.data()  );
        ASSERTV(X1.length(), STRING_LENGTH == X1.length());
        ASSERTV(X2.data(),   STRING2       == X2.data()  );
        ASSERTV(X2.length(), 0             == X2.length());

        invokeAdlSwap(&mX1, &mX2);

        ASSERTV(X1.data(),   STRING2       == X1.data()  );
        ASSERTV(X1.length(), 0             == X1.length());
        ASSERTV(X2.data(),   STRING1       == X2.data()  );
        ASSERTV(X2.length(), STRING_LENGTH == X2.length());

        invokePatternSwap(&mX1, &mX2);

        ASSERTV(X1.data(),   STRING1       == X1.data()  );
        ASSERTV(X1.length(), STRING_LENGTH == X1.length());
        ASSERTV(X2.data(),   STRING2       == X2.data()  );
        ASSERTV(X2.length(), 0             == X2.length());
    }

    // Check that no additional memory has been allocated.
    ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase5()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the basic accessors).
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Destruction of the source object has no effect on the
    //:  copy-constructed object.
    //:
    //: 5 Destruction of the created object has no effect on the original one.
    //:
    //: 6 Empty object can be copy-constructed.
    //:
    //: 7 No memory allocation occurs as a result of copy construction (e.g.,
    //:   there is no new dynamically created string).
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an empty object.  Create another empty object using copy
    //:   constructor and passing the first object as a parameter.  Verify the
    //:   target object using basic accessors('data()' and 'length()'.  (C-6)
    //:
    //: 3 Using a loop-based approach, construct a set of objects, 'O1', on the
    //:   constant string 'S' (that can include null symbol), referring to the
    //:   addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position1]', where the 'position1' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object 'os1'
    //:   (object source 1) in the 'O1' set:
    //:
    //:   1 In scope using copy constructor and passing 'os1' as a parameter
    //:     create an object 'ot1' (object target 1).
    //:
    //:   2 Using the basic accessors verify the value of 'os1'.
    //:
    //:   3 Using the basic accessors verify the value of 'ot1'. (C-1..2)
    //:
    //:   4 Let 'ot1' go out of scope and verify, that 'os1' remains unchanged.
    //:     (C-5)
    //:
    //: 4 Using a loop-based approach, in scope construct a set of objects,
    //:   'O2', on the constant string 'S' (that can include null symbol),
    //:   referring to the addresses in the range '[S, S + N]', where the 'N'
    //:   is some non-negative integer constant, and having lengths in the
    //:   range '[0, N - position2]', where the 'position2' is the size of the
    //:   indent of the address from the beginning of 'S'.  For each object
    //:   'os2' in the 'O2' set:
    //:
    //:   1 Using copy constructor and passing 'os1' as a parameter dynamically
    //:     create an object 'ot2'.
    //:
    //:   2 Using the basic accessors verify the value of 'os2'.  (C-3)
    //:
    //:   3 Using the basic accessors verify the value of 'ot2'.
    //:
    //:   4 Let 'os2' go out of scope and verify, that 'ot2' remains unchanged.
    //:     (C-4)
    //:
    //:   5 Destroy object 'ot2'
    //:
    //: 5 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-7)
    //
    // Testing:
    //   basic_string_view(basic_string_view original);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Testing empty objects copy construction.
    {
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

        Obj        mXEmptySource(0, 0);
        const Obj& XEmptySource = mXEmptySource;

        Obj        *objPtr = new(fa) Obj(XEmptySource);
        Obj         mXEmptyTarget = *objPtr;
        const Obj&  XEmptyTarget = mXEmptyTarget;

        ASSERTV(fa.numBytesInUse(), sizeof(Obj) == fa.numBytesInUse());

        ASSERTV(XEmptyTarget.data(),   0 == XEmptyTarget.data()  );
        ASSERTV(XEmptyTarget.length(), 0 == XEmptyTarget.length());

        fa.deleteObject(objPtr);

    }

    // Testing non-empty objects copy construction.

    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const TYPE      *START  = STRING + i;
            const size_type  LENGTH = j;

            // Create the first source object.
            Obj        mX1(START, LENGTH);
            const Obj& X1 = mX1;

            ASSERTV(X1.data(),   START  == X1.data()  );
            ASSERTV(X1.length(), LENGTH == X1.length());

            // Pointer to the second target object.
            Obj *pY2;

            {
                // Create the first target object.
                Obj        mY1(X1);
                const Obj& Y1 = mY1;

                ASSERTV(X1.data(),   START  == X1.data()  );
                ASSERTV(X1.length(), LENGTH == X1.length());
                ASSERTV(Y1.data(),   START  == Y1.data());
                ASSERTV(Y1.length(), LENGTH == Y1.length());

                // Create the second source object.
                Obj        mX2(START, LENGTH);
                const Obj& X2 = mX2;

                ASSERTV(X2.data(),   START  == X2.data()  );
                ASSERTV(X2.length(), LENGTH == X2.length());

                // Create the second target object.
                pY2 = new Obj(X2);
                const Obj& Y2 = *pY2;

                ASSERTV(X2.data(),   START  == X2.data()  );
                ASSERTV(X2.length(), LENGTH == X2.length());
                ASSERTV(Y2.data(),   START  == Y2.data());
                ASSERTV(Y2.length(), LENGTH == Y2.length());
            }

            // Check, that source/target object destruction doesn't affect the
            // target/source object.

            const Obj& Y2 = *pY2;

            ASSERTV(X1.data(),   START  == X1.data()  );
            ASSERTV(X1.length(), LENGTH == X1.length());
            ASSERTV(Y2.data(),   START  == Y2.data());
            ASSERTV(Y2.length(), LENGTH == Y2.length());

            delete pY2;
        }
    }

    // Check that no additional memory has been allocated.
    ASSERTV(da.numBytesTotal(), 0 == da.numBytesTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING EQUALITY COMPARISON
    //
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.  Two views have the same value if they have the same
    //:   length, and the characters at each respective position have the same
    //:   value according to 'CHAR_TRAITS::eq'.
    //:
    //: 2 'true  == (X == X)' (i.e., identity)
    //:
    //: 3 'false == (X != X)' (i.e., identity)
    //:
    //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if '!(X == Y)'
    //:
    //: 7 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 8 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //: 9 No memory allocation occurs as a result of comparison (e.g., there is
    //:   no new dynamically created string).
    //:
    //:10 The equality operator's signature and return type are standard.
    //:
    //:11 The inequality operator's signature and return type are standard.
    //:
    //:12 The empty objects can be compared.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality-comparison
    //:   operators defined in this component.  (C-7..8, 10..11)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Create a pair of empty objects, compare them and verify the results.
    //:
    //: 4 Using a loop-based approach, construct a set of objects, 'O1', on the
    //:   constant string 'S1' (that can include null symbol), referring to the
    //:   addresses in the range '[S1, S1 + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position1]', where the 'position1' is the size of the indent
    //:   of the address from the beginning of 'S1'.  For each object 'o1' in
    //:   the 'O1' set:
    //:
    //:   1 Compare 'o1' and 'o1' and verify the expected return value for both
    //:     '==' and '!='.  (C-2..3)
    //:
    //:   2 Create an empty object, compare 'o1' with it and verify the
    //:     expected return value for both '==' and '!='.  (C-12)
    //:
    //:   2 Using a loop-based approach, construct a set of objects, 'O2' on
    //:     the constant string 'S2' (that can include null symbol), pointing
    //:     to the addresses in the range '[S2, S2 + N]', where the 'N' is the
    //:     integer constant from P-4, and having lengths in the range
    //:     '[0, N - position2]', where the 'position2' is the size of the
    //:     indent of the address from the beginning of 'S2'.  For each object
    //:     'o2' in the 'O2' set:
    //:
    //:     1 Compare 'o1' and 'o2' and verify the expected return value for
    //:       both '==' and '!='.  (C-1, 4..6)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-9)
    //
    // Testing:
    //   bool operator==(basic_string_view, basic_string_view);
    //   bool operator!=(basic_string_view, basic_string_view);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef bool (*OP)(Obj, Obj);

        OP op = &bsl::operator==;
        (void) op;
        op    = &bsl::operator!=;
        (void) op;
    }
#endif

    if (verbose) printf("\tTesting basic behavior.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const TYPE      *STRING1       = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;
    TYPE             STRING2[STRING_LENGTH];

    memcpy(STRING2, STRING1, STRING_LENGTH * sizeof(TYPE));

    ASSERT(STRING1 != STRING2);
    ASSERT(0       == memcmp(STRING1, STRING2, STRING_LENGTH * sizeof(TYPE)));

    // Testing empty objects comparison.
    {
        Obj        mXEmpty1(0, 0);
        const Obj& XEmpty1 = mXEmpty1;

        Obj        mXEmpty2(0, 0);
        const Obj& XEmpty2 = mXEmpty2;

        ASSERTV(  XEmpty1 == XEmpty2 );
        ASSERTV(!(XEmpty1 != XEmpty2));
    }

    // Testing non-empty objects comparison.

    for (size_type i1 = 0; i1 < STRING_LENGTH; ++i1) {
        for (size_type j1 = 0; j1 <= STRING_LENGTH - i1; ++j1) {
            const size_type  OFFSET1  = i1;
            const size_type  LENGTH1  = j1;
            const TYPE      *START1   = STRING1 + OFFSET1;

            Obj  mXEmpty;    const Obj& XEmpty = mXEmpty;

            Obj         mX1(START1, LENGTH1);
            const Obj&  X1 = mX1;
            const bool  EMPTY_EXPECTED = (0 == LENGTH1);

            // Self comparison.

            ASSERT(  X1 == X1 );
            ASSERT(!(X1 != X1));

            // Comparison with empty object.

            ASSERT(( EMPTY_EXPECTED) == (X1     == XEmpty));
            ASSERT((!EMPTY_EXPECTED) == (X1     != XEmpty));
            ASSERT(( EMPTY_EXPECTED) == (XEmpty == X1    ));
            ASSERT((!EMPTY_EXPECTED) == (XEmpty != X1    ));

            for (size_type i2 = 0; i2 < STRING_LENGTH; ++i2) {
                for (size_type j2 = 0; j2 <= STRING_LENGTH - i2; ++j2) {
                    const size_type  OFFSET2 = i2;
                    const size_type  LENGTH2 = j2;
                    const TYPE      *START2  = STRING2 + OFFSET2;

                    Obj        mX2(START2, LENGTH2);
                    const Obj& X2 = mX2;
                    const bool EXPECTED =
                          (LENGTH1 == LENGTH2) &&
                          (0 == memcmp(START1, START2, LENGTH1* sizeof(TYPE)));

                    ASSERTV(i1, j1, i2, j2, EXPECTED,
                            ( EXPECTED) == (X1 == X2));
                    ASSERTV(i1, j1, i2, j2, EXPECTED,
                            ( EXPECTED) == (X2 == X1));
                    ASSERTV(i1, j1, i2, j2, EXPECTED,
                            (!EXPECTED) == (X1 != X2));
                    ASSERTV(i1, j1, i2, j2, EXPECTED,
                            (!EXPECTED) == (X2 != X1));
                }
            }
        }
    }

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING OPERATOR<<
    //   The result of the operator invocation depends not only on object's
    //   value, but on the stream's settings (width and adjustment).  This is
    //   taken into account when writing test.
    //
    // Concerns:
    //: 1 Empty object is correctly streamed.
    //:
    //: 2 The operator takes into account the state of the passed stream (its
    //:   width and adjustment).
    //:
    //: 3 Null symbols in the object are correctly streamed.
    //:
    //: 4 The operator sets width of the passed stream to zero.
    //:
    //: 5 The operator returns a reference to the passed stream.
    //:
    //
    // Plan:
    //: 1 Create an empty object, stream it using the 'operator<<' and verify
    //:   the results.  (C-1)
    //:
    //: 2 Construct a couple of 'std::ostringstream' objects having different
    //:   adjustments (left and right).  For each object:
    //:
    //:   1 Using a loop-based approach, construct a set of 'string_view'
    //:     objects 'O' on the constant string 'S' (that can include null
    //:     symbol) having lengths in the range '[0, N]', where the 'N' is some
    //:     non-negative integer constant.  For each object in the set 'O':
    //:
    //:     1 Adjust width of the stream objects from the P-1.  The width has
    //:       the same value for all 'string_view' objects, but due to the
    //:       different lengths of these objects it can be less, equal or
    //:       greater than they.
    //:
    //:     2 Use 'operator<<' to stream objects from the P-2.1 and verify the
    //:       results.  (C-2..5)
    //:
    //:     3 Return stream objects to the original state.
    //
    // Testing:
    //   operator<<(std::basic_ostream& stream, basic_string_view view);
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;
    const size_type  STREAM_WIDTH  = 11;
    const TYPE       FILL_SYMBOL   = 'a';

    // Testing empty object streaming.

    {
        const TYPE *NULL_PTR = 0;
        Obj         mXEmpty(NULL_PTR, 0);
        const Obj&  XEmpty = mXEmpty;

        std::basic_ostringstream<TYPE, TRAITS> streamLeft;
        std::basic_ostringstream<TYPE, TRAITS> streamRight;

        streamLeft.fill(FILL_SYMBOL);
        streamRight.fill(FILL_SYMBOL);
        streamLeft.width(STREAM_WIDTH);
        streamRight.width(STREAM_WIDTH);
        streamLeft.setf(std::ios::left, std::ios_base::adjustfield);
        streamRight.setf(std::ios::right, std::ios_base::adjustfield);

        std::basic_string<TYPE> LEFT_EXPECTED;
        std::basic_string<TYPE> RIGHT_EXPECTED;

        LEFT_EXPECTED.append(STREAM_WIDTH, FILL_SYMBOL);
        RIGHT_EXPECTED.append(STREAM_WIDTH, FILL_SYMBOL);

        std::basic_ostream<TYPE, TRAITS>& leftResult  = streamLeft  << XEmpty;
        std::basic_ostream<TYPE, TRAITS>& rightResult = streamRight << XEmpty;

        ASSERTV(&leftResult  == &streamLeft);
        ASSERTV(&rightResult == &streamRight);

        ASSERTV(streamLeft.width(),  0 == streamLeft.width() );
        ASSERTV(streamRight.width(), 0 == streamRight.width());

        ASSERT(LEFT_EXPECTED  == streamLeft.str());
        ASSERT(RIGHT_EXPECTED == streamRight.str());
    }

    // Testing non-empty object streaming.

    std::basic_ostringstream<TYPE, TRAITS> streamLeft;
    std::basic_ostringstream<TYPE, TRAITS> streamRight;

    streamLeft.fill(FILL_SYMBOL);
    streamRight.fill(FILL_SYMBOL);

    streamLeft.setf( std::ios::left,  std::ios_base::adjustfield);
    streamRight.setf(std::ios::right, std::ios_base::adjustfield);

    std::basic_string<TYPE> LESS_LEFT_EXPECTED;
    std::basic_string<TYPE> LESS_RIGHT_EXPECTED;
    std::basic_string<TYPE> GREATER_EXPECTED;

    LESS_LEFT_EXPECTED.reserve(STREAM_WIDTH);
    LESS_RIGHT_EXPECTED.reserve(STREAM_WIDTH);
    GREATER_EXPECTED.reserve(STRING_LENGTH);

    for (size_type i = 0; i < STREAM_WIDTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH; ++j) {
            const TYPE      *START  = STRING;
            const size_type  LENGTH = j;

            streamLeft.width(STREAM_WIDTH);
            streamRight.width(STREAM_WIDTH);

            if (LENGTH <= STREAM_WIDTH) {
                LESS_LEFT_EXPECTED.append(STREAM_WIDTH - LENGTH, FILL_SYMBOL);
                LESS_RIGHT_EXPECTED.append(STREAM_WIDTH - LENGTH, FILL_SYMBOL);

                LESS_LEFT_EXPECTED.insert(0, START, LENGTH);
                LESS_RIGHT_EXPECTED.insert(STREAM_WIDTH - LENGTH,
                                           START,
                                           LENGTH);
            }
            else {
                GREATER_EXPECTED.append(START, LENGTH);
            }

            Obj        mX(START, LENGTH);
            const Obj& X = mX;

            ASSERTV(streamLeft.width(),  STREAM_WIDTH == streamLeft.width() );
            ASSERTV(streamRight.width(), STREAM_WIDTH == streamRight.width());

            std::basic_ostream<TYPE, TRAITS>& leftResult  = streamLeft  << X;
            std::basic_ostream<TYPE, TRAITS>& rightResult = streamRight << X;

            ASSERTV(&leftResult  == &streamLeft);
            ASSERTV(&rightResult == &streamRight);

            ASSERTV(streamLeft.width(),  0 == streamLeft.width() );
            ASSERTV(streamRight.width(), 0 == streamRight.width());

            if (LENGTH <= STREAM_WIDTH) {
                ASSERT(LESS_LEFT_EXPECTED  == streamLeft.str());
                ASSERT(LESS_RIGHT_EXPECTED == streamRight.str());
            }
            else {
                ASSERT(GREATER_EXPECTED  == streamLeft.str());
                ASSERT(GREATER_EXPECTED  == streamRight.str());
            }

            // Final cleanup.

            LESS_LEFT_EXPECTED.clear();
            LESS_RIGHT_EXPECTED.clear();
            GREATER_EXPECTED.clear();

            streamLeft.str(
                     std::basic_string<TYPE, TRAITS, std::allocator<TYPE> >());
            streamRight.str(
                     std::basic_string<TYPE, TRAITS, std::allocator<TYPE> >());

            streamLeft.clear();
            streamRight.clear();
        }
    }
}

template <class TYPE, class TRAITS>
void TestDriver<TYPE, TRAITS>::testCase2()
{
    // ------------------------------------------------------------------------
    // PRIMARY MANIPULATORS AND BASIC ACCESSORS
    //   As tests for primary manipulator and basic accessors are identical,
    //   these methods are tested simultaneously.
    //
    // Concerns:
    //: 1 The constructor correctly handle null values, passed as parameters.
    //:
    //: 2 The null symbol ('\0') in the passed string is correctly handled by
    //:   the constructor.
    //:
    //: 3 Neither constructor nor accessors allocate memory from any allocator.
    //:
    //: 4 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 5 Each accessor method is declared 'const'.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 2 Create an object with null parameters and verify its value by
    //:   accessors.  (C-1)
    //:
    //: 3 Using a loop-based approach, dynamically construct a set of objects
    //:   on the constant string 'S' (that can include null symbol), pointing
    //:   to the addresses in the range '[S, S + N]', where the 'N' is some
    //:   non-negative integer constant, and having lengths in the range
    //:   '[0, N - position]', where the 'position' is the size of the indent
    //:   of the address from the beginning of 'S'.  For each object in the
    //:   set:
    //:
    //:   1 Use attribute accessors to verify object's value.  (C-2, 4..5)
    //:
    //:   2 Verify that no additional memory is allocated from the allocator
    //:     used for object construction.
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
    //:   (C-6)
    //:
    //: 6 Use the test allocator from P-1 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   basic_string_view(const CHAR_TYPE *str, size_type numChars);
    //   size_type length() const;
    //   const_pointer data() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("for %s type.\n", NameOf<TYPE>().name());

    const TYPE      *STRING        = s_testString;
    const size_type  STRING_LENGTH = s_testStringLength;

    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
    bslma::TestAllocator da("default",   veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\tTesting basic behavior.\n");

    // Testing empty object construction.

    {
        bslma::TestAllocator  efa("empty footprint", veryVeryVeryVerbose);
        Obj                  *emptyObjPtr = new (efa) Obj(0, 0);
        Obj&                  mXEmpty     = *emptyObjPtr;
        const Obj&            XEmpty      = mXEmpty;

        ASSERTV(0 == XEmpty.data()    );
        ASSERTV(0 == bsl::data(XEmpty));
        ASSERTV(0 == XEmpty.length()  );

        ASSERTV(efa.numBytesInUse(), sizeof(Obj) == efa.numBytesInUse());

        efa.deleteObject(emptyObjPtr);
    }

    // Testing non-empty object construction.

    for (size_type i = 0; i < STRING_LENGTH; ++i) {
        for (size_type j = 0; j <= STRING_LENGTH - i; ++j) {
            const size_type  OFFSET = i;
            const size_type  LENGTH = j;
            const TYPE      *START  = STRING + OFFSET;

            Obj        *objPtr = new (fa) Obj(START, LENGTH);
            Obj&        mX     = *objPtr;
            const Obj&  X      = mX;

            ASSERTV(i, j, START,  X.data(),     START  == X.data()    );
            ASSERTV(i, j, START,  bsl::data(X), START  == bsl::data(X));
            ASSERTV(i, j, LENGTH, X.length(),   LENGTH == X.length()  );

            ASSERTV(i, j, fa.numBytesInUse(),
                    sizeof(Obj) == fa.numBytesInUse());

            fa.deleteObject(objPtr);
        };
    }

#if !defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    if (verbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        ASSERT_SAFE_PASS((Obj(STRING, 0            )));
        ASSERT_SAFE_PASS((Obj(STRING, STRING_LENGTH)));
        ASSERT_SAFE_PASS((Obj(0     , 0            )));
        ASSERT_SAFE_FAIL((Obj(0     , STRING_LENGTH)));

        ASSERT_SAFE_PASS((Obj(STRING, (Obj::npos - 1) / sizeof(TYPE))));
        ASSERT_SAFE_FAIL((Obj(STRING, Obj::npos    )));
    }
#endif

    // Verify no memory was ever allocated.
    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

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

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                          veryVeryVeryVerbose);
    bslma::Allocator     *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    // Confirm no static initialization locked the global allocator
    ASSERT(globalAllocator_p == bslma::Default::globalAllocator());

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    defaultAllocator_p = &defaultAllocator;
    bslma::Default::setDefaultAllocator(defaultAllocator_p);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 25: {
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
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The 'bsl::string_view' can be used as a lightweight replacement of the
// 'bsl::string', unless you need to modify the content.  It takes up no more
// space and doesn't allocate memory:
//..
        bslma::TestAllocator         da("Default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator sfa ("StringFootprint",     veryVeryVeryVerbose);
        bslma::TestAllocator svfa("StringViewFootprint", veryVeryVeryVerbose);
        bslma::TestAllocator ssa ("StringSupplied",      veryVeryVeryVerbose);

        const char *LONG_STRING = "0123456789012345678901234567890123456789"
                                  "0123456789012345678901234567890123456789";

        bsl::string      *sPtr  = new (sfa ) bsl::string(LONG_STRING, &ssa);
        bsl::string_view *svPtr = new (svfa) bsl::string_view(LONG_STRING);

        ASSERT(sfa.numBytesInUse() >= svfa.numBytesInUse());
        ASSERT(0                   <   ssa.numBytesInUse());
        ASSERT(0                   ==   da.numBytesInUse());
//..
// At the same time it supports all most used 'access' operations of the
// 'bsl::string', using the overloads, accepting the same parameters:
//..
        const bsl::string&      STR = *sPtr;
        const bsl::string_view& SV  = *svPtr;

        ASSERT(STR.length()                == SV.length());
        ASSERT(STR.empty()                 == SV.empty());
        ASSERT(STR.front()                 == SV.front());
        ASSERT(STR.at(15)                  == SV.at(15));
        ASSERT(STR.find("345")             == SV.find("345"));
        ASSERT(STR.find_last_not_of("578") == SV.find_last_not_of("578"));
        ASSERT(STR.compare(0, 3, "012")    == SV.compare(0, 3, "012"));
//..
// However, using the 'bsl::string_view', you need to be especially attentive
// to the lifetime of the source character string, since the component
// explicitly refers to it:
//..
        ASSERT(LONG_STRING != STR.data());
        ASSERT(LONG_STRING == SV.data());

        sfa.deleteObject(sPtr);
        svfa.deleteObject(svPtr);
//..
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   The object is trivially copyable, bitwise moveable and should have
        //   appropriate bsl type traits to reflect this.
        //
        // Concerns:
        //: 1 The class has the bsl::is_trivially_copyable trait.
        //:
        //: 2 The class has the bslmf::IsBitwiseMoveable trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type.  (C-1..2)
        //
        // Testing:
        //   TYPE TRAITS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        ASSERT((bsl::is_trivially_copyable<bsl::string_view>::value));
        ASSERT((bslmf::IsBitwiseMoveable<bsl::string_view>::value));

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // CONSTRUCTION FROM 'std::basic_string'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING CONVERSION W.R.T. 'std::basic_string'"
                   "\n============================================\n");

        TestDriver<char>::testCase23();
        TestDriver<wchar_t>::testCase23();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase23();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase23();
        TestDriver<char32_t>::testCase23();
#endif
      } break;
      case 22: {

        if (verbose) printf ("\nTESTING LITERALS"
                             "\n================\n");

        // Due to special initialization of objects, there is no unified test
        // for both template specializations (char and wchar_t).  So test has
        // two sections and both types are tested.  But we need to run it only
        // once and template parameter of the 'TestDriver' is valueless for
        // this test case.

        TestDriver<char>::testCase22();

      } break;
      case 21: {

        if (verbose) printf ("\nTESTING 'STARTS_WITH' AND 'ENDS_WITH'"
                             "\n====================================\n");

#if defined(BSLSTL_STRINGVIEW_ENABLE_CPP20_METHODS)
        TestDriver<char>::testCase21();
        TestDriver<wchar_t>::testCase21();

        TestDriver<char8_t>::testCase21();
        TestDriver<char16_t>::testCase21();
        TestDriver<char32_t>::testCase21();
#else
        if (verbose) printf ("Methods not available prior to C++20\n");
#endif
      } break;
      case 20: {

        if (verbose) printf ("\nTESTING COMPARISON OPERATORS"
                             "\n============================\n");

        TestDriver<char>::testCase20();
        TestDriver<wchar_t>::testCase20();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase20();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase20();
        TestDriver<char32_t>::testCase20();
#endif
      } break;
      case 19: {

        if (verbose) printf ("\nTESTING HASHAPPEND"
                             "\n==================\n");

        TestDriver<char>::testCase19();
        TestDriver<wchar_t>::testCase19();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase19();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase19();
        TestDriver<char32_t>::testCase19();
#endif
      } break;
      case 18: {

        if (verbose) printf ("\nTESTING COMPARE"
                             "\n===============\n");

        TestDriver<char>::testCase18();
        TestDriver<wchar_t>::testCase18();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase18();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase18();
        TestDriver<char32_t>::testCase18();
#endif
      } break;
      case 17: {

        if (verbose) printf ("\nTESTING SUBSTR"
                             "\n==============\n");

        TestDriver<char>::testCase17();
        TestDriver<wchar_t>::testCase17();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase17();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase17();
        TestDriver<char32_t>::testCase17();
#endif
      } break;
      case 16: {

        if (verbose) printf (
                         "\nTESTING FIND_FIRST_NOT_OF AND FIND_LAST_NOT_OF"
                         "\n==============================================\n");

        TestDriver<char>::testCase16();
        TestDriver<wchar_t>::testCase16();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase16();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase16();
        TestDriver<char32_t>::testCase16();
#endif
      } break;
      case 15: {

        if (verbose) printf ("\nTESTING FIND_FIRST_OF AND FIND_LAST_OF"
                             "\n======================================\n");

        TestDriver<char>::testCase15();
        TestDriver<wchar_t>::testCase15();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase15();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase15();
        TestDriver<char32_t>::testCase15();
#endif
      } break;
      case 14: {

        if (verbose) printf ("\nTESTING FIND AND RFIND"
                             "\n======================\n");

        TestDriver<char>::testCase14();
        TestDriver<wchar_t>::testCase14();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase14();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase14();
        TestDriver<char32_t>::testCase14();
#endif
      } break;
      case 13: {

        if (verbose) printf ("\nTESTING COPY"
                             "\n============\n");

        TestDriver<char>::testCase13();
        TestDriver<wchar_t>::testCase13();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase13();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase13();
        TestDriver<char32_t>::testCase13();
#endif
      } break;
      case 12: {

        if (verbose) printf ("\nTESTING CUTTING METHODS"
                             "\n=======================\n");

        TestDriver<char>::testCase12();
        TestDriver<wchar_t>::testCase12();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase12();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase12();
        TestDriver<char32_t>::testCase12();
#endif
      } break;
      case 11: {

        if (verbose) printf ("\nTESTING ITERATOR ACCESSORS"
                             "\n==========================\n");

        TestDriver<char>::testCase11();
        TestDriver<wchar_t>::testCase11();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase11();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase11();
        TestDriver<char32_t>::testCase11();
#endif
      } break;
      case 10: {

        if (verbose) printf ("\nTESTING ELEMENT ACCESSORS"
                             "\n=========================\n");

        TestDriver<char>::testCase10();
        TestDriver<wchar_t>::testCase10();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase10();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase10();
        TestDriver<char32_t>::testCase10();
#endif
      } break;
      case 9: {

        if (verbose) printf ("\nTESTING SIZE ACCESSORS"
                             "\n======================\n");

        TestDriver<char>::testCase9();
        TestDriver<wchar_t>::testCase9();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase9();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase9();
        TestDriver<char32_t>::testCase9();
#endif
      } break;
      case 8: {

        if (verbose) printf ("\nTESTING CONSTRUCTORS"
                             "\n====================\n");

        TestDriver<char>::testCase8();
        TestDriver<wchar_t>::testCase8();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase8();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase8();
        TestDriver<char32_t>::testCase8();
#endif
      } break;
      case 7: {
        if (verbose) printf ("\nTESTING COPY-ASSIGNMENT OPERATOR"
                             "\n================================\n");

        TestDriver<char>::testCase7();
        TestDriver<wchar_t>::testCase7();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase7();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase7();
        TestDriver<char32_t>::testCase7();
#endif
      } break;
      case 6: {

        if (verbose) printf ("\nTESTING SWAP"
                             "\n============\n");

        TestDriver<char>::testCase6();
        TestDriver<wchar_t>::testCase6();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase6();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase6();
        TestDriver<char32_t>::testCase6();
#endif
      } break;
      case 5: {

        if (verbose) printf ("\nTESTING COPY CONSTRUCTOR"
                             "\n========================\n");

        TestDriver<char>::testCase5();
        TestDriver<wchar_t>::testCase5();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase5();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase5();
        TestDriver<char32_t>::testCase5();
#endif
      } break;
      case 4: {

        if (verbose) printf ("\nTESTING EQUALITY COMPARISON"
                             "\n===========================\n");

        TestDriver<char>::testCase4();
        TestDriver<wchar_t>::testCase4();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase4();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase4();
        TestDriver<char32_t>::testCase4();
#endif
      } break;
      case 3: {

        if (verbose) printf ("\nTESTING PRINT OPERATIONS"
                             "\n========================\n");

        TestDriver<char>::testCase3();
        TestDriver<wchar_t>::testCase3();

        // The facets of 'char8_t', 'char16_t' and 'char32_t' are such that
        // they throw on this test.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
//      TestDriver<char8_t>::testCase3();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
//      TestDriver<char16_t>::testCase3();
//      TestDriver<char32_t>::testCase3();
#endif
      } break;
      case 2: {

        if (verbose) printf ("\nPRIMARY MANIPULATORS AND BASIC ACCESSORS"
                             "\n========================================\n");

        TestDriver<char>::testCase2();
        TestDriver<wchar_t>::testCase2();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TestDriver<char8_t>::testCase2();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TestDriver<char16_t>::testCase2();
        TestDriver<char32_t>::testCase2();
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bsl::string_view            mX1("abcdef");
        const bsl::string_view&      X1 = mX1;
        bsl::string_view            mX2("klm");
        const bsl::string_view&      X2 = mX2;
        bsl::string_view::size_type pos;

        pos = X1.find(X2, 3);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find("klm", 3, 2);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find("klm");
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find('k');
        ASSERTV(pos, bsl::string_view::npos == pos);

        pos = X1.rfind(X2, 3);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.rfind("klm", 3, 2);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.rfind("klm");
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.rfind('k');
        ASSERTV(pos, bsl::string_view::npos == pos);

        pos = X1.find_last_of(X2, 3);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_last_of("klm", 3, 2);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_last_of("klm");
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_last_of('k');
        ASSERTV(pos, bsl::string_view::npos == pos);

        pos = X1.find_first_of(X2, 3);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_first_of("klm", 3, 2);
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_first_of("klm");
        ASSERTV(pos, bsl::string_view::npos == pos);
        pos = X1.find_first_of('k');
        ASSERTV(pos, bsl::string_view::npos == pos);

        pos = X1.find_last_not_of(X2, 3);
        ASSERTV(pos, 3 == pos);
        pos = X1.find_last_not_of("klm", 3, 2);
        ASSERTV(pos, 3 == pos);
        pos = X1.find_last_not_of("klm");
        ASSERTV(pos, 5 == pos);
        pos = X1.find_last_not_of('k');
        ASSERTV(pos, 5 == pos);

        pos = X1.find_first_not_of(X2, 3);
        ASSERTV(pos, 3 == pos);
        pos = X1.find_first_not_of("klm", 3, 2);
        ASSERTV(pos, 3 == pos);
        pos = X1.find_first_not_of("klm");
        ASSERTV(pos, 0 == pos);
        pos = X1.find_first_not_of('k');
        ASSERTV(pos, 0 == pos);

        bsl::string_view        mX3 = X1.substr(3,2);
        const bsl::string_view&  X3 = mX3;
        ASSERTV(X3.length() , 2   == X3.length() );
        ASSERTV(X3.size()   , 2   == X3.size()   );
        ASSERTV(*(X3.data()), 'd' == *(X3.data()));
        ASSERTV(X3.at(0)    , 'd' == X3.at(0)    );
        ASSERTV(X3.front()  , 'd' == X3.front()  );
        ASSERTV(X3.back()   , 'e' == X3.back()   );

        ASSERTV(X1.compare(X1 )           , 0 == X1.compare(X1)            );
        ASSERTV(X1.compare(X2)            , 0 >  X1.compare(X2 )           );
        ASSERTV(X1.compare(3, 3, X2)      , 0 >  X1.compare(3, 3, X2 )     );
        ASSERTV(X1.compare(3, 3, X2, 0, 3), 0 >  X1.compare(3, 3, X2, 0, 3));
        ASSERTV(X1.compare("klm")         , 0 >  X1.compare("klm")         );
        ASSERTV(X1.compare(3, 3, "klm", 3), 0 >  X1.compare(3, 3, "klm", 3));
        ASSERTV(X1.compare(3, 3, "klm")   , 0 >  X1.compare(3, 3, "klm")   );


#if defined (BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        // Testing u8string_view

        bsl::u8string_view sv8;
        ASSERT(0 == sv8.size());
        ASSERT((bsl::is_same<char8_t,
                             bsl::u8string_view::value_type>::value));
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        // Testing u16string_view and u32string_view

        bsl::u16string_view sv16;
        bsl::u32string_view sv32;
        ASSERT(0 == sv16.size());
        ASSERT(0 == sv32.size());
        ASSERT((bsl::is_same<char16_t,
                             bsl::u16string_view::value_type>::value));
        ASSERT((bsl::is_same<char32_t,
                             bsl::u32string_view::value_type>::value));
#endif

        // Testing literals.

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)
        {
            using namespace bsl::string_view_literals;
            bsl::string_view sv1 = "123\0abc";
            bsl::string_view sv2 = "123\0abc"_sv;
            ASSERTV(sv1.size(), 3 == sv1.size());
            ASSERTV(sv2.size(), 7 == sv2.size());

            bsl::wstring_view sv3 = L"123\0abc"_sv;
            ASSERTV(sv3.size(), 7 == sv3.size());
        }

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY &&
        // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
