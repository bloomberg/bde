// bslstl_deque.2.t.cpp                                               -*-C++-*-
#define BSLSTL_DEQUE_0T_AS_INCLUDE
#include <bslstl_deque.0.t.cpp>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_deque.0.t.cpp'.

// ============================================================================
//                       TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver2 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_DEQUE_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

                            // TEST CASES
    // CLASS METHODS
    static void testCase22();
        // Test proper use of 'std::length_error'.

    static void testCase21();
        // Test free comparison operators.

    static void testCase20_swap_noexcept();
        // Test noexcept specification of 'swap'.

    static void testCase20_dispatch();
        // Test 'swap' member.

    static void testCase19();
        // Test 'erase', 'pop_back', and 'pop_front'.

    template <class CONTAINER>
    static void testCase18(const CONTAINER&);
        // Test range 'insert' member.

    static void testCase17();
        // Test value 'insert' members.

    static void testCase16();
        // Test 'push_back' and 'push_front' members.

    static void testCase15();
        // Test iterators.

    static void testCase14();
        // Test element access.

    static void testCase13();
        // Test reserve and capacity-related methods.

    static void testCase12();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test 'assign' member template.

    static void testCase11();
        // Test value constructors.

    template <class CONTAINER>
    static void testCase11Range(const CONTAINER&);
        // Test range constructor.
};

                  // ==================================
                  // template class StdBslmaTestDriver2
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver2 : public StdBslmaTestDriverHelper<TestDriver2, TYPE>
{
};

                               // ----------
                               // TEST CASES
                               // ----------

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING 'std::length_error'
    //
    // Concerns:
    //   1) That any call to a constructor, 'assign', 'push_back' or 'insert'
    //      that would result in a value exceeding 'max_size()' throws
    //      'std::length_error'.
    //   2) That the 'max_size()' taken into consideration is that of the
    //      allocator, and not an absolute constant.
    //   3) That the value of the deque is unchanged if an exception is
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
    //   CONCERN: 'std::length_error' is used properly.
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    LimitAllocator<ALLOC> a(&oa);
    a.setMaxSize((size_t)-1);

    const int LENGTH = 32;
    typedef bsl::deque<TYPE,LimitAllocator<ALLOC> > LimitObj;

    LimitObj mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nConstructor 'deque(n, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nConstructor 'deque(n, T x, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nConstructor 'deque<Iter>(f, l, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nWith 'resize'.\n");
    {
        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);

                mX.resize(LENGTH);
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nWith 'resize' using a value.\n");
    {
        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);

                mX.resize(LENGTH, DEFAULT_VALUE);
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nWith 'assign'.\n");

    for (int assignMethod = 0; assignMethod <= 1; ++assignMethod) {

        if (veryVerbose) {
            switch (assignMethod) {
              case 0: printf("\tWith assign(n, T x).\n"); break;
              case 1: printf("\tWith assign<Iter>(f, l).\n"); break;
              default: ASSERT(0);
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);

                switch (assignMethod) {
                  case 0: {
                    mX.assign(LENGTH, DEFAULT_VALUE);
                  } break;
                  case 1: {
                    mX.assign(Y.begin(), Y.end());
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nWith 'insert'.\n");

    for (int insertMethod = 0; insertMethod <= 3; ++insertMethod) {

        if (verbose) {
            switch (insertMethod) {
              case 0: printf("\tWith push_back(c).\n");          break;
              case 1: printf("\tWith insert(p, T x).\n");        break;
              case 2: printf("\tWith insert(p, n, T x).\n");     break;
              case 3: printf("\tWith insert<Iter>(p, f, l).\n"); break;
              default: ASSERT(0);
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);  const LimitObj& X = mX;

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
                  default: ASSERT(0);
                };
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    const int PADDING = 16;
    const Obj X;

    if (verbose) printf("\nTesting requests for '(size_t) -1' elements with "
                        "default allocator.\n");

    if (verbose) printf("\nConstructor 'deque(n, T x, a = A())'"
                        " and 'max_size()' equal to %llu.\n",
                        (Uint64)X.max_size());
    {
        bool exceptionCaught = false;

        if (veryVerbose) {
            size_t minus2 = (size_t) -2;
            printf("\tWith max_size() equal to %llu.\n", (Uint64)minus2);
        }

        try {
            Obj mX(-1, DEFAULT_VALUE);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nWith 'reserve/resize' and"
                        " 'max_size()' equal to %llu.\n",
                        (Uint64)X.max_size());

    for (int capacityMethod = 0; capacityMethod <= 2; ++capacityMethod)
    {
        bool exceptionCaught = false;

        if (verbose) {
            switch (capacityMethod) {
              case 0: printf("\tWith reserve(n).\n");        break;
              case 1: printf("\tWith resize(n).\n");         break;
              case 2: printf("\tWith resize(n, T x).\n");    break;
              default: ASSERT(0);
            };
        }

        try {
            Obj mX;

            switch (capacityMethod) {
              case 0:  mX.reserve((size_t)-1);                  break;
              case 1:  mX.resize((size_t)-1);                   break;
              case 2:  mX.resize((size_t)-1, DEFAULT_VALUE);    break;
              default: ASSERT(0);
            };
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
            ASSERT(0);
        }
        catch (...) {
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
            ASSERT(0);
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());

    if (verbose) printf("\nTesting requests for 'X.max_size() + n' elements "
                        "with 'insert' and 'max_size()' equal to %llu.\n",
                        (Uint64)X.max_size());

    for (int insertMethod = 0; insertMethod <= 1; insertMethod += 2) {

        if (verbose) {
            switch (insertMethod) {
              case 0: printf("\tWith insert(pos, n, C c).\n");        break;
              case 1: printf("\tWith insert(p, n, C c).\n");          break;
              default: ASSERT(0);
            };
        }

        for (int limit = 1; limit <= 5; ++limit) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length 'max_size()' plus %d.\n",
                       limit);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);  const Obj& X = mX;

                const size_t LENGTH = X.max_size() - PADDING + limit;
                switch (insertMethod) {
                  case 0: {
                    mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                  } break;
                  default: ASSERT(0);
                };
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBytesInUse());
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase21()
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
    //   For a variety of 'deque's of different sizes and different values,
    //   test that the comparison returns as expected.  Note that capacity is
    //   not of concern here, the implementation specifically uses only
    //   'begin()', 'end()', and 'size()'.
    //
    // Testing:
    //   bool operator< (const deque& lhs, const deque& rhs);
    //   bool operator> (const deque& lhs, const deque& rhs);
    //   bool operator<=(const deque& lhs, const deque& rhs);
    //   bool operator>=(const deque& lhs, const deque& rhs);
    //   auto operator<=>(const deque& lhs, const deque& rhs);
    // ------------------------------------------------------------------------

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
        0  // null string required as last element
    };

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, v) in S X S \n.");
    {
        // Create first object.
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            Obj mU;  const Obj& U = gg(&mU, U_SPEC);

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object.
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                Obj mV;  const Obj& V = gg(&mV, V_SPEC);

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                const bool isLess = si < sj;
                const bool isLessEq = si <= sj;
                LOOP2_ASSERT(si, sj,  isLess   == (U < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V));
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
void TestDriver2<TYPE,ALLOC>::testCase20_swap_noexcept()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: NOEXCEPT SPECIFICATIONS
    //
    // Concerns:
    //: 1 If 'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the 'swap' functions are 'noexcept(true)'.
    //
    // Plan:
    //: 1 Compare the value of the trait with the member 'swap' function
    //:   noexcept specification.
    //:
    //: 2 Compare the value of the trait with the free 'swap' function noexcept
    //:   specification.
    //
    // Testing:
    //   deque::swap()
    //   swap(deque& , deque& )
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::deque<TYPE, ALLOC> a, b;

    const bool isNoexcept = AllocatorTraits::is_always_equal::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(a,b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase20_dispatch()
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
    //: 9 If 'allocator_traits<Allocator>::is_always_equal::value' is
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
    //: 6 To address concern 9 pass allocators with both 'is_always_equal'
    //:   values (true & false).
    //
    // Testing:
    //   void swap(deque& other);
    //   void swap(deque& a, deque& b);
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
                        bsls::NameOf<TYPE>().name(),
                        otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    // Assign the address of each function to a variable.
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator doa("default",   veryVeryVeryVerbose);
    bslma::TestAllocator ooa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator zoa("other",     veryVeryVeryVerbose);
    bslma::TestAllocator soa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&doa);

    ALLOC da(&doa);
    ALLOC oa(&ooa);
    ALLOC za(&zoa);
    ALLOC sa(&soa);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
               AllocatorTraits::propagate_on_container_move_assignment::value);
    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    // Use a table of distinct object values and expected memory usage.

    enum { NUM_DATA                         = DEFAULT_NUM_DATA };
    const  DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (veryVerbose) {
        printf("Testing swap with matching allocs, no exceptions.\n");
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *SPEC1 = DATA[ti].d_spec;

        Obj mW(oa);     const Obj& W  = gg(&mW,  SPEC1);
        Obj mXX(sa);    const Obj& XX = gg(&mXX, SPEC1);

        if (veryVerbose) { printf("noexcep: src: "); P_(SPEC1) P(XX) }

        // Ensure the first row of the table contains the default-constructed
        // value.

        if (0 == ti) {
            ASSERTV(SPEC1, Obj(sa), W, Obj(sa) == W);
        }

        for (int member = 0; member < 2; ++member) {
            bslma::TestAllocatorMonitor oam(&ooa);

            if (member) {
                mW.swap(mW);    // member 'swap'
            }
            else {
                swap(mW, mW);   // free function 'swap'
            }

            ASSERTV(SPEC1, XX, W, XX == W);
            ASSERTV(SPEC1, oa == W.get_allocator());
            ASSERTV(SPEC1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mYY(sa);   const Obj& YY = gg(&mYY, SPEC2);

            if (veryVerbose) { printf("noexcep: src: "); P_(SPEC1) P(YY) }

            ASSERT((ti == tj) == (XX == YY));

            for (int member = 0; member < 2; ++member) {
                Obj mX(oa);    const Obj& X  = gg(&mX,  SPEC1);
                Obj mY(oa);    const Obj& Y  = gg(&mY,  SPEC2);

                if (veryVerbose) {
                    T_ printf("before: "); P_(X) P(Y);
                }

                ASSERT(XX == X && YY == Y);

                if (veryVerbose) { T_ P_(SPEC2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&ooa);

                if (member) {
                    mX.swap(mY);    // member 'swap'
                }
                else {
                    swap(mX, mY);   // free function 'swap'
                }

                ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                ASSERTV(SPEC1, SPEC2, XX, Y, XX == Y);
                ASSERTV(SPEC1, SPEC2, oa == X.get_allocator());
                ASSERTV(SPEC1, SPEC2, oa == Y.get_allocator());
                ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
            }
        }
    }

    if (veryVerbose) {
        printf("Testing swap, non-matching, with injected exceptions.\n");
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1  = DATA[ti].d_spec;
        const size_t      LENGTH1 = strlen(DATA[ti].d_results);

        Obj mXX(sa);    const Obj& XX = gg(&mXX, SPEC1);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;
            const size_t      LENGTH2 = strlen(DATA[tj].d_results);

            if (4 < LENGTH2) {
                continue;    // time consuming, skip (it's O(LENGTH2^2))
            }

            Obj mYY(sa);   const Obj& YY = gg(&mYY, SPEC2);

            ASSERT((ti == tj) == (XX == YY));

            for (int member = 0; member < 2; ++member) {
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                    ++numPasses;

                    Int64 al = ooa.allocationLimit();
                    ooa.setAllocationLimit(-1);

                    Obj mX(oa);    const Obj& X = gg(&mX, SPEC1);
                    Obj mZ(za);    const Obj& Z = gg(&mZ, SPEC2);

                    if (veryVerbose) {
                        T_ printf("before: "); P_(X) P(Z);
                    }

                    ASSERT(XX == X && YY == Z);

                    ooa.setAllocationLimit(al);
                    bslma::TestAllocatorMonitor oam(&ooa);
                    bslma::TestAllocatorMonitor oazm(&zoa);

                    if (member) {
                        mX.swap(mZ);     // member 'swap'
                    }
                    else {
                        swap(mX, mZ);    // free function 'swap'
                    }

                    ooa.setAllocationLimit(-1);

                    if (veryVerbose) {
                        T_ printf("after:  "); P_(X) P(Z);
                    }

                    ASSERTV(SPEC1, SPEC2, YY, X, YY == X);
                    ASSERTV(SPEC1, SPEC2, XX, Z, XX == Z);
                    ASSERTV(SPEC1, SPEC2, (isPropagate ? za : oa) ==
                                                            X.get_allocator());
                    ASSERTV(SPEC1, SPEC2, (isPropagate ? oa : za) ==
                                                            Z.get_allocator());

                    ASSERTV(SPEC1, SPEC2, !isPropagate == oam.isTotalUp());
                    ASSERT(!PLAT_EXC ||   !isPropagate == (1 < numPasses));
                    ASSERTV(SPEC1, SPEC2, !isPropagate == oazm.isTotalUp());

                    ASSERTV(SPEC1, SPEC2, (LENGTH1 != LENGTH2 && !isPropagate)
                                 || (oam.isInUseSame() && oazm.isInUseSame()));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }

    {
        // 'A' values: Should cause memory allocation if possible.

        Obj mX(oa);     const Obj& X  = gg(&mX,  "DD");
        Obj mXX(sa);    const Obj& XX = gg(&mXX, "DD");

        Obj mY(oa);     const Obj& Y  = gg(&mY,  "ABC");
        Obj mYY(sa);    const Obj& YY = gg(&mYY, "ABC");

        if (veryVerbose) printf(
              "Invoke free 'swap' function in a context where ADL is used.\n");

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&ooa);

        invokeAdlSwap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }

        if (veryVerbose) printf("Invoke std BDE pattern 'swap' function.\n");

        invokePatternSwap(&mX, &mY);

        ASSERTV(YY, X, XX == X);
        ASSERTV(XX, Y, YY == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == doa.numBlocksTotal());

    // Test noexcept specifications of the 'swap' functions.
    testCase20_swap_noexcept();
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concerns:
    //: 1 That the resulting value is correct.
    //:
    //: 2 That erasing a suffix of the array never allocates, and thus never
    //:   throws.  In particular, 'pop_back()' and 'erase(..., X.end())' do not
    //:   throw.
    //:
    //: 3 That erasing is exception neutral w.r.t. memory allocation.
    //:
    //: 4 That no memory is leaked.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a deque
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using a
    //        'bslma::TestAllocator' and varying its *allocation* *limit*, but
    //        not computing the number of allocations or checking on the value
    //        in case an exception is thrown (it is enough to verify that all
    //        the elements have been destroyed indirectly by making sure that
    //        there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   void pop_front();
    //   void pop_back();
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'pop_back' on non-empty deques.\n");
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

                Obj mX(xoa);  const Obj& X = mX;

                stretch(&mX, INIT_LENGTH);
                mX.reserve(INIT_CAP);

                unsigned k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    bsls::ObjectBuffer<TYPE> buffer;
                    TstFacility::emplace(buffer.address(),
                                         'A' + k % NUM_VALUES,
                                         bslma::Default::defaultAllocator());
                    bslma::DestructorGuard<TYPE> guard(buffer.address());

                    mX[k] = MoveUtil::move(buffer.object());
                }

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBEFORE: "); P_(BB); P(B);
                }

                mX.pop_back();

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAFTER : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());
                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, i, l, k,
                                 VALUES[k % NUM_VALUES] == X[k]);
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const size_t LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t CAP = DATA[l].d_length;
                ASSERT(LENGTH <= CAP);

                Obj mX(xoa);

                stretch(&mX, LENGTH);
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_back();
                }
                catch (...) {
                    exceptionCaught = true;
                }
                LOOP_ASSERT(LINE, !exceptionCaught);
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
#endif

    if (verbose) printf("\nTesting 'pop_front' on non-empty deques.\n");
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

                Obj mX(xoa);  const Obj& X = mX;

                stretch(&mX, INIT_LENGTH);
                mX.reserve(INIT_CAP);

                unsigned k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    bsls::ObjectBuffer<TYPE> buffer;
                    TstFacility::emplace(buffer.address(),
                                         'A' + k % NUM_VALUES,
                                         bslma::Default::defaultAllocator());
                    bslma::DestructorGuard<TYPE> guard(buffer.address());

                    mX[k] = MoveUtil::move(buffer.object());
                }

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBEFORE: "); P_(BB); P(B);
                }

                mX.pop_front();

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAFTER : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());

                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, i, l, k,
                                 VALUES[(k+1) % NUM_VALUES] == X[k]);
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const size_t LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t CAP = DATA[l].d_length;
                ASSERT(LENGTH <= CAP);

                Obj mX(xoa);

                stretch(&mX, LENGTH);
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_front();
                }
                catch (...) {
                    exceptionCaught = true;
                }
                LOOP_ASSERT(LINE, !exceptionCaught);
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
#endif

    if (verbose) printf("\nTesting 'erase(pos)' on non-empty deques.\n");
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

                    Obj mX(xoa);  const Obj& X = mX;

                    stretch(&mX, INIT_LENGTH);
                    mX.reserve(INIT_CAP);

                    unsigned m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        bsls::ObjectBuffer<TYPE> buffer;
                        TstFacility::emplace(
                                           buffer.address(),
                                           'A' + m % NUM_VALUES,
                                           bslma::Default::defaultAllocator());
                        bslma::DestructorGuard<TYPE> guard(buffer.address());

                        mX[m] = MoveUtil::move(buffer.object());
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const Int64 BB = oa.numBlocksTotal();
                    const Int64  B = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBEFORE: "); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + POS);

                    const Int64 AA = oa.numBlocksTotal();
                    const Int64  A = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAFTER : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 LENGTH == X.size());

                    for (m = 0; m < POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[(m + 1) % NUM_VALUES] == X[m]);
                    }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

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

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        const int AL = (int) oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        Obj mX(xoa);  const Obj& X = mX;

                        stretch(&mX, INIT_LENGTH);
                        mX.reserve(INIT_CAP);

                        unsigned m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            bsls::ObjectBuffer<TYPE> buffer;
                            TstFacility::emplace(
                                           buffer.address(),
                                           'A' + m % NUM_VALUES,
                                           bslma::Default::defaultAllocator());
                            bslma::DestructorGuard<TYPE>
                                                       guard(buffer.address());

                            mX[m] = MoveUtil::move(buffer.object());
                        }

                        oa.setAllocationLimit(AL);

                        mX.erase(X.begin() + POS);  // test erase here

                        for (m = 0; m < POS; ++m) {
                            LOOP5_ASSERT(
                                      INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                      VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(
                                      INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                      VALUES[(m + 1) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
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
                    const int    NUM_ELEMENTS = (int) (END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj mX(xoa);  const Obj& X = mX;

                    stretch(&mX, INIT_LENGTH);
                    mX.reserve(INIT_CAP);

                    unsigned m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        bsls::ObjectBuffer<TYPE> buffer;
                        TstFacility::emplace(
                                           buffer.address(),
                                           'A' + m % NUM_VALUES,
                                           bslma::Default::defaultAllocator());
                        bslma::DestructorGuard<TYPE> guard(buffer.address());

                        mX[m] = MoveUtil::move(buffer.object());
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const Int64 BB = oa.numBlocksTotal();
                    const Int64  B = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBEFORE: "); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);

                    const Int64 AA = oa.numBlocksTotal();
                    const Int64  A = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAFTER : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, LENGTH == X.size());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }
                }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

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
                    const int    NUM_ELEMENTS = (int) (END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        const int AL = (int) oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        Obj mX(xoa);  const Obj& X = mX;

                        stretch(&mX, INIT_LENGTH);
                        mX.reserve(INIT_CAP);

                        unsigned m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            bsls::ObjectBuffer<TYPE> buffer;
                            TstFacility::emplace(
                                           buffer.address(),
                                           'A' + m % NUM_VALUES,
                                           bslma::Default::defaultAllocator());
                            bslma::DestructorGuard<TYPE>
                                                       guard(buffer.address());

                            mX[m] = MoveUtil::move(buffer.object());
                        }

                        oa.setAllocationLimit(AL);

                        mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);
                                                             // test erase here

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::testCase18(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING INPUT-RANGE INSERTION
    //
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
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
    //      - Using 'CONTAINER::const_iterator'.
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
    //   reallocations are for the new elements plus one if the deque
    //   undergoes a reallocation (capacity changes).
    //
    // Testing:
    //   iterator insert(const_iterator pos, ITER first, ITER last);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } U_DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDE"                   }, // 5
        { L_,   "ABCDEAB"                 }, // 7
        { L_,   "ABCDEABC"                }, // 8
        { L_,   "ABCDEABCD"               }, // 9
        { L_,   "ABCDEABCDEABCDE"         }, // 15
        { L_,   "ABCDEABCDEABCDEA"        }, // 16
        { L_,   "ABCDEABCDEABCDEAB"       }  // 17
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
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
                    const char   *SPEC         = U_DATA[ti].d_spec_p;
                    const int     NUM_ELEMENTS = (int) strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH, xoa); const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const Int64 BB = oa.numBlocksTotal();
                        const Int64  B = oa.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBEFORE: "); P_(BB); P_(B); P(mX);
                        }

                        iterator result =
                                mX.insert(X.begin() + POS, U.begin(), U.end());

                        const Int64 AA = oa.numBlocksTotal();
                        const Int64  A = oa.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAFTER : "); P_(AA); P_(A); P(mX);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     X.begin() + POS == result);

                        size_t m;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         U[m] == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }
                    }
                }
                ASSERT(0 == oa.numMismatches());
                ASSERT(0 == oa.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (4 < INIT_LENGTH && NUM_DATA-1 != i) {
                continue;
            }

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
                    const char   *SPEC         = U_DATA[ti].d_spec_p;
                    const int     NUM_ELEMENTS = (int) strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    if (4 < NUM_ELEMENTS && NUM_U_DATA-1 != ti) {
                        continue;
                    }

                    CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            const int AL = (int) oa.allocationLimit();
                            oa.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            oa.setAllocationLimit(AL);

                            if (veryVerbose) {
                                printf("\t\t\tBefore "); P(mX);
                            }

                            iterator result =
                                mX.insert(X.begin() + POS, U.begin(), U.end());
                                                         // test insertion here

                            if (veryVerbose) {
                                printf("\t\t\tAfter "); P(mX);
                            }

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         X.begin() + POS == result);

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             U[k - POS] == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == oa.numMismatches());
                ASSERT(0 == oa.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING VALUE INSERTION
    //
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
    //   2) That the 'insert' return (if any) value is a valid iterator, even
    //      when the deque underwent a reallocation.
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
    //      of the deque does not suffer from aliasing problems.
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
    //   deque reallocation (if capacity changes; all elements are moved), and
    //   for insertion the only reallocations should be for the new elements
    //   plus one if the deque undergoes a reallocation (capacity changes).
    //
    //   For concern 8, we insert an element of some deque where all the
    //   values are distinct into the same deque, taking care of the cases
    //   where the reference is before or after the position of insertion, and
    //   that the deque undergoes a reallocation or not (i.e., capacity
    //   changes or not).  We verify that the value is as expected, i.e.,
    //   identical to t it would be if the value had not been aliased.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   iterator insert(const_iterator pos, size_type n, const T& value);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'insert'.\n");

    if (verbose) printf("\tUsing a single 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];
            const size_t LENGTH      = INIT_LENGTH + 1;

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

                    Obj mX(INIT_LENGTH, xoa);  const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] = VALUES[k % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const Int64 BB = oa.numBlocksTotal();
                    const Int64  B = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBEFORE: "); P_(BB); P(B);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    iterator result = mX.insert(X.begin() + POS, VALUE);

                    const Int64 AA = oa.numBlocksTotal();
                    const Int64  A = oa.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAFTER : "); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                    LOOP3_ASSERT(INIT_LINE, i, j, X.begin() + POS == result);

                    for (k = 0; k < POS; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    for (++k; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

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

                        Obj mX(INIT_LENGTH, xoa); const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const Int64 BB = oa.numBlocksTotal();
                        const Int64  B = oa.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBEFORE: "); P_(BB); P(B);
                        }

                        iterator result =
                               mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);

                        const Int64 AA = oa.numBlocksTotal();
                        const Int64  A = oa.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAFTER : "); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     X.begin() + POS == result);

                        size_t m = 0;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (; k < POS + NUM_ELEMENTS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUE == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }
                    }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (4 < INIT_LENGTH && NUM_DATA-1 != i) {
                continue;
            }

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

                    if (4 < NUM_ELEMENTS && NUM_DATA-1 != ti) {
                        continue;
                    }

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            const int AL = (int) oa.allocationLimit();
                            oa.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            oa.setAllocationLimit(AL);

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P_(X.capacity()); P(POS);
                            }

                            iterator result =
                               mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; printf("After: "); P_(X);
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         X.begin() + POS == result);

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             VALUE == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");
    {
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

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                        mX.reserve(INIT_CAP);
                        const Obj& X = mX;

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X);  const Obj& Y = mY;  // control

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using value at "); P_(INDEX);
                            printf("\n");
                        }

                        mY.insert(Y.begin() + POS, X[INDEX]);  // control
                        mX.insert(X.begin() + POS, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        LOOP5_ASSERT(INIT_LINE, i, INIT_CAP, POS, INDEX,
                                     X == Y);
                    }
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING PUSH_FRONT & PUSH_BACK
    //
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
    //   2) That the resulting capacity is correctly set up.
    //   3) That insertion is exception neutral w.r.t. memory allocation.
    //   4) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   5) The initial internal state of the deque will not affect the
    //      correctness of the operation.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' using
    //   'push_back' and 'push_front'.  Perform the above test:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   void push_front(const T& value);
    //   void push_back(const T& value);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\nTesting 'push_back'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (size_t j = INIT_LENGTH; j <= LENGTH; ++j) {
                const size_t INIT_CAP = j;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                mX.reserve(INIT_CAP);
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\t'push_back' using "); P(VALUE);
                }

                TYPE mV(VALUE);

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBEFORE: "); P_(BB); P(B);
                }

                mX.push_back(mV);

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAFTER : "); P_(AA); P(A);
                    T_; T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, i, INIT_CAP, LENGTH == X.size());

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, LENGTH, k,
                                 DEFAULT_VALUE == X[k]);
                }
                LOOP3_ASSERT(INIT_LINE, INIT_CAP, INIT_LENGTH,
                             VALUE == X[INIT_LENGTH]);
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\nTesting 'push_front'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (size_t j = INIT_LENGTH; j <= LENGTH; ++j) {
                const size_t INIT_CAP = j;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                mX.reserve(INIT_CAP);
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\t'push_front' using "); P(VALUE);
                }

                TYPE mV(VALUE);

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBEFORE: "); P_(BB); P(B);
                }

                mX.push_front(mV);

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAFTER : "); P_(AA); P(A);
                    T_; T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, i, INIT_CAP, LENGTH == X.size());

                size_t k = 0;
                LOOP3_ASSERT(INIT_LINE, INIT_CAP, INIT_LENGTH,
                             VALUE == X[k]);
                for (k = 1; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, LENGTH, k,
                                 DEFAULT_VALUE == X[k]);
                }
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\tTesting 'push_back' with exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];

            for (size_t l = INIT_LENGTH; l < LENGTH; ++l) {
                const size_t INIT_CAP = l;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const int AL = (int) oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                    mX.reserve(INIT_CAP);
                    const Obj& X = mX;

                    oa.setAllocationLimit(AL);

                    if (veryVerbose) {
                        T_; T_; T_; printf("BEFORE: "); P_(X); P(X.capacity());
                    }

                    mX.push_back(VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; printf("AFTER : "); P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, l, LENGTH == X.size());

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, i, l, k,
                                     DEFAULT_VALUE == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, i, l, VALUE == X[k]);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());

    if (verbose) printf("\tTesting 'push_front' with exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];

            for (size_t l = INIT_LENGTH; l <= LENGTH; ++l) {
                const size_t INIT_CAP = l;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const int AL = (int) oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, xoa);
                    mX.reserve(INIT_CAP);
                    const Obj& X = mX;

                    oa.setAllocationLimit(AL);

                    if (veryVerbose) {
                        T_; T_; T_; printf("BEFORE: "); P_(X); P(X.capacity());
                    }

                    mX.push_front(VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; printf("AFTER : "); P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, l, LENGTH == X.size());

                    size_t k = 0;
                    LOOP3_ASSERT(INIT_LINE, i, l, VALUE == X[k]);
                    for (k = 1; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, i, l, k,
                                     DEFAULT_VALUE == X[k]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 That 'begin' and 'end' return mutable iterators for a reference to a
    //:   modifiable deque, and non-mutable iterators otherwise.
    //:
    //: 2 That the range '[begin(), end())' equals the value of the deque.
    //:
    //: 3 Same concerns with 'rbegin' and 'rend'.
    //:
    //: 4 That 'iterator' is a pointer to 'TYPE'.
    //:
    //: 5 That 'const_iterator' is a pointer to 'const TYPE'.
    //:
    //: 6 That 'reverse_iterator' and 'const_reverse_iterator' are implemented
    //:   by the (fully-tested) 'bslstl::ReverseIterator' over a pointer to
    //:   'TYPE' or 'const TYPE'.
    //
    // Plan:
    //   For 1--3, for each value given by variety of specifications of
    //   different lengths, create a test deque with this value, and access
    //   each element in sequence and in reverse sequence, both as a modifiable
    //   reference (setting it to a default value, then back to its original
    //   value, and as a non-modifiable reference.
    //
    //   For 4--6, use 'bsl::is_same' to assert the identity of iterator
    //   types.  Note that these concerns let us get away with other concerns
    //   such as testing that 'iter[i]' and 'iter + i' advance 'iter' by the
    //   correct number 'i' of positions, and other concern about traits,
    //   because 'bslstl::IteratorTraits' and 'bslstl::ReverseIterator' have
    //   already been fully tested in the 'bslstl_iterator' component.
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin() const;
    //   const_iterator cbegin() const;
    //   const_iterator end() const;
    //   const_iterator cend() const;
    //   const_reverse_iterator rbegin() const;
    //   const_reverse_iterator crbegin() const;
    //   const_reverse_iterator rend() const;
    //   const_reverse_iterator crend() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE DEFAULT_VALUE = TYPE();
    const int  BLOCK_LENGTH  =
                            bsl::Deque_BlockLengthCalcUtil<TYPE>::BLOCK_LENGTH;

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        ASSERT(1 == (bsl::is_same<iterator,
                        bslstl::RandomAccessIterator<TYPE,
                            bslalg::DequeIterator<TYPE, BLOCK_LENGTH>
                                                                  > >::value));
        ASSERT(1 == (bsl::is_same<const_iterator,
                     bslstl::RandomAccessIterator<const TYPE,
                        bslalg::DequeIterator<TYPE, BLOCK_LENGTH>
                                                                  > >::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            for (iterator iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                LOOP_ASSERT(LINE, Y[i] == *iter);
                *iter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *iter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, LENGTH == i);

            LOOP_ASSERT(LINE, Y == X);

            i = 0;
            for (const_iterator iter = X.begin(); iter != X.end();
                                                                 ++iter, ++i) {
                LOOP2_ASSERT(LINE, i, Y[i] == *iter);
            }
            LOOP_ASSERT(LINE, LENGTH == i);

            i = 0;
            for (const_iterator iter = X.cbegin(); iter != X.cend();
                                                                 ++iter, ++i) {
                LOOP2_ASSERT(LINE, i, Y[i] == *iter);
            }
            LOOP_ASSERT(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
        ASSERT(1 == (bsl::is_same<reverse_iterator,
                                   bsl::reverse_iterator<iterator> >::value));
        ASSERT(1 == (bsl::is_same<const_reverse_iterator,
                              bsl::reverse_iterator<const_iterator> >::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const int     LENGTH = (int) strlen(SPEC);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (reverse_iterator riter = mX.rbegin(); riter != mX.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
                *riter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *riter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, -1 == i);

            LOOP_ASSERT(LINE, Y == X);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.rbegin(); riter != X.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
            }
            LOOP_ASSERT(LINE, -1 == i);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.crbegin();
                                            riter != X.crend(); ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
            }
            LOOP_ASSERT(LINE, -1 == i);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase14()
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
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test deque with this value, and access each element
    //   (front, back, at each position) both as a modifiable reference
    //   (setting it to a default value, then back to its original value, and
    //   as a non-modifiable reference.  Verify that 'at' throws
    //   'std::out_of_range' when accessing the past-the-end element.
    //
    // Testing:
    //   reference operator[](size_type position);
    //   reference at(size_type position);
    //   reference front();
    //   reference back();
    //   const_reference front() const;
    //   const_reference back() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            if (LENGTH) {
                LOOP_ASSERT(LINE, TYPE(SPEC[0]) == X.front());
                mX.front() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.front());
                mX[0] = Y[0];

                LOOP_ASSERT(LINE, X[LENGTH - 1] == X.back());
                mX.back() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.back());
                mX[LENGTH - 1] = Y[LENGTH - 1];
            }

            LOOP_ASSERT(LINE, Y == X);

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X[j]);
                mX[j] = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X[j]);
                mX.at(j) = Y[j];
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X.at(j));
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            bool outOfRangeCaught = false;
            try {
                mX.at(LENGTH) = DEFAULT_VALUE;
            }
            catch (const std::out_of_range& ex) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, Y == X);
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
}

                     // ==============================
                     // struct TypeAllocatesForDefault
                     // ==============================

