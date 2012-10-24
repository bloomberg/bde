#include <bsl_iostream.h>
#include <bsl_algorithm.h>
#include <bdef_memfn.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_placeholder.h>
#include <bslmf_issame.h>

#include <bdeut_functionoutputiterator.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
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

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template<typename TYPE>
class Value {
  public:
    typedef TYPE value_type;

    static Value singleton;

    class Setter {
      private:
        static int instanceCount;

        const int  d_id;
        Value     *d_value_p;
      public:
        Setter()
            : d_id(++instanceCount)
            , d_value_p(&singleton)
        {
        };

        Setter(const Setter& src)
            : d_id(++instanceCount)
            , d_value_p(src.d_value_p)
        {
        }

        Setter(Value *value)
            : d_id(++instanceCount)
            , d_value_p(value)
        {
        };

        Setter& operator=(const Setter& src)
        {
            if (&src != this) {
                d_value_p = src.d_value_p;
            }
            return *this;
        }

        inline int getId() const { return d_id; };
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
    inline void set(const TYPE& data) { d_data = data; };
    inline int setLastSetterId(int id) { d_lastSetterId = id; };
    // ACCESSORS
    inline TYPE get() const { return d_data; };
    inline int getLastSetterId() const { return d_lastSetterId; };

    Setter createSetter() { return Setter(this); };
};

template<typename TYPE>
Value<TYPE> Value<TYPE>::singleton;

template<typename TYPE>
int Value<TYPE>::Setter::instanceCount = 0;

namespace {
    int simpleFunctionValue = 0;
}

void SimpleFunction(int value)
{
    simpleFunctionValue = value;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    verbose = true;
    for(test = 1; test <= 10; test++)
    switch (test) {

        case 0:

        case 10: {
            // ----------------------------------------------------------------
            // Use of template bdef_Function<> as a functional object:
            //   Ensure that the object of the class bdef_Function<>
            //   specified as an functional object will be invoked on
            //   assigment to dereferenced iterator.
            //
            // Concerns:
            //   1 Template class bdeut_FunctionOutputIterator<> can be
            //     instantiated with bdef_Function<>
            //
            // Plan:
            //   1 Create iterator object and initialize it with object of
            //     class bdef_Function<>
            //
            //   2 Check that member function associated with
            //     bdef_Function<> object is invoked on assignment of
            //     dereferenced iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator< bdef_Function<> >
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "USE OF bdef_Function<>" << bsl::endl
                    << "======================" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue = 17 };

            typedef Value<int> IntValue;
            typedef bdef_Function<void (*)(const int&)> Function;
            typedef bdeut_FunctionOutputIterator<Function> Iterator;

            IntValue value(initialValue);
            Iterator it(bdef_BindUtil::bind(&IntValue::set,
                                            &value,
                                            bdef_PlaceHolders::_1));
            *it = (IntValue::value_type) theValue;

            // check that functional object was invoked and 'theValue'
            // was set to 'value'
            ASSERT(theValue == value.get());

        } break;

        case 9: {
            // ----------------------------------------------------------------
            // Use of template bdef_MemFnInstance<> as a functional object:
            //   Ensure that the object of the class bdef_MemFnInstance<>
            //   specified as an functional object will be invoked on
            //   assigment to dereferenced iterator.
            //
            // Concerns:
            //   1 Template class bdeut_FunctionOutputIterator<> can be
            //     instantiated with bdef_MemFnInstance<>
            //
            // Plan:
            //   1 Create iterator object and initialize it with object of
            //     class bdef_MemFnInstance<>
            //
            //   2 Check that member function associated with
            //     bdef_MemFnInstance<> object is invoked on assignment of
            //     dereferenced iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator< bdef_MemFnInstance<> >
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "USE OF bdef_MemFnInstance<>" << bsl::endl
                    << "===========================" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue = 17 };

            typedef Value<int> IntValue;
            typedef bdef_MemFnInstance<
                void (IntValue::*)(const int&), IntValue*>
                Function;
            typedef bdeut_FunctionOutputIterator<Function> Iterator;

            IntValue value(initialValue);
            Iterator it(Function(&IntValue::set, &value));

            *it = (IntValue::value_type) theValue;

            // check that functional object was invoked and 'theValue'
            // was set to 'value'
            ASSERT(theValue == value.get());

        } break;

