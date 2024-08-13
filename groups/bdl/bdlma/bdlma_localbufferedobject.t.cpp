// bdlma_localbufferedobject.t.cpp                                    -*-C++-*-

#include <bdlma_localbufferedobject.h>

#include <bdlb_random.h>

#include <bslim_testutil.h>

#include <bsltf_movablealloctesttype.h>
#include <bsltf_moveonlyalloctesttype.h>

#include <bslma_allocator.h>
#include <bslma_bufferallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_sequentialpool.h>
#include <bslma_testallocator.h>

#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this test suite are to verify 1) that
// 'bdlma::LocalBufferedObject' correctly utilizes its arena memory allocator
// until it's exhausted, after which it uses the allocator passed at
// construction.
//
// All testing of C++11 'initializer_list's is done in TC 5.  Other than that
// constructors are exhaustively testing in TC 2, and 'emplace' is tested in
// TC 3.
//
//-----------------------------------------------------------------------------
// [ 5] constructor(initializer_list);
// [ 5] constructor(allocator_arg_t, allocator_type, initializer_list);
// [ 5] void emplace(initializer_list);
// [ 4] ASSIGNMENT
// [ 3] EMPLACE
// [ 2] CONSTRUCTORS
// [ 2] t_TYPE *operator->()
// [ 2] t_TYPE& operator*()
// [ 2] const t_TYPE *operator->() const
// [ 2] const t_TYPE& operator*() const
// [ 2] allocator_type get_allocator() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT  BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q  BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P  BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma::TestAllocator        TestAllocator;
typedef bsls::Types::Int64          Int64;
typedef bslmf::MovableRefUtil       MoveUtil;
typedef bsltf::MoveState            MoveState;
typedef bsltf::MovableAllocTestType MATT;

const int k_DEFAULT_BUFFER_SIZE = bdlma::LocalBufferedObject<bsl::string>::
                                                                 k_BUFFER_SIZE;

// ============================================================================
//                     GLOBAL VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                          USAGE EXAMPLE 1
// ----------------------------------------------------------------------------

namespace Usage {
//
///Usage
///-----
//
///Example 1: Configuring an Object to Allocate From Stack Memory
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have an array of 'bsl::string_view's containing names, with a
// large number of redundant entries, and we want to count how many unique
// names exist in the array.  We write a function 'countUniqueNames' which
// stores the names in an unordered set, and yields the 'size' accessor as the
// total count of unique names.
//
// The function will be called many times, and 'bsl::unordered_set' does a
// large number of small memory allocations.  These allocations would be faster
// if they came from a non-freeing allocator that gets its memory from a buffer
// on the stack.
//
// We can use a 'LocalBufferedObject' to create an 'unordered_set' with an
// 8192-byte stack buffer from which it is to allocate memory.
//..
    size_t countUniqueNames(const bsl::string_view *rawNames,
                            size_t                  numRawNames)
    {
        bdlma::LocalBufferedObject<bsl::unordered_set<bsl::string_view>,
                                   8192> uset;

        for (unsigned uu = 0; uu < numRawNames; ++uu) {
            uset->insert(rawNames[uu]);
        }

        return uset->size();
    }
//..
// Notice that this syntactic convenience equivalent to supplying a local
// 'LocalSequentialAllocator' to the 'bsl::unordered_set'.
//
// Below we show the allocation behavior of this function as the number of
// items in the 'unordered_set' increases.  Note that when the memory in the
// 8192-byte stack buffer is exhausted, further memory comes from the default
// allocator:
//..
//  'countUniqueNames':
//  Names: (raw:   25, unique:  23), used default allocator: 0
//  Names: (raw:   50, unique:  42), used default allocator: 0
//  Names: (raw:  100, unique:  70), used default allocator: 0
//  Names: (raw:  200, unique: 103), used default allocator: 0
//  Names: (raw:  400, unique: 130), used default allocator: 1
//  Names: (raw:  800, unique: 143), used default allocator: 1
//  Names: (raw: 1600, unique: 144), used default allocator: 1
//..
//
///Example 2: Eliding the Destructor
///- - - - - - - - - - - - - - - - -
// Because the only resource managed by the 'unordered_set' is memory, we can
// improve the performance of the previous example using the template's boolean
// 't_DISABLE_DESTRUCTOR' parameter.
//
// 'unordered_set' allocates a lot of small nodes, and when the container is
// destroyed, unordered set's destructor traverses the whole data structure,
// visting every node and calling 'bslma::Allocator::deallocate' on each one,
// which is a non-inline virtual function call eventually handled by the
// sequential allocator's 'deallocate' function, which does nothing.
//
// If we set the 3rd template parameter of 'LocalBufferedObject', which is
// 't_DISABLE_DESTRUCTION' of type 'bool', to the non-default value of 'true',
// the 'LocalBufferedObject' will not call the destructor of the held
// 'unordered_set'.  This isn't a problem because unordered set manages no
// resource other than memory, and all the memory it uses is managed by the
// local sequential allocator contained in the local buffered object.
//..
    size_t countUniqueNamesFaster(const bsl::string_view *rawNames,
                                  size_t                  numRawNames)
    {
        bdlma::LocalBufferedObject<bsl::unordered_set<bsl::string_view>,
                                   8192,
                                   true> uset;

        for (unsigned uu = 0; uu < numRawNames; ++uu) {
            uset->insert(rawNames[uu]);
        }

        return uset->size();
    }
//..
// And we see the calculations are exactly the same:
//..
//  'countUniqueNamesFaster': destructor disabled:
//  Names: (raw:   25, unique:  23), used default allocator: 0
//  Names: (raw:   50, unique:  42), used default allocator: 0
//  Names: (raw:  100, unique:  70), used default allocator: 0
//  Names: (raw:  200, unique: 103), used default allocator: 0
//  Names: (raw:  400, unique: 130), used default allocator: 1
//  Names: (raw:  800, unique: 143), used default allocator: 1
//  Names: (raw: 1600, unique: 144), used default allocator: 1
//..

// The following code calls the functions shown in the usage example in the .h
// file, but its code is not shown in the .h file.  Its output is.

class NameGenerator {
    bsl::vector<bsl::string>         d_nameVector;
    bsl::vector<bsl::string_view>    d_viewVector;

  public:
    // CREATORS
    NameGenerator(size_t numRawNames, bslma::Allocator *alloc);

