// bslstl_ranges.t.cpp                                                -*-C++-*-
#include <bslstl_ranges.h>

#include <bslstl_algorithm.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

#include <algorithm>
#include <string>
#include <vector>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
#include <tuple>
#endif

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

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

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
namespace U_COMMON_ITERATOR_NAMESPACE {
    using bsl::common_iterator;
}  // close namespace U_COMMON_ITERATOR_NAMESPACE

                            // =====================
                            // class RangesDummyView
                            // =====================

/// This class is inherited from `bsl::ranges::view_interface` and its only
/// purpose is to prove that `bsl::ranges::view_interface` exists and is
/// usable.
class RangesDummyView : public bsl::ranges::view_interface<RangesDummyView> {

  public:
    // ACCESSORS

    /// Unconditionally return `true`.
    constexpr bool empty() const
    {
        return true;
    }
};

class RangesDummyRandomGenerator{
  // This class satisfies the basic requirements for a random generator, that
  // can be used in constrained algorithms.

  public:
    // TYPES
    using result_type = unsigned int;

    // CLASS METHODS

    /// Unconditionally return `1u`.
    static constexpr result_type min()
    {
        return 1u;
    }

    /// Unconditionally return `5u`.
    static constexpr result_type max()
    {
        return 5u;
    }

    // MANIPULATORS

