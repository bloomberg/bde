// bslim_bslstandardheadertest.t.cpp                                  -*-C++-*-
#include <bslim_bslstandardheadertest.h>

#include <bsls_platform.h>

// These need to be define as early as possible to silence warnings, but after
// detection/configuration headers were included.
#ifdef BSLS_PLATFORM_CMP_MSVC
    #define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
    #define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
#endif

#include <bsls_libraryfeatures.h>

// These need to be define as early as possible to silence warnings, but after
// detection/configuration headers were included.
#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) && defined(__DEPRECATED)
    #define BSLIM_BSLSTANDARDHEADERTEST_GNU_DEPRECATED_WAS_SET
    #undef __DEPRECATED
#endif

#include <bslim_testutil.h>

#include <bslma_constructionutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslh_hashpair.h>
#include <bslh_defaulthashalgorithm.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_util.h>

#include <bsltf_templatetestfacility.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    #include <bslh_hashtuple.h>
    #include <tuple>     // 'std::tuple', 'std::make_tuple'
#endif //  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

// #include all of the headers defined in bsl+bslhdrs.

// The <version> header, added in C++20, defines numerous feature-test macros.
// Since these macros are also available when including various standard
// headers, in order to check the inclusion of the <version> header exactly, we
// have to break the rule of the coding standard and include <bsl_version.h>
// file before others.

#include <bsl_version.h>          // C++20 header

// Now we can check the availability of the feature-test macros, being sure
// that at the moment they could only be defined in the standard <version>
// header.

#if defined(__cpp_lib_generic_associative_lookup) &&                          \
    defined(__cpp_lib_string_udls) &&                                         \
    defined(__cpp_lib_chrono_udls)
    #define BSLIM_BSLSTANDARDHEADERTEST_VERSION_HEADER_WAS_INCLUDED
#endif

#include <bsl_algorithm.h>
#include <bsl_array.h>
#include <bsl_barrier.h>          // C++20 header
#include <bsl_bit.h>              // C++20 header
#include <bsl_bitset.h>
#include <bsl_c_assert.h>
#include <bsl_c_ctype.h>
#include <bsl_c_errno.h>
#include <bsl_c_float.h>
#include <bsl_c_iso646.h>
#include <bsl_c_limits.h>
#include <bsl_c_locale.h>
#include <bsl_c_math.h>
#include <bsl_c_setjmp.h>
#include <bsl_c_signal.h>
#include <bsl_c_stdarg.h>
#include <bsl_c_stddef.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>
#include <bsl_c_string.h>

#if !defined(BSLS_PLATFORM_CMP_MSVC)
// The POSIX header <sys/time.h> is not available from Visual Studio.
#include <bsl_c_sys_time.h>
#endif

#include <bsl_c_time.h>
#include <bsl_c_wchar.h>
#include <bsl_c_wctype.h>
#include <bsl_cassert.h>
#include <bsl_cctype.h>
#include <bsl_cerrno.h>
#include <bsl_cfloat.h>
#include <bsl_ciso646.h>
#include <bsl_climits.h>
#include <bsl_clocale.h>
#include <bsl_cmath.h>
#include <bsl_complex.h>
#include <bsl_concepts.h>         // C++20 header
#include <bsl_csetjmp.h>
#include <bsl_csignal.h>
#include <bsl_cstdarg.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>

#if defined(BSLS_PLATFORM_CMP_MSVC) && (BSLS_PLATFORM_CMP_VERSION >= 1900)
// The standard header <cuchar> is not available on most platforms.
#include <bsl_cuchar.h>
#endif

#include <bsl_cwchar.h>
#include <bsl_cwctype.h>
#include <bsl_deque.h>
#include <bsl_exception.h>
#include <bsl_execution.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_istream.h>
#include <bsl_iterator.h>
#include <bsl_latch.h>            // C++20 header
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_locale.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_mutex.h>
#include <bsl_new.h>
#include <bsl_numbers.h>          // C++20 header
#include <bsl_numeric.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_ranges.h>
#include <bsl_semaphore.h>        // C++20 header
#include <bsl_set.h>
#include <bsl_span.h>
#include <bsl_source_location.h>  // C++20 header
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_stop_token.h>       // C++20 header
#include <bsl_stdexcept.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_string_view.h>      // C++17 header ported to C++03
#include <bsl_system_error.h>     // C++11 header ported to C++03
#include <bsl_typeindex.h>        // C++11 header ported to C++03
#include <bsl_typeinfo.h>
#include <bsl_unordered_map.h>    // C++11 header ported to C++03
#include <bsl_unordered_set.h>    // C++11 header ported to C++03
#include <bsl_utility.h>
#include <bsl_valarray.h>
#include <bsl_vector.h>

#if !defined(BDE_OPENSOURCE_PUBLICATION) \
 && !defined(BDE_OMIT_INTERNAL_DEPRECATED)
    // stlport containers are deprecated and are not forwarded in the
    // opensource distribution

    #include <bsl_hash_map.h>
    #include <bsl_hash_set.h>
    #include <bsl_slist.h>
#endif  // ndef BDE_OMIT_INTERNAL_DEPRECATED && ndef BDE_OPENSOURCE_PUBLICATION

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #include <bsl_atomic.h>
    #include <bsl_cfenv.h>
    #include <bsl_chrono.h>
    #include <bsl_cinttypes.h>
    #include <bsl_condition_variable.h>
    #include <bsl_cstdbool.h>
    #include <bsl_ctgmath.h>
    #include <bsl_forward_list.h>
    #include <bsl_future.h>
    #include <bsl_initializer_list.h>
    #include <bsl_mutex.h>
    #include <bsl_random.h>
    #include <bsl_ratio.h>
    #include <bsl_regex.h>
    #include <bsl_scoped_allocator.h>
    #include <bsl_thread.h>
    #include <bsl_tuple.h>
#endif

#include <bsl_type_traits.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    #include <bsl_shared_mutex.h>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
    #include <bsl_filesystem.h>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
    #include <bsl_execution.h>
#endif

#include <utility>     // 'std::pair'

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

// Finished including potentially deprecated headers, rest of deprecations
// should be reported.
#ifdef BSLS_PLATFORM_CMP_MSVC
    #define snprintf _snprintf
    #undef _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING
    #undef _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
#endif
#ifdef BSLIM_BSLSTANDARDHEADERTEST_GNU_DEPRECATED_WAS_SET
    #define __DEPRECATED
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bslim;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component implements high level tests for standard library containers
// defined in 'bslstl'.
//
//-----------------------------------------------------------------------------
// [33] C++20 'bsl_concepts.h' HEADER
// [32] bsl::cmp_equal();
// [32] bsl::cmp_not_equal();
// [32] bsl::cmp_less();
// [32] bsl::cmp_greater();
// [32] bsl::cmp_less_equal();
// [32] bsl::cmp_greater_equal();
// [32] bsl::in_range();
// [31] MISC C++20 ADDITIONS TO HEADERS
// [30] C++20 'bsl_atomic.h' HEADER ADDITIONS
// [29] C++20 'bsl_memory.h' HEADER ADDITIONS
// [28] CONCERN: Entities from 'std::ranges' are available and usable.
// [27] CONCERN: The type 'bsl::stop_token' is available and usable.
// [27] CONCERN: The type 'bsl::stop_source' is available and usable.
// [27] CONCERN: The type 'bsl::nostopstate_t' is available and usable.
// [27] CONCERN: The type 'bsl::stop_callback' is available and usable.
// [27] CONCERN: 'bsl::nostopstate' is available and usable.
// [26] C++20 'bsl_source_location.h' HEADER
// [25] C++20 'bsl_bit.h' HEADER
// [24] C++20 'bsl_numbers.h' HEADER
// [23] CONCERN: 'bsl::barrier' is available and usable.
// [23] CONCERN: 'bsl::latch' is available and usable.
// [23] CONCERN: 'bsl::counting_semaphore' is available and usable.
// [23] CONCERN: 'bsl::binary_semaphore' is available and usable.
// [22] CONCERN: feature-test macros are available and usable.
// [21] CONCERN: 'bsl::span' is available and usable
// [20] CONCERN: 'bsl::invoke' is usable when available.
// [20] CONCERN: 'bsl::not_fn' is usable when available.
// [19] C++17 <bsl_filesystem.h>
// [19] CONCERN: 'hashAppend' of 'std::filesystem::path' is usable.
// [18] bsl::byte;
// [18] bsl::apply();
// [18] bsl::make_from_tuple();
// [18] bsl::aligned_alloc();
// [18] bsl::timespec_get();
// [17] bsl::assoc_laguerre();
// [17] bsl::assoc_laguerref();
// [17] bsl::assoc_laguerrel();
// [17] bsl::assoc_legendre();
// [17] bsl::assoc_legendref();
// [17] bsl::assoc_legendrel();
// [17] bsl::beta();
// [17] bsl::betaf();
// [17] bsl::betal();
// [17] bsl::comp_ellint_1();
// [17] bsl::comp_ellint_1f();
// [17] bsl::comp_ellint_1l();
// [17] bsl::comp_ellint_2();
// [17] bsl::comp_ellint_2f();
// [17] bsl::comp_ellint_2l();
// [17] bsl::comp_ellint_3();
// [17] bsl::comp_ellint_3f();
// [17] bsl::comp_ellint_3l();
// [17] bsl::cyl_bessel_i();
// [17] bsl::cyl_bessel_if();
// [17] bsl::cyl_bessel_il();
// [17] bsl::cyl_bessel_j();
// [17] bsl::cyl_bessel_jf();
// [17] bsl::cyl_bessel_jl();
// [17] bsl::cyl_bessel_k();
// [17] bsl::cyl_bessel_kf();
// [17] bsl::cyl_bessel_kl();
// [17] bsl::cyl_neumann();
// [17] bsl::cyl_neumannf();
// [17] bsl::cyl_neumannl();
// [17] bsl::ellint_1();
// [17] bsl::ellint_1f();
// [17] bsl::ellint_1l();
// [17] bsl::ellint_2();
// [17] bsl::ellint_2f();
// [17] bsl::ellint_2l();
// [17] bsl::ellint_3();
// [17] bsl::ellint_3f();
// [17] bsl::ellint_3l();
// [17] bsl::expint();
// [17] bsl::expintf();
// [17] bsl::expintl();
// [17] bsl::hermite();
// [17] bsl::hermitef();
// [17] bsl::hermitel();
// [17] bsl::legendre();
// [17] bsl::legendref();
// [17] bsl::legendrel();
// [17] bsl::laguerre();
// [17] bsl::laguerref();
// [17] bsl::laguerrel();
// [17] bsl::riemann_zeta();
// [17] bsl::riemann_zetaf();
// [17] bsl::riemann_zetal();
// [17] bsl::sph_bessel();
// [17] bsl::sph_besself();
// [17] bsl::sph_bessell();
// [17] bsl::sph_legendre();
// [17] bsl::sph_legendref();
// [17] bsl::sph_legendrel();
// [17] bsl::sph_neumann();
// [17] bsl::sph_neumannf();
// [17] bsl::sph_neumannl();
// [16] bsl::destroy()
// [16] bsl::destroy_at()
// [16] bsl::destroy_n()
// [16] bsl::uninitialized_default_construct()
// [16] bsl::uninitialized_default_construct_n()
// [16] bsl::uninitialized_move()
// [16] bsl::uninitialized_move_n()
// [16] bsl::uninitialized_value_construct()
// [16] bsl::uninitialized_value_construct_n()
// [15] bsl::scoped_lock;
// [14] bsl::align_val_t
// [14] bsl::launder()
// [13] bsl::clamp();
// [13] bsl::for_each_n();
// [13] bsl::sample();
// [12] bsl::exclusive_scan()
// [12] bsl::inclusive_scan()
// [12] bsl::gcd()
// [12] bsl::lcm()
// [12] bsl::transform_exclusive_scan()
// [12] bsl::transform_inclusive_scan()
// [12] bsl::reduce()
// [12] bsl::transform_reduce()
// [11] bsl::in_place_t
// [11] bsl::in_place
// [11] bsl::add_const<TYPE>::type& as_const(TYPE& value);
// [10] C++17 TYPE ALIASES
// [ 9] CONCERN: 'bslh::hashAppend' of 'std::pair'  is usable from 'bsl'.
// [ 9] CONCERN: 'bslh::hashAppend' of 'std::tuple' is usable from 'bsl'.
// [ 8] CONCERN: 'default_searcher'/'boyer_moore_horspool_searcher usable.
// [ 8] CONCERN: 'boyer_moore_searcher' usable when available.
// [ 8] CONCERN: 'bsl::search' function is usable.
// [ 5] CONCERN: Range functions are not ambiguous with 'std' under ADL
// [ 7] CONCERN: 'forward_list' is available in C++11 builds
// [ 4] maps of smart pointers
// [ 3] string vector resize
// [ 2] CONCERN: REGRESSION TEST FOR C99 FEATURES
// [ 1] CONCERN: Support references as 'mapped_type' in map-like containers.

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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

                            // ===================
                            // class MapTestDriver
                            // ===================

template <class CONTAINER>
class MapTestDriver {
    // This parameterized class provides a namespace for testing the high level
    // functionality of bsl map-like containers.  Each "testCase*" method test
    // a specific aspect of such containers.

  private:
    // TYPES

    typedef CONTAINER                            Obj;
    typedef typename CONTAINER::key_type         KeyType;
    typedef typename CONTAINER::mapped_type      MappedType;
    typedef typename CONTAINER::value_type       ValueType;
    typedef typename CONTAINER::allocator_type   AllocatorType;
    typedef typename CONTAINER::iterator         Iter;
    typedef typename CONTAINER::const_iterator   CIter;

    typedef bslma::ConstructionUtil              ConstrUtil;
    typedef bslmf::MovableRefUtil                MoveUtil;
    typedef bsltf::TemplateTestFacility          TTF;

  public:
    static void testCase1();
        // Testing reference as 'mapped_type'.
};

                         // -------------------
                         // class MapTestDriver
                         // -------------------

