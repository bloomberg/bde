// bdld_datumarraybuilder.t.cpp                                       -*-C++-*-
#include <bdld_datumarraybuilder.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace BloombergLP::bdld;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility for building 'Datum' objects holding
// arrays of 'Datum' objects.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] explicit DatumArrayBuilder(bslma::Allocator *);
// [ 2] DatumArrayBuilder(int, bslma::Allocator *);
// [ 5] ~DatumArrayBuilder();
// MANIPULATORS
// [ 4] void pushBack(const Datum&);
// [ 2] void append(const Datum *, int);
// [ 2] Datum commit();
// ACCESSORS
// [ 2] int capacity() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n";                         \
                aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": "            \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": "            \
                          << J << "\t" << #K << ": " << K << "\n";            \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J       \
                          << "\t" << #K << ": " << K << "\t" << #L << ": "    \
                          << L << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define T_() cout << "\t" << flush;           // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static bsl::size_t nextValue(bsl::string *value, const bsl::string& input)
    // Extract the next value from a list of comma separated values in the
    // specified 'input' string and load it in the specified 'value'.  Return
    // the index of the next value within 'input'.
{
    if (input.empty()) {
        return bsl::string::npos;
    }

    const bsl::size_t start = 0;
    const bsl::size_t nextIndex = input.find(',', start);
    if (bsl::string::npos != nextIndex) {
        *value = input.substr(start, nextIndex - start);
    }
    else {
        *value = input.substr(start);
    }
    return nextIndex;
}

