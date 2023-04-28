// bslstl_string.t.cpp                                                -*-C++-*-
#include <bslstl_string.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_stringref.h>

#include <bsla_fallthrough.h>
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
// TYPES:
// [40] bsl::u8string
// [40] bsl::u16string
// [40] bsl::u32string
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
// [26] basic_string(const std::basic_string<CHAR, TRAITS, A2>&);
// [12] basic_string(const STRING_VIEW_LIKE_TYPE& object, a = A());
// [12] basic_string(const STRING_VIEW_LIKE_TYPE& object, pos, n, a = A());
// [  ] basic_string(const StringRefData& strRefData, a = A());
// [33] basic_string(initializer_list<CHAR_TYPE> values, basicAllocator);
// [ 2] ~basic_string();
//
/// MANIPULATORS:
// [ 9] basic_string& operator=(const basic_string& rhs);
// [ 9] basic_string& operator=(const STRING_VIEW_LIKE_TYPE& rhs);
// [ 9] basic_string& operator=(MovableRef<basic_string> rhs);
// [  ] basic_string& operator=(const CHAR_TYPE *s);
// [  ] basic_string& operator=(CHAR_TYPE c);
// [33] basic_string& operator=(initializer_list<CHAR_TYPE> values);
// [  ] basic_string& operator+=(const basic_string& rhs);
// [  ] basic_string& operator+=(const CHAR_TYPE *s);
// [17] basic_string& operator+=(CHAR_TYPE c);
// [17] basic_string& operator+=(const STRING_VIEW_LIKE_TYPE& rhs);
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
// [13] basic_string& assign(const STRING_VIEW_LIKE_TYPE& s);
// [13] basic_string& assign(const STRING_VIEW_LIKE_TYPE& s, p, n = npos);
// [13] basic_string& assign(size_type n, CHAR_TYPE c);
// [13] template <class Iter> basic_string& assign(Iter first, Iter last);
// [33] basic_string& assign(initializer_list<CHAR_TYPE> values);
// [17] basic_string& append(const basic_string& str);
// [17] basic_string& append(const basic_string& str, pos, n = npos);
// [17] basic_string& append(const STRING_VIEW_LIKE_TYPE& strView);
// [17] basic_string& append(const STRING_VIEW_LIKE_TYPE& strView, pos,n);
// [17] basic_string& append(const CHAR_TYPE *s, size_type n);
// [17] basic_string& append(const CHAR_TYPE *s);
// [17] basic_string& append(size_type n, CHAR_TYPE c);
// [17] template <class Iter> basic_string& append(Iter first, Iter last);
// [33] basic_string& append(initializer_list<CHAR_TYPE> values);
// [ 2] void push_back(CHAR_TYPE c);
// [18] basic_string& insert(size_type pos1, const string& str);
// [18] basic_string& insert(size_type pos1, const string& str, pos2, n=npos);
// [18] basic_string& insert(size_type pos, const CHAR_TYPE *s, n);
// [18] basic_string& insert(size_type pos, const CHAR_TYPE *s);
// [18] basic_string& insert(size_type pos, size_type n, CHAR_TYPE c);
// [18] basic_string& insert(size_type p, const STRING_VIEW_LIKE_TYPE& s);
// [18] basic_string& insert(p1, const STRING_VIEW_LIKE_TYPE& s, p2, n);
// [18] iterator insert(const_iterator pos, CHAR_TYPE value);
// [18] iterator insert(const_iterator pos, size_type n, CHAR_TYPE value);
// [18] template <class Iter> iterator insert(const_iterator, Iter, Iter);
// [33] iterator insert(const_iterator pos, initializer_list<CHAR_TYPE>);
// [19] void pop_back();
// [19] iterator erase(size_type pos = 0, size_type n = npos);
// [19] iterator erase(const_iterator position);
// [19] iterator erase(const_iterator first, const_iterator last);
// [20] basic_string& replace(pos1, n1, const string& str);
// [20] basic_string& replace(pos1, n1, const STRING_VIEW_LIKE_TYPE& str);
// [20] basic_string& replace(pos1, n1, const string& str, pos2, n2=npos);
// [20] basic_string& replace(p1,n1,const STRING_VIEW_LIKE_TYPE& s,p2,n2);
// [20] basic_string& replace(pos1, n1, const C *s, n2);
// [20] basic_string& replace(pos1, n1, const C *s);
// [20] basic_string& replace(pos1, n1, size_type n2, C c);
// [20] basic_string& replace(const_iterator p, q, const string& str);
// [20] basic_string& replace(const_iterator p,q,const STRING_VIEW_LIKE&);
// [20] basic_string& replace(const_iterator p, q, const C *s, n2);
// [20] basic_string& replace(const_iterator p, q, const C *s);
// [20] basic_string& replace(const_iterator p, q, size_type n2, C c);
// [20] template <It> basic_string& replace(const_iterator p, q, It f, l);
// [36] CHAR_TYPE *data();
// [21] void swap(basic_string& other);
// [39] void shrink_to_fit();
//
// ACCESSORS:
// [26] operator std::basic_string<CHAR, TRAITS, A2>() const;
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
// [22] size_type find(const STRING_VIEW_LIKE_TYPE& str, pos = 0) const;
// [22] size_type find(const C *s, pos, n) const;
// [22] size_type find(const C *s, pos = 0) const;
// [22] size_type find(C c, pos = 0) const;
// [22] size_type rfind(const string& str, pos = npos) const;
// [22] size_type rfind(const STRING_VIEW_LIKE_TYPE& str, pos) const;
// [22] size_type rfind(const C *s, pos, n) const;
// [22] size_type rfind(const C *s, pos = npos) const;
// [22] size_type rfind(C c, pos = npos) const;
// [22] size_type find_first_of(const string& str, pos = 0) const;
// [22] size_type find_first_of(const STRING_VIEW_LIKE_TYPE& s, p) const;
// [22] size_type find_first_of(const C *s, pos, n) const;
// [22] size_type find_first_of(const C *s, pos = 0) const;
// [22] size_type find_first_of(C c, pos = 0) const;
// [22] size_type find_last_of(const string& str, pos = npos) const;
// [22] size_type find_last_of(const STRING_VIEW_LIKE_TYPE& str, p) const;
// [22] size_type find_last_of(const C *s, pos, n) const;
// [22] size_type find_last_of(const C *s, pos = npos) const;
// [22] size_type find_last_of(C c, pos = npos) const;
// [22] size_type find_first_not_of(const string& str, pos = 0) const;
// [22] size_type find_first_not_of(const STRING_VIEW_LIKE_TYPE&,p) const;
// [22] size_type find_first_not_of(const C *s, pos, n) const;
// [22] size_type find_first_not_of(const C *s, pos = 0) const;
// [22] size_type find_first_not_of(C c, pos = 0) const;
// [22] size_type find_last_not_of(const string& str, pos = npos) const;
// [22] size_type find_last_not_of(const STRING_VIEW_LIKE_TYPE&, p) const;
// [22] size_type find_last_not_of(const C *s, pos, n) const;
// [22] size_type find_last_not_of(const C *s, pos = npos) const;
// [22] size_type find_last_not_of(C c, pos = npos) const;
// [23] string substr(pos, n) const;
// [23] size_type copy(char *s, n, pos = 0) const;
// [24] int compare(const string& str) const;
// [24] int compare(pos1, n1, const string& str) const;
// [24] int compare(pos1, n1, const string& str, pos2, n2 = npos) const;
// [24] int compare(const C *s) const;
// [24] int compare(pos1, n1, const C *s) const;
// [24] int compare(pos1, n1, const C *s, n2) const;
// [41] bool starts_with(basic_string_view characterString) const;
// [41] bool starts_with(CHAR_TYPE character) const;
// [41] bool starts_with(const CHAR_TYPE *characterString) const;
// [41] bool ends_with(basic_string_view characterString) const;
// [41] bool ends_with(CHAR_TYPE character) const;
// [41] bool ends_with(const CHAR_TYPE *characterString) const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] operator==(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [ 6] operator==(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [ 6] bool operator==(const C *, const string<C,CT,A>&);
// [ 6] bool operator==(const string<C,CT,A>&, const C *);
// [ 6] bool operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
// [ 6] operator!=(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [ 6] operator!=(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [ 6] bool operator!=(const C *, const string<C,CT,A>&);
// [ 6] bool operator!=(const string<C,CT,A>&, const C *);
// [24] bool operator<(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [24] bool operator<(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [24] bool operator<(const C *, const string<C,CT,A>&);
// [24] bool operator<(const string<C,CT,A>&, const C *);
// [24] bool operator>(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [24] bool operator>(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [24] bool operator>(const C *, const string<C,CT,A>&);
// [24] bool operator>(const string<C,CT,A>&, const C *);
// [24] bool operator<=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator<=(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [24] bool operator<=(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [24] bool operator<=(const C *, const string<C,CT,A>&);
// [24] bool operator<=(const string<C,CT,A>&, const C *);
// [24] bool operator>=(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] bool operator>=(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
// [24] bool operator>=(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
// [24] bool operator>=(const C *, const string<C,CT,A>&);
// [24] bool operator>=(const string<C,CT,A>&, const C *);
// [24] auto operator<=>(const string<C,CT,A>&, const string<C,CT,A>&);
// [24] auto operator<=>(const string<C,CT,A>&, const C *);
// [24] auto operator<=>(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
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
// [29] hashAppend(HASHALG& hashAlg, const std::basic_string& str);
// [42] size_type erase(basic_string& str, const C& c);
// [42] size_type erase_if(basic_string& str, const UNARY_PRED& pred);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [43] USAGE EXAMPLE
// [11] CONCERN: The object has the necessary type traits
// [26] 'npos' VALUE
// [25] CONCERN: 'std::length_error' is used properly
// [27] DRQS 16870796
// [ 9] basic_string& operator=(const CHAR_TYPE *s); [NEGATIVE ONLY]
// [36] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [38] CLASS TEMPLATE DEDUCTION GUIDES
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

const int  LARGE_SIZE_VALUE = 2 * MAX_ALIGN;
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

#if 0
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
#endif

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

template <class CHAR_TYPE, class TRAITS, class ALLOC>
void debugprint(const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOC>& v)
    // This is intended to work for 'CHAR_TYPE' being 'char', 'wchar_t',
    // 'char8_t', 'unsigned char', 'char16_t', or 'char32_t'.  Using
    // 'wprintf("%s ...' was considered, but as that would work only for
    // 'sizeof(CHAR_TYPE) == sizeof(wchar_t)', we chose to cast the characters
    // to 'wchar_t' and print them one at a time.
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            printf("%lc", static_cast<wchar_t>(v[i]));
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

// Legacy debug print support.

template <class CHAR_TYPE>
void dbg_print_impl(const CHAR_TYPE *s)
{
    putchar('"');
    if (sizeof(CHAR_TYPE) == sizeof(char)) {
        printf("%s", reinterpret_cast<const char *>(s));
    }
    else {
        while (*s) {
            printf("%lc", static_cast<wchar_t>(*s));
            ++s;
        }
    }
    putchar('"');
}

void dbg_print(const char *s)
{
    dbg_print_impl(s);
}

