// bslstl_stringview.h                                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGVIEW
#define INCLUDED_BSLSTL_STRINGVIEW

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant 'basic_string_view' class template.
//
//@CLASSES:
//  bsl::basic_string_view: C++ compliant 'basic_string_view' implementation
//  bsl::string_view: 'typedef' for 'bsl::basic_string_view<char>'
//  bsl::wstring_view: 'typedef' for 'bsl::basic_string_view<wchar_t>'
//
//@CANONICAL_HEADER: bsl_string_view.h
//
//@SEE_ALSO: ISO C++ Standard, bdlb_stringviewutil
//
//@DESCRIPTION: This component defines a single class template
// 'bsl::basic_string_view' and aliases for ordinary and wide character
// specializations 'bsl::string_view' and 'bsl::wstring_view' implementing
// standard containers, 'std::string_view' and 'std::wstring_view', that can
// refer to a constant contiguous sequence of char-like objects with the first
// element of the sequence at position zero.
//
// An instantiation of 'basic_string_view' is a value-semantic type whose
// salient attribute is the sequence of characters it represents.  The
// 'basic_string_view' 'class' is parameterized by the character type,
// 'CHAR_TYPE' and that character type's traits, 'CHAR_TRAITS'.  The traits for
// each character type provide functions that assign, compare, and copy a
// sequence of those characters.
//
// A 'basic_string_view' meets the requirements of a sequential container with
// random access iterators as specified in the [basic.string_view] section of
// the C++ standard [24.4].  The 'basic_string_view' implemented here adheres
// to the C++17 standard, except that it does not have template specializations
// 'std::u16string_view' and 'std::u32string_view'.  Note that if compiler
// supports C++17 standard, then 'stl' implementation of 'basic_string_view' is
// used.
//
///Lexicographical Comparisons
///---------------------------
// Two 'basic_string_view's 'lhs' and 'rhs' are lexicographically compared by
// first determining 'N', the smaller of the lengths of 'lhs' and 'rhs', and
// comparing characters at each position between 0 and 'N - 1', using
// 'CHAR_TRAITS::compare' in lexicographical fashion.  If
// 'CHAR_TRAITS::compare' determines that string_views are non-equal (smaller
// or larger), then this is the result.  Otherwise, the lengths of the
// string_views are compared and the shorter string_view is declared the
// smaller.  Lexicographical comparison returns equality only when both
// string_views have the same length and the same character value in each
// respective position.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances of
// 'basic_string_view':
//..
//  Legend
//  ------
//  'V'              - the 'CHAR_TYPE' template parameter type of the
//                     'basic_string_view'
//  'a', 'b'         - two distinct objects of type 'basic_string_view<V>'
//  'k'              - an integral number
//  'p'              - a pointer defining a sequence of 'CHAR_TYPE' characters
//
//  +----------------------------------------------+--------------------------+
//  | Operation                                    | Complexity               |
//  |==============================================+==========================|
//  | basic_string_view<V> a (default construction)| O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | basic_string_view<V> a(b) (copy construction)| O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | basic_string_view<V> a(p)                    | O[n]                     |
//  |----------------------------------------------+--------------------------|
//  | basic_string_view<V> a(p, k)                 | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.~basic_string_view<V>() (destruction)      | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.begin(), a.end(),                          | O[1]                     |
//  | a.cbegin(), a.cend(),                        |                          |
//  | a.rbegin(), a.rend(),                        |                          |
//  | a.crbegin(), a.crend()                       |                          |
//  |----------------------------------------------+--------------------------|
//  | a.size()                                     | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.max_size()                                 | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.remove_prefix(k)                           | O[1]                     |
//  | a.remove_suffix(k)                           |                          |
//  |----------------------------------------------+--------------------------|
//  | a[k]                                         | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.at(k)                                      | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.front()                                    | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.back()                                     | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a.swap(b), swap(a, b)                        | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a = b; (assignment)                          | O[1]                     |
//  |----------------------------------------------+--------------------------|
//  | a == b, a != b                               | O[n]                     |
//  |----------------------------------------------+--------------------------|
//  | a < b, a <= b, a > b, a >= b                 | O[n]                     |
//  +----------------------------------------------+--------------------------+
//..
//
///User-defined literals
///---------------------
// The user-defined literal operators are declared for the 'bsl::string_view'
// and 'bsl::wstring_view' types.  The ud-suffix '_sv' is chosen to distinguish
// between the 'bsl'-string_view's user-defined literal operators and the
// 'std'-string_view's user-defined literal 'operator ""sv' introduced in the
// C++14 standard and implemented in the standard library provided by the
// compiler vendor.  Note that the 'bsl'-string_view's 'operator "" _sv',
// unlike the 'std'-string_view's 'operator ""sv', can be used in a client's
// code if the compiler supports the C++11 standard.  Also note that if the
// compiler supports the C++17 standard then the 'std'-string_view's
// 'operator ""sv' can be used to initialize a 'bsl'-string_view as follows:
//..
//  using namespace std::string_view_literals;
//  bsl::string_view sv = "test"sv;
//..
//
// Also note that 'bsl'-string_view's user-defined literal operators are
// declared in the 'bsl::literals::string_view_literals' namespace, where
// 'literals' and 'string_view_literals' are inline namespaces.  Access to
// these operators can be gained with either 'using namespace bsl::literals',
// 'using namespace bsl::string_view_literals' or
// 'using namespace bsl::literals::string_view_literals'.  But we recommend
// 'using namespace bsl::string_view_literals' to minimize the scope of the
// using declaration:
//..
//  using namespace bsl::string_view_literals;
//  bsl::string_view svr = "test"_sv;
//..
//
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
//      bslma::TestAllocator         da("Default", veryVeryVeryVerbose);
//      bslma::DefaultAllocatorGuard dag(&da);
//
//      bslma::TestAllocator sfa ("StringFootprint",     veryVeryVeryVerbose);
//      bslma::TestAllocator svfa("StringViewFootprint", veryVeryVeryVerbose);
//      bslma::TestAllocator ssa ("StringSupplied",      veryVeryVeryVerbose);
//
//      const char *LONG_STRING = "0123456789012345678901234567890123456789"
//                                "0123456789012345678901234567890123456789";
//
//      bsl::string      *sPtr  = new (sfa ) bsl::string(LONG_STRING, &ssa);
//      bsl::string_view *svPtr = new (svfa) bsl::string_view(LONG_STRING);
//
//      assert(sfa.numBytesInUse() >= svfa.numBytesInUse());
//      assert(0                   <   ssa.numBytesInUse());
//      assert(0                   ==   da.numBytesInUse());
//..
// At the same time it supports all most used 'access' operations of the
// 'bsl::string', using the same interfaces:
//..
//      const bsl::string&      STR = *sPtr;
//      const bsl::string_view& SV  = *svPtr;
//
//      assert(STR.length()                == SV.length());
//      assert(STR.empty()                 == SV.empty());
//      assert(STR.front()                 == SV.front());
//      assert(STR.at(15)                  == SV.at(15));
//      assert(STR.find("345")             == SV.find("345"));
//      assert(STR.find_last_not_of("578") == SV.find_last_not_of("578"));
//      assert(STR.compare(0, 3, "012")    == SV.compare(0, 3, "012"));
//..
// However, using the 'bsl::string_view', you need to be especially attentive
// to the lifetime of the source character string, since the component
// explicitly refers to it:
//..
//      assert(LONG_STRING != STR.data());
//      assert(LONG_STRING == SV.data());
//
//      sfa.deleteObject(sPtr);
//      svfa.deleteObject(svPtr);
//..

