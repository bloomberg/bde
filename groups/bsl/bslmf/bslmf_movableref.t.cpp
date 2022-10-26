// bslmf_movableref.t.cpp                                             -*-C++-*-
#include <bslmf_movableref.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_isarray.h>  // MSVC workaround, pre-VC 2017
#include <bslmf_isclass.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_voidtype.h>

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
// [ 6] MovableRef<TYPE>::operator TYPE&() const;
// [ 7] TYPE& MovableRefUtil::access(TYPE& lvalue);
// [ 7] TYPE& MovableRefUtil::access(MovableRef<TYPE> lvalue);
// [ 5] MovableRef<TYPE> MovableRefUtil::move(TYPE& lvalue);
// [ 5] MovableRef<RemoveRef<T>::type> MovableRefUtil::move(MovableRef<T> ref);
// [  ] enable_if<true> move_if_noexcept(TYPE& lvalue);
// [  ] enable_if<false> move_if_noexcept(TYPE& lvalue);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] MovableRef<TYPE>
// [ 3] MovableRefUtil::IsLvalueReference<TYPE>
// [ 3] MovableRefUtil::IsMovableReference<TYPE>
// [ 3] MovableRefUtil::IsReference<TYPE>
// [ 4] MovableRefUtil::RemoveReference<TYPE>
// [ 4] MovableRefUtil::AddLvalueReference<TYPE>
// [ 4] MovableRefUtil::AddMovableReference<TYPE>
// [ 4] MovableRefUtil::Decay<TYPE>
// [ 8] MovableRef<TYPE> VS. RVALUE
// [ 9] EXTENDING 'bsl::is_nothrow_move_constructible'
// [10] CONCERN: IBM XL C++ functions with default arguments workaround
// [11] BSLMF_MOVABLEREF_DEDUCE
// [12] USAGE EXAMPLE

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                      DEFECT DETECTION MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER) &&                   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
#define BSLMF_MOVABLEREF_USE_NATIVE_ORACLE 1
// 'std::is_nothrow_move_constructible' is available as a trusted
// oracle of the correct value for the 'bsl::is_nothrow_move_constructible'
// trait.  Note that MSVC 2013 provides the 'nothrow' traits, but does not
// support the 'noexcept' operator so the traits erroneously return 'false'
// in many cases.
#endif

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// There are two sides of move semantics:
//
//: 1 Classes or class templates that are _move-enabled_, i.e., which can
//:   transfer their internal representation to another object in some
//:   situations.  To become move-enabled a class needs to implement, at
//:   least, a move constructor.  It should probably also implement a move
//:   assignment operator.
//: 2 Users of a potentially move-enabled class may take advantage of moving
//:   objects by explicitly indicating that ownership of resources may be
//:   transferred.  When using C++11 the compiler can automatically detect
//:   some situations where it is safe to move objects but this features is
//:   not available with C++03.
//
// The usage example below demonstrate both use cases using a simplified
// version of 'std::Vector<T>'.  The class template is simplified to
// concentrate on the aspects relevant to 'bslmf::MovableRef<T>'.  Most of the
// operations are just normal implementations to create a container.  The last
// two operations described are using move operations.
//
// Assume we want to implement a class template similar to the standard library
// 'vector' facility.  First we declare the class template 'Vector<TYPE>'.  The
// definition of the this class template is rather straight forward, and for
// simplicity a few trivial operations are implemented directly in the class
// definition:
//..
    template <class TYPE>
    class Vector
    {
        TYPE *d_begin;
        TYPE *d_end;
        TYPE *d_endBuffer;

        static void swap(TYPE*& a, TYPE*& b);
            // Swap the specified pointers 'a' and 'b'.

      public:
        Vector();
            // Create an empty Vector.

        Vector(bslmf::MovableRef<Vector> other);                    // IMPLICIT
            // Create a Vector by transferring the content of the specified
            // 'other'.

        Vector(const Vector& other);
            // Create a Vector by copying the content of the specified 'other'.

        Vector& operator= (Vector other);
            // Assign a Vector by copying the content of the specified 'other'
            // and return a reference to this object.  Note that 'other' is
            // passed by value to have the copy or move already be done, or
            // even elided.  Within the body of the assignment operator the
            // content of 'this' and 'other' are simply swapped.

        ~Vector();
            // Destroy the Vector's elements and release any allocated memory.

        TYPE&       operator[](int index)      { return this->d_begin[index]; }
            // Return a reference to the object at the specified 'index'.

        const TYPE& operator[](int index) const{ return this->d_begin[index]; }
            // Return a reference to the object at the specified 'index'.

        TYPE       *begin()       { return this->d_begin; }
            // Return a pointer to the first element.

        const TYPE *begin() const { return this->d_begin; }
            // Return a pointer to the first element.

        int capacity() const { return int(this->d_endBuffer - this->d_begin); }
            // Return the capacity of the Vector.

        bool empty() const { return this->d_begin == this->d_end; }
            // Return 'true' if the Vector is empty and 'false' otherwise.

        TYPE       *end()       { return this->d_end; }
            // Return a pointer to the end of the range.

        const TYPE *end() const { return this->d_end; }
            // Return a pointer to the end of the range.

        void push_back(const TYPE& value);
            // Append a copy of the specified 'value' to the Vector.

        void push_back(bslmf::MovableRef<TYPE> value);
            // Append an object moving the specified 'value' to the new
            // location.

        void reserve(int newCapacity);
            // Reserve enough capacity to fit at least as many elements as
            // specified by 'newCapacity'.

        int size() const { return int(this->d_end - this->d_begin); }
            // Return the size of the object.

        void swap(Vector& other);
            // Swap the content of the Vector with the specified 'other'.
    };
//..
// The class stores pointers to the begin and the end of the elements as well
// as a pointer to the end of the allocated buffer.  If there are no elements,
// null pointers are stored.  There a number of accessors similar to the
// accessors used by 'std::Vector<TYPE>'.
//
// The default constructor creates an empty 'Vector<TYPE>' by simply
// initializing all member pointers to be null pointers:
//..
    template <class TYPE>
    Vector<TYPE>::Vector()
        : d_begin()
        , d_end()
        , d_endBuffer()
    {
    }
//..
// To leverage already implemented functionality some of the member functions
// operate on a temporary 'Vector<TYPE>' and move the result into place using
// the 'swap()' member function that simply does a memberwise 'swap()' (the
// function swapping pointers is implemented here to avoid any dependency on
// functions defined in another level):
//..
    template <class TYPE>
    void Vector<TYPE>::swap(TYPE*& a, TYPE*& b)
    {
        TYPE *tmp = a;
        a = b;
        b = tmp;
    }
    template <class TYPE>
    void Vector<TYPE>::swap(Vector& other)
    {
        this->swap(this->d_begin, other.d_begin);
        this->swap(this->d_end, other.d_end);
        this->swap(this->d_endBuffer, other.d_endBuffer);
    }