template <class CONTAINER>
void MapTestDriver<CONTAINER>::testCase1()
{
    // ------------------------------------------------------------------------
    // SUPPORT REFERENCES AS 'mapped_type'
    //
    // Concerns:
    //: 1 All bsl map-like containers (map, multimap, unordered_map,
    //:   unordered_multimap) accept references as 'mapped_type'.
    //:
    //: 2 Container does not make a copy of the referenced object on insertion.
    //:
    //: 3 The mapped value copy-assignment operator assigns to the original
    //:   object and does not rebind the reference.
    //:
    //: 4 The container copy-assignment operator does not copy mapped values.
    //:
    //: 5 The container copy constructor does not copy mapped values.
    //:
    //: 6 The erase operation does not destroy the referenced object.
    //
    // Plan:
    //: 1 Create the value type containing a reference as a 'mapped_type' and
    //:   insert it into the container.  (C-1)
    //:
    //: 2 Verify that the container inserts the reference and does not make a
    //:   copy of the referenced object.  (C-2)
    //:
    //: 3 Assign diffenent value to the mapped value and verify that the
    //:   referenced object is changed.  (C-3)
    //:
    //: 4 Copy-construct new container and verify that the mapped value in the
    //:   new container references original object.  (C-4)
    //:
    //: 5 Assign the original container to a different container and verify
    //:   that the mapped value in the new container references original
    //:   object.  (C-4)
    //:
    //: 5 Erase the container entry and verify that the original object is not
    //:   destroyed.  (C-4)
    //
    // Testing:
    //   CONCERN: Support references as 'mapped_type' in map-like containers.
    // ------------------------------------------------------------------------
    if (verbose) {
        cout << "\tTesting with: " << bsls::NameOf<ValueType>().name() << endl;
    }

    BSLMF_ASSERT(false == bsl::is_reference<KeyType>::value);
    BSLMF_ASSERT(true  == bsl::is_reference<MappedType>::value);

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bsls::ObjectBuffer<typename bsl::remove_const<KeyType>::type> tempKey;
    TTF::emplace(tempKey.address(), 1, &scratch);
    bslma::DestructorGuard<typename bsl::remove_const<KeyType>::type>
                                                   keyGuard(tempKey.address());

    bsls::ObjectBuffer<typename bsl::remove_reference<MappedType>::type>
                                                                    tempMapped;
    TTF::emplace(tempMapped.address(), 2, &scratch);
    bslma::DestructorGuard<typename bsl::remove_reference<MappedType>::type>
                                             mappedGuard(tempMapped.address());

    bsls::ObjectBuffer<typename bsl::remove_reference<MappedType>::type>
                                                                   tempMapped2;
    TTF::emplace(tempMapped2.address(), 18, &scratch);
    bslma::DestructorGuard<typename bsl::remove_reference<MappedType>::type>
                                           mappedGuard2(tempMapped2.address());

    CONTAINER mX;  const CONTAINER& X = mX;

    ValueType tempPair(tempKey.object(), tempMapped.object());

    mX.insert(MoveUtil::move(tempPair));

    Iter ret = mX.find(tempKey.object());

    ASSERT(ret != mX.end());

    // Reference still refers to the original object.
    ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());

    // Assign different value to the mapped.
    ret->second = tempMapped2.object();

    // Reference still refers to the original object.
    ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());
    ASSERT(tempMapped.object() == tempMapped2.object());

    // Copy-construct container.
    {
        CONTAINER mY(X);  const CONTAINER& Y = mY;

        CIter ret = Y.find(tempKey.object());

        ASSERT(ret != Y.end());

        // Reference still refers to the original object.
        ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());
    }

    // Copy-assign container.
    {
        CONTAINER mY;  const CONTAINER& Y = mY;

        mY = X;

        CIter ret = Y.find(tempKey.object());

        ASSERT(ret != Y.end());

        // Reference still refers to the original object.
        ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());
    }

    // Erasing the value.
    mX.erase(tempKey.object());

    ret = mX.find(tempKey.object());

    ASSERT(ret == mX.end());

    ASSERT(tempMapped.object() == tempMapped2.object());

    cout<< "here3\n";
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
namespace TestCxx17TypeAliases {
    // This namespace is for testing the C++17 type aliases that have been to
    // bsl.  The goal here is not to ensure that the platform standard library
    // has implemented this functionality correctly, but instead to ensure that
    // the aliases and inline variables added to  bsl match the types and
    // values from the platform library.

namespace SW {
    struct SwapA {
        // a class that can't be copied or assigned, so the default
        // implementation of 'swap' will not work.
        SwapA           (SwapA const&) = delete;
        SwapA& operator=(SwapA const&) = delete;
    };

    struct SwapB {
        // a class that can't be copied or assigned, so the default
        // implementation of 'swap' will not work.
        SwapB           (SwapB const&) = delete;
        SwapB& operator=(SwapB const&) = delete;
    };

    struct SwapC {};

    void swap(SwapA&, SwapA&)
        // Exchange two variables of type 'SwapA'.
    {}

    void swap(SwapA&, SwapB&) noexcept
        // Exchange the contents of a 'SwapA' and 'SwapB', w/o throwing.
    {}

    void swap(SwapB&, SwapA&) noexcept
        // Exchange the contents of a 'SwapB' and 'SwapA', w/o throwing.
    {}

    void swap(SwapA&, SwapC&) noexcept
        // Exchange the contents of a 'SwapB' and 'SwapA', w/o throwing.
    {}

    void swap(SwapC&, SwapA&)
        // Exchange the contents of a 'SwapA' and 'SwapC'.
    {}
}  // close namespace SW

void Conjunction ()
    // Test that 'bsl::conjunction_v<TYPES...>' returns the same value as
    // 'bsl::conjunction<TYPES...>::value.
{
    typedef bsl::false_type F;
    typedef bsl::true_type  T;

    static_assert(bsl::conjunction  <F>::value == bsl::conjunction_v<F>);
    static_assert(bsl::conjunction  <T>::value == bsl::conjunction_v<T>);
    static_assert(bsl::conjunction  <F, F>::value == bsl::conjunction_v<F, F>);
    static_assert(bsl::conjunction  <F, T>::value == bsl::conjunction_v<F, T>);
    static_assert(bsl::conjunction  <T, F>::value == bsl::conjunction_v<T, F>);
    static_assert(bsl::conjunction  <T, T>::value == bsl::conjunction_v<T, T>);
}

void Disjunction ()
    // Test that 'bsl::disjunction_v<TYPES...>' returns the same value as
    // 'bsl::disjunction<TYPES...>::value.
{
    typedef bsl::false_type F;
    typedef bsl::true_type  T;

    static_assert(bsl::disjunction<F>::value    == bsl::disjunction_v<F>);
    static_assert(bsl::disjunction<T>::value    == bsl::disjunction_v<T>);
    static_assert(bsl::disjunction  <F, F>::value == bsl::disjunction_v<F, F>);
    static_assert(bsl::disjunction  <F, T>::value == bsl::disjunction_v<F, T>);
    static_assert(bsl::disjunction  <T, F>::value == bsl::disjunction_v<T, F>);
    static_assert(bsl::disjunction  <T, T>::value == bsl::disjunction_v<T, T>);
}

void HasUniqueObjectReps ()
    // Test that 'bsl::has_unique_object_representations_v<TYPE>' returns the
    // same value as 'bsl::has_unique_object_representations<TYPE>::value'.
{
    struct S { int d_i; };

    static_assert(bsl::has_unique_object_representations  <int>::value ==
                  bsl::has_unique_object_representations_v<int>);
    static_assert(bsl::has_unique_object_representations  <S>::value ==
                  bsl::has_unique_object_representations_v<S>);

}

void IsAggregate ()
    // Test that 'bsl::is_aggregate_v<TYPE>' returns the same value as
    // 'bsl::is_aggregate<TYPE>::value'.
{
    struct S { int d_i; };

    static_assert(bsl::is_aggregate  <int>::value == bsl::is_aggregate_v<int>);
    static_assert(bsl::is_aggregate<S>::value == bsl::is_aggregate_v<S>);
}

void IsInvocable ()
    // Test that 'bsl::is_invocable_v<FN, Args...>' returns the same value as
    // 'bsl::is_invocable<FN, Args...>::value'.
{
    static_assert(bsl::is_invocable  <int>::value == bsl::is_invocable_v<int>);
    static_assert(bsl::is_invocable  <int(), long>::value ==
                  bsl::is_invocable_v<int(), long>);
}

void IsInvocableR ()
    // Test that 'bsl::is_invocable_r_v<RET, FN, Args...>' returns the same
    // value as 'bsl::is_invocable_r<RET, FN, Args...>::value'.
{
    static_assert(bsl::is_invocable  <void, int>::value ==
                  bsl::is_invocable_v<void, int>);
    static_assert(bsl::is_invocable  <float, float(int), int>::value ==
                  bsl::is_invocable_v<float, float(int), int>);
}

void IsSwappable ()
    // Test that 'bsl::is_swappable_v<TYPE>' and
    // 'bsl::is_nothrow_swappable_v<TYPE>' return the same values as
    // 'bsl::is_swappable<TYPE>::value' and
    // 'bsl::is_nothrow_swappable<TYPE>::value', respectively.
{
    static_assert(bsl::is_swappable  <SW::SwapA>::value ==
                  bsl::is_swappable_v<SW::SwapA>);
    static_assert(bsl::is_swappable  <char>::value ==
                  bsl::is_swappable_v<char>);
    static_assert(bsl::is_nothrow_swappable  <SW::SwapA>::value ==
                  bsl::is_nothrow_swappable_v<SW::SwapA>);
    static_assert(bsl::is_nothrow_swappable  <char>::value ==
                  bsl::is_nothrow_swappable_v<char>);
}

void IsSwappableWith ()
    // Test that 'bsl::is_swappable_with_v<TYPE>' and
    // 'bsl::is_nothrow_swappable_with_v<TYPE>' return the same values as
    // 'bsl::is_swappable_with<TYPE>::value' and
    // 'bsl::is_nothrow_swappable_with<TYPE>::value', respectively.
{
    static_assert(bsl::is_swappable_with  <SW::SwapA, SW::SwapB>::value ==
                  bsl::is_swappable_with_v<SW::SwapA, SW::SwapB>);
    static_assert(bsl::is_swappable_with  <SW::SwapA, SW::SwapC>::value ==
                  bsl::is_swappable_with_v<SW::SwapA, SW::SwapC>);
    static_assert(
        bsl::is_nothrow_swappable_with  <SW::SwapA, SW::SwapB>::value ==
        bsl::is_nothrow_swappable_with_v<SW::SwapA, SW::SwapB>);
    static_assert(
        bsl::is_nothrow_swappable_with  <SW::SwapA, SW::SwapC>::value ==
        bsl::is_nothrow_swappable_with_v<SW::SwapA, SW::SwapC>);
}

void Negation ()
    // Test that 'bsl::negation_v<TYPE>' returns the same value as
    // 'bsl::negation<TYPE>::value'.
{
    typedef bsl::false_type F;
    typedef bsl::true_type  T;

    static_assert(bsl::negation<F>::value == bsl::negation_v<F>);
    static_assert(bsl::negation<T>::value == bsl::negation_v<T>);
}

}  // close namespace TestCxx17TypeAliases


struct addOne {
    template <class TYPE>
    constexpr auto operator()(TYPE x) const noexcept
        // return the specified 'x' + 1
    {
        return static_cast<TYPE>(x + 1);
    }
};


struct TwoArgumentCtor {
    TwoArgumentCtor(double d, int i) : d_val(d + i) {}
    double d_val;
};

double TwoArgumentFunction(double d, int i)
    // Return the sum of the specified 'd' and 'i'.
{
    return d + i;
}

struct IsEven {
    bool operator()(int x) const noexcept
        // return 'true' if the specified 'x' is even, and 'false' otherwise.
    {
        return 0 == (x & 1);
    }
};

namespace {

struct CountingFunctor {
    static size_t s_count;
    // A functor that does nothing but call how many times it is called.

    template <class TYPE>
    void operator () (const TYPE &) const
        // update the count of times that this has been called
    {
        ++s_count;
    }
};

size_t CountingFunctor::s_count = 0;
}  // close unnamed namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
class CompletionFunction {
    // This class minimally satisfies the requirements of the 'bsl::barrier'
    // template parameter.

  public:
    // MANIPULATORS
    void operator()() noexcept {}
};
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

                            // =====================
                            // class RangesDummyView
                            // =====================

class RangesDummyView : public ranges::view_interface<RangesDummyView> {
    // This class is inherited from 'bsl::ranges::view_interface' and its only
    // purpose is to prove that 'bsl::ranges::view_interface' exists and is
    // usable.

  public:
    // ACCESSORS
    constexpr bool empty() const
        // Unconditionally return 'true'.
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
    static constexpr result_type min()
        // Unconditionally return '1u'.
    {
        return 1u;
    }

    static constexpr result_type max()
        // Unconditionally return '5u'.
    {
        return 5u;
    }

    // MANIPULATORS
    result_type operator()()
        // Unconditionally return '1u'.
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
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << "\n";

