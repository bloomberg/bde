// bslmf_movableref.t.cpp                                             -*-C++-*-
#include <bslmf_movableref.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addvolatile.h>
#include <bslmf_isclass.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <new>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ----------------------------------------------------------------------------
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// ----------------------------------------------------------------------------
// [ 4] MovableRef<TYPE>::operator TYPE&() const;
// [ 5] TYPE& access(TYPE& lvalue);
// [ 5] TYPE& access(MovableRef<TYPE>& lvalue);
// [ 3] MovableRef<TYPE> move(TYPE& lvalue);
// [ 3] MovableRef<remove_reference<T>::type> move(MovableRef<T> ref);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] MovableRef<TYPE>
// [ 6] MovableRef<TYPE> VS. RVALUE
// [ 7] EXTENDING 'bsl::is_nothrow_move_constructible'
// [ 8] USAGE EXAMPLE

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

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

template <class TYPE>
class vector {

    // DATA
    TYPE *d_begin;
    TYPE *d_end;
    TYPE *d_endBuffer;

    // PRIVATE CLASS METHODS
    static void swap(TYPE*& a, TYPE*& b);
        // Swap the specified pointers 'a' and 'b'.

  public:
    vector();
        // Create an empty vector.

    vector(bslmf::MovableRef<vector> other);                        // IMPLICIT
        // Create a vector by transfering the content of the specified 'other'.

    vector(const vector& other);
        // Create a vector by copying the content of the specified 'other'.

    vector& operator=(vector other);
        // Assign a vector by copying the content of the specified 'other' and
        // return a reference to this object.  Note that 'other' is passed by
        // value to have the copy or move already be done, or even elided.
        // Within the body of the assignment operator the content of 'this' and
        // 'other' are simply swapped.

    ~vector();
        // Destroy the vector's elements and release any allocated memory.

    TYPE&       operator[](int index)       { return this->d_begin[index]; }
        // Return a reference to the object at the specified 'index'.

    const TYPE& operator[](int index) const { return this->d_begin[index]; }
        // Return a reference to the object at the specified 'index'.

    TYPE       *begin()       { return this->d_begin; }
        // Return a pointer to the first element.

    const TYPE *begin() const { return this->d_begin; }
        // Return a pointer to the first element.

    int capacity() const { return int(this->d_endBuffer - this->d_begin); }
        // Return the capacity of the vector.

    bool empty() const { return this->d_begin == this->d_end; }
        // Return 'true' if the vector is empty and 'false' otherwise.

    TYPE       *end()       { return this->d_end; }
        // Return a pointer to the end of the range.

    const TYPE *end() const { return this->d_end; }
        // Return a pointer to the end of the range.

    void push_back(const TYPE& value);
        // Append a copy of the specified 'value' to the vector.

    void push_back(bslmf::MovableRef<TYPE> value);
        // Append an object moving the specified 'value' to the new location.

    void reserve(int newCapacity);
        // Reserve enough capacity to fit at least as many elements as the
        // specified 'newCapacity'.

    int size() const { return int(this->d_end - this->d_begin); }
        // Return the size of the object.

    void swap(vector& other);
        // Swap the content of the vector with the specified 'other'.
};

template <class TYPE>
vector<TYPE>::vector()
: d_begin()
, d_end()
, d_endBuffer()
{
}

template <class TYPE>
vector<TYPE>::vector(const vector& other)
: d_begin()
, d_end()
, d_endBuffer()
{
    if (!other.empty()) {
        this->reserve(4 < other.size()? other.size(): 4);

        ASSERT(other.size() <= this->capacity());
        for (TYPE* it = other.d_begin; it != other.d_end; ++it) {
            new (this->d_end) TYPE(*it);
            ++this->d_end;
        }
    }
}

