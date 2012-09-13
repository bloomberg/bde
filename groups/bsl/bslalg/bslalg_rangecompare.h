// bslalg_rangecompare.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_RANGECOMPARE
#define INCLUDED_BSLALG_RANGECOMPARE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Compare elements in an iterator range for equality or ordering.
//
//@CLASSES:
//  bslalg::RangeCompare: comparison algorithms for iterator ranges
//
//@SEE_ALSO: bslalg_typetraitbitwiseequalitycomparable
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides two functions, 'equal' and
// 'lexicographical', for comparing two ranges.  The function 'equal' uses
// optimizations based on the 'bslalg::TypeTraitBitwiseEqualityComparable'
// trait.  The function 'lexicographical' uses optimizations for ranges
// specified by contiguous arrays of unsigned character types.  Both functions
// are more efficient if the lengths of the ranges are specified.
//
///Usage
///-----
// This component is for use primarily by the 'bslstl' package.  Other clients
// should use the STL algorithms (in header '<algorithm>' and '<memory>').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SELECTTRAIT
#include <bslalg_selecttrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_CWCHAR
#include <cwchar>
#define INCLUDED_CWCHAR
#endif

namespace BloombergLP {

namespace bslalg {

                          // ==================
                          // class RangeCompare
                          // ==================

struct RangeCompare {
    // Namespace for range comparison algorithms.

    // PUBLIC TYPES
    typedef std::size_t  size_type;

    // CLASS METHODS
    template <typename INPUT_ITER>
    static bool equal(INPUT_ITER start1,
                      INPUT_ITER start2,
                      INPUT_ITER end1);
        // Compare each element in the range starting at the specified 'start1'
        // and ending immediately before the specified 'end1' with the
        // corresponding element in the range of the same length starting at
        // the specified 'start2', as if using 'operator=='.  Return 'true' if
        // every pair of corresponding elements compares equal, and 'false'
        // otherwise.  Note that this implementation uses 'operator==' to
        // perform the comparisons, or bitwise comparison if the value type has
        // the bit-wise equality comparable trait.

    template <typename INPUT_ITER>
    static bool equal(INPUT_ITER start1,
                      INPUT_ITER end1,
                      INPUT_ITER start2,
                      INPUT_ITER end2);
    template <typename INPUT_ITER>
    static bool equal(INPUT_ITER start1, INPUT_ITER end1, size_type length1,
                      INPUT_ITER start2, INPUT_ITER end2, size_type length2);
        // Compare each element in the range starting at the specified 'start1'
        // and ending immediately before the specified 'end1', to the
        // corresponding element in the range starting at 'start2' and ending
        // immediately before the specified 'end2', provided both elements
        // exist, as if by using 'operator==' element by element.  Optionally
        // specify both the length of each range in 'length1' and 'length2'.
        // Return 'true' if the ranges have the same length and every pair of
        // corresponding elements compares equal, and 'false' otherwise.  The
        // behavior is undefined unless 'length1' is either unspecified or
        // equals the length of the range '[start1, end1)', and 'length2' is
        // either unspecified or equals the length of the range
        // '[start2, end2)'.  Note that this implementation uses 'operator=='
        // to perform the comparisons, or bit-wise comparison if the value type
        // has the bit-wise equality comparable trait.  Also note that
        // providing lengths may reduce the cost of this operation.

    template <typename INPUT_ITER>
    static int lexicographical(INPUT_ITER start1,
                               INPUT_ITER end1,
                               INPUT_ITER start2,
                               INPUT_ITER end2);
    template <typename INPUT_ITER>
    static int lexicographical(INPUT_ITER start1,
                                INPUT_ITER end1,
                               size_type  length1,
                               INPUT_ITER start2,
                               INPUT_ITER end2,
                               size_type  length2);
        // Compare each element in the range starting at the specified 'start1'
        // and ending immediately before the specified 'end1', to the
        // corresponding element in the range starting at the specified
        // 'start2' and ending immediately before the specified 'end2'.
        // Optionally specify both the length of each range in 'length1' and
        // 'length2'.  Return -1 if the first range compares lexicographically
        // less than the second range, 0 if they are the same length and
        // compare equal, and 1 if the first range compares larger.  The
        // behavior is undefined unless 'length1' is either unspecified or
        // equals the length of the range '[start1, end1)', and 'length2' is
        // either unspecified or equals the length of the range
        // '[start2, end2)'.  Note that this implementation uses 'std::memcmp'
        // for unsigned character comparisons, 'std::wmemcmp' for wide
        // character comparisons, and 'operator<' for all other types.
};

                       // =======================
                       // struct RangeCompare_Imp
                       // =======================

struct RangeCompare_Imp {