#include <bslscm_version.h>

#include <bslstl_hash.h>
#include <bslstl_iterator.h>
#include <bslstl_stdexceptutil.h>

#include <bslalg_scalarprimitives.h>

#include <bslh_hash.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <string>      // for 'std::char_traits'
#include <functional>  // for 'std::less', 'std::greater_equal'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// 'BDE_DISABLE_CPP17_ABI' is intended for CI builds only, to allow simulation
// of Sun/AIX builds on Linux hosts.  It is an error to define this symbol in
// Bloomberg production builds.
#ifndef BDE_DISABLE_CPP17_ABI
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#   include <string_view>

namespace bsl {

using std::basic_string_view;
using std::string_view;
using std::wstring_view;

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
using std::u8string_view;
#   endif

using std::u16string_view;
using std::u32string_view;

using std::swap;

using std::operator==;
using std::operator!=;
using std::operator<;
using std::operator<=;
using std::operator>;
using std::operator>=;

}
# define BSLSTL_STRING_VIEW_IS_ALIASED
# endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // BDE_DISABLE_CPP17_ABI

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

#if defined(BSLS_PLATFORM_OS_WINDOWS) ||                                      \
   (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION <  0x5130) || \
   (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION == 0x5150)
  // Windows or Sun CC before version 5.12.4 or Sun CC version 5.12.6

# define BSLSTL_STRINGVIEW_IDENTITY_USE_WRAPPER    1
#else
# define BSLSTL_STRINGVIEW_IDENTITY_USE_WRAPPER    0
#endif

namespace BloombergLP {
namespace bslstl {

                         // ===========================
                         // struct 'StringView_Identity
                         // ===========================

template <class TYPE>
struct StringView_Identity {
#if BSLSTL_STRINGVIEW_IDENTITY_USE_WRAPPER
    // See 'Implementation Notes' in the implementation .cpp file.

    struct type {
        // DATA
        TYPE d_value;

        // CREATOR
        template <class ARG_TYPE>
        type(const ARG_TYPE& argument,
                     typename
                     bsl::enable_if<bsl::is_convertible<ARG_TYPE, TYPE>::value,
                                    int>::type = 0);
            // Initialize 'd_value' from the specified 'argument', of the
            // specified 'ARG_TYPE', where 'ARG_TYPE' can be any type that is
            // convertible to the specified 'TYPE'.

        // type(const type&) = default;

        // MANIPULATORS
        // type& operator=(const type&) = default;

        TYPE& operator=(const TYPE& rhs);
            // Assign the specified 'rhs' of type 'TYPE' to this object, and
            // return a reference providing modifiable access to the 'TYPE'
            // object held by this object.

        operator TYPE&();
            // Return a reference providing modifiable access to the 'TYPE'
            // object held by this object.

        // ACCESSOR
        operator const TYPE&() const;
            // Return a const reference to the 'TYPE' object held by this
            // object.
    };
#else
    typedef TYPE type;
#endif
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
// Import 'char_traits' into the 'bsl' namespace so that 'basic_string_view'
// and 'char_traits' are always in the same namespace.

using std::char_traits;

                        // =======================
                        // class basic_string_view
                        // =======================

template <class CHAR_TYPE, class CHAR_TRAITS = char_traits<CHAR_TYPE> >
class basic_string_view {
    // This class template provides an STL-compliant 'string_view'.  This
    // implementation offers strong exception guarantees (see below), with the
    // general rule that any method that attempts to access a position outside
    // the valid range of a string_view throws 'std::out_of_range'.
    //
    // Note that the search methods, such as 'find', 'rfind', etc, do *not*
    // actually access invalid positions, so they do *not* throw exceptions.
    //
    // More generally, this class supports an almost complete set of *in-core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison (but excluding 'ostream' printing since this
    // component is below STL).  A precise operational definition of when two
    // objects have the same value can be found in the description of
    // 'operator==' for the class.  This class is *exception* *neutral* with
    // full guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left
    // unchanged.  In no event is memory leaked.

  public:
    // TYPES
    typedef CHAR_TRAITS                            traits_type;
    typedef CHAR_TYPE                              value_type;
    typedef value_type                            *pointer;
    typedef const value_type                      *const_pointer;
    typedef value_type&                            reference;
    typedef const value_type&                      const_reference;
    typedef const value_type                      *const_iterator;
    typedef const_iterator                         iterator;

    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

    typedef std::size_t                            size_type;
    typedef std::ptrdiff_t                         difference_type;

    // CLASS DATA
    static const size_type npos = ~size_type(0);
        // Value used to denote "not-a-position", guaranteed to be outside the
        // range '[0 .. max_size()]'.

  private:
    // DATA
    const CHAR_TYPE *d_start_p;  // pointer to the data
    size_type        d_length;   // length of the view

    // PRIVATE ACCESSORS
    int privateCompareRaw(size_type        lhsPosition,
                          size_type        lhsNumChars,
                          const CHAR_TYPE *other,
                          size_type        otherNumChars) const;
        // Lexicographically compare the substring of this string starting at
        // the specified 'lhsPosition' of length 'lhsNumChars' with the
        // specified initial 'otherNumChars' characters in the specified
        // 'other' string, and return a negative value if the indicated
        // substring of this string is less than 'other', a positive value if
        // it is greater than 'other', and 0 in case of equality.  The behavior
        // is undefined unless 'lhsPosition <= length()',
        // 'lhsNumChars <= length()', and
        // 'lhsPosition <= length() - lhsNumChars'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(basic_string_view,
                                   bsl::is_trivially_copyable);

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    basic_string_view() BSLS_KEYWORD_NOEXCEPT;
        // Create an empty view.

    //! basic_string_view(const basic_string_view& original) = default;
        // Create a view that has the same value as the specified 'original'
        // object.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    basic_string_view(const CHAR_TYPE *characterString);  // IMPLICIT
        // Create a view of the specified null-terminated 'characterString' (of
        // length 'CHAR_TRAITS::length(characterString)').

    BSLS_KEYWORD_CONSTEXPR_CPP14
    basic_string_view(const CHAR_TYPE *characterString,
                      size_type        numChars);
        // Create a view that has the same value as the subview of the
        // optionally specified 'numChars' length starting at the beginning of
        // the specified 'characterString'.  The behavior is undefined unless
        // 'characterString || (numChars == 0)' and 'numChars <= max_size()'.

    template <class ALLOCATOR>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    basic_string_view(
              const std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str);
        // Create a view of the specified 'string'.

    //! ~basic_string_view() = default;
        // Destroy this object.

    // MANIPULATORS
    //! basic_string_view& operator=(const basic_string_view& rhs) = default;
        // Assign to this view the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this view.

