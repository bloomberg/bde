#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

using std::cout; using std::endl; using std::flush; using std::atoi;

// STANDARD MACROS
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << '\t' << flush;

// very simple dummy class for example purposes

class dummy {
    int d_i;
    friend bool operator==(const dummy& lhs, const dummy& rhs);
    friend std::ostream& operator<<(std::ostream& lhs, const dummy& rhs);
  public:
    explicit dummy(int i) : d_i(i) {};
    int value() const {return d_i;};
    void value(const int i) {d_i = i;};
};

bool operator==(const dummy& lhs, const dummy& rhs)
{
    return lhs.d_i == rhs.d_i;
}

bool operator!=(const dummy& lhs, const dummy& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& lhs, const dummy& rhs)
{
    lhs << rhs.d_i;
    return lhs;
}


// TEST UTILITY FUNCTIONS

template <typename U, typename V>
struct IsSame
{
    enum { VALUE = 0 };
};

template <typename U>
struct IsSame<U, U>
{
    enum { VALUE = 1 };
};

// XXX Configure these types for your container/iterator.

typedef std::vector<dummy> Obj;
typedef Obj::iterator Iterator;

typedef dummy IteratorValue;            // iterator's value_type typedef
typedef int   IteratorDifference;       // iterator's difference_type typedef
typedef dummy *IteratorPointer;         // iterator's pointer typedef
typedef dummy& IteratorReference;       // iterator's reference typedef
typedef std::random_access_iterator_tag IteratorCategory;
    // iterator's iterator_category typedef