    // CLASS METHODS
    template <typename VALUE_TYPE>
    static bool equal(const VALUE_TYPE   *start1,
                      const VALUE_TYPE   *end1,
                      const VALUE_TYPE   *start2,
                      const VALUE_TYPE   *end2,
                      const VALUE_TYPE&,
                      bslmf::MetaInt<1>);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static bool equal(INPUT_ITER          start1,
                      INPUT_ITER          end1,
                      INPUT_ITER          start2,
                      INPUT_ITER          end2,
                      const VALUE_TYPE&,
                      bslmf::MetaInt<0>);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static bool equal(INPUT_ITER         start1,
                      INPUT_ITER         end1,
                      INPUT_ITER         start2,
                      INPUT_ITER         end2,
                      const VALUE_TYPE&);
        // Compare the range starting at the specified 'start1' and ending
        // immediately before the specified 'end1' with the range starting at
        // the specified 'start2' and ending immediately before the specified
        // 'end2', as if using 'operator==' element by element.  The unnamed
        // 'VALUE_TYPE' argument is for automatic type deduction and is
        // ignored.  The fifth argument is for overloading resolution and is
        // also ignored.

    template <typename INPUT_ITER, typename VALUE_TYPE>
    static bool equal(INPUT_ITER        start1,
                      INPUT_ITER        end1,
                      INPUT_ITER        start2,
                      const VALUE_TYPE&,
                      TypeTraitBitwiseEqualityComparable);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static bool equal(INPUT_ITER        start1,
                      INPUT_ITER        end1,
                      INPUT_ITER        start2,
                      const VALUE_TYPE&,
                      bslmf::MatchAnyType);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static bool equal(INPUT_ITER start1,
                      INPUT_ITER end1,
                      INPUT_ITER start2,
                      const      VALUE_TYPE&);
        // Compare the range starting at the specified 'start1' and ending
        // immediately before the specified 'end1' with the range starting at
        // the specified 'start2' of the same length (namely, 'end1 - start1'),
        // as if using 'operator==' element by element.  The unnamed
        // 'VALUE_TYPE' argument is for automatic type deduction and is
        // ignored.  The fifth argument is for overloading resolution and is
        // also ignored.

    template <typename VALUE_TYPE>
    static bool equalBitwiseEqualityComparable(const VALUE_TYPE   *start1,
                                               const VALUE_TYPE   *end1,
                                               const VALUE_TYPE   *start2,
                                               bslmf::MetaInt<1>);
    template <typename INPUT_ITER>
    static bool equalBitwiseEqualityComparable(INPUT_ITER          start1,
                                               INPUT_ITER          end1,
                                               INPUT_ITER          start2,
                                               bslmf::MetaInt<0>);
        // These functions follow the 'equal' contract, using bit-wise
        // comparison when the parameterized 'VALUE_TYPE' is bitwise-equality
        // comparable.  The last argument is for removing overload ambiguities
        // and is not used.

    template <typename VALUE_TYPE>
    static int lexicographical(const VALUE_TYPE   *start1,
                               const VALUE_TYPE   *end1,
                               const VALUE_TYPE   *start2,
                               const VALUE_TYPE   *end2,
                               const VALUE_TYPE&,
                               bslmf::MetaInt<1>);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static int lexicographical(INPUT_ITER          start1,
                               INPUT_ITER          end1,
                               INPUT_ITER          start2,
                               INPUT_ITER          end2,
                               const VALUE_TYPE&,
                               bslmf::MetaInt<0>);
    template <typename INPUT_ITER, typename VALUE_TYPE>
    static int lexicographical(INPUT_ITER start1,
                               INPUT_ITER end1,
                               INPUT_ITER start2,
                               INPUT_ITER end2,
                               const      VALUE_TYPE&);
        // These functions follow the 'lexicographical' contract.  The first
        // overload, where the input iterator is a pointer type, can be
        // optimized.  The last argument is for removing overload ambiguities
        // and is not used.