    // ACCESSORS
    void assessNames() const;
};

// CREATORS
NameGenerator::NameGenerator(size_t numRawNames, bslma::Allocator *alloc)
: d_nameVector(alloc)
, d_viewVector(alloc)
{
    // 12 possible first names, 12 possible last names, therefore 144
    // possible names.

    // Names are of famous science fiction authors.

    static const char *firstNames[] = {
       "Isaac", "Arthur", "Robert", "Ursela" , "Philip", "Aldous", "George",
       "Stephen", "Jack", "Larry", "Mary", "H.G." };
    enum { e_NUM_FIRST_NAMES = sizeof firstNames / sizeof *firstNames };
    static const char *lastNames[] = {
       "Asimov", "Clarke", "Heinlein", "LeGuin", "Dick", "Huxley", "Orwell",
       "King", "London", "Niven", "Shelley", "Wells" };
    enum { e_NUM_LAST_NAMES = sizeof lastNames / sizeof *lastNames };

    int seed = 0xdeadbeef;

    bsl::string s(alloc);

    d_nameVector.reserve(numRawNames);
    d_viewVector.reserve(numRawNames);
    for (unsigned uu = 0; uu < numRawNames; ++uu) {
        int idx = bdlb::Random::generate15(&seed) % e_NUM_FIRST_NAMES;
        s = firstNames[idx];
        s += ' ';
        idx     = bdlb::Random::generate15(&seed) % e_NUM_LAST_NAMES;
        s += lastNames[idx];

        d_nameVector.push_back(s);
        d_viewVector.push_back(d_nameVector.back());
    }
};

// ACCESSORS
void NameGenerator::assessNames() const
{
    bslma::TestAllocator ossTa;
    bsl::ostringstream oss(&ossTa);

    bsl::ostream& stream = *(veryVerbose ? static_cast<bsl::ostream *>(&cout)
                                         : static_cast<bsl::ostream *>(&oss));

    stream << "\n'countUniqueNames':\n";
    for (size_t numRawNames = 25; numRawNames <= d_viewVector.size();
                                                            numRawNames *= 2) {
        bslma::TestAllocator         da;
        bslma::DefaultAllocatorGuard guard(&da);

        size_t uniqueNames = countUniqueNames(d_viewVector.data(),
                                              numRawNames);
        stream << "Names: (raw: " << bsl::setw(4) << numRawNames <<
                    ", unique: " << bsl::setw(3) << uniqueNames <<
                             "), used default allocator: " <<
                                             (0 < da.numAllocations()) << endl;
    }

    stream << "\n'countUniqueNamesFaster': destructor disabled:\n";
    for (size_t numRawNames = 25; numRawNames <= d_viewVector.size();
                                                            numRawNames *= 2) {
        bslma::TestAllocator         da;
        bslma::DefaultAllocatorGuard guard(&da);

        size_t uniqueNames = countUniqueNamesFaster(d_viewVector.data(),
                                                    numRawNames);

        stream << "Names: (raw: " << bsl::setw(4) << numRawNames <<
                    ", unique: " << bsl::setw(3) << uniqueNames <<
                             "), used default allocator: " <<
                                             (0 < da.numAllocations()) << endl;
    }
}

}  // close namespace Usage

// ============================================================================
//                  GLOBAL CLASSES/STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

bsl::ostream& operator<<(bsl::ostream& stream, MoveState::Enum value)
{
    const char *result = MoveState::e_NOT_MOVED == value ? "e_NOT_MOVED"
                       : MoveState::e_MOVED     == value ? "e_MOVED"
                       : MoveState::e_UNKNOWN   == value ? "e_UNKNOWN"
                       :                                   "e_GARBAGE";

    stream << result;

    return stream;
}

namespace {
namespace u {

void setMATTVector(bsl::vector<MATT> *result, unsigned multiple)
{
    for (unsigned uu = 0; uu < result->size(); ++uu) {
        (*result)[uu].setData(multiple * uu);
    }
}

struct A {
    int d_ii;

    BSLMF_NESTED_TRAIT_DECLARATION(A, bslma::UsesBslmaAllocator);

    explicit
    A(bslma::Allocator *alloc)
    : d_ii(-1)
    {
        ASSERT(0 != alloc);
    }

    A(int               ii,
      bslma::Allocator *alloc)
    : d_ii(ii)
    {
        ASSERT(0 != alloc);
    }

    int& value() { return d_ii; }

    int value() const { return d_ii; }
};

template <bool t_DESTRUCTOR_BLOWS_UP = false>
class B {
    // The constant 't_DESTRUCTOR_BLOWS_UP', if true, determines that the
    // destructor of 'u::B<t_SUPPRESS_DTOR>' will fail an assert if called.
    //
    // This 'class' has a class data member we use to count the number of times
    // the destructor is called, whether the destructor is to blow up or not.
    //
    // This 'class' contains 10 'MATT' objects which each allocate a 1-byte
    // memory segment, are movable, and track moves.  They can be independently
    // accessed via 'operator[]'.  In constructors, each can be initialized by
    // copy or move by a separate argument.  This is to test that contructors
    // can be passed some arguments by const ref while simultaneously passing
    // moved object to others.
    //
    // This 'class' also has copy and move c'tors.
    //
    // This 'class' also has a c'tor that takes an allocator and an
    // 'initializer_list'.
    //
    // This 'class' also has a vector data member which c'tors (other than copy
    // or move c'tors) initialize to empty, but which can allocate large
    // amoounts of memory through the 'useMemory' accessor.  This will exhaust
    // the arena memory of a local buffered object containing it and cause the
    // other memory alloctor to be used, useful for testing.

  public:
    // PUBLIC TYPES
    enum { e_NUM_MEMBERS = 10 };

    // PUBLIC CLASS DATA
    static int s_numDestructions;