int main(int argc, char *argv[])
{
    // GENERIC

    int test = argc > 1 ? atoi(argv[1]) : 0;

    // for bde_build.pl

    if (test > 1)
        return -1;

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

   if (verbose) cout << endl
                      << "Testing ITERATORS" << endl
                      << "=================" << endl;

    // *** TYPEDEF TESTS ***
    //
    // Any iterator should comply.

   if (veryVerbose) cout << "\ttypedef test" << endl;
    {
        typedef std::iterator_traits<Iterator> IterTraits;
        ASSERT((IsSame<IterTraits::value_type, IteratorValue>::VALUE));
        ASSERT((IsSame<IterTraits::difference_type,
                                                  IteratorDifference>::VALUE));
        ASSERT((IsSame<IterTraits::pointer, IteratorPointer>::VALUE));
        ASSERT((IsSame<IterTraits::reference, IteratorReference>::VALUE));
        ASSERT((IsSame<IterTraits::iterator_category,
                                                    IteratorCategory>::VALUE));
    }

    // XXX Specify the values you need/want.
    // Duplicate values should not be specified.

    const IteratorValue T1( 0);
    const IteratorValue T2( 1);
    const IteratorValue T3( 2);
    const IteratorValue T4(15);
    const IteratorValue T5(127);
    const IteratorValue * DATA[] = {&T1, &T2, &T3, &T4, &T5};

    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // 'nb' is the number of elements in the generated container, for
    // nb equal 0, NUM_DATA is not used.

    for (int nb = 0; nb <= NUM_DATA; ++nb) {
        Obj X;

        if (verbose) cout << "\nTesting with a size of " << nb << "." << endl;

        if (veryVerbose) cout << "\tgenerating object" << endl;

        // Generate object.

        for (int j = 0; j < nb; ++j) {
            // XXX Customize, this will depend on the test driver and on the
            // object/iterator.

            X.push_back(*DATA[j]);

            if (veryVeryVerbose) {
                T_; P_(*DATA[j]);
            }
        }
        if (veryVeryVerbose) cout << endl;

        // *** NON-MUTATING FORWARD ITERATOR TESTS ***
        //
        // Any iterator should comply.

        if (veryVerbose) cout << "\t non-mutating forward iterator tests"
                              << endl;

        // XXX You may need to customize the calls to begin(), end().  You may
        // also want to explicitly check the value of end() for whitebox
        // testing.

        // breathing test

        LOOP_ASSERT(nb, X.begin() == X.begin());
        LOOP_ASSERT(nb, X.end() == X.end());
        LOOP_ASSERT(nb, !nb || X.begin() != X.end());
        LOOP_ASSERT(nb,  nb || X.begin() == X.end());

        // primary manipulators and accessors test
        // pre-increment, deference, copy constructor, operator == test.
        // Note that operator* is the primary accessor for all values of
        // except X.end(), in that case == is the primary accessor.

        {
            Iterator i = X.begin();
            for (int c = 0; c < nb; ++c) {
                LOOP2_ASSERT(nb, c, !(i == X.end()));
                LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);

                // copy construction

                Iterator j(i);
                LOOP2_ASSERT(nb, c, j != X.end());
                LOOP4_ASSERT(nb, c, *DATA[c], *j, *DATA[c] == *j);

                // operator++

                Iterator k(++i);
                if (c < (nb - 1)) {
                    LOOP4_ASSERT(nb, c, *i, *k, *i == *k);
                    LOOP4_ASSERT(nb, c, *i, *j, *i != *j);
                }
                else {
                    LOOP2_ASSERT(nb, c, i == X.end());
                    LOOP2_ASSERT(nb, c, k == X.end());
                }
            }
            LOOP_ASSERT(nb, i == X.end());
        }

        // operator== and operator !=

        {
            // Note that in this test we reach 'X.end()'.

            Iterator i = X.begin();
            for (int c = 0; c <= nb; ++c) {
                Iterator j = X.begin();
                for (int d = 0; d <= nb; ++d) {

                    // The two tests are necessary if operator!= is NOT
                    // implemented as "return !(lhs == rhs);".

                    if (c == d) {
                        LOOP3_ASSERT(nb, c, d, i == j);
                        LOOP3_ASSERT(nb, c, d, !(i != j));
                    }
                    else {
                        LOOP3_ASSERT(nb, c, d, i != j);
                        LOOP3_ASSERT(nb, c, d, !(i == j));
                    }
                    if (d != nb) {
                        ++j;
                    }
                }
                if (c != nb) {
                    ++i;
                }
            }
        }

        // assignment operator

        {
            Iterator i = X.begin();
            for (int c = 0; c < nb; ++c) {
                Iterator j = X.begin();
                for (int d = 0; d < nb; ++d) {
                    Iterator k(i);

                    // For this self assignment test, you might use a allocator
                    // test to ensure we are not leaking memory.

                    i = i;
                    LOOP3_ASSERT(nb, c, d, k == i);

                    k = j;
                    LOOP3_ASSERT(nb, c, d, k == j);
                    ++j;
                }
                ++i;
            }
        }

        // operator->

        Iterator i = X.begin();
        for (i = X.begin(); i != X.end(); ++i) {
            LOOP_ASSERT(nb, (*i).value() == i->value());
        }

        // post increment operator test

        i = X.begin();
        for (int c = 0; c < nb; ++c) {
            LOOP2_ASSERT(nb, c, i != X.end());
            LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i++));
        }
        LOOP_ASSERT(nb, i == X.end());

        // *** NON-MUTATING BIDIRECTIONAL ITERATOR TESTS ***
        //
        // XXX Remove if the iterator is not bidirectional.

        if (veryVerbose) cout << "\t non-mutating bidirectional iterator tests"
                              << endl;

        // pre-decrement operator test

        i = X.end();
        for (int c = (nb - 1); c >= 0; --c) {
            Iterator j = --i;
            LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);
            LOOP2_ASSERT(nb, c, j == i);
        }

        // post-decrement operator test

        i = X.end();
        --i;
        for (int c = (nb - 1); c > 0; --c) {
            LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i--));
        }

        // test last value, if size > 0.

        LOOP_ASSERT(nb, !nb || *i == *DATA[0]);

        // *** NON-MUTATING RANDOM ACCESS ITERATOR TESTS ***
        //
        // XXX Remove if the iterator is not a random-access iterator.

        if (veryVerbose) cout << "\t non-mutating random-access iterator tests"
                              << endl;

        // iter operator+(iter, int)
        // IteratorDifference operator-(iter, iter)
        // iter operator-(iter, int)

        i = X.begin();
        for (int c = 0; c <= nb; ++c) {
            Iterator j = X.begin();
            for (int d = 0; d <= nb; ++d) {
                const IteratorDifference offset = d - c; // Offset can be <0.
                LOOP3_ASSERT(nb, c, d, (i + offset) == j);
                LOOP3_ASSERT(nb, c, d, (j - offset) == i);
                LOOP3_ASSERT(nb, c, d, offset == (j - i));
                ++j;
            }
            ++i;
        }

        // iter operator+=(int)
        // iter operator-=(int)

        i = X.begin();
        for (int c = 0; c <= nb; ++c) {
            for (int d = 0; d <= nb; ++d) {
                const IteratorDifference offset = d - c;
                Iterator j = i;
                LOOP3_ASSERT(nb, c, d, (i + offset) == (j += offset));
                LOOP3_ASSERT(nb, c, d, (i + offset) == j);

                const IteratorDifference negoff = -offset;
                j = i;
                LOOP3_ASSERT(nb, c, d, (i - negoff) == (j -= negoff));
                LOOP3_ASSERT(nb, c, d, (i - negoff) == j);
            }
            ++i;
        }

        // bool operator>(iter)
        // bool operator<(iter)
        // bool operator>=(iter)
        // bool operator<=(iter)

        for (i = X.begin(); i < X.end(); ++i) {
            for (Iterator j = X.begin(); j < X.end(); ++j) {
                if ((j - i) > 0) {
                    LOOP_ASSERT(nb,  (j >  i));
                    LOOP_ASSERT(nb,  (j >= i));
                    LOOP_ASSERT(nb, !(j <  i));
                    LOOP_ASSERT(nb, !(j <= i));
                    LOOP_ASSERT(nb, !(j == i));
                    LOOP_ASSERT(nb,  (j != i));
                }
                else if ((j - i) < 0) {
                    LOOP_ASSERT(nb, !(j >  i));
                    LOOP_ASSERT(nb, !(j >= i));
                    LOOP_ASSERT(nb,  (j <  i));
                    LOOP_ASSERT(nb,  (j <= i));
                    LOOP_ASSERT(nb, !(j == i));
                    LOOP_ASSERT(nb,  (j != i));
                }
                else {
                    LOOP_ASSERT(nb, !(j >  i));
                    LOOP_ASSERT(nb,  (j >= i));
                    LOOP_ASSERT(nb, !(j <  i));
                    LOOP_ASSERT(nb,  (j <= i));
                    LOOP_ASSERT(nb,  (i == j));
                    LOOP_ASSERT(nb, !(j != i));
                }
            }
        }

        // const operator[]

        for (i = X.begin(); i != X.end(); ++i) {
            for (Iterator j = X.begin(); j != X.end(); ++j) {
                LOOP_ASSERT(nb, i[j - i] == *j);
            }
        }

        // *** MUTATING ITERATOR TESTS ***
        // XXX These tests are only for non-const iterators.

        if (veryVerbose) cout << "\t mutating iterator tests" << endl;

        // XXX Provide data to test non-const iterators.

        const IteratorValue A1(256);
        const IteratorValue A2( 3);
        const IteratorValue A3( 98);
        const IteratorValue * ASSIGN[] = {&T1, &T2, &T3};
        const int NUM_ASSIGN = sizeof ASSIGN / sizeof *ASSIGN;

        for (int c = 0; c < NUM_ASSIGN; ++c) {
            const IteratorValue val = *ASSIGN[c];

            // operator*
            // for all kind of iterators

            for (i = X.begin(); i != X.end(); ++i) {

                // *i test

                LOOP2_ASSERT(nb, c, val == (*i = val));
                LOOP2_ASSERT(nb, c, val == *i);

                // test for potential side effects

                for (Iterator j = X.begin(); j != X.end(); ++j) {
                    if (i != j) {
                        LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                    }
                }
                *i = *DATA[i - X.begin()]; // reset old value
            }

            // operator->
            // Remove if operator-> is not implemented.

            for (i = X.begin(); i != X.end(); ++i) {
                i->value(val.value());
                LOOP2_ASSERT(nb, c, val == *i);

                // test for potential side effects

                for (Iterator j = X.begin(); j != X.end(); ++j) {
                    if (i != j) {
                        LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                    }
                }
                *i = *DATA[i - X.begin()];
            }

            // *** MUTATING FORWARD ITERATOR TESTS ***

            for (i = X.begin(); i != X.end(); ++i) {
                const Iterator curr = i;
                const Iterator next = ++i;
                i = curr;

                // *(i++) test

                LOOP2_ASSERT(nb, c, val == (*(i++) = val));
                LOOP2_ASSERT(nb, c, next == i);
                LOOP2_ASSERT(nb, c, val == *curr);
                i = curr;

                // test for potential side effects

                for (Iterator j = X.begin(); j != X.end(); ++j) {
                    if (i != j) {
                        LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                    }
                }
                *i = *DATA[i - X.begin()]; // Reset the old value.

                // *(++i) test

                if (next != X.end()) {
                    LOOP2_ASSERT(nb, c, val == (*(++i) = val));
                    LOOP2_ASSERT(nb, c, val == *i);
                    LOOP2_ASSERT(nb, c, next == i);

                    // test for potential side effects

                    for (Iterator j = X.begin(); j != X.end(); ++j) {
                        if (i != j) {
                            LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                        }
                    }
                    *i = *DATA[i - X.begin()]; // Reset the old value.
                    i = curr;
                }
            }


            // *** MUTATING BIDIRECTIONAL ITERATOR TESTS ***
            // Remove if your iterator is not a bidirectional iterator.

            for (i = X.begin(); i != X.end(); ++i) {
                if (i == X.begin())
                    continue;

                const Iterator curr = i;
                const Iterator prev = --i;
                i = curr;

                // *(i--) test

                LOOP2_ASSERT(nb, c, val == (*(i--) = val));
                LOOP2_ASSERT(nb, c, prev == i);
                LOOP2_ASSERT(nb, c, val == *curr);
                i = curr;

                // test for potential side effects

                for (Iterator j = X.begin(); j != X.end(); ++j) {
                    if (i != j) {
                        LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                    }
                }
                *i = *DATA[i - X.begin()]; // Reset the old value.

                // *(--i) test

                LOOP2_ASSERT(nb, c, val == (*(--i) = val));
                LOOP2_ASSERT(nb, c, prev == i);
                LOOP2_ASSERT(nb, c, val == *i);

                // test for potential side effects

                for (Iterator j = X.begin(); j != X.end(); ++j) {
                    if (i != j) {
                        LOOP2_ASSERT(nb, c, *DATA[j - X.begin()] == *j);
                    }
                }
                *i = *DATA[i - X.begin()]; // Reset the old value.
                i = curr;
            }

            // *** MUTATING RANDOM-ACCESS ITERATOR TESTS ***
            // operator[]

            for (i = X.begin(); i != X.end(); ++i) {
                for (Iterator j = X.begin(); j != X.end(); ++j) {

                    // shortcuts

                    const IteratorDifference index = j - i;
                    const IteratorValue old = i[index];

                    LOOP2_ASSERT(nb, c, val == (i[index] = val));
                    LOOP2_ASSERT(nb, c, val == i[index]);

                    // test for potential side effects

                    for (Iterator k = X.begin(); k != X.end(); ++k) {
                        if (j != k) {
                            LOOP2_ASSERT(nb, c, *DATA[k - X.begin()] == *k);
                        }
                    }
                    i[index] = old;
                }
            }
        }

        // *** REVERSE ITERATOR TESTS ***
        // These tests are very simple IF the reverse iterator is implemented
        // using a TESTED std::reserse_iterator<Iterator>-like template.
        //
        // If your reverse iterator is not using such a facility, you need to
        // test it using all the previous tests which apply.

        if (veryVerbose) cout << "\t reverse iterator tests" << endl;

        // XXX Configure the typedef.

        typedef Obj::reverse_iterator ReverseIterator;

        if (nb == 0) {
            LOOP_ASSERT(nb, X.rbegin() == X.rend());
        }
        else {
            ReverseIterator ri;

            for (i = X.end(), ri = X.rbegin(); ri != X.rend(); ++ri) {
                --i;
                LOOP_ASSERT(nb, *i == *ri);
                LOOP_ASSERT(nb, i->value() == ri->value());
            }
            LOOP_ASSERT(nb, X.begin() == i);

            for (ri = X.rend(), i = X.begin(); i != X.end(); ++i) {
                --ri;
                LOOP_ASSERT(nb, *i == *ri);
                LOOP_ASSERT(nb, i->value() == ri->value());
            }
            LOOP_ASSERT(nb, X.rbegin() == ri);
        }
    }

    return 0;
}
