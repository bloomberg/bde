// bslstl_vector.2.t.cpp                                              -*-C++-*-
#define BSLSTL_VECTOR_0T_AS_INCLUDE
#include <bslstl_vector.0.t.cpp>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_vector.0.t.cpp'.

//=============================================================================
//                       TEST DRIVER TEMPLATE
//=============================================================================

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver2 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_VECTOR_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

                                 // TEST CASES
    // CLASS METHODS
    static void testCase23();
        // Test move constructor.

    static void testCase22();
        // Test overloaded new/delete.

    static void testCase21();
        // Test proper use of 'std::length_error'.

    static void testCase20();
        // Test comparison free operators.

    static void testCase19();
    static void testCase19_pop_back();
        // Test 'erase' and 'pop_back'.

    static void testCase19Negative();
        // Negative testing for 'erase' and 'pop_back'.

    template <class CONTAINER>
    static void testCase18Range(const CONTAINER&);
        // Test 'insert' member template for iterator ranges.

    static void testCase17_n_copies();
        // Test 'insert' 'n' copies.

    static void testCase17_push_back();
        // New test for 'push_back' method taking a reference to non-modifiable
        // 'value_type' that is consistent with other containers and can be
        // easily duplicated for version taking an rvalue reference.

    static void testCase17_insert_constref();
        // New test for 'insert' method taking a reference to non-modifiable
        // 'value_type' that is consistent with other containers and can be
        // easily duplicated for version taking an rvalue reference.

    static void testCase17Negative();
        // Negative testing for 'insert'.

    static void testCase16();
        // Test iterators.

    static void testCase15();
        // Test element access.

    static void testCase15Negative();
        // Negative test for element access.

    static void testCase14();
        // Test reserve and capacity-related methods.

    static void testCase14a();
        // Added test for 'resize(size_t newSize)' (missing from orig test).

    static void testCase13();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase13Range(const CONTAINER&);
        // Test 'assign' member template.

    template <class CONTAINER>
    static void testCase13Negative(const CONTAINER&);
        // Negative-test 'assign' members.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase12NoDefault();
        // Test user-supplied constructors that do not require the element type
        // to have a default constructor.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testCase12Ambiguity();
        // Test Initial-Range vs.-Length Ambiguity for vectors of integral(ish)
        // type.

    static void testCase12AmbiguityForPointers();
        // Test Initial-Range vs.-Length Ambiguity for vectors of pointers.
};

                  // ==================================
                  // template class StdBslmaTestDriver2
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver2 : public StdBslmaTestDriverHelper<TestDriver2, TYPE>
{
};

                    // ==============================
                    // template class MetaTestDriver2
                    // ==============================

template <class TYPE>
struct MetaTestDriver2 {
    // This 'struct' is to be call by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver' instantiated
    // with different types of allocator.

    static void testCase24();
        // Test move-assign.
};

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 A constant-time move, with no additional memory allocations or
    //:   deallocations, is performed when no allocator or the same allocator
    //:   as that of the original object is passed to the move constructor.
    //:
    //: 5 A linear operation, where each element is move-inserted into the
    //:   newly created object, is performed when an allocator that is
    //:   different than that of the original object is explicitly passed to
    //:   the move constructor.
    //:
    //: 6 The original object is always left in a valid state; the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 7 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 8 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    //:10 Any memory allocation is exception neutral.
    //:
    // Plan:
    //: 1 Specify a vector 'V' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator: 1) no allocator passed in, 2) a '0' is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 CONTAINER SPECIFIC NOTE: none
    //:
    //:   3 Where a constant-time move is expected, ensure that no memory was
    //:     allocated, that element addresses did not change, and that the
    //:     original object is left in the default state.         (C-3..5, C-7)
    //:
    //:   4 Where a linear-time move is expected, ensure that the move
    //:     constructor was called for each element.                   (C-6..7)
    //:
    //:   5 CONTAINER SPECIFIC: none
    //:
    //:   6 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                    (C-3,C-9)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state.   (C-8)
    //:
    //:   8 Verify all memory is released when the object is destroyed.  (C-11)
    //;
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                  (C-10)
    //
    // Testing:
    //   vector(bslmf::MovableRef<vector> original);
    //   vector(bslmf::MovableRef<vector> original, const A& allocator);
    // ------------------------------------------------------------------------


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    const TestValues VALUES;

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

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    if (verbose)
        printf("\tTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ": ", LENGTH);
                P(SPEC);
            }

            // Create control object ZZ with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mZZ(xscratch);
            const Obj&           ZZ = gg(&mZZ, SPEC);

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ti, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                ALLOC                xsa(&sa);
                ALLOC                xza(&za);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Z'.
                Obj        *srcPtr = new (fa) Obj(xsa);
                Obj&        mZ     = *srcPtr;
                const Obj&  Z      = gg(&mZ, SPEC);

                typename Obj::const_pointer pointers[2] = {}; // init avoids
                                                              // warning on gcc
                storeFirstNElemAddr(pointers,
                                    Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::TestAllocatorMonitor oam(&da);
                bslma::TestAllocatorMonitor sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                bool empty = 0 == ZZ.size();

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), ALLOC(0));
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xsa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xza);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator& oa  = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;
                ALLOC                 xoa(&oa);

                Obj& mX = *objPtr;      const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // CONTAINER SPECIFIC NOTE: For 'vector', the original object
                // is left with the same number of elements but with each
                // element in the 'moved-from' state, whatever that is.

                if (xsa == xoa) {
                    // 1. no memory allocation
                    ASSERTV(SPEC, CONFIG, xsa == xoa, oam.isTotalSame());

                    // 2. unchanged address of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(
                                          pointers,
                                          X,
                                          sizeof pointers / sizeof *pointers));
                    // 3. original object left empty
                    ASSERTV(SPEC, CONFIG, xsa == xoa, Z, 0 == Z.size());
                }
                else {
                    // 1. each element in original move-inserted
                    if (!k_IS_WELL_BEHAVED) {
                        ASSERTV(SPEC,
                                X.end() == TstMoveUtil::findFirstNotMovedInto(
                                                                     X.begin(),
                                                                     X.end()));
                    }

                    // 2. original object left with same size
                    ASSERTV(SPEC, CONFIG, xsa == xoa, Z, LENGTH == Z.size());

                    // 3. additional memory checks
                    ASSERTV(SPEC, CONFIG, xsa == xoa,
                            oam.isTotalUp() || empty);
                }

                // Verify that 'X', 'Z', and 'ZZ' have the correct allocator.
                ASSERTV(SPEC, CONFIG, xscratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG, xsa      ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG, xoa      ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.
                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                primaryManipulator(&mZ, 'Z');
                if (xsa == xoa) {
                    ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                    ASSERTV(SPEC, CONFIG, VALUES['Z' - 'A'] == Z[0]);
                }
                else {
                    ASSERTV(SPEC, CONFIG, Z, LENGTH + 1 == Z.size());
                    ASSERTV(SPEC, CONFIG, VALUES['Z' - 'A'] == Z[LENGTH]);
                }
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.
                primaryManipulator(&mX, 'Z');

                ASSERTV(SPEC, LENGTH + 1        == X.size());
                ASSERTV(SPEC, VALUES['Z' - 'A'] == X[LENGTH]);
                ASSERTV(SPEC, X                 != ZZ);

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.
                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }

    if (verbose)
        printf("\tTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mZZ(xscratch);
            const Obj&           ZZ = gg(&mZZ, SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ": ", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            ALLOC                xza(&za);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mWW(xza);  const Obj& WW = gg(&mWW, SPEC);

                const Obj X(bslmf::MovableRefUtil::move(mWW), xoa);
                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }
                ASSERTV(SPEC, ZZ == X);
                ASSERTV(SPEC, WW.size(), LENGTH == WW.size());
                ASSERTV(SPEC, WW.get_allocator() != X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = oa.numBlocksTotal();
            const bsls::Types::Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
            }

            if (LENGTH == 0) {
                ASSERTV(SPEC, BB + 0 == AA);
                ASSERTV(SPEC, B  + 0 == A );
            }
            else {
                const int TYPE_ALLOCS =
                    TYPE_ALLOC *
                       static_cast<int>(LENGTH + LENGTH * (1 + LENGTH) / 2);

                ASSERTV(SPEC, TYPE_ALLOC, 0 <= TYPE_ALLOC);
                ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                ASSERTV(SPEC, B + 0 == A);
            }
        }
    }
