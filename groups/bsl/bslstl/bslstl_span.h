// bslstl_span.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_SPAN
#define INCLUDED_BSLSTL_SPAN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a (mostly) standard-compliant 'span' class template.
//
//@CLASSES:
//  bsl::span: C++03-compliant implementation of 'std::span'.
//
//@CANONICAL_HEADER: bslstl_span.h
//
//@SEE_ALSO: ISO C++ Standard
//
//@DESCRIPTION: This component provides the C+20 standard view type 'span',
// that is a view over a contiguous sequence of objects.  Note that if compiler
// supports the C++20 standard, then the 'std' implementation of 'span' is
// used.
//
// There are two implementations of 'span'; one for 'statically sized' (i. e.,
//  size fixed at compile time) spans, and 'dynamically sized' (size can be
//  altered at run-time).
//
// 'bsl::span' differs from 'std::span' in the following ways:
//: o The 'constexpr' inline symbol 'std::dynamic_extent' has been replaced by
//:   an enumeration for C++03 compatibility.
//: o A 'bsl::span' can be implicitly constructed from a 'bsl::array'.
//: o The implicit construction from an arbitrary container that supports
//:   'data()' and 'size()' is enabled only for C++11 and later.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Using Span To Pass A Portion Of An Array As A Container
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we already have an array of values of type 'TYPE', and we want to
// pass a subset of the array to a function, which is expecting some kind of a
// container.   We can create a 'span' from the array, and then pass that.
// Since the span is a 'view' into the array, i.e, the span owns no storage,
// the elements in the span are the same as the ones in the array.
//
// First, we create a template function that takes a generic container.  This
// function inspects each of the (numeric) values in the container, and if the
// low bit is set, flips it.  This has the effect of turning odd values into
// even values.
//..
//  template <class CONTAINER>
//  void MakeEven(CONTAINER &c)
//      // Make every value in the specified container 'c' even.
//  {
//      for (typename CONTAINER::iterator it = c.begin();
//                                        it != c.end();
//                                        ++it) {
//          if (*it & 1) {
//              *it ^= 1;
//          }
//      }
//  }
//..
//  We then create a span, and verify that it contains the values that we
//  expect, and pass it to 'MakeEven' to modify it.  Afterwards, we check that
//  none of the elements in the array that were not included in the span are
//  unchanged, and the ones in the span were.
//..
//  int            arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//  bsl::span<int> sp(arr + 3, 4);   // 4 elements, starting at 3.
//  for (int i = 0; i < 10; ++i)
//  {
//      assert(arr[i] == i);
//  }
//
//  assert(sp[0] == 3);
//  assert(sp[1] == 4);
//  assert(sp[2] == 5);
//  assert(sp[3] == 6);
//
//  MakeEven(sp);
//
//  assert(sp[0] == 2); // Has been changed
//  assert(sp[1] == 4);
//  assert(sp[2] == 4); // Has been changed
//  assert(sp[3] == 6);
//
//  assert(arr[0] == 0); // Not part of the span
//  assert(arr[1] == 1); // Not part of the span
//  assert(arr[2] == 2); // Not part of the span
//  assert(arr[3] == 2); // Has been changed
//  assert(arr[4] == 4);
//  assert(arr[5] == 4); // Has been changed
//  assert(arr[6] == 6);
//  assert(arr[7] == 7); // Not part of the span
//  assert(arr[8] == 8); // Not part of the span
//  assert(arr[9] == 9); // Not part of the span
//..
//
///Example 2: Returning A Subset Of A Container From A Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we already have a vector of values of type 'TYPE', and we want to
// return a (contiguous) subset of the vector from a function, which can then
// be processed processed using a range-based for loop.  To achieve that, we
// can use 'span' as the return type.  The calling code can then interate over
// the span as if it was a container.  Note that since the span doesn't own the
// elements of the vector, the span might become invalid when the vector is
// changed (or resized, or destroyed).
//
// First, we create the vector and define our function that returns a slice as
// a 'span'.
//..
//  bsl::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//
//  bsl::span<const int> slice(const bsl::vector<int>& vec,
//                             size_t                  first,
//                             size_t                  last)
//      // Return a span into the specified 'vec', starting at the specified
//      // 'first' index, and continuing up to (but not including) the
//      // specified 'last' index.
//  {
//      return bsl::span<const int>(vec.data() + first, last-first);
//  }
//..
//  We can now iterate over the elements in the slice using the span:
//..
//  bsl::span<const int> sp = slice(v, 4, 7);
//  int            val = 4;
//  for (int x: sp) {
//       assert(x == val++);
//   }
//..
//  Note that we can use the return value directly and avoid declaring the
//  variable 'sp':
//..
//   val = 2;
//   for (int x: slice(v, 2, 8)) {
//       assert(x == val++);
//   }
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#include <span>
namespace bsl {
    using std::dynamic_extent;
    using std::span;
    using std::as_bytes;
    using std::as_writable_bytes;
}
#else

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>
#include <bslmf_removepointer.h>
#include <bslmf_removecv.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_nullptr.h>

