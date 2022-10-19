// bslstl_vector.1.t.cpp                                              -*-C++-*-
#define BSLSTL_VECTOR_0T_AS_INCLUDE
#include <bslstl_vector.0.t.cpp>

#include <bsls_stopwatch.h>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_vector.0.t.cpp'.

//=============================================================================
//                       TEST DRIVER TEMPLATE
//=============================================================================

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver1 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_VECTOR_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

    // CONSTANTS
    static
    const bool s_typeIsMoveEnabled = bsl::is_nothrow_move_constructible<TYPE>::
                                                                         value;

                            // TEST CASES
    // CLASS METHODS
    static void testCase11();
        // Test allocator-related concerns.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG, bool OTHER_FLAGS>
    static void testCase8_propagate_on_container_swap_dispatch();
        // Test 'propagate_on_container_swap'.

    static void testCase8_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase8_swap_noexcept();
        // Test 'swap' noexcept specifications.

    static void testCase8_dispatch();
        // Test 'swap' member.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
        // Test 'select_on_container_copy_construction'.

    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase4a();
        // Test non-'const' accessors ('at' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2a();
        // Old test 2, now 2a

    static void testCase2();
        // Test primary manipulators ('push_back' and 'clear').

    static void testCase1();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    template <class CONTAINER>
    static void testCaseM1Range(const CONTAINER&);
        // Performance test for operators that take a range of inputs.

    static void testCaseM1();
        // Performance test.
};

                  // ==================================
                  // template class StdBslmaTestDriver1
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver1 : public StdBslmaTestDriverHelper<TestDriver1, TYPE>
{
};

                    // ==============================
                    // template class MetaTestDriver1
                    // ==============================

template <class TYPE>
struct MetaTestDriver1 {
    // This 'struct' is to be call by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver1' instantiated
    // with different types of allocator.

    static void testCase8();
        // Test member and free 'swap'.
};

template <class TYPE>
void MetaTestDriver1<TYPE>::testCase8()
{
    typedef bsl::allocator<TYPE>            BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    // The low-order bit of the identifier specifies whether the third boolean
    // argument of the stateful allocator, which indicates propagate on
    // container swap, is set.

    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<TYPE, false, false, true,  false> A01;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  false, true>  A10;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  true>  A11;

    if (verbose) printf("\n");

    TestDriver1<TYPE, BAP>::testCase8_dispatch();

    TestDriver1<TYPE, SAA>::testCase8_dispatch();

    TestDriver1<TYPE, A00>::testCase8_dispatch();
    TestDriver1<TYPE, A01>::testCase8_dispatch();
    TestDriver1<TYPE, A10>::testCase8_dispatch();
    TestDriver1<TYPE, A11>::testCase8_dispatch();

    // is_always_equal == true && propagate_on_container_swap == true
    TestDriver1<TYPE , StatelessAllocator<TYPE , true> >::
        testCase8_swap_noexcept();
    TestDriver1<TYPE*, StatelessAllocator<TYPE*, true> >::
        testCase8_swap_noexcept();

    // is_always_equal == true && propagate_on_container_swap == false
    TestDriver1<TYPE , StatelessAllocator<TYPE>  >::testCase8_swap_noexcept();
    TestDriver1<TYPE*, StatelessAllocator<TYPE*> >::testCase8_swap_noexcept();
}
                                // ----------
                                // TEST CASES
                                // ----------