    switch (test) { case 0:  // Zero is always the leading case.
      case 33: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_concepts.h' HEADER
        //
        // Concerns:
        //: 1 The definitions from '<concepts>' defined by the C++20 Standard
        //:   are available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bsl_concepts.h'.
        //
        // Plan:
        //: 1 Form some valid expressions with every name with 'bsl' prefix.
        //
        // Testing
        //   C++20 'bsl_concepts.h' HEADER
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 'bsl_concepts.h' HEADER"
                            "\n=====================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        BSLMF_ASSERT(( bsl::same_as<int, int>));
        BSLMF_ASSERT((!bsl::same_as<int, char>));

        {
            class B {};
            class D : public B {};
            BSLMF_ASSERT(( bsl::derived_from<D, B>));
            BSLMF_ASSERT((!bsl::derived_from<B, D>));
        }
        {
            class C {};
            BSLMF_ASSERT(( bsl::convertible_to<char, int>));
            BSLMF_ASSERT(( bsl::convertible_to<int, char>));
            BSLMF_ASSERT(( bsl::convertible_to<int, int>));
            BSLMF_ASSERT((!bsl::convertible_to<C, int>));
        }
        {
            class C {};
            BSLMF_ASSERT(( bsl::common_reference_with<int, char>));
            BSLMF_ASSERT(( bsl::common_reference_with<int, const int>));
            BSLMF_ASSERT((!bsl::common_reference_with<C, int>));
            BSLMF_ASSERT((!bsl::common_reference_with<int*, unsigned*>));
        }

        BSLMF_ASSERT(( bsl::common_with<int, char>));
        BSLMF_ASSERT(( bsl::common_with<int*, void*>));
        BSLMF_ASSERT((!bsl::common_with<int*, char*>));

        BSLMF_ASSERT(( bsl::integral<int>));
        BSLMF_ASSERT(( bsl::integral<char>));
        BSLMF_ASSERT((!bsl::integral<double>));

        BSLMF_ASSERT(( bsl::signed_integral<int>));
        BSLMF_ASSERT((!bsl::signed_integral<unsigned>));

        BSLMF_ASSERT(( bsl::unsigned_integral<unsigned>));
        BSLMF_ASSERT((!bsl::unsigned_integral<int>));

        BSLMF_ASSERT(( bsl::floating_point<double>));
        BSLMF_ASSERT((!bsl::floating_point<int>));

        BSLMF_ASSERT(( bsl::assignable_from<int&, int>));
        BSLMF_ASSERT((!bsl::assignable_from<int, int>));

        {
            struct C {
                C(const C&) = delete;
                C &operator=(const C&) = delete;
            };
            BSLMF_ASSERT(( bsl::swappable<int>));
            BSLMF_ASSERT((!bsl::swappable<C>));
        }

        BSLMF_ASSERT(( bsl::swappable_with<int&, int&>));
        BSLMF_ASSERT((!bsl::swappable_with<int, int>));

        {
            class C { ~C() = delete; };
            BSLMF_ASSERT(( bsl::destructible<int>));
            BSLMF_ASSERT((!bsl::destructible<C>));
        }

        BSLMF_ASSERT(( bsl::constructible_from<int, int>));
        BSLMF_ASSERT((!bsl::constructible_from<int, int*>));

        BSLMF_ASSERT(( bsl::default_initializable<int>));
        BSLMF_ASSERT((!bsl::default_initializable<int&>));

        {
            struct C { C(C&&) = delete; };
            BSLMF_ASSERT(( bsl::move_constructible<int>));
            BSLMF_ASSERT((!bsl::move_constructible<C>));
        }

        {
            struct C { C(const C&) = delete; };
            BSLMF_ASSERT(( bsl::copy_constructible<int>));
            BSLMF_ASSERT((!bsl::copy_constructible<C>));
        }
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING C++20 <BSL_UTILITY.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++20 special integer comparison functions all exist in
        //:   namespace 'bsl'.
        //:
        //: 2 The feature test macros defined in '<utility>' are available and
        //:   have appropriate values.
        //
        // Plan:
        //: 1 Verify that '__cpp_lib_integer_comparison_functions >= 202002L'.
        //:   (C-2)
        //:
        //: 2 Create simple examples that use these functions.  Compilation of
        //:   the example demonstrates that the calls can be found in 'bsl'.
        //:   (C-1)
        //
        // Testing:
        //   bsl::cmp_equal();
        //   bsl::cmp_not_equal();
        //   bsl::cmp_less();
        //   bsl::cmp_greater();
        //   bsl::cmp_less_equal();
        //   bsl::cmp_greater_equal();
        //   bsl::in_range();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 <BSL_UTILITY.H> ADDITIONS"
                            "\n=======================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_integer_comparison_functions >= 202002L);

        ASSERT(!bsl::cmp_equal        (-1, 1U));
        ASSERT( bsl::cmp_less         (-1, 1U));
        ASSERT( bsl::cmp_less_equal   (-1, 1U));
        ASSERT(!bsl::cmp_greater      (-1, 1U));
        ASSERT(!bsl::cmp_greater_equal(-1, 1U));
        ASSERT( bsl::cmp_not_equal    (-1, 1U));

        ASSERT(!bsl::in_range<unsigned int>(-1));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING MISC C++20 ADDITIONS TO HEADERS
        //
        // Concerns:
        //: 1 'lerp' is available in C++20 mode in the 'bsl' namespace to users
        //:   who include 'bsl_cmath.h'.
        //:
        //: 2 'midpoint' is available in C++20 mode in the 'bsl' namespace to
        //:   users who include 'bsl_numeric.h'.
        //:
        //: 3 'mbrtoc8' and 'c8rtomb' are available in C++20 mode in the 'bsl'
        //:   namespace to users who include 'bsl_cuchar.h' if
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV' macro is defined.
        //:
        //: 4 'unsequenced_policy' and 'unseq' are available in C++20 mode in
        //:   the 'bsl::execution' namespace to users who include
        //:   'bsl_execution.h' if
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS' macro is
        //:   defined.  (Implementation of the "parallel algorithms" feature
        //:   often depends on 3rd-party libraries like Intel TBB, so it isn't
        //:   always available.  That's why we need an additional check.)
        //:
        //: 5 'bind_front' is available in C++20 mode in the 'bsl' namespace to
        //:   users who include 'bsl_functional.h'.
        //:
        //: 6 The feature test macros defined for the imported features are
        //:   available and have appropriate values.
        //
        // Plan:
        //: 1 Verify that
        //:    o '__cpp_lib_interpolate >= 201902L',
        //:    o '__cpp_lib_execution >= 201902L',
        //:    o '__cpp_lib_bind_front >= 201907L'.
        //:
        //: 2 Form some valid expression with every name with 'bsl' prefix.
        //
        // Testing
        //   MISC C++20 ADDITIONS TO HEADERS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MISC C++20 ADDITIONS TO HEADERS"
                            "\n=======================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_interpolate >= 201902L);
        (void) bsl::lerp(1.0, 1.0, 1.0);
        (void) bsl::midpoint(1, 1);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
        (void) [](char8_t *out, bsl::mbstate_t *st) {
            (void) bsl::mbrtoc8(out, "", 0U, st);
        };
        (void) [](char *out, bsl::mbstate_t *st) {
            (void) bsl::c8rtomb(out, {}, st);
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
        BSLMF_ASSERT(__cpp_lib_execution >= 201902L);
        (void) (bsl::execution::unsequenced_policy *) 0;
        (void) bsl::execution::unseq;
#endif

        BSLMF_ASSERT(__cpp_lib_bind_front >= 201907L);
        (void) bsl::bind_front([](int){}, 1);
#endif
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_atomic.h' HEADER ADDITIONS
        //
        // Concerns:
        //: 1 The following names are available in 'bsl' to users who include
        //:   'bsl_atomic.h':
        //:    o 'atomic_ref';
        //:    o 'atomic_char8_t';
        //:    o 'atomic_signed_lock_free', 'atomic_unsigned_lock_free';
        //:    o 'atomic_wait', 'atomic_wait_explicit';
        //:    o 'atomic_notify_one', 'atomic_notify_all';
        //:    o 'atomic_flag_notify_one', 'atomic_flag_notify_all';
        //:    o 'atomic_flag_test', 'atomic_flag_test_explicit'.
        //:
        //: 2 The feature test macros defined in '<atomic>' for the imported
        //:   features are available and have appropriate values.
        //
        // Plan:
        //: 1 Verify that
        //:    o '__cpp_lib_atomic_ref >= 201806L',
        //:    o '__cpp_lib_char8_t >= 201907L',
        //:    o '__cpp_lib_atomic_lock_free_type_aliases >= 201907L',
        //:    o '__cpp_lib_atomic_wait >= 201907L',
        //:    o '__cpp_lib_atomic_flag_test >= 201907L'.
        //:
        //: 2 Form some valid expressions with every name with 'bsl' prefix.
        //
        // Testing
        //   C++20 'bsl_atomic.h' HEADER ADDITIONS
        // --------------------------------------------------------------------
        if (verbose) printf(
                          "\nTESTING C++20 'bsl_atomic.h' HEADER ADDITIONS"
                          "\n=============================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_atomic_ref >= 201806L);
        int value = 0;
        (void) bsl::atomic_ref<int>{value};

        BSLMF_ASSERT(__cpp_lib_char8_t >= 201907L);
        (void) bsl::atomic_char8_t{};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
        BSLMF_ASSERT(__cpp_lib_atomic_lock_free_type_aliases >= 201907L);
        (void) bsl::atomic_signed_lock_free{};
        (void) bsl::atomic_unsigned_lock_free{};
#endif

        BSLMF_ASSERT(__cpp_lib_atomic_wait >= 201907L);
        (void) [](bsl::atomic<int> *ptr) {
            bsl::atomic_wait(ptr, 1);
            bsl::atomic_wait_explicit(ptr, 1, bsl::memory_order::relaxed);
            bsl::atomic_notify_one(ptr);
            bsl::atomic_notify_all(ptr);
        };
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
        (void) [](bsl::atomic_flag *ptr) {
            bsl::atomic_flag_wait(ptr, true);
            bsl::atomic_flag_wait_explicit(ptr,
                                           true,
                                           bsl::memory_order::relaxed);
            bsl::atomic_flag_notify_one(ptr);
            bsl::atomic_flag_notify_all(ptr);
        };
#endif

        BSLMF_ASSERT(__cpp_lib_atomic_flag_test >= 201907L);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
        (void) [](bsl::atomic_flag *ptr) {
            (void) bsl::atomic_flag_test(ptr);
            (void) bsl::atomic_flag_test_explicit(ptr,
                                                  bsl::memory_order::relaxed);
        };
#endif
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_memory.h' HEADER ADDITIONS
        //
        // Concerns:
        //: 1 'to_address', 'assume_aligned', 'make_unique_for_overwrite' and
        //:   'construct_at' are available in 'bsl' to users who include
        //:   'bsl_memory.h'.
        //:
        //: 2 The feature test macros defined in '<memory>' for the imported
        //:   features are available and have appropriate values.
        //
        // Plan:
        //: 1 Verify that
        //:    o '__cpp_lib_to_address >= 201711L',
        //:    o '__cpp_lib_assume_aligned >= 201811L',
        //:    o '__cpp_lib_smart_ptr_for_overwrite >= 202002L',
        //:    o '__cpp_lib_constexpr_memory >= 201811L'.
        //:
        //: 2 Verify that 'bsl::to_address' returns correct address.
        //:
        //: 3 Verify that 'bsl::assume_aligned' can be called.
        //:
        //: 4 Verify that 'bsl::construct_at' can be called.
        //:
        //: 5 Verify that 'bsl::make_unique_for_overwrite' can be called with
        //:   single object template argument as well as with array template
        //:   argument.
        //:
        //: 6 Verify that 'bsl::pointer_traits' is usable in constexpr context.
        //
        // Testing
        //   C++20 'bsl_memory.h' HEADER ADDITIONS
        // --------------------------------------------------------------------
        if (verbose) printf(
                          "\nTESTING C++20 'bsl_memory.h' HEADER ADDITIONS"
                          "\n=============================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_to_address >= 201711L);
        int intValue = 0;
        ASSERT(bsl::to_address(&intValue) == &intValue);

        BSLMF_ASSERT(__cpp_lib_assume_aligned >= 201811L);
        (void) bsl::assume_aligned<alignof(int)>(&intValue);

        alignas(int) char buf[sizeof(int)];
        bsl::destroy_at(bsl::construct_at<int>(reinterpret_cast<int*>(buf)));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
#if !defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) || _GLIBCXX_RELSEASE >= 12
        // GNU libstdc++ 11 doesn't define this (but defines the functions)
        BSLMF_ASSERT(__cpp_lib_smart_ptr_for_overwrite >= 202002L);
#endif
        (void) bsl::make_unique_for_overwrite<int>();
        (void) bsl::make_unique_for_overwrite<int[]>(4);
#endif
        //bsl::make_shared_for_overwrite() // own implementation
        //bsl::allocate_shared_for_overwrite() // own implementation

        BSLMF_ASSERT(__cpp_lib_constexpr_memory >= 201811L);
        BSLMF_ASSERT((
                bsl::pointer_traits<int*>::pointer_to(intValue) == &intValue));

        //BSLMF_ASSERT(__cpp_lib_shared_ptr_arrays >= 201707L);
        //bsl::make_shared<int[]>() // own implementation

        //BSLMF_ASSERT(__cpp_lib_atomic_shared_ptr >= 201711L);
        //bsl::atomic<bsl::shared_ptr> // not implemented

        //BSLMF_ASSERT(__cpp_lib_constexpr_dynamic_alloc >= 201907L);
        // constexpr 'bsl::allocator' // not implemented
#endif
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING C++20 'std::ranges' ADDITION
        //
        // Concerns:
        //: 1 The definitions from '<ranges>' defined by the C++20 Standard are
        //:   available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bsl_ranges.h'.
        //:
        //: 2 Constrained versions of algorithms defined by the C++20 Standard
        //:   are available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bsl_algorithm.h'.
        //
        // Plan:
        //: 1 For every type from the 'std::ranges' namespace aliased in the
        //:   <bsl_ranges.h>, verify that the type exists and is usable.  (C-1)
        //:
        //: 2 For every algorithm from the 'std::ranges' namespace aliased in
        //:   the 'bslstl_algorithm.h', verify that the function exists and is
        //:   usable.  (C-2)
        //
        // Testing
        //   CONCERN: Entities from 'std::ranges' are available and usable.
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 'std::ranges' ADDITION"
                            "\n====================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        namespace ranges = bsl::ranges;

        // Testing types aliased in the 'bsl_ranges.h'

        using Vector = bsl::vector<int>;
        using Tuple  = bsl::tuple<int, int>;

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
        (void) base;            // suppress compiler warning

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

        // 'RangesDummyView' is inherited from 'bsl::ranges::view_interface'.

        RangesDummyView dummy;
        ASSERTV(true == dummy.empty())

        ranges::subrange subrange(vec.begin(), vec.end());
        ASSERTV(5 == subrange.size())

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

        bsl::istringstream                    iStringStream("1 2 3");
        ranges::basic_istream_view<int, char> basicIStreamView(iStringStream);
        auto                                  beginResult =
                                                     *basicIStreamView.begin();
        ASSERTV(beginResult, 1 == beginResult);

        ranges::istream_view<int> iStreamView(iStringStream);
        beginResult = *iStreamView.begin();
        ASSERTV(beginResult, 2 == beginResult);

        bsl::wstring wString;
        wString.push_back('1');
        bsl::wistringstream        wIStringStream(wString);
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
        (void) refViewRef;

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

        Vector            vec1{6, 7, 8};
        Vector            vecArray[] = {vec, vec1};
        ranges::join_view joinView = bsl::views::join(vecArray);
        ASSERTV(joinView.front(), 1 == joinView.front());  // vecArray[0][0]
        ASSERTV(joinView.back(),  8 == joinView.back() );  // vecArray[1][2]

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

        // The types of 'keysView' and 'valuesView' in the following two tests
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
        (void) subrangeKind;

        // Testing algorithms aliased in the 'bsl_algorithm.h'

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

        ranges::for_each_result forEachResult =
                               ranges::for_each(vecToTransform, multiplyByTwo);
        ASSERTV(0 == ranges::distance(vecToTransform.end(), forEachResult.in));
        ASSERTV(vecToTransform[0], 2 == vecToTransform[0]);

        ranges::for_each_n_result forEachNResult =
                  ranges::for_each_n(vecToTransform.begin(), 1, multiplyByTwo);
        ASSERTV(1 == ranges::distance(vecToTransform.begin(),
                                      forEachNResult.in));
        ASSERTV(vecToTransform[0], 4 == vecToTransform[0]);

        ASSERT(1 == ranges::count   (vec, 2));
        ASSERT(3 == ranges::count_if(vec, greaterThanTwo));

        ranges::mismatch_result mismatchResult =
                                         ranges::mismatch(vec, vecToTransform);
        ASSERT(0 == ranges::distance(vec.begin(), mismatchResult.in1));
        ASSERT(0 == ranges::distance(vecToTransform.begin(),
                                     mismatchResult.in2));

        ASSERT(true  == ranges::equal                  (vec, vec));
        ASSERT(false == ranges::lexicographical_compare(vec, vec));

        auto findResult         = ranges::find(vec, 1);
        ASSERT(0 == ranges::distance(vec.begin(), findResult));

        auto findIfResult       = ranges::find_if(vec, greaterThanTwo);
        ASSERT(2 == ranges::distance(vec.begin(), findIfResult));

        auto findIfNotResult    = ranges::find_if_not(vec, greaterThanTwo);
        ASSERT(0 == ranges::distance(vec.begin(), findIfNotResult));

        auto findEndResult      = ranges::find_end(vec, vecToTransform);
        ASSERT(0 == ranges::distance(vec.end(), findEndResult.begin()));

        auto findFirstOfResult  = ranges::find_first_of(vec, vec);
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

        ranges::copy_result copyResult =  ranges::copy(vec,
                                                       vecToCopyTo.begin());
        ASSERT(0 == ranges::distance(vec.end(), copyResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 4, 5

        ranges::copy_if_result copyIfResult = ranges::copy_if(
                                                       vecToTransform,
                                                       vecToCopyTo.begin() + 5,
                                                       greaterThanTwo);
        ASSERT(0 == ranges::distance(vecToTransform.end(), copyIfResult.in));
        ASSERTV(vecToCopyTo[5], 4  == vecToCopyTo[5]); // ... 4, 4, 6, 8, 10

        ranges::copy_n_result copyNResult = ranges::copy_n(
                                                     vecToCopyTo.begin(),
                                                     5,
                                                     vecToCopyTo.begin() + 10);
        ASSERT(0 == ranges::distance(vecToCopyTo.begin() + 5, copyNResult.in));
        ASSERTV(vecToCopyTo[10], 1  == vecToCopyTo[10]);  // ... 1, 2, 3, 4, 5

        ranges::copy_backward_result copyBackResult = ranges::copy_backward(
                                                            vec,
                                                            vecToCopyTo.end());
        ASSERT(0 == ranges::distance(vec.end(), copyBackResult.in));
        ASSERTV(vecToCopyTo[15], 1  == vecToCopyTo[15]);  // ... 1, 2, 3, 4, 5

        Vector vecToMoveFrom1 = {5, 4, 3, 2, 1};

        auto moveResult = ranges::move(vecToMoveFrom1, vecToCopyTo.begin());
        ASSERT(0 == ranges::distance(vecToMoveFrom1.end(), moveResult.in));
        ASSERTV(vecToCopyTo[0], 5 == vecToCopyTo[0]);  // 5, 4, 3, 2, 1 ...

        Vector vecToMoveFrom2 = {5, 4, 3, 2, 1};

        ranges::move_backward_result moveBackResult = ranges::move_backward(
                                                     vecToMoveFrom2,
                                                     vecToCopyTo.begin() + 10);
        ASSERT(0 == ranges::distance(vecToMoveFrom2.end(), moveBackResult.in));
        ASSERTV(vecToCopyTo[5], 5 == vecToCopyTo[5]);  // ... 5, 4, 3, 2, 1 ...

        ranges::fill(vecToTransform, -1);
        ASSERTV(vecToTransform[0], -1 == vecToTransform[0]);

        ranges::fill_n(vecToTransform.begin(), 1, 1);
        ASSERTV(vecToTransform[0],  1 == vecToTransform[0]);
        ASSERTV(vecToTransform[1], -1 == vecToTransform[1]);

        ranges::unary_transform_result uTransformResult = ranges::transform(
                                                        vecToTransform,
                                                        vecToTransform.begin(),
                                                        multiplyByTwo);
        ASSERT(0 == ranges::distance(vecToTransform.end(),
                                     uTransformResult.in));
        ASSERTV(vecToTransform[0], 2 == vecToTransform[0]);

        ranges::binary_transform_result bTransformResult = ranges::transform(
                                                       vec,
                                                       vecToTransform,
                                                       vecToTransform.begin(),
                                                       bsl::multiplies<int>());
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

        ranges::remove(vecToTransform, 2);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        vecToTransform[1] = 3;

        ranges::remove_if(vecToTransform, greaterThanTwo);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        ranges::remove_copy_result removeCopyResult = ranges::remove_copy(
                                                           vec,
                                                           vecToCopyTo.begin(),
                                                           1);
        ASSERTV(0 == ranges::distance(vec.end(), removeCopyResult.in));
        ASSERTV(vecToCopyTo[0], 2 == vecToCopyTo[0]);  // 2, 3, 4, 5, 1, ...

        ranges::remove_copy_if_result removeCopyIfResult =
                                             ranges::remove_copy_if(
                                                           vec,
                                                           vecToCopyTo.begin(),
                                                           greaterThanTwo);
        ASSERTV(0 == ranges::distance(vec.end(), removeCopyIfResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 4, 5, 1, ...

        ranges::replace(vecToTransform, 1, 3);
        ASSERTV(vecToTransform[1], 3 == vecToTransform[1]);  // 3, 3, 3, 3, 3

        ranges::replace_if(vecToTransform, greaterThanTwo, 1);
        ASSERTV(vecToTransform[1], 1 == vecToTransform[1]);  // 1, 1, 1, 1, 1

        ranges::replace_copy_result replaceCopyResult = ranges::replace_copy(
                                                           vec,
                                                           vecToCopyTo.begin(),
                                                           1,
                                                           3);
        ASSERTV(0 == ranges::distance(vec.end(), replaceCopyResult.in));
        ASSERTV(vecToCopyTo[0], 3 == vecToCopyTo[0]);  // 3, 2, 4, 5, 3, ...

        ranges::replace_copy_if_result replaceCopyIfResult =
                                             ranges::replace_copy_if(
                                                           vec,
                                                           vecToCopyTo.begin(),
                                                           greaterThanTwo,
                                                           1);
        ASSERTV(0 == ranges::distance(vec.end(), replaceCopyIfResult.in));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 1, 1, 1, ...

        Vector vecToTransform1{1, 2, 3, 4, 5};

        ranges::swap_ranges_result swapRangesResult = ranges::swap_ranges(
                                                              vecToTransform,
                                                              vecToTransform1);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      swapRangesResult.in1));
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]);  // 1, 2, 3, 4, 5

        ranges::reverse(vecToTransform);
        ASSERTV(vecToTransform[1], 4 == vecToTransform[1]);  // 5, 4, 3, 2, 1

        ranges::reverse_copy_result reverseCopyResult = ranges::reverse_copy(
                                                          vec,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), reverseCopyResult.in));
        ASSERTV(vec[1],         2 == vec[1]);          // 1, 2, 3, 4, 5
        ASSERTV(vecToCopyTo[1], 4 == vecToCopyTo[1]);  // 5, 4, 3, 2, 1, ...

        ranges::rotate(vecToTransform, vecToTransform.begin() + 1);
        ASSERTV(vecToTransform[1], 3 == vecToTransform[1]);  // 4, 3, 2, 1, 5

        ranges::rotate_copy_result rotateCopyResult = ranges::rotate_copy(
                                                          vec,
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

        ranges::unique(vecToTransform);
        ASSERTV(vecToTransform[1], 2 == vecToTransform[1]); // 1, 2, ?, ?, ?

        vecToTransform = Vector{1, 1, 1, 2, 2};

        ranges::unique_copy_result uniqueCopyResult = ranges::unique_copy(
                                                          vecToTransform,
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

        ranges::partition_copy_result partitionCopyResult =
                                                   ranges::partition_copy(
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

        ranges::merge_result mergeResult = ranges::merge(vec,
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

        ranges::set_difference_result setDifferenceResult =
                                   ranges::set_difference(vec.begin(),
                                                          vec.end(),
                                                          vec.begin(),
                                                          vec.begin() + 3,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), setDifferenceResult.in));
        ASSERTV(vecToCopyTo[0], 4 == vecToCopyTo[0]);  // 4, 5, 2, 2, 3, 3, ...

        ranges::set_intersection_result setIntersectionResult =
                                 ranges::set_intersection(vec,
                                                          vec,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(), setIntersectionResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 4, 5, 3, ...

        vecToTransform = Vector{4, 5, 6, 7, 8};

        ranges::set_symmetric_difference_result setSymmetricDifferenceResult =
                         ranges::set_symmetric_difference(vec,
                                                          vecToTransform,
                                                          vecToCopyTo.begin());
        ASSERTV(0 == ranges::distance(vec.end(),
                                      setSymmetricDifferenceResult.in1));
        ASSERTV(vecToCopyTo[0], 1 == vecToCopyTo[0]);  // 1, 2, 3, 6, 7, 8, ...

        ranges::set_union_result setUnionResult = ranges::set_union(
                                                          vec,
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

        ranges::minmax_result minMaxResult = ranges::minmax(vec);
        ASSERTV(minMaxResult.min, 1 == minMaxResult.min);

        ranges::minmax_element_result minMaxElementResult =
                                                   ranges::minmax_element(vec);
        ASSERTV(0 == ranges::distance(vec.begin(), minMaxElementResult.min));

        ASSERT(2 == bsl::clamp(1, 2, 3));

        ASSERT(true == ranges::is_permutation(vec, vec));

        vecToTransform = Vector{1, 2, 3, 4, 5};

        ranges::next_permutation_result nextPermutationResult =
                                      ranges::next_permutation(vecToTransform);
        ASSERTV(0 == ranges::distance(vecToTransform.end(),
                                      nextPermutationResult.in));
        ASSERTV(vecToTransform[3], 5 == vecToTransform[3]);  // 1, 2, 3, 5, 4

        ranges::prev_permutation_result prevPermutationResult =
                                      ranges::prev_permutation(vecToTransform);
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

        (void) inFunResult;
        (void) inInResult;
        (void) inOutResult;
        (void) inInOutResult;
        (void) inOutOutResult;
        (void) minMaxResult2;
        (void) inFoundResult;

#endif
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING C++20 <BSL_STOP_TOKEN.H> ADDITIONS
        //
        // Concerns:
        //: 1 The types defined in 'bsl_stop_token.h' exist in the 'bsl'
        //: namespace and, for C++20, are aliases to the standard library type.
        //
        //: 2 The variable 'nostopstate' exists in the 'bsl' namespace, and is
        //: of the type 'bsl::nostopstate_t'.
        //
        // Plan:
        //: 1 Attempt to declare a pointer to each of the types from the
        //:   header file 'bsl_stop_token.h'.
        //
        //: 2 Attempt to form a pointer to 'bsl::nostopstate'.
        //
        // Testing
        //   CONCERN: The type 'bsl::nostopstate_t' is available and usable.
        //   CONCERN: The type 'bsl::stop_token' is available and usable.
        //   CONCERN: The type 'bsl::stop_source' is available and usable.
        //   CONCERN: The type 'bsl::stop_callback' is available and usable.
        //   CONCERN: 'bsl::nostopstate' is available and usable.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C++20 <BSL_STOP_TOKEN.H> ADDITIONS"
                   "\n==========================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        class DummyCallback {
            // This class is used as a mock template parameter for the
            // 'bsl::stop_callback' instance.
        };

        bsl::stop_token                   *s_token    = nullptr;
        bsl::stop_source                  *s_source   = nullptr;
        bsl::stop_callback<DummyCallback> *s_callback = nullptr;
        const bsl::nostopstate_t          *no_stop    = &bsl::nostopstate;

        (void) s_token;
        (void) s_source;
        (void) s_callback;
        (void) no_stop;
#endif

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_source_location.h' HEADER
        //
        // Concerns:
        //: 1 The definitions from '<source_location>' defined by the C++20
        //:   Standard are available in C++20 mode in the 'bsl' namespace to
        //:   users who include 'bsl_source_location.h'.
        //:
        //: 2 The feature test macros defined in '<source_location>' are
        //:   available and have appropriate values.
        //
        // Plan:
        //: 1 Verify that '__cpp_lib_source_location >= 201907L'.
        //:
        //: 2 Verify that 'bsl::source_location::current()' can be successfully
        //:   called and an object of type 'bsl::source_location' is returned.
        //
        // Testing
        //   C++20 'bsl_source_location.h' HEADER
        // --------------------------------------------------------------------
        if (verbose) printf(
                           "\nTESTING C++20 'bsl_source_location.h' HEADER"
                           "\n============================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_source_location >= 201907L);

        const bsl::source_location sl = bsl::source_location::current();
        (void) sl;
#endif

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_bit.h' HEADER
        //
        // Concerns:
        //: 1 The definitions from '<bit>' defined by the C++20 Standard are
        //:   available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bsl_bit.h'.
        //:
        //: 2 The feature test macros defined in '<bit>' are available and have
        //:   appropriate values.
        //
        // Plan:
        //: 1 Verify that
        //:    o '__cpp_lib_endian >= 201907L',
        //:    o '__cpp_lib_bit_cast >= 201806L',
        //:    o '__cpp_lib_bitops >= 201907L',
        //:    o '__cpp_lib_int_pow2 >= 202002L'.
        //:
        //: 2 Form some valid expressions with every name with 'bsl' prefix and
        //:   perform couple of sanity tests.
        //
        // Testing
        //   C++20 'bsl_bit.h' HEADER
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 'bsl_bit.h' HEADER"
                            "\n================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        BSLMF_ASSERT(__cpp_lib_endian >= 201907L);
        ASSERT(bsl::endian::native == bsl::endian::big ||
               bsl::endian::native == bsl::endian::little);

        BSLMF_ASSERT(__cpp_lib_bit_cast >= 201806L);
        ASSERT(bsl::bit_cast<unsigned>(1) == 1U);

        BSLMF_ASSERT(__cpp_lib_bitops >= 201907L);

        ASSERT(bsl::rotl(1U, 1) == 2U);
        ASSERT(bsl::rotl(~(~0U >> 1), 1) == 1U);

        ASSERT(bsl::rotr(1U, 1) == ~(~0U >> 1));
        ASSERT(bsl::rotr(2U, 1) == 1U);

        ASSERT(bsl::countl_zero(0U) == sizeof(unsigned) * CHAR_BIT);
        ASSERT(bsl::countl_zero(1U) == sizeof(unsigned) * CHAR_BIT - 1);
        ASSERT(bsl::countl_zero(2U) == sizeof(unsigned) * CHAR_BIT - 2);
        ASSERT(bsl::countl_zero(~0U) == 0);
        ASSERT(bsl::countl_zero(~(~0U >> 1)) == 0);

        ASSERT(bsl::countl_one(0U) == 0);
        ASSERT(bsl::countl_one(~0U) == sizeof(unsigned) * CHAR_BIT);
        ASSERT(bsl::countl_one(bsl::rotr(0b111U, 3)) == 3);

        ASSERT(bsl::countr_zero(0U) == sizeof(unsigned) * CHAR_BIT);
        ASSERT(bsl::countr_zero(1U) == 0);
        ASSERT(bsl::countr_zero(2U) == 1);

        ASSERT(bsl::countr_one(0U) == 0);
        ASSERT(bsl::countr_one(1U) == 1);
        ASSERT(bsl::countr_one(2U) == 0);
        ASSERT(bsl::countr_one(0b1011U) == 2);

        ASSERT(bsl::popcount(0U) == 0);
        ASSERT(bsl::popcount(1U) == 1);
        ASSERT(bsl::popcount(2U) == 1);
        ASSERT(bsl::popcount(3U) == 2);

        BSLMF_ASSERT(__cpp_lib_int_pow2 >= 202002L);

        ASSERT(!bsl::has_single_bit(0U));
        ASSERT( bsl::has_single_bit(1U));
        ASSERT( bsl::has_single_bit(2U));
        ASSERT(!bsl::has_single_bit(3U));

        ASSERT(bsl::bit_ceil(0U) == 1U);
        ASSERT(bsl::bit_ceil(1U) == 1U);
        ASSERT(bsl::bit_ceil(2U) == 2U);
        ASSERT(bsl::bit_ceil(3U) == 4U);

        ASSERT(bsl::bit_floor(0U) == 0U);
        ASSERT(bsl::bit_floor(1U) == 1U);
        ASSERT(bsl::bit_floor(2U) == 2U);
        ASSERT(bsl::bit_floor(3U) == 2U);

        ASSERT(bsl::bit_width(0U) == 0);
        ASSERT(bsl::bit_width(1U) == 1);
        ASSERT(bsl::bit_width(2U) == 2);
        ASSERT(bsl::bit_width(3U) == 2);
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING C++20 'bsl_numbers.h' HEADER
        //
        // Concerns:
        //: 1 The definitions from '<numbers>' defined by the C++20 Standard
        //:   are available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bsl_numbers.h'.
        //:
        //: 2 The feature test macros defined in '<numbers>' are available and
        //:   have appropriate values.
        //
        // Plan:
        //: 1 Verify that '__cpp_lib_math_constants >= 201907L'.
        //:
        //: 2 Verify that each 'bsl::numbers::foo' value is equal to
        //:   'bsl::numbers::foo_v<double>' value.
        //:
        //: 3 Verify that each 'bsl::numbers::foo' has type 'const double'.
        //
        // Testing
        //   C++20 'bsl_numbers.h' HEADER
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 'bsl_numbers.h' HEADER"
                            "\n====================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
            BSLMF_ASSERT(__cpp_lib_math_constants >= 201907L);

            BSLMF_ASSERT((bsl::numbers::e_v<double> == bsl::numbers::e));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::e),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::log2e_v<double> ==
                                                         bsl::numbers::log2e));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::log2e),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::log10e_v<double> ==
                                                        bsl::numbers::log10e));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::log10e),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::pi_v<double> == bsl::numbers::pi));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::pi),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::inv_pi_v<double> ==
                                                        bsl::numbers::inv_pi));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::inv_pi),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::inv_sqrtpi_v<double> ==
                                                    bsl::numbers::inv_sqrtpi));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::inv_sqrtpi),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::ln2_v<double> == bsl::numbers::ln2));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::ln2),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::ln10_v<double> == bsl::numbers::ln10));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::ln10),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::sqrt2_v<double> ==
                                                         bsl::numbers::sqrt2));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::sqrt2),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::sqrt3_v<double> ==
                                                         bsl::numbers::sqrt3));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::sqrt3),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::inv_sqrt3_v<double> ==
                                                     bsl::numbers::inv_sqrt3));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::inv_sqrt3),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::egamma_v<double> ==
                                                        bsl::numbers::egamma));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::egamma),
                                         const double>));

            BSLMF_ASSERT((bsl::numbers::phi_v<double> == bsl::numbers::phi));
            BSLMF_ASSERT((bsl::is_same_v<decltype(bsl::numbers::phi),
                                         const double>));
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING C++20 THREAD_COORDINATION CLASSES
        //   The following classes have been added 'std::barrier',
        //    'std::latch', 'std::counting_semaphore' and
        //    'std::binary_semaphore'.  BDE aliases them in the following
        //    headers: <bsl_barrier.h>, <std_latch.h> and <bsl_semaphore.h>.
        //
        // Concerns:
        //: 1 The 'latch' type exists in the 'bsl' namespace and is an alias to
        //:   the standard library type.
        //:
        //: 2 The 'barrier' type exists in the 'bsl' namespace and is an alias
        //:   to the standard library type.
        //:
        //: 3 The 'counting_semaphore' type exists in the 'bsl' namespace and
        //:   is an alias to the standard library type.
        //:
        //: 4 The 'binary_semaphore' type exists in the 'bsl' namespace and is
        //:   an alias to the standard library type.
        //
        // Plan:
        //: 1 Attempt to construct an object of each class.
        //:
        //: 2 Verify that corresponding BDE and STD classes have the same type.
        //
        // Testing
        //   CONCERN: 'bsl::barrier' is available and usable.
        //   CONCERN: 'bsl::latch' is available and usable.
        //   CONCERN: 'bsl::counting_semaphore' is available and usable.
        //   CONCERN: 'bsl::binary_semaphore' is available and usable.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C++20 THREAD_COORDINATION CLASSES"
                   "\n=========================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
       bsl::barrier<CompletionFunction> barrier(0);
       ASSERT((bsl::is_same_v<std::barrier<CompletionFunction>,
                              bsl::barrier<CompletionFunction> >));

       bsl::latch latch(0);
       ASSERT((bsl::is_same_v<std::latch, bsl::latch>));

       bsl::counting_semaphore countingSemaphore(0);
       ASSERT((bsl::is_same_v<std::counting_semaphore<1>,
                              bsl::counting_semaphore<1> >));

       bsl::binary_semaphore binarySemaphore(0);
       ASSERT((bsl::is_same_v<std::binary_semaphore, bsl::binary_semaphore>));