void dbg_print(const wchar_t *s)
{
    dbg_print_impl(s);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
void dbg_print(const char8_t *s)
{
    dbg_print_impl(s);
}
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
void dbg_print(const char16_t *s)
{
    dbg_print_impl(s);
}

void dbg_print(const char32_t *s)
{
    dbg_print_impl(s);
}
#endif

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

static int numCopyCtorCalls    = 0;
static int numDestructorCalls  = 0;

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

                                 // ==============
                                 // class UserChar
                                 // ==============

template <int SIZE>
class UserChar {
    // This class is a simulation of a user-defined char type.  It has a
    // variable object size to test that the string works with chars larger
    // than 'char' and 'wchar_t'.
  private:
    // DATA
    union {
        size_t d_words[SIZE];
        char   d_char;
    };

  public:
    // CREATORS
    explicit
    UserChar(char c = 10);

    // ACCESSORS
    bool operator==(const UserChar& rhs) const;
    bool operator!=(const UserChar& rhs) const;
};

template <int SIZE>
inline
UserChar<SIZE>::UserChar(char c)
: d_char(c)
{}

template <int SIZE>
inline
bool UserChar<SIZE>::operator==(const UserChar& rhs) const {
    return d_char == rhs.d_char;
}

template <int SIZE>
inline
bool UserChar<SIZE>::operator!=(const UserChar& rhs) const {
    return !(*this == rhs);
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {
  private:
    // PRIVATE TYPES
    typedef ALLOC                        AllocBase;
    typedef bsl::allocator_traits<ALLOC> TraitsBase;

  public:
    // TYPES
    typedef typename TraitsBase::size_type         size_type;

    template <class BDE_OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be
        // 'ALLOC::rebind<BDE_OTHER_TYPE>::other' instead of
        // 'LimitAlloc<ALLOC::rebind<BDE_OTHER_TYPE>::otherX>'.

        typedef typename TraitsBase::template rebind_traits<BDE_OTHER_TYPE>
                                                              RebindTraitsBase;

        typedef LimitAllocator<typename RebindTraitsBase::allocator_type>
                                                              other;
    };

  private:
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
bool isNativeString(const std::basic_string<TYPE,TRAITS,ALLOC>&)
    { return true; }

namespace BloombergLP {
namespace bslma {
// Specialize trait to clarify for bde_verify that 'LimitAllocator' does not
// require satisfy the 'UsesBslmaAllocator' trait.
template <class ALLOC>
struct UsesBslmaAllocator<LimitAllocator<ALLOC> > : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace

                 // =================================
                 // class ConvertibleToStringViewType
                 // =================================

template <class TYPE,
          class TRAITS = bsl::char_traits<TYPE> >
class ConvertibleToStringViewType
    // This test class provides conversion operators to
    // 'bsl::basic_string_view' object.  It is used for testing methods that
    // accept 'StringViewLike' types.
{
    // DATA
    const TYPE *d_value_p;  // value (held, not owned)

  public:
    // CREATORS
    explicit ConvertibleToStringViewType(const TYPE *value);
        // Create an object that has the specified 'value'.

    // ACCESSORS
    operator bsl::basic_string_view<TYPE, TRAITS>() const;
        // Convert this object to a 'bsl::basic_string_view' object,
        // instantiated with the same character type and traits type.  The
        // return string will contain the same sequence of characters as this
        // object.  Note that this conversion operator can be invoked
        // implicitly (e.g., during argument passing).

    const TYPE *data() const;
        // Return the value of this object.

    size_t length() const;
        // Return the length of the underlying string.
};

                 // ---------------------------------
                 // class ConvertibleToStringViewType
                 // ---------------------------------

// CREATORS
template <class TYPE, class TRAITS>
inline
ConvertibleToStringViewType<TYPE, TRAITS>::
ConvertibleToStringViewType(const TYPE *value)
: d_value_p(value)
{
}

// ACCESSORS
template <class TYPE, class TRAITS>
inline
ConvertibleToStringViewType<TYPE, TRAITS>::
operator bsl::basic_string_view<TYPE, TRAITS>() const
{
    return bsl::basic_string_view<TYPE, TRAITS>(d_value_p);
}

template <class TYPE, class TRAITS>
inline
const TYPE *ConvertibleToStringViewType<TYPE, TRAITS>::data() const
{
    return d_value_p;
}

template <class TYPE, class TRAITS>
inline
size_t ConvertibleToStringViewType<TYPE, TRAITS>::length() const
{
    return TRAITS::length(d_value_p);
}

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

    typedef char size1[1];  // A type of size 1.
    typedef char size2[2];  // A type of size 2.

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

    static size1& fromStringView(...);
    static size2& fromStringView(const Obj &);
        // Test apparatus for implicit conversion from 'string_view' to string.

    // TEST CASES
    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_move_assignment_dispatch();
    static void testCase9_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase9Move();
        // Test move assignment operator ('operator=').

    static void testCase9Negative();
        // Negative test for assignment operator ('operator=').

    static void testCase8();
        // Test generator function 'g'.

    static void testCase7();
        // Test copy constructors.

    static void testCase7Move();
        // Test move constructor.

    static void testCase6();
        // Test equality operators ('operator==/!=').

    static void testCase6Negative();
        // Negative test for equality operators.

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators ('push_back' and 'clear').

    static void testCase1();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class TYPE, class TRAITS, class ALLOC>
int TestDriver<TYPE,TRAITS,ALLOC>::getValues(const TYPE **values)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    const int NUM_VALUES = 12;
    static const TYPE initValues[NUM_VALUES] = {
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
        TYPE(VL)
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
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

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

            {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::
                             testCase9_propagate_on_container_copy_assignment()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_copy_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_copy_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_copy_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_copy_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Copy-assign 'Y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_copy_assignment
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<TYPE, TRAITS, ALLOC>::
                    testCase9_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                       StdAlloc;

    typedef bsl::basic_string<TYPE, TRAITS, StdAlloc>  Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

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
            const char *const JSPEC = SPECS[tj];

            {
                Obj mY(ISPEC, mas);  const Obj& Y = mY;

                Obj mX(JSPEC, mat);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(JSPEC) P_(Y) P_(X) P(W) }

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::
                             testCase9_propagate_on_container_move_assignment()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_move_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_move_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_move_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_move_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Move-assign 'y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_move_assignment
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == false'\n");

    testCase9_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == true'\n");

    testCase9_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1) The value represented by any instance can be assigned to any
    //      other instance regardless of how either value is represented
    //      internally.
    //   2) The 'rhs' value must not be affected by the operation.
    //   3) 'rhs' going out of scope has no effect on the value of 'lhs'
    //      after the assignment.
    //   4) Aliasing (x = x): The assignment operator must always work -- even
    //      when the lhs and rhs are identically the same object and when the
    //      object is being assigned the data it owns (x = x.c_str()).
    //   5) The assignment operator must be neutral with respect to memory
    //      allocation exceptions.
    //   6) The copy constructor's internal functionality varies
    //      according to which bitwise copy/move trait is applied.
    //   7) The assignment can be made from a 'StringRefData' without creating
    //      temporary string and without using the default allocator.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and
    //   varied differences, ordered by increasing length.  The set S must
    //   include objects with the following states:
    //   I) empty
    //   II) short string
    //   III) long string
    //   IV) dynamic string with short-string length
    //
    //   For each value in S, construct an object x along with a sequence of
    //   similarly constructed duplicates x1, x2, ..., xN.  Attempt to affect
    //   every aspect of white-box state by altering each xi in a unique way.
    //   Let the union of all such objects be the set T.
    //
    //   To address concerns 1, 2, and 5, construct tests u = v for all
    //   (u, v) in T X T.  Using canonical controls UU and VV, assert
    //   before the assignment that UU == u, VV == v, and v == u if and only if
    //   VV == UU.  After the assignment, assert that VV == u,
    //   UU == v if the allocators match or VV == v otherwise and, for grins,
    //   that v == u.  Let v go out of scope and confirm that VV == u.
    //   All of these tests are performed within the 'bslma' exception testing
    //   apparatus.  Since the execution time is lengthy with exceptions,
    //   every permutation is not performed when exceptions are tested.
    //   Every permutation is also tested separately without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests y =
    //   y, y = y.c_str(), and y = StringRefData(y.begin(), y.end()) for all y
    //   in T.  Using a canonical control X, we will verify that X == y before
    //   and after the assignment.  In addition, we test construct tests y =
    //   y.c_str() + OFFSET, y = StringRefData(y.begin() + OFFSET, y.end()),
    //   where OFFSET is either 1 or y.length() - 1, to test partial
    //   self-assignment for all y in T except empty string. We will verify
    //   that X == y before the assignment and X.data() + OFFSET == y after the
    //   assigment. All of the above is done within the bslma exception testing
    //   apparatus.
    //
    //   To address concern 6, all these tests are performed on user
    //   defined types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   basic_string& operator=(const basic_string& rhs);
    //   basic_string& operator=(const STRING_VIEW_LIKE_TYPE& rhs);
    //   basic_string& operator=(const CHAR_TYPE *s);
    //   basic_string& operator=(CHAR_TYPE c);
    // ------------------------------------------------------------------------

    typedef bslstl::StringRefImp<TYPE>           StringRefImp;
    typedef bsl::basic_string_view<TYPE, TRAITS> StringView;
    typedef ConvertibleToStringViewType<TYPE>    StringViewLikeType;

    bslma::TestAllocator         testAllocator(veryVeryVerbose);
    Allocator                    Z(&testAllocator);
    bslma::TestAllocator         defaultAllocator(veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    enum {
        ASSIGN_MODE_FIRST            = 0,
        ASSIGN_MODE_STRING           = 0,
        ASSIGN_MODE_STRING_REF       = 1,
        ASSIGN_MODE_STRING_VIEW      = 2,
        ASSIGN_MODE_STRING_VIEW_LIKE = 3,
        ASSIGN_MODE_LAST             = 3
    };

    // --------------------------------------------------------------------

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "Without Exceptions\n");
    {
        static const char *SPECS[] = {                      // length
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "DEABC",                                        // 5
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        for (int assignMode = ASSIGN_MODE_FIRST;
             assignMode <= ASSIGN_MODE_LAST;
             ++assignMode) {
            const int MODE    = assignMode;
            int       uOldLen = -1;

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC); // control

                    const bool EQUAL = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            Obj mU(Z);  const Obj& U = mU;
                            stretchRemoveAll(&mU, U_N, VALUES[0]);
                            gg(&mU, U_SPEC);
                            {
                    // v--------
                    Obj mV(Z);  const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const int    NUM_CTOR   = numCopyCtorCalls;
                    const int    NUM_DTOR   = numDestructorCalls;
                    const size_t OLD_LENGTH = U.size();
                    const Int64  TDA        =
                                             defaultAllocator.numAllocations();

                    switch (MODE) {
                      case ASSIGN_MODE_STRING: {
                        mU = V; // test assignment here
                      } break;
                      case ASSIGN_MODE_STRING_REF: {
                        const StringRefImp SR(V.data(), V.length());
                        mU = SR; // test assignment here
                      } break;
                      case ASSIGN_MODE_STRING_VIEW: {
                        const StringView SV(V.data(), V.length());
                        mU = SV; // test assignment here
                      } break;
                      case ASSIGN_MODE_STRING_VIEW_LIKE: {
                        const StringViewLikeType SVL(V.data());
                        mU = SVL; // test assignment here
                      } break;
                      default: {
                        ASSERTV(MODE, !"Bad assignment mode.");
                      } return;                                       // RETURN
                    }

                    const Int64 TDB = defaultAllocator.numAllocations();

                    ASSERTV(MODE, U_SPEC, V_SPEC, TDB - TDA, TDB == TDA);
                    ASSERT((numCopyCtorCalls - NUM_CTOR) <= (int)V.size());
                    ASSERT((numDestructorCalls - NUM_DTOR) <=
                                                 (int)(V.size() + OLD_LENGTH));

                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    // ---------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "With Exceptions\n");
    {
        static const char *SPECS[] = {
            // spec                              length
            // ----                              ------
            "",                                   // 0
            "A",                                 // 1
            "AB",                                // 2
            "ABC",                               // 3
            "ABCD",                              // 4
            "ABCDEABC",                          // 8
            "ABCDEABCD",                         // 9
            "ABCDEABCDEA",                       // 11
            "ABCDEABCDEAB",                      // 12
            "ABCDEABCDEABC",                     // 13
            "ABCDEABCDEABCDE",                   // 15
            "ABCDEABCDEABCDEABCDEABC",           // 23
            "ABCDEABCDEABCDEABCDEABCD",          // 24
            "ABCDEABCDEABCDEABCDEABCDE",         // 25
            "ABCDEABCDEABCDEABCDEABCDEABCDE",    // 30
            0
        }; // Null string required as last element.

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 9, 11, 12, 13,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int iterationModulus = 1;
        int iteration = 0;

        for (int assignMode = ASSIGN_MODE_FIRST;
             assignMode <= ASSIGN_MODE_LAST;
             ++assignMode) {
            const int MODE    = assignMode;
            int       uOldLen = -1;

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      U_LEN  = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", U_LEN);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)U_LEN);
                uOldLen = static_cast<int>(U_LEN);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, U_LEN == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      V_LEN   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               V_LEN);
                        P(V_SPEC);
                    }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                    //--------------^
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(Z);  const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);

                        const Int64 TDA = defaultAllocator.numAllocations();

                        switch (MODE) {
                          case ASSIGN_MODE_STRING: {
                            mU = V; // test assignment here
                          } break;
                          case ASSIGN_MODE_STRING_REF: {
                            const StringRefImp SR(V.data(), V.length());
                            mU = SR; // test assignment here
                          } break;
                          case ASSIGN_MODE_STRING_VIEW: {
                            const StringView SV(V.data(), V.length());
                            mU = SV; // test assignment here
                          } break;
                          case ASSIGN_MODE_STRING_VIEW_LIKE: {
                            const StringViewLikeType SVL(V.data());
                            mU = SVL; // test assignment here
                          } break;
                          default: {
                            ASSERTV(MODE, !"Bad assignment mode.");
                          } return;                                   // RETURN
                        }
                        guard.release();

                        const Int64 TDB = defaultAllocator.numAllocations();

                        ASSERTV(MODE, U_SPEC, V_SPEC, TDB - TDA, TDB == TDA);

                        ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                        ASSERTV(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    //--------------v
                                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting self assignment (Aliasing).\n");
    {
        enum {
            SELF_ASSIGN_MODE_FIRST     = 0,
            SELF_ASSIGN_STRING         = 0,
            SELF_ASSIGN_CSTRING        = 1,
            SELF_ASSIGN_STRINGREF      = 2,
            SELF_ASSIGN_STRINGVIEW     = 3,
            SELF_ASSIGN_STRINGVIEWLIKE = 4,
            SELF_ASSIGN_MODE_LAST      = 4
        };

        enum {
            PARTIAL_SELF_ASSIGN_MODE_FIRST          = 0,
            PARTIAL_SELF_ASSIGN_MODE_CSTRING        = 0,
            PARTIAL_SELF_ASSIGN_MODE_STRINGREF      = 1,
            PARTIAL_SELF_ASSIGN_MODE_STRINGVIEW     = 2,
            PARTIAL_SELF_ASSIGN_MODE_STRINGVIEWLIKE = 3,
            PARTIAL_SELF_ASSIGN_MODE_LAST           = 3
        };

        enum {
            PARTIAL_SELF_ASSIGN_CONSTRUCT_FIRST      = 0,
            PARTIAL_SELF_ASSIGN_CONSTRUCT_LONG_TAIL  = 0,
            PARTIAL_SELF_ASSIGN_CONSTRUCT_SHORT_TAIL = 1,
            PARTIAL_SELF_ASSIGN_CONSTRUCT_LAST       = 1
        };

        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int curLen = (int) strlen(SPEC);

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            const Obj X = g(SPEC);
            LOOP_ASSERT(ti, curLen == (int)X.size());  // same lengths

            for (int assignMode = SELF_ASSIGN_MODE_FIRST;
                     assignMode <= SELF_ASSIGN_MODE_LAST;
                     ++assignMode)
            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(Z);  const Obj& Y = mY;
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP3_ASSERT(SPEC, N, assignMode, Y == Y);
                    LOOP3_ASSERT(SPEC, N, assignMode, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {
                        ExceptionGuard<Obj> guard(Y, L_);
                        switch (assignMode) {
                          case SELF_ASSIGN_STRING: {
                            mY = Y;
                          } break;
                          case SELF_ASSIGN_CSTRING: {
                            mY = Y.c_str();
                          } break;
                          case SELF_ASSIGN_STRINGREF: {
                            const StringRefImp SR(Y.data(), Y.length());
                            mY = SR;
                          } break;
                          case SELF_ASSIGN_STRINGVIEW: {
                            const StringView SV(Y.data(), Y.length());
                            mY = SV;
                          } break;
                          case SELF_ASSIGN_STRINGVIEWLIKE: {
                            const StringViewLikeType SVL(Y.data());
                            mY = SVL;
                          } break;
                          default: {
                            printf("***UNKNOWN SELF_ASSIGN MODE***\n");
                            ASSERT(0);
                          } break;
                        }
                    }

                    ASSERTV(SPEC, N, assignMode, Y == Y);
                    ASSERTV(SPEC, N, assignMode, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }

            // Partial self assigment
            if (X.empty()) {
                continue;
            }

            if (veryVerbose) {
                printf("\nTesting partial self assignment (Aliasing).\n");
            }

            for (int assignMode = PARTIAL_SELF_ASSIGN_MODE_FIRST;
                     assignMode <= PARTIAL_SELF_ASSIGN_MODE_LAST;
                     ++assignMode)
            for (int constructMode = PARTIAL_SELF_ASSIGN_CONSTRUCT_FIRST;
                     constructMode <= PARTIAL_SELF_ASSIGN_CONSTRUCT_LAST;
                     ++constructMode)
            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(Z);  const Obj& Y = mY;
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP4_ASSERT(SPEC, N, assignMode, constructMode, Y == Y);
                    LOOP4_ASSERT(SPEC, N, assignMode, constructMode, X == Y);

                    size_t OFFSET = 0;
                    switch (constructMode) {
                      case PARTIAL_SELF_ASSIGN_CONSTRUCT_LONG_TAIL: {
                        OFFSET = 1;
                      } break;
                      case PARTIAL_SELF_ASSIGN_CONSTRUCT_SHORT_TAIL: {
                        OFFSET = Y.length() - 1;
                      } break;
                      default: {
                        printf("***UNKNOWN "
                                    "PARTIAL_SELF_ASSIGN_CONSTRUCT MODE***\n");
                        ASSERT(0);
                      } break;
                    }

                    testAllocator.setAllocationLimit(AL);
                    {
                        ExceptionGuard<Obj> guard(Y, L_);
                        switch (assignMode) {
                          case PARTIAL_SELF_ASSIGN_MODE_CSTRING: {
                            mY = Y.c_str() + OFFSET;
                          } break;
                          case PARTIAL_SELF_ASSIGN_MODE_STRINGREF: {
                            const StringRefImp SR(Y.data()   + OFFSET,
                                                  Y.length() - OFFSET);
                            mY = SR;
                          } break;
                          case PARTIAL_SELF_ASSIGN_MODE_STRINGVIEW: {
                            const StringView SV(Y.data()   + OFFSET,
                                                Y.length() - OFFSET);
                            mY = SV;
                          } break;
                          case PARTIAL_SELF_ASSIGN_MODE_STRINGVIEWLIKE: {
                            const StringViewLikeType SVL(Y.data() + OFFSET);
                            mY = SVL;
                          } break;
                          default: {
                            printf("***UNKNOWN PARTIAL_SELF_ASSIGN MODE***\n");
                            ASSERT(0);
                          } break;
                        }
                        guard.release();
                    }

                    LOOP4_ASSERT(SPEC, N, assignMode, constructMode, Y == Y);
                    LOOP4_ASSERT(SPEC, N, assignMode, constructMode,
                                                      X.begin() + OFFSET == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assignment exception-safety.\n");
#else
    if (verbose) printf("\nTesting assignment exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        Obj src(defaultCapacity + 1, '1', &testAllocator);

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        // make the allocator throw on the next allocation
        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();
        testAllocator.setAllocationLimit(0);

        bool exceptionCaught = false;

        try
        {
            // the assignment will require to allocate more memory
            dst = src;
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

        exceptionCaught = false;
        testAllocator.setAllocationLimit(0);

        try
        {
            const StringRefImp SR(src.data(), src.length());

            // the assignment will require to allocate more memory
            dst = SR;
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

        exceptionCaught = false;
        testAllocator.setAllocationLimit(0);

        try
        {
            const StringView SV(src.data(), src.length());

            // the assignment will require to allocate more memory
            dst = SV;
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

        exceptionCaught = false;
        testAllocator.setAllocationLimit(0);

        try
        {
            const StringViewLikeType SVL(src.data());

            // the assignment will require to allocate more memory
            dst = SVL;
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

        // restore the allocator state
        testAllocator.setAllocationLimit(oldLimit);
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9Move()
{
    // --------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1) The value represented by any object can be assigned to any other
    //      object regardless of how either value is represented internally.
    //   2) The 'rhs' value is left usable after the move.
    //   3) 'rhs' going out of scope has no effect on the value of 'lhs' after
    //      the assignment.
    //   4) Aliasing ('x = x'): The assignment operator must always work --
    //      even when the lhs and rhs are identically the same object.
    //   5) The assignment operator must be neutral with respect to memory
    //      allocation exceptions.
    //   6) The move constructor's internal functionality varies according to
    //      which bitwise copy/move trait is applied.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and varied
    //   differences, ordered by increasing length.  For each value in S,
    //   construct an object x along with a sequence of similarly constructed
    //   duplicates x1, x2, ..., xN.  Attempt to affect every aspect of
    //   white-box state by altering each xi in a unique way.  Let the union of
    //   all such objects be the set T.
    //
    //   To address concerns 1, 2, and 5, construct tests u = v for all (u, v)
    //   in T X T.  Using canonical controls UU and VV, assert before the
    //   assignment that UU == u, VV == v, and v == u if and only if VV == UU.
    //   After the assignment, assert that VV == u, VV == v, and, for grins,
    //   that v == u.  Let v go out of scope and confirm that VV == u.  All of
    //   these tests are performed within the 'bslma' exception testing
    //   apparatus.  Since the execution time is lengthy with exceptions, every
    //   permutation is not performed when exceptions are tested.  Every
    //   permutation is also tested separately without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests y = y
    //   for all y in T.  Using a canonical control X, we will verify that
    //   X == y before and after the assignment, again within the bslma
    //   exception testing apparatus.
    //
    //   To address concern 6, all these tests are performed on user defined
    //   types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   basic_string& operator=(MovableRef<basic_string> rhs);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    // --------------------------------------------------------------------

    static const char *SPECS[] = {                      // length
        "",                                             // 0
        "A",                                            // 1
        "BC",                                           // 2
        "CDE",                                          // 3
        "DEAB",                                         // 4
        "DEABC",                                        // 5
        "CBAEDCBA",                                     // 8
        "EDCBAEDCB",                                    // 9
        "EDCBAEDCBAE",                                  // 11
        "EDCBAEDCBAED",                                 // 12
        "EDCBAEDCBAEDC",                                // 13
        "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
        "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
        "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
        "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
        0 // null string required as last element
    };

    static const int EXTEND[] = {
        0, 1, 2, 3, 4, 5, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30, 63, 64, 65
    };
    enum { NUM_EXTEND = sizeof(EXTEND) / sizeof(*EXTEND) };

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "Without Exceptions\n");
    { // Testing Move with same allocators
        if (verbose) printf("\twith the same allocators.\n");
        int uOldLen = -1;
        for (int ui = 0; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
            uOldLen = static_cast<int>(uLen);

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                const Obj VV = g(V_SPEC); // control

                const bool EQUAL = ui == vi; // flag indicating same values

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];
                    for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                        const int V_N = EXTEND[vj];

                        Obj mU(Z);  const Obj& U = mU;
                        stretchRemoveAll(&mU, U_N, VALUES[0]);
                        gg(&mU, U_SPEC);
                        {
                    // v--------
                    Obj mV(Z);  const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;

                    Obj& result = (mU = MoveUtil::move(mV));
                        // test move assignment here

                    ASSERT(&result == &U);

                    ASSERT((numCopyCtorCalls - NUM_CTOR) == 0);
                    ASSERT((numDestructorCalls - NUM_DTOR) == 0);

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,
                                 V.empty() || V == VV);
                    // ---------v
                        }
                        // 'mV' (and therefore 'V') now out of scope
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    }
                }
            }
        }
    }
    { // Testing Move with different allocators
        if (verbose) printf("\twith different allocators.\n");
        int uOldLen = -1;
        for (int ui = 0; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
            uOldLen = static_cast<int>(uLen);

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                const Obj VV = g(V_SPEC); // control

                const bool EQUAL = ui == vi; // flag indicating same values

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];
                    for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                        const int V_N = EXTEND[vj];

                        Obj mU; const Obj& U = mU;
                        stretchRemoveAll(&mU, U_N, VALUES[0]);
                        gg(&mU, U_SPEC);
                        {
                    // v--------
                    Obj mV(Z); const Obj& V = mV;
                    stretchRemoveAll(&mV, V_N, VALUES[0]);
                    gg(&mV, V_SPEC);

                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, EQUAL == (V == U));

                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;

                    // test move assignment here
                    mU = MoveUtil::move(mV);

                    ASSERT((numCopyCtorCalls - NUM_CTOR) == 0);
                    ASSERT((numDestructorCalls - NUM_DTOR) == 0);

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    // ---------v
                        }
                        // 'mV' (and therefore 'V') now out of scope
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    }
                }
            }
        }
    }

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "With Exceptions\n");
    {
        static const char *SPECS[] = {
            // spec                              length
            // ----                              ------
            "",                                   // 0
            "A",                                 // 1
            "AB",                                // 2
            "ABC",                               // 3
            "ABCD",                              // 4
            "ABCDEABC",                          // 8
            "ABCDEABCD",                         // 9
            "ABCDEABCDEA",                       // 11
            "ABCDEABCDEAB",                      // 12
            "ABCDEABCDEABC",                     // 13
            "ABCDEABCDEABCDE",                   // 15
            "ABCDEABCDEABCDEABCDEABC",           // 23
            "ABCDEABCDEABCDEABCDEABCD",          // 24
            "ABCDEABCDEABCDEABCDEABCDE",         // 25
            "ABCDEABCDEABCDEABCDEABCDEABCDE",    // 30
            0
        }; // Null string required as last element.

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 9, 11, 12, 13,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int iterationModulus = 1;
        int iteration = 0;
        {
            if (verbose) printf("\tWith the same allocators.\n");
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",vLen);
                            P(V_SPEC);
                        }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                //--------------^
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(Z); const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);
                        // test move assignment here
                        mU = MoveUtil::move(mV);
                        guard.release();

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,
                                     V.empty() || VV == V);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                //--------------v
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }

        iterationModulus = 1;
        iteration = 0;
        {
            if (verbose) printf("\tWith different allocators.\n");
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < (int)uLen);
                uOldLen = static_cast<int>(uLen);

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",vLen);
                            P(V_SPEC);
                        }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                //--------------^
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(Z);  const Obj& U = mU;
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    gg(&mU, U_SPEC);
                    {
                        Obj mV; const Obj& V = mV;
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ExceptionGuard<Obj> guard(U, L_);
                        testAllocator.setAllocationLimit(AL);
                        // test move assignment here
                        mU = MoveUtil::move(mV);
                        guard.release();

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                //--------------v
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting self assignment (Aliasing).\n");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int curLen = (int) strlen(SPEC);

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            const Obj X = g(SPEC);
            LOOP_ASSERT(ti, curLen == (int)X.size());  // same lengths

            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(Z);  const Obj& Y = mY;
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {  // test move assignment here
                        ExceptionGuard<Obj> guard(Y, L_);
                        mY = MoveUtil::move(mY);
                    }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
#ifndef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nSkip testing assignment exception-safety.\n");
#else
    if (verbose) printf("\nTesting assignment exception-safety.\n");

    {
        size_t defaultCapacity = Obj().capacity();

        // 'src' and 'dst' must have different allocators for the exception to
        // be thrown.
        Obj src(defaultCapacity + 1, '1');

        Obj dst(defaultCapacity, '2', &testAllocator);
        Obj dstCopy(dst, &testAllocator);

        // Make the allocator throw on the next allocation:
        bsls::Types::Int64 oldLimit = testAllocator.allocationLimit();
        testAllocator.setAllocationLimit(0);

        bool exceptionCaught = false;

        try
        {
            // the assignment will require to allocate more memory
            dst = MoveUtil::move(src);
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

        // Restore the allocator state.
        testAllocator.setAllocationLimit(oldLimit);

        ASSERT(exceptionCaught);
        ASSERT(dst == dstCopy);
    }
#endif
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase9Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1 Assigning a NULL C-string to a string object asserts.
    //
    // Plan:
    //   Construct a string object, then assign a NULL C-string to it and
    //   verify that it asserts.  After that assign a valid C-string and verify
    //   that it succeeds.
    //
    // Testing:
    //   basic_string& operator=(const CHAR_TYPE *);
    // --------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj X;

    if (verbose) printf("\tassigning a NULL C-string\n");

    {
        const TYPE *s = 0;
        (void) s; // to disable "unused variable" warning
        ASSERT_SAFE_FAIL(X = s);
    }

    if (verbose) printf("\tassigning a valid C-string\n");

    {
        Obj Y(g("ABC"));
        ASSERT_SAFE_PASS(X = Y.c_str());
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase8()
{
    // --------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //
    // Concerns:
    //   Since 'g' is implemented almost entirely using 'gg', we need to verify
    //   only that the arguments are properly forwarded, that 'g' does not
    //   affect the test allocator, and that 'g' returns an object by value.
    //
    // Plan:
    //   For each SPEC in a short list of specifications, compare the object
    //   returned (by value) from the generator function, 'g(SPEC)' with the
    //   value of a newly constructed OBJECT configured using 'gg(&OBJECT,
    //   SPEC)'.  Compare the results of calling the allocator's
    //   'numBlocksTotal' and 'numBytesInUse' methods before and after calling
    //   'g' in order to demonstrate that 'g' has no effect on the test
    //   allocator.  Finally, use 'sizeof' to confirm that the (temporary)
    //   returned by 'g' differs in size from that returned by 'gg'.
    //
    // Testing:
    //   string g(const char *spec);
    //   string g(size_t len, TYPE seed);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    static const char *SPECS[] = {
        "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0  // null string required as last element
    };

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; SPECS[ti]; ++ti) {
        const char *SPEC = SPECS[ti];
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(Z);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = "); debugprint(g(SPEC)); printf("\n");
            printf("\tgg = "); debugprint(X); printf("\n");
        }
        const Int64 TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const Int64 TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");

    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

              Obj  x(Z);
              Obj& r1 = gg(&x, SPEC);
              Obj& r2 = gg(&x, SPEC);
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&r2 == &r1);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);
    }

    if (verbose) printf("\nVerify generator g(size_t len, TYPE seed)\n");

    {
        if (veryVerbose) printf("  Returned string length is correct.\n");

        for (size_t i = 0; i < 200; ++i) {
            ASSERT(g(i, TYPE()).size() == i);
        }

        if (veryVerbose) printf("  Seed produces unique strings.\n");

        // scan the char range, wchar_t can be too wide (32bit)
        Obj  values[CHAR_MAX];
        Obj *values_end = values + CHAR_MAX;

        for (char s = 0; s != CHAR_MAX; ++s) {
            values[s] = g(10, TYPE(s));
        }

        // all values are unique
        std::sort(values, values_end);
        ASSERT(std::adjacent_find(values, values_end) == values_end);
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase7()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //
    // Concerns:
    //   1) copy constructor
    //       1a) The new object's value is the same as that of the original
    //           object (relying on the equality operator) and created with
    //           the correct capacity.
    //       1b) All internal representations of a given value can be used to
    //            create a new object of equivalent value.
    //       1c) The value of the original object is left unaffected.
    //       1d) Subsequent changes in or destruction of the source object have
    //            no effect on the copy-constructed object.
    //       1e) Subsequent changes ('push_back's) on the created object have
    //            no effect on the original and change the capacity of the new
    //            object correctly.
    //       1f) The object has its internal memory management system hooked up
    //            properly so that *all* internally allocated memory draws
    //            from a user-supplied allocator whenever one is specified.
    //       1g) The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the following
    //   tests.
    //
    //   For concerns a - d, for each value in S, initialize objects w and x,
    //   copy construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w or, in the case
    //   that x is moved-from, x is empty. Let x go out of scope and again
    //   verify that w == y.
    //
    //   For concern e, for each value in S initialize objects w and x, and
    //   copy construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w, and verify that the capacity of y changes
    //   correctly.
    //
    //   To address concern f, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 2 and 5.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern g, perform tests for concern 1 performed in the
    //   presence of exceptions during memory allocations using a
    //   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   string(const string<C,CT,A>& original);
    //   string(const string<C,CT,A>& original, const A& basicAllocator);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    {
        static const char *SPECS[] = {
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0  // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };

        enum { NUM_EXTEND = sizeof(EXTEND) / sizeof(*EXTEND) };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length " ZU ":\t", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object w.
            Obj mW; const Obj& W = mW;
            gg(&mW, SPEC);

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                Obj *pX = new Obj(Z);
                Obj& mX = *pX;

                stretchRemoveAll(&mX, N, VALUES[0]);
                const Obj& X = mX;  gg(&mX, SPEC);

                if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                {   // Testing concern 1a.

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Obj Y0(X);

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, W == X);
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());
                }
                {   // Testing concern 1e.

                    if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N + 1; ++i) {
                        const size_t oldCap = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        size_t newCap = 0 != remSlots
                                      ? -1
                                      : Imp::computeNewCapacity(LENGTH + i,
                                                                oldCap,
                                                                Y1.max_size());

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);

                        if (oldCap == 0) {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() ==
                                        INITIAL_CAPACITY_FOR_NON_EMPTY_OBJECT);
                        }
                        else if (remSlots == 0) {
                            LOOP5_ASSERT(SPEC, N, i, Y1.capacity(), newCap,
                                         Y1.capacity() == newCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == oldCap);
                        }
                    }
                }
                {   // Testing concern 1e with test allocator.

                    if (veryVerbose)
                        printf("\t\t\tInsert into created obj, "
                                "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(X, AllocType(&testAllocator));  // testing this

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    if (LENGTH <= DEFAULT_CAPACITY) {
                        // Should not allocate when the new string is short,
                        // even when the original string has allocated a buffer
                        // due to previous extension.
                        ASSERTV(SPEC, N, BB, AA, BB + 0 == AA);
                        ASSERTV(SPEC, N,  B,  A,  B + 0 ==  A);
                    }
                    else {
                        // Perform no more than one allocation for a long
                        // string.
                        ASSERTV(SPEC, N, BB, AA, BB + 1 == AA);
                        ASSERTV(SPEC, N,  B,  A,  B + 1 ==  A);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity()) {
                            if (oldCap == DEFAULT_CAPACITY) {
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                            }
                            else {
                                LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                                LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                            }
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                }
                {   // Exception checking.

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(X, AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case  :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                        LOOP2_ASSERT(SPEC, N,
                                     Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    const size_t initCap = DEFAULT_CAPACITY;

                    if (initCap < LENGTH) {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    }
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                }
                {                            // with 'original' destroyed
                    Obj Y5(X);
                    if (veryVerbose) {
                        printf("\t\t\tWith Original deleted: \n");
                        printf("\t\t\t\tBefore Delete : "); P(Y5);
                    }

                    delete pX;

                    LOOP2_ASSERT(SPEC, N, W == Y5);

                    for (int i = 1; i < N+1; ++i) {
                        stretch(&Y5, 1, VALUES[i % NUM_VALUES]);
                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append to new obj : ");
                            P_(i);P(Y5);
                        }
                        LOOP3_ASSERT(SPEC, N, i, W != Y5);
                    }
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase7Move()
{
    // --------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTORS:
    //
    // Concerns:
    //   1) move constructor
    //       1a) The new object's value is the same as that of the original
    //           object (relying on the equality operator) and created with
    //           the correct capacity.
    //       1b) All internal representations of a given value can be used to
    //            create a new object of equivalent value.
    //       1c) The value of the original object remains usable.
    //       1d) Subsequent changes in or destruction of the source object have
    //            no effect on the move-constructed object.
    //       1e) Subsequent changes ('push_back's) on the created object have
    //            no effect on the original and change the capacity of the new
    //            object correctly.
    //       1f) The object has its internal memory management system hooked up
    //            properly so that *all* internally allocated memory draws
    //            from a user-supplied allocator whenever one is specified.
    //       1g) The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the
    //   following tests.
    //
    //   For concerns a - d, for each value in S, initialize objects w and
    //   x, move construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w or, in the case
    //   that x is moved-from, x is empty. Let x go out of scope and again
    //   verify that w == y.
    //
    //   For concern e, for each value in S initialize objects w and x,
    //   and move construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w, and verify that the capacity of y
    //   changes correctly.
    //
    //   To address concern f, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 'a' and
    //       'e'.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern g, perform tests for concern 1 performed
    //   in the presence of exceptions during memory allocations using a
    //   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   string(MovableRef<string> original);
    //   string(MovableRef<string> original, ALLOC basicAllocator);
    // --------------------------------------------------------------------
    if (verbose) {
        printf("Testing Move Constructor\n");
    }

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    {
        static const char *SPECS[] = {
            "",                                             // 0
            "A",                                            // 1
            "BC",                                           // 2
            "CDE",                                          // 3
            "DEAB",                                         // 4
            "CBAEDCBA",                                     // 8
            "EDCBAEDCB",                                    // 9
            "EDCBAEDCBAE",                                  // 11
            "EDCBAEDCBAED",                                 // 12
            "EDCBAEDCBAEDC",                                // 13
            "EDCBAEDCBAEDCBAEDCBAEDC",                      // 23
            "EDCBAEDCBAEDCBAEDCBAEDCB",                     // 24
            "EDCBAEDCBAEDCBAEDCBAEDCBA",                    // 25
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBA",               // 30
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAE",              // 31
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAED",             // 32
            "EDCBAEDCBAEDCBAEDCBAEDCBAEDCBAEDC",            // 33
            0  // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9,
            DEFAULT_CAPACITY - 1,
            DEFAULT_CAPACITY,
            DEFAULT_CAPACITY + 1,
            DEFAULT_CAPACITY * 5
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length " ZU ":\t", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object w.
            Obj mW; gg(&mW, SPEC);
            const Obj& W = mW;

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                {   // Testing concern 1a with implicit allocator.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const bool XAllocated = X.capacity() != DEFAULT_CAPACITY;

                    if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Int64 B  = testAllocator.numBlocksTotal();
                    const Int64 BB = testAllocator.numBlocksInUse();

                    // Note that 'Obj Y0 = bslmf::...' will not compile on a
                    // strict C++03 compiler due to an ambiguity.  We do not
                    // add a separate C++11 only test for that syntax, as that
                    // would be testing the compiler, rather than the library.
                    Obj Y0(MoveUtil::move(mX));

                    const Int64 A  = testAllocator.numBlocksTotal();
                    const Int64 AA = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                                            X.get_allocator());
                    // If 'X' had a dynamically allocated string, check that
                    // we stole the string and left 'X' empty.  Otherwise,
                    // check that 'X' is unchanged.
                    if (XAllocated) {
                        LOOP2_ASSERT(SPEC, N, X.empty());
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, X == W);
                    }

                    ASSERTV(A,   B,  A == B);
                    ASSERTV(AA, BB, AA == BB);

                    // Testing concern 1d by modifying X.
                    stretch(&mX, 1, VALUES[ei % NUM_VALUES]);
                    LOOP2_ASSERT(SPEC, N, W == Y0);

                    delete pX;

                    // Testing concern 1d after deleting X.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                }
                {   // Testing concern 1a with specified different allocator.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    if (veryVerbose) { printf("\t\t\tRegular Case With "
                                                     "Different Allocators :");
                    }

                    const Int64 B  = testAllocator.numBlocksTotal();
                    const Int64 BB = testAllocator.numBlocksInUse();

                    Obj Y0(MoveUtil::move(mX), &testAllocator);

                    const Int64 A  = testAllocator.numBlocksTotal();
                    const Int64 AA = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N,
                                      Y0.get_allocator() != X.get_allocator());
                    LOOP2_ASSERT(SPEC, N,
                                         Y0.get_allocator() == &testAllocator);

                    // Check that 'X' is left unchanged because it does not
                    // have the same allocator as 'Y0'.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, W == X);
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                                               &testAllocator);
                    LOOP2_ASSERT(SPEC, N, LENGTH <= Y0.capacity());

                    if (!isShort) {
                        LOOP2_ASSERT(SPEC, N,  A == B  + 1);
                        LOOP2_ASSERT(SPEC, N, AA == BB + 1);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N,  A == B);
                        LOOP2_ASSERT(SPEC, N, AA == BB);
                    }

                    // Testing concern 1d by modifying X.
                    stretch(&mX, 1, VALUES[ei % NUM_VALUES]);
                    LOOP2_ASSERT(SPEC, N, W == Y0);

                    delete pX;

                    // Testing concern 1d after deleting X.
                    LOOP2_ASSERT(SPEC, N, W == Y0);
                }
                {   // Testing concern 1e.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    if (veryVerbose) printf("\t\t\tMove into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(MoveUtil::move(mX));
                    // Control object for X.
                    Obj X_ctrl(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N + 1; ++i) {
                        const size_t oldCap = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        size_t newCap = 0 != remSlots
                                      ? -1
                                      : Imp::computeNewCapacity(LENGTH + i,
                                                                oldCap,
                                                                Y1.max_size());

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X == X_ctrl);
                        if (remSlots == 0) {
                            LOOP5_ASSERT(SPEC, N, i, Y1.capacity(), newCap,
                                         Y1.capacity() == newCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == oldCap);
                        }
                    }

                    delete pX;
                }
                {   // Testing concern 1e with test allocator.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    if (veryVerbose)
                        printf("\t\t\tMove into created obj, "
                                                     "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(MoveUtil::move(mX),
                            AllocType(&testAllocator));
                    // Control object for X.
                    Obj X_ctrl(X);

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();
                        const size_t initCap = DEFAULT_CAPACITY;

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity() && oldCap == initCap) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                        }
                        else if (oldCap < Y11.capacity()) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                        } else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X == X_ctrl);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                    delete pX;
                }
                {   // Testing concern 1e with different allocator.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    if (veryVerbose)
                        printf("\t\t\t\tMove into created obj, "
                                "with test allocator:\n");

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(MoveUtil::move(mX), AllocType(&testAllocator));

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    if (isShort) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 1 ==  A);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y11.capacity();
                        const size_t initCap = DEFAULT_CAPACITY;

                        const Int64 CC = testAllocator.numBlocksTotal();
                        const Int64  C = testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const Int64 DD = testAllocator.numBlocksTotal();
                        const Int64  D = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore Append: ");  P_(DD); P(D);
                            printf("\t\t\t\tAfter Append : "); P_(CC); P(C);
                            T_ T_ T_ T_ T_ P_(i); P_(Y11.capacity()); P(Y11);
                        }

                        // Blocks allocated should increase only when trying to
                        // add more than capacity.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        if (oldCap < Y11.capacity() && oldCap == initCap) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 1  == D);
                        }
                        else if (oldCap < Y11.capacity()) {
                            LOOP3_ASSERT(SPEC, N, i, CC + 1 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0  == D);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i, CC + 0 == DD);
                            LOOP3_ASSERT(SPEC, N, i,  C + 0 ==  D);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X == W);
                        LOOP3_ASSERT(SPEC, N, i,
                                         Y11.get_allocator() ==&testAllocator);
                    }
                    delete pX;
                }
                {   // Exception checking with same allocators.
                    Obj *pX = new Obj(Z);
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    bool XAllocated = X.capacity() != DEFAULT_CAPACITY;

                    AllocType testA = &testAllocator;
                    ASSERT(X.get_allocator() == testA);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(MoveUtil::move(mX),
                                     AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case with same "
                                                             "allocators :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N,
                                     Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    if (XAllocated) {
                        LOOP2_ASSERT(SPEC, N,  B - 1 ==  A);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }

                    delete pX;
                }
                {   // Exception checking with different allocators.
                    Obj *pX = new Obj();
                    Obj& mX = *pX;
                    stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  gg(&mX, SPEC);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64  B = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    bool isShort = X.size() <= DEFAULT_CAPACITY;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(MoveUtil::move(mX),
                                     AllocType(&testAllocator));
                        if (veryVerbose) {
                            printf("\t\t\tException Case with "
                                                   "different allocators :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N,
                                          Y2.get_allocator() ==&testAllocator);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64  A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    if (isShort) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                    }
                    else {
                        LOOP2_ASSERT(SPEC, N, BB + 1 == AA);
                    }
                    LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);

                    delete pX;
                }
            }
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    //
    // Concerns:
    //   1 Objects constructed with the same values are returned as equal.
    //   2 Objects constructed such that they have same (logical) value but
    //     different internal representation (due to the lack or presence
    //     of an allocator, and/or different capacities) are always returned
    //     as equal.
    //   3 Unequal objects are always returned as unequal.
    //   4 Equality comparisons with 'const CHAR_TYPE *' yield the same results
    //     as equality comparisons with 'string' objects.
    //   5 Equality comparisons with 'std::string' yield the same results as
    //     equality comparisons with 'string' objects.
    //   6 Correctly selects the 'bitwiseEqualityComparable' traits.
    //
    // Plan:
    //   For concerns 1 and 3, Specify a set A of unique allocators including
    //   no allocator.  Specify a set S of unique object values having various
    //   minor or subtle differences, ordered by non-decreasing length.
    //   Verify the correctness of 'operator==' and 'operator!=' (returning
    //   either true or false) using all elements (u, ua, v, va) of the
    //   cross product S X A X S X A.
    //
    //   For concern 2 create two objects using all elements in S one at a
    //   time.  For the second object change its internal representation by
    //   extending it by different amounts in the set E, followed by erasing
    //   its contents using 'clear'.  Then recreate the original value and
    //   verify that the second object still return equal to the first.
    //
    //   For concern 4, test equality operators taking 'const CHAR_TYPE *'
    //   parameters right after equality comparisons of 'string' objects have
    //   been verified to perform correctly.
    //
    //   For concern 5, test equality operators taking 'std::string' parameters
    //   right after equality comparisons of 'string' objects have been
    //   verified to perform correctly.
    //
    //   For concern 6, we instantiate this test driver on a test type having
    //   allocators or not, and possessing the bitwise-equality-comparable
    //   trait or not.
    //
    // Testing:
    //   operator==(const string<C,CT,A>&, const string<C,CT,A>&);
    //   operator==(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
    //   operator==(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
    //   operator==(const C *, const string<C,CT,A>&);
    //   operator==(const string<C,CT,A>&, const C *);
    //   operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
    //   operator!=(const string<C,CT,A1>&, const std::string<C,CT,A2>&);
    //   operator!=(const std::string<C,CT,A1>&, const string<C,CT,A2>&);
    //   operator!=(const C *, const string<C,CT,A>&);
    //   operator!=(const string<C,CT,A>&, const C *);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    Allocator *AllocType[] = {
        new Allocator(&testAllocator1),
        new Allocator(&testAllocator2)
    };

    const int NUM_AllocType = sizeof AllocType / sizeof *AllocType;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const char *SPECS[] = {
        "",
        "A",      "B",
        "AA",     "AB",     "BB",     "BA",
        "AAA",    "BAA",    "ABA",    "AAB",
        "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
        "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
        "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
        "AAAAAAA",          "BAAAAAA",          "AAAAABA",
        "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
        "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
        "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
        "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
        "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
        "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
        "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA",
        0  // null string required as last element
    };

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_AllocType; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(*AllocType[ai]); const Obj& U = gg(&mU, U_SPEC);
                LOOP2_ASSERT(si, ai, LENGTH == static_cast<int>(U.size()));
                                                                // same lengths

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n", LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);//non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) { T_; T_;
                    P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_AllocType; ++aj) {

                        const char *const V_SPEC = SPECS[sj];
                        Obj mV(*AllocType[aj]);
                        const Obj& V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            T_; T_; P_(sj); P_(V_SPEC); P(V);
                        }

                        // First test comparisons with 'string'
                        const bool isSame = si == sj;
                        LOOP2_ASSERT(si, sj,  isSame == (U == V));
                        LOOP2_ASSERT(si, sj, !isSame == (U != V));

                        // And then with 'const CHAR_TYPE *'
                        LOOP2_ASSERT(si, sj,  isSame == (U == V.c_str()));
                        LOOP2_ASSERT(si, sj, !isSame == (U != V.c_str()));

                        LOOP2_ASSERT(si, sj,  isSame == (U.c_str() == V));
                        LOOP2_ASSERT(si, sj, !isSame == (U.c_str() != V));

                        // Finally test comparisons with 'std::string'
                        const std::basic_string<TYPE,TRAITS> stdU(U), stdV(V);
                        LOOP2_ASSERT(si, sj,  isSame == (U == stdV));
                        LOOP2_ASSERT(si, sj, !isSame == (U != stdV));

                        LOOP2_ASSERT(si, sj,  isSame == (stdU == V));
                        LOOP2_ASSERT(si, sj, !isSame == (stdU != V));
                    }
                }
            }
        }
    }

    if (verbose) printf("\nCompare each pair of similar values (u, ua, v, va)"
                        " in S X A X S X A after perturbing.\n");
    {
        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 11, 12, 13, 15, 23, 24, 25, 30
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_AllocType; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const size_t    LENGTH = strlen(U_SPEC);

                Obj mU(*AllocType[ai]); const Obj& U = mU;
                gg(&mU, U_SPEC);
                LOOP_ASSERT(si, LENGTH == U.size());  // same lengths

                if ((int)LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length " ZU ".\n",
                                LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= (int)LENGTH);
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_AllocType; ++aj) {
                        //Perform perturbation
                        for (int e = 0; e < NUM_EXTEND; ++e) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(*AllocType[aj]); const Obj& V = mV;
                            gg(&mV, V_SPEC);

                            stretchRemoveAll(&mV, EXTEND[e],
                                             VALUES[e % NUM_VALUES]);
                            gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                T_; T_; P_(sj); P_(V_SPEC); P(V);
                            }

                            // First test comparisons with 'string'
                            const bool isSame = si == sj;
                            LOOP2_ASSERT(si, sj,  isSame == (U == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V));

                            // And then with 'const CHAR_TYPE *'
                            LOOP2_ASSERT(si, sj,  isSame == (U == V.c_str()));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V.c_str()));

                            LOOP2_ASSERT(si, sj,  isSame == (U.c_str() == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U.c_str() != V));

                            // Finally test comparisons with 'std::string'
                            const std::basic_string<TYPE,TRAITS> stdU(U),
                                                                 stdV(V);
                            LOOP2_ASSERT(si, sj,  isSame == (U == stdV));
                            LOOP2_ASSERT(si, sj, !isSame == (U != stdV));

                            LOOP2_ASSERT(si, sj,  isSame == (stdU == V));
                            LOOP2_ASSERT(si, sj, !isSame == (stdU != V));
                        }
                    }
                }
            }
        }
    }

    delete AllocType[0];
    delete AllocType[1];
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase6Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING EQUALITY OPERATORS
    //
    // Concerns:
    //   Equality comparison operators taking C-string pointer parameters
    //   assert on undefined behavior when the pointer is NULL.
    //
    // Plan:
    //   For each equality comparison operator, create a non-empty string and
    //   use it to test equality comparison operators by passing NULL to
    //   C-string pointer parameters.
    //
    // Testing:
    //   operator==(const C *s, const string<C,CT,A>& str);
    //   operator==(const string<C,CT,A>& str, const C *s);
    //   operator!=(const C *s, const string<C,CT,A>& str);
    //   operator!=(const string<C,CT,A>& str, const C *s);
    // -----------------------------------------------------------------------

    bsls::AssertTestHandlerGuard guard;

    Obj mX(g("ABCDE"));
    const Obj& X = mX;

    TYPE *nullStr = NULL;
    // disable "unused variable" warning in non-safe mode:
    (void) nullStr;

    if (veryVerbose) printf("\toperator==(s, str)\n");

    {
        ASSERT_SAFE_FAIL(if(nullStr == X){});
        ASSERT_SAFE_PASS(if(X.c_str() == X){});
    }

    if (veryVerbose) printf("\toperator==(str, s)\n");

    {
        ASSERT_SAFE_FAIL(if(X == nullStr){});
        ASSERT_SAFE_PASS(if(X == X.c_str()){});
    }

    if (veryVerbose) printf("\toperator!=(s, str)\n");

    {
        ASSERT_SAFE_FAIL(if(nullStr != X){});
        ASSERT_SAFE_PASS(if(X.c_str() != X){});
    }

    if (veryVerbose) printf("\toperator==(str, s)\n");

    {
        ASSERT_SAFE_FAIL(if(X != nullStr){});
        ASSERT_SAFE_PASS(if(X != X.c_str()){});
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase4()
{
    // --------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    //
    // Concerns:
    //   1) The returned value for operator[] and function at() is correct
    //      as long as pos < size(), and 'operator[] const' returns C() for
    //      'pos == size()'.
    //   2) The at() function throws out_of_range exception if
    //      pos >= size().
    //   3) The elements stored at indices 0 up to size() - 1 are stored
    //      contiguously.
    //   4) Changing the internal representation to get the same (logical)
    //      final value, should not change the result of the element
    //      accessor functions.
    //   5) The internal memory management is correctly hooked up so that
    //      changes made to the state of the object via these accessors
    //      do change the state of the object.
    //   6) The 'data' method's signature and return type are standard.
    //   7) The 'c_str' method's signature and return type are standard.
    //   8) The 'data' and the 'c_str' methods return the address of a
    //      buffer containing all the characters of this string object.
    //   9) The 'data' and the 'c_str' methods return the address of a
    //      buffer containing a null character at the last (equal to the length
    //      of the object) position, even for an empty object.
    //
    // Plan:
    //   For 1 and 4 do the following:
    //   Specify a set S of representative object values ordered by
    //   increasing length.  For each value w in S, initialize a newly
    //   constructed object x with w using 'gg' and verify that each basic
    //   accessor returns the expected result.  Reinitialize and repeat
    //   the same test on an existing object y after perturbing y so as to
    //   achieve an internal state representation of w that is potentially
    //   different from that of x.

    //   For 3, using the object values of the previous experiment, verify that
    //   the array of 'size()' beginning at '&X[0]' is identical in contents to
    //   the object value.
    //
    //   For 2, check that function at() throws a out_of_range exception
    //   when pos >= size().
    //
    //   For 5, For each value w in S, create a object x with w using
    //   'gg'.  Create another empty object y and make it 'resize' capacity
    //   equal to the size of x.  Now using the element accessor functions
    //   recreate the value of x in y.  Verify that x == y.
    //   Note - Using untested resize(int).
    //
    //   For 6 and 7, use the addresses of 'data' and 'c_str' to initialize
    //   member-function pointers having the appropriate signature and
    //   return type.
    //
    //   For 8 and 9, iterate through the buffers and compare their values with
    //   the symbols, returned by the already tested 'operator[]'.  Verify,
    //   that the null character encloses these sequences.
    //
    // Testing:
    //   size_type size() const;
    //   reference operator[](size_type pos);
    //   const_reference operator[](size_type pos) const;
    //   reference at(size_type pos);
    //   const_reference at(size_type pos) const;
    //   const CHAR_TYPE *c_str() const;
    //   const CHAR_TYPE *data() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    Allocator *AllocType[] = {
        new Allocator(&testAllocator),
        new Allocator(&testAllocator1),
        new Allocator(&testAllocator2)
    };

    enum { NUM_AllocType = sizeof AllocType / sizeof *AllocType };

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const size_t MAX_LENGTH = 32;

    static const struct {
        int         d_lineNum;                   // source line number
        const char *d_spec_p;                    // specification string
        int         d_length;                    // expected length
        char        d_elements[MAX_LENGTH + 1];  // expected element values
    } DATA[] = {
        //line  spec            length  elements
        //----  --------------  ------  ------------------------
        { L_,   "",                  0, { }                     },
        { L_,   "A",                 1, { VA }                  },
        { L_,   "B",                 1, { VB }                  },
        { L_,   "AB",                2, { VA, VB }              },
        { L_,   "BC",                2, { VB, VC }              },
        { L_,   "BCA",               3, { VB, VC, VA }          },
        { L_,   "CAB",               3, { VC, VA, VB }          },
        { L_,   "CDAB",              4, { VC, VD, VA, VB }      },
        { L_,   "DABC",              4, { VD, VA, VB, VC }      },
        { L_,   "ABCDE",             5, { VA, VB, VC, VD, VE }  },
        { L_,   "EDCBA",             5, { VE, VD, VC, VB, VA }  },
        { L_,   "ABCDEA",            6, { VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "ABCDEAB",           7, { VA, VB, VC, VD, VE,
                                          VA, VB }              },
        { L_,   "BACDEABC",          8, { VB, VA, VC, VD, VE,
                                          VA, VB, VC }          },
        { L_,   "CBADEABCD",         9, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD }      },
        { L_,   "CBADEABCDAB",      11, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABC",     12, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC }              },
        { L_,   "CBADEABCDABCDE",   14, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE }      },
        { L_,   "CBADEABCDABCDEA",  15, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA }  },
        { L_,   "CBADEABCDABCDEAB", 16, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA", 31,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB", 32,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA, VB }              }
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting operator[] and function at(),"
                        " where pos < size().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;

            Obj mExp;
            const Obj& EXP = gg(&mExp, e);   // expected spec

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                Obj mX(*AllocType[ai]);

                const Obj& X = gg(&mX, SPEC);    // canonical organization

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf( "\ton objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                size_t i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX.at(i));
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X.at(i));

                    LOOP3_ASSERT(LINE, ai, i, &mX[0] + i == &mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, &X[0] + i == &X[i]);
                }
                LOOP2_ASSERT(LINE, ai, &mX[0] == &X[0]);
                LOOP2_ASSERT(LINE, ai, TYPE() == X[LENGTH]);

                for (; i < MAX_LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, 0 == e[i]);
                }

                // Check for perturbation.
                static const int EXTEND[] = {
                    0, 1, 2, 3, 4, 5, 7, 8, 9,
                    DEFAULT_CAPACITY - 1,
                    DEFAULT_CAPACITY,
                    DEFAULT_CAPACITY + 1,
                    DEFAULT_CAPACITY * 5
                };

                enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

                Obj mY(*AllocType[ai]);

                const Obj& Y = gg(&mY, SPEC);

                {                             // Perform the perturbation
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                        stretchRemoveAll(&mY, EXTEND[ei],
                                         VALUES[ei % NUM_VALUES]);
                        gg(&mY, SPEC);

                        if (veryVerbose) { T_; T_; T_; P(Y); }

                        size_t j;
                        for (j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY.at(j));
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y.at(j));
                        }

                        for (; j < MAX_LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, 0 == e[j]);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting non-'const' 'operator[]' and "
                        "function 'at()' modify state of object correctly.\n");
    {

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                Obj mX(*AllocType[ai]);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(LINE, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf("\tOn objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(*AllocType[ai]); const Obj& Y = mY;
                Obj mZ(*AllocType[ai]); const Obj& Z = mZ;

                mY.resize(LENGTH);
                mZ.resize(LENGTH);

                // When '0 == LENGTH', all have the default value.  Otherwise,
                // we should observe a change of value after the assignments.

                LOOP2_ASSERT(LINE, ai, !LENGTH || Y != X);
                LOOP2_ASSERT(LINE, ai, !LENGTH || Z != X);

                // Change state of Y and Z so its same as X

                for (size_t j = 0; j < LENGTH; j++) {
                    mY[j]    = TYPE(e[j]);
                    mZ.at(j) = TYPE(e[j]);
                }

                if (veryVerbose) {
                    printf("\t\tNew object1: "); P(Y);
                    printf("\t\tNew object2: "); P(Z);
                }

                LOOP2_ASSERT(LINE, ai, Y == X);
                LOOP2_ASSERT(LINE, ai, Z == X);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tTesting for out_of_range exceptions thrown"
                        " by at() when pos >= size().\n");
    {

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE    = DATA[ti].d_lineNum;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const size_t      LENGTH  = DATA[ti].d_length;

            for (int ai = 0; ai < NUM_AllocType; ++ai) {
                int exceptions, trials;

                const int NUM_TRIALS = 2;

                // Check exception behavior for non-'const' version of 'at()'
                // Checking the behavior for 'pos == size()' and
                // 'pos > size()'.

                for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                               ; ++trials) {
                    try {
                        Obj mX(*AllocType[ai]);
                        gg(&mX, SPEC);
                        mX.at(LENGTH + trials);
                    } catch (const std::out_of_range&) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception.\n");
                            P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);

                // Check exception behavior for const version of at()
                for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                               ; ++trials) {

                    try {
                        Obj mX(*AllocType[ai]);
                        const Obj& X = gg(&mX, SPEC);
                        X.at(LENGTH + trials);
                    } catch (const std::out_of_range&) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception." );
                            P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);
            }
        }
    }