    template <class ALLOCATOR>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    basic_string_view& operator=(
               const std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& rhs)
    BSLS_KEYWORD_NOEXCEPT;
        // Assign to this view the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this view.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void remove_prefix(size_type numChars);
        // Move the start of this view forward by the specified 'numChars'.
        // The behavior is undefined unless 'numChars <= length()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void remove_suffix(size_type numChars);
        // Move the end of this view back by the specified 'numChars'.  The
        // behavior is undefined unless 'numChars <= length()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void swap(basic_string_view& other) BSLS_KEYWORD_NOEXCEPT;
        // Exchange the value of this view with the value of the specified
        // 'other' object.

    // ACCESSORS

                      // *** iterator support ***

    BSLS_KEYWORD_CONSTEXPR
    const_iterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // character of this view (or the past-the-end iterator if this view is
        // empty).

    BSLS_KEYWORD_CONSTEXPR
    const_iterator  end() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end iterator for this view.

    BSLS_KEYWORD_CONSTEXPR
    const_reverse_iterator  rbegin() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last character of this view (or the past-the-end reverse iterator if
        // this view is empty).

    BSLS_KEYWORD_CONSTEXPR
    const_reverse_iterator  rend() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator for this view.

                      // *** capacity ***

    BSLS_KEYWORD_CONSTEXPR
    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the length of this view.

    BSLS_KEYWORD_CONSTEXPR
    size_type length() const BSLS_KEYWORD_NOEXCEPT;
        // Return the length of this view.

    BSLS_KEYWORD_CONSTEXPR
    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the maximal possible length of this view.  Note that requests
        // to create a view longer than this number of characters are
        // guaranteed to raise an 'std::length_error' exception.

    BSLS_KEYWORD_CONSTEXPR
    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this view has length 0, and 'false' otherwise.

                      // *** element access ***

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the character
        // at the specified 'position' in this view.  The behavior is undefined
        // unless 'position < length()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference at(size_type position) const;
        // Return a reference providing non-modifiable access to the character
        // at the specified 'position' in this view.  Throw 'std::out_of_range'
        // if 'position >= length()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference front() const;
        // Return a reference providing non-modifiable access to the character
        // at the first position in this view.  The behavior is undefined if
        // this view is empty.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference back() const;
        // Return a reference providing non-modifiable access to the character
        // at the last position in this view.  The behavior is undefined if
        // this view is empty.  Note that the last position is 'length() - 1'.

    BSLS_KEYWORD_CONSTEXPR
    const_pointer data() const BSLS_KEYWORD_NOEXCEPT;
        // Return an address providing non-modifiable access to the underlying
        // character array.  Note that this array may be not null-terminated.

                      // *** string operations ***

    size_type copy(CHAR_TYPE *characterString,
                   size_type  numChars,
                   size_type  position = 0) const;
        // Copy from this view, starting from the optionally specified
        // 'position', the specified 'numChars' or 'length() - position'
        // characters, whichever is smaller, into the specified
        // 'characterString' buffer, and return the number of characters
        // copied.  If 'position' is not specified, 0 is used.  Throw
        // 'std::out_of_range' if 'position > length()'.  Note that the output
        // 'characterString' is *not* null-terminated.  The behavior is
        // undefined unless 'characterString' has enough room to hold at least
        // 'numChars' or 'length() - position', whichever is smaller, and the
        // 'characterString' does not lie within the source range to be copied.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    basic_string_view substr(size_type position = 0,
                             size_type numChars = npos) const;
        // Return a view whose value is the subview starting at the optionally
        // specified 'position' in this view, of length the optionally
        // specified 'numChars' or 'length() - position', whichever is smaller.
        // If 'position' is not specified, 0 is used (i.e., the subview is from
        // the beginning of this view).  If 'numChars' is not specified, 'npos'
        // is used (i.e., the entire suffix from 'position' to the end of the
        // view is returned).  Throw 'std::out_of_range' if
        // 'position > length()'.

    BSLS_KEYWORD_CONSTEXPR
    int compare(basic_string_view other) const BSLS_KEYWORD_NOEXCEPT;
        // Lexicographically compare this view with the specified 'other' view,
        // and return a negative value if this view is less than 'other', a
        // positive value if it is greater than 'other', and 0 in case of
        // equality.  See {Lexicographical Comparisons}.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    int compare(size_type         position,
                size_type         numChars,
                basic_string_view other) const;
        // Lexicographically compare the subview of this view of the specified
        // 'numChars' length starting at the specified 'position' (or the
        // suffix of this view starting at 'position' if
        // 'position + numChars > length()') with the specified 'other' view,
        // and return a negative value if the indicated subview of this view is
        // less than 'other', a positive value if it is greater than 'other',
        // and 0 in case of equality.  Throw 'std::out_of_range' if
        // 'position > length()'.  See {Lexicographical Comparisons}.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    int compare(size_type         lhsPosition,
                size_type         lhsNumChars,
                basic_string_view other,
                size_type         otherPosition,
                size_type         otherNumChars) const;
        // Lexicographically compare the subview of this view of the specified
        // 'lhsNumChars' length starting at the specified 'lhsPosition' (or the
        // suffix of this view starting at 'lhsPosition' if
        // 'lhsPosition + lhsNumChars > length()') with the subview of the
        // specified 'other' view of the specified 'otherNumChars' length
        // starting at the specified 'otherPosition' (or the suffix of 'other'
        // starting at 'otherPosition' if
        // 'otherPosition + otherNumChars > other.length()').  Return a
        // negative value if the indicated subview of this view is less than
        // the indicated subview of 'other', a positive value if it is greater
        // than the indicated subview of 'other', and 0 in case of equality.
        // Throw 'std::out_of_range' if 'lhsPosition > length()' or
        // 'otherPosition > other.length()'.  See
        // {Lexicographical Comparisons}.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    int compare(const CHAR_TYPE *other) const;
        // Lexicographically compare this view with the specified
        // null-terminated 'other' string (of length
        // 'CHAR_TRAITS::length(other)'), and return a negative value if this
        // view is less than 'other', a positive value if it is greater than
        // 'other', and 0 in case of equality.  The behavior is undefined
        // unless 'other' is a null-terminated string.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    int compare(size_type        lhsPosition,
                size_type        lhsNumChars,
                const CHAR_TYPE *other) const;
        // Lexicographically compare the subview of this view of the specified
        // 'lhsNumChars' length starting at the specified 'lhsPosition' (or the
        // suffix of this view starting at 'lhsPosition' if
        // 'lhsPosition + lhsNumChars > length()') with the specified
        // null-terminated 'other' string (of length
        // 'CHAR_TRAITS::length(other)'), and return a negative value if the
        // indicated subview of this view is less than 'other', a positive
        // value if it is greater than 'other', and 0 in case of equality.
        // Throw 'std::out_of_range' if 'lhsPosition > length()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    int compare(size_type        lhsPosition,
                size_type        lhsNumChars,
                const CHAR_TYPE *other,
                size_type        otherNumChars) const;
        // Lexicographically compare the subview of this view of the specified
        // 'lhsNumChars' length starting at the specified 'lhsPosition' (or the
        // suffix of this view starting at 'lhsPosition' if
        // 'lhsPosition + lhsNumChars > length()') with the specified 'other'
        // string of the specified 'otherNumChars' length, and return a
        // negative value if the indicated subview of this view is less than
        // 'other', a positive value if it is greater than 'other', and 0 in
        // case of equality.  'CHAR_TRAITS::lt' is used to compare characters.
        // Throw 'std::out_of_range' if 'lhsPosition > length()'.  The behavior
        // is undefined unless 'other || 0 == otherNumChars'.

