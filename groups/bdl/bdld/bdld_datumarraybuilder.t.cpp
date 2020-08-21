// bdld_datumarraybuilder.t.cpp                                       -*-C++-*-
#include <bdld_datumarraybuilder.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is an utility for building 'Datum' objects holding
// arrays of 'Datum' objects.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit DatumArrayBuilder(allocator_type);
// [ 2] DatumArrayBuilder(SizeType, allocator_type);
// [ 2] ~DatumArrayBuilder();
//
// MANIPULATORS
// [ 2] void pushBack(const Datum& value);
// [ 4] void append(const Datum *, SizeType);
// [ 2] Datum commit();
//
// ACCESSORS
// [ 3] SizeType capacity() const;
// [ 3] allocator_type get_allocator() const;
// [ 3] SizeType size() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
// [ 5] TYPE TRAITS

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdld::DatumArrayBuilder Obj;
typedef Obj::allocator_type     AllocType;

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'DatumArrayBuilder' to Create a 'Datum' array.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we receive a string that is constructed by streaming a bunch of
// values together in the format shown below:
//..
//  "2.34,4,hi there,true"
//..
// Notice that the values are separated by a ','.  Also note that a ',' is not
// allowed to be part of a string value to simplify the implementation of the
// utility that parses this string.  The following code snippets illustrate how
// to create a 'Datum' object that holds an array of 'Datum' objects
// constructed using the streamed values.
//
// First we define a function 'nextValue' that we will use to tokenize the
// input string:
//..
    bsl::size_t nextValue(bsl::string *value, const bsl::string& input)
        // Extract the next value from a list of comma separated values in the
        // specified 'input' string and load it in the specified 'value'.
        // Return the index of the next value within 'input'.
    {
        if (input.empty()) {
            return bsl::string::npos;
        }
        int start = 0;
        bsl::size_t nextIndex = input.find(',', start);
        if (bsl::string::npos != nextIndex) {
            *value = input.substr(start, nextIndex - start);
        }
        else {
            *value = input.substr(start);
        }
        return nextIndex;
    }
//..
// Next, we define a function 'convertToDatum' that will convert a string
// token into a 'Datum' scalar value:
//..
    bdld::Datum convertToDatum(const bsl::string&  token,
                               bslma::Allocator   *basicAllocator)
        // Convert the specified 'token' into the appropriate type of scalar
        // value and then create and return a 'Datum' object using that value.
        // Use the specified 'basicAllocator' to allocate memory.
    {
        bool isInteger = true;
        bool isDouble = false;
        bool isBoolean = false;
        for (bsl::size_t i = 0; i < token.size(); ++i) {
            if (!isdigit(token[i])) {
                if ('.' == token[i] && !isDouble) {
                    isDouble = true;
                    isInteger = false;
                    continue;
                }
                isInteger = false;
                isDouble = false;
                break;
            }
        }

        if (!isInteger && !isDouble) {
            if ("true" == token || "false" == token) {
                isBoolean = true;
            }
        }

        if (isInteger) { // integer token
            return bdld::Datum::createInteger(atoi(token.c_str()));
        }
        else if (isDouble) { // double token
            return bdld::Datum::createDouble(atof(token.c_str()));
        }
        else if (isBoolean) { // boolean token
            return bdld::Datum::createBoolean("true" == token ? true : false);
        }
        else { // string value
            return bdld::Datum::copyString(token, basicAllocator);
        }
    }