#endif

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING C++20 <BSL_VERSION.H> ADDITION
        //   The <version> header, added in C++20, defines numerous
        //   feature-test macros.  Since the same macros are available when
        //   including various standard headers, in order to check the
        //   inclusion of the <version> header exactly, we included it before
        //   other headers and made a check for the availability of
        //   feature-test macros (see the definition of the
        //   'BSLIM_BSLSTANDARDHEADERTEST_VERSION_HEADER_WAS_INCLUDED' macro
        //   above).
        //
        // Concerns:
        //: 1 Standard feature-test macros are available when <bsl_version.h>
        //:   header is included.
        //
        // Plan:
        //: 1 Check if the '__cpp_lib_generic_associative_lookup',
        //:   '__cpp_lib_string_udls' and '__cpp_lib_chrono_udls' macros are
        //:   available after 'bsl_version.h' header inclusion.  (C-1)
        //
        // Testing
        //   CONCERN: feature-test macros are available and usable.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C++20 <BSL_VERSION.H> ADDITION"
                   "\n======================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION

#if !defined(BSLIM_BSLSTANDARDHEADERTEST_VERSION_HEADER_WAS_INCLUDED)
       ASSERTV("<version> is not included", false);
#endif

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING C++20 <BSL_SPAN.H> ADDITION
        //
        // Concerns:
        //: 1 The 'span' type exists in the 'bsl' namespace and, for C++20, is
        //:   an alias to the standard library type.
        //:
        //: 2 'dynamic_extent' is defined in the 'bsl' namespace and, for
        //:   C++20, is an alias to the standard library type.
        //
        // Plan:
        //: 1 Attempt to construct both static extent and dynamic extent 'span'
        //:   types.
        //:
        //: 2 Check the integer value of 'dynamic_extent' is as expected.
        //:
        //: 3 For C++20, check that 'span' and 'dynamic_extent' have the same
        //:   types whether accessed via the 'bsl' or the 'std' namespaces.
        //:
        //: 4 For C++20 check that 'bsl::dynamic_extent' and
        //:   'std::dynamic_extent' have the same value.
        //
        // Testing
        //   CONCERN: 'bsl::span' is available and usable.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C++20 <BSL_SPAN.H> ADDITION"
                   "\n===================================\n");
        int               arr[5] = {0, 1, 2, 3, 4};
        bsl::span<int, 5> ss0(arr);
        bsl::span<int>    sd0(&arr[0], 5);
        ASSERT(5 == ss0.size());
        ASSERT(5 == sd0.size());

        ASSERT(bsl::dynamic_extent == size_t(-1));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        ASSERT((bsl::is_same_v<std::span<int, 99>, bsl::span<int, 99> >));
        ASSERT((bsl::is_same_v<std::span<int>,     bsl::span<int>     >));
        ASSERT((bsl::is_same_v<decltype(bsl::dynamic_extent),
                               decltype(std::dynamic_extent)>));
        ASSERT(bsl::dynamic_extent == std::dynamic_extent);
