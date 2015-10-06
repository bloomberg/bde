// bdlb_functionoutputiterator.t.cpp                                  -*-C++-*-
#include <bdlb_functionoutputiterator.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_algorithm.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test-driver tests two mostly orthogonal sets of concerns:
//: 1 That features intrinsic to an STL output iterator are implemented
//:   correctly.  I.e., type traits, pre- and post-increment operators,
//:   assignment to the dereferenced iterator.
//:
//: 2 That bdlb::FunctionOutputIterator can be correctly created with either
//:   a function pointer or functor.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] FunctionOutputIterator();
// [ 2] explicit FunctionOutputIterator<FUNCTION>(const FUNCTION&);
// [ 4] FunctionOutputIterator(const FunctionOutputIterator&);
// [ 2] ~FunctionOutputIterator();
//
// MANIPULATORS
// [ 5] FunctionOutputIterator& operator=(const FunctionOutputIterator&);
// [ 2] AssignmentProxy operator*();
//
// FREE OPERATORS
// [ 7] FunctionOutputIterator& operator++(FunctionOutputIterator&);
// [ 7] FunctionOutputIterator operator++(FunctionOutputIterator&, int);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 6] BSL Traits

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose = 0;
static bool veryVerbose = 0;
static bool veryVeryVerbose = 0;

template<class TYPE>
class Value {
    // This class provides a facility for tracking any activities with objects
    // of local class 'Setter'.

  public:
    // LOCAL TYPE
    typedef TYPE value_type;

    // CLASS DATA
    static Value singleton;  // static object using for default construction of
                             // local class 'Setter' object

    class Setter {
        // This class provides a facility for tracking constructors and
        // available operators call.

        // CLASS DATA
        static int instanceCount;  // number of existing class objects

        // DATA
        const int  d_id;       // unique identifier of object
        Value     *d_value_p;  // pointer to actions tracker

      public:
        // CREATORS
        Setter()
            // Create a 'Setter' object with 'Value' singleton used as tracker
            // and notify tracker about creation.
        : d_id(++instanceCount)
        , d_value_p(&singleton)
        {
            d_value_p->setLastCreatedSetterId(d_id);
        };

        Setter(const Setter& original)
            // Create a 'Setter' object having the value of the specified
            // 'original' object and notify tracker about creation.
        : d_id(++instanceCount)
        , d_value_p(original.d_value_p)
        {
            d_value_p->setLastCreatedSetterId(d_id);
        }

        explicit Setter(Value *value)
            // Create a 'Setter' object with the specified 'value' object used
            // as tracker and notify tracker about creation.
        : d_id(++instanceCount)
        , d_value_p(value)
        {
            d_value_p->setLastCreatedSetterId(d_id);
        };

        // MANIPULATORS
        Setter& operator=(const Setter& rhs)
            // Assign to this object the value of the specified 'rhs' setter,
            // and return a reference providing modifiable access to this
            // object.
        {
            d_value_p = rhs.d_value_p;
            return *this;
        }

        void operator()(const TYPE& data)
            // Update the tracker with the specified 'data' and notify it about
            // object invocation.
        {
            d_value_p->set(data);
            d_value_p->setLastCalledSetterId(d_id);
        };

        // ACCESSORS
        int getId() const { return d_id; }
            // Return object identifier.
    };

  private:
    // DATA
    TYPE d_data;                // object identifier
    int  d_lastCalledSetterId;  // identifier of the last called tracked object
    int  d_lastCreatedSetterId; // identifier of the last created tracked
                                // object

  public:
    // CREATORS
    Value() : d_data() {}
        // Create a 'Value' object and call default constructor for identifier.

    explicit Value(const TYPE& data) : d_data(data) {}
        // Create a 'Value' object and call value constructor for identifier
        // with the specified 'data' as a parameter.

    // MANIPULATORS
    Setter createSetter() { return Setter(this); }
        // Create a 'Setter' class instance as an object to track.