//..
// Now, in our example main, we tokenize an input string "2.34,4,hi there,true"
// to populate a 'Datum' array containing the values '[2.34, 4, "hi there",
// true]':
//..
    void exampleMain() {
        bslma::TestAllocator allocator;
        const bsl::string    input("2.34,4,hi there,true", &allocator);
//..
// Here, we create a 'DatumArrayBuilder', and iterate over the parsed tokens
// from 'input', using 'pushBack' on the array builder to add the tokens to a
// 'Datum' array value:
//..
        bdld::DatumArrayBuilder builder(0, &allocator);

        bsl::string str(input, &allocator);

        bsl::string value;
        bsl::size_t nextIndex;
        do {
            nextIndex = nextValue(&value, str);
            builder.pushBack(convertToDatum(value, &allocator));
            if (bsl::string::npos == nextIndex) {
                break;
            }
            str = str.substr(nextIndex + 1);
        } while (bsl::string::npos != nextIndex);

        bdld::Datum result = builder.commit();
//..
// Notice that calling 'commit' on the 'DatumArrayBuilder' adopts ownership
// for the returned 'Datum' object, and that the behavior is undefined if
// 'pushBack' is called after 'commit'.
//
// Finally, we verify that 'result' has the expected array value, and destroy
// the created 'Datum' value:
//..
        ASSERT(true       == result.isArray());
        ASSERT(4          == result.theArray().length());
        ASSERT(true       == result.theArray()[0].isDouble());
        ASSERT(2.34       == result.theArray()[0].theDouble());
        ASSERT(true       == result.theArray()[1].isInteger());
        ASSERT(4          == result.theArray()[1].theInteger());
        ASSERT(true       == result.theArray()[2].isString());
        ASSERT("hi there" == result.theArray()[2].theString());
        ASSERT(true       == result.theArray()[3].isBoolean());
        ASSERT(true       == result.theArray()[3].theBoolean());

        // Destroy the 'Datum' object.
        bdld::Datum::destroy(result, &allocator);
    }
//..
}  // close unnamed namespace