#include <bslstl_array.h>
#include <bslstl_iterator.h>
#include <bslstl_vector.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <array>     // 'std::array'
#endif
#include <iterator>  // 'reverse_iterator', '!='
#include <stddef.h>  // 'size_t', 'NULL', 'std::byte'

namespace bsl {

enum { dynamic_extent = size_t(-1) };

template <class TYPE, size_t EXTENT = dynamic_extent> class span;


                        // ===================
                        // struct Span_Utility
                        // ===================
struct Span_Utility
    // This component-private struct provides a namespace for meta-programming
    // utilities used by the 'span' implementation.
{
    // PUBLIC TYPES
    template <class FROM, class TO>
    struct IsArrayConvertible : public bsl::integral_constant<bool,
                              bsl::is_convertible<FROM(*)[], TO(*)[]>::value>
    {};

    template <class TYPE, size_t EXTENT, size_t COUNT, size_t OFFSET>
    struct SubspanReturnType
    {
        // PUBLIC TYPES
        typedef bsl::span<TYPE, COUNT != dynamic_extent
          ? COUNT
          : EXTENT - OFFSET> type;

    };

    template <class TYPE>
    struct TypeIdentity
    {
        // PUBLIC TYPES
        typedef TYPE type;
    };

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class TP>
    struct IsSpanImpl : public bsl::false_type {};

    template <class TP, size_t SZ>
    struct IsSpanImpl<span<TP, SZ> > : public bsl::true_type {};

    template <class TP>
    struct IsSpan : public IsSpanImpl<typename bsl::remove_cv<TP>::type> {};

    template <class TP>
    struct IsBSLArrayImpl : public bsl::false_type {};

    template <class TP, size_t SZ>
    struct IsBSLArrayImpl<bsl::array<TP, SZ> > : public bsl::true_type {};

    template <class TP>
    struct IsBSLArray
                : public IsBSLArrayImpl<typename bsl::remove_cv<TP>::type> {};

    template <class TP>
    struct IsSTDArrayImpl : public bsl::false_type {};

    template <class TP, size_t SZ>
    struct IsSTDArrayImpl<std::array<TP, SZ> > : public bsl::true_type {};

    template <class TP>
    struct IsSTDArray
                : public IsSTDArrayImpl<typename bsl::remove_cv<TP>::type> {};

    template <class TP, class ELEMENT_TYPE, class = void>
    struct IsSpanCompatibleContainer : public bsl::false_type {};

    template <class TP, class ELEMENT_TYPE>
    struct IsSpanCompatibleContainer<TP, ELEMENT_TYPE,
        bsl::void_t<
        // is not a specialization of span
            typename bsl::enable_if<!IsSpan<TP>::value, bsl::nullptr_t>::type,
        // is not a specialization of bsl::array
            typename bsl::enable_if<
                                 !IsBSLArray<TP>::value, bsl::nullptr_t>::type,
        // is not a specialization of std::array
            typename bsl::enable_if<
                                 !IsSTDArray<TP>::value, bsl::nullptr_t>::type,
        // is not a C-style array
            typename bsl::enable_if<
                              !bsl::is_array<TP>::value, bsl::nullptr_t>::type,
        // data(cont) and size(cont) are well formed
            decltype(data(std::declval<TP>())),
            decltype(size(std::declval<TP>())),
        // The underlying types are compatible
            typename bsl::enable_if<
                Span_Utility::IsArrayConvertible<
                   typename bsl::remove_pointer<
                                   decltype(data(std::declval<TP &>()))>::type,
                   ELEMENT_TYPE>::value,
                bsl::nullptr_t>::type
            > >
        : public bsl::true_type {};
#endif

};

template <class TYPE, size_t EXTENT>
class span {

  public:
    // PUBLIC TYPES
    typedef TYPE                                   element_type;
    typedef typename bsl::remove_cv<TYPE>::type    value_type;
    typedef size_t                                 size_type;
    typedef ptrdiff_t                              difference_type;
    typedef TYPE                                  *pointer;
    typedef const TYPE                            *const_pointer;
    typedef TYPE&                                  reference;
    typedef const TYPE&                            const_reference;
    typedef pointer                                iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03  // Constant ... names must begin with 's_' or 'k_'
    // PUBLIC CLASS DATA
    static const size_type extent = EXTENT;
// BDE_VERIFY pragma: pop

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 span() BSLS_KEYWORD_NOEXCEPT;
        // Construct an empty 'span' object.  The behavior is undefined unless
        // '0 == EXTENT'