#if defined(BSLSTL_STRINGVIEW_ENABLE_CPP20_METHODS)
    BSLS_KEYWORD_CONSTEXPR_CPP14
    bool starts_with(basic_string_view subview) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this view starts with the specified 'subview', and
        // 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR
    bool starts_with(CHAR_TYPE character) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this view starts with the specified 'character',
        // and 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bool starts_with(const CHAR_TYPE* characterString) const;
        // Return 'true' if this view starts with the specified
        // 'characterString', and 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bool ends_with(basic_string_view subview) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this view ends with the specified 'subview', and
        // 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR
    bool ends_with(CHAR_TYPE character) const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this view ends with the specified 'character', and
        // 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bool ends_with(const CHAR_TYPE* characterString) const;
        // Return 'true' if this view ends with the specified
        // 'characterString', and 'false' otherwise.
#endif

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find(basic_string_view subview,
                   size_type         position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the starting position of the *first* occurrence of the
        // specified 'subview', if it can be found in this view (on or *after*
        // the optionally specified 'position') using 'CHAR_TRAITS::eq' to
        // compare characters, and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find(const CHAR_TYPE *characterString,
                   size_type        position,
                   size_type        numChars) const;
        // Return the starting position of the *first* occurrence of the
        // specified 'characterString' of the specified 'numChars' length, if
        // such a string can be found in this view (on or *after* the specified
        // 'position') using 'CHAR_TRAITS::eq' to compare characters, and
        // return 'npos' otherwise.  The behavior is undefined unless
        // 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find(const CHAR_TYPE *characterString,
                   size_type        position = 0) const;
        // Return the starting position of the *first* occurrence of the
        // specified null-terminated 'characterString', if such a string can be
        // found in this view (on or *after* the optionally specified
        // 'position') using 'CHAR_TRAITS::eq' to compare characters, and
        // return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find(CHAR_TYPE character,
                   size_type position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *first* occurrence of the specified
        // 'character', if such an occurrence can be found in this view (on or
        // *after* the optionally specified 'position' if such a 'position' is
        // specified), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type rfind(
                basic_string_view subview,
                size_type         position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the starting position of the *last* occurrence of the
        // specified 'subview' within this view, if such a sequence can be
        // found in this view (on or *before* the optionally specified
        // 'position' if such a 'position' is specified) using
        // 'CHAR_TRAITS::eq' to compare characters, and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type rfind(const CHAR_TYPE *characterString,
                    size_type        position,
                    size_type        numChars) const;
        // Return the starting position of the *last* occurrence of the
        // specified 'characterString' of the specified 'numChars' length, if
        // such a string can be found in this view (on or *after* the specified
        // 'position') using 'CHAR_TRAITS::eq' to compare characters, and
        // return 'npos' otherwise.  The behavior is undefined unless
        // 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type rfind(const CHAR_TYPE *characterString,
                    size_type        position = npos) const;
        // Return the starting position of the *last* occurrence of the
        // specified null-terminated 'characterString', if such a string can be
        // found in this view (on or *after* the optionally specified
        // 'position') using 'CHAR_TRAITS::eq' to compare characters, and
        // return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type rfind(CHAR_TYPE character,
                    size_type position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *last* occurrence of the specified
        // 'character', if such an occurrence can be found in this view (on or
        // *before* the optionally specified 'position' if such a 'position' is
        // specified), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_of(
                   basic_string_view subview,
                   size_type         position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'subview', if such an occurrence can be
        // found in this view (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_of(const CHAR_TYPE *characterString,
                            size_type        position,
                            size_type        numChars) const;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'characterString' of the specified
        // 'numChars' length, if such a string can be found in this view (on or
        // *after* the specified 'position') using 'CHAR_TRAITS::eq' to compare
        // characters, and return 'npos' otherwise.  The behavior is undefined
        // unless 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_of(const CHAR_TYPE *characterString,
                            size_type        position = 0) const;
        // Return the position of the *first* occurrence of a character
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this view (on or *after* the optionally specified
        // 'position'), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_of(
                           CHAR_TYPE character,
                           size_type position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *first* occurrence of the specified
        // 'character', if such an occurrence can be found in this view (on or
        // *after* the optionally specified 'position' if such a 'position' is
        // specified), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_of(
                basic_string_view subview,
                size_type         position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'subview', if such an occurrence can be
        // found in this view (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_of(const CHAR_TYPE *characterString,
                           size_type        position,
                           size_type        numChars) const;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'characterString' of the specified
        // 'numChars' length, if such a string can be found in this view (on or
        // *after* the specified 'position') using 'CHAR_TRAITS::eq' to compare
        // characters, and return 'npos' otherwise.  The behavior is undefined
        // unless 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_of(const CHAR_TYPE *characterString,
                           size_type        position = npos) const;
        // Return the position of the *last* occurrence of a character
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this view (on or *after* the optionally specified
        // 'position'), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_of(
                        CHAR_TYPE character,
                        size_type position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *last* occurrence of the specified
        // 'character', if such an occurrence can be found in this view (on or
        // *before* the optionally specified 'position' if such a 'position' is
        // specified), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_not_of(
                   basic_string_view subview,
                   size_type         position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'subview', if such an occurrence can be
        // found in this view (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_not_of(const CHAR_TYPE *characterString,
                                size_type        position,
                                size_type        numChars) const;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'characterString' of the specified
        // 'numChars' length, if such an occurrence can be found in this view
        // (on or *after* the specified 'position') using 'CHAR_TRAITS::eq' to
        // compare characters, and return 'npos' otherwise.  The behavior is
        // undefined unless 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_not_of(const CHAR_TYPE *characterString,
                                size_type        position = 0) const;
        // Return the position of the *first* occurrence of a character *not*
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this view (on or *after* the optionally specified
        // 'position'), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_first_not_of(
                           CHAR_TYPE character,
                           size_type position = 0) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *first* occurrence of a character
        // *different* from the specified 'character', if such an occurrence
        // can be found in this view (on or *after* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_not_of(
                basic_string_view subview,
                size_type         position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'subview', if such an occurrence can be
        // found in this view (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_not_of(const CHAR_TYPE *characterString,
                               size_type        position,
                               size_type        numChars) const;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'characterString' of the specified
        // 'numChars' length, if such an occurrence can be found in this view
        // (on or *after* the specified 'position') using 'CHAR_TRAITS::eq' to
        // compare characters, and return 'npos' otherwise.  The behavior is
        // undefined unless 'characterString || (numChars == 0)'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_not_of(const CHAR_TYPE *characterString,
                               size_type        position = npos) const;
        // Return the position of the *last* occurrence of a character *not*
        // belonging to the specified 'characterString', if such an occurrence
        // can be found in this view (on or *after* the optionally specified
        // 'position'), and return 'npos' otherwise.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    size_type find_last_not_of(
                        CHAR_TYPE character,
                        size_type position = npos) const BSLS_KEYWORD_NOEXCEPT;
        // Return the position of the *last* occurrence of a character
        // *different* from the specified 'character', if such an occurrence
        // can be found in this view (on or *before* the optionally specified
        // 'position' if such a 'position' is specified), and return 'npos'
        // otherwise.

            // *** BDE compatibility with platform libraries: ***

    template <class ALLOCATOR>
    BSLS_KEYWORD_EXPLICIT
    operator std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>()
    const
        // Convert this object to a string type native to the compiler's
        // library, instantiated with the same character type and traits type.
        // The return string will contain the same sequence of characters as
        // this object and will have a default-constructed allocator.
    {
        // See {DRQS 131792157} for why this is inline.
        return std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>(
            d_start_p, d_length);
    }
};