#endif  // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING OVERLOADED NEW/DELETE
    //
    // Concern:
    //: 1 Overloaded new is not used on construction.
    //:
    //: 2 Overloaded new is not used on insert.
    //:
    //: 3 Overloaded delete is not used on destruction.
    //
    // Plan:
    //: 1 Use a type with overloaded new and delete that will assert when
    //:   new/delete is called.
    //:
    //: 2 Construct vectors of that type with multiple elements in the
    //:   vector using different variations of the constructor.  Notice that
    //:   there is no need for other verification as the type should assert if
    //:   the overload new is used.
    //:
    //: 3 Insert an element into the vector.
    //:
    //: 4 Destroy any vectors that were created.
    //
    // Testing:
    //  CONCERN: Vector support types with overloaded new/delete
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    if (veryVerbose) printf(
     "\nTesting with vector(size_type, const VALUE_TYPE&, const ALLOCATOR&\n");
    {
        Obj mX(1, TYPE(), &ta);  const Obj& X = mX;
        ASSERT(TYPE() == X[0]);

        Obj mY(2, TYPE(), &ta);  const Obj& Y = mY;
        ASSERT(TYPE() == Y[0]);
        ASSERT(TYPE() == Y[1]);
    }

    if (veryVerbose) printf(
                       "\nTesting with other constructors and manipulators\n");
    {
        TYPE element;
        Obj mX(BSLS_UTIL_ADDRESSOF(element),
               BSLS_UTIL_ADDRESSOF(element) + 1,
               &ta);
        const Obj& X = mX;

        Obj mY(X, &ta);  const Obj& Y = mY;
        ASSERT(TYPE() == Y[0]);

        mX.push_back(TYPE());
        mX.resize(3);
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING 'std::length_error'
    //
    // Concerns:
    //   1) That any call to a constructor, 'assign', 'push_back' or 'insert'
    //      which would result in a value exceeding 'max_size()' throws
    //      'std::length_error'.
    //   2) That the 'max_size()' taken into consideration is that of the
    //      allocator, and not an absolute constant.
    //   3) That the value of the vector is unchanged if an exception is
    //      thrown.
    //   4) That integer overflows are correctly handled when 'length_error'
    //      exceeds 'Obj::max_size()' (which is the largest representable
    //      size_type).
    //
    // Plan:
    //   For concern 2, we use an allocator wrapper that provides the same
    //   functionality as 'ALLOC' but changes the return value of 'max_size()'
    //   to a 'limit' value settable at runtime.  Note that the operations
    //   throw unless 'length <= limit'.
    //
    //   Construct objects with value large enough that the constructor throws.
    //   For 'assign', 'insert', 'push_back', we construct a small (non-empty)
    //   object, and use the corresponding function to request an increase in
    //   size that is guaranteed to result in a value exceeding 'max_size()'.
    //
    // Testing:
    //   Proper use of 'std::length_error'
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    const TestValues VALUES;
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    LimitAllocator<ALLOC> la(&ta);
    la.setMaxSize((size_t)-1);

    const int LENGTH = 32;
    typedef vector<TYPE,LimitAllocator<ALLOC> > LimitObj;

    LimitObj        mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tConstructor 'vector(n, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        la.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, la);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit, exceptionCaught, (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tConstructor 'vector(n, T x, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        la.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, la);  // test here

            // If we succeeded, then limit must not have been exceeded.
            ASSERTV(LENGTH, limit, LENGTH <= limit);
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV( limit,  LENGTH,    exceptionCaught,
                (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tConstructor 'vector<Iter>(f, l, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        la.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), la);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERTV(limit, exceptionCaught, (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tWith 'resize'.\n");
    {
        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            la.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(la);

                mX.resize(LENGTH, DEFAULT_VALUE);
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit, exceptionCaught,
                    (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tWith 'assign'.\n");

    for (int assignMethod = 0; assignMethod <= 1; ++assignMethod) {

        if (veryVerbose) {
            switch (assignMethod) {
              case 0: {
                  printf("\tWith assign(n, T x).\n");
              } break;
              case 1: {
                  printf("\tWith assign<Iter>(f, l).\n");
              } break;
              default: {
                  ASSERT(0);
              }
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            la.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(la);

                switch (assignMethod) {
                  case 0: {
                      mX.assign(LENGTH, DEFAULT_VALUE);
                  } break;
                  case 1: {
                      mX.assign(Y.begin(), Y.end());
                  } break;
                  default: {
                      ASSERT(0);
                  }
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tWith 'insert'.\n");

    for (int insertMethod = 0; insertMethod <= 3; ++insertMethod) {

        if (verbose) {
            switch (insertMethod) {
              case 0: {
                  printf("\tWith push_back(c).\n");
              } break;
              case 1: {
                  printf("\tWith insert(p, T x).\n");
              } break;
              case 2: {
                  printf("\tWith insert(p, n, T x).\n");
              } break;
              case 3: {
                  printf("\tWith insert<Iter>(p, f, l).\n");
              } break;
              default: {
                  ASSERT(0);
              }
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            la.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(la); const LimitObj& X = mX;

                switch (insertMethod) {
                  case 0: {
                      for (int i = 0; i < LENGTH; ++i) {
                          mX.push_back(Y[i]);
                      }
                  } break;
                  case 1: {
                      for (int i = 0; i < LENGTH; ++i) {
                          mX.insert(X.begin(), DEFAULT_VALUE);
                      }
                  } break;
                  case 2: {
                      mX.insert(X.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  case 3: {
                      mX.insert(X.begin(), Y.begin(), Y.end());
                  } break;
                  default: {
                      ASSERT(0);
                  }
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            ASSERTV(limit, exceptionCaught,
                    (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    const int PADDING = 16;

    std::size_t        expMaxSize = -1;
    const std::size_t& EXP_MAX_SIZE = expMaxSize;
    {
        const Obj X;
        expMaxSize = X.max_size();
    }
    ASSERTV(EXP_MAX_SIZE, (size_t)-1 > EXP_MAX_SIZE);

    if (EXP_MAX_SIZE >= (size_t)-1) {
        printf("\n\nERROR: Cannot continue this test case without attempting\n"
               "to allocate huge amounts of memory.  *** Aborting. ***\n\n");
        return;                                                       // RETURN
    }

    const std::size_t DATA[] = {
        EXP_MAX_SIZE + 1,
        EXP_MAX_SIZE + 2,
        EXP_MAX_SIZE / 2 + (size_t)-1 / 2,
        (size_t)-2,
        (size_t)-1,
        0  // must be the last value
    };

    if (verbose) printf("\tConstructor 'vector(n, T x, a = A())'"
                        " and 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)
    {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tWith 'n' = " ZU "\n", DATA[i]);

        try {
            Obj mX(DATA[i], DEFAULT_VALUE);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tWith 'reserve/resize' and"
                        " 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int capacityMethod = 0; capacityMethod <= 2; ++capacityMethod)
    {
        if (verbose) {
            switch (capacityMethod) {
              case 0: {
                  printf("\tWith reserve(n).\n");
              } break;
              case 1: {
                  printf("\tWith resize(n).\n");
              } break;
              case 2: {
                  printf("\tWith resize(n, T x).\n");
              } break;
              default: {
                  ASSERT(0);
              }
            };
        }

        for (int i = 0; DATA[i]; ++i)
        {
            bool exceptionCaught = false;

            if (veryVerbose) printf("\t\tWith 'n' = " ZU "\n", DATA[i]);

            try {
                Obj mX;

                switch (capacityMethod) {
                  case 0: {
                      mX.reserve(DATA[i]);
                  } break;
                  case 1: {
                      mX.resize(DATA[i]);
                  } break;
                  case 2: {
                      mX.resize(DATA[i], DEFAULT_VALUE);
                  } break;
                  default: {
                      ASSERT(0);
                  }
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
                ASSERT(0);
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());

    if (verbose) printf("\tWith 'insert' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)  {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tCreating vector of length " ZU ".\n",
                                DATA[i]);

        try {
            Obj mX(PADDING, DEFAULT_VALUE, la);  const Obj& X = mX;

            mX.insert(X.begin(), DATA[i] - PADDING, DEFAULT_VALUE);
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n",
                       e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBytesInUse());
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING FREE COMPARISON OPERATORS
    //
    // Concerns:
    //   1) 'operator<' returns the lexicographic comparison on two arrays.
    //   2) 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //      'operator<'.
    //   3) 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //   4) That traits get selected properly.
    //
    // Plan:
    //   For a variety of vectors of different sizes and different values, test
    //   that the comparison returns as expected.  Note that capacity is not of
    //   concern here, the implementation specifically uses only 'begin()',
    //   'end()', and 'size()'.
    //
    // Testing:
    //   bool operator<(const vector<T,A>&, const vector<T,A>&);
    //   bool operator>(const vector<T,A>&, const vector<T,A>&);
    //   bool operator<=(const vector<T,A>&, const vector<T,A>&);
    //   bool operator>=(const vector<T,A>&, const vector<T,A>&);
    //   auto operator<=>(const vector<T,A>&, const vector<T,A>&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "AAA",
        "AAAA",
        "AAAAA",
        "AAAAAA",
        "AAAAAAA",
        "AAAAAAAA",
        "AAAAAAAAA",
        "AAAAAAAAAA",
        "AAAAAAAAAAA",
        "AAAAAAAAAAAA",
        "AAAAAAAAAAAAA",
        "AAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAA",
        "AAAAAB",
        "AAAAABA",
        "AAAAABAA",
        "AAAAABAAA",
        "AAAAABAAAA",
        "AAAAABAAAAA",
        "AAAAABAAAAAA",
        "AAAAABAAAAAAA",
        "AAAAABAAAAAAAA",
        "AAAAABAAAAAAAAA",
        "AAAAB",
        "AAAABAAAAAA",
        "AAAABAAAAAAA",
        "AAAABAAAAAAAA",
        "AAAABAAAAAAAAA",
        "AAAABAAAAAAAAAA",
        "AAAB",
        "AAABA",
        "AAABAA",
        "AAABAAAAAA",
        "AAB",
        "AABA",
        "AABAA",
        "AABAAA",
        "AABAAAAAA",
        "AB",
        "ABA",
        "ABAA",
        "ABAAA",
        "ABAAAAAA",
        "B",
        "BA",
        "BAA",
        "BAAA",
        "BAAAA",
        "BAAAAA",
        "BAAAAAA",
        "BB",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    {
        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            const char *const U_SPEC = SPECS[si];

            Obj mU;     const Obj& U = gg(&mU, U_SPEC);

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; sj < NUM_SPECS; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                Obj mV;     const Obj& V = gg(&mV, V_SPEC);

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                const bool isLess = si < sj;
                const bool isLessEq = si <= sj;
                ASSERTV(si, sj,  isLess   == (U < V));
                ASSERTV(si, sj, !isLessEq == (U > V));
                ASSERTV(si, sj,  isLessEq == (U <= V));
                ASSERTV(si, sj, !isLess   == (U >= V));
#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                const auto cmp = U <=> V;
                LOOP2_ASSERT(si, sj,  isLess   == (cmp < 0));
                LOOP2_ASSERT(si, sj, !isLessEq == (cmp > 0));
                LOOP2_ASSERT(si, sj,  isLessEq == (cmp <= 0));
                LOOP2_ASSERT(si, sj, !isLess   == (cmp >= 0));
#endif
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase19_pop_back()
{
    // ------------------------------------------------------------------------
    // TESTING 'pop_back'
    //
    // Concerns:
    //: 1 The resulting value is correct, i.e., the final element is removed,
    //:   and no other element is changed or lost.
    //: 2 'pop_back()' never throws (when called within contract).
    //: 3 No memory is leaked, i.e., allocator-aware elements correctly release
    //:   their own storage.
    //: 4 Avoidable undefined behavior is trapped in appropriate build modes.
    //:   i.e., calling pop_back() on an empty 'vector'.
    //: 5 'pop_back' invalidates only iterators that refer to the erased
    //:   element, and no other iterators into this container.
    //: 6 'pop_back' does not require more of the element type than the minimum
    //:   set of requirements in the standard.  In particular, 'pop_back' does
    //:   not require assignability.
    //
    // Plan:
    //:  1 Create an empty vector, installing a test allocator, and reserve a
    //:    capacity at least as large as needed for following steps.
    //:  2 Populate the vector with a distinct pattern of values that can be
    //:    easily verified.
    //:  3 Call 'pop_back' to erase just the last element.
    //:  4 Verify that the vector is one element shorter than before the call.
    //:  5 Verify that capacity has not changed.
    //:  6 Verify each remaining element of the vector has the same value as
    //:    before the call.  (C-1)
    //:  7 Verify total number of allocations has not changed, and that there
    //:    has been exactly one deallocation if and only if the element type
    //:    uses our allocator, otherwise no memory is released. (C-3)
    //:  8 Create an empty vector, and install a negative test guard.
    //:  9 Verify that calling 'pop_back' on the empty vector triggers an
    //:    assert check in 'SAFE' builds. (C-4)
    //: 10 Push an element onto the vector, and verify that 'pop_back' can be
    //:    called without asserting (in any build mode).
    //: 11 Verify that calling 'pop_back' on the re-empted vector triggers an
    //:    assert check in 'SAFE' builds. (C-4)
    //: 12 As 'vector::iterator' is just a pointer, iterators will not be
    //:    invalidated as long as 'begin' does not change value, so verify that
    //:    'begin' gives the same result before and after each call to
    //:    'pop_back' (C-5).
    //: 13 CONCERN: If any exceptions are thrown, they will not be caught and
    //:    the test case will fail implicitly.  This addresses (C-2).
    //: 14 CONCERN: Ensure that no operation in the test case requires any
    //:    operation on the vector element type than mandated by the standard.
    //:    Building this test driver for awkward minimal types will satisfy
    //:    (C-6).
    //
    // Testing:
    //   void pop_back();
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

    enum {
        TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value ||
                        bsl::uses_allocator<TYPE, ALLOC>::value
    };

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'pop_back' on non-empty vectors.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE    = DATA[i].d_lineNum;
            const size_t INIT_LENGTH  = DATA[i].d_length;
            const size_t FINAL_LENGTH = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                Obj mX(xta);    const Obj& X = mX;
                mX.reserve(INIT_CAP);

                for (size_t k = 0; k != INIT_LENGTH; ++k) {
                    mX.push_back(VALUES[(k + INIT_LINE) % NUM_VALUES]);
                }

                const bsls::Types::Int64 BB = ta.numBlocksTotal();
                const bsls::Types::Int64 B  = ta.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                typename Obj::iterator it = mX.begin();

                mX.pop_back();

                const bsls::Types::Int64 AA = ta.numBlocksTotal();
                const bsls::Types::Int64 A  = ta.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(INIT_LINE, FINAL_LENGTH, INIT_CAP,
                                                 FINAL_LENGTH == X.size());
                ASSERTV(INIT_LINE, FINAL_LENGTH, INIT_CAP,
                                                 INIT_CAP == X.capacity());

                for (size_t k = 0; k != FINAL_LENGTH; ++k) {
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, k,
                            VALUES[(k + INIT_LINE) % NUM_VALUES] == X[k]);
                }

                ASSERT(X.begin() == it);

                ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, BB == AA);
                ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             B - TYPE_ALLOC == A );
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (veryVerbose) printf("\tNegative testing of 'pop_back'.\n");
    {
        Obj mX;

        bsls::AssertTestHandlerGuard guard;

        // pop_back on empty vector
        ASSERT_SAFE_FAIL_RAW(mX.pop_back());

        // set the vector 'mX' to a non-empty state, and demonstrate that it
        // does not assert when calling 'pop_back' until the vector is restored
        // to an empty state.
        mX.push_back(VALUES[0]);

        ASSERT_SAFE_PASS_RAW(mX.pop_back());
        ASSERT_SAFE_FAIL_RAW(mX.pop_back());
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // We have the following concerns:
    //   1) That the resulting value is correct.
    //   2) That erasing a suffix of the array never allocates, and thus never
    //      throws.  In particular, 'erase(..., X.end())' does not throw.
    //   3) That erasing is exception neutral w.r.t. memory allocation.
    //   4) That no memory is leaked.
    //   5) That avoidable undefined behavior is trapped in appropriate build
    //      modes.  Avoidable undefined behavior might be:
    //      i)  Calling 'erase(const_iterator)' with an invalid iterator, a
    //          non-dereferenceable iterator, or an iterator into a different
    //          container.
    //      ii) Calling 'erase(const_iterator, const_iterator)' with an
    //          invalid range, or a non-empty range that is not entirely
    //          contained within this vector, or an empty range where the
    //          iterator demarcating the range does is not 'this->end()' and
    //          does not otherwise point into this vector.
    //   6) The erase function invalidates (in appropriate build modes) all
    //      iterators into this container that refer to the erased elements, to
    //      any succeeding elements, to copies of the 'end' iterator, and no
    //      other iterators into this container.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a vector
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but not computing the number of allocations or checking on the
    //        value in case an exception is thrown (it is enough to verify that
    //        all the elements have been destroyed indirectly by making sure
    //        that there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    enum {
        TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value ||
                        bsl::uses_allocator<TYPE, ALLOC>::value
    };

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        k_IS_MOVABLE);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'erase(pos)' on non-empty vectors.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                    const Obj& X = mX;

                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore: "); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + POS);

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAfter : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                            LENGTH == X.size());
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                            INIT_CAP == X.capacity());

                    for (m = 0; m < POS; ++m) {
                        ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                VALUES[(m + 1) % NUM_VALUES] == X[m]);
                    }

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                                   TYPE_ALLOC && !k_IS_MOVABLE
                                                   ? LENGTH - POS
                                                   : 0;
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                            BB + TYPE_ALLOCS == AA);
                    ASSERTV(bsls::NameOf<TYPE>(), BB, TYPE_ALLOCS, AA,
                            BB + TYPE_ALLOCS == AA);
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                            B - TYPE_ALLOC == A );
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                        const bsls::Types::Int64 AL = ta.allocationLimit();
                        ta.setAllocationLimit(-1);

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ta.setAllocationLimit(AL);

                        mX.erase(X.begin() + POS);  // test erase here

                        for (m = 0; m < POS; ++m) {
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                    VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                    VALUES[(m + 1) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tTesting 'erase(first, last)'.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j <  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const int    NUM_ELEMENTS = static_cast<int>(
                                                          END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                    const Obj& X = mX;

                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP,
                            NUM_ELEMENTS, LENGTH == X.size());
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP,
                            NUM_ELEMENTS, INIT_CAP == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        ASSERTV(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        ASSERTV(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                          TYPE_ALLOC && !k_IS_MOVABLE &&
                                                                   NUM_ELEMENTS
                                          ? INIT_LENGTH - END_POS
                                          : 0;
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                            BB + TYPE_ALLOCS == AA);
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                            B - NUM_ELEMENTS * TYPE_ALLOC == A );
                }
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j <  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                        const bsls::Types::Int64 AL = ta.allocationLimit();
                        ta.setAllocationLimit(-1);

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        ta.setAllocationLimit(AL);

                        mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);
                                                             // test erase here

                        for (m = 0; m < BEGIN_POS; ++m) {
                            ASSERTV(
                                  INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS, m,
                                  VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            ASSERTV(
                              INIT_LINE, INIT_LENGTH, INIT_CAP,END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase19Negative()
{
    // ------------------------------------------------------------------------
    // NEGATIVE TESTING ERASE
    //
    // Concerns:
    //   1 'pop_back' asserts on undefined behavior when the vector is empty,
    //   2 'erase' asserts on undefined behavior when iterators are not valid
    //   on the string being tested or they don't make a valid range.
    //
    // Plan:
    //   For concern (1), create an empty vector and call 'pop_back' which
    //   should assert.  Then 'push_back' a default-constructed element, and
    //   show a subsequent 'pop_back' no longer asserts.  Finally, call
    //   'pop_back' one more time on the now-empty container, and show that it
    //   asserts once again.
    //   For concern (2), create a non-empty vector and test 'erase' with
    //   different combinations of invalid iterators and iterator ranges.
    //
    // Testing:
    //   void pop_back();
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    if (veryVerbose) printf("\tNegative testing of 'erase(iterator)'.\n");
    {
        Obj mX;
        gg(&mX, "ABCDE");

        bsls::AssertTestHandlerGuard guard;

        // position < begin()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.begin() - 1));

        // It is safe to call 'erase' on the boundaries of the range
        // [begin, end)
        ASSERT_SAFE_PASS_RAW(mX.erase(mX.begin()));
        ASSERT_SAFE_PASS_RAW(mX.erase(mX.end() - 1));

        // position >= end()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.end()));
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.end() + 1));
    }

    if (veryVerbose)
        printf("\tNegative testing of 'erase(iterator, iterator)'\n");
    {
        Obj mX;
        gg(&mX, "ABCDE");

        bsls::AssertTestHandlerGuard guard;

        // first < begin()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.begin() - 1, mX.end()));

        // last > end()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.begin(), mX.end() + 1));

        // first > last
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.end(), mX.begin()));
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.begin() + 1, mX.begin()));
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.end(), mX.end() - 1));

        // first > end()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.end() + 1, mX.end()));

        // last < begin()
        ASSERT_SAFE_FAIL_RAW(mX.erase(mX.begin(), mX.begin() - 1));

        ASSERT_SAFE_PASS_RAW(mX.erase(mX.begin(), mX.begin()));
        ASSERT_SAFE_PASS_RAW(mX.erase(mX.end(), mX.end()));
        ASSERT_SAFE_PASS_RAW(mX.erase(mX.begin()+1, mX.end()-1));
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase17_n_copies()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the 'insert' return (if any) value is a valid iterator, even
    //      when the vector underwent a reallocation.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   7) The move 'push_back' and 'insert' move the value, capacity, and
    //      allocator correctly, and without performing any allocation.
    //   8) That inserting a 'const T& value' that is a reference to an element
    //      of the vector does not suffer from aliasing problems.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' at various
    //   positions, or a variable number of copies of this value.  Perform the
    //   above tests:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there a change in capacity, 0 otherwise
    //      - 1 if the type uses an allocator and the value is an alias.
    //
    //   For concerns 4 and 7, we test with a bitwise-moveable type that the
    //   only allocation for a move 'push_back' or 'insert' is the one for the
    //   vector reallocation (if capacity changes; all elements are moved), and
    //   for insertion the only reallocations should be for the new elements
    //   plus one if the vector undergoes a reallocation (capacity changes).
    //
    //   For concern 8, we insert an element of some vector where all the
    //   values are distinct into the same vector, taking care of the cases
    //   where the reference is before or after the position of insertion, and
    //   that the vector undergoes a reallocation or not (i.e., capacity
    //   changes or not).  We verify that the value is as expected, i.e.,
    //   identical to t it would be if the value had not been aliased.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   void insert(const_iterator pos, size_type n, const T& value);
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];
    enum {
        TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value ||
                        bsl::uses_allocator<TYPE, ALLOC>::value
    };

    const size_t AP = allocationsByPrimaryManipulator();
        // Allocations per element inserted by primary manipulator, assuming
        // there is sufficient capacity.

    if (verbose) printf(
            "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, AP = " ZU ").\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        k_IS_MOVABLE,
                        AP);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'insert'.\n");

    if (verbose) printf("\tUsing a single 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                    const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                     ? 0
                                     : LENGTH <= INIT_CAP
                                     ? -1
                                     : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    iterator result = mX.insert(X.begin() + POS, VALUE);

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(INIT_LINE, i, j, LENGTH == X.size());
                    if (CAP != (size_t) -1) {
                        ASSERTV(INIT_LINE, i, j, CAP == X.capacity());
                    }
                    ASSERTV(INIT_LINE, i, j, X.begin() + POS == result);

                    for (k = 0; k < POS; ++k) {
                        ASSERTV(INIT_LINE, LENGTH, POS, k,
                                VALUES[k % NUM_VALUES] == X[k]);
                    }
                    ASSERTV(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    for (++k; k < LENGTH; ++k) {
                        ASSERTV(INIT_LINE, LENGTH, POS, k,
                                VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                  TYPE_ALLOC && !k_IS_MOVABLE
                                  ? (REALLOC ? INIT_LENGTH : INIT_LENGTH - POS)
                                  : 0;
                    const bsls::Types::Int64 EXP_ALLOCS =
                                            REALLOC + TYPE_ALLOCS + TYPE_ALLOC;

                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, TYPE_ALLOCS,
                            BB + EXP_ALLOCS == AA);
                    ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                            B + (0 == INIT_CAP) + TYPE_ALLOC == A );
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                         ? 0
                                         : LENGTH <= INIT_CAP
                                         ? -1
                                         : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const bsls::Types::Int64 BB = ta.numBlocksTotal();
                        const bsls::Types::Int64 B  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);

                        const bsls::Types::Int64 AA = ta.numBlocksTotal();
                        const bsls::Types::Int64 A  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                        if (static_cast<size_t>(-1) != CAP) {
                            ASSERTV(INIT_LINE, LINE, i, j,
                                    CAP == X.capacity());
                        }

                        size_t m = 0;
                        for (k = 0; k < POS; ++k) {
                            ASSERTV(INIT_LINE, LINE, j, k,
                                    VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (; k < POS + NUM_ELEMENTS; ++k) {
                            ASSERTV(INIT_LINE, LINE, j, k, VALUE == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            ASSERTV(INIT_LINE, LINE, j, k, m,
                                    VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;

                        // Have to separate const's from vars in boolean
                        // operations or we get these really stupid warnings
                        // from g++.

                        const bsls::Types::Int64 TYPE_ALLOCS =
                                              !TYPE_ALLOC || k_IS_MOVABLE
                                              ? 0
                                              : 0 == NUM_ELEMENTS
                                                ? 0
                                                : REALLOC ? INIT_LENGTH
                                                          : INIT_LENGTH - POS;

                        const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                        ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                BB + EXP_ALLOCS == AA);
                        ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                B + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC == A );
                    }
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    if (4 < LENGTH && NUM_DATA-1 != i && NUM_DATA-1 != ti) {
                        continue;
                    }

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                            const bsls::Types::Int64 AL = ta.allocationLimit();
                            ta.setAllocationLimit(-1);

                            Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                            const Obj& X = mX;
                            mX.reserve(INIT_CAP);

                            const size_t CAP = (0 == INIT_CAP && 0 == LENGTH)
                                             ? 0
                                             : LENGTH <= INIT_CAP
                                             ? -1
                                             : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ta.setAllocationLimit(AL);

                            mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                            if ((size_t) -1 != CAP) {
                                ASSERTV(INIT_LINE, LINE, i, j,
                                        CAP == X.capacity());
                            }

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        VALUE == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");
    {
        // insert a single element
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        const size_t INDEX = h;

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X); const Obj& Y = mY;  // control

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using value at "); P(INDEX);
                        }

                        mY.insert(Y.begin() + POS, X[INDEX]);  // control
                        mX.insert(X.begin() + POS, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        ASSERTV(INIT_LINE, INIT_CAP, POS, INDEX, X, Y, X == Y);
                    }
                }
            }
        }

        // insert 'n; copies within capacity
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        const size_t INDEX = h;

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X); const Obj& Y = mY;  // control

                        ASSERTV(INIT_LINE, INIT_CAP, POS, INDEX, X, Y, X == Y);

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using enough of value at "); P_(INDEX);
                            printf(" to reach capacity.\n");
                        }

                        size_t EXTRA = X.capacity() - X.size();

                        mY.insert(Y.begin() + POS, EXTRA, X[INDEX]); // control
                        mX.insert(X.begin() + POS, EXTRA, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        ASSERTV(INIT_LINE, INIT_CAP, POS, INDEX, X, Y, X == Y);
                    }
                }
            }
        }

        // insert 'n; copies while overflowing capacity
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        const size_t INDEX = h;

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X); const Obj& Y = mY;  // control

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using 0 copies of value at "); P(INDEX);
                        }

                        mY.insert(Y.begin() + POS, 0, X[INDEX]);  // control
                        mX.insert(X.begin() + POS, 0, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        ASSERTV(INIT_LINE, INIT_CAP, POS, INDEX, X, Y, X == Y);

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using enough of value at "); P_(INDEX);
                            printf(" to exceed capacity.\n");
                        }

                        size_t EXTRA = X.capacity() - X.size() + 1;

                        mY.insert(Y.begin() + POS, EXTRA, X[INDEX]); // control
                        mX.insert(X.begin() + POS, EXTRA, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        ASSERTV(INIT_LINE, INIT_CAP, POS, INDEX, X, Y, X == Y);
                    }
                }
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase17_push_back()
{
    // ------------------------------------------------------------------------
    // TESTING 'push_back(const T&)'
    //
    // Concerns:
    //: 1 A new element is added to the end of the container and the order of
    //:   the container remains correct.
    //:
    //: 2 The capacity is increased as expected.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For 'push_back' we will create objects of varying sizes and
    //:   capacities containing default values, and insert a 'value'.
    //:
    //:   1 Verify that the element was added to the end of the container.(C-1)
    //:
    //:   2 Compute the number of allocations and verify it is as expected.
    //:                                                                   (C-2)
    //:
    //:   3 Verify all allocations are from the object's allocator.       (C-3)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-4)
    //
    // Testing:
    //   void push_back(const value_type& value);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        k_IS_MOVABLE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec                element  results
        //----  ------------------- -------  -------------------
        { L_,   "",                 'A',     "A"                 },
        { L_,   "A",                'A',     "AA"                },
        { L_,   "A",                'B',     "AB"                },
        { L_,   "B",                'A',     "BA"                },
        { L_,   "AB",               'A',     "ABA"               },
        { L_,   "BC",               'D',     "BCD"               },
        { L_,   "BCA",              'Z',     "BCAZ"              },
        { L_,   "CAB",              'C',     "CABC"              },
        { L_,   "CDAB",             'D',     "CDABD"             },
        { L_,   "DABC",             'Z',     "DABCZ"             },
        { L_,   "ABCDE",            'Z',     "ABCDEZ"            },
        { L_,   "EDCBA",            'E',     "EDCBAE"            },
        { L_,   "ABCDEA",           'E',     "ABCDEAE"           },
        { L_,   "ABCDEAB",          'Z',     "ABCDEABZ"          },
        { L_,   "BACDEABC",         'D',     "BACDEABCD"         },
        { L_,   "CBADEABCD",        'Z',     "CBADEABCDZ"        },
        { L_,   "CBADEABCDAB",      'B',     "CBADEABCDABB"      },
        { L_,   "CBADEABCDABC",     'Z',     "CBADEABCDABCZ"     },
        { L_,   "CBADEABCDABCDE",   'B',     "CBADEABCDABCDEB"   },
        { L_,   "CBADEABCDABCDEA",  'E',     "CBADEABCDABCDEAE"  },
        { L_,   "CBADEABCDABCDEAB", 'Z',     "CBADEABCDABCDEABZ" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    if (verbose) printf("\tTesting 'push_back' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, xoa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.push_back(VALUES[ELEMENT - 'A']);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));

                // Vector increases capacity only if the current length is zero
                // or a power of 2.  In addition, when the type allocates, an
                // extra allocation is used for the new element, and when the
                // type is not bitwise moveable, size() allocations are used
                // during the move, but an equal amount is destroyed thus the
                // number of blocks in use is unchanged.

                const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                       TYPE_ALLOC * (1 + SIZE * !k_IS_MOVABLE);
                if (expectToAllocate(SIZE)) {
                    ASSERTV(LINE, CONFIG, BB, AA,
                            BB + 1 + TYPE_ALLOC_MOVES == AA);
                }
                else {
                    ASSERTV(LINE, CONFIG, BB, AA, BB + 0 + TYPE_ALLOC == AA);
                }

                ASSERTV(LINE, CONFIG, B, A,
                        B + (SIZE == 0) + TYPE_ALLOC ==  A);
            }
        }
    }

    if (verbose) printf("\tTesting 'push_back' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            if (4 < SIZE && NUM_DATA-1 != ti) {
                continue;
            }

            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    mX.push_back(VALUES[ELEMENT - 'A']);

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

    if (verbose) printf("\tTesting 'push_back' for alias-safety.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        Obj                  mX(xoa);
        mX.reserve(8);
        const Obj&           X = gg(&mX, "ABCDE");  // 5 elements will avoid
                                                    // reallocation below.

        // Verify any attribute allocators are installed properly.

        ASSERTV(xoa == X.get_allocator());
        ASSERTV(X.size(), 5 == X.size());
        ASSERTV(X.capacity(), 8 == X.capacity());

        if (veryVerbose) printf("\t\talias-safety of 'front'\n");
        {
            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X.front());
            mX.push_back(X.front());

            ASSERTV(X, Y, X == Y);
        }

        if (veryVerbose) printf("\t\talias-safety of 'back'\n");
        {
            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X.back());
            mX.push_back(X.back());

            ASSERTV(X, Y, X == Y);
        }

        if (veryVerbose) printf("\t\talias-safety of a middle position\n");
        {
            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X[2]);
            mX.push_back(X[2]);

            ASSERTV(X, Y, X == Y);
        }
    }

    if (verbose) printf("\tTesting 'push_back' for alias-safety on growth.\n");
    {
        if (veryVerbose) printf("\t\talias-safety of 'front'\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = gg(&mX, "ABCD");  // 4 elements will force
                                                       // reallocation below.

            // Verify any attribute allocators are installed properly.

            ASSERTV(xoa == X.get_allocator());
            ASSERTV(X.size(), X.capacity(), X.size() == X.capacity());

            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X.front());
            mX.push_back(X.front());

            ASSERTV(X, Y, X == Y);
        }

        if (veryVerbose) printf("\t\talias-safety of 'back'\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = gg(&mX, "ABCD");  // 4 elements will force
                                                       // reallocation below.

            // Verify any attribute allocators are installed properly.

            ASSERTV(xoa == X.get_allocator());
            ASSERTV(X.size(), X.capacity(), X.size() == X.capacity());

            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X.back());
            mX.push_back(X.back());

            ASSERTV(X, Y, X == Y);
        }

        if (veryVerbose) printf("\t\talias-safety of a middle position\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = gg(&mX, "ABCD");  // 4 elements will force
                                                       // reallocation below.

            // Verify any attribute allocators are installed properly.

            ASSERTV(xoa == X.get_allocator());
            ASSERTV(X.size(), X.capacity(), X.size() == X.capacity());

            Obj mY(X, xoa);  const Obj& Y = mY;

            mY.push_back(X[2]);
            mX.push_back(X[2]);

            ASSERTV(X, Y, X == Y);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase17_insert_constref()
{
    // ------------------------------------------------------------------------
    // TESTING 'insert(const_iterator position, const T&)'
    //
    // Concerns:
    //: 1 A new element is inserted at the correct position in the container
    //:   and the order of elements in the container, before and after the
    //:   insertion point, remain correct.
    //:
    //: 2 The capacity is increased as expected.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For 'insert' we will create objects of varying sizes and
    //:   capacities containing default values, and insert a 'value' at
    //:   various positions.
    //:
    //:   1 Verify that the element was added at the expected position in the
    //:     container.(C-1)
    //:
    //:   2 Ensure that the order is preserved for elements before and after
    //:     the insertion point.
    //:
    //:   3 Compute the number of allocations and verify it is as expected.
    //:                                                                   (C-2)
    //:
    //:   4 Verify all allocations are from the object's allocator.       (C-3)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-4)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int IS_NOT_MOVABLE = !k_IS_MOVABLE;
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
                    "\nTesting '%s' (TYPE_ALLOC = %d, IS_NOT_MOVABLE = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC,
                        IS_NOT_MOVABLE);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        int         d_pos;      // position to insert
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec         pos  element  results
        //----  -----------  ---  -------  -----------
        { L_,   "",          -1,  'Z',     "Z"         },
        { L_,   "",          99,  'Z',     "Z"         },
        { L_,   "A",         -1,  'Z',     "ZA"        },
        { L_,   "A",          0,  'Z',     "ZA"        },
        { L_,   "A",          1,  'Z',     "AZ"        },
        { L_,   "A",         99,  'Z',     "AZ"        },
        { L_,   "AB",        -1,  'B',     "BAB"       },
        { L_,   "AB",         0,  'B',     "BAB"       },
        { L_,   "AB",         1,  'Z',     "AZB"       },
        { L_,   "AB",         2,  'A',     "ABA"       },
        { L_,   "AB",        99,  'Z',     "ABZ"       },
        { L_,   "CAB",       -1,  'Z',     "ZCAB"      },
        { L_,   "CAB",        0,  'A',     "ACAB"      },
        { L_,   "CAB",        1,  'B',     "CBAB"      },
        { L_,   "CAB",        2,  'C',     "CACB"      },
        { L_,   "CAB",        3,  'Z',     "CABZ"      },
        { L_,   "CAB",       99,  'Z',     "CABZ"      },
        { L_,   "CABD",      -1,  'A',     "ACABD"     },
        { L_,   "CABD",       0,  'Z',     "ZCABD"     },
        { L_,   "CABD",       1,  'Z',     "CZABD"     },
        { L_,   "CABD",       2,  'B',     "CABBD"     },
        { L_,   "CABD",       3,  'Z',     "CABZD"     },
        { L_,   "CABD",       4,  'B',     "CABDB"     },
        { L_,   "CABD",      99,  'A',     "CABDA"     },
        { L_,   "HGFEDCBA",   0,  'Z',     "ZHGFEDCBA" },
        { L_,   "HGFEDCBA",   7,  'Z',     "HGFEDCBZA" },
        { L_,   "HGFEDCBA",   8,  'Z',     "HGFEDCBAZ" }
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, xoa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                size_t index = POS == -1 ? 0
                             : POS == 99 ? X.size()
                             :             POS;
                mX.insert(X.begin() + index, VALUES[ELEMENT - 'A']);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));

                // Vector increases capacity only if the current length is zero
                // or a power of 2.  In addition, when the type allocates, an
                // extra allocation is used for the new element, and when the
                // type is not bitwise moveable, size() allocations are used
                // during the move, but an equal amount is destroyed thus the
                // number of blocks in use is unchanged.

                if (expectToAllocate(SIZE)) {
                    const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                      TYPE_ALLOC * (1 + SIZE * IS_NOT_MOVABLE);
                    ASSERTV(LINE, CONFIG, BB, AA,
                            BB + 1 + TYPE_ALLOC_MOVES == AA);
                }
                else {
                    const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                            TYPE_ALLOC * (1 + (SIZE - index) * IS_NOT_MOVABLE);
                    ASSERTV(LINE, CONFIG, BB, AA,
                            BB + 0 + TYPE_ALLOC_MOVES == AA);
                }

                ASSERTV(LINE, CONFIG, B, A,
                        B + (SIZE == 0) + TYPE_ALLOC ==  A);
            }
        }
    }

    // TBD: There is no strong exception guarantee when the copy constructor
    // throws during 'insert' of a single element...

    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            if (4 < SIZE && NUM_DATA-1 != ti) {
                continue;
            }

            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                // TBD: since there is no strong exception guarantee, the
                //      following two lines moved to within the exception test
                //      block
                // Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);
                // ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    // TBD: no strong exception guarantee
                    // ExceptionProctor<Obj> guard(&X, L_, &scratch);

                    const bsls::Types::Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    size_t index = POS == -1 ? 0
                                 : POS == 99 ? X.size()
                                 :             POS;

                    oa.setAllocationLimit(AL);

                    mX.insert(X.begin() + index, VALUES[ELEMENT - 'A']);

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    // guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE, ALLOC>::testCase18Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the initial range is correctly imported and then moved if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes with different
    //   'value' as argument.  Perform the above tests:
    //      - From the parameterized 'CONTAINER::const_iterator'.
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there is a change in capacity, 0 otherwise
    //      - 1 if the type uses an allocator and the value is an alias.
    //      -
    //   For concern 4, we test with a bitwise-moveable type that the only
    //   reallocations are for the new elements plus one if the vector
    //   undergoes a reallocation (capacity changes).
    //
    //   template <class InputIter>
    //   iterator insert(const_iterator pos, InputIter first, InputIter last);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;  // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    const int TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value;

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                   length
        //----  -------------------    ------
        { L_,   ""                  }, // 0
        { L_,   "A"                 }, // 1
        { L_,   "AB"                }, // 2
        { L_,   "ABC"               }, // 3
        { L_,   "ABCD"              }, // 4
        { L_,   "ABCDE"             }, // 5
        { L_,   "ABCDEAB"           }, // 7
        { L_,   "ABCDEABC"          }, // 8
        { L_,   "ABCDEABCD"         }, // 9
        { L_,   "ABCDEABCDEABCDE"   }, // 15
        { L_,   "ABCDEABCDEABCDEA"  }, // 16
        { L_,   "ABCDEABCDEABCDEAB" }  // 17
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    if (verbose) printf("\tUsing '%s::const_iterator'.\n",
                        NameOf<CONTAINER>().name());
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int    LINE         = U_DATA[ti].d_lineNum;
                    const char  *SPEC         = U_DATA[ti].d_spec;
                    const int    NUM_ELEMENTS = static_cast<int>(strlen(SPEC));
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    Obj       mUA;    const Obj&       UA = gg(&mUA, SPEC);
                    CONTAINER mU(UA); const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, &ta);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                         ? 0
                                         : LENGTH <= INIT_CAP
                                         ? -1
                                         : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const bsls::Types::Int64 BB = ta.numBlocksTotal();
                        const bsls::Types::Int64 B  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        mX.insert(X.begin() + POS, U.begin(), U.end());

                        const bsls::Types::Int64 AA = ta.numBlocksTotal();
                        const bsls::Types::Int64 A  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                        if ((size_t) -1 != CAP) {
                            ASSERTV(INIT_LINE, LINE, i, j,
                                    CAP == X.capacity());
                        }

                        size_t m;
                        for (k = 0; k < POS; ++k) {
                            ASSERTV(INIT_LINE, LINE, j, k,
                                    VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            ASSERTV(INIT_LINE, LINE, j, k, m, U[m] == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            ASSERTV(INIT_LINE, LINE, j, k, m,
                                    VALUES[m % NUM_VALUES] == X[k]);
                        }

                        if (k_IS_MOVABLE && INPUT_ITERATOR_TAG) {
                            ASSERTV(NUM_ELEMENTS,  NUM_NUM_ALLOCS,
                                    NUM_ELEMENTS + ZERO < NUM_NUM_ALLOCS);
                            ASSERTV(X.capacity(),  NUM_NUM_ALLOCS,
                                    X.capacity() < NUM_NUM_ALLOCS);

                            const int REALLOC = X.capacity() > INIT_CAP
                                              ? NUM_ALLOCS[NUM_ELEMENTS] -
                                                NUM_ALLOCS[X.capacity()]
                                              : 0;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !k_IS_MOVABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    BB + EXP_ALLOCS,   AA,
                                    BB + EXP_ALLOCS <= AA);
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    B  + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        } else {
                            const int                REALLOC =
                                                       X.capacity() > INIT_CAP;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !k_IS_MOVABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    BB + EXP_ALLOCS,   AA,
                                    BB + EXP_ALLOCS <= AA);
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    B  + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        }
                    }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    if (4 < LENGTH && NUM_DATA-1 != i && NUM_U_DATA-1 != ti) {
                        continue;
                    }

                    Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
                    CONTAINER mU(UA);   const CONTAINER& U  = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                            const bsls::Types::Int64 AL = ta.allocationLimit();
                            ta.setAllocationLimit(-1);

                            Obj        mX(INIT_LENGTH, DEFAULT_VALUE, &ta);
                            const Obj& X = mX;
                            mX.reserve(INIT_CAP);

                            const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                             ? 0
                                             : LENGTH <= INIT_CAP
                                             ? -1
                                             : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ta.setAllocationLimit(AL);

                            mX.insert(X.begin() + POS, U.begin(), U.end());
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                            if ((size_t) -1 != CAP) {
                                ASSERTV(INIT_LINE, LINE, i, j,
                                        CAP == X.capacity());
                           }

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        U[k - POS] == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());
}


