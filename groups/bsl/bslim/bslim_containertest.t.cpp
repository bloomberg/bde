// bslim_containertest.t.cpp                                          -*-C++-*-
#include <bslim_containertest.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_constructionutil.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#include <bsltf_templatetestfacility.h>

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
    //:   unordered_multimap) support references as 'mapped_type'.
    //:
    //: 2 Container does not make a copy of the referenced object on insertion.
    //:
    //: 3 The mapped value copy-assignment operator assigns to the original
    //:   object and does not rebind the reference.
    //:
    //: 4 The container copy-assignment operator does not copy mapped values.
    //:
    //: 5 The container destructor does not destroy the referenced objects.
    //
    // Plan:
    //: 1 Create the value type containing a reference as a 'mapped_type' and
    //:   insert it into the container.  (C-1)
    //:
    //: 2 Verify that the container inserts the reference and does not make a
    //:   copy of the referenced object.  (C-2)
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

    ValueType tempPair(tempKey.object(), tempMapped.object());

    CONTAINER mX;  const CONTAINER& X = mX;

    mX.insert(MoveUtil::move(tempPair));

    CIter ret = X.find(tempKey.object());

    ASSERT(ret != X.end());

    // Reference still refers to the original object.
    ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());

    {
        CONTAINER mY;  const CONTAINER& Y = mY;

        mY = X;

        CIter ret = Y.find(tempKey.object());

        ASSERT(ret != Y.end());

        // Reference still refers to the original object.
        ASSERT(bsls::Util::addressOf(ret->second) == tempMapped.address());
    }

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
      case 1: {
#if !defined(BSLS_PLATFORM_CMP_SUN)
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
#else
        if (verbose) {
            bsl::cout << "Not testing references as mapped_type "
                      << "on this platform.\n";
        }
#endif
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
// Copyright 2016 Bloomberg Finance L.P.
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