template <class TYPE, bool = bslma::UsesBslmaAllocator<TYPE>::value>
struct TypeAllocatesForDefault {
    static int test()
    {
        return 0;
    }
};

template <class TYPE>
struct TypeAllocatesForDefault<TYPE, true> {
    static int test()
    {
        bslma::TestAllocator queryAllocator("query allocate on default");
        const TYPE defaultObject(&queryAllocator);
        return static_cast<int>(queryAllocator.numAllocations());
    }
};

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING CAPACITY
    //   Testing methods related to 'capacity' and 'size', such as 'reserve',
    //   'resize', 'max_size', and 'empty'.  Note that the standard does not
    //   provide the strong exception safety guarantee (rollback when an
    //   exception is thrown) for 'resize', nor does the BDE 'reserve'
    //   extension.
    //
    // Concerns:
    //: 1 That 'v.reserve(n)' reserves sufficient capacity in 'v' to hold 'n'
    //:   elements without reallocation, but does not change the value of 'v'.
    //:   In addition, if 'v.reserve(n)' allocates, it must allocate for a
    //:   capacity of exactly 'n' elements.
    //:
    //: 2 That 'v.resize(n, value)' changes the size of 'v' to 'n', appending
    //:   elements having value 'value' if 'n' is larger than the current size.
    //:
    //: 3 That existing elements are moved without copy-construction if the
    //:   bitwise-moveable trait is present.
    //:
    //: 4 That 'reserve' and 'resize' are exception-neutral.  Note that there
    //:   is no strong exception-safety guarantee on these methods.
    //:
    //: 5 That the accessors such as 'capacity' and 'empty' return the correct
    //:   value.
    //
    // Plan:
    //: 1 For deque 'v' having various initial capacities, call 'v.reserve(n)'
    //:   for various values of 'n'.  Verify that sufficient capacity is
    //:   allocated by filling 'v' with 'n' elements.  Perform each test in the
    //:   standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void reserve(size_type n);
    //   void resize(size_type n);
    //   void resize(size_type n, const T& value);
    //   size_type max_size() const;
    //   size_type capacity() const;
    //   bool empty() const;
    // ------------------------------------------------------------------------

    typedef typename Obj::size_type size_type;

    bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
    ALLOC                 xoa(&oa);
    ASSERT(0 == oa.numBytesInUse());

    static const size_t EXTEND[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

    const int PAGE_LENGTH = PageLength<TYPE>::k_VALUE;
    BSLMF_ASSERT(PAGE_LENGTH >= 7);
    const int BLOCKS_LENGTH = 5;     // Length of blocks array for a
                                     // default-constructed empty deque.

    static const size_t DATA[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int FS = (PAGE_LENGTH - 1) / 2;    // front space
    const int BS = PAGE_LENGTH - 1 - FS;     // back space

    struct {
        int         d_line;
        const char *d_spec_p;
        int         d_size;
    } SPECS[] = {
        // Must not be empty

        { L_, "A",            1 },
        { L_, "<B|<a|",       PAGE_LENGTH - 1 },
        { L_, "<B|",          BS },
        { L_, "<a|",          FS },
        { L_, "<B...|<a...|", PAGE_LENGTH - 6 - 1 },
        { L_, "<a...|",       FS - 3 },
        { L_, "<B...|",       BS - 3 }, };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    const TYPE         *values     = 0;
    const TYPE *const&  TEST_VALUES     = values;
    const int           NUM_TEST_VALUES = getValues(&values);
    (void) NUM_TEST_VALUES;

    Int64 numAllocsPerItem;
    if (verbose) printf("\tTesting 'max_size', init numAllocsPerItem.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj mX(xoa);  const Obj& X = mX;
        ASSERT(~(size_t)0 / sizeof(TYPE) >= X.max_size());

        Int64 initBlocks = oa.numBlocksInUse();
        mX.push_back(TEST_VALUES[0]);
        numAllocsPerItem = oa.numBlocksInUse() - initBlocks;
    }

    if (veryVerbose) { P_(PAGE_LENGTH); P_(FS); P_(BS); P(numAllocsPerItem); }

    if (verbose) printf("\tTesting 'capacity, pushing at both ends'.\n");
    {
        Obj mX(xoa);  const Obj& X = mX;
        const size_type INITIAL_CAPACITY = X.capacity();

        // An empty deque allocates the first page of memory for elements, so
        // capacity should be non-zero.

        ASSERTV(INITIAL_CAPACITY, 0 < INITIAL_CAPACITY);

        // insert up to initial capacity, one item at a time.
        //  alternately push_back/push_front, watching capacity
        //  (want to set the situation where one push decrements by one,
        //  pushing at the other end has no effect, but a second push at the
        //  other end decrements by one.  Verify that no memory is allocated
        //  until capacity is 0, then one allocation when pushing at each end.
        //  Also make sure that capacity remains zero after first allocating
        //  push, and a big jump after pushing at the other end.

        size_type lastCapacity = INITIAL_CAPACITY;

        // Capacity is size plus minimum number of pushes at either end before
        // the container must reallocate.  As we don't know if the front or the
        // back of the deque has a lower threshold, we will push at both ends,
        // expecting size to grow by 2, but capacity to grow by only one.  For
        // the insertion where size equals capacity, we know we have exhausted
        // that end of the deque and should push only at the other end, until
        // it too is full.  We can spot this by first popping the last inserted
        // element so that free capacity at the full end is now 1, and we can
        // spot 1 dropping to 0 when the opposite end is given its last push.
        // Finally, push that initial element one last time to truly full the
        // deque, and confirm that, for an element type that does not allocate,
        // no allocations have yet occurred.  Then push one element onto either
        // end and observe that allocation occurs, and capacity grows.  Note
        // that we have no guarantee that the new capacity will be the old
        // capacity plus one, as re-allocation may leave the container with a
        // better balanced spread of elements starting and ending in the middle
        // of blocks in the new arrangement.

        const Int64 initBlocks = oa.numBlocksInUse();
        Int64 newBlocks;
        size_type iterations  = 0;
        ASSERT(X.empty());
        ASSERT(bsl::empty(X));
        ASSERT(2 == initBlocks);   // first block + array of block pointers

        // Note that 'findRoomierEnd' may both deallocate and allocate.

        mX.push_back(TEST_VALUES[2]);
        typename Obj::const_iterator it = mX.begin();

        int room;
        while ((room = findRoomierEnd(&mX)),
                                        0 != room || X.capacity() > X.size()) {
            if (0 < room) {
                // More room in front.

                mX.push_front(TEST_VALUES[0]);
            }
            else {
                // Symmetrical or more room in back.

                mX.push_back(TEST_VALUES[1]);
            }
            newBlocks = oa.numBlocksInUse();
            ASSERTV(newBlocks <= numAllocsPerItem * (int) X.size() +
                                             + initBlocks + BLOCKS_LENGTH - 1);

            const size_type NEW_CAPACITY = X.capacity();
            ASSERTV(NEW_CAPACITY,   lastCapacity,
                    NEW_CAPACITY == lastCapacity ||
                    NEW_CAPACITY == lastCapacity + 1);

            lastCapacity = NEW_CAPACITY;
            ++iterations;
            ASSERT(X.size() == iterations + 1);
        }
        ASSERTV(X.capacity(), X.size(), X.capacity() == X.size());
        ASSERTV(TEST_VALUES[2] == *it);    // iterator not invalidated

        // After loop, the deque should be full to touching both ends, which
        // means there will be one space open at the back end.

        ASSERTV(PAGE_LENGTH, BLOCKS_LENGTH, iterations,
                                  BLOCKS_LENGTH * PAGE_LENGTH - 1 == X.size());
        ASSERT(0 == room);

        // There should be exactly 'BLOCKS_LENGTH' blocks in the deque, plus
        // the array of pointers to blocks.

        ASSERT(numAllocsPerItem * (Int64) X.size() + BLOCKS_LENGTH + 1
               == oa.numBlocksInUse());

        // Now verify that the next push forces an allocation.  Need to
        // recreate the situation twice, once for 'push_back' and a second time
        // for 'push_front'.

        // Investigate all 4 push/pop sequences:
        //   push_back / pop_back
        //   push_back / pop_front
        //   push_front/ pop_front
        //   push_front/ pop_back
    }

    if (verbose) printf("\tTesting 'capacity, fill back before front'.\n");
    {
        Obj mX(xoa);  const Obj& X = mX;
        const size_type INITIAL_CAPACITY = X.capacity();

        // An empty deque allocates the first page of memory for elements, so
        // capacity should be non-zero.

        ASSERTV(INITIAL_CAPACITY, 0 < INITIAL_CAPACITY);

        // insert up to initial capacity, one item at a time.
        //  alternately push_back/push_front, watching capacity
        //  (want to set the situation where one push decrements by one,
        //  pushing at the other end has no effect, but a second push at the
        //  other end decrements by one.  Verify that no memory is allocated
        //  until capacity is 0, then one allocation when pushing at each end.
        //  Also make sure that capacity remains zero after first allocating
        //  push, and a big jump after pushing at the other end.

        size_type lastCapacity = INITIAL_CAPACITY;

        // Capacity is size plus minimum number of pushes at either end before
        // the container must reallocate.  As we don't know if the front or the
        // back of the deque has a lower threshold, we will push at both ends,
        // expecting size to grow by 2, but capacity to grow by only one.  For
        // the insertion where size equals capacity, we know we have exhausted
        // that end of the deque and should push only at the other end, until
        // it too is full.  We can spot this by first popping the last inserted
        // element so that free capacity at the full end is now 1, and we can
        // spot 1 dropping to 0 when the opposite end is given its last push.
        // Finally, push that initial element one last time to truly full the
        // deque, and confirm that, for an element type that does not allocate,
        // no allocations have yet occurred.  Then push one element onto either
        // end and observe that allocation occurs, and capacity grows.  Note
        // that we have no guarantee that the new capacity will be the old
        // capacity plus one, as re-allocation may leave the container with a
        // better balanced spread of elements starting and ending in the middle
        // of blocks in the new arrangement.

        const Int64 initBlocks = oa.numBlocksInUse();
        Int64 newBlocks;
        bool      backIsFull  = 0 == INITIAL_CAPACITY;
        bool      frontIsFull = 0 == INITIAL_CAPACITY;
        size_type iterations  = 0;

        ASSERT(2 == initBlocks);   // first block + array of block pointers

        mX.push_back(TEST_VALUES[2]);
        typename Obj::const_iterator it = mX.begin();

        while (!backIsFull) {
            mX.push_back(TEST_VALUES[0]);
            const size_type BACK_CAPACITY  = X.capacity();

            // 'push_back' does not affect 'capacity' if less room at the
            // back, otherwise it should grow by exactly one.
            ASSERTV(BACK_CAPACITY,   lastCapacity,
                    BACK_CAPACITY == lastCapacity ||
                    BACK_CAPACITY == lastCapacity + 1);

            newBlocks = oa.numBlocksInUse();
            ASSERT(newBlocks <= numAllocsPerItem * (Int64) X.size() +
                                         initBlocks + (BLOCKS_LENGTH - 1) / 2);

            backIsFull = X.capacity() == X.size();
            if (backIsFull) {
                // Make sure we can spot the front filling up.
                mX.pop_back();
            }

            lastCapacity = X.capacity();

            if (++iterations > 2 * INITIAL_CAPACITY) {
                // An early abort to avoid an infinite loop if the test is
                // failing.

                break;
            }
        }

        iterations = 0;   // Reset safety-valve counter

        while (!frontIsFull) {
            mX.push_front(TEST_VALUES[1]);
            const size_type FRONT_CAPACITY = X.capacity();
            ASSERTV(FRONT_CAPACITY,   lastCapacity,
                    FRONT_CAPACITY == lastCapacity ||
                    FRONT_CAPACITY == lastCapacity + 1);

            frontIsFull  = X.capacity() == X.size();
            if (frontIsFull) {
                // Restore the previously popped element to fill deque.

                mX.push_back(TEST_VALUES[1]);
            }

            newBlocks = oa.numBlocksInUse();
            ASSERT(newBlocks <= numAllocsPerItem * (Int64) X.size() +
                                               initBlocks + BLOCKS_LENGTH - 1);

            // Note that there is potential when inserting into an empty
            // container that the very first insertion may reduce capacity in
            // both directions, so a subsequent insert reduces capacity again.

            lastCapacity = X.capacity();

            if (++iterations > 2 * INITIAL_CAPACITY) {
                // An early abort to avoid an infinite loop if the test is
                // failing.

                break;
            }
        }

        // After loop, capacity should have been exhausted
        ASSERTV(X.capacity(), X.size(), X.capacity() == X.size());
        ASSERT(TEST_VALUES[2] == *it);        // iterator still valid
        ASSERT(0 == findRoomierEnd(&mX));

        ASSERTV(BLOCKS_LENGTH * PAGE_LENGTH - 1 == X.size());

        // There should be exactly 'BLOCKS_LENGTH' blocks in the deque, plus
        // the array of pointers to blocks.

        ASSERT(numAllocsPerItem * (Int64) X.size() +
                                     BLOCKS_LENGTH + 1 == oa.numBlocksInUse());
    }

    if (verbose) printf("\tTesting 'capacity, fill front before back'.\n");
    {
        Obj mX(xoa);  const Obj& X = mX;
        const size_type INITIAL_CAPACITY = X.capacity();

        // An empty deque allocates the first page of memory for elements, so
        // capacity should be non-zero.

        ASSERTV(INITIAL_CAPACITY, 0 < INITIAL_CAPACITY);

        // insert up to initial capacity, one item at a time.
        //  alternately push_back/push_front, watching capacity
        //  (want to set the situation where one push decrements by one,
        //  pushing at the other end has no effect, but a second push at the
        //  other end decrements by one.  Verify that no memory is allocated
        //  until capacity is 0, then one allocation when pushing at each end.
        //  Also make sure that capacity remains zero after first allocating
        //  push, and a big jump after pushing at the other end.

        size_type lastCapacity = INITIAL_CAPACITY;

        // Capacity is size plus minimum number of pushes at either end before
        // the container must reallocate.  As we don't know if the front or the
        // back of the deque has a lower threshold, we will push at both ends,
        // expecting size to grow by 2, but capacity to grow by only one.  For
        // the insertion where size equals capacity, we know we have exhausted
        // that end of the deque and should push only at the other end, until
        // it too is full.  We can spot this by first popping the last inserted
        // element so that free capacity at the full end is now 1, and we can
        // spot 1 dropping to 0 when the opposite end is given its last push.
        // Finally, push that initial element one last time to truly full the
        // deque, and confirm that, for an element type that does not allocate,
        // no allocations have yet occurred.  Then push one element onto either
        // end and observe that allocation occurs, and capacity grows.  Note
        // that we have no guarantee that the new capacity will be the old
        // capacity plus one, as re-allocation may leave the container with a
        // better balanced spread of elements starting and ending in the middle
        // of blocks in the new arrangement.

        const Int64 initBlocks = oa.numBlocksInUse();
        Int64 newBlocks;
        bool      backIsFull  = 0 == INITIAL_CAPACITY;
        bool      frontIsFull = 0 == INITIAL_CAPACITY;
        size_type iterations  = 0;

        mX.push_front(TEST_VALUES[2]);
        typename Obj::const_iterator it = mX.begin();

        while (!frontIsFull) {
            mX.push_front(TEST_VALUES[0]);
            const size_type FRONT_CAPACITY = X.capacity();

            // 'push_front' does not affect 'capacity' if less room at the
            // front, otherwise it should grow by exactly one.

            ASSERTV(FRONT_CAPACITY,   lastCapacity,
                    FRONT_CAPACITY == lastCapacity ||
                    FRONT_CAPACITY == lastCapacity + 1);

            newBlocks = oa.numBlocksInUse();
            ASSERT(newBlocks <= numAllocsPerItem * (Int64) X.size() +
                                         initBlocks + (BLOCKS_LENGTH - 1) / 2);

            frontIsFull  = X.capacity() == X.size();
            if (frontIsFull) {
                // Make sure we can spot the back filling up in the next step.

                mX.pop_front();
            }

            // Note that there is potential when inserting into an empty
            // container that the very first insertion may reduce capacity in
            // both directions, so a subsequent insert reduces capacity again.

            lastCapacity = X.capacity();

            if (++iterations > 2 * INITIAL_CAPACITY) {
                // An early abort to avoid an infinite loop if the test is
                // failing.

                break;
            }
        }

        iterations = 0;   // Reset safety-valve counter

        while (!backIsFull) {
            mX.push_back(TEST_VALUES[1]);
            const size_type BACK_CAPACITY  = X.capacity();

            // 'push_back' does not affect 'capacity' if less room at the
            // back, otherwise it should grow by exactly one.

            ASSERTV(BACK_CAPACITY,   lastCapacity,
                    BACK_CAPACITY == lastCapacity ||
                    BACK_CAPACITY == lastCapacity + 1);

            backIsFull = X.capacity() == X.size();
            if (backIsFull) {
                // Restore the previously popped element to fill deque.

                mX.push_front(TEST_VALUES[0]);
            }

            newBlocks = oa.numBlocksInUse();
            ASSERT(newBlocks <= numAllocsPerItem * (Int64) X.size() +
                                               initBlocks + BLOCKS_LENGTH - 1);

            lastCapacity = X.capacity();

            if (++iterations > 2 * INITIAL_CAPACITY) {
                // An early abort to avoid an infinite loop if the test is
                // failing.

                break;
            }
        }

        // After loop, capacity should have been exhausted

        ASSERTV(X.capacity(), X.size(), X.capacity() == X.size());
        ASSERT(TEST_VALUES[2] == *it);        // iterator still valid

        ASSERT(0 == findRoomierEnd(&mX));

        ASSERTV(BLOCKS_LENGTH * PAGE_LENGTH - 1 == X.size());

        // There should be exactly 'BLOCKS_LENGTH' blocks in the deque, plus
        // the array of pointers to blocks.

        ASSERT(numAllocsPerItem * (int) X.size() +
                                     BLOCKS_LENGTH + 1 == oa.numBlocksInUse());
    }

    if (verbose) printf("\tTesting 'reserve', 'capacity' and 'empty'.\n");
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const int     LINE = SPECS[ti].d_line;
        const char   *SPEC = SPECS[ti].d_spec_p;
        const size_t  SZ   = SPECS[ti].d_size;

        for (int tj = 0; tj < NUM_EXTEND; ++tj) {
            const size_t NE = EXTEND[tj];

            Obj mX(xoa);  const Obj& X = mX;
            ASSERT(X.empty());
            ASSERT(bsl::empty(X));
            ASSERTV(LINE, 0 > ggg(&mX, SPEC));
            ASSERTV(LINE, X.size(), SZ, X.size() == SZ);
            ASSERTV(SZ <= PAGE_LENGTH);
            ASSERT((0 == SZ) == X.empty());
            ASSERT((0 == SZ) == bsl::empty(X));

            typename Obj::iterator it = mX.begin();
            TYPE v = *it;
            TYPE *pv = BSLS_UTIL_ADDRESSOF(*it);

            const size_t preCap = X.capacity();

            Int64 numBlocksBefore = oa.numBlocksInUse();
            int throwCount = -1;
            Obj mY(X);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ExceptionProctor<Obj, ALLOC> proctor(&mX, X, L_);
                ASSERT(X.capacity() == preCap);
                ++throwCount;

                mX.reserve(NE);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            size_t postCap = X.capacity();
            ASSERT((NE <= preCap - SZ) == (postCap == preCap));
            ASSERT(X == mY);    // capacity may have changed, but not salient
            LOOP_ASSERT(ti, SZ == X.size());
            ASSERT((0 == SZ) == X.empty());
            ASSERT((0 == SZ) == bsl::empty(X));

            Int64 numBlocksAfter = oa.numBlocksInUse();
            if (NE <= preCap - SZ) {
                ASSERT(numBlocksBefore == numBlocksAfter);
                ASSERT(postCap == preCap);
                ASSERT(0 == throwCount);
                ASSERT(v == *it);                  // iterators not invalidated
            }
            else {
                ASSERT(numBlocksBefore < numBlocksAfter);
                ASSERT(postCap > preCap);
                ASSERT(0 < throwCount);
            }
            ASSERT(postCap - SZ >= NE);
            size_t space = postCap - SZ;

            it = mX.begin();
            v = *it;

            // Make symmetrical.

            int room;
            while (0 != (room = findRoomierEnd(&mX))) {
                if (0 < room) {
                    mX.push_front(TEST_VALUES[1]);
                }
                else if (0 > room) {
                    mX.push_back( TEST_VALUES[0]);
                }
            }
            ASSERT(0 == findRoomierEnd(&mX));
            space = X.capacity() - X.size();
            mY = X;

            // Adding 'postCap' more items to each of both ends will not
            // invalidate iterators, and will only alloc the amount necessary
            // for the items themselves.

            numBlocksBefore = oa.numBlocksInUse();
            for (size_t tk = 0; tk < space; ++tk) {
                mX.push_back( TEST_VALUES[2]);
                mX.push_front(TEST_VALUES[3]);
            }
            numBlocksAfter = oa.numBlocksInUse();
            const size_t distToV = it - mX.begin();
            ASSERT(v == *it);                      // iterators not invalidated
            ASSERT(!space == (X == mY));
            ASSERTV(LINE, numBlocksAfter,
                                      numBlocksBefore, space, numAllocsPerItem,
                    numBlocksAfter == numBlocksBefore + BLOCKS_LENGTH - 1 +
                                         2 * (Int64) space * numAllocsPerItem);
            ASSERTV(X.capacity(), 0 == X.capacity() - X.size());
            ASSERT(0 == findRoomierEnd(&mX));
            ASSERTV(X.capacity() == X.size());
            ASSERT(X.size() % PAGE_LENGTH == PAGE_LENGTH - 1);

            // Verify the layout of the deque.  Items in the same block will
            // be adjacent.  Since we're using the test allocator, we know that
            // allocated segments begin and end with padding, so items in
            // different blocks can't be adjacent.

            for (size_t szB = X.size(), ii = 0; ii < szB; ++ii) {
                const size_t stop =  (szB < ii + PAGE_LENGTH
                                    ? szB : ii + PAGE_LENGTH) - 1;
                size_t jj = ii;
                for (; jj < stop; ++jj) {
                    // adjacent; same block
                    ASSERT(BSLS_UTIL_ADDRESSOF(X[jj]) + 1 ==
                           BSLS_UTIL_ADDRESSOF(X[jj + 1]));
                }
                ii = jj;
                ASSERT(ii + 1 == szB ||
                           BSLS_UTIL_ADDRESSOF(X[ii]) + 1 !=
                           BSLS_UTIL_ADDRESSOF(X[ii + 1]));
                                              // not adjacent; different blocks
            }

            // Pushing one more item to each end will cause a grow of one page
            // on either end, will invalidate iterators.  With the current imp
            // of iterators, comparing 'it' with 'itB' below with '!=' won't
            // work, because '==' and '!=' just look at the 'd_value_p' field,
            // while it's the 'd_block_p' field that will change, so we have to
            // use 'memcmp' to look at the guts of the whole iterator to see
            // that the old iterator is no longer valid.

            ASSERT(X.size() >= 2);
            ASSERT((1 < PAGE_LENGTH) ==     // same block
                 (BSLS_UTIL_ADDRESSOF(X[1]) == BSLS_UTIL_ADDRESSOF(X[0]) + 1));
            size_t preSuperCap = X.size();
            numBlocksBefore = oa.numBlocksInUse();
            mX.push_back( TEST_VALUES[0]);
            mX.push_front(TEST_VALUES[1]);
            numBlocksAfter = oa.numBlocksInUse();
            typename Obj::iterator itB = mX.begin() + distToV + 1;
            ASSERT(v ==   *itB);
            ASSERT(pv == BSLS_UTIL_ADDRESSOF(*itB));       // hasn't moved
            ASSERT(0 != memcmp(&it, &itB, sizeof(it)));    // iterators changed
            ASSERT(BSLS_UTIL_ADDRESSOF(X[1]) !=
                   BSLS_UTIL_ADDRESSOF(X[0]) + 1);         // different blocks
            ASSERT(numBlocksAfter ==
                                   numBlocksBefore + 2 * numAllocsPerItem + 2);
            size_t postSuperCap = X.size() + 2 * (X.capacity() - X.size());
            ASSERT(postSuperCap >= preSuperCap + 2 * PAGE_LENGTH);

            mX.clear();
            ASSERT(X.empty());
            ASSERT(bsl::empty(X));
        }
    }

    if (verbose) printf("\tTesting 'resize'.\n");
    const typename Obj::value_type DEFAULT_VALUE = typename Obj::value_type();
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
              const int AL = (int) oa.allocationLimit();
              oa.setAllocationLimit(-1);

              Obj mX(xoa);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, bsl::empty(X));
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              Int64 numBlocksBefore = oa.numBlocksInUse();

              oa.setAllocationLimit(AL);

              mX.resize(NE);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              Int64 numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, DELTA, numBlocksAfter, numBlocksBefore,
                      NE > PAGE_LENGTH / 2 - 1 ||
                                numBlocksAfter == (int) NE * numAllocsPerItem +
                                                              numBlocksBefore);

              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, DEFAULT_VALUE == X[j]);
              }

              typename Obj::iterator it;
              if (NE) it = mX.begin();

              numBlocksBefore = oa.numBlocksInUse();

              // This will add 1 block, but it will not reallocate 'd_blocks'.

              mX.resize(NE + PAGE_LENGTH);  // test here

              numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, PAGE_LENGTH, numBlocksAfter, numBlocksAfter,
                      numBlocksBefore, numBlocksAfter ==
                         1 + PAGE_LENGTH * numAllocsPerItem + numBlocksBefore);

              // Iterators not invalidated.

              typename Obj::iterator itB;
              if (NE) itB = mX.begin();
              ASSERT(0 == memcmp(&it, &itB, sizeof(it)));

              for (size_t j = 0; j < NE + PAGE_LENGTH; ++j) {
                  LOOP2_ASSERT(ti, j, DEFAULT_VALUE == X[j]);
              }

              it = mX.begin();

              numBlocksBefore = oa.numBlocksInUse();

              // This will add 1 block, but it will not reallocate 'd_blocks'.

              mX.resize(NE + 4 * PAGE_LENGTH);  // test here

              numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, PAGE_LENGTH, numBlocksAfter, numBlocksAfter,
                  numBlocksBefore, numBlocksAfter ==
                     3 + 3 * PAGE_LENGTH * numAllocsPerItem + numBlocksBefore);

              // Iterators invalidated.

              itB = mX.begin();
              ASSERT(0 != memcmp(&it, &itB, sizeof(it)));

              for (size_t j = 0; j < NE + 4 * PAGE_LENGTH; ++j) {
                  LOOP2_ASSERT(ti, j, DEFAULT_VALUE == X[j]);
              }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'resize' with value.\n");
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];
        const TYPE   TV = TEST_VALUES[1];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
              const int AL = (int) oa.allocationLimit();
              oa.setAllocationLimit(-1);

              Obj mX(xoa);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, bsl::empty(X));
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              Int64 numBlocksBefore = oa.numBlocksInUse();

              oa.setAllocationLimit(AL);

              mX.resize(NE, TV);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              Int64 numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, DELTA, numBlocksAfter, numBlocksBefore,
                      NE > PAGE_LENGTH / 2 - 1 ||
                      numBlocksAfter == (Int64) NE * numAllocsPerItem +
                                                              numBlocksBefore);

              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, TV == X[j]);
              }

              typename Obj::iterator it;
              if (NE) it = mX.begin();

              numBlocksBefore = oa.numBlocksInUse();

              // This will add 1 block, but it will not reallocate 'd_blocks'.

              mX.resize(NE + PAGE_LENGTH, TV);  // test here

              numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, PAGE_LENGTH, numBlocksAfter,
                      numBlocksBefore, numBlocksAfter ==
                         1 + PAGE_LENGTH * numAllocsPerItem + numBlocksBefore);

              // Iterators not invalidated.

              typename Obj::iterator itB;
              if (NE) itB = mX.begin();
              ASSERT(0 == memcmp(&it, &itB, sizeof(it)));

              for (size_t j = 0; j < NE + PAGE_LENGTH; ++j) {
                  LOOP2_ASSERT(ti, j, TV == X[j]);
              }

              it = mX.begin();

              numBlocksBefore = oa.numBlocksInUse();

              // This will add 1 block, but it will not reallocate 'd_blocks'.

              mX.resize(NE + 4 * PAGE_LENGTH, TV);  // test here

              numBlocksAfter = oa.numBlocksInUse();

              ASSERTV(NE, PAGE_LENGTH, numBlocksAfter, numBlocksAfter,
                 numBlocksBefore, numBlocksAfter ==
                     3 + 3 * PAGE_LENGTH * numAllocsPerItem + numBlocksBefore);

              // Iterators invalidated.

              itB = mX.begin();
              ASSERT(0 != memcmp(&it, &itB, sizeof(it)));

              for (size_t j = 0; j < NE + 4 * PAGE_LENGTH; ++j) {
                  LOOP2_ASSERT(ti, j, TV == X[j]);
              }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    ASSERT(0 == oa.numMismatches());
    ASSERT(0 == oa.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   The concerns are the same as for the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value must be freed properly.
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
    //   void assign(size_type n, const T& value);
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
    ALLOC                 xoa(&oa);

    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

    if (verbose) printf("\nTesting initial-length assignment.\n");
    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        6   },
            { L_,        7   },
            { L_,        8   },
            { L_,        9   },
            { L_,       11   },
            { L_,       12   },
            { L_,       14   },
            { L_,       15   },
            { L_,       16   },
            { L_,       17   }
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

                Obj mX(INIT_LENGTH, xoa);  const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, VALUE == X[j]);
                    }
                }
            }
            ASSERT(0 == oa.numMismatches());
            ASSERT(0 == oa.numBlocksInUse());
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

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        const int AL = (int) oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, xoa);  const Obj& X = mX;
                        ExceptionProctor<Obj, ALLOC> proctor(&mX, Obj(), L_);

                        oa.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        proctor.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == oa.numMismatches());
                    ASSERT(0 == oa.numBlocksInUse());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::testCase12Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING RANGE 'assign'
    //
    // Concerns:
    //   The concerns are the same as for the constructor with the same
    //   signature (case 12), except that the implementation is different, and
    //   in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - Using 'CONTAINER::const_iterator'.
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
    //   void assign(ITER first, ITER last);
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
    ALLOC                 xoa(&oa);

    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // container spec
    } U_DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDE"                   }, // 5
        { L_,   "ABCDEAB"                 }, // 7
        { L_,   "ABCDEABC"                }, // 8
        { L_,   "ABCDEABCD"               }, // 9
        { L_,   "ABCDEABCDEABCDE"         }, // 15
        { L_,   "ABCDEABCDEABCDEA"        }, // 16
        { L_,   "ABCDEABCDEABCDEAB"       }  // 17
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

            Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES], xoa);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec_p;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

                Obj mY;  const Obj& Y = gg(&mY, SPEC);
                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == oa.numMismatches());
        ASSERT(0 == oa.numBlocksInUse());
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
                const char   *SPEC   = U_DATA[ti].d_spec_p;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY;  const Obj& Y = gg(&mY, SPEC);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const int AL = (int) oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, xoa);  const Obj& X = mX;
                    ExceptionProctor<Obj, ALLOC> proctor(&mX, Obj(), L_);

                    oa.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    proctor.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(oa.numMismatches(),  0 == oa.numMismatches());
                LOOP_ASSERT(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver2<TYPE,ALLOC>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING INITIAL-LENGTH CONSTRUCTORS
    //   We have the following concerns:
    //    1) The initial value is correct.
    //    2) The initial capacity is correctly set up.
    //    3) The constructor is exception neutral w.r.t. memory allocation.
    //    4) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
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
    //
    // Testing:
    //   deque(size_type n, const A& a = A());
    //   deque(size_type n, const T& value, const A& a = A());
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    const TYPE           DEFAULT_VALUE = TYPE();

    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

    if (verbose) printf("\nTesting initial-length ctor "
                        "with (default) initial value.\n");
    {
        static const struct {
            int d_lineNum;  // source line number
            int d_length;   // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        6   },
            { L_,        7   },
            { L_,        8   },
            { L_,        9   },
            { L_,       11   },
            { L_,       12   },
            { L_,       14   },
            { L_,       15   },
            { L_,       16   },
            { L_,       17   },
            { L_,       31   },
            { L_,       32   },
            { L_,       33   },
            { L_,       63   },
            { L_,       64   },
            { L_,       65   }
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

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWithout passing in an allocator, "
                            "using non-default values.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
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

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) { printf("\t\t\tBEFORE: "); P_(BB); P(B); }

                Obj mX(LENGTH, DEFAULT_VALUE, xoa);
                const Obj& X = mX;

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\tAFTER : "); P_(AA); P(A);
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing in an allocator, "
                            "using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE, xoa);
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing an allocator and checking for "
                            "allocation exceptions using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBEFORE: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj mX(LENGTH, DEFAULT_VALUE, xoa);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}
            }
        }

        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const Int64 BB = oa.numBlocksTotal();
                const Int64  B = oa.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBEFORE: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj mX(LENGTH, VALUE, xoa);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = oa.numBlocksTotal();
                const Int64  A = oa.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

                LOOP2_ASSERT(LINE, ti, 0 == oa.numBlocksInUse());
            }
        }

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                (void) LINE;

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                ALLOC                xsa(&sa);
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (verbose) {
                    printf("\t\tCreating object of "); P(LENGTH);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ASSERT(0 == da.numBytesTotal());
                    ASSERT(0 == sa.numBytesInUse());

                    Obj x(LENGTH, DEFAULT_VALUE, xsa);

                    ASSERT(0 == da.numBytesInUse());
                    ASSERT(0 != sa.numBytesInUse());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == sa.numBytesInUse());
            }
        }

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "non-default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];
                (void) LINE;

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                ALLOC                xsa(&sa);
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ASSERT(0 == da.numBytesInUse());
                    ASSERT(0 == sa.numBytesInUse());

                    Obj x(LENGTH, VALUE, xsa);

                    ASSERT(0 == da.numBytesInUse());
                    ASSERT(0 != sa.numBytesInUse());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == sa.numBytesInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver2<TYPE,ALLOC>::testCase11Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING RANGE CONSTRUCTOR
    //   We have the following concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported and then moved if the
    //       initial 'FWD_ITER' is an input iterator.
    //    2) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    3) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    4) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   We will create objects of varying sizes and capacities containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - Using 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   deque(ITER first, ITER last, const A& a = A());
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    ALLOC                xoa(&oa);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;   // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "AB"              },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            Obj mY;  const Obj& Y = gg(&mY, SPEC);
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) { printf("\t\tCreating object "); P(SPEC); }

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;
            Obj mY;  const Obj& Y = gg(&mY, SPEC);

            const Int64 BB = oa.numBlocksTotal();
            const Int64  B = oa.numBlocksInUse();

            Obj        mX(U.begin(), U.end(), xoa);
            const Obj& X = mX;

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec_p;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;
            Obj mY;  const Obj& Y = gg(&mY, SPEC);

            const Int64 BB = oa.numBlocksTotal();
            const Int64  B = oa.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBEFORE: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mX(U.begin(), U.end(), xoa);

                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = oa.numBlocksTotal();
            const Int64  A = oa.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

            LOOP2_ASSERT(LINE, ti, 0 == oa.numBlocksInUse());
        }
    }
}

                        // ======================
                        // struct MetaTestDriver2
                        // ======================