#endif

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_FUNCTIONAL.H> ADDITIONS
        //
        // Concerns:
        //: 1 'invoke' and 'not_fn' are available in 'bsl' to users who include
        //:   'bsl_functional.h'.
        //
        // Plan:
        //: 1 Create a simple example that uses both classes.  Compilation of
        //:   the example demonstrates that the classes can be found in 'bsl'.
        //
        // Testing
        //   CONCERN: 'bsl::invoke' is usable when available.
        //   CONCERN: 'bsl::not_fn' is usable when available.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_FUNCTIONAL.H> ADDITIONS"
                            "\n==========================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        IsEven even;
        ASSERT(!bsl::invoke(even, 23));
        ASSERT( bsl::invoke(even, 24));

        ASSERT( bsl::not_fn(even)(23));
        ASSERT(!bsl::not_fn(even)(24));
        ASSERT( bsl::invoke(bsl::not_fn(even), 23));
        ASSERT(!bsl::invoke(bsl::not_fn(even), 24));

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_FILESYSTEM.H>
        //
        // Concerns:
        //: 1 The types defined in the filesystem library exist in the 'bsl'
        //:   namespace.
        //:
        //: 2 'bsl::hash' and 'bslh::hashAppend' are both defined for
        //:   'bsl::filesystem::path'.
        //:
        //: 3 'std::filesystem::path' works correctly as the key for a
        //:   'bsl::unordered_map<std::filesystem::path, int>'.
        //
        // Plan:
        //: 1 Attempt to declare a pointer to each of the types from the
        //:   filesystem library.
        //:
        //: 2 Define such an 'unordered_map'.
        //:
        //: 3 Verify that 'insert', 'find', and an update via 'operator[]' work
        //:   correctly.
        //
        // Testing:
        //   C++17 <bsl_filesystem.h>
        //   CONCERN: 'hashAppend' of 'std::filesystem::path' is usable.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_FILESYSTEM.H>"
                            "\n================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
        // Make sure that the types exist (P-1)
        bsl::filesystem::path                         *ppath = nullptr;
        bsl::filesystem::filesystem_error             *pfe = nullptr;
        bsl::filesystem::directory_entry              *pde = nullptr;
        bsl::filesystem::directory_iterator           *pdi = nullptr;
        bsl::filesystem::recursive_directory_iterator *prdi = nullptr;
        bsl::filesystem::file_status                  *pfs = nullptr;
        bsl::filesystem::space_info                   *psi = nullptr;
        bsl::filesystem::file_type                    *pft = nullptr;
        bsl::filesystem::perms                        *pperms = nullptr;
        bsl::filesystem::perm_options                 *ppo = nullptr;
        bsl::filesystem::copy_options                 *pco = nullptr;
        bsl::filesystem::directory_options            *pdo = nullptr;
        bsl::filesystem::file_time_type               *pftt = nullptr;

        (void) ppath;
        (void) pfe;
        (void) pde;
        (void) pdi;
        (void) prdi;
        (void) pfs;
        (void) psi;
        (void) pft;
        (void) pperms;
        (void) ppo;
        (void) pco;
        (void) pdo;
        (void) pftt;

        bsl::hash<bsl::filesystem::path> hasher;
        (void) hasher;

        bslh::DefaultHashAlgorithm hash;
        // Note that the 'hashAppend' overload for 'path' is defined in
        // namespace 'bslh', not namespace 'std', so it will not be found for
        // hash algorithms in other namespaces.
        hashAppend(hash, bsl::filesystem::path());

        typedef std::filesystem::path               TYPE;
        typedef bsl::unordered_map<TYPE, int>       MAP_TYPE;
        typedef bsl::pair<MAP_TYPE::iterator, bool> RESULT_PAIR;
        typedef TYPE::string_type                   STRING_TYPE;
        typedef STRING_TYPE::value_type             CH;

        STRING_TYPE sep = STRING_TYPE(1, TYPE::preferred_separator);

        const TYPE P1(sep + CH('a') + sep + CH('b') + sep + CH('c'));
        const TYPE P1_1(sep + CH('a') + sep + CH('b') + sep + sep + CH('c'));
        const TYPE P2(sep + CH('a') + sep + CH('b') + sep + CH('c') + sep +
                      CH('d'));

        // P-2
        MAP_TYPE testMap;

        // P-3
        RESULT_PAIR result = testMap.insert(std::make_pair(P1, 1));
        ASSERT(result.second == true);
        ASSERT(P1 == result.first->first);
        ASSERT(1 == result.first->second);
        ASSERT(1 == testMap[P1]);

        ASSERT(P1 == P1_1);
        ASSERT(testMap.find(P1)   != testMap.end());
        ASSERT(testMap.find(P1_1) != testMap.end());
        ASSERT(testMap.find(P2)   == testMap.end());

        testMap[P1] = 2;
        ASSERT(2 == testMap[P1]);
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_CSTDDEF/TUPLE/CTIME/CSTDDEF.H> ADDITIONS
        //
        // Concerns:
        //: 1 The type 'bsl::byte' exists.
        //: 2 The calls 'bsl::apply' and 'bls:make_from_tuple' exist and return
        //:   expected values for simple cases.
        //: 3 The calls 'bsl::aligned_alloc' and 'bls::timespec_get' are
        //:   callable if they exist.
        //
        // Plan:
        //: 1 Verify that 'bsl::byte' exists.
        //: 2 Call each of 'bsl::apply' and 'bsl::make_from_tuple' with simple
        //:   inputs and verify that the results are correct.
        //: 3 If they exist, call each of 'bsl::aligned_alloc' and
        //:   'bsl::timespec_get' with sample inputs.
        //
        // Testing:
        //   bsl::byte;
        //   bsl::apply();
        //   bsl::make_from_tuple();
        //   bsl::aligned_alloc();
        //   bsl::timespec_get();
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING C++17 <BSL_CSTDDEF/TUPLE/CTIME/CSTDDEF.H> ADDITIONS"
                "\n==========================================================="
                "\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        bsl::byte *bp = nullptr;
        ASSERT(nullptr == bp);

        bsl::tuple<double, int> t(3, 2);
        TwoArgumentCtor         ta = bsl::make_from_tuple<TwoArgumentCtor>(t);
        ASSERT(5.0 == ta.d_val);

        ASSERT(5.0 == bsl::apply(TwoArgumentFunction, t));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
        {
            int* p = static_cast<int*>(bsl::aligned_alloc(1024, 1024));
            bsl::free(p);
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
        {
            bsl::timespec ts;
            bsl::timespec_get(&ts, TIME_UTC);
        }
#endif

#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_CMATH.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++17 special math functions all exist in namespace 'bsl'.
        //
        // Plan:
        //: 1 Create simple examples that use these algorithms.  Compilation of
        //:   the example demonstrates that the calls can be found in 'bsl'.
        //:
        //: 2 Compare the results with the results of calling the native
        //:   standard library.
        //
        // Testing:
        //   bsl::assoc_laguerre();
        //   bsl::assoc_laguerref();
        //   bsl::assoc_laguerrel();
        //   bsl::assoc_legendre();
        //   bsl::assoc_legendref();
        //   bsl::assoc_legendrel();
        //   bsl::beta();
        //   bsl::betaf();
        //   bsl::betal();
        //   bsl::comp_ellint_1();
        //   bsl::comp_ellint_1f();
        //   bsl::comp_ellint_1l();
        //   bsl::comp_ellint_2();
        //   bsl::comp_ellint_2f();
        //   bsl::comp_ellint_2l();
        //   bsl::comp_ellint_3();
        //   bsl::comp_ellint_3f();
        //   bsl::comp_ellint_3l();
        //   bsl::cyl_bessel_i();
        //   bsl::cyl_bessel_if();
        //   bsl::cyl_bessel_il();
        //   bsl::cyl_bessel_j();
        //   bsl::cyl_bessel_jf();
        //   bsl::cyl_bessel_jl();
        //   bsl::cyl_bessel_k();
        //   bsl::cyl_bessel_kf();
        //   bsl::cyl_bessel_kl();
        //   bsl::cyl_neumann();
        //   bsl::cyl_neumannf();
        //   bsl::cyl_neumannl();
        //   bsl::ellint_1();
        //   bsl::ellint_1f();
        //   bsl::ellint_1l();
        //   bsl::ellint_2();
        //   bsl::ellint_2f();
        //   bsl::ellint_2l();
        //   bsl::ellint_3();
        //   bsl::ellint_3f();
        //   bsl::ellint_3l();
        //   bsl::expint();
        //   bsl::expintf();
        //   bsl::expintl();
        //   bsl::hermite();
        //   bsl::hermitef();
        //   bsl::hermitel();
        //   bsl::legendre();
        //   bsl::legendref();
        //   bsl::legendrel();
        //   bsl::laguerre();
        //   bsl::laguerref();
        //   bsl::laguerrel();
        //   bsl::riemann_zeta();
        //   bsl::riemann_zetaf();
        //   bsl::riemann_zetal();
        //   bsl::sph_bessel();
        //   bsl::sph_besself();
        //   bsl::sph_bessell();
        //   bsl::sph_legendre();
        //   bsl::sph_legendref();
        //   bsl::sph_legendrel();
        //   bsl::sph_neumann();
        //   bsl::sph_neumannf();
        //   bsl::sph_neumannl();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_CMATH.H> ADDITIONS"
                            "\n=====================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS

        ASSERT(std::sph_neumannl(3U, 10.0L) ==
               bsl::sph_neumannl(3U, 10.0L));
        ASSERT(std::assoc_laguerre (1U, 10U, 0.5F) ==
               bsl::assoc_laguerre (1U, 10U, 0.5F));
        ASSERT(std::assoc_laguerre (1U, 10U, 0.5) ==
               bsl::assoc_laguerre (1U, 10U, 0.5));
        ASSERT(std::assoc_laguerre (1U, 10U, 0.5L) ==
               bsl::assoc_laguerre (1U, 10U, 0.5L));
        ASSERT(std::assoc_laguerref(1U, 10U, 0.5F) ==
               bsl::assoc_laguerref(1U, 10U, 0.5F));
        ASSERT(std::assoc_laguerrel(1U, 10U, 0.5L) ==
               bsl::assoc_laguerrel(1U, 10U, 0.5L));

        ASSERT(std::beta (4,    6)    == bsl::beta (4,    6));
        ASSERT(std::beta (4.0,  6.0)  == bsl::beta (4.0,  6.0));
        ASSERT(std::betaf(4.0F, 6.0F) == bsl::betaf(4.0F, 6.0F));
        ASSERT(std::betal(4.0L, 6.0L) == bsl::betal(4.0L, 6.0L));

        ASSERT(std::comp_ellint_1 (0)    == bsl::comp_ellint_1 (0));
        ASSERT(std::comp_ellint_1 (0.5F) == bsl::comp_ellint_1 (0.5F));
        ASSERT(std::comp_ellint_1 (0.5)  == bsl::comp_ellint_1 (0.5));
        ASSERT(std::comp_ellint_1 (0.5L) == bsl::comp_ellint_1 (0.5L));
        ASSERT(std::comp_ellint_1f(0.5F) == bsl::comp_ellint_1f(0.5F));
        ASSERT(std::comp_ellint_1l(0.5L) == bsl::comp_ellint_1l(0.5L));

        ASSERT(std::comp_ellint_2 (1)    == bsl::comp_ellint_2 (1));
        ASSERT(std::comp_ellint_2 (0.5F) == bsl::comp_ellint_2 (0.5F));
        ASSERT(std::comp_ellint_2 (0.5)  == bsl::comp_ellint_2 (0.5));
        ASSERT(std::comp_ellint_2 (0.5L) == bsl::comp_ellint_2 (0.5L));
        ASSERT(std::comp_ellint_2f(0.5F) == bsl::comp_ellint_2f(0.5F));
        ASSERT(std::comp_ellint_2l(0.5L) == bsl::comp_ellint_2l(0.5L));

        ASSERT(std::comp_ellint_3 (0,    0) ==
               bsl::comp_ellint_3 (0,    0));
        ASSERT(std::comp_ellint_3 (0.5F, 0.0F) ==
               bsl::comp_ellint_3 (0.5F, 0.0F));
        ASSERT(std::comp_ellint_3 (0.5,  0.0) ==
               bsl::comp_ellint_3 (0.5,  0.0));
        ASSERT(std::comp_ellint_3 (0.5L, 0.0F) ==
               bsl::comp_ellint_3 (0.5L, 0.0F));
        ASSERT(std::comp_ellint_3f(0.5F, 0.0F) ==
               bsl::comp_ellint_3f(0.5F, 0.0F));
        ASSERT(std::comp_ellint_3l(0.5L, 0.0L) ==
               bsl::comp_ellint_3l(0.5L, 0.0L));

        ASSERT(std::cyl_bessel_i (0,    1) ==
               bsl::cyl_bessel_i (0,    1));
        ASSERT(std::cyl_bessel_i (0.0,  1.0) ==
               bsl::cyl_bessel_i (0.0,  1.0));
        ASSERT(std::cyl_bessel_if(0.0F, 1.0F) ==
               bsl::cyl_bessel_if(0.0F, 1.0F));
        ASSERT(std::cyl_bessel_il(0.0L, 1.0L) ==
               bsl::cyl_bessel_il(0.0L, 1.0L));

        ASSERT(std::cyl_bessel_j (0,    1) ==
               bsl::cyl_bessel_j (0,    1));
        ASSERT(std::cyl_bessel_j (0.0,  1.0) ==
               bsl::cyl_bessel_j (0.0,  1.0));
        ASSERT(std::cyl_bessel_jf(0.0F, 1.0F) ==
               bsl::cyl_bessel_jf(0.0F, 1.0F));
        ASSERT(std::cyl_bessel_jl(0.0L, 1.0L) ==
               bsl::cyl_bessel_jl(0.0L, 1.0L));

        ASSERT(std::cyl_bessel_k (0,    1) ==
               bsl::cyl_bessel_k (0,    1));
        ASSERT(std::cyl_bessel_k (0.0 , 1.0) ==
               bsl::cyl_bessel_k (0.0 , 1.0));
        ASSERT(std::cyl_bessel_kf(0.0F, 1.0F) ==
               bsl::cyl_bessel_kf(0.0F, 1.0F));
        ASSERT(std::cyl_bessel_kl(0.0L, 1.0L) ==
               bsl::cyl_bessel_kl(0.0L, 1.0L));

        ASSERT(std::cyl_neumann (0,    1) ==
               bsl::cyl_neumann (0,    1));
        ASSERT(std::cyl_neumann (0.0 , 1.0) ==
               bsl::cyl_neumann (0.0 , 1.0));
        ASSERT(std::cyl_neumannf(0.0F, 1.0F) ==
               bsl::cyl_neumannf(0.0F, 1.0F));
        ASSERT(std::cyl_neumannl(0.0L, 1.0L) ==
               bsl::cyl_neumannl(0.0L, 1.0L));

        ASSERT(std::ellint_1 (0,    1) ==
               bsl::ellint_1 (0,    1));
        ASSERT(std::ellint_1 (0.0 , 1.0) ==
               bsl::ellint_1 (0.0 , 1.0));
        ASSERT(std::ellint_1f(0.0F, 1.0F) ==
               bsl::ellint_1f(0.0F, 1.0F));
        ASSERT(std::ellint_1l(0.0L, 1.0L) ==
               bsl::ellint_1l(0.0L, 1.0L));

        ASSERT(std::ellint_2 (0,    1) ==
               bsl::ellint_2 (0,    1));
        ASSERT(std::ellint_2 (0.0 , 1.0) ==
               bsl::ellint_2 (0.0 , 1.0));
        ASSERT(std::ellint_2f(0.0F, 1.0F) ==
               bsl::ellint_2f(0.0F, 1.0F));
        ASSERT(std::ellint_2l(0.0L, 1.0L) ==
               bsl::ellint_2l(0.0L, 1.0L));

        ASSERT(std::ellint_3 (0,    1,    1) ==
               bsl::ellint_3 (0,    1,    1));
        ASSERT(std::ellint_3 (0.0 , 1.0,  1.0) ==
               bsl::ellint_3 (0.0 , 1.0,  1.0));
        ASSERT(std::ellint_3f(0.0F, 1.0F, 1.0F) ==
               bsl::ellint_3f(0.0F, 1.0F, 1.0F));
        ASSERT(std::ellint_3l(0.0L, 1.0L, 1.0L) ==
               bsl::ellint_3l(0.0L, 1.0L, 1.0L));

        ASSERT(std::expint (1)    == bsl::expint (1));
        ASSERT(std::expint (1.0)  == bsl::expint (1.0));
        ASSERT(std::expintf(1.0F) == bsl::expintf(1.0F));
        ASSERT(std::expintl(1.0L) == bsl::expintl(1.0L));

        ASSERT(std::hermite (3U, 10)    == bsl::hermite (3U, 10));
        ASSERT(std::hermite (3U, 10.0F) == bsl::hermite (3U, 10.0F));
        ASSERT(std::hermite (3U, 10.0)  == bsl::hermite (3U, 10.0));
        ASSERT(std::hermite (3U, 10.0L) == bsl::hermite (3U, 10.0L));
        ASSERT(std::hermitef(3U, 10.0F) == bsl::hermitef(3U, 10.0F));
        ASSERT(std::hermitel(3U, 10.0L) == bsl::hermitel(3U, 10.0L));

        ASSERT(std::legendre (3U, 1)    == bsl::legendre (3U, 1));
        ASSERT(std::legendre (3U, 1.0F) == bsl::legendre (3U, 1.0F));
        ASSERT(std::legendre (3U, 1.0)  == bsl::legendre (3U, 1.0));
        ASSERT(std::legendre (3U, 1.0L) == bsl::legendre (3U, 1.0L));
        ASSERT(std::legendref(3U, 1.0F) == bsl::legendref(3U, 1.0F));
        ASSERT(std::legendrel(3U, 1.0L) == bsl::legendrel(3U, 1.0L));

        ASSERT(std::laguerre (3U, 10)    == bsl::laguerre (3U, 10));
        ASSERT(std::laguerre (3U, 10.0F) == bsl::laguerre (3U, 10.0F));
        ASSERT(std::laguerre (3U, 10.0)  == bsl::laguerre (3U, 10.0));
        ASSERT(std::laguerre (3U, 10.0L) == bsl::laguerre (3U, 10.0L));
        ASSERT(std::laguerref(3U, 10.0F) == bsl::laguerref(3U, 10.0F));
        ASSERT(std::laguerrel(3U, 10.0L) == bsl::laguerrel(3U, 10.0L));

        ASSERT(std::riemann_zeta (0)    == bsl::riemann_zeta (0));
        ASSERT(std::riemann_zeta (0.0F) == bsl::riemann_zeta (0.0F));
        ASSERT(std::riemann_zeta (0.0)  == bsl::riemann_zeta (0.0));
        ASSERT(std::riemann_zeta (0.0L) == bsl::riemann_zeta (0.0L));
        ASSERT(std::riemann_zetaf(0.0F) == bsl::riemann_zetaf(0.0F));
        ASSERT(std::riemann_zetal(0.0L) == bsl::riemann_zetal(0.0L));

        ASSERT(std::sph_bessel (3U, 10) ==
               bsl::sph_bessel (3U, 10));
        ASSERT(std::sph_bessel (3U, 10.0F) ==
               bsl::sph_bessel (3U, 10.0F));
        ASSERT(std::sph_bessel (3U, 10.0) ==
               bsl::sph_bessel (3U, 10.0));
        ASSERT(std::sph_bessel (3U, 10.0L) ==
               bsl::sph_bessel (3U, 10.0L));
        ASSERT(std::sph_besself(3U, 10.0F) ==
               bsl::sph_besself(3U, 10.0F));
        ASSERT(std::sph_bessell(3U, 10.0L) ==
               bsl::sph_bessell(3U, 10.0L));

        ASSERT(std::sph_legendre (3U, 1U, 10) ==
               bsl::sph_legendre (3U, 1U, 10));
        ASSERT(std::sph_legendre (3U, 1U, 10.0F) ==
               bsl::sph_legendre (3U, 1U, 10.0F));
        ASSERT(std::sph_legendre (3U, 1U, 10.0) ==
               bsl::sph_legendre (3U, 1U, 10.0));
        ASSERT(std::sph_legendre (3U, 1U, 10.0L) ==
               bsl::sph_legendre (3U, 1U, 10.0L));
        ASSERT(std::sph_legendref(3U, 1U, 10.0F) ==
               bsl::sph_legendref(3U, 1U, 10.0F));
        ASSERT(std::sph_legendrel(3U, 1U, 10.0L) ==
               bsl::sph_legendrel(3U, 1U, 10.0L));

        ASSERT(std::sph_neumann (3U, 10) ==
               bsl::sph_neumann (3U, 10));
        ASSERT(std::sph_neumann (3U, 10.0F) ==
               bsl::sph_neumann (3U, 10.0F));
        ASSERT(std::sph_neumann (3U, 10.0) ==
               bsl::sph_neumann (3U, 10.0));
        ASSERT(std::sph_neumann (3U, 10.0L) ==
               bsl::sph_neumann (3U, 10.0L));
        ASSERT(std::sph_neumannf(3U, 10.0F) ==
               bsl::sph_neumannf(3U, 10.0F));
        ASSERT(std::sph_neumannl(3U, 10.0L) ==
               bsl::sph_neumannl(3U, 10.0L));

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_MEMORY.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++17 memory algorithms are available in 'bsl' to users who
        //:   include 'bsl_memory.h'.
        //
        // Plan:
        //: 1 Create a simple example that uses these algorithms.  Compilation
        //:   of the example demonstrates that the calls can be found in 'bsl'.
        //
        // Testing:
        //   bsl::destroy()
        //   bsl::destroy_at()
        //   bsl::destroy_n()
        //   bsl::uninitialized_default_construct()
        //   bsl::uninitialized_default_construct_n()
        //   bsl::uninitialized_move()
        //   bsl::uninitialized_move_n()
        //   bsl::uninitialized_value_construct()
        //   bsl::uninitialized_value_construct_n()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_MEMORY.H> ADDITIONS"
                            "\n======================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        int vals[5];
        bsl::destroy   (vals, vals+5);
        bsl::destroy_n (vals,      5);
        bsl::destroy_at(vals);

        bsl::uninitialized_default_construct  (vals, vals+5);
        bsl::uninitialized_default_construct_n(vals,      5);

        bsl::uninitialized_move  (vals, vals+2, vals+3);
        bsl::uninitialized_move_n(vals,      2, vals+3);

        bsl::uninitialized_value_construct  (vals, vals+5);
        bsl::uninitialized_value_construct_n(vals,      5);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_MUTEX.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++17 mutex primitives are available in 'bsl' to users who
        //:   include 'bsl_mutex.h'.
        //
        // Plan:
        //: 1 Attempt to use these routines in a simple example.  Compilation
        //:   of the example demonstrates that the types can be found in 'bsl'.
        //
        // Testing:
        //   bsl::scoped_lock;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_MUTEX.H> ADDITIONS"
                            "\n=====================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

        bsl::mutex mtx;
        {
            bsl::scoped_lock sl(mtx);
            if (veryVerbose)
            {
                puts("scoped_lock acquired lock");
            }
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_NEW.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++17 memory algorithms are available in 'bsl' to users who
        //:   include 'bsl_new.h'.
        //
        // Plan:
        //: 1 Create a simple example that uses these algorithms.  Compilation
        //:   of the example demonstrates that the classes can be found in
        //:   'bsl'.
        //
        // Testing:
        //   bsl::align_val_t
        //   bsl::launder()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_NEW.H> ADDITIONS"
                            "\n===================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        typedef bsl::align_val_t MyAlign;
        MyAlign ma;
        (void) bsl::launder(&ma);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_ALGORITHM.H> ADDITIONS
        //
        // Concerns:
        //: 1 The calls 'bsl::clamp', 'bls:for_each_n', and 'bsl::sample' all
        //    exist and return expected values for simple cases.
        //
        // Plan:
        //: 1 Call each of the three algorithms with simple inputs and verify
        //:   that the result is correct.
        //
        // Testing:
        //   bsl::clamp();
        //   bsl::for_each_n();
        //   bsl::sample();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_ALGORITHM.H> ADDITIONS"
                            "\n=========================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        ASSERT(3 == bsl::clamp(3, 1, 10));

        const bsl::string source = "abcdef";
        bsl::string       dest;

        CountingFunctor::s_count = 0;
        bsl::for_each_n(source.begin(), 3, CountingFunctor{});
        ASSERT(3 == CountingFunctor::s_count);

        dest.clear();
        bsl::sample(source.begin(),
                    source.end(),
                    bsl::back_inserter(dest),
                    5,
                    bsl::mt19937{bsl::random_device{}()});
        ASSERT(5 == dest.size());
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_NUMERIC.H> ADDITIONS
        //
        // Concerns:
        //: 1 The C++17 numeric algorithms are available in 'bsl' to users who
        //:   include 'bsl_numeric.h'.
        //
        // Plan:
        //: 1 Create a simple example that uses these algorithms.  Compilation
        //:   of the example demonstrates that the classes can be found in
        //:   'bsl'.
        //
        // Testing:
        //   bsl::exclusive_scan()
        //   bsl::inclusive_scan()
        //   bsl::gcd()
        //   bsl::lcm()
        //   bsl::transform_exclusive_scan()
        //   bsl::transform_inclusive_scan()
        //   bsl::reduce()
        //   bsl::transform_reduce()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_NUMERIC.H> ADDITIONS"
                            "\n=======================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            bsl::array<size_t, 10> v;
            bsl::fill(v.begin(), v.end(), 3);
            bsl::exclusive_scan(v.begin(), v.end(), v.begin(), size_t{50});
            for (size_t i = 0; i < v.size(); ++i)
            {
                ASSERT(v[i] == 50 + i * 3);
            }
        }

        {
            bsl::array<size_t, 10> v;
            bsl::fill(v.begin(), v.end(), 3);
            bsl::inclusive_scan(v.begin(), v.end(), v.begin());
            for (size_t i = 0; i < v.size(); ++i)
            {
                ASSERT(v[i] == (i+1) * 3);
            }
        }

        ASSERT(2  == bsl::gcd(6, 8));
        ASSERT(24 == bsl::lcm(6, 8));

        {
            bsl::array<size_t, 10> v;
            bsl::fill(v.begin(), v.end(), 3);
            bsl::transform_exclusive_scan(v.begin(),
                                          v.end(),
                                          v.begin(),
                                          size_t{50},
                                          bsl::plus<>(),
                                          addOne{});
            for (size_t i = 0; i < v.size(); ++i)
            {
                ASSERT(v[i] == 50 + i * 4);
            }
        }

        {
            bsl::array<size_t, 10> v;
            bsl::fill(v.begin(), v.end(), 3);
            bsl::transform_inclusive_scan(v.begin(),
                                          v.end(),
                                          v.begin(),
                                          bsl::plus<>(),
                                          addOne{},
                                          size_t{50});
            for (size_t i = 0; i < v.size(); ++i)
            {
                ASSERT(v[i] == 50 + (i + 1) * 4);
            }
        }

        {
            bsl::array<size_t, 10> v;
            bsl::fill(v.begin(), v.end(), 3);
            ASSERT(30 == bsl::reduce(v.begin(), v.end()));
        }

        {
            bsl::array<size_t, 10> v1;
            bsl::array<size_t, 10> v2;
            bsl::fill(v1.begin(), v1.end(), 3);
            bsl::fill(v2.begin(), v2.end(), 5);
            ASSERT (200 == bsl::transform_reduce(v1.begin(),
                                                 v1.end(),
                                                 v2.begin(),
                                                 size_t{50}));
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
        // Check that the types exist
        ASSERT(
             bsl::is_execution_policy<bsl::execution::parallel_policy>::value);
        ASSERT(bsl::is_execution_policy_v<bsl::execution::sequenced_policy>);

        ASSERT((!bsl::is_same<bsl::execution::sequenced_policy,
                              bsl::execution::parallel_policy>::value));
        ASSERT((!bsl::is_same<bsl::execution::parallel_unsequenced_policy,
                              bsl::execution::sequenced_policy>::value));
#endif

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_UTILITY.H> ADDITIONS
        //
        // Concerns:
        //: 1 The call 'bsl::as_const' returns it's parameter as a const &.
        //
        // Plan:
        //: 1 Statically check that the return type of 'bsl::as_const' is
        //:   correct.
        //: 2 Dynamically check that the return value of 'bsl::as_const' is
        //:   correct.
        //: 3 Check that 'bsl::in_place' and 'in_place_t' are accessible here.
        //
        //
        // Testing:
        //   bsl::add_const<TYPE>::type& as_const(TYPE& value);
        //   bsl::in_place_t
        //   bsl::in_place
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_UTILITY.H> ADDITIONS"
                            "\n=======================================\n");
        typedef bsl::pair<int, double> Obj;

        Obj X;
        X.first = 23;
        X.second = 42.0;
        const Obj &cX = bsl::as_const(X);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        ASSERT((bsl::is_same<decltype(bsl::as_const(X)), const Obj &>::value));
#endif
        ASSERT(&cX.first == &X.first);  // same object

        // Just check that the identifiers are accessible
        bsl::in_place_t my_inplace = bsl::in_place;
        (void) my_inplace;

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING C++17 TYPE ALIASES
        //
        // Concerns:
        //: 1 The *_v inline variable definitions added for C++17 type alias
        //:   features are available and return the correct answers.
        //
        // Plan:
        //: 1 Ensure that 'bsl::xxx_v<values>' (which we implement) is equal to
        //:   'bsl::xxx<values>::value' (which we import).
        //
        // Testing:
        //   C++17 TYPE ALIASES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 TYPE ALIASES"
                            "\n==========================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // We don't really need to call these routines, because all they do is
        // 'static_assert', but this keeps the compiler from warning about
        // "unused functions".
        TestCxx17TypeAliases::Conjunction();
        TestCxx17TypeAliases::Disjunction();
        TestCxx17TypeAliases::HasUniqueObjectReps();
        TestCxx17TypeAliases::IsAggregate();
        TestCxx17TypeAliases::IsInvocable();
        TestCxx17TypeAliases::IsInvocableR();
        TestCxx17TypeAliases::IsSwappable();
        TestCxx17TypeAliases::IsSwappableWith();
        TestCxx17TypeAliases::Negation();
#endif

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'bslh::hashAppend' OF 'std::pair' AND 'std::tuple'
        //
        // Concerns:
        //: 1 The 'bslh::hashAppend' functions defined for 'std::pair' and
        //:   'std::tuple' are found and usable by 'bsl' container classes that
        //:   have a 'HASH' template parameter.
        //
        // Plan:
        //: 1 Create an 'bsl::unordered_set' objects keyed on instances of
        //:   'std::pair' and 'std::tuple'.  Confirm that keys of the
        //:   appropriate type can be inserted and later found.
        //
        // Testing:
        //   CONCERN: 'bslh::hashAppend' of 'std::pair'  is usable from 'bsl'.
        //   CONCERN: 'bslh::hashAppend' of 'std::tuple' is usable from 'bsl'.
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                 "\nTESTING 'bslh::hashAppend' OF 'std::pair' AND 'std::tuple'"
                 "\n=========================================================="
                 "\n");

        if (verbose) printf("Testing 'hashAppend' of 'std::pair'\n");

        typedef std::pair<int, int> StdKeyPair;
        typedef bsl::pair<int, int> BslKeyPair;

        ASSERT((!bsl::is_same<StdKeyPair, BslKeyPair>::value));

        typedef bsl::unordered_set<StdKeyPair> SetOfStdKeyPairs;

        const bsls::NameOf<SetOfStdKeyPairs::hasher> keyNamePairs;

        const char * expPair0 = "bsl::hash<std::pair<int, int>>";
        const char * expPair1 = "bsl::hash<std::pair<int,int>>";
        const char * expPair2 = "bsl::hash<std::__1::pair<int, int>>";

        ASSERT(0 == bsl::strcmp(expPair0, keyNamePairs)
            || 0 == bsl::strcmp(expPair1, keyNamePairs)
            || 0 == bsl::strcmp(expPair2, keyNamePairs));

        SetOfStdKeyPairs setOfStdKeyPairs;

        StdKeyPair itemPair0 = std::make_pair(0, 0);
        StdKeyPair itemPair1 = std::make_pair(1, 1);
        StdKeyPair itemPair2 = std::make_pair(2, 2);

        setOfStdKeyPairs.insert(itemPair0);
        setOfStdKeyPairs.insert(itemPair1);
        setOfStdKeyPairs.insert(itemPair2);

        ASSERT(3 == setOfStdKeyPairs.size());

        SetOfStdKeyPairs::const_iterator itrPair0 = setOfStdKeyPairs.find(
                                                                    itemPair0);
        SetOfStdKeyPairs::const_iterator itrPair1 = setOfStdKeyPairs.find(
                                                                    itemPair1);
        SetOfStdKeyPairs::const_iterator itrPair2 = setOfStdKeyPairs.find(
                                                                    itemPair2);

        ASSERT(setOfStdKeyPairs.end() != itrPair0);
        ASSERT(setOfStdKeyPairs.end() != itrPair1);
        ASSERT(setOfStdKeyPairs.end() != itrPair2);

        ASSERT(itemPair0 == *itrPair0);
        ASSERT(itemPair1 == *itrPair1);
        ASSERT(itemPair2 == *itrPair2);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

        if (verbose) printf("Testing 'hashAppend' of 'std::tuple'\n");

        using StdKeyTuple = std::tuple<int, int, int>;
        using BslKeyTuple = bsl::tuple<int, int, int>;

        ASSERT(( bsl::is_same<StdKeyTuple, BslKeyTuple>::value));

        typedef bsl::unordered_set<StdKeyTuple> SetOfStdKeyTuples;

        const bsls::NameOf<SetOfStdKeyTuples::hasher> keyNameTuples;

        const char * expTuple0 = "bsl::hash<std::tuple<int, int, int>>";
        const char * expTuple1 = "bsl::hash<std::tuple<int,int,int>>";
        const char * expTuple2 = "bsl::hash<std::__1::tuple<int, int, int>>";

        ASSERT(0 == bsl::strcmp(expTuple0, keyNameTuples)
            || 0 == bsl::strcmp(expTuple1, keyNameTuples)
            || 0 == bsl::strcmp(expTuple2, keyNameTuples));

        SetOfStdKeyTuples setOfStdKeyTuples;

        StdKeyTuple itemTuple0 = std::make_tuple(0, 0, 0);
        StdKeyTuple itemTuple1 = std::make_tuple(1, 1, 1);
        StdKeyTuple itemTuple2 = std::make_tuple(2, 2, 2);

        setOfStdKeyTuples.insert(itemTuple0);
        setOfStdKeyTuples.insert(itemTuple1);
        setOfStdKeyTuples.insert(itemTuple2);

        ASSERT(3 == setOfStdKeyTuples.size());

        SetOfStdKeyTuples::const_iterator itrTuple0 = setOfStdKeyTuples.find(
                                                                   itemTuple0);
        SetOfStdKeyTuples::const_iterator itrTuple1 = setOfStdKeyTuples.find(
                                                                   itemTuple1);
        SetOfStdKeyTuples::const_iterator itrTuple2 = setOfStdKeyTuples.find(
                                                                   itemTuple2);

        ASSERT(setOfStdKeyTuples.end() != itrTuple0);
        ASSERT(setOfStdKeyTuples.end() != itrTuple1);
        ASSERT(setOfStdKeyTuples.end() != itrTuple2);

        ASSERT(itemTuple0 == *itrTuple0);
        ASSERT(itemTuple1 == *itrTuple1);
        ASSERT(itemTuple2 == *itrTuple2);
