// bslstl_string_test.t.cpp                                           -*-C++-*-
#include <bslstl_string_test.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_string.h>
#include <bslstl_string.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_assert.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_issame.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_macrorepeat.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsltf_stdstatefulallocator.h>

#include <algorithm>    // 'adjacent_find', 'sort'
#include <cstring>      // 'memcmp'
#include <iomanip>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <limits.h>     // 'CHAR_MAX'
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if defined(std)
// This is a workaround for the way test drivers are built in an IDE-friendly
// manner in Visual Studio.  A "normal" test driver built from the command line
// will not have 'std' defined as a macro, and so will not need this
// workaround.  This workaround simply undoes the illusion that namespace 'std'
// is namespace 'bsl', so returning this test driver to its expected view.
#undef std
#endif

using namespace BloombergLP;
using bsls::NameOf;
using bsls::nameOfType;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_string.t.cpp' (cases 1-11, plus the usage example),
// and 'bslstl_string_test.cpp' (cases 12 and higher).
//
//                              Overview
//                              --------
// The object under testing is a container whose interface and contract is
// dictated by the C++ standard.  In particular, the standard mandates "strong"
// exception safety (with full guarantee of rollback) along with throwing
// 'std::length_error' if about to request memory for more than 'max_size()'
// elements.  (Note: 'max_size' depends on the parameterized 'VALUE_TYPE'.)
// The general concerns are compliance, exception safety, and proper
// dispatching (for member function templates such as assign and insert).  In
// addition, it is a value-semantic type whose salient attributes are size and
// value of each element in sequence.  This container is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.  Regarding the allocator template argument, we use
// mostly a 'bsl::allocator' together with a 'bslma::TestAllocator' mechanism,
// but we also verify the C++ standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'clear' methods to be used by the generator functions 'g'
// and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that some
// manipulators must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bslma_testallocator'
// component.  After the mandatory sequence of cases (1--10) for value-semantic
// types (cases 5 and 10 are not implemented, as there is not output or
// streaming below bslstl), we test each individual constructor, manipulator,
// and accessor in subsequent cases.
//
// Regarding the allocation model, we attempt to write a general test driver
// that will work regardless of the allocation strategy chosen (short-string
// optimization, static null string, etc.)  However, some testing is necessary
// to make sure we don't overallocate capacity, and for this, knowledge of the
// internal allocation model of the implementation under test is necessary, of
// course.  This is done by the 'DEFAULT_CAPACITY' and
// 'INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT' constants, as well as the
// 'NUM_ALLOCS' array of constants, which would need to be adjusted if the
// allocation strategy were changed.  Since this version provides access to the
// new capacity computation (in the 'Imp' class method 'computeNewCapacity'),
// we reuse that but abstract it in the test facility 'computeNewCapacity'.
//`
// Abbreviations:
// --------------
// Throughout this test driver, we use
//..
//     C               VALUE_TYPE (template argument, no default)
//     A               ALLOC (template argument, dflt: bsl::allocator<T>)
//     basic_string    basic_string<C,CT,A> if template arguments not specified
//     string<C,CT,A>  basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>
//..
// In the signatures, to keep one-liners, the arguments 'pos', 'pos1', pos'2',
// 'n', 'n1', and 'n2', are always of 'size_type', and 'a' is always of type
// 'const A&'.
//-----------------------------------------------------------------------------
// class string<C,CT,A> (string)
// =============================
// [11] TRAITS
//
// CREATORS:
// [ 2] basic_string(const ALLOC& a = ALLOC());
// [ 7] basic_string(const basic_string& original);
// [ 7] basic_string(const basic_string& original, basicAllocator);
// [ 7] basic_string(MovableRef<basic_string> original);
// [ 7] basic_string(MovableRef<basic_string> original, basicAllocator);
// [12] basic_string(const basic_string& str, pos, a = A());
// [12] basic_string(const basic_string& str, pos, n, a = A());
// [12] basic_string(const CHAR_TYPE *s, a = A());
// [12] basic_string(const CHAR_TYPE *s, size_type n, a = A());
// [12] basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
// [12] template<class Iter> basic_string(Iter first, Iter last, a = A());
// [26] basic_string(const native_std::basic_string<CHAR, TRAITS, A2>&);
// [  ] basic_string(const StringRefData& strRefData, a = A());
// [33] basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
// [ 2] ~basic_string();
//
/// MANIPULATORS:
// [ 9] basic_string& operator=(const basic_string& rhs);
// [ 9] basic_string& operator=(const StringRefData& strRefData);
// [ 9] basic_string& operator=(MovableRef<basic_string> rhs);
// [  ] basic_string& operator=(const CHAR_TYPE *s);
// [  ] basic_string& operator=(CHAR_TYPE c);
// [33] basic_string& operator=(initializer_list<CHAR_TYPE> values);
// [  ] basic_string& operator+=(const basic_string& rhs);
// [  ] basic_string& operator+=(const CHAR_TYPE *s);
// [17] basic_string& operator+=(CHAR_TYPE c);
// [17] basic_string& operator+=(const StringRefData& strRefData);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, CHAR_TYPE c);
// [14] void reserve(size_type n);
// [ 2] void clear();
// [15] reference operator[](size_type pos);
// [15] reference at(size_type pos);
// [15] reference front();
// [15] reference back();
// [13] basic_string& assign(const basic_string& str);
// [13] basic_string& assign(bslmf::MovableRef<basic_string> str);
// [13] basic_string& assign(const basic_string& str, pos, n = npos);
// [13] basic_string& assign(const CHAR_TYPE *s, size_type n);
// [13] basic_string& assign(const CHAR_TYPE *s);
// [13] basic_string& assign(const StringRefData<CHAR_TYPE>& strRef);
// [13] basic_string& assign(size_type n, CHAR_TYPE c);
// [13] template <class Iter> basic_string& assign(Iter first, Iter last);
// [33] basic_string& assign(initializer_list<CHAR_TYPE> values);
// [17] basic_string& append(const basic_string& str);
// [17] basic_string& append(const basic_string& str, pos, n = npos);
// [17] basic_string& append(const CHAR_TYPE *s, size_type n);
// [17] basic_string& append(const CHAR_TYPE *s);
// [17] basic_string& append(size_type n, CHAR_TYPE c);
// [17] template <class Iter> basic_string& append(Iter first, Iter last);
// [33] basic_string& append(initializer_list<CHAR_TYPE> values);
// [ 2] void push_back(CHAR_TYPE c);
// [18] basic_string& insert(size_type pos1, const string& str);
// [18] basic_string& insert(size_type pos1, const string& str, pos2, n=npos);
// [18] basic_string& insert(size_type pos, const CHAR_TYPE *s, n2);
// [18] basic_string& insert(size_type pos, const CHAR_TYPE *s);
// [18] basic_string& insert(size_type pos, size_type n, CHAR_TYPE c);
// [18] iterator insert(const_iterator pos, CHAR_TYPE value);
// [18] iterator insert(const_iterator pos, size_type n, CHAR_TYPE value);
// [18] template <class Iter> iterator insert(const_iterator, Iter, Iter);
// [33] iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
// [19] void pop_back();
// [19] iterator erase(size_type pos = 0, size_type n = npos);
// [19] iterator erase(const_iterator position);
// [19] iterator erase(const_iterator first, const_iterator last);
// [20] basic_string& replace(pos1, n1, const string& str);
// [20] basic_string& replace(pos1, n1, const string& str, pos2, n2=npos);
// [20] basic_string& replace(pos1, n1, const C *s, n2);
// [20] basic_string& replace(pos1, n1, const C *s);
// [20] basic_string& replace(pos1, n1, size_type n2, C c);
// [20] basic_string& replace(const_iterator p, q, const string& str);
// [20] basic_string& replace(const_iterator p, q, const C *s, n2);
// [20] basic_string& replace(const_iterator p, q, const C *s);
// [20] basic_string& replace(const_iterator p, q, size_type n2, C c);
// [20] template <It> basic_string& replace(const_iterator p, q, It f, l);
// [36] CHAR_TYPE *data();
// [21] void swap(basic_string& other);
//
// ACCESSORS:
// [26] operator native_std::basic_string<CHAR, TRAITS, A2>() const;
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [15] const_reference front() const;
// [15] const_reference back() const;
// [  ] size_type length() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [14] size_type capacity() const;
// [14] bool empty() const;
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
// [  ] const_iterator cbegin() const;
// [  ] const_iterator cend() const;
// [  ] const_reverse_iterator crbegin() const;
// [  ] const_reverse_iterator crend() const;
// [ 4] const CHAR_TYPE *c_str() const;
// [ 4] const CHAR_TYPE *data() const;
// [  ] allocator_type get_allocator() const;
// [22] size_type find(const string& str, pos = 0) const;
// [22] size_type find(const C *s, pos, n) const;
// [22] size_type find(const C *s, pos = 0) const;
// [22] size_type find(C c, pos = 0) const;
// [22] size_type rfind(const string& str, pos = 0) const;
// [22] size_type rfind(const C *s, pos, n) const;
// [22] size_type rfind(const C *s, pos = 0) const;
// [22] size_type rfind(C c, pos = 0) const;
// [22] size_type find_first_of(const string& str, pos = 0) const;
// [22] size_type find_first_of(const C *s, pos, n) const;
// [22] size_type find_first_of(const C *s, pos = 0) const;
// [22] size_type find_first_of(C c, pos = 0) const;
// [22] size_type find_last_of(const string& str, pos = 0) const;
// [22] size_type find_last_of(const C *s, pos, n) const;
// [22] size_type find_last_of(const C *s, pos = 0) const;
// [22] size_type find_last_of(C c, pos = 0) const;
// [22] size_type find_first_not_of(const string& str, pos = 0) const;
// [22] size_type find_first_not_of(const C *s, pos, n) const;
// [22] size_type find_first_not_of(const C *s, pos = 0) const;
// [22] size_type find_first_not_of(C c, pos = 0) const;
// [22] size_type find_last_not_of(const string& str, pos = 0) const;
// [22] size_type find_last_not_of(const C *s, pos, n) const;
// [22] size_type find_last_not_of(const C *s, pos = 0) const;
// [22] size_type find_last_not_of(C c, pos = 0) const;
// [23] string substr(pos, n) const;
// [23] size_type copy(char *s, n, pos = 0) const;
// [24] int compare(const string& str) const;
// [24] int compare(pos1, n1, const string& str) const;
// [24] int compare(pos1, n1, const string& str, pos2, n2 = npos) const;
// [24] int compare(const C *s) const;
// [24] int compare(pos1, n1, const C *s) const;
// [24] int compare(pos1, n1, const C *s, n2) const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] bool operator==(const C *, const string<C,CT,A>&);
// [ 6] bool operator==(const string<C,CT,A>&, const C *);
// [ 6] bool operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] bool operator!=(const C *, const string<C,CT,A>&);
// [ 6] bool operator!=(const string<C,CT,A>&, const C *);
// [24] bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<(const C *, const string<C,CT,A>&);
// [24] bool operator<(const string<C,CT,A>&, const C *);
// [24] bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>(const C *, const string<C,CT,A>&);
// [24] bool operator>(const string<C,CT,A>&, const C *);
// [24] bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<=(const C *, const string<C,CT,A>&);
// [24] bool operator<=(const string<C,CT,A>&, const C *);
// [24] bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>=(const C *, const string<C,CT,A>&);
// [24] bool operator>=(const string<C,CT,A>&, const C *);
// [34] string operator ""_s(const char *, size_t);
// [34] wstring operator ""_s(const wchar_t *, size_t);
// [34] string operator ""_S(const char *, size_t);
// [34] wstring operator ""_S(const wchar_t *, size_t);
// [21] void swap(basic_string& lhs, basic_string& rhs);
// [30] int stoi(const string& str, std::size_t *pos = 0, int base = 10);
// [30] int stoi(const wstring& str, std::size_t *pos = 0, int base = 10);
// [30] long stol(const string& str, std::size_t *pos, int base);
// [30] long stol(const wstring& str, std::size_t *pos, int base);
// [30] unsigned long stoul(const string& s, std::size_t *pos, int base);
// [30] unsigned long stoul(const wstring& s, std::size_t *pos, int base);
// [30] long long stoll(const string& str, std::size_t *pos, int base);
// [30] long long stoll(const wstring& str, std::size_t *pos, int base);
// [30] unsigned long long stoull(const string&, std::size_t *, int);
// [30] unsigned long long stoull(const wstring&, std::size_t *, int);
// [31] float stof(const string& str, std::size_t *pos =0);
// [31] float stof(const wstring& str, std::size_t *pos =0);
// [31] double stod(const string& str, std::size_t *pos =0);
// [31] double stod(const wstring& str, std::size_t *pos =0);
// [31] long double stold(const string& str, std::size_t *pos =0);
// [31] long double stold(const wstring& str, std::size_t *pos =0);
// [32] string to_string(int value);
// [32] string to_string(long value);
// [32] string to_string(long long value);
// [32] string to_string(unsigned value);
// [32] string to_string(unsigned long value);
// [32] string to_string(unsigned long long value);
// [32] string to_string(float value);
// [32] string to_string(double value);
// [32] string to_string(long double value);
// [32] wstring to_wstring(int value);
// [32] wstring to_wstring(long value);
// [32] wstring to_wstring(long long value);
// [32] wstring to_wstring(unsigned value);
// [32] wstring to_wstring(unsigned long value);
// [32] wstring to_wstring(unsigned long long value);
// [32] wstring to_wstring(float value);
// [32] wstring to_wstring(double value);
// [32] wstring to_wstring(long double value);
// [  ] basic_ostream& operator<<(basic_ostream& stream, const string& str);
// [  ] basic_istream& operator>>(basic_istream& stream, string& str);
// [29] hashAppend(HASHALG& hashAlg, const basic_string& str);
// [29] hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [35] USAGE EXAMPLE
// [11] CONCERN: The object has the necessary type traits
// [26] 'npos' VALUE
// [25] CONCERN: 'std::length_error' is used properly
// [27] DRQS 16870796
// [ 9] basic_string& operator=(const CHAR_TYPE *s); [NEGATIVE ONLY]
// [36] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int TestDriver:ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] Obj& TestDriver:gg(Obj *object, const char *spec);
// [ 8] Obj TestDriver::g(const char *spec);
// [ 8] Obj TestDriver::g(size_t length, TYPE seed);

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                      COMPILER DEFECT DETECTION MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION >= 1910
# define BSLS_STRING_OPT_HOISTS_THROWING_FUNCTIONS 1
    // MSVC 2017 has an optimizer bug for 64-bit builds that will hoist a
    // potentially throwing function ahead of the check to see whether that
    // function can be safely called without throwing.  This causes exceptions
    // to be thrown that are NOT caught in test case 31, testing the 'strtod'
    // family of functions, and consequently calling 'terminate'.
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS) ||                                   \
   !(defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 800000)
# define BSLS_STRING_DISABLE_S_LITERALS 1
    // The Solaris platform has a function-like '_S' macro that conflicts with
    // the BDE '_S' literal operator on gcc, at least for early versions of the
    // compiler that do not correctly parse '""_S' as a single token, which
    // would avoid the problem of being parsed as a macro.  This is the same
    // trick that allows '""if' for complex floats, but appears to persist into
    // later compiler versions as the interaction with the preprocessor occurs
    // earlier in the parsing.
#endif
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                                             Element;
typedef bsl::basic_string<char, bsl::char_traits<char> > Obj;
typedef bsl::String_Imp<char, size_t>                    Imp;

typedef bslma::TestAllocatorMonitor                      Tam;

// CONSTANTS
const int MAX_ALIGN      = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
const int MAX_ALIGN_MASK = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;

const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
const char VF = 'F';
const char VG = 'G';
const char VH = 'H';
const char VI = 'I';
const char VJ = 'J';
const char VK = 'K';
const char VL = 'L';
    // All test types have character value type.

const int  LARGE_SIZE_VALUE = 2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
    // Declare a large value for insertions into the string.  Note this value
    // will cause multiple resizes during insertion into the string.

const size_t SHORT_STRING_BUFFER_BYTES
                           = (20 + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
    // The size of the short string buffer, according to our implementation (20
    // bytes rounded to the word boundary - 1).  Appending one more than this
    // number of characters to a default object causes a reallocation.

const size_t INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT = 1;
                                // bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    // The capacity of a default constructed object after the first
    // 'push_back', according to our implementation.

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a string of the following size
    // by using 'push_back' repeatedly (without initial reserve):
#if BSLS_PLATFORM_CPU_64_BIT
    // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,

    // 17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4
#else
    // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,

    // 17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3
#endif
};

// Provide expected values for short string buffer capacity.  Note that we
// clearly call out the expected values on Windows and Unix (2/4-btye 'wchar_t'
// representations) as these are the overwhelmingly common cases, and for a
// test driver the clarity of seeing exact numbers is more important than the
// redundancy involved in the manual evaluation of the formula below.

#if defined(BSLS_PLATFORM_CPU_32_BIT)
const size_t k_SHORT_BUFFER_CAPACITY_CHAR    = 19;
const size_t k_SHORT_BUFFER_CAPACITY_WCHAR_T = 2 == sizeof(wchar_t) ? 9
                                             : 4 == sizeof(wchar_t) ? 4
                                             : 20 / sizeof(wchar_t) - 1;
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
const size_t k_SHORT_BUFFER_CAPACITY_CHAR    = 23;
const size_t k_SHORT_BUFFER_CAPACITY_WCHAR_T = 2 == sizeof(wchar_t) ? 11
                                             : 4 == sizeof(wchar_t) ? 5
                                             : 24 / sizeof(wchar_t) - 1;
#else
// Unknown platform configuration is likely to error when trying to use either
// of the two constants not defined in this block.
#endif

template <class CHAR_TYPE>
struct ExpectedShortBufferCapacity;

template <>
struct ExpectedShortBufferCapacity<char>
    : bsl::integral_constant<size_t, k_SHORT_BUFFER_CAPACITY_CHAR> {};

template <>
struct ExpectedShortBufferCapacity<wchar_t>
    : bsl::integral_constant<size_t, k_SHORT_BUFFER_CAPACITY_WCHAR_T> {};

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

template <class TRAITS, class ALLOC>
void debugprint(const bsl::basic_string<wchar_t, TRAITS, ALLOC>& v)
    // Print the contents of the specified string 'v' to 'stdout', then flush.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            printf("%lc", wint_t(v[i]));
        }
    }
    fflush(stdout);
}
}  // close namespace bsl

// Legacy debug print support.
inline
void dbg_print(const char *s) { printf("\"%s\"", s); }
void dbg_print(const wchar_t *s)
{
    putchar('"');
    while (*s) {
        printf("%lc", wint_t(*s));
        ++s;
    }
    putchar('"');
}

// Generic debug print function (3-arguments).
template <class TYPE>
void dbg_print(const char *s, const TYPE& val, const char *nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

// String utilities
inline
size_t max(size_t lhs, size_t rhs)
    // Return the larger of the specified 'lhs' and 'rhs'.
{
    return lhs < rhs ? rhs : lhs;
}

inline
size_t min(size_t lhs, size_t rhs)
    // Return the smaller of the specified 'lhs' and 'rhs'.
{
    return lhs < rhs ? lhs : rhs;
}

size_t computeNewCapacity(size_t newLength,
                          size_t /* initLength */,
                          size_t capacity,
                          size_t maxSize)
    // Compute the expected capacity of a string constructed with the specified
    // 'newLength' and 'capacity' as in by:
    //..
    //  Obj mX(newLength, ' ', allocator);
    //  mX.reserve(capacity);
    //..
    // and later modified to the 'newLength' by inserting characters (either by
    // 'insert' or 'push_back', it should not matter), assignment (using
    // 'assign), or replacement (using 'replace').  We assume that none of the
    // intermediate quantities can overflow the range of 'size_t', although we
    // accept the specified 'maxSize' as a ceiling for the new capacity.  Note
    // that 'newLength' is not necessarily larger than 'initLength'.
{
    // This implementation conforms to the one in the header, and is verified
    // to provide constant amortized time per character for all manipulators
    // (w.r.t. memory allocation).

    if (newLength <= capacity) {
        return capacity;                                              // RETURN
    }

    // adjust capacity the same way as it's done by the string implementation
    capacity += (capacity >> 1);
    if (newLength > capacity) {
        capacity = newLength;
    }

    return capacity > maxSize ? maxSize : capacity;                   // RETURN
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
namespace std {
void debugprint(const std::initializer_list<char>& v)
{
    if (0 == v.size()) {
        printf("<empty>");
    }
    else {
        for (std::initializer_list<char>::iterator itr  = v.begin(),
                                                   end  = v.end();
                                                   end != itr;
                                                   ++itr) {
            printf("'%c'", *itr);
        }
    }
    fflush(stdout);
}

void debugprint(const std::initializer_list<wchar_t>& v)
{
    if (0 == v.size()) {
        printf("<empty>");
    }
    else {
        for (std::initializer_list<wchar_t>::iterator itr  = v.begin(),
                                                      end  = v.end();
                                                      end != itr;
                                                      ++itr) {
            printf("'l%c'", *itr);
        }
    }
    fflush(stdout);
}
}  // close namespace std

#endif

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static bool verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;

static bslma::TestAllocator *globalAllocator_p,
                            *defaultAllocator_p,
                            *objectAllocator_p;

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class VALUE_TYPE>
struct ExceptionGuard {
    // This scoped guard helps to verify the full guarantee of rollback in
    // exception-throwing code.

    // DATA
    int               d_lineNum;
    VALUE_TYPE        d_value;
    const VALUE_TYPE *d_object_p;

  public:
    // CREATORS
    ExceptionGuard(const VALUE_TYPE &object, int line)
    : d_lineNum(line)
    , d_value(object)
    , d_object_p(&object)
    {}

    ~ExceptionGuard() {
        if (d_object_p) {
            const int LINE = d_lineNum;
            LOOP3_ASSERT(LINE, d_value, *d_object_p, d_value == *d_object_p);
        }
    }

    // MANIPULATORS
    void release() {
        d_object_p = 0;
    }

    void resetValue(const VALUE_TYPE& value, int line) {
        d_lineNum = line;
        d_value = value;
    }
};

                          // =======================
                          // class AllocatorUseGuard
                          // =======================

struct AllocatorUseGuard {
    // This scoped guard helps to verify that no allocations or deallocations
    // were triggered for a specific allocator.

    // DATA
    const bslma::TestAllocator *d_allocator_p;
    Int64                       d_numAllocations;
    Int64                       d_numDeallocations;
    Int64                       d_numBytesInUse;

  public:
    // CREATORS
    AllocatorUseGuard(const bslma::TestAllocator *allocatorPtr)
    : d_allocator_p(allocatorPtr)
    , d_numAllocations(allocatorPtr->numAllocations())
    , d_numDeallocations(allocatorPtr->numDeallocations())
    , d_numBytesInUse(allocatorPtr->numBytesInUse())
    {}

    ~AllocatorUseGuard() {
        ASSERT(d_numAllocations == d_allocator_p->numAllocations());
        ASSERT(d_numDeallocations == d_allocator_p->numDeallocations());
        ASSERT(d_numBytesInUse == d_allocator_p->numBytesInUse());
    }
};

                               // ==============
                               // class CharList
                               // ==============

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC = bsl::allocator<TYPE> >
class CharList {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    typedef bsl::basic_string<TYPE,TRAITS,ALLOC> Obj;

    // DATA
    Obj  d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE*> const_iterator;
        // Input iterator.

    // CREATORS
    CharList() {}
    explicit CharList(const Obj& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE, class TRAITS, class ALLOC>
CharList<TYPE,TRAITS,ALLOC>::CharList(const Obj& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE, class TRAITS, class ALLOC>
const TYPE& CharList<TYPE,TRAITS,ALLOC>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharList<TYPE,TRAITS,ALLOC>::const_iterator
CharList<TYPE,TRAITS,ALLOC>::begin() const {
    return const_iterator(&*d_value.begin());
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharList<TYPE,TRAITS,ALLOC>::const_iterator
CharList<TYPE,TRAITS,ALLOC>::end() const {
    return const_iterator(&*d_value.end());
}

                              // ===============
                              // class CharArray
                              // ===============

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC = bsl::allocator<TYPE> >
class CharArray {
    // This array class is a simple wrapper on a string offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // parameterized 'TYPE'.

    typedef bsl::basic_string<TYPE,TRAITS,ALLOC> Obj;

    // DATA
    Obj  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    explicit CharArray(const Obj& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE, class TRAITS, class ALLOC>
CharArray<TYPE,TRAITS,ALLOC>::CharArray(const Obj& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE, class TRAITS, class ALLOC>
const TYPE& CharArray<TYPE,TRAITS,ALLOC>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharArray<TYPE,TRAITS,ALLOC>::const_iterator
CharArray<TYPE,TRAITS,ALLOC>::begin() const {
    return const_iterator(&*d_value.begin());
}

template <class TYPE, class TRAITS, class ALLOC>
typename CharArray<TYPE,TRAITS,ALLOC>::const_iterator
CharArray<TYPE,TRAITS,ALLOC>::end() const {
    return const_iterator(&*d_value.end());
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {

  public:
    // TYPES
    typedef typename ALLOC::value_type        value_type;
    typedef typename ALLOC::pointer           pointer;
    typedef typename ALLOC::const_pointer     const_pointer;
    typedef typename ALLOC::reference         reference;
    typedef typename ALLOC::const_reference   const_reference;
    typedef typename ALLOC::size_type         size_type;
    typedef typename ALLOC::difference_type   difference_type;

    template <class OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be 'ALLOC::rebind<OTHER_TYPE>::other'
        // instead of 'LimitAlloc<X>'.

        typedef LimitAllocator<typename ALLOC::template
                                             rebind<OTHER_TYPE>::other > other;
    };

  private:
    // PRIVATE TYPES
    typedef ALLOC AllocBase;

    // DATA
    size_type d_limit;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1) {}

    explicit LimitAllocator(bslma::Allocator *mechanism)
    : AllocBase(mechanism), d_limit(-1) { }

    explicit LimitAllocator(const ALLOC& rhs)
    : AllocBase((const AllocBase&) rhs), d_limit(-1) { }

    ~LimitAllocator() { }

    // MANIPULATORS
    void setMaxSize(size_type maxSize) { d_limit = maxSize; }

    // ACCESSORS
    size_type max_size() const { return d_limit; }
};

template <class TYPE, class TRAITS, class ALLOC>
inline
bool isNativeString(const bsl::basic_string<TYPE,TRAITS,ALLOC>&)
    { return false; }

template <class TYPE, class TRAITS, class ALLOC>
inline
bool isNativeString(const native_std::basic_string<TYPE,TRAITS,ALLOC>&)
    { return true; }

namespace BloombergLP {
namespace bslma {
// Specialize trait to clarify for bde_verify that 'LimitAllocator' does not
// require satisfy the 'UsesBslmaAllocator' trait.
template <class ALLOC>
struct UsesBslmaAllocator<LimitAllocator<ALLOC> > : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE>,
          class ALLOC  = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..E] correspond to arbitrary (but unique) 'char'
    // values to be appended to the 'string' object.  A tilde ('~') indicates
    // that the logical (but not necessarily physical) state of the object is
    // to be set to its initial, empty state (via the 'clear' method).
    //..
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'H'
    //                                      // unique but otherwise arbitrary
    // <CLEAR>      ::= '~'
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Append the value corresponding to A.
    // "AA"         Append two values both corresponding to A.
    // "ABC"        Append three values corresponding to A, B and C.
    // "ABC~"       Append three values corresponding to A, B and C and then
    //              remove all the elements (set array length to 0).  Note that
    //              this spec yields an object that is logically equivalent
    //              (but not necessarily identical internally) to one
    //              yielded by ("").
    // "ABC~DE"     Append three values corresponding to A, B, and C; empty
    //              the object; and append values corresponding to D and E.
    //-------------------------------------------------------------------------
    //..

    // CONSTANTS
    enum {
        DEFAULT_CAPACITY = SHORT_STRING_BUFFER_BYTES / sizeof(TYPE) > 0
                                ? SHORT_STRING_BUFFER_BYTES / sizeof(TYPE) - 1
                                : 0
    };

    // TYPES
    typedef bsl::basic_string<TYPE, TRAITS, ALLOC> Obj;
        // Type under testing.

    typedef typename Obj::allocator_type           Allocator;
        // And its allocator (may be different from 'ALLOC' via rebind).

    typedef ALLOC AllocType;
        // Utility typedef for xlC10 silliness.

    typedef typename Obj::iterator                 iterator;
    typedef typename Obj::const_iterator           const_iterator;
    typedef typename Obj::reverse_iterator         reverse_iterator;
    typedef typename Obj::const_reverse_iterator   const_reverse_iterator;

    typedef bslmf::MovableRefUtil                  MoveUtil;

    struct InputIterator {
        // Input iterator type to test functions that take input iterators.

        const TYPE *d_data;

        typedef bsl::input_iterator_tag  iterator_category;
        typedef TYPE                     value_type;
        typedef bsls::Types::IntPtr      difference_type;
        typedef TYPE                    *pointer;
        typedef TYPE&                    reference;

        explicit InputIterator(const TYPE *initVal)
        : d_data(initVal)
            // Create an input iterator pointing to the specified '*initVal'
            // (or after 'initVal[-1]').
        {}

        // MANIPULATORS
        InputIterator& operator++()
            // Increment this iterator and return the new value.
        {
            ++d_data;

            return *this;
        }

        // ACCESSORS
        const TYPE& operator*() const
            // Dereference this iterator and return a reference to the object
            // it refers to.
        {
            return *d_data;
        }

        bool operator==(const InputIterator& rhs) const
            // Return 'true' if this iterator is equal to the specified 'rhs'
            // and 'false' otherwise.
        {
            return d_data == rhs.d_data;
        }

        bool operator!=(const InputIterator& rhs) const
            // Return 'true' if this iterator is not equal to the specified
            // 'rhs' and 'false' otherwise.
        {
            return d_data != rhs.d_data;
        }
    };

    // TEST APPARATUS
    static int getValues(const TYPE **values);
        // Load the specified 'values' with the address of an array containing
        // initialized values of the parameterized 'TYPE' and return the length
        // of that array.

    static int ggg(Obj *object, const char *spec, int verboseFlag = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a zero
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static Obj g(size_t length, TYPE seed);
        // Return, by value, a new string object with the specified 'length'
        // and the specified 'seed' character.  The actual content of the
        // string is not important, only the string length and the fact that
        // two different 'seeds' produce two different results.

    static void stretch(Obj *object, size_t size, const TYPE& value = TYPE());
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified 'size' by adding copies of the optionally
        // specified 'value', or with the null character for the (template
        // parameter) 'TYPE' if 'value' is not specified.  The resulting value
        // is not specified.

    static void stretchRemoveAll(Obj         *object,
                                 size_t       size,
                                 const TYPE&  value = TYPE());
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' to (at least) the specified 'size' by adding
        // copies of the optionally specified 'value' or with the null
        // character for the (template parameter) 'TYPE' if 'value' is not
        // specified; then remove all elements leaving 'object' empty.

    static void checkCompare(const Obj& X, const Obj& Y, int result);
        // Compare the specified 'X' and 'Y' strings according to the
        // specifications, and check that the specified 'result' agrees.

    // TEST CASES
    static void testCase36();
        // Test 'data' manipulator.

    static void testCase35();
        // Test 'noexcept' specifications.

    static void testCase33();
        // Test the 'initializer_list' functions where supported.

    static void testCase29();
        // Test the hash append specialization.

    static void testCase28();
        // Test the short string optimization.

    static void testCase26();
        // Test conversions to/from native strings.

    static void testCase25();
        // Test proper use of 'std::length_error'.

    static void testCase24();
        // Test comparison free operators.

    static void testCase24Negative();
        // Negative test 'compare' and comparison free operators.

    static void testCase23();
        // Test 'copy' and 'substr' methods.

    static void testCase23Negative();
        // Negative test for 'copy'.

    static void testCase22();
        // Test 'find...' methods.

    static void testCase22Negative();
        // Negative test for 'find...' methods.

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase21_propagate_on_container_swap_dispatch();
    static void testCase21_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase21();
        // Test 'swap' member.

    static void testCase20();
        // Test 'replace'.

    static void testCase20MatchTypes();
        // Test 'replace' where the types of 'NUM_ELEMENTS' and 'VALUE' match.

    template <class CONTAINER>
    static void testCase20Range(const CONTAINER&);
        // Test 'replace' member template.

    static void testCase20Negative();
        // Negative test for 'replace'.

    static void testCase19();
        // Test 'erase' and 'pop_back'.

    static void testCase19Negative();
        // Negative test for 'erase' and 'pop_back'.

    static void testCase18();
        // Test 'insert' and move 'push_back'.

    template <class CONTAINER>
    static void testCase18Range(const CONTAINER&);
        // Test 'insert' member template.

    static void testCase18Negative();
        // Negative test for 'insert'.

    static void testCase17();
        // Test 'append'.

    template <class CONTAINER>
    static void testCase17Range(const CONTAINER&);
        // Test 'append' member template.

    static void testCase17Negative();
        // Negative test for 'append'.

    static void testCase16();
        // Test iterators.

    static void testCase15();
        // Test element access.

    static void testCase15Negative();
        // Negative test for element access.

    static void testCase14();
        // Test 'reserve' and capacity-related methods.

    static void testCase13();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase13Range(const CONTAINER&);
        // Test 'assign' member template.

    static void testCase13StrRefData();
        // Test 'assign' member template from 'StrRefData' type.

    static void testCase13InputIterator();
        // Test 'assign' member template from a pair of input iterators.

    static void testCase13Negative();
        // Negative test for 'assign' members.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase12Negative();
        // Negative test for user-supplied constructors.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testCaseM1(const int NITER, const int RANDOM_SEED);
        // Performance regression test, performing the specified 'NITER'
        // iterations, and using the specified 'RANDOM_SEED' to vary the test
        // conditions.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::checkCompare(const Obj& X,
                                                 const Obj& Y,
                                                 int        result)
{
    // As per C++ standard, chapter 21, clause 21.3.7.9.

    typename Obj::size_type rlen = min(X.length(), Y.length());
    int ret = TRAITS::compare(X.data(), Y.data(), rlen);
    if (ret) {
        ASSERT(ret == result);
        return;                                                       // RETURN
    }
    if (X.size() > Y.size()) {
        ASSERT(result > 0);
        return;                                                       // RETURN
    }
    if (X.size() < Y.size()) {
        ASSERT(result < 0);
        return;                                                       // RETURN
    }
    if (X.size() == Y.size()) {
        ASSERT(result == 0);
        return;                                                       // RETURN
    }
    ASSERT(0);
}

template <class TYPE, class TRAITS, class ALLOC>
int TestDriver<TYPE,TRAITS,ALLOC>::getValues(const TYPE **values)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    const int NUM_VALUES = 12;
    static const TYPE initValues[NUM_VALUES + 1] = {
        TYPE(VA),
        TYPE(VB),
        TYPE(VC),
        TYPE(VD),
        TYPE(VE),
        TYPE(VF),
        TYPE(VG),
        TYPE(VH),
        TYPE(VI),
        TYPE(VJ),
        TYPE(VK),
        TYPE(VL),
        0
    };

    *values = initValues;
    return NUM_VALUES;
}

template <class TYPE, class TRAITS, class ALLOC>
int TestDriver<TYPE,TRAITS,ALLOC>::ggg(Obj        *object,
                                       const char *spec,
                                       int         verboseFlag)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'L') {
            object->push_back(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->clear();
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.           // RETURN
        }
   }
   return SUCCESS;
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>& TestDriver<TYPE,TRAITS,ALLOC>::gg(
                                                            Obj        *object,
                                                            const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>
TestDriver<TYPE,TRAITS,ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class TYPE, class TRAITS, class ALLOC>
bsl::basic_string<TYPE,TRAITS,ALLOC>
TestDriver<TYPE,TRAITS,ALLOC>::g(size_t length, TYPE seed)
{
    Obj object(length, TYPE());

    for (size_t i = 0; i < length; ++i) {
        object[i] = TYPE(i + seed);
    }

    return object;
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::stretch(Obj         *object,
                                            size_t       size,
                                            const TYPE&  value)
{
    ASSERT(object);
    for (size_t i = 0; i < size; ++i) {
        object->push_back(value);
    }
    ASSERT(object->size() >= size);
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::stretchRemoveAll(Obj         *object,
                                                     size_t       size,
                                                     const TYPE&  value)
{
    ASSERT(object);
    stretch(object, size, value);
    object->clear();
    ASSERT(0 == object->size());
}

                                // ----------
                                // TEST CASES
                                // ----------
template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE, TRAITS, ALLOC>::testCase36()
{
    // ------------------------------------------------------------------------
    // TESTING 'data' MANIPULATOR
    //
    // Concerns:
    //: 1 The 'data()' method returns the address of a buffer containing all
    //:   the characters of this string object.
    //:
    //: 2 The 'data()' method returns the address of a buffer containing the
    //:   null character at the last (equal to the length of the object)
    //:   position, even for an empty object.
    //:
    //: 3 Changing the buffer contents (via the returned address) changes the
    //:   value of the string object.
    //:
    //: 4 The method works for string lengths that are less than and greater
    //:   than the small string optimization limit.
    //:
    //: 5 QoI: The address returned by this 'data()' method is the same as the
    //:   address returned by the 'const'-qualified 'data()' method.
    //
    // Plan:
    //: 1 Specify a set S of representative object values ordered by increasing
    //:   length.  For each value w in S, initialize a newly constructed object
    //:   x with w.
    //:
    //: 2 For each x from P-1:
    //:
    //:   1 Call 'data()' method and iterate through the obtained buffer and
    //:     compare its value with the symbols, returned by the 'operator[]'.
    //:     (C-1)
    //:
    //:   2 Modify each buffer's character and verify, that object's value is
    //:     modified accordingly.  (C-3)
    //:
    //:   3 Verify, that the null character encloses the character sequence.
    //:     (C-2)
    //:
    //:   4 Compare the address returned by 'data()' method with the result of
    //:     the 'const'-qualified 'data()' method invocation.  (C-5)
    //:
    //: 3 Construct long string and repeat the steps from P-2.  (C-4)
    //
    // Testing:
    //   CHAR_TYPE *data();
    // ------------------------------------------------------------------------

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // specification string
    } DATA[] = {
        //line  spec
        //----  -----------------------------------
        { L_,   ""                                 },
        { L_,   "A"                                },
        { L_,   "B"                                },
        { L_,   "AB"                               },
        { L_,   "BC"                               },
        { L_,   "BCA"                              },
        { L_,   "CAB"                              },
        { L_,   "CDAB"                             },
        { L_,   "DABC"                             },
        { L_,   "ABCDE"                            },
        { L_,   "EDCBA"                            },
        { L_,   "ABCDEA"                           },
        { L_,   "ABCDEAB"                          },
        { L_,   "BACDEABC"                         },
        { L_,   "CBADEABCD"                        },
        { L_,   "CBADEABCDAB"                      },
        { L_,   "CBADEABCDABC"                     },
        { L_,   "CBADEABCDABCDE"                   },
        { L_,   "CBADEABCDABCDEA"                  },
        { L_,   "CBADEABCDABCDEAB"                 },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA"  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB" }
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

        Obj        mX;
        const Obj& X = gg(&mX, SPEC);


        TYPE         *dataPtr = mX.data();
        const size_t  LENGTH  = X.length();

        for (size_t i = 0; i < LENGTH; ++i) {
            LOOP3_ASSERT(LINE, X[i], *(dataPtr + i), X[i] == *(dataPtr + i));

            *(dataPtr + i) = 'Z';
            LOOP2_ASSERT(LINE, X[i], 'Z' == X[i]);
        }

        LOOP_ASSERT(LINE, 0 == *(dataPtr + LENGTH));

        LOOP_ASSERT(LINE, X.data() == mX.data());
    }

    // Testing a long string whose length exceeds the small string optimization
    // limit.

    {
        const char   *longStr = "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ";
        Obj           mX;
        const Obj&    X       = gg(&mX, longStr);
        const size_t  LENGTH  = X.length();
        TYPE         *dataPtr = mX.data();

        ASSERT(DEFAULT_CAPACITY < LENGTH);

        for (size_t i = 0; i < LENGTH; ++i) {
            LOOP3_ASSERT(i, X[i], *(dataPtr + i), X[i] == *(dataPtr + i));

            *(dataPtr + i) = 'Z';
            LOOP2_ASSERT(i, X[i], 'Z' == X[i]);
        }

        LOOP_ASSERT(*(dataPtr + LENGTH), 0 == *(dataPtr + LENGTH));

        ASSERT(X.data() == mX.data());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE, TRAITS, ALLOC>::testCase35()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the unary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualified 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<TYPE>())
        P(bsls::NameOf<TRAITS>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 699: String classes

    // page 704
    //..
    //  // 21.3.1.2, construct/copy/destroy:
    //  basic_string() noexcept(noexcept(Allocator())) :
    //                      basic_string(Allocator()) { }
    //  explicit basic_string(const Allocator& a) noexcept;
    //  basic_string(basic_string&& str) noexcept;
    //  basic_string& operator=(basic_string&& str) noexcept(
    //      allocator_traits<Allocator>::
    //                         propagate_on_container_move_assignment::value ||
    //      allocator_traits<Allocator>::is_always_equal::value);
    //..

    {
        // Not implemented: 'basic_string()'

        ALLOC a;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(Obj(a)));

        Obj str;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(Obj(MoveUtil::move(str))));

        Obj s;
        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s = MoveUtil::move(str)));
    }

    // page 704
    //..
    //  // 21.3.1.3, iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;  (void)S;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.rbegin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.rend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.cend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.crend()));
    }

    // page 704 - 705
    //..
    //  // 21.3.1.4, capacity:
    //  size_type size() const noexcept;
    //  size_type length() const noexcept;
    //  size_type max_size() const noexcept;
    //  size_type capacity() const noexcept;
    //  void clear() noexcept;
    //  bool empty() const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;  (void)S;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.length()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.max_size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.capacity()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.clear()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.empty()));
    }

    // page 705 - 706
    //..
    //  // 21.3.1.6, modifiers:
    //  basic_string& assign(basic_string&& str) noexcept(
    //      allocator_traits<Allocator>::
    //                         propagate_on_container_move_assignment::value ||
    //      allocator_traits<Allocator>::is_always_equal::value);
    //  void swap(basic_string& str) noexcept(
    //       allocator_traits<Allocator>::propagate_on_container_swap::value ||
    //       allocator_traits<Allocator>::is_always_equal::value);
    //..

    // page 706 - 707
    //..
    //  // 21.3.1.7, string operations:
    //  const charT* c_str() const noexcept;
    //  const charT* data() const noexcept;
    //  charT* data() noexcept;
    //  allocator_type get_allocator() const noexcept;
    //
    //  size_type find (const basic_string& str,
    //                  size_type pos = 0) const noexcept;
    //  size_type rfind(const basic_string& str,
    //                  size_type pos = npos) const noexcept;
    //  size_type find_first_of    (const basic_string& str,
    //                              size_type pos = 0) const noexcept;
    //  size_type find_last_of     (const basic_string& str,
    //                              size_type pos = npos) const noexcept;
    //  size_type find_first_not_of(const basic_string& str,
    //                              size_type pos = 0) const noexcept;
    //  size_type find_last_not_of (const basic_string& str,
    //                              size_type pos = npos) const noexcept;
    //  int compare(const basic_string& str) const noexcept;
    //..

    {
        Obj s;  const Obj& S = s;  (void)S;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.c_str()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.data()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.data()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.get_allocator()));

        Obj    str;
        size_t pos(0);  (void)pos;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.find             (str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.rfind            (str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.find_first_of    (str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.find_last_of     (str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.find_first_not_of(str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.find_last_not_of (str, pos)));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.compare(str)));
    }

    // page 725
    //..
    //  21.3.2.2 operator== [string::operator==]
    //  template<class charT, class traits, class Allocator>
    //  bool operator==(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.3 operator!= [string::op!=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator!=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.4 operator< [string::op<]
    //  template<class charT, class traits, class Allocator>
    //  bool operator< (const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.5 operator> [string::op>]
    //  template<class charT, class traits, class Allocator>
    //  bool operator> (const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.6 operator<= [string::op<=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator<=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.7 operator>= [string::op>=]
    //  template<class charT, class traits, class Allocator>
    //  bool operator>=(const basic_string<charT,traits,Allocator>& lhs,
    //                  const basic_string<charT,traits,Allocator>& rhs)
    //                                                                noexcept;
    //  21.3.2.8 swap [string.special]
    //  template<class charT, class traits, class Allocator>
    //  void swap(basic_string<charT,traits,Allocator>& lhs,
    //            basic_string<charT,traits,Allocator>& rhs)
    //      noexcept(noexcept(lhs.swap(rhs)));

    {
        Obj lhs;
        Obj rhs;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs == rhs));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs != rhs));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs <  rhs));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs >  rhs));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs <= rhs));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs >= rhs));

        using bsl::swap;
        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(lhs, rhs)));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase33()
{
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    // --------------------------------------------------------------------
    // TESTING 'initializer_list' FUNCTIONS
    //   Throughout this test, there are three categories of string lengths
    //   to be considered:
    //
    //: o Zero length
    //: o A length below the maximum length for short string optimization
    //: o A length above the short-string optimization limit (i.e., a string
    //:   that requires allocated memory)
    //
    // Concerns:
    //: 1 Construction
    //:
    //:   1 The created string has the specified value and uses the specified
    //:     allocator.
    //:
    //:   2 There is no allocation when the initial string is less than or
    //:     equal to the short-string optimization limit.
    //:
    //:   3 The created strings can be of any of the three length categories
    //:
    //:   4 The constructor is exception-safe.
    //:
    //: 2 Assignment
    //:
    //:   1 The 'assign' and 'operator=' methods behave identically.
    //:
    //:   2 The 'lhs' value is set to the 'rhs' value.
    //:
    //:   3 The 'lhs' and 'rhs' can each be in any of the three string length
    //:     categories.
    //:
    //:   4 The assignment methods are exception-safe.
    //:
    //: 3 Insertion
    //:
    //:   1 The insertion point can be at the beginning, middle, or end of the
    //:     target string.
    //:
    //:   2 The insertion argument must be 'const'.
    //:
    //:   3 The target string and source initializer list can be of any of the
    //:     three string length categories.
    //:
    //:   4 The target string is set to the expected value.
    //:
    //:   5 The returned iterator has the expected value.
    //:
    //:   6 The returned iterator provided modifiable access to the string.
    //:
    //:   7 The insertion method is exception-safe.
    //:
    //: 4 Appending
    //:
    //:   1 The initializer list elements are inserted at the end of the target
    //:     string.
    //:
    //:   2 The target string and source initializer list can be of any of the
    //:     three string length categories.
    //:
    //:   3 The target string is set to the expected value.
    //:
    //:   4 The reference returned is to the target object (i.e., '*this').
    //:
    //:   5 The reference returned provides modifiable access to the string.
    //:
    //:   6 The 'append' method is exception-safe.
    //
    // Plan:
    //: 1 Construction (C-1.1..4)
    //:
    //:   1 For an initializer list at the boundaries between the three string
    //:     length categories, create a test object (string) that uses a test
    //:     allocator.
    //:
    //:   2 Confirm that the created object has the expected value.
    //:
    //:   3 Confirm that the object's expected allocator is reported the
    //:     accessor and shows allocation when expected and the default
    //:     allocator shows no use.
    //:
    //:   4 Wrap the construction with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that each allocation experiences an injected exception at
    //:     some point.
    //:
    //:   5 Repeat P-1.1 to P-1.4 for an object created to use the currently
    //:     installed default constructor and tests adjusted to expect use of
    //:     the default constructor.
    //:
    //: 2 Assignment (C-2.1..4)
    //:
    //:   1 Using a table-drive approach, create a lhs object (a string) and a
    //:     rhs object (an initializer list) so that we at least one data point
    //:     in the cross product of the boundaries between the three string
    //:     length categories.
    //:
    //:   2 Confirm that the 'lhs' is set to the expected value.
    //:
    //:   3 Confirm that the 'lhs' allocator shows use when expected.
    //:
    //:   4 Wrap the assignments with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that each allocation experiences an injected exception at
    //:     some point.
    //:
    //:   5 For each step P-2.1 to P-2.4, shadow each use of the 'assign'
    //:     method with an analogous use of 'operator=' on an equivalent 'lhs'
    //:     object.
    //:
    //: 3 Insertion (C-3.1..7)
    //:
    //:   1 Using a table-drive approach, create a target object (a string) and
    //:     a source object (an initializer list) so that we have at least one
    //:     data point in the cross product of the boundaries between the three
    //:     string length categories.  The table also defines insertion points
    //:     corresponding to the beginning, middle, and end of the target
    //:     string.
    //:
    //:   2 Supply the position argument to the 'insert' method as a
    //:     'const_iterator' object.
    //:
    //:   3 Confirm that the target string is set to the expected value.
    //:
    //:   4 Confirm that the allocator of the target string shows use when
    //:     expected.
    //:
    //:   5 Confirm that the return value can be assigned to a (non-'const')
    //:     'iterator' object.
    //:
    //:   6 Wrap the insertions with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that each allocation experiences an injected exception at
    //:     some point.
    //:
    //: 4 Appending (C-4.1..6)
    //:
    //:   1 Using a table-drive approach, create a target object (a string) and
    //:     a source object (an initializer list) so that we have at least one
    //:     data point in the cross product of the boundaries between the three
    //:     string length categories.
    //:
    //:   2 Call 'append' and confirm that the target string is set to the
    //:     expected value.
    //:
    //:   3 Confirm that the allocator of the target string shows use when
    //:     expected.
    //:
    //:   4 Confirm that the return value has the expected type and value.
    //:
    //:   5 Wrap the append call with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
    //:     macros so that each allocation experiences an injected exception at
    //:     some point.
    //
    // Testing:
    //   basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
    //   basic_string& operator=(initializer_list<CHAR_TYPE> values);
    //   basic_string& assign(initializer_list<CHAR_TYPE> values);
    //   basic_string& append(initializer_list<CHAR_TYPE> values);
    //   iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
    // --------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting parameters: TYPE = %s.\n", NameOf<TYPE>().name());
        P(sizeof(TYPE))
    }

    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    // Define values that require allocations.

    typedef std::initializer_list<TYPE> IList;

#define LET_A0
#define LET_A1  'A'
#define LET_A2  'A', 'A'
#define LET_A3  'A', 'A', 'A'
#define LET_A4  'A', 'A', 'A', 'A'
#define LET_A5  'A', 'A', 'A', 'A', 'A'
#define LET_A6  'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A7  'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A8  'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A9  'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'

#define LET_A10 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'
#define LET_A20 LET_A10, LET_A10
#define LET_A30 LET_A10, LET_A10, LET_A10
#define LET_A40 LET_A10, LET_A10, LET_A10, LET_A10
#define LET_A50 LET_A10, LET_A10, LET_A10, LET_A10, LET_A10

    const IList iListBySize[] = {
        {          LET_A0 }
      , {          LET_A1 }
      , {          LET_A2 }
      , {          LET_A3 }
      , {          LET_A4 }
      , {          LET_A5 }
      , {          LET_A6 }
      , {          LET_A7 }
      , {          LET_A8 }
      , {          LET_A9 }

      , { LET_A10, LET_A0 }
      , { LET_A10, LET_A1 }
      , { LET_A10, LET_A2 }
      , { LET_A10, LET_A3 }
      , { LET_A10, LET_A4 }
      , { LET_A10, LET_A5 }
      , { LET_A10, LET_A6 }
      , { LET_A10, LET_A7 }
      , { LET_A10, LET_A8 }
      , { LET_A10, LET_A9 }

      , { LET_A20, LET_A0 }
      , { LET_A20, LET_A1 }
      , { LET_A20, LET_A2 }
      , { LET_A20, LET_A3 }
      , { LET_A20, LET_A4 }
      , { LET_A20, LET_A5 }
      , { LET_A20, LET_A6 }
      , { LET_A20, LET_A7 }
      , { LET_A20, LET_A8 }
      , { LET_A20, LET_A9 }

      , { LET_A30, LET_A0 }
      , { LET_A30, LET_A1 }
      , { LET_A30, LET_A2 }
      , { LET_A30, LET_A3 }
      , { LET_A30, LET_A4 }
      , { LET_A30, LET_A5 }
      , { LET_A30, LET_A6 }
      , { LET_A30, LET_A7 }
      , { LET_A30, LET_A8 }
      , { LET_A30, LET_A9 }

      , { LET_A40, LET_A0 }
      , { LET_A40, LET_A1 }
      , { LET_A40, LET_A2 }
      , { LET_A40, LET_A3 }
      , { LET_A40, LET_A4 }
      , { LET_A40, LET_A5 }
      , { LET_A40, LET_A6 }
      , { LET_A40, LET_A7 }
      , { LET_A40, LET_A8 }
      , { LET_A40, LET_A9 }

      , { LET_A50, LET_A0 }
    };
    const size_t NUM_iListBySize = sizeof iListBySize / sizeof *iListBySize;

    // Check table correctness

    for (size_t i = 0; i < NUM_iListBySize; ++i) {
        IList LIST = iListBySize[i];
        LOOP_ASSERT(i, i == LIST.size());
        for (typename IList::const_iterator cur  = LIST.begin(),
                                            end  = LIST.end();
                                            end != cur;
                                            ++cur) {
            LOOP_ASSERT(i, 'A' == *cur);
        }
    }

#define STR_A0  ""
#define STR_A1  "A"
#define STR_A2  "A"  "A"
#define STR_A3  "A"  "A"  "A"
#define STR_A4  "A"  "A"  "A"  "A"
#define STR_A5  "A"  "A"  "A"  "A"  "A"
#define STR_A6  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A7  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A8  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A9  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"

#define STR_A10 "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"  "A"
#define STR_A20 STR_A10  STR_A10
#define STR_A30 STR_A10  STR_A10  STR_A10
#define STR_A40 STR_A10  STR_A10  STR_A10  STR_A10
#define STR_A50 STR_A10  STR_A10  STR_A10  STR_A10  STR_A10

    const char *iSpecBySize[] = {
                   STR_A0
      ,            STR_A1
      ,            STR_A2
      ,            STR_A3
      ,            STR_A4
      ,            STR_A5
      ,            STR_A6
      ,            STR_A7
      ,            STR_A8
      ,            STR_A9

      ,   STR_A10  STR_A0
      ,   STR_A10  STR_A1
      ,   STR_A10  STR_A2
      ,   STR_A10  STR_A3
      ,   STR_A10  STR_A4
      ,   STR_A10  STR_A5
      ,   STR_A10  STR_A6
      ,   STR_A10  STR_A7
      ,   STR_A10  STR_A8
      ,   STR_A10  STR_A9

      ,   STR_A20  STR_A0
      ,   STR_A20  STR_A1
      ,   STR_A20  STR_A2
      ,   STR_A20  STR_A3
      ,   STR_A20  STR_A4
      ,   STR_A20  STR_A5
      ,   STR_A20  STR_A6
      ,   STR_A20  STR_A7
      ,   STR_A20  STR_A8
      ,   STR_A20  STR_A9

      ,   STR_A30  STR_A0
      ,   STR_A30  STR_A1
      ,   STR_A30  STR_A2
      ,   STR_A30  STR_A3
      ,   STR_A30  STR_A4
      ,   STR_A30  STR_A5
      ,   STR_A30  STR_A6
      ,   STR_A30  STR_A7
      ,   STR_A30  STR_A8
      ,   STR_A30  STR_A9

      ,   STR_A40  STR_A0
      ,   STR_A40  STR_A1
      ,   STR_A40  STR_A2
      ,   STR_A40  STR_A3
      ,   STR_A40  STR_A4
      ,   STR_A40  STR_A5
      ,   STR_A40  STR_A6
      ,   STR_A40  STR_A7
      ,   STR_A40  STR_A8
      ,   STR_A40  STR_A9

      ,   STR_A50  STR_A0
    };
    const size_t NUM_iSpecBySize = sizeof iSpecBySize / sizeof *iSpecBySize;

    // Check table correctness

    ASSERT(NUM_iListBySize == NUM_iSpecBySize);

    for (size_t i = 0; i < NUM_iSpecBySize; ++i) {
        const char * const SPEC = iSpecBySize[i];

        LOOP_ASSERT(i, i == strlen(SPEC));

        for (const char *       cur  = SPEC,
                        * const end  = SPEC + i;
                                end != cur;
                                ++cur) {
            LOOP_ASSERT(i, 'A' == *cur);
        }
    }

    const size_t k_SHORT_BUFFER_CAPACITY = ExpectedShortBufferCapacity<TYPE>
                                                                       ::value;

    const size_t     maxShortStrLen   = k_SHORT_BUFFER_CAPACITY;
    const size_t exceedsShortStrLen   = maxShortStrLen + 1;

    ASSERT(2 * exceedsShortStrLen < NUM_iListBySize);

    IList                        emptyList = iListBySize[0];
    const char * const           emptySpec = iSpecBySize[0];

    IList                  maxShortStrList = iListBySize[    maxShortStrLen];
    const char * const     maxShortStrSpec = iSpecBySize[    maxShortStrLen];

    IList              exceedsShortStrList = iListBySize[exceedsShortStrLen];
    const char * const exceedsShortStrSpec = iSpecBySize[exceedsShortStrLen];

    if (verbose) printf(
  "\nTesting constructor: initializer lists and specified object allocator\n");
    {
        const struct {
            int                         d_line;       // source line number
            std::initializer_list<TYPE> d_list;       // source list
            const char                 *d_specResult; // expected result
        } DATA[] = {
                //line  list                   result
                //----  -------------------    -------------------
                { L_,     {               },                    "" },
                { L_,     { 'A'           },                   "A" },
                { L_,     { 'A', 'A'      },                  "AA" },
                { L_,     { 'B', 'A'      },                  "BA" },
                { L_,     { 'A', 'B', 'C' },                 "ABC" },
                { L_,     { 'A', 'B', 'A' },                 "ABA" },

                { L_,             emptyList,             emptySpec },
                { L_,       maxShortStrList,       maxShortStrSpec },
                { L_,   exceedsShortStrList,   exceedsShortStrSpec }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE) P_(LIST) P(SPEC_RESULT) }

            if (veryVeryVerbose) { P_(Obj().capacity()) P(LIST.size()) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            Tam oam(objectAllocator_p);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Obj mX(LIST, objectAllocator_p); const Obj& X = mX;

            ASSERT(Y                 == X);
            ASSERT(objectAllocator_p == X.get_allocator());
            ASSERT(&scratch          == Y.get_allocator());

            if (Obj().capacity() < LIST.size()) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }

        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf(
           "\nTesting constructor: initializer lists and default allocator\n");
    {
        const struct {
            int                         d_line;       // source line number
            std::initializer_list<TYPE> d_list;       // source list
            const char                 *d_specResult; // expected result
        } DATA[] = {
                //line  list                  result
                //----  -------------------   -------------------
                { L_,     {               },                   "" },
                { L_,     { 'A'           },                  "A" },
                { L_,     { 'A', 'A'      },                 "AA" },
                { L_,     { 'B', 'A'      },                 "BA" },
                { L_,     { 'A', 'B', 'C' },                "ABC" },
                { L_,     { 'A', 'B', 'A' },                "ABA" },

                { L_,             emptyList,            emptySpec },
                { L_,       maxShortStrList,      maxShortStrSpec },
                { L_,   exceedsShortStrList,  exceedsShortStrSpec }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE) P_(LIST) P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            Tam dam(defaultAllocator_p);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Obj mX(LIST); const Obj& X = mX;

            ASSERT(Y                  == X);
            ASSERT(defaultAllocator_p == X.get_allocator());
            ASSERT(&scratch           == Y.get_allocator());

            if (Obj().capacity() < LIST.size()) {
                ASSERT(dam.isTotalUp());
            }
            else {
                ASSERT(dam.isTotalSame());
            }
        }
    }

    if (verbose) printf("\nTesting constructor: exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t length = 0; length < NUM_LENGTHS; ++length) {

            if (veryVerbose) { T_ P(length) }

            LOOP_ASSERT(length, length < NUM_iListBySize);
            IList              LIST = iListBySize[length];
            const char * const SPEC = iSpecBySize[length];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            int        exceptionLoopCount         = 0;
            const bool allocationExpected         = k_SHORT_BUFFER_CAPACITY
                                                    < length;
            const int  expectedExceptionLoopCount = allocationExpected
                                                    ? 2
                                                    : 1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                ++exceptionLoopCount;

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                             P(exceptionLoopCount)
                                     }
                Tam oem(objectAllocator_p);

                Obj mX(LIST, objectAllocator_p); const Obj& X = mX;

                if (allocationExpected) {
                    ASSERT(oem.isTotalUp());
                }
                else {
                    ASSERT(oem.isTotalSame());
                }

                ASSERTV(length, Y                 == X);
                ASSERTV(length, objectAllocator_p == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(length,
                    expectedExceptionLoopCount == exceptionLoopCount);
        }
        ASSERT(oam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting 'assign' and 'operator=' with initializer lists\n");
    {
        const struct {
            int                         d_line;        // source line number
            const char                 *d_specLhs;     // target string
            std::initializer_list<TYPE> d_list;        // source list
            const char                 *d_specResult;  // expected result
        } DATA[] = {
       //-------^
       //line  lhs                  list                 result
       //----  -------------------- -------------------- -------------------
       { L_,                    "",   {               },                  "" },
       { L_,                    "",   { 'A'           },                 "A" },
       { L_,                   "A",   {               },                 ""  },
       { L_,                   "A",   { 'B'           },                "B"  },
       { L_,                   "A",   { 'A', 'B'      },               "AB"  },
       { L_,                   "A",   { 'B', 'C'      },               "BC"  },
       { L_,                  "AB",   {               },                 ""  },
       { L_,                  "AB",   { 'A', 'B', 'C' },              "ABC"  },
       { L_,                  "AB",   { 'C', 'D', 'E' },              "CDE"  },

       { L_,             emptySpec,           emptyList,           emptySpec },
       { L_,             emptySpec,     maxShortStrList,     maxShortStrSpec },
       { L_,             emptySpec, exceedsShortStrList, exceedsShortStrSpec },

       { L_,       maxShortStrSpec,           emptyList,           emptySpec },
       { L_,       maxShortStrSpec,     maxShortStrList,     maxShortStrSpec },
       { L_,       maxShortStrSpec, exceedsShortStrList, exceedsShortStrSpec },

       { L_,   exceedsShortStrSpec,           emptyList,           emptySpec },
       { L_,   exceedsShortStrSpec,     maxShortStrList,     maxShortStrSpec },
       { L_,   exceedsShortStrSpec, exceedsShortStrList, exceedsShortStrSpec }
       //-------V
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE        = DATA[ti].d_line;
            const char * const SPEC_LHS    = DATA[ti].d_specLhs;
            IList              LIST        = DATA[ti].d_list;
            const char * const SPEC_RESULT = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE)
                               P_(LIST)
                               P_(SPEC_LHS)
                               P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_RESULT) == LIST.size()); // table sanity check

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, SPEC_LHS);
            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC_LHS);
            Obj mZ(&scratch);          const Obj& Z = gg(&mZ, SPEC_RESULT);

            Tam          oam(objectAllocator_p);
            const size_t capacity = X.capacity();

            mX.assign(LIST);
            ASSERTV(Z, X, Z == X);

            mY = LIST;
            ASSERTV(Z, Y, Z == Y);

            if (capacity < LIST.size()) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting 'assign' and 'operator=': exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen,
                                   exceedsShortStrLen + 1
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t dstLength = 0; dstLength < NUM_LENGTHS; ++dstLength) {

            if (veryVerbose) { T_ P(dstLength) }

            LOOP_ASSERT(dstLength, dstLength < NUM_iSpecBySize);

            const char * const SPEC = iSpecBySize[dstLength];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            for (size_t srcLength = 0; srcLength < NUM_LENGTHS; ++srcLength) {
                if (veryVerbose) { T_ T_ P(srcLength) }

                LOOP_ASSERT(srcLength, srcLength < NUM_iListBySize);

                IList LIST  = iListBySize[srcLength];

                const bool allocationExpected = max(Y.capacity(),
                                                    k_SHORT_BUFFER_CAPACITY)
                                              < srcLength;

                int                   allocationCount = 0;
                const int     expectedAllocationCount = allocationExpected
                                                        ? 2  // two methods
                                                        : 0;
                int                exceptionLoopCount = 0;
                const int  expectedExceptionLoopCount = expectedAllocationCount
                                                      + 1;

                Obj        mX(LIST, objectAllocator_p);
                const Obj&  X = mX;

                Obj        mYmethod(Y, objectAllocator_p);
                const Obj&  Ymethod = mYmethod;

                Obj        mYoperator(Y, objectAllocator_p);
                const Obj&  Yoperator = mYoperator;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                    ++exceptionLoopCount;

                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                                 P(exceptionLoopCount)
                                         }
                    Tam oem(objectAllocator_p);

                    mYmethod.assign(LIST);  // test 'assign'    method
                    if (allocationExpected) ++allocationCount;

                    mYoperator = LIST;      // test 'operator=' method
                    if (allocationExpected) ++allocationCount;

                    ASSERTV(X == Ymethod);
                    ASSERTV(X == Yoperator);

                    if (allocationExpected) {
                        ASSERT(oem.isTotalUp());
                    }
                    else {
                        ASSERT(oem.isTotalSame());
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(srcLength,
                         expectedExceptionLoopCount ==  exceptionLoopCount);
                ASSERTV(srcLength,
                            expectedAllocationCount ==     allocationCount);
            }
        }
        ASSERT(oam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert' with initializer lists\n");
    {
        const char * const            maxShortStrSpec2 = iSpecBySize[
                                                           2 * maxShortStrLen];
        const char * const        exceedsShortStrSpec2 = iSpecBySize[
                                                       2 * exceedsShortStrLen];
        const char * const maxPlusExceedsShortStrSpec = iSpecBySize[
                                          maxShortStrLen + exceedsShortStrLen];

        const size_t M = maxShortStrLen / 2;
        const size_t E = maxShortStrLen;

        const struct {
            int                         d_line;        // source line number
            const char                 *d_specInitial; // initial state
            int                         d_position;    // position to insert
            std::initializer_list<TYPE> d_list;        // source list
            const char                 *d_specResult;  // expected result
        } DATA[] = {
   //-----------^
   //line  source              pos  list                result
   //----  ------------------  ---  ------------------  -------------------
   { L_,                   "", -1,   {                },                  "" },
   { L_,                   "", 99,   { 'A'            },                 "A" },
   { L_,                  "A",  0,   {                },                 "A" },
   { L_,                  "A",  0,   { 'B'            },                "BA" },
   { L_,                  "A",  1,   { 'B'            },                "AB" },
   { L_,                 "AB",  0,   {                },                "AB" },
   { L_,                 "AB",  0,   { 'A', 'B'       },              "ABAB" },
   { L_,                 "AB",  1,   { 'B', 'C'       },              "ABCB" },
   { L_,                 "AB",  2,   { 'A', 'B', 'C'  },             "ABABC" },
   { L_,                "ABC",  0,   { 'D'            },              "DABC" },

   { L_,            emptySpec,  0,           emptyList,           emptySpec  },
   { L_,            emptySpec,  0,     maxShortStrList,     maxShortStrSpec  },
   { L_,            emptySpec,  0, exceedsShortStrList, exceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  0,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  0,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  0, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  M,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  M,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  M, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,      maxShortStrSpec,  E,           emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,  E,     maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  E, exceedsShortStrList,
                                                 maxPlusExceedsShortStrSpec  },

   { L_,  exceedsShortStrSpec,  0,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  0,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  0, exceedsShortStrList, exceedsShortStrSpec2 },

   { L_,  exceedsShortStrSpec,  M,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  M,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  M, exceedsShortStrList, exceedsShortStrSpec2 },

   { L_,  exceedsShortStrSpec,  E,           emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  E,     maxShortStrList,
                                                 maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  E, exceedsShortStrList, exceedsShortStrSpec2 }
   //-----------V
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE         = DATA[ti].d_line;
            const char * const SPEC_INITIAL = DATA[ti].d_specInitial;
            const int          POSITION     = DATA[ti].d_position;
            IList              LIST         = DATA[ti].d_list;
            const char * const SPEC_RESULT  = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE)
                               P_(LIST)
                               P_(SPEC_INITIAL)
                               P_(POSITION)
                               P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_INITIAL) + LIST.size()
                == strlen(SPEC_RESULT));  // table sanity check

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, SPEC_INITIAL);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Tam oam(objectAllocator_p);

            size_t index = POSITION == -1 ? 0
                         : POSITION == 99 ? X.size()
                         : /* else */       POSITION;

            bool  expectAllocation = X.capacity() - X.length() < LIST.size();

            const_iterator position =  X.begin() + index;
            iterator       result   = mX.insert(position, LIST);

            ASSERTV(LINE,       result == X.begin() + index);
            ASSERTV(LINE, X, Y, X      == Y);

            if (expectAllocation) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'insert': exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen,
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t dstLength = 0; dstLength < NUM_LENGTHS; ++dstLength) {

            if (veryVerbose) { T_ P(dstLength) }

            LOOP_ASSERT(dstLength, dstLength < NUM_iSpecBySize);

            const char * const SPEC = iSpecBySize[dstLength];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            const size_t srcLength = Y.capacity() - Y.length() + 1;
            ASSERTV(dstLength, srcLength, srcLength < NUM_iListBySize);

            IList LIST = iListBySize[srcLength];

            const size_t expectedLength = Y.length() + srcLength;
            ASSERTV(dstLength, srcLength, expectedLength < NUM_iSpecBySize);

            const char * const expectedSPEC = iSpecBySize[expectedLength];

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, expectedSPEC);

            int                exceptionLoopCount = 0;
            const int  expectedExceptionLoopCount = 2;
            int                   allocationCount = 0;
            const int     expectedAllocationCount = 1;

            Tam oem(objectAllocator_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                ++exceptionLoopCount;

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                             P(exceptionLoopCount) }

                mY.insert(Y.size(), LIST);
                ++allocationCount;  // 'LIST' calculated to allocate.

                ASSERTV(srcLength, X == Y);
                ASSERTV(srcLength, oem.isInUseUp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(srcLength,
                     expectedExceptionLoopCount ==  exceptionLoopCount);
            ASSERTV(srcLength,
                        expectedAllocationCount ==     allocationCount);
        }
        ASSERT(oam.isInUseSame());
    }

    if (verbose) printf("\nTesting 'append' with initializer lists\n");
    {
        const char * const            maxShortStrSpec2 = iSpecBySize[
                                                           2 * maxShortStrLen];
        const char * const        exceedsShortStrSpec2 = iSpecBySize[
                                                       2 * exceedsShortStrLen];
        const char * const maxPlusExceedsShortStrSpec = iSpecBySize[
                                          maxShortStrLen + exceedsShortStrLen];

        const struct {
            int                          d_line;        // source line number
            const char                  *d_specInitial; // initial state
            std::initializer_list<TYPE>  d_list;        // source list
            const char                  *d_specResult;  // expected result
        } DATA[] = {
   //-----------^
   //line  source               list                result
   //----  ------------------   ------------------  -------------------
   { L_,                   "",   {                },                  "" },
   { L_,                   "",   { 'A'            },                 "A" },
   { L_,                  "A",   {                },                 "A" },
   { L_,                  "A",   { 'B'            },                "AB" },
   { L_,                 "AB",   {                },                "AB" },
   { L_,                 "AB",   { 'A', 'B'       },              "ABAB" },
   { L_,                 "AB",   { 'A', 'B', 'C'  },             "ABABC" },
   { L_,                "ABC",   { 'D'            },              "ABCD" },

   { L_,      maxShortStrSpec,            emptyList,     maxShortStrSpec  },
   { L_,      maxShortStrSpec,      maxShortStrList,     maxShortStrSpec2 },
   { L_,      maxShortStrSpec,  exceedsShortStrList,
                                              maxPlusExceedsShortStrSpec  },

   { L_,  exceedsShortStrSpec,            emptyList, exceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,      maxShortStrList,
                                              maxPlusExceedsShortStrSpec  },
   { L_,  exceedsShortStrSpec,  exceedsShortStrList, exceedsShortStrSpec2 }
   //-----------V
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        Tam dam(defaultAllocator_p);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE         = DATA[ti].d_line;
            const char * const SPEC_INITIAL = DATA[ti].d_specInitial;
            IList              LIST         = DATA[ti].d_list;
            const char * const SPEC_RESULT  = DATA[ti].d_specResult;

            if (veryVerbose) { P_(LINE)
                               P_(LIST)
                               P_(SPEC_INITIAL)
                               P(SPEC_RESULT) }

            ASSERT(strlen(SPEC_INITIAL) + LIST.size()
                == strlen(SPEC_RESULT));  // table sanity check

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, SPEC_INITIAL);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mY(&scratch); const Obj& Y = gg(&mY, SPEC_RESULT);

            Tam oam(objectAllocator_p);

            bool  expectAllocation = X.capacity() - X.length() < LIST.size();

            Obj *mR = &mX.append(LIST);

            ASSERTV(LINE, X,    Y,  X == Y);
            ASSERTV(LINE, mR, &mX, mR == &mX);

            if (expectAllocation) {
                ASSERT(oam.isTotalUp());
            }
            else {
                ASSERT(oam.isTotalSame());
            }
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose) printf("\nTesting 'append': exception-safety\n");
    {
        const size_t LENGTHS[] = {                  0,
                                       maxShortStrLen,
                                   exceedsShortStrLen,
                                 };
        const size_t NUM_LENGTHS = sizeof LENGTHS / sizeof *LENGTHS;

        Tam oam(objectAllocator_p);

        for (size_t dstLength = 0; dstLength < NUM_LENGTHS; ++dstLength) {

            if (veryVerbose) { T_ P(dstLength) }

            LOOP_ASSERT(dstLength, dstLength < NUM_iSpecBySize);

            const char * const SPEC = iSpecBySize[dstLength];

            Obj mY(objectAllocator_p); const Obj& Y = gg(&mY, SPEC);

            const size_t srcLength = Y.capacity() - Y.length() + 1;
            ASSERTV(dstLength, srcLength, srcLength < NUM_iListBySize);

            IList LIST = iListBySize[srcLength];

            const size_t expectedLength = Y.length() + srcLength;
            ASSERTV(dstLength, srcLength, expectedLength < NUM_iSpecBySize);

            const char * const expectedSPEC = iSpecBySize[expectedLength];

            Obj mX(objectAllocator_p); const Obj& X = gg(&mX, expectedSPEC);

            int                exceptionLoopCount = 0;
            const int  expectedExceptionLoopCount = 2;
            int                   allocationCount = 0;
            const int     expectedAllocationCount = 1;

            Tam oem(objectAllocator_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*objectAllocator_p) {
                ++exceptionLoopCount;

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody)
                                             P(exceptionLoopCount) }

                mY.append(LIST);
                ++allocationCount;  // 'LIST' calculated to allocate.

                ASSERTV(srcLength, X == Y);
                ASSERTV(srcLength, oem.isInUseUp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(srcLength,
                     expectedExceptionLoopCount ==  exceptionLoopCount);
            ASSERTV(srcLength,
                        expectedAllocationCount ==     allocationCount);
        }
        ASSERT(oam.isInUseSame());
    }
#else
    // Empty test.
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase29()
{
    // --------------------------------------------------------------------
    // TESTING 'hashAppend'
    //   Verify that the hashAppend function works properly and is picked
    //   up by 'bslh::Hash'
    //
    // Concerns:
    //: 1 'bslh::Hash' picks up 'hashAppend(string)' and can hash strings
    //:
    //: 2 'hashAppend' hashes the entire string, regardless of 'char' or
    //:   'wchar'
    //:
    //: 3 Empty strings can be hashed
    //:
    //: 4 Hash is not computed on data beyond the end of very short strings
    //
    // Plan:
    //: 1 Use 'bslh::Hash' to hash a few values of strings with each char type.
    //:   (C-1,2)
    //:
    //: 2 Hash an empty string. (C-3)
    //:
    //: 3 Hash two very short strings with the same value and assert that they
    //:   produce equal hashes. (C-4)
    //
    // Testing:
    //   hashAppend(HASHALG& hashAlg, const basic_string& str);
    //   hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
    // --------------------------------------------------------------------
    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef typename Hasher::result_type HashType;
    typedef native_std::basic_string<TYPE,TRAITS,ALLOC> NativeObj;

    const int PRIME = 100003; // Arbitrary large prime to be used in hash-table
                              // like testing

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif

    int       collisions[PRIME]       = {};
    int       nativeCollisions[PRIME] = {};

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

    Hasher    hasher;
    size_t    prevHash                = 0;
    HashType  hash                    = 0;

    if (verbose) printf("Use 'bslh::Hash' to hash a few values of strings with"
                        " each char type. (C-1,2)\n");
    {
        for (int i = 0; i != PRIME; ++i) {
            Obj num;
            if (i > 66000){
                //Make sure we're testing long strings
                for (int j = 0; j < 40; ++j) {
                    num.push_back(TYPE('A'));
                }
            }
            else if (i > 33000) {
                //Make sure we're testing with null characters in the strings
                for (int j = 0; j < 5; ++j) {
                    num.push_back(TYPE('A'));
                    num.push_back(TYPE('\0'));
                }
            }
            num.push_back( TYPE('0' + (i/1000000)     ));
            num.push_back( TYPE('0' + (i/100000)  %10 ));
            num.push_back( TYPE('0' + (i/10000)   %10 ));
            num.push_back( TYPE('0' + (i/1000)    %10 ));
            num.push_back( TYPE('0' + (i/100)     %10 ));
            num.push_back( TYPE('0' + (i/10)      %10 ));
            num.push_back( TYPE('0' + (i)         %10 ));

            if (veryVerbose) dbg_print("Testing hash of ", num.data(), "\n");

            prevHash = hash;
            hash     = hasher(num);

            // Check consecutive values are not hashing to the same hash
            ASSERT(prevHash != hash);

            // Check that minimal collisions are happening
            ASSERT(++collisions[hash % PRIME] <= 11);  // Choose 11 as a max
                                                   // number collisions

            Obj numCopy = num;

            // Verify same hash is produced for the same value
            ASSERT(num == numCopy);
            ASSERT(hash == hasher(numCopy));
        }
    }

    if (verbose) printf("Use 'bslh::Hash' to hash a few values of 'native_std'"
                        " strings with each char type. (C-1,2)\n");
    {
        for (int i = 0; i != PRIME; ++i) {
            NativeObj num;
            if (i > 66000){
                //Make sure we're testing long strings
                for (int j = 0; j < 40; ++j) {
                    num.push_back(TYPE('A'));
                }
            }
            else if (i > 33000) {
                //Make sure we're testing with null characters in the strings
                for (int j = 0; j < 5; ++j) {
                    num.push_back(TYPE('A'));
                    num.push_back(TYPE('\0'));
                }
            }
            num.push_back( TYPE('0' + (i/1000000)     ));
            num.push_back( TYPE('0' + (i/100000)  %10 ));
            num.push_back( TYPE('0' + (i/10000)   %10 ));
            num.push_back( TYPE('0' + (i/1000)    %10 ));
            num.push_back( TYPE('0' + (i/100)     %10 ));
            num.push_back( TYPE('0' + (i/10)      %10 ));
            num.push_back( TYPE('0' + (i)         %10 ));

            if (veryVerbose) dbg_print("Testing hash of ", num.data(), "\n");

            prevHash = hash;
            hash     = hasher(num);

            // Check consecutive values are not hashing to the same hash
            ASSERT(prevHash != hash);

            // Check that minimal collisions are happening
            ASSERT(++nativeCollisions[hash % PRIME] <= 11);
                                                         // Choose 11 as a max
                                                         // number collisions

            NativeObj numCopy = num;

            // Verify same hash is produced for the same value
            ASSERT(num == numCopy);
            ASSERT(hash == hasher(numCopy));
        }
    }

    if (verbose) printf("Hash an empty string. (C-3)\n");
    {
        Obj empty;
        ASSERT(hasher(empty));
    }

    if (verbose) printf("Hash two very short strings with the same value and"
                        " assert that they produce equal hashes. (C-4)\n");
    {
        Obj small1;
        Obj small2;
        small1.push_back( TYPE('0') );
        small2.push_back( TYPE('0') );
        ASSERT(hasher(small1) == hasher(small2));
    }

    if (verbose) printf("Verify that bsl::hash hashes as strings.\n");
    {
        Obj s;
        s.push_back( TYPE('a') );
        s.push_back( TYPE('b') );
        s.push_back( TYPE('c') );
        ASSERT(bsl::hash<Obj>()(s) == bsl::hash<Obj>()(s.data()));
        ASSERT(bsl::hash<Obj>()(s) == bsl::hash<Obj>()(s.data()));
        TYPE abc[] = { TYPE('a'), TYPE('b'), TYPE('c'), TYPE(0) };
        ASSERT(bsl::hash<Obj>()(s) == bsl::hash<Obj>()(abc));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase28()
{
    // --------------------------------------------------------------------
    // TESTING THE SHORT STRING OPTIMIZATION
    //
    // Concerns:
    //  1) String should have an initial non-zero capacity (short string
    //     buffer).
    //  2) It should not allocate up to that capacity.
    //  3) It should work with the char_type larger than the short string
    //     buffer.
    //  4) A Long string with length smaller than the size of the short string
    //     buffer copied to a new string should produce a short new string.
    //  5) It should work with the NULL-terminator different from '\0' to make
    //     sure that the implementation always uses char_type() default
    //     constructor to terminate the string rather than a null literal.
    //
    // Plan:
    //  1) Construct an empty string and check its capacity.
    //  2) Construct strings with lengths from 0 to N (where N > initial
    //     capacity) and verify that the string class allocates when the string
    //     length becomes larger than the short string buffer.
    //  3) Construct a long string.  Erase some characters from it, so the
    //     length becomes smaller than the size of the short string buffer.
    //     Then make a copy of this string using the test allocator and verify
    //     that the new copy did not need any new memory.
    // ------------------------------------------------------------------------

    if (verbose) printf("\nString has a non-zero initial capacity.\n");

    {
        Obj emptyStr;
        ASSERT(emptyStr.capacity() == DEFAULT_CAPACITY);
    }

    if (verbose) printf("\nString doesn't allocate while it uses the short "
                        "string buffer.\n");

    {
        // make some reasonable number of test iterations
        const size_t specSize = 2 * DEFAULT_CAPACITY > 10
                                     ? 2 * DEFAULT_CAPACITY
                                     : 10;
        char spec[specSize];

        for (size_t size = 1; size < specSize; ++size) {

            // construct the spec string
            for (size_t i = 0; i < size; ++i) {
                spec[i] = (i & 1) ? 'A' : 'B';
            }

            spec[size] = '\0';

            // check if the string allocates
            bslma::TestAllocator testAllocator(veryVeryVerbose);
            Obj str(g(spec), &testAllocator);

            // allocates only if larger than the short string buffer
            ASSERT((size <= DEFAULT_CAPACITY) ==
                   (testAllocator.numBytesInUse() == 0));
            ASSERT((size <= DEFAULT_CAPACITY) ==
                   (testAllocator.numBlocksTotal() == 0));

            // check if copy-constructor allocates
            Obj strCpy(str, &testAllocator);

            ASSERT((size <= DEFAULT_CAPACITY)
                    == (testAllocator.numBytesInUse() == 0));
            ASSERT((size <= DEFAULT_CAPACITY)
                    == (testAllocator.numBlocksTotal() == 0));

            // check that copying a long string with a short length results in
            // a short string
            if (str.size() > DEFAULT_CAPACITY) {
                Obj strLong(str);
                size_t oldCapacity = strLong.capacity();

                // remove some characters from the string to shorten it
                strLong.erase(0, strLong.size() - DEFAULT_CAPACITY);

                // check that 'erase' did not change the capacity
                ASSERT(strLong.capacity() == oldCapacity);

                // check that copying produces a short string now and that it
                // doesn't allocate
                bslma::TestAllocator testAllocatorShort(veryVeryVerbose);
                Obj strShort(strLong, &testAllocatorShort);

                ASSERT(testAllocatorShort.numBytesInUse() == 0);
                ASSERT(testAllocatorShort.numBlocksTotal() == 0);
            }

            // check that the string is terminated properly with 'TYPE()' value
            // rather than just '\0'
            ASSERT(*(str.c_str() + str.size()) == TYPE());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase26()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSIONS WITH NATIVE STRINGS
    //
    // Testing:
    //   CONCERNS:
    //    - A bsl::basic_string is implicitly convertible to a
    //      native_std::basic_string with the same CHAR_TYPE and
    //      CHAR_TRAITS.
    //    - A native_std::basic_string is explicitly convertible to a
    //      bsl::basic_string with the same CHAR_TYPE and
    //      CHAR_TRAITS.
    //    - A bsl::basic_string and a native_std::basic_string with the
    //      same template parameters will have the same npos value.
    //
    // Plan:
    //   For a variety of strings of different sizes and different values, test
    //   that the string is implicitly convertible to native_std::string and
    //   that the conversion yields the same value.  Test that one can
    //   construct a bsl::string from the native_std::string.  Test that
    //   bslstl::string::npos compares equal to native_std::string::npos.
    //
    // Testing:
    //   npos
    //   operator native_std::basic_string<CHAR, CHAR_TRAITS, ALLOC2>() const;
    //   basic_string(const native_std::basic_string<CHAR,
    //                                               CHAR_TRAITS,
    //                                               ALLOC2>&);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "ABA",
        "ABB",
        "AAAAABAAAAAAAAA",
        0  // null string required as last element
    };

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    typedef LimitAllocator<ALLOC> AltAlloc;
    typedef native_std::basic_string<TYPE, TRAITS, ALLOC> NativeObj;
    typedef native_std::basic_string<TYPE, TRAITS, AltAlloc> NativeObjAlt;

    if (verbose) printf("\tTesting npos\n");

    LOOP2_ASSERT(Obj::npos, NativeObj::npos, Obj::npos == NativeObj::npos);

    if (verbose) printf("\tTesting conversion to native string\n");

    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            NativeObjAlt v;
            const NativeObjAlt& V = v;

            ASSERT(! isNativeString(U));
            ASSERT(  isNativeString(V));

            v = U;  // Assignment requires implicit conversion
            const NativeObjAlt V2(U);   // implicit Conversion
            const NativeObjAlt V3 = U;  // implicit Conversion
            ASSERT(Obj(V.c_str()) == Obj(U.c_str()));

            const Obj U2(V);  // Explicit conversion construction
            ASSERT(Obj(V.c_str()) == Obj(U2.c_str()));
            ASSERT(U2 == U);

            // 'operator=='
            ASSERT(V == U);
            ASSERT(U == V);

            // 'operator!='
            ASSERT(!(V != U));
            ASSERT(!(U != V));

            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const X_SPEC = SPECS[sj];
                const Obj X(g(X_SPEC));

                if (veryVerbose) {
                    T_; T_; T_; P_(X_SPEC); P(X);
                }

                // Since free operators for the case when both arguments are
                // 'bsl::string' is thoroughly tested already, we can compare
                // the results of 'std::string < bsl::string' against
                // 'bsl::string < bsl::string', etc...

                ASSERT((U < X) == (V < X));
                ASSERT((X < U) == (X < V));
                ASSERT((U > X) == (V > X));
                ASSERT((X > U) == (X > V));

                ASSERT((U <= X) == (V <= X));
                ASSERT((X <= U) == (X <= V));
                ASSERT((U >= X) == (V >= X));
                ASSERT((X >= U) == (X >= V));

                ASSERT((U + X) == (V + X));
                ASSERT((X + U) == (X + V));
                ASSERT((U + X) == (V + X));
                ASSERT((X + U) == (X + V));
            }
        }

        Obj b(VALUES);
        NativeObj n(VALUES);
        n += b + VALUES;
        ASSERT(0 == std::memcmp(&n[0 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERT(0 == std::memcmp(&n[1 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERT(0 == std::memcmp(&n[2 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERTV(3u * NUM_VALUES == n.size());
    }

    if (verbose) printf("\tTesting conversion from native string\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));
            NativeObjAlt v;         const NativeObjAlt& V = v;
            v = U;

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            ASSERT(!isNativeString(U));
            ASSERT( isNativeString(V));

            Obj bslString;
            bslString = V;  // Assignment requires implicit conversion

            ASSERT(Obj(bslString.c_str()) == Obj(V.c_str()));

            const Obj U2(V);  // Explicit conversion construction
            ASSERT(Obj(V.c_str()) == Obj(U2.c_str()));
            ASSERT(U2 == U);

            // 'operator=='
            ASSERT(V == U);
            ASSERT(U == V);
        }

        Obj b(VALUES);
        NativeObj n(VALUES);
        b += n + VALUES;
        ASSERT(0 == std::memcmp(&b[0 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERT(0 == std::memcmp(&b[1 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERT(0 == std::memcmp(&b[2 * NUM_VALUES], VALUES, NUM_VALUES));
        ASSERT(3u * NUM_VALUES == b.size());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase25()
{
    // --------------------------------------------------------------------
    // TESTING 'std::length_error'
    //
    // Concerns:
    //   1) That any call to a constructor, 'append', 'assign', 'insert',
    //      'operator+=', or 'replace' which would result in a value exceeding
    //      'max_size()' throws 'std::length_error'.
    //   2) That the 'max_size()' taken into consideration is that of the
    //      allocator, and not an absolute constant.
    //   3) That the value of the string is unchanged if an exception is
    //      thrown.
    //   4) That integer overflows are correctly handled when length_error
    //      exceeds 'Obj::max_size()' (which is the largest representable
    //      size_type).
    //
    // Plan:
    //   For concern 2, we use an allocator wrapper that provides the same
    //   functionality as 'ALLOC' but changes the return value of 'max_size()'
    //   to a 'limit' value settable at runtime.  Note that the allocator
    //   'max_size()' includes the null-terminating char, and so the string
    //   'max_size()' is one less than the allocator; in other words, the
    //   operations throw unless 'length <= limit - 1', i.e., they throw if
    //   'limit <= length'.
    //
    //   Construct objects with value large enough that the constructor throws.
    //   For 'append', 'assign', 'insert', 'operator+=', or 'replace', we
    //   construct a small (non-empty) object, and use the corresponding
    //   function to request an increase in size that is guaranteed to result
    //   in a value exceeding 'max_size()'.
    //
    // Testing:
    //   Proper use of 'std::length_error'
    // ------------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    LimitAllocator<ALLOC> a(&testAllocator);
    a.setMaxSize((size_t)-1);

    const size_t LENGTH = 32;
    typedef bsl::basic_string<TYPE,TRAITS,LimitAllocator<ALLOC> > LimitObj;

    LimitObj mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nConstructor 'string(str, pos, n, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y, 0, LENGTH, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        } catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        } catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(C *s, n, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.c_str(), LENGTH, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(C *s, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.c_str(), a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string(n, c, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'string<Iter>(f, l, a = A())'.\n");

    for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = " ZU "\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit,
                exceptionCaught,
                (limit <= LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'resize'.\n");
    {
        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                mX.resize(LENGTH, DEFAULT_VALUE);
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'assign'.\n");

    for (int assignMethod = 0; assignMethod <= 5; ++assignMethod) {

        if (veryVerbose) {
            switch (assignMethod) {
                case 0: printf("\tWith assign(str).\n");          break;
                case 1: printf("\tWith assign(str, pos, n).\n");  break;
                case 2: printf("\tWith assign(C *s, n).n");       break;
                case 3: printf("\tWith assign(C *s).\n");         break;
                case 4: printf("\tWith assign(n, c).n");          break;
                case 5: printf("\tWith assign<Iter>(f, l).\n");   break;
                case 6: printf("\tWith assign<Iter>(i, i).\n");   break;
                default: ASSERT(0);
            }
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (assignMethod) {
                  case 0: mX.assign(Y);                                 break;
                  case 1: mX.assign(Y, 0, LENGTH);                      break;
                  case 2: mX.assign(Y.c_str(), LENGTH);                 break;
                  case 3: mX.assign(Y.c_str());                         break;
                  case 4: mX.assign(LENGTH, Y[0]);                      break;
                  case 5: mX.assign(Y.begin(), Y.end());                break;
                  case 6: mX.assign(LENGTH, static_cast<size_t>(Y[0])); break;
                  default: ASSERT(0);
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'operator+='.\n");

    for (int operatorMethod = 0; operatorMethod <= 2; ++operatorMethod) {

        if (veryVerbose) {
            switch (operatorMethod) {
                case 0: printf("\toperator+=(str).\n");   break;
                case 1: printf("\toperator+=(C *s).\n");  break;
                case 2: printf("\toperator+=(C c).\n");   break;
                default: ASSERT(0);
            }
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (operatorMethod) {
                  case 0: {
                    mX += Y;
                  } break;
                  case 1: {
                    mX += Y.c_str();
                  } break;
                  case 2: {
                    for (size_t i = 0; i < Y.size(); ++i) {
                        mX += Y[i];
                    }
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'append'.\n");

    for (int appendMethod = 0; appendMethod <= 5; ++appendMethod) {

        if (verbose) {
            switch (appendMethod) {
              case 0: printf("\tWith append(str).\n");         break;
              case 1: printf("\tWith append(str, pos, n).\n"); break;
              case 2: printf("\tWith append(C *s, n).\n");     break;
              case 3: printf("\tWith append(C *s).\n");        break;
              case 4: printf("\tWith append(n, c).\n");        break;
              case 5: printf("\tWith append<Iter>(f, l).\n");  break;
              case 6: printf("\tWith append<Iter>(i, i).\n");  break;
              default: ASSERT(0);
            }
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (appendMethod) {
                  case 0: {
                    mX.append(Y);
                  } break;
                  case 1: {
                    mX.append(Y, 0, LENGTH);
                  } break;
                  case 2: {
                    mX.append(Y.c_str(), LENGTH);
                  } break;
                  case 3: {
                    mX.append(Y.c_str());
                  } break;
                  case 4: {
                    mX.append(LENGTH, DEFAULT_VALUE);
                  } break;
                  case 5: {
                    mX.append(Y.begin(), Y.end());
                  } break;
                  case 6: {
                    mX.append(LENGTH, static_cast<size_t>(DEFAULT_VALUE));
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert'.\n");

    for (int insertMethod = 0; insertMethod <= 8; ++insertMethod) {

        if (verbose) {
            switch (insertMethod) {
              case 0: printf("\tWith push_back(c).\n");               break;
              case 1: printf("\tWith insert(pos, str).\n");           break;
              case 2: printf("\tWith insert(pos, str, pos2, n).\n");  break;
              case 3: printf("\tWith insert(pos, C *s, n).\n");       break;
              case 4: printf("\tWith insert(pos, C *s).\n");          break;
              case 5: printf("\tWith insert(pos, n, C c).\n");        break;
              case 6: printf("\tWith insert(p, C c).\n");             break;
              case 7: printf("\tWith insert(p, n, C c).\n");          break;
              case 8: printf("\tWith insert<Iter>(p, f, l).\n");      break;
              default: ASSERT(0);
            }
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (insertMethod) {
                  case 0: {
                    for (size_t i = 0; i < LENGTH; ++i) {
                        mX.push_back(Y[i]);
                    }
                  } break;
                  case 1: {
                    mX.insert(0, Y);
                  } break;
                  case 2: {
                    mX.insert(0, Y, 0, LENGTH);
                  } break;
                  case 3: {
                    mX.insert(0, Y.c_str(), LENGTH);
                  } break;
                  case 4: {
                    mX.insert(0, Y.c_str());
                  } break;
                  case 5: {
                    mX.insert((size_t)0, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 6: {
                    for (size_t i = 0; i < LENGTH; ++i) {
                        mX.insert(mX.begin(), DEFAULT_VALUE);
                    }
                  } break;
                  case 7: {
                    mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  case 8: {
                    mX.insert(mX.cbegin(), Y.begin(), Y.end());
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'replace'.\n");

    for (int replaceMethod = 1; replaceMethod < 8; ++replaceMethod) {

        if (verbose) {
            switch (replaceMethod) {
              case 1: printf("\tWith replace(pos1, n1, str).\n");
                      break;
              case 2: printf("\tWith replace(pos1, n1, str, pos2, n2).\n");
                      break;
              case 3: printf("\tWith replace(pos1, n1, C *s, n2).\n");
                      break;
              case 4: printf("\tWith replace(pos1, n1, C *s).\n");
                      break;
              case 5: printf("\tWith replace(pos1, n1, n2, C c).\n");
                      break;
              case 6: printf("\tWith replace(f, l, const C *s, n2).\n");
                      break;
              case 7: printf("\tWith replace(f, l, n, const C *s).\n");
                      break;
              case 8: printf("\treplace(f, l, n2, C c).\n");
                      break;
              default: ASSERT(0);
            }
        }

        for (size_t limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = " ZU "\n", limit);

            try {
                LimitObj mX(a);

                switch (replaceMethod) {
                  case 1: {
                    mX.replace(0, 1, Y);
                  } break;
                  case 2: {
                    mX.replace(0, 1, Y, 0, LENGTH);
                  } break;
                  case 3: {
                    mX.replace(0, 1, Y.c_str(), LENGTH);
                  } break;
                  case 4: {
                    mX.replace(0, 1, Y.c_str());
                  } break;
                  case 5: {
                    mX.replace(0, 1, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 6: {
                    mX.replace(mX.begin(), mX.end(), Y.c_str(), LENGTH);
                  } break;
                  case 7: {
                    mX.replace(mX.begin(), mX.end(), Y.c_str());
                  } break;
                  case 8: {
                    mX.replace(mX.begin(), mX.end(), LENGTH, DEFAULT_VALUE);
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            } catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            } catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t \tCaught std::exception(%s).\n", e.what());
                }
            } catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit,
                    exceptionCaught,
                    (limit <= LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    const int PADDING = 16;
    size_t expMaxSize = -1;
    const size_t& EXP_MAX_SIZE = expMaxSize;
    {
        const Obj X;
        expMaxSize = X.max_size();
    }
    ASSERTV(EXP_MAX_SIZE, (size_t)-1 > EXP_MAX_SIZE);

    if (EXP_MAX_SIZE >= (size_t)-2) {
        printf("\n\nERROR: Cannot continue this test case without attempting\n"
               "to allocate huge amounts of memory.  *** Aborting. ***\n\n");
        return;                                                       // RETURN
    }

    const size_t DATA[] = {
        EXP_MAX_SIZE + 1,
        EXP_MAX_SIZE + 2,
        (EXP_MAX_SIZE + 1) / 2 + (size_t)-1 / 2,
        (size_t)-2,
        (size_t)-1,
        0  // must be the last value
    };

    if (verbose) printf("\nConstructor 'string(n, c, a = A())'"
                        " and 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)
    {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tWith 'n' = " ZU "\n", DATA[i]);

        try {
            Obj mX(DATA[i], DEFAULT_VALUE);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (const std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'reserve/resize' and"
                        " 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int capacityMethod = 0; capacityMethod < 3; ++capacityMethod)
    {
        if (verbose) {
            switch (capacityMethod) {
              case 0: printf("\tWith reserve(n).\n");        break;
              case 1: printf("\tWith resize(n).\n");         break;
              case 2: printf("\tWith resize(n, C c).\n");    break;
              default: ASSERT(0);
            }
        }

        for (int i = 0; DATA[i]; ++i)
        {
            bool exceptionCaught = false;

            if (veryVerbose) printf("\t\tWith 'n' = " ZU "\n", DATA[i]);

            try {
                Obj mX;

                switch (capacityMethod) {
                  case 0:  mX.reserve(DATA[i]);                  break;
                  case 1:  mX.resize(DATA[i]);                   break;
                  case 2:  mX.resize(DATA[i], DEFAULT_VALUE);    break;
                  default: ASSERT(0);
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'append' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int appendMethod = 4; appendMethod <= 4; ++appendMethod) {

        if (verbose) {
            switch (appendMethod) {
              case 4: printf("\tWith append(n, c).\n");        break;
              default: ASSERT(0);
            }
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);

                mX.append(DATA[i] - PADDING, DEFAULT_VALUE);
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int insertMethod = 5; insertMethod <= 7; insertMethod += 2) {

        if (verbose) {
            switch (insertMethod) {
              case 5: printf("\tWith insert(pos, n, C c).\n");        break;
              case 7: printf("\tWith insert(p, n, C c).\n");          break;
              default: ASSERT(0);
            }
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);

                const size_t LENGTH = DATA[i] - PADDING;
                switch (insertMethod) {
                  case 5: {
                    mX.insert((size_t)0, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 7: {
                    mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'replace' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int replaceMethod = 5; replaceMethod <= 8; replaceMethod += 3) {

        if (verbose) {
            switch (replaceMethod) {
              case 5: printf("\tWith replace(pos1, n1, n2, C c).\n");  break;
              case 8: printf("\tWith replace(f, l, n2, C c).\n");      break;
              default: ASSERT(0);
            }
        }

        for (int i = 0; DATA[i]; ++i) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length " ZU ".\n", DATA[i]);

            try {
                Obj mX(3 * PADDING, DEFAULT_VALUE);

                const size_t LENGTH = DATA[i] - PADDING;
                switch (replaceMethod) {
                  case 5: {
                    mX.replace(PADDING, PADDING, LENGTH, DEFAULT_VALUE);
                  } break;
                  case 8: {
                    mX.replace(mX.begin() + PADDING,
                               mX.begin() + 2 * PADDING,
                               LENGTH,
                               DEFAULT_VALUE);
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                }
            } catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            } catch (const std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t \tCaught std::exception(%s).\n", e.what());
                }
            } catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase24()
{
    // --------------------------------------------------------------------
    // TESTING COMPARISONS
    //
    // Concerns:
    //   1) 'operator<' returns the lexicographic comparison on two arrays.
    //   2) 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //      'operator<'.
    //   3) 'compare' returns the correct result.
    //   4) That traits get selected properly.
    //
    // Plan:
    //   For a variety of strings of different sizes and different values, test
    //   that the comparison returns as expected.  Note that capacity is not of
    //   concern here, the implementation specifically uses only 'begin()',
    //   'end()', and 'size()'.  For concern 4, use 'TRAITS::compare()'
    //   explicitly in the 'check_compare' helper function, and check that the
    //   return value has not only the correct sign, but the same value as
    //   well.
    //
    // Testing:
    //   int  compare(const string& str) const;
    //   int  compare(pos1, n1, const string& str) const;
    //   int  compare(pos1, n1, const string& str, pos2, n2 = npos) const;
    //   int  compare(const C* s) const;
    //   int  compare(pos1, n1, const C* s) const;
    //   int  compare(pos1, n1, const C* s, n2) const;
    //   bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator<(const C *, const string<C,CT,A>&);
    //   bool operator<(const string<C,CT,A>&, const C *);
    //   bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator>(const C *, const string<C,CT,A>&);
    //   bool operator>(const string<C,CT,A>&, const C *);
    //   bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator<=(const C *, const string<C,CT,A>&);
    //   bool operator<=(const string<C,CT,A>&, const C *);
    //   bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   bool operator>=(const C *, const string<C,CT,A>&);
    //   bool operator>=(const string<C,CT,A>&, const C *);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "AAA",
        "AAAA",
        "AAAAA",
        "AAAAAA",
        "AAAAAAA",
        "AAAAAAAA",
        "AAAAAAAAA",
        "AAAAAAAAAA",
        "AAAAAAAAAAA",
        "AAAAAAAAAAAA",
        "AAAAAAAAAAAAA",
        "AAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAA",
        "AAAAAB",
        "AAAAABA",
        "AAAAABAA",
        "AAAAABAAA",
        "AAAAABAAAA",
        "AAAAABAAAAA",
        "AAAAABAAAAAA",
        "AAAAABAAAAAAA",
        "AAAAABAAAAAAAA",
        "AAAAABAAAAAAAAA",
        "AAAAB",
        "AAAABAAAAAA",
        "AAAABAAAAAAA",
        "AAAABAAAAAAAA",
        "AAAABAAAAAAAAA",
        "AAAABAAAAAAAAAA",
        "AAAB",
        "AAABA",
        "AAABAA",
        "AAABAAAAAA",
        "AAB",
        "AABA",
        "AABAA",
        "AABAAA",
        "AABAAAAAA",
        "AB",
        "ABA",
        "ABAA",
        "ABAAA",
        "ABAAAAAA",
        "B",
        "BA",
        "BAA",
        "BAAA",
        "BAAAA",
        "BAAAAA",
        "BAAAAAA",
        "BB",
        0  // null string required as last element
    };

    if (verbose) printf("\tTesting free operators <, >, <=, >=.\n");

    if (veryVerbose) printf("\tCompare each pair of similar and different"
                            " values (u, v) in S X S.\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                const Obj V(g(V_SPEC));

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                // First comparisons with 'string' objects
                const bool isLess = si < sj;
                const bool isLessEq = !(sj < si);
                LOOP2_ASSERT(si, sj,  isLess   == (U < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V));

                // Then test comparisons with C-strings
                LOOP2_ASSERT(si, sj,  isLess   == (U.c_str() < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U.c_str() > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U.c_str() <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U.c_str() >= V));

                LOOP2_ASSERT(si, sj,  isLess   == (U < V.c_str()));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V.c_str()));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V.c_str()));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V.c_str()));
            }
        }
    }

    if (verbose) printf("\tTesting 'compare'.\n");

    if (veryVerbose) printf("\tCompare each substring previous.\n");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];
            const size_t      U_LEN  = strlen(U_SPEC);

            const Obj U(g(U_SPEC));

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];
                const size_t      V_LEN  = strlen(V_SPEC);

                const Obj V(g(V_SPEC));

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                checkCompare(U, V, U.compare(V));
                checkCompare(U, V, U.compare(V.c_str()));

                for (size_t i = 0; i <= U_LEN; ++i) {
                    for (size_t j = 0; j <= V_LEN; ++j) {
                        const Obj U1(U, i, 1);
                        const Obj UN(U, i, U_LEN);
                        const Obj V1(V, j, 1);
                        const Obj VN(V, j, V_LEN);

                        // int compare(pos1, n1, str) const;

                        checkCompare(U1, V,  U.compare(i,     1, V));
                        checkCompare(UN, V,  U.compare(i, U_LEN, V));

                        // int compare(pos1, n1, str, pos2, n2 = npos) const;

                        checkCompare(U1, V1, U.compare(i,     1, V, j,     1));
                        checkCompare(UN, V1, U.compare(i, U_LEN, V, j,     1));

                        checkCompare(U1, VN, U.compare(i,     1, V, j, V_LEN));
                        checkCompare(UN, VN, U.compare(i, U_LEN, V, j, V_LEN));

                        checkCompare(U1, VN, U.compare(i,     1, V, j));
                        checkCompare(UN, VN, U.compare(i, U_LEN, V, j));
                    }
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase24Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COMPARISONS
    //
    // Concerns:
    //   1 'compare' asserts on undefined behavior when it's passed a NULL
    //   C-string pointer.
    //   2 comparison free operator overloads assert on undefined behavior when
    //   they are passed a NULL C-string pointer.
    //
    // Plan:
    //   For each 'compare' method overload, create a non-empty string and test
    //   'compare' with a NULL C-string pointer parameter.
    //
    //   For each comparison free-function overload, do the same as for
    //   'compare' method.
    //
    // Testing:
    //   int  compare(const C* s) const;
    //   int  compare(pos1, n1, const C* s) const;
    //   int  compare(pos1, n1, const C* s, n2) const;
    //   bool operator<(const C *s, const string<C,CT,A>& str);
    //   bool operator<(const string<C,CT,A>& str, const C *s);
    //   bool operator>(const C *s, const string<C,CT,A>& str);
    //   bool operator>(const string<C,CT,A>& str, const C *s);
    //   bool operator<=(const C *s, const string<C,CT,A>& str);
    //   bool operator<=(const string<C,CT,A>& str, const C *s);
    //   bool operator>=(const C *s, const string<C,CT,A>& str);
    //   bool operator>=(const string<C,CT,A>& str, const C *s);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    const TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
    (void) nullStr;

    if (veryVerbose) printf("\tcompare(s)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(nullStr));
        ASSERT_SAFE_PASS(X.compare(X.c_str()));
    }

    if (veryVerbose) printf("\tcompare(pos1, n1, s)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(0, X.size(), nullStr));
        ASSERT_SAFE_PASS(X.compare(0, X.size(), X.c_str()));
    }

    if (veryVerbose) printf("\tcompare(pos1, n1, s, n2)\n");

    {
        ASSERT_SAFE_FAIL(X.compare(0, X.size(), nullStr, X.size()));
        ASSERT_SAFE_PASS(X.compare(0, X.size(), X.c_str(), X.size()));
    }

    if (veryVerbose) printf("\toperator<\n");

    {
        ASSERT_SAFE_FAIL(if(X < nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr < X){});
        ASSERT_SAFE_PASS(if(X < X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() < X){});
    }

    if (veryVerbose) printf("\toperator>\n");

    {
        ASSERT_SAFE_FAIL(if(X > nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr > X){});
        ASSERT_SAFE_PASS(if(X > X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() > X){});
    }

    if (veryVerbose) printf("\toperator<=\n");

    {
        ASSERT_SAFE_FAIL(if(X <= nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr >= X){});
        ASSERT_SAFE_PASS(if(X <= X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() <= X){});
    }

    if (veryVerbose) printf("\toperator>=\n");

    {
        ASSERT_SAFE_FAIL(if(X >= nullStr){});
        ASSERT_SAFE_FAIL(if(nullStr >= X){});
        ASSERT_SAFE_PASS(if(X >= X.c_str()){});
        ASSERT_SAFE_PASS(if(X.c_str() >= X){});
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase23()
{
    // --------------------------------------------------------------------
    // TESTING SUBSTRING
    //
    // Concerns:
    //   1) That the 'substr' and 'copy' operations have the correct behavior
    //      and return value, cases where 'n' is smaller than, equal to, or
    //      larger than 'length() - pos'.
    //   2. That 'substr' and 'copy' throw 'std::out_of_range' when passed an
    //      out-of-bound position.
    //   3) That 'copy' does not overwrite beyond the buffer boundaries.
    //
    // Plan:
    //   For a set of string values, create the substring using the already
    //   tested constructors, and compare the results to those constructed
    //   substrings, with 'n' being either 0, 1 (smaller than 'length() - pos'
    //   whenever there are remaining characters), 'length() - pos' exactly,
    //   and 'length() + 1' and 'npos'.  Leave padding on both ends for the
    //   'copy' buffer and verify that padding has not been written into.
    //
    // Testing:
    //   string substr(pos, n) const;
    //   size_type copy(char *s, n, pos = 0) const;
    // ------------------------------------------------------------------------

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const int MAX_LEN = 128;
    TYPE buffer[MAX_LEN];

    const size_t npos = Obj::npos;  // note: NPOS is a system macro

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // specifications
    } DATA[] = {
        //line  spec                                       length
        //----  ----                                       ------
        { L_,   ""                                      }, //   0
        { L_,   "A"                                     }, //   1
        { L_,   "AB"                                    }, //   2
        { L_,   "ABC"                                   }, //   3
        { L_,   "ABCD"                                  }, //   4
        { L_,   "ABCDE"                                 }, //   5
        { L_,   "ABCDEA"                                }, //   6
        { L_,   "ABCDEAB"                               }, //   7
        { L_,   "ABCDEABC"                              }, //   8
        { L_,   "ABCDEABCD"                             }, //   9
        { L_,   "ABCDEABCDEA"                           }, //  11
        { L_,   "ABCDEABCDEAB"                          }, //  12
        { L_,   "ABCDEABCDEABCD"                        }, //  14
        { L_,   "ABCDEABCDEABCDE"                       }, //  15
        { L_,   "ABCDEABCDEABCDEA"                      }, //  16
        { L_,   "ABCDEABCDEABCDEAB"                     }, //  17
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEA"       }, //  31
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"      }, //  32
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABC"     }, //  33
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                      "ABCDEABCDEABCDEABCDEABCDEABCDEA" }, //  63
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEAB"  }, //  64
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEABC" }  //  65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting substr(pos, n).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\tOn a string of length " ZU ":\t", LENGTH); P(SPEC);
            }

            for (size_t i = 0; i <= LENGTH; ++i) {
                const Obj EXP1 = Obj(X, i, 1);
                const Obj EXPN = Obj(X, i, LENGTH);

                const Obj Y0 = X.substr(i, 0);
                LOOP2_ASSERT(LINE, i, Y0.empty());

                const Obj Y1 = X.substr(i, 1);
                LOOP2_ASSERT(LINE, i, EXP1 == Y1);

                const Obj YM = X.substr(i, LENGTH - i);
                LOOP2_ASSERT(LINE, i, EXPN == YM);

                const Obj YL = X.substr(i, LENGTH + 1);
                LOOP2_ASSERT(LINE, i, EXPN == YL);

                const Obj YN = X.substr(i, npos);
                LOOP2_ASSERT(LINE, i, EXPN == YN);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\t\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            bool outOfRangeCaught = false;
            try {
                const Obj Y = X.substr(LENGTH + 1, 0);
                ASSERT(0);
            }
            catch (const std::out_of_range&) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif

    if (verbose) printf("\tTesting copy(s, n, pos).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));
            ASSERT(LENGTH < MAX_LEN - 2);

            if (veryVerbose) {
                printf("\tOn a string of length " ZU ":\t", LENGTH); P(SPEC);
            }

            for (size_t i = 0; i <= LENGTH; ++i) {
                for (size_t j = 0; j <= LENGTH + 1; ++j) {
                    for (int k = 0; k < MAX_LEN; ++k) {
                        buffer[k] = DEFAULT_VALUE;
                    }

                    size_t ret = X.copy(buffer + 1, j, i);

                    size_t m = j < LENGTH - i ? j : LENGTH - i;
                    LOOP3_ASSERT(LINE, i, j, m == ret);

                    LOOP3_ASSERT(LINE, i, j, DEFAULT_VALUE == buffer[0]);
                    for (m = 0; m < j && m < LENGTH - i; ++m) {
                        LOOP4_ASSERT(LINE, i, j, m, buffer[1 + m] == X[i + m]);
                    }
                    LOOP3_ASSERT(LINE, i, j, ret == m);
                    LOOP3_ASSERT(LINE, i, j, DEFAULT_VALUE == buffer[1 + m]);
                }
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\t\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            const Obj X(g(SPEC));

            bool outOfRangeCaught = false;
            try {
                (void) X.copy(buffer + 1, MAX_LEN - 2, LENGTH + 1);
                ASSERT(0);
            }
            catch (const std::out_of_range&) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase23Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COPY:
    //
    // Concerns:
    //   1 'copy' asserts on undefined behavior when it's passed a NULL
    //   C-string pointer.
    //
    // Plan:
    //   Create a non-empty string and test 'copy' with a NULL C-string pointer
    //   parameter.
    //
    // Testing:
    //   size_type copy(char *s, n, pos = 0) const;
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tcopy(s, n, pos)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
        (void) nullStr;

        TYPE dest[10];
        ASSERT(sizeof dest / sizeof *dest > X.size());

        // characterString == NULL
        ASSERT_SAFE_FAIL(X.copy(nullStr, X.size(), 0));

        // pass
        ASSERT_SAFE_PASS(X.copy(dest, X.size(), 0));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase22()
{
    // --------------------------------------------------------------------
    // TESTING FIND VARIANTS
    //
    // Concerns:
    //   1) That the return value is correct, even in the presence of no or
    //      multiple occurrences of the search pattern.
    //   2) That passing a 'pos' argument that is out-of-bounds is not an
    //      error, and does not throw.
    //
    // Plan:
    //   For a set of carefully selected set of string and search pattern
    //   values, compare the value of 'find' and 'rfind' against expected
    //   return values.  For each string, exercise the special case of an empty
    //   pattern (the find operations always return the current position,
    //   except for 'find' when out-of-bounds: npos is returned instead).  Also
    //   exercise the special case of a single character (different signature),
    //   and verify that return value equals that of a brute-force
    //   computation.
    //
    //   For 'find_first_...' and 'find_last_...', use the Cartesian product of
    //   a set of strings and a set of search patterns, and compare the results
    //   to those of a brute-force computation.  Also exercise the special
    //   cases of a single-character pattern (different signature) and of an
    //   empty pattern (although redundant since the set of search patterns
    //   includes an empty string, this also enables us to test the correctness
    //   of our brute-force implementation in the boundary cases).
    //
    // Testing:
    //   size_type find(const string& str, pos = 0) const;
    //   size_type find(const C *s, pos, n) const;
    //   size_type find(const C *s, pos = 0) const;
    //   size_type find(C c, pos = 0) const;
    //   size_type rfind(const string& str, pos = 0) const;
    //   size_type rfind(const C *s, pos, n) const;
    //   size_type rfind(const C *s, pos = 0) const;
    //   size_type rfind(C c, pos = 0) const;
    //   size_type find_first_of(const string& str, pos = 0) const;
    //   size_type find_first_of(const C *s, pos, n) const;
    //   size_type find_first_of(const C *s, pos = 0) const;
    //   size_type find_first_of(C c, pos = 0) const;
    //   size_type find_last_of(const string& str, pos = 0) const;
    //   size_type find_last_of(const C *s, pos, n) const;
    //   size_type find_last_of(const C *s, pos = 0) const;
    //   size_type find_last_of(C c, pos = 0) const;
    //   size_type find_first_not_of(const string& str, pos = 0) const;
    //   size_type find_first_not_of(const C *s, pos, n) const;
    //   size_type find_first_not_of(const C *s, pos = 0) const;
    //   size_type find_first_not_of(C c, pos = 0) const;
    //   size_type find_last_not_of(const string& str, pos = 0) const;
    //   size_type find_last_not_of(const C *s, pos, n) const;
    //   size_type find_last_not_of(const C *s, pos = 0) const;
    //   size_type find_last_not_of(C c, pos = 0) const;
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const size_t npos = Obj::npos;  // note: NPOS is a system macro

    if (verbose) printf("\nTesting 'find' and 'rfind'.\n");
    {
        static const struct {
            int         d_lineNum;
            const char *d_spec;
            const char *d_pattern;
            size_t      d_exp;
            size_t      d_rexp;
        } DATA[] = {
            //line spec         pattern                   exp           rexp
            //---- ----         -------                   ---           ----
            { L_,  "",          "A",              (size_t)-1,   (size_t)-1,  },
            { L_,  "A",         "A",              (size_t) 0,   (size_t) 0,  },
            { L_,  "A",         "B",              (size_t)-1,   (size_t)-1,  },
            { L_,  "AABAA",     "B",              (size_t) 2,   (size_t) 2,  },
            { L_,  "ABABA",     "B",              (size_t) 1,   (size_t) 3,  },
            { L_,  "BAAAB",     "B",              (size_t) 0,   (size_t) 4,  },
            { L_,  "ABCDE",     "BCD",            (size_t) 1,   (size_t) 1,  },
            { L_,  "ABABA",     "ABA",            (size_t) 0,   (size_t) 2,  },
            { L_,  "ABACABA",   "ABA",            (size_t) 0,   (size_t) 4,  },
            { L_,  "ABABABAB",  "BABAB",          (size_t) 1,   (size_t) 3,  },
            { L_,  "ABABABAB",  "C",              (size_t)-1,   (size_t)-1,  },
            { L_,  "ABABABAB",  "ABABABAC",       (size_t)-1,   (size_t)-1,  },
            { L_,  "A",         "ABABA",          (size_t)-1,   (size_t)-1,  },
            { L_,  "AABAA",     "CDCDC",          (size_t)-1,   (size_t)-1,  },

            // Add further tests below, but note that test will fail if the
            // spec has the pattern in more than two occurrences.
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_lineNum;
            const char* const SPEC    = DATA[i].d_spec;
            const size_t      LENGTH  = strlen(SPEC);
            const char* const PATTERN = DATA[i].d_pattern;
            const size_t      N       = strlen(PATTERN);
            const size_t      EXP     = DATA[i].d_exp;
            const size_t      REXP    = DATA[i].d_rexp;

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and empty pattern.\n",
                       SPEC,
                       LENGTH);
                printf("\t\tExpecting 'find' and 'rfind' at each position.\n");
            }

            const Obj Z;

            LOOP2_ASSERT(LINE, SPEC, 0 == X.find(Z));
            LOOP2_ASSERT(LINE, SPEC, 0 == X.find(Z.c_str()));

            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str()));

            for (size_t j = 0; j <= LENGTH; ++j) {
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z, j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z.c_str(), j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.find(Z.c_str(), j, 0));

                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z, j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z.c_str(), j));
                LOOP3_ASSERT(LINE, SPEC, j, j == X.rfind(Z.c_str(), j, 0));
            }

            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z, LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z, npos));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), npos));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), LENGTH + 1, 0));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z, LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z, npos));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), LENGTH + 1));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), npos));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), LENGTH + 1,
                                                       0));
            LOOP2_ASSERT(LINE, SPEC, LENGTH == X.rfind(Z.c_str(), npos, 0));

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and every 'char'.\n",
                       SPEC,
                       LENGTH);
                printf("\t\tComparing with values computed ad hoc.\n");
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const TYPE C = VALUES[i];

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : LENGTH - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        if (TRAITS::eq(C, X[k])) {
                            exp[j] = k; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (EXP[j] < LENGTH &&
                                              C == X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        if (TRAITS::eq(C, X[k])) {
                            rExp[j] = k; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                               C == X[REXP[j]]));
                }

                LOOP2_ASSERT(LINE, SPEC, EXP[0]       == X.find(C));
                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C));

                for (size_t j = 0; j <= LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, SPEC, j, EXP[j]  == X.find(C, j));
                        LOOP3_ASSERT(LINE, SPEC, j, REXP[j] == X.rfind(C, j));
                }

                LOOP2_ASSERT(LINE, SPEC, npos == X.find(C, LENGTH + 1));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find(C, npos));

                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C, LENGTH +
                                                                           1));
                LOOP2_ASSERT(LINE, SPEC, REXP[LENGTH] == X.rfind(C, npos));

                delete[] exp;
                delete[] rExp;
            }

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU
                       " and pattern \"%s\".\n",
                       SPEC,
                       LENGTH,
                       PATTERN);
                printf("\t\tExpecting 'find' at " ZU
                       " and 'rfind' at " ZU ".\n",
                       EXP,
                       REXP);
            }

            const Obj Y(g(PATTERN));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,  EXP  == X.find(Y));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,  EXP  == X.find(Y.c_str()));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP, REXP == X.rfind(Y));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str()));

            if (EXP == npos) {
                ASSERT(EXP == REXP);
                for (size_t j = 0; j <= LENGTH + 2; ++j) {
                    LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                                 EXP == X.find(Y, j));
                    LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                                 EXP == X.find(Y.c_str(), j, N));
                }
                continue;
            }

            for (size_t j = 0; j < EXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.find(Y.c_str(), j, N));

                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             npos == X.rfind(Y.c_str(), j, N));
            }

            LOOP4_ASSERT(LINE, SPEC, PATTERN, EXP,
                         EXP  == X.find(Y, EXP));

            for (size_t j = EXP + 1; j <= REXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.find(Y.c_str(), j, N));
            }
            for (size_t j = EXP; j < REXP; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             EXP == X.rfind(Y.c_str(), j, N));
            }

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP, REXP == X.rfind(Y, REXP));

            for (size_t j = REXP + 1; j < LENGTH + 2; ++j) {
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y, j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, EXP,
                             npos == X.find(Y.c_str(), j, N));

                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y.c_str(), j));
                LOOP5_ASSERT(LINE, SPEC, PATTERN, j, REXP,
                             REXP == X.rfind(Y.c_str(), j, N));
            }
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y, npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y.c_str(), npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         npos == X.find(Y.c_str(), npos, N));

            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y, npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str(), npos));
            LOOP4_ASSERT(LINE, SPEC, PATTERN, REXP,
                         REXP == X.rfind(Y.c_str(), npos, N));

        }
    }

    if (verbose) printf("\nTesting 'find_first_...' and 'find_last_...'.\n");
    {
        static const struct {
            int         d_lineNum;
            const char *d_spec;
        } DATA[] = {
            //line spec
            //---- ----
            { L_,  ""                         },
            { L_,  "A"                        },
            { L_,  "B"                        },
            { L_,  "AB"                       },
            { L_,  "ABABABABAB"               },
            { L_,  "AAAAAABBBB"               },
            { L_,  "ABCDABCD"                 },
            { L_,  "ABCDEABCDE"               },
            { L_,  "AAAABBBBCCCCDDDDEEEE"     }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct {
            int         d_lineNum;
            const char *d_pattern;
        } PATTERNS[] = {
            //line pattern
            //---- -------
            { L_,  "",            },
            { L_,  "A",           },
            { L_,  "AAA",         },
            { L_,  "AB",          },
            { L_,  "ABC",         },
            { L_,  "ABCDE",       },
            { L_,  "B",           },
            { L_,  "E",           },
            { L_,  "F",           },
            { L_,  "FGHIJKL",     }
        };
        enum { NUM_PATTERNS = sizeof PATTERNS / sizeof *PATTERNS };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_lineNum;
            const char* const SPEC    = DATA[i].d_spec;
            const size_t      LENGTH  = strlen(SPEC);

            if (veryVerbose) {
                printf("\tWith SPEC: \"%s\" of length " ZU ".\n",
                       SPEC,
                       LENGTH);
            }

            const Obj X(g(SPEC));

            if (veryVerbose) {
                printf("\t\tWith empty pattern.\n");
                printf("\t\t\tExpecting 'find_.._not_of' at each position.\n");
            }

            const Obj Z;

            LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_of(Z));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_of(Z.c_str()));

            LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_of(Z));
            LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_of(Z.c_str()));

            if (LENGTH) {
                LOOP2_ASSERT(LINE, SPEC, 0 == X.find_first_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, 0 == X.find_first_not_of(Z.c_str()));

                LOOP2_ASSERT(LINE, SPEC, LENGTH - 1 == X.find_last_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, LENGTH - 1 == X.find_last_not_of(
                                                                   Z.c_str()));
            }
            else {
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_first_not_of(
                                                                   Z.c_str()));

                LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_not_of(Z));
                LOOP2_ASSERT(LINE, SPEC, npos == X.find_last_not_of(
                                                                   Z.c_str()));
            }

            for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_first_of(Z.c_str(), j, 0));

                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 npos == X.find_last_of(Z.c_str(), j, 0));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_first_not_of(Z.c_str(), j, 0));

                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z.c_str(), j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 j == X.find_last_not_of(Z.c_str(), j, 0));
            }

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_last_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_last_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_not_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         npos == X.find_first_not_of(Z.c_str(), npos, 0));

            LOOP2_ASSERT(LINE, SPEC,
                         LENGTH - 1 == X.find_last_not_of(Z, npos));
            LOOP2_ASSERT(LINE, SPEC,
                         LENGTH - 1 == X.find_last_not_of(Z.c_str(), npos, 0));

            if (veryVerbose) {
                printf("\t\tWith 'char' pattern.\n");
                printf("\t\t\tComparing with values computed ad hoc.\n");
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const TYPE C = VALUES[i];

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        if (!TRAITS::eq(C, X[k])) {
                            exp[j] = k; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] &&
                                              EXP[j] < LENGTH &&
                                              C != X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        if (!TRAITS::eq(C, X[k])) {
                            rExp[j] = k; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                               C != X[REXP[j]]));
                }

                LOOP2_ASSERT(LINE, SPEC, X.find(C)  == X.find_first_of(C));
                LOOP2_ASSERT(LINE, SPEC, X.rfind(C) == X.find_last_of(C));

                LOOP2_ASSERT(LINE, SPEC, EXP[0] == X.find_first_not_of(C));
                LOOP2_ASSERT(LINE, SPEC,
                             REXP[LENGTH] == X.find_last_not_of(C));

                for (size_t j = 0; j <= LENGTH + 2; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 X.find(C, j)  == X.find_first_of(C, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 X.rfind(C, j) == X.find_last_of(C, j));
                }

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 EXP[j]  == X.find_first_not_of(C, j));
                    LOOP3_ASSERT(LINE, SPEC, j,
                                 REXP[j] == X.find_last_not_of(C, j));
                }

                LOOP2_ASSERT(LINE, SPEC,
                             npos == X.find_first_not_of(C, LENGTH + 1));
                LOOP2_ASSERT(LINE, SPEC,
                             REXP[LENGTH] == X.find_last_not_of(C,
                                                                LENGTH + 1));

                delete[] exp;
                delete[] rExp;
            }

            for (int k = 0; k < NUM_PATTERNS; ++k) {
                const int         PLINE   = PATTERNS[k].d_lineNum;
                const char* const PATTERN = PATTERNS[k].d_pattern;
                const size_t      N       = strlen(PATTERN);
                const Obj Y(g(PATTERN));

                if (veryVerbose) {
                    printf("\t\tWith pattern \"%s\".\n", PATTERN);
                    printf("\t\t\tComparing with values computed ad hoc.\n");
                    printf("\t\t\tFor 'find_{first,last}_of'.\n");
                }

                size_t *exp  = new size_t[LENGTH + 1];
                const size_t *EXP  = exp;

                size_t *rExp = new size_t[LENGTH + 1];
                const size_t *REXP = rExp;

                size_t *expN  = new size_t[LENGTH + 1];
                const size_t *EXP_N  = expN;

                size_t *rExpN = new size_t[LENGTH + 1];
                const size_t *REXP_N = rExpN;

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH && EXP[j] == npos; ++k) {
                        for (size_t m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                exp[j] = k; expN[j] = m; break;
                            }
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] && EXP[j] < LENGTH &&
                                                    Y[EXP_N[j]] == X[EXP[j]]));
                    for (int k = static_cast<int>(lastJ);
                         k >= 0 && REXP[j] == npos;
                         --k) {
                        for (size_t m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                rExp[j] = k; rExpN[j] = m; break;
                            }
                        }
                    }
                    ASSERT(npos == REXP[j] || (REXP[j] <= lastJ &&
                                                  Y[REXP_N[j]] == X[REXP[j]]));
                }

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_of(Y.c_str()));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_of(Y.c_str()));

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP[j] == X.find_first_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_of(Y.c_str(), j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_of(Y.c_str(), j, N));

                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y.c_str(), j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_of(Y.c_str(), j, N));
                }

                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y, npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y.c_str(), npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_of(Y.c_str(), npos, N));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y, npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y.c_str(), npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_of(Y.c_str(),
                                                            npos,
                                                            N));

                if (veryVerbose)
                    printf("\t\t\tFor 'find_{first,last}_not_of'.\n");

                for (size_t j = 0; j <= LENGTH; ++j) {
                    size_t lastJ = j < LENGTH ? j : j - 1;
                    exp[j] = rExp[j] = npos;
                    for (size_t k = j; k < LENGTH; ++k) {
                        size_t m;
                        for (m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                break;
                            }
                        }
                        if (m == N) {
                            exp[j] = k; expN[j] = m; break;
                        }
                    }
                    ASSERT(npos == EXP[j] || (j <= EXP[j] && EXP[j] < LENGTH));
                    for (int k = static_cast<int>(lastJ); k >= 0; --k) {
                        size_t m;
                        for (m = 0; m < N; ++m) {
                            if (TRAITS::eq(Y[m], X[k])) {
                                break;
                            }
                        }
                        if (m == N) {
                            rExp[j] = k; rExpN[j] = m; break;
                        }
                    }
                    ASSERT(npos == REXP[j] || REXP[j] <= lastJ);
                }

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_not_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, EXP[0],
                             EXP[0]  == X.find_first_not_of(Y.c_str()));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_not_of(Y));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[0],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str()));

                for (size_t j = 0; j <= LENGTH; ++j) {
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP[j] == X.find_first_not_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                                 EXP [j]== X.find_first_not_of(Y.c_str(), j));
                    LOOP6_ASSERT(
                               LINE, PLINE, SPEC, PATTERN, j, EXP[j],
                               EXP [j]== X.find_first_not_of(Y.c_str(), j, N));

                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_not_of(Y, j));
                    LOOP6_ASSERT(LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                                 REXP[j] == X.find_last_not_of(Y.c_str(), j));
                    LOOP6_ASSERT(
                               LINE, PLINE, SPEC, PATTERN, j, REXP[j],
                               REXP[j] == X.find_last_not_of(Y.c_str(), j, N));
                }

                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y, npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y.c_str(), npos));
                LOOP4_ASSERT(LINE, PLINE, SPEC, PATTERN,
                             npos == X.find_first_not_of(Y.c_str(), npos, N));

                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y, npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str(),
                                                                npos));
                LOOP5_ASSERT(LINE, PLINE, SPEC, PATTERN, REXP[LENGTH],
                             REXP[LENGTH] == X.find_last_not_of(Y.c_str(),
                                                                npos,
                                                                N));

                delete[] exp;
                delete[] expN;
                delete[] rExp;
                delete[] rExpN;
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase22Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING COPY:
    //
    // Concerns:
    //   'find' asserts on undefined behavior when it's passed a NULL C-string
    //   pointer.
    //
    // Plan:
    //   For each variant of the 'find...' method, create a non-empty string
    //   and test the 'find' method with a NULL C-string pointer parameter.
    //
    // Testing:
    //   size_type find(const C *s, pos, n) const;
    //   size_type find(const C *s, pos = 0) const;
    //   size_type rfind(const C *s, pos, n) const;
    //   size_type rfind(const C *s, pos = 0) const;
    //   size_type find_first_of(const C *s, pos, n) const;
    //   size_type find_first_of(const C *s, pos = 0) const;
    //   size_type find_last_of(const C *s, pos, n) const;
    //   size_type find_last_of(const C *s, pos = 0) const;
    //   size_type find_first_not_of(const C *s, pos, n) const;
    //   size_type find_first_not_of(const C *s, pos = 0) const;
    //   size_type find_last_not_of(const C *s, pos, n) const;
    //   size_type find_last_not_of(const C *s, pos = 0) const;
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    const TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
    (void) nullStr;

    if (veryVerbose) printf("\tfind(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find(nullStr, 0));
        ASSERT_SAFE_PASS(X.find(X.c_str(), 0));
    }

    if (veryVerbose) printf("\trfind(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.rfind(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.rfind(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\trfind(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.rfind(nullStr, 0));
        ASSERT_SAFE_PASS(X.rfind(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_first_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_first_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_first_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_first_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_last_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_last_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_last_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_last_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_first_not_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_not_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_first_not_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_first_not_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_first_not_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_first_not_of(X.c_str(), 0));
    }

    if (veryVerbose) printf("\tfind_last_not_of(s, pos, n)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_not_of(nullStr, 0, X.size()));
        ASSERT_SAFE_PASS(X.find_last_not_of(X.c_str(), 0, X.size()));
    }

    if (veryVerbose) printf("\tfind_last_not_of(s, pos)\n");

    {
        ASSERT_SAFE_FAIL(X.find_last_not_of(nullStr, 0));
        ASSERT_SAFE_PASS(X.find_last_not_of(X.c_str(), 0));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                              testCase21_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
        "This is very long string which must allocate",
        "This is another very long string which must allocate"
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC = SPECS[ti];

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj mW(ISPEC, scratch);  const Obj& W = mW;  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            const Obj mZ(JSPEC, scratch);  const Obj& Z = mZ;  // control

            for (int member = 0; member < 2; ++member) {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                ASSERTV(ISPEC, JSPEC, Z, X, Z == X);
                ASSERTV(ISPEC, JSPEC, W, Y, W == Y);

                if (member) {
                    mX.swap(mY);
                }
                else {
                    swap(mX, mY);
                }

                ASSERTV(ISPEC, JSPEC, W, X, W == X);
                ASSERTV(ISPEC, JSPEC, Z, Y, Z == Y);

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mas == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mat == Y.get_allocator());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mat == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, PROPAGATE, mas == Y.get_allocator());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE, TRAITS, ALLOC>::testCase21_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the
    //:   allocators used by the source and target objects remain unchanged
    //:   (i.e., the allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the
    //:   allocator used by the target (source) object is updated to be a copy
    //:   of that used by the source (target) object (i.e., the allocators
    //:   *are* exchanged).
    //:
    //: 3 If the allocators are propagated (i.e., exchanged), there is no
    //:   additional allocation from any allocator.
    //:
    //: 4 The effect of the 'propagate_on_container_swap' trait is independent
    //:   of the other three allocator propagation traits.
    //:
    //: 5 Following the swap operation, neither object holds on to memory
    //:   allocated from the other object's allocator.
    //
    // Plan:
    //: 1 Specify a set S of object values with distinct values, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'Z' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Using both member 'swap' and free function 'swap', swap 'X' with
    //:     'Y' and use 'operator==' to verify that 'X' and 'Y' have the
    //:     expected values.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocators of 'X'
    //:     and 'Y' are *not* exchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocators of 'X' and 'Y'
    //:   *are* exchanged.  Also verify that there is no additional allocation
    //:   from any allocator.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------

    if (verbose) printf("\t'propagate_on_container_swap::value == false'\n");

    testCase21_propagate_on_container_swap_dispatch<false, false>();
    testCase21_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\t'propagate_on_container_swap::value == true'\n");

    testCase21_propagate_on_container_swap_dispatch<true, false>();
    testCase21_propagate_on_container_swap_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase21()
{
    // --------------------------------------------------------------------
    // TESTING SWAP
    //
    // Concerns:
    //   1) Swapping containers does not swap allocators.
    //   2) Swapping containers with same allocator results in no allocation
    //      or deallocation operations.
    //   3) Swapping containers with different allocators does result in
    //      allocation and deallocation operations.
    //   4) Swap free function works the same way as the 'swap' method.
    //   5) Swap works correctly for the short string optimization (swapping
    //      short and long strings).
    //
    // Plan:
    //   Construct 'str1' and 'str2' with different test allocators.
    //   Add data to 'str1'.  Remember allocation statistics.
    //   Swap 'str1' and 'str2'.
    //   Verify that contents were swapped.
    //   Verify that allocators for each are unchanged.
    //   Verify that allocation statistics changed for each test allocator.
    //   Create a 'str3' with same allocator as 'str2'.
    //   Swap 'str2' and 'str3'
    //   Verify that contents were swapped.
    //   Verify that allocation statistics did not change.
    //   Let 'str3' got out of scope.
    //   Verify that memory was returned to allocator.
    //   Construct two strings, apply a free function swap to them and verify
    //   the result.
    //   Construct short and long strings, swap them with each other and verify
    //   the result.
    //
    // Testing:
    //   swap(string<C,CT,A>& rhs);                       // method
    //   swap(string<C,CT,A>& lhs, string<C,CT,A>& rhs);  // free function
    // ------------------------------------------------------------------------

    const size_t LENGTH = DEFAULT_CAPACITY * 2;

    {
        bslma::TestAllocator testAlloc2(veryVeryVerbose);
        ASSERT(0 == testAlloc2.numBytesInUse());

        Obj str1(g(LENGTH, TYPE('0')));
        Obj str1cpy(str1);
        Obj str2(&testAlloc2);

        if (verbose) printf("Swap strings with unequal allocators.\n");

        str1.swap(str2);

        ASSERT(0  == str1.size());
        ASSERT(LENGTH == str2.size());
        ASSERT(str1cpy == str2);

        ASSERT(bslma::Default::defaultAllocator() == str1.get_allocator());
        ASSERT(&testAlloc2 == str2.get_allocator());


        if (verbose) printf("Swap strings with equal allocators.\n");

        {
            const Int64 inUse2 = testAlloc2.numBytesInUse();
            Obj str3(&testAlloc2);
            ASSERT(testAlloc2.numBytesInUse() == inUse2);

            {
                AllocatorUseGuard guard(&testAlloc2);

                str3.swap(str2);
                ASSERT(str2.empty());
                ASSERT(LENGTH == str3.size());
                ASSERT(str1cpy == str3);
            }
        }

        // Destructor for str3 should have freed memory
        ASSERT(0 == testAlloc2.numBytesInUse());
    }

    if (verbose) printf("Swap free function.\n");

    {
        Obj str1(g(LENGTH, TYPE('0')));
        Obj str1cpy(str1);

        Obj str2(g(LENGTH, TYPE('9')));
        Obj str2cpy(str2);

        using bsl::swap;
        swap(str1, str2);

        ASSERT(str1 == str2cpy);
        ASSERT(str2 == str1cpy);
    }

    if (verbose) printf("Swap and short string optimization.\n");

    {
        if (veryVerbose) printf("    short <-> short\n");

        {
            Obj shortStr1(g(DEFAULT_CAPACITY, TYPE('0')));
            Obj shortStr1Cpy(shortStr1);

            Obj shortStr2(g(DEFAULT_CAPACITY, TYPE('9')));
            Obj shortStr2Cpy(shortStr2);

            shortStr1.swap(shortStr2);

            ASSERT(shortStr1 == shortStr2Cpy);
            ASSERT(shortStr2 == shortStr1Cpy);
        }

        if (veryVerbose) printf("    short <-> long\n");

        {
            Obj shortStr(g(DEFAULT_CAPACITY, TYPE('0')));
            Obj shortStrCpy(shortStr);

            Obj longStr(g(DEFAULT_CAPACITY * 2, TYPE('9')));
            Obj longStrCpy(longStr);

            shortStr.swap(longStr);

            ASSERT(shortStr == longStrCpy);
            ASSERT(longStr == shortStrCpy);
        }

        if (veryVerbose) printf("     long <-> short\n");

        {
            Obj longStr(g(DEFAULT_CAPACITY * 2, TYPE('0')));
            Obj longStrCpy(longStr);

            Obj shortStr(g(DEFAULT_CAPACITY, TYPE('9')));
            Obj shortStrCpy(shortStr);

            longStr.swap(shortStr);

            ASSERT(longStr == shortStrCpy);
            ASSERT(shortStr == longStrCpy);
        }

        if (veryVerbose) printf("     long <-> long\n");

        {
            Obj longStr1(g(DEFAULT_CAPACITY * 2, TYPE('0')));
            Obj longStr1Cpy(longStr1);

            Obj longStr2(g(DEFAULT_CAPACITY * 2, TYPE('9')));
            Obj longStr2Cpy(longStr2);

            longStr1.swap(longStr2);

            ASSERT(longStr1 == longStr2Cpy);
            ASSERT(longStr2 == longStr1Cpy);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20()
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'replace' return value is a reference to self.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   The plan is similar to 'insert' (case 17) with two nested loops for
    //   the beginning and end of the replace range (instead of only one for
    //   the insert position).  Since both 'erase' and 'insert' have been
    //   tested, and conceptually replace is equivalent to 'erase' followed by
    //   'insert', is suffices to perform 'replace' using this alternate method
    //   and compare the resulting strings.
    //
    // Testing:
    //   string& replace(pos1, n1, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        REPLACE_CHAR_MODE_FIRST    = 0,
        REPLACE_CHAR_N_AT_INDEX    = 0,
        REPLACE_CHAR_N_AT_ITERATOR = 1,
        REPLACE_CHAR_MODE_LAST     = 1
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,                  0     },
        { L_,                  1     },
        { L_,                  2     },
        { L_,                  3     },
        { L_,                  4     },
        { L_,                  5     },
        { L_,                  9     },
        { L_,   DEFAULT_CAPACITY - 1 },  // May result in duplicate test values
        { L_,   DEFAULT_CAPACITY,    },  // for wide strings.
        { L_,   DEFAULT_CAPACITY + 1 },
        { L_,   DEFAULT_CAPACITY * 5 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'replace'.\n");

    for (int replaceMode  = REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', replaceMode = %d.\n",
                   replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               ALLOC(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(SIZE);
                            printf("elements between "); P_(BEGIN); P_(END);
                            printf("using "); P_(NUM_ELEMENTS); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (replaceMode) {
                          case REPLACE_CHAR_N_AT_ITERATOR: {
                            //   void replace(iterator p, iterator q,
                            //                size_type n, C c);
                            Obj& result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          case REPLACE_CHAR_N_AT_INDEX: {
                            // string& replace(pos1, n1, n2, C c);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          default: {
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     LENGTH == X.size());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     CAP == X.capacity());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     BB + REALLOC == AA);
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     B + A_ALLOC ==  A);
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  =  REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   ALLOC(&testAllocator));  const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (replaceMode) {
                              case REPLACE_CHAR_N_AT_INDEX: {
                                // string& replace(pos1, n1, n2, C c);
                                Obj &result = mX.replace(BEGIN, SIZE,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, SIZE,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              case REPLACE_CHAR_N_AT_ITERATOR: {
                                // void replace(iterator p, iterator q,
                                //              size_type n2, C c);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, END,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              default: {
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                              } break;
                            }
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                             EXP == X);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20MatchTypes()
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    // We have the following concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'replace' return value is a reference to self.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   The plan is similar to 'insert' (case 17) with two nested loops
    //   for the beginning and end of the replace range (instead of only one
    //   for the insert position).  Since both 'erase' and 'insert' have been
    //   tested, and conceptually replace is equivalent to 'erase' followed by
    //   'insert', is suffices to perform 'replace' using this alternate method
    //   and compare the resulting strings.
    //
    // Testing:
    //   string& replace(pos1, n1, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        REPLACE_CHAR_MODE_FIRST    = 0,
        REPLACE_CHAR_N_AT_INDEX    = 0,
        REPLACE_CHAR_N_AT_ITERATOR = 1,
        REPLACE_CHAR_MODE_LAST     = 1
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       18   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       32   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'replace'.\n");

    for (int replaceMode  = REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', replaceMode = %d.\n",
                   replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = std::min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               ALLOC(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(SIZE);
                            printf("elements between "); P_(BEGIN); P_(END);
                            printf("using "); P_(NUM_ELEMENTS); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (replaceMode) {
                          case REPLACE_CHAR_N_AT_ITERATOR: {
                            //   void replace(iterator p, iterator q,
                            //                size_type n, C c);
                            Obj& result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          case REPLACE_CHAR_N_AT_INDEX: {
                            // string& replace(pos1, n1, n2, C c);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     NUM_ELEMENTS,
                                                     VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, ti, &X == &result);
                          } break;
                          default: {
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     LENGTH == X.size());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     CAP == X.capacity());
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     BB + REALLOC == AA);
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                     B + A_ALLOC ==  A);
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  =  REPLACE_CHAR_MODE_FIRST;
             replaceMode <= REPLACE_CHAR_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   ALLOC(&testAllocator));  const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, NUM_ELEMENTS, VALUE);

                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (replaceMode) {
                              case REPLACE_CHAR_N_AT_INDEX: {
                                // string& replace(pos1, n1, n2, C c);
                                Obj &result = mX.replace(BEGIN, SIZE,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, SIZE,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              case REPLACE_CHAR_N_AT_ITERATOR: {
                                // void replace(iterator p, iterator q,
                                //              size_type n2, C c);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         NUM_ELEMENTS,
                                                         VALUE);
                                LOOP4_ASSERT(INIT_LINE, LINE, BEGIN, END,
                                             &X == &result);
                                checkResultFlag = true;
                              } break;
                              default: {
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                              } break;
                            }

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, END, SIZE,
                                             EXP == X);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING REPLACE:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the return value is a reference to self.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly so that
    //      *all* internally allocated memory draws from a user-supplied
    //      allocator whenever one is specified.
    //
    // Plan:
    //   See 'testCase20'.
    //
    // Testing:
    //   string& replace(pos1, n1, const string& str);
    //   string& replace(pos1, n1, const string& str, pos2, n2 = npos);
    //   string& replace(pos1, n1, const C *s, n2);
    //   string& replace(pos1, n1, const C *s);
    //   replace(const_iterator first, const_iterator last, const C *s, n2);
    //   replace(const_iterator first, const_iterator last, const C *s);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    enum {
        REPLACE_STRING_MODE_FIRST        = 0,
        REPLACE_SUBSTRING_AT_INDEX       = 0,
        REPLACE_SUBSTRING_AT_INDEX_NPOS  = 1,
        REPLACE_STRING_AT_INDEX          = 2,
        REPLACE_CSTRING_N_AT_INDEX       = 3,
        REPLACE_CSTRING_AT_INDEX         = 4,
        REPLACE_STRING_AT_ITERATOR       = 5,
        REPLACE_CONST_STRING_AT_ITERATOR = 6,
        REPLACE_CSTRING_N_AT_ITERATOR    = 7,
        REPLACE_CSTRING_AT_ITERATOR      = 8,
        REPLACE_STRING_MODE_LAST         = 8
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int replaceMode  = REPLACE_STRING_AT_INDEX;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace "); P_(NUM_ELEMENTS);
                            printf("between "); P_(BEGIN); P_(END);
                            printf("using "); P(SPEC);
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, Y);

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (replaceMode) {
                          case REPLACE_STRING_AT_INDEX: {
                            // string& replace(pos1, n1, const string& str);
                            Obj &result = mX.replace(BEGIN, SIZE, Y);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_N_AT_INDEX: {
                            // string& replace(pos1, n1, const C *s, n2);
                            Obj &result = mX.replace(BEGIN,
                                                     SIZE,
                                                     Y.data(),
                                                     NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_AT_INDEX: {
                            // string& replace(pos1, n1, const C *s);
                            Obj &result = mX.replace(BEGIN, SIZE, Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_N_AT_ITERATOR: {
                            // string& replace(iterator p, q, const C *s);
                            Obj &result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     Y.data(),
                                                     NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_CSTRING_AT_ITERATOR: {
                            // string& replace(iterator p, q, const C *s);
                            Obj &result = mX.replace(mX.begin() + BEGIN,
                                                     mX.begin() + END,
                                                     Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case REPLACE_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            //   void replace(iterator p, iterator q,
                            //                InputIter first, last);
                            mX.replace(mX.begin() + BEGIN,
                                       mX.begin() + END,
                                       mU.begin(),
                                       mU.end());
                          } break;
                          case REPLACE_CONST_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            //   void replace(iterator p, iterator q,
                            //                InputIter first, last);
                            mX.replace(mX.begin() + BEGIN,
                                       mX.begin() + END,
                                       U.begin(),
                                       U.end());
                          } break;
                          default: {
                            printf("***UNKNOWN REPLACE MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         CAP == X.capacity());
                        }
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, BEGIN, END, SIZE,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, ti,
                                     B + A_ALLOC <=  A);
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with replaceMode = %d.\n",
                        REPLACE_SUBSTRING_AT_INDEX);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t BEGIN = b;
                        const size_t SIZE  = s;
                        const size_t END   = min(b + s, INIT_LENGTH);
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS -
                                                                 (END - BEGIN);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        Obj mExp(X);  const Obj& EXP = mExp;
                        mExp.erase(mExp.begin() + BEGIN, mExp.begin() + END);
                        mExp.insert(BEGIN, Y, POS2, NUM_ELEMENTS);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tReplace"); P_(NUM_ELEMENTS_INS);
                            printf("between "); P_(BEGIN); P_(END);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& replace(pos1, n1, const string& str,
                        //                 pos2, n2);
                        Obj &result = mX.replace(BEGIN, SIZE,
                                                 Y, POS2, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, POS2,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, POS2,
                                         CAP == X.capacity());
                        }
                        LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                     EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     BEGIN, END, BB + REALLOC <= AA);
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     BEGIN, END,  B + A_ALLOC <=  A);
                    }
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int replaceMode  = REPLACE_STRING_MODE_FIRST;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                    for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                        const size_t BEGIN  = b;
                        const size_t SIZE   = s;
                        const size_t END    = min(b + s, INIT_LENGTH);

                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS -
                                                                 (END - BEGIN);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);

                            Obj mExp(X);  const Obj& EXP = mExp;
                            mExp.erase(mExp.begin() + BEGIN,
                                       mExp.begin() + END);
                            mExp.insert(BEGIN, Y);

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            switch (replaceMode) {
                              case REPLACE_STRING_AT_INDEX: {
                                // string& replace(pos1, n1,
                                //                 const string& str);
                                Obj &result = mX.replace(BEGIN, SIZE, Y);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_SUBSTRING_AT_INDEX: {
                                // string& replace(pos1, n1, const string& str,
                                //                 pos2, n2);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y,
                                                         0,
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_SUBSTRING_AT_INDEX_NPOS: {
                                // string& replace(pos1, n1, const string& str,
                                //                 pos2, n2);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y,
                                                         0);
                                                         // 'npos' dflt. arg.
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_N_AT_INDEX: {
                                // string& replace(pos1, n1, const C *s, n2);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y.data(),
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_AT_INDEX: {
                                // string& replace(pos1, n1, const C *s);
                                Obj &result = mX.replace(BEGIN,
                                                         SIZE,
                                                         Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_N_AT_ITERATOR: {
                                // replace(const_iterator p, q, const C *s);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         Y.data(),
                                                         NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_CSTRING_AT_ITERATOR: {
                                // replace(const_iterator p, q, const C *s);
                                Obj &result = mX.replace(mX.begin() + BEGIN,
                                                         mX.begin() + END,
                                                         Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case REPLACE_STRING_AT_ITERATOR: {
                                // template <class InputIter>
                                // replace(const_iterator p, const_iterator q,
                                //         InputIter first, last);
                                mX.replace(mX.begin() + BEGIN,
                                           mX.begin() + END,
                                           mU.begin(),
                                           mU.end());
                              } break;
                              case REPLACE_CONST_STRING_AT_ITERATOR: {
                                // template <class InputIter>
                                // replace(const_iterator p, const_iterator q,
                                //         InputIter first, last);
                                mX.replace(mX.begin() + BEGIN,
                                           mX.begin() + END,
                                           U.begin(),
                                           U.end());
                              } break;
                              default: {
                                printf("***UNKNOWN REPLACE MODE***\n");
                                ASSERT(0);
                              } break;
                            }
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         LENGTH == X.size());
                            LOOP5_ASSERT(INIT_LINE, LINE, BEGIN, SIZE, END,
                                         EXP == X);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int replaceMode  = REPLACE_STRING_MODE_FIRST;
             replaceMode <= REPLACE_STRING_MODE_LAST;
             ++replaceMode)
    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d.\n", replaceMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    const size_t BEGIN = b;
                    const size_t SIZE  = s;
                    const size_t END   = min(b + s, INIT_LENGTH);

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    for (size_t k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    Obj mY(X); const Obj& Y = mY;  // control

                    if (veryVerbose) {
                        printf("\t\t\tReplace with "); P_(Y);
                        printf(" between "); P_(BEGIN); P_(END);
                    }

                    switch (replaceMode) {
                      case REPLACE_STRING_AT_INDEX: {
                        // string& replace(pos1, n1, const string& str);
                        mX.replace(BEGIN, SIZE, Y);
                        mY.replace(BEGIN, SIZE, Y);
                      } break;
                      case REPLACE_CSTRING_N_AT_INDEX: {
                        // string& replace(pos1, n1, const C *s, n);
                        mX.replace(BEGIN, SIZE, Y.data(), INIT_LENGTH);
                        mY.replace(BEGIN, SIZE, Y.data(), INIT_LENGTH);
                      } break;
                      case REPLACE_CSTRING_AT_INDEX: {
                        // string& replace(pos1, n1, const C *s);
                        mX.replace(BEGIN, SIZE, Y.c_str());
                        mY.replace(BEGIN, SIZE, Y.c_str());
                      } break;
                      case REPLACE_SUBSTRING_AT_INDEX: {
                        // string& replace(pos1, n1, const string& str,
                        //                 pos2, n);
                        mX.replace(BEGIN, SIZE, Y, 0, INIT_LENGTH);
                        mY.replace(BEGIN, SIZE, Y, 0, INIT_LENGTH);
                      } break;
                      case REPLACE_SUBSTRING_AT_INDEX_NPOS: {
                        // string& replace(pos1, n1, const string& str,
                        //                 pos2, n);
                        mX.replace(BEGIN, SIZE, Y, 0);  // 'npos' default arg.
                        mY.replace(BEGIN, SIZE, Y, 0);
                      } break;
                      case REPLACE_STRING_AT_ITERATOR: {
                        // replace(const_iterator p, q, InputIter first, last);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   mY.begin(), mY.end());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   mY.begin(), mY.end());
                      } break;
                      case REPLACE_CONST_STRING_AT_ITERATOR: {
                        // replace(const_iterator p, q, InputIter first, last);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.begin(), Y.end());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.begin(), Y.end());
                      } break;
                      case REPLACE_CSTRING_N_AT_ITERATOR: {
                        // replace(const_iterator p, q, const C *s, n);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.data(), INIT_LENGTH);
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.data(), INIT_LENGTH);
                      } break;
                      case REPLACE_CSTRING_AT_ITERATOR: {
                        // string& replace(pos, const C *s);
                        mX.replace(mX.begin() + BEGIN, mX.begin() + END,
                                   Y.c_str());
                        mY.replace(mY.begin() + BEGIN, mY.begin() + END,
                                   Y.c_str());
                      } break;
                      default: {
                        printf("***UNKNOWN REPLACE MODE***\n");
                        ASSERT(0);
                      } break;
                    }

                    if (veryVerbose) {
                        T_; T_; T_; T_; P(X);
                        T_; T_; T_; T_; P(Y);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, BEGIN, SIZE, X == Y);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        if (verbose)
            printf("\t\tUsing string with replaceMode = %d (complete).\n",
                   REPLACE_SUBSTRING_AT_INDEX);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t b = 0; b <= INIT_LENGTH; ++b) {
                for (size_t s = 0; s <= INIT_LENGTH; ++s) {
                    const size_t BEGIN = b;
                    const size_t SIZE  = s;
                    const size_t END   = min(b + s, INIT_LENGTH);

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        for (size_t m = 0; m < INIT_LENGTH; ++m) {
                            const size_t INDEX        = h;
                            const size_t NUM_ELEMENTS = m;

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            for (size_t k = 0; k < INIT_LENGTH; ++k) {
                                mX[k] = VALUES[k % NUM_VALUES];
                            }

                            Obj mY(X); const Obj& Y = mY;  // control

                            if (veryVerbose) {
                                printf("\t\t\tInsert substring of itself");
                                printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                                printf("between "); P_(BEGIN); P(END);
                            }

                            if (INDEX + NUM_ELEMENTS >= Y.length()) {
                                mX.replace(BEGIN, SIZE, Y, INDEX);  // 'npos'
                                                                    // default
                                                                    // argument
                                mY.replace(BEGIN, SIZE, Y, INDEX);
                            }
                            else {
                                mX.replace(BEGIN, SIZE, Y, INDEX,NUM_ELEMENTS);
                                mY.replace(BEGIN, SIZE, Y, INDEX,NUM_ELEMENTS);
                            }

                            if (veryVerbose) {
                                T_; T_; T_; T_; P(X);
                                T_; T_; T_; T_; P(Y);
                            }

                            LOOP6_ASSERT(INIT_LINE, INIT_CAP, BEGIN, SIZE,
                                         INDEX, NUM_ELEMENTS, X == Y);
                        }
                    }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase20Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING REPLACE:
    //
    // Concerns:
    //   1 'replace' asserts on undefined behavior when it's passed either a
    //   NULL C-string pointer, invalid iterators, or invalid iterator ranges.
    //
    // Plan:
    //   For each 'replace' overload create a non-empty string and test
    //   'replace' with different combinations of invalid parameters.
    //
    // Testing:
    //   replace(const_iterator first, const_iterator last, size_type n2, C c);
    //   replace(const_iterator first, const_iterator last, const string& str);
    //   replace(const_iterator first, const_iterator last, InputIter f, l);
    //   replace(pos1, n1, const C *s);
    //   replace(pos1, n1, const C *s, n2);
    //   replace(const_iterator first, const_iterator last, const C *s);
    //   replace(const_iterator first, const_iterator last, const C *s, n2);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\treplase(first, last, n, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), 1, X[0]));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), 1, X[0]));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, 1, X[0]));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, 1, X[0]));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), 1, X[0]));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), 1, X[0]));
    }

    if (veryVerbose) printf("\treplace(first, last, str)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("AB"));    // replacement

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), mY));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), mY));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, mY));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, mY));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), mY));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), mY));
    }

    if (veryVerbose) printf("\treplace(first, last, f, l)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("AB"));    // replacement
        const Obj& Y = mY;

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(),
                                    mY.begin(), mY.end()));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(),
                                    mY.begin(), mY.end()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1,
                                    mY.begin(), mY.end()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1,
                                    mY.begin(), mY.end()));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(),
                                    mY.begin(), mY.end()));

        // stringFirst > stringLast (non-'const' iterators)
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(),
                                    mY.end(), mY.begin()));

        // stringFirst > stringLast (const iterators)
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(),
                                    Y.end(), Y.begin()));

        // pass (non-'const' iterators)
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(),
                                    mY.begin(), mY.end()));

        // pass (const iterators)
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(),
                                    Y.begin(), Y.end()));
    }

    if (veryVerbose) printf("\treplace(pos1, n1, s)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(0, X.size(), NULL));

        // pass
        ASSERT_SAFE_PASS(mX.replace(0, X.size(), X.c_str()));
    }

    if (veryVerbose) printf("\treplace(pos1, n1, s, n2)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
        (void) nullStr;

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(0, X.size(), nullStr, 10));

        // pass
        ASSERT_SAFE_PASS(mX.replace(0, X.size(), X.c_str(), X.size()));
    }

    if (veryVerbose) printf("\treplace(first, last, s)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
       (void) nullStr;

        // first < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin() - 1, X.end(), X.c_str()));

        // first > end()
        ASSERT_SAFE_FAIL(mX.replace(X.end() + 1, X.end(), X.c_str()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.begin() - 1, X.c_str()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end() + 1, X.c_str()));

        // first > last
        ASSERT_SAFE_FAIL(mX.replace(X.begin() + 1, X.begin(), X.c_str()));

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(), nullStr));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), X.c_str()));
    }

    if (veryVerbose) printf("\treplace(first, last, s, n2)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        const TYPE *nullStr = NULL;
        // disable "unused variable" warning in non-safe mode:
        (void) nullStr;

        // first < begin()
        ASSERT_SAFE_FAIL(
                      mX.replace(X.begin() - 1, X.end(), X.c_str(), X.size()));

        // first > end()
        ASSERT_SAFE_FAIL(
                        mX.replace(X.end() + 1, X.end(), X.c_str(), X.size()));

        // last < begin()
        ASSERT_SAFE_FAIL(
                    mX.replace(X.begin(), X.begin() - 1, X.c_str(), X.size()));

        // last > end()
        ASSERT_SAFE_FAIL(
                      mX.replace(X.begin(), X.end() + 1, X.c_str(), X.size()));

        // first > last
        ASSERT_SAFE_FAIL(
                    mX.replace(X.begin() + 1, X.begin(), X.c_str(), X.size()));

        // characterString == NULL
        ASSERT_SAFE_FAIL(mX.replace(X.begin(), X.end(), nullStr, X.size()));

        // pass
        ASSERT_SAFE_PASS(mX.replace(X.begin(), X.end(), X.c_str(), X.size()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase19()
{
    // --------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concerns:
    //   1) That the resulting value is correct.
    //   2) That erasing a suffix of the array never allocates, and thus never
    //      throws.  In particular, 'pop_back()' and 'erase(..., X.end())' do
    //      not throw.
    //   3) That erasing is exception neutral w.r.t. memory allocation.
    //   4) That erasing does not modify the capacity (i.e., shrink).
    //   5) That no memory is leaked.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a string
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but not computing the number of allocations or checking on the
    //        value in case an exception is thrown (it is enough to verify that
    //        all the elements have been destroyed indirectly by making sure
    //        that there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   void pop_back();
    //   string& erase(size_type pos, size_type n);
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'pop_back' on non-empty strings.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH       = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                const Obj& X = mX;
                mX.reserve(INIT_RES);
                const size_t INIT_CAP = X.capacity();

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] =  VALUES[k % NUM_VALUES];
                }

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                mX.pop_back();

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             INIT_CAP == X.capacity());

                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, k,
                                 VALUES[k % NUM_VALUES] == X[k]);
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BB == AA);
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,  B ==  A);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const size_t LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t CAP = DATA[l].d_length;
                ASSERT(LENGTH <= CAP);

                Obj mX(LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_back();
                }
                catch (...) {
                    exceptionCaught = true;
                }
                LOOP_ASSERT(LINE, !exceptionCaught);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
#endif

    if (verbose) printf("\nTesting 'erase(pos, n)'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <=  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH + 2; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k >= INIT_LENGTH
                                              ? INIT_LENGTH : k;
                    const size_t NUM_ELEMENTS = k - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH -
                                                         (END_POS - BEGIN_POS);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    Obj *result = &mX.erase(BEGIN_POS, NUM_ELEMENTS);
                                                             // test erase here

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, result == &mX);
                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, LENGTH == X.size());
                    LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BEGIN_POS,
                                 NUM_ELEMENTS, CAPACITY == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 B + 0 ==  A);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k >= INIT_LENGTH
                                              ? INIT_LENGTH : k;
                    const size_t NUM_ELEMENTS = k - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH -
                                                         (END_POS - BEGIN_POS);

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        Obj *result = &mX.erase(BEGIN_POS, NUM_ELEMENTS);
                                                             // test erase here
                        guard.release();
                        ASSERT(&X == result);

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
    if (verbose) printf("\nTesting 'erase(pos)' on non-empty strings.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore: "); P_(BB); P(B);
                    }

                    typename Obj::iterator result = mX.erase(mX.begin() + POS);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAfter : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 result == mX.begin() + POS);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 CAPACITY == X.capacity());

                    for (m = 0; m < POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[(m + 1) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 B + 0 ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        mX.erase(mX.begin() + POS);  // test erase here
                        guard.release();

                        for (m = 0; m < POS; ++m) {
                            LOOP5_ASSERT(
                                       INIT_LINE, INIT_LENGTH, INIT_CAP,
                                       POS, m, VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(
                                 INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 POS, m, VALUES[(m + 1) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const size_t CAPACITY = X.capacity();

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    typename Obj::iterator result = mX.erase(
                                                        mX.begin() + BEGIN_POS,
                                                        mX.begin() + END_POS);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(
                               INIT_LINE, INIT_LENGTH, INIT_CAP,
                               NUM_ELEMENTS, result == mX.begin() + BEGIN_POS);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, CAPACITY == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 BB + 0 == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 B + 0 ==  A);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_RES);
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        mX.erase(mX.begin() + BEGIN_POS, mX.begin() + END_POS);
                                                             // test erase here
                        guard.release();

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase19Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ERASE
    //
    // Concerns:
    //   1 'pop_back' asserts on undefined behavior when the string is empty,
    //   2 'erase' asserts on undefined behavior when iterators are not valid
    //   on the string being tested or they don't make a valid range.
    //
    // Plan:
    //   For concern (1), create an empty string and call 'pop_back' which
    //   should assert.  For concern (2), create a non-empty string and test
    //   'erase' with different combinations of invalid iterators and iterator
    //   ranges.
    //
    // Testing:
    //   void pop_back();
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative testing pop_back\n");

    {
        Obj mX;

        // pop_back on empty string
        ASSERT_SAFE_FAIL(mX.pop_back());
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // position < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1));

        // position >= end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1));
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator, iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // first < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1, mX.end()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.end() + 1));

        // first > last
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() + 1, mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.end() - 1));

        // first > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1, mX.end()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.begin() - 1));
    }

    if (veryVerbose) {
        printf("\tnow try some valid parameters for pop_back/erase\n");
    }

    {
        Obj mX(g("ABCDE"));
        ASSERT_SAFE_PASS(mX.pop_back());
        ASSERT_SAFE_PASS(mX.erase(mX.begin()));
        ASSERT_SAFE_PASS(mX.erase(mX.begin(), mX.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the 'insert' return (if any) value is a reference to self,
    //      or a valid iterator, even when the string underwent a reallocation.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved via Traits::move.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' at various
    //   positions, or a variable number of copies of this value.  Perform the
    //   above tests:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   basic_string& insert(size_type pos, size_type n, C c);
    //   iterator insert(const_iterator p, size_type n, C c);
    //   iterator insert(const_iterator p, C c);
    //   // string& insert(size_type pos, const C *s, n2);
    //   // string& insert(size_type pos, const C *s);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        INSERT_CHAR_MODE_FIRST    = 0,
        INSERT_CHAR_N_AT_INDEX    = 0,
        INSERT_CHAR_N_AT_ITERATOR = 1,
        INSERT_CHAR_AT_ITERATOR   = 2,
        INSERT_CHAR_MODE_LAST     = 2
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        // This portion of the test would be appropriate if we performed
        // short-string optimization, for which the boundary sizes would differ
        // in 32 and 64-bit modes, but we do not in this version.

        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'insert'.\n");

    if (verbose) printf("\tUsing a single 'value'.\n");

    for (int insertMode  = INSERT_CHAR_AT_ITERATOR;
             insertMode <= INSERT_CHAR_MODE_LAST;
             ++insertMode)
    {
        if (verbose)
            printf("\t\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    switch (insertMode) {
                      case INSERT_CHAR_AT_ITERATOR: {
                        //   iterator insert(iterator p, C c);
                        iterator result = mX.insert(mX.begin() + POS, VALUE);
                        LOOP3_ASSERT(INIT_LINE, i, j,
                                     X.begin() + POS == result);
                      } break;
                      default: {
                        printf("***UNKNOWN INSERT MODE***\n");
                        ASSERT(0);
                      } break;
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                    LOOP3_ASSERT(INIT_LINE, i, j, CAP == X.capacity());

                    for (k = 0; k < POS; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    for (++k; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    for (int insertMode = INSERT_CHAR_MODE_FIRST;
             insertMode < INSERT_CHAR_AT_ITERATOR;
         ++insertMode)
    {
        if (verbose)
            printf("\tUsing 'n' copies of 'value', insertMode = %d.\n",
                   insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (insertMode) {
                          case INSERT_CHAR_N_AT_ITERATOR: {
                            // iterator insert(const_iterator, size_type, C);
                            iterator result = mX.insert(X.begin() + POS,
                                                        NUM_ELEMENTS,
                                                        VALUE);
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         X.begin() + POS == result);
                          } break;
                          case INSERT_CHAR_N_AT_INDEX: {
                            // string& insert(pos, n, C c);
                            Obj &result = mX.insert(POS, NUM_ELEMENTS, VALUE);
                            LOOP3_ASSERT(INIT_LINE, i, j, &X == &result);
                          } break;
                          default: {
                            printf("***UNKNOWN INSERT MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     CAP == X.capacity());

                        size_t m = 0;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (; k < POS + NUM_ELEMENTS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUE == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC == AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC ==  A);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int insertMode  = INSERT_CHAR_MODE_FIRST;
             insertMode <= INSERT_CHAR_MODE_LAST;
         ++insertMode)
    {
        if (verbose)
            printf("\t\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            const size_t CAP = computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            bool checkResultFlag = false;
                            switch (insertMode) {
                              case INSERT_CHAR_N_AT_INDEX: {
                                // string& insert(pos, n, C c);
                                Obj &result = mX.insert(POS,
                                                        NUM_ELEMENTS,
                                                        VALUE);
                                LOOP3_ASSERT(INIT_LINE, i, j, &X == &result);
                                checkResultFlag = true;
                              } break;
                              case INSERT_CHAR_N_AT_ITERATOR: {
                                // iterator insert(const_iterator p,
                                //                 size_type n, C c);
                                mX.insert(mX.begin() + POS,
                                          NUM_ELEMENTS,
                                          VALUE);
                                checkResultFlag = true;
                              } break;
                              case INSERT_CHAR_AT_ITERATOR: {
                                if (NUM_ELEMENTS != 1) {
                                    break;
                                }
                                //   iterator insert(iterator p, C c);
                                iterator result =
                                            mX.insert(mX.begin() + POS, VALUE);
                                LOOP3_ASSERT(INIT_LINE, i, j,
                                             X.begin() + POS == result);
                                checkResultFlag = true;
                              } break;
                              default: {
                                printf("***UNKNOWN INSERT MODE***\n");
                                ASSERT(0);
                              } break;
                            }
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            if (checkResultFlag) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             LENGTH == X.size());
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());

                                size_t k;
                                for (k = 0; k < POS; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 DEFAULT_VALUE == X[k]);
                                }
                                for (; k < POS + NUM_ELEMENTS; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 VALUE == X[k]);
                                }
                                for (; k < LENGTH; ++k) {
                                    LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                                 DEFAULT_VALUE == X[k]);
                                }
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    //
    // Concerns:
    //   1) That the resulting string value is correct.
    //   2) That the initial range is correctly imported and then moved if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes with different
    //   'value' as argument.  Perform the above tests:
    //      - From the parameterized 'CONTAINER::const_iterator'.
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there is a change in capacity, 0 otherwise
    //      - 1 if the  type uses an allocator and the value is an alias.
    //      -
    //   For concern 4, we test with a bitwise-moveable type that the only
    //   reallocations are for the new elements plus one if the string
    //   undergoes a reallocation (capacity changes).
    //
    // Testing:
    //   template <class InputIter>
    //   iterator insert(const_iterator p, InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                     typename bsl::iterator_traits<
                      typename CONTAINER::const_iterator>::iterator_category
                   >::value;

    enum {
        INSERT_STRING_MODE_FIRST        = 0,
        INSERT_SUBSTRING_AT_INDEX       = 0,
        INSERT_SUBSTRING_AT_INDEX_NPOS  = 1,
        INSERT_STRING_AT_INDEX          = 2,
        INSERT_CSTRING_N_AT_INDEX       = 3,
        INSERT_CSTRING_AT_INDEX         = 4,
        INSERT_STRING_AT_ITERATOR       = 5,
        INSERT_STRING_AT_CONST_ITERATOR = 6,
        INSERT_STRING_MODE_LAST         = 6
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int insertMode  = INSERT_STRING_AT_INDEX;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        if (verbose)
            printf("\tUsing string with insertMode = %d.\n", insertMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        switch (insertMode) {
                          case INSERT_STRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str);
                            Obj &result = mX.insert(POS, Y);
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_CSTRING_N_AT_INDEX: {
                            // string& insert(pos, const C *s, n);
                            Obj &result = mX.insert(POS,
                                                    Y.data(),
                                                    NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_CSTRING_AT_INDEX: {
                            // string& insert(pos, const C *s);
                            Obj &result = mX.insert(POS, Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case INSERT_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                            mX.insert(mX.cbegin() + POS, mU.begin(), mU.end());
                          } break;
                          case INSERT_STRING_AT_CONST_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                            mX.insert(mX.cbegin() + POS, U.begin(), U.end());
                          } break;
                          default: {
                            printf("***UNKNOWN INSERT MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m = 0;
                        for (k = 0; k < POS; ++k, ++m) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }
                        for (m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         Y[m] == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC <=  A);
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with insertMode = %d.\n",
                        INSERT_SUBSTRING_AT_INDEX);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t POS  = j;
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));
                        const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS_INS);
                            printf("at "); P_(POS);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& insert(pos1, const string<C,CT,A>& str,
                        //                pos2, n);
                        Obj &result = mX.insert(POS, Y, POS2, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m;
                        for (n = 0; n < POS; ++n) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, n,
                                         VALUES[n % NUM_VALUES] == X[n]);
                        }
                        for (m = 0; m < NUM_ELEMENTS_INS; ++m, ++n) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, m, n,
                                         Y[POS2 + m] == X[n]);
                        }
                        for (m = POS; n < LENGTH; ++m, ++n) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, m, n,
                                         VALUES[m % NUM_VALUES] == X[n]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     B + A_ALLOC <=  A);
                    }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int insertMode  = INSERT_STRING_MODE_FIRST;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const Int64 AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            const size_t CAP = computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ExceptionGuard<Obj> guard(X, L_);
                            testAllocator.setAllocationLimit(AL);

                            switch (insertMode) {
                              case INSERT_STRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str);
                                Obj &result = mX.insert(POS, Y);
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_CSTRING_N_AT_INDEX: {
                            // string& insert(pos, const C *s, n);
                                Obj &result = mX.insert(POS,
                                                        Y.data(),
                                                        NUM_ELEMENTS);
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_CSTRING_AT_INDEX: {
                            // string& insert(pos, const C *s);
                                Obj &result = mX.insert(POS, Y.c_str());
                                ASSERT(&result == &mX);
                              } break;
                              case INSERT_STRING_AT_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                                mX.insert(mX.cbegin() + POS,
                                          mU.begin(),
                                          mU.end());
                              } break;
                              case INSERT_STRING_AT_CONST_ITERATOR: {
                            // template <class InputIter>
                            // insert(const_iterator p, InputIter first, last);
                                mX.insert(mX.cbegin() + POS,
                                          U.begin(),
                                          U.end());
                              } break;
                              case INSERT_SUBSTRING_AT_INDEX: {
                            // string& insert(pos1, const string<C,CT,A>& str,
                            //                pos2, n);
                                mX.insert(POS, Y, 0, NUM_ELEMENTS);
                              } break;
                              case INSERT_SUBSTRING_AT_INDEX_NPOS: {
                            // string& insert(pos1, const string<C,CT,A>& str,
                            //                pos2, n);
                                mX.insert(POS, Y, 0);  // 'npos' default arg.
                              } break;
                              default: {
                                printf("***UNKNOWN INSERT MODE***\n");
                                ASSERT(0);
                              } break;
                            }
                            guard.release();

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            if (!INPUT_ITERATOR_TAG) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());
                            }

                            size_t k, m;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (m = 0; m < NUM_ELEMENTS; ++k, ++m) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             Y[m] == X[k]);
                            }
                            for (m = POS; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int insertMode  = INSERT_STRING_MODE_FIRST;
             insertMode <= INSERT_STRING_MODE_LAST;
             ++insertMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));
                    const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    for (size_t k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    Obj mY(X); const Obj& Y = mY;  // control

                    if (veryVerbose) {
                        printf("\t\t\tInsert itself");
                        printf(" at "); P(POS);
                    }

                    switch (insertMode) {
                      case INSERT_STRING_AT_INDEX: {
                    // string& insert(pos1, const string<C,CT,A>& str);
                        mX.insert(POS, Y);
                        mY.insert(POS, Y);
                      } break;
                      case INSERT_CSTRING_N_AT_INDEX: {
                    // string& insert(pos, const C *s, n);
                        mX.insert(POS, Y.data(), INIT_LENGTH);
                        mY.insert(POS, Y.data(), INIT_LENGTH);
                      } break;
                      case INSERT_CSTRING_AT_INDEX: {
                    // string& insert(pos, const C *s);
                        mX.insert(POS, Y.c_str());
                        mY.insert(POS, Y.c_str());
                      } break;
                      case INSERT_SUBSTRING_AT_INDEX: {
                    // string& insert(pos1, const string<C,CT,A>& str, pos2, n)
                        mX.insert(POS, Y, 0, INIT_LENGTH);
                        mY.insert(POS, Y, 0, INIT_LENGTH);
                      } break;
                      case INSERT_SUBSTRING_AT_INDEX_NPOS: {
                    // string& insert(pos1, const string<C,CT,A>& str, pos2, n)
                        mX.insert(POS, Y, 0);  // 'npos' default arg.
                        mY.insert(POS, Y, 0);
                      } break;
                      case INSERT_STRING_AT_ITERATOR: {
                    // insert(const_iterator p, InputIter first, last);
                        mX.insert(mX.cbegin() + POS, Y.begin(), Y.end());
                        mY.insert(mY.cbegin() + POS, Y.begin(), Y.end());
                      } break;
                      case INSERT_STRING_AT_CONST_ITERATOR: {
                    // insert(const_iterator p, InputIter first, last);
                        mX.insert(mX.cbegin() + POS, mY.begin(), mY.end());
                        mY.insert(mY.cbegin() + POS, mY.begin(), mY.end());
                      } break;
                      default: {
                        printf("***UNKNOWN INSERT MODE***\n");
                        ASSERT(0);
                      } break;
                    }

                    if (veryVerbose) {
                        T_; T_; T_; T_; P(X);
                        T_; T_; T_; T_; P(Y);
                    }

                    LOOP4_ASSERT(INIT_LINE, i, INIT_CAP, POS, X == Y);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        for (size_t m = 0; m < INIT_LENGTH; ++m) {
                            const size_t INDEX        = h;
                            const size_t NUM_ELEMENTS = m;

                            Obj mX(INIT_LENGTH,
                                   DEFAULT_VALUE,
                                   AllocType(&testAllocator));
                            const Obj& X = mX;
                            mX.reserve(INIT_RES);
                            const size_t INIT_CAP = X.capacity();

                            for (size_t k = 0; k < INIT_LENGTH; ++k) {
                                mX[k] = VALUES[k % NUM_VALUES];
                            }

                            Obj mY(X); const Obj& Y = mY;  // control

                            if (veryVerbose) {
                                printf("\t\t\tInsert substring of itself");
                                printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                                printf(" at "); P_(POS);
                            }

                            if (INDEX + NUM_ELEMENTS >= Y.length()) {
                                mX.insert(POS, Y, INDEX);  // 'npos' dflt. arg.
                                mY.insert(POS, Y, INDEX);
                            }
                            else {
                                mX.insert(POS, Y, INDEX, NUM_ELEMENTS);
                                mY.insert(POS, Y, INDEX, NUM_ELEMENTS);
                            }

                            if (veryVerbose) {
                                T_; T_; T_; T_; P(X);
                                T_; T_; T_; T_; P(Y);
                            }

                            LOOP5_ASSERT(INIT_LINE, i, INIT_CAP, POS, INDEX,
                                         X == Y);
                        }
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase18Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING INSERTION:
    //
    // Concerns:
    //   1 'insert' methods assert on undefined behavior when either a NULL
    //     C-string pointer is passed or invalid iterators are passed.  Other
    //     valid parameters do not change the ability of 'insert' to assert on
    //     invalid parameters.
    //
    // Plan:
    //   Construct a string object with some string data, call 'insert' with a
    //   NULL C-string pointer and verify that it asserts.  Then call 'insert'
    //   with invalid iterators and verify that it asserts.
    //
    // Testing:
    //   string& insert(size_type pos, const C *s);
    //   string& insert(size_type pos, const C *s, n2);
    //   iterator insert(const_iterator p, C c);
    //   iterator insert(const_iterator p, size_type n, C c);
    //   template <class InputIter>
    //     iterator insert(const_iterator p, InputIter first, InputIter last);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative testing insert(pos, s)\n");

    {
        Obj mX(g("ABCDE"));
        const TYPE *nullStr = 0;
        // disable "unused variable" warning in non-safe mode:
        (void) nullStr;

        ASSERT_SAFE_FAIL(mX.insert(1, nullStr));
        ASSERT_SAFE_FAIL(mX.insert(mX.length() + 1, nullStr));

        ASSERT_SAFE_PASS(mX.insert(1, mX.c_str()));
    }

    if (veryVerbose) printf("\tnegative testing insert(pos, s, n)\n");

    {
        Obj mX(g("ABCDE"));
        const TYPE *nullStr = 0;
        // disable "unused variable" warning in non-safe mode:
        (void) nullStr;

        ASSERT_SAFE_PASS(mX.insert(1, nullStr, 0));
        ASSERT_SAFE_FAIL(mX.insert(mX.length() + 1, nullStr, 10));

        ASSERT_SAFE_PASS(mX.insert(1, mX.c_str(), mX.length()));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0]));

        // begin() <= position < end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0]));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, n, c)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0], 0));
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0], 2));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0], 0));
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0], 2));

        // begin() <= position <= end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0], 0));
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0], 2));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, first, last)\n");

    {
        Obj mX(g("ABCDE"));
        const Obj& X = mX;

        Obj mY(g("ABE"));
        const Obj& Y = mY;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, Y.begin(), Y.end()));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, Y.begin(), Y.end()));

        // first > last
        ASSERT_SAFE_FAIL(mX.insert(X.begin(), Y.end(), Y.begin()));

        // begin() <= position <= end() && first <= last
        ASSERT_SAFE_PASS(mX.insert(X.begin(), Y.begin(), Y.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17()
{
    // --------------------------------------------------------------------
    // TESTING 'append'
    //
    // Plan:
    //   For appending, we will create objects of varying sizes containing
    //   default values for type T, and then append different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the 'append'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, check QoI of 'append' in that it does not internally
    //   allocate a temporary string.
    //
    // Testing:
    //   basic_string& operator+=(CHAR_TYPE c);
    //   basic_string& append(size_type n, CHAR_TYPE c);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       17   },
        { L_,       19   },
        { L_,       21   },
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   },
        { L_,       35   },
        { L_,       40   },
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'append'.\n");

    if (verbose) printf("\tUsing multiple copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tAppend "); P_(NUM_ELEMENTS);
                        printf("using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    // string& operator+=(C c);
                    for (int j = 0; j != NUM_ELEMENTS; ++j) {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        Obj &result = mX += VALUE;
                        LOOP2_ASSERT(INIT_LINE, i, &X == &result);
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    // TBD: Accurately determine the capacity/allocator
                    //      behavior of repeated '+=' operators with a single
                    //      character, equivalent to repeated 'push_back' calls

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
//                    LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());

                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    for (; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUE == X[k]);
                    }

//                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

//                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
//                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing multiple copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tAppend "); P_(NUM_ELEMENTS);
                        printf("using "); P(VALUE);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    // string& append(n, C c);
                    {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        Obj &result = mX.append(NUM_ELEMENTS, VALUE);
                        LOOP2_ASSERT(INIT_LINE, i, &X == &result);
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                    LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());

                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    for (; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                     VALUE == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 BB + REALLOC == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              X.length(),
                                                              X.capacity(),
                                                              X.max_size());

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        // void append(size_type n, C c);
                        mX.append(NUM_ELEMENTS, VALUE);
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        LOOP2_ASSERT(INIT_LINE, LINE, CAP    == X.capacity());

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         DEFAULT_VALUE == X[k]);
                        }
                        for (; k < LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         VALUE == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'append'
    //
    // Plan:
    //   For appending we will create objects of varying sizes containing
    //   default values for type T, and then append different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, check QoI of 'append' in that it does not internally
    //   allocate a temporary string.
    //
    // Testing:
    //   basic_string& operator+=(const StringRefData& strRefData);
    //   basic_string& append(const basic_string& str);
    //   basic_string& append(const basic_string& str, pos, n = npos);
    //   basic_string& append(const CHAR_TYPE *s, size_type n);
    //   basic_string& append(const CHAR_TYPE *s);
    //   template <class Iter> basic_string& append(Iter first, Iter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                     >::value;

    enum {
        APPEND_STRING_MODE_FIRST  = 0,
        APPEND_SUBSTRING          = 0,
        APPEND_SUBSTRING_NPOS     = 1,
        APPEND_STRING             = 2,
        APPEND_CSTRING_N          = 3,
        APPEND_CSTRING_NULL_0     = 4,
        APPEND_CSTRING            = 5,
        APPEND_RANGE              = 6,
        APPEND_CONST_RANGE        = 7,
        APPEND_STRINGVIEW         = 8,
        APPEND_STRING_MODE_LAST   = 8
    };

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   }
#else
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    for (int appendMode = APPEND_STRING;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        if (verbose)
            printf("\tUsing string with appendMode = %d.\n", appendMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS =
                      (APPEND_CSTRING_NULL_0 == appendMode) ? 0 : strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    bsl::basic_string_view<TYPE>       mV(Y.begin(),
                                                          Y.length());
                    const bsl::basic_string_view<TYPE> V = mV;

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                        printf("using "); P(SPEC);
                    }

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        switch (appendMode) {
                          case APPEND_STRING: {
                            // string& append(const string<C,CT,A>& str);
                            Obj &result = mX.append(Y);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_N: {
                            // string& append(pos, const C *s, n);
                            Obj &result = mX.append(Y.data(), NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_NULL_0: {
                            // string& append(pos, const C *s, n);
                            Obj &result = mX.append(0, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING: {
                            // string& append(const C *s);
                            Obj &result = mX.append(Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_RANGE: {
                            // void append(InputIter first, last);
                            Obj &result = mX.append(mU.begin(), mU.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CONST_RANGE: {
                            // void append(InputIter first, last);
                            Obj &result = mX.append(U.begin(), U.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_STRINGVIEW: {
                            //operator+=(bsl::basic_string_view strView);
                            Obj &result = mX += V;
                            ASSERT(&result == &mX);
                          } break;
                          default: {
                            printf("***UNKNOWN APPEND MODE***\n");
                            ASSERT(0);
                          } break;
                        }
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                    if (!INPUT_ITERATOR_TAG) {
                        LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                    }

                    size_t m = 0;
                    for (k = 0; k < INIT_LENGTH; ++k, ++m) {
                        LOOP3_ASSERT(INIT_LINE, LINE, k,
                                     VALUES[m % NUM_VALUES] == X[k]);
                    }
                    for (m = 0; k < LENGTH; ++k, ++m) {
                        LOOP4_ASSERT(INIT_LINE, LINE, k, m,
                                     Y[m] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 BB + REALLOC <= AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                 B + A_ALLOC <=  A);
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with appendMode = %d.\n",
                        APPEND_SUBSTRING);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t POS2 = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS2;
                        const size_t LENGTH = INIT_LENGTH + NUM_ELEMENTS_INS;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tAppend"); P_(NUM_ELEMENTS_INS);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS2);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& append(const string<C,CT,A>& str,
                        //                pos2, n);
                        {
                            AllocatorUseGuard guardG(globalAllocator_p);
                            AllocatorUseGuard guardD(defaultAllocator_p);
                            Obj &result = mX.append(Y, POS2, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                        }

                        size_t m;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            LOOP3_ASSERT(INIT_LINE, LINE, n,
                                         VALUES[n % NUM_VALUES] == X[n]);
                        }
                        for (m = 0; m < NUM_ELEMENTS_INS; ++m, ++n) {
                            LOOP4_ASSERT(INIT_LINE, LINE, m, n,
                                         Y[POS2 + m] == X[n]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                     BB + REALLOC <= AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, CAP,
                                     B + A_ALLOC <=  A);
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int appendMode = APPEND_STRING_MODE_FIRST;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS =
                      (APPEND_CSTRING_NULL_0 == appendMode) ? 0 : strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    bsl::basic_string_view<TYPE>       mV(Y.begin(),
                                                          Y.length());
                    const bsl::basic_string_view<TYPE> V = mV;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        ExceptionGuard<Obj> guard(X, L_);
                        testAllocator.setAllocationLimit(AL);

                        switch (appendMode) {
                          case APPEND_STRING: {
                        // string& append(const string<C,CT,A>& str);
                            Obj &result = mX.append(Y);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_N: {
                        // string& append(const C *s, n);
                            Obj &result = mX.append(Y.data(), NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING_NULL_0: {
                        // string& append(const C *s, n); 's = 0';
                            Obj &result = mX.append(0, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CSTRING: {
                        // string& append(const C *s);
                            Obj &result = mX.append(Y.c_str());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_RANGE: {
                        // string& append(InputIter first, last);
                            Obj &result = mX.append(mU.begin(), mU.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_CONST_RANGE: {
                        // string& append(InputIter first, last);
                            Obj &result = mX.append(U.begin(), U.end());
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_SUBSTRING: {
                        // string& append(const string<C,CT,A>& str, pos2, n);
                            Obj &result = mX.append(Y, 0, NUM_ELEMENTS);
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_SUBSTRING_NPOS: {
                        // string& append(const string<C,CT,A>& str, pos2, n);
                            Obj &result = mX.append(Y, 0); // 'npos' dflt. arg.
                            ASSERT(&result == &mX);
                          } break;
                          case APPEND_STRINGVIEW: {
                        // string& operator+=(bsl::basic_string_view strView);
                            Obj &result = mX += V;
                            ASSERT(&result == &mX);
                          } break;
                          default: {
                            printf("***UNKNOWN APPEND MODE***\n");
                            ASSERT(0);
                          } break;
                        }

                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                        }

                        size_t k, m;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         DEFAULT_VALUE == X[k]);
                        }
                        for (m = 0; m < NUM_ELEMENTS; ++k, ++m) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, k,
                                         Y[m] == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int appendMode  = APPEND_STRING_MODE_FIRST;
             appendMode <= APPEND_STRING_MODE_LAST;
             ++appendMode)
    {
        if (verbose)
            printf("\tUsing string with appendMode = %d.\n", appendMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                Obj mX(INIT_LENGTH,
                       DEFAULT_VALUE,
                       AllocType(&testAllocator));  const Obj& X = mX;
                mX.reserve(INIT_RES);

                for (size_t k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] = VALUES[k % NUM_VALUES];
                }

                Obj mY(X, AllocType(&testAllocator)); const Obj& Y = mY;
                                                                 // ^-- control

                bsl::basic_string_view<TYPE>       mV(Y.begin(),
                                                      Y.length());
                const bsl::basic_string_view<TYPE> V = mV;

                if (veryVerbose) {
                    printf("\t\t\tAppend itself.\n");
                }

                {
                    AllocatorUseGuard guardG(globalAllocator_p);
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    switch (appendMode) {
                      case APPEND_STRING: {
                    // string& append(const string<C,CT,A>& str);
                        mX.append(Y);
                        mY.append(Y);
                      } break;
                      case APPEND_CSTRING_N: {
                    // string& append(pos, const C *s, n);
                        mX.append(Y.data(), INIT_LENGTH);
                        mY.append(Y.data(), INIT_LENGTH);
                      } break;
                      case APPEND_CSTRING_NULL_0: {
                    // string& append(pos, const C *s, n);
                        mX.append(0, 0);
                        mY.append(0, 0);
                      } break;
                      case APPEND_CSTRING: {
                    // string& append(const C *s);
                        mX.append(Y.c_str());
                        mY.append(Y.c_str());
                      } break;
                      case APPEND_SUBSTRING: {
                    // string& append(const string<C,CT,A>& str, pos2, n);
                        mX.append(Y, 0, INIT_LENGTH);
                        mY.append(Y, 0, INIT_LENGTH);
                      } break;
                      case APPEND_SUBSTRING_NPOS: {
                    // string& append(const string<C,CT,A>& str, pos2, n);
                        mX.append(Y, 0);  // 'npos' default argument
                        mY.append(Y, 0);
                      } break;
                      case APPEND_RANGE: {
                    // void append(InputIter first, last);
                        mX.append(mY.begin(), mY.end());
                        mY.append(mY.begin(), mY.end());
                      } break;
                      case APPEND_CONST_RANGE: {
                    // void append(InputIter first, last);
                        mX.append(Y.begin(), Y.end());
                        mY.append(Y.begin(), Y.end());
                      } break;
                      case APPEND_STRINGVIEW: {
                    //operator+=(bsl::basic_string_view strView);
                        mX += V;
                        mY += V;
                      } break;
                      default: {
                        printf("***UNKNOWN APPEND MODE***\n");
                        ASSERT(0);
                      } break;
                    }
                }

                if (veryVerbose) {
                    T_; T_; T_; T_; P(X);
                    T_; T_; T_; T_; P(Y);
                }

                LOOP5_ASSERT(INIT_LINE, appendMode, INIT_RES, X, Y, X == Y);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    {
        if (verbose) printf("\tUsing string with appendMode = %d (complete)."
                            "\n",
                            APPEND_SUBSTRING);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t h = 0; h < INIT_LENGTH; ++h) {
                    for (size_t m = 0; m < INIT_LENGTH; ++m) {
                        const size_t INDEX        = h;
                        const size_t NUM_ELEMENTS = m;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X,
                               AllocType(&testAllocator)); const Obj& Y = mY;
                                                                 // ^-- control

                        if (veryVerbose) {
                            printf("\t\t\tAppend substring of itself");
                            printf(" with "); P_(INDEX); P(NUM_ELEMENTS);
                        }

                        // string& append(const string<C,CT,A>& str, pos2, n);
                        {
                            AllocatorUseGuard guardG(globalAllocator_p);
                            AllocatorUseGuard guardD(defaultAllocator_p);

                            if (INDEX + NUM_ELEMENTS >= Y.length()) {
                                mX.append(Y, INDEX);  // 'npos' default arg.
                                mY.append(Y, INDEX);
                            }
                            else {
                                mX.append(Y, INDEX, NUM_ELEMENTS);
                                mY.append(Y, INDEX, NUM_ELEMENTS);
                            }
                        }

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        LOOP5_ASSERT(INIT_LINE, INIT_RES, INDEX, X, Y, X == Y);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase17Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING 'append'
    //
    // Concerns:
    //   'append', 'operator+=', and 'operator+' assert on undefined behavior:
    //   1 when a character string pointer parameter is NULL,
    //   2 when the 'first'/'last' parameters do not specify a valid iterator
    //     range
    //
    // Plan:
    //   1 Create a string object and then test the following things that
    //     should produce an assert for undefined behavior:
    //     o call 'append' with a NULL pointer,
    //     o call 'operator+=' with a NULL pointer,
    //     o call 'operator+' with a NULL pointer,
    //     o call 'append' with various invalid iterator ranges.
    //   2 After that, call those methods with valid parameters and verify that
    //     they don't assert.
    //
    // Testing:
    //   string& append(const C *s, size_type n);
    //   string& append(const C *s);
    //   template <class InputIter> append(InputIter first, InputIter last);
    //   operator+=(const string& rhs);
    //   operator+(const string& lhs, const CHAR_TYPE *rhs);
    //   operator+(const CHAR_TYPE *lhs, const string& rhs);
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tappend/operator+/+= with NULL\n");

    {
        Obj mX;
        const TYPE *nullStr = 0;

        ASSERT_SAFE_FAIL(mX.append(nullStr));
        ASSERT_SAFE_PASS(mX.append(nullStr, 0));
        ASSERT_SAFE_FAIL(mX.append(nullStr, 10));
        ASSERT_SAFE_FAIL(mX += nullStr);
        ASSERT_SAFE_FAIL(mX + nullStr);
        ASSERT_SAFE_FAIL(nullStr + mX);

        if (veryVerbose) printf("\tappend/operator+/+= with valid C-string\n");

        Obj nonNull(g("ABCDE"));

        ASSERT_SAFE_PASS(mX.append(nonNull.c_str()));
        ASSERT_SAFE_PASS(mX.append(nonNull.c_str(), 0));
        ASSERT_SAFE_PASS(mX.append(nonNull.c_str(), nonNull.length()));
        ASSERT_SAFE_PASS(mX += nonNull.c_str());
        ASSERT_SAFE_PASS(mX + nonNull.c_str());
        ASSERT_SAFE_PASS(nonNull.c_str() + mX);
    }

    if (veryVerbose) printf("\t'append' with invalid range\n");

    {
        Obj mX;
        Obj mY(g("ABCDE"));
        const Obj& Y = mY;
        (void) Y; // to disable "unused variable" warning

        ASSERT_SAFE_FAIL(mX.append(mY.end(), mY.begin()));
        ASSERT_SAFE_FAIL(mX.append(Y.end(), Y.begin()));
    }

    if (veryVerbose) printf("\t'append' with valid range\n");

    {
        Obj mX;
        Obj mY(g("ABCDE"));
        const Obj& Y = mY;

        ASSERT_SAFE_PASS(mX.append(mY.begin(), mY.end()));
        ASSERT_SAFE_PASS(mX.append(Y.begin(), Y.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase16()
{
    // --------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //   1) That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable string, and non-mutable iterators
    //      otherwise.
    //   2) That the range '[begin(), end())' equals the value of the string.
    //   3) Same concerns with 'rbegin' and 'rend'.
    // In addition:
    //   4) That 'iterator' is a pointer to 'TYPE'.
    //   5) That 'const_iterator' is a pointer to 'const TYPE'.
    //   6) That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bslstl::ReverseIterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //   6. That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bsl::reverse_iterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //
    // Plan:
    //   For 1--3, for each value given by variety of specifications of
    //   different lengths, create a test string with this value, and access
    //   each element in sequence and in reverse sequence, both as a reference
    //   offering modifiable access (setting it to a default value, then back
    //   to its original value, and as a reference offering non-modifiable
    //   access.
    //
    // For 4--6, use 'bsl::is_same' to assert the identity of iterator types.
    // Note that these concerns let us get away with other concerns such as
    // testing that 'iter[i]' and 'iter + i' advance 'iter' by the correct
    // number 'i' of positions, and other concern about traits, because
    // 'bsl::iterator_traits' and 'bsl::reverse_iterator' have already been
    // fully tested in the 'bslstl_iterator' component.
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin();
    //   const_iterator end();
    //   const_reverse_iterator rbegin();
    //   const_reverse_iterator rend();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            for (iterator iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                LOOP_ASSERT(LINE, Y[i] == *iter);
                *iter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *iter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, LENGTH == i);

            LOOP_ASSERT(LINE, Y == X);

            i = 0;
            for (const_iterator iter = X.begin(); iter != X.end();
                                                                 ++iter, ++i) {
                LOOP2_ASSERT(LINE, i, Y[i] == *iter);
            }
            LOOP_ASSERT(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;
            const int   LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (reverse_iterator riter = mX.rbegin(); riter != mX.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
                *riter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *riter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, -1 == i);

            LOOP_ASSERT(LINE, Y == X);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.rbegin(); riter != X.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
            }
            LOOP_ASSERT(LINE, -1 == i);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   1) That 'v[x]', as well as 'v.front()' and 'v.back()', allow to modify
    //      its indexed element when 'v' is an lvalue, but must not modify its
    //      indexed element when it is an rvalue.
    //   2) That 'v.at(pos)' returns 'v[x]' or throws if 'pos == v.size())'.
    //   3) That 'v.front()' is identical to 'v[0]' and 'v.back()' the same as
    //      'v[v.size() - 1]'.
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test string with this value, and access each element
    //   (front, back, at each position) both as a reference offering
    //   modifiable access (setting it to a default value, then back to its
    //   original value, and as a reference offering non-modifiable access.
    //   Verify that 'at' throws 'std::out_of_range' when accessing the
    //   past-the-end element.
    //
    // Testing:
    //   reference operator[](size_type position);
    //   reference at(size_type n);
    //   reference front();
    //   reference back();
    //   const_reference front() const;
    //   const_reference back() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            if (veryVerbose) { T_; P_(LINE); P(SPEC); }

            LOOP_ASSERT(LINE, Y == X);

            // Test front/back.
            if (!mX.empty()) {
                LOOP_ASSERT(LINE, mX.front() == mX[0]);
                LOOP_ASSERT(LINE, mX.back() == mX[mX.size() - 1]);
            }

            // Test operator[].
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X[j]);
                mX[j] = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X[j]);
                mX.at(j) = Y[j];
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X.at(j));
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(g(SPEC), AllocType(&testAllocator));  const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // control

            bool outOfRangeCaught = false;
            try {
                mX.at(LENGTH) = DEFAULT_VALUE;
            }
            catch (const std::out_of_range&) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, Y == X);
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase15Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   For a string 's', the following const and non-'const' operations
    //   assert on undefined behavior:
    //   1 s[x] - when the index 'x' is out of range
    //   2 s.front() - when 's' is empty
    //   3 s.back() - when 's' is empty
    //
    // Plan:
    //   To test concerns (2) and (3), create an empty string and verify that
    //   'front'/'back' methods assert correctly.  Then insert a single
    //   character into the string and verify that the methods don't assert any
    //   more.  Then remove the character to make the string empty again, and
    //   verify that the methods start asserting again.
    //
    //   To test concern (1), create a string using a variety of specifications
    //   of different lengths, then scan the range of negative and positive
    //   indices for 'operator[]' and verify that 'operator[]' asserts when the
    //   index is out of range.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   const T& operator[](size_type position) const;
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDE"               }, // 20
#ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }, // 15
        { L_,   "ABCDEABCDEABCDEABCDE"               }, // 20
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }, // 30
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABCDEABCDE" }, // 40
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (veryVerbose) printf("\toperator[]\n");

    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            (void) LINE;
            (void) LENGTH;

            Obj mX(g(SPEC));
            const Obj& X = mX;

            const int numChars = static_cast<int>(X.size());
            for (int i = -numChars - 1; i < numChars * 2 + 2; ++i) {
                if (i >= 0 && i <= numChars) {
                    ASSERT_SAFE_PASS(X[i]);
                    ASSERT_SAFE_PASS(mX[i]);
                }
                else {
                    ASSERT_SAFE_FAIL(X[i]);
                    ASSERT_SAFE_FAIL(mX[i]);
                }
            }
        }
    }

    if (veryVerbose) printf("\tfront/back\n");

    {
        Obj mX;
        const Obj& X = mX;
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());

        mX.push_back(TYPE('A'));
        ASSERT_SAFE_PASS(X.front());
        ASSERT_SAFE_PASS(mX.front());
        ASSERT_SAFE_PASS(X.back());
        ASSERT_SAFE_PASS(mX.back());

        mX.pop_back();
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase14()
{
    // --------------------------------------------------------------------
    // TESTING CAPACITY
    //
    // Concerns:
    //   1) That 'v.reserve(n)' reserves sufficient capacity in 'v' to hold
    //      'n' elements without reallocation, but does not change value.
    //      In addition, if 'v.reserve(n)' allocates, it must allocate for a
    //      capacity of exactly 'n' bytes.
    //   2) That 'v.resize(n, val)' brings the new size to 'n', adding elements
    //      equal to 'val' if 'n' is larger than the current size.
    //   3) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   4) That 'reserve' and 'resize' are exception-neutral with full
    //      guarantee of rollback.
    //   5) That the accessors such as 'capacity', 'empty', return the correct
    //      value.
    //
    // Plan:
    //   For string 'v' having various initial capacities, call 'v.reserve(n)'
    //   for various values of 'n'.  Verify that sufficient capacity is
    //   allocated by filling 'v' with 'n' elements.  Perform each test in the
    //   standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void reserve(size_type n);
    //   void resize(size_type n);
    //   void resize(size_type n, CHAR_TYPE c);
    //   size_type max_size() const;
    //   size_type capacity() const;
    //   bool empty() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;
    ASSERT(0 == testAllocator.numBytesInUse());

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const size_t EXTEND[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

    static const size_t DATA[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.  Any reasonable value must be 2^31 -1 at least.

        Obj X;
        ASSERT(~(size_t)0 / sizeof(TYPE)  - 1 >= X.max_size());
        ASSERT(~(unsigned)0 / sizeof(TYPE) / 2  - 1 <= X.max_size());
    }

    if (verbose) printf("\tTesting 'reserve', 'capacity' and 'empty'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);

              const Int64  NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY         = X.capacity();
              {
                  ExceptionGuard<Obj> guard(X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, CAP == X.size());
                  LOOP_ASSERT(ti, CAPACITY >= NE || NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, CAP + DELTA == X.size());
              LOOP_ASSERT(ti,
                          NUM_ALLOC_AFTER == testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP = EXTEND[ei];

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              testAllocator.setAllocationLimit(AL);
              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();
              {
                  ExceptionGuard<Obj> guard(X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, 0  == X.size());
                  LOOP_ASSERT(ti, NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, NE);
              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti,
                          NUM_ALLOC_AFTER == testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'resize'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);
              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();

              ExceptionGuard<Obj> guard(X, L_);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = CAP; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              const Int64 NUM_ALLOC_BEFORE = testAllocator.numAllocations();
              const size_t CAPACITY        = X.capacity();

              ExceptionGuard<Obj> guard(X, L_);

              testAllocator.setAllocationLimit(AL);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const Int64 NUM_ALLOC_AFTER = testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAPACITY ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13()
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   The concerns are similar to those of the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value of the string being assigned to must be
    //   freed properly.  There is a further concern about aliasing (although
    //   assigning a portion of oneself is not subject to aliasing in most
    //   implementations), QoI concern that 'assign' does not internally
    //   allocate a temporary string, and an additional concern that allocators
    //   are not propagated (until we support the C++11 allocator propagation
    //   traits).  Finally, all 'assign' functions must return a reference to
    //   the string that has just been assigned to, just like the assignment
    //   operators.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'copy' followed by 'erase + append', and append will
    //   be tested more completely in test case 25.
    //
    // Testing:
    // * basic_string& assign(const basic_string& str);
    //   basic_string& assign(bslmf::MovableRef<basic_string> str);
    // * basic_string& assign(basic_string& str, pos, n = npos);
    // * basic_string& assign(const CHAR_TYPE *s, size_type n);
    // * basic_string& assign(const CHAR_TYPE *s);
    //   basic_string& assign(size_type n, CHAR_TYPE c);
    // --------------------------------------------------------------------

    bslma::TestAllocator         testAllocator(veryVeryVerbose);
    bslma::Allocator            *Z = &testAllocator;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        ASSIGN_STRING_MODE_FIRST  = 0,
        ASSIGN_SUBSTRING_AT_INDEX = 0,
        ASSIGN_STRING_AT_INDEX    = 1,
        ASSIGN_CSTRING_N_AT_INDEX = 2,
        ASSIGN_CSTRING_AT_INDEX   = 3,
        ASSIGN_STRING_AT_ITERATOR = 4,
        ASSIGN_STRING_MODE_LAST   = 5
    };

    if (verbose) printf("\nTesting initial-length assignment.\n");
    {
        static const struct {
            int         d_lineNum;          // source line number
            int         d_length;           // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        9   },

            { L_,       11   },
            { L_,       12   },
            { L_,       13   },
            { L_,       15   },
            { L_,       23   },
            { L_,       24   },
            { L_,       25   },
            { L_,       30   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        Obj& result = mX.assign(LENGTH, VALUE);    // test here
                        ASSERT(&result == &mX);
                    }

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 X.capacity() >= X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, VALUE == X[j]);
                    }
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf("\tWith exceptions.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;
                        ExceptionGuard<Obj> excGuard(X, L_);

                        testAllocator.setAllocationLimit(AL);

                        Obj& result = mX.assign(LENGTH, VALUE);    // test here
                        ASSERT(&result == &mX);
                        excGuard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == testAllocator.numMismatches());
                    ASSERT(0 == testAllocator.numBlocksInUse());
                }
            }
        }

        if (verbose) printf("\nTesting move assign.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                Obj src(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                Obj dst(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    Obj& result1 = src.assign(LENGTH, VALUE);
                    ASSERT(&result1 == &src);
                    const Obj src_copy(src);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        Obj& result = dst.assign(MoveUtil::move(src));
                        ASSERT(&result == &dst);                // ^--test here
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, dst == src_copy);
                    LOOP2_ASSERT(INIT_LINE, LINE,
                                         BB == testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(INIT_LINE, LINE,
                                          B >= testAllocator.numBlocksInUse());
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf(
            "\nTesting move assign with different allocators.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    Obj src(INIT_LENGTH, DEFAULT_VALUE);
                    Obj dst(INIT_LENGTH, DEFAULT_VALUE,
                            AllocType(&testAllocator));
                    ASSERT(src.get_allocator() != dst.get_allocator());

                    Obj& result1 = src.assign(LENGTH, VALUE);
                    ASSERT(&result1 == &src);
                    const Obj src_copy(src);

                    const bool noAlloc = dst.capacity() >= src.size();
                    const bool reAlloc = dst.capacity() > DEFAULT_CAPACITY &&
                                         !noAlloc;
                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    {
                        AllocatorUseGuard guardG(globalAllocator_p);
                        AllocatorUseGuard guardD(defaultAllocator_p);
                        Obj& result = dst.assign(MoveUtil::move(src));
                        ASSERT(&result == &dst);                   // test here
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE, dst == src);
                    LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                   src.get_allocator() != dst.get_allocator());

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();
                    if (noAlloc) {
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     BB + 0 == AA);
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     B  + 0 ==  A);
                    }
                    else {
                        LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                     BB + 1 == AA);
                        if (!reAlloc) {
                            LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                         B  + 1 ==  A);
                        }
                        else {
                            LOOP4_ASSERT(INIT_LINE, LINE, LENGTH, VALUE,
                                         B + 0 == A);
                        }
                    }
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf("\nTesting exception safety of move assignment"
                            " with string having different allocators.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;

                        Obj mY(LENGTH, VALUE);
                        const Obj& Y = mY;
                        ExceptionGuard<Obj> guard(X, L_);

                        testAllocator.setAllocationLimit(AL);

                        Obj& result = mX.assign(MoveUtil::move(Y));  // test
                        ASSERT(&result == &mX);
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == testAllocator.numMismatches());
                    ASSERT(0 == testAllocator.numBlocksInUse());
                }
            }
        }
    }

#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assign() exception-safety.\n");
#else
    if (verbose) printf("\nTesting assign() exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        Obj src(defaultCapacity + 1, '1', &testAllocator);

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();

        {
            // make the allocator throw on the next allocation
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                // the assignment will require to allocate more memory
                dst.assign(src);
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src, 0, Obj::npos);
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.c_str());
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.c_str(), src.size());
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.size(), 'c');
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        {
            testAllocator.setAllocationLimit(0);
            bool exceptionCaught = false;

            try
            {
                dst.assign(src.begin(), src.end());
            }
            catch (const bslma::TestAllocatorException &)
            {
                exceptionCaught = true;
            }
            catch (...)
            {
                exceptionCaught = true;
                ASSERT(0 && "Wrong exception caught");
            }

            ASSERT(exceptionCaught);
            ASSERT(dst == dstCopy);
        }

        // restore the allocator state
        testAllocator.setAllocationLimit(oldLimit);
    }
#endif

}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13StrRefData()
{
    // --------------------------------------------------------------------
    // TESTING 'assign' FROM STRING REF
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the 'bslstl::StringRefData' object.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   basic_string& assign(const StringRefData<CHAR_TYPE>& strRef);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        unsigned    d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        const Obj dummy;
        ASSERT(dummy.capacity() < DATA[NUM_DATA - 1].d_length);
    }

    if (verbose) printf("\tUsing 'bslstl::StringRefData'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj mY(g(SPEC)); const Obj& Y = mY;

                const bsl::basic_string_view<TYPE> strView(Y.begin(),
                                                           Y.length());

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                const size_t preCapacity = X.capacity();

                const bsls::Types::Int64 numAllocs =
                                                testAllocator.numAllocations();

                {
                    AllocatorUseGuard guardG(globalAllocator_p);
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    mX.assign(strView);
                }

                if (LENGTH <= preCapacity) {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                                  testAllocator.numAllocations() == numAllocs);
                }
                else {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                              testAllocator.numAllocations() == numAllocs + 1);
                }

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13InputIterator()
{
    // --------------------------------------------------------------------
    // TESTING 'assign' FROM STRING REF
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the 'bslstl::StringRefData' object.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        const Obj dummy;
        ASSERT(dummy.capacity() < (size_t)DATA[NUM_DATA - 1].d_length);
    }

    if (verbose) printf("\tUsing 'bslstl::StringRefData'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj mY(g(SPEC)); const Obj& Y = mY;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                {
                    AllocatorUseGuard guardG(globalAllocator_p);
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    mX.assign(InputIterator(Y.data()),
                              InputIterator(Y.data() + LENGTH));
                }

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   The concerns are the same as for the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class Iter> basic_string& assign(Iter first, Iter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                     >::value;

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       13   },
        { L_,       15   },
        { L_,       23   },
        { L_,       24   },
        { L_,       25   },
        { L_,       30   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }, // 15
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    {
        // Ensure at least the longest test sequence will exceed the short
        // string capacity.

        const Obj dummy;
        ASSERT(dummy.capacity() < (size_t)(DATA[NUM_DATA - 1].d_length));
    }

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES],
                       AllocType(&testAllocator));
                const Obj& X = mX;

                const size_t preCapacity = X.capacity();

                const bsls::Types::Int64 numAllocs =
                                                testAllocator.numAllocations();

                {
                    AllocatorUseGuard guardG(globalAllocator_p);
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    mX.assign(U.begin(), U.end());
                }

                if (LENGTH <= preCapacity) {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                                  testAllocator.numAllocations() == numAllocs);
                }
                else {
                    LOOP2_ASSERT(testAllocator.numAllocations(), numAllocs,
                              testAllocator.numAllocations() == numAllocs + 1);
                }

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP6_ASSERT(INIT_LINE, LINE, i, ti, LENGTH, X.size(),
                                                           LENGTH == X.size());
                if (!INPUT_ITERATOR_TAG) {
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 X.capacity() >= X.size());
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;
                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default char value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;  // control

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);  const Obj& X = mX;
                    ExceptionGuard<Obj> guard(X, L_);

                    testAllocator.setAllocationLimit(AL);

                    Obj& result = mX.assign(U.begin(), U.end());   // test here
                    ASSERT(&result == &mX);
                    guard.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(testAllocator.numMismatches(),
                            0 == testAllocator.numMismatches());
                LOOP_ASSERT(testAllocator.numBlocksInUse(),
                            0 == testAllocator.numBlocksInUse());
            }
        }
    }

    enum {
        ASSIGN_STRING_MODE_FIRST          = 0,
        ASSIGN_SUBSTRING                  = 0,
        ASSIGN_SUBSTRING_NPOS             = 1,
        ASSIGN_STRING                     = 2,
        ASSIGN_CSTRING_N                  = 3,
        ASSIGN_CSTRING                    = 4,
        ASSIGN_STRING_FROM_ITERATOR       = 5,
        ASSIGN_STRING_FROM_CONST_ITERATOR = 6,
        ASSIGN_STRING_MODE_LAST           = 6
    };

    for (int assignMode  = ASSIGN_STRING;
             assignMode <= ASSIGN_STRING_MODE_LAST;
             ++assignMode)
    {
        if (verbose)
            printf("\tUsing string with assignMode = %d.\n", assignMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    const size_t LENGTH = Y.size();

                    Obj mX(INIT_LENGTH,
                           DEFAULT_VALUE,
                           AllocType(&testAllocator));  const Obj& X = mX;
                    mX.reserve(INIT_RES);
                    const size_t INIT_CAP = X.capacity();

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = computeNewCapacity(LENGTH,
                                                          INIT_LENGTH,
                                                          INIT_CAP,
                                                          X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tAssign "); P_(NUM_ELEMENTS);
                        printf("using "); P(SPEC);
                    }

                    Obj mExp;  const Obj& EXP = mExp;
                    mExp.append(Y);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    switch (assignMode) {
                      case ASSIGN_STRING: {
                        // string& assign(const string& str);
                        Obj& result = mX.assign(Y);
                        ASSERT(&result == &mX);
                      } break;
                      case ASSIGN_CSTRING_N: {
                        // string& assign(const C *s, n);
                        Obj& result = mX.assign(Y.data(), NUM_ELEMENTS);
                        ASSERT(&result == &mX);
                      } break;
                      case ASSIGN_CSTRING: {
                        // string& assign(const C *s);
                        Obj& result = mX.assign(Y.c_str());
                        ASSERT(&result == &mX);
                      } break;
                      case ASSIGN_STRING_FROM_ITERATOR: {
                        // template <class InputIter>
                        //   void assign(InputIter first, last);
                        mX.assign(mU.begin(), mU.end());
                      } break;
                      case ASSIGN_STRING_FROM_CONST_ITERATOR: {
                        // template <class InputIter>
                        //   void assign(InputIter first, last);
                        mX.assign(U.begin(), U.end());
                      } break;
                      default: {
                        printf("***UNKNOWN ASSIGN MODE***\n");
                        ASSERT(0);
                      } break;
                    }

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                    if (!INPUT_ITERATOR_TAG) {
                        LOOP2_ASSERT(INIT_LINE, LINE, CAP == X.capacity());
                    }
                    LOOP2_ASSERT(INIT_LINE, LINE, EXP == X);

                    const int REALLOC = X.capacity() > INIT_CAP;
                    const int A_ALLOC = DEFAULT_CAPACITY >= INIT_CAP
                                     && X.capacity() > DEFAULT_CAPACITY;

                    LOOP2_ASSERT(INIT_LINE, INIT_LENGTH,
                                 BB + REALLOC <= AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, ti,
                                 B + A_ALLOC <=  A);
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing string with assignMode = %d.\n",
                        ASSIGN_SUBSTRING);
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    for (size_t k = 0; k <= NUM_ELEMENTS; ++k) {
                        const size_t POS = k;

                        const size_t NUM_ELEMENTS_INS = NUM_ELEMENTS - POS;
                        const size_t LENGTH           = NUM_ELEMENTS_INS;

                        Obj mX(INIT_LENGTH,
                               DEFAULT_VALUE,
                               AllocType(&testAllocator));  const Obj& X = mX;
                        mX.reserve(INIT_RES);
                        const size_t INIT_CAP = X.capacity();

                        size_t n;
                        for (n = 0; n < INIT_LENGTH; ++n) {
                            mX[n] =  VALUES[n % NUM_VALUES];
                        }

                        Obj mExp;  const Obj& EXP = mExp;
                        mExp.append(Y, POS, NUM_ELEMENTS);

                        const size_t CAP = computeNewCapacity(LENGTH,
                                                              INIT_LENGTH,
                                                              INIT_CAP,
                                                              X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tAssign"); P_(NUM_ELEMENTS_INS);
                            printf("using "); P_(SPEC);
                            printf("starting at "); P(POS);
                        }

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64  B = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        // string& assign(const string& str, pos, n);
                        Obj& result = mX.assign(Y, POS, NUM_ELEMENTS);
                        ASSERT(&result == &mX);

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64  A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP3_ASSERT(INIT_LINE, LINE, POS,
                                     LENGTH == X.size());
                        if (!INPUT_ITERATOR_TAG) {
                            LOOP3_ASSERT(INIT_LINE, LINE, POS,
                                         CAP == X.capacity());
                        }
                        LOOP2_ASSERT(INIT_LINE, LINE, EXP == X);

                        const int REALLOC = X.capacity() > INIT_CAP;
                        const int A_ALLOC =
                            DEFAULT_CAPACITY >= INIT_CAP &&
                            X.capacity() > DEFAULT_CAPACITY;

                        LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     BB + REALLOC <= AA);
                        LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                     B + A_ALLOC <=  A);
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");

    for (int assignMode  = ASSIGN_STRING_MODE_FIRST;
             assignMode <= ASSIGN_STRING_MODE_LAST;
             ++assignMode)
    {
        if (verbose)
            printf("\t\tUsing string with assignMode = %d.\n", assignMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using default char value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);

                    Obj mY(g(SPEC));  const Obj& Y = mY;

                    CONTAINER mU(Y);  const CONTAINER& U = mU;

                    const size_t LENGTH = Y.size();

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                      const Int64 AL = testAllocator.allocationLimit();
                      testAllocator.setAllocationLimit(-1);

                      Obj mX(INIT_LENGTH,
                             DEFAULT_VALUE,
                             AllocType(&testAllocator));
                      const Obj& X = mX;
                      mX.reserve(INIT_RES);

                      Obj mExp;  const Obj& EXP = mExp;
                      mExp.append(Y);

                      ExceptionGuard<Obj> guard(X, L_);
                      testAllocator.setAllocationLimit(AL);

                      switch (assignMode) {
                        case ASSIGN_STRING: {
                          // string& assign(const string& str);
                          Obj& result = mX.assign(Y);
                          ASSERT(&result == &mX);
                        } break;
                        case ASSIGN_SUBSTRING: {
                          // string& assign(const string& str, pos, n);
                          Obj& result = mX.assign(Y, 0, NUM_ELEMENTS);
                          ASSERT(&result == &mX);
                        } break;
                        case ASSIGN_SUBSTRING_NPOS: {
                          // string& assign(const string& str, pos, n);
                          Obj& result = mX.assign(Y, 0);  // 'npos' dflt. arg.
                          ASSERT(&result == &mX);
                        } break;
                        case ASSIGN_CSTRING_N: {
                          // string& assign(const C *s, n);
                          Obj& result = mX.assign(Y.data(), NUM_ELEMENTS);
                          ASSERT(&result == &mX);
                        } break;
                        case ASSIGN_CSTRING: {
                          // string& assign(const C *s);
                          Obj& result = mX.assign(Y.c_str());
                          ASSERT(&result == &mX);
                        } break;
                        case ASSIGN_STRING_FROM_ITERATOR: {
                          // template <class InputIter>
                          //  assign(InputIter first, last);
                          mX. assign(mU.begin(), mU.end());
                        } break;
                        case ASSIGN_STRING_FROM_CONST_ITERATOR: {
                          // template <class InputIter>
                          //   assign(InputIter first, last);
                          mX.assign(U.begin(), U.end());
                        } break;
                        default: {
                          printf("***UNKNOWN ASSIGN MODE***\n");
                          ASSERT(0);
                        } break;
                      }
                      guard.release();

                      if (veryVerbose) {
                          T_; T_; T_; P_(X); P(X.capacity());
                      }

                      LOOP2_ASSERT(INIT_LINE, LINE, LENGTH == X.size());
                      LOOP2_ASSERT(INIT_LINE, LINE, EXP    == X);

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");

    for (int assignMode  = ASSIGN_STRING_MODE_FIRST;
             assignMode <= ASSIGN_STRING_MODE_LAST;
             ++assignMode)
    {
        if (verbose)
            printf("\t\tUsing string with assignMode = %d.\n", assignMode);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_RES = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_RES);

                if (veryVerbose) {
                    printf("\t\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_RES);
                    printf("using distinct (cyclic) values.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, AllocType(&testAllocator));
                const Obj& X = mX;
                mX.reserve(INIT_RES);
                const size_t INIT_CAP = X.capacity();

                for (size_t k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] = VALUES[k % NUM_VALUES];
                }

                Obj mY(X); const Obj& Y = mY;  // control

                if (veryVerbose) {
                    printf("\t\t\tAssign with "); P(Y);
                }

                switch (assignMode) {
                  case ASSIGN_STRING: {
                    // string& assign(const string& str);
                    mX.assign(Y);
                    mY.assign(Y);
                  } break;
                  case ASSIGN_CSTRING_N: {
                    // string& assign(const C *s, n);
                    mX.assign(Y.data(), INIT_LENGTH);
                    mY.assign(Y.data(), INIT_LENGTH);
                  } break;
                  case ASSIGN_CSTRING: {
                    // string& assign(const C *s);
                    mX.assign(Y.c_str());
                    mY.assign(Y.c_str());
                  } break;
                  case ASSIGN_SUBSTRING: {
                    // string& assign(const string& str, pos, n);
                    mX.assign(Y, 0, INIT_LENGTH);
                    mY.assign(Y, 0, INIT_LENGTH);
                  } break;
                  case ASSIGN_SUBSTRING_NPOS: {
                    // string& assign(const string& str, pos, n);
                    mX.assign(Y, 0);  // 'npos' default arg.
                    mY.assign(Y, 0);
                  } break;
                  case ASSIGN_STRING_FROM_ITERATOR: {
                    // string& assign(InputIter first, last);
                    mX.assign(mY.begin(), mY.end());
                    mY.assign(mY.begin(), mY.end());
                  } break;
                  case ASSIGN_STRING_FROM_CONST_ITERATOR: {
                    // string& assign(InputIter first, last);
                    mX.assign(Y.begin(), Y.end());
                    mY.assign(Y.begin(), Y.end());
                  } break;
                  default: {
                    printf("***UNKNOWN ASSIGN MODE***\n");
                    ASSERT(0);
                  } break;
                }

                if (veryVerbose) {
                    T_; T_; T_; T_; P(X);
                    T_; T_; T_; T_; P(Y);
                }

                LOOP2_ASSERT(INIT_LINE, INIT_CAP, X == Y);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase13Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING 'assign'
    //
    // Concerns:
    //   'assign' methods asserts on undefined behavior when:
    //   1 C-string pointer parameter is NULL
    //   2 'first' and 'last' iterators do not make a valid range
    //      (i.e., first > last).
    //
    // Plan:
    //   For concern (1), create a string and call 'assign' methods with NULL
    //   pointer.
    //   For concern (2), create a string and call 'assign' with two iterators
    //   that do not make a valid range.
    //
    // Testing:
    //   void assign(const C *s, size_type n);
    //   void assign(const C *s);
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tnegative test assign with NULL C-string\n");

    {
        Obj mX;
        ASSERT_SAFE_FAIL(mX.assign(0));
        ASSERT_SAFE_PASS(mX.assign(0, size_t(0)));
        ASSERT_SAFE_FAIL(mX.assign(0, size_t(10)));

        Obj mY(g("ABCD"));
        ASSERT_SAFE_PASS(mX.assign(mY.c_str()));
        ASSERT_SAFE_PASS(mX.assign(mY.c_str(), mY.length()));
    }

    if (veryVerbose) {
        printf("\tnegative test assign with invalid iterator range\n");
    }

    {
        Obj mX;
        Obj mY(g("ABCD"));

        // first > last
        ASSERT_SAFE_FAIL(mX.assign(mY.end(), mY.begin()));
        ASSERT_SAFE_FAIL(mX.assign(mY.begin() + 1, mY.begin()));

        // first <= last
        ASSERT_SAFE_PASS(mX.assign(mY.begin(), mY.end()));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTORS:
    //
    // Concerns:
    //  1) The initial value is correct.
    //  2) The initial capacity is correctly set up.
    //  3) The constructor is exception neutral w.r.t. memory allocation.
    //  4) The internal memory management system is hooked up properly so that
    //     *all* internally allocated memory draws from a  user-supplied
    //     allocator whenever one is specified.
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   basic_string(const basic_string& str, pos, a = A());
    //   basic_string(const basic_string& str, pos, n, a = A());
    //   basic_string(const CHAR_TYPE *s, a = A());
    //   basic_string(const CHAR_TYPE *s, size_type n, a = A());
    //   basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE           *values     = 0;
    const TYPE *const&    VALUES     = values;
    const int             NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // specifications
    } DATA[] = {
        //line  spec                                       length
        //----  ----                                       ------
        { L_,   ""                                      }, //   0
        { L_,   "A"                                     }, //   1
        { L_,   "AB"                                    }, //   2
        { L_,   "ABC"                                   }, //   3
        { L_,   "ABCD"                                  }, //   4
        { L_,   "ABCDE"                                 }, //   5
        { L_,   "ABCDEA"                                }, //   6
        { L_,   "ABCDEAB"                               }, //   7
        { L_,   "ABCDEABC"                              }, //   8
        { L_,   "ABCDEABCD"                             }, //   9
        { L_,   "ABCDEABCDEA"                           }, //  11
        { L_,   "ABCDEABCDEAB"                          }, //  12
        { L_,   "ABCDEABCDEABC"                         }, //  13
        { L_,   "ABCDEABCDEABCDE"                       }, //  15
        { L_,   "ABCDEABCDEABCDEA"                      }, //  16
        { L_,   "ABCDEABCDEABCDEAB"                     }, //  17
        { L_,   "ABCDEABCDEABCDEABCDEABC"               }, //  23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"              }, //  24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"             }, //  25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEA"       }, //  31
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"      }, //  32
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEABC"     }, //  33
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                      "ABCDEABCDEABCDEABCDEABCDEABCDEA" }, //  63
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEAB"  }, //  64
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDEAB"
                    "ABCDEABCDEABCDEABCDEABCDEABCDEABC" }  //  65
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting string(n, c, a = A()).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);
            const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

            if (veryVerbose) {
                printf("\t\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                const char           *minorStr;
                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    minorStr = "\t\tWithout passing in an allocator.\n";
                    objPtr = new (fa) Obj(LENGTH, VALUE);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    minorStr = "\t\tWith passing in null allocator.\n";
                    objPtr = new (fa) Obj(
                                  LENGTH,
                                  VALUE,
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    minorStr = "\t\tWith passing in an allocator.\n";
                    objPtr = new (fa) Obj(LENGTH, VALUE, &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                          // RETURN
                }

                if (verbose) {
                    printf("%s", minorStr);
                }

                Obj&                   mX = *objPtr;
                const Obj&              X =  mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa =  'c' != CONFIG ? sa : da;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }

                if (LENGTH <= DEFAULT_CAPACITY) {
                    LOOP2_ASSERT(LINE, ti, 0 ==  oa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 ==  oa.numBlocksInUse());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksInUse());
                } else {
                    LOOP2_ASSERT(LINE, ti, 1 ==  oa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 1 ==  oa.numBlocksInUse());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksInUse());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
    }

    if (verbose) printf("\tTesting string(const C *s, a = A())\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj        mY(g(SPEC));  // source object
            const Obj&  Y = mY;

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                const char           *minorStr;
                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    minorStr = "\t\tWithout passing in an allocator.\n";
                    objPtr = new (fa) Obj(&Y[0]);
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    minorStr = "\t\tWith passing in null allocator.\n";
                    objPtr = new (fa) Obj(
                                 &Y[0],
                                 reinterpret_cast<bslma::TestAllocator *>(0));
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    minorStr = "\t\tWith passing in an allocator.\n";
                    objPtr = new (fa) Obj(&Y[0], &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                if (verbose) {
                    printf("%s", minorStr);
                }

                Obj&                   mX = *objPtr;
                const Obj&              X =  mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa =  'c' != CONFIG ? sa : da;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y      == X);

                if (LENGTH <= DEFAULT_CAPACITY) {
                    LOOP2_ASSERT(LINE, ti, 0 ==  oa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 ==  oa.numBlocksInUse());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksInUse());
                } else {
                    LOOP2_ASSERT(LINE, ti, 1 ==  oa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 1 ==  oa.numBlocksInUse());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksTotal());
                    LOOP2_ASSERT(LINE, ti, 0 == noa.numBlocksInUse());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
    }

    if (verbose) printf("\tTesting string(const C *s, n, a = A())\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE   = DATA[ti].d_lineNum;
            const char  *SPEC   = DATA[ti].d_spec;
            const size_t LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\t\tCreating object up to "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj        mY(g(SPEC));  // source object
            const Obj&  Y = mY;

            for (size_t i = 0; i < LENGTH; ++i) {
                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                    const char   CONFIG     = cfg;
                    const size_t EXP_LENGTH = i;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    const char           *minorStr;
                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        minorStr = "\t\tWithout passing in an allocator.\n";
                        objPtr = new (fa) Obj(&Y[0], EXP_LENGTH);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        minorStr = "\t\tWith passing in null allocator.\n";
                        objPtr = new (fa) Obj(
                                  &Y[0],
                                  EXP_LENGTH,
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        minorStr = "\t\tWith passing in an allocator.\n";
                        objPtr = new (fa) Obj(&Y[0], EXP_LENGTH, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                      // RETURN
                    }

                    if (veryVerbose) {
                        printf("%s", minorStr);
                    }

                    Obj&                   mX = *objPtr;
                    const Obj&              X =  mX;
                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa =  'c' != CONFIG ? sa : da;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH == X.size());
                    LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH <= X.capacity());

                    for (size_t j = 0; j < EXP_LENGTH; ++j) {
                        LOOP4_ASSERT(LINE, ti, i, j, Y[j] == X[j]);
                    }

                    if (EXP_LENGTH <= DEFAULT_CAPACITY) {
                        LOOP3_ASSERT(LINE, ti, i, 0 ==  oa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 ==  oa.numBlocksInUse());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksInUse());
                    } else {
                        LOOP3_ASSERT(LINE, ti, i, 1 ==  oa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 1 ==  oa.numBlocksInUse());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksInUse());
                    }

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }
            }
        }
    }

    if (verbose) printf("\tTesting string(str, pos, a = A()).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\t\tCreating object up to "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj        mY(g(SPEC));  // source object
            const Obj&  Y = mY;

            for (size_t i = 0; i < LENGTH; ++i) {
                const size_t POS        = i;
                const size_t EXP_LENGTH = LENGTH - POS;

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                    const char CONFIG = cfg;

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    const char           *minorStr;
                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        minorStr = "\t\tWithout passing in an allocator.\n";
                        objPtr = new (fa) Obj(Y, POS);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        minorStr = "\t\tWith passing in null allocator.\n";
                        objPtr = new (fa) Obj(
                                  Y,
                                  POS,
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        minorStr = "\t\tWith passing in an allocator.\n";
                        objPtr = new (fa) Obj(Y, POS, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                      // RETURN
                    }

                    if (veryVerbose) {
                        printf("%s", minorStr);
                    }

                    Obj&                   mX = *objPtr;
                    const Obj&              X =  mX;
                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa =  'c' != CONFIG ? sa : da;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH == X.size());
                    LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH <= X.capacity());

                    for (size_t j = POS; j < EXP_LENGTH; ++j) {
                        LOOP4_ASSERT(LINE, ti, i, j, Y[j] == X[j-POS]);
                    }

                    if (EXP_LENGTH <= DEFAULT_CAPACITY) {
                        LOOP3_ASSERT(LINE, ti, i, 0 ==  oa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 ==  oa.numBlocksInUse());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksInUse());
                    } else {
                        LOOP3_ASSERT(LINE, ti, i, 1 ==  oa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 1 ==  oa.numBlocksInUse());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksTotal());
                        LOOP3_ASSERT(LINE, ti, i, 0 == noa.numBlocksInUse());
                    }

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }
            }
        }
    }

    if (verbose) printf("\tTesting string(str, pos, n, a = A()).\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\t\tCreating object up to "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj        mY(g(SPEC));  // source object
            const Obj&  Y = mY;

            for (size_t i = 0; i < LENGTH; ++i) {
                const size_t POS = i;
                for (size_t j = 0; j < LENGTH - POS; ++j) {
                    const size_t EXP_LENGTH = j;
                    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                        const char CONFIG = cfg;

                        bslma::TestAllocator da("default",
                                                veryVeryVeryVerbose);
                        bslma::TestAllocator fa("footprint",
                                                veryVeryVeryVerbose);
                        bslma::TestAllocator sa("supplied",
                                                veryVeryVeryVerbose);

                        bslma::DefaultAllocatorGuard dag(&da);

                        const char           *minorStr;
                        Obj                  *objPtr;
                        bslma::TestAllocator *objAllocatorPtr;

                        switch (CONFIG) {
                          case 'a': {
                            minorStr =
                                      "\t\tWithout passing in an allocator.\n";
                            objPtr = new (fa) Obj(Y, POS, EXP_LENGTH);
                            objAllocatorPtr = &da;
                          } break;
                          case 'b': {
                            minorStr = "\t\tWith passing in null allocator.\n";
                            objPtr = new (fa) Obj(
                                  Y,
                                  POS,
                                  EXP_LENGTH,
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                            objAllocatorPtr = &da;
                          } break;
                          case 'c': {
                            minorStr = "\t\tWith passing in an allocator.\n";
                            objPtr = new (fa) Obj(Y, POS, EXP_LENGTH, &sa);
                            objAllocatorPtr = &sa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }

                        if (veryVerbose) {
                            printf("%s", minorStr);
                        }

                        Obj&                   mX = *objPtr;
                        const Obj&              X =  mX;
                        bslma::TestAllocator&  oa = *objAllocatorPtr;
                        bslma::TestAllocator& noa =  'c' != CONFIG ? sa : da;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH == X.size());
                        LOOP3_ASSERT(LINE, ti, i, EXP_LENGTH <= X.capacity());

                        for (size_t k = POS; k < POS + EXP_LENGTH; ++k) {
                            LOOP5_ASSERT(LINE, ti, i, j, k, Y[k] == X[k-POS]);
                        }

                        if (EXP_LENGTH <= DEFAULT_CAPACITY) {
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 ==  oa.numBlocksTotal());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 ==  oa.numBlocksInUse());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 == noa.numBlocksTotal());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 == noa.numBlocksInUse());
                        } else {
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         1 ==  oa.numBlocksTotal());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         1 ==  oa.numBlocksInUse());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 == noa.numBlocksTotal());
                            LOOP4_ASSERT(LINE, ti, i, j,
                                         0 == noa.numBlocksInUse());
                        }

                        // Reclaim dynamically allocated object under test.

                        fa.deleteObject(objPtr);

                        // Verify all memory is released on object destruction.

                        ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                                0 == da.numBlocksInUse());
                        ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                                0 == fa.numBlocksInUse());
                        ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                                0 == sa.numBlocksInUse());
                    }
                }
            }
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);
            const TYPE    VALUE  = VALUES[ti % NUM_VALUES];

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(VALUE);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(LENGTH, VALUE, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (veryVerbose) {
                printf("\t\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj mY(g(SPEC));  const Obj& Y = mY;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(&Y[0], AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(&Y[0], LENGTH, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(Y, 0, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(Y, 0, LENGTH, AllocType(&testAllocator));
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tAllocators hooked up properly.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);
            const TYPE    VALUE  = VALUES[ti % NUM_VALUES];

            if (veryVerbose) {
                printf("\t\tCreating object of "); P(LENGTH);
            }

            Obj        mY(g(SPEC));
            const Obj&  Y = mY;

            for (int i = 0; i < 5; ++i) {
                AllocatorUseGuard guardG(globalAllocator_p);
                AllocatorUseGuard guardD(defaultAllocator_p);

                switch (i) {
                  case 0: {
                    Obj x(LENGTH, VALUE, objectAllocator_p);
                  } break;
                  case 1: {
                    Obj x(&Y[0], objectAllocator_p);
                  } break;
                  case 2: {
                    Obj x(&Y[0], LENGTH, objectAllocator_p);
                  } break;
                  case 3: {
                    Obj x(Y, 0, objectAllocator_p);
                  } break;
                  case 4: {
                    Obj x(Y, 0, LENGTH, objectAllocator_p);
                  } break;
                }

                ASSERTV(LINE, 0 == globalAllocator_p->numBytesInUse());
                ASSERTV(LINE, 0 == objectAllocator_p->numBytesInUse());
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS:
    //
    //   Concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported and then moved if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    4) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    5) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   We will create objects of varying sizes and capacities containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   template<class InputIter>
    //     string(InputIter first, InputIter last, a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const int INPUT_ITERATOR_TAG =
         bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
#if 1  // #ifndef BSLS_PLATFORM_CPU_64_BIT
        { L_,   "ABCDEABCDEA"                        }, // 11
        { L_,   "ABCDEABCDEAB"                       }, // 12
        { L_,   "ABCDEABCDEABC"                      }, // 13
        { L_,   "ABCDEABCDEABCDE"                    }  // 15
#else
        { L_,   "ABCDEABCDEABCDEABCDEABC"            }, // 23
        { L_,   "ABCDEABCDEABCDEABCDEABCD"           }, // 24
        { L_,   "ABCDEABCDEABCDEABCDEABCDE"          }, // 25
        { L_,   "ABCDEABCDEABCDEABCDEABCDEABCDE"     }  // 30
#endif
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) { printf("\t\tCreating object "); P(SPEC); }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            AllocatorUseGuard guardG(globalAllocator_p);
            AllocatorUseGuard guardD(defaultAllocator_p);
            Obj mX(U.begin(), U.end(), AllocType(&testAllocator));
            const Obj& X = mX;

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }

            if (LENGTH <= DEFAULT_CAPACITY) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else if (INPUT_ITERATOR_TAG) {
             // LOOP2_ASSERT(LINE, ti, BB + 1 + NUM_ALLOCS[LENGTH] == AA);
                LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
            } else {
             // LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 1 ==  A);
            }
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));        const Obj& Y = mY;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(U.begin(), U.end(), AllocType(&testAllocator));

                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            if (LENGTH <= DEFAULT_CAPACITY) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else {
                if (INPUT_ITERATOR_TAG) {
                    LOOP2_ASSERT(LINE, ti, BB + 1 <= AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
            }

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase12Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING FOR CONSTRUCTORS:
    //
    // Concerns:
    //   1 Constructing a string with a NULL pointer, or integral NULL value
    //   including 'false' is undefined behavior and it asserts.
    //   2 Constructing a string with an invalid iterator range asserts on
    //   undefined behavior.
    //
    // Plan:
    //   For (1), call the proper constructor with a NULL value and verify that
    //   it asserts.  Then call the same constructor with a valid C-string
    //   pointer and verify that it doesn't assert.
    //   For (2), construct a string with the constructor taking an invalid
    //   iterator range ('first > last') and verify that it asserts.
    //
    // Testing:
    //   string<C,CT,A>(const C *s, n, a = A());
    //   string<C,CT,A>(const C *s, a = A());
    //   template<class InputIter>
    //     string(InputIter first, InputIter last, a = A());
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    ASSERT_SAFE_FAIL_RAW(Obj(0));

    ASSERT_SAFE_PASS_RAW(Obj(0, size_t(0)));
    ASSERT_SAFE_FAIL_RAW(Obj(0, size_t(10)));

    Obj mY(g("ABCDE"));
    ASSERT_SAFE_FAIL_RAW(Obj(mY.end(), mY.begin()));

    Obj mX(g("ABCDE"));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str()));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str(), 0));
    ASSERT_SAFE_PASS_RAW(Obj(mX.c_str(), mX.length()));
    ASSERT_SAFE_PASS_RAW(Obj(mY.begin(), mY.end()));
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCaseM1(const int /* NITER */,
                                               const int /* RANDOM_SEED */)
{
    // --------------------------------------------------------------------
    // PERFORMANCE TEST
    //
    // Concerns:
    //   1) That performance does not regress between versions.
    //   2) That no surprising performance (both extremely fast or slow) is
    //      detected, which might be indicating missed optimizations or
    //      inadvertent loss of performance (e.g., by wrongly setting the
    //      capacity and triggering too frequent reallocations).
    //   3) That small "improvements" can be tested w.r.t. to performance, in a
    //      uniform benchmark (e.g., measuring the overhead of allocating for
    //      empty strings).
    //
    // Plan:
    //   We follow a simple benchmark that performs the operation under a
    //   timing test in a loop.  Specifically, we wish to measure the time
    //   taken by:
    //     C1) The various constructors.
    //     C2) The copy constructor.
    //     A1) The copy assignment.
    //     A2) The 'assign' operations.
    //     P1) The 'append' operation in its various forms (including
    //         'push_back').
    //     I1) The 'insert' operation in its various forms, at the end (should
    //         be compared to the corresponding 'append' sequence).
    //     B1) The 'insert' operation in its various forms.
    //     M3) The 'replace' operation in its various forms.
    //     S1) The 'swap' operation in its various forms.
    //     F1) The 'find' and 'rfind' operations.
    //     F2) The 'find_first_of' and 'find_last_of' operations.
    //     F3) The 'find_first_not_of' and 'find_last_not_of' operations.
    //   Also we wish to record the size of the various string
    //   instantiations.
    //
    //   We create two tables, one containing long strings, and another
    //   containing short strings.  All strings are preallocated so that we do
    //   not measure the performance of the random generator.  In order not to
    //   measure the time overhead of the test allocator, we use the default
    //   allocator throughout.  As for choosing the overloads to use, we rely
    //   on the implementation and avoid those that are simple inline
    //   forwarding calls (this might need to be adjusted for different
    //   implementations, however most implementations rely on a single
    //   implementation and the other calls can reduce to it).

    //   Note: This is a *synthetic* benchmark.  It does not replace measuring
    //   real benchmarks (e.g., for strings, ADSP) whose conclusions may differ
    //   due to different memory allocation and access patterns, function call
    //   frequencies, etc.  Its main use is for comparing two implementations
    //   (versions) against the same benchmark, and to test that one
    //   improvement in one function does not translate into a slow-down in
    //   another function.
    //
    //   Also note, that this is spaghetti code but we make no attempt at
    //   shortening it with helper functions or macros, since we feel that
    //
    // Testing:
    //   This "test" measures performance of basic operations, for performance
    //   regression.
    //
    //
    // RESULTS: Native SunProSTL on sundev13 as of Tue Jun 24 16:48:36 EDT 2008
    // ------------------------------------------------------------------------
    //
    //  String size:            4 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.003849s       0.000098s       0.003947s
    //      C2  From 'char*':   0.01829s        0.00043s        0.018722s
    //      C3  Copy ctor:      0.02700s        0.00044s        0.027436s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.02696s        0.00051s        0.027473s
    //      A2  Assign string:  0.10399s        0.00984s        0.113824s
    //      A3  Assign range:   0.11826s        0.01286s        0.131116s
    //  Append (with reallocations):
    //      P1  Append chars:   0.58007s        2.54784s        3.127911s
    //      P2  Append string:  0.87475s        3.07695s        3.951698s
    //          (without reallocation):
    //      P3  Append string:  0.13705s        0.02840s        0.165452s
    //      P4  Append range:   0.15452s        0.07269s        0.227208s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.22898s        2.03761s        2.266580s
    //      I2  Insert string:  0.54691s        2.39825s        2.945158s
    //          at end (without reallocation):
    //      I3  Insert string:  0.15257s        0.06108s        0.213649s
    //      I4  Insert range:   0.17466s        0.07843s        0.253086s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.23185s        2.17408s        2.405936s
    //      B2  Insert string:  0.24647s        2.07533s        2.321807s
    //          (without reallocation):
    //      B3  Insert string:  0.14609s        0.22978s        0.375868s
    //      B4  Insert range:   0.17793s        0.20753s        0.385453s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16203s        0.06911s        0.231147s
    //      R2  Replace at begin: 0.15793s      1.08946s        1.247386s
    //  Misc.:
    //      M1  Swap reverse:   0.07511s        0.06057s        0.135686s
    //      M2  Substring:      5.60723s        14.51155s       20.118778s
    //
    // RESULTS: bslstl 1.11 on sundev13 as of Mon Jun 30 14:23:52 EDT 2008
    // --------------------------------------------------------------------
    //
    //  String size:            16 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //  Total amount of work should be about same in both
    //  long and short columns (one long string vs. many
    //  short strings), except for reallocations.
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.007775s       0.000244s       0.008019s
    //      C2  From 'char*':   0.06390s        0.00924s        0.073137s
    //      C3  Copy ctor:      0.06638s        0.00931s        0.075684s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.02828s        0.01366s        0.041943s
    //      A2  Assign string:  0.02685s        0.01311s        0.039953s
    //      A3  Assign range:   0.02439s        0.01429s        0.038676s
    //  Append (with reallocations):
    //      P1  Append chars:   0.56922s        0.52341s        1.092630s
    //      P2  Append string:  0.69453s        0.60602s        1.300548s
    //          (without reallocation):
    //      P3  Append string:  0.13577s        0.02976s        0.165528s
    //      P4  Append range:   0.13689s        0.06424s        0.201128s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.23625s        0.19838s        0.434627s
    //      I2  Insert string:  0.22206s        0.16017s        0.382224s
    //          at end (without reallocation):
    //      I3  Insert string:  0.13643s        0.06546s        0.201888s
    //      I4  Insert range:   0.14023s        0.06459s        0.204820s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.22982s        1.27716s        1.506978s
    //      B2  Insert string:  0.22201s        0.38553s        0.607539s
    //          (without reallocation):
    //      B3  Insert string:  0.14811s        0.49707s        0.645182s
    //      B4  Insert range:   0.14965s        0.50169s        0.651337s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16184s        0.07699s        0.238831s
    //      R2  Replace at begin: 0.16303s      1.23734s        1.400366s
    //  Misc.:
    //      M1  Swap reverse:   0.23449s        0.09505s        0.329545s
    //      M2  Substring:      5.18101s        31.76464s       36.945650s
    //
    // RESULTS: Native gcc STL on sundev13 as of Tue Jun 24 16:48:36 EDT 2008
    // ----------------------------------------------------------------------
    //
    //  String size:            4 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.001821s       0.000066s       0.001887s
    //      C2  From 'char*':   0.01383s        0.00048s        0.014304s
    //      C3  Copy ctor:      0.01726s        0.00019s        0.017452s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.00372s        0.00007s        0.003786s
    //      A2  Assign string:  0.00128s        0.00003s        0.001310s
    //      A3  Assign range:   0.11978s        0.01919s        0.138965s
    //  Append (with reallocations):
    //      P1  Append chars:   0.61198s        0.48359s        1.095569s
    //      P2  Append string:  0.85236s        0.66129s        1.513646s
    //          (without reallocation):
    //      P3  Append string:  0.15236s        0.02859s        0.180949s
    //      P4  Append range:   0.16615s        0.05822s        0.224371s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.19453s        0.15631s        0.350844s
    //      I2  Insert string:  0.19757s        0.14761s        0.345184s
    //          at end (without reallocation):
    //      I3  Insert string:  0.14629s        0.05944s        0.205725s
    //      I4  Insert range:   0.14613s        0.06080s        0.206930s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.21156s        0.22866s        0.440219s
    //      B2  Insert string:  0.24475s        0.23070s        0.475451s
    //          (without reallocation):
    //      B3  Insert string:  0.15124s        0.19940s        0.350643s
    //      B4  Insert range:   0.18475s        0.21291s        0.397659s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.16595s        0.07109s        0.237031s
    //      R2  Replace at begin: 0.22566s      1.30403s        1.529692s
    //  Misc.:
    //      M1  Swap reverse:   1.30312s        0.39883s        1.701954s
    //      M2  Substring:      4.79876s        11.50113s       16.299887s
    //
    // RESULTS: Native STL on ibm1 as of  Tue Jun 24 16:48:36 EDT 2008
    // ------------------------------------------------------------------
    //
    //  String size:            16 bytes.
    //  Using 50000 short words and 1000 long words.
    //  Total length about same: 999618 (short) and 997604 (long).
    //
    //                          Short           Long            Total
    //                          -----           ----            -----
    //  Constructors:
    //      C1  Default ctor:   0.001741s       0.000031s       0.001772s
    //      C2  From 'char*':   0.02315s        0.00278s        0.025921s
    //      C3  Copy ctor:      0.02373s        0.00277s        0.026501s
    //  Assignment (no reallocation):
    //      A1  'operator=':    0.00253s        0.00071s        0.003236s
    //      A2  Assign string:  0.00672s        0.00098s        0.007693s
    //      A3  Assign range:   0.00788s        0.00042s        0.008292s
    //  Append (with reallocations):
    //      P1  Append chars:   0.15841s        0.17874s        0.337155s
    //      P2  Append string:  0.17893s        0.18019s        0.359119s
    //          (without reallocation):
    //      P3  Append string:  0.06268s        0.01879s        0.081468s
    //      P4  Append range:   0.04941s        0.03089s        0.080301s
    //  Insertions at end (with reallocations):
    //      I1  Insert chars:   0.06832s        0.06723s        0.135552s
    //      I2  Insert string:  0.07338s        0.06499s        0.138365s
    //          at end (without reallocation):
    //      I3  Insert string:  0.04835s        0.02264s        0.070989s
    //      I4  Insert range:   0.05462s        0.02913s        0.083743s
    //  Insertions at beginning (with reallocations):
    //      B1  Insert chars:   0.06699s        0.07817s        0.145161s
    //      B2  Insert string:  0.07192s        0.07696s        0.148877s
    //          (without reallocation):
    //      B3  Insert string:  0.03464s        0.03325s        0.067894s
    //      B4  Insert range:   0.04500s        0.04113s        0.086138s
    //  Replacements (without reallocation):
    //      R1  Replace at end: 0.05194s        0.02473s        0.076671s
    //      R2  Replace at begin: 0.04611s      0.29988s        0.345990s
    //  Misc.:
    //      M1  Swap reverse:   0.01834s        0.01893s        0.037267s
    //      M2  Substring:      2.23025s        6.25982s        8.490068s
    //
    // RESULTS: bslstl 1.11 on ibm1 as of Mon Jun 30 14:24:46 EDT 2008
    // --------------------------------------------------------------------
    //
    // --------------------------------------------------------------------

    bsls::Stopwatch t;

    printf("\n\tString size:\t\t" ZU " bytes.\n", sizeof(Obj));

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    // DATA  INITIALIZATION (NOT TIMED)
    const int SHORT_LENGTH = 16;     // length of short words
    const int SHORT_SIGMA  = 8;      // max deviation for short words
    const int NSHORT       = 50000;  // number of short words

    const int LONG_LENGTH  = 800;    // length of long words
    const int LONG_SIGMA   = 400;    // max deviation for long words
    const int NLONG        = 1000;   // number of long words

    ASSERT(NSHORT * (SHORT_LENGTH + SHORT_SIGMA / 2) ==
                                       NLONG * (LONG_LENGTH + LONG_SIGMA / 2));

    const int SL_RATIO     = NSHORT / NLONG;
    ASSERT(NLONG < NSHORT && 0 < SL_RATIO );

    Obj *shortWords = new Obj[NSHORT];
    size_t totalShortLength = 0;

    Obj *longWords  = new Obj[NLONG];
    size_t totalLongLength = 0;

    for (int i = 0; i < NSHORT; ++i) {
        const size_t LENGTH = SHORT_LENGTH + (rand() % (SHORT_SIGMA + 1));
        shortWords[i].assign(LENGTH, DEFAULT_VALUE);
        totalShortLength += LENGTH;
    }

    for (int i = 0; i < NLONG; ++i) {
        const size_t LENGTH = LONG_LENGTH + (rand() % (LONG_SIGMA + 1));
        longWords[i].assign(LENGTH, DEFAULT_VALUE);
        totalLongLength += LENGTH;
    }

    printf("\tUsing %d short words and %d long words.\n"
           "\tTotal length about same: " ZU " (short) and " ZU " (long).\n\n",
           NSHORT, NLONG, totalShortLength, totalLongLength);

    printf("\tTotal amount of work should be about same in both\n"
           "\tlong and short columns (one long string vs. many\n"
           "\tshort strings), except for reallocations.\n\n");

    printf("\t\t\t\tShort\t\tLong\t\tTotal\n");
    printf("\t\t\t\t-----\t\t----\t\t-----\n");

    printf("\tConstructors:\n");
    // {
        // C1) CONSTRUCTORS
        double timeC1 = 0., timeC1short = 0., timeC1long = 0.;

        t.reset(); t.start();
        Obj *shortStrings = new Obj[NSHORT];
        timeC1short = t.elapsedTime();
        size_t *shortStringLength = new size_t[NSHORT];

        t.reset(); t.start();
        Obj *longStrings  = new Obj[NLONG];
        timeC1long = t.elapsedTime();
        size_t *longStringLength = new size_t[NLONG];

        printf("\t    C1\tDefault ctor:\t%1.6fs\t%1.6fs\t%1.6fs\n",
               timeC1short, timeC1long, timeC1 = timeC1short + timeC1long);

        // C2) CONSTRUCTORS
        double timeC2 = 0., timeC2short = 0., timeC2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {
            (shortStrings + i)->~Obj();
        }
        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            new(shortStrings + i) Obj(shortWords[i]);
        }
        timeC2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringLength[i] = shortStrings[i].length();
        }

        for (int i = 0; i < NLONG; ++i) {
            (longStrings + i)->~Obj();
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            new(longStrings + i) Obj(longWords[i]);
        }
        timeC2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) {
            longStringLength[i] = longStrings[i].length();
        }

        printf("\t    C2\tFrom 'char*':\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeC2short, timeC2long, timeC2 = timeC2short + timeC2long);

        // C3) COPY CONSTRUCTOR (with allocations)
        double timeC3 = 0., timeC3short = 0., timeC3long = 0.;
        void *shortStringBufCopy = new bsls::ObjectBuffer<Obj>[NSHORT];
        void *longStringBufCopy  = new bsls::ObjectBuffer<Obj>[NLONG];
        Obj *shortStringCopy = (Obj *)shortStringBufCopy;
        Obj *longStringCopy  = (Obj *)longStringBufCopy;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            new(shortStringCopy + i) Obj(shortStrings[i]);
        }
        timeC3short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            new(longStringCopy + i) Obj(longStrings[i]);
        }
        timeC3long = t.elapsedTime();

        printf("\t    C3\tCopy ctor:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeC3short, timeC3long, timeC3 = timeC3short + timeC3long);
    // }

    printf("\tAssignment (no reallocation):\n");
    // {
        // A1) COPY ASSIGNMENT (without allocations)
        double timeA1 = 0., timeA1short = 0., timeA1long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i] = shortStrings[i];
        }
        timeA1short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i] = longStrings[i];
        }
        timeA1long = t.elapsedTime();

        printf("\t    A1\t'operator=':\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA1short, timeA1long, timeA1 = timeA1short + timeA1long);

        // A2) ASSIGN OPERATIONS (again, no allocation)
        double timeA2 = 0., timeA2short = 0., timeA2long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i].assign(shortStrings[i]);
        }
        timeA2short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i].assign(longStrings[i]);
        }
        timeA2long = t.elapsedTime();

        printf("\t    A2\tAssign string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA2short, timeA2long, timeA2 = timeA2short + timeA2long);

        // A3.
        double timeA3 = 0., timeA3short = 0., timeA3long = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NSHORT; ++i) {
            shortStringCopy[i].assign(shortStrings[i].begin(),
                                      shortStrings[i].end());
        }
        timeA3short = t.elapsedTime();

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            longStringCopy[i].assign(longStrings[i].begin(),
                                     longStrings[i].end());
        }
        timeA3long = t.elapsedTime();

        printf("\t    A3\tAssign range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeA3short, timeA3long, timeA3 = timeA3short + timeA3long);
    // }

#define COMPARE_WITH_NATIVE_SUNPRO_STL 1
    // When comparing performance with the native Sunpro STL (based on Rogue
    // Wave), a bug causes the native STL to reallocate every time after
    // push_back, instead of doubling the capacity.  This prohibits the testing
    // of P1, I1, and B1.

    printf("\tAppend (with reallocations):\n");
    // {
        // P1) PUSH_BACK OPERATION (with reallocations, i.e., changes capacity)
        // Pushing individual chars into either short/long strings, should take
        // slightly shorter for long strings since there will be fewer
        // reallocations.

        double timeP1 = 0., timeP1short = 0., timeP1long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].push_back(longStringCopy[ilong][j]);
            }
#else
            shortStrings[i].append(longStringCopy[ilong]);
#endif
        }
        timeP1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k) {
                    longStrings[i].push_back(longStringCopy[i][j]);
                }
            }
#else
            for (int k = 0; k < SL_RATIO; ++k) {
                longStrings[i].append(longStringCopy[i]);
            }
#endif
        }
        timeP1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    P1\tAppend chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP1short, timeP1long, timeP1 = timeP1short + timeP1long);

        // P2) APPEND OPERATION (with reallocations, i.e., changes capacity)
        // Unlike P1, pushing whole string at once will be more efficient since
        // only one allocation for short strings (final length is known), or
        // only a few reallocations for long strings.

        double timeP2 = 0., timeP2short = 0., timeP2long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong]);
        }
        timeP2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i]);
            }
        }
        timeP2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P2\tAppend string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP2short, timeP2long, timeP2 = timeP2short + timeP2long);
    // }

    printf("\t\t(without reallocation):\n");
    // {
        // P3) AGAIN (this time, without reallocation)
        double timeP3 = 0., timeP3short = 0., timeP3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong]);
        }
        timeP3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i, ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i]);
            }
        }
        timeP3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P3\tAppend string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP3short, timeP3long, timeP3 = timeP3short + timeP3long);

        // P4) APPEND RANGE
        double timeP4 = 0., timeP4short = 0., timeP4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].append(longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeP4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].append(longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeP4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    P4\tAppend range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeP4short, timeP4long, timeP4 = timeP4short + timeP4long);
    // }

    printf("\tInsertions at end (with reallocations):\n");
    // {
        // I1) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeI1 = 0., timeI1short = 0., timeI1long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStrings[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
            // ASSERT(shortStringLength[i] == shortStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].insert(shortStrings[i].end(),
                                       longStrings[ilong][j]);
            }
#else
            shortStrings[i].insert(shortStrings[i].length(),
                                   longStrings[ilong]);
#endif
        }
        timeI1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {  // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStrings[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
            // ASSERT(longStringLength[i] == longStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (int j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k) {
                    longStrings[i].insert(longStrings[i].end(),
                                          longStringCopy[i][j]);
                }
            }
#else
            for (int k = 0; k < SL_RATIO; ++k) {
                longStrings[i].insert(longStrings[i].length(),
                                      longStringCopy[i]);
            }
#endif
        }
        timeI1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    I1\tInsert chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI1short, timeI1long, timeI1 = timeI1short + timeI1long);

        // I2) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeI2 = 0., timeI2short = 0., timeI2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStringCopy[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
            // ASSERT(shortStringLength[i] == shortStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].length(),
                                      longStrings[ilong]);
        }
        timeI2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStringCopy[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
            // ASSERT(longStringLength[i] == longStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].length(),
                                         longStrings[i]);
            }
        }
        timeI2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I2\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI2short, timeI2long, timeI2 = timeI2short + timeI2long);
    // }

    printf("\t\tat end (without reallocation):\n");
    // {
        // I3) AGAIN (this time, without reallocation)
        double timeI3 = 0., timeI3short = 0., timeI3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].length(),
                                      longStrings[ilong]);
        }
        timeI3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].length(),
                                         longStrings[i]);
            }
        }
        timeI3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I3\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI3short, timeI3long, timeI3 = timeI3short + timeI3long);

        // I4) INSERT RANGE
        double timeI4 = 0., timeI4short = 0., timeI4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].end(),
                                      longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeI4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].insert(longStringCopy[i].end(),
                                         longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeI4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    I4\tInsert range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeI4short, timeI4long, timeI4 = timeI4short + timeI4long);
    // }

    printf("\tInsertions at beginning (with reallocations):\n");
    // {
        // B1) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeB1 = 0., timeB1short = 0., timeB1long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStrings[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
            // ASSERT(shortStringLength[i] == shortStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (size_t j = 0; j < longStringLength[ilong]; ++j) {
                shortStrings[i].insert(j, 1, longStringCopy[ilong][j]);
            }
#else
            shortStrings[i].insert((size_t)0, longStringCopy[ilong]);
#endif
        }
        timeB1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) {  // restore length but not capacity
            shortStrings[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStrings[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStrings[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
            // ASSERT(longStringLength[i] == longStrings[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
#if !COMPARE_WITH_NATIVE_SUNPRO_STL
            for (size_t j = 0; j < longStringLength[i]; ++j) {
                for (int k = 0; k < SL_RATIO; ++k, ++pos) {
                    longStrings[i].insert(pos, 1, longStringCopy[i][j]);
                }
            }
#else
                for (int k = 0; k < SL_RATIO; ++k, ++pos) {
                    longStrings[i].insert(pos, longStringCopy[i]);
                }
#endif
        }
        timeB1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStrings[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStrings[i].length());
        }

        printf("\t    B1\tInsert chars:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB1short, timeB1long, timeB1 = timeB1short + timeB1long);

        // B2) INSERT OPERATION (with reallocations, i.e., changes capacity)
        double timeB2 = 0., timeB2short = 0., timeB2long = 0.;

        for (int i = 0; i < NSHORT; ++i) {  // restore length *and* capacity
            Obj(shortWords[i]).swap(shortStringCopy[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
            // ASSERT(shortStringLength[i] == shortStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert((size_t)0, longStrings[ilong]);
        }
        timeB2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        for (int i = 0; i < NLONG; ++i) {  // restore length *and* capacity
            Obj(longWords[i]).swap(longStringCopy[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
            // ASSERT(longStringLength[i] == longStringCopy[i].capacity());
        }
        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos, longStrings[i]);
            }
        }
        timeB2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B2\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB2short, timeB2long, timeB2 = timeB2short + timeB2long);
    // }

    printf("\t\t(without reallocation):\n");
    // {
        // B3) AGAIN (this time, without reallocation)
        double timeB3 = 0., timeB3short = 0., timeB3long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert((size_t)0, longStrings[ilong]);
        }
        timeB3short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStringLength[i]);
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            size_t pos = 0;
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos, longStrings[i]);
            }
        }
        timeB3long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStringLength[i]);
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B3\tInsert string:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB3short, timeB3long, timeB3 = timeB3short + timeB3long);

        // B4) INSERT RANGE
        double timeB4 = 0., timeB4short = 0., timeB4long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].insert(shortStringCopy[i].begin(),
                                      longStrings[ilong].begin(),
                                      longStrings[ilong].end());
        }
        timeB4short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            typename Obj::iterator pos = longStringCopy[i].begin();
            for (int k = 0; k < SL_RATIO; ++k, pos += longStringLength[i]) {
                longStringCopy[i].insert(pos,
                                         longStrings[i].begin(),
                                         longStrings[i].end());
            }
        }
        timeB4long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    B4\tInsert range:"
               "\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeB4short, timeB4long, timeB4 = timeB4short + timeB4long);
    // }

    printf("\tReplacements (without reallocation):\n");
    // {
        // R1) REPLACE AT END
        double timeR1 = 0., timeR1short = 0., timeR1long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].replace(shortStringCopy[i].end() - 1,
                                       shortStringCopy[i].end(),
                                       longStrings[ilong]);
        }
        timeR1short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].replace(longStringCopy[i].end() - 1,
                                          longStringCopy[i].end(),
                                          longStrings[i]);
            }
        }
        timeR1long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    R1\tReplace at end:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeR1short, timeR1long, timeR1 = timeR1short + timeR1long);

        // R2) REPLACE AT BEGINNING
        double timeR2 = 0., timeR2short = 0., timeR2long = 0.;

        t.reset(); t.start();
        for (int i = 0, ilong = 0; i < NSHORT; ++i, ++ilong) {
            ilong = ilong < NLONG ? ilong : ilong - NLONG;
            shortStringCopy[i].replace(shortStringCopy[i].begin(),
                                       shortStringCopy[i].begin() + 1,
                                       longStrings[ilong]);
        }
        timeR2short = t.elapsedTime();
        for (int i = 0; i < NSHORT; ++i) { // restore length but not capacity
            shortStringCopy[i].erase(shortStrings[i].length());
            ASSERT(shortStringLength[i] == shortStringCopy[i].length());
        }

        t.reset(); t.start();
        for (int i = 0; i < NLONG; ++i) {
            for (int k = 0; k < SL_RATIO; ++k) {
                longStringCopy[i].replace(longStringCopy[i].begin(),
                                          longStringCopy[i].begin() + 1,
                                          longStrings[i]);
            }
        }
        timeR2long = t.elapsedTime();
        for (int i = 0; i < NLONG; ++i) { // restore length but not capacity
            longStringCopy[i].erase(longStrings[i].length());
            ASSERT(longStringLength[i] == longStringCopy[i].length());
        }

        printf("\t    R2\tReplace at begin: %1.5fs\t%1.5fs\t%1.6fs\n",
               timeR2short, timeR2long, timeR2 = timeR2short + timeR2long);
    // }

    printf("\tMisc.:\n");
    // {
        // M1) SWAP OPERATION
        double timeM1 = 0., timeM1short = 0., timeM1long = 0.;

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO; ++k) {
            for (int i = 0; i < NSHORT; ++i) {
                shortStrings[i].swap(shortStrings[NSHORT - 1 - i]);
            }
        }
        timeM1short = t.elapsedTime();

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO * SL_RATIO; ++k) {
            for (int i = 0; i < NLONG; ++i) {
                longStrings[i].swap(longStrings[NLONG - 1 - i]);
            }
        }
        timeM1long = t.elapsedTime();

        printf("\t    M1\tSwap reverse:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeM1short, timeM1long, timeM1 = timeM1short + timeM1long);

        // M2) SUBSTRING (plus assignment)
        double timeM2 = 0., timeM2short = 0., timeM2long = 0.;

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO; ++k) {
            for (int i = 0; i < NSHORT; ++i) {
                shortStringCopy[i] = shortStrings[i].substr(
                                                      0, shortStringLength[i]);
            }
        }
        timeM2short = t.elapsedTime();

        t.reset(); t.start();
        for (int k = 0; k < SL_RATIO * SL_RATIO; ++k) {
            for (int i = 0; i < NLONG; ++i) {
                longStringCopy[i] = longStrings[i].substr(0,
                                                          longStringLength[i]);
            }
        }
        timeM2long = t.elapsedTime();

        printf("\t    M2\tSubstring:\t%1.5fs\t%1.5fs\t%1.6fs\n",
               timeM2short, timeM2long, timeM2 = timeM2short + timeM2long);
    // }

    // F1) FIND AND RFIND OPERATIONS

    // F2) FIND_FIRST_OF AND FIND_LAST_OF OPERATIONS

    // F3) FIND_FIRST_NOT_OF AND FIND_LAST_NOT_OF OPERATIONS
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
    bslma::Allocator *originalGlobalAllocator =
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
      case 36: {
        // --------------------------------------------------------------------
        // TESTING 'data' MANIPULATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING 'data' MANIPULATOR" "\n"
                                 "==========================" "\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase36();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase36();

      } break;
      case 35: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase35();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase35();

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING 'operator ""_s' and 'operator ""_S'
        //
        // Concerns:
        //: 1 That the 'operator ""_s' and operator ""_S correctly forwards
        //:   arguments to the constructor of the 'basic_string' type.
        //:
        //: 2 That the length of the resultant string is determined by the
        //:   specified 'length' argument and does not depend on character
        //:   values of the input character array, for example null character.
        //:
        //: 3 That the 'operator ""_s' uses the default allocator to supply
        //:   memory.
        //:
        //: 4 That the 'operator ""_S' uses the global allocator to supply
        //:   memory.
        //:
        //: 5 That an access to 'operator ""_s' and 'operator ""_S' can be
        //:   gained using either 'bsl::literals', 'bsl::string_literals' or
        //:   'bsl::literals::string_literals' namespaces.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify that the argument values were passed correctly.
        //:
        //: 2 For a variety of strings of different sizes and different values
        //:   including embedded null character test that the resultant strings
        //:   have expected values and lengths.
        //:
        //: 3 Confirm that no memory being allocated except of the object's
        //:   allocator.
        //
        // Testing:
        //    string operator ""_s(const char*    str, std::size_t len);
        //   wstring operator ""_s(const wchar_t* str, std::size_t len);
        //    string operator ""_S(const char*    str, std::size_t len);
        //   wstring operator ""_S(const wchar_t* str, std::size_t len);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'operator\"\"_s' and 'operator\"\"_S'"
                   "\n=============================================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

        if (verbose) printf("Testing operator\"\"_s and "
                                   "'operator\"\"_S' with strings.\n");
        {
            typedef bsl::string Obj;

            { // C-1..3
                using namespace bsl::string_literals;

                AllocatorUseGuard guardG(globalAllocator_p);
                Tam               dam(defaultAllocator_p);

                struct {
                    int                    d_line;
                    Obj                    d_str;
                    const Obj::value_type *d_expected;
                    Obj::size_type         d_len;
                } DATA[] = {
                    //----------------------------------------------------
                    // LINE |     STR     |            EXPECTED |  LEN
                    //----------------------------------------------------
                    { L_,             ""_s,                   "",      0 },
                    { L_,           "\0"_s,                 "\0",      1 },
                    { L_,         "\0\0"_s,               "\0\0",      2 },
                    { L_,       "\0\0\0"_s,             "\0\0\0",      3 },
                    { L_,            "a"_s,                  "a",      1 },
                    { L_,           "ab"_s,                 "ab",      2 },
                    { L_,          "abc"_s,                "abc",      3 },
                    { L_,       "abcdef"_s,             "abcdef",      6 },
                    { L_,     "\0abcdef"_s,           "\0abcdef",      7 },
                    { L_,     "a\0bcdef"_s,           "a\0bcdef",      7 },
                    { L_,     "ab\0cdef"_s,           "ab\0cdef",      7 },
                    { L_,     "abc\0def"_s,           "abc\0def",      7 },
                    { L_,     "abcd\0ef"_s,           "abcd\0ef",      7 },
                    { L_,     "abcde\0f"_s,           "abcde\0f",      7 },
                    { L_,     "abcdef\0"_s,           "abcdef\0",      7 },
                    { L_,   "\0abcdef\0"_s,         "\0abcdef\0",      8 },
                    { L_,   "a\0bcde\0f"_s,         "a\0bcde\0f",      8 },
                    { L_,   "ab\0cd\0ef"_s,         "ab\0cd\0ef",      8 },
                    { L_,   "abc\0\0def"_s,         "abc\0\0def",      8 },
                    { L_,   "\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_s,
                            "\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_",   34 },
                };
                const int NUM_DATA =
                                  static_cast<int>(sizeof DATA / sizeof *DATA);

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int              LINE     = DATA[ti].d_line;
                    const Obj&             STR      = DATA[ti].d_str;
                    const Obj::size_type   LEN      = DATA[ti].d_len;
                    const Obj::value_type *EXPECTED = DATA[ti].d_expected;

                    LOOP_ASSERT(LINE,
                                0 == std::memcmp(STR.c_str(), EXPECTED, LEN));
                    LOOP_ASSERT(LINE, STR.length() == LEN);
                }

                ASSERT(dam.isTotalUp());
            }
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
            { // C-4
                const bsls::Types::Int64 GLOBAL_NUM_BYTES_IN_USE =
                                            globalAllocator_p->numBytesInUse();
                {
                    using namespace bsl::literals;
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    Tam               gam(globalAllocator_p);

                    const Obj mX = ""_S;
                    const Obj mY = "\0_1_3_4_5_6_7_8_9_"_S;
                    const Obj mZ = "\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_S;

                    ASSERT(gam.isInUseUp());;
                }
                ASSERT(GLOBAL_NUM_BYTES_IN_USE ==
                       globalAllocator_p->numBytesInUse());
            }
# endif
            { // C-5
                using namespace bsl::literals;
                Obj mX = "test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = "test"_S;
                (void) mY;
# endif
            }
            { // C-5
                using namespace bsl::string_literals;
                Obj mX = "test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = "test"_S;
                (void) mY;
# endif
            }
            { // C-5
                using namespace bsl::literals::string_literals;
                Obj mX = "test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = "test"_S;
                (void) mY;
# endif
            }

            if (veryVerbose) printf("\tnegative testing\n");
            { // C-6
                using namespace bsl::string_literals;
                bsls::AssertTestHandlerGuard hG;

                ASSERT_SAFE_PASS(operator ""_s("12345", 0));
                ASSERT_SAFE_PASS(operator ""_s("12345", 5));

                ASSERT_SAFE_PASS(operator ""_s(static_cast<char*>(0), 0));
                ASSERT_SAFE_FAIL(operator ""_s(static_cast<char*>(0), 5));

# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                ASSERT_SAFE_PASS(operator ""_S("12345", 0));
                ASSERT_SAFE_PASS(operator ""_S("12345", 5));

                ASSERT_SAFE_PASS(operator ""_S(static_cast<char*>(0), 0));
                ASSERT_SAFE_FAIL(operator ""_S(static_cast<char*>(0), 5));
# endif
            }
        }

        if (verbose) printf("Testing operator\"\"_s and "
                                   "'operator\"\"_S' with wstrings.\n");
        {
            typedef bsl::wstring Obj;

            { // C-1..3
                using namespace bsl::string_literals;

                AllocatorUseGuard guardG(globalAllocator_p);
                Tam               dam(defaultAllocator_p);

                struct {
                    int                    d_line;
                    Obj                    d_str;
                    const Obj::value_type *d_expected;
                    Obj::size_type         d_len;
                } DATA[] = {
                    //-------------------------------------------------------
                    // LINE |      STR     |            EXPECTED  |    LEN
                    //-------------------------------------------------------
                    { L_,             L""_s,                   L"",      0 },
                    { L_,           L"\0"_s,                 L"\0",      1 },
                    { L_,         L"\0\0"_s,               L"\0\0",      2 },
                    { L_,       L"\0\0\0"_s,             L"\0\0\0",      3 },
                    { L_,            L"a"_s,                  L"a",      1 },
                    { L_,           L"ab"_s,                 L"ab",      2 },
                    { L_,          L"abc"_s,                L"abc",      3 },
                    { L_,       L"abcdef"_s,             L"abcdef",      6 },
                    { L_,     L"\0abcdef"_s,           L"\0abcdef",      7 },
                    { L_,     L"a\0bcdef"_s,           L"a\0bcdef",      7 },
                    { L_,     L"ab\0cdef"_s,           L"ab\0cdef",      7 },
                    { L_,     L"abc\0def"_s,           L"abc\0def",      7 },
                    { L_,     L"abcd\0ef"_s,           L"abcd\0ef",      7 },
                    { L_,     L"abcde\0f"_s,           L"abcde\0f",      7 },
                    { L_,     L"abcdef\0"_s,           L"abcdef\0",      7 },
                    { L_,   L"\0abcdef\0"_s,         L"\0abcdef\0",      8 },
                    { L_,   L"a\0bcde\0f"_s,         L"a\0bcde\0f",      8 },
                    { L_,   L"ab\0cd\0ef"_s,         L"ab\0cd\0ef",      8 },
                    { L_,   L"abc\0\0def"_s,         L"abc\0\0def",      8 },
                    { L_,   L"\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_s,
                            L"\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_",    34 },
                };
                const int NUM_DATA =
                                  static_cast<int>(sizeof DATA / sizeof *DATA);

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int              LINE     = DATA[ti].d_line;
                    const Obj&             STR      = DATA[ti].d_str;
                    const Obj::size_type   LEN      = DATA[ti].d_len;
                    const Obj::value_type *EXPECTED = DATA[ti].d_expected;

                    LOOP_ASSERT(LINE,
                                0 == std::wmemcmp(STR.c_str(), EXPECTED, LEN));
                    LOOP_ASSERT(LINE, STR.length() == LEN);
                }

                ASSERT(dam.isTotalUp());
            }
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
            { // C-4
                const bsls::Types::Int64 GLOBAL_NUM_BYTES_IN_USE =
                                            globalAllocator_p->numBytesInUse();
                {
                    using namespace bsl::literals;
                    AllocatorUseGuard guardD(defaultAllocator_p);
                    Tam               gam(globalAllocator_p);

                    const Obj mX = L""_S;
                    const Obj mY = L"\0_1_3_4_5_6_7_8_9_"_S;
                    const Obj mZ = L"\0_1_3_4_5_6_7_8_9_\0_1_2_3_4_5_6_7_"_S;

                    ASSERT(gam.isInUseUp());;
                }
                ASSERT(GLOBAL_NUM_BYTES_IN_USE ==
                       globalAllocator_p->numBytesInUse());
            }
# endif
            { // C-5
                using namespace bsl::literals;
                Obj mX = L"test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = L"test"_S;
                (void) mY;
# endif
            }
            { // C-5
                using namespace bsl::string_literals;
                Obj mX = L"test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = L"test"_S;
                (void) mY;
# endif
            }
            { // C-5
                using namespace bsl::literals::string_literals;
                Obj mX = L"test"_s;
                (void) mX;
# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                const Obj mY = L"test"_S;
                (void) mY;
# endif
            }

            if (veryVerbose) printf("\tnegative testing\n");
            { // C-6
                using namespace bsl::string_literals;
                bsls::AssertTestHandlerGuard hG;

                ASSERT_SAFE_PASS(operator ""_s(L"12345", 0));
                ASSERT_SAFE_PASS(operator ""_s(L"12345", 5));

                ASSERT_SAFE_PASS(operator ""_s(static_cast<char*>(0), 0));
                ASSERT_SAFE_FAIL(operator ""_s(static_cast<char*>(0), 5));

# if !defined(BSLS_STRING_DISABLE_S_LITERALS)
                ASSERT_SAFE_PASS(operator ""_S(L"12345", 0));
                ASSERT_SAFE_PASS(operator ""_S(L"12345", 5));

                ASSERT_SAFE_PASS(operator ""_S(static_cast<char*>(0), 0));
                ASSERT_SAFE_FAIL(operator ""_S(static_cast<char*>(0), 5));
# endif
            }
        }
#else
        if (veryVerbose) { printf("Cannot test 'operator ""_s' "
                                  "in pre-C++11 mode or if the compiler "
                                  "does not support inline namespaces.\n"); }
#endif
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING 'initializer_list' FUNCTIONS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase33' for details.
        //
        // Testing:
        //   basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
        //   basic_string& operator=(initializer_list<CHAR_TYPE> values);
        //   basic_string& assign(initializer_list<CHAR_TYPE> values);
        //   basic_string& append(initializer_list<CHAR_TYPE> values);
        //   iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'initializer_list' FUNCTIONS"
                            "\n====================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        TestDriver<char   >::testCase33();
        TestDriver<wchar_t>::testCase33();
#else
        if (verbose) printf("\nSkip %d: Requires C++11 Initializers\n", test);
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING 'to_string' AND 'to_wstring'
        //
        // Concerns:
        //: 1  The 'to_string' function has sufficient overloads to create a
        //:    'string' for any arithmetic type.
        //: 2  The 'to_wstring' function has sufficient overloads to create a
        //:    'wstring' for any arithmetic type.
        //: 3  The numeric -> string conversion functions return an
        //:    appropriately formatted (short) string containing the correct
        //:    value.
        //
        // Plan:
        //: 1 use 'sprintf' and 'swprintf' with an arbitrarily large buffer,
        //:   (in this test case the buffer size will be 384) and compare it to
        //:   the output of 'to_string' and 'to_wstring'.  The buffer must be
        //:   at least as large as the largest floating point value that might
        //:   print, which is roughly 308 decimal digits.
        //:
        //: 2 Using table-driven testing, test a suitable range of numbers that
        //:   are, each in turn, cast to every integer type that can hold each
        //:   value in order to check every overload is called and handled
        //:   correctly.
        //
        // Testing:
        //   string to_string(int value);
        //   string to_string(long value);
        //   string to_string(long long value);
        //   string to_string(unsigned value);
        //   string to_string(unsigned long value);
        //   string to_string(unsigned long long value);
        //   string to_string(float value);
        //   string to_string(double value);
        //   string to_string(long double value);
        //   wstring to_wstring(int value);
        //   wstring to_wstring(long value);
        //   wstring to_wstring(long long value);
        //   wstring to_wstring(unsigned value);
        //   wstring to_wstring(unsigned long value);
        //   wstring to_wstring(unsigned long long value);
        //   wstring to_wstring(float value);
        //   wstring to_wstring(double value);
        //   wstring to_wstring(long double value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'to_string' AND 'to_wstring'"
                            "\n====================================\n");

        static const size_t BUF_LEN = 384;
        char    tempBuf[BUF_LEN];  // 'char' buffer for largest number
        wchar_t wTempBuf[BUF_LEN]; // 'wchar_t' buffer for largest number

        static const struct {
            int         d_lineNum;
            long long   d_value;
            bool        d_isShortChar;
            bool        d_isShortWchar;
        } DATA[] = {
          //                             string is short:
          // LINE                VALUE   'char' 'wchar_t'
            { L_,                    0,   true,  true                },
            { L_,                    1,   true,  true                },
            { L_,                   -1,   true,  true                },
            { L_,                10101,   true,  sizeof(wchar_t) < 4 },
            { L_,               -10101,   true,  sizeof(wchar_t) < 4 },
            { L_,                32767,   true,  sizeof(wchar_t) < 4 },
            { L_,               -32767,   true,  sizeof(wchar_t) < 4 },
            { L_,             11001100,   true,  sizeof(wchar_t) < 4 },
            { L_,            -11001100,   true,  sizeof(wchar_t) < 4 },
            { L_,           2147483647,   true,  false               },
            { L_,          -2147483647,   true,  false               },
            { L_,  9223372036854775807LL, true,  false               },
            { L_, -9223372036854775807LL, false, false               }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct {
            int         d_lineNum;
            double      d_value;
        } DOUBLE_DATA[] = {
            //   value
            {L_, 1.0},
            {L_, 1.01},
            {L_, 1.001},
            {L_, 1.0101},
            {L_, 1.01001},
            {L_, 1.010101},
            {L_, 1.01010101},
            {L_, 1.0101019},
            {L_, 3.1415926},
            {L_, 5.156},
            {L_, 24.0},
            {L_, 24.1111111111111111111},
            {L_, 12345.12345678},
            {L_, 123456789.123456789},
            {L_, 123456789012345.123456},
            {L_, 1234567890123456789.123456789},
            {L_, std::numeric_limits<float>::max()},
            {L_, std::numeric_limits<float>::min()},
            {L_, 1.79769e+308},
            {L_,-1.79769e+308},
        };
        enum { NUM_DOUBLE_DATA = sizeof DOUBLE_DATA / sizeof *DOUBLE_DATA };

        if (verbose) printf("\nTesting 'to_string' with integrals.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE           = DATA[ti].d_lineNum;
            const long long          VALUE          = DATA[ti].d_value;
//          const bool               IS_SHORT_CHAR  = DATA[ti].d_isShortChar;
//          const bool               IS_SHORT_WCHAR = DATA[ti].d_isShortWchar;
            const unsigned long long U_VALUE        =
                                        static_cast<unsigned long long>(VALUE);

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'string'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            // Set up the oracle results for signed integers, using 'long long'
            // versions of the corresponding C library 'sprintf' functions.

            sprintf(tempBuf, "%lld", VALUE);

            if (VALUE <= std::numeric_limits<int>::max() &&
                VALUE >= std::numeric_limits<int>::min()) {
                const int TEST_VALUE = static_cast<int>(VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            if (VALUE <= std::numeric_limits<long>::max() &&
                VALUE >= std::numeric_limits<long>::min()) {
                const long TEST_VALUE = static_cast<long>(VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            {
                bsl::string str = bsl::to_string(VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 MM = defaultAllocator.numBlocksTotal();
            const Int64  M = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tMiddle: ");P_(MM);P(M);
            }

            // Set up the oracle results for unsigned integers, using the
            // 'unsigned long long' versions of the corresponding C library
            // 'sprintf' functions.

            sprintf(tempBuf, "%llu", U_VALUE);

            if (U_VALUE <= std::numeric_limits<unsigned int>::max()) {
                const unsigned int TEST_VALUE =
                                            static_cast<unsigned int>(U_VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str);
            }

            if (U_VALUE <= std::numeric_limits<unsigned long>::max()) {
                const unsigned long TEST_VALUE =
                                           static_cast<unsigned long>(U_VALUE);
                bsl::string str = bsl::to_string(TEST_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            {
                bsl::string str = bsl::to_string(U_VALUE);
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting 'to_wstring' with integrals.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const long long          VALUE   = DATA[ti].d_value;
            const unsigned long long U_VALUE =
                                        static_cast<unsigned long long>(VALUE);

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'wstring'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            // Set up the oracle results for signed integers, using 'long long'
            // versions of the corresponding C library 'sprintf' functions.

            swprintf(wTempBuf, BUF_LEN, L"%lld", VALUE);

            if (VALUE <= std::numeric_limits<int>::max() &&
                VALUE >= std::numeric_limits<int>::min()) {
                const int TEST_VALUE = static_cast<int>(VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr);
            }

            if (VALUE <= std::numeric_limits<long>::max() &&
                VALUE >= std::numeric_limits<long>::min()) {
                const long TEST_VALUE = static_cast<long>(VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            {
                bsl::wstring wstr = bsl::to_wstring(VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            // Set up the oracle results for unsigned integers, using the
            // 'unsigned long long' versions of the corresponding C library
            // 'sprintf' functions.

            sprintf(tempBuf, "%llu", U_VALUE);
            swprintf(wTempBuf, BUF_LEN, L"%llu", U_VALUE);

            if (U_VALUE <= std::numeric_limits<unsigned int>::max()) {
                const unsigned int TEST_VALUE =
                                            static_cast<unsigned int>(U_VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr);
            }

            if (U_VALUE <= std::numeric_limits<unsigned long>::max()) {
                const unsigned long TEST_VALUE =
                                           static_cast<unsigned long>(U_VALUE);
                bsl::wstring wstr = bsl::to_wstring(TEST_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            {
                bsl::wstring wstr = bsl::to_wstring(U_VALUE);
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting extreme integer values.\n");
        {
            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose)
            {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }

        if (verbose) printf("\nTesting 'to_string' with floating points.\n");

        for (int ti = 0; ti < NUM_DOUBLE_DATA; ++ti){
            const int                LINE  = DOUBLE_DATA[ti].d_lineNum;
            const double             VALUE = DOUBLE_DATA[ti].d_value;

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'string'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                sprintf(tempBuf, "%f", static_cast<float>(VALUE));
                bsl::string str = bsl::to_string(static_cast<float>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            if (VALUE <= std::numeric_limits<double>::max()){
                sprintf(tempBuf, "%f", static_cast<double>(VALUE));
                bsl::string str = bsl::to_string(static_cast<double>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                sprintf(tempBuf, "%Lf", static_cast<long double>(VALUE));
                bsl::string str =
                               bsl::to_string(static_cast<long double>(VALUE));
                ASSERTV(LINE, tempBuf, str, tempBuf == str );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());

        if (verbose) printf("\nTesting 'to_wstring' with floating points.\n");

        for (int ti = 0; ti < NUM_DOUBLE_DATA; ++ti){
            const int    LINE  = DOUBLE_DATA[ti].d_lineNum;
            const double VALUE = DOUBLE_DATA[ti].d_value;

            if (veryVerbose){
                printf("\tConverting ");P_(VALUE);
                printf("to a 'wstring'.\n");
            }

            const Int64 BB = defaultAllocator.numBlocksTotal();
            const Int64  B = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tBefore: ");P_(BB);P(B);
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%f",
                         static_cast<float>(VALUE));
                bsl::wstring wstr = bsl::to_wstring(static_cast<float>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            if (VALUE <= std::numeric_limits<double>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%f",
                         static_cast<double>(VALUE));
                bsl::wstring wstr =
                                   bsl::to_wstring(static_cast<double>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            if (VALUE <= std::numeric_limits<float>::max()){
                swprintf(wTempBuf,
                         sizeof wTempBuf / sizeof *wTempBuf,
                         L"%Lf",
                         static_cast<long double>(VALUE));
                bsl::wstring wstr =
                              bsl::to_wstring(static_cast<long double>(VALUE));
                ASSERTV(LINE, wTempBuf, wstr, wTempBuf == wstr );
            }

            const Int64 AA = defaultAllocator.numBlocksTotal();
            const Int64  A = defaultAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\tAfter: ");P_(AA);P(A);
            }
        }
        ASSERT(0 == defaultAllocator.numMismatches());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING STRING TO FLOATING POINT NUMBER CONVERSIONS
        //
        // Concerns:
        //: 1 'stof', 'stod', and 'stold' parse the string correctly into a
        //:   floating point number of the requested size.
        //:
        //: 2 The methods discard leading white space characters and create a
        //:   valid floating point number by consuming the longest sequence of
        //:   characters that can represent a floating point number..
        //:
        //: 3 Detects the correct base with leading 0X or 0x.
        //:
        //: 4 The methods detect exponents correctly.
        //:
        //: 5 The methods correctly identify INF/INFINITY as appropriate.
        //:
        //: 6 Malformed strings report errors correctly.
        //:
        //: 7 The functions are overloaded to convert from 'bsl::string' and
        //:   'bsl::wstring' objects.
        //
        // Plan:
        //: 1 Use 'stof', 'stod', and 'stold' on a variety of valid values to
        //:   ensure that the methods parse correctly (C-1)
        //:
        //: 2 Try to convert partially valid strings, ie strings that contain
        //:   characters that are not valid in the base of the number.
        //:
        //: 3 Test a variety of numbers in base 0 to check if they detect the
        //:   correct base
        //
        // Testing:
        //   float stof(const string& str, std::size_t *pos =0);
        //   float stof(const wstring& str, std::size_t *pos =0);
        //   double stod(const string& str, std::size_t *pos =0);
        //   double stod(const wstring& str, std::size_t *pos =0);
        //   long double stold(const string& str, std::size_t *pos =0);
        //   long double stold(const wstring& str, std::size_t *pos =0);
        // --------------------------------------------------------------------

        if (verbose) printf(
                  "\nTESTING STRING TO FLOATING POINT NUMBER CONVERSIONS"
                  "\n===================================================\n");

        enum { k_DEFAULT_POSITION = 54321 };

        if (verbose) printf("Testing stof, stod, and stold with strings.\n");
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_input;            // input
                size_t      d_pos;              // position of character after
                                                // the numeric value
                double      d_spec;             // specifications
            } DATA[] = {
                //line  input               pos  spec
                //----  -----               ---  ----
                { L_,   "0",                1,    0     },
                { L_,   "-0",               2,    0     },
                { L_,   "+.5",              3,    0.5   },
                { L_,   "3.145gg",          5,    3.145 },
                { L_,   "    -5.9991",      11,  -5.9991},
                { L_,   "10e",              2,    10    },
                { L_,   "10e1",             4,    1e2   },
                { L_,   "10p2",             2,    10    },
#if !(defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR <=1800))
                { L_,   "0xf.f",            5,    15.937500},
#endif
#if __cplusplus >= 201103L
                { L_,   "inF",              3,    std::numeric_limits
                                                        <double>::infinity() },
                { L_,   "-inFinITY",        9,   -std::numeric_limits
                                                        <double>::infinity() },
                { L_,   "+InF",             4,    std::numeric_limits
                                                        <double>::infinity() },
#endif
                // Awkward - fails only for 'float'
                { L_,   "   9e99",          7,    9e99 },

                // Range errors
                { L_,   "9e9999",           0,   -1 },
                { L_,   "-9e9999",          0,   -1 },
                { L_,   "+9e9999",          0,   -1 },

                // Invalid buffers
                { L_,   "",                 0,   -2 },
                { L_,   ".",                0,   -2 },
                { L_,   "-",                0,   -2 },
                { L_,   "e",                0,   -2 },
                { L_,   ".e",               0,   -2 },
                { L_,   "+e",               0,   -2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE   = DATA[ti].d_lineNum;
                const char   *INPUT  = DATA[ti].d_input;
                const size_t  POS    = DATA[ti].d_pos;
                double        SPEC   = DATA[ti].d_spec;

                if (veryVeryVerbose) {
                    printf("\tPOS: " ZU ", INPUT: \"%s\"\n", POS, INPUT);
                }

                const bsl::string      inV(INPUT);
                bsl::string::size_type position = k_DEFAULT_POSITION;

                if (POS && POS != 7)  // 7 uniquely denotes the awkward row
                {
                    float value = bsl::stof(inV, NULL);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        float value = bsl::stof(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(SPEC, POS, -1 == SPEC || 7 == POS);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (veryVeryVerbose) {
                    printf("\tPOS: " ZU ", INPUT: \"%s\"\n", POS, INPUT);
                }

                if (0 != POS)
                {
#if defined(BSLS_STRING_OPT_HOISTS_THROWING_FUNCTIONS)
                    ASSERTV(POS, POS);
#endif

                    double value = bsl::stod(inV, NULL);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(LINE, POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(LINE, POS, position,
                            POS == position);

                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    if (veryVeryVerbose) {
                        printf("\t\tNO POS: " ZU ", INPUT: \"%s\"\n", POS, INPUT);
                    }

                    try {
                        double value = bsl::stod(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -1 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (0 != POS)
                {
#if defined(BSLS_STRING_OPT_HOISTS_THROWING_FUNCTIONS)
                    ASSERTV(POS, POS);
#endif

                    long double value = bsl::stold(inV, NULL);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(LINE, POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(LINE, POS, position,
                            POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    if (veryVeryVerbose) {
                        printf("\t\tNO POS (2): " ZU ", INPUT: \"%s\"\n", POS, INPUT);
                    }

                    try {
                        long double value = bsl::stold(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -1 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, SPEC, -2 == SPEC);
                        ASSERTV(LINE, position,
                                k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
            }
        }

        if (verbose) printf("Testing stof, stod and stold with wstrings.\n");
        {
            static const struct {
                int            d_lineNum;          // source line number
                const wchar_t *d_input;            // input
                size_t         d_pos;              // position of character
                                                   // after the numeric value
                long double    d_spec;             // specifications
            } DATA[] = {
                //line  input               pos  spec
                //----  -----               ---  ----
                { L_,   L"0",               1,    0     },
                { L_,   L"-0",              2,    0     },
                { L_,   L"+.5",             3,    0.5   },
                { L_,   L"3.125gg",         5,    3.125 },
                { L_,   L"    -4.6875",     11,  -4.6875},
                { L_,   L"10e",             2,    10    },
                { L_,   L"10e1",            4,    1e2   },
                { L_,   L"10p2",            2,    10    },
#if !(defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR <=1800))
                { L_,   L"0xf.f",           5,    15.937500},
#endif
#if __cplusplus >= 201103L
                { L_,   L"inF",             3,    std::numeric_limits
                                                        <double>::infinity() },
                { L_,   L"-inFinITY",       9,   -std::numeric_limits
                                                        <double>::infinity() },
                { L_,   L"+InF",            4,    std::numeric_limits
                                                        <double>::infinity() },
#endif
                // Awkward - fails only for 'float'
                { L_,   L" 8.5e99",         7,    8.5e99 },

                // Range errors
                { L_,   L"9e9999",          0,   -1 },
                { L_,   L"-9e9999",         0,   -1 },
                { L_,   L"+9e9999",         0,   -1 },

                // Invalid buffers
                { L_,   L"",                0,   -2 },
                { L_,   L".",               0,   -2 },
                { L_,   L"-",               0,   -2 },
                { L_,   L"e",               0,   -2 },
                { L_,   L".e",              0,   -2 },
                { L_,   L"+e",              0,   -2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_lineNum;
                const wchar_t *INPUT  = DATA[ti].d_input;
                const size_t   POS    = DATA[ti].d_pos;
                long double    SPEC   = DATA[ti].d_spec;

                if (veryVeryVerbose) {
                    printf("\tPOS: " ZU ", SPEC: \"%Lf\"\n", POS, SPEC);
                }

                const bsl::wstring      inV(INPUT);
                bsl::wstring::size_type position = k_DEFAULT_POSITION;

                if (POS && POS != 7)  // 7 uniquely denotes the awkward row
                {
                    float value = bsl::stof(inV, NULL);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stof(inV, &position);
                    ASSERTV(LINE, value, (float)SPEC, value == (float)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        float value = bsl::stof(inV, NULL);
                        ASSERTV(LINE, inV, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, POS, -1 == SPEC || 7 == POS);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (0 != POS)
                {
#if defined(BSLS_STRING_OPT_HOISTS_THROWING_FUNCTIONS)
                    ASSERTV(POS, POS);
#endif

                    double value = bsl::stod(inV, NULL);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stod(inV, &position);
                    ASSERTV(LINE, value, (double)SPEC, value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        double value = bsl::stod(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -1 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(LINE, inV, SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
                position = k_DEFAULT_POSITION;

                if (0 != POS)
                {
#if defined(BSLS_STRING_OPT_HOISTS_THROWING_FUNCTIONS)
                    ASSERTV(POS, POS);
#endif

                    long double value = bsl::stold(inV, NULL);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);

                    position = k_DEFAULT_POSITION;

                    value = bsl::stold(inV, &position);
                    ASSERTV(LINE, (double)value,   (double)SPEC,
                                  (double)value == (double)SPEC);
                    ASSERTV(POS, position, POS == position);
                }
                else {
#if defined(BDE_BUILD_TARGET_EXC)
                    try {
                        long double value = bsl::stold(inV, NULL);
                        ASSERTV(LINE, INPUT, value, SPEC, !"Did not throw");
                    }
                    catch(const std::out_of_range&) {
                        // Expected code path
                        ASSERTV(SPEC, -1 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::invalid_argument&) {
                        // Expected code path
                        ASSERTV(SPEC, -2 == SPEC);
                        ASSERTV(position, k_DEFAULT_POSITION == position);
                    }
                    catch(const std::exception&) {
                        ASSERTV(LINE, INPUT, SPEC, !"Unknown 'std' exception");
                    }
                    catch(...) {
                        ASSERTV(LINE, INPUT, SPEC, !"Non-standard exception");
                    }
#endif
                }
            }
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING STRING TO INTEGER CONVERSIONS
        //
        // Concerns:
        //: 1 'stoi', 'stol', and 'stoll' parse the string properly into the
        //:   correct signed integer value.
        //:
        //: 2 'stoul', and 'stoull' parse the string properly into the
        //:   correct unsigned integer value, even when the input is a string
        //:   denoting a negative number.
        //:
        //: 3 The methods discard leading white space characters and create
        //:   largest valid integral number.
        //:
        //: 4 The correct base if deduced if '0 == base'.
        //:
        //: 5 The 'stoX' functions handle null pointers to 'pos' correctly.
        //
        // Plan:
        //: 1 Use 'stoi', 'stol', 'stoll', 'stoul', and 'stoull' on a variety
        //:   of valid values to ensure that the methods parse correctly (C-1)
        //:
        //: 2 Try to convert partially valid strings, ie strings that contain
        //:   characters that are not valid in the base of the number.
        //:
        //: 3 Test a variety of numbers in base 0 to check if they detect the
        //:   correct base.
        //
        // Testing:
        //   int stoi(const string& str, std::size_t *pos = 0, int base = 10);
        //   int stoi(const wstring& str, std::size_t *pos = 0, int base = 10);
        //   long stol(const string& str, std::size_t *pos, int base);
        //   long stol(const wstring& str, std::size_t *pos, int base);
        //   unsigned long stoul(const string& s, std::size_t *pos, int base);
        //   unsigned long stoul(const wstring& s, std::size_t *pos, int base);
        //   long long stoll(const string& str, std::size_t *pos, int base);
        //   long long stoll(const wstring& str, std::size_t *pos, int base);
        //   unsigned long long stoull(const string&, std::size_t *, int);
        //   unsigned long long stoull(const wstring&, std::size_t *, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STRING TO INTEGER CONVERSIONS"
                            "\n=====================================\n");

        enum { k_DEFAULT_POSITION = 54321 };

        enum IntType {
            e_INT,
            e_LONG_INT,
            e_UNSIGNED_LONG_INT,
            e_LONG_LONG_INT,
            e_UNSIGNEDLONG_LONG_INT,
            e_NOT_INT
        };

        enum ErrType {
            e_PASS,
            e_INVALID,
            e_RANGE
        };

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_input;            // input
            int         d_base;             // base of input
            size_t      d_pos;              // position of character after the
                                            // numeric value
            long long   d_spec;             // specifications
        } DATA[] = {
            //line input                   base  pos  spec
            //---- -----                   ----  ---  ----
            { L_,  "0",                    10,   1,   0    },
            { L_,  "0w",                   10,   1,   0    },
            { L_,  "0x",                   10,   1,   0    },
            { L_,  "-0",                   10,   2,   0    },
            { L_,  "10101",                10,   5,   10101},
            { L_,  "-10101",               10,   6,  -10101},
            { L_,  "32767",                10,   5,   32767},
            { L_,  "-32767",               10,   6,  -32767},
            { L_,  "000032767",            10,   9,   32767},
            { L_,  "2147483647",           10,   10,  2147483647LL},
            { L_,  "2147483648",           10,   10,  2147483648LL},
            { L_,  "-2147483647",          10,   11, -2147483647LL},
            { L_,  "-2147483648",          10,   11, -2147483648LL},
            { L_,  " -2147483649",         10,   12, -2147483649LL},
            { L_,  "4294967295",           10,   10,  4294967295LL},
            { L_,  "9223372036854775807",  10,   19,  9223372036854775807LL  },
            { L_,  "-9223372036854775807", 10,   20, -9223372036854775807LL  },
            { L_,  "-9223372036854775808", 10,   20, -9223372036854775807LL-1},
            { L_,  "-9223372036854775809", 10,   20,  9223372036854775807LL  },
            { L_,  "19223372036854775808", 10,   0,   0},

            //test usage of spaces, and non valid characters with in the string
            { L_,  "  515",                10,   5,   515},
            { L_,  "  515  505050",        10,   5,   515},
            { L_,  " 99abc99",             10,   3,   99 },
            { L_,  " 3.14159",             10,   2,   3  },
            { L_,  "0x555",                10,   1,   0  },

            //test different bases
            { L_,  "111",                  2,    3,   7      },
            { L_,  "101",                  2,    3,   5      },
            { L_,  "100",                  2,    3,   4      },
            { L_,  "101010101010 ",        2,    12,  2730   },
            { L_,  "1010101010102 ",       2,    12,  2730   },
            { L_,  "111111111111111",      2,    15,  32767  },
            { L_,  "-111111111111111",     2,    16, -32767  },
            { L_,  "77777",                8,    5,   32767  },
            { L_,  "-77777",               8,    6,  -32767  },
            { L_,  "7FFF",                 16,   4,   32767  },
            { L_,  "0x7FfF",               16,   6,   32767  },
            { L_,  "-00000x7FFf",          16,   6,  -0      },
            { L_,  "ZZZZ",                 36,   4,   1679615},

            // base zero
            { L_,  "79FFZZZf",             0,    2,   79   },
            { L_,  "0xFfAb",               0,    6,   65451},
            { L_,  "05471",                0,    5,   2873 },
            { L_,  "0X5471",               0,    6,   21617},
            { L_,  "5471",                 0,    4,   5471 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("Testing stoi, stol, stoll, stoul and stoull with"
                            " 'char' strings.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_lineNum;
            const char        *INPUT  = DATA[ti].d_input;
            const int          BASE   = DATA[ti].d_base;
            const size_t       POS    = DATA[ti].d_pos;
            const long long    SPEC   = DATA[ti].d_spec;

            if (veryVerbose) {
                P_(INPUT) P_(BASE) P_(POS) P(SPEC);
            }

            const bsl::string      STRING_VALUE(INPUT);
            bsl::string::size_type position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<int>::max()
             && SPEC >= std::numeric_limits<int>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<long>::max()
             && SPEC >= std::numeric_limits<long>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                long value = bsl::stol(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long value = bsl::stol(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (((sizeof(long) == sizeof(long long)
                 || SPEC <= std::numeric_limits<unsigned long>::max())
                 && SPEC >= std::numeric_limits<long>::min()
                 && POS)
             || POS == 12) {
                const unsigned long EXPECT = static_cast<unsigned long>(SPEC);

                unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (POS && (POS < 20 || SPEC < 0)) {
                long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;

            if (POS) {
                const unsigned long long EXPECT = SPEC;

                unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS,
                        position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;
        }

        static const struct {
            int            d_lineNum;      // source line number
            const wchar_t *d_input;        // input
            int            d_base;         // base of input
            size_t         d_pos;          // position of character after the
                                           // numeric value
            long long      d_spec;         // specifications
        } WDATA[] = {
            //line input                   base  pos  spec
            //---- -----                   ----  ---  ----
            { L_,  L"0",                    10,  1,   0    },
            { L_,  L"0w",                   10,  1,   0    },
            { L_,  L"0x",                   10,  1,   0    },
            { L_,  L"-0",                   10,  2,   0    },
            { L_,  L"10101",                10,  5,   10101},
            { L_,  L"-10101",               10,  6,  -10101},
            { L_,  L"32767",                10,  5,   32767},
            { L_,  L"-32767",               10,  6,  -32767},
            { L_,  L"000032767",            10,  9,   32767},
            { L_,  L"2147483647",           10,  10,  2147483647LL},
            { L_,  L"2147483648",           10,  10,  2147483648LL},
            { L_,  L"-2147483647",          10,  11, -2147483647LL},
            { L_,  L"-2147483648",          10,  11, -2147483648LL},
            { L_,  L" -2147483649",         10,  12, -2147483649LL},
            { L_,  L"4294967295",           10,  10,  4294967295LL},
            { L_,  L"9223372036854775807",  10,  19,  9223372036854775807LL  },
            { L_,  L"-9223372036854775807", 10,  20, -9223372036854775807LL  },
            { L_,  L"-9223372036854775808", 10,  20, -9223372036854775807LL-1},
            { L_,  L"-9223372036854775809", 10,  20,  9223372036854775807LL  },
            { L_,  L"19223372036854775808", 10,  0,   0},

            //test usage of spaces, and non valid characters with in the string
            { L_,  L"  515",                10,  5,   515},
            { L_,  L"  515  505050",        10,  5,   515},
            { L_,  L" 99abc99",             10,  3,   99 },
            { L_,  L" 3.14159",             10,  2,   3  },
            { L_,  L"0x555",                10,  1,   0  },

            //test different bases
            { L_,  L"111",                  2,   3,   7      },
            { L_,  L"101",                  2,   3,   5      },
            { L_,  L"100",                  2,   3,   4      },
            { L_,  L"101010101010 ",        2,   12,  2730   },
            { L_,  L"1010101010102 ",       2,   12,  2730   },
            { L_,  L"111111111111111",      2,   15,  32767  },
            { L_,  L"-111111111111111",     2,   16, -32767  },
            { L_,  L"77777",                8,   5,   32767  },
            { L_,  L"-77777",               8,   6,  -32767  },
            { L_,  L"7FFF",                 16,  4,   32767  },
            { L_,  L"0x7FfF",               16,  6,   32767  },
            { L_,  L"-00000x7FFf",          16,  6,  -0      },
            { L_,  L"ZZZZ",                 36,  4,   1679615},

            // base zero
            { L_,  L"79FFZZZf",             0,   2,   79   },
            { L_,  L"0xFfAb",               0,   6,   65451},
            { L_,  L"05471",                0,   5,   2873 },
            { L_,  L"0X5471",               0,   6,   21617},
            { L_,  L"5471",                 0,   4,   5471 },
        };
        const int NUM_WDATA = sizeof WDATA / sizeof *WDATA;

        if (verbose) printf("Testing stoi, stol, stoll, stoul and stoull with"
                            " 'wstring'.\n");

        for (int ti = 0; ti < NUM_WDATA; ++ti) {
            const int           LINE   = WDATA[ti].d_lineNum;
            const wchar_t      *INPUT  = WDATA[ti].d_input;
            const int           BASE   = WDATA[ti].d_base;
            const size_t        POS    = WDATA[ti].d_pos;
            const long long     SPEC   = WDATA[ti].d_spec;

            if (veryVerbose) {
                P_(INPUT) P_(BASE) P_(POS) P(SPEC);
            }

            const bsl::wstring      STRING_VALUE(INPUT);
            bsl::wstring::size_type position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<int>::max()
             && SPEC >= std::numeric_limits<int>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoi(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (SPEC <= std::numeric_limits<long>::max()
             && SPEC >= std::numeric_limits<long>::min()
             && POS
             && (POS < 20 || SPEC < 0)) {
                long value = bsl::stol(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stol(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long value = bsl::stol(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (((sizeof(long) == sizeof(long long) ||
                   SPEC <= std::numeric_limits<unsigned long>::max())
                && SPEC >= std::numeric_limits<long>::min()
                && POS)
             || POS == 12) {
                const unsigned long EXPECT = static_cast<unsigned long>(SPEC);

                unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoul(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
#if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long value = bsl::stoul(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
#endif
            }
            position = k_DEFAULT_POSITION;

            if (POS && (POS < 20 || SPEC < 0)) {
                long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoll(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, value, SPEC, value == SPEC);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    long long value = bsl::stoll(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;

            if (POS) {
                const unsigned long long EXPECT = SPEC;

                unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);

                position = k_DEFAULT_POSITION;

                value = bsl::stoull(STRING_VALUE, &position, BASE);
                ASSERTV(LINE, STRING_VALUE, SPEC, EXPECT,   value,
                                                  EXPECT == value);
                ASSERTV(LINE, STRING_VALUE, position, POS, position == POS);
            }
            else {
# if defined(BDE_BUILD_TARGET_EXC)
                try {
                    unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                    ASSERTV(LINE, STRING_VALUE, value, SPEC, !"Did not throw");
                }
                catch(const std::out_of_range&) {
                    // Expected code path
                    ASSERTV(position, k_DEFAULT_POSITION == position);
                }
# endif
            }
            position = k_DEFAULT_POSITION;
        }

#if defined(BDE_BUILD_TARGET_EXC)
        // Testing error handling requires catching exceptions thrown from the
        // 'bsl' function as the standard-mandated error-reporting technique.

        if (verbose) printf("Testing error handling for stoi, stol, stoll,"
                            "  stoul and stoull with 'char' strings.\n");
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_input;            // input
                int         d_base;             // base of input
                ErrType     d_error;            // expected exception type
            } DATA[] = {
                //line  input                   base   error
                //----  -----                   ----   -----
                { L_,   "0",                    10,    e_PASS },
                { L_,   "-0",                   10,    e_PASS },
                { L_,   "--0",                  10,    e_INVALID },
                { L_,   "-+0",                  10,    e_INVALID },
                { L_,   "+0",                   10,    e_PASS },
                { L_,   "+-0",                  10,    e_INVALID },
                { L_,   "++0",                  10,    e_INVALID },
                { L_,   "",                     10,    e_INVALID },
                { L_,   "-",                    10,    e_INVALID },
                { L_,   "+",                    10,    e_INVALID },

                { L_,   " 0",                   10,    e_PASS },
                { L_,   " -0",                  10,    e_PASS },
                { L_,   " --0",                 10,    e_INVALID },
                { L_,   " -+0",                 10,    e_INVALID },
                { L_,   " +0",                  10,    e_PASS },
                { L_,   " +-0",                 10,    e_INVALID },
                { L_,   " ++0",                 10,    e_INVALID },
                { L_,   " ",                    10,    e_INVALID },
                { L_,   " -",                   10,    e_INVALID },
                { L_,   " +",                   10,    e_INVALID },

                { L_,   "0",                    10,    e_PASS },
                { L_,   "0",                    36,    e_PASS },

                { L_,   "-0x",                  10,    e_PASS },
                { L_,   "--0x",                 10,    e_INVALID },
                { L_,   "-+0x",                 10,    e_INVALID },
                { L_,   "+0x",                  10,    e_PASS },
                { L_,   "+-0x",                 10,    e_INVALID },
                { L_,   "++0x",                 10,    e_INVALID },

                { L_,   "-0x",                  36,    e_PASS },
                { L_,   "--0x",                 36,    e_INVALID },
                { L_,   "-+0x",                 36,    e_INVALID },
                { L_,   "+0x",                  36,    e_PASS },
                { L_,   "+-0x",                 36,    e_INVALID },
                { L_,   "++0x",                 36,    e_INVALID },
                { L_,   "",                     36,    e_INVALID },
                { L_,   "-",                    36,    e_INVALID },
                { L_,   "+",                    36,    e_INVALID },

                { L_,   "0x",                   0,     e_PASS },
                { L_,   "-0x",                  0,     e_PASS },
                { L_,   "--0x",                 0,     e_INVALID },
                { L_,   "-+0x",                 0,     e_INVALID },
                { L_,   "+0x",                  0,     e_PASS },
                { L_,   "+-0x",                 0,     e_INVALID },
                { L_,   "++0x",                 0,     e_INVALID },
                { L_,   "",                     0,     e_INVALID },
                { L_,   "-",                    0,     e_INVALID },
                { L_,   "+",                    0,     e_INVALID },

                { L_,   "0x",                   16,    e_PASS },
                { L_,   "-0x",                  16,    e_PASS },
                { L_,   "--0x",                 16,    e_INVALID },
                { L_,   "-+0x",                 16,    e_INVALID },
                { L_,   "+0x",                  16,    e_PASS },
                { L_,   "+-0x",                 16,    e_INVALID },
                { L_,   "++0x",                 16,    e_INVALID },

                { L_,   "+Z",                   35,    e_INVALID },
                { L_,   "+z",                   36,    e_PASS },
                { L_,   "ZzZzzZZzzZZzzzz",      36,    e_RANGE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int intType = e_INT; intType != e_NOT_INT; ++intType) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int          LINE   = DATA[ti].d_lineNum;
                    const char        *INPUT  = DATA[ti].d_input;
                    const int          BASE   = DATA[ti].d_base;
                    const ErrType      ERROR  = DATA[ti].d_error;

                    const bsl::string  STRING_VALUE(INPUT);

                    if (veryVerbose) {
                        T_ P_(INPUT) P_(BASE) P((int)ERROR);
                    }

                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                          bsl::stoul(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_LONG_LONG_INT : {
                            long long value =
                                          bsl::stoll(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                    }

                    size_t next = 0;
                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                         bsl::stoul(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_LONG_LONG_INT : {
                            long long value =
                                         bsl::stoll(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                        bsl::stoull(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                        ASSERTV(next, !next);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                        ASSERTV(next, !next);
                    }
                }

                if (veryVerbose) printf("negative testing");
                {
                    bsls::AssertTestHandlerGuard guard;

                    size_t next = 0;
                    for (int i = -3; i != 43; ++i) {
                        if (!i || (2 <= i && i <= 36)) {
                            ASSERT_PASS(bsl::stoi("0", NULL, i));
                            ASSERT_PASS(bsl::stoi("-1", &next, i));
                        }
                        else {
                            ASSERT_FAIL(bsl::stoi("0", NULL, i));
                            ASSERT_FAIL(bsl::stoi("-1", &next, i));
                        }
                    }
                }
            }
        }

        if (verbose) printf("Testing error handling for stoi, stol, stoll,"
                            "  stoul and stoull with 'wchar_t' strings.\n");
        {
            static const struct {
                int            d_lineNum;          // source line number
                const wchar_t *d_input;            // input
                int            d_base;             // base of input
                ErrType        d_error;            // expected exception type
            } DATA[] = {
                //line  input                   base   error
                //----  -----                   ----   -----
                { L_,   L"0",                    10,    e_PASS },
                { L_,   L"-0",                   10,    e_PASS },
                { L_,   L"--0",                  10,    e_INVALID },
                { L_,   L"-+0",                  10,    e_INVALID },
                { L_,   L"+0",                   10,    e_PASS },
                { L_,   L"+-0",                  10,    e_INVALID },
                { L_,   L"++0",                  10,    e_INVALID },
                { L_,   L"",                     10,    e_INVALID },
                { L_,   L"-",                    10,    e_INVALID },
                { L_,   L"+",                    10,    e_INVALID },

                { L_,   L" 0",                   10,    e_PASS },
                { L_,   L" -0",                  10,    e_PASS },
                { L_,   L" --0",                 10,    e_INVALID },
                { L_,   L" -+0",                 10,    e_INVALID },
                { L_,   L" +0",                  10,    e_PASS },
                { L_,   L" +-0",                 10,    e_INVALID },
                { L_,   L" ++0",                 10,    e_INVALID },
                { L_,   L" ",                    10,    e_INVALID },
                { L_,   L" -",                   10,    e_INVALID },
                { L_,   L" +",                   10,    e_INVALID },

                { L_,   L"0",                    10,    e_PASS },
                { L_,   L"0",                    36,    e_PASS },

                { L_,   L"-0x",                  10,    e_PASS },
                { L_,   L"--0x",                 10,    e_INVALID },
                { L_,   L"-+0x",                 10,    e_INVALID },
                { L_,   L"+0x",                  10,    e_PASS },
                { L_,   L"+-0x",                 10,    e_INVALID },
                { L_,   L"++0x",                 10,    e_INVALID },

                { L_,   L"-0x",                  36,    e_PASS },
                { L_,   L"--0x",                 36,    e_INVALID },
                { L_,   L"-+0x",                 36,    e_INVALID },
                { L_,   L"+0x",                  36,    e_PASS },
                { L_,   L"+-0x",                 36,    e_INVALID },
                { L_,   L"++0x",                 36,    e_INVALID },
                { L_,   L"",                     36,    e_INVALID },
                { L_,   L"-",                    36,    e_INVALID },
                { L_,   L"+",                    36,    e_INVALID },

                { L_,   L"0x",                   0,     e_PASS },
                { L_,   L"-0x",                  0,     e_PASS },
                { L_,   L"--0x",                 0,     e_INVALID },
                { L_,   L"-+0x",                 0,     e_INVALID },
                { L_,   L"+0x",                  0,     e_PASS },
                { L_,   L"+-0x",                 0,     e_INVALID },
                { L_,   L"++0x",                 0,     e_INVALID },
                { L_,   L"",                     0,     e_INVALID },
                { L_,   L"-",                    0,     e_INVALID },
                { L_,   L"+",                    0,     e_INVALID },

                { L_,   L"0x",                   16,    e_PASS },
                { L_,   L"-0x",                  16,    e_PASS },
                { L_,   L"--0x",                 16,    e_INVALID },
                { L_,   L"-+0x",                 16,    e_INVALID },
                { L_,   L"+0x",                  16,    e_PASS },
                { L_,   L"+-0x",                 16,    e_INVALID },
                { L_,   L"++0x",                 16,    e_INVALID },

                { L_,   L"+Z",                   35,    e_INVALID },
                { L_,   L"+z",                   36,    e_PASS },
                { L_,   L"ZzZzzZZzzZZzzzz",      36,    e_RANGE },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int intType = e_INT; intType != e_NOT_INT; ++intType) {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int          LINE   = DATA[ti].d_lineNum;
                    const wchar_t     *INPUT  = DATA[ti].d_input;
                    const int          BASE   = DATA[ti].d_base;
                    const ErrType      ERROR  = DATA[ti].d_error;

                    const bsl::wstring  STRING_VALUE(INPUT);

                    if (veryVerbose) {
                        T_ P_(INPUT) P_(BASE) P((int)ERROR);
                    }

                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                          bsl::stoul(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, INPUT, intType, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_LONG_LONG_INT : {
                            long long value =
                                          bsl::stoll(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, intType, INPUT, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                         bsl::stoull(STRING_VALUE, NULL, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                          } break;
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                    }

                    size_t next = 0;
                    try {
                        switch (intType) {
                          case e_INT : {
                            int value = bsl::stoi(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_LONG_INT : {
                            long value = bsl::stol(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_UNSIGNED_LONG_INT : {
                            unsigned long value =
                                         bsl::stoul(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_LONG_LONG_INT : {
                            long long value =
                                         bsl::stoll(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                          case e_UNSIGNEDLONG_LONG_INT : {
                            unsigned long long value =
                                        bsl::stoull(STRING_VALUE, &next, BASE);
                            ASSERTV(LINE, STRING_VALUE, value, (int)ERROR,
                                    e_PASS == ERROR || !"Did not throw");
                            ASSERTV(next, 0 != next);
                          } break;
                        }
                    }
                    catch(const std::invalid_argument&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_INVALID == ERROR);
                        ASSERTV(next, !next);
                    }
                    catch(const std::out_of_range&) {
                        ASSERTV(LINE, intType, STRING_VALUE, BASE, (int)ERROR,
                                e_RANGE == ERROR);
                        ASSERTV(next, !next);
                    }
                }

                if (veryVerbose) printf("negative testing");
                {
                    bsls::AssertTestHandlerGuard guard;

                    size_t next = 0;
                    for (int i = -3; i != 43; ++i) {
                        if (!i || (2 <= i && i <= 36)) {
                            ASSERT_PASS(bsl::stoi("0", NULL, i));
                            ASSERT_PASS(bsl::stoi("-1", &next, i));
                        }
                        else {
                            ASSERT_FAIL(bsl::stoi("0", NULL, i));
                            ASSERT_FAIL(bsl::stoi("-1", &next, i));
                        }
                    }
                }
            }
        }
#endif // BDE_BUILD_TARGET_EXC
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Verify that the hashAppend function works properly and is picked
        //   up by 'bslh::Hash'
        //
        // Concerns:
        //: 1 'bslh::Hash' picks up 'hashAppend(string)' and can hash strings
        //: 2 'hashAppend' hashes the entire string, regardless of 'char' or
        //:   'wchar'
        //
        // Plan:
        //: 1 Use 'bslh::Hash' to hash a few values of strings with each char
        //:   type. (C-1,2)
        //
        // Testing:
        //   hashAppend(HASHALG& hashAlg, const basic_string& str);
        //   hashAppend(HASHALG& hashAlg, const native_std::basic_string& str);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase29();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase29();

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING THE SHORT STRING OPTIMIZATION
        //
        // Concerns:
        //  - String should have an initial non-zero capacity (short string
        //    buffer).
        //  - It shouldn't allocate up to that capacity.
        //  - It should work with the char_type larger than the short string
        //    buffer.
        //  - It should work with the NULL-terminator different from '\0' to
        //    make sure that the implementation always uses char_type() default
        //    constructor to terminate the string rather than a null literal.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase28' for details.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING THE SHORT STRING OPTIMIZATION"
                            "\n=====================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase28();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase28();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // REPRODUCING KNOWN BUG CAUSING SEGFAULT IN 'find'
        //   This is a problem with the native library, being pursued in DRQS
        //   16870796.  This test will do nothing unless run in verbose mode.
        //
        // Concerns:
        //   That a known bug in string::find on Sun cc is reproduced in this
        //   test suite.
        //
        // Plan:
        //
        // Testing:
        //   DRQS 16870796
        // --------------------------------------------------------------------

        if (verbose)
                printf("\nREPRODUCING KNOWN BUG CAUSING SEGFAULT IN 'find'"
                       "\n================================================\n");

        if (verbose) printf("\nReproducing known segfault in string::find"
                            "\n==========================================\n");

        if (verbose) {
            const char *pc = std::char_traits<char>::find("bcabcd", 2, 'a');
            P((const void *) pc);
            ASSERT(0 == pc);

            const wchar_t *pw =
                           std::char_traits<wchar_t>::find(L"bcabcd", 2, L'a');
            P((const void *) pw);
            ASSERT(0 == pw);

            bsl::basic_string<wchar_t, std::char_traits<wchar_t> > s =
                                                                 L"aababcabcd";
            s.find(L"abcde", 0, 5);    // segfaults
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING CONVERSIONS WITH NATIVE STRINGS
        //
        // Concerns:
        //: 1 A bsl::basic_string is implicitly convertible to a
        //:     native_std::basic_string with the same CHAR_TYPE and
        //: 2   CHAR_TRAITS.
        //:     A native_std::basic_string is implicitly convertible to a
        //:     bsl::basic_string with the same CHAR_TYPE and
        //: 3   CHAR_TRAITS.
        //:     A bsl::basic_string and a native_std::basic_string with the
        //:     same template parameters will have the same npos value.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase26' for details.
        //
        // Testing:
        //   'npos' VALUE
        //   operator native_std::basic_string<CHAR, TRAITS, A2>() const;
        //   basic_string(const native_std::basic_string<CHAR, TRAITS, A2>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSIONS WITH NATIVE STRINGS"
                            "\n=======================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase26();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase26();

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTIONS
        //
        // Concerns:
        //: 1 The expect 'logic_error' derived exceptions are thrown where
        //:   mandated by the C++ standard.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase25' for details.
        //
        // Testing:
        //   CONCERN: 'std::length_error' is used properly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EXCEPTIONS"
                            "\n==================\n");

        if (verbose) printf("\nTesting use of 'std::length_error'"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase25();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase25();

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING COMPARISONS
        //
        // Concerns:
        //: 1 The 'compare' member function and comparison operators return the
        //:   canonical ordering of two string values.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase24' for details.
        //
        // Testing:
        //   int compare(const string& str) const;
        //   int compare(pos1, n1, const string& str) const;
        //   int compare(pos1, n1, const string& str, pos2, n2 = npos) const;
        //   int compare(const C *s) const;
        //   int compare(pos1, n1, const C *s) const;
        //   int compare(pos1, n1, const C *s, n2) const;
        //   bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator<(const C *, const string<C,CT,A>&);
        //   bool operator<(const string<C,CT,A>&, const C *);
        //   bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator>(const C *, const string<C,CT,A>&);
        //   bool operator>(const string<C,CT,A>&, const C *);
        //   bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator<=(const C *, const string<C,CT,A>&);
        //   bool operator<=(const string<C,CT,A>&, const C *);
        //   bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator>=(const C *, const string<C,CT,A>&);
        //   bool operator>=(const string<C,CT,A>&, const C *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISONS"
                            "\n===================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase24();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase24();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing comparisons"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase24Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase24Negative();
#endif

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING SUBSTRING OPERATIONS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase23' for details.
        //
        // Testing:
        //   string substr(pos, n) const;
        //   size_type copy(char *s, n, pos = 0) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SUBSTRING OPERATIONS"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase23();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase23();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing 'copy'"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase23Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase23Negative();
#endif

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'find...' VARIANTS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase22' for details.
        //
        // Testing:
        //   size_type find(const string& str, pos = 0) const;
        //   size_type find(const C *s, pos, n) const;
        //   size_type find(const C *s, pos = 0) const;
        //   size_type find(C c, pos = 0) const;
        //   size_type rfind(const string& str, pos = 0) const;
        //   size_type rfind(const C *s, pos, n) const;
        //   size_type rfind(const C *s, pos = 0) const;
        //   size_type rfind(C c, pos = 0) const;
        //   size_type find_first_of(const string& str, pos = 0) const;
        //   size_type find_first_of(const C *s, pos, n) const;
        //   size_type find_first_of(const C *s, pos = 0) const;
        //   size_type find_first_of(C c, pos = 0) const;
        //   size_type find_last_of(const string& str, pos = 0) const;
        //   size_type find_last_of(const C *s, pos, n) const;
        //   size_type find_last_of(const C *s, pos = 0) const;
        //   size_type find_last_of(C c, pos = 0) const;
        //   size_type find_first_not_of(const string& str, pos = 0) const;
        //   size_type find_first_not_of(const C *s, pos, n) const;
        //   size_type find_first_not_of(const C *s, pos = 0) const;
        //   size_type find_first_not_of(C c, pos = 0) const;
        //   size_type find_last_not_of(const string& str, pos = 0) const;
        //   size_type find_last_not_of(const C *s, pos, n) const;
        //   size_type find_last_not_of(const C *s, pos = 0) const;
        //   size_type find_last_not_of(C c, pos = 0) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'find...' VARIANTS"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase22();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase22();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing 'find...' methods."
                            "\n===================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase22Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase22Negative();
#endif

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase21' for details.
        //
        // Testing:
        //   void swap(basic_string& other);
        //   void swap(basic_string& lhs, basic_string& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase21();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase21();

        if (verbose) printf("\nTesting Allocator Propagation on Swap"
                            "\n=====================================\n");
        TestDriver<char>::testCase21_propagate_on_container_swap();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'replace'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase20' for details.
        //
        // Testing:
        //   basic_string& replace(pos1, n1, const string& str);
        //   basic_string& replace(pos1, n1, const string& str, pos2, n2=npos);
        //   basic_string& replace(pos1, n1, const C *s, n2);
        //   basic_string& replace(pos1, n1, const C *s);
        //   basic_string& replace(pos1, n1, size_type n2, C c);
        //   basic_string& replace(const_iterator p, q, const string& str);
        //   basic_string& replace(const_iterator p, q, const C *s, n2);
        //   basic_string& replace(const_iterator p, q, const C *s);
        //   basic_string& replace(const_iterator p, q, size_type n2, C c);
        //   template <It> basic_string& replace(const_iterator p, q, It f, l);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'replace'"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'replace' with value"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase20();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase20();

        if (verbose) printf("\n... with 'char' & matching integral types.\n");
        TestDriver<char>::testCase20MatchTypes();

        if (verbose) printf("\n... with 'wchar_t' & matching integ types.\n");
        TestDriver<wchar_t>::testCase20MatchTypes();

        if (verbose) printf("\nTesting 'replace' with range"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase20Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase20Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase20Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase20Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing 'replace'"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase20Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase20Negative();
#endif

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'erase' AND 'pop_back'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase19' for details.
        //
        // Testing:
        //   iterator erase(size_type pos = 0, size_type n = npos);
        //   iterator erase(const_iterator position);
        //   iterator erase(const_iterator first, const_iterator last);
        //   void pop_back();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'erase' AND 'pop_back'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase19();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase19();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing for 'erase' and 'pop_back'"
                            "\n===========================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase19Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase19Negative();
#endif

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase18' for details.
        //
        // Testing:
        //   basic_string& insert(size_type pos, size_type n, CHAR_TYPE c);
        //   iterator insert(const_iterator pos, CHAR_TYPE value);
        //   iterator insert(const_iterator pos, size_type n, CHAR_TYPE value);
        //   template <class Iter> iterator insert(const_iterator, Iter, Iter);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INSERTION"
                            "\n=================\n");

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase18();

        if (verbose) printf("\nTesting Range Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase18Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase18Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase18Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase18Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Insertion"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase18Negative();
#endif

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'append'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase17' for details.
        //
        // Testing:
        //   basic_string& operator+=(CHAR_TYPE c);
        //   basic_string& operator+=(const StringRefData& strRefData);
        //   basic_string& append(const basic_string& str);
        //   basic_string& append(const basic_string& str, pos, n = npos);
        //   basic_string& append(const CHAR_TYPE *s, size_type n);
        //   basic_string& append(const CHAR_TYPE *s);
        //   basic_string& append(size_type n, CHAR_TYPE c);
        //   template <class Iter> basic_string& append(Iter first, Iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'append'"
                            "\n================\n");

        if (verbose) printf("\nTesting Value Append"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase17();

        if (verbose) printf("\nTesting Range Append"
                            "\n====================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase17Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase17Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase17Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase17Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Range Append"
                            "\n=============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase17Negative();
#endif

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase16' for details.
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   reverse_iterator rbegin();
        //   reverse_iterator rend();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        //   const_reverse_iterator rbegin() const;
        //   const_reverse_iterator rend() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase16();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase16();

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase15' for details.
        //
        // Testing:
        //   reference operator[](size_type pos);
        //   reference at(size_type pos);
        //   reference front();
        //   reference back();
        //   const_reference front() const;
        //   const_reference back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ELEMENT ACCESS"
                            "\n======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase15();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Element Access"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase15Negative();
#endif

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'reserve' AND 'capacity'
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase14' for details.
        //
        // Testing:
        //   void reserve(size_type n);
        //   void resize(size_type n);
        //   void resize(size_type n, CHAR_TYPE c);
        //   size_type max_size() const;
        //   size_type capacity() const;
        //   bool empty() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'reserve' AND 'capacity'"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase14();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase14();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase13' for details.
        //
        // Testing:
        //   basic_string& assign(const basic_string& str);
        //   basic_string& assign(bslmf::MovableRef<basic_string> str);
        //   basic_string& assign(const basic_string& str, pos, n = npos);
        //   basic_string& assign(const CHAR_TYPE *s, size_type n);
        //   basic_string& assign(const CHAR_TYPE *s);
        //   basic_string& assign(const StringRefData<CHAR_TYPE>& strRef);
        //   basic_string& assign(size_type n, CHAR_TYPE c);
        //   template <class Iter> basic_string& assign(Iter first, Iter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT"
                            "\n==================\n");

        if (verbose) printf("\nTesting Initial-Length Assignment"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase13();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase13();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase13InputIterator();

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary forward iterator.\n");
        TestDriver<char>::testCase13Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase13Range(CharArray<char>());

        if (verbose) printf("\n... with 'char' "
                            "and StringRefData.\n");
        TestDriver<char>::testCase13StrRefData();

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase13InputIterator();

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary forward iterator.\n");
        TestDriver<wchar_t>::testCase13Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase13Range(CharArray<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and StringRefData.\n");
        TestDriver<wchar_t>::testCase13StrRefData();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Assignment"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase13Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase13Negative();
#endif

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase12' for details.
        //
        // Testing:
        //   basic_string(const basic_string& str, pos, n = npos, a = A());
        //   basic_string(const CHAR_TYPE *s, a = A());
        //   basic_string(const CHAR_TYPE *s, size_type n, a = A());
        //   basic_string(size_type n, CHAR_TYPE c = CHAR_TYPE(), a = A());
        //   template<class Iter> basic_string(Iter first, Iter last, a = A());
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase12();

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase12Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase12Range(CharArray<char>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary input iterator.\n");
        TestDriver<wchar_t>::testCase12Range(CharList<wchar_t>());

        if (verbose) printf("\n... with 'wchar_t' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<wchar_t>::testCase12Range(CharArray<wchar_t>());

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative testing of Constructors"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase12Negative();
#endif

      } break;
      case 11: // falls through
      case 10: // falls through
      case  9: // falls through
      case  8: // falls through
      case  7: // falls through
      case  6: // falls through
      case  5: // falls through
      case  4: // falls through
      case  3: // falls through
      case  2: // falls through
      case  1: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY PRIMARY TEST DRIVER"
                   "\n==============================================\n",
                   test);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   1) That performance does not regress between versions.
        //   2) That small "improvements" can be tested w.r.t. to performance,
        //      in a uniform benchmark.
        //
        // Plan:
        //   We follow a simple benchmark that copies strings into a table.  We
        //   create two tables, one containing long strings, and another
        //   containing short strings.  All strings are preallocated so that we
        //   do not measure the performance of the random generator.
        //   Specifically, we wish to measure the time taken by
        //     C1) The various constructors.
        //     C2) The copy constructor.
        //     A1) The copy assignment.
        //     A2) The 'assign' operations.
        //     M1) The 'append' operation in its various forms (including
        //         'push_back').
        //     M2) The 'insert' operation in its various forms.
        //     M3) The 'replace' operation in its various forms.
        //     S1) The 'swap' operation in its various forms.
        //     F1) The 'find' and 'rfind' operations.
        //     F2) The 'find_first_of' and 'find_last_of' operations.
        //     F3) The 'find_first_not_of' and 'find_last_not_of' operations.
        //   Also we wish to record the size of the various string
        //   instantiations.
        //
        //   Note: This is a *synthetic* benchmark.  It does not replace
        //   measuring real benchmarks (e.g., for strings, ADSP) whose
        //   conclusions may differ due to different memory allocation and
        //   access patterns, function call frequencies, etc.
        //
        // Testing:
        //   This "test" measures performance of basic operations, for
        //   performance regression.
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        const int NITER = (argc < 3) ? 1 : atoi(argv[2]);

        if (verbose)
          printf("\tUsing %d repetitions of the tests (default 1).\n", NITER);

        const int RANDOM_SEED = (argc < 4) ? 0x12345678 : atoi(argv[3]);

        if (veryVerbose)
          printf("\tUsing %d random seed (of the tests (default 1).\n", NITER);

        srand(RANDOM_SEED);

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCaseM1(NITER, RANDOM_SEED);

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