template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase11()
{
    // ------------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // Concerns:
    //: 1 That the 'computeNewCapacity' class method does not overflow
    //: 2 That creating an empty vector does not allocate
    //: 3 That the allocator is passed through to elements
    //: 4 That the vector class has the 'bslma::UsesBslmaAllocator' trait.
    //
    // Plan:
    //: 1 We first verify that the 'vector' class has the traits, and
    //:   that allocator TBD
    //
    // Testing:
    //   TRAITS
    //
    // TBD When a new vector object Y is created from an old vector object
    //      X, then the standard states that Y should get its allocator by
    //      copying X's allocator (23.1, Point 8).  Our vector implementation
    //      does not follow this rule for 'bslma::Allocator'-based allocators.
    //      To verify this behavior for non-'bslma::Allocator', should test
    //      copy constructor using one and verify standard is followed.
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    const TestValues    VALUES;

    if (verbose)
        printf("\tTesting 'bslma::UsesBslmaAllocator'.\n");

    ASSERT((bsl::is_convertible<bslma::Allocator *,
                                typename Obj::allocator_type>::value));
    ASSERT((bslma::UsesBslmaAllocator<Obj>::value));

    if (verbose)
        printf("\tTesting that empty vector does not allocate.\n");
    {
        Obj mX(&ta);
        ASSERT(0 == ta.numBytesInUse());
    }

    if (verbose)
        printf("\tTesting passing allocator through to elements.\n");

    ASSERT(( bslma::UsesBslmaAllocator<TYPE>::value));
    {
        // Note that this constructor has not otherwise been tested yet
        Obj mX(1, VALUES[0], &ta);  const Obj& X = mX;
        ASSERT(&ta == X[0].allocator());
        ASSERT(2   == ta.numBlocksInUse());
    }
    {
        Obj mX(&ta);  const Obj& X = mX;
        mX.push_back(VALUES[0]);
        ASSERT(&ta == X[0].allocator());
        ASSERT(2   == ta.numBlocksInUse());
    }

    ASSERT(0 == ta.numBytesInUse());
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver1<TYPE, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS> StdAlloc;

    typedef bsl::vector<TYPE, StdAlloc>         Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC = SPECS[ti];
        TestValues        IVALUES(ISPEC);

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        Obj mW(scratch);
        const Obj& W = gg(&mW, ISPEC);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC = SPECS[tj];
            TestValues        JVALUES(JSPEC);
            {
                IVALUES.resetIterators();

                Obj        mY(mas);
                const Obj& Y = gg(&mY, ISPEC);

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj        mX(mat);
                const Obj& X = gg(&mX, JSPEC);

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC, W,  X,   W  == X);
                ASSERTV(ISPEC, JSPEC, W,  Y,   W  == Y);
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

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR
    //
    // We have the following concerns:
    //: 1 The value represented by any object can be assigned to any other
    //:   object regardless of how either value is represented internally.
    //:
    //: 2 The 'rhs' value must not be affected by the operation.
    //:
    //: 3 'rhs' going out of scope has no effect on the value of 'lhs' after
    //:   the assignment.
    //:
    //: 4 Aliasing (x = x): The assignment operator must always work -- even
    //:   when the lhs and rhs are identically the same object.
    //:
    //: 5 The assignment operator must be neutral with respect to memory
    //:   allocation exceptions.
    //:
    //: 6 The assignment's internal functionality varies according to which
    //:   bitwise copy/move trait is applied.
    //
    // Plan:
    //: 1 Specify a set S of unique object values with substantial and varied
    //:   differences, ordered by increasing length.  For each value in S,
    //:   construct an object x along with a sequence of similarly constructed
    //:   duplicates x1, x2, ..., xN.  Attempt to affect every aspect of
    //:   white-box state by altering each xi in a unique way.  Let the union
    //:   of all such objects be the set T.
    //:
    //: 2 To address concerns 1, 2, and 5, construct tests u = v for all (u, v)
    //:   in T X T.  Using canonical controls UU and VV, assert before the
    //:   assignment that UU == u, VV == v, and v == u if and only if VV == UU.
    //:   After the assignment, assert that VV == u, VV == v, and, for grins,
    //:   that v == u.  Let v go out of scope and confirm that VV == u.  All of
    //:   these tests are performed within the 'bslma' exception testing
    //:   apparatus.  Since the execution time is lengthy with exceptions,
    //:   every permutation is not performed when exceptions are tested.  Every
    //:   permutation is also tested separately without exceptions.
    //:
    //: 3 As a separate exercise, we address 4 and 5 by constructing tests
    //:   y = y for all y in T.  Using a canonical control X, we will verify
    //:   that X == y before and after the assignment, again within the bslma
    //:   exception testing apparatus.
    //:
    //: 4 To address concern 6, all these tests are performed on user defined
    //:   types:
    //:
    //:   1 With allocator, copyable
    //:   2 With allocator, moveable
    //:   3 With allocator, not moveable
    //
    // Testing:
    //   vector<T,A>& operator=(const vector<T,A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues    VALUES;

    if (verbose) printf("\tTesting without exceptions.\n");
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEA",  // Try equal-size assignment of different values.
            "DEAB",
            "CBAEDCBA",
            "EDCBAEDCB",
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        {
            int uOldLen = -1;
            for (int ui = 0; ui < NUM_SPECS; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = static_cast<int>(strlen(U_SPEC));

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                ASSERTV(U_SPEC, uOldLen <= uLen);
                uOldLen = uLen;

                Obj mUU; const Obj& UU = gg(&mUU, U_SPEC);  // control
                // same lengths
                ASSERTV(ui, uLen == static_cast<int>(UU.size()));

                for (int vi = 0; vi < NUM_SPECS; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    Obj mVV; const Obj& VV = gg(&mVV, V_SPEC);  // control

                    const bool Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            Obj mU(xta);
                            stretchRemoveAll(
                                        &mU,
                                        U_N,
                                        TstFacility::getIdentifier(VALUES[0]));
                            const Obj& U = gg(&mU, U_SPEC);
                            {
                                Obj mV(xta);
                                stretchRemoveAll(
                                        &mV,
                                        V_N,
                                        TstFacility::getIdentifier(VALUES[0]));
                                const Obj& V = gg(&mV, V_SPEC);
                    // v--------
                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                    // TBD: moving to 'bsltf' types does not allow for this
                    //      test -- we can either add this to 'bsltf' or find
                    //      another way to test this (as in 'set')
                    // const int NUM_CTOR = numCopyCtorCalls;
                    // const int NUM_DTOR = numDestructorCalls;
                    //
                    // const size_t OLD_LENGTH = U.size();

                    mU = V; // test assignment here

                    // ASSERT((numCopyCtorCalls - NUM_CTOR) <= (int)V.size());
                    // ASSERT((numDestructorCalls - NUM_DTOR)
                    //                                     <= (int)OLD_LENGTH);

                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    ASSERTV(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    // ---------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            ASSERTV(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\tTesting with exceptions.\n");
    {
        static const char *SPECS[] = { // len: 0-2, 4, 9,
            "",
            "A",
            "BC",
            "DEAB",
            "EDCBAEDCB",
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        static const int EXTEND[] = {
            0, 1, 3, 5
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int iterationModulus = 1;
        int iteration = 0;
        {
            int uOldLen = -1;
            for (int ui = 0; ui < NUM_SPECS; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = static_cast<int>(strlen(U_SPEC));

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                ASSERTV(U_SPEC, uOldLen < uLen);
                uOldLen = uLen;

                Obj mUU; const Obj& UU = gg(&mUU, U_SPEC);  // control
                // same lengths
                ASSERTV(ui, uLen == static_cast<int>(UU.size()));

                // int vOldLen = -1;
                for (int vi = 0; vi < NUM_SPECS; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    // control
                    Obj mVV; const Obj& VV = gg(&mVV, V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                    //--------------^
                    const bsls::Types::Int64 AL = ta.allocationLimit();
                    ta.setAllocationLimit(-1);
                    Obj mU(xta);
                    stretchRemoveAll(&mU,
                                     U_N,
                                     TstFacility::getIdentifier(VALUES[0]));
                    const Obj& U = gg(&mU, U_SPEC);
                    {
                        Obj mV(xta);
                        stretchRemoveAll(
                                        &mV,
                                        V_N,
                                        TstFacility::getIdentifier(VALUES[0]));
                        const Obj& V = gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        ta.setAllocationLimit(AL);
                        {
                            mU = V; // test assignment here
                        }

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

    if (verbose) printf("\tTesting self assignment (Aliasing).\n");
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEAB",
            "ABCDEABC",
            "ABCDEABCD",
            "ABCDEABCDEABCDE",
            "ABCDEABCDEABCDEA",
            "ABCDEABCDEABCDEAB",
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
        };
        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0;  ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int         curLen = static_cast<int>(strlen(SPEC));

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            ASSERTV(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            Obj mX; const Obj& X = gg(&mX, SPEC);
            ASSERTV(ti, curLen == (int)X.size());  // same lengths

            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                    const bsls::Types::Int64 AL = ta.allocationLimit();
                    ta.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(xta);
                    stretchRemoveAll(&mY,
                                     N,
                                     TstFacility::getIdentifier(VALUES[0]));
                    const Obj& Y = gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    ASSERTV(SPEC, N, Y == Y);
                    ASSERTV(SPEC, N, X == Y);

                    ta.setAllocationLimit(AL);
                    {
                        ExceptionProctor<Obj, ALLOC> proctor(&mY, Y, L_);
                        mY = Y; // test assignment here
                    }

                    ASSERTV(SPEC, N, Y == Y);
                    ASSERTV(SPEC, N, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver1<TYPE, ALLOC>::testCase8_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<TYPE,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS> StdAlloc;

    typedef bsl::vector<TYPE, StdAlloc>              Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator xoa("x-original", veryVeryVeryVerbose);
        bslma::TestAllocator yoa("y-original", veryVeryVeryVerbose);

        StdAlloc xma(&xoa);
        StdAlloc yma(&yoa);

        StdAlloc scratch(&da);

        const Obj ZZ(IVALUES.begin(), IVALUES.end(), scratch);  // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const Obj WW(JVALUES.begin(), JVALUES.end(), scratch);  // control

            {
                IVALUES.resetIterators();

                Obj mX(IVALUES.begin(), IVALUES.end(), xma);
                const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                JVALUES.resetIterators();

                Obj mY(JVALUES.begin(), JVALUES.end(), yma);
                const Obj& Y = mY;

                ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    mX.swap(mY);

                    ASSERTV(ISPEC, JSPEC, WW, X, WW == X);
                    ASSERTV(ISPEC, JSPEC, ZZ, Y, ZZ == Y);

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

                    ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                    ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                }
            }
            ASSERTV(ISPEC, 0 == xoa.numBlocksInUse());
            ASSERTV(ISPEC, 0 == yoa.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase8_propagate_on_container_swap()
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
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'ZZ' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'WW' using a
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

    testCase8_propagate_on_container_swap_dispatch<false, false>();
    testCase8_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\t'propagate_on_container_swap::value == true'\n");

    testCase8_propagate_on_container_swap_dispatch<true, false>();
    testCase8_propagate_on_container_swap_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase8_swap_noexcept()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: NOEXCEPT SPECIFICATIONS
    //
    // Concerns:
    //: 1 If either 'allocator_traits<Allocator>::propagate_on_container_swap::
    //:   value' or 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the swap functions are 'noexcept(true)'.
    //
    // Plan:
    //: 1 Get ORed value of the both traits.
    //:
    //: 2 Compare the value with the member 'swap' function noexcept
    //:   specification.
    //:
    //: 3 Compare the value with the free 'swap' function noexcept
    //:   specification.
    //
    // Testing:
    //   vector::swap()
    //   swap(vector& , vector&)
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::vector<TYPE, ALLOC> a, b;

    const bool isNoexcept =
                         AllocatorTraits::propagate_on_container_swap::value ||
                         AllocatorTraits::is_always_equal::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(a, b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase8_dispatch()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped uses the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 9 If either 'allocator_traits<Allocator>::is_always_equal::value' or
    //:   'allocator_traits<Allocator>::propagate_on_container_swap::value' is
    //:   true, the 'swap' functions are 'noexcept(true)'.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range
    //:     of values that each individual attribute can independently
    //:     attain, and (c) values that should require allocation from each
    //:     individual attribute that can independently allocate memory.
    //:
    //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
    //:     the expectation of memory allocation for all typical
    //:     implementations of individual attribute types: ('Y') "Yes",
    //:     ('N') "No", or ('?') "implementation-dependent".
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
    //:
    //:     1 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX' (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is not empty,
    //:         and temporary memory were allocated from 'oaz' if 'mX' is not
    //:         empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the member and free 'swap'
    //:       functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:       after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //:    7 Create a new object allocator, 'oap'.
    //:
    //:    8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:      'mP', having the value described by 'R2'; also use the copy
    //:      constructor to create, using a "scratch" allocator, a const 'Obj',
    //:      'PP', from 'P.
    //:
    //:    9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:      be an alias to 'true_type' (Instead of this manual step, use an
    //:      allocator that enables propagate_on_container_swap when
    //:      AllocatorTraits supports it) and use the member and free 'swap'
    //:      functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:      after each swap, that:  (C-1, 6)
    //:
    //:      1 The values have been exchanged.  (C-1)
    //:
    //:      2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:      3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   3 Use the default constructor and 'oa' to create a modifiable
    //:     'Obj' 'mX' (having default attribute values); also use the copy
    //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
    //:     'XX' from 'mX'.
    //:
    //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mY' having the value described by the 'Ai' attributes; also
    //:     use the copy constructor and a "scratch" allocator to create a
    //:     'const' 'Obj' 'YY' from 'mY'.
    //:
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //:
    //: 6 To address concern 9, pass allocators with all combinations of
    //:   'is_always_equal' and 'propagate_on_container_swap' values.
    //
    // Testing:
    //   void swap(vector&);
    //   void swap(vector<T,A>& lhs, vector<T,A>& rhs);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                           AllocatorTraits::propagate_on_container_swap::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING SWAP '%s' OTHER:%c PROP:%c ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<TYPE>().name(), otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    // Assign the address of each function to a variable.
    {
        typedef bsl::vector<TYPE, ALLOC>  IObj;
        typedef bsl::vector<TYPE *>       PObj;
        typedef bsl::vector<const TYPE *> CPObj;

        typedef void (IObj::*MemberPtr)(IObj&);
        typedef void (*FreeFuncPtr)    (Obj&,   Obj&);
        typedef void (*FreePFuncPtr)   (PObj&,  PObj&);
        typedef void (*FreeCPFuncPtr)  (CPObj&, CPObj&);

        // Verify that the signatures and return types are standard.

        MemberPtr memberSwap = &IObj::swap;

        (void) memberSwap;  // quash potential compiler warnings

#if !defined(BSLS_PLATFORM_CMP_SUN)
        // This should work, but Solaris CC can't cope with it, which is
        // probably a compiler bug.

        FreeFuncPtr   freeSwap   = &bsl::swap;
        FreePFuncPtr  freePSwap  = &bsl::swap;
        FreeCPFuncPtr freeCPSwap = &bsl::swap;

        (void) freeSwap;
        (void) freePSwap;
        (void) freeCPSwap;
#endif
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::TestAllocator         oa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator         za("other",     veryVeryVeryVerbose);
    bslma::TestAllocator         sa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator         fa("footprint", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    ALLOC xda(&da);
    ALLOC xoa(&oa);
    ALLOC xza(&za);
    ALLOC xsa(&sa);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
               AllocatorTraits::propagate_on_container_move_assignment::value);
    ASSERT((otherTraitsSet ? xsa : xda) ==
                  AllocatorTraits::select_on_container_copy_construction(xsa));

    // Use a table of distinct object values and expected memory usage.

    enum { NUM_DATA                         = DEFAULT_NUM_DATA };
    const  DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (veryVerbose) {
        printf("Testing swap with matching allocs, no exceptions.\n");
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *SPEC1 = DATA[ti].d_spec;

        Obj mW(xoa);     const Obj& W  = gg(&mW,  SPEC1);
        Obj mXX(xsa);    const Obj& XX = gg(&mXX, SPEC1);

        if (veryVerbose) { printf("noexcep: src: "); P_(SPEC1) P(XX) }

        // Ensure the first row of the table contains the default-constructed
        // value.

        if (0 == ti) {
            ASSERTV(SPEC1, Obj(xsa), W, Obj(xsa) == W);
        }

        for (int member = 0; member < 2; ++member) {
            bslma::TestAllocatorMonitor oam(&oa);

            if (member) {
                mW.swap(mW);    // member 'swap'
            }
            else {
                swap(mW, mW);   // free function 'swap'
            }

            ASSERTV(SPEC1, XX, W, XX == W);
            ASSERTV(SPEC1, xoa == W.get_allocator());
            ASSERTV(SPEC1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mYY(xsa);   const Obj& YY = gg(&mYY, SPEC2);

            if (veryVerbose) { printf("noexcep: src: "); P_(SPEC1) P(YY) }

            ASSERT((ti == tj) == (XX == YY));

            for (int member = 0; member < 2; ++member) {
                Obj mX(xoa);    const Obj& X  = gg(&mX,  SPEC1);
                Obj mY(xoa);    const Obj& Y  = gg(&mY,  SPEC2);

                if (veryVerbose) {
                    T_ printf("before: "); P_(X) P(Y);
                }

                ASSERT(XX == X && YY == Y);

                if (veryVerbose) { T_ P_(SPEC2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&oa);

                if (member) {
                    mX.swap(mY);    // member 'swap'
                }
                else {
                    swap(mX, mY);   // free function 'swap'
                }

                ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                ASSERTV(SPEC1, SPEC2, XX, Y, XX == Y);
                ASSERTV(SPEC1, SPEC2, xoa == X.get_allocator());
                ASSERTV(SPEC1, SPEC2, xoa == Y.get_allocator());
                ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
            }
        }
    }

    if (veryVerbose) {
        printf("Testing swap, non-matching, with injected exceptions.\n");
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1  = DATA[ti].d_spec;
        const size_t      LENGTH1 = strlen(SPEC1);

        Obj mXX(xsa);    const Obj& XX = gg(&mXX, SPEC1);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;
            const size_t      LENGTH2 = strlen(SPEC2);

            if (4 < LENGTH2) {
                continue;    // time consuming, skip (it's O(LENGTH2^2))
            }

            Obj mYY(xsa);   const Obj& YY = gg(&mYY, SPEC2);

            ASSERT((ti == tj) == (XX == YY));

            for (int member = 0; member < 2; ++member) {
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    Int64 al = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(xoa);    const Obj& X = gg(&mX, SPEC1);
                    Obj mZ(xza);    const Obj& Z = gg(&mZ, SPEC2);

                    if (veryVerbose) {
                        T_ printf("before: "); P_(X) P(Z);
                    }

                    ASSERT(XX == X && YY == Z);

                    oa.setAllocationLimit(al);
                    bslma::TestAllocatorMonitor oam(&oa);
                    bslma::TestAllocatorMonitor zam(&za);

                    if (member) {
                        mX.swap(mZ);     // member 'swap'
                    }
                    else {
                        swap(mX, mZ);    // free function 'swap'
                    }

                    oa.setAllocationLimit(-1);

                    if (veryVerbose) {
                        T_ printf("after:  "); P_(X) P(Z);
                    }

                    ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                    ASSERTV(SPEC1, SPEC2, XX, Z, XX == Z);
                    ASSERTV(SPEC1, SPEC2, (isPropagate ? xza : xoa) ==
                                                            X.get_allocator());
                    ASSERTV(SPEC1, SPEC2, (isPropagate ? xoa : xza) ==
                                                            Z.get_allocator());

                    if (isPropagate || 0 == LENGTH2) {
                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                    }
                    else {
                        ASSERT(!PLAT_EXC || 1 < numPasses);
                        ASSERTV(SPEC1, SPEC2, oam.isTotalUp());
                    }

                    if (isPropagate || 0 == LENGTH1) {
                        ASSERTV(SPEC1, SPEC2, zam.isTotalSame());
                    }
                    else {
                        ASSERTV(SPEC1, SPEC2, zam.isTotalUp());
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

    if (veryVerbose) printf(
              "Invoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        Obj mX(xoa);  const Obj& X  = gg(&mX,  "DD");
        Obj mXX(xsa); const Obj& XX = gg(&mXX, "DD");

        Obj mY(xoa);  const Obj& Y  = gg(&mY,  "ABC");
        Obj mYY(xsa); const Obj& YY = gg(&mYY, "ABC");

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }

        invokePatternSwap(&mX, &mY);

        ASSERTV(YY, X, XX == X);
        ASSERTV(XX, Y, YY == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == da.numBlocksTotal());

    // Verify 'swap' noexcept specifications for the basic template and the
    // partial specialization of 'vector' for pointer types

    TestDriver1<TYPE , ALLOC>::testCase8_swap_noexcept();
    TestDriver1<TYPE*, ALLOC>::testCase8_swap_noexcept();
}

template <class TYPE, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver1<TYPE, ALLOC>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    TYPE,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS> StdAlloc;

    typedef bsl::vector<TYPE, StdAlloc>          Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);

        {
            Obj mW(ma); const Obj& W = gg(&mW, SPEC);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(ma); const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
            ASSERTV(SPEC, PROPAGATE, PROPAGATE == (ma == Y.get_allocator()));
            ASSERTV(SPEC, PROPAGATE,               ma == X.get_allocator());

            if (PROPAGATE) {
                ASSERTV(SPEC, 0 != TYPE_ALLOC || dam.isInUseSame());
                ASSERTV(SPEC, 0 ==     LENGTH || oam.isInUseUp());
            }
            else {
                ASSERTV(SPEC, 0 ==     LENGTH || dam.isInUseUp());
                ASSERTV(SPEC, oam.isTotalSame());
            }
        }
        ASSERTV(SPEC, 0 == da.numBlocksInUse());
        ASSERTV(SPEC, 0 == oa.numBlocksInUse());
    }
}

template <class TYPE, class ALLOC>
void
TestDriver1<TYPE, ALLOC>::testCase7_select_on_container_copy_construction()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 The allocator of a source object using a standard allocator is
    //:   propagated to the newly constructed object according to the
    //:   'select_on_container_copy_construction' method of the allocator.
    //:
    //: 2 In the absence of a 'select_on_container_copy_construction' method,
    //:   the allocator of a source object using a standard allocator is always
    //:   propagated to the newly constructed object (C++03 semantics).
    //:
    //: 3 The effect of the 'select_on_container_copy_construction' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create a 'bsltf::StdStatefulAllocator' with its
    //:   'select_on_container_copy_construction' property configured to
    //:   'false'.  In two successive iterations of P-3..5, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value in S, initialize objects 'W' (a control) and 'X' using
    //:   the allocator from P-2.
    //:
    //: 4 Copy construct 'Y' from 'X' and use 'operator==' to verify that both
    //:   'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //: 5 Use the 'get_allocator' method to verify that the allocator of 'X'
    //:   is *not* propagated to 'Y'.
    //:
    //: 6 Repeat P-2..5 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'X' *is*
    //:   propagated to 'Y'.  (C-1)
    //:
    //: 7 Repeat P-2..5 except that this time use a 'StatefulStlAllocator',
    //:   which does not define a 'select_on_container_copy_construction'
    //:   method, and verify that the allocator of 'X' is *always* propagated
    //:   to 'Y'.  (C-2..3)
    //
    // Testing:
    //   select_on_container_copy_construction
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    if (verbose) printf("\t'select_on_container_copy_construction' "
                        "propagates *default* allocator.\n");

    testCase7_select_on_container_copy_construction_dispatch<false, false>();
    testCase7_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\t'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase7_select_on_container_copy_construction_dispatch<true, false>();
    testCase7_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\tVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<TYPE>   Allocator;
    typedef bsl::vector<TYPE, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            Obj mW(a); const Obj& W = gg(&mW, SPEC);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(a); const Obj& X = gg(&mX, SPEC);
            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
    //
    // We have the following concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 All internal representations of a given value can be used to create
    //:   a new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('push_back's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   a user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For concerns 1 - 4, for each value in S, initialize objects w and x,
    //:   copy construct y from x and use 'operator==' to verify that both x
    //:   and y subsequently have the same value as w.  Let x go out of scope
    //:   and again verify that w == y.
    //:
    //: 3 For concern 5, for each value in S initialize objects w and x, and
    //:   copy construct y from x.  Change the state of y, by using the
    //:   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //:   that y differs from x and w, and verify that the capacity of y
    //:   changes correctly.
    //:
    //: 4 To address concern 6, we will perform tests performed for concern 1:
    //:
    //:    1 While passing a testAllocator as a parameter to the new object and
    //:      ascertaining that the new object gets its memory from the provided
    //:      'testAllocator'.  Also perform test for concerns 2 and 5.
    //:
    //:    2 Where the object is constructed with an object allocator, and
    //:       neither of global and default allocator is used to supply memory.
    //:
    //: 5 To address concern 7, perform tests for concern 1 performed in the
    //:   presence of exceptions during memory allocations using a
    //:   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   vector<T,A>(const vector<T,A>& original);
    //   vector<T,A>(const vector<T,A>& original, const A& alloc);
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const size_t     NUM_VALUES = VALUES.size();

    const bool NOT_MOVABLE = !(bslmf::IsBitwiseMoveable<TYPE>::value ||
                                                          s_typeIsMoveEnabled);

        // if moveable, moves do not count as allocs
    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                            bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
                       "\nTesting '%s' (TYPE_ALLOC = %d, NOT_MOVABLE = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        NOT_MOVABLE);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEAB",
            "ABCDEABC",
            "ABCDEABCD",
            "ABCDEABCDEABCDE",
            "ABCDEABCDEABCDEA",
            "ABCDEABCDEABCDEAB",
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\tFor an object of length " ZU ":\t", LENGTH);
                P(SPEC);
            }

            ASSERTV(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object 'W'.
            Obj        mW;
            const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                // Use an 'ObjectBuffer' below as 'X' will have a lifetime
                // shorter than the whole loop iteration.

                bsls::ObjectBuffer<Obj> xBuf;
                new (xBuf.buffer()) Obj(xta);
                bslma::DestructorProctor<Obj> proctorX(xBuf.address());
                Obj&  mX = xBuf.object();

                stretchRemoveAll(&mX,
                                 N,
                                 TstFacility::getIdentifier(VALUES[0]));
                const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                {   // Testing concern 1.

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Obj Y0(X);

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    ASSERTV(SPEC, N, W == Y0);
                    ASSERTV(SPEC, N, W == X);
                    ASSERTV(SPEC, N, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());
                    ASSERTV(SPEC, N, LENGTH == Y0.capacity());
                }
                {   // Testing concern 5.

                    if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap   = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        stretch(&Y1,
                                1,
                                TstFacility::getIdentifier(
                                                      VALUES[i % NUM_VALUES]));

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        ASSERTV(SPEC, N, i, Y1.size() == LENGTH + i);
                        ASSERTV(SPEC, N, i, W         != Y1);
                        ASSERTV(SPEC, N, i, X         != Y1);

                        if (oldCap == 0) {
                            ASSERTV(SPEC, N, i, Y1.capacity() == 1);
                        }
                        else if (remSlots == 0) {
                            ASSERTV(SPEC, N, i, Y1.capacity() == 2 * oldCap);
                        }
                        else {
                            ASSERTV(SPEC, N, i, Y1.capacity() == oldCap);
                        }
                    }
                }
                {   // Testing concern 6 with test allocator.

                    if (veryVerbose) printf(
                      "\t\t\tInsert into created obj, with test allocator:\n");

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    // Make a copy of the reference object, 'X', using the same
                    // allocator.

                    Obj mY11(X, xta);   const Obj& Y11 = mY11;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    // No allocations if both objects are empty; otherwise, one
                    // allocation for vector storage, and if the element type
                    // uses allocators, exactly one additional allocation per
                    // element.

                    if (LENGTH == 0) {
                        ASSERTV(SPEC, N, AA, BB, 0 == AA - BB);
                        ASSERTV(SPEC, N, A,  B,  0 == A  -  B);
                    }
                    else {
                        const  bsls::Types::Int64 TYPE_ALLOCS =
                                                         TYPE_ALLOC * X.size();
                        ASSERTV(SPEC, N, AA, BB, 1 + TYPE_ALLOCS == AA - BB);
                        ASSERTV(SPEC, N, A,  B,  1 + TYPE_ALLOCS == A  -  B);
                    }

                    // Copies always optimize initial storage to full capacity.

                    ASSERTV(SPEC, N, Y11.capacity(),   Y11.size(),
                                     Y11.capacity() == Y11.size());

                    // Now, repeatedly extend by one element, and verify that
                    // the expected number of allocations are performed and
                    // retained by the expected allocator.

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldSize  = Y11.size();
                        const size_t oldCap   = Y11.capacity();
                        const size_t remSlots = Y11.capacity() - Y11.size();

                        const bsls::Types::Int64 CC = ta.numBlocksTotal();
                        const bsls::Types::Int64 C  = ta.numBlocksInUse();

                        stretch(&mY11,
                                 1,
                                 TstFacility::getIdentifier(
                                                      VALUES[i % NUM_VALUES]));

                        const bsls::Types::Int64 DD = ta.numBlocksTotal();
                        const bsls::Types::Int64 D  = ta.numBlocksInUse();

                        // There will be no allocations for the stretch element
                        // unless it uses allocators itself.  Then, there will
                        // be two allocations to make the proto-value and then
                        // the copy, unless it can move-optimize down to just
                        // one allocation (irrespective of 'noexcept' traits).

                        const Int64 STRETCH_ALLOC = TYPE_ALLOC
                                                  ? s_typeIsMoveEnabled ? 1 : 2
                                                  : 0;

                        // Additional allocations for buffer if capacity grows,
                        // and potentially for copying elements on growth if
                        // the element type is non-trivial or otherwise does
                        // not detect as having a 'noexcept' move.

                        const bool  SHOULD_GROW    = (0 == remSlots);
                        const Int64 GROWTH_REALLOC = TYPE_ALLOC && NOT_MOVABLE
                                                   ? oldSize
                                                   : 0;
                        const Int64 GROWTH_ALLOC   = SHOULD_GROW
                                                   ? 1 + GROWTH_REALLOC
                                                   : 0;

                        // Increase in total allocations:
                        const Int64 ALLOC_BLOCKS   = DD - CC;

                        ASSERTV(SPEC, N,
                                STRETCH_ALLOC,  GROWTH_ALLOC,   ALLOC_BLOCKS,
                                STRETCH_ALLOC + GROWTH_ALLOC == ALLOC_BLOCKS);

                        if (LENGTH == 0 && i == 1) {
                            // Very special case for first growth, with an
                            // extra (net) allocation for the initial buffer,
                            // and the capacity is set to 1 rather than 2*0.

                            ASSERTV(SPEC, N, i, D,  C, TYPE_ALLOC,
                                                D - C == (TYPE_ALLOC ? 2 : 1));
                            ASSERTV(SPEC, N, i, Y11.capacity(),
                                                Y11.capacity() == 1);
                        }
                        else {
                            // Retain more memory only if the new element
                            // allocates.

                            ASSERTV(SPEC, N, D,  C, TYPE_ALLOC,
                                                   (D - C != 0) == TYPE_ALLOC);

                            if (SHOULD_GROW) {
                                ASSERTV(SPEC, N, Y11.capacity(),   oldCap,
                                                 Y11.capacity() == oldCap * 2);
                            }
                            else {
                                ASSERTV(SPEC, N, Y11.capacity(),   oldCap,
                                                 Y11.capacity() == oldCap);
                            }
                        }

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append : ");
                            P_(i); P(Y11);
                        }

                        ASSERTV(SPEC, N, i, Y11.size() == LENGTH + i);
                        ASSERTV(SPEC, N, i, W != Y11);
                        ASSERTV(SPEC, N, i, X != Y11);
                        ASSERTV(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                }
#if defined(BDE_BUILD_TARGET_EXC)
                {   // Exception checking.

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                        const Obj Y2(X, xta);
                        if (veryVerbose) {
                            printf("\t\t\tException Case  :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        ASSERTV(SPEC, N, W == Y2);
                        ASSERTV(SPEC, N, W == X);
                        ASSERTV(SPEC, N,
                                Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    if (LENGTH == 0) {
                        ASSERTV(SPEC, N, BB + 0 == AA);
                        ASSERTV(SPEC, N, B  + 0 == A );
                    }
                    else {
                        const bsls::Types::Int64 TYPE_ALLOCS = TYPE_ALLOC *
                                          (LENGTH + LENGTH * (1 + LENGTH) / 2);
                        ASSERTV(SPEC, N, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                        ASSERTV(SPEC, N,         B  + 0               == A );
                    }
                }
#endif  // BDE_BUILD_TARGET_EXC
                {                            // with 'original' destroyed
                    Obj Y5(X);
                    if (veryVerbose) {
                        printf("\t\t\tWith Original deleted: \n");
                        printf("\t\t\t\tBefore Delete : "); P(Y5);
                    }

                    // Destroy 'X' and make final memory checks
                    proctorX.release();
                    xBuf.address()->~Obj();

                    ASSERTV(SPEC, N, W == Y5);

                    for (int i = 1; i < N+1; ++i) {
                        stretch(&Y5,
                                1,
                                TstFacility::getIdentifier(
                                                      VALUES[i % NUM_VALUES]));
                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append to new obj : ");
                            P_(i);P(Y5);
                        }
                        ASSERTV(SPEC, N, i, W != Y5);
                    }
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS
    // Concerns:
    //: 1 Objects constructed with the same values are returned as equal.
    //:
    //: 2 Objects constructed such that they have same (logical) value but
    //:   different internal representation (due to the lack or presence of an
    //:   allocator, and/or different capacities) are always returned as equal.
    //:
    //: 3 Unequal objects are always returned as unequal.
    //:
    //: 4 Correctly selects the 'bitwiseEqualityComparable' traits.
    //
    // Plan:
    //: 1 For concerns 1 and 3, Specify a set A of unique allocators including
    //:   no allocator.  Specify a set S of unique object values having various
    //:   minor or subtle differences, ordered by non-decreasing length.
    //:   Verify the correctness of 'operator==' and 'operator!=' (returning
    //:   either 'true' or 'false') using all elements (u, ua, v, va) of the
    //:   cross product S X A X S X A.
    //:
    //: 2 For concern 2 create two objects using all elements in S one at a
    //:   time.  For the second object change its internal representation by
    //:   extending it by different amounts in the set E, followed by erasing
    //:   its contents using 'clear'.  Then recreate the original value and
    //:   verify that the second object still return equal to the first.
    //:
    //: 3 For concern 4, we instantiate this test driver on a test type having
    //:   allocators or not, and possessing the bitwise-equality-comparable
    //:   trait or not.
    //
    // Testing:
    //   operator==(const vector<T,A>&, const vector<T,A>&);
    //   operator!=(const vector<T,A>&, const vector<T,A>&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta1(veryVeryVeryVerbose);
    bslma::TestAllocator ta2(veryVeryVeryVerbose);
    ALLOC                xta1(&ta1);
    ALLOC                xta2(&ta2);

    ALLOC ALLOCATOR[] = {
        xta1,
        xta2
    };

    enum { NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR };

    const TestValues VALUES;
    const size_t     NUM_VALUES = VALUES.size();

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
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    if (verbose) printf("\tCompare each pair without perturbation.\n");
    {

        int oldLen = -1;

        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
                // same lengths
                ASSERTV(si, ai, LENGTH == static_cast<int>(U.size()));

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n",
                                                                  LENGTH);
                    ASSERTV(U_SPEC, oldLen <= LENGTH);//non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) { T_; T_;
                    P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; sj < NUM_SPECS; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {

                        const char *const V_SPEC = SPECS[sj];
                        Obj               mV(ALLOCATOR[aj]);
                        const Obj&        V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            T_; T_; P_(sj); P_(V_SPEC); P(V);
                        }

                        const bool isSame = si == sj;
                        ASSERTV(si, sj,  isSame == (U == V));
                        ASSERTV(si, sj, !isSame == (U != V));
                    }
                }
            }
        }
    }

    if (verbose) printf("\tCompare each pair after perturbing.\n");
    {
        static const std::size_t EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;

        // Create first object
        for (int si = 0;  si < NUM_SPECS; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         U_LENGTH = static_cast<int>(strlen(U_SPEC));

                if (5 < U_LENGTH) {
                    continue;
                }

                Obj        mU(ALLOCATOR[ai]);
                const Obj& U = gg(&mU, U_SPEC);

                // same lengths
                ASSERTV(si, U_LENGTH == static_cast<int>(U.size()));

                if (U_LENGTH != oldLen) {
                    if (verbose)
                        printf("\tUsing lhs objects of length %d.\n",
                               U_LENGTH);

                    ASSERTV(U_SPEC, oldLen <= U_LENGTH);
                    oldLen = U_LENGTH;
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; sj < NUM_SPECS; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {
                        //Perform perturbation
                        for (int e = 0; e < NUM_EXTEND; ++e) {

                            const char *const V_SPEC = SPECS[sj];
                            const int         V_LENGTH =
                                              static_cast<int>(strlen(V_SPEC));

                            if (5 < V_LENGTH) {
                                continue;
                            }

                            Obj               mV(ALLOCATOR[aj]);
                            const Obj&        V = gg(&mV, V_SPEC);

                            stretchRemoveAll(&mV,
                                             EXTEND[e],
                                             TstFacility::getIdentifier(
                                                      VALUES[e % NUM_VALUES]));
                            gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                T_; T_; P_(sj); P_(V_SPEC); P(V);
                            }

                            const bool isSame = si == sj;
                            ASSERTV(si, sj,  isSame == (U == V));
                            ASSERTV(si, sj, !isSame == (U != V));
                        }
                    }
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase4a()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - operator[]
    //     - at
    //     - get_allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 The range '[cbegin(), cend())' contains inserted elements the sorted
    //:   order.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'get_allocator' method.
    //:
    //:     3 Verify the object contains the expected number of elements.
    //:
    //:     4 Use 'cbegin' and 'cend' to iterate through all elements and
    //:       verify the values are as expected.  (C-1..2, 4)
    //:
    //:     5 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   reference operator[](size_type pos);
    //   reference at(size_type pos);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    const TestValues    VALUES;

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
//------^
//line spec                                elements
//---- ----------------------------------- ----------------------------------
{ L_,  "",                                 ""                                },
{ L_,  "A",                                "A"                               },
{ L_,  "B",                                "B"                               },
{ L_,  "AB",                               "AB"                              },
{ L_,  "BC",                               "BC"                              },
{ L_,  "BCA",                              "BCA"                             },
{ L_,  "CAB",                              "CAB"                             },
{ L_,  "CDAB",                             "CDAB"                            },
{ L_,  "DABC",                             "DABC"                            },
{ L_,  "ABCDE",                            "ABCDE"                           },
{ L_,  "EDCBA",                            "EDCBA"                           },
{ L_,  "ABCDEA",                           "ABCDEA"                          },
{ L_,  "ABCDEAB",                          "ABCDEAB"                         },
{ L_,  "BACDEABC",                         "BACDEABC"                        },
{ L_,  "CBADEABCD",                        "CBADEABCD"                       },
{ L_,  "CBADEABCDAB",                      "CBADEABCDAB"                     },
{ L_,  "CBADEABCDABC",                     "CBADEABCDABC"                    },
{ L_,  "CBADEABCDABCDE",                   "CBADEABCDABCDE"                  },
{ L_,  "CBADEABCDABCDEA",                  "CBADEABCDABCDEA"                 },
{ L_,  "CBADEABCDABCDEAB",                 "CBADEABCDABCDEAB"                },
{ L_,  "CBADEABCDABCDEABCBADEABCDABCDEA",  "CBADEABCDABCDEABCBADEABCDABCDEA" },
{ L_,  "CBADEABCDABCDEABCBADEABCDABCDEAB", "CBADEABCDABCDEABCBADEABCDABCDEAB"}
//------v
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting non-'const' versions of operator[] and "
                        "function at() modify state of object correctly.\t");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) {T_ T_ P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj((bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj&                  mX = *objPtr;
                const Obj&            X  = gg(&mX, SPEC);
                bslma::TestAllocator& oa = *objAllocatorPtr;

                ASSERT(&oa == X.get_allocator());

                // Verify basic accessor

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                ASSERTV(ti, CONFIG, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf("\tOn objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    ASSERTV(LINE, CONFIG, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                {
                    Obj mY(&oa); const Obj& Y = mY;
                    Obj mZ(&oa); const Obj& Z = mZ;

                    mY.resize(LENGTH);
                    mZ.resize(LENGTH);

                    // Change state of Y and Z so its same as X

                    bslma::TestAllocator scratch;
                    for (size_t j = 0; j < LENGTH; j++) {
                        const TYPE&                   ELEM = EXP[j];
                        bsls::ObjectBuffer<ValueType> e1;
                        bsls::ObjectBuffer<ValueType> e2;
                        TstFacility::emplace(e1.address(),
                                             TstFacility::getIdentifier(ELEM),
                                             &scratch);
                        TstFacility::emplace(e2.address(),
                                             TstFacility::getIdentifier(ELEM),
                                             &scratch);
                        bslma::DestructorGuard<ValueType> guard1(
                                       bsls::Util::addressOf(e1.object()));
                        bslma::DestructorGuard<ValueType> guard2(
                                       bsls::Util::addressOf(e2.object()));
                        mY[j]    = bslmf::MovableRefUtil::move(e1.object());
                        mZ.at(j) = bslmf::MovableRefUtil::move(e2.object());
                    }

                    if (veryVerbose) {
                        printf("\t\tNew object1: "); P(Y);
                        printf("\t\tNew object2: "); P(Z);
                    }

                    ASSERTV(ti, CONFIG, Y == X);
                    ASSERTV(ti, CONFIG, Z == X);
                }

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                // TBD: This check is part of our standard template but fails
                //      in this test driver - there is a temporary created
                //      using the default allocator in 'resize' - we should
                //      revisit.
                // ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                //         0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }
#ifdef BDE_BUILD_TARGET_EXC

     if (verbose) printf("\tTesting for out_of_range exceptions thrown"
                         " by at() when pos >= size().\t");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { T_ T_ P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj *objPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj((bslma::Allocator *)0);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa1);
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj&                  mX  = *objPtr;
                const Obj&            X   = gg(&mX, SPEC);
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                          ? da
                                          : sa1;

                // Verify basic accessor

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                const int NUM_TRIALS = 2;

                // Check exception behavior for non-'const' version of 'at()'.
                // Checking the behavior for 'pos == size()' and
                // 'pos > size()'.

                int exceptions = 0;
                int trials;
                for (trials = 0; trials < NUM_TRIALS; ++trials)
                {
                    try {
                        mX.at(LENGTH + trials);
                    }
                    catch (const std::out_of_range&) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("\t\tIn out_of_range exception.\n");
                            T_ T_ P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);

                exceptions = 0;
                // Check exception behavior for const version of at()
                for (trials = 0; trials < NUM_TRIALS; ++trials)
                {
                    try {
                        X.at(LENGTH + trials);
                    } catch (const std::out_of_range&) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("\t\tIn out_of_range exception." );
                            T_ T_ P_(LINE); P(trials);
                        }
                        continue;
                    }
                }

                ASSERT(exceptions == trials);

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - operator[]
    //     - at
    //     - get_allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 The range '[cbegin(), cend())' contains inserted elements in the
    //:   correct order.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'get_allocator' method.
    //:
    //:     3 Verify the object contains the expected number of elements.
    //:
    //:     4 Use 'cbegin' and 'cend' to iterate through all elements and
    //:       verify the values are as expected.  (C-1..2, 4)
    //:
    //:     5 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   const_reference operator[](size_type pos) const;
    //   const_reference at(size_type pos) const;
    //   allocator_type  get_allocator() const;
    //   size_type       size() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
//------^
//line spec                                elements
//---- ----------------------------------- ----------------------------------
{ L_,  "",                                 ""                                },
{ L_,  "A",                                "A"                               },
{ L_,  "B",                                "B"                               },
{ L_,  "AB",                               "AB"                              },
{ L_,  "BC",                               "BC"                              },
{ L_,  "BCA",                              "BCA"                             },
{ L_,  "CAB",                              "CAB"                             },
{ L_,  "CDAB",                             "CDAB"                            },
{ L_,  "DABC",                             "DABC"                            },
{ L_,  "ABCDE",                            "ABCDE"                           },
{ L_,  "EDCBA",                            "EDCBA"                           },
{ L_,  "ABCDEA",                           "ABCDEA"                          },
{ L_,  "ABCDEAB",                          "ABCDEAB"                         },
{ L_,  "BACDEABC",                         "BACDEABC"                        },
{ L_,  "CBADEABCD",                        "CBADEABCD"                       },
{ L_,  "CBADEABCDAB",                      "CBADEABCDAB"                     },
{ L_,  "CBADEABCDABC",                     "CBADEABCDABC"                    },
{ L_,  "CBADEABCDABCDE",                   "CBADEABCDABCDE"                  },
{ L_,  "CBADEABCDABCDEA",                  "CBADEABCDABCDEA"                 },
{ L_,  "CBADEABCDABCDEAB",                 "CBADEABCDABCDEAB"                },
{ L_,  "CBADEABCDABCDEABCBADEABCDABCDEA",  "CBADEABCDABCDEABCBADEABCDABCDEA" },
{ L_,  "CBADEABCDABCDEABCBADEABCDABCDEAB", "CBADEABCDABCDEABCBADEABCDABCDEAB"}
//------v
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting const and non-'const' versions of "
                        "operator[] and function at() where pos < size().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { T_ T_ P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj((bslma::TestAllocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj&                  mX  = *objPtr;
                const Obj&            X   = gg(&mX, SPEC);
                bslma::TestAllocator& oa  = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                          ? da
                                          : sa1;

                // Verify basic accessor.

                ASSERT(&oa == X.get_allocator());

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());

                if (veryVerbose) {
                    printf( "\ton objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    ASSERTV(LINE, CONFIG, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                size_t i;
                for (i = 0; i < LENGTH; ++i) {
                    ASSERTV(LINE, CONFIG, i, EXP[i] == mX[i]);
                    ASSERTV(LINE, CONFIG, i, EXP[i] == X[i]);
                    ASSERTV(LINE, CONFIG, i, EXP[i] == mX.at(i));
                    ASSERTV(LINE, CONFIG, i, EXP[i] == X.at(i));
                }

                ASSERT(oam.isTotalSame());

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == i);

                {
                    // Check for perturbation.
                    static const std::size_t EXTEND[] = {
                        0, 1, 2, 3, 4, 5, 7, 8, 9, 15
                    };

                    enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

                    Obj        mY(&oa);
                    const Obj& Y = gg(&mY, SPEC);

                    // Perform the perturbation
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                        stretchRemoveAll(&mY,
                                         EXTEND[ei],
                                         TstFacility::getIdentifier(
                                                     VALUES[ei % NUM_VALUES]));
                        gg(&mY, SPEC);

                        if (veryVerbose) { T_; T_; T_; P(Y); }

                        size_t j;
                        for (j = 0; j < LENGTH; ++j) {
                            ASSERTV(LINE, CONFIG, j, ei, EXP[j] == mY[j]);
                            ASSERTV(LINE, CONFIG, j, ei, EXP[j] == Y[j]);
                            ASSERTV(LINE, CONFIG, j, ei, EXP[j] == mY.at(j));
                            ASSERTV(LINE, CONFIG, j, ei, EXP[j] == Y.at(j));
                        }
                    }
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions, we want to verify (1) that valid
    //   generator syntax produces expected results and (2) that invalid
    //   syntax is detected and reported.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'gg'
    //:   to set the state of a newly created object.  Verify that 'gg' returns
    //:   a valid reference to the modified argument object and, using basic
    //:   accessors, that the value of the object is as expected.  Repeat the
    //:   test for a longer 'spec' generated by prepending a string ending in a
    //:   '~' character (denoting 'clear').  Note that we are testing the
    //:   parser only; the primary manipulators are already assumed to work.
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.  Verify that 'ggg'
    //:   returns the expected value corresponding to the location of the first
    //:   invalid value of the 'spec'.  Repeat the test for a longer 'spec'
    //:   generated by prepending a string ending in a '~' character (denoting
    //:   'clear').  Note that we are testing the parser only; the primary
    //:   manipulators are already assumed to work.
    //
    // Testing:
    //   vector<T,A>& gg(vector<T,A> *object, const char *spec);
    //   int ggg(vector<T,A> *object, const char *spec, int vF = 1);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator oa(veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    if (verbose) printf("\tTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec             elements
            //----  --------------   ---------
            { L_,   "",              ""        },

            { L_,   "A",             "A"       },
            { L_,   "B",             "B"       },
            { L_,   "~",             ""        },

            { L_,   "CD",            "CD"      },
            { L_,   "E~",            ""        },
            { L_,   "~E",            "E"       },
            { L_,   "~~",            ""        },

            { L_,   "ABC",           "ABC"     },
            { L_,   "~BC",           "BC"      },
            { L_,   "A~C",           "C"       },
            { L_,   "AB~",           ""        },
            { L_,   "~~C",           "C"       },
            { L_,   "~B~",           ""        },
            { L_,   "A~~",           ""        },
            { L_,   "~~~",           ""        },

            { L_,   "ABCD",          "ABCD"    },
            { L_,   "~BCD",          "BCD"     },
            { L_,   "A~CD",          "CD"      },
            { L_,   "AB~D",          "D"       },
            { L_,   "ABC~",          ""        },

            { L_,   "ABCDE",         "ABCDE"   },
            { L_,   "~BCDE",         "BCDE"    },
            { L_,   "AB~DE",         "DE"      },
            { L_,   "ABCD~",         ""        },
            { L_,   "A~C~E",         "E"       },
            { L_,   "~B~D~",         ""        },

            { L_,   "~CBA~~ABCDE",   "ABCDE"   },

            { L_,   "ABCDE~CDEC~E",  "E"       }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj        mY(xoa);
            const Obj& Y = gg(&mY, buf);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(X);
                T_; T_; T_; P(Y);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERTV(0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(0 == verifyContainer(Y, EXP, LENGTH));
        }
    }

    if (verbose) printf("\tTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,   },  // control

            { L_,   "~",      -1,   },  // control
            { L_,   " ",       0,   },
            { L_,   ".",       0,   },
            { L_,   "E",      -1,   },  // control
            { L_,   "Z",       0,   },

            { L_,   "AE",     -1,   },  // control
            { L_,   "aE",      0,   },
            { L_,   "Ae",      1,   },
            { L_,   ".~",      0,   },
            { L_,   "~!",      1,   },
            { L_,   "  ",      0,   },

            { L_,   "ABC",    -1,   },  // control
            { L_,   " BC",     0,   },
            { L_,   "A C",     1,   },
            { L_,   "AB ",     2,   },
            { L_,   "?#:",     0,   },
            { L_,   "   ",     0,   },

            { L_,   "ABCDE",  -1,   },  // control
            { L_,   "aBCDE",   0,   },
            { L_,   "ABcDE",   2,   },
            { L_,   "ABCDe",   4,   },
            { L_,   "AbCdE",   1,   }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int          LINE  = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(xoa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase2a()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
    //   The basic concern of this test case is that the default constructor,
    //   the destructor, and, under normal conditions (i.e., no aliasing), the
    //   primary manipulators
    //      - push_back             (black-box)
    //      - clear                 (white-box)
    //   operate as expected.
    //
    // Concerns:
    //: 1 The default constructor
    //:
    //:   1 creates the correct initial value.
    //:
    //:   2 does *not* allocate memory.
    //:
    //:   3 has the internal memory management system hooked up properly so
    //:     that *all* internally allocated memory draws from the same
    //:     user-supplied allocator whenever one is specified.
    //:
    //: 2 The destructor properly deallocates all allocated memory to its
    //:   corresponding allocator from any attainable state.
    //:
    //: 3 'push_back'
    //:
    //:    1 produces the expected value.
    //:
    //:    2 increases capacity as needed.
    //:
    //:    3 maintains valid internal state.
    //:
    //:    4 is exception neutral with respect to memory allocation.
    //:
    //: 4 'clear'
    //:
    //:    1 produces the expected value (empty).
    //:
    //:    2 properly destroys each contained element value.
    //:
    //:    3 maintains valid internal state.
    //:
    //:    4 does not allocate memory.
    //:
    //: 5 The size based parameters of the class reflect the platform.
    //
    // Plan:
    //: 1 To address concerns 1.1 - 1.3, create an object using the default
    //:  constructor:
    //:   - With and without passing in an allocator.
    //:   - In the presence of exceptions during memory allocations using
    //:       a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:   - Where the object is constructed with an object allocator, and
    //:       neither of global and default allocator is used to supply memory.
    //:
    //: 2 To address concerns 3.1 - 3.3, construct a series of independent
    //:   objects, ordered by increasing length.  In each test, allow the
    //:   object to leave scope without further modification, so that the
    //:   destructor asserts internal object invariants appropriately.  After
    //:   the final insert operation in each test, use the (untested) basic
    //:   accessors to cross-check the value of the object and the
    //:   'bslma::TestAllocator' to confirm whether a resize has occurred.
    //:
    //: 3 To address concerns 4.1-4.3, construct a similar test, replacing
    //:   'push_back' with 'clear'; this time, however, use the test
    //:   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //:
    //: 4 To address concerns 2, 3.4, 4.4, create a small "area" test that
    //:   exercises the construction and destruction of objects of various
    //:   lengths and capacities in the presence of memory allocation
    //:   exceptions.  Two separate tests will be performed.
    //:
    //: 5 Let S be the sequence of integers { 0 .. N - 1 }.
    //:   1 for each i in S, use the default constructor and 'push_back' to
    //:     create an object of length i, confirm its value (using basic
    //:     accessors), and let it leave scope.
    //:   2 for each (i, j) in S X S, use 'push_back' to create an object of
    //:     length i, use 'clear' to clear its value and confirm (with
    //:     'length'), use insert to set the object to a value of length j,
    //:     verify the value, and allow the object to leave scope.
    //:
    //: 6 The first test acts as a "control" in that 'clear' is not called; if
    //:   only the second test produces an error, we know that 'clear' is to
    //:   blame.  We will rely on 'bslma::TestAllocator' and purify to address
    //:   concern 2, and on the object invariant assertions in the destructor
    //:   to address concerns 3.4 and 4.4.
    //:
    //: 7 To address concern 5, the values will be explicitly compared to the
    //:   expected values.  This will be done first so as to ensure all other
    //:   tests are reliable and may depend upon the class's constants.
    //
    // Testing:
    //   vector<T,A>();
    //   vector<T,A>(const A& a = A());
    //   ~vector<T,A>();
    //   void push_back(const T&);
    //   void clear();
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

    const int IS_NOT_MOVABLE = !(bslmf::IsBitwiseMoveable<TYPE>::value ||
                                                          s_typeIsMoveEnabled);
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
                    "\nTesting '%s' (TYPE_ALLOC = %d, IS_NOT_MOVABLE = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        IS_NOT_MOVABLE);

    if (verbose) printf("\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {
        const Obj X;
        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        const bsls::Types::Int64 AA = ta.numBlocksTotal();
        const bsls::Types::Int64 A  = ta.numBlocksInUse();

        const Obj X(xta);

        const bsls::Types::Int64 BB = ta.numBlocksTotal();
        const bsls::Types::Int64 B  = ta.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        ASSERT(AA + 0 == BB);
        ASSERT(A  + 0 == B);
    }

    if (verbose) printf("\t\tIn place using a buffer allocator.\n");
    {
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(xoa);

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 == oa.numBytesInUse());
    }

    // ------------------------------------------------------------------------

    if (verbose)
        printf("\tTesting 'push_back' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (veryVerbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tBEFORE: "); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            if (veryVerbose){
                printf("\t\t\tAFTER: "); P_(X.capacity()); P(X);
            }

            ASSERTV(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

            ASSERTV(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // ------------------------------------------------------------------------

    if (verbose)
        printf("\tTesting 'push_back' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (veryVerbose)
                    printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(xta);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const bsls::Types::Int64 BB = ta.numBlocksTotal();
            const bsls::Types::Int64 B  = ta.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            const bsls::Types::Int64 AA = ta.numBlocksTotal();
            const bsls::Types::Int64 A  = ta.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // Vector increases capacity only if the current length is zero or
            // a power of 2.  In addition, when the type allocates, an extra
            // allocation is used for the new element, and when the type is not
            // bitwise moveable, size() allocations are used during the move,
            // but an equal amount is destroyed thus the number of blocks in
            // use is unchanged.

            if (li == 0) {
                ASSERTV(li, BB + 1 + TYPE_ALLOC == AA);
                ASSERTV(li, B  + 1 + TYPE_ALLOC == A);
            }
            else if ((li & (li - 1)) == 0) {
                const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                        TYPE_ALLOC * (1 + li * IS_NOT_MOVABLE);
                ASSERTV(li, BB + 1 + TYPE_ALLOC_MOVES == AA);
                ASSERTV(li, B  + 0 + TYPE_ALLOC       == A);
            }
            else {
                ASSERTV(li, BB + 0 + TYPE_ALLOC == AA);
                ASSERTV(li, B  + 0 + TYPE_ALLOC == A);
            }

            ASSERTV(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }
            ASSERTV(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }


    if (verbose) printf("\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (veryVerbose)
                    printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            if (veryVerbose){
                printf("\t\t\tBEFORE ");
                P_(X.capacity()); P(X);
            }

            ASSERTV(li, li == X.size());

            mX.clear();

            if (veryVerbose){
                printf("\t\t\tAFTER ");
                P_(X.capacity()); P(X);
            }

            ASSERTV(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT ");
                P_(X.capacity()); P(X);
            }
        }
    }

    if (verbose) printf("\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (veryVerbose)
                  printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(xta);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const bsls::Types::Int64 BB = ta.numBlocksTotal();
            const bsls::Types::Int64 B  = ta.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B);
                typename Obj::size_type Cap = X.capacity();
                P_(Cap);P(X);
            }

            mX.clear();

            const bsls::Types::Int64 AA = ta.numBlocksTotal();
            const bsls::Types::Int64 A  = ta.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const bsls::Types::Int64 CC = ta.numBlocksTotal();
            const bsls::Types::Int64 C  = ta.numBlocksInUse();

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P_(X.capacity()); P(X);
            }

            ASSERTV(li, li == X.size());

            ASSERTV(li, BB                           == AA);
            ASSERTV(li, BB    + (int)li * TYPE_ALLOC == CC);

            ASSERTV(li, B - 0 - (int)li * TYPE_ALLOC == A);
            ASSERTV(li, B - 0                        == C);
        }
    }

    if (verbose) printf("\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an object
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the object to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (veryVerbose) printf(
                                 "\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {

            Obj mX(xta);  const Obj& X = mX;                              // 1.
            for (size_t i = 0; i < li; ++i) {                             // 2.
                ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                proctor.release();
            }

            ASSERTV(li, li == X.size());                                  // 3.
            for (size_t i = 0; i < li; ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                        // 4.
          ASSERTV(li, 0 == ta.numBlocksInUse());                          // 5.
        }
    }

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an object
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from object
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the object to leave scope
        //    9) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (veryVerbose)
                printf("\t\t\tOn an object of initial length " ZU ".\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVeryVerbose)
                    printf("\t\t\t\tAnd with final length " ZU ".\n", j);

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                size_t k; // loop index

                Obj mX(xta);  const Obj& X = mX;                          // 1.
                for (k = 0; k < i; ++k) {                                 // 2.
                    ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);
                    mX.push_back(VALUES[0]);
                    proctor.release();
                }

                ASSERTV(i, j, i == X.size());                             // 3.
                for (k = 0; k < i; ++k) {
                    ASSERTV(i, j, k, VALUES[0] == X[k]);
                }

                mX.clear();                                               // 4.
                ASSERTV(i, j, 0 == X.size());                             // 5.

                for (k = 0; k < j; ++k) {                                 // 6.
                    ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);
                    mX.push_back(VALUES[k % NUM_VALUES]);
                    proctor.release();
                }

                ASSERTV(i, j, j == X.size());                             // 7.
                for (k = 0; k < j; ++k) {
                    ASSERTV(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                    // 8.
              ASSERTV(i, 0 == ta.numBlocksInUse());                       // 9.
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
    //   The basic concern is that the default constructor, the destructor,
    //   and the primary manipulators:
    //      - push_back(T&&)
    //      - clear
    //   operate as expected.
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    //:10 'push_back' adds a new element to the end of the container, the new
    //:    element is move-inserted, and the order of the container remains
    //:    correct.
    //:
    //:11 'clear' properly destroys each contained element value.
    //:
    //:12 'clear' does not allocate memory.
    //:
    //:13 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'get_allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presence of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 13..14)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-11..12)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   vector();
    //   vector(const A& allocator);
    //   ~vector();
    //   void push_back(value_type&& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    const TestValues VALUES;  // contains 52 distinct increasing values
    const size_t     MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\tTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            ALLOC                xsa(&sa);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\t\tTesting default constructor.\n");
            }

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(ALLOC(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(xsa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;                                             // RETURN
              } break;
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            // ----------------------------------------------------------------

            if (veryVerbose)
                printf("\t\tTesting 'push_back' (bootstrap).\n");

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);

            if (0 < LENGTH) {
                if (veryVeryVerbose) {
                    printf("\t\tOn an object of initial length " ZU ".\n",
                           LENGTH);
                }

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj]);

                    primaryManipulator(&mX, id);

                    ASSERT(tj + 1 == X.size());
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj], X[tj],
                            VALUES[tj] == X[tj]);

                    for (size_t tk = 0; tk < tj; ++tk) {
                        ASSERTV(LENGTH, tj, CONFIG, VALUES[tk], X[tk],
                                VALUES[tk] == X[tk]);
                    }
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\t\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.clear();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
                ASSERTV(LENGTH, CONFIG, B, A,
                        B - (int)LENGTH * TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj]);

                    primaryManipulator(&mX, id);

                    ASSERT(tj + 1 == X.size());
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == X[tj]);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
            }

            // ----------------------------------------------------------------

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

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
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
    //   assignment for a non-empty object [11] and allow the result to
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
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    const TestValues    VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(&ta);  const Obj& X1 = mX1;
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if (veryVerbose){
        typename Obj::size_type capacity = X1.capacity();
        T_; T_;
        P(capacity);
    }

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1.\n");
    ASSERT(  X1 == X1 );
    ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");
    Obj mX2(X1, &ta);  const Obj& X2 = mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check the initial state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");
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

    Obj mX3(&ta);  const Obj& X3 = mX3;
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

    Obj mX4(X2, &ta);  const Obj& X4 = mX4;
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

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver1<TYPE, ALLOC>::testCaseM1Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // PERFORMANCE TEST (RANGE)
    //
    // Concerns:
    //: 1 That performance does not regress between versions.
    //:
    //: 2 That no surprising performance (both extremely fast or slow) is
    //:   detected, which might be indicating missed optimizations or
    //:   inadvertent loss of performance (e.g., by wrongly setting the
    //:   capacity and triggering too frequent reallocations).
    //:
    //: 3 That small "improvements" can be tested w.r.t. to performance, in a
    //:   uniform benchmark (e.g., measuring the overhead of allocating for
    //:   empty strings).
    //
    // Plan:
    //: 1 We follow a simple benchmark that performs the operation under
    //:   timing test in a loop.  Specifically, we wish to measure the time
    //:   taken by:
    //:
    //:   C1) The constructors.
    //:   A1) The 'assign' operation.
    //:   I1) The 'insert' operation at the end.
    //:   I2) The 'insert' operation at the front.
    //:   I3) The 'insert' operation everywhere.
    //:   E1) The 'erase' operation.
    // ------------------------------------------------------------------------

    bsls::Stopwatch t;

    // DATA INITIALIZATION (NOT TIMED)
    const TestValues  VALUES;
    const int         NUM_VALUES  = 5; // TBD: fix this

    const int         LENGTH      = 1000;
    const int         NUM_VECTOR  = 300;

    const char       *SPECREF     = "ABCDE";
    const size_t      SPECREF_LEN = strlen(SPECREF);
    char              SPEC[LENGTH + 1];

    for (int i = 0; i < LENGTH; ++i) {
        SPEC[i] = SPECREF[i % SPECREF_LEN];
    }
    SPEC[LENGTH] = '\0';

    Obj mZ;  const Obj& Z = gg(&mZ, SPEC);
    CONTAINER mU(Z); const CONTAINER& U = mU;

    void * addr = bslma::Default::defaultAllocator()->allocate(
                                       NUM_VECTOR * LENGTH * 2 * sizeof(TYPE));
    bslma::Default::defaultAllocator()->deallocate(addr);

    printf("\tC1) Constructor:\n");
    {
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

        // vector(f, l)
        double  time = 0.;
        Obj    *vectorBuffers = static_cast<Obj *>(
                                        sa.allocate(sizeof(Obj) * NUM_VECTOR));

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            new(&vectorBuffers[i]) Obj(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tvector(f,l):             %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectorBuffers[i].~Obj();
        }

        sa.deallocate(vectorBuffers);
    }

    printf("\tA1) Assign:\n");
    {
        // assign(f, l)
        double  time = 0.;
        Obj    *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->assign(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tassign(f,l):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI1) Insert (at front):\n");
    {
        // insert(p, f, l)
        double  time = 0.;
        Obj    *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->begin(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI2) Insert (at back):\n");
    {
        // insert(p, f, l)
        double  time = 0.;
        Obj    *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->end(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p, f, l)
        double  time = 0.;
        Obj    *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = 0; i < NUM_VECTOR; ++i) {
            // NOTE: Uses NUM_VECTOR for length.
            vectors[i] = new Obj(NUM_VECTOR, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->begin() + i, U.begin(), U.end());
        }
        time += t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::testCaseM1()
{
    // ------------------------------------------------------------------------
    // PERFORMANCE TEST
    //
    // We have the following concerns:
    //: 1 That performance does not regress between versions.
    //:
    //: 2 That no surprising performance (both extremely fast or slow) is
    //:   detected, which might be indicating missed optimizations or
    //:   inadvertent loss of performance (e.g., by wrongly setting the
    //:   capacity and triggering too frequent reallocations).
    //:
    //: 3 That small "improvements" can be tested w.r.t. to performance, in a
    //:   uniform benchmark (e.g., measuring the overhead of allocating for
    //:   empty strings).
    //
    // Plan:
    //: 1 We follow a simple benchmark that performs the operation under
    //:   timing test in a loop.  Specifically, we wish to measure the time
    //:   taken by:
    //:
    //:    C1) The various constructors.
    //:    C2) The copy constructor.
    //:    A1) The copy assignment.
    //:    A2) The 'assign' operations.
    //:    P1) The 'push_back' operation.
    //:    P2) The 'push_front' operation.
    //:    P3) The 'pop_back' operation.
    //:    P4) The 'pop_front' operation.
    //:    I1) The 'insert' operation in its various forms, at the front
    //:    I2) The 'insert' operation in its various forms, at the back
    //:    I3) The 'insert' operation in its various forms.
    //:    E1) The 'erase' operation in its various forms.
    // ------------------------------------------------------------------------

    bsls::Stopwatch t;

    // DATA INITIALIZATION (NOT TIMED)
    const TestValues VALUES;

    const int NUM_VALUES   = 5; // TBD: fix this
    const int LENGTH_S     = bsl::is_same<TYPE,char>::value ? 5000  : 1000;
    const int LENGTH_L     = bsl::is_same<TYPE,char>::value ? 20000 : 5000;
    const int NUM_VECTOR_S = bsl::is_same<TYPE,char>::value ? 500   : 100;
    const int NUM_VECTOR_L = bsl::is_same<TYPE,char>::value ? 5000  : 1000;

    // INITIAL ALLOCATION (NOT TIMED)
    void * addr = bslma::Default::defaultAllocator()->allocate(
                                   NUM_VECTOR_L * LENGTH_L * 2 * sizeof(TYPE));
    bslma::Default::defaultAllocator()->deallocate(addr);
    bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

    // C1) CONSTRUCTORS
    printf("\tC1) Constructors:\n");
    {
        // vector()
        double time = 0.;

        t.reset(); t.start();
        Obj *vectors = new Obj[NUM_VECTOR_L];
        time = t.elapsedTime();

        printf("\t\tvector():                %1.6fs\n", time);
        delete[] vectors;
    }

    Obj *vectorBuffers = static_cast<Obj *>(
                                      sa.allocate(sizeof(Obj) * NUM_VECTOR_L));

    {
        // vector(n)
        double time = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            new (&vectorBuffers[i]) Obj(LENGTH_S);
        }
        time = t.elapsedTime();

        printf("\t\tvector(n):               %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectorBuffers[i].~Obj();
        }
    }
    {
        // vector(n,v)
        double time = 0.;

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            new (&vectorBuffers[i]) Obj(LENGTH_S, VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tvector(n,v):             %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectorBuffers[i].~Obj();
        }
    }
    // C2) COPY CONSTRUCTOR
    printf("\tC2) Copy Constructors:\n");
    {
        // vector(vector)
        double time = 0.;
        Obj    original(LENGTH_L);

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            new (&vectorBuffers[i]) Obj(original);
        }
        time = t.elapsedTime();

        printf("\t\tvector(d):               %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectorBuffers[i].~Obj();
        }
    }

    sa.deallocate(vectorBuffers);

    // A1) COPY ASSIGNMENT
    printf("\tA1) Copy Assginment:\n");
    {
        // operator=(vector)
        double  time = 0;
        Obj     deq(LENGTH_L / 2);
        Obj    *vectors[NUM_VECTOR_L];

        ASSERT(LENGTH_L >= NUM_VECTOR_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_L) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            *vectors[i] = deq;
        }
        time = t.elapsedTime();

        printf("\t\toperator=(d):                   %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            ASSERT(*vectors[i] == deq);
            delete vectors[i];
        }
    }

    // A2) ASSIGN
    printf("\tA2) Assign:\n");
    {
        // assign(n)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_L];

        ASSERT(LENGTH_L >= NUM_VECTOR_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_L) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }
        const TYPE val    = TYPE();
        const int  length = LENGTH_L / 2;

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectors[i]->assign(length, val);
        }
        time = t.elapsedTime();

        printf("\t\tassign(n):                      %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            delete vectors[i];
        }
    }

    // P1) PUSH_BACK
    printf("\tP1) Push_back:\n");
    {
        // push_back(v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Push in 1000 objects.
            for (int j = 0; j < LENGTH_L; ++j) {
                vectors[i]->push_back(VALUES[j % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tpush_back(v):                   %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // P1) POP_BACK
    printf("\tP3) Pop_back:\n");
    {
        // pop_back()
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Pop out all objects.
            for (size_t j = 0; j < sizes[i]; ++j) {
                vectors[i]->pop_back();
            }
        }
        time = t.elapsedTime();

        printf("\t\tpop_back():                     %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I1) INSERT (FRONT)
    printf("\tI1) Insert (at front):\n");
//  {
//      // Takes too long...
//      // insert(p,v)
//      double  time = 0;
//      Obj    *vectors[NUM_VECTOR_S];
//
//      // Spread out the initial lengths.
//      for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
//               i < (LENGTH_S + NUM_VECTOR_S) / 2;
//               ++i, ++j) {
//          vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
//      }
//
//      t.reset(); t.start();
//      for (int i = 0; i < NUM_VECTOR_S; ++i) {
//          vectors[i]->insert(vectors[i]->begin(), VALUES[i % NUM_VALUES]);
//      }
//      time = t.elapsedTime();
//
//      printf("\t\tinsert(p,v):                    %1.6fs\n", time);
//      for (int i = 0; i < NUM_VECTOR_S; ++i) {
//          delete vectors[i];
//      }
//  }
    {
        // insert(n,p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i]->insert(vectors[i]->begin(), LENGTH_L,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I2) INSERT (BACK)
    printf("\tI2) Insert (at back):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (int j = 0; j < LENGTH_S; ++j) {
                vectors[i]->insert(vectors[i]->end(), VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
    {
        // insert(n,p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i]->insert(vectors[i]->end(), LENGTH_L,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I3) INSERT
    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j]  = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (size_t j = 0; j < sizes[i]; ++j) {
                vectors[i]->insert(vectors[i]->begin() + j,
                                   VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
    {
        // insert(n,p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Insert in middle.
            vectors[i]->insert(vectors[i]->begin() + sizes[i] / 2, LENGTH_S,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // E1) ERASE
    printf("\tE1) Erase:\n");
    {
        // erase(p)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i] = new Obj(LENGTH_S * 2, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (int j = 0; j < LENGTH_S; ++j) {
                vectors[i]->erase(vectors[i]->begin() + j);
            }
        }
        time = t.elapsedTime();

        printf("\t\terase(p):                       %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Matrix Type
///- - - - - - - - - - - - - - - - -
// Suppose we want to define a value-semantic type representing a dynamically
// resizable two-dimensional matrix.
//
// First, we define the public interface for the 'MyMatrix' class template:
//..
template <class TYPE>
class MyMatrix {
    // This value-semantic type characterizes a two-dimensional matrix of
    // objects of the (template parameter) 'TYPE'.  The numbers of columns and
    // rows of the matrix can be specified at construction and, at any time,
    // via the 'reset', 'insertRow', and 'insertColumn' methods.  The value of
    // each element in the matrix can be set and accessed using the 'theValue',
    // and 'theModifiableValue' methods respectively.  A free operator,
    // 'operator*', is available to return the product of two specified
    // matrices.

  public:
    // PUBLIC TYPES
//..
// Here, we create a type alias, 'RowType', for an instantiation of
// 'bsl::vector' to represent a row of 'TYPE' objects in the matrix.  We create
// another type alias, 'MatrixType', for an instantiation of 'bsl::vector' to
// represent the entire matrix of 'TYPE' objects as a list of rows:
//..
    typedef bsl::vector<TYPE>    RowType;
        // This is an alias representing a row of values of the (template
        // parameter) 'TYPE'.

    typedef bsl::vector<RowType> MatrixType;
        // This is an alias representing a two-dimensional matrix of values of
        // the (template parameter) 'TYPE'.

  private:
    // DATA
    MatrixType d_matrix;      // matrix of values
    int        d_numColumns;  // number of columns

    // FRIENDS
    template <class T>
    friend bool operator==(const MyMatrix<T>&, const MyMatrix<T>&);

  public:
    // PUBLIC TYPES
    typedef typename MatrixType::const_iterator ConstRowIterator;

    // CREATORS
    MyMatrix(int               numRows,
             int               numColumns,
             bslma::Allocator *basicAllocator = 0);
        // Create a 'MyMatrix' object having the specified 'numRows' and the
        // specified 'numColumns'.  All elements of the (template parameter)
        // 'TYPE' in the matrix will have the default-constructed value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numRows' and
        // '0 <= numColumns'

    MyMatrix(const MyMatrix&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create a 'MyMatrix' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~MyMatrix = default;
        // Destroy this object.

    // MANIPULATORS
    MyMatrix& operator=(const MyMatrix& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void clear();
        // Remove all rows and columns from this object.

    void insertColumn(int columnIndex);
        // Insert, into this matrix, an column at the specified 'columnIndex'.
        // All elements of the (template parameter) 'TYPE' in the column will
        // have the default constructed value.  The behavior is undefined
        // unless '0 <= columnIndex <= numColumns()'.

    void insertRow(int rowIndex);
        // Insert, into this matrix, an row at the specified 'rowIndex'.  All
        // elements of the (template parameter) 'TYPE' in the row will have the
        // default-constructed value.  The behavior is undefined unless
        // '0 <= rowIndex <= numRows()'.

    TYPE& theModifiableValue(int rowIndex, int columnIndex);
        // Return a reference providing modifiable access to the element at the
        // specified 'rowIndex' and the specified 'columnIndex' in this matrix.
        // The behavior is undefined unless '0 <= rowIndex < numRows()' and
        // '0 <= columnIndex < numColumns()'.

    // ACCESSORS
    int numRows() const;
        // Return the number of rows in this matrix.

    int numColumns() const;
        // Return the number of columns in this matrix.

    ConstRowIterator beginRow() const;
        // Return an iterator providing non-modifiable access to the 'RowType'
        // objects representing the first row in this matrix.

    ConstRowIterator endRow() const;
        // Return an iterator providing non-modifiable access to the 'RowType'
        // objects representing the past-the-end row in this matrix.

    const TYPE& theValue(int rowIndex, int columnIndex) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'rowIndex' and the specified 'columnIndex' in this
        // matrix.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()' and '0 <= columnIndex < numColumns()'.
};
//..
// Then we declare the free operator for 'MyMatrix':
//..
// FREE OPERATORS
template <class TYPE>
MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyMatrix' objects have the same
    // value if they have the same number of rows and columns and every element
    // in both matrices compare equal.

template <class TYPE>
MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MyMatrix' objects do not have
    // the same value if they do not have the same number of rows and columns
    // or every element in both matrices do not compare equal.
//..
// Now, we define the methods of 'MyMatrix':
//..
// CREATORS
template <class TYPE>
MyMatrix<TYPE>::MyMatrix(int numRows,
                         int numColumns,
                         bslma::Allocator *basicAllocator)
: d_matrix(numRows, basicAllocator)
, d_numColumns(numColumns)
{
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(0 <= numColumns);

    for (typename MatrixType::iterator itr = d_matrix.begin();
         itr != d_matrix.end();
         ++itr) {
        itr->resize(d_numColumns);
    }
}

template <class TYPE>
MyMatrix<TYPE>::MyMatrix(const MyMatrix& original,
                         bslma::Allocator *basicAllocator)
: d_matrix(original.d_matrix, basicAllocator)
, d_numColumns(original.d_numColumns)
{
}
//..
// Notice that we pass the contained 'bsl::vector' ('d_matrix') the allocator
// specified at construction to supply memory.  If the (template parameter)
// 'TYPE' of the elements has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
// this allocator will be passed by the vector to the elements as well.
//..
// MANIPULATORS
template <class TYPE>
MyMatrix<TYPE>& MyMatrix<TYPE>::operator=(const MyMatrix& rhs)
{
    d_matrix = rhs.d_matrix;
    d_numColumns = rhs.d_numColumns;
}

template <class TYPE>
void MyMatrix<TYPE>::clear()
{
    d_matrix.clear();
    d_numColumns = 0;
}

template <class TYPE>
void MyMatrix<TYPE>::insertColumn(int columnIndex)
{
    for (typename MatrixType::iterator itr = d_matrix.begin();
         itr != d_matrix.end();
         ++itr) {
        itr->insert(itr->begin() + columnIndex, TYPE());
    }
    ++d_numColumns;
}

template <class TYPE>
void MyMatrix<TYPE>::insertRow(int rowIndex)
{
    typename MatrixType::iterator itr =
        d_matrix.insert(d_matrix.begin() + rowIndex, RowType());
    itr->resize(d_numColumns);
}

template <class TYPE>
TYPE& MyMatrix<TYPE>::theModifiableValue(int rowIndex, int columnIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT((typename MatrixType::size_type) rowIndex < d_matrix.size());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(columnIndex < d_numColumns);

    return d_matrix[rowIndex][columnIndex];
}

// ACCESSORS
template <class TYPE>
int MyMatrix<TYPE>::numRows() const
{
    return d_matrix.size();
}

template <class TYPE>
int MyMatrix<TYPE>::numColumns() const
{
    return d_numColumns;
}

template <class TYPE>
typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::beginRow() const
{
    return d_matrix.begin();
}

template <class TYPE>
typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::endRow() const
{
    return d_matrix.end();
}

template <class TYPE>
const TYPE& MyMatrix<TYPE>::theValue(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT((typename MatrixType::size_type) rowIndex < d_matrix.size());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(columnIndex < d_numColumns);

    return d_matrix[rowIndex][columnIndex];
}
//..
// Finally, we defines the free operators for 'MyMatrix':
//..
// FREE OPERATORS
template <class TYPE>
MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs)
{
    return lhs.d_numColumns == rhs.d_numColumns &&
                                                  lhs.d_matrix == rhs.d_matrix;
}

template <class TYPE>
MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs)
{
    return !(lhs == rhs);
}
//..

}  // close unnamed namespace

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

    (void) veryVeryVeryVerbose; // Suppressing the "unused variable" warning

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    BSLMF_ASSERT(!bsl::Vector_IsRandomAccessIterator<int>::VALUE);
    BSLMF_ASSERT(bsl::Vector_IsRandomAccessIterator<
                                           bsl::vector<int>::iterator>::VALUE);
#endif

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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

        // Do some ad-hoc breathing test for the 'MyMatrix' type defined in the
        // usage example

        {
            bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

            // 1 2
            //
            // 3 4

            MyMatrix<int> m1(1, 1, &oa);
            m1.theModifiableValue(0, 0) = 4;
            m1.insertRow(0);
            m1.theModifiableValue(0, 0) = 2;
            m1.insertColumn(0);
            m1.theModifiableValue(0, 0) = 1;
            m1.theModifiableValue(1, 0) = 3;

            ASSERT(1 == m1.theValue(0, 0));
            ASSERT(2 == m1.theValue(0, 1));
            ASSERT(3 == m1.theValue(1, 0));
            ASSERT(4 == m1.theValue(1, 1));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-RELATED CONCERNS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATOR-RELATED CONCERNS"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase11,
                      bsltf::AllocTestType);
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
        // TESTING ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATOR"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver1,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase9,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        RUN_EACH_TYPE(MetaTestDriver1,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
                      );

        // Split additional cover into a separate macro invocation, to avoid
        // breaking the limit of 20 arguments to this macro.
        RUN_EACH_TYPE(MetaTestDriver1,
                      testCase8,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      NotAssignable,
                      BitwiseNotAssignable);

        // 'select_on_container_copy_construction' testing

        RUN_EACH_TYPE(TestDriver1,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase7,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY COMPARISION OPERATORS"
                            "\n======================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver1,
                      testCase6,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase6,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
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
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver1,
                      testCase4,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(TestDriver1,
                      testCase4a,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING TEST MACHINERY\n"
                            "======================\n");

        if (veryVerbose) printf("Verify sanity of 'DEFAULT_DATA'\n");

        {
            const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            bool foundMax = false;
            for (unsigned ii = 0; ii < NUM_DATA; ++ii) {
                size_t len = strlen(DATA[ii].d_spec);
                ASSERT(len <= DEFAULT_MAX_LENGTH);
                foundMax |= DEFAULT_MAX_LENGTH == len;

                for (unsigned jj = 0; jj < NUM_DATA; ++jj) {
                    ASSERT(ii == jj || strcmp(DATA[ii].d_spec, DATA[jj].d_spec));
                }
            }
            ASSERT(foundMax);
        }

        if (veryVerbose) printf("TESTING GENERATOR FUNCTIONS\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver1,
                      testCase3,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase3,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS (BOOTSTRAP)"
                            "\n========================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver1,
                      testCase2,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(TestDriver1,
                      testCase2a,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase2,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      testCase2a,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   We want to exercise basic value-semantic functionality.  This is
        //   achieved by the 'testCase1' class method of the test driver
        //   template, instantiated for a few basic test types.  See that
        //   function for a list of concerns and a test plan.  In addition, we
        //   want to make sure that we can use any standard-compliant
        //   allocator, including not necessarily rebound to the same type as
        //   the contained element, and that various manipulators and accessors
        //   work as expected in normal operation.
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nStandard value-semantic test.\n");

        RUN_EACH_TYPE(TestDriver1,
                      testCase1,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);

        // Disabled: in order to use bslstl_vector, we disabled this very
        // infrequent usage for vector (it will be flagged by 'BSLMF_ASSERT'):
        //..
        // vector<int, bsl::allocator<void*> > zz1, zz2(zza);
        //..

        if (verbose) printf("\nAdditional tests: misc.\n");

        vector<char> myVec(5, 'a');
        vector<char>::const_iterator citer;
        ASSERT(5 == myVec.size());
        for (citer = myVec.begin(); citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.insert(myVec.begin(), 'z');
        ASSERT(6 == myVec.size());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.erase(myVec.begin() + 2, myVec.begin() + 4);
        ASSERT(4 == myVec.size());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        vector<vector<char> > vv;
        vv.push_back(myVec);
        if (verbose) P(myVec);

        if (verbose) printf("\nAdditional tests: traits.\n");

        ASSERT((bslmf::IsBitwiseMoveable<vector<char> >::value));
        ASSERT(
             (bslmf::IsBitwiseMoveable<vector<bsltf::AllocTestType> >::value));
        ASSERT((bslmf::IsBitwiseMoveable<vector<vector<int> > >::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            // Check primary template
            ASSERT((0 == []() -> bsl::vector<int> { return {}; }().size()));
            ASSERT((1 == []() -> bsl::vector<int> { return {1}; }().size()));
            ASSERT((2 == []() -> bsl::vector<int> { return {0,0}; }().size()));
            ASSERT((3 == []() -> bsl::vector<int> {
                return {3, 1, 3};
            }().size()));

            // Check again for pointer specializations
            ASSERT((0 ==
                []() -> bsl::vector<int(*)()> { return {}; }().size()));
            ASSERT((1 ==
                []() -> bsl::vector<int(*)()> { return {0}; }().size()));
            ASSERT((2 ==
                []() -> bsl::vector<int(*)()> { return {0, 0}; }().size()));
            ASSERT((3 ==
                []() -> bsl::vector<int(*)()> { return {0, 0, 0}; }().size()));

            ASSERT((0 ==
              []() -> bsl::vector<const int*> { return {}; }().size()));
            ASSERT((1 ==
              []() -> bsl::vector<const int*> { return {0}; }().size()));
            ASSERT((2 ==
              []() -> bsl::vector<const int*> { return {0, 0}; }().size()));
            ASSERT((3 ==
              []() -> bsl::vector<const int*> { return {0, 0, 0}; }().size()));
        }
#endif
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //: 1 Provide benchmark for subsequent improvements to 'bslstl_vector'.
        //:   The benchmark should measure the speed of various operations such
        //:   as 'push_back', 'pop_back', 'erase', 'insert',
        //:   etc.
        //
        // Plan:
        //: 1 Using 'bsls_stopwatch', the run time of the various methods under
        //:   test be tallied over various iterations.  These values should
        //:   only be used as a comparison across various versions.  They are
        //:   *not* meant to be used to compare which method runs faster since
        //:   the loops they run in have various lengths.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        if (verbose) printf("\n... with 'char' type.\n");
        TestDriver1<char>::testCaseM1();

        if (verbose) printf("\n... with 'AllocTestType'.\n");
        TestDriver1<bsltf::AllocTestType>::testCaseM1();

        if (verbose) printf("\n... with 'SimpleTest'.\n");
        TestDriver1<bsltf::SimpleTestType>::testCaseM1();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver1<bsltf::BitwiseMoveableTestType>::testCaseM1();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' .\n");
        TestDriver1<bsltf::BitwiseCopyableTestType>::testCaseM1();

        if (verbose) printf("\nPERFORMANCE TEST RANGE"
                            "\n======================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver1<bsltf::AllocTestType>::testCaseM1Range(
                                             ListLike<bsltf::AllocTestType>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver1<bsltf::AllocTestType>::testCaseM1Range(
                                            ArrayLike<bsltf::AllocTestType>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver1<bsltf::BitwiseMoveableTestType>::testCaseM1Range(
                                   ListLike<bsltf::BitwiseMoveableTestType>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver1<bsltf::BitwiseMoveableTestType>::testCaseM1Range(
                                  ArrayLike<bsltf::BitwiseMoveableTestType>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver1<bsltf::BitwiseCopyableTestType>::testCaseM1Range(
                                   ListLike<bsltf::BitwiseCopyableTestType>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver1<bsltf::BitwiseCopyableTestType>::testCaseM1Range(
                                  ArrayLike<bsltf::BitwiseCopyableTestType>());

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(defaultAllocator.numBlocksInUse(),
            0 == defaultAllocator.numBlocksInUse());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