#else
        if (verbose) cout << "SKIP: 'std::tuple' not available" << endl;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SEARCHERS
        //
        // Concerns:
        //: 1 'default_searcher' and 'boyer_moore_horspool_seacher' are
        //:   available in 'bsl' to users who include 'bsl_functional.h'.
        //
        // Plan:
        //: 1 Create a simple example that uses both classes.  Compilation of
        //:   the example demonstrates that the classes can be found in 'bsl'.
        //
        // Testing
        //   CONCERN: 'default_searcher'/'boyer_moore_horspool_searcher usable.
        //   CONCERN: 'boyer_moore_searcher' usable when available.
        //   CONCERN: 'bsl::search' function is usable.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SEARCHERS"
                            "\n=================\n");

        typedef const char *                                 ConstItr;
        typedef bsl::default_searcher             <ConstItr> DftSearcher;
        typedef bsl::boyer_moore_horspool_searcher<ConstItr> BmhSearcher;

        const char     needle[]    = "world";
        const ConstItr needleFirst = needle;
        const ConstItr needleLast  = needleFirst + sizeof needle - 1;

        DftSearcher dftSearcher(needleFirst, needleLast);
        BmhSearcher bmhSearcher(needleFirst, needleLast);

        const char     haystack[]    = "Hello, world.";
        const ConstItr haystackFirst = haystack;
        const ConstItr haystackLast  = haystackFirst + sizeof haystack - 1;

        typedef bsl::pair<ConstItr, ConstItr> Result;

        const Result dftResult = dftSearcher(haystackFirst, haystackLast);
        const Result bmhResult = bmhSearcher(haystackFirst, haystackLast);

        ASSERT(dftResult == bmhResult);
        ASSERT(        7 == bmhResult.first  - haystackFirst);
        ASSERT(        5 == bmhResult.second - bmhResult.first);

        ASSERT(7 == bsl::search(haystackFirst, haystackLast, dftSearcher)
                                                              - haystackFirst);
        ASSERT(7 == bsl::search(haystackFirst, haystackLast, bmhSearcher)
                                                              - haystackFirst);

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS)
        typedef bsl::boyer_moore_searcher<ConstItr> BmSearcher;

        BmSearcher bmSearcher(needleFirst, needleLast);

        const Result bmResult = bmSearcher(haystackFirst, haystackLast);

        ASSERT(bmResult == bmhResult);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'forward_list'
        //
        // Concerns:
        //: 1 'bsl::forward_list' is available in baseline C++11 builds.
        //: 2 'bsl::forward_list' correctly uses 'bslma' allocators
        //: 3 Including 'bsl_forward_list.h' gives access to 'bsl::begin'
        //
        // Plan:
        //: 1 Create a 'bsl::forward_list<bsl::string>' using a test allocator,
        //:   emplace a string into the container, and verify that the emplaced
        //:   string is using the correct allocator.
        //: 2 Call 'bsl::begin' on both const and non-const versions of the
        //:   container, and ensure that the calls return the first element
        //:   in the container.
        //
        // Testing
        //   CONCERN: 'forward_list' is available in C++11 builds
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'forward_lst'"
                            "\n=====================\n");