template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase17Negative()
{
    // ------------------------------------------------------------------------
    // NEGATIVE TESTING INSERTION
    //
    // Concerns:
    //   1 'insert' methods assert (in appropriate build modes) on undefined
    //     behavior when the iterator passed to specify the insertion point is
    //     not a valid iterator for this vector object.
    //
    //   2 An attempt to insert a range of elements specified by a pair of
    //     iterators should assert (in appropriate build modes) if the range is
    //     observably not valid i.e., a pair of random access iterators where
    //     'last' precedes 'first'.
    //
    // Plan:
    //   Construct a string object with some string data, and then call
    //   'insert' with a
    //   NULL C-string pointer and verify that it asserts.  Then call 'insert'
    //   with invalid iterators and verify that it asserts.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& val);
    //   iterator insert(const_iterator pos, size_type n, const T& val);
    //   template <class InputIter>
    //       void insert(const_iterator pos, InputIter first, InputIter last);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    const typename Obj::const_iterator badIterator =
                                               typename Obj::const_iterator();
    (void) badIterator;    // Quell 'unused' warnings in unsafe builds

    bsls::AssertTestHandlerGuard guard;

    if (veryVerbose) printf("\tNegative testing of 'insert(p, c)'.\n");

    {
        Obj mX;     const Obj& X = gg(&mX, "ABCDE");
        Obj mY;     const Obj& Y = gg(&mY, "ABCDE");
        (void) Y;    // Quell 'unused' warnings in unsafe builds.

        // position < begin()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.begin() - 1, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.end() + 1, X[0]));

        // arbitrary bad iterator
        ASSERT_SAFE_FAIL_RAW(mX.insert(badIterator, X[0]));

        // iterator to another container
        ASSERT_SAFE_FAIL_RAW(mX.insert(Y.begin(), X[0]));

        // begin() <= position < end()
        ASSERT_SAFE_PASS_RAW(mX.insert(X.begin() + 1, X[0]));
        ASSERT_SAFE_PASS_RAW(mX.insert(X.end(), X[0]));
    }

    if (veryVerbose) printf("\tNegative testing of 'insert(p, n, c)'.\n");

    {
        Obj mX;     const Obj& X = gg(&mX, "ABCDE");
        Obj mY;     const Obj& Y = gg(&mY, "ABCDE");
        (void) Y;    // Quell 'unused' warnings in unsafe builds.

        // position < begin()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.begin() - 1, 0, X[0]));
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.begin() - 1, 2, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.end() + 1, 0, X[0]));
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.end() + 1, 2, X[0]));

        // arbitrary bad iterator
        ASSERT_SAFE_FAIL_RAW(mX.insert(badIterator, 0, X[0]));
        ASSERT_SAFE_FAIL_RAW(mX.insert(badIterator, 2, X[0]));

        // iterator to another container
        ASSERT_SAFE_FAIL_RAW(mX.insert(Y.begin(), 0, X[0]));
        ASSERT_SAFE_FAIL_RAW(mX.insert(Y.begin(), 2, X[0]));

        // begin() <= position <= end()
        ASSERT_SAFE_PASS_RAW(mX.insert(X.begin() + 1, 0, X[0]));
        ASSERT_SAFE_PASS_RAW(mX.insert(X.end(), 2, X[0]));
    }

    if (veryVerbose)
        printf("\tNegative testing of 'insert(p, first, last)'.\n");

    {
        Obj mX;     const Obj& X = gg(&mX, "ABCDE");
        Obj mY;     const Obj& Y = gg(&mY, "ABCDE");

        // position < begin()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.begin() - 1, Y.begin(), Y.end()));

        // position > end()
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.end() + 1, Y.begin(), Y.end()));

        // first > last
        ASSERT_SAFE_FAIL_RAW(mX.insert(X.begin(), Y.end(), Y.begin()));
        ASSERT_SAFE_PASS_RAW(mX.insert(X.begin(), Y.end(), Y.end()));

        // two null pointers form a valid (empty) range
        const TYPE *nullPtr = 0;
        ASSERT_SAFE_PASS_RAW(mX.insert(X.begin(), nullPtr, nullPtr));

        // begin() <= position <= end() && first <= last
        ASSERT_SAFE_PASS_RAW(mX.insert(X.begin(), Y.begin(), Y.end()));
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //   1) That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable vector, and non-mutable iterators
    //      otherwise.
    //   2) That the range '[begin(), end())' equals the value of the vector.
    //   3) Same concerns with 'rbegin' and 'rend'.
    // In addition:
    //   4) That 'iterator' is a pointer to 'TYPE'.
    //   5) That 'const_iterator' is a pointer to 'const TYPE'.
    //   6) That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bslstl::ReverseIterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //
    // Plan:
    //   For 1--3, for each value given by variety of specifications of
    //   different lengths, create a test vector with this value, and access
    //   each element in sequence and in reverse sequence, both as a reference
    //   offering modifiable access (setting it to a default value, then back
    //   to its original value, and as a const-reference.
    //
    // For 4--6, use 'bsl::is_same' to assert the identity of iterator types.
    // Note that these concerns let us get away with other concerns such as
    // testing that 'iter[i]' and 'iter + i' advance 'iter' by the correct
    // number 'i' of positions, and other concern about traits, because
    // 'bslstl::IteratorTraits' and 'bslstl::ReverseIterator' have already been
    // fully tested in the 'bslstl_iterator' component.
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_reverse_iterator rbegin() const;
    //   const_reverse_iterator rend() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEAB"   },
        { L_,  "ABCDEABC"  },
        { L_,  "ABCDEABCD" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        ASSERT(1 == (bsl::is_same<iterator, TYPE *>::value));
        ASSERT(1 == (bsl::is_same<const_iterator, const TYPE *>::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);
            Obj        mY(X);
            const Obj& Y = mY;  // control

            if (veryVerbose) { T_ T_ P_(LINE); P(SPEC); }

            size_t i = 0;
            for (iterator iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                ASSERTV(LINE, Y[i] == *iter);
                *iter = DEFAULT_VALUE;
                ASSERTV(LINE, DEFAULT_VALUE == *iter);
                mX[i] = Y[i];
            }
            ASSERTV(LINE, LENGTH == i);
            ASSERTV(LINE, Y      == X);

            i = 0;
            for (const_iterator iter = X.begin(); iter != X.end();
                                                                 ++iter, ++i) {
                ASSERTV(LINE, i, Y[i] == *iter);
            }
            ASSERTV(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("\tTesting 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
#ifdef BSLS_PLATFORM_CMP_SUN
#else
        ASSERT(1 == (bsl::is_same<reverse_iterator,
                                   bsl::reverse_iterator<TYPE *> >::value));
        ASSERT(1 == (bsl::is_same<const_reverse_iterator,
                                bsl::reverse_iterator<const TYPE *> >::value));
#endif

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = static_cast<int>(strlen(SPEC));

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);
            Obj        mY(X);
            const Obj& Y = mY;  // control

            if (veryVerbose) { T_ T_ P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (reverse_iterator riter = mX.rbegin(); riter != mX.rend();
                                                                ++riter, --i) {
                ASSERTV(LINE, Y[i] == *riter);
                *riter = DEFAULT_VALUE;
                ASSERTV(LINE, DEFAULT_VALUE == *riter);
                mX[i] = Y[i];
            }
            ASSERTV(LINE, -1 == i);
            ASSERTV(LINE, Y  == X);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.rbegin(); riter != X.rend();
                                                                ++riter, --i) {
                ASSERTV(LINE, Y[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   1) That 'v[x]', as well as 'v.front()' and 'v.back()', allow to modify
    //      its indexed element when 'v' is an lvalue, but must not modify its
    //      indexed element when it is an rvalue.
    //   2) That 'v.at(pos)' returns 'v[x]' or throws if 'pos == v.size())'.
    //   3) That 'v.front()' is identical to 'v[0]' and 'v.back()' the same as
    //      'v[v.size() - 1]'.
    //   4) That 'data()' returns the address of the first element, whether or
    //      not it is 'const', unless the vector is empty.
    //   5) That 'data()' returns a valid address when the vector is empty.
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test vector with this value, and access each element
    //   (front, back, at each position) both as a reference offering
    //   modifiable access (setting it to a default value, then back to its
    //   original value, and as a const-reference.  Verify that 'at' throws
    //   'std::out_of_range' when accessing the past-the-end element.  Verify
    //   that the address of the referenced element returned from both
    //   'operator[]' and 'at' is that same as that return by adding the
    //   specified index to the pointer returned by 'data()'.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   T& at(size_type n);
    //   T& front();
    //   T& back();
    //   T *data();
    //   const T& operator[](size_type position) const;
    //   const T& at(size_type n) const;
    //   const T& front() const;
    //   const T& back() const;
    //   const T *data() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    const TestValues VALUES;
    const TYPE       DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEAB"   },
        { L_,  "ABCDEABC"  },
        { L_,  "ABCDEABCD" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);
            Obj        mY(X);
            const Obj& Y = mY;  // control

            if (veryVerbose) { T_ T_ P_(LINE); P(SPEC); }

            if (LENGTH) {
                // TYPE element;
                // makeElement(BSLS_UTIL_ADDRESSOF(element), SPEC[0]);
                // const TYPE& ELEM = element;
                const TYPE& ELEM = VALUES[SPEC[0] - 'A'];
                ASSERTV(LINE, ELEM == X.front());
                mX.front() = DEFAULT_VALUE;
                ASSERTV(LINE, DEFAULT_VALUE == X.front());
                mX[0] = Y[0];

                ASSERTV(LINE, X[LENGTH - 1] == X.back());
                mX.back() = DEFAULT_VALUE;
                ASSERTV(LINE, DEFAULT_VALUE == X.back());
                mX[LENGTH - 1] = Y[LENGTH - 1];
            }

            ASSERTV(LINE, Y == X);

            TYPE *const       dataMptr = mX.data();
            const TYPE *const dataCptr = X.data();
            ASSERTV(LINE, dataMptr, dataCptr, dataMptr == dataCptr);
            ASSERT(dataMptr == bsl::data(mX));
            ASSERT(dataCptr == bsl::data(X));

            for (size_t j = 0; j < LENGTH; ++j) {
                // TYPE element;
                // makeElement(BSLS_UTIL_ADDRESSOF(element), SPEC[j]);
                // const TYPE& ELEM = element;
                const TYPE& ELEM = VALUES[SPEC[j] - 'A'];
                ASSERTV(LINE, ELEM == X[j]);
                mX[j] = DEFAULT_VALUE;
                ASSERTV(LINE, DEFAULT_VALUE == X[j]);
                ASSERTV(LINE, BSLS_UTIL_ADDRESSOF( X[j]) == (dataCptr + j));
                ASSERTV(LINE, BSLS_UTIL_ADDRESSOF(mX[j]) == (dataMptr + j));
                mX.at(j) = Y[j];
                ASSERTV(LINE, ELEM == X.at(j));
                ASSERTV(LINE, BSLS_UTIL_ADDRESSOF( X.at(j)) == (dataCptr + j));
                ASSERTV(LINE, BSLS_UTIL_ADDRESSOF(mX.at(j)) == (dataMptr + j));
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);
            Obj        mY(X);
            const Obj& Y = mY;  // control

            bool outOfRangeCaught = false;
            try {
                mX.at(LENGTH) = DEFAULT_VALUE;
            }
            catch (const std::out_of_range&) {
                outOfRangeCaught = true;
            }
            ASSERTV(LINE, Y == X);
            ASSERTV(LINE, outOfRangeCaught);
        }
    }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase15Negative()
{
    // ------------------------------------------------------------------------
    // NEGATIVE TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   For a vector 'v', the following const and non-'const' operations
    //   assert on undefined behavior:
    //   1 v[x] - when the index 'x' is out of range
    //   2 v.front() - when 'v' is empty
    //   3 v.back() - when 'v' is empty
    //
    // Plan:
    //   To test concerns (2) and (3), create an empty vector and verify that
    //   'front'/'back' methods assert correctly.  Then insert a single
    //   element into the vector and verify that the methods don't assert any
    //   more.  Then remove the element to make the vector empty again, and
    //   verify that the methods start asserting again.
    //
    //   To test concern (1), create a vector using a variety of specifications
    //   of different lengths, then scan the range of negative and positive
    //   indices for 'operator[]' and verify that 'operator[]' asserts when the
    //   index is out of range.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   const T& operator[](size_type position) const;
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bsls::AssertTestHandlerGuard guard;

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial
    } DATA[] = {
        //line  spec           length
        //----  -----------    ------
        { L_,   ""          }, // 0
        { L_,   "A"         }, // 1
        { L_,   "AB"        }, // 2
        { L_,   "ABC"       }, // 3
        { L_,   "ABCD"      }, // 4
        { L_,   "ABCDE"     }, // 5
        { L_,   "ABCDEA"    }, // 6
        { L_,   "ABCDEAB"   }, // 7
        { L_,   "ABCDEABC"  }, // 8
        { L_,   "ABCDEABCD" }, // 9
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (veryVerbose) printf("\tNegative testing of 'operator[]'.\n");

    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            Obj mX; const Obj& X = gg(&mX, SPEC);

            for (int i = -int(X.size()) - 1; i < int(X.size() * 2) + 2; ++i) {
                if (veryVeryVerbose) { T_ T_ T_ P(i); }

                if (i >= 0 && i < (int) X.size()) {
                    ASSERT_SAFE_PASS_RAW( X[i]);
                    ASSERT_SAFE_PASS_RAW(mX[i]);
                }
                else {
                    ASSERT_SAFE_FAIL_RAW( X[i]);
                    ASSERT_SAFE_FAIL_RAW(mX[i]);
                }
            }
        }
    }

    if (veryVerbose) printf("\tNegative testing of 'front/back'.\n");

    {
        Obj        mX;
        const Obj& X = mX;
        ASSERT_SAFE_FAIL_RAW( X.front());
        ASSERT_SAFE_FAIL_RAW(mX.front());
        ASSERT_SAFE_FAIL_RAW (X.back());
        ASSERT_SAFE_FAIL_RAW(mX.back());

        mX.push_back(DEFAULT_VALUE);
        ASSERT_SAFE_PASS_RAW( X.front());
        ASSERT_SAFE_PASS_RAW(mX.front());
        ASSERT_SAFE_PASS_RAW( X.back());
        ASSERT_SAFE_PASS_RAW(mX.back());

        mX.pop_back();
        ASSERT_SAFE_FAIL_RAW( X.front());
        ASSERT_SAFE_FAIL_RAW(mX.front());
        ASSERT_SAFE_FAIL_RAW( X.back());
        ASSERT_SAFE_FAIL_RAW(mX.back());
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase14a()
{
    // --------------------------------------------------------------------
    // TESTING 'resize(size_t newSize)'
    //
    // Concerns:
    //   1) That 'v.resize(n)' brings the new size to 'n', adding default-
    //      constructed elements if 'n' is larger than the current size.
    //   2) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   3) That existing elements are moved using move constructor if the
    //      the move constructor is present and does not throw.
    //   4) That 'resize' provide the strong exception guarantee.
    //   5) That the accessors such as 'capacity', 'empty', return the correct
    //      value.
    //
    // Plan:
    //   For vector 'v' having various initial capacities, call
    //   'v.reserve(n)' for various values of 'n'.  Verify that sufficient
    //   capacity is allocated by filling 'v' with 'n' elements.  Perform
    //   each test in the standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void resize(size_type n);
    // --------------------------------------------------------------------

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                            bsl::uses_allocator<TYPE, ALLOC>::value;

    const bool TYPE_COPY = !k_IS_MOVABLE
                        &&  bsl::is_copy_constructible<TYPE>::value;

    const TYPE DEFAULT_VALUE = TYPE();

    if (verbose) printf(
         "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, TYPE_COPY = %d).\n",
         NameOf<TYPE>().name(),
         TYPE_ALLOC,
         k_IS_MOVABLE,
         TYPE_COPY);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        int         d_size;    // new size
        const char *d_result;  // expected element values
    } DATA[] = {
        //line  spec       ns    results
        //----  --------   ---   ------------
        { L_,   "",         0,   ""           },
        { L_,   "",         1,   "*"          },
        { L_,   "",         2,   "**"         },
        { L_,   "",         3,   "***"        },
        { L_,   "",        10,   "**********" },
        { L_,   "A",        0,   ""           },
        { L_,   "A",        1,   "A"          },
        { L_,   "A",        2,   "A*"         },
        { L_,   "A",        3,   "A**"        },
        { L_,   "A",       10,   "A*********" },
        { L_,   "AB",       0,   ""           },
        { L_,   "AB",       1,   "A"          },
        { L_,   "AB",       2,   "AB"         },
        { L_,   "AB",       3,   "AB*"        },
        { L_,   "AB",      10,   "AB********" },
        { L_,   "ABCD",     0,   ""           },
        { L_,   "ABCD",     2,   "AB"         },
        { L_,   "ABCD",     4,   "ABCD"       },
        { L_,   "ABCD",     5,   "ABCD*"      },
        { L_,   "ABCD",    10,   "ABCD******" },
        { L_,   "ABCDE",    0,   ""           },
        { L_,   "ABCDE",    2,   "AB"         },
        { L_,   "ABCDE",    5,   "ABCDE"      },
        { L_,   "ABCDE",    7,   "ABCDE**"    },
        { L_,   "ABCDE",   10,   "ABCDE*****" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'resize(size_t newSize)'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE  = DATA[ti].d_line;
        const char   *SPEC  = DATA[ti].d_spec;
        const size_t  OSIZE = strlen(SPEC);
        const size_t  NSIZE = DATA[ti].d_size;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        ALLOC                xscratch(&scratch);
        Obj                  mZ(xscratch);
        const Obj&           Z = gg(&mZ, SPEC);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        Obj                  mX(xoa);
        const Obj&           X = gg(&mX, SPEC);

        const size_t OCAP = X.capacity();

        const size_t BB = size_t(oa.numBlocksTotal());
        const size_t B  = size_t(oa.numBlocksInUse());

        mX.resize(NSIZE);

        ASSERT(NSIZE == X.size());

        for (size_t tj = 0; tj < OSIZE && tj < NSIZE; ++tj) {
            ASSERT(X[tj] == Z[tj]);
        }

        for (size_t tk = OSIZE; tk < NSIZE; ++tk) {
            ASSERTV(DEFAULT_VALUE, X[tk], DEFAULT_VALUE == X[tk]);
        }

        const size_t ADDED   = NSIZE > OSIZE ? NSIZE - OSIZE : 0;
        const size_t REMOVED = NSIZE < OSIZE ? OSIZE - NSIZE : 0;

        const size_t AA = size_t(oa.numBlocksTotal());
        const size_t A  = size_t(oa.numBlocksInUse());

        if (NSIZE > OCAP) {
            ASSERT(ADDED > 0);
            if (OSIZE > 0) {
                ASSERTV(OSIZE, numMovedInto(X, 0, OSIZE),
                        !k_IS_MOVABLE || (OSIZE == numMovedInto(X, 0, OSIZE)));
            }
            ASSERTV(ADDED, numNotMovedInto(X, OSIZE),
                    !k_IS_MOVABLE || (ADDED == numNotMovedInto(X, OSIZE)));
            const size_t EXP = BB
                          +  1                                  // realloc
                          +  TYPE_ALLOC * (NSIZE - OSIZE)       // new elements
                          +  TYPE_ALLOC * (OSIZE * TYPE_COPY);  // OSIZE copies
            ASSERTV(LINE, BB, AA, EXP, EXP == AA);
            ASSERTV(LINE, B,  A,  B + TYPE_ALLOC * ADDED + (0 == OSIZE) == A);
        }
        else if (NSIZE > OSIZE) {
            ASSERTV(LINE, BB, AA, BB + TYPE_ALLOC * ADDED == AA);
            ASSERTV(LINE, B,  A,  B  + TYPE_ALLOC * ADDED == A );
        }
        else {
            ASSERTV(LINE, BB, AA, BB                       == AA);
            ASSERTV(LINE, B,  A,  B - TYPE_ALLOC * REMOVED == A );
        }
    }

    if (verbose) printf("\tTesting 'resize(size_t newSize)' with injected"
                        " exceptions.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE  = DATA[ti].d_line;
        const char   *SPEC  = DATA[ti].d_spec;
        const size_t  OSIZE = strlen(SPEC);
        const size_t  NSIZE = DATA[ti].d_size;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        ALLOC                xscratch(&scratch);
        Obj                  mZ(xscratch);
        const Obj&           Z = gg(&mZ, SPEC);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        ALLOC                xoa(&oa);
        Obj                  mX(xoa);
        const Obj&           X = gg(&mX, SPEC);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

            ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

            mX.resize(NSIZE);

            proctor.release();

            ASSERT(NSIZE == X.size());

            for (size_t tj = 0; tj < OSIZE && tj < NSIZE; ++tj) {
                ASSERTV(LINE, X[tj] == Z[tj]);
            }

            for (size_t tk = OSIZE; tk < NSIZE; ++tk) {
                ASSERTV(LINE, DEFAULT_VALUE,   X[tk],
                              DEFAULT_VALUE == X[tk]);
            }

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING CAPACITY
    //
    // Concerns:
    //   1) That 'v.reserve(n)' reserves sufficient capacity in 'v' to hold
    //      'n' elements without reallocation, but does not change value.
    //      In addition, if 'v.reserve(n)' allocates, it must allocate for a
    //      capacity of exactly 'n' bytes.
    //   2) That 'v.resize(n, val)' brings the new size to 'n', adding elements
    //      equal to 'val' if 'n' is larger than the current size.
    //   3) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   4) That 'reserve' and 'resize' are exception-neutral with full
    //      guarantee of rollback.
    //   5) That the accessors such as 'capacity', 'empty', return the correct
    //      value.
    //
    // Plan:
    //   For vector 'v' having various initial capacities, call
    //   'v.reserve(n)' for various values of 'n'.  Verify that sufficient
    //   capacity is allocated by filling 'v' with 'n' elements.  Perform
    //   each test in the standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void vector<T>::reserve(size_type n);
    //   void resize(size_type n);
    //   void resize(size_type n, T val = T());
    //   void shrink_to_fit();
    //   size_type max_size() const;
    //   size_type capacity() const;
    //   bool empty() const;
    // ------------------------------------------------------------------------

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                            bsl::uses_allocator<TYPE, ALLOC>::value;

    const char *name = NameOf<TYPE>().name();

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        name,
                        TYPE_ALLOC);

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);
    ASSERT(0 == ta.numBytesInUse());

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

    static const size_t EXTEND[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

    static const size_t DATA[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const size_t AP = allocationsByPrimaryManipulator();
        // Allocations per element inserted by primary manipulator, assuming
        // there is sufficient capacity.

    if (veryVerbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        const Obj X;
        ASSERT(~(size_t)0 / sizeof(TYPE) >= X.max_size());
    }

    if (veryVerbose) printf("\tTesting 'reserve', 'capacity' and 'empty'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
              const bsls::Types::Int64 AL = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              Obj mX(xta);  const Obj& X = mX;
              ASSERTV(ti, X.empty());
              ASSERTV(ti, bsl::empty(X));

              stretch(&mX, CAP);
              ASSERTV(ti, CAP == X.size());
              ASSERTV(ti, CAP <= X.capacity());
              ASSERTV(ti, !(bool)X.size() == X.empty());
              ASSERTV(ti, !(bool)X.size() == bsl::empty(X));

              ta.setAllocationLimit(AL);

              const bsls::Types::Int64 NUM_ALLOC_BEFORE = ta.numAllocations();
              const size_t             CAPACITY         = X.capacity();
              {
                  ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

                  mX.reserve(NE);
                  ASSERTV(ti, CAP == X.size());
                  ASSERTV(ti, NE < CAPACITY || NE == X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const bsls::Types::Int64 NUM_ALLOC_AFTER = ta.numAllocations();
              ASSERTV(ti, NE > CAP || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const bsls::Types::Int64 AL2 = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              ASSERTV(ti, CAP, DELTA, X.size(), CAP + DELTA == X.size());
              if (TYPE_ALLOC) {
                  ASSERTV(ti,
                     NUM_ALLOC_AFTER,  int(AP * DELTA),   ta.numAllocations(),
                     NUM_ALLOC_AFTER + int(AP * DELTA) == ta.numAllocations());
              }
              else {
                  ASSERTV(ti, NUM_ALLOC_AFTER,   ta.numAllocations(),
                              NUM_ALLOC_AFTER == ta.numAllocations());
              }

              ta.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP = EXTEND[ei];

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
              const bsls::Types::Int64 AL = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              Obj mX(xta);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              ASSERTV(ti, X.empty());
              ASSERTV(ti, bsl::empty(X));
              ASSERTV(ti, 0   == X.size());
              ASSERTV(ti, CAP <= X.capacity());

              ta.setAllocationLimit(AL);
              const bsls::Types::Int64 NUM_ALLOC_BEFORE = ta.numAllocations();
              {
                  ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

                  mX.reserve(NE);
                  ASSERTV(ti, 0  == X.size());
                  ASSERTV(ti, NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const bsls::Types::Int64 NUM_ALLOC_AFTER = ta.numAllocations();
              ASSERTV(ti, NE > CAP || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const bsls::Types::Int64 AL2 = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              stretch(&mX, NE);
              ASSERTV(ti, NE == X.size());
              if (TYPE_ALLOC) {
                  ASSERTV(ti,
                        NUM_ALLOC_AFTER,           NE,    ta.numAllocations(),
                        NUM_ALLOC_AFTER + int(AP * NE) == ta.numAllocations());
              }
              else {
                  ASSERTV(ti, name, NUM_ALLOC_AFTER,   ta.numAllocations(),
                                    NUM_ALLOC_AFTER == ta.numAllocations());
              }

              ta.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBlocksInUse());
        }
    }

    if (veryVerbose) printf("\tTesting 'resize'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
              const bsls::Types::Int64 AL = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              Obj mX(xta);  const Obj& X = mX;
              ASSERTV(ti, X.empty());
              ASSERTV(ti, bsl::empty(X));

              stretch(&mX, CAP);
              ASSERTV(ti, CAP == X.size());
              ASSERTV(ti, CAP <= X.capacity());
              ASSERTV(ti, !(bool)X.size() == X.empty());
              ASSERTV(ti, !(bool)X.size() == bsl::empty(X));

              ta.setAllocationLimit(AL);
              const bsls::Types::Int64     NUM_ALLOC_BEFORE =
                                                           ta.numAllocations();
              ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              ASSERTV(ti, NE == X.size());
              ASSERTV(ti, NE <= X.capacity());
              const bsls::Types::Int64 NUM_ALLOC_AFTER = ta.numAllocations();

              ASSERTV(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = CAP; j < NE; ++j) {
                  ASSERTV(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              proctor.release();

              const bsls::Types::Int64 AL2 = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              ASSERTV(ti, NE + DELTA == X.size());
              ta.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
              const bsls::Types::Int64 AL = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              Obj mX(xta);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              ASSERTV(ti, X.empty());
              ASSERTV(ti, bsl::empty(X));
              ASSERTV(ti, 0   == X.size());
              ASSERTV(ti, CAP <= X.capacity());

              const bsls::Types::Int64     NUM_ALLOC_BEFORE =
                                                           ta.numAllocations();
              ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

              ta.setAllocationLimit(AL);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              ASSERTV(ti, NE == X.size());
              ASSERTV(ti, NE <= X.capacity());
              const bsls::Types::Int64 NUM_ALLOC_AFTER = ta.numAllocations();

              ASSERTV(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = 0; j < NE; ++j) {
                  ASSERTV(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              proctor.release();

              const bsls::Types::Int64 AL2 = ta.allocationLimit();
              ta.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              ASSERTV(ti, NE + DELTA == X.size());
              ta.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'shrink_to_fit'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        if (veryVerbose)
            printf("LINE = %d, ti = %d\n", L_, ti);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
            const bsls::Types::Int64 AL = ta.allocationLimit();
            ta.setAllocationLimit(-1);

            Obj mX(xta);  const Obj& X = mX;
            ASSERTV(ti, X.empty());
            ASSERTV(ti, bsl::empty(X));

            stretch(&mX,
                    NE,
                    TstFacility::getIdentifier(VALUES[ti % NUM_VALUES]));

            ASSERTV(ti, NE == X.size());
            ASSERTV(ti, NE <= X.capacity());
            const size_t DELTA = X.capacity() - NE;
            ASSERTV(ti, !(bool)X.size() == X.empty());
            ASSERTV(ti, !(bool)X.size() == bsl::empty(X));

            ta.setAllocationLimit(AL);
            const bsls::Types::Int64     NUM_ALLOC_BEFORE =
                                                           ta.numAllocations();
            ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);

            mX.shrink_to_fit();  // test here

            ASSERTV(ti, NE == X.size());
            ASSERTV(ti, NE == X.capacity());
            const bsls::Types::Int64 NUM_ALLOC_AFTER = ta.numAllocations();

            ASSERTV(ti, DELTA, DELTA || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

            for (size_t j = 0; j < NE; ++j) {
                ASSERTV(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
            }
            proctor.release();

            const bsls::Types::Int64 AL2 = ta.allocationLimit();
            ta.setAllocationLimit(-1);

            stretch(&mX, DELTA);
            ASSERTV(ti, NE + DELTA == X.size());
            ta.setAllocationLimit(AL2);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBlocksInUse());
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'assign'
    //
    // The concerns are the same as for the constructor with the same signature
    // (case 12), except that the implementation is different, and in addition
    // the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   assign(size_type n, const T& value);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    if (verbose) printf("\tTesting initial-length assignment.\n");
    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0 },
            { L_,        1 },
            { L_,        2 },
            { L_,        3 },
            { L_,        4 },
            { L_,        5 },
            { L_,        6 },
            { L_,        7 },
            { L_,        8 },
            { L_,        9 },
            { L_,       11 },
            { L_,       12 },
            { L_,       14 },
            { L_,       15 },
            { L_,       16 },
            { L_,       17 }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf("using default value.\n");
                }

                Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    const size_t CAP = LENGTH <= INIT_LENGTH
                                     ? INIT_LENGTH
                                     : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 X.max_size());
                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    ASSERTV(INIT_LINE, LINE, i, ti, CAP == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        ASSERTV(INIT_LINE, LINE, i, ti, j, VALUE == X[j]);
                    }
                }
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBlocksInUse());
        }

        if (verbose) printf("\tWith exceptions.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    const size_t CAP = LENGTH <= INIT_LENGTH
                                     ? INIT_LENGTH
                                     : ImpUtil::computeNewCapacity(
                                                             LENGTH,
                                                             INIT_LENGTH,
                                                             Obj().max_size());
                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                        const bsls::Types::Int64 AL = ta.allocationLimit();
                        ta.setAllocationLimit(-1);

                        Obj        mX(INIT_LENGTH, DEFAULT_VALUE, xta);
                        const Obj& X = mX;

                        ExceptionProctor<Obj, ALLOC> proctor(&mX, Obj(), L_);

                        ta.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        proctor.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        ASSERTV(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                        ASSERTV(INIT_LINE, LINE, i, ti, CAP == X.capacity());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            ASSERTV(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == ta.numMismatches());
                    ASSERT(0 == ta.numBlocksInUse());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE, ALLOC>::testCase13Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING 'assign'
    //
    // The concerns are the same as for the constructor with the same signature
    // (case 12), except that the implementation is different, and in addition
    // the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class InputIter> assign(InputIter f, InputIter l);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<CONTAINER>().name());

    bslma::TestAllocator  ta("X Object under test", veryVeryVeryVerbose);

    const TestValues VALUES;
    const int        NUM_VALUES = 5; // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                    length
        //----  -------------------     ------
        { L_,   ""                  },  // 0
        { L_,   "A"                 },  // 1
        { L_,   "AB"                },  // 2
        { L_,   "ABC"               },  // 3
        { L_,   "ABCD"              },  // 4
        { L_,   "ABCDE"             },  // 5
        { L_,   "ABCDEAB"           },  // 7
        { L_,   "ABCDEABC"          },  // 8
        { L_,   "ABCDEABCD"         },  // 9
        { L_,   "ABCDEABCDEABCDE"   },  // 15
        { L_,   "ABCDEABCDEABCDEA"  },  // 16
        { L_,   "ABCDEABCDEABCDEAB" }   // 17
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default value.\n");
            }

            Obj        mX(INIT_LENGTH, VALUES[i % NUM_VALUES], &ta);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
                CONTAINER mU(UA);   const CONTAINER& U  = mU;

                const size_t CAP = LENGTH <= INIT_LENGTH
                                 ? INIT_LENGTH
                                 : ImpUtil::computeNewCapacity(LENGTH,
                                                               INIT_LENGTH,
                                                               X.max_size());
                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(INIT_LINE, LINE, i, ti, LENGTH,   X.size(),
                                                LENGTH == X.size());
                ASSERTV(INIT_LINE, LINE, i, ti, CAP, X.capacity(),
                                                CAP == X.capacity());

                Obj mY;     const Obj& Y = gg(&mY, SPEC);
                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBlocksInUse());
    }

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                Obj mUA;            const Obj& UA = gg(&mUA, SPEC);
                CONTAINER mU(UA);   const CONTAINER& U = mU;

                const size_t CAP = LENGTH <= INIT_LENGTH
                                 ? INIT_LENGTH
                                 : ImpUtil::computeNewCapacity(
                                                             LENGTH,
                                                             INIT_LENGTH,
                                                             Obj().max_size());
                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY;     const Obj& Y = gg(&mY, SPEC);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                    const bsls::Types::Int64 AL = ta.allocationLimit();
                    ta.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &ta); const Obj& X = mX;

                    ExceptionProctor<Obj, ALLOC> proctor(&mX, Obj(), L_);

                    ta.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    proctor.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(INIT_LINE, LINE, i, ti, LENGTH,   X.size(),
                                                    LENGTH == X.size());
                    ASSERTV(INIT_LINE, LINE, i, ti, CAP,   X.capacity(),
                                                    CAP == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        ASSERTV(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(ta.numMismatches(),  0 == ta.numMismatches());
                ASSERTV(ta.numBlocksInUse(), 0 == ta.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE, ALLOC>::testCase13Negative(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // NEGATIVE TESTING 'assign'
    //
    // Concerns:
    //   Assigning an invalid range to a vector should be trapped and asserted
    //   in appropriate build modes.
    //
    // Plan:
    //   Attempt to 'assign' a range to a 'vector' from another 'vector' object
    //   where the specified iterators are in the reversed order.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<CONTAINER>().name());

    bsls::AssertTestHandlerGuard guard;

    Obj       mCA;      const Obj&       CA = gg(&mCA, "ABCDE");
    CONTAINER mC(CA);   const CONTAINER& C = mC;

    const Obj X(C.begin(), C.end());

    Obj mY(X);

    if (verbose) {
        printf("\tUsing an empty range made up of null pointers\n");
    }
    // null pointers form a valid range
    const TYPE *nullPtr = 0;
    ASSERT_SAFE_PASS_RAW(mY.assign(nullPtr, nullPtr));

    if (verbose) {
        printf("\tUsing an empty range made up of stack pointers\n");
    }
    const TYPE null = TYPE();
    ASSERT_SAFE_PASS_RAW(mY.assign(BSLS_UTIL_ADDRESSOF(null),
                                   BSLS_UTIL_ADDRESSOF(null)));


    if (verbose) {
        printf("\tUsing a reversed range of vector iterators\n");
    }
    // first > last
    ASSERT_SAFE_FAIL_RAW(mY.assign(X.end(), X.begin()));

    if (verbose) {
        printf("\tFinally test a valid range of vector iterators\n");
    }
    ASSERT_SAFE_PASS_RAW(mY.assign(X.end(), X.end()));
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS
    //
    // We have the following concerns:
    //    1) The initial value is correct.
    //    2) The initial capacity is correctly set up.
    //    3) The constructor is exception neutral w.r.t. memory allocation.
    //    4) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    5) The move constructor moves value, capacity, and allocator
    //       correctly, and without performing any allocation.
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   As for concern 5, we simply move-construct each value into a new
    //   vector and check that the value, capacity, and allocator are as
    //   expected, and that no allocation was performed.
    //
    // Testing:
    //   vector(size_type n, const A& a = A());
    //   vector(size_type n, const T& value, const A& a = A());
    //   vector(vector<T,A>&& original);
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    const TYPE DEFAULT_VALUE = TYPE();

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0 },
            { L_,        1 },
            { L_,        2 },
            { L_,        3 },
            { L_,        4 },
            { L_,        5 },
            { L_,        6 },
            { L_,        7 },
            { L_,        8 },
            { L_,        9 },
            { L_,       11 },
            { L_,       12 },
            { L_,       14 },
            { L_,       15 },
            { L_,       16 },
            { L_,       17 },
            { L_,       31 },
            { L_,       32 },
            { L_,       33 },
            { L_,       63 },
            { L_,       64 },
            { L_,       65 }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\tWithout passing in an allocator, "
                            "using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                Obj mX(LENGTH);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(LINE, ti, LENGTH == X.size());
                ASSERTV(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(LINE, ti, j, DEFAULT_VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing in an allocator, "
                            "using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const bsls::Types::Int64 BB = ta.numBlocksTotal();
                const bsls::Types::Int64 B  = ta.numBlocksInUse();

                if (veryVerbose) { printf("\t\t\tBefore:"); P_(BB); P(B); }

                Obj        mX(LENGTH, xta);
                const Obj& X = mX;

                const bsls::Types::Int64 AA = ta.numBlocksTotal();
                const bsls::Types::Int64 A  = ta.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\tAfter :"); P_(AA); P(A);
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(LINE, ti, LENGTH == X.size());
                ASSERTV(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(LINE, ti, j, DEFAULT_VALUE,   X[j],
                                         DEFAULT_VALUE == X[j]);
                }

                if (LENGTH == 0) {
                    ASSERTV(LINE, ti, BB + 0 == AA);
                    ASSERTV(LINE, ti, B  + 0 == A );
                }
                else {
                    ASSERTV(LINE, ti, BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE, ti, B  + 1 + (int)LENGTH * TYPE_ALLOC == A );
                }
            }
        }

#if defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf("\tWith passing an allocator and checking for "
                            "allocation exceptions using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const bsls::Types::Int64 BB = ta.numBlocksTotal();
                const bsls::Types::Int64 B  = ta.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {

                    Obj        mX(LENGTH, xta);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE, ti, LENGTH == X.size());
                    ASSERTV(LINE, ti, LENGTH == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        ASSERTV(LINE, ti, j, DEFAULT_VALUE,   X[j],
                                             DEFAULT_VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = ta.numBlocksTotal();
                const bsls::Types::Int64 A  = ta.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

                if (LENGTH == 0) {
                    ASSERTV(LINE, ti, BB + 0 == AA);
                    ASSERTV(LINE, ti, B  + 0 == A );
                }
                else {
                    const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    ASSERTV(LINE, ti, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(LINE, ti, B  + 0               == A );
                }
                ASSERTV(LINE, ti, 0 == ta.numBlocksInUse());
            }
        }
#endif  // BDE_BUILD_TARGET_EXC

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                (void) LINE;

                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (verbose) {
                    printf("\t\tCreating object of "); P(LENGTH);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ASSERT(0 == da.numBytesTotal());
                    ASSERT(0 == oa.numBytesInUse());

                    Obj mX(LENGTH, xoa);

                    ASSERT(0 == da.numBytesInUse());
                    ASSERTV(LENGTH, 0 == LENGTH || 0 != oa.numBytesInUse());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numBytesInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase12NoDefault()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS
    //
    // We have the following concerns:
    //    1) The initial value is correct.
    //    2) The initial capacity is correctly set up.
    //    3) The constructor is exception neutral w.r.t. memory allocation.
    //    4) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    5) The move constructor moves value, capacity, and allocator
    //       correctly, and without performing any allocation.
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   As for concern 5, we simply move-construct each value into a new
    //   vector and check that the value, capacity, and allocator are as
    //   expected, and that no allocation was performed.
    //
    // Testing:
    //   vector(size_type n, const A& a = A());
    //   vector(size_type n, const T& value, const A& a = A());
    //   vector(vector<T,A>&& original);
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0 },
            { L_,        1 },
            { L_,        2 },
            { L_,        3 },
            { L_,        4 },
            { L_,        5 },
            { L_,        6 },
            { L_,        7 },
            { L_,        8 },
            { L_,        9 },
            { L_,       11 },
            { L_,       12 },
            { L_,       14 },
            { L_,       15 },
            { L_,       16 },
            { L_,       17 },
            { L_,       31 },
            { L_,       32 },
            { L_,       33 },
            { L_,       63 },
            { L_,       64 },
            { L_,       65 }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) printf("\tWithout passing in an allocator, "
                            "using non-default values.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE(   VALUES[ti % NUM_VALUES]);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(LINE, ti, LENGTH == X.size());
                ASSERTV(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(LINE, ti, j, VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing in an allocator, "
                            "using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE(   VALUES[ti % NUM_VALUES]);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const bsls::Types::Int64 BB = ta.numBlocksTotal();
                const bsls::Types::Int64 B  = ta.numBlocksInUse();

                Obj        mX(LENGTH, VALUE, xta);
                const Obj& X = mX;

                const bsls::Types::Int64 AA = ta.numBlocksTotal();
                const bsls::Types::Int64 A  = ta.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(LINE, ti, LENGTH == X.size());
                ASSERTV(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(LINE, ti, j, VALUE == X[j]);
                }

                if (LENGTH == 0) {
                    ASSERTV(LINE, ti, BB + 0 == AA);
                    ASSERTV(LINE, ti, B  + 0 == A );
                }
                else {
                    ASSERTV(LINE, ti, BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE, ti, B  + 1 + (int)LENGTH * TYPE_ALLOC == A );
                }
            }
        }

#if defined(BDE_BUILD_TARGET_EXC)
        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE(   VALUES[ti % NUM_VALUES]);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const bsls::Types::Int64 BB = ta.numBlocksTotal();
                const bsls::Types::Int64 B  = ta.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {

                    Obj        mX(LENGTH, VALUE, xta);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE, ti, LENGTH == X.size());
                    ASSERTV(LINE, ti, LENGTH == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        ASSERTV(LINE, ti, j, VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = ta.numBlocksTotal();
                const bsls::Types::Int64 A  = ta.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

                if (LENGTH == 0) {
                    ASSERTV(LINE, ti, BB + 0 == AA);
                    ASSERTV(LINE, ti, B  + 0 == A );
                }
                else {
                    // Because of exceptions, the number of allocations will be
                    // 'LENGTH' trials that allocate the array for the vector,
                    // plus '1 + 2 + ... + LENGTH == LENGTH * (1 + LENGTH) / 2'
                    // for the vector elements at each successive trial, plus
                    // one for the final trial that succeeds.

                    const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    ASSERTV(LINE, ti, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(LINE, ti, B  + 0               == A );
                }

                ASSERTV(LINE, ti, 0 == ta.numBlocksInUse());
            }
        }
#endif  // BDE_BUILD_TARGET_EXC

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "non-default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE(   VALUES[ti % NUM_VALUES]);
                (void) LINE;

                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ASSERT(0 == da.numBytesInUse());
                    ASSERT(0 == oa.numBytesInUse());

                    Obj x(LENGTH, VALUE, xoa);

                    ASSERT(0 == da.numBytesInUse());
                    ASSERTV(LENGTH, 0 == LENGTH || 0 != oa.numBytesInUse());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numBytesInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE, ALLOC>::testCase12Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS
    //
    // We have the following concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported and then moved if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    4) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    5) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    6) QoI: That passing an invalid range is detected and asserted, where
    //       possible, and in appropriate build modes only.
    //
    // Plan:
    //   We will create objects of varying sizes and capacities containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Finally, if the iterator category for 'CONTAINER' supports random
    //   access iterators, then we implement negative testing for passing
    //   reverse-ordered ranges.
    //
    // Testing:
    //   template <class InputIter>
    //     vector(InputIter first, InputIter last, const A& a = A());
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value;

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d).\n",
                        NameOf<CONTAINER>().name(),
                        TYPE_ALLOC,
                        k_IS_MOVABLE);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial spec
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "AB"        },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEAB"   },
        { L_,  "ABCDEABC"  },
        { L_,  "ABCDEABCD" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
            CONTAINER mU(UA);   const CONTAINER& U  = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
            }

            ASSERTV(LINE, ti, LENGTH == X.size());
            ASSERTV(LINE, ti, LENGTH <= X.capacity());

            Obj mY;     const Obj& Y = gg(&mY, SPEC);
            for (size_t j = 0; j < LENGTH; ++j) {
                ASSERTV(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) { printf("\t\tCreating object "); P(SPEC); }

            Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
            CONTAINER mU(UA);   const CONTAINER& U  = mU;
            Obj       mY;       const Obj&       Y  = gg(&mY, SPEC);

            const bsls::Types::Int64 BB = ta.numBlocksTotal();
            const bsls::Types::Int64 B  = ta.numBlocksInUse();

            Obj        mX(U.begin(), U.end(), &ta);
            const Obj& X = mX;

            const bsls::Types::Int64 AA = ta.numBlocksTotal();
            const bsls::Types::Int64 A  = ta.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            ASSERTV(LINE, ti, LENGTH,   X.size(),     LENGTH == X.size());
            ASSERTV(LINE, ti, LENGTH,   X.capacity(), LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                ASSERTV(LINE, ti, j, Y[j], X[j], Y[j] == X[j]);
            }

            if (LENGTH == 0) {
                ASSERTV(LINE, ti, BB, AA, BB + 0 == AA);
                ASSERTV(LINE, ti, B,  A,  B  + 0 == A );
            }
            else if (INPUT_ITERATOR_TAG) {
                BSLS_ASSERT_OPT(LENGTH < NUM_NUM_ALLOCS);

                const bsls::Types::Int64 TYPE_ALLOCS = TYPE_ALLOC * LENGTH;
                if (k_IS_MOVABLE) {
                    // Elements are create once, and then moved (no
                    // allocation), so 'TYPE_ALLOCS' is exactly the number of
                    // allocations triggered by elements.

                    ASSERTV(LINE, ti, BB,  AA,
                            BB + TYPE_ALLOCS + NUM_ALLOCS[LENGTH],
                            BB + TYPE_ALLOCS + NUM_ALLOCS[LENGTH] == AA);
                    ASSERTV(LINE, ti, B,   A,  TYPE_ALLOCS,
                            B + TYPE_ALLOCS + 1,
                            B + TYPE_ALLOCS + 1  == A );
                } else {
                    ASSERTV(LINE, ti, BB,  AA,
                            BB + TYPE_ALLOCS + NUM_ALLOCS[LENGTH],
                            BB + TYPE_ALLOCS + NUM_ALLOCS[LENGTH] <= AA);
                    ASSERTV(LINE, ti, B, A,
                            B  + TYPE_ALLOCS + 1,
                            B  + TYPE_ALLOCS + 1 == A );
                }
            } else {
                ASSERTV(LINE, ti, BB, AA, (int)LENGTH,
                        BB + 1 + (int)LENGTH * TYPE_ALLOC,
                        BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                ASSERTV(LINE, ti, B, A, (int)LENGTH,
                        B  + 1 + (int)LENGTH * TYPE_ALLOC,
                        B  + 1 + (int)LENGTH * TYPE_ALLOC == A );
            }
        }
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
            CONTAINER mU(UA);   const CONTAINER& U  = mU;
            Obj       mY;       const Obj&       Y  = gg(&mY, SPEC);

            const bsls::Types::Int64 BB = ta.numBlocksTotal();
            const bsls::Types::Int64 B  = ta.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {

                Obj        mX(U.begin(), U.end(), &ta);
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(LINE, ti, LENGTH,   X.size(),
                                  LENGTH == X.size());
                ASSERTV(LINE, ti, LENGTH,   X.capacity(),
                                  LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    ASSERTV(LINE, ti, j, Y[j], X[j], Y[j] == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = ta.numBlocksTotal();
            const bsls::Types::Int64 A  = ta.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            if (LENGTH == 0) {
                ASSERTV(LINE, ti, BB, AA, BB + 0 == AA);
                ASSERTV(LINE, ti, B,  A,  B  + 0 == A );
            }
            else {
                const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                if (INPUT_ITERATOR_TAG) {
                    ASSERTV(LINE, ti, BB, AA, BB + 1 + TYPE_ALLOCS <= AA);
                    ASSERTV(LINE, ti, B,  A,  B  + 0               == A );
                } else {
                    ASSERTV(LINE, ti, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(LINE, ti, B,  A,  B  + 0               == A );
                }
            }

            ASSERTV(LINE, ti, 0 == ta.numBlocksInUse());
        }
    }
#endif  // BDE_BUILD_TARGET_EXC

    const int RANDOM_ACCESS_ITERATOR_TAG =
          bsl::is_same<std::random_access_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    if (RANDOM_ACCESS_ITERATOR_TAG) {
        if (verbose) { printf("\tNegative testing\n"); }

        bsls::AssertTestHandlerGuard guard;

        const char *SPECS[] = {
            "A",
            "AB",
            "ABC",
            "ABCD",
            "ABCDE",
            "ABCDEAB",
            "ABCDEABC",
            "ABCDEABCD"
        };
        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        if (verbose) printf("\t\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_SPECS; ++ti) {
                const char   *SPEC   = SPECS[ti];
                const size_t  LENGTH = strlen(SPEC);

                if (verbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
                CONTAINER mU(UA);   const CONTAINER& U  = mU;

                typename CONTAINER::const_iterator it1 = U.begin(); ++it1;

                ASSERT_SAFE_PASS_RAW(Obj mX(U.begin(), U.begin()));
                ASSERT_SAFE_FAIL_RAW(Obj mX(it1,       U.begin()));
                ASSERT_SAFE_FAIL_RAW(Obj mX(U.end(),   U.begin()));
                ASSERT_SAFE_PASS_RAW(Obj mX(U.end(),   U.end()  ));

                // two null pointers form a valid (empty) range
                const TYPE *nullPtr = 0;
                ASSERT_SAFE_PASS_RAW(Obj mX(nullPtr, nullPtr));
            }
        }

        if (verbose) printf("\t\tWith passing in an allocator.\n");
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_SPECS; ++ti) {
                const char   *SPEC   = SPECS[ti];
                const size_t  LENGTH = strlen(SPEC);

                if (verbose) {
                    printf("\t\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                Obj       mUA;      const Obj&       UA = gg(&mUA, SPEC);
                CONTAINER mU(UA);   const CONTAINER& U  = mU;

                typename CONTAINER::const_iterator it1 = U.begin(); ++it1;

                ASSERT_SAFE_PASS_RAW(Obj mX(U.begin(), U.begin(), &ta));
                ASSERT_SAFE_FAIL_RAW(Obj mX(it1,       U.begin(), &ta));
                ASSERT_SAFE_FAIL_RAW(Obj mX(U.end(),   U.begin(), &ta));
                ASSERT_SAFE_PASS_RAW(Obj mX(U.end(),   U.end(),   &ta));

                // two null pointers form a valid (empty) range
                const TYPE *nullPtr = 0;
                ASSERT_SAFE_PASS_RAW(Obj mX(nullPtr, nullPtr, &ta));
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase12Ambiguity()
{
    // Concerns:
    //: 1 A 'vector' is created with expected number of elements (given by the
    //:   first argument), each having the expected value (given by the second
    //:   argument).
    //: 2 The newly created vector has the expected capacity, equal to the
    //:   size.
    //: 3 The requested size might, or might not, be a power of two.
    //: 4 If the requested size is zero, no memory is allocated.
    //: 5 All memory is supplied by the expected allocator, and no additional
    //:   memory is requested from the default allocator.
    //: 6 No memory leaks if an exception is thrown during construction.
    //: 7 The requested element value might, or might not, have the same value
    //:   as the requested size.
    //: 8 Where representable, requesting more than 'max_size()' elements will
    //:   throw a 'std::length_error' exception.
    //: 9 QoI TBD: For a type that is neither integral nor an iterator, but is
    //:   convertible to an integral value, we should be able to create a
    //:   vector having the number of element specified by converting to an
    //:   integer from the first argument of the convertible type.
    //
    // Note that for the allocator-aware concerns, 'TYPE' must be an allocator-
    // aware type that is implicitly convertible from some integral type.

    static const struct {
        int         d_lineNum;  // source line number
        signed char d_length;   // expected length, as smallest integral type
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 },
        { L_,       27 },  // one extra value away from expected boundaries
        { L_,       63 },
        { L_,       64 },
        { L_,       65 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    const TYPE t0(0);
    const TYPE t1(1);
    const TYPE t127(127);

    {
        if (verbose) printf("\tWithout passing in an allocator.\n");
        {
            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'signed char'\n");
            }
            {
                Obj mX((signed char)0, t0);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY((signed char)0, t1);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ((signed char)0, t127);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }

            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by raw constants\n");
            }
            {
                Obj mX(0, t0);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY(0, t1);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ(0, t127);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }

            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'TYPE'\n");
            }
            {
                Obj mX((size_t)0, t0);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY((size_t)0, t1);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ((size_t)0, t127);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }


            for (signed char ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const signed char LENGTH = DATA[ti].d_length;

                const struct {
                    int         d_lineNum;  // source line number
                    signed char d_value;    // value for constructor to copy
                } VALUES[] = {
                    //line  length
                    //----  ------
                    { L_,   0          },
                    { L_,   (signed char)(LENGTH - 1) },
                    { L_,   (signed char) LENGTH      },
                    { L_,   (signed char)(LENGTH + 1) },
                    { L_,   127        }
                };
                enum { NUM_VALUES = sizeof VALUES / sizeof *VALUES };

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'signed char'\n");
                    }

                    Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'int'\n");
                    }

                    Obj        mX((int)LENGTH, DEFAULT);
                    const Obj&  X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'size_t'\n");
                    }

                    Obj        mX((size_t)LENGTH, DEFAULT);
                    const Obj&  X = mX;


                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }
                }
            }
        }

        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {
            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'TYPE'\n");
            }
            {
                Obj mX((signed char)0, t0, xta);
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY((signed char)0, t1, xta);
                const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ((signed char)0, t127, xta);
                const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }

            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'signed char'\n");
            }
            {
                Obj mX(0, 0, xta);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY(0, 1, xta);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ(0, 127, xta);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }

            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'signed char'\n");
            }
            {
                Obj mX((size_t)0, t0, xta);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY((size_t)0, t1, xta);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 0 == Y.capacity());

                Obj mZ((size_t)0, t127, xta);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     0 == Z.size());
                ASSERTV(Z.capacity(), 0 == Z.capacity());
            }


            for (signed char ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const signed char LENGTH = DATA[ti].d_length;

                const struct {
                    int         d_lineNum;  // source line number
                    signed char d_value;    // value for constructor to copy
                } VALUES[] = {
                    //line  length
                    //----  ------
                    { L_,   0          },
                    { L_,   (signed char)(LENGTH - 1) },
                    { L_,   (signed char) LENGTH      },
                    { L_,   (signed char)(LENGTH + 1) },
                    { L_,   127        }
                };
                enum { NUM_VALUES = sizeof VALUES / sizeof *VALUES };

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'signed char'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    Obj mX(LENGTH, VALUE, xta);  const Obj& X = mX;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }

                    ASSERTV(LINE2, ti, BB + 1 + LENGTH * TYPE_ALLOC,   AA,
                                       BB + 1 + LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE2, ti, B  + 1 + LENGTH * TYPE_ALLOC,   A,
                                       B  + 1 + LENGTH * TYPE_ALLOC == A );
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'int'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    Obj        mX((int)LENGTH, (int)VALUE, xta);
                    const Obj&  X = mX;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }

                    ASSERTV(LINE2, ti, BB + 1 + LENGTH * TYPE_ALLOC,   AA,
                                       BB + 1 + LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE2, ti, B  + 1 + LENGTH * TYPE_ALLOC,   A,
                                       B  + 1 + LENGTH * TYPE_ALLOC == A );
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'size_t'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    Obj        mX((size_t)LENGTH, DEFAULT, xta);
                    const Obj&  X = mX;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }

                    ASSERTV(LINE2, ti, BB + 1 + LENGTH * TYPE_ALLOC,   AA,
                                       BB + 1 + LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE2, ti, B  + 1 + LENGTH * TYPE_ALLOC,   A,
                                       B  + 1 + LENGTH * TYPE_ALLOC == A );
                }
            }
        }