  private:
    // DATA
    bsltf::MovableAllocTestType d_0, d_1, d_2, d_3, d_4, d_5, d_6, d_7, d_8,
                                                                           d_9;
    bsl::vector<char>           d_vec;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(B, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit B(bslma::Allocator *alloc = 0);

    template <class T0,
              class T1,
              class T2,
              class T3,
              class T4,
              class T5,
              class T6,
              class T7,
              class T8,
              class T9>
    B(BSLS_COMPILERFEATURES_FORWARD_REF(T0)  a0,
      BSLS_COMPILERFEATURES_FORWARD_REF(T1)  a1,
      BSLS_COMPILERFEATURES_FORWARD_REF(T2)  a2,
      BSLS_COMPILERFEATURES_FORWARD_REF(T3)  a3,
      BSLS_COMPILERFEATURES_FORWARD_REF(T4)  a4,
      BSLS_COMPILERFEATURES_FORWARD_REF(T5)  a5,
      BSLS_COMPILERFEATURES_FORWARD_REF(T6)  a6,
      BSLS_COMPILERFEATURES_FORWARD_REF(T7)  a7,
      BSLS_COMPILERFEATURES_FORWARD_REF(T8)  a8,
      BSLS_COMPILERFEATURES_FORWARD_REF(T9)  a9,
      bslma::Allocator                      *alloc = 0);

    B(const B& original, bslma::Allocator *alloc = 0);

    explicit
    B(bslmf::MovableRef<B> original, bslma::Allocator *alloc = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE>
    B(std::initializer_list<INIT_LIST_TYPE>  il,
    bslma::Allocator                        *alloc);
#endif

    ~B();

    // MANIPULATORS
    B& operator=(const B& rhs)
    {
        for (int ii = 0; ii < e_NUM_MEMBERS; ++ii) {
            (*this)[ii] = rhs[ii];
        }

        d_vec = rhs.d_vec;

        return *this;
    }

    B& operator=(bslmf::MovableRef<B> rhs)
    {
        B& local = rhs;

        for (int ii = 0; ii < e_NUM_MEMBERS; ++ii) {
            (*this)[ii] = MoveUtil::move(local[ii]);
        }

        d_vec = MoveUtil::move(local.d_vec);

        return *this;
    }

    B& operator=(const A& rhs)
    {
        for (int ii = 0; ii < e_NUM_MEMBERS; ++ii) {
            (*this)[ii].setData(ii * rhs.d_ii);
        }

        d_vec.clear();

        return *this;
    }

    B& operator=(bslmf::MovableRef<A> rhs)
    {
        A& local = rhs;

        for (int ii = 0; ii < e_NUM_MEMBERS; ++ii) {
            (*this)[ii].setData(ii * local.d_ii);
            (*this)[ii].setMovedInto(MoveState::e_MOVED);
        }

        local.d_ii = -1;

        d_vec.clear();

        return *this;
    }

    MATT& operator[](int ii)
    {
        switch (ii) {
          case 0: return d_0;                                         // RETURN
          case 1: return d_1;                                         // RETURN
          case 2: return d_2;                                         // RETURN
          case 3: return d_3;                                         // RETURN
          case 4: return d_4;                                         // RETURN
          case 5: return d_5;                                         // RETURN
          case 6: return d_6;                                         // RETURN
          case 7: return d_7;                                         // RETURN
          case 8: return d_8;                                         // RETURN
          case 9: return d_9;                                         // RETURN
        }

        ASSERT(0 && "B::[] out of range");
        return d_0;
    }

    void clearMemory()
    {
        d_vec.clear();
    }

    void setToMultiple(int multiple)
    {
        for (int ii = 0; ii < e_NUM_MEMBERS; ++ii) {
            (*this)[ii].setData(multiple * ii);
        }
    }

    void useMemory(bsl::size_t size =
                        bdlma::LocalBufferedObject<bsl::string>::k_BUFFER_SIZE)
    {
        d_vec.resize(d_vec.size() + size);
    }

    // ACCESSORS
    const MATT& operator[](int ii) const
    {
        switch (ii) {
          case 0: return d_0;                                         // RETURN
          case 1: return d_1;                                         // RETURN
          case 2: return d_2;                                         // RETURN
          case 3: return d_3;                                         // RETURN
          case 4: return d_4;                                         // RETURN
          case 5: return d_5;                                         // RETURN
          case 6: return d_6;                                         // RETURN
          case 7: return d_7;                                         // RETURN
          case 8: return d_8;                                         // RETURN
          case 9: return d_9;                                         // RETURN
        }

        ASSERT(0 && "B::[] out of range");
        return d_0;
    }

    bool operator==(const B& rhs) const
    {
        const B& lhs = *this;

        int ii = 0;
        for (; ii < e_NUM_MEMBERS; ++ii) {
            if (lhs[ii] != rhs[ii]) {
                break;
            }
        }

        return e_NUM_MEMBERS == ii && d_vec == rhs.d_vec;
    }

    bool operator!=(const B& rhs) const
    {
        return !(*this == rhs);
    }

    bslma::Allocator *allocator() const
    {
        bslma::Allocator *alloc = d_0.allocator();

        int ii = 1;
        for (; ii < e_NUM_MEMBERS; ++ii) {
            if ((*this)[ii].allocator != alloc) {
                break;
            }
        }

        ASSERT(e_NUM_MEMBERS == ii);

        return alloc;
    }

    MoveState::Enum movedFrom() const
    {
        const MoveState::Enum ret = d_0.movedFrom();

        int ii = 1;
        for (; ii < 10; ++ii) {
            if ((*this)[ii].movedFrom() != ret) {
                break;
            }
        }

        return 10 == ii ? ret : MoveState::e_UNKNOWN;
    }

    MoveState::Enum movedInto() const
    {
        const MoveState::Enum ret = d_0.movedInto();

        int ii = 1;
        for (; ii < 10; ++ii) {
            if ((*this)[ii].movedInto() != ret) {
                break;
            }
        }

        return 10 == ii ? ret : MoveState::e_UNKNOWN;
    }
};

template <bool t_DESTRUCTOR_BLOWS_UP>
B<t_DESTRUCTOR_BLOWS_UP>::B(bslma::Allocator *alloc)
: d_0(alloc)
, d_1(alloc)
, d_2(alloc)
, d_3(alloc)
, d_4(alloc)
, d_5(alloc)
, d_6(alloc)
, d_7(alloc)
, d_8(alloc)
, d_9(alloc)
, d_vec(alloc)
{}

template <bool t_DESTRUCTOR_BLOWS_UP>
template <class T0,
          class T1,
          class T2,
          class T3,
          class T4,
          class T5,
          class T6,
          class T7,
          class T8,
          class T9>
B<t_DESTRUCTOR_BLOWS_UP>::B(
     BSLS_COMPILERFEATURES_FORWARD_REF(T0)  a0,
     BSLS_COMPILERFEATURES_FORWARD_REF(T1)  a1,
     BSLS_COMPILERFEATURES_FORWARD_REF(T2)  a2,
     BSLS_COMPILERFEATURES_FORWARD_REF(T3)  a3,
     BSLS_COMPILERFEATURES_FORWARD_REF(T4)  a4,
     BSLS_COMPILERFEATURES_FORWARD_REF(T5)  a5,
     BSLS_COMPILERFEATURES_FORWARD_REF(T6)  a6,
     BSLS_COMPILERFEATURES_FORWARD_REF(T7)  a7,
     BSLS_COMPILERFEATURES_FORWARD_REF(T8)  a8,
     BSLS_COMPILERFEATURES_FORWARD_REF(T9)  a9,
     bslma::Allocator                      *alloc)
: d_0(BSLS_COMPILERFEATURES_FORWARD(T0, a0), alloc)
, d_1(BSLS_COMPILERFEATURES_FORWARD(T1, a1), alloc)
, d_2(BSLS_COMPILERFEATURES_FORWARD(T2, a2), alloc)
, d_3(BSLS_COMPILERFEATURES_FORWARD(T3, a3), alloc)
, d_4(BSLS_COMPILERFEATURES_FORWARD(T4, a4), alloc)
, d_5(BSLS_COMPILERFEATURES_FORWARD(T5, a5), alloc)
, d_6(BSLS_COMPILERFEATURES_FORWARD(T6, a6), alloc)
, d_7(BSLS_COMPILERFEATURES_FORWARD(T7, a7), alloc)
, d_8(BSLS_COMPILERFEATURES_FORWARD(T8, a8), alloc)
, d_9(BSLS_COMPILERFEATURES_FORWARD(T9, a9), alloc)
, d_vec(alloc)
{}

template <bool t_DESTRUCTOR_BLOWS_UP>
B<t_DESTRUCTOR_BLOWS_UP>::B(const B& original, bslma::Allocator *alloc)
: d_0(original.d_0, alloc)
, d_1(original.d_1, alloc)
, d_2(original.d_2, alloc)
, d_3(original.d_3, alloc)
, d_4(original.d_4, alloc)
, d_5(original.d_5, alloc)
, d_6(original.d_6, alloc)
, d_7(original.d_7, alloc)
, d_8(original.d_8, alloc)
, d_9(original.d_9, alloc)
, d_vec(original.d_vec, alloc)
{}

template <bool t_DESTRUCTOR_BLOWS_UP>
B<t_DESTRUCTOR_BLOWS_UP>::B(bslmf::MovableRef<B>  original,
                            bslma::Allocator     *alloc)
: d_0(MoveUtil::move(MoveUtil::access(original).d_0), alloc)
, d_1(MoveUtil::move(MoveUtil::access(original).d_1), alloc)
, d_2(MoveUtil::move(MoveUtil::access(original).d_2), alloc)
, d_3(MoveUtil::move(MoveUtil::access(original).d_3), alloc)
, d_4(MoveUtil::move(MoveUtil::access(original).d_4), alloc)
, d_5(MoveUtil::move(MoveUtil::access(original).d_5), alloc)
, d_6(MoveUtil::move(MoveUtil::access(original).d_6), alloc)
, d_7(MoveUtil::move(MoveUtil::access(original).d_7), alloc)
, d_8(MoveUtil::move(MoveUtil::access(original).d_8), alloc)
, d_9(MoveUtil::move(MoveUtil::access(original).d_9), alloc)
, d_vec(MoveUtil::move(MoveUtil::access(original).d_vec), alloc)
{}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <bool t_DESTRUCTOR_BLOWS_UP>
template <class INIT_LIST_TYPE>
B<t_DESTRUCTOR_BLOWS_UP>::B(std::initializer_list<INIT_LIST_TYPE>  il,
                            bslma::Allocator                      *alloc)
: d_0(*(il.begin() + 0), alloc)
, d_1(*(il.begin() + 1), alloc)
, d_2(*(il.begin() + 2), alloc)
, d_3(*(il.begin() + 3), alloc)
, d_4(*(il.begin() + 4), alloc)
, d_5(*(il.begin() + 5), alloc)
, d_6(*(il.begin() + 6), alloc)
, d_7(*(il.begin() + 7), alloc)
, d_8(*(il.begin() + 8), alloc)
, d_9(*(il.begin() + 9), alloc)
, d_vec(alloc)
{
    ASSERT(e_NUM_MEMBERS == il.end() - il.begin());
}
#endif

template <bool t_DESTRUCTOR_BLOWS_UP>
B<t_DESTRUCTOR_BLOWS_UP>::~B()
{
    ++s_numDestructions;

    ASSERT(!t_DESTRUCTOR_BLOWS_UP);
}

template <bool t_SUPPRESS_DTOR>
int B<t_SUPPRESS_DTOR>::s_numDestructions = 0;

struct DerivedTestAllocator : bslma::TestAllocator {
    typedef bslma::TestAllocator Base;

    explicit
    DerivedTestAllocator(const char *name)
    : Base(name)
    {}

    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;
};

void *DerivedTestAllocator::allocate(size_type size)
{
    return Base::allocate(size);
}

}  // close namespace u
}  // close unnamed namespace

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <bool t_SUPPRESS_DTOR>
void testCase5_initializerLists()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER_LISTS
    //
    // Concerns:
    //: 1 If the local buffered object contains a type with a constructor that
    //:   takes an initializer list, that the constructor of the local buffered
    //:   objecct will properly propagate the initailizer list to the contained
    //:   type.
    //:
    //: 2 This works whether or not an alloctaor is also passed to the
    //:   constructor.
    //:
    //: 3 Test with & without destructor suppression.  This function takes a
    //:   'bool' template parameter to drive that, and is called with either
    //:   value of that 'bool'.
    //:
    //; 4 That calling 'emplace' with an initializer list functions properly.
    //
    // Plan:
    //: 1 The constant 't_SUPPRESS_DTOR' determines two things:
    //:   o If true, determines that the destructor of 'u::B<t_SUPPRESS_DTOR>'
    //:     will fail an assert if called.
    //:
    //:   o If true, determines that the local buffered object container will
    //:     suppress calling the destructor of 'B' in the destructor of the
    //:     container, and in the 'emplace' call.
    //:
    //:   o The test is run with both values of 'T_SUPPRESS_DTOR',
    //:
    //:   o Type 'u::B' has a class data member we use to count how many times
    //:     the number of times the destructor is called, whether the
    //:     destructor is to blow up or not.
    //:
    //:   o Type 'u::B' == 'TestB' contains 10 'MATT' objects which each
    //:     allocate a 1-byte memory segment, are movable, and track moves.
    //:     They can be independently accessed via 'operator[]'.  In
    //:     constructors, each can be initialized by copy or move by a separate
    //:     argument.
    //:
    //: 2 We test various 2 constructors of 'u::B':
    //:   o With an 'initializer_list' alone.
    //:
    //:   o With an allocator and an 'initializer_list'.
    //:
    //: 3 We test one overload of 'emplace', taking an 'initializer_list'
    //:   alone.
    //
    // Testing:
    //   constructor(initializer_list);
    //   constructor(allocator_arg_t, allocator_type, initializer_list);
    //   void emplace(initializer_list);
    // ------------------------------------------------------------------------