template <class TYPE>
vector<TYPE>::vector(bslmf::MovableRef<vector> other)
: d_begin(bslmf::MovableRefUtil::access(other).d_begin)
, d_end(bslmf::MovableRefUtil::access(other).d_end)
, d_endBuffer(bslmf::MovableRefUtil::access(other).d_endBuffer)
{
    vector& reference(other);
    reference.d_begin = 0;
    reference.d_end = 0;
    reference.d_endBuffer = 0;
}

template <class TYPE>
vector<TYPE>::~vector()
{
    if (this->d_begin) {
        while (this->d_begin != this->d_end) {
            --this->d_end;
            this->d_end->~TYPE();
        }
        operator delete(this->d_begin);
    }
}

template <class TYPE>
vector<TYPE>& vector<TYPE>::operator=(vector other)
{
    this->swap(other);
    return *this;
}

template <class TYPE>
void vector<TYPE>::push_back(const TYPE& value)
{
    if (this->d_end == this->d_endBuffer) {
        this->reserve(this->size()? int(1.5 * this->size()): 4);
    }
    ASSERT(this->d_end != this->d_endBuffer);
    new(this->d_end) TYPE(value);
    ++this->d_end;
}

template <class TYPE>
void vector<TYPE>::push_back(bslmf::MovableRef<TYPE> value)
{
    if (this->d_end == this->d_endBuffer) {
        this->reserve(this->size()? int(1.5 * this->size()): 4);
    }
    ASSERT(this->d_end != this->d_endBuffer);
    new(this->d_end) TYPE(bslmf::MovableRefUtil::move(value));
    ++this->d_end;
}

template <class TYPE>
void vector<TYPE>::reserve(int newCapacity)
{
    if (this->capacity() < newCapacity) {
        vector tmp;
        int    size = int(sizeof(TYPE) * newCapacity);
        tmp.d_begin = static_cast<TYPE*>(operator new(size));
        tmp.d_end = tmp.d_begin;
        tmp.d_endBuffer = tmp.d_begin + newCapacity;

        for (TYPE* it = this->d_begin; it != this->d_end; ++it) {
            new (tmp.d_end) TYPE(*it);
            ++tmp.d_end;
        }
        this->swap(tmp);
    }
}

template <class TYPE>
void vector<TYPE>::swap(TYPE*& a, TYPE*& b)
{
    TYPE *tmp = a;
    a = b;
    b = tmp;
}

template <class TYPE>
void vector<TYPE>::swap(vector& other)
{
    this->swap(this->d_begin, other.d_begin);
    this->swap(this->d_end, other.d_end);
    this->swap(this->d_endBuffer, other.d_endBuffer);
}

}  // close unnamed namespace

//=============================================================================

namespace {

template <class TYPE>
struct TestMovableRefArgument
{
    static bool test(TYPE&) { return false; }
        // Return 'false', indicating that the argument is not an r-value
        // reference.

    static bool test(const TYPE&) { return false; }
        // Return 'false', indicating that the argument is not an r-value
        // reference.

    static bool test(bslmf::MovableRef<TYPE>) { return true; }
        // Return 'true', indicating that the argument is an r-value reference.
};

template <class TYPE>
struct MovableAddress
{
    static TYPE* get(bslmf::MovableRef<TYPE> movable)
        // Return a pointer to object referenced by the specified 'movable'.
    {
        TYPE& reference(bslmf::MovableRefUtil::access(movable));
        return &reference;
    }
};

class TestMoving {
    // This class is used to test some move operations.  It deliberately hides
    // the assignment operator, and poisons the address-of operator.

    // DATA
    int *d_pointer;

  private:
    // NOT IMPLEMENTED
    void operator= (TestMoving&); // = delete;

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
  public:
#endif
    TestMoving(const TestMoving& other)
        // Create a 'TestMoving' object that is a copy of the specified
        // 'other'.  In this case copying 'other' means allocating a new 'int'
        // with a new unique pointer.
        : d_pointer(new int(1 + *other.d_pointer)) {
    }