Datum convertToDatum(const bsl::string&  value,
                     bslma::Allocator    *basicAllocator)
    // Convert the specified 'value' into the appropriate type of scalar value
    // and then create and return a 'Datum' object using that value.  Use
    // the specified 'basicAllocator' to allocate memory.
{
    bool isInteger = true;
    bool isDouble = false;
    bool isBoolean = false;
    for (int i = 0; i < value.size(); ++i) {
        if (!isdigit(value[i])) {
            if ('.' == value[i] && !isDouble) {
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
        if ("true" == value || "false" == value) {
            isBoolean = true;
        }
    }

    if (isInteger) { // integer value
        return Datum::createInteger(atoi(value.c_str()));
    }
    else if (isDouble) { // double value
        return Datum::createDouble(atof(value.c_str()));
    }
    else if (isBoolean) { // boolean value
        return Datum::createBoolean("true" == value ? true : false);
    }
    else { // string value
        return Datum::copyString(value.c_str(),
                                 value.size(),
                                 basicAllocator);
    }
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&ta);
    bslma::Allocator *alloc(&ta);

    int numDeletes = 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST:
        //
        // Concerns:
        //    The usage example provided in the component header file must
        //    compile, link, and run on all platforms as shown.
        //
        // Plan:
        //    Incorporate usage example from header into driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        const bsl::string input("2.34,4,hi there,true", alloc);

        // Create a builder object.
        DatumArrayBuilder builder(alloc);

        bsl::string str(input, alloc);

        bsl::string value;
        int numValues = 0;
        bsl::size_t nextIndex;
        do {
            nextIndex = nextValue(&value, str);
            ++numValues;
            builder.pushBack(convertToDatum(value, alloc));
            if (bsl::string::npos == nextIndex) {
                break;
            }
            str = str.substr(nextIndex + 1);
        } while (bsl::string::npos != nextIndex);

        Datum result = builder.commit();

        ASSERT(result.isArray());
        ASSERT(numValues == result.theArray().length());
        ASSERT(result.theArray()[0].isDouble());
        ASSERT(2.34 == result.theArray()[0].theDouble());
        ASSERT(result.theArray()[1].isInteger());
        ASSERT(4 == result.theArray()[1].theInteger());
        ASSERT(result.theArray()[2].isString());
        ASSERT("hi there" == result.theArray()[2].theString());
        ASSERT(result.theArray()[3].isBoolean());
        ASSERT(true == result.theArray()[3].theBoolean());

        // Destroy the 'Datum' object.
        Datum::destroy(result, alloc);

    } break;
    case 5: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND MANIPULATORS - 3
        //
        // Concerns:
        //    The destructor should destroy the array if it is not committed.
        //
        // Plan:
        //    Create a 'DatumArrayBuilder' object using the explicit
        //    constructor.  Push back few elements to the array.  Destroy the
        //    'DatumArrayBuilder' object and verify that the array has been
        //    destroyed.  Verify no memory was leaked.
        //
        // Testing:
        //    ~DatumArrayBuilder();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS AND MANIPULATORS - 3" << endl
                          << "=====================================" << endl;

        const int numElements = 3;
        Datum values[numElements] = {
            Datum::createInteger(2),
            Datum::createBoolean(true),
            Datum::copyString("hi there", alloc)
        };
        const int numAllocations = ta.numAllocations();

        {
            DatumArrayBuilder builder(alloc);
            ASSERT(numAllocations == ta.numAllocations());

            builder.pushBack(values[0]);
            ASSERT(numAllocations < ta.numAllocations());

            builder.pushBack(values[1]);
            builder.pushBack(values[2]);
        }

        ASSERT(0 == ta.status());

    } break;
    case 4: {
        // -------------------------------------------------------------------
        // TESTING CREATORS AND MANIPULATORS - 2
        //
        // Concerns:
        //    The explicit constructor should create a 'DatumArrayBuilder'
        //    object with the zero capacity.  The manipulator should push back
        //    the given element and grow the array.  The destructor should not
        //    destroy the array once it is committed.
        //
        // Plan:
        //    Create a 'DatumArrayBuilder' object using the explicit
        //    constructor and verify the capacity.  Push back few elements to
        //    the array and verify that the capacity has increased.  Get a
        //    'Datum' object by committing the array and verify it has the
        //    expected values.  Destroy the 'DatumArrayBuilder' object and
        //    verify that the array still exists. Verify no memory was leaked.
        //
        // Testing:
        //    void pushBack(const Datum&);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS AND MANIPULATORS - 2" << endl
                          << "=====================================" << endl;

        Datum result;
        const int numElements = 3;
        Datum values[numElements] = {
            Datum::createInteger(2),
            Datum::createBoolean(true),
            Datum::copyString("hi there", alloc)
        };

        {
            DatumArrayBuilder builder(alloc);
            ASSERT(0 == builder.capacity());

            builder.pushBack(values[0]);
            ASSERT(0 < builder.capacity());

            builder.pushBack(values[1]);
            builder.pushBack(values[2]);

            result = builder.commit();

            ASSERT(result.isArray());
            ASSERT(result.theArray().length() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, result.theArray()[i] == values[i]);
            }
        }

        ASSERT(result.isArray());
        ASSERT(result.theArray().length() == numElements);
        for (int i = 0; i < numElements; ++i) {
            LOOP_ASSERT(i, result.theArray()[i] == values[i]);
        }
        Datum::destroy(result, alloc);
        ASSERT(0 == ta.status());

    } break;
    case 3: {
        // --------------------------------------------------------------------
        // EMPTY ARRAY TEST
        //
        // Concerns:
        //    The explicit constructor should create a 'DatumArrayBuilder'
        //    object with zero capacity.  When no elements are appended and
        //    'commit' is called, an empty array should be created.
        //
        // Plan:
        //    Create a 'DatumArrayBuilder' object using the explicit
        //    constructor and verify that the capacity is 0.  Call 'commit' and
        //    verify that an empty array was created.  Destroy the
        //    'DatumArrayBuilder' object and verify that the array still
        //    exists.
        //
        // Testing:
        //    explicit DatumArrayBuilder(bslma::Allocator *);
        //    ~DatumArrayBuilder();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "EMPTY ARRAY TEST" << endl
                          << "================" << endl;

        {
            Datum result;

            {
                DatumArrayBuilder builder(alloc);
                ASSERT(0 == builder.capacity());
                ASSERT(0 == ta.status());

                result = builder.commit();
                ASSERT(0 == ta.status());

                ASSERT(result.isArray());
                ASSERT(0 == result.theArray().length());
            }

            ASSERT(result.isArray());
            ASSERT(0 == result.theArray().length());
            ASSERT(0 == ta.status());
        }

    } break;
    case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND MANIPULATORS - 1
        //
        // Concerns:
        //    The constructor should create a 'DatumArrayBuilder' object with
        //    the correct capacity.  The manipulator should append the given
        //    elements and grow the array if it's length exceeds the capacity.
        //    The destructor should not destroy the array once it is committed.
        //
        // Plan:
        //    Create a 'DatumArrayBuilder' object with a given capacity and
        //    verify the capacity.  Append a range of elements to the array
        //    such that the length of the array exceeds its capacity.  Verify
        //    that the capacity has increased.  Get a 'Datum' object by
        //    committing the array and verify it has the expected values.
        //    Destroy the 'DatumArrayBuilder' object and verify that the array
        //    still exists.  Verify no memory was leaked.
        //
        // Testing:
        //    DatumArrayBuilder(int, bslma::Allocator *);
        //    ~DatumArrayBuilder();
        //    void append(const Datum *, int);
        //    Datum commit();
        //    int capacity() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS AND MANIPULATORS - 1" << endl
                          << "=====================================" << endl;

        Datum result;
        const int numElements = 3;
        Datum values[numElements] = {
            Datum::createInteger(2),
            Datum::createBoolean(true),
            Datum::copyString("hi there", alloc)
        };

        {
            const int capacity = 2;
            DatumArrayBuilder builder(capacity, alloc);
            ASSERT(capacity == builder.capacity());

            builder.append(values, numElements);
            ASSERT(capacity < builder.capacity());

            result = builder.commit();
            ASSERT(result.isArray());
            ASSERT(result.theArray().length() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, result.theArray()[i] == values[i]);
            }
        }

        ASSERT(result.isArray());
        ASSERT(result.theArray().length() == numElements);
        for (int i = 0; i < numElements; ++i) {
            LOOP_ASSERT(i, result.theArray()[i] == values[i]);
        }
        Datum::destroy(result, alloc);
        ASSERT(0 == ta.status());

    } break;
    case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //    Exercise basic functionality before beginning testing in earnest.
        //
        // Plan:
        //    Create a 'DatumArrayBuilder' object and verify that no
        //    memory is leaked on destruction.  Display object values
        //    frequently in verbose mode.
        //
        // Testing:
        //     This test case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            DatumArrayBuilder builder(alloc);
        }
        ASSERT(0 == ta.status());

    } break;
    default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
    }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