// TYPEDEFS
typedef basic_string_view<char>         string_view;
typedef basic_string_view<wchar_t>     wstring_view;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
typedef basic_string_view<char8_t>    u8string_view;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
typedef basic_string_view<char16_t>  u16string_view;
typedef basic_string_view<char32_t>  u32string_view;
#endif

// FREE FUNCTIONS
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP14
void swap(basic_string_view<CHAR_TYPE, CHAR_TRAITS>& a,
          basic_string_view<CHAR_TYPE, CHAR_TRAITS>& b) BSLS_KEYWORD_NOEXCEPT;
    // Exchange the value of the specified 'a' object with the value of the
    // specified 'b' object.

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl_stringview_relops {

                    // =============================
                    // struct StringView_CompareUtil
                    // =============================

template <class CHAR_TYPE, class CHAR_TRAITS>
struct StringView_CompareUtil {
    // This component-private utility 'struct' contains functions for comparing
    // two 'string_view' objects.  This functionality is needed to implement
    // the comparison operators operating on string_view objects without
    // resorting to code duplication or delegating directly between different
    // overloads of the same operator.  The need to avoid delegation to
    // overloads stems from a bug in xlC 12 on AIX leading to incorrect
    // overload resolution and infinite recursion.

    typedef bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> StringView;

    static
    BSLS_KEYWORD_CONSTEXPR
    bool equals(StringView lhs, StringView rhs) BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if 'lhs == rhs' and 'false' otherwise.

    static
    BSLS_KEYWORD_CONSTEXPR_CPP14
    bool lessThan(StringView lhs, StringView rhs) BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if 'lhs < rhs' and 'false' otherwise.
};

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator==(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
                bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator==(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator==(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has the same value as the
    // specified 'rhs' view, and 'false' otherwise.  Two views have the same
    // value if they have the same length, and the characters at each
    // respective position have the same value according to 'CHAR_TRAITS::eq'.

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
                bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has a different value from the
    // specified 'rhs' view, and 'false' otherwise.  Two views have the same
    // value if they have the same length, and the characters at each
    // respective position have the same value according to 'CHAR_TRAITS::eq'.

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP14
bool operator<(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              lhs,
               bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator<(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator<(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has a lexicographically
    // smaller value than the specified 'rhs' view, and 'false' otherwise.  See
    // {Lexicographical Comparisons}.

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              lhs,
               bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has a lexicographically larger
    // value than the specified 'rhs' view, and 'false' otherwise.  See
    // {Lexicographical Comparisons}.

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
                bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has a value lexicographically
    // smaller than or or equal to the specified 'rhs' view, and 'false'
    // otherwise.  See {Lexicographical Comparisons}.

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
                bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT;
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' view has a value lexicographically
    // larger than or equal to the specified 'rhs' view, and 'false' otherwise.
    // See {Lexicographical Comparisons}.

}  // close namespace bslstl_stringview_relops
}  // close namespace BloombergLP

namespace bsl {

template <class CHAR_TYPE, class CHAR_TRAITS>
std::basic_ostream<CHAR_TYPE>&
operator<<(std::basic_ostream<CHAR_TYPE>&            stream,
           basic_string_view<CHAR_TYPE, CHAR_TRAITS> stringView);
    // Write the value of the string bound to the specified 'stringView' to the
    // specified output 'stream' and return a reference to the modifiable
    // 'stream'.

// HASH SPECIALIZATIONS
template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS>
void hashAppend(HASHALG&                                         hashAlg,
                const basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input);
    // Pass the specified 'input' string to the specified 'hashAlg' hashing
    // algorithm of the (template parameter) type 'HASHALG'.

template <class CHAR_TYPE, class CHAR_TRAITS>
struct hash<basic_string_view<CHAR_TYPE, CHAR_TRAITS> >
    : ::BloombergLP::bslh::Hash<>
    // Specialize 'bsl::hash' for strings, including an overload for pointers
    // to allow character arrays to be hashed without converting them first.
{
    // PUBLIC ACCESSORS
    std::size_t operator()(
                 const basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input) const;
        // Compute and return the hash value of the specified 'input'.

    std::size_t operator()(const CHAR_TYPE *input) const;
        // Compute and return the hash value of the contents of the specified
        // null-terminated 'input'.  This value will be the same as the hash
        // value of a 'basic_string_view' constructed from 'input'.
};

#if defined(BSLS_PLATFORM_CMP_SUN)  // {DRQS 132030795}

// Sun CC 12.3 has trouble with the partial specializations above in certain
// circumstances (see {DRQS 132030795}).  Adding these explicit specializations
// for 'string_view' and 'wstring_view' makes the problematic cases work.

template <>
struct hash<string_view> : ::BloombergLP::bslh::Hash<>
{
    // PUBLIC ACCESSORS
    std::size_t operator()(const string_view& input) const;
        // Compute and return the hash value of the specified 'input'.

    std::size_t operator()(const char *input) const;
        // Compute and return the hash value of the contents of the specified
        // null-terminated 'input'.  This value will be the same as the hash
        // value of a 'basic_string_view' constructed from 'input'.
};

template <>
struct hash<wstring_view> : ::BloombergLP::bslh::Hash<>
{
    // PUBLIC ACCESSORS
    std::size_t operator()(const wstring_view& input) const;
        // Compute and return the hash value of the specified 'input'.

    std::size_t operator()(const wchar_t *input) const;
        // Compute and return the hash value of the contents of the specified
        // null-terminated 'input'.  This value will be the same as the hash
        // value of a 'basic_string_view' constructed from 'input'.
};

#endif  // defined(BSLS_PLATFORM_CMP_SUN)

}  // close namespace bsl

#endif  // BSLSTL_STRING_VIEW_IS_ALIASED


#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)
namespace bsl {
inline namespace literals {
inline namespace string_view_literals {
 string_view operator ""_sv(const char    *characterString,
                            std::size_t    length);
wstring_view operator ""_sv(const wchar_t *characterString,
                            std::size_t    length);
    // Convert a character sequence of the specified 'length' excluding the
    // terminating null character starting at the beginning of the specified
    // 'characterString' to a string_view object of the indicated return type.
    // (See the "User-Defined Literals" section in the component-level
    // documentation.)
    //
    // Example:
    //..
    //     using namespace bsl::string_view_literals;
    //     bsl::string_view sv1 = "123\0abc";
    //     bsl::string_view sv2 = "123\0abc"_sv;
    //     assert(3 == sv1.size());
    //     assert(7 == sv2.size());
    //
    //     bsl::wstring_view sv3 = L"123\0abc"_sv;
    //     assert(7 == sv3.size());
    //..

}  // close namespace string_view_literals
}  // close namespace literals
}  // close namespace bsl
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY &&
        // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
namespace BloombergLP {
namespace bslh {

template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void hashAppend(HASHALG&                                              hashAlg,
                const std::basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input);
    // Pass the specified 'input' string to the specified 'hashAlg' hashing
    // algorithm of the (template parameter) type 'HASHALG'.  Note that this
    // function violates the BDE coding standard, adding a function for a
    // namespace for a different package, and none of the function parameters
    // are from this package either.  This is necessary in order to provide an
    // implementation of 'bslh::hashAppend' for the (native) standard library
    // 'string_view' type as we are not allowed to add overloads directly into
    // namespace 'std', and this component essentially provides the interface
    // between 'bsl' and 'std' string types.

}  // close namespace bslh
}  // close enterprise namespace
#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================



#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

#if BSLSTL_STRINGVIEW_IDENTITY_USE_WRAPPER

namespace BloombergLP {
namespace bslstl {

template <class TYPE>
template <class ARG_TYPE>
inline
StringView_Identity<TYPE>::type::type(
            const ARG_TYPE& argument,
            typename bsl::enable_if<bsl::is_convertible<ARG_TYPE, TYPE>::value,
                                    int>::type)
: d_value(argument)
{}

// MANIPULATORS
template <class TYPE>
inline
TYPE& StringView_Identity<TYPE>::type::operator=(const TYPE& rhs)
{
    d_value = rhs;

    return d_value;
}

template <class TYPE>
inline
StringView_Identity<TYPE>::type::operator TYPE&()
{
    return d_value;
}

// ACCESSOR
template <class TYPE>
inline
StringView_Identity<TYPE>::type::operator const TYPE&() const
{
    return d_value;
}

}  // close package namespace
}  // close enterprise namespace

#endif

namespace bsl {

                          // -----------------------
                          // class basic_string_view
                          // -----------------------

// CLASS DATA
template <class CHAR_TYPE, class CHAR_TRAITS>
const typename basic_string_view<CHAR_TYPE,CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE,CHAR_TRAITS>::npos;

// PRIVATE ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS>
int basic_string_view<CHAR_TYPE,CHAR_TRAITS>::privateCompareRaw(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(lhsPosition <= length());

    size_type numChars  = lhsNumChars < otherNumChars ? lhsNumChars
                                                      : otherNumChars;
    int       cmpResult = CHAR_TRAITS::compare(data() + lhsPosition,
                                               other,
                                               numChars);
    if (cmpResult) {
        return cmpResult;                                             // RETURN
    }
    if (lhsNumChars < otherNumChars) {
        return -1;                                                    // RETURN
    }
    if (lhsNumChars > otherNumChars) {
        return 1;                                                     // RETURN
    }
    return 0;
}

// CREATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
basic_string_view<CHAR_TYPE,CHAR_TRAITS>::basic_string_view()
                                                          BSLS_KEYWORD_NOEXCEPT
: d_start_p(0)
, d_length(0)
{}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::basic_string_view(
                                              const CHAR_TYPE *characterString)
{
    BSLS_ASSERT_SAFE(characterString);

    d_start_p = characterString;
    d_length  = CHAR_TRAITS::length(characterString);

    BSLS_ASSERT_SAFE(d_length <= max_size());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::basic_string_view(
                                             const CHAR_TYPE  *characterString,
                                             size_type         numChars)
{
    BSLS_ASSERT_SAFE(characterString || (numChars == 0));
    BSLS_ASSERT_SAFE(numChars <= max_size());

    d_start_p = characterString;
    d_length  = numChars;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
template <class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::basic_string_view(
               const std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& str)
{
    d_start_p = str.data();
    d_length  = str.size();
}

// MANIPULATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
template <class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_string_view<CHAR_TYPE, CHAR_TRAITS>&
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::operator=(
               const std::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>& rhs)
BSLS_KEYWORD_NOEXCEPT
{
    d_start_p = rhs.data();
    d_length  = rhs.size();
    return *this;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP14
void
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::remove_prefix(size_type numChars)
{
    BSLS_ASSERT_SAFE(d_length >= numChars);
    d_start_p += numChars;
    d_length  -= numChars;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP14
void
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::remove_suffix(size_type numChars)
{
    BSLS_ASSERT_SAFE(d_length >= numChars);
    d_length -= numChars;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP14
void
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::swap(
                                basic_string_view& other) BSLS_KEYWORD_NOEXCEPT
{
    BloombergLP::bslalg::ScalarPrimitives::swap(d_length,  other.d_length);
    BloombergLP::bslalg::ScalarPrimitives::swap(d_start_p, other.d_start_p);
}

// ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_start_p;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return begin();
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::end() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return begin() + d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return end();
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reverse_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reverse_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::crbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reverse_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reverse_iterator
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::length() const BSLS_KEYWORD_NOEXCEPT
{
    return d_length;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::max_size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return (npos - 1) / sizeof(CHAR_TYPE);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::empty() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return (0 == d_length);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reference
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::operator[](size_type position) const
{
    BSLS_ASSERT_SAFE(position < length());

    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reference
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::at(size_type position) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                       "string_view::at(n): invalid position");
    }
    return *(begin() + position);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reference
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *begin();
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_reference
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(end() - 1);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::const_pointer
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_start_p;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::copy(CHAR_TYPE *characterString,
                                                size_type  numChars,
                                                size_type  position) const
{

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                             "string_view::copy(str,pos,n): invalid position");
    }

    BSLS_ASSERT_SAFE(characterString);

    if (numChars > length() - position) {
        numChars = length() - position;
    }

    // Check that the destination buffer start is not within the source.
    BSLS_ASSERT_SAFE(std::less<const CHAR_TYPE *>()(
                         characterString, d_start_p + position) ||
                     std::greater_equal<const CHAR_TYPE *>()(
                         characterString, d_start_p + position + numChars));

    CHAR_TRAITS::move(characterString, d_start_p + position, numChars);

    return numChars;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_string_view<CHAR_TYPE, CHAR_TRAITS>
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::substr(size_type position,
                                                  size_type numChars) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > length())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                               "string_view::substr(pos,n): invalid position");
    }

    const size_type maxLen = length() - position;

    return basic_string_view<CHAR_TYPE, CHAR_TRAITS>(
                                        data() + position,
                                        numChars < maxLen ? numChars : maxLen);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                           basic_string_view other) const BSLS_KEYWORD_NOEXCEPT
{
    return privateCompareRaw(size_type(0),
                             length(),
                             other.data(),
                             other.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                                                 size_type         position,
                                                 size_type         numChars,
                                                 basic_string_view other) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < position)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
            "const string_view<...>::compare(pos,n, other): invalid position");
    }

    if (numChars > length() - position) {
        numChars = length() - position;
    }
    return privateCompareRaw(position, numChars, other.data(), other.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                                         size_type         lhsPosition,
                                         size_type         lhsNumChars,
                                         basic_string_view other,
                                         size_type         otherPosition,
                                         size_type         otherNumChars) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
               "const string_view<...>::compare(pos,n,other,other_pos,other_n)"
               ": invalid lhs position");
    }

    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                             other.length() < otherPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
               "const string_view<...>::compare(pos,n,other,other_pos,other_n)"
               ": invalid rhs position");
    }

    if (otherNumChars > other.length() - otherPosition) {
        otherNumChars = other.length() - otherPosition;
    }
    return privateCompareRaw(lhsPosition,
                             lhsNumChars,
                             other.data() + otherPosition,
                             otherNumChars);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                                                  const CHAR_TYPE *other) const
{
    BSLS_ASSERT_SAFE(other);

    size_type otherLength = CHAR_TRAITS::length(other);

    return privateCompareRaw(size_type(0),
                             length(),
                             other,
                             otherLength);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                                                  size_type        lhsPosition,
                                                  size_type        lhsNumChars,
                                                  const CHAR_TYPE *other) const
{
    BSLS_ASSERT_SAFE(other);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
             "const string_view<...>::compare(pos,n,other): invalid position");
    }

    size_type otherLength = CHAR_TRAITS::length(other);

    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }

    return compare(lhsPosition,
                   lhsNumChars,
                   other,
                   otherLength);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