  public:
    TestMoving(): d_pointer(new int(0)) {}
        // Create a 'TestMoving' object using the default constructor.  The
        // object is initialized with a unique pointer to 'int'.

    explicit TestMoving(bslmf::MovableRef<TestMoving> rvalue)
        // Create a 'TestMoving' object that moves the value referenced by the
        // specified 'rvalue'.
        : d_pointer(bslmf::MovableRefUtil::access(rvalue).d_pointer) {
        bslmf::MovableRefUtil::access(rvalue).d_pointer = 0;
    }

    ~TestMoving() { delete this->d_pointer; }
        // Destroy this object. Delete the allocated pointer.

    void operator&() const {}
        // Do nothing.  The address-of operator gets in the way.

    TestMoving *getAddress() { return this; }
        // Return the address of this object.

    const int  *getPointer() const { return this->d_pointer; }
        // Return the held pointer.
};

// The following test types and the macros defined further below were
// originally in the 'bslmf_isnothrowmoveconstructible' test driver (to verify
// that the 'bsl::is_nothrow_move_constructible' meta-function can be correctly
// extended), but were moved here to eliminate a cycle, i.e., a direct cycle
// between that component and this one.  Note that 'bslmf::MovableRef' must be
// used to expose extensions of 'bsl::is_nothrow_move_constructible' to C++03
// environments.  Testing of such extensions is done in case 7.

struct UserDefinedNothrowTestType {
    // This user-defined type, which is marked to have a nothrow move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    UserDefinedNothrowTestType() {}
    UserDefinedNothrowTestType(const UserDefinedNothrowTestType& original)
    {
        (void)original;
    }
    UserDefinedNothrowTestType(bslmf::MovableRef<UserDefinedNothrowTestType>)
                                                BSLS_CPP11_NOEXCEPT // IMPLICIT
    {}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UserDefinedNothrowTestType2 {
    // This user-defined type, which is marked to have a 'nothrow' move
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedNothrowTestType2,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    UserDefinedNothrowTestType2() {}
    UserDefinedNothrowTestType2(const UserDefinedNothrowTestType2& original)
    {
        (void)original;
    }
    UserDefinedNothrowTestType2(bslmf::MovableRef<UserDefinedNothrowTestType2>)
                                                BSLS_CPP11_NOEXCEPT // IMPLICIT
    {}
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UserDefinedThrowTestType {
    // This user-defined type, which is not marked to be 'nothrow' move
    // constructible, is used for testing.

    // CREATORS
    UserDefinedThrowTestType() {}
    UserDefinedThrowTestType(
                       bslmf::MovableRef<UserDefinedThrowTestType>) // IMPLICIT
    {}
    UserDefinedThrowTestType(const UserDefinedThrowTestType& original)
    {
        (void)original;
    }
};

}  // close unnamed namespace

namespace bsl {

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
template <>
struct is_nothrow_move_constructible<UserDefinedNothrowTestType>
            : bsl::true_type { };
#endif

}  // close namespace bsl

//=============================================================================
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// These macros were copied from the 'bslmf_isnothrowmoveconstructible' test
// driver.

#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT)               \
    ASSERT( bsl::is_nothrow_move_constructible<TYPE>::value == RESULT);       \
    ASSERT( bsl::is_nothrow_move_constructible<                               \
                                       bsl::add_pointer<TYPE>::type>::value); \
    ASSERT( bsl::is_nothrow_move_constructible<                               \
                    bsl::add_lvalue_reference<TYPE>::type>::value);

#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)            \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT);                  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(                                \
                                      bsl::add_cv<TYPE>::type, RESULT);

// Two additional macros will allow testing on old MSVC compilers when 'TYPE'
// is an array of unknown bound.

#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT)        \
    ASSERT( bsl::is_nothrow_move_constructible<TYPE>::value == RESULT);       \
    ASSERT( bsl::is_nothrow_move_constructible<                               \
                                         bsl::add_pointer<TYPE>::type>::value);

#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE, RESULT)     \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT);           \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_NO_REF(                         \
                                         bsl::add_const<TYPE>::type, RESULT); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_NO_REF(                         \
                                      bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE_NO_REF(                         \
                                      bsl::add_cv<TYPE>::type, RESULT);

#if defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)           \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)

#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
// Old Microsoft compilers do not support references to arrays of unknown
// bound.

# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)           \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)         \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[], RESULT)       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[][8], RESULT)

#else
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)                \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)           \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)         \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[], RESULT)              \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TYPE(TYPE[][8], RESULT)
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 8: {
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
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        vector<int> vector0;
        for (int i = 0; i != 5; ++i) {
            vector0.push_back(i);
        }
        for (int i = 0; i != 5; ++i) {
            ASSERT(vector0[i] == i);
        }

        // To verify that copying of 'vector<TYPE>' objects works, a copy is
        // created:

        vector<int> vector1(vector0);
        ASSERT(vector1.size() == 5);
        ASSERT(vector1.size() == vector0.size());
        for (int i = 0; i != vector1.size(); ++i) {
            ASSERT(vector1[i] == i);
            ASSERT(vector1[i] == vector0[i]);
        }

        // When using moving this 'vector0' to a new location the
        // representation of the new object should use the original 'begin()':

        const int *first = vector0.begin();
        vector<int> vector2(bslmf::MovableRefUtil::move(vector0));
        ASSERT(first == vector2.begin());

        // When create a 'vector<vector<int> >' and using 'push_back()' on
        // this object with 'vector2' a copy should be inserted:

        vector<vector<int> > vvector;
        vvector.push_back(vector2);                          // copy
        ASSERT(vector2.size() == 5);
        ASSERT(vvector.size() == 1);
        ASSERT(vvector[0].size() == vector2.size());
        ASSERT(vvector[0].begin() != first);
        for (int i = 0; i != 5; ++i) {
            ASSERT(vvector[0][i] == i);
            ASSERT(vector2[i] == i);
        }

        // When adding another element by moving 'vector2' the 'begin()' of the
        // newly inserted element will be the same as 'first', i.e., the
        // representation is transferred:

        vvector.push_back(bslmf::MovableRefUtil::move(vector2)); // move
        ASSERT(vvector.size() == 2);
        ASSERT(vvector[1].begin() == first);
        ASSERT(vvector[1].size() == 5);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // EXTENDING 'bsl::is_nothrow_move_constructible'
        //   Ensure the 'bsl::is_nothrow_move_constructible' meta-function
        //   returns the correct value for types explicitly specified to have
        //   a 'nothrow' move constructor.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for normal user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bsl::is_nothrow_move_constructible' on that
        //    type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 For cv-qualified types, the meta-function returns 'true' if the
        //:   corresponding cv-unqualified type is 'nothrow' move constructible
        //    and 'false' otherwise.
        //:
        //: 5 For array types, the meta-function returns 'true' if the array
        //:   element is 'nothrow' move constructible, and 'false' otherwise.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C-4..5)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-3, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   EXTENDING 'bsl::is_nothrow_move_constructible'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nEXTENDING 'bsl::is_nothrow_move_constructible'"
                   "\n==============================================\n");

        // C-1
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                                 UserDefinedThrowTestType,
                                                 false);

        // C-2
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                                 UserDefinedNothrowTestType,
                                                 true);

        // C-3
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_OBJECT_TYPE(
                                                 UserDefinedNothrowTestType2,
                                                 true);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MOVABLEREF<TYPE> VS. RVALUE
        //
        // Concerns:
        //: 1 A function declared to take a 'MovableRef<TYPE>' as argument can
        //:   be called with a temporary of type 'TYPE' when using a C++11
        //:   implementation.
        //: 2 A function can be overloaded for 'const TYPE&', 'TYPE&', and
        //:   'MovableRef<TYPE>' and be called appropriately.
        //
        // Plan:
        //: 1 Call an overloaded function taking a 'MovableRef<TYPE>' argument
        //:   as well as a 'TYPE&' and a 'const TYPE&' and verify that the
        //:   correct overload is called.  For a C++11 implementation the
        //:   overload taking a 'MovableRef<TYPE>' has to be called otherwise
        //:   one of the two lvalue overloads is called.
        //: 2 Calling the overloaded function from the previous item and
        //:   confirming the result also addresses this concern.
        //
        // Testing:
        //   bslmf::MovableRef<TYPE> VS. RVALUE
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVABLEREF<TYPE> VS. RVALUE"
                            "\n===========================\n");

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        ASSERT(TestMovableRefArgument<int>::test(int(18)));
        ASSERT(TestMovableRefArgument<vector<int> >::test(vector<int>()));
        ASSERT(TestMovableRefArgument<TestMoving>::test(TestMoving()));