    /// Unconditionally return `1u`.
    result_type operator()()
    {
        return 1u;
    }
};
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

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

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
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
        // 1. Add any component-related code here.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        namespace ranges = bsl::ranges;

        // Testing types aliased in the `bsl_ranges.h`, using `std` types
        // because we do not want to add new test-only dependencies that would
        // inevitably force a cycle.

        using Vector = std::vector<int>;
        using Tuple  = std::tuple<int, int>;

        Vector vec{1, 2, 3, 4, 5};

        auto vBegin   = ranges::begin(vec);
        auto vEnd     = ranges::end(vec);
        auto vCBegin  = ranges::cbegin(vec);
        auto vCEnd    = ranges::cend(vec);
        auto vRBegin  = ranges::rbegin(vec);
        auto vREnd    = ranges::rend(vec);
        auto vCRBegin = ranges::crbegin(vec);
        auto vCREnd   = ranges::crend(vec);
        auto vSize    = ranges::size(vec);
        auto vSSize   = ranges::ssize(vec);
        bool vEmpty   = ranges::empty(vec);
        auto vData    = ranges::data(vec);
        auto vCData   = ranges::cdata(vec);

        ASSERTV(*vBegin,   1        == *vBegin  );
        ASSERTV(           vBegin   !=  vEnd    );
        ASSERTV(*vCBegin,  1        == *vCBegin );
        ASSERTV(           vCBegin  !=  vCEnd   );
        ASSERTV(*vRBegin,  5        == *vRBegin );
        ASSERTV(           vRBegin  !=  vREnd   );
        ASSERTV(*vCRBegin, 5        == *vCRBegin);
        ASSERTV(           vCRBegin !=  vCREnd  );

        ASSERTV( vSize,    5        ==  vSize   );
        ASSERTV( vSSize,   5        ==  vSSize  );
        ASSERTV(           false    ==  vEmpty  );
        ASSERTV(*vData,    1        == *vData   );
        ASSERTV(*vCData,   1        == *vCData  );

        // Range primitives

        ranges::iterator_t<Vector>               vBeginT     = vec.begin();
        ranges::sentinel_t<Vector>               vEndT       = vec.end();
        ranges::range_size_t<Vector>             vSizeT      = vec.size();
        ranges::range_difference_t<Vector>       vDiffT      =
                                 bsl::ranges::distance(vec.begin(), vec.end());
                                              // defined in <bslstl_iterator.h>
        ranges::range_value_t<Vector>            vValueT     = vec[0];
        ranges::range_reference_t<Vector>        vRefT       = vec[0];
        ranges::range_rvalue_reference_t<Vector> vRvalueRefT = 10 * vec[0];

        ASSERTV(*vBeginT,     1    == *vBeginT    );
        ASSERTV(              vEnd ==  vEndT      );
        ASSERTV( vSizeT,      5    ==  vSizeT     );
        ASSERTV( vDiffT,      5    ==  vDiffT     );
        ASSERTV( vValueT,     1    ==  vValueT    );
        ASSERTV( vRefT,       1    ==  vRefT      );
        ASSERTV( vRvalueRefT, 10   ==  vRvalueRefT);

        // Range concepts

        ASSERT(true  == ranges::range<Vector>                              );
        ASSERT(false == ranges::borrowed_range<Vector>                     );
        ASSERT(false == ranges::enable_borrowed_range<Vector>              );
        ASSERT(true  == ranges::sized_range<Vector>                        );
        ASSERT(false == ranges::disable_sized_range<Vector>                );
        ASSERT(false == ranges::view<Vector>                               );
        ASSERT(false == ranges::enable_view<Vector>                        );

        ranges::view_base base;
        (void)base;            // suppress compiler warning

        ASSERT( true  == ranges::input_range<Vector>                       );
        ASSERT((false == ranges::output_range<Vector,
                                              ranges::iterator_t<Vector> >));
        ASSERT( true  == ranges::forward_range<Vector>                     );
        ASSERT( true  == ranges::bidirectional_range<Vector>               );
        ASSERT( true  == ranges::random_access_range<Vector>               );
        ASSERT( true  == ranges::contiguous_range<Vector>                  );
        ASSERT( true  == ranges::common_range<Vector>                      );
        ASSERT( true  == ranges::viewable_range<Vector>                    );


        // Views

        // `RangesDummyView` is inherited from `bsl::ranges::view_interface`.

        RangesDummyView dummy;
        ASSERTV(true == dummy.empty());

        ranges::subrange subrange(vec.begin(), vec.end());
        ASSERTV(5 == subrange.size());

        // Dangling iterator handling

        auto maxElement = ranges::max_element(vec);
                                             // defined in <bslstl_algorithm.h>

        ASSERTV((!bsl::is_same_v<decltype(maxElement), ranges::dangling>));

        ASSERTV(( bsl::is_same_v<ranges::borrowed_iterator_t<Vector>,
                                ranges::dangling>));
        ASSERTV(( bsl::is_same_v<ranges::borrowed_subrange_t<Vector>,
                                ranges::dangling>));

        // Factories

        ranges::empty_view<int> emptyView;
        ASSERTV(!emptyView        );
        ASSERTV( emptyView.empty());

        ranges::single_view<int> singleView(1);
        ASSERTV(singleView.size(), 1 == singleView.size());

        ranges::iota_view<int, int> iotaView(1, 2);
        ASSERTV(iotaView.size(), 1 == iotaView.size());

        std::istringstream                    iStringStream("1 2 3");
        ranges::basic_istream_view<int, char> basicIStreamView(iStringStream);
        auto                                  beginResult =
                                                     *basicIStreamView.begin();
        ASSERTV(beginResult, 1 == beginResult);

        ranges::istream_view<int> iStreamView(iStringStream);
        beginResult = *iStreamView.begin();
        ASSERTV(beginResult, 2 == beginResult);

        std::wstring wString;
        wString.push_back('1');
        std::wistringstream        wIStringStream(wString);
        ranges::wistream_view<int> wIStreamView(wIStringStream);
        beginResult = *wIStreamView.begin();
        ASSERTV(beginResult, 1 == beginResult);

        auto viewsEmpty = bsl::views::empty<int>;
        ASSERTV(!viewsEmpty        );
        ASSERTV( viewsEmpty.empty());

        auto viewsSingle = bsl::views::single(1);
        ASSERTV(viewsSingle.size(), 1 == viewsSingle.size());

        auto viewsIota = bsl::views::iota(1, 2);
        ASSERTV(viewsIota.size(), 1 == viewsIota.size());

        auto viewsIStream = bsl::views::istream<int>(iStringStream);
        beginResult = *viewsIStream.begin();
        ASSERTV(beginResult, 3 == beginResult);

        // Adaptors

        ranges::ref_view refView = bsl::views::all(vec);
        ASSERTV(&vec[0] == &refView[0]);

        bsl::views::all_t<decltype((vec))> &refViewRef = refView;
        (void)refViewRef;

        auto countedView = bsl::views::counted(vec.begin(), 3);
        ASSERTV(countedView.size(),  3 == countedView.size());

        Vector vecToMove{1, 2, 3, 4, 5};
        ranges::owning_view owningView = bsl::views::all(bsl::move(vecToMove));
        ASSERTV(vecToMove.size(),  0 == vecToMove.size());
        ASSERTV(owningView.size(), 5 == owningView.size());

        ranges::filter_view filterView =
                                       bsl::views::filter(vec,
                                                          [](int value)
                                                          {
                                                              return value > 3;
                                                          });
        ASSERTV(vec.front(),        1 == vec.front()       );  // 1, 2, 3, 4, 5
        ASSERTV(filterView.front(), 4 == filterView.front());  // 4, 5

        ranges::transform_view transformView =
                                       bsl::views::transform(vec,
                                                          [](int value)
                                                          {
                                                              return value * 3;
                                                          });
        ASSERTV(vec[0],           1 == vec[0]          );  // 1, 2, 3, 4, 5
        ASSERTV(transformView[0], 3 == transformView[0]);  // 3, 6, 9, 12, 15

        ranges::take_view takeView = bsl::views::take(vec, 3);
        ASSERTV(takeView.size(), 3 == takeView.size());  // 1, 2, 3

        ranges::take_while_view takeWhileView =
                                   bsl::views::take_while(vec,
                                                          [](int value)
                                                          {
                                                              return value < 3;
                                                          });
        ASSERTV(takeWhileView.front(), 1 == takeWhileView.front());  // 1, 2

        ranges::drop_view dropView = bsl::views::drop(vec, 3);
        ASSERTV(dropView.size(), 2 == dropView.size());  // 4, 5

        ranges::drop_while_view dropWhileView =
                                   bsl::views::drop_while(vec,
                                                          [](int value)
                                                          {
                                                              return value < 3;
                                                          });
        ASSERTV(dropWhileView.size(), 3 == dropWhileView.size());  // 3, 4, 5