#if defined(BDE_BUILD_TARGET_EXC)
        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {

            for (signed char ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const signed char LENGTH = DATA[ti].d_length;

                const struct {
                    int         d_lineNum;  // source line number
                    signed char d_value;    // value for constructor to copy
                } VALUES[] = {
                    //line  length
                    //----  ------
                    { L_,   0          },
                    { L_,   (signed char)(LENGTH - 1) },
                    { L_,   (signed char) LENGTH      },
                    { L_,   (signed char)(LENGTH + 1) },
                    { L_,   127        }
                };
                enum { NUM_VALUES = sizeof VALUES / sizeof *VALUES };

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'signed char'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {

                        Obj mX(LENGTH, VALUE, xta);  const Obj& X = mX;

                        if (veryVerbose) {
                            T_; T_; P_(X); P(X.capacity());
                        }

                        ASSERTV(LINE2, ti, LENGTH, X.size(),
                                static_cast<size_t>(LENGTH) == X.size());
                        ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                                static_cast<size_t>(LENGTH) == X.capacity());

                        // Use 'x.size()' rather than 'LENGTH' to ensure there
                        // is no out-of-contract 'operator[]' call in the case
                        // that a previous 'ASSERT' reports an error.
                        for (size_t j = 0; j != X.size(); ++j) {
                            ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A); }

                    // Because of exceptions, the number of allocations will be
                    // 'LENGTH' trials that allocate the array for the vector,
                    // plus '1 + 2 + ... + LENGTH == LENGTH * (1 + LENGTH) / 2'
                    // for the vector elements at each successive trial, plus
                    // one for the final trial that succeeds.

                    const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    ASSERTV(LINE2, ti, BB + 1 + TYPE_ALLOCS,   AA,
                                       BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(LINE2, ti, B, A, B == A);

                    ASSERTV(LINE2, ti, 0 == ta.numBlocksInUse());
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'int'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    Obj        mX((int)LENGTH, (int)VALUE, xta);
                    const Obj&  X = mX;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }

                    ASSERTV(LINE2, ti, BB + 1 + LENGTH * TYPE_ALLOC,   AA,
                                       BB + 1 + LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE2, ti, B  + 1 + LENGTH * TYPE_ALLOC,   A,
                                       B  + 1 + LENGTH * TYPE_ALLOC == A );
                }

                for (int tj = 0; tj < NUM_VALUES; ++tj) {
                    const int         LINE2 = VALUES[tj].d_lineNum;
                    const signed char VALUE = VALUES[tj].d_value;

                    const TYPE DEFAULT(VALUE);

                    if (verbose) {
                        printf("\t\tCreating ");  P_(LENGTH);
                        printf("objects of value "); P_(VALUE);
                        printf("indexed by 'size_t'\n");
                    }

                    const bsls::Types::Int64 BB = ta.numBlocksTotal();
                    const bsls::Types::Int64 B  = ta.numBlocksInUse();

                    Obj        mX((size_t)LENGTH, DEFAULT, xta);
                    const Obj&  X = mX;

                    const bsls::Types::Int64 AA = ta.numBlocksTotal();
                    const bsls::Types::Int64 A  = ta.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    ASSERTV(LINE2, ti, LENGTH, X.size(),
                            static_cast<size_t>(LENGTH) == X.size());
                    ASSERTV(LINE2, ti, LENGTH,   X.capacity(),
                            static_cast<size_t>(LENGTH) == X.capacity());

                    // Use 'x.size()' rather than 'LENGTH' to ensure there is
                    // no out-of-contract 'operator[]' call in the case that a
                    // previous 'ASSERT' reports an error.
                    for (size_t j = 0; j != X.size(); ++j) {
                        ASSERTV(LINE, ti, tj, j, DEFAULT == X[j]);
                    }

                    ASSERTV(LINE2, ti, BB + 1 + LENGTH * TYPE_ALLOC,   AA,
                                       BB + 1 + LENGTH * TYPE_ALLOC == AA);
                    ASSERTV(LINE2, ti, B  + 1 + LENGTH * TYPE_ALLOC,   A,
                                       B  + 1 + LENGTH * TYPE_ALLOC == A );
                }
            }
        }