    static int lexicographical(const char *start1,
                               const char *end1,
                               const char *start2);
    static int lexicographical(const unsigned char *start1,
                               const unsigned char *end1,
                               const unsigned char *start2);
    static int lexicographical(const wchar_t *start1,
                               const wchar_t *end1,
                               const wchar_t *start2);
    template <typename INPUT_ITER>
    static int lexicographical(INPUT_ITER           start1,
                               INPUT_ITER           end1,
                               INPUT_ITER           start2,
                               bslmf::MatchAnyType);
    template <typename INPUT_ITER>
    static int lexicographical(INPUT_ITER start1,
                               INPUT_ITER end1,
                               INPUT_ITER start2);
        // Compare each element in the range starting at the specified 'start1'
        // and ending immediately before the specified 'end1' with the
        // corresponding element in the range of the same length starting at
        // the specified 'start2'.  Return a negative value if the first range
        // compares lexicographically less than the second range, 0 if they
        // compare equal, and a positive value if the first range compares
        // larger.
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                         // -------------------
                         // struct RangeCompare
                         // -------------------

// CLASS METHODS
template <typename INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         INPUT_ITER start2)
{
    if (start1 == end1) {
        return true;
    }
    return RangeCompare_Imp::equal(start1, end1, start2, *start1);
}

template <typename INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         INPUT_ITER start2,
                         INPUT_ITER end2)
{
    if (start1 == end1) {
        return start2 == end2;
    }
    return RangeCompare_Imp::equal(start1, end1, start2, end2, *start1);
}

template <typename INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         size_type  length1,
                         INPUT_ITER start2,
                         INPUT_ITER,
                         size_type  length2)
{
    if (length1 != length2) {
        return false;
    }
    if (start1 == end1) {
        return true;
    }
    return RangeCompare_Imp::equal(start1, end1, start2, *start1);
}

template <typename INPUT_ITER>
int RangeCompare::lexicographical(INPUT_ITER start1,
                                  INPUT_ITER end1,
                                  INPUT_ITER start2,
                                  INPUT_ITER end2)
{
    if (start1 == end1) {
        return start2 != end2 ? -1 : 0;
    }
    return RangeCompare_Imp::lexicographical(start1,
                                             end1,
                                             start2,
                                             end2,
                                             *start1);
}

template <typename INPUT_ITER>
int RangeCompare::lexicographical(INPUT_ITER start1,
                                  INPUT_ITER end1,
                                  size_type  length1,
                                  INPUT_ITER start2,
                                  INPUT_ITER end2,
                                  size_type  length2)
{
    int result = (length2 < length1)
            ? - RangeCompare_Imp::lexicographical(start2, end2, start1)
            :   RangeCompare_Imp::lexicographical(start1, end1, start2);

    if (result < 0) {
        return -1;
    }
    if (0 < result) {
        return 1;
    }
    if (length1 < length2) {
        return -1;
    }
    if (length2 < length1) {
        return 1;
    }
    return 0;
}

                       // -----------------------
                       // struct RangeCompare_Imp
                       // -----------------------

// CLASS METHODS

                          // *** equal overloads: ***

template <typename VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(const VALUE_TYPE  *start1,
                             const VALUE_TYPE  *end1,
                             const VALUE_TYPE  *start2,
                             const VALUE_TYPE  *end2,
                             const VALUE_TYPE&,
                             bslmf::MetaInt<1>)
{
    return RangeCompare::equal(start1,
                               end1,
                               end1 - start1,
                               start2,
                               end2,
                               end2 - start2);
}

template <typename INPUT_ITER, typename VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             INPUT_ITER        end2,
                             const VALUE_TYPE&,
                             bslmf::MetaInt<0>)
{
    for ( ; start1 != end1 && start2 != end2; ++start1, ++start2) {
        if (! (*start1 == *start2)) {
            return false;
        }
    }
    return (start1 == end1) && (start2 == end2);
}

template <typename INPUT_ITER, typename VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             INPUT_ITER        end2,
                             const VALUE_TYPE& value)
{
    typedef typename bslmf::IsConvertible<INPUT_ITER, const VALUE_TYPE*>::Type
                                                      CanUseLengthOptimization;
    return equal(start1,
                 end1,
                 start2,
                 end2,
                 value,
                 CanUseLengthOptimization());
}

template <typename INPUT_ITER, typename VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             const VALUE_TYPE&,
                             TypeTraitBitwiseEqualityComparable)
{
    // Note: We are forced to call a different function to resolve whether
    // 'INPUT_ITER' is convertible to 'const TARGET_TYPE *' or not, otherwise
    // we would be introducing ambiguities (the additional parameter
    // 'CanUseBitwiseCopyOptimization' is necessary to remove further
    // ambiguities on SunPro).

    typedef typename bslmf::IsConvertible<INPUT_ITER, const VALUE_TYPE*>::Type
                                              CanUseBitwiseCompareOptimization;
    return equalBitwiseEqualityComparable(start1,
                                          end1,
                                          start2,
                                          CanUseBitwiseCompareOptimization());
}

template <typename INPUT_ITER, typename VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER         start1,
                             INPUT_ITER         end1,
                             INPUT_ITER         start2,
                             const VALUE_TYPE&,
                             bslmf::MatchAnyType)
{
    for ( ; start1 != end1; ++start1, ++start2) {
        if (! (*start1 == *start2)) {
            return false;
        }
    }
    return true;
}