    if (verbose) cout << "INITIALIZER_LIST: suppress: " <<
                                                       t_SUPPRESS_DTOR << endl;

    u::DerivedTestAllocator       da("default");
    bsl::allocator<>              daa(&da);
    bslma::DefaultAllocatorGuard  guard(&da);
    bslma::TestAllocator          sa("source allocator");
    bslma::TestAllocator          ta("test");
    bsl::allocator<>              taa(&ta);

    typedef u::B<t_SUPPRESS_DTOR>                                    TestB;
    typedef bdlma::LocalBufferedObject<TestB, 1024, t_SUPPRESS_DTOR> LBOB;
    typedef bsl::vector<bsltf::MovableAllocTestType>                 Vector;

    Vector v(&sa);    const Vector& V = v;
    v.resize(10);

    LBOB mS(bsl::allocator_arg_t(), taa);    const LBOB& S = mS;

    int multiple = 5;

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    // If we just pass '{ V[0], V[1], ...' etc to the initializer list, they
    // are all copied into it by value using the default allocator, so we
    // explicitly construct the objects using 'sa' to avoid that.

    if (veryVerbose) cout << "10 const objects\n";
    {
        const Int64 numAllocs = da.numAllocations();

        LBOB mX({ MATT(V[0], &sa), MATT(V[1], &sa), MATT(V[2], &sa),
                  MATT(V[3], &sa), MATT(V[4], &sa), MATT(V[5], &sa),
                  MATT(V[6], &sa), MATT(V[7], &sa), MATT(V[8], &sa),
                  MATT(V[9], &sa) });
        const LBOB& X = mX;

        ASSERT(*S == *X);
        ASSERT(X.get_allocator() == daa);

        ASSERTV(da.numAllocations(), numAllocs,
                                             da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERTV(da.numAllocations(), numAllocs,
                                             da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    multiple += 571;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    Int64 daNumAllocs = da.numAllocations();

    if (veryVerbose) cout << "10 const objects + allocator\n";
    {
        const Int64 numAllocs   = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(),
                taa,
                { MATT(V[0], &sa), MATT(V[1], &sa), MATT(V[2], &sa),
                  MATT(V[3], &sa), MATT(V[4], &sa), MATT(V[5], &sa),
                  MATT(V[6], &sa), MATT(V[7], &sa), MATT(V[8], &sa),
                  MATT(V[9], &sa) });
        const LBOB& X = mX;

        ASSERT(*S == *X);
        ASSERT(X.get_allocator() == taa);

        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(daNumAllocs == da.numAllocations());

    multiple += 109;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    daNumAllocs = da.numAllocations();

    if (veryVerbose) cout << "Emplace 10 const objects\n";
    {
        Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(),
                taa,
                V[0], V[1], V[2], V[3], V[4],
                V[5], V[6], V[7], V[8], V[9]);
        const LBOB& X = mX;

        ASSERT(*S == *X);
        ASSERT(X.get_allocator() == taa);

        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);

        multiple += 301;
        mS->setToMultiple(multiple);
        u::setMATTVector(&v, multiple);

        ASSERT(*S != *X);

        numAllocs = ta.numAllocations();

        mX.emplace({ MATT(V[0], &sa), MATT(V[1], &sa), MATT(V[2], &sa),
                     MATT(V[3], &sa), MATT(V[4], &sa), MATT(V[5], &sa),
                     MATT(V[6], &sa), MATT(V[7], &sa), MATT(V[8], &sa),
                     MATT(V[9], &sa) });
        ASSERT(*S == *X);

        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERTV(ta.numAllocations(), numAllocs,
                                             ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(daNumAllocs == da.numAllocations());
}

#endif

template <bool t_SUPPRESS_DTOR>
void testCase4_assignment()
{
    // ------------------------------------------------------------------------
    // TESTING ASSIGNMENT
    //
    // Concerns:
    //: 1 That an object can be assigned to when contained in
    //:   'LocalBufferedObject'.
    //:
    //: 2 That the contained object can be assigned to from either a const
    //:   object or a moved object, with the correct semantics in each case.
    //:
    //: 3 That we can assign an object of the same type of the contained type.
    //:
    //: 4 That we can assign an object of a different type than the contained
    //:   type if it is convertible to the contained type.  We want this tested
    //:   for both a const object and a moved object.
    //:
    //: 5 That destruction can be suppressed, or not, while all this goes on.
    //
    // Plan:
    //: 1 We use types 'u::A' and 'u::B', where 'u::A' has an 'int' value, and
    //:   'u::B' contains a 'bsltf' move-aware type.
    //:   o 'const u::A' can be assigned to 'u::B'
    //:
    //:   o 'u::A&&' can also be assigned to 'u::B', and the move-aware type
    //:     will then be set to "moved into".
    //:
    //: 2 We have type 'LBOB' which is a local buffered objected containing a
    //:   'u::B'.
    //:
    //: 3 Assign a 'const u::B' and a 'u::B&&' to type 'LBOB'.
    //:
    //: 4 Assign a 'const u::A' and a 'u::A&&' to type 'LBOB'.
    //:
    //: 5 Repeat the experiment with 't_DISABLE_DESTRUCTION' template parameter
    //:   of the local buffered object set to both possible values, and verify
    //:   that the expected number of destructions occur.
    //
    // Testing:
    //   ASSIGNMENT
    // ------------------------------------------------------------------------

    if (verbose) cout << "ASSIGN: suppress: " << t_SUPPRESS_DTOR << endl;

    u::DerivedTestAllocator       da("default");
    bsl::allocator<>              daa(&da);
    bslma::DefaultAllocatorGuard  guard(&da);
    bslma::TestAllocator          sa("source allocator");
    bslma::TestAllocator          ta("test");
    bsl::allocator<>              taa(&ta);

    typedef u::B<t_SUPPRESS_DTOR>                                    TestB;
    typedef bdlma::LocalBufferedObject<TestB, 1024, t_SUPPRESS_DTOR> LBOB;

    int multiple = 5;

    LBOB mS(bsl::allocator_arg_t(), taa);    const LBOB& S = mS;
    LBOB mY(bsl::allocator_arg_t(), taa);    const LBOB& Y = mY;

    ++multiple;
    mS->setToMultiple(multiple);
    mY->setToMultiple(multiple);

    if (veryVerbose) cout << "Assign from 'const t_TYPE&'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;
        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX = *Y;
        ASSERT(*S == *Y);
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        ASSERT(MoveState::e_NOT_MOVED == Y->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == Y->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    mY->setToMultiple(multiple);

    if (veryVerbose) cout << "Assign from 't_TYPE&&'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;
        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX = MoveUtil::move(*mY);
        ASSERT(*S != *Y);
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        ASSERT(MoveState::e_MOVED == Y->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == Y->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);

    if (veryVerbose) cout << "Assign from 'const t_ANY_TYPE != t_TYPE&'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        u::A mZ(multiple, &ta);    const u::A& Z = mZ;

        LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;
        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX = Z;
        ASSERT(Z.d_ii == multiple);
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);

    if (veryVerbose) cout << "Assign from 't_ANY_TYPE&& != t_TYPE&&'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        u::A mZ(multiple, &ta);    const u::A& Z = mZ;

        LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;
        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX = MoveUtil::move(mZ);
        ASSERT(Z.d_ii == -1);
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(0 == da.numAllocations());
}

template <bool t_SUPPRESS_DTOR>
void testCase3_emplace()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 That the 'emplace' manipulator can support a type with a complex
    //:   constructor.
    //:
    //: 2 That 'emplace' can handle some arguments being passed const
    //:   references while others are passed rvalue references.
    //:
    //: 3 That 'emplace' can handle a whole contained object passed in via
    //:   const reference.
    //:
    //: 3 That 'emplace' can handle a whole contained object passed in via
    //:   rvalue reference.
    //:
    //: 4 That, when small amounts of memory are used by the contained object,
    //:   it all comes from the arena.
    //:
    //: 5 That 'emplace()' will call the constructor with just the memory
    //:   allocator argument.
    //:
    //: 6 That 'emplace' calls the destructor only when 't_DISABLE_DESTRUCTION'
    //:   is 'false'.
    //
    // Plan:
    // For each of the subtests in this test case we perform the following
    // operations:
    //: o when the vector 'v' is the source, set local buffered
    //:   object and 'mS' to have corresponding values.
    //:
    //: o set 'mX' to some different value
    //:
    //: o perform the emplace
    //:
    //: o check that '*S == *X'
    //:
    //: o verify the move state of the members of '*mX' and the source
    //:
    //: o use 'mX->useMemory' to observe the memory allocation behavior of the
    //: 'mX'.
    //
    // The test case consists of the following numbered subtests:
    //: 1 Default state -- call 'emplace()'
    //:
    //: 2 10 const refs -- pass the 10 elements of the vector 'v' to the 10
    //:   arguments as const refs.
    //:
    //: 3 10 rvalue refs -- move the 10 elements of the vector 'v' to the 10
    //:   arguments as rvalue refs.
    //:
    //: 4 5 const refs, 5 rvalue refs -- pass the 10 elements of the vector 'v'
    //:   to the 10 arguments, with every other argument being a const ref and
    //:   the others being moved as rvalue refs.
    //:
    //: 5 emplace a single 'u::B' const ref
    //:
    //: 6 emplace a single 'u::B' rvalue ref
    //
    // Testing:
    //   EMPLACE
    // ------------------------------------------------------------------------

    if (verbose) cout << "EMPLACE: suppress: " << t_SUPPRESS_DTOR << endl;

    u::DerivedTestAllocator       da("default");
    bsl::allocator<>              daa(&da);
    bslma::DefaultAllocatorGuard  guard(&da);
    bslma::TestAllocator          sa("source allocator");
    bslma::TestAllocator          ta("test");
    bsl::allocator<>              taa(&ta);

    typedef u::B<t_SUPPRESS_DTOR>                                    TestB;
    typedef bdlma::LocalBufferedObject<TestB, 1024, t_SUPPRESS_DTOR> LBOB;
    typedef bsl::vector<bsltf::MovableAllocTestType>                 Vector;

    Vector v(&sa);    const Vector& V = v;
    v.resize(10);

    int multiple = 5;

    LBOB mS(bsl::allocator_arg_t(), taa);    const LBOB& S = mS;
    LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;

    if (veryVerbose) cout << "emplace with no args\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(3);
        ASSERT(*S != *X);

        mX.emplace();
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    if (veryVerbose) cout << "emplace with 10 const refs\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX.emplace(V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8], V[9]);
        ASSERT(*S == *X);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());

        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    if (veryVerbose) cout << "emplace with 10 moved objects\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX.emplace(MoveUtil::move(v[0]),
                   MoveUtil::move(v[1]),
                   MoveUtil::move(v[2]),
                   MoveUtil::move(v[3]),
                   MoveUtil::move(v[4]),
                   MoveUtil::move(v[5]),
                   MoveUtil::move(v[6]),
                   MoveUtil::move(v[7]),
                   MoveUtil::move(v[8]),
                   MoveUtil::move(v[9]));
        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT(MoveState::e_MOVED == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    if (veryVerbose) cout << "emplace with 5 const refs, 5 moved objects\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX.emplace(V[0], MoveUtil::move(v[1]), V[2], MoveUtil::move(v[3]),
                   V[4], MoveUtil::move(v[5]), V[6], MoveUtil::move(v[7]),
                   V[8], MoveUtil::move(v[9]));

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_UNKNOWN == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((ii % 2 ? MoveState::e_MOVED : MoveState::e_NOT_MOVED)
                                                      == (*X)[ii].movedInto());
            ASSERT((ii % 2 ? MoveState::e_MOVED : MoveState::e_NOT_MOVED)
                                                         == V[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    if (veryVerbose) cout << "emplace with 'const u::B&'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX.emplace(*S);

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        ASSERT(*X == *S);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ++multiple;
    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    if (veryVerbose) cout << "emplace with moved 'u::B'\n";
    {
        const Int64 numAllocs = ta.numAllocations();

        mX->setToMultiple(multiple + 5);
        ASSERT(*S != *X);

        mX.emplace(MoveUtil::move(*mS));

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        ASSERT(MoveState::e_MOVED == S->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == S->movedInto());
        mS->setToMultiple(multiple);
        ASSERT(*X == *S);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(0 == da.numAllocations());
}

template <bool t_SUPPRESS_DTOR>
void testCase2_creators()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS, BASIC MANIPULATORS & ACCESSORS
    //
    // Concerns:
    //: 1 That the constructor for a local buffered object can fully support a
    //:   type with a complex constructor.
    //:
    //: 2 That the constructor can handle some arguments being passed const
    //:   references while others are passed rvalue references.
    //:
    //: 3 That the constructor can handle a whole contained object passed in
    //:   via const reference.
    //:
    //: 3 That the constructor can handle a whole contained object passed in
    //:   via rvalue reference.
    //:
    //: 4 That, when small amounts of memory are used by the contained object,
    //:   it all comes from the arena.
    //:
    //: 5 That the allocator passed (or the default allocator if none is
    //:   passed) is used for allocation beyond the arena allocator.
    //:
    //: 6 That when the local buffered object is destroyed, the contained
    //:   object is not destroyed unless 't_DISABLE_DESTRUCTOR' is 'false'.
    //
    // Plan:
    // For each of the subtests in this test case we perform the following
    // operations:
    //: o have the local buffered object 'mS' which is set the desired
    //:   post-construction value
    //:
    //: o construct 'mX'
    //:
    //: o observe the salient value is as expected
    //:
    //: o observe that no memory was allocated outside the arena
    //:
    //: o use 'mX->useMemory' to observe that once the arena is exhausted,
    //:   memory is allocated from the allocator passed at construction
    //
    // The test case consists of the following numbered subtests:
    //: 1 constructor with just the allocator
    //:
    //: 2 default constructor (using default allocator)
    //:
    //: 3 constructor with 10 values passed to the arguments as const refs and
    //:   an allocator
    //:
    //: 4 constructor with 10 values passed to the arguments as const refs and
    //:   no allocator (using default allocator.
    //:
    //: 5 constructor with 10 values moved to the arguments as rvalue refs and
    //:   an allocator
    //:
    //: 6 constructor with 10 values moved to the arguments as rvlaue refs and
    //:   no allocator (using default allocator.
    //:
    //: 7 constructor with 10 values moved to the arguments, every other arg as
    //:   a const ref and the other args moved as rvalue refs and an allocator
    //:
    //: 8 constructor with 10 values moved to the arguments, every other arg as
    //:   a const ref and the other args moved as rvalue refs and no allocator
    //:   (using default allocator)
    //:
    //: 9 copy constructor pass 'const u::B&' and allocator
    //:
    //: 10 copy constructor with 'const u::B&' and no allocator (using default
    //:    allocator)
    //:
    //: 11 move constructor with 'u::B&&' and allocator
    //:
    //: 12 move constructor with 'u::B&&' and no allocator (using default
    //     allocator)
    //
    // Testing:
    //   CONSTRUCTORS
    //   t_TYPE *operator->()
    //   t_TYPE& operator*()
    //   const t_TYPE *operator->() const
    //   const t_TYPE& operator*() const
    //   allocator_type get_allocator() const;
    // ------------------------------------------------------------------------

    u::DerivedTestAllocator       da("default");
    bsl::allocator<>              daa(&da);
    bslma::DefaultAllocatorGuard  guard(&da);
    bslma::TestAllocator          sa("source allocator");
    bslma::TestAllocator          ta("test");
    bsl::allocator<>              taa(&ta);

    typedef u::B<t_SUPPRESS_DTOR>                                    TestB;
    typedef bdlma::LocalBufferedObject<TestB, 1024, t_SUPPRESS_DTOR> LBOB;

    bsl::vector<bsltf::MovableAllocTestType> v(&sa);
    const bsl::vector<bsltf::MovableAllocTestType>& V = v;
    v.resize(10);

    int multiple = 5;

    LBOB mS(bsl::allocator_arg_t(), taa);    const LBOB& S = mS;

    {
        Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(), taa);    const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == 0);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        (*mX).useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        (*mX).useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
        numAllocs = ta.numAllocations();
        (*mX).useMemory(3 * k_DEFAULT_BUFFER_SIZE);
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(0 == da.numAllocations());

    {
        const Int64 numAllocs = da.numAllocations();

        LBOB mX;    const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == 0);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    Int64 daNumAllocs = da.numAllocations();

    mS->setToMultiple(multiple);
    u::setMATTVector(&v, multiple);

    {
        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(),
                taa,
                V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8], V[9]);
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT(MoveState::e_NOT_MOVED == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(da.numAllocations() == daNumAllocs);

    {
        u::setMATTVector(&v, multiple);

        const Int64 numAllocs = da.numAllocations();

        LBOB mX(V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8], V[9]);
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT(MoveState::e_NOT_MOVED == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    daNumAllocs = da.numAllocations();

    {
        u::setMATTVector(&v, multiple);

        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(),
                taa,
                MoveUtil::move(v[0]),
                MoveUtil::move(v[1]),
                MoveUtil::move(v[2]),
                MoveUtil::move(v[3]),
                MoveUtil::move(v[4]),
                MoveUtil::move(v[5]),
                MoveUtil::move(v[6]),
                MoveUtil::move(v[7]),
                MoveUtil::move(v[8]),
                MoveUtil::move(v[9]));
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT(MoveState::e_MOVED == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(da.numAllocations() == daNumAllocs);

    {
        u::setMATTVector(&v, multiple);

        const Int64 numAllocs = da.numAllocations();

        LBOB mX(MoveUtil::move(v[0]),
                MoveUtil::move(v[1]),
                MoveUtil::move(v[2]),
                MoveUtil::move(v[3]),
                MoveUtil::move(v[4]),
                MoveUtil::move(v[5]),
                MoveUtil::move(v[6]),
                MoveUtil::move(v[7]),
                MoveUtil::move(v[8]),
                MoveUtil::move(v[9]));
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT(MoveState::e_MOVED == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    daNumAllocs = da.numAllocations();

    {
        u::setMATTVector(&v, multiple);

        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(),
                taa,
                v[0], MoveUtil::move(v[1]), v[2], MoveUtil::move(v[3]),
                v[4], MoveUtil::move(v[5]), v[6], MoveUtil::move(v[7]),
                v[8], MoveUtil::move(v[9]));
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_UNKNOWN == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT((ii % 2 ? MoveState::e_MOVED :  MoveState::e_NOT_MOVED)
                                                      == (*X)[ii].movedInto());
            ASSERT((ii % 2 ? MoveState::e_MOVED :  MoveState::e_NOT_MOVED)
                                                         == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(da.numAllocations() == daNumAllocs);

    {
        u::setMATTVector(&v, multiple);

        const Int64 numAllocs = da.numAllocations();

        LBOB mX(V[0], MoveUtil::move(v[1]), V[2], MoveUtil::move(v[3]),
                V[4], MoveUtil::move(v[5]), V[6], MoveUtil::move(v[7]),
                V[8], MoveUtil::move(v[9]));
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_UNKNOWN == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == ii * multiple);
            ASSERT((ii % 2 ? MoveState::e_MOVED :  MoveState::e_NOT_MOVED)
                                                      == (*X)[ii].movedInto());
            ASSERT((ii % 2 ? MoveState::e_MOVED :  MoveState::e_NOT_MOVED)
                                                         == v[ii].movedFrom());
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    daNumAllocs = da.numAllocations();

    {
        mS->setToMultiple(multiple);

        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(), taa, *S);    const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == multiple * ii);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(da.numAllocations() == daNumAllocs);

    {
        mS->setToMultiple(multiple);

        const Int64 numAllocs = da.numAllocations();

        LBOB mX(*S);    const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_NOT_MOVED == X->movedInto());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == multiple * ii);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }

    daNumAllocs = da.numAllocations();

    {
        mS->setToMultiple(multiple);
        ASSERT(MoveState::e_NOT_MOVED == S->movedInto());
        ASSERT(MoveState::e_NOT_MOVED == S->movedFrom());

        const Int64 numAllocs = ta.numAllocations();

        LBOB mX(bsl::allocator_arg_t(), taa, MoveUtil::move(*mS));
        const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        ASSERT(MoveState::e_NOT_MOVED == S->movedInto());
        ASSERT(MoveState::e_MOVED == S->movedFrom());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == multiple * ii);
            (*mS)[ii].setData(multiple * ii);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == taa);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(ta.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(ta.numAllocations(), numAllocs,
                                              ta.numAllocations() > numAllocs);
    }

    ASSERT(da.numAllocations() == daNumAllocs);

    {
        mS->setToMultiple(multiple);
        ASSERT(MoveState::e_NOT_MOVED == S->movedInto());
        ASSERT(MoveState::e_NOT_MOVED == S->movedFrom());

        const Int64 numAllocs = da.numAllocations();

        LBOB mX(MoveUtil::move(*mS));    const LBOB& X = mX;

        ASSERT(MoveState::e_NOT_MOVED == X->movedFrom());
        ASSERT(MoveState::e_MOVED == X->movedInto());
        ASSERT(MoveState::e_NOT_MOVED == S->movedInto());
        ASSERT(MoveState::e_MOVED == S->movedFrom());
        for (int ii = 0; ii < TestB::e_NUM_MEMBERS; ++ii) {
            ASSERT((*X)[ii].data() == multiple * ii);
            (*mS)[ii].setData(multiple * ii);
        }
        ASSERT(*X == *S);
        ASSERT(X.get_allocator() == daa);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory(3 * k_DEFAULT_BUFFER_SIZE / 4);
        ASSERT(da.numAllocations() == numAllocs);
        mX->useMemory();
        ASSERTV(da.numAllocations(), numAllocs,
                                              da.numAllocations() > numAllocs);
    }
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;            (void) verbose;
    veryVerbose = argc > 3;        (void) veryVerbose;
    veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        bslma::TestAllocator sa;

        const Usage::NameGenerator ng(2560, &sa);

        ng.assessNames();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST
        //
        // See 'TC::testCase' for test documentation.
        // --------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        if (verbose) cout << "C++03 -- Initializer List Test Skipped.\n"
                             "=======================================\n";
#else
        if (verbose) cout << "Initializer List Test\n"
                             "=====================\n";

        testCase5_initializerLists<false>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          5 == u::B<false>::s_numDestructions);
        testCase5_initializerLists<true>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          0 == u::B<true >::s_numDestructions);
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // See 'TC::testCase' for test documentation.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Assignment\n"
                             "==================\n";