#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)

        if (verbose) printf("\n'bsl::forward_list' is not supported"
                            "\n====================================\n");
#else
        using Obj = bsl::forward_list<bsl::string>;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator ta("forward", veryVeryVeryVerbose);

        if (verbose) printf("Testing allocators are hooked up correctly.\n");
        {
            Obj mX(&ta);    const Obj& X = mX;

            mX.emplace_front("This string must not be a short string.");
            mX.emplace_front("This string cannot be a short string either!");
            mX.emplace_front("One final test string that must not be short.");

#if !defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC) || _HAS_ITERATOR_DEBUGGING == 0
            const long long EXPECTED_ALLOCATIONS = 6;
#else
            // Microsoft allocate an extra sentinel node when iterator
            // debugging is enabled.
            const long long EXPECTED_ALLOCATIONS = 7;
#endif

            ASSERT(
                 X.front() == "One final test string that must not be short.");

            Obj::iterator       itM = bsl::begin(mX);
            Obj::const_iterator itC = bsl::begin(X);
            ASSERT(*itM == X.front());
            ASSERT(*itC == X.front());

            ASSERTV(da.numBytesInUse(), 0 == da.numBytesInUse());
            ASSERTV(ta.numBlocksInUse(),
                    ta.numBlocksInUse() == EXPECTED_ALLOCATIONS);
        }
        ASSERTV(da.numBytesInUse(),  0 == da.numBytesInUse());
        ASSERTV(ta.numBlocksInUse(), 0 == ta.numBlocksInUse());
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING SUPPORT FOR MOVE-ONLY TYPES
        //   This tests a general concern for C++11 compilers that the 'bsl'
        //   implementation of standard components support move-only types.
        //   Motivated by DRQS 126478885.
        //
        // Concerns:
        //: 1 Containers like 'vector', and 'bsl::pair', support move-only
        //:   types like 'unique_ptr'..
        //
        // Plan:
        //: 1 Call all 8 range functions (unqualified) for a 'bsl::set<int>',
        //:   as this will be associated with both namespace 'bsl' and native
        //:   'std' (for 'std::less' as a template parameter).
        //
        // Testing
        //   CONCERN: Range functions are not ambiguous with 'std' under ADL
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SUPPORT FOR MOVE-ONLY TYPES"
                            "\n===================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        typedef bsl::unique_ptr<int> Id;
        typedef bsl::pair<int, Id>   Item;