//..
// The member function 'reserve()' arranges for the 'Vector<TYPE>' to have
// enough capacity for the number of elements specified as argument.  The
// function first creates an empty 'Vector<TYPE>' called 'tmp' and sets 'tmp'
// up to have enough capacity by allocating sufficient memory and assigning the
// different members to point to the allocated buffer.  The function then
// iterates over the elements of 'this' and for each element it constructs a
// new element in 'tmp'.
//..
    template <class TYPE>
    void Vector<TYPE>::reserve(int newCapacity)
    {
        if (this->capacity() < newCapacity) {
            Vector tmp;
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
//..
// Any allocated data and constructed elements need to be release in the
// destructor.  The destructor does so by calling the destructor of the
// elements in the buffer from back to front.  Once the elements are destroyed
// the buffer is released:
//..
    template <class TYPE>
    Vector<TYPE>::~Vector()
    {
        if (this->d_begin) {
            while (this->d_begin != this->d_end) {
                --this->d_end;
                this->d_end->~TYPE();
            }
            operator delete(this->d_begin);
        }
    }
//..
// Using 'reserve()' and constructing the elements it is straight forward to
// implement the copy constructor.  First the member pointers are initialed to
// null.  If 'other' is empty there is nothing further to do as it is desirable
// to not allocate a buffer for an empty 'Vector'.  If there are elements to
// copy the buffer is set up by calling 'reserve()' to create sufficient
// capacity.  Once that is done elements are copied by iterating over the
// elements of 'other' and constructing elements using placement new in the
// appropriate location.
//..
    template <class TYPE>
    Vector<TYPE>::Vector(const Vector& other)
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
//..
// A simple copy assignment operator can be implemented in terms of copy/move
// constructors, 'swap()', and destructor (in a real implementation the copy
// assignment would probably try to use already allocated objects).  In this
// implementation that argument is taken by value, i.e., the argument is
// already constructed using copy or move construction (which may have been
// elided), the content of 'this' is swapped with the content of 'other'
// leaving this in the desired state, and the destructor will release the
// former representation of 'this' when 'other' is destroyed':
//..
    template <class TYPE>
    Vector<TYPE>& Vector<TYPE>::operator= (Vector other)
    {
        this->swap(other);
        return *this;
    }
//..
// To complete the normal C++03 operations of 'Vector<TYPE>' the only remaining
// member function is 'push_back()'.  This function calls 'reserve()' to obtain
// more capacity if the current capacity is filled and then constructs the new
// element at the location pointed to by 'd_end':
//..
    template <class TYPE>
    void Vector<TYPE>::push_back(const TYPE& value)
    {
        if (this->d_end == this->d_endBuffer) {
            this->reserve(this->size()? 2 * this->size() : 4);
        }
        ASSERT(this->d_end != this->d_endBuffer);
        new(this->d_end) TYPE(value);
        ++this->d_end;
    }
//..
// The first operation actually demonstrating the use of 'MovableRef<TYPE>' is
// the move constructor:
//..
    template <class TYPE>
    Vector<TYPE>::Vector(bslmf::MovableRef<Vector> other)
        : d_begin(bslmf::MovableRefUtil::access(other).d_begin)
        , d_end(bslmf::MovableRefUtil::access(other).d_end)
        , d_endBuffer(bslmf::MovableRefUtil::access(other).d_endBuffer)
    {
        Vector& reference(other);
        reference.d_begin = 0;
        reference.d_end = 0;
        reference.d_endBuffer = 0;
    }
//..
// This constructor gets an 'MovableRef<Vector<TYPE> >' passed as argument that
// indicates that the referenced objects can be modified as long as it is left
// in a state meeting the class invariants.  The implementation of this
// constructor first copies the 'd_begin', 'd_end', and 'd_capacity' members of
// 'other'.  Since 'other' is either an object of type
// 'MovableRef<Vector<TYPE> >' (when compiling using a C++03 compiler) or an
// r-value reference 'Vector<TYPE>&&' the members are accessed using
// 'MovableRefUtil::access(other)' to get a reference to a 'Vector<TYPE>'.
// Within the body of the constructor an l-value reference is obtained either
// via the conversion operator of 'MovableRef<T>' or directly as 'other' is
// just an l-value when compiling with a C++11 compiler.  This reference is
// used to set the pointer members of the object referenced by 'other' to '0'
// completing the move of the content to the object under construction.
//
// Finally, a move version of 'push_back()' is provided: it takes an
// 'MovableRef<TYPE>' as argument.  The type of this argument indicates that
// the state can be transferred and after arranging enough capacity in the
// 'Vector<TYPE>' object a new element is move constructed at the position
// 'd_end':
//..
    template <class TYPE>
    void Vector<TYPE>::push_back(bslmf::MovableRef<TYPE> value)
    {
        if (this->d_end == this->d_endBuffer) {
            this->reserve(this->size()? int(1.5 * this->size()): 4);
        }
        ASSERT(this->d_end != this->d_endBuffer);
        new(this->d_end) TYPE(bslmf::MovableRefUtil::move(value));
        ++this->d_end;
    }
//..
// Note that this implementation of 'push_back()' uses
// 'bslmf::MovableRefUtil::move(value)' to move the argument.  For a C++03
// implementation the argument would be moved even when using 'value' directly
// because the type of 'value' stays 'bslmf::MovableRef<TYPE>'.  However, for a
// C++11 implementation the argument 'value' is an l-value and using it
// directly would result in a copy.

}  // close unnamed namespace

//=============================================================================
//                      GLOBAL TYPES FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class Class;
    // This class declaration provides a name for an incomplete class type.

struct TestIsDeducible {
    // This 'struct' defines a function object whose overloaded function-call
    // operator can be used to test whether an argument's type is deducible as
    // a movable ref using the 'BSLMF_MOVABLEREF_DEDUCE' macro.  Note that the
    // overload taking an ellipsis "..." has the lowest possible precedence for
    // overload resolution and will only be selected if substitution fails for
    // the (deduced) movable-ref overload.  In essence, this overload set
    // allows testing whether or not substitution fails for the deduced movable
    // reference type.

    // ACCESSORS
    template <class TYPE>
    void operator()(int  line,
                    bool shouldTypeBeDeduced,
                    BSLMF_MOVABLEREF_DEDUCE(TYPE)) const
    {
        ASSERTV(line, shouldTypeBeDeduced);
    }

    void operator()(int line, bool shouldTypeBeDeduced, ...) const
    {
        ASSERTV(line, !shouldTypeBeDeduced);
    }
};

template <class ARG1, class ARG2, class ARG3>
struct ClassTemplate {
    // This 'struct' template defines trivial and standard-layout (POD) type
    // template.
};

struct TestTemplateIsDeducible {
    // This 'struct' defines a function object whose overloaded function-call
    // operator can be used to test whether an argument of a specialization of
    // an (optionally 'const'-qualified) 'ClassTemplate' type is deducible as a
    // movable ref using the 'BSLMF_MOVABLEREF_DEDUCE' macro.

    // ACCESSORS
    template <class ARG1, class ARG2, class ARG3>
    void
    operator()(int  line,
               bool shouldTypeBeDeduced,
               bool shouldPickConstOverload,
               BSLMF_MOVABLEREF_DEDUCE(ClassTemplate<ARG1, ARG2, ARG3>)) const
    {
        ASSERTV(line, shouldTypeBeDeduced);
        ASSERTV(line, !shouldPickConstOverload);
    }

    template <class ARG1, class ARG2, class ARG3>
    void operator()(
          int  line,
          bool shouldTypeBeDeduced,
          bool shouldPickConstOverload,
          BSLMF_MOVABLEREF_DEDUCE(const ClassTemplate<ARG1, ARG2, ARG3>)) const
    {
        ASSERTV(line, shouldTypeBeDeduced);
        ASSERTV(line, shouldPickConstOverload);
    }

    void operator()(int line, bool shouldTypeBeDeduced, bool, ...) const
    {
        ASSERTV(line, !shouldTypeBeDeduced);
    }
};

struct TestOverloadSelection {
    // This 'struct' defines a function object whose overloaded function-call
    // operator can be used to test (1) whether an overload set for an argument
    // with both a deduced movable reference type and alternatively for an
    // argument with a deduced 'const' 'lvalue' reference is unambiguous, and
    // (2) which overload is selected for a particular argument.

    // ACCESSORS
    template <class TYPE>
    void operator()(int line,
                    bool shouldPickMovableRefOverload,
                    BSLMF_MOVABLEREF_DEDUCE(TYPE)) const
    {
        ASSERTV(line, shouldPickMovableRefOverload);
    }

    template <class TYPE>
    void operator()(int line,
                    bool shouldPickMovableRefOverload,
                    const TYPE&) const
    {
        ASSERTV(line, !shouldPickMovableRefOverload);
    }
};

struct TestOverloadSelectionWithConstMovableRef {
    // This 'struct' defines a function object whose function-call operator
    // overload set can be used to test (1) whether an overload set for an
    // argument with both a deduced movable reference type and alternatively
    // for an argument with a deduced movable reference to 'const' is
    // unambiguous, and (2) which overload is selected for a particular
    // argument.

    // ACCESSORS
    template <class TYPE>
    void operator()(int line,
                    bool shouldPickConstOverload,
                    BSLMF_MOVABLEREF_DEDUCE(TYPE)) const
    {
        ASSERTV(line, !shouldPickConstOverload);
    }

    template <class TYPE>
    void operator()(int line,
                    bool shouldPickConstOverload,
                    BSLMF_MOVABLEREF_DEDUCE(const TYPE)) const
    {
        ASSERTV(line, shouldPickConstOverload);
    }
};


template <class TYPE>
struct TestMovableRefArgument {
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
struct MovableAddress {
    static TYPE *get(bslmf::MovableRef<TYPE> movable)
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
    void operator=(TestMoving&);  // = delete;

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
  public:
#endif
    TestMoving(const TestMoving& other)
    // Create a 'TestMoving' object that is a copy of the specified
    // 'other'.  In this case copying 'other' means allocating a new 'int'
    // with a new unique pointer.
    : d_pointer(new int(1 + *other.d_pointer))
    {
    }

  public:
    TestMoving()
    : d_pointer(new int(0))
        // Create a 'TestMoving' object using the default constructor.  The
        // object is initialized with a unique pointer to 'int'.
    {
    }

    explicit TestMoving(bslmf::MovableRef<TestMoving> rvalue)
        // Create a 'TestMoving' object that moves the value referenced by the
        // specified 'rvalue'.
    : d_pointer(bslmf::MovableRefUtil::access(rvalue).d_pointer)
    {
        bslmf::MovableRefUtil::access(rvalue).d_pointer = 0;
    }

    ~TestMoving() { delete this->d_pointer; }
        // Destroy this object. Delete the allocated pointer.

    void operator&() const {}
        // Do nothing.  The address-of operator gets in the way.

    TestMoving *getAddress() { return this; }
        // Return the address of this object.

    const int *getPointer() const { return this->d_pointer; }
        // Return the held pointer.
};

template <class TYPE>
bool testForwardRefArgument(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) arg)
    // Move from specified 'arg', which must be of type
    // 'MovableRef<TestMoving>'. This function tests that 'MovableRef'
    // arguments can be passed and used through forwarding references.
{
    TestMoving moveTo(bslmf::MovableRefUtil::move(arg));
        // Return true if 'arg' was moved from
    return 0 == bslmf::MovableRefUtil::access(arg).getPointer();
}

void f(int)
    // This function takes a single integer argument, which does not have a
    // default value, and returns nothing.  This function, along with 'g'
    // below, are used to test that the type traits provided by
    // 'bslmf::MovableRefUtil' work correctly when applied to deduced function
    // types having arguments with and without default values using the IBM XL
    // C++ compiler line.  Said line of compilers have a defect in which
    // 'typedef's cannot be formed to function types coming from functions that
    // have default arguments.
{
}

void g(int = 0)
    // This function takes a single integer argument, which has a default
    // value, and returns nothing.  This function, along with 'f' above, are
    // used to test that the type traits provided by 'bslmf::MovableRefUtil'
    // work correctly when applied to deduced function types having arguments
    // with and without default values using the IBM XL C++ compiler line.
    // Said line of compilers have a defect in which 'typedef's cannot be
    // formed to function types coming from functions that have default
    // arguments.
{
}

template <bool EXPECTED_VALUE, template <class> class TRAIT, class TYPE>
void testBooleanTrait(int LINE, TYPE&)
    // Apply the specified 'TYPE' to the specified 'TRAIT' template and assert
    // that the so-appplied 'TRAIT' template provides a static, Boolean data
    // member named 'value' that is equal to the specified 'EXPECTED_VALUE'.
{
    ASSERTV(LINE,
            EXPECTED_VALUE,
            TRAIT<TYPE>::value,
            EXPECTED_VALUE == TRAIT<TYPE>::value);
}

template <class EXPECTED_TYPE, template <class> class TRAIT, class TYPE>
void testTransformationTrait(int LINE, TYPE&)
    // Apply the specified 'TYPE' to the specified 'TRAIT' template and assert
    // that the so-applied 'TRAIT' template provides a member 'typedef' named
    // 'type' that is the same as the specified 'EXPECTED_TYPE'.
{
    ASSERTV(LINE,
            (bsl::is_same<EXPECTED_TYPE, typename TRAIT<TYPE>::type>::value));
};

//=============================================================================
//              TEST VOCABULARY FOR BSL_IS_NOTHROW_MOVE_CONSTRUCTIBLE
//-----------------------------------------------------------------------------

// The following test types (and the macros defined further below) were
// originally in the 'bslmf_isnothrowmoveconstructible' test driver (to verify
// that the 'bsl::is_nothrow_move_constructible' metafunction can be correctly
// extended), but were moved here to eliminate a cycle, i.e., a direct cycle
// between that component and this one.  Note that 'bslmf::MovableRef' must be
// used to expose extensions of 'bsl::is_nothrow_move_constructible' to C++03
// environments.  Testing of such extensions is done in case 7.

class ImmovableClass {
  private:
    // NOT IMPLEMENTED
    ImmovableClass(const ImmovableClass&) BSLS_KEYWORD_NOEXCEPT;  // = delete;
    ImmovableClass(bslmf::MovableRef<ImmovableClass>) BSLS_KEYWORD_NOEXCEPT;
};

class ImmovableUnion {
  private:
    // NOT IMPLEMENTED
    ImmovableUnion(const ImmovableUnion&) BSLS_KEYWORD_NOEXCEPT;  // = delete;
    ImmovableUnion(bslmf::MovableRef<ImmovableUnion>) BSLS_KEYWORD_NOEXCEPT;
};

struct LyingMovableClass {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    LyingMovableClass();
    LyingMovableClass(const LyingMovableClass&);
    LyingMovableClass(bslmf::MovableRef<LyingMovableClass>); // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct LyingMovableUnion {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    LyingMovableUnion();
    LyingMovableUnion(const LyingMovableUnion&);
    LyingMovableUnion(bslmf::MovableRef<LyingMovableUnion>); // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct LyingNestedTraitMovableClass {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(LyingNestedTraitMovableClass,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    LyingNestedTraitMovableClass();
    LyingNestedTraitMovableClass(const LyingNestedTraitMovableClass&);
    LyingNestedTraitMovableClass(
                  bslmf::MovableRef<LyingNestedTraitMovableClass>); // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct LyingNestedTraitMovableUnion {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(LyingNestedTraitMovableUnion,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    LyingNestedTraitMovableUnion();
    LyingNestedTraitMovableUnion(const LyingNestedTraitMovableUnion&);
    LyingNestedTraitMovableUnion(
                  bslmf::MovableRef<LyingNestedTraitMovableUnion>); // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NestedTraitNothrowCopyableClass {
    // This user-defined type, which is marked to have a 'nothrow' copy
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitNothrowCopyableClass,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    NestedTraitNothrowCopyableClass();
    NestedTraitNothrowCopyableClass(const NestedTraitNothrowCopyableClass&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NestedTraitNothrowCopyableUnion {
    // This user-defined type, which is marked to have a 'nothrow' copy
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitNothrowCopyableUnion,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    NestedTraitNothrowCopyableUnion();
    NestedTraitNothrowCopyableUnion(const NestedTraitNothrowCopyableUnion&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NestedTraitNothrowMovableClass {
    // This user-defined type, which is marked to have a 'nothrow' move
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitNothrowMovableClass,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    NestedTraitNothrowMovableClass();
    NestedTraitNothrowMovableClass(const NestedTraitNothrowMovableClass&);
    NestedTraitNothrowMovableClass(
                             bslmf::MovableRef<NestedTraitNothrowMovableClass>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NestedTraitNothrowMovableUnion {
    // This user-defined type, which is marked to have a 'nothrow' move
    // constructor using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used
    // for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitNothrowMovableUnion,
                                   bsl::is_nothrow_move_constructible);

    // CREATORS
    NestedTraitNothrowMovableUnion();
    NestedTraitNothrowMovableUnion(const NestedTraitNothrowMovableUnion&);
    NestedTraitNothrowMovableUnion(
                             bslmf::MovableRef<NestedTraitNothrowMovableUnion>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NestedTraitTrivialClass {
    // This user-defined type, which is correctly marked as trivially copyable
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitTrivialClass,
                                   bsl::is_trivially_copyable);
};

struct NestedTraitTrivialUnion {
    // This user-defined type, which is correctly marked as trivially copyable
    // using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(NestedTraitTrivialUnion,
                                   bsl::is_trivially_copyable);
};

struct NothrowCopyableTestClass {
    // This user-defined type, which is marked to have a no-throw copy
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    NothrowCopyableTestClass();
    NothrowCopyableTestClass(const NothrowCopyableTestClass&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NothrowCopyableTestUnion {
    // This user-defined type, which is marked to have a no-throw copy
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    NothrowCopyableTestUnion();
    NothrowCopyableTestUnion(const NothrowCopyableTestUnion&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NothrowMovableTestClass {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    NothrowMovableTestClass();
    NothrowMovableTestClass(const NothrowMovableTestClass&);
    NothrowMovableTestClass(bslmf::MovableRef<NothrowMovableTestClass>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct NothrowMovableTestUnion {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    NothrowMovableTestUnion();
    NothrowMovableTestUnion(const NothrowMovableTestUnion& original);
    NothrowMovableTestUnion(bslmf::MovableRef<NothrowMovableTestUnion>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct TrivialClass {
    // This user-defined type, which is correctly marked as trivially copyable
    // by explicit specialization of the 'bsl::is_trivially_copyable' trait, is
    // used for testing.
};

struct TrivialUnion {
    // This user-defined type, which is correctly marked as trivially copyable
    // by explicit specialization of the 'bsl::is_trivially_copyable' trait, is
    // used for testing.
};


struct UnspecializedNothrowCopyableClass {
    // This user-defined type, which is marked to have a no-throw copy
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    UnspecializedNothrowCopyableClass();
    UnspecializedNothrowCopyableClass(const UnspecializedNothrowCopyableClass&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UnspecializedNothrowCopyableUnion {
    // This user-defined type, which is marked to have a no-throw copy
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    UnspecializedNothrowCopyableUnion();
    UnspecializedNothrowCopyableUnion(const UnspecializedNothrowCopyableUnion&)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UnspecializedNothrowMovableClass {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    UnspecializedNothrowMovableClass();
    UnspecializedNothrowMovableClass(const UnspecializedNothrowMovableClass&);
    UnspecializedNothrowMovableClass(
                           bslmf::MovableRef<UnspecializedNothrowMovableClass>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UnspecializedNothrowMovableUnion {
    // This user-defined type, which is marked to have a no-throw move
    // constructor using template specialization (below), is used for testing.

    // CREATORS
    UnspecializedNothrowMovableUnion();
    UnspecializedNothrowMovableUnion(const UnspecializedNothrowMovableUnion&);
    UnspecializedNothrowMovableUnion(
                           bslmf::MovableRef<UnspecializedNothrowMovableUnion>)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Explicitly supply constructors that do nothing, to ensure that this
        // class has no trivial traits detected with a conforming C++11 library
        // implementation.
};

struct UnspecializedTrivialClass {
    // This user-defined type, which is correctly marked as trivially copyable
    // by explicit specialization of the 'bsl::is_trivially_copyable' trait, is
    // used for testing.
};

struct UnspecializedTrivialUnion {
    // This user-defined type, which is correctly marked as trivially copyable
    // by explicit specialization of the 'bsl::is_trivially_copyable' trait, is
    // used for testing.
};

struct UserDefinedThrowTestType {
    // This user-defined type, which is not marked to be 'nothrow' move
    // constructible, is used for testing.

    // CREATORS
    UserDefinedThrowTestType();
    UserDefinedThrowTestType(
                      bslmf::MovableRef<UserDefinedThrowTestType>); // IMPLICIT
    UserDefinedThrowTestType(const UserDefinedThrowTestType&);
};


}  // close unnamed namespace

namespace bsl {

template <>
struct is_nothrow_move_constructible<LyingMovableClass>
    : bsl::true_type {};
template <>
struct is_nothrow_move_constructible<LyingMovableUnion>
    : bsl::true_type {};

template <>
struct is_nothrow_move_constructible<NothrowCopyableTestClass>
    : bsl::true_type {};
template <>
struct is_nothrow_move_constructible<NothrowCopyableTestUnion>
    : bsl::true_type {};

template <>
struct is_nothrow_move_constructible<NothrowMovableTestClass>
    : bsl::true_type {};
template <>
struct is_nothrow_move_constructible<NothrowMovableTestUnion>
    : bsl::true_type {};

template <>
struct is_trivially_copyable<TrivialClass> : bsl::true_type {};
template <>
struct is_trivially_copyable<TrivialUnion> : bsl::true_type {};


}  // close namespace bsl

namespace test {

template <class TYPE>
struct IgnoreOracle : bsl::false_type {};

template <>
struct IgnoreOracle<LyingMovableClass> : bsl::true_type {};
template <>
struct IgnoreOracle<LyingMovableUnion> : bsl::true_type {};
template <>
struct IgnoreOracle<LyingNestedTraitMovableClass> : bsl::true_type {};
template <>
struct IgnoreOracle<LyingNestedTraitMovableUnion> : bsl::true_type {};

}  // close namespace test

//=============================================================================
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below test the 'bsl::is_nothrow_move_constructible'
// trait with a set of variations on a type.  There are several layers of
// macros, as object types support the full range of variation, but function
// types cannot form an array, nor be cv-qualified.  Similarly, 'void' may be
// cv-qualified, but still cannot form an array.  As macros are strictly
// text-substitution we must use the appropriate 'add_decoration' traits to
// transform types in a manner that is guaranteed to be syntactically valid.
// Note that these are not type-dependent contexts, so there is no need to use
// 'typename' when fetching the result from any of the queried traits.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
# define ASSERT_V_SAME(TYPE)                                                  \
    ASSERT(bsl::is_nothrow_move_constructible  <TYPE>::value ==               \
            bsl::is_nothrow_move_constructible_v<TYPE>)
    // 'ASSERT' that 'is_nothrow_move_constructible_v' has the same value as
    // 'is_nothrow_move_constructible::value'.
#else
# define ASSERT_V_SAME(TYPE)
    // Variable templates are not supported, so there is nothing to test.
#endif


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE
//   This macro validates that the 'bsl' trait has the same result as the
//   native trait, if it is available.
#if defined(BSLMF_MOVABLEREF_USE_NATIVE_ORACLE)
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE)             \
    ASSERT( test::IgnoreOracle<TYPE>::value ||                                \
           (std::is_nothrow_move_constructible<TYPE>::value ==                \
            bsl::is_nothrow_move_constructible<TYPE>::value))
    // Confirm that the result of 'bsl::is_nothrow_move_constructible<TYPE>'
    // agrees with the oracle 'std::is_nothrow_move_constructible'.
#else
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE)            \
    // The native trait is not available to act as an oracle, so there is no
    // test to perform.
#endif


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE
//   This macro tests the result of 'bsl::is_nothrow_move_constructible<TYPE>'
//   has the same value as the expected 'RESULT'; it then confirms that the
//   expected 'RESULT' agrees with the native oracle, where available; finally.
//   it confirms that the associated variable template, when available, has a
//   value that agrees with this trait instantiation.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE, RESULT)                    \
    ASSERT( bsl::is_nothrow_move_constructible<TYPE>::value ==  RESULT);      \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CONSULT_ORACLE(TYPE);                \
    ASSERT_V_SAME(TYPE)


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for an rvalue reference to the given 'TYPE' on platforms that
//   implement language support, and performs no test otherwise.  Note that the
//   native trait implementation shipping with Visual C++ compilers prior to
//   VC 2017 erroneously reports that rvalue-references to arrays are NOT
//   no-throw move constructible.
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1910
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)          \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                      bsl::add_rvalue_reference<TYPE>::type,  \
                                      !bsl::is_array<TYPE>::value)
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)          \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                 bsl::add_rvalue_reference<TYPE>::type, true)
#else
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)
#endif


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE
//   This macro tests that the 'is_move_constructible' trait has the expected
//   'RESULT' for the given 'TYPE', and pointers/references to that type.
//   Pointers and references are always no-throw move constructible.
# define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, RESULT)              \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE, RESULT);                       \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(bsl::add_pointer<TYPE>::type, true); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(                                     \
                                bsl::add_lvalue_reference<TYPE>::type, true); \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_RVAL_REF(TYPE, RESULT)


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST
//   This macro tests ...
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE, NO_CVQ, CONST)     \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(TYPE, NO_CVQ);                  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(bsl::add_const<TYPE>::type,     \
                                              CONST);                         \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(bsl::add_volatile<TYPE>::type,  \
                                              false);                         \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TYPE(bsl::add_cv<TYPE>::type,false)


// Macro: ASSERT_ARRAYS_ARE_NOT_NOTHROW_MOVE_CONSTRUCTIBLE
//   This macro tests that array types are never move constructible.  This is a
//   consequence of the C++ Standard defining this trait in terms of direct
//   initialization of a variable of this type.  Arrays as data members support
//   element-wise move, but this trait does not report that.
#define ASSERT_ARRAYS_ARE_NOT_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE)                \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE[],      false, false);  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE[][8],   false, false);  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE[128],   false, false);  \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE[12][8], false, false)


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS
//   This macro tests that a the trait for a type matches the expected result,
//   but that cv-qualified versions of that type are never no-throw movable.
//   DO NOT CALL THIS MACRO FOR TRIVIAL CLASS TYPES, as the cv-qualified
//   results will be wrong.  Use the '_TRIVIAL_' macro below instead.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(TYPE, RESULT)              \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE, RESULT, false);        \
    ASSERT_ARRAYS_ARE_NOT_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE)


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS
//   This macro confirms that the specified 'TYPE' is a trivial class for
//   platforms that can natively determine the property, and does nothing
//   otherwise.
#if defined(BSLMF_ISTRIVIALLYCOPYABLE_NATIVE_IMPLEMENTATION)
# define ASSERT_IS_TRIVIAL_CLASS(TYPE)                                        \
    ASSERT(bsl::is_trivially_copyable<TYPE>::value)
#else
# define ASSERT_IS_TRIVIAL_CLASS(TYPE)
#endif


// Macro: ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS
//   This macro tests that a the trait for a trivial class type matches the
//   expected trait results: a trivial type is no-throw movable /and/ no-throw
//   copyable, so a 'const'-qualified, but not a 'volatile'-qualified', version
//   of the class will also detect as no-throw movable.  but that cv-qualified
//   versions of that type are never no-throw movable.
#define ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(TYPE)              \
    ASSERT_IS_TRIVIAL_CLASS(TYPE);                                            \
    ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(TYPE, true, true);           \
    ASSERT_ARRAYS_ARE_NOT_NOTHROW_MOVE_CONSTRUCTIBLE(TYPE)


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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
        //:   leading comment characters, and replace 'ASSERT' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// To demonstrate the newly created 'Vector<TYPE>' class in action, first a
// 'Vector<int>' is created and filled with a few elements:
//..
    Vector<int> vector0;
    for (int i = 0; i != 5; ++i) {
        vector0.push_back(i);
    }
    for (int i = 0; i != 5; ++i) {
        ASSERT(vector0[i] == i);
    }
//..
// To verify that copying of 'Vector<TYPE>' objects works, a copy is created:
//..
    Vector<int> vector1(vector0);
    ASSERT(vector1.size() == 5);
    ASSERT(vector1.size() == vector0.size());
    for (int i = 0; i != vector1.size(); ++i) {
        ASSERT(vector1[i] == i);
        ASSERT(vector1[i] == vector0[i]);
    }
//..
// When using moving this 'vector0' to a new location the representation of the
// new object should use the original 'begin()':
//..
    const int   *first = vector0.begin();
    Vector<int>  vector2(bslmf::MovableRefUtil::move(vector0));
    ASSERT(first == vector2.begin());
//..
// When create a 'Vector<Vector<int> >' and using 'push_back()' on this object
// with 'vector2' a copy should be inserted:
//..
    Vector<Vector<int> > vVector;
    vVector.push_back(vector2);                          // copy
    ASSERT(vector2.size() == 5);
    ASSERT(vVector.size() == 1);
    ASSERT(vVector[0].size() == vector2.size());
    ASSERT(vVector[0].begin() != first);
    for (int i = 0; i != 5; ++i) {
        ASSERT(vVector[0][i] == i);
        ASSERT(vector2[i] == i);
    }
//..
// When adding another element by moving 'vector2' the 'begin()' of the newly
// inserted element will be the same as 'first', i.e., the representation is
// transferred:
//..
    vVector.push_back(bslmf::MovableRefUtil::move(vector2)); // move
    ASSERT(vVector.size() == 2);
    ASSERT(vVector[1].begin() == first);
    ASSERT(vVector[1].size() == 5);
//..
// Compiling this code with both C++03 and C++11 compilers shows that there is
// no need for conditional compilation in when using 'MovableRef<TYPE>' while
// move semantics is enabled in both modes.
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DEDUCTION
        //   Ensure that the macro 'BSLMF_MOVABLEREF_DEDUCE' expands to a type
        //   that is an instantiation of 'bslmf::MovableRef' that is deducible,
        //   even if the argument to the macro is, for example, a template
        //   instantiation with multiple arguments.  Also, ensure that the
        //   so-deduced type is always an rvalue reference and never an lvalue
        //   reference when compiled in C++11 mode and later, such that
        //   substitution fails for would-be-deduced lvalue references.
        //
        // Concerns:
        //: 1 The macro only deduces movable references and never lvalue
        //:   references, and does so in a sfinae-friendly way.
        //:
        //: 2 The types that are deduced can be arguments to type templates,
        //:   e.g., 'BSLMF_MOVABLEREF_DEDUCE(bsl::pair<T1, T2>)' should be able
        //:   to deduce 'T1' and 'T2', and expand to a movable reference to
        //:   'bsl::pair<T1, T2>' even though the argument to the macro has a
        //:   comma in it.
        //:
        //: 3 It is safe to use a deduced movable reference in an overload set
        //:   that also takes an lvalue reference, and overload selection
        //:   behaves as if the movable reference were 'MovableRef<T>' for some
        //:   'T' in C++03, and as 'T&&' in C++11 and later.
        //:
        //: 4 The macro is able to deduce a movable reference to 'const',
        //:   it is safe to have an overload set with 'const' and non-'const'
        //:   deduced movable references, and overload resolution behaves as
        //:   if the movable references to const were 'MovableRef<const T>'
        //:   in C++03, and as 'const T&&' in C++11 and later.
        //
        // Plan:
        //: 1 Given all possible combinations of 'const' and reference
        //:   qualification for 'int' and a trivial class type 'O', verify that
        //:   movable references can be deduced for expressions of such types
        //:   having lvalue, xvalue, and prvalue value categories (where
        //:   possible for the language version and reference qualification).
        //:   Verify that movable references can be deduced for xvalue and
        //:   prvalue expressions of movable reference type (but not lvalue
        //:   expressions when in C++11 and later). (C-1)
        //:
        //: 2 Given all possible combinations of 'const' and reference
        //:   qualification for a trivial class template specialization
        //:   ('ClassTemplate<int, int, int>'), verify that a deduced movable
        //:   reference to an (optionally 'const') 'ClassTemplate<T1, T2, T3>'
        //:   where 'T1', 'T2', and 'T3' are template arguments, is deducible.
        //:   (C-2)
        //:
        //: 3 Given all possible combinations of 'const' and reference
        //:   qualification for 'int' and a trivial class type 'O', verify that
        //:   an overload set consisting of an lvalue reference to a 'const'
        //:   'T' and a deduced movable reference select the lvalue-reference
        //:   overload for lvalue expressions, and select the
        //:   deduced-movable-reference overload for xvalue and prvalue
        //:   expressions. (C-3)
        //:
        //: 4 Given all possible combinations of 'const' and reference
        //:   qualification for 'int' and a trivial class type 'O', verify that
        //:   the 'const' qualification of a deduced movable reference matches
        //:   the 'const' qualification of the expression (except when the
        //:   expression is a fundamental type, in which the 'const' is
        //:   dropped).  Also verify that there are no overload resolution
        //:   ambiguity problems for overload sets taking both 'const' and
        //:   non-'const' deduced movable references. (C-4)
        //
        // Testing:
        //   BSLMF_MOVABLEREF_DEDUCE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING DEDUCTION"
                   "\n=================\n");

        // Short for "Not Applicable"
        static const bool NA = false;

        static const bool YES = true;
        static const bool NO = false;

        typedef bslmf::MovableRefUtil U;

        typedef int I;
        typedef I& rI;
        typedef const I cI;
        typedef const I& rcI;
        typedef bslmf::MovableRef<I> rrI;
        typedef bslmf::MovableRef<cI> rrcI;
        I i = 0;
        rI ri = i;
        rrI rri = bslmf::MovableRefUtil::move(i);
        cI ci = 0;
        rcI rci = ci;
        rrcI rrci = bslmf::MovableRefUtil::move(ci);

        typedef TrivialClass O;
        typedef O& rO;
        typedef const O cO;
        typedef const O& rcO;
        typedef bslmf::MovableRef<O> rrO;
        typedef bslmf::MovableRef<cO> rrcO;
        O o;
        rO ro = o;
        rrO rro = bslmf::MovableRefUtil::move(o);
        cO co;
        rcO rco = co;
        rrcO rrco = bslmf::MovableRefUtil::move(co);

        typedef ClassTemplate<int, int, int> T;
        typedef T& rT;
        typedef const T cT;
        typedef const T& rcT;
        typedef bslmf::MovableRef<T> rrT;
        typedef bslmf::MovableRef<const T> rrcT;
        T t;
        rT rt = t;
        rrT rrt = bslmf::MovableRefUtil::move(t);
        cT ct;
        rcT rct = ct;
        rrcT rrct = bslmf::MovableRefUtil::move(ct);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        const bool IF_RVALUE_REF = true;
#else
        const bool IF_RVALUE_REF = false;
#endif

        const TestIsDeducible TEST1;

        //       LINE
        //      .----
        //     / IS MR DEDUCIBLE?   EXPRESSION
        //    -- ---------------- ---------------
        TEST1(L_,   IF_RVALUE_REF, I()           );
        TEST1(L_,   IF_RVALUE_REF, cI()          );
        TEST1(L_,   NO           , i             );
        TEST1(L_,   NO           , ri            );
        TEST1(L_, ! IF_RVALUE_REF, rri           );
        TEST1(L_,   NO           , ci            );
        TEST1(L_,   NO           , rci           );
        TEST1(L_, ! IF_RVALUE_REF, rrci          );
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST1(L_,   YES          , U::move(I())  );
#endif
        TEST1(L_,   YES          , U::move(i)    );
        TEST1(L_,   YES          , U::move(ri)   );
        TEST1(L_,   YES          , U::move(rri)  );
        TEST1(L_,   YES          , U::move(ci)   );
        TEST1(L_,   YES          , U::move(rci)  );
        TEST1(L_,   YES          , U::move(rrci) );

        TEST1(L_,   IF_RVALUE_REF, O()           );
        TEST1(L_,   IF_RVALUE_REF, cO()          );
        TEST1(L_,   NO           , o             );
        TEST1(L_,   NO           , ro            );
        TEST1(L_, ! IF_RVALUE_REF, rro           );
        TEST1(L_,   NO           , co            );
        TEST1(L_,   NO           , rco           );
        TEST1(L_, ! IF_RVALUE_REF, rrco          );
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST1(L_,   YES          , U::move(O())  );
        TEST1(L_,   YES          , U::move(cO()) );
#endif
        TEST1(L_,   YES          , U::move(o)    );
        TEST1(L_,   YES          , U::move(ro)   );
        TEST1(L_,   YES          , U::move(rro)  );
        TEST1(L_,   YES          , U::move(co)   );
        TEST1(L_,   YES          , U::move(rco)  );
        TEST1(L_,   YES          , U::move(rrco) );

        const TestTemplateIsDeducible TEST2;

        //       LINE
        //      .----
        //     / IS MR DEDUCIBLE?    CONST?     EXPRESSION
        //    -- ---------------- ----------- ---------------
        TEST2(L_,   IF_RVALUE_REF, NO        , T()           );
        TEST2(L_,   IF_RVALUE_REF, YES       , cT()          );
        TEST2(L_,   NO           , NA        , t             );
        TEST2(L_,   NO           , NA        , rt            );
        TEST2(L_, ! IF_RVALUE_REF, NO        , rrt           );
        TEST2(L_,   NO           , NA        , ct            );
        TEST2(L_,   NO           , NA        , rct           );
        TEST2(L_, ! IF_RVALUE_REF, YES       , rrct          );
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST2(L_,   YES          , NO        , U::move(T())  );
        TEST2(L_,   YES          , YES       , U::move(cT()) );
#endif
        TEST2(L_,   YES          , NO        , U::move(t)    );
        TEST2(L_,   YES          , NO        , U::move(rt)   );
        TEST2(L_,   YES          , NO        , U::move(rrt)  );
        TEST2(L_,   YES          , YES       , U::move(ct)   );
        TEST2(L_,   YES          , YES       , U::move(rct)  );
        TEST2(L_,   YES          , YES       , U::move(rrct) );

        const TestOverloadSelection TEST3;

        //       LINE
        //      .----
        //     / IS MR DEDUCIBLE?   EXPRESSION
        //    -- ---------------- ---------------
        TEST3(L_,   IF_RVALUE_REF, I()           );
        TEST3(L_,   IF_RVALUE_REF, cI()          );
        TEST3(L_,   NO           , i             );
        TEST3(L_,   NO           , ri            );
        TEST3(L_, ! IF_RVALUE_REF, rri           );
        TEST3(L_,   NO           , ci            );
        TEST3(L_,   NO           , rci           );
        TEST3(L_, ! IF_RVALUE_REF, rrci          );
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST3(L_,   YES          , U::move(I())  );
        TEST3(L_,   YES          , U::move(cI()) );
#endif
        TEST3(L_,   YES          , U::move(i)    );
        TEST3(L_,   YES          , U::move(ri)   );
        TEST3(L_,   YES          , U::move(rri)  );
        TEST3(L_,   YES          , U::move(ci)   );
        TEST3(L_,   YES          , U::move(rci)  );
        TEST3(L_,   YES          , U::move(rrci) );

        TEST3(L_,   IF_RVALUE_REF, O()           );
        TEST3(L_,   IF_RVALUE_REF, cO()          );
        TEST3(L_,   NO           , o             );
        TEST3(L_,   NO           , ro            );
        TEST3(L_, ! IF_RVALUE_REF, rro           );
        TEST3(L_,   NO           , co            );
        TEST3(L_,   NO           , rco           );
        TEST3(L_, ! IF_RVALUE_REF, rrco          );
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST3(L_,   YES          , U::move(O())  );
        TEST3(L_,   YES          , U::move(cO()) );
#endif
        TEST3(L_,   YES          , U::move(o)    );
        TEST3(L_,   YES          , U::move(ro)   );
        TEST3(L_,   YES          , U::move(rro)  );
        TEST3(L_,   YES          , U::move(co)   );
        TEST3(L_,   YES          , U::move(rco)  );
        TEST3(L_,   YES          , U::move(rrco) );

        const TestOverloadSelectionWithConstMovableRef TEST4;

        //       LINE
        //      .----
        //     /   DEDUCE CONST?     EXPRESSION
        //    -- ---------------- ---------------
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST4(L_,   NO           , I()           );
        TEST4(L_,   NO           , cI()          );
        TEST4(L_,   NO           , U::move(I())  );
        TEST4(L_,   NO           , U::move(cI()) );
#endif
        TEST4(L_,   NO           , U::move(i)    );
        TEST4(L_,   NO           , U::move(ri)   );
        TEST4(L_,   NO           , U::move(rri)  );
        TEST4(L_,   YES          , U::move(ci)   );
        TEST4(L_,   YES          , U::move(rci)  );
        TEST4(L_,   YES          , U::move(rrci) );

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TEST4(L_,   NO           , O()           );
        TEST4(L_,   YES          , cO()          );
        TEST4(L_,   NO           , U::move(O())  );
        TEST4(L_,   YES          , U::move(cO()) );
#endif
        TEST4(L_,   NO           , U::move(o)    );
        TEST4(L_,   NO           , U::move(ro)   );
        TEST4(L_,   NO           , U::move(rro)  );
        TEST4(L_,   YES          , U::move(co)   );
        TEST4(L_,   YES          , U::move(rco)  );
        TEST4(L_,   YES          , U::move(rrco) );

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING IBM XL C++ FUNCTIONS WITH DEFAULT ARGUMENTS WORKAROUND
        //   Ensure that the property and transformation traits provided by
        //   the 'bslmf::MovableRefUtil' utility 'struct' provide correct
        //   results when applied to function types deduced from functions,
        //   where possible, including when compiled with the IBM XL C++ line
        //   of compilers.  These compilers have a defect in which 'typedef's
        //   to function types deduced from functions that have default
        //   arguments cannot be formed.
        //
        // Concerns:
        //: 1 All property and transformation traits in
        //:   'bslmf::MovableRefUtil', except 'RemoveReference', are
        //:   instantiable with function types deduced from functions with
        //:   default arguments when compiled with the IBM XL C++ line of
        //:   compilers.
        //:
        //: 2 All property and transformation traits in 'bslmf::MovableRefUtil'
        //:   are instantiable with function types deduced from arbitrary
        //:   functions with all other compiler lines.
        //:
        //: 3 Such instantiations provide correct results.
        //
        // Plan:
        //: 1 For each property and transformation trait in
        //:   'bslmf::MovableRefUtil', instantiate the trait template with
        //:   a type deduced from a function without default arguments, and
        //:   a function with default arguments, and verify that the value of
        //:   the trait is correct per its contract.  However, do not attempt
        //:   to instantiate the 'RemoveReference' trait using a type deduced
        //:   from a function with default arguments using the IBM XL C++ line
        //:   of compilers.  Such an attempt will cause compilation to fail as
        //:   a result of triggering a compiler defect.
        //
        // Testing:
        //   CONCERN: IBM XL C++ functions with default arguments workaround
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING IBM XL C++ FUNCTIONS WITH DEFAULT ARGUMENTS "
                   "WORKAROUND"
                   "\n===================================================="
                   "==========\n");

        typedef bslmf::MovableRefUtil Util;

        testBooleanTrait<false, Util::IsLvalueReference >(L_, f);
        testBooleanTrait<false, Util::IsLvalueReference >(L_, g);
        testBooleanTrait<false, Util::IsMovableReference>(L_, f);
        testBooleanTrait<false, Util::IsMovableReference>(L_, g);
        testBooleanTrait<false, Util::IsReference       >(L_, f);
        testBooleanTrait<false, Util::IsReference       >(L_, g);

#define MR ::BloombergLP::bslmf::MovableRef

        typedef void F(int);

        testTransformationTrait<F    , Util::RemoveReference    >(L_, f);
#if !defined(BSLS_PLATFORM_CMP_IBM) || BSLS_PLATFORM_CMP_VERSION > 4097
        testTransformationTrait<F    , Util::RemoveReference    >(L_, g);
#endif
        testTransformationTrait<F&   , Util::AddLvalueReference >(L_, f);
        testTransformationTrait<F&   , Util::AddLvalueReference >(L_, g);
        testTransformationTrait<MR<F>, Util::AddMovableReference>(L_, f);
        testTransformationTrait<MR<F>, Util::AddMovableReference>(L_, g);
        testTransformationTrait<F *  , Util::Decay              >(L_, f);
        testTransformationTrait<F *  , Util::Decay              >(L_, g);

#undef MR

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // EXTENDING 'bsl::is_nothrow_move_constructible'
        //   Ensure the 'bsl::is_nothrow_move_constructible' metafunction
        //   returns the correct value for class types explicitly specified to
        //   have a 'nothrow' move constructor.  This test case completes the
        //   'bslmf_isnothrowmoveconstructible' test driver validating (only)
        //   class types across all variations where explicit BDE traits may
        //   agree or disagree with implicitly deduced C++11 traits.
        //
        // Concerns:
        //:  1 The metafunction returns 'false' for non-trivial user-defined
        //:    types without a no-throw move constructor.
        //:
        //:  2 The metafunction returns 'false' types with deleted (or private)
        //:    copy and move constructors, even when declared 'noexcept'.
        //:
        //:  3 For a user-defined trivially copyable class type that is
        //:    correctly associated with either of the C++03 trait
        //:    customization facilities, both the native trait oracle and the
        //:    'bsl' trait return 'true' for the potentially const-qualified
        //:    type, and 'false' for volatile-qualified versions of that type.
        //:
        //:  4 For a user-defined type with a move constructor marked as
        //:    'noexcept' in C++11, which is properly associated with the
        //:    'bsl::is_nothrow_move_constructible' trait, both the native
        //:    oracle  and the 'bsl' trait are defined to inherit from
        //:    'true_type' for the non-cv-qualified type, and 'false_type' for
        //:    cv-qualified versions of that type.
        //:
        //:             // TYPES THAT DISAGREE BETWEEN C++03 AND C++11
        //:
        //:  5 For a user-defined trivially copyable class type that is not
        //:    associated with either of the C++03 trait customization
        //:    facilities, the trait returns 'false' for all cv-qualified
        //:    variations of this type in C++03, but correctly deduces 'true'
        //:    for potentially 'const' (but not 'volatile') qualified versions
        //:    of this type in C++11.
        //:
        //:  6 For a user-defined type with a move constructor marked as
        //:    'noexcept' in C++11, which is properly associated with the
        //:    'bsl::is_nothrow_move_constructible' trait, both the native
        //:    oracle  and the 'bsl' trait are defined to inherit from
        //:    'true_type' for the non-cv-qualified type, and 'false_type' for
        //:    cv-qualified versions of that type.
        //:
        //:  7 For a user-defined type with a copy constructor marked as
        //:    'noexcept' in C++11 and no declared move constructor, which is
        //:    properly associated with the 'is_nothrow_move_constructible'
        //:    trait, both the native oracle and the 'bsl' trait are defined to
        //:    inherit from 'true_type' for the non-cv-qualified type, and
        //:    'false_type' for cv-qualified versions of that type.
        //:
        //:  8 For a user-defined type with a move constructor not marked as
        //:    'noexcept' in C++11, and which is improperly associated with the
        //:    'bsl::is_nothrow_move_constructible' trait, the 'bsl' trait will
        //:    return 'true' in both C++03 and C++11, but the native oracle
        //:    will return 'false'.
        //:
        //:  9 Traits apply equally to unions as to classes.  Note that unions
        //:    are known to cause problems in certain template metaprograms if
        //:    they rely on testing with mix-in inheritance, and this concern
        //:    is simply that this implementation does not have those problems.
        //:
        //: 10 For array of any of these types, the metafunction always returns
        //:   'false'.
        //
        // Plan:
        //:  1 Create a set of representative class types for all scenarios
        //:    where C++03 and C++11 compilers should agree:
        //:    o Simple types without any of the traits (C-1)
        //:    o Types with inaccessible copy and move constructors that do not
        //:      satisfy the traits (C-2)
        //:    o Trivial types that correctly associate with the C++03 trait
        //:      (C-3)
        //:    o Non-trivial types that associate with the trait, AND associate
        //:      explicitly with the trait (C-4)
        //:
        //:  2 Create a set of representative class types for all scenarios
        //:    where C++03 and C++11 compilers should disagree:
        //:    o Simple types without any of the traits
        //:
        //:  3 For each category of type in concerns sets 1 and 2, create a
        //:    similar union type.
        //:
        //:  4 For each type in steps 1-3, use the appropriate test macro for
        //:    confirm the correct result.
        //
        // Testing:
        //   EXTENDING 'bsl::is_nothrow_move_constructible'
        // --------------------------------------------------------------------

        if (verbose)
                  printf("\nEXTENDING 'bsl::is_nothrow_move_constructible'"
                         "\n==============================================\n");

        // C-0: Verify that the trait tests do not give different answers for
        // fundamental types.  This is testing the test machinery, as this test
        // concern was proven in test case 1 of
        // 'bslmf_isnothrowmoveconstructible.t.cpp'.

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(char, true );
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE(void, false);

        // C-1: Verify that the trait tests do not give different answers for
        // potentially-throwing types that do not customize our traits.

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(UserDefinedThrowTestType,
                                                   false);

        // C-2

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(ImmovableClass, false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(ImmovableUnion, false);

        // C-3

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(
                                                      NestedTraitTrivialClass);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(
                                                      NestedTraitTrivialUnion);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(TrivialClass);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(TrivialUnion);

        // C-4

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(NothrowMovableTestClass,
                                                   true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(NothrowMovableTestUnion,
                                                   true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                                NestedTraitNothrowMovableClass,
                                                true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                                NestedTraitNothrowMovableUnion,
                                                true);

        // C-5
#if defined(BSLMF_MOVABLEREF_USE_NATIVE_ORACLE)                               \
 || defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
        // MSVC 2013 is an awkward platform, supporting mostly accurate native
        // traits, but not the 'noexcept' operator.  In particular, it will
        // give the correct (noexcept) answer for trivial classes, but does not
        // guarantee to correctly identify a trivial class with the native
        // trivial traits.

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(
                                                    UnspecializedTrivialClass);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_TRIVIAL_CLASS(
                                                    UnspecializedTrivialUnion);
#else
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(UnspecializedTrivialClass,
                                                   false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(UnspecializedTrivialClass,
                                                   false);
#endif

        // C-6

#if defined(BSLMF_MOVABLEREF_USE_NATIVE_ORACLE)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                              UnspecializedNothrowMovableClass,
                                              true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                              UnspecializedNothrowMovableUnion,
                                              true);
#else
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                              UnspecializedNothrowMovableClass,
                                              false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                              UnspecializedNothrowMovableUnion,
                                              false);
#endif

        // C-7

#if defined(BSLMF_MOVABLEREF_USE_NATIVE_ORACLE)
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(
                                             UnspecializedNothrowCopyableClass,
                                             true,
                                             true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CV_TEST(
                                             UnspecializedNothrowCopyableUnion,
                                             true,
                                             true);
#else
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                             UnspecializedNothrowCopyableClass,
                                             false);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                             UnspecializedNothrowCopyableUnion,
                                             false);
#endif

        // C-8

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(LyingMovableClass, true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(LyingMovableUnion, true);

        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                                  LyingNestedTraitMovableClass,
                                                  true);
        ASSERT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_CLASS(
                                                  LyingNestedTraitMovableClass,
                                                  true);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MOVABLEREF<TYPE> VS. RVALUE
        //
        // Concerns:
        //: 1 A function declared to take a 'MovableRef<TYPE>' as argument can
        //:   be called with a temporary of type 'TYPE' when using a C++11
        //:   implementation.
        //: 2 A function can be overloaded for 'const TYPE&', 'TYPE&', and
        //:   'MovableRef<TYPE>' and be called appropriately.
        //: 3 A function taking a parameter of type
        //:   'BSLS_COMPILERFEATURES_FORWARD_REF(TYPE)' can be called with an
        //:   argument of type 'MovableRef<SomeType>'.
        //
        // Plan:
        //: 1 Call an overloaded function taking a 'MovableRef<TYPE>' argument
        //:   as well as a 'TYPE&' and a 'const TYPE&' and verify that the
        //:   correct overload is called.  For a C++11 implementation the
        //:   overload taking a 'MovableRef<TYPE>' has to be called otherwise
        //:   one of the two lvalue overloads is called.
        //: 2 Calling the overloaded function from the previous item and
        //:   confirming the result also addresses this concern.
        //: 3 Call a function template taking a
        //:   'BSLS_COMPILERFEATURES_FORWARD_REF(TYPE)', passing lvalues as
        //:   well as the result of 'MovableRefUtil::move'.
        //
        // Testing:
        //   bslmf::MovableRef<TYPE> VS. RVALUE
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVABLEREF<TYPE> VS. RVALUE"
                            "\n===========================\n");

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        ASSERT(TestMovableRefArgument<int>::test(int(18)));
        ASSERT(TestMovableRefArgument<Vector<int> >::test(Vector<int>()));
        ASSERT(TestMovableRefArgument<TestMoving>::test(TestMoving()));
#else
        ASSERT(!TestMovableRefArgument<int>::test(int(18)));
        ASSERT(!TestMovableRefArgument<Vector<int> >::test(Vector<int>()));
        ASSERT(!TestMovableRefArgument<TestMoving>::test(TestMoving()));
#endif
        TestMoving tm;
        ASSERT(testForwardRefArgument(bslmf::MovableRefUtil::move(tm)));

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'MovableRefUtil::access'
        //
        // Concerns:
        //: 1 'MovableRefUtil::access()' yields an lvalue reference the object
        //:   referenced by an lvalue or a (possibly const) 'MovableRef<TYPE>'.
        //
        // Plan:
        //: 1 Call a function taking a 'MovableRef<TYPE>' argument and verify
        //:   that the result of 'MovableRefUtil::access()' can be bound to an
        //:   lvalue reference with the same address as the original object.
        //
        // Testing:
        //   TYPE& MovableRefUtil::access(TYPE& lvalue);
        //   TYPE& MovableRefUtil::access(MovableRef<TYPE> lvalue);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'MovableRefUtil::access'"
                            "\n================================\n");
        {
            int  value(19);
            int *address(MovableAddress<int>::get(
                                          bslmf::MovableRefUtil::move(value)));

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            const
#endif
            bslmf::MovableRef<int> ref(bslmf::MovableRefUtil::move(value));

            ASSERT(&value == address);
            ASSERT(&value == &bslmf::MovableRefUtil::access(value));
            ASSERT(&value == &bslmf::MovableRefUtil::access(ref));
            ASSERT(&value == &bslmf::MovableRefUtil::access(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(&value == &bslmf::MovableRefUtil::access(ref));
        }
        {
            Vector<int>  value;
            Vector<int> *address(MovableAddress<Vector<int> >::get(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(&value == address);
            ASSERT(&value == &bslmf::MovableRefUtil::access(value));
        }
        {
            const int  value(19);
            const int *address(MovableAddress<const int>::get(
                                          bslmf::MovableRefUtil::move(value)));

            ASSERT(&value == address);
            ASSERT(&value == &bslmf::MovableRefUtil::access(value));
            ASSERT(&value == &bslmf::MovableRefUtil::access(
                                          bslmf::MovableRefUtil::move(value)));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'MovableRef<TYPE>::operator TYPE&'
        //
        // Concerns:
        //: 1 (Possibly const) 'MovableRef<TYPE>' converts to an lvalue
        //:   reference of type 'TYPE&' and that the address of the referenced
        //:   object is identical to the address of the original object.
        //: 2 If 'TYPE' is const, then the resulting lvalue is const.
        //
        // Plan:
        //: 1 Use 'MovableRefUtil::move()' with an lvalue, initialize a
        //:   'MovableRef<T>', convert the result to an lvalue reference, and
        //:   check that the addresses are identical.
        //: 2 Repeat step 1 with const 'T' types.
        //
        // Testing:
        //   MovableRef<TYPE>::operator TYPE&() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'MovableRef<TYPE>::operator TYPE&'"
                            "\n==========================================\n");

        {
            int                    value(17);
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            const
#endif
            bslmf::MovableRef<int> rvalue(bslmf::MovableRefUtil::move(value));
            int&                   lvalue(rvalue);
            ASSERT(&value == &lvalue);
        }
        {
            Vector<int>                     value;
            bslmf::MovableRef<Vector<int> > rvalue(
                                           bslmf::MovableRefUtil::move(value));
            Vector<int>&                    lvalue(rvalue);
            ASSERT(&value == &lvalue);
        }
        {
            TestMoving                    value;
            bslmf::MovableRef<TestMoving> rvalue(
                                           bslmf::MovableRefUtil::move(value));
            TestMoving&                   lvalue(rvalue);
            ASSERT(value.getAddress() == lvalue.getAddress());
        }
        {
            const int                    value(17);
            bslmf::MovableRef<const int> rvalue(
                                           bslmf::MovableRefUtil::move(value));
            const int&                   lvalue(rvalue);
            ASSERT(&value == &lvalue);
        }
      } break;
      case 5: {
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
        //: 5 When compiling with a C++11 implementation (that supports
        //:   'constexpr') the result of 'MovableRefUtil::move()' can be used
        //:   in a 'constexpr' context.
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
        //: 5 If 'constexpr' is supported, use 'MovableRefUtil::move()' with an
        //:   rvalue and initialize a 'constexpr' variable with the result.
        //:   Note that the 'constexpr' keyword is used deliberately, rather
        //:   than a substitution macro, to make it clear what we are testing.
        //
        // Testing:
        //      MovableRef<TYPE> MovableRefUtil::move(TYPE& lvalue);
        //      MovableRef<RemoveRef<T>::type> move(MovableRef<T> ref);
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
            Vector<int>            value;
            ASSERT(TestMovableRefArgument<Vector<int> >::test(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(TestMovableRefArgument<Vector<int> >::test(
             bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::move(value))));
        }
        {
            TestMoving            value;
            ASSERT(TestMovableRefArgument<TestMoving>::test(
                                          bslmf::MovableRefUtil::move(value)));
            ASSERT(TestMovableRefArgument<TestMoving>::test(
             bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::move(value))));
        }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
        {
            constexpr int value(bslmf::MovableRefUtil::move(42));
            ASSERT(value == 42);
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TRANSFORMATION TRAITS
        //
        // Concerns:
        //: 1 'MovableRefUtil::RemoveReference<TYPE>::type' evaluates to
        //:   'TYPE' if 'TYPE' is not a reference and to the type it refers to
        //:   if 'TYPE' is a reference.
        //: 2 'MovableRefUtil::AddLvalueReference<TYPE>::type' evaluates to
        //:   'TYPE&' if 'TYPE' is not a reference and to 'T1&' if 'TYPE' is a
        //:   (movable or lvalue) reference to 'T1'.
        //: 3 'MovableRefUtil::AddMovableReference<TYPE>::type' evaluates to
        //:   'MovableRef<TYPE>' if 'TYPE' is not a reference, 'T1&' if 'TYPE'
        //:   is an lvalue reference to 'T1', and 'MovableRef<T1>' if 'TYPE'
        //:   is a movable reference to 'T1'.
        //: 4 In C++03, 'MovableRef<TYPE>' is an object type that simulates a
        //:   reference type.  Ensure that these three traits work correctly
        //:   for 'const MovableRef<TYPE>' and reference to 'MovableRef' types.
        //: 5 'MovableRefUtil::Decay<TYPE>::type' evaluates to the same
        //:   type as 'bsl::decay<TYPE>::type' if 'bsl::decay' also treated
        //:   'MovableRef<T>' as a (movable) reference-qualified T.
        //
        // Plan:
        //: 1 For concerns 1 through 4, use a compile-time table where the
        //:   first column is the type to test and the other three columns are
        //:   the expected types of the three reference addition and removal
        //:   traits.  Each row tests a different type.  Test that each trait
        //:   evaluation is the same as the expected type for that column.
        //:
        //: 2 For concern 5, use a compile-time table where the first column
        //:   is the type to test and the second column is the expected type
        //:   of 'MovableRefUtil::Decay'.  Test that the trait evaluation is
        //:   the same as the expected type.
        //
        // Testing:
        //      MovableRefUtil::RemoveReference<TYPE>
        //      MovableRefUtil::AddLvalueReference<TYPE>
        //      MovableRefUtil::AddMovableReference<TYPE>
        //      MovableRefUtil::Decay<TYPE>
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTRANSFORMATION TRAITS"
                   "\n=====================\n");

        typedef bslmf::MovableRefUtil Util;

        if (veryVerbose)
            printf("\nRemoveRef, AddLvalueRef, AddMovableRef"
                   "\n--------------------------------------");

#define MR bslmf::MovableRef

#define TEST(T, REMREF, ADDLVR, ADDMR) do {                                   \
        ASSERTV(#T, (bsl::is_same<REMREF,                                     \
                                  Util::RemoveReference<T >::type>::value));  \
        ASSERTV(#T, (bsl::is_same<ADDLVR,                                     \
                                Util::AddLvalueReference<T >::type>::value)); \
        ASSERTV(#T, (bsl::is_same<ADDMR,                                      \
                               Util::AddMovableReference<T >::type>::value)); \
    } while (false)

        typedef const void          CVoid;
        typedef volatile void       VVoid;
        typedef const volatile void CVVoid;

        //   TYPE                  RemRef      AddLvRef     AddMovRef
        //   --------------------  ----------  -----------  --------------
        TEST(void                , void      , void       , void          );
        TEST(CVoid               , CVoid     , CVoid      , CVoid         );
        TEST(VVoid               , VVoid     , VVoid      , VVoid         );
        TEST(CVVoid              , CVVoid    , CVVoid     , CVVoid        );
        TEST(int                 , int       , int&       , MR<int>       );
        TEST(const int           , const int , const int& , MR<const int> );
        TEST(TestMoving          , TestMoving, TestMoving&, MR<TestMoving>);
        TEST(int&                , int       , int&       , int&          );
        TEST(const int&          , const int , const int& , const int&    );
        TEST(TestMoving&         , TestMoving, TestMoving&, TestMoving&   );
        TEST(MR<int>             , int       , int&       , MR<int>       );
        TEST(MR<TestMoving>      , TestMoving, TestMoving&, MR<TestMoving>);
        TEST(MR<const int>       , const int , const int& , MR<const int> );
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        // Test reference-to-MovableRef only for C++03
        TEST(MR<int>&            , int       , int&       , MR<int>       );
        TEST(const MR<const int> , const int , const int& , MR<const int> );
        TEST(const MR<const int>&, const int , const int& , MR<const int> );
#endif

#undef TEST

        if (veryVerbose)
            printf("\nDecay"
                   "\n-----");

#define TEST(T, DECAY)                                                        \
    do {                                                                      \
        ASSERTV(#T, (bsl::is_same<DECAY, Util::Decay<T >::type>::value));     \
    } while (false)

        //   TYPE                           DECAYED TYPE
        //   -----------------------------  --------------------
        // void types
        TEST(void                         , void                );
        TEST(const void                   , void                );
        TEST(volatile void                , void                );
        TEST(const volatile void          , void                );

        // (example) object types
        TEST(int                          , int                 );
        TEST(int&                         , int                 );
        TEST(MR<int>                      , int                 );
        TEST(const int                    , int                 );
        TEST(const int&                   , int                 );
        TEST(MR<const int>                , int                 );
        TEST(volatile int                 , int                 );
        TEST(volatile int&                , int                 );
        TEST(MR<volatile int>             , int                 );
        TEST(const volatile int           , int                 );
        TEST(const volatile int&          , int                 );
        TEST(MR<const volatile int>       , int                 );

        // function types
        TEST(void ()                      , void (*)()          );
        TEST(void (&)()                   , void (*)()          );
        TEST(MR<void ()>                  , void (*)()          );

        // pointer-to-function types
        TEST(void (*)()                   , void (*)()          );
        TEST(void (*&)()                  , void (*)()          );
        TEST(MR<void (*)()>               , void (*)()          );
        TEST(void (* const)()             , void (*)()          );
        TEST(void (* const&)()            , void (*)()          );
        TEST(MR<void (* const)()>         , void (*)()          );
        TEST(void (* volatile)()          , void (*)()          );
        TEST(void (* volatile&)()         , void (*)()          );
        TEST(MR<void (* volatile)()>      , void (*)()          );
        TEST(void (* const volatile)()    , void (*)()          );
        TEST(void (* const volatile&)()   , void (*)()          );
        TEST(MR<void (* const volatile)()>, void (*)()          );

        // pointer types
        TEST(int *                        , int *               );
        TEST(int *&                       , int *               );
        TEST(MR<int *>                    , int *               );
        TEST(const int *                  , const int *         );
        TEST(const int *&                 , const int *         );
        TEST(MR<const int *>              , const int *         );
        TEST(volatile int *               , volatile int *      );
        TEST(volatile int *&              , volatile int *      );
        TEST(MR<volatile int *>           , volatile int *      );
        TEST(const volatile int *         , const volatile int *);
        TEST(const volatile int *&        , const volatile int *);
        TEST(MR<const volatile int *>     , const volatile int *);

        // array types with extent
        TEST(int[10]                      , int *               );
        TEST(int(&)[10]                   , int *               );
        TEST(MR<int[10]>                  , int *               );
        TEST(const int[10]                , const int *         );
        TEST(const int(&)[10]             , const int *         );
        TEST(MR<const int[10]>            , const int *         );
        TEST(volatile int[10]             , volatile int *      );
        TEST(volatile int(&)[10]          , volatile int *      );
        TEST(MR<volatile int[10]>         , volatile int *      );
        TEST(const volatile int[10]       , const volatile int *);
        TEST(const volatile int(&)[10]    , const volatile int *);
        TEST(MR<const volatile int[10]>   , const volatile int *);

        // array types without extent
        TEST(int[]                        , int *               );
        TEST(int(&)[]                     , int *               );
        TEST(MR<int[]>                    , int *               );
        TEST(const int[]                  , const int *         );
        TEST(const int(&)[]               , const int *         );
        TEST(MR<const int[]>              , const int *         );
        TEST(volatile int[]               , volatile int *      );
        TEST(volatile int(&)[]            , volatile int *      );
        TEST(MR<volatile int[]>           , volatile int *      );
        TEST(const volatile int[]         , const volatile int *);
        TEST(const volatile int(&)[]      , const volatile int *);
        TEST(MR<const volatile int[]>     , const volatile int *);

#undef TEST
#undef MR

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PREDICATE TRAITS
        //
        // Concerns:
        //: 1 'MovableRefUtil::IsLvalueReference<TYPE>::value' evaluates to
        //:   true iff 'TYPE' is an lvalue reference.
        //: 2 'MovableRefUtil::IsMovableReference<TYPE>::value' evaluates to
        //:   true iff 'TYPE' is an rvalue reference (C++11 and later) or a
        //:   specialization of 'MovableRef' (C++03)'.
        //: 3 'MovableRefUtil::IsReference<TYPE>::value' evaluates to true iff
        //:   either 'IsLvalueReference<TYPE>' or 'IsMovableReference<TYPE>'
        //:   are true.
        //: 4 In C++03, 'MovableRef<TYPE>' is an object type that simulates a
        //:   reference type.  Ensure that these three traits work correctly
        //:   for 'const MovableRef<TYPE>' and reference to 'MovableRef' types.
        //
        // Plan:
        //: 1 Use a compile-time table where the first column is the type to
        //:   test and the other tree columns are the expected values of the
        //:   three traits being tested here.  Each row tests a different
        //:   type.  Test that each trait results in the same value as the
        //:   expected value for that column.
        //
        // Testing:
        //      MovableRefUtil::IsLvalueReference<TYPE>
        //      MovableRefUtil::IsMovableReference<TYPE>
        //      MovableRefUtil::IsReference<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\nPREDICATE TRAITS"
                            "\n================\n");

        typedef bslmf::MovableRefUtil Util;

#define TEST(T, IS_LVREF, IS_MREF, IS_REF) do {                          \
        ASSERTV(#T, ! (IS_LVREF && IS_MREF));                            \
        ASSERTV(#T, IS_REF == (IS_LVREF || IS_MREF));                    \
        ASSERTV(#T, IS_LVREF == (Util::IsLvalueReference<T >::value));   \
        ASSERTV(#T, IS_MREF == (Util::IsMovableReference<T >::value));   \
        ASSERTV(#T, IS_REF == (Util::IsReference<T >::value));           \
    } while (false)

        //   TYPE                                  IsLv   IsMov  IsRef
        //   -----------------------------------   -----  -----  -----
        TEST(void                                , false, false, false);
        TEST(const void                          , false, false, false);
        TEST(volatile void                       , false, false, false);
        TEST(const volatile void                 , false, false, false);
        TEST(int                                 , false, false, false);
        TEST(const int                           , false, false, false);
        TEST(TestMoving                          , false, false, false);
        TEST(int&                                , true , false, true );
        TEST(const int&                          , true , false, true );
        TEST(TestMoving&                         , true , false, true );
        TEST(bslmf::MovableRef<int>              , false, true , true );
        TEST(bslmf::MovableRef<TestMoving>       , false, true , true );
        TEST(bslmf::MovableRef<const int>        , false, true , true );
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        // Test reference-to-MovableRef only for C++03
        TEST(bslmf::MovableRef<int>&             , false, true , true );
        TEST(const bslmf::MovableRef<const int>  , false, true , true );
        TEST(const bslmf::MovableRef<const int>& , false, true , true );
#endif

#undef TEST

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'MovableRef<TYPE>'
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

        if (verbose) printf("\n'MovableRef<TYPE>'"
                            "\n==================\n");

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        ASSERT((bsl::is_same<bslmf::MovableRef<int>, int&&>::value));
        ASSERT((bsl::is_same<bslmf::MovableRef<Vector<int> >,
                                                       Vector<int>&&>::value));
        ASSERT((bsl::is_same<bslmf::MovableRef<TestMoving>,
                                                        TestMoving&&>::value));
#else
        ASSERT(bsl::is_class<bslmf::MovableRef<int> >::value);
        ASSERT(bsl::is_class<bslmf::MovableRef<Vector<int> > >::value);
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
        int&                   lvalue2(bslmf::MovableRefUtil::access(
                                          bslmf::MovableRefUtil::move(value)));

        ASSERT(&reference == &value);
        ASSERT(&reference == &lvalue0);
        ASSERT(&reference == &lvalue1);
        ASSERT(&reference == &lvalue2);

        typedef bslmf::MovableRefUtil Util;

        ASSERT(!Util::IsLvalueReference<int>::value);
        ASSERT(!Util::IsMovableReference<int>::value);
        ASSERT(!Util::IsReference<int>::value);
        ASSERT( Util::IsLvalueReference<int&>::value);
        ASSERT(!Util::IsMovableReference<int&>::value);
        ASSERT( Util::IsReference<int&>::value);
        ASSERT(!Util::IsLvalueReference<bslmf::MovableRef<int> >::value);
        ASSERT( Util::IsMovableReference<bslmf::MovableRef<int> >::value);
        ASSERT( Util::IsReference<bslmf::MovableRef<int> >::value);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
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