        testCase4_assignment<false>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          6 == u::B<false>::s_numDestructions);
        testCase4_assignment<true>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          0 == u::B<true >::s_numDestructions);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE
        //
        // See 'TC::testCase' for test documentation.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Emplace\n"
                             "===============\n";

        testCase3_emplace<false>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          8 == u::B<false>::s_numDestructions);
        testCase3_emplace<true>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          0 == u::B<true >::s_numDestructions);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // See 'TC::testCase' for test documentation.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Constructors\n"
                             "====================\n";

        testCase2_creators<false>();
        ASSERTV(u::B<false>::s_numDestructions,
                                         13 == u::B<false>::s_numDestructions);
        testCase2_creators<true>();
        ASSERTV(u::B<false>::s_numDestructions,
                                          0 == u::B<true >::s_numDestructions);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        bslma::TestAllocator         da, ta;
        bsl::allocator<>             taa(&ta);
        bslma::DefaultAllocatorGuard guard(&da);

        bdlma::LocalBufferedObject<u::A> mX(5);

        ASSERT(5 == mX->value());
        ASSERT(da.numAllocations() == 0);

        bdlma::LocalBufferedObject<u::A> mXB;
        mXB->value() = mX->value();
        ASSERT(da.numAllocations() == 0);

        bdlma::LocalBufferedObject<bsl::vector<int> > mV;
        mV->reserve(100);
        for (int ii = 0; ii < 100; ++ii) {
            mV->push_back(ii);
        }

        for (int ii = 0; ii < 100; ++ii) {
            mV->at(ii) *= 3;
        }
        for (int ii = 0; ii < 100; ++ii) {
            ASSERT((*mV)[ii] == 3 * ii);
        }
        ASSERT(da.numAllocations() == 0);

        bdlma::LocalBufferedObject<bsl::deque<int> > mD(mV->begin(),
                                                        mV->end());
        for (int ii = 0; ii < 100; ++ii) {
            ASSERT((*mV)[ii] == mD->at(ii));
        }
        ASSERT(da.numAllocations() == 0);

        for (int ii = 0; ii < 100; ++ii) {
            (*mV)[ii] *= 10;
        }
        for (int ii = 0; ii < 100; ++ii) {
            ASSERT(30 * ii == (*mV)[ii]);
            ASSERT(10 * (*mD)[ii] == (*mV)[ii]);
        }

        mD.emplace(mV->begin(), mV->end());

        for (int ii = 0; ii < 100; ++ii) {
            ASSERT(30 * ii == (*mD)[ii]);
            ASSERT((*mD)[ii] == (*mV)[ii]);
        }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        bdlma::LocalBufferedObject<bsl::deque<int>, 1024 > mD2({ 5, 10, 15 });
        for (int ii = 0; ii < 3; ++ii) {
            ASSERT((ii + 1) * 5 == mD2->at(ii));
        }