template <class TYPE>
struct MetaTestDriver2 {
    // This 'struct' is to be called by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver' instantiated
    // with different types of allocator.

    typedef bsl::allocator<TYPE>            BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    static void testCase20();
        // Test member and free 'swap'.
};

template <class TYPE>
void MetaTestDriver2<TYPE>::testCase20()
{
    // Limit the # of tests if we're in C++11 to avoid running out of compiler
    // memory.

    // The low-order bit of the identifier specifies whether the third boolean
    // argument of the stateful allocator, which indicates propagate on
    // container swap, is set.

    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<TYPE, false, false, true,  false> A01;
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  false, true>  A10;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  true>  A11;
#endif

    if (verbose) printf("\n");

    TestDriver2<TYPE, BAP>::testCase20_dispatch();

    TestDriver2<TYPE, A00>::testCase20_dispatch();
    TestDriver2<TYPE, A01>::testCase20_dispatch();

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    TestDriver2<TYPE, SAA>::testCase20_dispatch();

    TestDriver2<TYPE, A10>::testCase20_dispatch();
    TestDriver2<TYPE, A11>::testCase20_dispatch();
#endif

    // is_always_equal == true
    TestDriver2<TYPE, StatelessAllocator<TYPE> >::testCase20_swap_noexcept();
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 22: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTIONS
        //
        // Testing:
        //   CONCERN: 'std::length_error' is used properly.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING EXCEPTIONS\n"
                            "==================\n");

        TestDriver2<TTA>::testCase22();

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        //
        // Testing:
        //   bool operator< (const deque& lhs, const deque& rhs);
        //   bool operator> (const deque& lhs, const deque& rhs);
        //   bool operator<=(const deque& lhs, const deque& rhs);
        //   bool operator>=(const deque& lhs, const deque& rhs);
        //   auto operator<=>(const deque& lhs, const deque& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING FREE COMPARISON OPERATORS\n"
                            "=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver2<char>::testCase21();

        if (verbose) printf("\n... with 'TestTypeAlloc'.\n");
        TestDriver2<TTA>::testCase21();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Testing:
        //   void swap(deque&);
        //   void swap(deque&, deque&);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SWAP\n"
                            "============\n");

        RUN_EACH_TYPE(MetaTestDriver2,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver2,
                      testCase20,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ERASE AND POP_BACK|POP_FRONT
        //
        // Testing:
        //   void pop_front();
        //   void pop_back();
        //   iterator erase(const_iterator pos);
        //   iterator erase(const_iterator first, const_iterator last);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ERASE AND POP_BACK|POP_FRONT\n"
                            "====================================\n");

        // TBD Want to add an instrumented sanity test for 'pop_back'.  By
        // inspection, it looks like 'pop_back' destroys the item *preceding*
        // the last item, potentially after deallocating the trailing page if
        // the last item was the first item on a data page.

        RUN_EACH_TYPE(TestDriver2,
                      testCase19,
                      char,
                      TTA,
                      SmlTT,
                      MedTT,
                      LrgTT,
                      BMTTA,
                      BCTT);

        RUN_EACH_TYPE(TestDriver2,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver2,
                      testCase19,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver2,
                      testCase19,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING INPUT-RANGE INSERTION
        //
        // Testing:
        //   iterator insert(const_iterator pos, ITER first, ITER last);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING INPUT-RANGE INSERTION\n"
                            "=============================\n");

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<TTA>::testCase18(CharList<TTA>());

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<TTA>::testCase18(CharArray<TTA>());


        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<MedTT>::testCase18(CharList<MedTT>());

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<MedTT>::testCase18(CharArray<MedTT>());


        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BMTTA>::testCase18(CharList<BMTTA>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BMTTA>::testCase18(CharArray<BMTTA>());


        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BCTT>::testCase18(CharList<BCTT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BCTT>::testCase18(CharArray<BCTT>());

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase18(CharArray<AllocInt>());

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING VALUE INSERTION
        //
        // Testing:
        //   iterator insert(const_iterator pos, const T& value);
        //   iterator insert(const_iterator pos, size_type n, const T& value);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING VALUE INSERTION\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase17,
                      char,
                      TTA,
                      SmlTT,
                      MedTT,
                      LrgTT,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase17();

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING PUSHING VALUES
        //
        // Testing:
        //   void push_front(const T& value);
        //   void push_back(const T& value);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PUSHING VALUES\n"
                            "======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase16,
                      char,
                      TTA,
                      SmlTT,
                      MedTT,
                      LrgTT,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase16();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   reverse_iterator rbegin();
        //   reverse_iterator rend();
        //   const_iterator begin() const;
        //   const_iterator cbegin() const;
        //   const_iterator end() const;
        //   const_iterator cend() const;
        //   const_reverse_iterator rbegin() const;
        //   const_reverse_iterator crbegin() const;
        //   const_reverse_iterator rend() const;
        //   const_reverse_iterator crend() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ITERATORS\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase15,
                      char,
                      TTA);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Testing:
        //   reference operator[](size_type position);
        //   reference at(size_type position);
        //   reference front();
        //   reference back();
        //   const_reference front() const;
        //   const_reference back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ELEMENT ACCESS\n"
                            "======================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase14,
                      char,
                      TTA,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase14();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RESERVE AND CAPACITY
        //
        // Testing:
        //   void reserve(size_type n);
        //   void resize(size_type n);
        //   void resize(size_type n, const T& value);
        //   size_type max_size() const;
        //   size_type capacity() const;
        //   bool empty() const;
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RESERVE AND CAPACITY\n"
                            "============================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase13,
                      char,
                      TTA,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase13();

        if (verbose) printf("\nTesting default-insertable type support"
                            "\n========================================\n");

        bsl::deque<NonCopyableType> mX;
        ASSERT(mX.empty());

        mX.resize(10); // grow
        ASSERT(mX.size() == 10);
        // Every element is value-initialized
        for(bsl::deque<NonCopyableType>::iterator it =  mX.begin();
                                                  it != mX.end();
                                                  ++it) {
            ASSERT(*it == NonCopyableType());
        }

        mX.resize(1); // shorten
        ASSERT(mX.size() == 1);
        ASSERT(mX.front() == NonCopyableType());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ASSIGN
        //
        // Testing:
        //   void assign(size_t n, const T& value);
        //   void assign(ITER first, ITER last);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ASSIGNMENT\n"
                            "==================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase12,
                      char,
                      TTA,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase12();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver2<char>::testCase12Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<char>::testCase12Range(CharArray<char>());

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<TTA>::testCase12Range(CharList<TTA>());

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<TTA>::testCase12Range(CharArray<TTA>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BMTTA>::testCase12Range(CharList<BMTTA>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BMTTA>::testCase12Range(CharArray<BMTTA>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BCTT>::testCase12Range(CharList<BCTT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BCTT>::testCase12Range(CharArray<BCTT>());

        StdBslmaTestDriver2<AllocInt>::testCase12Range(CharArray<AllocInt>());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING RANGE AND INITIAL-LENGTH CONSTRUCTORS
        //
        // Testing:
        //   deque(size_type n, const A& a = A());
        //   deque(size_type n, const T& value, const A& a = A());
        //   deque(ITER first, ITER last, const A& a = A());
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RANGE AND INITIAL-LENGTH CONSTRUCTORS\n"
                            "=============================================\n");

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver2,
                      testCase11,
                      char,
                      TTA,
                      BMTTA,
                      BCTT);

        typedef bsltf::StdAllocTestType<bsl::allocator<int> > AllocInt;
        StdBslmaTestDriver2<AllocInt>::testCase11();

        if (verbose) printf("\nTesting default-insertable type support"
                            "\n========================================\n");

        bsl::deque<NonCopyableType> mX(5);
        ASSERT(mX.size() == 5);
        // Every element is value-initialized
        for(bsl::deque<NonCopyableType>::iterator it =  mX.begin();
                                                  it != mX.end();
                                                  ++it) {
            ASSERT(*it == NonCopyableType());
        }

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver2<char>::testCase11Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<char>::testCase11Range(CharArray<char>());

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<TTA>::testCase11Range(CharList<TTA>());

        if (verbose) printf("\n... with 'TestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<TTA>::testCase11Range(CharArray<TTA>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BMTTA>::testCase11Range(CharList<BMTTA>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestTypeAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BMTTA>::testCase11Range(CharArray<BMTTA>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver2<BCTT>::testCase11Range(CharList<BCTT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestTypeNoAlloc' "
                            "and arbitrary random-access iterator.\n");
        TestDriver2<BCTT>::testCase11Range(CharArray<BCTT>());

        StdBslmaTestDriver2<AllocInt>::testCase11Range(CharArray<AllocInt>());

      } break;
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
                   "See 'bslstl_deque.0.t.cpp' for the test plan.\n",
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