template <typename INPUT_ITER, typename VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             const VALUE_TYPE& value)
{
    typedef typename SelectTrait<VALUE_TYPE,
                        TypeTraitBitwiseEqualityComparable>::Type Trait;
    return equal(start1, end1, start2, value, Trait());
}

             // *** equalBitwiseEqualityComparable overloads: ***

template <typename VALUE_TYPE>
inline
bool RangeCompare_Imp::equalBitwiseEqualityComparable(
                                                    const VALUE_TYPE   *start1,
                                                    const VALUE_TYPE   *end1,
                                                    const VALUE_TYPE   *start2,
                                                    bslmf::MetaInt<1>)
{
    std::size_t numBytes = (const char *)end1 - (const char *)start1;
    return 0 == std::memcmp((const void *)start1,
                            (const void *)start2,
                            numBytes);
}

template <typename INPUT_ITER>
inline
bool RangeCompare_Imp::equalBitwiseEqualityComparable(
                                                   INPUT_ITER           start1,
                                                   INPUT_ITER           end1,
                                                   INPUT_ITER           start2,
                                                   bslmf::MetaInt<0>)
{
    // We can't be as optimized as above.

    return equal(start1, end1, start2, *start1, bslmf::MatchAnyType(0));
}

                     // *** lexicographical overloads: ***

template <typename VALUE_TYPE>
inline
int RangeCompare_Imp::lexicographical(const VALUE_TYPE   *start1,
                                      const VALUE_TYPE   *end1,
                                      const VALUE_TYPE   *start2,
                                      const VALUE_TYPE   *end2,
                                      const VALUE_TYPE&,
                                      bslmf::MetaInt<1>)
{
    // In this case, we can compute the length directly, and avoid the overhead
    // of the two comparisons in the loop condition (one is enough).

    return RangeCompare::lexicographical(start1,
                                         end1,
                                         end1 - start1,
                                         start2,
                                         end2,
                                         end2 - start2);
}

template <typename INPUT_ITER, typename VALUE_TYPE>
int RangeCompare_Imp::lexicographical(INPUT_ITER        start1,
                                      INPUT_ITER        end1,
                                      INPUT_ITER        start2,
                                      INPUT_ITER        end2,
                                      const VALUE_TYPE&,
                                      const bslmf::MetaInt<0>)
{
    for ( ; start1 != end1 && start2 != end2; ++start1, ++start2) {
        if (*start1 < *start2) {
            return -1;
        }
        else if (*start2 < *start1) {
            return 1;
        }
    }
    if (start1 != end1) {
        return 1;
    }
    if (start2 != end2) {
        return -1;
    }
    return 0;
}

template <typename INPUT_ITER, typename VALUE_TYPE>
inline
int RangeCompare_Imp::lexicographical(INPUT_ITER        start1,
                                      INPUT_ITER        end1,
                                      INPUT_ITER        start2,
                                      INPUT_ITER        end2,
                                      const VALUE_TYPE& value)
{
    typedef typename bslmf::IsConvertible<INPUT_ITER, const VALUE_TYPE*>::Type
                                                      CanUseLengthOptimization;
    return lexicographical(start1, end1, start2, end2, value,
                           CanUseLengthOptimization());
}

inline
int RangeCompare_Imp::lexicographical(const unsigned char *start1,
                                      const unsigned char *end1,
                                      const unsigned char *start2)
{
    return std::memcmp(start1, start2, (end1 - start1));
}

inline
int RangeCompare_Imp::lexicographical(const char *start1,
                                      const char *end1,
                                      const char *start2)
{
#if (CHAR_MAX == SCHAR_MAX)
    return std::memcmp(start1, start2, (end1 - start1));
#else
    return lexicographical<const char*>(start1, end1, start2, 0);
#endif
}

inline
int RangeCompare_Imp::lexicographical(const wchar_t *start1,
                                      const wchar_t *end1,
                                      const wchar_t *start2)
{
    return std::wmemcmp(start1, start2, (end1 - start1));
}

template <typename INPUT_ITER>
int RangeCompare_Imp::lexicographical(INPUT_ITER start1,
                                      INPUT_ITER end1,
                                      INPUT_ITER start2,
                                      bslmf::MatchAnyType)
{
    for ( ; start1 != end1; ++start1, ++start2) {
        if (*start1 < *start2) {
            return -1;
        }
        else if (*start2 < *start1) {
            return 1;
        }
    }
    return 0;
}

template <typename INPUT_ITER>
inline
int RangeCompare_Imp::lexicographical(INPUT_ITER start1,
                                      INPUT_ITER end1,
                                      INPUT_ITER start2)
{
    if (start1 != end1) {
        return lexicographical(start1, end1, start2, *start1);
    }
    return 0;
}

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::RangeCompare bslalg_RangeCompare;
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLALG_RANGECOMPARE)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