#else
        ASSERT(!TestMovableRefArgument<int>::test(int(18)));
        ASSERT(!TestMovableRefArgument<vector<int> >::test(vector<int>()));
        ASSERT(!TestMovableRefArgument<TestMoving>::test(TestMoving()));
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'MovableRefUtil::access'
        //
        // Concerns:
        //: 1 'MovableRefUtil::access()' yields an lvalue reference the object
        //:   referenced by an lvalue or a 'MovableRef<TYPE>'.
        //
        // Plan:
        //: 1 Call a function taking a 'MovableRef<TYPE>' argument and verify
        //:   that the result of 'MovableRefUtil::access()' can be bound to an
        //:   lvalue reference with the same address as the original object.
        //
        // Testing:
        //   TYPE& access(TYPE& lvalue);
        //   TYPE& access(MovableRef<TYPE>& lvalue);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'MovableRefUtil::access'"
                            "\n================================\n");
        {
            int  value(19);
            int *address(MovableAddress<int>::get(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(&value == address);
            ASSERT(&value == &bslmf::MovableRefUtil::access(value));
        }
        {
            vector<int>  value;
            vector<int> *address(MovableAddress<vector<int> >::get(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(&value == address);
            ASSERT(&value == &bslmf::MovableRefUtil::access(value));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'MovableRef<TYPE>::operator TYPE&'
        //
        // Concerns:
        //: 1 'MovableRef<TYPE>' converts to an lvalue reference of type
        //:   'TYPE&' and that the address of the referenced object is
        //:    identical to the address of the original object.
        //
        // Plan:
        //: 1 Use 'MovableRefUtil::move()' with an lvalue, initialize a
        //:   'MovableRef<T>', convert the result to an lvalue reference, and
        //:   check that the addresses are identical.
        //
        // Testing:
        //   MovableRef<TYPE>::operator TYPE&() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'MovableRef<TYPE>::operator TYPE&'"
                            "\n==========================================\n");

        {
            int                    value(17);
            bslmf::MovableRef<int> rvalue(bslmf::MovableRefUtil::move(value));
            int&                   lvalue(rvalue);
            ASSERT(&value == &lvalue);
        }
        {
            vector<int>                     value;
            bslmf::MovableRef<vector<int> > rvalue(
                                           bslmf::MovableRefUtil::move(value));
            vector<int>&                    lvalue(rvalue);
            ASSERT(&value == &lvalue);
        }
        {
            TestMoving                    value;
            bslmf::MovableRef<TestMoving> rvalue(
                                           bslmf::MovableRefUtil::move(value));
            TestMoving&                   lvalue(rvalue);
            ASSERT(value.getAddress() == lvalue.getAddress());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'MovableRefUtil::move'
        //
        // Concerns:
        //: 1 'MovableRefUtil::move()' produces a result that can be used to
        //:   initialize a 'MovableRef<T>'.
        //: 2 'MovableRefUtil::move()' can be used with an argument of a type
        //:   with an overloaded address-of operator.
        //: 3 Moving a 'MovableRef<T>' results in an object that can be bound
        //:   to a 'MovableRef<T>'.
        //: 4 When compiling with a C++11 implementation an rvalue can be bound
        //:   to a 'MovableRef<T>'.
        //
        // Plan:
        //: 1 Use 'MovableRefUtil::move()' with an lvalue and initialize a
        //:   'MovableRef<T>'.  Also, call a function overloaded on a 'T&' and
        //:    a 'MovableRef<T>'.
        //: 2 Use 'MovableRefUtil::move()' with an object of a type with an
        //:   overloaded address-of operator.
        //: 3 Use 'MovableRefUtil::move()' on the result of
        //:   'MovableRefUtil::move()'.
        //: 4 Call the test function with an rvalue when compiling with a C++11
        //:   implementation.
        //
        // Testing:
        //   MovableRef<TYPE> move(TYPE& lvalue);
        //   MovableRef<remove_reference<T>::type> move(MovableRef<T> ref);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'MovableRefUtil::move'"
                            "\n==============================\n");

        {
            int                    value(17);
            bslmf::MovableRef<int> rvalue1(bslmf::MovableRefUtil::move(value));
            ASSERT(!TestMovableRefArgument<int>::test(value));
            ASSERT(TestMovableRefArgument<int>::test(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(value == rvalue1);

            bslmf::MovableRef<int> rvalue2(
              bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::move(value)));
            ASSERT(value == rvalue2);
        }
        {
            vector<int>            value;
            ASSERT(TestMovableRefArgument<vector<int> >::test(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(TestMovableRefArgument<vector<int> >::test(
             bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::move(value))));
        }
        {
            TestMoving            value;
            ASSERT(TestMovableRefArgument<TestMoving>::test(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(TestMovableRefArgument<TestMoving>::test(
             bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::move(value))));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MOVABLEREF<TYPE>
        //
        // Concerns:
        //: 1 'MovableRef<TYPE>' exists and in case of using a C++11
        //:   implementation is an alias for 'TYPE&&'.
        //
        // Plan:
        //: 1 Use 'bsl::is_same<...>' to make sure 'MovableRef<TYPE>' refers
        //:   the expected type.
        //
        // Testing:
        //   MovableRef<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVABLEREF<TYPE>"
                            "\n================\n");

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        ASSERT((bsl::is_same<bslmf::MovableRef<int>, int&&>::value));
        ASSERT((bsl::is_same<bslmf::MovableRef<vector<int> >,
                                                       vector<int>&&>::value));
        ASSERT((bsl::is_same<bslmf::MovableRef<TestMoving>,
                                                        TestMoving&&>::value));
#else
        ASSERT(bsl::is_class<bslmf::MovableRef<int> >::value);
        ASSERT(bsl::is_class<bslmf::MovableRef<vector<int> > >::value);
        ASSERT(bsl::is_class<bslmf::MovableRef<TestMoving> >::value);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Verify that all operations of 'MovableRef<TYPE>' and
        //:   'MovableRefUtil' can be used.
        //
        // Plan:
        //: 1 Use 'MovableRefUtil::move()' to create an 'MovableRef<int>' and
        //:   use the implicit conversion to 'int&' and
        //:   'MovableRefUtil::access()' to obtain a reference to the original
        //:   value.
        //
        // Testing:
        //     BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        typedef bslmf::MovableRef_Helper<int>::type MovableInt;
        ASSERT((bsl::is_same<int&&, MovableInt>::value));
#endif

        int                    value(0);
        bslmf::MovableRef<int> rvalue(bslmf::MovableRefUtil::move(value));
        int&                   reference(rvalue);
        int&                   lvalue0(bslmf::MovableRefUtil::access(rvalue));
        int&                   lvalue1(bslmf::MovableRefUtil::access(value));
        ASSERT(&reference == &lvalue0);
        ASSERT(&reference == &lvalue1);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