#endif // BDE_BUILD_TARGET_EXC

    if (verbose) printf("\nTesting 'c_str()' and 'data()'.\n");
    {
        typedef const TYPE *(Obj::*MemberFunction)() const;

        MemberFunction dataMemberFunction = &Obj::data;
        MemberFunction cStrMemberFunction = &Obj::c_str;
        (void)&dataMemberFunction;
        (void)&cStrMemberFunction;
    }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);

            const TYPE   *cStrPtr = X.c_str();
            const TYPE   *dataPtr = X.data();
            const size_t  LENGTH  = X.length();

            ASSERT(dataPtr == bsl::data(X));
            for (size_t i = 0; i < LENGTH; ++i) {
                LOOP3_ASSERT(LINE, X[i], *(cStrPtr + i),
                             X[i] == *(cStrPtr + i));
                LOOP3_ASSERT(LINE, X[i], *(dataPtr + i),
                             X[i] == *(dataPtr + i));
            }

            LOOP_ASSERT(LINE, 0 == *(cStrPtr + LENGTH));
            LOOP_ASSERT(LINE, 0 == *(dataPtr + LENGTH));
        }
    }

    delete AllocType[0];
    delete AllocType[1];
    delete AllocType[2];
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase3()
{
    // --------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg':
    //
    // Concerns:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions, we want to verify (1) that valid
    //   generator syntax produces expected results and (2) that invalid
    //   syntax is detected and reported.
    //
    // Plan:
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'gg' to set the state of a newly created object.  Verify that 'gg'
    //   returns a valid reference to the modified argument object and,
    //   using basic accessors, that the value of the object is as
    //   expected.  Repeat the test for a longer 'spec' generated by
    //   prepending a string ending in a '~' character (denoting
    //   'clear').  Note that we are testing the parser only; the
    //   primary manipulators are already assumed to work.
    //
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'ggg' to set the state of a newly created object.  Verify that
    //   'ggg' returns the expected value corresponding to the location of
    //   the first invalid value of the 'spec'.  Repeat the test for a
    //   longer 'spec' generated by prepending a string ending in a '~'
    //   character (denoting 'clear').  Note that we are testing the
    //   parser only; the primary manipulators are already assumed to work.
    //
    // Testing:
    //   string<C,CT,A>& gg(string<C,CT,A> *object, const char *spec);
    //   int ggg(string<C,CT,A> *object, const char *spec, int vF = 1);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int MAX_LENGTH = 10;
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
            int         d_length;                // expected length
            char        d_elements[MAX_LENGTH];  // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { 0 }                   },

            { L_,   "A",            1,      { VA }                  },
            { L_,   "B",            1,      { VB }                  },
            { L_,   "~",            0,      { 0 }                   },

            { L_,   "CD",           2,      { VC, VD }              },
            { L_,   "E~",           0,      { 0 }                   },
            { L_,   "~E",           1,      { VE }                  },
            { L_,   "~~",           0,      { 0 }                   },

            { L_,   "ABC",          3,      { VA, VB, VC }          },
            { L_,   "~BC",          2,      { VB, VC }              },
            { L_,   "A~C",          1,      { VC }                  },
            { L_,   "AB~",          0,      { 0 }                   },
            { L_,   "~~C",          1,      { VC }                  },
            { L_,   "~B~",          0,      { 0 }                   },
            { L_,   "A~~",          0,      { 0 }                   },
            { L_,   "~~~",          0,      { 0 }                   },

            { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
            { L_,   "~BCD",         3,      { VB, VC, VD }          },
            { L_,   "A~CD",         2,      { VC, VD }              },
            { L_,   "AB~D",         1,      { VD }                  },
            { L_,   "ABC~",         0,      { 0 }                   },

            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
            { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
            { L_,   "AB~DE",        2,      { VD, VE }              },
            { L_,   "ABCD~",        0,      { 0 }                   },
            { L_,   "A~C~E",        1,      { VE }                  },
            { L_,   "~B~D~",        0,      { 0 }                   },

            { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

            { L_,   "ABCDE~CDEC~E", 1,      { VE }                  }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t    LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;
            const int         curLen = (int)strlen(SPEC);

            Obj mX(Z);
            const Obj& X = gg(&mX, SPEC);   // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj mY(Z);
            const Obj& Y = gg(&mY, buf);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(X);
                T_; T_; T_; P(Y);
            }

            LOOP_ASSERT(LINE, LENGTH == X.size());
            LOOP_ASSERT(LINE, LENGTH == Y.size());
            for (size_t i = 0; i < LENGTH; ++i) {
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == X[i]);
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == Y[i]);
            }

        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec            index
            //----  -------------   -----
            { L_,   "",             -1,     }, // control

            { L_,   "~",            -1,     }, // control
            { L_,   " ",             0,     },
            { L_,   ".",             0,     },
            { L_,   "L",             -1,    }, // control
            { L_,   "M",             0,     },
            { L_,   "Z",             0,     },

            { L_,   "AE",           -1,     }, // control
            { L_,   "aE",            0,     },
            { L_,   "Ae",            1,     },
            { L_,   ".~",            0,     },
            { L_,   "~!",            1,     },
            { L_,   "  ",            0,     },

            { L_,   "ABC",          -1,     }, // control
            { L_,   " BC",           0,     },
            { L_,   "A C",           1,     },
            { L_,   "AB ",           2,     },
            { L_,   "?#:",           0,     },
            { L_,   "   ",           0,     },

            { L_,   "ABCDE",        -1,     }, // control
            { L_,   "aBCDE",         0,     },
            { L_,   "ABcDE",         2,     },
            { L_,   "ABCDe",         4,     },
            { L_,   "AbCdE",         1,     }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(Z);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                // LOOP_ASSERT(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            LOOP_ASSERT(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //
    // Concerns:
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push_back             (black-box)
    //      - clear                 (white-box)
    //   operate as expected.  We have the following specific concerns:
    //    1) The default constructor
    //        1a) creates the correct initial value.
    //        1b) does *not* allocate memory.
    //        1c) has the internal memory management system hooked up
    //              properly so that *all* internally allocated memory
    //              draws from the same user-supplied allocator whenever
    //              one is specified.
    //    2) The destructor properly deallocates all allocated memory to
    //         its corresponding allocator from any attainable state.
    //    3) 'push_back'
    //        3a) produces the expected value.
    //        3b) increases capacity as needed.
    //        3c) maintains valid internal state.
    //        3d) is exception neutral with respect to memory allocation.
    //    4) 'clear'
    //        4a) produces the expected value (empty).
    //        4b) properly destroys each contained element value.
    //        4c) maintains valid internal state.
    //        4d) does not allocate memory.
    //    5) The size based parameters of the class reflect the platform.
    //
    // Plan:
    //   To address concerns 1a - 1c, create an object using the default
    //   constructor:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concerns 3a - 3c, construct a series of independent
    //   objects, ordered by increasing length.  In each test, allow the
    //   object to leave scope without further modification, so that the
    //   destructor asserts internal object invariants appropriately.
    //   After the final insert operation in each test, use the (untested)
    //   basic accessors to cross-check the value of the object
    //   and the 'bslma::TestAllocator' to confirm whether a resize has
    //   occurred.
    //
    //   To address concerns 4a-4c, construct a similar test, replacing
    //   'push_back' with 'clear'; this time, however, use the test
    //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //
    //   To address concerns 2, 3d, 4d, create a small "area" test that
    //   exercises the construction and destruction of objects of various
    //   lengths and capacities in the presence of memory allocation
    //   exceptions.  Two separate tests will be performed.
    //
    //   Let S be the sequence of integers { 0 .. N - 1 }.
    //      (1) for each i in S, use the default constructor and 'push_back'
    //          to create an instance of length i, confirm its value (using
    //           basic accessors), and let it leave scope.
    //      (2) for each (i, j) in S X S, use 'push_back' to create an
    //          instance of length i, use 'clear' to clear its value
    //          and confirm (with 'length'), use insert to set the instance
    //          to a value of length j, verify the value, and allow the
    //          instance to leave scope.
    //
    //   The first test acts as a "control" in that 'clear' is not
    //   called; if only the second test produces an error, we know that
    //   'clear' is to blame.  We will rely on 'bslma::TestAllocator'
    //   and purify to address concern 2, and on the object invariant
    //   assertions in the destructor to address concerns 3d and 4d.
    //
    //   To address concern 5, the values will be explicitly compared to
    //   the expected values.  This will be done first so as to ensure all
    //   other tests are reliable and may depend upon the class's
    //   constants.
    //
    // Testing:
    //   basic_string(const ALLOC& a = ALLOC());
    //   ~basic_string();
    //   void push_back(CHAR_TYPE c);
    //   void clear();
    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const Obj X;
        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        if (1 == sizeof(TYPE)) {
            ASSERTV(k_SHORT_BUFFER_CAPACITY_CHAR,
                    X.capacity(),
                    k_SHORT_BUFFER_CAPACITY_CHAR == X.capacity());
        }
        else if (sizeof(wchar_t) == sizeof(TYPE)) {
            ASSERTV(k_SHORT_BUFFER_CAPACITY_WCHAR_T,
                    X.capacity(),
                    k_SHORT_BUFFER_CAPACITY_WCHAR_T == X.capacity());
        }

        // Verify no allocation from default allocators
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("object",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Allocator Z(&oa);

        const Obj X(Z);

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        // Verify no allocation from default and object allocators.
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(oa.numBlocksTotal(), 0 == oa.numBlocksTotal());
        ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
    }

    if (verbose) printf("\t\tIn place using a test allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());

        Obj x(objectAllocator_p);

        // Verify no allocation from global, default and object allocators.
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());
    }
    ASSERT(0 == globalAllocator_p->numBytesInUse());
    ASSERT(0 == defaultAllocator_p->numBytesInUse());
    ASSERT(0 == objectAllocator_p->numBytesInUse());

    // --------------------------------------------------------------------
    bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tBEFORE: "); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            if(veryVerbose){
                printf("\t\t\tAFTER: "); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX(Z);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // LOOP_ASSERT(li, BB + NUM_ALLOCS[li + 1] - NUM_ALLOCS[li] == AA);
            // LOOP_ASSERT(li, B + 1 == A);

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }
            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            if(veryVerbose){
                printf("\t\t\tBEFORE ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            mX.clear();

            if(veryVerbose){
                printf("\t\t\tAFTER ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT ");
                P_(X.capacity()); P(X);
            }
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) printf("\t\tOn an object of initial length " ZU ".\n",
                                li);

            Obj mX(Z);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B);
                typename Obj::size_type Cap = X.capacity();P_(Cap);P(X);
            }

            mX.clear();

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 CC = testAllocator.numBlocksTotal();
            const Int64 C  = testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            LOOP_ASSERT(li, BB == AA);
            LOOP_ASSERT(li, BB == CC);

            LOOP_ASSERT(li, B  == A);
            LOOP_ASSERT(li, B  == C);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an instance
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the instance to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose) printf("\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(Z);  const Obj& X = mX;                           // 1.
            for (size_t i = 0; i < li; ++i) {                        // 2.
                ExceptionGuard<Obj> guard(X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                   // 4.
          LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
        }
    }

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an instance
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from instance
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the instance to leave scope
        //    9) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (verbose)
                printf("\t\t\tOn an object of initial length " ZU ".\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose)
                    printf("\t\t\t\tAnd with final length " ZU ".\n", j);

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                size_t k; // loop index

                Obj mX(Z);  const Obj& X = mX;         // 1.
                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionGuard<Obj> guard(X, L_);
                    mX.push_back(VALUES[0]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                for (k = 0; k < i; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[0] == X[k]);
                }

                mX.clear();                                         // 4.
                LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                for (k = 0; k < j; ++k) {                           // 6.
                    ExceptionGuard<Obj> guard(X, L_);
                    mX.push_back(VALUES[k % NUM_VALUES]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                for (k = 0; k < j; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
            }
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting no implicit conversion from "
                        "'string_view'.\n");
    {
        bsl::basic_string_view<TYPE> bsw;
        ASSERTV(1 == sizeof(fromStringView(bsw)));
    }
}

template <class TYPE, class TRAITS, class ALLOC>
void TestDriver<TYPE,TRAITS,ALLOC>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //
    // Concerns:
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator==' and 'operator!='
    //      - primary manipulators: 'push_back' and 'clear' methods
    //      - basic accessors: 'size' and 'operator[]'
    //   In addition we would like to exercise objects with potentially
    //   different internal organizations representing the same value.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic
    //   accessors, equality operators, and the assignment operator.
    //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
    //   assignment operator [9&10] in situations where the internal data
    //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
    //   assignment for a non-empty instance [11] and allow the result to
    //   leave scope, enabling the destructor to assert internal object
    //   invariants.  Display object values frequently in verbose mode:
    //
    // 1) Create an object x1 (default ctor).         { x1: }
    // 2) Create a second object x2 (copy from x1).   { x1: x2: }
    // 3) Append an element value A to x1).           { x1:A x2: }
    // 4) Append the same element value A to x2).     { x1:A x2:A }
    // 5) Append another element value B to x2).      { x1:A x2:AB }
    // 6) Remove all elements from x1.                { x1: x2:AB }
    // 7) Create a third object x3 (default ctor).    { x1: x2:AB x3: }
    // 8) Create a forth object x4 (copy of x2).      { x1: x2:AB x3: x4:AB }
    // 9) Assign x2 = x1 (non-empty becomes empty).   { x1: x2: x3: x4:AB }
    // 10) Assign x3 = x4 (empty becomes non-empty).  { x1: x2: x3:AB x4:AB }
    // 11) Assign x4 = x4 (aliasing).                 { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Allocator            Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    (void) NUM_VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(Z);  const Obj& X1 = mX1;
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if(veryVerbose){
        typename Obj::size_type capacity = X1.capacity();
        T_; T_;
        P(capacity);
    }

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");
    Obj mX2(X1, AllocType(&testAllocator));  const Obj& X2 = mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check the initial state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 3) Append an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_back(VALUES[0]);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(1 == X1.size());
    ASSERT(VALUES[0] == X1[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(VALUES[0]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(1 == X2.size());
    ASSERT(VALUES[0] == X2[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5) Append another element value B to x2)."
                         "\t\t{ x1:A x2:AB }\n");
    mX2.push_back(VALUES[1]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(2 == X2.size());
    ASSERT(VALUES[0] == X2[0]);
    ASSERT(VALUES[1] == X2[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));          ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:AB }\n");
    mX1.clear();
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(0 == X1.size());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3(Z);  const Obj& X3 = mX3;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(0 == X3.size());

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );          ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2, AllocType(&testAllocator));  const Obj& X4 = mX4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");

    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );          ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));          ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2, x3, x4.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );          ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));          ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(2 == X3.size());
    ASSERT(VALUES[0] == X3[0]);
    ASSERT(VALUES[1] == X3[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X3 == X1));          ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );          ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose)
        printf("\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// BDE_VERIFY pragma: push  // usage example relaxes rules for clear exposition
//
// BDE_VERIFY pragma: -FD01 // Contracts are often expository
// BDE_VERIFY pragma: -FD02 // Contracts are often expository
// BDE_VERIFY pragma: -FD03 // Contracts are often expository

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 2: 'string' as a data member
///- - - - - - - - - - - - - - - - - -
// The most common use of 'string' objects are as data members in user-defined
// classes.  In this example, we will show how 'string' objects can be used as
// data members.
//
// First, we begin to define a 'class', 'Employee', that represents the data
// corresponding to an employee of a company:
//..
    class Employee {
        // This simply constrained (value-semantic) attribute class represents
        // the information about an employee.  An employee's first and last
        // name are represented as 'string' objects and their employee
        // identification number is represented by an 'int'.  Note that the
        // class invariants are identically the constraints on the individual
        // attributes.
        //
        // This class:
        //: o supports a complete set of *value-semantic* operations
        //:   o except for 'bslx' serialization
        //: o is *exception-neutral* (agnostic)
        //: o is *alias-safe*
        //: o is 'const' *thread-safe*

        // DATA
        bsl::string d_firstName;       // first name
        bsl::string d_lastName;        // last name
        int         d_id;              // identification number
//..
//  Next, we define the creators for this class:
//..
      public:
        // CREATORS
        explicit Employee(bslma::Allocator *basicAllocator = 0);
            // Create a 'Employee' object having the (default) attribute
            // values:
            //..
            //  firstName() == ""
            //  lastName()  == ""
            //  id()        == 0
            //..
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default
            // allocator is used.

        Employee(const bsl::string_view&  firstName,
                 const bsl::string_view&  lastName,
                 int                      id,
                 bslma::Allocator        *basicAllocator = 0);
            // Create a 'Employee' object having the specified 'firstName',
            // 'lastName', and 'id' attribute values.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        Employee(const Employee&   original,
                 bslma::Allocator *basicAllocator = 0);
            // Create a 'Employee' object having the same value as the
            // specified 'original' object.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        //! ~Employee() = default;
            // Destroy this object.
//..
// Notice that all constructors of the 'Employee' class are optionally provided
// an allocator that is then passed through to the 'string' data members of
// 'Employee'.  This allows the user to control how memory is allocated by
// 'Employee' objects.  Also note that the type of the 'firstName' and
// 'lastName' arguments of the value constructor is 'bsl::string_view'.  The
// 'bsl::string_view' allows specifying a 'string' or a 'const char *' to
// represent a string value.  For the sake of brevity its implementation is
// not explored here.
//
// Then, declare the remaining methods of the class:
//..
        // MANIPULATORS
        Employee& operator=(const Employee& rhs);
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.

        void setFirstName(const bsl::string_view& value);
            // Set the 'firstName' attribute of this object to the specified
            // 'value'.

        void setLastName(const bsl::string_view& value);
            // Set the 'lastName' attribute of this object to the specified
            // 'value'.

        void setId(int value);
            // Set the 'id' attribute of this object to the specified 'value'.

        // ACCESSORS
        const bsl::string& firstName() const;
            // Return a reference providing non-modifiable access to the
            // 'firstName' attribute of this object.

        const bsl::string& lastName() const;
            // Return a reference providing non-modifiable access to the
            // 'lastName' attribute of this object.

        int id() const;
            // Return the value of the 'id' attribute of this object.
    };