#endif  // BDE_BUILD_TARGET_EXC
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE, ALLOC>::testCase12AmbiguityForPointers()
{
    // Concerns:
    //: 1 '0' is a null-pointer literal that can, via template deduction, be
    //:   deduced as either an integer or a pointer value.  Operations that
    //:   are overloaded to take an iterator range, and a value and number of
    //:   copies, should always treat the arguments '0, 0' as a request for no
    //:   null-pointers, rather than a nonsensical iterator range.  Testing
    //:   for detailed behavior is already covered by the primary testing, so
    //:   simply provide sanity checks that the correct overload is called.

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ALLOC                xta(&ta);


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<TYPE>().name(),
                        TYPE_ALLOC);

    {
        if (verbose) printf("\tWithout passing in an allocator.\n");
        {
            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'signed char'\n");
            }
            {
                Obj mX(0, 0);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                mX.insert(X.begin(), 0, 0);

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                mX.assign(0, 0);

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY(1, 0);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     1 == Y.size());
                ASSERTV(Y.capacity(), 1 == Y.capacity());

                mY.insert(Y.begin(), 0, 0);

                ASSERTV(Y.size(),     1 == Y.size());
                ASSERTV(Y.capacity(), 1 == Y.capacity());

                mY.insert(Y.begin(), 1, 0);

                ASSERTV(Y.size(),     2 == Y.size());
                ASSERTV(Y.capacity(), 2 <= Y.capacity());

                mY.assign(0, 0);

                ASSERTV(Y.size(),     0 == Y.size());
                ASSERTV(Y.capacity(), 2 <= Y.capacity());

                mY.assign(1, 0);

                ASSERTV(Y.size(),     1 == Y.size());
                ASSERTV(Y.capacity(), 2 <= Y.capacity());

                Obj mZ(127, 0);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     127 == Z.size());
                ASSERTV(Z.capacity(), 127 <= Z.capacity());

                mZ.insert(Z.begin(), 0, 0);

                ASSERTV(Z.size(),     127 == Z.size());
                ASSERTV(Z.capacity(), 127 == Z.capacity());

                mZ.insert(Z.begin(), 127, 0);

                ASSERTV(Z.size(),     254 == Z.size());
                ASSERTV(Z.capacity(), 254 <= Z.capacity());

                mZ.assign(1, 0);

                ASSERTV(Z.size(),       1 == Z.size());
                ASSERTV(Z.capacity(), 254 <= Z.capacity());

                mZ.assign(127, 0);

                ASSERTV(Z.size(),     127 == Z.size());
                ASSERTV(Z.capacity(), 254 <= Z.capacity());
            }

        }

        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {

            if (verbose) {
                printf("\t\tCreating empty vector of objects, supplying a "
                          "value to copy 0 times, indexed by 'signed char'\n");
            }
            {
                Obj mX(0, 0, xta);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                ASSERTV(X.size(),     0 == X.size());
                ASSERTV(X.capacity(), 0 == X.capacity());

                Obj mY(1, 0, xta);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_; T_; P_(Y); P(Y.capacity());
                }

                ASSERTV(Y.size(),     1 == Y.size());
                ASSERTV(Y.capacity(), 1 == Y.capacity());

                Obj mZ(127, 0, xta);  const Obj& Z = mZ;

                if (veryVerbose) {
                    T_; T_; P_(Z); P(Z.capacity());
                }

                ASSERTV(Z.size(),     127 == Z.size());
                ASSERTV(Z.capacity(), 127 == Z.capacity());
            }
        }
    }
}

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

    // Test integrity of DATA

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 23: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTION"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType,
                      NotAssignable,
                      BitwiseNotAssignable);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase23,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING NON-STANDARD TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NON-STANDARD TYPES"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase22,
                      bsltf::NonTypicalOverloadsTestType);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING USE OF 'std::length_error'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USE OF 'std::length_error'"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase21,
                      int,
                      const char *,
                      bsltf::AllocTestType,
                      bsltf::TemplateTestFacility::ObjectPtr,
                      bsltf::TemplateTestFacility::FunctionPtr);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE COMPARISON OPERATORS"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase20,
                      signed char,
                      const char *,
                      bsltf::AllocTestType,
                      bsltf::TemplateTestFacility::ObjectPtr,
                      bsltf::TemplateTestFacility::FunctionPtr);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'erase' AND 'pop_back'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'erase' AND 'pop_back'"
                            "\n==============================\n");

        if (verbose) printf("\nbasic testing of 'erase'"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose) printf("\nNegative testing 'erase'"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase19Negative,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose) printf("\nbasic and negative testing of 'pop_back'"
                            "\n========================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase19_pop_back,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      NotAssignable);

        if (verbose) printf(
                      "\nTesting 'erase' and 'pop_back' with std allocator"
                      "\n=================================================\n");

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase19,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase19Negative,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase19_pop_back,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      NotAssignable);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING RANGE INSERTION
        // --------------------------------------------------------------------

        // Note that each invocation of the 'ITER_CONTAINER_RUN_EACH_TYPE'
        // macro below requires a list of exactly 4 types.  Focused lists of
        // types designed to stress the broadest range of 'vector' template
        // instantiations are chosen.  Note that concerns for "move only" types
        // are delegated to higher level concerns in test cases 25 and above.

        if (verbose) printf("\nTESTING RANGE INSERTION"
                            "\n=======================\n");

        if (verbose) printf("\nFirst, with scalar types:"
                            "\n========================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase18Range,
                                     char,
                                     bsltf::EnumeratedTestType::Enum,
                                     bsltf::TemplateTestFacility::MethodPtr,
                                     size_t);