    BSLS_KEYWORD_CONSTEXPR_CPP14 span(const span& original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a span that refers to the same data as the specified
        // 'original' object.

    BSLS_KEYWORD_CONSTEXPR_CPP14 explicit span(pointer ptr, size_type count);
        // Construct a span that refers to the specified 'count' consecutive
        // objects starting from the specified 'ptr'.  The behavior is
        // undefined unless 'EXTENT == count'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 explicit span(pointer first, pointer last);
        // Construct a span from the specified 'first' and specified 'last'.
        // The behavior is undefined unless
        // 'EXTENT == bsl::distance(first, last)'.

    template <size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
          typename Span_Utility::TypeIdentity<element_type>::type (&arr)[SIZE])
          BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified C-style array 'arr'.  The
        // behavior is undefined unless 'SIZE == EXTENT'.

#ifndef BSLSTL_ARRAY_IS_ALIASED
    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
       bsl::array<t_OTHER_TYPE, EXTENT>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified bsl::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.

    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
     const bsl::array<t_OTHER_TYPE, EXTENT>& arr,
     typename bsl::enable_if<
         Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
         void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified bsl::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
           std::array<t_OTHER_TYPE, EXTENT>& arr,
           typename bsl::enable_if<
               Span_Utility::IsArrayConvertible<
                                            t_OTHER_TYPE, element_type>::value,
               void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified std::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.

    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
     const std::array<t_OTHER_TYPE, EXTENT>& arr,
     typename bsl::enable_if<
         Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
         void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified std::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.
#endif

    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
           const span<t_OTHER_TYPE, EXTENT>& other,
           typename bsl::enable_if<
               Span_Utility::IsArrayConvertible<
                                            t_OTHER_TYPE, element_type>::value,
               void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified span 'other'.  This constructor
        // participates in overload resolution only if 't_OTHER_TYPE(*)[]' is
        // convertible to 'element_type(*)[]'.

    template <class t_OTHER_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
            const span<t_OTHER_TYPE, dynamic_extent>& other,
            typename bsl::enable_if<
               Span_Utility::IsArrayConvertible<
                                            t_OTHER_TYPE, element_type>::value,
               void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified span 'other'.  This constructor
        // participates in overload resolution only if 't_OTHER_TYPE(*)[]' is
        // convertible to 'element_type(*)[]'.  The behavior is undefined
        // unless 'other.size() == EXTENT'.

    //  ~span() noexcept = default;
        // Destroy this object.

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 reference back() const BSLS_KEYWORD_NOEXCEPT
        // Return a reference to the last element of this span.  The behavior
        // is undefined if this span is empty.
    {
        // Implemented inline because of Sun/AIX compiler limitations.
        BSLMF_ASSERT(EXTENT > 0);
        return d_data_p[size() - 1];
    }

    BSLS_KEYWORD_CONSTEXPR pointer data() const BSLS_KEYWORD_NOEXCEPT;
        // Return a pointer to the data referenced by this span.

    BSLS_KEYWORD_CONSTEXPR bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this span contains no elements and 'false'
        // otherwise.

    template <size_t COUNT>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, COUNT> first() const BSLS_KEYWORD_NOEXCEPT;
        // Return a statically-sized span consisting of the first 'COUNT'
        // elements of this span.  The behavior is undefined unless
        // 'COUNT <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, dynamic_extent>
    first(size_type count) const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span consisting of the first (specified)
        // 'count' elements of this span.  The behavior is undefined unless
        // 'count <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 reference front() const BSLS_KEYWORD_NOEXCEPT
        // Return a reference to the first element of this span.  The behavior
        // is undefined if this span is empty.
    {
        // Implemented inline because of Sun/AIX compiler limitations.
        BSLMF_ASSERT(EXTENT > 0);
        return d_data_p[0];
    }

    template <size_t COUNT>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, COUNT> last() const BSLS_KEYWORD_NOEXCEPT;
        // Return a statically-sized span consisting of the last 'COUNT'
        // elements of this span.  The behavior is undefined unless
        // 'COUNT <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, dynamic_extent>
    last(size_type count) const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span consisting of the last (specified)
        // 'count' elements of this span.  The behavior is undefined unless
        // 'count <= size()'.

    BSLS_KEYWORD_CONSTEXPR size_type size() const BSLS_KEYWORD_NOEXCEPT
        // Return the size of this span.
    {
        // Implemented inline because of Sun/AIX compiler limitations.
        return EXTENT;
    }

    BSLS_KEYWORD_CONSTEXPR size_type size_bytes() const BSLS_KEYWORD_NOEXCEPT;
        // Return the size of this span in bytes.

