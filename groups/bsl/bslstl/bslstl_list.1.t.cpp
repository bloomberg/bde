// bslstl_list.1.t.cpp                                                -*-C++-*-
#define BSLSTL_LIST_0T_AS_INCLUDE
#include <bslstl_list.0.t.cpp>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_list.0.t.cpp'.

//=============================================================================
//                       TEST DRIVER TEMPLATE
//=============================================================================

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver1 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_LIST_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

                            // TEST CASES
    // CLASS METHODS
    static void test01_breathingTest();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void test02_primaryManipulators();
        // Test primary manipulators ('push_back' and 'clear').

    static void test03_generatorGG();
        // Test generator functions 'ggg' and 'gg'.

    static void test04_basicAccessors();
        // Test basic accessors ('size', 'begin' and 'end').

    static void test06_equalityOp();
        // Test equality operator ('operator==').

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void test07_select_on_container_copy_construction_dispatch();
    static void test07_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void test07_copyCtor();
        // Test copy constructor.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void test09_propagate_on_container_copy_assignment_dispatch();
    static void test09_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void test09_copyAssignmentOp();
        // Test assignment operator ('operator=').

    static void test11_allocators();
    template <class LHS_TYPE, class RHS_TYPE>
    static void test11_allocators(LHS_TYPE, RHS_TYPE);
    static void test11_allocators(bsl::true_type, bsl::true_type);
        // The first two-argument overload is a no-op.  The second two-argument
        // overload is called only when both 'Obj' and 'TYPE' use
        // 'bslma::Allocator'.  The first overload, with no arguments,
        // dispatches to the other two overloads.
};

                  // ==================================
                  // template class StdBslmaTestDriver1
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver1 : public StdBslmaTestDriverHelper<TestDriver1, TYPE>
{
};

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
template <class LHS_TYPE, class RHS_TYPE>
void TestDriver1<TYPE,ALLOC>::test11_allocators(LHS_TYPE, RHS_TYPE)
{
    BSLMF_ASSERT(!ObjHasBslmaAlloc::value || !TypeUsesBslmaAlloc::value);

    if (verbose) printf("testAllocators: not testing\n");

    // no-op
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test11_allocators(bsl::true_type, bsl::true_type)
{
    // ------------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // Concerns:
    //: o For ALLOC that is a bslma_Allocator
    //:   1 The list class has the 'bslma::UsesBslmaAllocator' trait.
    //:   2 The allocator is not copied when the list is copy-constructed.
    //:   3 The allocator is set with the extended copy-constructor.
    //:   4 The allocator is passed through to elements if the elements also
    //:     use bslma_Allocator.
    //:   5 Creating an empty list allocates exactly one block.
    //
    // Plan:
    //: 1 Default construct a list and observe that a single allocation takes
    //:   place (for the sentinel).
    //:
    //: 2 If the type contained in the list allocates, push a value onto the
    //:   back of the list and observe that 2 more allocations take place.
    //:
    //: 3 Call the 'allocator' accessor on an element in the list and observe
    //:   that the allocator matches that passed to the list.
    //
    // Testing:
    //   Allocator traits and propagation
    // ------------------------------------------------------------------------

    if (verbose) printf("testAllocators: testing\n");

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT(ObjHasBslmaAlloc::value);
    BSLMF_ASSERT(TypeUsesBslmaAlloc::value);

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    if (verbose)
        printf("Testing 'bslma::UsesBslmaAllocator'.\n");

    if (verbose)
        printf("Testing that empty list allocates one block.\n");
    {
        const Int64 BB = oa.numBlocksTotal();
        Obj         mX(&oa);

        ASSERTV(BB + 1 == oa.numBlocksTotal());
        ASSERTV(1 == oa.numBlocksInUse());
    }

    if (verbose)
        printf("Testing allocator propagation for copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(&oa); const Obj& X = mX;
        Obj mY(X);   const Obj& Y = mY;

        // Allocator not copied
        ASSERTV(&oa == X.get_allocator());
        ASSERTV(&oa != Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

        Obj mZ(X,&oa2); const Obj& Z = mZ;

        ASSERTV(&oa2 == Z.get_allocator());
    }

    if (verbose)
        printf("Testing passing allocator through to elements.\n");

    TestValues VALUES("A");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        {
            Obj mX(1, VALUES[0], &oa); const Obj& X = mX;

            ASSERTV(&oa == X.back().allocator());
            ASSERTV(3 == oa.numBlocksInUse());
        }
        {
            Obj mX(&oa);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            ASSERTV(&oa == X.back().allocator());
            ASSERTV(3 == oa.numBlocksInUse());
        }
    }
    else
    {
        {
            Obj mX(1, VALUES[0], &oa); const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                     dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(&oa != elemAlloc);
            ASSERTV(2 == oa.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(&oa);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(&oa != elemAlloc);
            ASSERTV(2 == oa.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    ASSERTV(0 == oa.numBytesInUse());
    ASSERTV(DD == OtherAllocatorDefaultImp.numBytesInUse());
}

#if 0
// TBD: this is meant to test objects using allocators other than bslma
// ones.  Perhaps this code can be resurrected once we have updated bsltf
// appropriately, though probably not.

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test11_allocators(bsl::false_type)
{
    // ------------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS FOR NON-BSLMA ALLOCATORS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //   1. The list class does not have the
    //      'bslma::UsesBslmaAllocator' trait.
    //   2. The allocator is not passed through to elements
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is copied when the list is copy-constructed.
    //   5. Creating an empty list allocates exactly one block.
    //   6. 'get_allocator' returns the allocator used to construct the
    //      list object.
    //
    // Testing:
    //   allocator_type get_allocator() const;
    //   Allocator traits
    //   Allocator propagation
    //   Allocator traits and propagation
    // ------------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT( !ObjHasBslmaAlloc::value );

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    OtherAllocator<TYPE> objAllocator(&testAllocator);

    TestValues VALUES("ABCDEFGH");

    if (verbose)
        printf("Testing 'bslma::UsesBslmaAllocator'.\n");

    ASSERTV(! bslma::UsesBslmaAllocator<Obj>::value);

    if (verbose)
        printf("Testing that empty list allocates one block.\n");
    {
        const Int64 BB = testAllocator.numBlocksTotal();
        Obj         mX(objAllocator);
        ASSERTV(BB + 1 == testAllocator.numBlocksTotal());
        ASSERTV(1 == testAllocator.numBlocksInUse());
    }

    if (verbose)
        printf("Testing that allocator propagation for "
               "copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(objAllocator);    const Obj& X = mX;
        Obj mY(X);               const Obj& Y = mY;

        // Allocator copied
        ASSERTV(objAllocator == X.get_allocator());
        ASSERTV(objAllocator == Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator a2;
        OtherAllocator<char> objAlloc2(&a2);

        Obj mZ(X,objAlloc2);     const Obj& Z = mZ;

        // Allocator set to objAlloc2 (not copied)
        ASSERTV(objAllocator   != Z.get_allocator());
        ASSERTV(objAlloc2      == Z.get_allocator());
    }

    if (verbose)
       printf("Testing that allocator is not passed through to elements.\n");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        // Elements in container should use default allocator while the
        // container itself uses 'testAllocator'.  Set the default allocator
        // here so that we can track its use.
        bslma::TestAllocator         defAlloc(veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defAlloc);

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&defAlloc == elemAlloc);
            ASSERTV(1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&defAlloc == elemAlloc);
            ASSERTV(1 == elemAlloc->numBlocksInUse());
        }

        ASSERTV(0 == defAlloc.numBlocksInUse());
    }
    else
    {
        // Elements in container should use 'OtherAllocator::d_defaultImp'
        // while the container itself uses 'testAllocator'.

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;

            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            ASSERTV(objAllocator == X.get_allocator());
            ASSERTV(&testAllocator != elemAlloc);
            ASSERTV(2 == testAllocator.numBlocksInUse());
            ASSERTV(&OtherAllocatorDefaultImp == elemAlloc);
            ASSERTV(DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    ASSERTV(0 == testAllocator.numBytesInUse());
    ASSERTV(DD == OtherAllocatorDefaultImp.numBytesInUse());
}
#endif

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test11_allocators()
{
    if (verbose) {
        printf("testAllocators: Obj has bslma: %d, TYPE uses bslma: %d\n",
                           ObjHasBslmaAlloc::value, TypeUsesBslmaAlloc::value);
    }

    // Dispatch to the appropriate function
    test11_allocators(ObjHasBslmaAlloc(), TypeUsesBslmaAlloc());
}

template <class TYPE, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver1<TYPE, ALLOC>::
                       test09_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   TYPE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS> StdAlloc;

    typedef bsl::list<TYPE, StdAlloc>           Obj;

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
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);

        StdAlloc scratch(&da);

        const Obj W(IVALUES.begin(), IVALUES.end(), scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj        mY(IVALUES.begin(), IVALUES.end(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj        mX(JVALUES.begin(), JVALUES.end(), mat);
                const Obj& X = mX;

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

template <class TYPE, class ALLOC>
void TestDriver1<TYPE, ALLOC>::test09_propagate_on_container_copy_assignment()
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

    if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == false'\n");

    test09_propagate_on_container_copy_assignment_dispatch<false, false>();
    test09_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == true'\n");

    test09_propagate_on_container_copy_assignment_dispatch<true, false>();
    test09_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test09_copyAssignmentOp()
{
    // ------------------------------------------------------------------------
    // TESTING COPY ASSIGNMENT OPERATOR
    //
    // Concerns:
    //: 1 The 'rhs' value must not be affected by the operation.
    //:
    //: 2 'rhs' going out of scope has no effect on the value of 'lhs' after
    //:   the assignment.
    //:
    //: 3 After the assignment, no element of the 'lhs' has the same address an
    //:   element of 'rhs' (except in the case of self-assignment).
    //:
    //: 4 Aliasing (x = x): The assignment operator must always work -- even
    //:   when the lhs and rhs are identically the same object.
    //:
    //: 5 The assignment operator must be neutral with respect to memory
    //:   allocation exceptions.
    //:
    //: 6 The 'lhs' object must allocate all of its internal memory from its
    //:   own allocator, even of 'rhs' uses a different allocator.
    //:
    //: 7 TBD: C++0x move assignment moves the value but not the allocator.
    //:   Memory is allocated only if the 'lhs' and 'rhs' allocators are
    //:   different.
    //
    // Plan:
    //: 1 Specify a set S of unique object values with substantial and varied
    //:   differences, ordered by increasing length.  For each value in S,
    //:   construct an object x along with a sequence of similarly constructed
    //:   duplicates x1, x2, ..., xN.  The elements within each object in S are
    //:   unique so that re-ordering elements cannot preserve equality.
    //:   Attempt to affect every aspect of white-box state by altering each xi
    //:   in a unique way.  Let the union of all such objects be the set T.
    //:
    //: 2 To address concerns 2, 3, and 5, construct tests u = v for all
    //:   '(u, v)' in T X T.  Using canonical controls 'UU' and 'VV', assert
    //:   before the assignment that 'UU == u', 'VV == v', and 'v == u' if and
    //:   only if 'VV == UU'.  After the assignment, assert that 'VV == u',
    //:   'VV == v', and, for grins, that 'v == u'.  Verify that each element
    //:   in v has a different address from the corresponding element in v.
    //:   Let v go out of scope and confirm that 'VV == u'.  All of these tests
    //:   are performed within the 'bslma' exception testing apparatus.  Since
    //:   the execution time is lengthy with exceptions, not every permutation
    //:   is performed when exceptions are tested.  Every permutation is also
    //:   tested separately without exceptions.
    //:
    //: 3 As a separate exercise, we address 4 and 5 by constructing tests
    //:   y = y for all y in T.  Using a canonical control X, we will verify
    //:   that X == y before and after the assignment, again within the bslma
    //:   exception testing apparatus.
    //:
    //: 4 To address concern 5, all these tests are performed on user defined
    //:   types:
    //:   o With allocator, copyable
    //:   o With allocator, movable
    //:   o With allocator, not movable
    //
    // Testing:
    //   list& operator=(const list& rhs);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
    bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);
    ALLOC                xoa1(&oa1);
    ALLOC                xoa2(&oa2);

    int numThrows;

    // Set 'assignOfTypeAllocates' to indicate whether
    // 'TYPE& operator=(const TYPE&)' allocates.

    bool assignOfTypeAllocates;
    {

        Obj mX(xoa1);
        ASSERT(0 > ggg(&mX, "AB"));

        typename Obj::iterator itFront = mX.begin(), itBack = itFront;
        ++itBack;
        ASSERT(mX.end() != itBack && itFront != itBack);
        const Int64 nbt = oa1.numBlocksTotal();
        *itFront = *itBack;
        assignOfTypeAllocates = oa1.numBlocksTotal() > nbt;
    }

    if (verbose)
        printf("Testing 'operator=' with TYPE: %s, assignAllocates: %d\n",
               bsls::NameOf<TYPE>().name(), assignOfTypeAllocates);

    // ------------------------------------------------------------------------

    if (veryVerbose) printf("Assign cross product of values "
                            "with varied representations.\n"
                            "Without Exceptions\n");
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEA",       // Try equal-size assignment of different values
            "DEAB",
            "BACEDEDC",
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        {
            for (int ui = 0; ui < NUM_SPECS; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = std::strlen(U_SPEC);

                if (veryVerbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                Obj mUU;  const Obj& UU = gg(&mUU, U_SPEC);  // control
                ASSERTV(ui, uLen == UU.size());              // same lengths

                for (int vi = 0; vi < NUM_SPECS; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = std::strlen(V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    Obj mVV;  const Obj& VV = gg(&mVV, V_SPEC); // control

                    const bool EXP = (ui == vi); // flag indicating same values

                    {
                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                        Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int    NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int    NUM_DTOR_BEFORE = numDestructorCalls;
                        const size_t U_LEN_BEFORE = U.size();

                        const Int64 BB1 = oa1.numBlocksTotal();
                        const Int64 B1  = oa1.numBlocksInUse();
                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        mU = V; // test assignment here

                        const Int64 AA1 = oa1.numBlocksTotal();
                        const Int64 A1  = oa1.numBlocksInUse();
                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size() objects,
                        // but could (through the use of element-by-element
                        // assignment) construct and destroy fewer elements.

                        const int NUM_CTOR =
                                          numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                                          numDestructorCalls - NUM_DTOR_BEFORE;
                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, VV  == U);
                        ASSERTV(U_SPEC, V_SPEC, VV  == V);
                        ASSERTV(U_SPEC, V_SPEC, V   == U);
                        ASSERTV(U_SPEC, V_SPEC, BB1 == AA1);
                        ASSERTV(U_SPEC, V_SPEC, B1  == A1 );
                        ASSERTV(U_SPEC, V_SPEC,
                                               BB2 + deltaBlocks(vLen) >= AA2);

                        ptrdiff_t difference =
                                           static_cast<ptrdiff_t>(vLen - uLen);

                        ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                        for (const_iterator iu = U.begin(), iv = V.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                                  bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }

                    // Try move assign with different allocators, should be
                    // the same.

                    {
                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                        Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int    NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int    NUM_DTOR_BEFORE = numDestructorCalls;
                        const size_t U_LEN_BEFORE    = U.size();

                        const Int64 BB1 = oa1.numBlocksTotal();
                        const Int64 B1  = oa1.numBlocksInUse();
                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, UU == U);

                        mU = MoveUtil::move(mV); // test assignment here

                        const Int64 AA1 = oa1.numBlocksTotal();
                        const Int64 A1  = oa1.numBlocksInUse();
                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size() objects,
                        // but could (through the use of element-by-element
                        // assignment) construct and destroy fewer elements.

                        const int NUM_CTOR =
                            numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                            numDestructorCalls - NUM_DTOR_BEFORE;

                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, VV == U);
                        ASSERTV(U_SPEC, V_SPEC, BB1 == AA1);
                        ASSERTV(U_SPEC, V_SPEC, B1, A1, B1 >= A1 );
                        ASSERTV(U_SPEC, V_SPEC,
                                               BB2 + deltaBlocks(vLen) >= AA2);

                        ptrdiff_t difference =
                                           static_cast<ptrdiff_t>(vLen - uLen);

                        ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                        for (const_iterator iu = U.begin(), iv = V.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                                  bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }

                    // Try move assign with the same allocator, should be just
                    // a swap.

                    {
                        Int64 BBB2 = oa2.numBlocksTotal();

                        Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);

                        ASSERT(oa2.numBlocksTotal() > BBB2);
                        BBB2 = oa2.numBlocksTotal();

                        Obj mV(xoa2); const Obj& V = gg(&mV, V_SPEC);

                        ASSERT(oa2.numBlocksTotal() > BBB2);

                        if (veryVeryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        ASSERTV(U_SPEC, V_SPEC, UU == U);
                        ASSERTV(U_SPEC, V_SPEC, VV == V);
                        ASSERTV(U_SPEC, V_SPEC, EXP == (V==U));

                        const int NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int NUM_DTOR_BEFORE = numDestructorCalls;

                        typename Obj::iterator itU = mU.begin();
                        typename Obj::iterator itV = mV.begin();

                        const Int64 BB2 = oa2.numBlocksTotal();
                        const Int64 B2  = oa2.numBlocksInUse();

                        mU = MoveUtil::move(mV); // test assignment here

                        const Int64 AA2 = oa2.numBlocksTotal();
                        const Int64 A2  = oa2.numBlocksInUse();

                        ASSERT(NUM_CTOR_BEFORE == numCopyCtorCalls);
                        ASSERT(NUM_DTOR_BEFORE == numDestructorCalls);

                        ASSERT(mU.begin() == itV);
                        ASSERT(mV.begin() == itU);

                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        ASSERTV(U_SPEC, V_SPEC, checkIntegrity(V, uLen));
                        ASSERTV(U_SPEC, V_SPEC, VV  == U);
                        ASSERTV(U_SPEC, V_SPEC, UU  == V);
                        ASSERTV(U_SPEC, V_SPEC, BB2 == AA2);
                        ASSERTV(U_SPEC, V_SPEC, B2  == A2 );

                        for (const_iterator iu = U.begin(), iv = VV.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                    bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }

                        for (const_iterator iu = UU.begin(), iv = V.begin();
                             iu != UU.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            ASSERTV(U_SPEC, V_SPEC, *iu,
                                    bsls::Util::addressOf(*iv) !=
                                                   bsls::Util::addressOf(*iu));
                        }
                    }
                } // end for (vi)
            } // end for (ui)
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (veryVerbose)
        printf("Assign cross product of values With Exceptions\n");

    {
        static const char *SPECS[] = { // len: 0-2, 4, 4, 9,
            "",
            "A",
            "BC",
            "DEAB",
            "CBEA",
            "BACEDEDCB"
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        for (int ui = 0; ui < NUM_SPECS; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = std::strlen(U_SPEC);

            if (veryVerbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            Obj mUU; const Obj& UU = gg(&mUU, U_SPEC);  // control

            ASSERTV(ui, uLen == UU.size());             // same lengths

            // int vOldLen = -1;
            for (int vi = 0; vi < NUM_SPECS; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = std::strlen(V_SPEC);

                if (veryVeryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                // control
                Obj mVV; const Obj& VV = gg(&mVV, V_SPEC);

                // Exception-test macros must use 'oa2':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    oa2.setAllocationLimit(AL);
                    mU = V; // test assignment here

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                    ASSERTV(U_SPEC, V_SPEC, VV == V);
                    ASSERTV(U_SPEC, V_SPEC, V  == U);
                    ASSERTV(U_SPEC, V_SPEC, BB2 + deltaBlocks(vLen) >= AA2);

                    ptrdiff_t difference = static_cast<ptrdiff_t>(vLen-uLen);

                    ASSERTV(U_SPEC, V_SPEC,B2 + deltaBlocks(difference) == A2);

                    ASSERTV(numThrows,
                            U_SPEC,
                            V_SPEC,
                            assignOfTypeAllocates,
                            bsls::NameOf<TYPE>(),
                            k_NO_EXCEPT || (numThrows > 0) ==
                                (VV.size() > UU.size() ||
                                      (!VV.empty() && assignOfTypeAllocates)));

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // now test move assign where operands use different
                // allocators, should turn out just like normal copy-assign

                // Exception-test macros must use 'oa2':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa1); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, UU == U);
                    ASSERTV(U_SPEC, V_SPEC, VV == V);

                    oa2.setAllocationLimit(AL);
                    mU = MoveUtil::move(mV);       // test move assignment here

                    ASSERTV(k_NO_EXCEPT || (0 < numThrows) ==
                                (UU.size() < VV.size()
                             || (!VV.empty() && k_SCOPED_ALLOC)));

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                    ASSERTV(U_SPEC, V_SPEC, BB2 + deltaBlocks(vLen) >= AA2);

                    ptrdiff_t difference = static_cast<ptrdiff_t>(vLen-uLen);

                    ASSERTV(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(U_SPEC, V_SPEC, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // now test move assign where operands use the same allocator.
                // Should swap without allocating.

                // Exception-test macros must use 'testAllocator':
                numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa2) {
                    ++numThrows;

                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off the
                    // limit until we're ready to test assignment.

                    const Int64 AL = oa2.allocationLimit();
                    oa2.setAllocationLimit(-1);

                    Obj mU(xoa2); const Obj& U = gg(&mU, U_SPEC);
                    Obj mV(xoa2); const Obj& V = gg(&mV, V_SPEC);

                    if (veryVeryVerbose) {
                        printf("\t| "); P_(U); P(V);
                    }

                    const Int64 BB2 = oa2.numBlocksTotal();
                    const Int64 B2  = oa2.numBlocksInUse();

                    oa2.setAllocationLimit(AL);

                    mU = MoveUtil::move(mV);     // test move assignment here,
                                                 // which should be just a swap

                    ASSERT(0 == numThrows);

                    const Int64 AA2 = oa2.numBlocksTotal();
                    const Int64 A2  = oa2.numBlocksInUse();

                    ASSERTV(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                    ASSERTV(U_SPEC, V_SPEC, VV  == U);
                    ASSERTV(U_SPEC, V_SPEC, UU  == V);
                    ASSERTV(U_SPEC, V_SPEC, BB2 == AA2);
                    ASSERTV(U_SPEC, V_SPEC, B2  == A2);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (vi)
        } // end for (ui)
    } // end exception test
#endif

    // Exception testing of aliasing assigns is inappropriate, they should
    // never allocate and hence never throw.

    if (veryVerbose) printf("Testing self assignment (Aliasing).");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int         curLen = static_cast<int>(std::strlen(SPEC));

            if (veryVerbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }

            ASSERTV(SPEC, oldLen < curLen);        // strictly increasing

            oldLen = curLen;

            Obj mX; const Obj& X = gg(&mX, SPEC);  // control

            ASSERTV(ti, curLen == (int)X.size());  // same lengths

            Obj mY(xoa2); const Obj& Y = gg(&mY, SPEC);

            if (veryVeryVerbose) { T_; T_; P(Y); }

            ASSERTV(SPEC, Y == Y);
            ASSERTV(SPEC, X == Y);

            Int64 B2, B3, A2, A3;
            B2 = oa2.numBlocksInUse();
            B3 = oa2.numBlocksTotal();

            // test aliasing assignment here, shouldn't throw or allocate

            mY = Y;

            A2 = oa2.numBlocksInUse();
            A3 = oa2.numBlocksTotal();

            ASSERTV(SPEC, Y  == Y);
            ASSERTV(SPEC, X  == Y);
            ASSERTV(SPEC, B2 == A2);
            ASSERTV(SPEC, B3 == A3);

            B2 = oa2.numBlocksInUse();
            B3 = oa2.numBlocksTotal();

            // test aliasing move assign here, shouldn't throw or allocate

            mY = MoveUtil::move(mY);

            A2 = oa2.numBlocksInUse();
            A3 = oa2.numBlocksTotal();

            ASSERTV(SPEC, Y  == Y);
            ASSERTV(SPEC, X  == Y);
            ASSERTV(SPEC, B2 == A2);
            ASSERTV(SPEC, B3 == A3);
        } // end for (ti)
    } // end self-assignment test
}

template <class TYPE, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver1<TYPE, ALLOC>::
                        test07_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    TYPE,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS> StdAlloc;
    typedef bsl::list<TYPE, StdAlloc>            Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = { "", "A", "BC", "CDE" };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);

        {
            const Obj W(VALUES.begin(), VALUES.end(), ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), ma);  const Obj& X = mX;

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
void TestDriver1<TYPE, ALLOC>::test07_select_on_container_copy_construction()
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

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates *default* allocator.\n");

    test07_select_on_container_copy_construction_dispatch<false, false>();
    test07_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    test07_select_on_container_copy_construction_dispatch<true, false>();
    test07_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<TYPE> Allocator;
    typedef bsl::list<TYPE, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE"
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            TestValues VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), a);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test07_copyCtor()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTORS:
    //
    // Concerns:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   allocator.
    //:
    //: 2 The value of the original object is left unaffected.
    //:
    //: 3 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 4 Subsequent changes ('push_back's) on the created object have no
    //:   effect on the original.
    //:
    //: 5 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 The function is exception neutral w.r.t. memory allocation.
    //:
    //: 7 An object copied from an rvalue with no allocator, or with a matching
    //:   allocator specified, will leave the copied object in a valid,
    //:   default-constructed state.
    //:   1 No allocator specified to c'tor.
    //:   2 Allocator specified to c'tor.
    //:
    //: 8 An object copied from an rvalue with a non-matching allocator
    //:   supplied will not modify the rvalue.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For concerns 1 - 3, for each value in S, initialize objects w and x,
    //:   copy construct y from x and use 'operator==' to verify that both x
    //:   and y subsequently have the same value as w.  Let x go out of scope
    //:   and again verify that w == y.
    //:
    //: 3 For concern 4, for each value in S initialize objects w and x, and
    //:   copy construct y from x.  Change the state of y, by using the
    //:   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //:   that y differs from x and w.
    //:
    //: 4 To address concern 5, we will perform tests performed for concern 1:
    //:   o While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.  Also perform test for concerns 2 and 4.
    //:   o Where the object is constructed with an object allocator, and
    //:     neither of global and default allocator is used to supply memory.
    //:
    //: 5 To address concern 6, perform tests for concern 1 performed in the
    //:   presence of exceptions during memory allocations using a
    //:   'bslma_TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   list(const list& orig);
    //   list(const list& orig, const A&);
    //   list(list&& orig);
    //   list(list&& orig, const A&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        bsls::NameOf<TYPE>().name(), k_SCOPED_ALLOC);

    bslma::TestAllocator oaa("objectA", veryVeryVeryVerbose);
    const ALLOC          xoa(&oaa);

    bslma::TestAllocator oab("objectB", veryVeryVeryVerbose);
    const ALLOC          xob(&oab);

    ASSERT(xoa!= xob);

    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

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
            "ABCDEABCDEABCDEAB"
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = std::strlen(SPEC);

            if (verbose) {
                printf("For an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            ASSERTV(SPEC, oldLen < (int)LENGTH); // strictly increasing

            oldLen = static_cast<int>(LENGTH);

            // Create control object 'W' and 'D'
            Obj mW;  const Obj& W = gg(&mW, SPEC);

            const Obj D;                         // default constructed

            ASSERTV(ti, LENGTH == W.size());     // same lengths

            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERT(X == W);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(xoa);
                gg(pX, SPEC);
                Obj mY0(*pX); const Obj& Y0 = mY0;

                ASSERTV(SPEC, checkIntegrity(Y0, LENGTH));
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);

                mY0.push_back(VALUES[LENGTH % NUM_VALUES]);
                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append to new obj : ");
                    P(Y0);
                }
                ASSERTV(SPEC, checkIntegrity(Y0, LENGTH + 1));
                ASSERTV(SPEC, W != Y0);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());
            }

            {   // Testing concern 4.

                if (veryVerbose) printf("\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                Y1.push_back(VALUES[Y1.size() % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y1, LENGTH + 1));
                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, W == X);
            }

            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\tInsert into created obj, "
                           "with test allocator:\n");

                const Int64 BB = oaa.numBlocksTotal();
                const Int64  B = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, xoa);

                const Int64 AA = oaa.numBlocksTotal();
                const Int64  A = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                ASSERTV(SPEC, BB + expectedBlocks(LENGTH) == AA);
                ASSERTV(SPEC,  B + expectedBlocks(LENGTH) ==  A);

                const Int64 CC = oaa.numBlocksTotal();
                const Int64  C = oaa.numBlocksInUse();

                Y11.push_back(VALUES[LENGTH % NUM_VALUES]);

                const Int64 DD = oaa.numBlocksTotal();
                const Int64  D = oaa.numBlocksInUse();

                // Allocations should increase by one node block for the list.
                // If TYPE uses an allocator, allocations should increase by
                // one more block.
                ASSERTV(SPEC, CC + deltaBlocks(1) == DD);
                ASSERTV(SPEC, C  + deltaBlocks(1) == D );

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
                ASSERTV(SPEC, X == W);
            }

#ifdef BDE_BUILD_TARGET_EXC
            {   // Exception checking.

                const Int64 BB = oaa.numBlocksTotal();
                const Int64  B = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    const Obj Y2(X, xoa);

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64  A = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
#endif // BDE_BUILD_TARGET_EXC

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue copy, no alloc :"); }

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV));

                if (veryVerbose) {
                    printf("\tObj : "); P(Y7);
                }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 0));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D == RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists

                // Concern 8 - after original is modified

                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists
            }

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue copy, match alloc :");}

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV), xoa);

                if (veryVerbose) { printf("\tObj : "); P(Y7); }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 0));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D == RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists

                // Concern 8 - after original is modified

                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xoa); // propagates
                ASSERTV(SPEC, RV.get_allocator() == xoa); // persists
            }

            {   // Testing concern 7.
                if (veryVerbose) { printf("\tRvalue cp, non-match alloc:");}

                Obj       RV(X, xoa);
                const Obj Y7(MoveUtil::move(RV), xob);

                if (veryVerbose) { printf("\tObj : "); P(Y7); }

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, LENGTH));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, Y7.get_allocator() == xob);   // no propagate
                ASSERTV(SPEC, RV.get_allocator() == xoa);   // persists

                // Concern 8 - after original is modified

                RV.clear();
                RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                ASSERTV(SPEC, checkIntegrity(Y7, LENGTH));
                ASSERTV(SPEC, checkIntegrity(RV, 1));
                ASSERTV(SPEC, W == Y7);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, D != RV);
                ASSERTV(SPEC, Y7.get_allocator() == xob);
                ASSERTV(SPEC, RV.get_allocator() == xoa); // allocator persists
            }