//..
// Next, we declare the free operators for 'Employee':
//..
    inline
    bool operator==(const Employee& lhs, const Employee& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two 'Employee' objects have the same
        // value if all of their corresponding values of their 'firstName',
        // 'lastName', and 'id' attributes are the same.

    inline
    bool operator!=(const Employee& lhs, const Employee& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same value, and 'false' otherwise.  Two 'Employee' objects do
        // not have the same value if any of the corresponding values of their
        // 'firstName', 'lastName', or 'id' attributes are not the same.
//..
// Then, we implement the various methods of the 'Employee' class:
//..
    // CREATORS
    inline
    Employee::Employee(bslma::Allocator *basicAllocator)
    : d_firstName(basicAllocator)
    , d_lastName(basicAllocator)
    , d_id(0)
    {
    }

    inline
    Employee::Employee(const bsl::string_view&  firstName,
                       const bsl::string_view&  lastName,
                       int                      id,
                       bslma::Allocator        *basicAllocator)
    : d_firstName(firstName.begin(), firstName.end(), basicAllocator)
    , d_lastName(lastName.begin(), lastName.end(), basicAllocator)
    , d_id(id)
    {
        BSLS_ASSERT_SAFE(!firstName.empty());
        BSLS_ASSERT_SAFE(!lastName.empty());
    }

    inline
    Employee::Employee(const Employee&   original,
                       bslma::Allocator *basicAllocator)
    : d_firstName(original.d_firstName, basicAllocator)
    , d_lastName(original.d_lastName, basicAllocator)
    , d_id(original.d_id)
    {
    }
//..
// Notice that the 'basicAllocator' parameter can simply be passed as an
// argument to the constructor of 'bsl::string'.
//
// Now, we implement the remaining manipulators of the 'Employee' class:
//..
    // MANIPULATORS
    inline
    Employee& Employee::operator=(const Employee& rhs)
    {
        d_firstName = rhs.d_firstName;
        d_lastName  = rhs.d_lastName;
        d_id        = rhs.d_id;
        return *this;
    }

    inline
    void Employee::setFirstName(const bsl::string_view& value)
    {
        BSLS_ASSERT_SAFE(!value.empty());

        d_firstName.assign(value.begin(), value.end());
    }

    inline
    void Employee::setLastName(const bsl::string_view& value)
    {
        BSLS_ASSERT_SAFE(!value.empty());

        d_lastName.assign(value.begin(), value.end());
    }

    inline
    void Employee::setId(int value)
    {
        d_id = value;
    }

    // ACCESSORS
    inline
    const bsl::string& Employee::firstName() const
    {
        return d_firstName;
    }

    inline
    const bsl::string& Employee::lastName() const
    {
        return d_lastName;
    }

    inline
    int Employee::id() const
    {
        return d_id;
    }
//..
// Finally, we implement the free operators for 'Employee' class:
//..
    inline
    bool operator==(const Employee& lhs, const Employee& rhs)
    {
        return lhs.firstName() == rhs.firstName()
            && lhs.lastName()  == rhs.lastName()
            && lhs.id()        == rhs.id();
    }

    inline
    bool operator!=(const Employee& lhs, const Employee& rhs)
    {
        return lhs.firstName() != rhs.firstName()
            || lhs.lastName()  != rhs.lastName()
            || lhs.id()        != rhs.id();
    }
//..
//
///Example 3: A stream text replacement filter
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize the 'string' type and its associated
// utility functions to define a function that reads data from an input stream,
// replaces all occurrences of a specified text fragment with another text
// fragment, and writes the resulting text to an output stream.
//
// First, we define the signature of the function, 'replace':
//..
    void replace(std::ostream&      outputStream,
                 std::istream&      inputStream,
                 const bsl::string& oldString,
                 const bsl::string& newString)
        // Read data from the specified 'inputStream' and replace all
        // occurrences of the text contained in the specified 'oldString' in
        // the stream with the text contained in the specified 'newString'.
        // Write the modified data to the specified 'outputStream'.
//..
// Then, we provide the implementation for 'replace':
//..
    {
        const bsl::string::size_type oldStringSize = oldString.size();
        const bsl::string::size_type newStringSize = newString.size();
        bsl::string                  line;

        bsl::getline(inputStream, line);
//..
// Notice that we can use the 'getline' free function defined in this component
// to read a single line of data from an input stream into a 'bsl::string'.
//..
        if (!inputStream) {
            return;                                                   // RETURN
        }

        do {
//..
// Next, we use the 'find' function to search the contents of 'line' for
// characters matching the contents of 'oldString':
//..
            bsl::string::size_type pos = line.find(oldString);
            while (bsl::string::npos != pos) {
//..
// Now, we use the 'replace' method to modify the contents of 'line' matching
// 'oldString' to 'newString':
//..
                line.replace(pos, oldStringSize, newString);
                pos = line.find(oldString, pos + newStringSize);
//..
// Notice that we provide 'find' with the starting position from which to start
// searching.
//..
            }
//..
// Finally, we write the updated contents of 'line' to the output stream:
//..
            outputStream << line;

            bsl::getline(inputStream, line);
        } while (inputStream);
    }
//..

}  // close namespace UsageExample

namespace BloombergLP {
namespace bslma {
// Specialize trait to clarify for bde_verify that 'LimitAllocator' does not
// require satisfy the 'UsesBslmaAllocator' trait.
template <>
struct UsesBslmaAllocator<UsageExample::Employee> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

// BDE_VERIFY pragma: pop  // end of usage example

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
      case 43: {
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

        {
            using namespace UsageExample;
            bslma::TestAllocator defaultAllocator("defaultAllocator");
            bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

            bslma::TestAllocator objectAllocator("objectAllocator");
            bslma::TestAllocator scratch("scratch");

///Example 1: Basic Syntax
///- - - - - - - - - - - -
// In this example, we will show how to create and use the 'string' typedef.
//
// First, we will default-construct a 'string' object:
//..
    bsl::string s;
    ASSERT(s.empty());
    ASSERT(0  == s.size());
    ASSERT("" == s);
//..
// Then, we will construct a 'string' object from a string literal:
//..
    bsl::string t = "Hello World";
    ASSERT(!t.empty());
    ASSERT(11 == t.size());
    ASSERT("Hello World" == t);
//..
// Next, we will clear the contents of 't' and assign it a couple of values:
// first from a string literal; and second from another 'string' object:
//..
    t.clear();
    ASSERT(t.empty());
    ASSERT("" == t);
//
    t = "Good Morning";
    ASSERT(!t.empty());
    ASSERT("Good Morning" == t);
//
    t = s;
    ASSERT(t.empty());
    ASSERT("" == t);
    ASSERT(t == s);
//..
// Then, we will create three 'string' objects: the first representing a street
// name, the second a state, and the third a zipcode.  We will then concatenate
// them into a single address 'string' and print the contents of that 'string'
// on standard output:
//..
    const bsl::string street  = "731 Lexington Avenue";
    const bsl::string state   = "NY";
    const bsl::string zipcode = "10022";
//
    const bsl::string fullAddress = street + " " + state + " " + zipcode;
//
  if (veryVerbose) {
      debugprint(fullAddress);
  }
//..
// The above print statement should produce a single line of output:
//..
//  731 Lexington Avenue NY 10022
//..
// Then, we search the contents of 'address' (using the 'find' function) to
// determine if it lies on a specified street:
//..
    const bsl::string streetName = "Lexington";
//
    if (bsl::string::npos != fullAddress.find(streetName, 0)) {
      if (veryVerbose) {
          debugprint("The address " + fullAddress + " is located on "
                     + streetName + ".");
      }
    }
//..
// Next, we show how to get a reference providing modifiable access to the
// null-terminated string literal stored by a 'string' object using the 'c_str'
// function.  Note that the returned string literal can be passed to various
// standard functions expecting a null-terminated string:
//..
    const bsl::string  v = "Another string";
    const char        *cs = v.c_str();
    ASSERT(strlen(cs) == v.size());
//..
// Then, we construct two 'string' objects, 'x' and 'y', using a user-specified
// allocator:
//..
    bslma::TestAllocator allocator1, allocator2;
//
    const char *SHORT_STRING = "A small string";
    const char *LONG_STRING  = "This long string would definitely cause "
                               "memory to be allocated on creation";
//
    const bsl::string x(SHORT_STRING, &allocator1);
    const bsl::string y(LONG_STRING,  &allocator2);
//
    ASSERT(SHORT_STRING == x);
    ASSERT(LONG_STRING  == y);
//..
// Notice that, no memory was allocated from the allocator for object 'x'
// because of the short-string optimization used in the 'string' type.
//
// Finally, we can track memory usage of 'x' and 'y' using 'allocator1' and
// 'allocator2' and check that memory was allocated only by 'allocator2':
//..
    ASSERT(0 == allocator1.numBlocksInUse());
    ASSERT(1 == allocator2.numBlocksInUse());
//..
//
        }

///Example 2: 'string' as a data member
///- - - - - - - - - - - - - - - - - -
        {
            using namespace UsageExample;

            // Default constructor
            Employee e1;  const Employee& E1 = e1;
            ASSERT("" == E1.firstName());
            ASSERT("" == E1.lastName());
            ASSERT(0  == E1.id());

            // Value constructor
            bsl::string      FIRST_NAME = "Joe";
            bsl::string      LAST_NAME  = "Smith";
            bsl::string_view FIRST(FIRST_NAME.begin(), FIRST_NAME.length());
            bsl::string_view LAST(LAST_NAME.begin(), LAST_NAME.length());
            int              ID         = 1;

            Employee e2(FIRST, LAST, ID);  const Employee& E2 = e2;
            ASSERT(FIRST_NAME == E2.firstName());
            ASSERT(LAST_NAME  == E2.lastName());
            ASSERT(ID         == E2.id());

            // Equality operators
            ASSERT(! (e1 == e2));
            ASSERT(   e1 != e2);

            // Manipulators and accessors
            e1.setFirstName(FIRST);
            ASSERT(FIRST_NAME == e1.firstName());

            e1.setLastName(LAST);
            ASSERT(LAST_NAME == e1.lastName());

            e1.setId(ID);
            ASSERT(ID == e1.id());

            ASSERT(   e1 == e2);
            ASSERT(! (e1 != e2));

            // Copy constructor
            Employee e3(e1);  const Employee& E3 = e3;
            (void) E3;

            ASSERT(   e1 == e3);
            ASSERT(! (e1 != e3));
        }

///Example 3: A 'string' replace function
///- - - - - - - - - - - - - - - - - - -
        {
            using namespace UsageExample;

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_old_p;           // old string to replace
                const char *d_new_p;           // new string to replace with
                const char *d_orig_p;          // original result
                const char *d_exp_p;           // expected result
            } DATA[] = {
            //line  old           new           orig          exp
            //----  ----          ----          ----          ----
            { L_,  "a",          "b",          "abcdeabc",   "bbcdebbc"    },
            { L_,  "b",          "a",          "abcdeabc",   "aacdeaac"    },
            { L_,  "ab",         "xy",         "ababefgh",   "xyxyefgh"    },
            { L_,  "abc",        "xyz",        "abcdefgh",   "xyzdefgh"    },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const bsl::string OLD   = DATA[ti].d_old_p;
                const bsl::string NEW   = DATA[ti].d_new_p;
                const bsl::string ORIG  = DATA[ti].d_orig_p;
                const bsl::string EXP   = DATA[ti].d_exp_p;

                std::istringstream is(ORIG);
                std::ostringstream os;
                replace(os, is, OLD, NEW);
                LOOP_ASSERT(LINE, EXP == os.str());
            }
        }
      } break;
      case 42:     BSLA_FALLTHROUGH;
      case 41:     BSLA_FALLTHROUGH;
      case 40:     BSLA_FALLTHROUGH;
      case 39:     BSLA_FALLTHROUGH;
      case 38:     BSLA_FALLTHROUGH;
      case 37:     BSLA_FALLTHROUGH;
      case 36:     BSLA_FALLTHROUGH;
      case 35:     BSLA_FALLTHROUGH;
      case 34:     BSLA_FALLTHROUGH;
      case 33:     BSLA_FALLTHROUGH;
      case 32:     BSLA_FALLTHROUGH;
      case 31:     BSLA_FALLTHROUGH;
      case 30:     BSLA_FALLTHROUGH;
      case 29:     BSLA_FALLTHROUGH;
      case 28:     BSLA_FALLTHROUGH;
      case 27:     BSLA_FALLTHROUGH;
      case 26:     BSLA_FALLTHROUGH;
      case 25:     BSLA_FALLTHROUGH;
      case 24:     BSLA_FALLTHROUGH;
      case 23:     BSLA_FALLTHROUGH;
      case 22:     BSLA_FALLTHROUGH;
      case 21:     BSLA_FALLTHROUGH;
      case 20:     BSLA_FALLTHROUGH;
      case 19:     BSLA_FALLTHROUGH;
      case 18:     BSLA_FALLTHROUGH;
      case 17:     BSLA_FALLTHROUGH;
      case 16:     BSLA_FALLTHROUGH;
      case 15:     BSLA_FALLTHROUGH;
      case 14:     BSLA_FALLTHROUGH;
      case 13:     BSLA_FALLTHROUGH;
      case 12: {
        if (verbose) printf(
                 "\nTEST CASE %d IS DELEGATED TO 'bslstl_string_test.t.cpp'"
                 "\n=======================================================\n",
                 test);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST TYPE TRAITS
        //   In this test we verify that the following set of the traits have
        //   expected values:
        //   - 'bslma::UsesBslmaAllocator'
        //   - 'bslmf::IsBitwiseMoveable'
        //   - 'bslalg::HasStlIterators'
        //   - 'bsl::is_nothrow_move_constructible'
        //
        // Concerns:
        //: 1 'bslma::UsesBslmaAllocator' trait is set for string types that
        //:    use 'bslma' allocators and NOT set otherwise.
        //:
        //: 2 'bsl::basic_string' should be bitwise movable if the allocator
        //:   type is bitwise movable.
        //:
        //: 3 'bslalg::HasStlIterators' trait is set for 'bsl::basic_string'
        //:
        //: 4 'bsl::is_nothrow_move_constructible' trait is set when compiled
        //:   in C++11 mode.
        //
        // Plan:
        //: 1 Use 'ASSERT' to verify the type traits.  (C-1..4)
        //
        // Testing:
        //   CONCERN: The object has the necessary type traits
        // --------------------------------------------------------------------

        if (veryVerbose) printf("\nTESTING TYPE TRAITS"
                                "\n===================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
        const bool EXP_NOTHROW = true;
#else
        const bool EXP_NOTHROW = false;
#endif

        if (veryVerbose) printf("\tTesting 'bsl::string'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::string>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::string>::value);
        ASSERT(bslalg::HasStlIterators<bsl::string>::value);
        ASSERT(EXP_NOTHROW ==
                       bsl::is_nothrow_move_constructible<bsl::string>::value);


        if (veryVerbose) printf("\tTesting 'bsl::wstring'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::wstring>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::wstring>::value);
        ASSERT(bslalg::HasStlIterators<bsl::wstring>::value);
        ASSERT(EXP_NOTHROW ==
                      bsl::is_nothrow_move_constructible<bsl::wstring>::value);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)

        if (veryVerbose) printf("\tTesting 'bsl::u8string'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::u8string>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::u8string>::value);
        ASSERT(bslalg::HasStlIterators<bsl::u8string>::value);
        ASSERT(EXP_NOTHROW ==
                     bsl::is_nothrow_move_constructible<bsl::u8string>::value);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (veryVerbose) printf("\tTesting 'bsl::u16string'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::u16string>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::u16string>::value);
        ASSERT(bslalg::HasStlIterators<bsl::u16string>::value);
        ASSERT(EXP_NOTHROW ==
                    bsl::is_nothrow_move_constructible<bsl::u16string>::value);

        if (veryVerbose) printf("\tTesting 'bsl::u32string'.\n");

        ASSERT(bslma::UsesBslmaAllocator<bsl::u32string>::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::u32string>::value);
        ASSERT(bslalg::HasStlIterators<bsl::u32string>::value);
        ASSERT(EXP_NOTHROW ==
                    bsl::is_nothrow_move_constructible<bsl::u32string>::value);
#endif

        if (veryVerbose) printf("\tTesting 'std::string'.\n");

        ASSERT(!bslma::UsesBslmaAllocator<std::string>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<std::string>::value);

        // MSVC has unconditional 'noexcept' specifications for move
        // constructor in C++11 mode.  Fixed in MSVC 2015.
#if !defined(BSLS_PLATFORM_CMP_MSVC) || \
    (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
                bsl::is_nothrow_move_constructible<std::string>::value);
#endif

        if (veryVerbose) printf("\tTesting 'std::wstring'.\n");

        ASSERT(!bslma::UsesBslmaAllocator<std::wstring>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<std::wstring>::value);

#if !defined(BSLS_PLATFORM_CMP_MSVC) || \
    (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
               bsl::is_nothrow_move_constructible<std::wstring>::value);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (veryVerbose) printf("\tTesting 'std::u8string'.\n");

        ASSERT(!bslma::UsesBslmaAllocator<std::u8string>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<std::u8string>::value);

        // MSVC has unconditional 'noexcept' specifications for move
        // constructor in C++11 mode.  Fixed in MSVC 2015.
        ASSERT(EXP_NOTHROW ==
                bsl::is_nothrow_move_constructible<std::string>::value);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        ASSERT(!bslma::UsesBslmaAllocator<std::u16string>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<std::u16string>::value);

        // MSVC has unconditional 'noexcept' specifications for move
        // constructor in C++11 mode.  Fixed in MSVC 2015.
# if !defined(BSLS_PLATFORM_CMP_MSVC) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
                bsl::is_nothrow_move_constructible<std::u16string>::value);
# endif

        ASSERT(!bslma::UsesBslmaAllocator<std::u32string>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<std::u32string>::value);

        // MSVC has unconditional 'noexcept' specifications for move
        // constructor in C++11 mode.  Fixed in MSVC 2015.
# if !defined(BSLS_PLATFORM_CMP_MSVC) || \
     (defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900))
        ASSERT(EXP_NOTHROW ==
                bsl::is_nothrow_move_constructible<std::u32string>::value);
# endif
#endif

        if (veryVerbose)
            printf("\tTesting string with bitwise movable allocator.\n");

        typedef bslma::StdTestAllocator<char>  BitwiseMoveableAllocator;
        typedef bsl::basic_string<char,
                                  bsl::char_traits<char>,
                                  BitwiseMoveableAllocator> StringBM;

        ASSERT(bslma::UsesBslmaAllocator<StringBM>::value);
        ASSERT(bslmf::IsBitwiseMoveable<BitwiseMoveableAllocator>::value);
        ASSERT(bslmf::IsBitwiseMoveable<StringBM>::value);
        ASSERT(bslalg::HasStlIterators<StringBM>::value);
        ASSERT(EXP_NOTHROW ==
                          bsl::is_nothrow_move_constructible<StringBM>::value);

        if (veryVerbose)
            printf("\tTesting string with not bitwise movable allocator.\n");

        typedef LimitAllocator<bsl::allocator<char> >
                                                   NotBitwiseMoveableAllocator;
        typedef bsl::basic_string<char,
                                  bsl::char_traits<char>,
                                  NotBitwiseMoveableAllocator> StringNBM;

        ASSERT(!bslma::UsesBslmaAllocator<StringNBM>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<NotBitwiseMoveableAllocator>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<StringNBM>::value);
        ASSERT(bslalg::HasStlIterators<StringNBM>::value);
        ASSERT(EXP_NOTHROW ==
                         bsl::is_nothrow_move_constructible<StringNBM>::value);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose)
            printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //   Now that we can generate many values for our test objects, and
        //   compare results of assignments, we can test the assignment
        //   operator.    This is achieved by the 'testCase9' class method of
        //   the test driver template, instantiated for the basic test type.
        //   See that function for a list of concerns and a test plan.
        //
        // Plan:
        //: 1 See 'TestDriver<CHAR_TYPE>::testCase9' for details.
        //:
        //: 2 Note that this test case was taking too much time, so testing
        //:   on 'CHAR_TYPE' other than 'char' and 'wchar_t' is done only in
        //:   verbose mode.
        //
        // Testing:
        //   basic_string& operator=(const basic_string& rhs);
        //   basic_string& operator=(MovableRef<basic_string> rhs);
        //   basic_string& operator=(const CHAR_TYPE *s); [NEGATIVE ONLY]
        //   basic_string& operator=(const STRING_VIEW_LIKE_TYPE& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATORS"
                            "\n===========================\n");

        if (verbose) printf("\nTesting Copy Assignment Operator"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9();

        if (verbose) {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
            if (verbose) printf("\n... with 'char8_t'.\n");
            TestDriver<char8_t>::testCase9();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
            if (verbose) printf("\n... with 'char16_t'.\n");
            TestDriver<char16_t>::testCase9();

            if (verbose) printf("\n... with 'char32_t'.\n");
            TestDriver<char32_t>::testCase9();
#endif
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Assignment Operator"
                            "\n====================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9Negative();

        if (verbose) {
# if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
            if (verbose) printf("\n... with 'char8_t'.\n");
            TestDriver<char8_t>::testCase9Negative();
# endif

# if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
            if (verbose) printf("\n... with 'char16_t'.\n");
            TestDriver<char16_t>::testCase9Negative();

            if (verbose) printf("\n... with 'char16_t'.\n");
            TestDriver<char16_t>::testCase9Negative();
# endif
        }
#endif

        if (verbose) printf("\nTesting Move Assignment Operator"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9Move();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase9Move();

        if (verbose) {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
            if (verbose) printf("\n... with 'char8_t'.\n");
            TestDriver<char8_t>::testCase9Move();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
            if (verbose) printf("\n... with 'char16_t'.\n");
            TestDriver<char16_t>::testCase9Move();

            if (verbose) printf("\n... with 'char32_t'.\n");
            TestDriver<char32_t>::testCase9Move();
#endif
        }

        if (verbose) printf("\nTesting Alloc Propagation on Copy char"
                            "\n======================================\n");
        TestDriver<char>::testCase9_propagate_on_container_copy_assignment();

        if (verbose) printf("\nTesting Alloc Propagation on Move char"
                            "\n======================================\n");
        TestDriver<char>::testCase9_propagate_on_container_move_assignment();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.  Because the generator is used for various types
        //   in higher numbered test cases, we need to test it on all test
        //   types.  This is achieved by the 'testCase8' class method of the
        //   test driver template, instantiated for the basic test type.
        //
        // Concerns:
        //: See 'TestDriver<CHAR_TYPE>::testCase8' for details.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase8' for details.
        //
        // Testing:
        //   Obj TestDriver::g(const char *spec);
        //   Obj TestDriver::g(size_t length, TYPE seed);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTION 'g'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase8();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase8();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase8();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase8();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase8();
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY AND MOVE CONSTRUCTORS
        //   Having now full confidence in 'operator==', we can use it
        //   to test that copy constructors preserve the notion of
        //   value.  This is achieved by the 'testCase7' class method of the
        //   test driver template, instantiated for the basic test type.  See
        //   that function for a list of concerns and a test plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase7' for details.
        //
        // Testing:
        //   basic_string(const basic_string& original);
        //   basic_string(const basic_string& original, basicAllocator);
        //   basic_string(MovableRef<basic_string> original);
        //   basic_string(MovableRef<basic_string> original, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY AND MOVE CONSTRUCTORS"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase7();
        TestDriver<char>::testCase7Move();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase7();
        TestDriver<wchar_t>::testCase7Move();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase7();
        TestDriver<char8_t>::testCase7Move();
# endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase7();
        TestDriver<char16_t>::testCase7Move();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase7();
        TestDriver<char32_t>::testCase7Move();
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value.  This is achieved by the 'testCase6' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase6' for details.
        //
        // Testing:
        //   bool operator==(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator==(const C *, const string<C,CT,A>&);
        //   bool operator==(const string<C,CT,A>&, const C *);
        //   bool operator!=(const string<C,CT,A>&, const string<C,CT,A>&);
        //   bool operator!=(const C *, const string<C,CT,A>&);
        //   bool operator!=(const string<C,CT,A>&, const C *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON OPERATORS"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase6();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase6();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase6();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase6();
#endif

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Equality Operators"
                            "\n===================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6Negative();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase6Negative();

# if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase6Negative();
# endif

# if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase6Negative();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase6Negative();
# endif
#endif

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OUTPUT (<<) OPERATOR"
                            "\n============================\n");

        if (verbose)
            printf("There is no output operator for this component.\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Having implemented an effective generation mechanism, we now would
        //   like to test thoroughly the basic accessor functions
        //     - size() const
        //     - operator[](int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.  This
        //   is achieved by the 'testCase4' class method of the test driver
        //   template, instantiated for the basic test type.  See that function
        //   for a list of concerns and a test plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase4' for details.
        //
        // Testing:
        //   size_type size() const;
        //   const_reference operator[](size_type pos) const;
        //   const_reference at(size_type pos) const;
        //   const CHAR_TYPE *c_str() const;
        //   const CHAR_TYPE *data() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase4();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase4();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase4();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase4();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase4();
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //   This is achieved by the 'testCase3' class method of the test
        //   driver template, instantiated for the basic test type.  See that
        //   function for a list of concerns and a test plan.
        //
        // Concerns:
        //: See 'TestDriver<CHAR_TYPE>::testCase3' for details.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase3' for details.
        //
        // Testing:
        //   int TestDriver:ggg(Obj *object, const char *spec, int vF = 1);
        //   Obj& TestDriver:gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase3();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase3();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase3();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase3();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase3();
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   We want to ensure that the primary manipulators
        //      - push_back             (black-box)
        //      - clear                 (white-box)
        //   operate as expected.  This is achieved by the 'testCase2' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase2' for details.
        //
        // Testing:
        //   basic_string(const ALLOC& a = ALLOC());
        //   ~basic_string();
        //   void push_back(CHAR_TYPE c);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS (BOOTSTRAP)"
                            "\n========================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase2();

        if (verbose) printf("\n... with 'wchar_t'.\n");
        TestDriver<wchar_t>::testCase2();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n... with 'char8_t'.\n");
        TestDriver<char8_t>::testCase2();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n... with 'char16_t'.\n");
        TestDriver<char16_t>::testCase2();

        if (verbose) printf("\n... with 'char32_t'.\n");
        TestDriver<char32_t>::testCase2();
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //   We want to exercise basic value-semantic functionality.  This is
        //   achieved by the 'testCase1' class method of the test driver
        //   template, instantiated for a few basic test types.  See that
        //   function for a list of concerns and a test plan.  In addition, we
        //   want to make sure that we can use any standard-compliant
        //   allocator, including not necessarily rebound to the same type as
        //   the contained element, and that various manipulators and accessors
        //   work as expected in normal operation.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //: 2 That 'bsls::NameOf' can properly represent the 'bsl::string' as
        //:   "bsl::string".
        //
        // Plan:
        //: See 'TestDriver<CHAR_TYPE>::testCase1' for details.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nStandard value-semantic test.\n");

        if (verbose) printf("\n\t... with 'char' type.\n");
        TestDriver<char>::testCase1();

        if (verbose) printf("\n\t... with 'wchar_t' type.\n");
        TestDriver<wchar_t>::testCase1();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        if (verbose) printf("\n\t... with 'char8_t' type.\n");
        TestDriver<char8_t>::testCase1();
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        if (verbose) printf("\n\t... with 'char16_t' type.\n");
        TestDriver<char16_t>::testCase1();

        if (verbose) printf("\n\t... with 'char32_t' type.\n");
        TestDriver<char32_t>::testCase1();
#endif

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);
        bsl::basic_string<char,
                          bsl::char_traits<char>,
                          bsl::allocator<char> > zz1, zz2(zza);

        if (verbose) printf("\nAdditional tests: misc.\n");

        bsl::basic_string<char, bsl::char_traits<char> > myStr(5, 'a');
        bsl::basic_string<char, bsl::char_traits<char> >::const_iterator citer;
        ASSERT(5 == myStr.size());
        for (citer = myStr.begin(); citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        myStr.insert(myStr.begin(), 'z');
        ASSERT(6 == myStr.size());
        ASSERT('z' == myStr[0]);
        for (citer = myStr.begin() + 1; citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        myStr.erase(myStr.begin() + 2, myStr.begin() + 4);
        ASSERT(4 == myStr.size());
        ASSERT('z' == myStr[0]);
        for (citer = myStr.begin() + 1; citer != myStr.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myStr);

        if (verbose) printf("\nAdditional tests: traits.\n");

        ASSERT((bslmf::IsBitwiseMoveable<bsl::basic_string<char,
                                         bsl::char_traits<char> > >::value));
        ASSERT((bslmf::IsBitwiseMoveable<bsl::basic_string<wchar_t,
                                        bsl::char_traits<wchar_t> > >::value));

        if (verbose) printf("\nStreaming test.\n");

        std::ostringstream ostrm;
        myStr.assign("hello world");
        ostrm << myStr << '\0';
        ASSERT(ostrm.good());
        LOOP_ASSERT(ostrm.str().c_str(),
                    0 == strcmp(ostrm.str().c_str(), "hello world"));

        // can operator<< handle negative width?
        ostrm.str("");
        ostrm << std::setw(-10) << myStr;
        ASSERT(ostrm.str() == myStr);

        char inbuf[] = " goodbye world\n";
        std::istringstream istrm(inbuf);
        istrm >> myStr;
        ASSERT(istrm.good());
        LOOP_ASSERT(myStr.c_str(), "goodbye" == myStr);
        getline(istrm, myStr);
        ASSERT(! istrm.fail());
        LOOP_ASSERT(myStr.c_str(), " world" == myStr);
        getline(istrm, myStr);
        ASSERT(istrm.fail());
        myStr = "done";
        getline(istrm, myStr);
        LOOP_ASSERT(myStr.c_str(), "done" == myStr);

        // can operator>> handle negative width?
        istrm.str("setw");
        istrm.clear();
        istrm >> std::setw(-10) >> myStr;
        LOOP_ASSERT(myStr.c_str(), myStr == "setw");

        // Does 'bsls::NameOf' succeed in shortening the string name from the
        // 'bsl::basic_string<...' monstrosity?

        bsl::string strName = NameOf<bsl::string>().name();
        if ("unknown_type" != strName) {
            ASSERTV(strName, "bsl::string" == strName);
            ASSERTV(nameOfType(strName), strName,
                                               nameOfType(strName) == strName);

            strName = NameOf<std::string>();
            ASSERTV(strName, "std::string" == strName);
            ASSERTV(nameOfType(strName), strName,
                                               nameOfType(strName) != strName);
            ASSERT(nameOfType(std::string("woof")) == strName);
        }
        else {
            ASSERT(0 && "'NameOf' should work everywhere but old Sun CC");
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((""    == []() -> bsl::string { return {}; }()));
            ASSERT(("a"   == []() -> bsl::string { return {'a'}; }()));
            ASSERT(("aba" == []() -> bsl::string {
                return {'a', 'b', 'a'};
            }()));
        }
#endif

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