#endif
        ASSERT(da.numAllocations() == 0);

        mV->reserve(10 * 1024);

        const Int64 afterReserve = da.numAllocations();
        ASSERT(afterReserve > 0);

        bdlma::LocalBufferedObject<bsl::list<int>, 8192> mL(
                                                        bsl::allocator_arg_t(),
                                                        taa,
                                                        mD->begin(),
                                                        mD->end());
        ASSERTV(ta.numBytesInUse(), ta.numAllocations() == 0);

        bdlma::LocalBufferedObject<bsl::vector<int> > mV2(
                                                        bsl::allocator_arg_t(),
                                                        taa,
                                                        mL->begin(),
                                                        mL->end());
        for (int ii = 0; ii < 100; ++ii) {
            ASSERT(30 * ii == mV2->at(ii));
        }
        ASSERT(ta.numAllocations() == 0);

        for (int jj = 0; jj < 20; ++jj) {
            mV2->insert(mV2->end(), mL->begin(), mL->end());
        }

        const Int64 afterStuff = ta.numAllocations();

        ASSERT(afterStuff > 0);
        ASSERT(da.numAllocations() == afterReserve);

        bdlma::LocalBufferedObject<bsl::vector<int> > mV3(
                                                  bsl::allocator_arg_t(), taa);

        mV3 = *mV;

        ASSERT(ta.numAllocations() == afterStuff);
        ASSERT(da.numAllocations() == afterReserve);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