    template <size_t OFFSET,
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
              size_t COUNT = dynamic_extent>
#else
              size_t COUNT>
#endif
    BSLS_KEYWORD_CONSTEXPR_CPP14
    typename Span_Utility::SubspanReturnType<TYPE, EXTENT, COUNT, OFFSET>::type
    subspan() const BSLS_KEYWORD_NOEXCEPT
        // If the template parameter 'COUNT' is 'dynamic_extent', return a
        // dynamically-sized span consisting consisting of the elements of this
        // span in the half-open range '[OFFSET, EXTENT)'.  Otherwise, return a
        // statically-sized span consisting of the elements of this span in the
        // half-open range '[OFFSET, OFFSET+COUNT)'.  The behavior is undefined
        // unless 'OFFSET <= EXTENT'.  If 'COUNT != dynamic_extent', the
        // behavior is undefined unless 'OFFSET + COUNT <= EXTENT'.
    {
        // Implemented inline because of Sun/AIX compiler limitations.
        typedef typename
            Span_Utility::SubspanReturnType<TYPE, EXTENT, COUNT, OFFSET>::type
                                                                    ReturnType;
        BSLMF_ASSERT(OFFSET <= EXTENT);
        BSLMF_ASSERT(COUNT == dynamic_extent || OFFSET + COUNT <= EXTENT);
        return ReturnType(data() + OFFSET,
                            COUNT == dynamic_extent ? size() - OFFSET : COUNT);
    }

    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, dynamic_extent>
    subspan(size_type offset, size_type count = dynamic_extent)
                                                   const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span starting at the specified 'offset'.
        // If the optionally specified 'count' is 'dynamic_extent', the span
        // will consist of the half-open range '[offset, size () - offset)' and
        // the behavior is undefined if 'offset > size()'.  Otherwise, the span
        // will consist of the half-open range '[offset, count)' and the
        // behavior is undefined if 'offset + count > size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference operator[](size_type index) const BSLS_KEYWORD_NOEXCEPT
        // Return a reference to the element at the specified 'index'.  The
        // behavior is undefined unless 'index < size()'.
    {
        // Implemented inline because of Sun/AIX compiler limitations.
        BSLS_ASSERT(index < size());
        return d_data_p[index];
    }

    //                      ITERATOR OPERATIONS
    BSLS_KEYWORD_CONSTEXPR_CPP14 iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // of this span, and the past-the-end iterator if this span is empty.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end iterator providing modifiable access to this
        //  span.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element of this span, and the past-the-end reverse iterator if this
        // span is empty.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this span.

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span& operator=(const span&) BSLS_KEYWORD_NOEXCEPT;
        // Assign to this span the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this span.

    BSLS_KEYWORD_CONSTEXPR_CPP14 void swap(span &other) BSLS_KEYWORD_NOEXCEPT;
        // Exchange the value of this span with the value of the specified
        // 'other' object.

  private:
    // DATA
    pointer    d_data_p;
};


template <class TYPE>
class span<TYPE, dynamic_extent> {
  public:
    // PUBLIC TYPES
    typedef TYPE                                   element_type;
    typedef typename bsl::remove_cv<TYPE>::type    value_type;
    typedef size_t                                 size_type;
    typedef ptrdiff_t                              difference_type;
    typedef TYPE                                  *pointer;
    typedef const TYPE                            *const_pointer;
    typedef TYPE&                                  reference;
    typedef const TYPE&                            const_reference;
    typedef pointer                                iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03  // Constant ... names must begin with 's_' or 'k_'
    // PUBLIC CLASS DATA
    static const size_type extent = dynamic_extent;
// BDE_VERIFY pragma: pop

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 span() BSLS_KEYWORD_NOEXCEPT;
        // Construct an empty 'span' object.