        case 8: {
            // ----------------------------------------------------------------
            // Use of function pointer as a functional object:
            //   Ensure that the function pointer specified as an functional
            //   object will be invoked on assigment to dereferenced iterator.
            //
            // Concerns:
            //   1 Template class bdeut_FunctionOutputIterator<> can be
            //     instantiated with function pointer
            //
            // Plan:
            //   1 Create iterator object and initialize it with function
            //     pointer
            //
            //   2 Check that function is invoked on assignment of dereferenced
            //     iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator<void (*Fn)()>
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "USE OF FUNCTION POINTER" << bsl::endl
                    << "=======================" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue = 17 };

            typedef void (*Function)(int);
            typedef bdeut_FunctionOutputIterator<Function> Iterator;

            Iterator it(&SimpleFunction);
            simpleFunctionValue = initialValue;
            *it = (int) theValue;

            // check that functional object was invoked and 'theValue'
            // was set to 'simpleFunctionValue'
            ASSERT(theValue == simpleFunctionValue);

        } break;

        case 7: {
            // ----------------------------------------------------------------
            // Pre-increment and post-increment operators
            //   Ensure that pre-increment and post-increment operators
            //   are defined and doen't affect iterator behaviour.
            //
            // Concerns:
            //   1 Pre-increment and post-increment operators should be
            //     declared and defined so they could be called by client code
            //
            //   2 Iterator should preserve its behavour after increment
            //     operator was invoked
            //
            // Plan:
            //   1 Create iterator object
            //
            //   2 Invoke iterator pre-increment operator
            //
            //   3 Check that functor is invoked on assignment of dereferenced
            //     iterator
            //
            //   4 Invoke iterator post-increment operator
            //
            //   5 Check that functor is invoked on assignment of dereferenced
            //     iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator<FUNCTION>::operator++()
            //   bdeut_FunctionOutputIterator<FUNCTION>::operator++(int)
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "PRE- and POST-INCREMENT OPERATORS" << bsl::endl
                    << "=================================" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue1 = 17,
                   theValue2 = 19,
                   theValue3 = 23 };

            typedef Value<int> IntValue;
            typedef bdeut_FunctionOutputIterator<IntValue::Setter> Iterator;
            IntValue value(initialValue);

            Iterator it(value.createSetter());

            *it = (IntValue::value_type) theValue1;

            // check that functional object was invoked and 'theValue1'
            // was set to 'value'
            ASSERT(theValue1 == value.get());

            const int setterId = value.getLastSetterId();

            ++it;

            *it = (IntValue::value_type) theValue2;
            ASSERT(theValue2 == value.get());
            ASSERT(setterId == value.getLastSetterId());

            it++;

            *it = (IntValue::value_type) theValue3;
            ASSERT(theValue3 == value.get());
            ASSERT(setterId == value.getLastSetterId());

        } break;

        case 6: {

            // ----------------------------------------------------------------
            // Assignment operator:
            //   Ensure that the functional object of the original iterator
            //   (i.e. r-value iterator) will be invoked on assigment to
            //   dereferenced iterator.
            //
            // Concerns:
            //   1 L-value iterator is in appropriate state after assigment.
            //
            //   2 R-value iterator can be 'const'.
            //
            //   3 After assignment L-value iterator holds a copy of
            //     functional object from the r-value iterator.
            //
            //   4 After assignment l-value iterator invokes functional object
            //
            // Plan:
            //   1 Create original iterator object
            //
            //   2 Create another iterator object and assign original
            //     iterator to it
            //
            //   3 Check that assignment operator makes a copy of
            //     the functional object of the original iterator
            //
            //   4 Check that functor is invoked on assignment of dereferenced
            //     iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator<FUNCTION>::operator=(
            //       const bdeut_FunctionOutputIterator<FUNCTION>&)
            // ----------------------------------------------------------------
            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "ASSIGNMENT OPERATOR" << bsl::endl
                    << "===================" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue1 = 17,
                   theValue2 = 19 };

            typedef Value<int> IntValue;
            typedef bdeut_FunctionOutputIterator<IntValue::Setter> Iterator;
            IntValue value(initialValue);

            // original iterator
            Iterator itOriginal(value.createSetter());

            *itOriginal = (IntValue::value_type) theValue1;

            // check that functional object was invoked and 'theValue1'
            // was set to 'value'
            ASSERT(theValue1 == value.get());

            const int originalSetterId = value.getLastSetterId();

            // copy of the original iterator
            Iterator itCopy;
            itCopy = (const Iterator) itOriginal;

            *itCopy = (IntValue::value_type) theValue2;

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
            //   (i.e. argument of the copy ctor) will be invoked
            //   on assigment to dereferenced iterator.
            //
            // Concerns:
            //   1 The newly created object is in appropriate state.
            //
            //   2 The copy CTOR argument can be 'const'.
            //
            //   3 The newly created iterator holds a copy of functional object
            //     from the iterator specified in the copy ctor.
            //
            //   4 The newly created iterator invokes functional object
            //
            // Plan:
            //   1 Create original iterator object
            //
            //   2 Create a copy of the original iterator object with copy CTOR
            //
            //   3 Check that copy CTOR makes a copy of the functional object
            //     of the original iterator (specified in the copy constructor)
            //
            //   4 Check that functor is invoked on assignment of dereferenced
            //     iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator<FUNCTION>(
            //       const bdeut_FunctionOutputIterator<FUNCTION>&)
            // ----------------------------------------------------------------
            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "COPY CTOR" << bsl::endl
                    << "=========" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue1 = 17,
                   theValue2 = 19 };

            typedef Value<int> IntValue;
            typedef bdeut_FunctionOutputIterator<IntValue::Setter> Iterator;

            IntValue value(initialValue);

            // original iterator
            Iterator itOriginal(value.createSetter());

            *itOriginal = (IntValue::value_type) theValue1;

            // check that functional object was invoked and 'theValue1'
            // was set to 'value'
            ASSERT(theValue1 == value.get());

            const int originalSetterId = value.getLastSetterId();

            // copy of the original iterator
            Iterator itCopy((const Iterator) itOriginal);

            *itCopy = (IntValue::value_type) theValue2;

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
            //   the value ctor will be invoked on assigment to dereferenced
            //   iterator.
            //
            // Concerns:
            //   1 The newly created object is in appropriate state.
            //
            //   2 Ctor argument can be 'const'.
            //
            //   3 The newly created iterator holds a copy of functional object
            //     specified in the ctor.
            //
            //   4 The newly created iterator invokes functional object
            //
            // Plan:
            //   1 Create iterator object with value contructor specifing const
            //     original functor
            //
            //   2 Check that iterastor makes a copy of original
            //     (specified in the constructor) functor
            //
            //   3 Check that functor is invoked on assignment of dereferenced
            //     iterator
            //
            // Testing:
            //   bdeut_FunctionOutputIterator<FUNCTION>(const FUNCTION&)
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "VALUE CTOR" << bsl::endl
                    << "==========" << bsl::endl;
            }

            enum { initialValue = 1,
                   theValue = 17 };

            typedef Value<int> IntValue;
            IntValue value(initialValue);

            const IntValue::Setter setter = value.createSetter();

            // value CTOR
            bdeut_FunctionOutputIterator<IntValue::Setter> it(setter);

            *it = (IntValue::value_type) theValue;

            // check that copy of the functional object 'setter' was
            // actually invoked
            ASSERT((setter.getId() + 1) == value.getLastSetterId());

            // check that functional object was invoked and 'theValue' was set
            // to 'value'
            ASSERT(theValue == value.get());

        } break;

        case 3: {

            // ----------------------------------------------------------------
            // DEFAULT CTOR
            //
            // Concerns:
            //   1 Default ctor of iterator invokes default ctor of the
            //     functional object.
            //
            //   2 Functional object created with default constructor
            //     should be invoked any way.
            //
            //   Iterator has no accessors to check attribute values.
            //   Iterator has no manipulators which change attribute values.
            //
            // Plan:
            //  1 Create an object, using the default ctor.
            //
            //  2 Assign value to dereferenced iterator.
            //
            //  3 Check that corrsponding functional object (singleton) was
            //    invoked with correct value.
            //
            // Testing:
            //   bdeut_FunctionalIterator<FUNCTION>();
            // ----------------------------------------------------------------

            if (verbose) cout
                << bsl::endl
                << "DEFAULT CTOR" << bsl::endl
                << "============" << bsl::endl;

            enum { initialValue = 1,
                   theValue = 17 };

            typedef Value<int> IntValue;
            IntValue::singleton.set(initialValue);

            // default CTOR
            bdeut_FunctionOutputIterator<IntValue::Setter> it;

            *it = (IntValue::value_type) theValue;

            // check that functional object was invoked and 'theValue' was set
            // to 'value'
            ASSERT(theValue == IntValue::singleton.get());


        } break;

        case 2: {
            // ----------------------------------------------------------------
            // BREATHING TEST:
            //   Developers' Sandbox.
            //
            // Concerns:
            //  1 The class is functional enough to enable comprehensive
            //    testing in subsequent cases
            //
            // Plan:
            //  1 Create an object, using the ctor with functional object.
            //
            //  2 Assign value to dereferenced iterator.
            //
            //  3 Check that functional object was invoked with correct value.
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

            enum { initialValue = 1,
                   theValue = 17 };

            typedef Value<int> IntValue;
            IntValue value(initialValue);
            bdeut_FunctionOutputIterator<IntValue::Setter>
                                it(value.createSetter());

            *it = (IntValue::value_type) theValue;

            // check that functional object was invoked and 'theValue' was set
            // to 'value'
            ASSERT(theValue == value.get());

        } break;

        case 1: {

            // ----------------------------------------------------------------
            // TRAITS TEST:
            // Ensure that bdeut_FunctionOutputIterator defines required traits
            //
            // Concerns:
            //  1 There must be following types (traits) define in
            //    bdeut_FunctionOutputIterator:
            //    iterator_category
            //    value_type
            //    difference_type
            //    pointer
            //    reference
            //
            // Plan:
            //  1 Check that required types are defined
            //
            // Testing:
            //   bdeut_FunctionOutputIterator::iterator_category
            //   bdeut_FunctionOutputIterator::value_type
            //   bdeut_FunctionOutputIterator::difference_type
            //   bdeut_FunctionOutputIterator::pointer
            //   bdeut_FunctionOutputIterator::reference
            // ----------------------------------------------------------------

            if (verbose) {
                bsl::cout
                    << bsl::endl
                    << "TRAITS TEST" << bsl::endl
                    << "===========" << bsl::endl;
            }

            int res = 0;

            // iterator_category == output_iterator_tag
            res = bslmf_IsSame<
                bdeut_FunctionOutputIterator<int>::iterator_category,
                bsl::output_iterator_tag>::VALUE;
            ASSERT(0 != res);

            // value_type == void
            res = bslmf_IsSame<
                bdeut_FunctionOutputIterator<int>::value_type,
                void>::VALUE;

            // difference_type == void
            res = bslmf_IsSame<
                bdeut_FunctionOutputIterator<int>::difference_type,
                void>::VALUE;
            ASSERT(0 != res);

            // pointer == void
            res = bslmf_IsSame<
                bdeut_FunctionOutputIterator<int>::pointer,
                void>::VALUE;
            ASSERT(0 != res);

            // reference == void
            res = bslmf_IsSame<
                bdeut_FunctionOutputIterator<int>::reference,
                void>::VALUE;
            ASSERT(0 != res);
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