    void set(const TYPE& data) { d_data = data; }
        // Set the specified 'data' as an identifier.

    void setLastCalledSetterId(int id) { d_lastCalledSetterId = id; }
        // Update the identifier of the last called tracked object with the
        // specified 'id' value.

    void setLastCreatedSetterId(int id) { d_lastCreatedSetterId = id; }
        // Update the identifier of the last created tracked object with the
        // specified 'id' value.

    // ACCESSORS
    TYPE get() const { return d_data; }
        // Return object identifier.

    int getLastCalledSetterId() const { return d_lastCalledSetterId; }
        // Return identifier of the last called tracked object.

    int getLastCreatedSetterId() const { return d_lastCreatedSetterId; }
        // Return identifier of the last called tracked object.
};

template<class TYPE>
Value<TYPE> Value<TYPE>::singleton;

template<class TYPE>
int Value<TYPE>::Setter::instanceCount = 0;

namespace {

int simpleFunctionValue = 0;

void simpleFunction(int value)
{
    simpleFunctionValue = value;
}

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Supplying a Free-Function
/// - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'bdlb::FunctionOutputIterator' with a
// free-function.  Consider we have a function 'foo' that prints integers in
// some predefined format, and we would like to print out all unique elements
// of sorted 'array' collection.
//
// First, we define that function foo:
//..
typedef void (*Function)(const int&);
void foo(const int& value)
{
    if (veryVerbose)
    cout << value << " ";
}
//..
//
///Example 2: Supplying a User Defined Functor
///- - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a user defined functor object.  Consider the 'Accumulator' class for
// accumulating integer values into a total.  We want to adapt 'Accumulator'
// for use with the algorithm 'bsl::unique_copy'.
//
// First, we define an 'Accumulator' class that will total the values supplied
// to the 'increment' method:
//..
class Accumulator {
    // This class provides a value accumulating functionality.

    // DATA
    int d_sum;
  public:
    // CREATORS
    Accumulator() : d_sum(0) {};

    // MANIPULATORS
    void increment(int value) { d_sum += value; };

    // ACCESSORS
    int total() const { return d_sum; }
};
//..
// Next, we define a functor, 'AccumulatorFunctor', that adapts 'Accumulator'
// to a function object:
//..
class AccumulatorFunctor {
    // This class implements a function object that invokes 'increment' in
    // response of calling operator()(int).

    // DATA
    Accumulator *d_accumulator_p;  // accumulator (held, not owned)

  public:
    // CREATORS
    explicit AccumulatorFunctor(Accumulator *accumulator)
    : d_accumulator_p(accumulator)
    {}

