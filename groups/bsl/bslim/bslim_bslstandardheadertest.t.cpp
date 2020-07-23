// bslim_bslstandardheadertest.t.cpp                                  -*-C++-*-
#include <bslim_bslstandardheadertest.h>

#include <bslim_testutil.h>

#include <bslma_constructionutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsltf_templatetestfacility.h>

// #include all of the headers defined in bsl+bslhdrs.

#include <bsl_algorithm.h>
#include <bsl_array.h>
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
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_istream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_locale.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_new.h>
#include <bsl_numeric.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
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

#ifndef BDE_OPENSOURCE_PUBLICATION
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// stlport containers are deprecated and are not forwarded in the opensource
// distribution

#include <bsl_hash_map.h>
#include <bsl_hash_set.h>
#include <bsl_slist.h>
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
#endif  // BDE_OPENSOURCE_PUBLICATION

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
#include <bsl_type_traits.h>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
#include <bsl_shared_mutex.h>
#endif

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
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
// [ 8] CONCERN: 'default_searcher'/'boyer_moore_horspool_searcher usable.
// [ 8] CONCERN: 'boyer_moore_searcher' usable when available.
// [ 8] CONCERN: 'bsl::search' function is usable.
// [ 5] CONCERN: Range functions are not ambiguous with 'std' under ADL
// [ 6] CONCERN: 'forward_list' is available in C++11 builds
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
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
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
    // functionality of bsl map-like containers. Each "testCase*" method test a
    // specific aspect of such containers.

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
        //:   the exmaple demonstrates that the classes can be found in 'bsl'.
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
        //
        // Plan:
        //: 1 Create a 'bsl::forward_list<bsl::string>' using a test allocator,
        //:   emplace a string into the container, and verify that the emplaced
        //:   string is using the correct allocator.
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
        static_assert(native_std::is_copy_constructible<Id>::value,
            "Failed to detect the no-deleted-ctor bug, reconsider workaround");
        static_assert(native_std::is_copy_constructible<Item>::value,
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
              FuncPtrType funcPtr = &bsl::fpclassify; (void)funcPtr;
#endif
        }
      } break;
      case 1: {
        if (verbose) { bsl::cout << "Testing references as 'mapped_type'.\n"; }

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