#if 0
        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase18Range,
                                     bool,
                                     char16_t,
                                     char32_t,
                                     bsl::nullptr_t);
#endif
        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase18Range,
                                     const char *,
                                     volatile void *,
                                     bsltf::TemplateTestFacility::ObjectPtr,
                                     bsltf::TemplateTestFacility::FunctionPtr);

        if (verbose) printf("\nSecond, with non-allocating types:"
                            "\n=================================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase18Range,
                                     bsltf::NonDefaultConstructibleTestType,
                                     bsltf::NonTypicalOverloadsTestType,
                                     bsltf::BitwiseMoveableTestType,
                                     bsltf::BitwiseCopyableTestType);

        if (verbose) printf("\nThird, with move-optimized types:"
                            "\n=================================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase18Range,
                                     bsltf::AllocBitwiseMoveableTestType,
                                     bsltf::AllocTestType,
                                     bsltf::MovableTestType,
                                     bsltf::MovableAllocTestType);

        if (verbose) printf(
                     "\nFinally, testing iterator vs. value type deduction"
                     "\n==================================================\n");

        {
            vector<size_t> vna;
            vna.insert(vna.end(), 13, 42);

            ASSERT(13 == vna.size());
            ASSERT(42 == vna.front());
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INSERTION"
                            "\n=================\n");

        if (verbose) printf("\nFirst, with BDE allocators:"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase17_push_back,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      testCase17_insert_constref,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      testCase17_n_copies,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);


        if (verbose) printf("\nThen, with 'std' allocators:"
                            "\n============================\n");

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase17_push_back,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase17_insert_constref,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase17_n_copies,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        if (verbose) printf("\nNegative Testing Insertions"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase17Negative,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ELEMENT ACCESS"
                            "\n======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      testCase15Negative,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'reserve', 'resize', AND 'capacity'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'reserve', 'resize', AND 'capacity'"
                            "\n===========================================\n");

        // TBD: Test coverage with a default-constructible, non-assignable type
        RUN_EACH_TYPE(TestDriver2,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      testCase14a,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase14,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase14a,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'assign'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'assign'"
                            "\n================\n");

        if (verbose) printf("\nTesting Initial-Length Assignment"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Range,
                                     char,
                                     bsltf::TemplateTestFacility::ObjectPtr,
                                     bsltf::TemplateTestFacility::FunctionPtr,
                                     bsltf::TemplateTestFacility::MethodPtr);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Range,
                                     const char *,
                                     bsltf::NonTypicalOverloadsTestType,
                                     bsltf::BitwiseMoveableTestType,
                                     bsltf::BitwiseCopyableTestType);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Range,
                                     bsltf::AllocBitwiseMoveableTestType,
                                     bsltf::AllocTestType,
                                     bsltf::MovableTestType,
                                     bsltf::MovableAllocTestType);



        if (verbose) printf("\nNegative-testing Assignment"
                            "\n===========================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Negative,
                                     char,
                                     bsltf::TemplateTestFacility::ObjectPtr,
                                     bsltf::TemplateTestFacility::FunctionPtr,
                                     bsltf::TemplateTestFacility::MethodPtr);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Negative,
                                     const char *,
                                     bsltf::NonTypicalOverloadsTestType,
                                     bsltf::BitwiseMoveableTestType,
                                     bsltf::BitwiseCopyableTestType);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase13Negative,
                                     bsltf::AllocBitwiseMoveableTestType,
                                     bsltf::AllocTestType,
                                     bsltf::MovableTestType,
                                     bsltf::MovableAllocTestType);


        if (verbose) printf("\nTest iterator vs. value type deduction"
                            "\n======================================\n");

        {
            vector<size_t> vna;
            vna.assign(13, 42);
            ASSERT(13 == vna.size());
            ASSERT(42 == vna.front());
        }

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase13,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        if (verbose) printf(
               "\nTesting Initial-Length Constructor (using default value)"
               "\n========================================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        if (verbose) printf(
               "\nTesting Initial-Length Constructor (copying given value)"
               "\n========================================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase12NoDefault,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      NotAssignable,
                      BitwiseNotAssignable);

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase12Range,
                                     char,
                                     bsltf::TemplateTestFacility::ObjectPtr,
                                     bsltf::TemplateTestFacility::FunctionPtr,
                                     bsltf::TemplateTestFacility::MethodPtr);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase12Range,
                                     const char *,
                                     bsltf::NonTypicalOverloadsTestType,
                                     bsltf::BitwiseMoveableTestType,
                                     bsltf::BitwiseCopyableTestType);

        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver2,
                                     testCase12Range,
                                     bsltf::AllocBitwiseMoveableTestType,
                                     bsltf::AllocTestType,
                                     NotAssignable,
                                     BitwiseNotAssignable);

        if (verbose) printf("\nTesting Initial-Range vs. -Length Ambiguity"
                            "\n===========================================\n");

        TestDriver2<char  >::testCase12Ambiguity();
        TestDriver2<int   >::testCase12Ambiguity();
        TestDriver2<size_t>::testCase12Ambiguity();
        TestDriver2<double>::testCase12Ambiguity();
        TestDriver2<bsltf::AllocTestType>::testCase12Ambiguity();
        TestDriver2<const char *>::testCase12AmbiguityForPointers();
        TestDriver2<char(*)(...)>::testCase12AmbiguityForPointers();

        if (verbose) printf("\nTesting with standard-conforming allocators"
                            "\n===========================================\n");

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase12,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 11: BSLA_FALLTHROUGH;
      case 10: BSLA_FALLTHROUGH;
      case  9: BSLA_FALLTHROUGH;
      case  8: BSLA_FALLTHROUGH;
      case  7: BSLA_FALLTHROUGH;
      case  6: BSLA_FALLTHROUGH;
      case  5: BSLA_FALLTHROUGH;
      case  4: BSLA_FALLTHROUGH;
      case  3: BSLA_FALLTHROUGH;
      case  2: BSLA_FALLTHROUGH;
      case  1: {
        if (verbose)
            printf("Test case %d is in another test driver part.\n"
                   "See 'bslstl_vector.0.t.cpp' for the test plan.\n",
                   test);
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