    // MANIPULATORS
    void operator()(int value) { d_accumulator_p->increment(value); };
//..
};

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;

    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                           << "=============" << endl;

// We define a data sequence to process:
//..
    enum { NUM_VALUES_1 = 7 };
    const int array1[NUM_VALUES_1] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Next, we use 'bdlb::FunctionOutputIterator' to wrap 'foo' for use in the
// algorithm 'bsl::unqiue_copy':
//..
    unique_copy(
        array1,
        array1 + NUM_VALUES_1,
        bdlb::FunctionOutputIterator<Function>(&foo));
//..
// Notice, that each time 'bsl::unique_copy' copies an element from the
// supplied range and assigns it to the output iterator, the function 'foo' is
// called for the element.
//
// Finally, the resulting console output:
//..
//  2 3 5 7 11
//..
// Then, we define another data sequence to process:
//..
    enum { NUM_VALUES_2 = 7 };
    const int   array2[NUM_VALUES_2] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Next, we create a 'bdlb::FunctionOutputIterator' for 'AccumulatorFunctor'
// and supply it to the 'bsl::unique_copy' algorithm to accumulate a sequence
// of values:
//..
    Accumulator accumulator;
    unique_copy(
        array2,
        array2 + NUM_VALUES_2,
        bdlb::FunctionOutputIterator<AccumulatorFunctor>(
            AccumulatorFunctor(&accumulator)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
    ASSERT(28 == accumulator.total());
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PRE- AND POST-INCREMENT OPERATORS
        //   Ensure that pre-increment and post-increment operators are defined
        //   and do not affect iterator behavior.
        //
        // Concerns:
        //: 1 Pre-increment and post-increment operators are declared and
        //:   defined so they could be called by client code.
        //:
        //: 2 Iterator preserves its behavior after increment operator
        //:   invocation.
        //
        // Plan:
        //: 1 Create iterator object.
        //:
        //: 2 Invoke iterator pre-increment operator.  (C-1)
        //:
        //: 3 Check that functor is invoked on assignment of dereferenced
        //:   iterator.  (C-2)
        //:
        //: 4 Invoke iterator post-increment operator.  (C-1)
        //:
        //: 5 Check that functor is invoked on assignment of dereferenced
        //:   iterator.  (C-2)
        //
        // Testing:
        //   FunctionOutputIterator& operator++(FunctionOutputIterator&);
        //   FunctionOutputIterator operator++(FunctionOutputIterator&, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRE- AND POST-INCREMENT OPERATORS" << endl
                          << "=================================" << endl;

        const int initialValue =  1;
        const int theValue1    = 17;
        const int theValue2    = 19;
        const int theValue3    = 23;

        typedef Value<int> IntValue;
        typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;
        IntValue value(initialValue);

        Iterator it(value.createSetter());
        *it = theValue1;

        const int setterId = value.getLastCalledSetterId();

        ++it;

        *it = theValue2;
        ASSERT(theValue2 == value.get());
        ASSERT(setterId == value.getLastCalledSetterId());

        it++;

        *it = theValue3;
        ASSERT(theValue3 == value.get());
        ASSERT(setterId == value.getLastCalledSetterId());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BSL TRAITS
        //   Ensure that bdlb::FunctionOutputIterator defines required traits.
        //
        // Concerns:
        //: 1 The following types (traits) are defined in
        //:   bdlb::FunctionOutputIterator:
        //:     iterator_category
        //:     value_type
        //:     difference_type
        //:     pointer
        //:     reference
        //
        // Plan:
        //: 1 Check that the required types are defined.
        //
        // Testing:
        //   BSL Traits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSL TRAITS" << endl
                          << "==========" << endl;

        typedef bdlb::FunctionOutputIterator<int> Obj;
        typedef bsl::iterator_traits<Obj>         ObjTraits;

        // iterator_category == output_iterator_tag
        BSLMF_ASSERT((bsl::is_same<ObjTraits::iterator_category,
                                   bsl::output_iterator_tag>::value));

        // value_type == void
        BSLMF_ASSERT((bsl::is_same<ObjTraits::value_type, void>::value));

        // difference_type == void
        BSLMF_ASSERT((bsl::is_same<ObjTraits::difference_type, void>::value));

        // pointer == void
        BSLMF_ASSERT((bsl::is_same<ObjTraits::pointer, void>::value));

        // reference == void
        BSLMF_ASSERT((bsl::is_same<ObjTraits::reference, void>::value));
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        //   Ensure that the functional object of the original iterator
        //   will be invoked on assignment to dereferenced copy.
        //
        //   Note that this operation is currently supplied by the compiler.
        //
        // Concerns:
        //: 1 L-value iterator is in appropriate state after assignment.
        //:
        //: 2 R-value iterator can be 'const'.
        //:
        //: 3 After assignment L-value iterator holds a copy of functional
        //:   object from the r-value iterator.
        //:
        //: 4 After assignment l-value iterator invokes functional object.
        //
        // Plan:
        //: 1 Create original iterator object.
        //:
        //: 2 Create another iterator object and assign original iterator to
        //:   it.
        //:
        //: 3 Check that assignment operator makes a copy of the functional
        //:   object of the original iterator.  (C-1..3)
        //:
        //: 4 Check that functor is invoked on assignment of dereferenced
        //:   iterator.  (C-4)
        //
        // Testing:
        //   FunctionOutputIterator& operator=(const FunctionOutputIterator&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "ASSIGNMENT OPERATOR" << endl
                          << "===================" << endl;

        const int initialValue =  1;
        const int theValue1    = 17;
        const int theValue2    = 19;

        typedef Value<int> IntValue;
        typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;
        IntValue value(initialValue);

        // original iterator
        Iterator itOriginal(value.createSetter());

        *itOriginal = theValue1;

        const int originalSetterId = value.getLastCalledSetterId();

        // copy of the original iterator
        Iterator itCopy;
        itCopy = itOriginal;

        *itCopy = theValue2;

        // check that copy of the functional object 'setter' was actually
        // invoked
        ASSERT((originalSetterId + 1) == value.getLastCalledSetterId());

        // check that functional object was invoked and 'theValue2' was set to
        // 'value'
        ASSERT(theValue2 == value.get());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that the functional object of the original iterator (i.e.,
        //   argument of the copy ctor) will be invoked on assignment to
        //   dereferenced iterator.
        //
        //   Note that this operation is currently supplied by the compiler.
        //
        // Concerns:
        //: 1 The newly created object is in appropriate state.
        //:
        //: 2 The copy CONSTRUCTOR argument can be 'const'.
        //:
        //: 3 The newly created iterator holds a copy of functional object
        //:   from the iterator specified in the copy ctor.
        //:
        //: 4 The newly created iterator invokes functional object
        //
        // Plan:
        //: 1 Create original iterator object.
        //:
        //: 2 Create a copy of the original iterator object with copy CTOR.
        //:
        //: 3 Check that copy CTOR makes a copy of the functional object of the
        //:   original iterator (specified in the copy constructor).  (C-1..3)
        //:
        //: 4 Check that functor is invoked on assignment of dereferenced
        //:   iterator.  (C-4)
        //
        // Testing:
        //   FunctionOutputIterator(const FunctionOutputIterator&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        const int initialValue =  1;
        const int theValue1    = 17;
        const int theValue2    = 19;

        typedef Value<int> IntValue;
        typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;

        IntValue value(initialValue);

        // original iterator
        Iterator itOriginal(value.createSetter());
        const Iterator& ItOriginal = itOriginal;

        *itOriginal = theValue1;

        const int originalSetterId = value.getLastCalledSetterId();

        // copy of the original iterator
        Iterator itCopy(ItOriginal);

        *itCopy = theValue2;

        // check that copy of the functional object 'setter' was actually
        // invoked
        ASSERT((originalSetterId + 1) == value.getLastCalledSetterId());

        // check that 'theValue2' was set to 'value'
        ASSERT(theValue2 == value.get());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        //
        // Concerns:
        //: 1 Default ctor of iterator invokes default ctor of the functional
        //:   object.
        //:
        //: 2 Functional object created with default constructor is invoked
        //:   any way.
        //
        // Note that the iterator has no accessors to check attribute values
        // and no manipulators that change attribute values.
        //
        // Plan:
        //: 1 Create an object, using the default ctor.
        //:
        //: 2 Assign value to dereferenced iterator.
        //:
        //: 3 Check that corresponding functional object (singleton) was
        //:   invoked with correct value.
        //
        // Testing:
        //   FunctionOutputIterator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CONSTRUCTOR" << endl
                          << "===================" << endl;

        const int initialValue =  1;
        const int theValue     = 17;

        typedef Value<int> IntValue;
        IntValue::singleton.set(initialValue);

        // default CTOR
        bdlb::FunctionOutputIterator<IntValue::Setter> it;

        *it = theValue;

        // check that functional object was invoked and 'theValue' was set to
        // 'value'
        ASSERT(theValue == IntValue::singleton.get());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC METHODS
        //
        // Concerns:
        //: 1 Value constructor creates 'bdlb::FunctionOutputIterator' object
        //:   and calls copy constructor for template field 'd_function'.
        //:
        //: 2 Ctor argument can be 'const'.
        //:
        //: 3 Indirection operator returns an object that can appear on the
        //:   left-hand side of n assignment.
        //:
        //: 4 Value constructor applies function pointers as a parameter.
        //:
        //: 5 Objects can be destroyed.
        //
        // Plan:
        //: 1 Create an iterator using a const instance of the test type
        //:   'VALUE<int>' and 'VALUE<int>::Setter', dereference and assign a
        //:   value, use functions provided by the test type to verify that the
        //:   functor was copied and invoked correctly. (C-1..3).
        //:
        //: 2 Create an iterator and supply the test 'simpleFunction'
        //:   function. (C-4)
        //:
        //: 3 Verify the destructor is publicly accessible by allowing the
        //:   'bdlb::FunctionOutputIterator' object to leave scope and be
        //:   destroyed. (C-5)
        //
        //
        // Testing:
        //   explicit FunctionOutputIterator<FUNCTION>(const FUNCTION&);
        //   AssignmentProxy operator*();
        //   ~FunctionOutputIterator();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC METHODS"<< endl
                          << "=============" << endl;

        if (veryVerbose) {
            cout << "\tSupply a functor at construction" << endl;
        }
        {
            const int initialValue =  1;

            typedef Value<int> IntValue;

            IntValue               value(initialValue);
            const IntValue::Setter setter = value.createSetter();
            const int              originalSetterId =
                                                value.getLastCreatedSetterId();

            bdlb::FunctionOutputIterator<IntValue::Setter> it(setter);

            ASSERT(originalSetterId + 1 == value.getLastCreatedSetterId())
        }

        if (veryVerbose) {
            cout << "\tChecking indirection operator"
                      << endl;
        }
        {
            const int initialValue =  1;
            const int valueA       = 17;

            typedef Value<int> IntValue;
            IntValue value(initialValue);

            const IntValue::Setter setter = value.createSetter();

            bdlb::FunctionOutputIterator<IntValue::Setter> it(setter);

            *it = valueA;

            // check that copy of the functional object 'setter' was actually
            // invoked
            ASSERT((setter.getId() + 1) == value.getLastCalledSetterId());

            // check that 'valueA' was set to 'value'
            ASSERT(valueA == value.get());
        }

        if (veryVerbose) {
            cout << "\tSupply a function pointer at construction"
                      << endl;
        }
        {
            const int initialValue =  1;
            const int valueA       = 17;

            simpleFunctionValue = initialValue;

            bdlb::FunctionOutputIterator<void (*)(int)> it(&simpleFunction);

            *it = valueA;

            // check that function was invoked and 'simpleFunctionValue' was
            // set to 'valueA'.
            ASSERT(valueA == simpleFunctionValue);
        }
      } break;
      case 1: {
        // ----------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is functional enough to enable comprehensive
        //:   testing in subsequent cases
        //
        // Plan:
        //: 1 Create an object, using the ctor with functional object.
        //:
        //: 2 Assign value to dereferenced iterator.
        //:
        //: 3 Check that functional object was invoked with correct value.
        //
        // Testing:
        //   BREATHING TEST
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int initialValue =  1;
        const int theValue     = 17;

        typedef Value<int> IntValue;
        IntValue value(initialValue);

        bdlb::FunctionOutputIterator<IntValue::Setter>
                                                      it(value.createSetter());

        *it = theValue;

        // check that functional object was invoked and 'theValue' was set to
        // 'value'
        ASSERT(theValue == value.get());

      } break;
      default: {
         cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
         testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