#if BSL_RANGES_HAS_JOIN_VIEW
        Vector            vec1{6, 7, 8};
        Vector            vecArray[] = {vec, vec1};
        ranges::join_view joinView = bsl::views::join(vecArray);
        ASSERTV(joinView.front(), 1 == joinView.front());  // vecArray[0][0]
        ASSERTV(joinView.back(),  8 == joinView.back() );  // vecArray[1][2]
#endif  // BSL_RANGES_HAS_JOIN_VIEW

        ranges::lazy_split_view lazySplitView = bsl::views::lazy_split(vec, 3);
        ASSERTV(false == lazySplitView.empty());

        ranges::split_view splitView = bsl::views::split(vec, 3);
        ASSERTV(false == splitView.empty());

        ranges::common_view commonView =
                             vec | bsl::views::take_while([](int value)
                                                          {
                                                              return value < 3;
                                                          })
                                 | bsl::views::common;
        ASSERTV(commonView.front(), 1 == commonView.front());  // 1, 2

        ranges::reverse_view reverseView = bsl::views::reverse(vec);
        ASSERTV(reverseView.front(), 5 == reverseView.front());
                                                               // 5, 4, 3, 2, 1

        Tuple tuples[] = {{1, 2}, {3, 4}, {5, 6}};
        ranges::elements_view elementsView = std::views::elements<1>(tuples);
        ASSERTV(elementsView.size(),  3 == elementsView.size());  // 2, 4, 6
        ASSERTV(elementsView.front(), 2 == elementsView.front());

        // The types of `keysView` and `valuesView` in the following two tests
        // look bulky.  But we have to declare template parameters implicitly
        // due to a defect in the standard.
        // See https://cplusplus.github.io/LWG/issue3563

        ranges::keys_view<decltype(bsl::views::all(tuples))> keysView =
                                                      bsl::views::keys(tuples);
        ASSERTV(keysView.size(),  3 == keysView.size());  // 1, 3, 4
        ASSERTV(keysView.front(), 1 == keysView.front());

        ranges::values_view<decltype(bsl::views::all(tuples))> valuesView =
                                                    bsl::views::values(tuples);
        ASSERTV(valuesView.size(),  3 == valuesView.size());  // 2, 4, 6
        ASSERTV(valuesView.front(), 2 == valuesView.front());

        // Enumerations

        ranges::subrange_kind subrangeKind;
        (void)subrangeKind;

        // Testing algorithms aliased in `<bsl_algorithm.h>`

        auto greaterThanTwo = [](int  i)
                               {
                                   return i > 2;
                               };

        ASSERT(!ranges::all_of (vec, greaterThanTwo));
        ASSERT( ranges::any_of (vec, greaterThanTwo));
        ASSERT(!ranges::none_of(vec, greaterThanTwo));

        Vector vecToTransform{1, 2, 3, 4, 5};
        auto   multiplyByTwo = [](int& i)
                               {
                                   i *= 2;
                                   return i;
                               };

        auto forEachResult =
                               ranges::for_each(vecToTransform, multiplyByTwo);
        ASSERTV(0 == ranges::distance(vecToTransform.end(), forEachResult.in));
        ASSERTV(vecToTransform[0], 2 == vecToTransform[0]);

        auto forEachNResult =
                  ranges::for_each_n(vecToTransform.begin(), 1, multiplyByTwo);
        ASSERTV(1 == ranges::distance(vecToTransform.begin(),
                                      forEachNResult.in));
        ASSERTV(vecToTransform[0], 4 == vecToTransform[0]);

        ASSERT(1 == ranges::count   (vec, 2));
        ASSERT(3 == ranges::count_if(vec, greaterThanTwo));

        auto mismatchResult = ranges::mismatch(vec, vecToTransform);
        ASSERT(0 == ranges::distance(vec.begin(), mismatchResult.in1));
        ASSERT(0 == ranges::distance(vecToTransform.begin(),
                                     mismatchResult.in2));

        ASSERT(true  == ranges::equal                  (vec, vec));
        ASSERT(false == ranges::lexicographical_compare(vec, vec));

        auto findResult = ranges::find(vec, 1);
        ASSERT(0 == ranges::distance(vec.begin(), findResult));

        auto findIfResult = ranges::find_if(vec, greaterThanTwo);
        ASSERT(2 == ranges::distance(vec.begin(), findIfResult));

        auto findIfNotResult = ranges::find_if_not(vec, greaterThanTwo);
        ASSERT(0 == ranges::distance(vec.begin(), findIfNotResult));

        auto findEndResult = ranges::find_end(vec, vecToTransform);
        ASSERT(0 == ranges::distance(vec.end(), findEndResult.begin()));

        auto findFirstOfResult = ranges::find_first_of(vec, vec);
        ASSERT(0 == ranges::distance(vec.begin(), findFirstOfResult));

        auto adjacentFindResult = ranges::adjacent_find(vecToTransform);
        ASSERT(0 == ranges::distance(vecToTransform.begin(),
                                     adjacentFindResult));

        auto searchResult = ranges::search(vec, vec);
        ASSERT(0 == ranges::distance(vec.begin(), searchResult.begin()));

        auto searchNResult = ranges::search_n(vecToTransform, 2, 4);
        ASSERT(0 == ranges::distance(vecToTransform.begin(),
                                     searchNResult.begin()));

        Vector vecToCopyTo(20);

        auto copyResult = ranges::copy(vec, vecToCopyTo.begin());
        ASSERT(0 == ranges::distance(vec.end(), copyResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 4, 5

        auto copyIfResult = ranges::copy_if(vecToTransform,
                                            vecToCopyTo.begin() + 5,
                                            greaterThanTwo);
        ASSERT(0 == ranges::distance(vecToTransform.end(), copyIfResult.in));
        ASSERTV(vecToCopyTo[5], 4  == vecToCopyTo[5]); // ... 4, 4, 6, 8, 10

        auto copyNResult = ranges::copy_n(vecToCopyTo.begin(),
                                          5,
                                          vecToCopyTo.begin() + 10);
        ASSERT(0 == ranges::distance(vecToCopyTo.begin() + 5, copyNResult.in));
        ASSERTV(vecToCopyTo[10], 1  == vecToCopyTo[10]);  // ... 1, 2, 3, 4, 5

        auto copyBackResult = ranges::copy_backward(vec, vecToCopyTo.end());
        ASSERT(0 == ranges::distance(vec.end(), copyBackResult.in));
        ASSERTV(vecToCopyTo[15], 1  == vecToCopyTo[15]);  // ... 1, 2, 3, 4, 5

        Vector vecToMoveFrom1 = {5, 4, 3, 2, 1};

        auto moveResult = ranges::move(vecToMoveFrom1, vecToCopyTo.begin());
        ASSERT(0 == ranges::distance(vecToMoveFrom1.end(), moveResult.in));
        ASSERTV(vecToCopyTo[0], 5 == vecToCopyTo[0]);  // 5, 4, 3, 2, 1 ...

        Vector vecToMoveFrom2 = {5, 4, 3, 2, 1};

        auto moveBackResult = ranges::move_backward(vecToMoveFrom2,
                                                    vecToCopyTo.begin() + 10);
        ASSERT(0 == ranges::distance(vecToMoveFrom2.end(), moveBackResult.in));
        ASSERTV(vecToCopyTo[5], 5 == vecToCopyTo[5]);  // ... 5, 4, 3, 2, 1 ...

        ranges::fill(vecToTransform, -1);
        ASSERTV(vecToTransform[0], -1 == vecToTransform[0]);

        ranges::fill_n(vecToTransform.begin(), 1, 1);
        ASSERTV(vecToTransform[0],  1 == vecToTransform[0]);
        ASSERTV(vecToTransform[1], -1 == vecToTransform[1]);

        auto uTransformResult = ranges::transform(vecToTransform,
                                                  vecToTransform.begin(),
                                                  multiplyByTwo);
        ASSERT(0 == ranges::distance(vecToTransform.end(),
                                     uTransformResult.in));
        ASSERTV(vecToTransform[0], 2 == vecToTransform[0]);

        auto bTransformResult = ranges::transform(vec,
                                                  vecToTransform,
                                                  vecToTransform.begin(),
                                                  std::multiplies<int>());
        ASSERT(0 == ranges::distance(vec.end(), bTransformResult.in1));
        ASSERTV(vecToTransform[1], -4 == vecToTransform[1]);

        auto generateOne = []()
                           {
                               return 1u;
                           };

        ranges::generate(vecToTransform, generateOne);
        ASSERTV(vecToTransform[0], 1 == vecToTransform[0]);

        vecToTransform[0] = 2;

        ranges::generate_n(vecToTransform.begin(), 1, generateOne);
        ASSERTV(vecToTransform[0], 1 == vecToTransform[0]);  // 1, 1, 1, 1, 1

        vecToTransform[1] = 2;

        (void)ranges::remove(vecToTransform, 2);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        vecToTransform[1] = 3;

        (void)ranges::remove_if(vecToTransform, greaterThanTwo);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        auto removeCopyResult = ranges::remove_copy(vec,
                                                    vecToCopyTo.begin(),
                                                    1);
        ASSERTV(0 == ranges::distance(vec.end(), removeCopyResult.in));
        ASSERTV(vecToCopyTo[0], 2 == vecToCopyTo[0]);  // 2, 3, 4, 5, 1, ...

        auto removeCopyIfResult = ranges::remove_copy_if(vec,
                                                         vecToCopyTo.begin(),
                                                         greaterThanTwo);
        ASSERTV(0 == ranges::distance(vec.end(), removeCopyIfResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 4, 5, 1, ...

        ranges::replace(vecToTransform, 1, 3);
        ASSERTV(vecToTransform[1], 3 == vecToTransform[1]);  // 3, 3, 3, 3, 3

        ranges::replace_if(vecToTransform, greaterThanTwo, 1);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        auto replaceCopyResult = ranges::replace_copy(vec,
                                                      vecToCopyTo.begin(),
                                                      1,
                                                      3);
        ASSERTV(0 == ranges::distance(vec.end(), replaceCopyResult.in));
        ASSERTV(vecToCopyTo[0], 3 == vecToCopyTo[0]);  // 3, 2, 4, 5, 3, ...

        auto replaceCopyIfResult = ranges::replace_copy_if(vec,
                                                           vecToCopyTo.begin(),
                                                           greaterThanTwo,
                                                           1);
        ASSERTV(0 == ranges::distance(vec.end(), replaceCopyIfResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 1, 1, 1, ...

        Vector vecToTransform1{1, 2, 3, 4, 5};

        auto swapRangesResult = ranges::swap_ranges(vecToTransform,
                                                    vecToTransform1);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      swapRangesResult.in1));
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]);  // 1, 2, 3, 4, 5

        ranges::reverse(vecToTransform);
        ASSERTV(vecToTransform[1], 4 == vecToTransform[1]);  // 5, 4, 3, 2, 1

        auto reverseCopyResult = ranges::reverse_copy(vec,
                                                      vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), reverseCopyResult.in));
        ASSERTV(vec[1],         2 == vec[1]);          // 1, 2, 3, 4, 5
        ASSERTV(vecToCopyTo[1], 4 == vecToCopyTo[1]);  // 5, 4, 3, 2, 1, ...

        ranges::rotate(vecToTransform, vecToTransform.begin() + 1);
        ASSERTV(vecToTransform[1], 3 == vecToTransform[1]);  // 4, 3, 2, 1, 5

        auto rotateCopyResult = ranges::rotate_copy(vec,
                                                    vec.begin() + 2,
                                                    vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), rotateCopyResult.in));
        ASSERTV(vec[1],         2 == vec[1]);          // 1, 2, 3, 4, 5
        ASSERTV(vecToCopyTo[1], 4 == vecToCopyTo[1]);  // 3, 4, 5, 1, 2, ...

        ranges::shuffle(vecToTransform, RangesDummyRandomGenerator());
        ASSERTV(vecToTransform[1], 1 <= vecToTransform[1]
                                && 5 >= vecToTransform[1]);


        ranges::sample(vec,
                       vecToTransform.begin(),
                       1,
                       RangesDummyRandomGenerator());
        ASSERTV(vecToTransform[0], 1 <= vecToTransform[0]
                                && 5 >= vecToTransform[0]);

        vecToTransform = Vector{1, 1, 1, 2, 2};

        (void)ranges::unique(vecToTransform);
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]); // 1, 2, ?, ?, ?

        vecToTransform = Vector{1, 1, 1, 2, 2};

        auto uniqueCopyResult = ranges::unique_copy(vecToTransform,
                                                    vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      uniqueCopyResult.in));
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 2, 2
        ASSERTV(vecToCopyTo[1],    2 == vecToCopyTo[1]);     // 1, 2, 5, 1, 2,

        ASSERTV(false == ranges::is_partitioned(vec, greaterThanTwo));

        vecToTransform = Vector{2, 2, 2, 3, 3};

        ranges::partition(vecToTransform, greaterThanTwo);
        ASSERTV(vecToTransform[1], 3 == vecToTransform[1]);  // 3, 3, 2, 2, 2

        vecToTransform = Vector{2, 2, 2, 3, 3};

        auto partitionCopyResult = ranges::partition_copy(
                                                       vecToTransform,
                                                       vecToCopyTo.begin(),
                                                       vecToCopyTo.begin() + 5,
                                                       greaterThanTwo);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      partitionCopyResult.in));
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]);  // 2, 2, 2, 3, 3
        ASSERTV(vecToCopyTo[1],    3 == vecToCopyTo[1]);  // 3, 3, 5, 1, 2, 2,

        vecToTransform = Vector{2, 2, 2, 3, 4};

        ranges::stable_partition(vecToTransform, greaterThanTwo);
        ASSERTV(vecToTransform[1], 4 == vecToTransform[1]);  // 3, 4, 2, 2, 2

        auto partitionPoint = ranges::partition_point(vecToTransform,
                                                      greaterThanTwo);
        ASSERTV(0 == ranges::distance(vecToTransform.begin() + 2,
                                      partitionPoint));

        ASSERTV(true == ranges::is_sorted(vec));

        auto isSortedUntilResult = ranges::is_sorted_until(vecToTransform);
        ASSERTV(0 == ranges::distance(vecToTransform.begin() + 2,
                                      isSortedUntilResult));

        ranges::sort(vecToTransform);
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]);  // 2, 2, 2, 3, 4

        vecToTransform = Vector{5, 4, 3, 2, 1};

        ranges::partial_sort(vecToTransform, vecToTransform.begin() + 1);
        ASSERTV(vecToTransform[0], 1 == vecToTransform[0]);  // 1, ?, ?, ?, ?

        vecToTransform = Vector{5, 4, 3, 2, 1};

        ranges::partial_sort_copy(vecToTransform,
                                  vecToCopyTo);
        ASSERTV(vecToTransform[0], 5 == vecToTransform[0]);  // 5, 4, 3, 2, 1
        ASSERTV(vecToCopyTo[0],    1 == vecToCopyTo[0]   );  // 1, 2, 3, 4, 5,

        ranges::stable_sort(vecToTransform);
        ASSERTV(vecToTransform[0], 1 == vecToTransform[0]);  // 1, 2, 3, 4, 5

        vecToTransform = Vector{5, 4, 3, 2, 1};

        ranges::nth_element(vecToTransform, vecToTransform.begin() + 1);
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]);  // 1, 2, ?, ?, ?

        auto lowerBoundResult = ranges::lower_bound(vec, 3);
        ASSERTV(0 == ranges::distance(vec.begin() + 2, lowerBoundResult));

        auto upperBoundResult = ranges::upper_bound(vec, 3);
        ASSERTV(0 == ranges::distance(vec.begin() + 3, upperBoundResult));

        ASSERTV(true == ranges::binary_search(vec, 3));

        auto equalRangeResult = ranges::equal_range(vec, 3);
        ASSERTV(3 == equalRangeResult[0]);

        auto mergeResult = ranges::merge(vec,
                                         vec,
                                         vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), mergeResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 1, 2, 2, 3, 3, ...
        ASSERTV(true == ranges::is_sorted(vecToCopyTo.begin(),
                                          vecToCopyTo.begin() + 10));

        vecToTransform = Vector{4, 5, 1, 2, 3};

        ranges::inplace_merge(vecToTransform, vecToTransform.begin() + 2);
        ASSERTV(vecToTransform[0], 1 == vecToTransform[0]);  // 1, 2, 3, 4, 5

        ASSERTV(true == ranges::includes(vec, vec));

        auto setDifferenceResult = ranges::set_difference(vec.begin(),
                                                          vec.end(),
                                                          vec.begin(),
                                                          vec.begin() + 3,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), setDifferenceResult.in));
        ASSERTV(vecToCopyTo[0], 4 == vecToCopyTo[0]);  // 4, 5, 2, 2, 3, 3, ...

        auto setIntersectionResult =
                                 ranges::set_intersection(vec,
                                                          vec,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), setIntersectionResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 4, 5, 3, ...

        vecToTransform = Vector{4, 5, 6, 7, 8};

        auto setSymmetricDifferenceResult =
                         ranges::set_symmetric_difference(vec,
                                                          vecToTransform,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(),
                                      setSymmetricDifferenceResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 6, 7, 8, ...

        auto setUnionResult = ranges::set_union(vec,
                                                vecToTransform,
                                                vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(),
                                      setUnionResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 4, 5, 6, ...

        ASSERTV(false == ranges::is_heap(vecToTransform));

        ASSERTV(true == ranges::is_heap(vecToTransform.begin(),
                                        vecToTransform.begin() + 1));

        ranges::make_heap(vecToTransform);
        ASSERTV(vecToTransform[0], 8 == vecToTransform[0]);  // 8, 7, 6, 4, 5

        vecToTransform.push_back(9);

        ranges::push_heap(vecToTransform);
        ASSERTV(vecToTransform[0], 9 == vecToTransform[0]); // 9, 7, 8, 4, 5, 6

        ranges::pop_heap(vecToTransform);
        ASSERTV(vecToTransform[0], 8 == vecToTransform[0]); // 8, 7, 6, 4, 5, 9

        ranges::push_heap(vecToTransform);

        ranges::sort_heap(vecToTransform);
        ASSERTV(vecToTransform[0], 4 == vecToTransform[0]); // 4, 5, 6, 7, 8, 9

        ASSERTV(5 == ranges::max(vec));

        maxElement = ranges::max_element(vec);
        ASSERTV(0 == ranges::distance(vec.begin() + 4, maxElement));

        ASSERTV(1 == ranges::min(vec));

        auto minElement = ranges::min_element(vec);
        ASSERTV(0 == ranges::distance(vec.begin(), minElement));

        auto minMaxResult = ranges::minmax(vec);
        ASSERTV(minMaxResult.min, 1 == minMaxResult.min);

        auto minMaxElementResult = ranges::minmax_element(vec);
        ASSERTV(0 == ranges::distance(vec.begin(), minMaxElementResult.min));

        ASSERT(2 == bsl::clamp(1, 2, 3));

        ASSERT(true == ranges::is_permutation(vec, vec));

        vecToTransform = Vector{1, 2, 3, 4, 5};

        auto nextPermutationResult = ranges::next_permutation(vecToTransform);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      nextPermutationResult.in));
        ASSERTV(vecToTransform[3], 5 == vecToTransform[3]);  // 1, 2, 3, 5, 4

        auto prevPermutationResult = ranges::prev_permutation(vecToTransform);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      prevPermutationResult.in));
        ASSERTV(vecToTransform[3], 4 == vecToTransform[3]);  // 1, 2, 3, 4, 5

        // Return types.

        ranges::in_fun_result<int*, RangesDummyRandomGenerator> inFunResult;
        ranges::in_in_result<int*, int*>                        inInResult;
        ranges::in_out_result<int*, int*>                       inOutResult;
        ranges::in_in_out_result<int*, int*, int*>              inInOutResult;
        ranges::in_out_out_result<int*, int*, int*>             inOutOutResult;
        ranges::min_max_result<int>                             minMaxResult2;
        ranges::in_found_result<int*>                           inFoundResult;

        (void)inFunResult;
        (void)inInResult;
        (void)inOutResult;
        (void)inInOutResult;
        (void)inOutOutResult;
        (void)minMaxResult2;
        (void)inFoundResult;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
        {
            BSLMF_ASSERT(__cpp_lib_containers_ranges >= 202202L);
            bsl::from_range_t v{bsl::from_range};
            (void) v;
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
        {
            BSLMF_ASSERT(__cpp_lib_ranges_to_container >= 202202L);
            const int arr[] = {1, 2};
            auto v = bsl::ranges::to<std::vector>(arr);
            ASSERT(v.size() == 2);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
        {
            BSLMF_ASSERT(__cpp_lib_ranges >= 202202L);
            struct FirstChar : bsl::ranges::range_adaptor_closure<FirstChar> {
                std::string_view operator()(std::string_view s) const
                {
                    if (s.empty()) return {};
                    return s.substr(0, 1);
                }
            } firstChar;
            std::string_view char1 = std::string_view{"abc"} | firstChar;
            ASSERT(char1 == "a");
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
        {
            BSLMF_ASSERT(__cpp_lib_ranges_cartesian_product >= 202207L);
            const int nums[] = {1, 2, 3};
            const char chars[] = {'a', 'b'};
            bsl::ranges::cartesian_product_view res{
                                   bsl::views::cartesian_product(nums, chars)};
            const std::pair<int, char> expected[] = {
                {1, 'a'}, {1, 'b'},
                {2, 'a'}, {2, 'b'},
                {3, 'a'}, {3, 'b'}
            };
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE == 13
            ASSERT(bsl::ranges::equal(res, expected,
                                       std::equal_to<std::pair<int, char>>{}));
#else
            ASSERT(bsl::ranges::equal(res, expected));
#endif
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
        {
            BSLMF_ASSERT(__cpp_lib_ranges_chunk >= 202202L);
            const int nums[] = {1, 2, 3, 4, 5, 6};
            bsl::ranges::chunk_view view{bsl::views::chunk(nums, 2)};
            std::initializer_list<std::initializer_list<int>> expected =
                {{1, 2}, {3, 4}, {5, 6}};
            ASSERT(bsl::ranges::equal(view, expected, bsl::ranges::equal));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
        {
            BSLMF_ASSERT(__cpp_lib_ranges_enumerate >= 202302L);
            const char chars[] = {'a', 'b', 'c'};
            bsl::ranges::enumerate_view view{bsl::views::enumerate(chars)};
            const std::tuple<int, char> expected[] =
                {{0, 'a'}, {1, 'b'}, {2, 'c'}};
            ASSERT(bsl::ranges::equal(view, expected));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
        {
            BSLMF_ASSERT(__cpp_lib_ranges_join_with >= 202202L);
            const std::string_view words[] = {"w1", "w2", "w3"};
            bsl::ranges::join_with_view view{bsl::views::join_with(words,' ')};
            ASSERT(bsl::ranges::equal(view, std::string_view{"w1 w2 w3"}));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
        {
            BSLMF_ASSERT(__cpp_lib_ranges_slide >= 202202L);
            const int nums[] = {1, 2, 3, 4, 5};
            bsl::ranges::slide_view view{bsl::views::slide(nums, 3)};
            std::initializer_list<std::initializer_list<int>> expected =
                {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
            ASSERT(bsl::ranges::equal(view, expected, bsl::ranges::equal));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
        {
            BSLMF_ASSERT(__cpp_lib_ranges_stride >= 202207L);
            const int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            bsl::ranges::stride_view view{bsl::views::stride(nums, 3)};
            const int expected[] = {1, 4, 7};
            ASSERT(bsl::ranges::equal(view, expected));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
        {
            BSLMF_ASSERT(__cpp_lib_ranges_zip >= 202110L);
            const int nums[] = {1, 2, 3, 4, 5};
            const char chars[] = {'a', 'b', 'c'};
            {
                bsl::ranges::zip_view view{bsl::views::zip(nums, chars)};
                const std::pair<int, char> expected[] =
                    {{1, 'a'}, {2, 'b'}, {3, 'c'}};
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE == 13
                ASSERT(bsl::ranges::equal(view, expected,
                                       std::equal_to<std::pair<int, char>>{}));
#else
                ASSERT(bsl::ranges::equal(view, expected));
#endif
            }
            {
                const auto concat = [](int n, char c) {
                    return std::to_string(n) + c;
                };
                bsl::ranges::zip_transform_view view{
                               bsl::views::zip_transform(concat, nums, chars)};
                const std::string_view expected[] = {"1a", "2b", "3c"};
                ASSERT(bsl::ranges::equal(view, expected));
            }
            {
                bsl::ranges::adjacent_view view{bsl::views::adjacent<3>(nums)};
                const std::tuple<int, int, int> expected[] =
                    {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
                ASSERT(bsl::ranges::equal(view, expected));
            }
            {
                const auto sum = [](auto... nums) { return (0 + ... + nums); };
                bsl::ranges::adjacent_transform_view view{
                                 bsl::views::adjacent_transform<3>(nums, sum)};
                const int expected[] = {6, 9, 12};
                ASSERT(bsl::ranges::equal(view, expected));
            }
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
        {
            BSLMF_ASSERT(__cpp_lib_generator >= 202207L);
            const int nums[] = {1, 2};
            (void) bsl::ranges::elements_of(nums);
        }
#endif
        {
            int arr[3] = {1, 2, 3};
            // Defined in `bslstl_iterator.h` but must be also available
            // form `bslstl_ranges.h`
            ASSERT(bsl::ranges::begin(arr) == arr);
            ASSERT(bsl::ranges::end(arr) == arr + sizeof arr / sizeof arr[0]);
        }
        {
            bsl::from_range_t v = bsl::from_range;
            (void) v;
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