// ============================================================================
//                              TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&ta));

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE TEST" << endl
                          << "==================" << endl;

        exampleMain();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   The object uses bslma allocators and should have appropriate bslma
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class has the bslma::UsesBslmaAllocator trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type. (C-1..2)
        //
        // Testing:
        //   TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TYPE TRAITS" << endl
                          << "===================" << endl;

        ASSERT((bslma::UsesBslmaAllocator<Obj>::value));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'append' METHOD
        //   Verify the functionality of the 'append' method.
        //
        // Concerns:
        //: 1 Ensure that calling 'append' with an array of n 'Datum' objects
        //:   is functionally equivalent to sequentially calling 'pushBack'
        //:   with individual 'Datum' from the same array.
        //
        // Plan:
        //: 1 Build two different 'Datum' objects using 'append' and 'pushBack'
        //:   methods and verify that they compare-equal.
        //
        // Testing:
        //   void append(const Datum *, SizeType);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'append' METHOD" << endl
                          << "=======================" << endl;
        {
            bdld::Datum appendResult;
            bdld::Datum pushBackResult;

            bdld::Datum valuesAppend[] = {
                bdld::Datum::createBoolean(true),
                bdld::Datum::createInteger(47),
                bdld::Datum::createDouble(2.25),
                bdld::Datum::createDate(bdlt::Date()),
                bdld::Datum::createDatetime(bdlt::Datetime(), &ta),
                bdld::Datum::createDatetimeInterval(bdlt::DatetimeInterval(),
                                                    &ta),
                bdld::Datum::copyString("long string", &ta),
                bdld::Datum::createNull()
            };

            bdld::Datum valuesPushBack[] = {
                bdld::Datum::createBoolean(true),
                bdld::Datum::createInteger(47),
                bdld::Datum::createDouble(2.25),
                bdld::Datum::createDate(bdlt::Date()),
                bdld::Datum::createDatetime(bdlt::Datetime(), &ta),
                bdld::Datum::createDatetimeInterval(bdlt::DatetimeInterval(),
                                                    &ta),
                bdld::Datum::copyString("long string", &ta),
                bdld::Datum::createNull()
            };

            const bsl::size_t numValues = sizeof(valuesAppend)
                                          /sizeof(valuesAppend[0]);

            Obj               mBA(0, &ta);
            Obj               mBB(0, &ta);

            mBA.append(valuesAppend, numValues);
            appendResult = mBA.commit();

            ASSERT(true      == appendResult.isArray());
            ASSERT(numValues == appendResult.theArray().length());

            for (bsl::size_t i = 0; i < numValues; ++i) {
                mBB.pushBack(valuesPushBack[i]);
            }

            pushBackResult = mBB.commit();

            ASSERT(true      == pushBackResult.isArray());
            ASSERT(numValues == pushBackResult.theArray().length());

            ASSERTV(appendResult, pushBackResult,
                    appendResult == pushBackResult);

            bdld::Datum::destroy(appendResult, &ta);
            bdld::Datum::destroy(pushBackResult, &ta);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors functionality.
        //
        // Concerns:
        //: 1 The 'capacity' method returns the capacity of the datum-array
        //:   builder.
        //:
        //: 2 The 'get_allocator' method returns the allocator specified at
        //:   construction, and that is a default allocator if none was
        //:   specified at construction.
        //:
        //: 3 The 'size' method returns the current size of the datum-array
        //:   builder.
        //
        // Plan:
        //: 1 Create a 'DatumArrayBuilder' object.  Append a few elements to
        //:   the builder and verify that the 'capacity' and 'size' methods
        //:   return expected values.  (C-1,2)
        //:
        //: 2 Execute a loop that creates an object but invokes the default
        //:   constructor differently in each iteration: (a) without passing an
        //:   allocator, (b) passing a default-constructed allocator explicitly
        //:   (c) passing the address of a test allocator distinct from the
        //:   default, and (d) passing in an allocator constructed from the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations verify that the correct allocator is returned
        //:   by 'get_allocator()' and is used when memory is actually
        //:   allocated.
        //
        // Testing:
        //    SizeType       capacity()      const;
        //    allocator_type get_allocator() const;
        //    SizeType       size()          const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'capacity' and 'size' method." << endl;
        {
            static const struct {
                int           d_line;               // line number
                Obj::SizeType d_initialCapacity;    // capacity at construction
                Obj::SizeType d_numElements;        // elements to 'pushBack'
                Obj::SizeType d_expectedCapacity;   // expected capacity
            } DATA[] = {
                // LINE  INITIAL   NUMBER ELEMS  EXPECTED
                //       CAPACITY  TO PUSH       CAPACITY
                // ----  --------  ------------  --------
                { L_,    0,        0,            0        },
                { L_,    0,        1,            1        },
                { L_,    0,        2,            2        },
                { L_,    0,        3,            4        },
                { L_,    1,        0,            1        },
                { L_,    1,        1,            1        },
                { L_,    1,        2,            2        },
                { L_,    1,        3,            4        },
                { L_,    100,      0,            100      },
                { L_,    100,      99,           100      },
                { L_,    100,      100,          100      },
                { L_,    100,      101,          200      },
            };
            const size_t NUM_DATA = sizeof(DATA)/sizeof(DATA[0]);

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int           LINE          = DATA[i].d_line;
                const Obj::SizeType INIT_CAPACITY = DATA[i].d_initialCapacity;
                const Obj::SizeType NUM_PUSH      = DATA[i].d_numElements;
                const Obj::SizeType EXP_CAPACITY  = DATA[i].d_expectedCapacity;

                if (veryVerbose) {
                    T_ P_(LINE) P_(INIT_CAPACITY) P_(NUM_PUSH) P(EXP_CAPACITY)
                }

                bdld::Datum result;
                {
                    Obj        mB(INIT_CAPACITY, &ta);
                    const Obj& B = mB;

                    ASSERTV(i, B.capacity(), INIT_CAPACITY == B.capacity());
                    ASSERTV(i, &ta, B.get_allocator().mechanism(),
                            &ta == B.get_allocator());
                    ASSERTV(i, B.size(),     0             == B.size());

                    for (size_t n = 0; n < NUM_PUSH; ++n) {
                        const int   value = static_cast<int>(n);
                        bdld::Datum datum = bdld::Datum::createInteger(value);
                        mB.pushBack(datum);
                    }

                    ASSERTV(i, B.capacity(), EXP_CAPACITY == B.capacity());
                    ASSERTV(i, &ta, B.get_allocator().mechanism(),
                            &ta == B.get_allocator());
                    ASSERTV(i, B.size(),     NUM_PUSH     == B.size());

                    result = mB.commit();

                    ASSERTV(i, true     == result.isArray());
                    ASSERTV(i, NUM_PUSH == result.theArray().length());

                    for (size_t n = 0; n < NUM_PUSH; ++n) {
                        const int   value = static_cast<int>(n);
                        bdld::Datum datum = bdld::Datum::createInteger(value);
                        ASSERTV(i, n, datum == result.theArray()[n]);
                    }
                }

                ASSERTV(i, true     == result.isArray());
                ASSERTV(i, NUM_PUSH == result.theArray().length());

                for (size_t n = 0; n < NUM_PUSH; ++n) {
                    const int   value = static_cast<int>(n);
                    bdld::Datum datum = bdld::Datum::createInteger(value);
                    ASSERTV(i, n, datum == result.theArray()[n]);
                }

                bdld::Datum::destroy(result, &ta);
                ASSERT(0 == ta.status());
            }
        }

        if (verbose) cout << "\nTesting construction, allocation, and "
                          << "'get_allocator'." << endl;

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(Obj::allocator_type());
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              case 'd': {
                objAllocatorPtr = &sa;
                Obj::allocator_type alloc(objAllocatorPtr);
                objPtr = new (fa) Obj(alloc);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // Verify the object's 'get_allocator' accessor.

            ASSERTV(CONFIG, &oa, X.get_allocator().mechanism(),
                    &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            {
                // put something in the map to force a single allocation
                bdld::Datum element(bdld::Datum::createInteger(1));

                mX.append(&element, 1);
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            ASSERTV(CONFIG, oa.numBlocksMax(), 1 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        //   Ensure that we can create an object and bring it to a well known
        //   state for subsequent tests.
        //
        // Concerns:
        //: 1 An object created with a constructor (with or without a supplied
        //:   allocator) has expected state.
        //:
        //: 2 If an allocator is not supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator is supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 The destructor should not destroy the array once it is committed.
        //:
        //: 9 The destructor should deallocate all memory if 'commit' method
        //:   was not called prior object destruction.
        //:
        //:10 An object is capable of creating valid 'Datum' object when
        //:   'commit' method is invoked.
        //
        // Plan:
        //: 1 Construct three distinct objects, in turn, but configured
        //:   differently: (a) without passing an allocator, (b) passing a null
        //:   allocator address explicitly, and (c) passing the address of a
        //:   test allocator distinct from the default.  Verify that right
        //:   allocator is used to obtain memory in each case.  For each object
        //:   instantiation: (C-1..8)
        //:
        //:   1 Create distinct 'bdsma::TestAllocator' objects and install
        //:     one as the current default allocator (note that an unique
        //:     test allocator is already installed as the global allocator).
        //:
        //:   2 Use value constructor to create an object "mB', with its
        //:     object allocator configured appropriately. (C-1..5)
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-7)
        //:
        //:   4 Use (yet untested) 'pushBack' method to trigger memory
        //:     allocation and verify that memory comes from the correct
        //:     allocator.  (C-6)
        //:
        //:   5 Verify that all object memory is released when the object is
        //:     destroyed prior calling 'commit' method.  (C-8)
        //:
        //: 2 Use 'pushBack' to append datum objects into the
        //:   'DatumArrayBuilder' object and verify that the capacity increases
        //:   when the length of the array exceeds the capacity.
        //:
        //: 3 Use 'commit' to build a 'Datum' object containing the array and
        //:   verify that the resulting 'Datum' instance has correct type and
        //:   length.  (C-10)
        //:
        //: 4 Verify that the 'DatumArrayBuilder' releases all memory when
        //:   destroyed prior calling 'commit'.  (C-9)
        //:
        //: 5 Verify that the 'DatumArrayBuilder' does not release any memory
        //:   after the 'bdld::Datum' object holding an array was completely
        //:   built by calling 'commit'.  Destroying 'DatumArrayBuilder' after
        //:   'commit' does not destroy constructed 'bdld::Datum' object.
        //:   (C-6,8)
        //
        // Testing:
        //    explicit DatumArrayBuilder(SizeType, allocator_type);
        //    DatumArrayBuilder(SizeType);
        //    ~DatumArrayBuilder();
        //    void pushBack(const Datum& value);
        //    Datum commit();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nAllocator installation test." << endl;
        {
            if (verbose) cout << "\tConstructor with default allocator."
                              << endl;
            {
                bslma::TestAllocator         da("def", veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                bdld::Datum                  value = bdld::Datum::createNull();

                ASSERT(0 == da.numAllocations());

                Obj        mB(0, 0);
                const Obj& B = mB;

                ASSERT(&da == B.get_allocator().mechanism());

                ASSERT(0 == B.capacity());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mB.pushBack(value);

                ASSERT(&da == B.get_allocator().mechanism());

                ASSERT(0 <  B.capacity());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose)
                cout << "\tConstructor with explicit default allocator."
                     << endl;
            {
                bslma::TestAllocator         da("def", veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                bdld::Datum                  value = bdld::Datum::createNull();

                ASSERT(0 == da.numAllocations());

                Obj        mB(0, static_cast<bslma::Allocator *>(0));
                const Obj& B = mB;

                ASSERT(&da == B.get_allocator().mechanism());

                ASSERT(0 == B.capacity());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesTotal());

                mB.pushBack(value);

                ASSERT(&da == B.get_allocator().mechanism());

                ASSERT(0 <  B.capacity());
                ASSERT(1 == da.numAllocations());
                ASSERT(0 != da.numBytesTotal());
            }

            if (verbose)
                cout << "\tConstructor with object allocator." << endl;
            {
                bslma::TestAllocator         da("def", veryVeryVeryVerbose);
                bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                bdld::Datum                  value = bdld::Datum::createNull();

                ASSERT(0 == da.numAllocations());
                ASSERT(0 == oa.numAllocations());

                Obj        mB(0, &oa);
                const Obj& B = mB;

                ASSERT(&oa == B.get_allocator().mechanism());

                ASSERT(0 == B.capacity());
                ASSERT(0 == da.numAllocations());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesTotal());

                mB.pushBack(value);

                ASSERT(&oa == B.get_allocator().mechanism());

                ASSERT(0 <  B.capacity());
                ASSERT(0 == da.numAllocations());
                ASSERT(1 == oa.numAllocations());
                ASSERT(0 != oa.numBytesTotal());
            }
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            if (verbose) cout << "\tTesting committed, empty array." << endl;
            {
                bslma::TestAllocator         da("def",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());
                {
                    bdld::Datum result;
                    ASSERT(0 == da.numAllocations());

                    Obj        mB;
                    const Obj& B = mB;

                    ASSERT(&da == B.get_allocator().mechanism());

                    ASSERT(0 == da.numAllocations());
                    ASSERT(0 == B.capacity());

                    result = mB.commit();

                    ASSERT(&da == B.get_allocator().mechanism());

                    ASSERT(0    == da.numAllocations());
                    ASSERT(true == result.isArray());
                    ASSERT(0    == result.theArray().length());
                }
                ASSERTV(da.numAllocations(), 0 == da.numAllocations());
            }

            if (verbose)
                cout << "\tTesting committed, non-empty array." << endl;
            {
                bslma::TestAllocator         da("def", veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);
                bdld::Datum                  result;

                ASSERT(0 == da.numAllocations());
                {
                    bdld::Datum value = bdld::Datum::createNull();
                    ASSERT(0 == da.numAllocations());

                    Obj        mB(0, 0);
                    const Obj& B = mB;

                    ASSERT(&da == B.get_allocator().mechanism());

                    ASSERT(0 == B.capacity());

                    mB.pushBack(value);

                    ASSERT(0 < da.numAllocations());
                    ASSERT(0 < B.capacity());

                    result = mB.commit();

                    ASSERT(&da == B.get_allocator().mechanism());

                    ASSERT(0    == B.capacity());
                    ASSERT(true == result.isArray());
                    ASSERT(1    == result.theArray().length());
                }
                // Verify that array is not destroyed
                ASSERT(true == result.isArray());
                ASSERT(1    == result.theArray().length());

                // Now explicitly destroy array and check that all memory has
                // been released.
                bdld::Datum::destroy(result, &da);

                ASSERT(da.numAllocations() == da.numDeallocations());
                ASSERT(0 == da.numBlocksInUse());
            }

            if (verbose)
                cout << "\tTesting non-committed, non-empty array." << endl;
            {
                bslma::TestAllocator         da("def", veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());
                {
                    bdld::Datum value = bdld::Datum::createNull();
                    ASSERT(0 == da.numAllocations());

                    Obj        mB(0, 0);
                    const Obj& B = mB;

                    ASSERT(&da == B.get_allocator().mechanism());

                    ASSERT(0 == B.capacity());

                    mB.pushBack(value);

                    ASSERT(0 <  da.numAllocations());
                    ASSERT(0 == da.numDeallocations());
                    ASSERT(0 <  B.capacity());

                    // Not calling 'commit' to verify that destructor will
                    // deallocate partially built array.
                }
                ASSERT(da.numAllocations() == da.numDeallocations());
                ASSERT(0 == da.numBlocksInUse());
            }
        }

        if (verbose) cout << "\tTesting 'Datum' types in array." << endl;
        {
            bslma::TestAllocator         da("def", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);
            ASSERT(0 == da.numAllocations());

            bdld::Datum result;

            bdld::Datum values[] = {
                bdld::Datum::createBoolean(true),
                bdld::Datum::createInteger(47),
                bdld::Datum::createDouble(2.25),
                bdld::Datum::createDate(bdlt::Date()),
                bdld::Datum::createDatetime(bdlt::Datetime(), &da),
                bdld::Datum::createDatetimeInterval(bdlt::DatetimeInterval(),
                                                    &da),
                bdld::Datum::copyString("long string", &da),
                bdld::Datum::createNull()
            };

            const bsl::size_t numValues = sizeof(values)/sizeof(values[0]);

            {
                Obj        mB(0, 0);
                const Obj& B = mB;

                ASSERT(&da == B.get_allocator().mechanism());

                ASSERT(0 == B.capacity());

                for (bsl::size_t i = 0; i < numValues; ++i) {
                    mB.pushBack(values[i]);
                }

                ASSERT(numValues <= B.capacity());

                result = mB.commit();
                ASSERT(true      == result.isArray());
                ASSERT(numValues == result.theArray().length());

                for (bsl::size_t i = 0; i < numValues; ++i) {
                    ASSERTV(i, result.theArray()[i] == values[i]);
                }
            }

            ASSERT(true      == result.isArray());
            ASSERT(numValues == result.theArray().length());

            for (bsl::size_t i = 0; i < numValues; ++i) {
                ASSERTV(i, result.theArray()[i] == values[i]);
            }

            bdld::Datum::destroy(result, &da);
            ASSERT(0 == da.status());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Developer test sandbox.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            Obj        mB;
            const Obj& B = mB;

            ASSERT(0 == B.capacity());
        }

        {
            Obj        mB(0, 0);
            const Obj& B = mB;

            ASSERT(0 == B.capacity());
        }

        {
            Obj        mB(100);
            const Obj& B = mB;

            ASSERT(100 == B.capacity());
        }
        ASSERT(0 == ta.status());

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