    BSLS_KEYWORD_CONSTEXPR_CPP14 span(const span& original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a span that refers to the same data as the specified
        // 'original' object.

    BSLS_KEYWORD_CONSTEXPR_CPP14 span(pointer ptr, size_type count);
        // Construct a span that refers to the specified 'count' consecutive
        // objects starting from the specified 'ptr'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 span(pointer first, pointer last);
        // Construct a span from the specified 'first' and specified 'last'.

    template <size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
          typename Span_Utility::TypeIdentity<element_type>::type (&arr)[SIZE])
          BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified C-style array 'arr'.

#ifndef BSLSTL_ARRAY_IS_ALIASED
    template <class t_OTHER_TYPE, size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(bsl::array<t_OTHER_TYPE, SIZE>& arr,
           typename bsl::enable_if<
               Span_Utility::IsArrayConvertible<
                                            t_OTHER_TYPE, element_type>::value,
               void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified bsl::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.

    template <class t_OTHER_TYPE, size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
                                     const bsl::array<t_OTHER_TYPE, SIZE>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
           void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified bsl::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class t_OTHER_TYPE, size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(std::array<t_OTHER_TYPE, SIZE>& arr,
           typename bsl::enable_if<
               Span_Utility::IsArrayConvertible<
                                            t_OTHER_TYPE, element_type>::value,
               void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified std::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.

    template <class t_OTHER_TYPE, size_t SIZE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
     const std::array<t_OTHER_TYPE, SIZE>& arr,
     typename bsl::enable_if<
         Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
         void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified std::array 'arr'.  This
        // constructor participates in overload resolution only if
        // 't_OTHER_TYPE(*)[]' is convertible to 'element_type(*)[]'.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    template <class CONTAINER>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
         CONTAINER& c,
         typename bsl::enable_if<
             Span_Utility::IsSpanCompatibleContainer<CONTAINER, TYPE>::value,
             void *>::type = NULL)
    : d_data_p(bsl::data(c))
    , d_size(bsl::size(c))
    {
    }

    template <class CONTAINER>
    BSLS_KEYWORD_CONSTEXPR_CPP14 span(
       const CONTAINER& c,
       typename bsl::enable_if<
         Span_Utility::IsSpanCompatibleContainer<const CONTAINER, TYPE>::value,
         void *>::type = NULL)
    : d_data_p(bsl::data(c))
    , d_size(bsl::size(c))
    {
    }
#endif

    template <class t_OTHER_TYPE, size_t OTHER_EXTENT>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span(const span<t_OTHER_TYPE, OTHER_EXTENT>& other,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type = NULL) BSLS_KEYWORD_NOEXCEPT;
        // Construct a span from the specified span 'other'.  This constructor
        // participates in overload resolution only if 't_OTHER_TYPE(*)[]' is
        // convertible to 'element_type(*)[]'.

    //  ~span() noexcept = default;
        // Destroy this object.

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 reference back() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the last element of this span.  The behavior
        // is undefined if this span is empty.

    BSLS_KEYWORD_CONSTEXPR pointer data() const BSLS_KEYWORD_NOEXCEPT;
        // Return a pointer to the data referenced by this span.

    BSLS_KEYWORD_CONSTEXPR bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if 'size() == 0' and 'false' otherwise.

    template <size_t COUNT>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, COUNT> first() const BSLS_KEYWORD_NOEXCEPT;
        // Return a statically-sized span consisting of the first 'COUNT'
        // elements of this span.  The behavior is undefined unless
        // 'COUNT <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, dynamic_extent>
    first(size_type count) const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span consisting of the first (specified)
        // 'count' elements of this span.  The behavior is undefined unless
        // 'count <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 reference front() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the first element of this span.  The behavior
        // is undefined if this span is empty.

    template <size_t COUNT>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, COUNT> last() const BSLS_KEYWORD_NOEXCEPT;
        // Return a statically-sized span consisting of the last 'COUNT'
        // elements of this span.  The behavior is undefined unless
        // 'COUNT <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, dynamic_extent>
    last(size_type count) const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span consisting of the last (specified)
        // 'count' elements of this span.  The behavior is undefined unless
        // 'count <= size()'.

    BSLS_KEYWORD_CONSTEXPR size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the size of this span.

    BSLS_KEYWORD_CONSTEXPR size_type size_bytes() const BSLS_KEYWORD_NOEXCEPT;
        // Return the size of this span in bytes.

    template <size_t OFFSET,
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
              size_t COUNT = dynamic_extent>
#else
              size_t COUNT>
#endif
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span<element_type, COUNT> subspan() const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span consisting of the 'COUNT' elements
        // of this span starting at 'OFFSET'.  The behavior is undefined unless
        // 'COUNT + OFFSET <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 span<element_type, dynamic_extent>
    subspan(size_type offset, size_type count = dynamic_extent)
                                                   const BSLS_KEYWORD_NOEXCEPT;
        // Return a dynamically-sized span starting at the specified 'offset'.
        // If the optionally specified 'count' is 'dynamic_extent', the span
        // will consist of the half-open range '[offset, size () - offset)' and
        // the behavior is undefined unless 'offset <= size()'.  Otherwise, the
        // span will consist of the half-open range '[offset, count)' and the
        // behavior is undefined unless 'offset + count <= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference operator[](size_type index) const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the element at the specified 'index'.  The
        // behavior is undefined unless 'index < size()'.

    //                      ITERATOR OPERATIONS
    BSLS_KEYWORD_CONSTEXPR_CPP14 iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // of this span, and the past-the-end iterator if this span is empty.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end iterator providing modifiable access to this
        //  span.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element of this span, and the past-the-end reverse iterator if this
        // span is empty.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this span.

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14
    span& operator=(const span& rhs) BSLS_KEYWORD_NOEXCEPT
        // Assign to this span the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this span.
    {
        d_data_p = rhs.d_data_p;
        d_size = rhs.d_size;
        return *this;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP14 void swap(span &other) BSLS_KEYWORD_NOEXCEPT;
        // Exchange the value of this span with the value of the specified
        // 'other' object.

  private:
    // DATA
    pointer   d_data_p;
    size_type d_size;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <class TYPE, size_t SIZE>
span(TYPE (&)[SIZE]) -> span<TYPE, SIZE>;
    // Deduce the template parameters 'TYPE' and 'SIZE' from the type and size
    // of the array supplied to the constructor of 'span'.

#ifndef BSLSTL_ARRAY_IS_ALIASED
template <class TYPE, size_t SIZE>
span(bsl::array<TYPE, SIZE> &) -> span<TYPE, SIZE>;
    // Deduce the template parameters 'TYPE' and 'SIZE' from the corresponding
    // template parameters of the 'bsl::array' supplied to the constructor of
    // 'span'.

template <class TYPE, size_t SIZE>
span(const bsl::array<TYPE, SIZE> &) -> span<const TYPE, SIZE>;
    // Deduce the template parameters 'TYPE' and 'SIZE' from the corresponding
    // template parameters of the 'bsl::array' supplied to the constructor of
    // 'span'.
#endif

template <class TYPE, size_t SIZE>
span(std::array<TYPE, SIZE> &) -> span<TYPE, SIZE>;
    // Deduce the template parameters 'TYPE' and 'SIZE' from the corresponding
    // template parameters of the 'std::array' supplied to the constructor of
    // 'span'.

template <class TYPE, size_t SIZE>
span(const std::array<TYPE, SIZE> &) -> span<const TYPE, SIZE>;
    // Deduce the template parameters 'TYPE' and 'SIZE' from the corresponding
    // template parameters of the 'std::array' supplied to the constructor of
    // 'span'.

template <class TYPE, class ALLOCATOR>
span(bsl::vector<TYPE, ALLOCATOR> &) -> span<TYPE>;
    // Deduce the template parameters 'TYPE' from the corresponding template
    // parameter of the 'bsl::vector' supplied to the constructor of 'span'.

template <class TYPE, class ALLOCATOR>
span(const bsl::vector<TYPE, ALLOCATOR> &) -> span<const TYPE>;
    // Deduce the template parameters 'TYPE' from the corresponding template
    // parameter of the 'bsl::vector' supplied to the constructor of 'span'.
#endif

// FREE FUNCTIONS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 span<const std::byte, EXTENT * sizeof(TYPE)>
as_bytes(span<TYPE, EXTENT> s) BSLS_KEYWORD_NOEXCEPT;
    // Return a span referring to same data as the specified 's', but
    // referring to the data as a span of non-modifiable bytes.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 span<const std::byte, dynamic_extent>
as_bytes(span<TYPE, dynamic_extent> s) BSLS_KEYWORD_NOEXCEPT;
    // Return a span referring to same data as the specified 's', but
    // referring to the data as a span of non-modifiable bytes.

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 span<std::byte, EXTENT * sizeof(TYPE)>
as_writable_bytes(span<TYPE, EXTENT> s) BSLS_KEYWORD_NOEXCEPT;
    // Return a span referring to same data as the specified 's', but
    // referring to the data as a span of modifiable bytes.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 span<std::byte, dynamic_extent>
as_writable_bytes(span<TYPE, dynamic_extent> s) BSLS_KEYWORD_NOEXCEPT;
    // Return a span referring to same data as the specified 's', but
    // referring to the data as a span of modifiable bytes.

#endif

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 void swap(span<TYPE, EXTENT>& a,
                                       span<TYPE, EXTENT>& b)
                                                         BSLS_KEYWORD_NOEXCEPT;
    // Exchange the value of the specified 'a' object with the value of the
    // specified 'b' object.

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                            // ----------------
                            // class span<T, N>
                            // ----------------

// CREATORS
template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span() BSLS_KEYWORD_NOEXCEPT
: d_data_p(NULL)
{
    BSLMF_ASSERT(EXTENT == 0);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(const span &original) BSLS_KEYWORD_NOEXCEPT
: d_data_p(original.d_data_p)
{
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(pointer ptr, size_type count)
: d_data_p(ptr)
{
    (void)count;
    BSLS_ASSERT(EXTENT == count);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(pointer first, pointer last)
: d_data_p(first)
{
    (void)last;
    BSLS_ASSERT(EXTENT == bsl::distance(first, last));
}


template <class TYPE, size_t EXTENT>
template <size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(
     typename bsl::Span_Utility::TypeIdentity<element_type>::type (&arr)[SIZE])
                                                          BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr)
{
    BSLMF_ASSERT(SIZE == EXTENT);
}

#ifndef BSLSTL_ARRAY_IS_ALIASED
template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(bsl::array<t_OTHER_TYPE, EXTENT>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
{
}

template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(const bsl::array<t_OTHER_TYPE, EXTENT>& arr,
    typename bsl::enable_if<
        Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
        void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
{
}
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(std::array<t_OTHER_TYPE, EXTENT>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
{
}

template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(const std::array<t_OTHER_TYPE, EXTENT>& arr,
   typename bsl::enable_if<
     Span_Utility::IsArrayConvertible<const t_OTHER_TYPE, element_type>::value,
     void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
{
}
#endif

template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(const bsl::span<t_OTHER_TYPE, EXTENT>& other,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(other.data())
{
}


template <class TYPE, size_t EXTENT>
template <class t_OTHER_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>::span(
       const bsl::span<t_OTHER_TYPE, bsl::dynamic_extent>& other,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(other.data())
{
    BSLS_ASSERT(EXTENT == other.size());
}

// ACCESSORS
template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR inline
typename bsl::span<TYPE, EXTENT>::pointer
bsl::span<TYPE, EXTENT>::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR inline
bool bsl::span<TYPE, EXTENT>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == EXTENT;
}

template <class TYPE, size_t EXTENT>
template <size_t COUNT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, COUNT>
bsl::span<TYPE, EXTENT>::first() const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, COUNT> ReturnType;
    BSLMF_ASSERT(COUNT <= EXTENT);
    return ReturnType(data(), COUNT);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, EXTENT>::first(size_type count) const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(count <= size());
    return ReturnType(data(), count);
}

template <class TYPE, size_t EXTENT>
template <size_t COUNT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, COUNT>
bsl::span<TYPE, EXTENT>::last() const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, COUNT> ReturnType;
    BSLMF_ASSERT(COUNT <= EXTENT);
    return ReturnType(data() + size() - COUNT, COUNT);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, EXTENT>::last(size_type count) const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(count <= size());
    return ReturnType(data() + size() - count, count);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR inline
typename bsl::span<TYPE, EXTENT>::size_type
bsl::span<TYPE, EXTENT>::size_bytes() const BSLS_KEYWORD_NOEXCEPT
{
    return EXTENT * sizeof(element_type);
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, EXTENT>::subspan(size_type offset, size_type count) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(offset <= size());
    BSLS_ASSERT(count  <= size() || count == bsl::dynamic_extent);
    if (count == bsl::dynamic_extent)
        return ReturnType(data() + offset, size() - offset);          // RETURN

    BSLS_ASSERT(offset <= size() - count);
    return ReturnType(data() + offset, count);
}

//                          ITERATOR OPERATIONS
template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, EXTENT>::iterator
bsl::span<TYPE, EXTENT>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return iterator(data());
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, EXTENT>::iterator
bsl::span<TYPE, EXTENT>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return iterator(data() + size());
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, EXTENT>::reverse_iterator
bsl::span<TYPE, EXTENT>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, EXTENT>::reverse_iterator
bsl::span<TYPE, EXTENT>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

// MANIPULATORS
template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, EXTENT>&
bsl::span<TYPE, EXTENT>::operator=(const span &rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_data_p = rhs.d_data_p;
    return *this;
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
void bsl::span<TYPE, EXTENT>::swap(span &other) BSLS_KEYWORD_NOEXCEPT
{
    pointer p = d_data_p;
    d_data_p = other.d_data_p;
    other.d_data_p = p;
}

              // -----------------------------------------
              // class span<T, bsl::dynamic_extent>
              // -----------------------------------------

// CREATORS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span() BSLS_KEYWORD_NOEXCEPT
: d_data_p(NULL)
, d_size(0)
{
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(const span& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_data_p(original.d_data_p)
, d_size(original.d_size)
{
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(pointer ptr, size_type count)
: d_data_p(ptr)
, d_size(count)
{
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(pointer first, pointer last)
: d_data_p(first)
, d_size(bsl::distance(first, last))
{
}


template <class TYPE>
template <size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
     typename bsl::Span_Utility::TypeIdentity<element_type>::type (&arr)[SIZE])
                                                          BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr)
, d_size(SIZE)
{
}

#ifndef BSLSTL_ARRAY_IS_ALIASED
template <class TYPE>
template <class t_OTHER_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
       bsl::array<t_OTHER_TYPE, SIZE>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
, d_size(SIZE)
{
}

template <class TYPE>
template <class t_OTHER_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
  const bsl::array<t_OTHER_TYPE, SIZE>& arr,
  typename bsl::enable_if<
     Span_Utility::IsArrayConvertible<const t_OTHER_TYPE, element_type>::value,
     void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
, d_size(SIZE)
{
}
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class TYPE>
template <class t_OTHER_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
       std::array<t_OTHER_TYPE, SIZE>& arr,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
, d_size(SIZE)
{
}

template <class TYPE>
template <class t_OTHER_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
    const std::array<t_OTHER_TYPE, SIZE>& arr,
        typename bsl::enable_if<
            Span_Utility::IsArrayConvertible<
                                      const t_OTHER_TYPE, element_type>::value,
            void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(arr.data())
, d_size(SIZE)
{
}
#endif

template <class TYPE>
template <class t_OTHER_TYPE, size_t OTHER_EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>::span(
       const bsl::span<t_OTHER_TYPE, OTHER_EXTENT>& other,
       typename bsl::enable_if<
           Span_Utility::IsArrayConvertible<t_OTHER_TYPE, element_type>::value,
           void *>::type) BSLS_KEYWORD_NOEXCEPT
: d_data_p(other.data())
, d_size(other.size())
{
}

// ACCESSORS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::reference
bsl::span<TYPE, bsl::dynamic_extent>::back() const BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT(size() > 0);
    return d_data_p[size() - 1];
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
typename bsl::span<TYPE, bsl::dynamic_extent>::pointer
bsl::span<TYPE, bsl::dynamic_extent>::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
bool
bsl::span<TYPE, bsl::dynamic_extent>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == size();
}

template <class TYPE>
template <size_t COUNT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, COUNT>
bsl::span<TYPE, bsl::dynamic_extent>::first() const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, COUNT> ReturnType;
    BSLS_ASSERT(COUNT <= size());
    return ReturnType(data(), COUNT);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, bsl::dynamic_extent>::first(size_type count) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(count <= size());
    return ReturnType(data(), count);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::reference
bsl::span<TYPE, bsl::dynamic_extent>::front() const BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT(size() > 0);
    return d_data_p[0];
}

template <class TYPE>
template <size_t COUNT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, COUNT>
bsl::span<TYPE, bsl::dynamic_extent>::last() const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, COUNT> ReturnType;
    BSLS_ASSERT(COUNT <= size());
    return ReturnType(data() + size() - COUNT, COUNT);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, bsl::dynamic_extent>::last(size_type count) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(count <= size());
    return ReturnType(data() + size() - count, count);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
typename bsl::span<TYPE, bsl::dynamic_extent>::size_type
bsl::span<TYPE, bsl::dynamic_extent>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
typename bsl::span<TYPE, bsl::dynamic_extent>::size_type
bsl::span<TYPE, bsl::dynamic_extent>::size_bytes() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return size() * sizeof(element_type);
}

template <class TYPE>
template <size_t OFFSET, size_t COUNT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, COUNT>
bsl::span<TYPE, bsl::dynamic_extent>::subspan() const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, COUNT> ReturnType;
    BSLS_ASSERT(OFFSET <= size());
    BSLS_ASSERT(COUNT == bsl::dynamic_extent || OFFSET + COUNT <= size());
    return ReturnType(data() + OFFSET,
                COUNT == bsl::dynamic_extent ? size() - OFFSET : COUNT);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<TYPE, bsl::dynamic_extent>
bsl::span<TYPE, bsl::dynamic_extent>::subspan(
                                   size_type offset,
                                   size_type count) const BSLS_KEYWORD_NOEXCEPT
{
    typedef bsl::span<TYPE, bsl::dynamic_extent> ReturnType;
    BSLS_ASSERT(offset <= size());
    BSLS_ASSERT(count  <= size() || count == bsl::dynamic_extent);
    if (count == bsl::dynamic_extent)
        return ReturnType(data() + offset, size() - offset);          // RETURN

    BSLS_ASSERT(offset <= size() - count);
    return ReturnType(data() + offset, count);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::reference
bsl::span<TYPE, bsl::dynamic_extent>::operator[](size_type index) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT(index < size());
    return d_data_p[index];
}

// MANIPULATORS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
void bsl::span<TYPE, bsl::dynamic_extent>::swap(span &other)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    pointer p = d_data_p;
    d_data_p = other.d_data_p;
    other.d_data_p = p;

    size_t sz = d_size;
    d_size = other.d_size;
    other.d_size = sz;
}

//                          ITERATOR OPERATIONS
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::iterator
bsl::span<TYPE, bsl::dynamic_extent>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return iterator(data());
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::iterator
bsl::span<TYPE, bsl::dynamic_extent>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return iterator(data() + size());
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::reverse_iterator
bsl::span<TYPE, bsl::dynamic_extent>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
typename bsl::span<TYPE, bsl::dynamic_extent>::reverse_iterator
bsl::span<TYPE, bsl::dynamic_extent>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

// FREE FUNCTIONS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SAL01:  //  Possible strict-aliasing violation

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<const std::byte, EXTENT * sizeof(TYPE)>
bsl::as_bytes(bsl::span<TYPE, EXTENT> s) BSLS_KEYWORD_NOEXCEPT
{
    return bsl::span<const std::byte, EXTENT * sizeof(TYPE)> (
                                 reinterpret_cast<const std::byte *>(s.data()),
                                 s.size_bytes());
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<const std::byte, bsl::dynamic_extent>
bsl::as_bytes(bsl::span<TYPE, bsl::dynamic_extent> s) BSLS_KEYWORD_NOEXCEPT
{
    return bsl::span<const std::byte, bsl::dynamic_extent>(
                                 reinterpret_cast<const std::byte *>(s.data()),
                                 s.size_bytes());
}

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<std::byte, EXTENT * sizeof(TYPE)>
bsl::as_writable_bytes(bsl::span<TYPE, EXTENT> s) BSLS_KEYWORD_NOEXCEPT
{
    return bsl::span<std::byte, EXTENT * sizeof(TYPE)>(
                                       reinterpret_cast<std::byte *>(s.data()),
                                       s.size_bytes());
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
bsl::span<std::byte, bsl::dynamic_extent>
bsl::as_writable_bytes(bsl::span<TYPE, bsl::dynamic_extent> s)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return bsl::span<std::byte, bsl::dynamic_extent>(
                                       reinterpret_cast<std::byte *>(s.data()),
                                       s.size_bytes());
}

// BDE_VERIFY pragma: pop
#endif

template <class TYPE, size_t EXTENT>
BSLS_KEYWORD_CONSTEXPR_CPP14 inline
void
bsl::swap(bsl::span<TYPE, EXTENT>& a, bsl::span<TYPE, EXTENT>& b)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#endif  // INCLUDED_BSLSTL_SPAN

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