#ifdef BDE_BUILD_TARGET_EXC
            {   // Exception checking, concern 7.1

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj       RV(X, xoa);
                    const Obj Y2(MoveUtil::move(RV));

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 0));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D == RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator()); // propagates

                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }

            {   // Exception checking, concern 7.1

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj       RV(X, xoa);
                    const Obj Y2(MoveUtil::move(RV), xoa);

                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 0));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D == RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());

                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xoa == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }

            {   // Exception checking, concern 7.2

                const Int64 BB = oaa.numBlocksTotal();
                const Int64 B  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oaa) {
                    allocations += bslmaExceptionCounter;

                    Obj RV(X, xoa);

                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, W == RV);

                    const Obj Y2(MoveUtil::move(RV), xob);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, LENGTH));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, xob == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());  // not prop.

                    RV.clear();
                    RV.push_back(VALUES[LENGTH % NUM_VALUES]);

                    ASSERTV(SPEC, checkIntegrity(Y2, LENGTH));
                    ASSERTV(SPEC, checkIntegrity(RV, 1));
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, D != RV);
                    ASSERTV(SPEC, xob == Y2.get_allocator());
                    ASSERTV(SPEC, xoa == RV.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oaa.numBlocksTotal();
                const Int64 A  = oaa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                ASSERTV(SPEC, BB + (int) allocations == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
#endif // BDE_BUILD_TARGET_EXC
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test06_equalityOp()
{
    // ------------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS
    //
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes (i.e., 'allocator') participate.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:11 The equality operator's signature and return type are standard.
    //:
    //:12 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 11..12)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a "scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create two (object) allocators, 'oax' and 'oay'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2'.
    //:
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-10)
    //
    // Testing:
    //   bool operator==(const list& lhs, const list& rhs);
    //   bool operator!=(const list& lhs, const list& rhs);
    // ------------------------------------------------------------------------

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        using namespace bsl;

        operatorPtr operatorEq = operator==;
        (void) operatorEq;  // quash potential compiler warnings

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        operatorPtr operatorNe = operator!=;
        (void) operatorNe;
#else
        bool ne = Obj() != Obj();
        (void) ne;
#endif
    }

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
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA"
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof SPECS[0] };

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1   = SPECS[ti];
            const size_t      LENGTH1 = strlen(SPEC1);

            if (veryVerbose) { T_ P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);

                Obj mX(xscratch);  const Obj& X = gg(&mX, SPEC1);

                ASSERTV(SPEC1, X,   X == X);
                ASSERTV(SPEC1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2   = SPECS[tj];
                const size_t      LENGTH2 = strlen(SPEC2);
                const bool        EXP     = ti == tj;  // expected result

                if (veryVerbose) { T_ T_ P_(LENGTH2) P(SPEC2) }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    ALLOC xxa(&xa);
                    ALLOC xya(&ya);

                    Obj mX(xxa);  const Obj& X = gg(&mX, SPEC1);
                    Obj mY(xya);  const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(SPEC1, SPEC2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(SPEC1, SPEC2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(SPEC1, SPEC2, CONFIG,  EXP == (X == Y));
                    ASSERTV(SPEC1, SPEC2, CONFIG,  EXP == (Y == X));

                    ASSERTV(SPEC1, SPEC2, CONFIG, !EXP == (X != Y));
                    ASSERTV(SPEC1, SPEC2, CONFIG, !EXP == (Y != X));

                    ASSERTV(SPEC1, SPEC2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(SPEC1, SPEC2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test04_basicAccessors()
{
    // ------------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    //
    // Concerns:
    //: 1 size() returns the list size.
    //:
    //: 2 begin() == end() if and only if the list is empty
    //:
    //: 3 Iterating from begin() to end() will visit every value in a list and
    //:   only the values in that list.
    //:
    //: 4 Iteration works for both const containers (using const_iterator) and
    //:   non-'const' containers (using iterator).
    //:
    //: 5 empty() returns true if size() return 0.
    //:
    //: 6 The test function 'succ' increments an iterator by n.
    //:
    //: 7 The test function 'nthElem' returns the nth element of a list.
    //:
    //: 8 The test function 'is_mutable' returns true if its argument is a
    //:   mutable lvalue.
    //
    // Plan:
    //: 1 Specify a set S of representative object values ordered by increasing
    //:   length.  For each value w in S, initialize a newly constructed object
    //:   x with w using 'gg' and verify that each basic accessor returns the
    //:   expected result.
    //
    // NOTE: This is not a thorough test of iterators.  This test is only
    // sufficient for using iterators to access the contents of a list in
    // order.
    //
    // Testing:
    //   int size() const;
    //   bool empty() const;
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //
    //   iterator succ(iterator);
    //   const_iterator succ(iterator) const;
    //   T& nthElem(list& x, int n);
    //   const T& nthElem(list& x, int n) const;
    //   bool is_mutable(T& val);
    //   bool is_mutable(const T& val);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const int MAX_LENGTH = 32;

    static const struct {
        int         d_lineNum;                   // source line number
        const char *d_spec_p;                    // specification string
        int         d_length;                    // expected length
        int         d_elements[MAX_LENGTH + 1];  // expected element values
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

    enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

    if (verbose) printf("Testing is_mutable.\n");
    {
        TYPE        mV;
        TYPE&       mVref = mV;
        const TYPE& cmVref = mV;

        ASSERT(  is_mutable(mV));
        ASSERT(  is_mutable(mVref));
        ASSERT(! is_mutable(cmVref));
    }

    if (verbose) printf("Testing const and non-'const' versions of "
                        "'begin()' and 'end()'.\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec_p;
            const int   LENGTH = DATA[ti].d_length;
            const int  *EXP    = DATA[ti].d_elements;

            ASSERT(LENGTH <= MAX_LENGTH);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(ti, LENGTH == static_cast<int>(X.size()));
            ASSERTV(ti, (LENGTH == 0) == X.empty());
            ASSERTV(ti, (LENGTH == 0) == bsl::empty(X));

            if (veryVerbose) {
                printf( "\ton objects of length %d:\n", LENGTH);
            }

            // non-decreasing
            ASSERTV(LINE, oldLen <= LENGTH);
            oldLen = LENGTH;

            if (veryVerbose) printf("\t\tSpec = \"%s\"", SPEC);

            if (veryVerbose) {
                T_; T_; T_; P(X);
            }

            int            i;
            iterator       imX;
            const_iterator iX;
            for (i = 0, imX = mX.begin(), iX = X.begin(); i < LENGTH;
                                                        ++i, ++imX, ++iX) {
                TYPE v;
                assignTo(bsls::Util::addressOf(v), EXP[i]);

                ASSERTV(LINE, i, iX  != X.end());
                ASSERTV(LINE, i, imX != mX.end());
                ASSERTV(LINE, i, imX == iX);
                ASSERTV(LINE, i, v == *iX);
                ASSERTV(LINE, i, v == *imX);
                ASSERTV(LINE, i, succ(X.begin(),i) == iX);
                ASSERTV(LINE, i, succ(mX.begin(),i) == imX);
                ASSERTV(LINE, i, bsls::Util::addressOf(nthElem(X,i)) ==
                                               bsls::Util::addressOf(*iX));
                ASSERTV(LINE, i, bsls::Util::addressOf(nthElem(mX,i)) ==
                                              bsls::Util::addressOf(*imX));
            }
            ASSERTV(LINE, i, iX  == X.end());
            ASSERTV(LINE, i, imX == mX.end());

            // Sanity check that the test driver doesn't have unused data:
            for (; i < MAX_LENGTH; ++i) {
                ASSERTV(LINE, i, 0 == EXP[i]);
            }
        }
    }

    if (verbose) printf("Testing non-'const' iterators "
                        "modify state of object correctly.\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = DATA[ti].d_length;
            const int    *EXP    = DATA[ti].d_elements;

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(ti, LENGTH == X.size()); // same lengths
            ASSERTV(ti, (LENGTH == 0) == X.empty());
            ASSERTV(ti, (LENGTH == 0) == bsl::empty(X));

            if (veryVerbose) {
                printf("\tOn objects of length " ZU ":\n", LENGTH);
            }

            // non-decreasing

            ASSERTV(LINE, oldLen <= (int)LENGTH);
            oldLen = static_cast<int>(LENGTH);

            if (veryVerbose) printf( "\t\tSpec = \"%s\"", SPEC);

            if (veryVerbose) {
                T_; T_; T_; P(X);
            }

            Obj mY(xoa);  const Obj& Y = mY;

            for (size_t j = 0; j < LENGTH; j++) {
                primaryManipulator(&mY, 0);
            }

            // Change state of Y so its same as X

            size_t j = 0;
            for (iterator imY = mY.begin(); imY != mY.end(); ++imY, ++j) {
                assignTo(bsls::Util::addressOf(*imY), EXP[j]);
            }
            ASSERTV(ti, LENGTH == j);

            if (veryVerbose) {
                printf("\t\tNew object1: "); P(Y);
            }

            const_iterator iX;
            const_iterator iY;
            for (j = 0, iX = X.begin(), iY = Y.begin(); iX != X.end();
                                                         ++j, ++iX, ++iY) {
                ASSERTV(ti, j, *iY == *iX);
            }
            ASSERTV(ti, iY == Y.end());

            // Just for kicks, use the (untested) operator==
            ASSERTV(ti, Y == X);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test03_generatorGG()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg':
    //
    // Concerns:
    //: 1 Having demonstrated that our primary manipulators work as expected
    //:   under normal conditions, we want to verify that:
    //:   o Valid generator syntax produces expected results.
    //:   o Invalid syntax is detected and reported.
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
    //:   'clear').
    //
    // Testing:
    //   list<T,A>& gg(list<T,A> *object, const char *spec);
    //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        bsls::NameOf<TYPE>().name(), k_SCOPED_ALLOC);

    bslma::TestAllocator oa("object", veryVeryVerbose);
    const ALLOC          xoa(&oa);

    {
        enum { MAX_LENGTH = 10 };
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
            int         d_length;                // expected length
            int         d_elements[MAX_LENGTH];  // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ----------------------
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
        enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = DATA[ti].d_length;
            const int    *EXP    = DATA[ti].d_elements;
            const int     curLen = static_cast<int>(std::strlen(SPEC));

            Obj mW(xoa);  const Obj& W = mW;

            ASSERT(0 > ggg(&mW, SPEC));

            Obj mU(xoa);  const Obj& U = gg(&mU, SPEC);  // original spec

            ASSERT(&U == &mU);

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";

            char buf[100];
            std::strcpy(buf, MORE_SPEC);
            std::strcat(buf, SPEC);

            Obj mV(xoa);  const Obj& V = gg(&mV, buf);   // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);         // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(U);
                T_; T_; T_; P(V);
            }

            ASSERTV(LINE, LENGTH == U.size());
            ASSERTV(LINE, LENGTH == V.size());

            const_iterator iu = U.begin();
            const_iterator iv = V.begin();
            const_iterator iw = W.begin();
            for (size_t i = 0; i < LENGTH; ++i, ++iu, ++iv, ++iw) {
                TYPE element;
                assignTo(bsls::Util::addressOf(element), EXP[i]);

                ASSERTV(LINE, i, element == *iu);
                ASSERTV(LINE, i, element == *iv);
                ASSERTV(LINE, i, element == *iw);
            }
        }
    }

    if (verbose) printf("Testing generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec     index
            //----  -------  -----
            { L_,   "",      -1,  }, // control

            { L_,   "~",     -1,  }, // control
            { L_,   " ",      0,  },
            { L_,   ".",      0,  },
            { L_,   "E",     -1,  }, // control
            { L_,   "I",      0,  },
            { L_,   "Z",      0,  },

            { L_,   "AE",    -1,  }, // control
            { L_,   "aE",     0,  },
            { L_,   "Ae",     1,  },
            { L_,   ".~",     0,  },
            { L_,   "~!",     1,  },
            { L_,   "  ",     0,  },

            { L_,   "ABC",   -1,  }, // control
            { L_,   " BC",    0,  },
            { L_,   "A C",    1,  },
            { L_,   "AB ",    2,  },
            { L_,   "?#:",    0,  },
            { L_,   "   ",    0,  },

            { L_,   "ABCDE", -1,  }, // control
            { L_,   "aBCDE",  0,  },
            { L_,   "ABcDE",  2,  },
            { L_,   "ABCDe",  4,  },
            { L_,   "AbCdE",  1,  }
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const int     INDEX  = DATA[ti].d_index;
            const size_t  LENGTH = std::strlen(SPEC);

            Obj mX(xoa);

            if (static_cast<int>(LENGTH) != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test02_primaryManipulators()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //
    // Concerns:
    //: 1 The default constructor
    //:   o creates the correct initial value.
    //:   o allocates exactly one block.
    //:
    //: 2 The destructor properly deallocates all allocated memory to its
    //:   corresponding allocator from any attainable state.
    //:
    //: 3 'primaryManipulator' and 'primaryCopier'
    //:   o produces the expected value.
    //:   o maintains valid internal state.
    //:   o preserves the strong exception guarantee and is exception-neutral
    //:     with respect to memory allocation.
    //:   o does not change the address of any other list elements
    //:   o has the internal memory management system hooked up properly so
    //:     that *all* internally allocated memory draws from the same
    //:     user-supplied allocator whenever one is specified.
    //:
    //: 4 'clear'
    //:   o produces the expected value (empty).
    //:   o properly destroys each contained element value.
    //:   o maintains valid internal state.
    //:   o does not allocate memory.
    //:   o deallocates all element memory
    //:
    //: 5 The size-based parameters of the class reflect the platform.
    //
    // Plan:
    //: 1 To address concerns 1a - 1c, create an object using the default
    //:   constructor:
    //:   o With and without passing in an allocator.
    //:   o In the presence of exceptions during memory allocations using a
    //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
    //:   o Where the object is constructed with an object allocator and
    //:     neither of global and default allocator is used to supply memory.
    //:
    //: 2 To address concerns 3a - 3e, construct a series of independent
    //:   objects, ordered by increasing length.  In each test, allow the
    //:   object to leave scope without further modification, so that the
    //:   destructor asserts internal object invariants appropriately.  After
    //:   the final insert operation in each test, use the (untested) basic
    //:   accessors to cross-check the value of the object and the
    //:   'bslma_TestAllocator' to confirm whether memory allocation has
    //:   occurred.
    //:
    //: 3 To address concerns 4a-4e, construct a similar test, replacing
    //:   'primaryCopier' with 'clear'; this time, however, use the test
    //:   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //:
    //: 4 To address concerns 2, 3d, 4d, create a small "area" test that
    //:   exercises the construction and destruction of objects of various
    //:   lengths in the presence of memory allocation exceptions.  Two
    //:   separate tests will be performed:
    //..
    //     Let S be the sequence of integers { 0 .. N - 1 }.
    //     (1) for each i in S, use the default constructor and
    //         'primaryCopier/primaryManipulator' to create an instance
    //         of length i, confirm its value (using basic accessors), and
    //         let it leave scope.
    //     (2) for each (i, j) in S X S, use
    //         'primaryCopier/primaryManipulator' to create an instance of
    //         length i, use 'clear' to clear its value and confirm (with
    //         'length'), use insert to set the instance to a value of
    //         length j, verify the value, and allow the instance to leave
    //         scope.
    //..
    //: 5 The first test acts as a "control" in that 'clear' is not called; if
    //:   only the second test produces an error, we know that 'clear' is to
    //:   blame.  We will rely on 'bslma_TestAllocator' and purify to address
    //:   concern 2, and on the object invariant assertions in the destructor
    //:   to address concerns 3d and 4d.
    //:
    //: 6 To address concern 5, the values will be explicitly compared to the
    //:   expected values.  This will be done first so as to ensure all other
    //:   tests are reliable and may depend upon the class's constants.
    //
    // Testing:
    //   list<T,A>(const A& a = A());
    //   ~list<T,A>(); void
    //   primaryManipulator(list<T,A>*, int); void
    //   primaryCopier(list<T,A>*,const T&); void clear();
    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting type: %s (ALLOC = %d).\n",
                        bsls::NameOf<TYPE>().name(), k_SCOPED_ALLOC);

    TestValues VALUES("ABCDEFGH");
    const int  NUM_VALUES = 8;

    const char   *spec = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t  numSpecValues = std::strlen(spec);


    if (verbose) printf("\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const Obj X;

        const Int64 AA = da.numBlocksTotal();
        const Int64 A  = da.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }

        ASSERTV(X.size(), 0 == X.size());
        ASSERTV(&da == X.get_allocator().mechanism());
        ASSERTV(AA, expectedBlocks(0) == AA);
        ASSERTV(A,  expectedBlocks(0) == A);
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        const ALLOC          xoa(&oa);

        const Obj mX(xoa);  const Obj& X = mX;

        const Int64 AA = oa.numBlocksTotal();
        const Int64 A  = oa.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }

        ASSERTV(X.size(), 0 == X.size());
        ASSERTV(xoa == X.get_allocator());
        ASSERTV(AA, expectedBlocks(0) == AA);
        ASSERTV(A,  expectedBlocks(0) == A);
    }

    // ------------------------------------------------------------------------
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    const ALLOC          xoa(&oa);

    if (verbose) printf(
          "\tTesting 'primaryManipulator' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            const TYPE *elemAddrs[NUM_TRIALS];

            ASSERT(mX.get_allocator().mechanism() == defaultAllocator_p);

            for (size_t i = 0; i < li; ++i) {
                primaryManipulator(&mX, spec[i % numSpecValues]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tBEFORE: "); P(X);
            }

            primaryManipulator(&mX, spec[li % numSpecValues]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            if (veryVerbose){
                printf("\t\t\tAFTER: "); P(X);
            }

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, spec[i] == value_of(*it));
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, spec[li] == value_of(*it));
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose)
        printf("\tTesting 'primaryCopier' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            const TYPE *elemAddrs[NUM_TRIALS];

            ASSERT(mX.get_allocator().mechanism() == defaultAllocator_p);

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            if (veryVerbose){
                printf("\t\t\tBEFORE: "); P(X);
            }

            primaryCopier(&mX, VALUES[li % NUM_VALUES]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            if (veryVerbose){
                printf("\t\t\tAFTER: "); P(X);
            }

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, VALUES[li % NUM_VALUES] == *it);
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf(
             "\tTesting 'primaryManipulator' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj         mX(xoa);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                primaryManipulator(&mX, spec[i % numSpecValues]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            primaryManipulator(&mX, spec[li % numSpecValues]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            ASSERTV(li, bsls::NameOf<TYPE>(), BB, deltaBlocks(1), AA,
                    BB + deltaBlocks(1) == AA);

            ASSERTV(li, B + deltaBlocks(1) == A);

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, spec[i % numSpecValues] == value_of(*it));
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, spec[li % numSpecValues] == value_of(*it));
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf(
                  "\tTesting 'primaryCopier' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj         mX(xoa);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            primaryCopier(&mX, VALUES[li % NUM_VALUES]);
            elemAddrs[li] = bsls::Util::addressOf(X.back());

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            ASSERTV(li, bsls::NameOf<TYPE>(), BB, deltaBlocks(1), AA,
                    BB + deltaBlocks(1) == AA);

            ASSERTV(li, B + deltaBlocks(1) == A);

            ASSERTV(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }

            ASSERTV(li, VALUES[li % NUM_VALUES] == *it);
            ASSERTV(li, elemAddrs[li] == bsls::Util::addressOf(*it));
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            if (veryVerbose){
                printf("\t\t\tBEFORE "); P(X);
            }

            ASSERTV(li, li == X.size());

            mX.clear();

            if (veryVerbose){
                printf("\t\t\tAFTER "); P(X);
            }

            ASSERTV(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT "); P(X);
            }

            ASSERTV(li, li == X.size());
            const_iterator it = X.begin();
            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
            }
        }
    }

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(xoa);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const Int64 BB = oa.numBlocksTotal();
            const Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.clear();

            const Int64 AA = oa.numBlocksTotal();
            const Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
            }

            ASSERTV(li, li == X.size());

            const Int64 CC = oa.numBlocksTotal();
            const Int64 C  = oa.numBlocksInUse();

            if (veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P(X);
            }

            ASSERTV(li, li == X.size());

            ASSERTV(li, BB == AA);
            ASSERTV(li, B - deltaBlocks(li) == A);

            ASSERTV(li, BB + deltaBlocks(li) == CC);
            ASSERTV(li, B == C);
        }
    }

    ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());

    // ------------------------------------------------------------------------

    if (verbose) printf("\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'primaryCopier' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an instance
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the instance to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose)
                printf("\t\t\tOn an object of length " ZU ".\n", li);

          int         throws = -1;
          const Int64 beforeInUse = oa.numBlocksInUse();
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++throws;

            if (oa.numBlocksInUse() != beforeInUse) {
                int jj = 0;
                ++jj;    // someplace to break point
            }

            Obj mX(xoa);  const Obj& X = mX;                        // 1.

            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {                       // 2.
                ExceptionProctor proctor(&mX, L_);
                primaryCopier(&mX, VALUES[i % NUM_VALUES]);
                elemAddrs[i] = bsls::Util::addressOf(X.back());
                proctor.release();
            }

            ASSERTV(li, li == X.size());                            // 3.
            const_iterator it = X.begin();

            for (unsigned i = 0; i < li; ++it, ++i) {
                ASSERTV(li, i, VALUES[i % NUM_VALUES] == *it);
                ASSERTV(li, i, elemAddrs[i] == bsls::Util::addressOf(*it));
            }
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                  // 4.
          ASSERTV(li, oa.numBlocksInUse() - beforeInUse,
                  beforeInUse == oa.numBlocksInUse());              // 5.
        }
    }

    if (verbose) printf("\t\tWith 'primaryCopier' and 'clear'\n");
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

                size_t k; // loop index

                Obj mX(xoa);  const Obj& X = mX;                    // 1.

                const TYPE *elemAddrs[NUM_TRIALS];

                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionProctor proctor(&mX, L_);
                    primaryCopier(&mX, VALUES[0]);
                    elemAddrs[k] = bsls::Util::addressOf(X.back());
                    proctor.release();
                }

                ASSERTV(i, j, i == X.size());                       // 3.
                const_iterator it = X.begin();
                for (k = 0; k < i; ++k, ++it) {
                    ASSERTV(i, j, k, VALUES[0] == *it);
                    ASSERTV(i, j, k,
                                   elemAddrs[k] == bsls::Util::addressOf(*it));
                }

                // verify clear doesn't throw

                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                  ++numThrows;
                  mX.clear();                                       // 4.
                  ASSERTV(i, j, 0 == X.size());                     // 5.
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END            // 8.
                ASSERT(0 == numThrows);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver1<TYPE,ALLOC>::test01_breathingTest()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST:
    //
    // Concern:
    //: 1 We want to exercise basic value-semantic functionality.  In
    //:   particular we want to demonstrate a base-line level of correct
    //:   operation of the following methods and operators:
    //:   o default and copy constructors (and also the destructor)
    //:   o the assignment operator (including aliasing)
    //:   o equality operators: 'operator==' and 'operator!='
    //:   o primary manipulators: 'push_back' and 'clear' methods
    //:   o basic accessors: 'size' and 'operator[]'
    //
    // Plan:
    //: 1 Create four objects using both the default and copy constructors.
    //:
    //: 2 Exercise these objects using
    //:   o primary manipulators
    //:   o basic accessors
    //:   o equality operators
    //:   o the assignment operator
    //:
    //: 3 Try aliasing with assignment for a non-empty instance [11] and allow
    //:   the result to leave scope, enabling the destructor to assert internal
    //:   object invariants.
    //:
    //: 4 Display object values frequently in verbose mode:
    //..
    // 1) Create an object x1 (default ctor).         { x1: }
    // 2) Create a second object x2 (copy from x1).   { x1: x2: }
    // 3) Prepend an element value A to x1).          { x1:A x2: }
    // 4) Append the same element value A to x2).     { x1:A x2:A }
    // 5) Prepend/Append two values B & C to x2).     { x1:A x2:BAC }
    // 6) Remove all elements from x1.                { x1: x2:BAC }
    // 7) Create a third object x3 (default ctor).    { x1: x2:BAC x3: }
    // 8) Create a forth object x4 (copy of x2).      { x1: x2:BAC x3: x4:BAC }
    // 9) Assign x2 = x1 (non-empty becomes empty).   { x1: x2: x3: x4:BAC }
    // 10) Assign x3 = x4 (empty becomes non-empty).  { x1: x2: x3:BAC x4:BAC }
    // 11) Assign x4 = x4 (aliasing).                 { x1: x2: x3:BAC x4:BAC }
    //..
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // ------------------------------------------------------------------------

    TestValues   VALUES("ABCDEFGH");
    const TYPE&  A = VALUES[0];
    const TYPE&  B = VALUES[1];
    const TYPE&  C = VALUES[2];

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(&oa);  const Obj& X1 = mX1;

    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1.\n");

    ASSERT(  X1 == X1);
    ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");

    Obj mX2(X1, &oa);  const Obj& X2 = mX2;

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check the initial state of x2.\n");

    ASSERT(0 == X2.size());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 3) Prepend an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_front(A);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check new state of x1.\n");

    ASSERT(1 == X1.size());
    ASSERT(A == X1.front());
    ASSERT(A == X1.back());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1, x2.\n");

    ASSERT(  X1 == X1 );
    ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));
    ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(A);

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(1 == X2.size());
    ASSERT(A == X2.front());
    ASSERT(A == X2.back());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 5) Prepend another element value B and append "
                        "another element c to x2).\t\t{ x1:A x2:BAC }\n");
    mX2.push_front(B);
    mX2.push_back(C);

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(3 == X2.size());
    ASSERT(B == X2.front());
    ASSERT(A == nthElem(X2, 1));
    ASSERT(C == X2.back());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1, x2.\n");

    ASSERT(!(X2 == X1));
    ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:BAC }\n");
    mX1.clear();

    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check new state of x1.\n");

    ASSERT(0 == X1.size());

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1, x2.\n");

    ASSERT(  X1 == X1 );
    ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));
    ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:BAC x3: }\n");

    Obj mX3(&oa);  const Obj& X3 = mX3;

    if (verbose) { T_;  P(X3); }

    if (verbose) printf("\ta) Check new state of x3.\n");
    ASSERT(0 == X3.size());

    if (verbose) printf("\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );
    ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));
    ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );
    ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:BAC x3: x4:BAC }\n");

    Obj mX4(X2, &oa);  const Obj& X4 = mX4;

    if (verbose) { T_;  P(X4); }

    if (verbose) printf("\ta) Check new state of x4.\n");

    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose) printf("\tb) Try equality operators: x4 <op> x1 .. x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );
    ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));
    ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );
    ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf(" 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:BAC }\n");

    mX2 = X1;

    if (verbose) { T_;  P(X2); }

    if (verbose) printf("\ta) Check new state of x2.\n");

    ASSERT(0 == X2.size());

    if (verbose) printf("\tb) Try equality operators: x2 <op> x1 .. x4.\n");
    ASSERT(  X2 == X1 );
    ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );
    ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );
    ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));
    ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX3 = X4;

    if (verbose) { T_;  P(X3); }

    if (verbose) printf("\ta) Check new state of x3.\n");

    ASSERT(3 == X3.size());
    ASSERT(B == nthElem(X3, 0));
    ASSERT(A == nthElem(X3, 1));
    ASSERT(C == nthElem(X3, 2));

    if (verbose) printf("\tb) Try equality operators: x3 <op> x1 .. x4.\n");
    ASSERT(!(X3 == X1));
    ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));
    ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );
    ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );
    ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX4 = X4;

    if (verbose) { T_;  P(X4); }

    if (verbose) printf("\ta) Check new state of x4.\n");

    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose) printf("\tb) Try equality operators: x4 <op> x1 .. x4.\n");
    ASSERT(!(X4 == X1));
    ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));
    ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );
    ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );
    ASSERT(!(X4 != X4));

    (void) value_of(A);
}

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace UsageExamples {

using bsl::list;

///Example 1: Filter "Twinkle Star"
/// - - - - - - - - - - - - - - - -
// Suppose an observatory needs to analyze the results of a sky survey.  The
// raw data is a text file of star observations where each star is represented
// by a tuple of three numbers: (x, y, b), where x and y represent the angular
// coordinates of the star in the sky and b represents its brightness on a
// scale of 0 to 100.  A star having brightness 75 or higher is of particular
// interest, which is named "twinkle star".
//
// Our first example will read such a data file as described above, filter out
// the dim stars (brightness less than 75), and count the "twinkle star"s left
// in the list.  Our test data set has been selected such that there are 10
// stars in the set, of which 4 are sufficiently bright as to pass our filter.
//
// First, we define the class 'Star' that encapsulates a single tuple, and
// provides accessors functions 'x', 'y', and 'brightness', file I/O functions
// 'read' and 'write', and free operators '==', '!=', and '<':
//..
    class Star
        // This class represents a star as seen through a digital telescope.
    {
        // DATA
        double d_x, d_y;     // coordinates

        int    d_brightness; // brightness on a scale of 0 to 100

      public:
        // CREATORS
        Star()
            // Create a 'Star' object located at coordinates '(0, 0)' having
            // '0' brightness.
        : d_x(0), d_y(0), d_brightness(0)
        {
        }

        Star(double x, double y, int b)
            // Create a 'Star' object located at the specified coordinates
            // '(x, y)' having the specified 'b' brightness.
        : d_x(x), d_y(y), d_brightness(b)
        {
        }

        // Compiler-generated copy construction, assignment, and destructor
        //! Star(const Star&) = default;
        //! Star& operator=(const Star&) = default;
        //! ~Star() = default;

        // MANIPULATORS
        bool read(FILE *input);
            // Read x, y, and brightness from the specified 'input' file.
            // Return 'true' if the read succeeded and 'false' otherwise.

        void write(FILE *output) const;
            // Write x, y, and brightness to the specified 'output' file
            // followed by a newline.

        // ACCESSORS
        double x() const
            // Return the x coordinate of this 'Star' object.
        {
            return d_x;
        }

        double y() const
            // Return the y coordinate of this 'Star' object.
        {
            return d_y;
        }

        int brightness() const
            // Return the brightness of this 'Star' object.
        {
            return d_brightness;
        }
    };

    bool Star::read(FILE *input)
    {
        int ret = fscanf(input, "%lf %lf %d", &d_x, &d_y, &d_brightness);
        return 3 == ret;
    }

    void Star::write(FILE *output) const
    {
        fprintf(output, "%f %f %d\n", d_x, d_y, d_brightness);
    }

    bool operator==(const Star& lhs, const Star& rhs)
    {
        return lhs.x() == rhs.x()
            && lhs.y() == rhs.y()
            && lhs.brightness() == rhs.brightness();
    }

    bool operator!=(const Star& lhs, const Star& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const Star& lhs, const Star& rhs)
    {
        if      (lhs.x() < rhs.x()) {
            return true;                                              // RETURN
        }
        else if (rhs.x() < lhs.x()) {
            return false;                                             // RETURN
        }
        else if (lhs.y() < rhs.y()) {
            return true;                                              // RETURN
        }
        else if (rhs.y() < lhs.y()) {
            return false;                                             // RETURN
        }
        else {
            return lhs.brightness() < rhs.brightness();               // RETURN
        }
    }

    // FREE FUNCTIONS
    bool operator==(const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value equal to that of
        // the specified 'rhs'.
    bool operator!=(const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value that is not equal
        // to that of the specified 'rhs'.
    bool operator< (const Star& lhs, const Star& rhs);
        // Return 'true' if the specified 'lhs' has a value less than that of
        // the specified 'rhs'.

//..
// Then, we define a 'readData' method that reads a file of data points and
// appends each onto a list.  The stars are stored in the data file in
// ascending sorted order by x and y coordinates.
//..
    void readData(list<Star> *starList, FILE *input)
        // Read stars from the specified 'input' and populate the specified
        // list '*starList' with them.
    {
        Star s;
        while (s.read(input)) {
            starList->push_back(s);
        }
    }
//..
// Now, we define the 'filter' method, which is responsible for removing stars
// with a brightness of less than 75 from the data set.  It does this by
// iterating over the list and erasing any element that does not pass the
// filter.  The list object features a fast 'erase' member function.  The
// return value of 'erase' is an iterator to the element immediately following
// the erased element:
//..
    void filter(list<Star> *starList)
        // Remove stars with brightness less than 75 from the specified
        // '*starList'.
    {
        static const int threshold = 75;

        list<Star>::iterator i = starList->begin();
        while (i != starList->end()) {
            if (i->brightness() < threshold) {
                i = starList->erase(i);  // Erase and advance to next element.
            }
            else {
                ++i;  // Advance to next element without erasing
            }
        }
    }
//..
// Finally, we use the methods defined in above steps to put together our
// program to find twinkle stars:
//..
int usageExample1(int verbose)
    // Demonstrate the usage of this component, with the level of output
    // controlled by the specified 'verbose'.
{
    FILE *input = fopen("star_data1.txt", "r");  // Open input file.
    ASSERT(input);

    list<Star> starList;                         // Define a list of stars.
    ASSERT(starList.empty());                    // A list should be empty
                                                 // after default construction.

    readData(&starList, input);                  // Read input to the list.
    ASSERT(10 == starList.size());               // Verify correct reading.
    fclose(input);                               // Close input file.

    filter(&starList);                           // Pick twinkle stars.
    ASSERT(4 == starList.size());                // Verify correct filter.

    // Print out twinkle stars.
    if (verbose) {
        for (list<Star>::const_iterator i = starList.begin();
                i != starList.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..

///Example 2: Combine Two Star Surveys
///- - - - - - - - - - - - - - - - - -
// Now we want to combine the results from two star surveys into a single list,
// using the same 'Star' class defined in the first usage example.
//
// First, we begin by reading both lists and filtering them.  (Our test data is
// selected so that the second data file contains 8 starts of which 3 are
// sufficiently bright as to pass our filter:
//..
int usageExample2(int verbose)
    // Demonstrate the usage of this component, with the level of output
    // controlled by the specified 'verbose'.
{
    FILE *input = fopen("star_data1.txt", "r");  // Open first input file.
    ASSERT(input);

    list<Star> starList1;                    // Define first star list.
    ASSERT(starList1.empty());

    readData(&starList1, input);             // Read input into list.
    ASSERT(10 == starList1.size());
    fclose(input);                           // Close first input file.

    input = fopen("star_data2.txt", "r");    // Open second input file.
    ASSERT(input);

    list<Star> starList2;                    // Define second list.
    ASSERT(starList2.empty());

    readData(&starList2, input);             // Read input into list.
    ASSERT(8 == starList2.size());
    fclose(input);                           // Close input file.

    filter(&starList1);                      // Pick twinkle stars from the
                                             // first star list.
    ASSERT(4 == starList1.size());

    filter(&starList2);                      // Pick twinkle stars from the
                                             // second star list.
    ASSERT(3 == starList2.size());

// Then, we combine the two lists, 'starList1' and 'starList2'.  One way to do
// this is to simply insert the second list at the end of the first:
//..
    list<Star> tmp1(starList1);  // Make a copy of the first list
    list<Star> tmp2(starList2);  // Make a copy of the second list
    tmp1.insert(tmp1.end(), tmp2.begin(), tmp2.end());
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(3 == tmp2.size());    // 'tmp2' should be unchanged.
//..
// Next, let's have a closer look of the above code and see if we can improve
// the combination performance.  The above 'insert' method appends a copy of
// each element in 'tmp2' onto the end of 'tmp1'.  This copy is unnecessary
// because we have no need for 'tmp2' after the lists have been combined.  A
// faster and less-memory-intensive technique is to use the 'splice' function,
// which *moves* rather than *copies* elements from one list to another:
//..
    tmp1 = starList1;
    tmp2 = starList2;
    tmp1.splice(tmp1.begin(), tmp2);
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(0 == tmp2.size());    // 'tmp2' should be emptied by the splice.
//..
// Notice that, while the original lists were sorted in ascending order
// (because the data files were originally sorted), the combined list is no
// longer sorted.  To fix it, we sort 'tmp1' using the 'sort' member function:
//..
    tmp1.sort();
//..
// Then, we suggest a third, and also the best approach to combine two lists,
// which is to take advantage of the fact that the lists were originally
// sorted, using the 'merge' function:
//..
    starList1.merge(starList2);     // Merge 'starList2' into 'starList1'.
    ASSERT(7 == starList1.size());  // Verify combined size.
    ASSERT(0 == starList2.size());  // starList2 should be emptied by the
                                    // merge.
//..
// Now, since the two star surveys may overlap, we want to eliminate
// duplicates.  We accomplish this by using the 'unique' member function:
//..
    starList1.unique();             // Eliminate duplicates in 'starList1'.
    ASSERT(6 == starList1.size());  // Verify size after elimination.
//..
// Finally, we print the result:
//..
    if (verbose) {
        for (list<Star>::const_iterator i = starList1.begin();
             i != starList1.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..

}  // close namespace UsageExamples

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create two
    // test allocators.

    // Default Test Allocator.
    bslma::TestAllocator         defaultAllocator("Default Allocator",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // check DEFAULT_DATA and associated variables

    {
        bool maxed = false;
        for (int ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const char *pcI  = DEFAULT_DATA[ti].d_spec_p;
            size_t      iLen = std::strlen(pcI);

            ASSERT(iLen <= DEFAULT_MAX_LENGTH);
            if (DEFAULT_MAX_LENGTH == iLen) {
                maxed = true;
            }

            for (int tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const char *pcJ = DEFAULT_DATA[tj].d_spec_p;

                ASSERT((ti == tj) == !std::strcmp(pcI, pcJ));
            }
        }
        ASSERT(maxed);
    }

    // This compile-only test verifies that various list constructors do not
    // require a copy-assignable 'value_type' (template parameter 'VALUE').

    bsltf::NonAssignableTestType            value(1);
    bsl::list<bsltf::NonAssignableTestType> firstList(20, value);
    bsl::list<bsltf::NonAssignableTestType> secondList(firstList);
    bsl::list<bsltf::NonAssignableTestType> thirdList(firstList.begin(),
                                                      firstList.end());

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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // 10 data points with 4 stars at or above 75 brightness

        static const char STAR_DATA1[] =
            "-1.21 +0.11 76\n"
            "-1.05 +0.70 39\n"
            "-0.89 +1.03 93\n"
            "-0.61 +0.35 71\n"
            "-0.48 +1.19 61\n"
            "-0.01 +0.85 43\n"
            "+0.37 -0.01 90\n"
            "+0.37 +0.90 78\n"
            "+0.70 +0.48 54\n"
            "+0.91 +1.35 38\n";

        // 8 data points with 3 stars at or above 75 brightness
        // 1 point overlaps STAR_DATA1

        static const char STAR_DATA2[] =
            "-1.40 -0.48 74\n"
            "-0.95 -1.00 40\n"
            "-0.91 -0.21 51\n"
            "-0.51 -0.23 88\n"
            "-0.16 -0.55 30\n"
            "+0.37 -0.01 90\n"
            "+0.48 -0.57 66\n"
            "+0.93 -0.35 75\n";

        // Create test data files
        FILE* output = fopen("star_data1.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA1);
        fclose(output);

        output = fopen("star_data2.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA2);
        fclose(output);

        if (verbose) printf("usageExample1\n");
        UsageExamples::usageExample1(veryVerbose);

        if (verbose) printf("usageExample2\n");
        UsageExamples::usageExample2(veryVerbose);

        // Erase output files.

        remove("star_data1.txt");
        remove("star_data2.txt");
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR-RELATED CONCERNS
        //
        // Concerns:
        //: 1 For ALLOC that is a bslma_Allocator
        //:
        //:   1 The list class has the 'bslma::UsesBslmaAllocator' trait.
        //:
        //:   2 The allocator is not copied when the list is copy-constructed.
        //:
        //:   3 The allocator is set with the extended copy-constructor.
        //:
        //:   4 The allocator is passed through to elements if the elements
        //:     also use bslma_Allocator.
        //:
        //:   5 Creating an empty list allocates exactly one block.
        //
        // Plan:
        //: 1 Default construct a list and observe that a single allocation
        //:   takes place (for the sentinel).
        //:
        //: 2 If the type contained in the list allocates, push a value onto
        //:   the back of the list and observe that 2 more allocations take
        //:   place.
        //:
        //: 3 Call the 'allocator' accessor on an element in the list and
        //:   observe that the allocator matches that passed to the list.
        //
        // Testing:
        //   Allocator traits and propagation
        //   A get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TEST ALLOCATOR-RELATED CONCERNS\n"
                            "===============================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test11_allocators,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   No test.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING STREAMING FUNCTIONALITY\n"
                            "===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 The 'rhs' value must not be affected by the operation.
        //:
        //: 2 'rhs' going out of scope has no effect on the value of 'lhs'
        //:   after the assignment.
        //:
        //: 3 After the assignment, no element of the 'lhs' has the same
        //:   address an element of 'rhs' (except in the case of
        //:   self-assignment).
        //:
        //: 4 Aliasing (x = x): The assignment operator must always work --
        //:   even when the lhs and rhs are identically the same object.
        //:
        //: 5 The assignment operator must be neutral with respect to memory
        //:   allocation exceptions.
        //:
        //: 6 The 'lhs' object must allocate all of its internal memory from
        //:   its own allocator, even of 'rhs' uses a different allocator.
        //:
        //: 7 TBD: C++0x move assignment moves the value but not the allocator.
        //:   Memory is allocated only if the 'lhs' and 'rhs' allocators are
        //:   different.
        //
        // Plan:
        //: 1 Specify a set S of unique object values with substantial and
        //:   varied differences, ordered by increasing length.  For each value
        //:   in S, construct an object x along with a sequence of similarly
        //:   constructed duplicates x1, x2, ..., xN.  The elements within each
        //:   object in S are unique so that re-ordering elements cannot
        //:   preserve equality.  Attempt to affect every aspect of white-box
        //:   state by altering each xi in a unique way.  Let the union of all
        //:   such objects be the set T.
        //:
        //: 2 To address concerns 2, 3, and 5, construct tests u = v for all
        //:   '(u, v)' in T X T.  Using canonical controls 'UU' and 'VV',
        //:   assert before the assignment that 'UU == u', 'VV == v', and
        //:   'v == u' if and only if 'VV == UU'.  After the assignment, assert
        //:   that 'VV == u', 'VV == v', and, for grins, that 'v == u'.  Verify
        //:   that each element in v has a different address from the
        //:   corresponding element in v.  Let v go out of scope and confirm
        //:   that 'VV == u'.  All of these tests are performed within the
        //:   'bslma' exception testing apparatus.  Since the execution time is
        //:   lengthy with exceptions, not every permutation is performed when
        //:   exceptions are tested.  Every permutation is also tested
        //:   separately without exceptions.
        //:
        //: 3 As a separate exercise, we address 4 and 5 by constructing tests
        //:   y = y for all y in T.  Using a canonical control X, we will
        //:   verify that X == y before and after the assignment, again within
        //:   the bslma exception testing apparatus.
        //:
        //: 4 To address concern 5, all these tests are performed on user
        //:   defined types:
        //:
        //:   o With allocator, copyable
        //:
        //:   o With allocator, movable
        //:
        //:   o With allocator, not movable
        //
        // Testing:
        //   list& operator=(const list& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING COPY ASSIGNMENT OPERATOR\n"
                            "================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test09_copyAssignmentOp,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test09_copyAssignmentOp,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver1,
                      test09_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   Obj g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING GENERATOR FUNCTION 'g'\n"
                            "==============================\n");

        if (verbose) printf(
                "The 'g' generator function has been removed as 'bsltf'\n"
                "contains a test type that lacks a copy c'tor.  Also, it is\n"
                "not an essential function.\n");
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original
        //:   object (relying on the equality operator) and created with
        //:   the correct allocator.
        //:
        //: 2 The value of the original object is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the source object have
        //:   no effect on the copy-constructed object.
        //:
        //: 4 Subsequent changes ('push_back's) on the created object have no
        //:   effect on the original.
        //:
        //: 5 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws
        //:   from a user-supplied allocator whenever one is specified.
        //:
        //: 6 The function is exception neutral w.r.t. memory allocation.
        //:
        //: 7 An object copied from an rvalue with no allocator, or with a
        //:   matching allocator specified, will leave the copied object
        //:   in a valid, default-constructed state.
        //:   1 No allocator specified to c'tor.
        //:   2 Allocator specified to c'tor.
        //:
        //: 8 An object copied from an rvalue with a non-matching allocator
        //:   supplied will not modify the rvalue.
        //
        // Plan:
        //: 1 Specify a set S of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used in the
        //:   following tests.
        //:
        //: 2 For concerns 1 - 3, for each value in S, initialize objects w and
        //:   x, copy construct y from x and use 'operator==' to verify that
        //:   both x and y subsequently have the same value as w.  Let x go out
        //:   of scope and again verify that w == y.
        //:
        //: 3 For concern 4, for each value in S initialize objects w and x,
        //:   and copy construct y from x.  Change the state of y, by using the
        //:   *primary* *manipulator* 'push_back'.  Using the 'operator!='
        //:   verify that y differs from x and w.
        //:
        //: 4 To address concern 5, we will perform tests performed for concern
        //:   1:
        //:
        //:   o While passing a testAllocator as a parameter to the new object
        //:     and ascertaining that the new object gets its memory from the
        //:     provided testAllocator.  Also perform test for concerns 2 and
        //:     4.
        //:
        //:   o Where the object is constructed with an object allocator, and
        //:     neither of global and default allocator is used to supply
        //:     memory.
        //:
        //: 4 To address concern 6, perform tests for concern 1 performed in
        //:   the presence of exceptions during memory allocations using a
        //:   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //
        // Testing:
        //   list(const list& orig);
        //   list(const list& orig, const A&);
        //   list(list&& orig);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING COPY CONSTRUCTORS\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test07_copyCtor,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test07_copyCtor,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test07_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test07_select_on_container_copy_construction,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Objects constructed with the same values compare equal.
        //:
        //: 2 Objects constructed such that they have same (logical) value but
        //:   different internal representation (due to the lack or presence of
        //:   an allocator) always compare as equal.  3 Unequal objects are
        //:   always compare as unequal.
        //
        // Plan:
        //: 1 Specify a set A of unique allocators including no allocator.
        //:   Specify a set S of unique object values having various minor or
        //:   subtle differences, ordered by non-decreasing length.  Verify the
        //:   correctness of 'operator==' and 'operator!=' (returning either
        //:   true or false) using all elements '(u, ua, v, va)' of the cross
        //:   product S X A X S X A.
        //
        // Note: Since 'operators==' is implemented in terms of basic
        // accessors, it is sufficient to verify only that a difference in
        // value of any one basic accessor for any two given objects implies
        // inequality.  However, to test that no other internal state
        // information is being considered, we want also to verify that
        // 'operator==' reports true when applied to any two objects whose
        // internal representations may be different yet still represent the
        // same (logical) value.  This is achieved by the 'test06_equalityOp'
        // class method of the test driver template, instantiated for the basic
        // test type.  See that function for a list of concerns and a test
        // plan.
        //
        // Testing:
        //   bool operator==(const list&, const list&);
        //   bool operator!=(const list&, const list&);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING EQUALITY OPERATORS\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test06_equalityOp,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test06_equalityOp,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //: 1 No test.
        //
        // Plan:
        //: 1 No test.
        //
        // Testing:
        //   No test.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING OUTPUT (<<) OPERATOR:\n"
                            "=============================\n");

        if (verbose) {
            printf("There is no 'operator<<' for this component.\n");
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //: 1 'size()' returns the list size.
        //:
        //: 2 'begin() == end()' if and only if the list is empty
        //:
        //: 3 Iterating from 'begin()' to 'end()' will visit every value in a
        //:   list and only the values in that list.
        //:
        //: 4 Iteration works for both 'const' containers (using
        //:   'const_iterator') and non-'const' containers (using 'iterator').
        //:
        //: 5 'empty()' returns true if 'size()' return 0
        //:
        //: 6 The test function 'succ' increments an iterator by n.
        //:
        //: 7 The test function 'nthElem' returns the nth element of a list.
        //:
        //: 8 The test function 'is_mutable' returns true if its argument is a
        //:   mutable lvalue.
        //
        // Plan:
        //: 1 Specify a set S of representative object values ordered by
        //:   increasing length.  For each value w in S, initialize a newly
        //:   constructed object x with w using 'gg' and verify that each basic
        //:   accessor returns the expected result.
        //
        // NOTE: This is not a thorough test of iterators.  This test is only
        // sufficient for using iterators to access the contents of a list in
        // order.
        //
        // Testing:
        //   size_type size() const;
        //   bool empty() const;
        //
        //   iterator succ(iterator);
        //   const_iterator succ(iterator) const;
        //   T& nthElem(list& x, int n);
        //   const T& nthElem(list& x, int n) const;
        //   bool is_mutable(T& value);
        //   bool is_mutable(const T& value);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING BASIC ACCESSORS\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test04_basicAccessors,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test04_basicAccessors,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
        //
        // Concerns:
        //: 1 Having demonstrated that our primary manipulators work as
        //:   expected under normal conditions, we want to verify that:
        //:   o Valid generator syntax produces expected results.
        //:   o Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'gg' to set the state of a newly created object.  Verify that
        //:   'gg' returns a valid reference to the modified argument object
        //:   and, using basic accessors, that the value of the object is as
        //:   expected.  Repeat the test for a longer 'spec' generated by
        //:   prepending a string ending in a '~' character (denoting 'clear').
        //:   Note that we are testing the parser only; the primary
        //:   manipulators are already assumed to work.
        //:
        //: 2 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'ggg' to set the state of a newly created object.  Verify that
        //:   'ggg' returns the expected value corresponding to the location of
        //:   the first invalid value of the 'spec'.  Repeat the test for a
        //:   longer 'spec' generated by prepending a string ending in a '~'
        //:   character (denoting 'clear').
        //
        // Testing:
        //   list<T,A>& gg(list<T,A> *object, const char *spec);
        //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'\n"
                    "====================================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test03_generatorGG,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test03_generatorGG,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //: 1 The default constructor
        //:   o creates the correct initial value.
        //:   o allocates exactly one block.
        //:
        //: 2 The destructor properly deallocates all allocated memory to its
        //:   corresponding allocator from any attainable state.
        //:
        //: 3 'primaryManipulator' and 'primaryCopier'
        //:   o produces the expected value.
        //:   o maintains valid internal state.
        //:   o preserves the strong exception guarantee and is
        //:     exception-neutral with respect to memory allocation.
        //:   o does not change the address of any other list elements
        //:   o has the internal memory management system hooked up properly so
        //:     that *all* internally allocated memory draws from the same
        //:     user-supplied allocator whenever one is specified.
        //:
        //: 4 'clear'
        //:   o produces the expected value (empty).
        //:   o properly destroys each contained element value.
        //:   o maintains valid internal state.
        //:   o does not allocate memory.
        //:   o deallocates all element memory
        //:
        //: 5 The size-based parameters of the class reflect the platform.
        //
        // Plan:
        //: 1 To address concerns 1a - 1c, create an object using the default
        //:   constructor:
        //:
        //:   o With and without passing in an allocator.
        //:
        //:   o In the presence of exceptions during memory allocations using a
        //:     'bslma_TestAllocator' and varying its *allocation* *limit*.
        //:
        //:   o Where the object is constructed with an object allocator and
        //:     neither of global and default allocator is used to supply
        //:     memory.
        //:
        //: 2 To address concerns 3a - 3e, construct a series of independent
        //:   objects, ordered by increasing length.  In each test, allow the
        //:   object to leave scope without further modification, so that the
        //:   destructor asserts internal object invariants appropriately.
        //:   After the final insert operation in each test, use the (untested)
        //:   basic accessors to cross-check the value of the object and the
        //:   'bslma_TestAllocator' to confirm whether memory allocation has
        //:   occurred.
        //:
        //: 3 To address concerns 4a-4e, construct a similar test, replacing
        //:   'primaryCopier' with 'clear'; this time, however, use the test
        //:   allocator to record *numBlocksInUse* rather than
        //:   *numBlocksTotal*.
        //:
        //: 4 To address concerns 2, 3d, 4d, create a small "area" test that
        //:   exercises the construction and destruction of objects of various
        //:   lengths in the presence of memory allocation exceptions.  Two
        //:   separate tests will be performed:
        //..
        //     Let S be the sequence of integers { 0 .. N - 1 }.
        //     (1) for each i in S, use the default constructor and
        //         'primaryCopier/primaryManipulator' to create an instance
        //         of length i, confirm its value (using basic accessors), and
        //         let it leave scope.
        //     (2) for each (i, j) in S X S, use
        //         'primaryCopier/primaryManipulator' to create an instance of
        //         length i, use 'clear' to clear its value and confirm (with
        //         'length'), use insert to set the instance to a value of
        //         length j, verify the value, and allow the instance to leave
        //         scope.
        //..
        //: 5 The first test acts as a "control" in that 'clear' is not called;
        //:   if only the second test produces an error, we know that 'clear'
        //:   is to blame.  We will rely on 'bslma_TestAllocator' and purify to
        //:   address concern 2, and on the object invariant assertions in the
        //:   destructor to address concerns 3d and 4d.
        //:
        //: 6 To address concern 5, the values will be explicitly compared to
        //:   the expected values.  This will be done first so as to ensure all
        //:   other tests are reliable and may depend upon the class's
        //:   constants.
        //
        // Testing:
        //   list(const A& a = A());
        //   ~list();
        //   void primaryManipulator(list*, int);
        //   void primaryCopier(list*, const T&);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PRIMARY MANIPULATORS (BOOTSTRAP)\n"
                            "========================================\n");

        RUN_EACH_TYPE(TestDriver1,
                      test02_primaryManipulators,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      TTA,
                      TNA,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver1,
                      test02_primaryManipulators,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //: 1 We want to exercise basic value-semantic functionality.  In
        //:   particular we want to demonstrate a base-line level of correct
        //:   operation of the following methods and operators:
        //:
        //:   o default and copy constructors (and also the destructor)
        //:
        //:   o the assignment operator (including aliasing)
        //:
        //:   o equality operators: 'operator==' and 'operator!='
        //:
        //:   o primary manipulators: 'push_back' and 'clear' methods
        //:
        //:   o basic accessors: 'size' and 'operator[]'
        //
        // Plan:
        //: 1 Create four objects using both the default and copy constructors.
        //:
        //: 2 Exercise these objects using
        //:
        //:   o primary manipulators
        //:
        //:   o basic accessors
        //:
        //:   o equality operators
        //:
        //:   o the assignment operator
        //:
        //: 3 Try aliasing with assignment for a non-empty instance [11] and
        //:   allow the result to leave scope, enabling the destructor to
        //:   assert internal object invariants.
        //:
        //: 4 Display object values frequently in verbose mode.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        if (verbose) printf("Standard value-semantic test.\n");

        RUN_EACH_TYPE(TestDriver1,
                      test01_breathingTest,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver1,
                      test01_breathingTest,
                      TTA,
                      TNA);

        // Disabled: we disabled this very infrequent usage for list (it will
        // be flagged by 'BSLMF_ASSERT'), which is illegal as of C++0x anyway:
        //..
        // bslma::TestAllocator oa(veryVeryVerbose);
        // bsl::allocator<int>  xoa(&oa);
        //
        // list<int, bsl::allocator<void*> > zz1, zz2(xoa);
        //..

        if (verbose) printf("Additional tests: misc.\n");

        bsl::list<char>                 myLst(5, 'a');
        bsl::list<char>::const_iterator citer;

        ASSERT(5 == myLst.size());

        for (citer = myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myLst);

        myLst.insert(myLst.begin(), 'z');

        ASSERT(6 == myLst.size());
        ASSERT('z' == myLst.front());

        for (citer = ++myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }

        if (verbose) P(myLst);

        myLst.erase(succ(myLst.begin(),2), succ(myLst.begin(),4));

        ASSERT(4 == myLst.size());
        ASSERT('z' == myLst.front());

        for (citer = succ(myLst.begin()); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }

        if (verbose) P(myLst);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        citer = myLst.insert(succ(myLst.begin(), 2), { 'd', 'e', 'f'});
        ASSERT('d' == *citer);
        ASSERT(succ(myLst.begin(), 2) == citer);
        ASSERT(7 == myLst.size());

        ASSERT(bsl::list<char>({'z', 'a', 'd', 'e', 'f', 'a', 'a'}) == myLst);
#endif

        bsl::list<bsl::list<char> > vv;
        vv.push_front(myLst);
        if (verbose) P(myLst);

        if (verbose) printf("Additional tests: traits.\n");

        ASSERT(  bslmf::IsBitwiseMoveable<bsl::list<char> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<bsl::list<TTA> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<bsl::list<bsl::list<int> > >::value);

        ASSERT(! bsl::is_trivially_copyable<bsl::list<char> >::value);
        ASSERT(! bsl::is_trivially_copyable<bsl::list<TTA> >::value);
        ASSERT(! bsl::is_trivially_copyable<bsl::list<
                                                    bsl::list<int> > >::value);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((0 == []() -> bsl::list<int> { return {}; }().size()));
            ASSERT((1 == []() -> bsl::list<int> { return {1}; }().size()));
            ASSERT((3 == []() -> bsl::list<int> {
                return {3, 1, 3};
            }().size()));
        }
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

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