# if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1800
        bsl::vector<Item> items;

        Item item(7, Id(new int(14)));

        items.emplace_back(bsl::move(item));

        bsl::map<int, Id> index;
        index.emplace(13, Id(new int(42)));
# else
        static_assert(std::is_copy_constructible<Id>::value,
            "Failed to detect the no-deleted-ctor bug, reconsider workaround");
        static_assert(std::is_copy_constructible<Item>::value,
            "Failed to detect the no-deleted-ctor bug, reconsider workaround");
        // These checks are mostly provided as documentation of the exact issue
        // that needs the workaround.  The 'static_assert' tests verify that
        // the MSVC 2013 compiler bug is in effect.  If these tests were ever
        // to fail, that would indicate that the '#ifdef' check above can be
        // safely removed.
# endif
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ITERATOR RANGE FUNCTIONS
        //
        // Concerns:
        //: 1 Iterator range functions can be used with 'std' containers under
        //:   ADL.
        //
        // Plan:
        //: 1 Call all 8 range functions (unqualified) for a 'bsl::set<int>',
        //:   as this will be associated with both namespace 'bsl' and native
        //:   'std' (for 'std::less' as a template parameter).
        //
        // Testing
        //   CONCERN: Range functions are not ambiguous with 'std' under ADL
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATOR RANGE FUNCTIONS"
                            "\n================================\n");

        bsl::set<int> mX;

        (void)(begin  (mX));
        (void)(cbegin (mX));
        (void)(rbegin (mX));
        (void)(crbegin(mX));

        (void)(end  (mX));
        (void)(cend (mX));
        (void)(rend (mX));
        (void)(crend(mX));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: MAPS CONTAINING SMART POINTERS
        //
        // Concerns:
        //: 1 Insertion into maps of unique_ptr should work.
        //: 2 Insertion into maps of shared_ptr should work.
        //
        // Plan:
        //: 1 Do such operations.
        //
        // Testing:
        //   maps of smart pointers
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "CONCERN: MAPS CONTAINING SMART POINTERS\n"
                      << "=======================================\n";
        }
#if __cplusplus >= 201103
        if (veryVerbose) {
            bsl::cout << "Testing bsl::map of bsl::unique_ptr\n";
        }
        {
            bsl::map<char, bsl::unique_ptr<char>> m;
            ASSERTV(m.insert(bsl::make_pair(
                                 'a', bsl::unique_ptr<char>(new char('b'))))
                        .second);
        }
#else
        if (veryVerbose) {
            bsl::cout << "Not testing bsl::map of bsl::unique_ptr\n";
        }
#endif

        if (veryVerbose) {
            bsl::cout << "Testing bsl::map of bsl::shared_ptr\n";
        }
        {
            bsl::map<char, bsl::shared_ptr<char> > m;
            ASSERTV(m.insert(bsl::make_pair(
                                 'a', bsl::shared_ptr<char>(new char('b'))))
                        .second);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: RESIZE OF EMPTY VECTOR OF STRINGS
        //
        // Concerns:
        //: 1 {DRQS 99966534} reports an assertion failure on resizing an empty
        //:   vector of strings in opt/safe mode with gcc >= 5.
        //
        // Plan:
        //: 1 Do such an operation.
        //
        // Testing:
        //   string vector resize
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "CONCERN: RESIZE OF EMPTY VECTOR OF STRINGS\n"
                      << "==========================================\n";
        }
        {
            bsl::vector<bsl::string>().resize(10);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCERN: REGRESSION TEST FOR C99 FEATURES
        //
        // Concerns:
        //: 1 Ensure that some C99 functions are available on some Bloomberg
        //:   production platforms (to avoid any regressions from the
        //:   introduction of bsls_libraryfeatures).
        //
        // Plan:
        //: 1 Verify 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' is 'true', and
        //:   important '<cmath>' functions are available on Bloomberg
        //:   production platforms.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "CONCERN: REGRESSION TEST FOR C99 FEATURES\n"
                      << "=========================================\n";
        }
        {
              if (verbose) { bsl::cout << "Testing C99 as aliases.\n"; }
#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
              typedef int (*FuncPtrType)(double);

              FuncPtrType funcPtr = &bsl::fpclassify;
              (void)funcPtr;
#endif
        }
      } break;
      case 1: {
        if (verbose) {
            bsl::cout << "TESTING REFERENCES AS 'mapped_type'.\n"
                      << "====================================\n";
        }

        if (verbose) { bsl::cout << "Testing 'map' container.\n"; }
        {
            typedef bsl::map<int, bsltf::SimpleTestType&>  MapType1;
            typedef bsl::map<int, bsltf::AllocTestType&>   MapType2;
            typedef bsl::map<int, bsltf::MovableTestType&> MapType3;

            RUN_EACH_TYPE(MapTestDriver,
                          testCase1,
                          MapType1, MapType2, MapType3);
        }

        if (verbose) { bsl::cout << "Testing 'multimap' container.\n"; }
        {
            typedef bsl::multimap<int, bsltf::SimpleTestType&>  MapType1;
            typedef bsl::multimap<int, bsltf::AllocTestType&>   MapType2;
            typedef bsl::multimap<int, bsltf::MovableTestType&> MapType3;

            RUN_EACH_TYPE(MapTestDriver,
                          testCase1,
                          MapType1, MapType2, MapType3);
        }

        if (verbose) { bsl::cout << "Testing 'unordered_map' container.\n"; }
        {
            typedef bsl::unordered_map<int, bsltf::SimpleTestType&>  MapType1;
            typedef bsl::unordered_map<int, bsltf::AllocTestType&>   MapType2;
            typedef bsl::unordered_map<int, bsltf::MovableTestType&> MapType3;

            RUN_EACH_TYPE(MapTestDriver,
                          testCase1,
                          MapType1, MapType2, MapType3);
        }

        if (verbose) {
            bsl::cout << "Testing 'unordered_multimap' container.\n";
        }
        {
            typedef bsl::unordered_multimap<int, bsltf::SimpleTestType&>
                                                                      MapType1;

            typedef bsl::unordered_multimap<int, bsltf::AllocTestType&>
                                                                      MapType2;

            typedef bsl::unordered_multimap<int, bsltf::MovableTestType&>
                                                                      MapType3;

            RUN_EACH_TYPE(MapTestDriver,
                          testCase1,
                          MapType1, MapType2, MapType3);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << ".\n";
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
