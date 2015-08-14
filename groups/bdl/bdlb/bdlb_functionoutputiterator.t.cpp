// bdlb_functionoutputiterator.t.cpp                                  -*-C++-*-
#include <bdlb_functionoutputiterator.h>

#include <bdlf_memfn.h>
#include <bdlf_function.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslmf_issame.h>

#include <bsl_iostream.h>
#include <bsl_algorithm.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test-driver tests two mostly orthogonal sets of concerns:
//: 1 That features intrinsic to an STL output iterator are implemented
//:   correctly.  I.e., type traits, pre- and post-increment operators,
//:   assignment to the dereferenced iterator.
//:
//: 2 That bdlb::FunctionOutputIterator can be correctly created with either
//:   a function pointer, bdlf::MemFnInstance, bdef_Function.
//
//=============================================================================
//
//                       // ----------------------------
//                       // bdlb::FunctionOutputIterator
//                       // ----------------------------
//
// CREATORS
// [ 3] bdlb::FunctionOutputIterator();
// [ 4] explicit bdlb::FunctionOutputIterator(const FUNCTION& function);
// [ 5] bdlb::FunctionOutputIterator(FunctionOutputIterator&);
// [ 3] ~bdlb::FunctionOutputIterator();
//
// MANIPULATORS
// [ 6] bdlb::FunctionOutputIterator& operator=(FunctionOutputIterator&);
// [ 4] AssignmentProxy operator*();
//
// FREE OPERATORS
// [ 7] bdlb::FunctionOutputIterator& operator++(FunctionOutputIterator&);
// [ 7] bdlb::FunctionOutputIterator operator++(FunctionOutputIterator&, int);
// ---------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 2] CONCERN: All 'output_iterator' traits are correctly defined.

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// Note assert and debug macros all output to cerr instead of cout, unlike
// most other test drivers.  This is necessary because test case 2 plays
// tricks with cout and examines what is written there.

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cerr << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X)  { aSsErT( !(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cerr << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cerr << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cerr << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cerr << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cerr << '\t' << flush;          // Print tab w/o newline.


//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose = 0;
static bool veryVerbose = 0;
static bool veryVeryVerbose = 0;

template<class TYPE>
class Value {
  public:
    typedef TYPE value_type;

    static Value singleton;

    class Setter {
        static int instanceCount;

        const int  d_id;
        Value     *d_value_p;
      public:
        Setter()
        : d_id(++instanceCount)
        , d_value_p(&singleton)
        {
        };

        Setter(const Setter& original)
        : d_id(++instanceCount)
        , d_value_p(original.d_value_p)
        {
        }

        explicit Setter(Value *value)
        : d_id(++instanceCount)
        , d_value_p(value)
        {
        };

        Setter& operator=(const Setter& rhs)
        {
            d_value_p = rhs.d_value_p;
            return *this;
        }

        int getId() const { return d_id; };
        void operator()(const TYPE& data)
        {
            d_value_p->set(data);
            d_value_p->setLastSetterId(d_id);
        };
    };

  private:
    // DATA
    TYPE d_data;
    int  d_lastSetterId;
  public:
    // CREATORS
    Value() : d_data() {};
    explicit Value(const TYPE& data) : d_data(data) {};
    // MANIPULATORS
    void set(const TYPE& data) { d_data = data; };
    void setLastSetterId(int id) { d_lastSetterId = id; };
    // ACCESSORS
    TYPE get() const { return d_data; };
    int getLastSetterId() const { return d_lastSetterId; };

    Setter createSetter() { return Setter(this); };
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
    bsl::cout << value << " ";
}
//..
// Next, we define a function, 'outputArray', in which we'll use the algorithm
// 'bsl::unique_copy' to call 'foo' on the unique elements in a sequence:
//..
void outputArray()
{
    enum { NUM_VALUES = 7 };
    const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Here, the algorithm 'unique_copy' iterates over all elements in supplied
// range, except consecutive duplicates, and copies them to the supplied
// output iterator.  We wrap the function 'foo' into a
// 'bdlb::FunctionOutputIterator' that we pass as an output iterator to the
// 'bsl::unique_copy' algorithm:
//..
    bsl::unique_copy(
        array,
        array + NUM_VALUES,
        bdlb::FunctionOutputIterator<Function>(&foo));
}
//..
// Notice that each time 'bsl::unique_copy' copies an element from the
// supplied range and assigns it to the output iterator the function 'foo' is
// called for the element.
//
// Finally, the resulting console output looks like:
//..
//  2 3 5 7 11
//..
//
///Example 2: Supplying a User Defined Functor
///- - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a user defined functor object.  Consider we have an 'Accumulator'
// class, for accumulating integer values into a total, and we want to adapt
// it to use with the algorithm 'bsl::unique_copy'.
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
//
    // MANIPULATORS
    void increment(int value) { d_sum += value; };
//
    // ACCESSORS
    int total() const { return d_sum; }
};
//..
// Next, we define a functor, 'AccumulatorFunctor', that adapts 'Accumulator'
// to a function object:
//..
//
class AccumulatorFunctor {
    // This class implements function object that invokes 'increment' in
    // response of calling operator()(int).

    // DATA
    Accumulator *d_accumulator_p;  // accumulator (held, not owned)
//
  public:
    // CREATORS
    explicit AccumulatorFunctor(Accumulator *accumulator)
    : d_accumulator_p(accumulator)
    {}
//
    // MANIPULATORS
    void operator()(int value) { d_accumulator_p->increment(value); };
};

// Now, we define a function 'accumulateArray' that will create a
// 'bdlb::FunctionOutputIterator' for 'AccumulatorFunctor' and supply it to
// the 'bsl::unique_copy' algorithm to accumulate a sequence of values:
//..
void accumulateArray()
{
    enum { NUM_VALUES = 7 };
    const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
    Accumulator accumulator;
    bsl::unique_copy(
        array,
        array + NUM_VALUES,
        bdlb::FunctionOutputIterator<AccumulatorFunctor>(
            AccumulatorFunctor(&accumulator)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
    ASSERT(28 == accumulator.total());
}
//..
//
///Example 3: use of bdlf::Function and bdlf::BindUtil::bind
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a functor created using 'bdlf::BindUtil'.  Consider the 'Accumulator'
// class defined in Example 2, which we want to adapt to accumulate a set of
// integer values.
//
// First, define an alias to a 'bdlf::Function', defining an accumulation
// functor, and and then define a function 'accumulateArray2' to accumulate the
// integer values in an array:
//..
typedef bdlf::Function<void (*)(int)> AccumulatorFunction;
    // Define function object
//
void accumulateArray2()
{
    enum { NUM_VALUES = 7 };
    const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
    Accumulator accumulator;
//..
// Here, we create a 'bdlb::FunctionOutputIterator' for a functor created using
// 'bdlf::BindUtil' (matching the 'AccumulatorFunction' alias), and supply
// it to the 'bsl::unique_copy' algorithm to accumulate a sequence of values:
//..
//
    bsl::unique_copy(
        array,
        array + NUM_VALUES,
        bdlb::FunctionOutputIterator<AccumulatorFunction>(
            bdlf::BindUtil::bind(
            &Accumulator::increment,
            &accumulator,
            bdlf::PlaceHolders::_1)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
    ASSERT(28 == accumulator.total());
}
//..

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
          if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                                 << "\n=============\n";

          outputArray();
          accumulateArray();
          accumulateArray2();
      } break;
      case 7: {
          // ----------------------------------------------------------------
          // Pre-increment and post-increment operators
          //   Ensure that pre-increment and post-increment operators
          //   are defined and doesn't affect iterator behavior.
          //
          // Concerns:
          //: 1 Pre-increment and post-increment operators should be
          //:   declared and defined so they could be called by client code
          //:
          //: 2 Iterator should preserve its behavior after increment
          //:   operator was invoked
          //
          // Plan:
          //: 1 Create iterator object
          //:
          //: 2 Invoke iterator pre-increment operator
          //:
          //: 3 Check that functor is invoked on assignment of dereferenced
          //:   iterator
          //:
          //: 4 Invoke iterator post-increment operator
          //:
          //: 5 Check that functor is invoked on assignment of dereferenced
          //:   iterator
          //
          // Testing:
          //   FunctionOutputIterator& operator++(FunctionOutputIterator&)
          //   FunctionOutputIterator operator++(FunctionOutputIterator&, int)
          // ----------------------------------------------------------------

          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "PRE- and POST-INCREMENT OPERATORS" << bsl::endl
                  << "=================================" << bsl::endl;
          }

          const int initialValue =  1;
          const int theValue1    = 17;
          const int theValue2    = 19;
          const int theValue3    = 23;

          typedef Value<int> IntValue;
          typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;
          IntValue value(initialValue);

          Iterator it(value.createSetter());

          *it = theValue1;

          // check that functional object was invoked and 'theValue1'
          // was set to 'value'
          ASSERT(theValue1 == value.get());

          const int setterId = value.getLastSetterId();

          ++it;

          *it = theValue2;
          ASSERT(theValue2 == value.get());
          ASSERT(setterId == value.getLastSetterId());

          it++;

          *it = theValue3;
          ASSERT(theValue3 == value.get());
          ASSERT(setterId == value.getLastSetterId());

      } break;
      case 6: {
          // ----------------------------------------------------------------
          // Assignment operator:
          //   Ensure that when the functional object of the original iterator
          //   (i.e., r-value iterator in the assument being tested) will be
          //   invoked on assignment to dereferenced iterator.
          //
          //   Note that this operation is currently supplied by the compiler.
          //
          // Concerns:
          //: 1 L-value iterator is in appropriate state after assignment.
          //:
          //: 2 R-value iterator can be 'const'.
          //:
          //: 3 After assignment L-value iterator holds a copy of
          //:   functional object from the r-value iterator.
          //:
          //: 4 After assignment l-value iterator invokes functional object
          //
          // Plan:
          //: 1 Create original iterator object
          //:
          //: 2 Create another iterator object and assign original
          //:   iterator to it
          //:
          //: 3 Check that assignment operator makes a copy of
          //:   the functional object of the original iterator
          //:
          //: 4 Check that functor is invoked on assignment of dereferenced
          //:   iterator
          //
          // Testing:
          //   FunctionOutputIterator::operator=(const FunctionOutputIterator&)
          // ----------------------------------------------------------------
          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "ASSIGNMENT OPERATOR" << bsl::endl
                  << "===================" << bsl::endl;
          }

          const int initialValue =  1;
          const int theValue1    = 17;
          const int theValue2    = 19;

          typedef Value<int> IntValue;
          typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;
          IntValue value(initialValue);

          // original iterator
          Iterator itOriginal(value.createSetter());

          *itOriginal = theValue1;

          // check that functional object was invoked and 'theValue1'
          // was set to 'value'
          ASSERT(theValue1 == value.get());

          const int originalSetterId = value.getLastSetterId();

          // copy of the original iterator
          Iterator itCopy;
          itCopy = itOriginal;

          *itCopy = theValue2;

          // check that copy of the functional object 'setter' was
          // actually invoked
          ASSERT((originalSetterId + 1) == value.getLastSetterId());

          // check that functional object was invoked and 'theValue2'
          // was set to 'value'
          ASSERT(theValue2 == value.get());

      } break;
      case 5: {
          // ----------------------------------------------------------------
          // Copy CTOR:
          //   Ensure that the functional object of the original iterator
          //   (i.e., argument of the copy ctor) will be invoked
          //   on assignment to dereferenced iterator.
          //
          //   Note that this operation is currently supplied by the compiler.
          //
          // Concerns:
          //: 1 The newly created object is in appropriate state.
          //:
          //: 2 The copy CTOR argument can be 'const'.
          //:
          //: 3 The newly created iterator holds a copy of functional object
          //:   from the iterator specified in the copy ctor.
          //:
          //: 4 The newly created iterator invokes functional object
          //
          // Plan:
          //: 1 Create original iterator object
          //:
          //: 2 Create a copy of the original iterator object with copy CTOR
          //:
          //: 3 Check that copy CTOR makes a copy of the functional object
          //:   of the original iterator (specified in the copy constructor)
          //:
          //: 4 Check that functor is invoked on assignment of dereferenced
          //:   iterator
          //
          // Testing:
          //   bdlb::FunctionOutputIterator(const FunctionOutputIterator&)
          // ----------------------------------------------------------------
          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "COPY CTOR" << bsl::endl
                  << "=========" << bsl::endl;
          }

          const int initialValue =  1;
          const int theValue1    = 17;
          const int theValue2    = 19;

          typedef Value<int> IntValue;
          typedef bdlb::FunctionOutputIterator<IntValue::Setter> Iterator;

          IntValue value(initialValue);

          // original iterator
          Iterator itOriginal(value.createSetter());

          *itOriginal = theValue1;

          // check that functional object was invoked and 'theValue1'
          // was set to 'value'
          ASSERT(theValue1 == value.get());

          const int originalSetterId = value.getLastSetterId();

          // copy of the original iterator
          Iterator itCopy((const Iterator) itOriginal);

          *itCopy = theValue2;

          // check that copy of the functional object 'setter' was
          // actually invoked
          ASSERT((originalSetterId + 1) == value.getLastSetterId());

          // check that functional object was invoked and 'theValue2'
          // was set to 'value'
          ASSERT(theValue2 == value.get());
      } break;
      case 4: {
          // ----------------------------------------------------------------
          // VALUE CTOR:
          //   Ensure that the functional object specified in
          //   the value ctor will be invoked on assignment to dereferenced
          //   iterator.
          //
          // Concerns:
          //: 1 The newly created object is in appropriate state.
          //:
          //: 2 Ctor argument can be 'const'.
          //:
          //: 3 That, if a functor is supplied, the newly created iterator
          //:   holds a copy of functor, and invokes it when the iterator is
          //:   dereferenced and assigned to.
          //:
          //: 4 That, if a function pointer is supplied, the newly created
          //:   iterator holds a pointer to the function, and invokes it when
          //:   the iterator is dereferenced and assigned to.
          //
          //:Plan:
          //: 1 Create an iterator using an instance of the test type
          //:   'VALUE<int>' and 'VALUE<int>::Setter', derefence and assign a
          //:   value a couple times, use functions provided by the test type
          //:   to verify that the functor was copied and invoked correctly.
          //:   (C-1,2,3).
          //:
          //: 2 Create an iterator and supply the test 'simpleFunction'
          //:   function. (C-4)
          //
          //
          // Testing:
          //   bdlb::FunctionOutputIterator<FUNCTION>(const FUNCTION&)
          // ----------------------------------------------------------------

          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "VALUE CTOR" << bsl::endl
                  << "==========" << bsl::endl;
          }

          if (veryVerbose) {
              bsl::cout << "\tSupply a functor at construction" << bsl::endl;
          }
          {
              const int initialValue =  1;
              const int valueA       = 17;
              const int valueB       = 20;

              typedef Value<int> IntValue;
              IntValue value(initialValue);

              const IntValue::Setter setter = value.createSetter();

              // value CTOR
              bdlb::FunctionOutputIterator<IntValue::Setter> it(setter);

              *it = valueA;

              // check that copy of the functional object 'setter' was
              // actually invoked
              ASSERT((setter.getId() + 1) == value.getLastSetterId());

              // check that functional object was invoked and 'valueA' was
              // set to 'value'
              ASSERT(valueA == value.get());

              *it = valueB;

              // check that copy of the functional object 'setter' was
              // actually invoked
              ASSERT((setter.getId() + 1) == value.getLastSetterId());

              // check that functional object was invoked and 'valueB' was
              // set to 'value'
              ASSERT(valueB == value.get());

          }

          if (veryVerbose) {
              bsl::cout << "\tSupply a function pointer at construction"
                        << bsl::endl;
          }
          {
              const int initialValue =  1;
              const int valueA       = 17;
              const int valueB       = 20;

              simpleFunctionValue = initialValue;

              // value CTOR
              bdlb::FunctionOutputIterator<void (*)(int)> it(&simpleFunction);


              *it = valueA;

              // check that function was invoked and 'simpleFunctionValue' was
              // set to 'valueA'.
              ASSERT(valueA == simpleFunctionValue);

              *it = valueB;

              // check that function was invoked and 'simpleFunctionValue' was
              // set to 'valueB'.
              ASSERT(valueB == simpleFunctionValue);

          }
      } break;
      case 3: {
          // ----------------------------------------------------------------
          // DEFAULT CTOR
          //
          // Concerns:
          //: 1 Default ctor of iterator invokes default ctor of the
          //:   functional object.
          //:
          //: 2 Functional object created with default constructor
          //:   should be invoked any way.
          //
          // Note that the iterator has no accessors to check attribute values
          // and no manipulators which change attribute values.
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
          //   bdlb::FunctionOutputIterator<FUNCTION>();
          // ----------------------------------------------------------------

          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "DEFAULT CTOR" << bsl::endl
                  << "============" << bsl::endl;
          }

          const int initialValue =  1;
          const int theValue     = 17;

          typedef Value<int> IntValue;
          IntValue::singleton.set(initialValue);

          // default CTOR
          bdlb::FunctionOutputIterator<IntValue::Setter> it;

          *it = theValue;

          // check that functional object was invoked and 'theValue' was set
          // to 'value'
          ASSERT(theValue == IntValue::singleton.get());
      } break;
      case 2: {
          // ----------------------------------------------------------------
          // CONCERN: All 'output_iterator' traits are correctly defined.
          //
          //   Ensure that bdlb::FunctionOutputIterator defines required traits
          //
          // Concerns:
          //: 1 There must be following types (traits) define in
          //:   bdlb::FunctionOutputIterator:
          //:     iterator_category
          //:     value_type
          //:     difference_type
          //:     pointer
          //:     reference
          //
          // Plan:
          //: 1 Check that required types are defined
          //
          // Testing:
          //   bdlb::FunctionOutputIterator::iterator_category
          //   bdlb::FunctionOutputIterator::value_type
          //   bdlb::FunctionOutputIterator::difference_type
          //   bdlb::FunctionOutputIterator::pointer
          //   bdlb::FunctionOutputIterator::reference
          // ----------------------------------------------------------------

          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "TRAITS TEST" << bsl::endl
                  << "===========" << bsl::endl;
          }

          int res = 0;

          // iterator_category == output_iterator_tag
          res = bslmf::IsSame<
              bdlb::FunctionOutputIterator<int>::iterator_category,
              bsl::output_iterator_tag>::VALUE;
          ASSERT(0 != res);

          // value_type == void
          res = bslmf::IsSame<
              bdlb::FunctionOutputIterator<int>::value_type,
              void>::VALUE;

          // difference_type == void
          res = bslmf::IsSame<
              bdlb::FunctionOutputIterator<int>::difference_type,
              void>::VALUE;
          ASSERT(0 != res);

          // pointer == void
          res = bslmf::IsSame<
              bdlb::FunctionOutputIterator<int>::pointer,
              void>::VALUE;
          ASSERT(0 != res);

          // reference == void
          res = bslmf::IsSame<
              bdlb::FunctionOutputIterator<int>::reference,
              void>::VALUE;
          ASSERT(0 != res);
      } break;
      case 1: {
          // ----------------------------------------------------------------
          // BREATHING TEST:
          //   Developers' Sandbox.
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

          if (verbose) {
              bsl::cout
                  << bsl::endl
                  << "BREATHING TEST" << bsl::endl
                  << "==============" << bsl::endl;
          }

          const int initialValue =  1;
          const int theValue     = 17;

          typedef Value<int> IntValue;
          IntValue value(initialValue);
          bdlb::FunctionOutputIterator<IntValue::Setter>
              it(value.createSetter());

          *it = theValue;

          // check that functional object was invoked and 'theValue' was set
          // to 'value'
          ASSERT(theValue == value.get());

      } break;
      default: {
          bsl::cerr << "WARNING: CASE '" << test << "' NOT FOUND."
                    << bsl::endl;
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