int basic_string_view<CHAR_TYPE, CHAR_TRAITS>::compare(
                                          size_type        lhsPosition,
                                          size_type        lhsNumChars,
                                          const CHAR_TYPE *other,
                                          size_type        otherNumChars) const
{
    BSLS_ASSERT_SAFE(other || 0 == otherNumChars);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(length() < lhsPosition)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                       "const string_view<...>::compare(pos,n,other,other_pos)"
                      ": invalid position");
    }

    if (lhsNumChars > length() - lhsPosition) {
        lhsNumChars = length() - lhsPosition;
    }
    return privateCompareRaw(lhsPosition,
                             lhsNumChars,
                             other,
                             otherNumChars);
}

#if defined(BSLSTL_STRINGVIEW_ENABLE_CPP20_METHODS)
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::starts_with(
                         basic_string_view subview) const BSLS_KEYWORD_NOEXCEPT
{
    return (size() >= subview.size() &&
            0 == compare(0, subview.size(), subview));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::starts_with(
                               CHAR_TYPE character) const BSLS_KEYWORD_NOEXCEPT
{
    return (!empty() && CHAR_TRAITS::eq(front(), character));
}


template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::starts_with(
                                        const CHAR_TYPE* characterString) const
{
    BSLS_ASSERT_SAFE(characterString);
    for (size_type i = 0; i < d_length; ++i) {
        if (characterString[i] == 0) {
            // Ran out of characterString, so is prefix.
            return true;                                              // RETURN
        }
        if (d_start_p[i] != characterString[i]) {
            // Mismatch.
            return false;                                             // RETURN
        }
    }
    // Ran out of string_view, so check characterString is not longer.
    return characterString[d_length] == 0;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::ends_with(
                         basic_string_view subview) const BSLS_KEYWORD_NOEXCEPT
{
    return (size() >= subview.size() &&
            0 == compare(size() - subview.size(), npos, subview));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::ends_with(
                               CHAR_TYPE character) const BSLS_KEYWORD_NOEXCEPT
{
    return (!empty() && CHAR_TRAITS::eq(back(), character));
}


template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
bool basic_string_view<CHAR_TYPE, CHAR_TRAITS>::ends_with(
                                        const CHAR_TYPE* characterString) const
{
    return ends_with(basic_string_view(characterString));
}
#endif

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
 BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return find(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find(
                                               const CHAR_TYPE *substring,
                                               size_type        position,
                                               size_type        numChars) const
{
    BSLS_ASSERT_SAFE(substring || (numChars == 0));

    size_type remChars = length() - position;
    if (position > length() || numChars > remChars) {
        return npos;                                                  // RETURN
    }
    if (0 == numChars) {
        return position;                                              // RETURN
    }
    const CHAR_TYPE *thisString = data() + position;
    const CHAR_TYPE *nextString = 0;
    for (remChars -= numChars - 1;
         0 !=
            (nextString = CHAR_TRAITS::find(thisString, remChars, *substring));
         remChars -= ++nextString - thisString, thisString = nextString) {
        if (0 == CHAR_TRAITS::compare(nextString, substring, numChars)) {
            return nextString - data();                               // RETURN
        }
    }

    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find(characterString,
                position,
                CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    if (position >= length()) {
        return npos;                                                  // RETURN
    }
    const CHAR_TYPE *result = CHAR_TRAITS::find(data() + position,
                                                length() - position,
                                                character);
    return result ? result - data() : npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rfind(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return rfind(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rfind(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (0 == numChars) {
        return position > length() ? length() : position;             // RETURN
    }
    if (numChars <= length()) {
        if (position > length() - numChars) {
            position = length() - numChars;
        }
        const CHAR_TYPE *thisString = data() + position;
        for (; position != npos; --thisString, --position) {
            if (0 == CHAR_TRAITS::compare(thisString,
                                          characterString,
                                          numChars)) {
                return position;                                      // RETURN
            }
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rfind(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return rfind(characterString,
                 position,
                 CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::rfind(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    return rfind(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_of(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_first_of(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_first_of(characterString,
                         position,
                         CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (0 < numChars && position < length()) {
        for (const CHAR_TYPE *current = data() + position;
             current != end();
             ++current)
        {
            if (CHAR_TRAITS::find(characterString, numChars, *current) != 0) {
                return current - data();                              // RETURN
            }
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_of(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_first_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_of(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_last_of(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_last_of(characterString,
                        position,
                        CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (0 < numChars && 0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = data() + remChars;
             current >= data();
             --current)
        {
            if (CHAR_TRAITS::find(characterString, numChars, *current)) {
                return current - data();                              // RETURN
            }
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_of(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_last_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
 BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_not_of(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_first_not_of(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_first_not_of(characterString,
                             position,
                             CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (position < length()) {
        for (const CHAR_TYPE *current = data() + position;
             current != end();
             ++current)
        {
            if (!CHAR_TRAITS::find(characterString, numChars, *current)) {
                return current - data();                              // RETURN
            }
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_first_not_of(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_first_not_of(&character, position, size_type(1));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_not_of(
                        basic_string_view subview,
                        size_type         position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_last_not_of(subview.data(), position, subview.length());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position) const
{
    BSLS_ASSERT_SAFE(characterString);

    return find_last_not_of(characterString,
                            position,
                            CHAR_TRAITS::length(characterString));
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_not_of(
                                              const CHAR_TYPE *characterString,
                                              size_type        position,
                                              size_type        numChars) const
{
    BSLS_ASSERT_SAFE(characterString || 0 == numChars);

    if (0 < length()) {
        size_type remChars = position < length() ? position : length() - 1;
        for (const CHAR_TYPE *current = data() + remChars;
             current >= data();
             --current)
        {
            if (!CHAR_TRAITS::find(characterString, numChars, *current)) {
                return current - data();                              // RETURN
            }
        }
    }
    return npos;
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
BSLS_KEYWORD_CONSTEXPR_CPP14
typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
basic_string_view<CHAR_TYPE, CHAR_TRAITS>::find_last_not_of(
                                CHAR_TYPE character,
                                size_type position) const BSLS_KEYWORD_NOEXCEPT
{
    return find_last_not_of(&character, position, size_type(1));
}

// PUBLIC ACCESSORS
template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
std::size_t hash<basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::
operator()(const basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input) const
{
    using ::BloombergLP::bslh::hashAppend;
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;
    hashAlg(input.data(), sizeof(CHAR_TYPE) * input.size());
    hashAppend(hashAlg, input.size());
    return static_cast<std::size_t>(hashAlg.computeHash());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
std::size_t hash<basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::
operator()(const CHAR_TYPE *input) const
{
    BSLS_ASSERT_SAFE(input);
    using ::BloombergLP::bslh::hashAppend;
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;

    std::size_t length = CHAR_TRAITS::length(input);

    hashAlg(input, sizeof(CHAR_TYPE) * length);
    hashAppend(hashAlg, length);
    return static_cast<std::size_t>(hashAlg.computeHash());
}

#if defined(BSLS_PLATFORM_CMP_SUN)  // {DRQS 132030795}

inline
std::size_t hash<string_view>::operator()(const string_view& input) const
{
    using ::BloombergLP::bslh::hashAppend;
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;
    hashAlg(input.data(), input.size());
    hashAppend(hashAlg, input.size());
    return static_cast<std::size_t>(hashAlg.computeHash());
}

inline
std::size_t hash<string_view>::operator()(const char *input) const
{
    BSLS_ASSERT_SAFE(input);
    using ::BloombergLP::bslh::hashAppend;
    std::size_t length = char_traits<char>::length(input);
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;
    hashAlg(input, length);
    hashAppend(hashAlg, length);
    return static_cast<std::size_t>(hashAlg.computeHash());
}

inline
std::size_t hash<wstring_view>::operator()(const wstring_view& input) const
{
    using ::BloombergLP::bslh::hashAppend;
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;
    hashAlg(input.data(), sizeof(wchar_t) * input.size());
    hashAppend(hashAlg, input.size());
    return static_cast<std::size_t>(hashAlg.computeHash());
}

inline
std::size_t hash<wstring_view>::operator()(const wchar_t *input) const
{
    BSLS_ASSERT_SAFE(input);
    using ::BloombergLP::bslh::hashAppend;
    std::size_t length = char_traits<wchar_t>::length(input);
    ::BloombergLP::bslh::Hash<>::HashAlgorithm hashAlg;
    hashAlg(input, sizeof(wchar_t) * length);
    hashAppend(hashAlg, length);
    return static_cast<std::size_t>(hashAlg.computeHash());
}

#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl_stringview_relops {

                            // ----------------------
                            // StringView_CompareUtil
                            // ----------------------

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(
                          StringView lhs, StringView rhs) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.size() == rhs.size()
        && 0 == CHAR_TRAITS::compare(lhs.data(), rhs.data(), lhs.size());
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
bool StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(
                          StringView lhs, StringView rhs) BSLS_KEYWORD_NOEXCEPT
{
    const std::size_t minLen = lhs.length() < rhs.length()
                             ? lhs.length() : rhs.length();

    int ret = CHAR_TRAITS::compare(lhs.data(), rhs.data(), minLen);
    if (0 == ret) {
        return lhs.length() < rhs.length();                           // RETURN
    }
    return (ret < 0);
}

}  // close namespace bslstl_stringview_relops
}  // close enterprise namespace

// FREE FUNCTIONS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
void bsl::swap(
            basic_string_view<CHAR_TYPE, CHAR_TRAITS>& a,
            basic_string_view<CHAR_TYPE, CHAR_TRAITS>& b) BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator==(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>        lhs,
           bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>        rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator==(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator==(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator!=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
           bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator!=(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator!=(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::equals(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
bool BloombergLP::bslstl_stringview_relops::
operator<(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              lhs,
          bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator<(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
          const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator<(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
          typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              lhs,
          bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>              rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
          const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
          typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator<=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
           bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator<=(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator<=(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(rhs, lhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>=(bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             lhs,
           bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>             rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>=(typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type lhs,
           const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
BSLS_KEYWORD_CONSTEXPR
bool BloombergLP::bslstl_stringview_relops::
operator>=(const bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS>            lhs,
           typename BloombergLP::bslstl::StringView_Identity<
                    bsl::basic_string_view<CHAR_TYPE, CHAR_TRAITS> >::type rhs)
BSLS_KEYWORD_NOEXCEPT
{
    return !StringView_CompareUtil<CHAR_TYPE, CHAR_TRAITS>::lessThan(lhs, rhs);
}

namespace bsl {

using BloombergLP::bslstl_stringview_relops::operator==;
using BloombergLP::bslstl_stringview_relops::operator!=;
using BloombergLP::bslstl_stringview_relops::operator<;
using BloombergLP::bslstl_stringview_relops::operator>;
using BloombergLP::bslstl_stringview_relops::operator<=;
using BloombergLP::bslstl_stringview_relops::operator>=;

}

template <class CHAR_TYPE, class CHAR_TRAITS>
std::basic_ostream<CHAR_TYPE>&
bsl::operator<<(std::basic_ostream<CHAR_TYPE>&            stream,
                basic_string_view<CHAR_TYPE, CHAR_TRAITS> stringView)
{
    typedef CHAR_TYPE                                           char_type;
    typedef typename std::basic_ostream<char_type>::ios_base    ios_base;
    typedef typename basic_string_view<CHAR_TYPE, CHAR_TRAITS>::size_type
                                                                size_type;

    size_type width = static_cast<size_type>(stream.width());
    size_type len = stringView.length();

    if (len < width) {
        bool leftAdjusted =
                    (stream.flags() & ios_base::adjustfield) == ios_base::left;

        char_type fillChar = stream.fill();

        if (leftAdjusted) {
            if (stringView.data()) {
                 stream.write(stringView.data(), stringView.length());
            }
        }

        for (size_type n = 0; n != width - len; ++n) {
            stream.put(fillChar);
        }

        if (!leftAdjusted) {
            if (stringView.data()) {
                 stream.write(stringView.data(), stringView.length());
            }
        }
    }
    else {
        if (stringView.data()) {
            stream.write(stringView.data(), stringView.length());
        }
    }

    stream.width(0);

    return stream;
}

// HASH SPECIALIZATIONS
template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS>
inline
void bsl::hashAppend(HASHALG&                                         hashAlg,
                     const basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.size());
    hashAppend(hashAlg, input.size());
}

#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
namespace BloombergLP {

template <class HASHALG, class CHAR_TYPE, class CHAR_TRAITS>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void bslh::hashAppend(
                 HASHALG&                                              hashAlg,
                 const std::basic_string_view<CHAR_TYPE, CHAR_TRAITS>& input)
{
    hashAlg(input.data(), sizeof(CHAR_TYPE)*input.size());
    hashAppend(hashAlg, input.size());
}

}  // close enterprise namespace
#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

#endif

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
