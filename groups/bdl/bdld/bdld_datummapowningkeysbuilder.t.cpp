// bdld_datummapowningkeysbuilder.t.cpp                               -*-C++-*-
#include <bdld_datummapowningkeysbuilder.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;
using namespace BloombergLP::bdld;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility for building 'Datum' objects holding
// maps (owning keys) of 'Datum' objects.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] explicit DatumMapOwningKeysBuilder(bslma::Allocator *);
// [ 2] DatumMapOwningKeysBuilder(SizeType, SizeType, bslma::Allocator *);
// [ 5] ~DatumMapOwningKeysBuilder();
// MANIPULATORS
// [ 4] void pushBack(const bslstl::StringRef&, const Datum&);
// [ 2] void append(const DatumMapEntry *, int);
// [ 2] Datum commit();
// [ 7] void setSorted(bool);
// [ 6] Datum sortAndCommit();
// ACCESSORS
// [ 2] SizeType capacity() const;
// [ 2] SizeType keysBufferCapacity() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
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

bsl::size_t nextEntry(bsl::string        *key,
                      bsl::string        *value,
                      const bsl::string&  input)
// Extract the next key and value pair from a list of comma separated
// entries in the specified 'input' string.  Load the key into the specified
// 'key' and value in the specified 'value'.  Return the index of the next
// entry within 'input'.
{
    if (input.empty() || input[0] != '(') {
        return bsl::string::npos;
    }

    const bsl::size_t start = 0;
    const bsl::size_t nextIndex = input.find(')', start);
    if (bsl::string::npos == nextIndex) {
        return bsl::string::npos;
    }

    const bsl::size_t keyIndex = start + 1;
    const bsl::size_t valueIndex = input.find(',', keyIndex);
    if (valueIndex < nextIndex) {
        *key = input.substr(keyIndex, valueIndex - keyIndex);
        *value = input.substr(valueIndex + 1, nextIndex - valueIndex - 1);
        return nextIndex == (input.size() - 1) ? bsl::string::npos :
                                                 nextIndex + 1;
    }

    return bsl::string::npos;
}

Datum convertToDatum(const bsl::string&  value,
                     bslma::Allocator   *basicAllocator)
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
        return Datum::createBoolean(
            "true" == value ? true : false);
    }
    else { // string value
        return Datum::copyString(value.c_str(),
                                 value.size(),
                                 basicAllocator);
    }
}

bool compareGreater(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is greater than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() > rhs.key();
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
    case 8: {
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

        const bsl::string input(
            "(first,2.34),(second,4),(third,hi there),(fourth,true)", alloc);

        // Create a builder object.
        DatumMapOwningKeysBuilder builder(alloc);

        bsl::string str(input, alloc);

        bsl::string key;
        bsl::string value;
        int numEntries = 0;
        bsl::size_t nextIndex;
        do {
            nextIndex = nextEntry(&key, &value, str);
            builder.pushBack(key, convertToDatum(value, alloc));
            ++numEntries;
            if (bsl::string::npos == nextIndex) {
                break;
            }
            str = str.substr(nextIndex + 1);
        } while (bsl::string::npos != nextIndex);

        Datum result = builder.commit();

        ASSERT(result.isMap());
        ASSERT(numEntries == result.theMap().size());
        ASSERT("first" == bsl::string(result.theMap()[0].key()));
        ASSERT(result.theMap()[0].value().isDouble());
        ASSERT(2.34 == result.theMap()[0].value().theDouble());
        ASSERT("second" == bsl::string(result.theMap()[1].key()));
        ASSERT(result.theMap()[1].value().isInteger());
        ASSERT(4 == result.theMap()[1].value().theInteger());
        ASSERT("third" == bsl::string(result.theMap()[2].key()));
        ASSERT(result.theMap()[2].value().isString());
        ASSERT("hi there" == result.theMap()[2].value().theString());
        ASSERT("fourth" == bsl::string(result.theMap()[3].key()));
        ASSERT(result.theMap()[3].value().isBoolean());
        ASSERT(true == result.theMap()[3].value().theBoolean());

        // Destroy the 'Datum' object.
        Datum::destroy(result, alloc);

    } break;
    case 7: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS - 3
        //
        // Concerns:
        //    'setSorted' should change the sort flag.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object.  Append few elements
        //    to the map in a sorted order.  Set the sort flag.  Get a 'Datum'
        //    object by calling 'commit' and verify it has expected values.
        //    Also verify that the elements in the map are still in sorted
        //    order.  Create another 'DatumMapBuilder' object.  Append few
        //    elements to map.  Set the sort flag and then reset it.  Get a
        //    'Datum' object by calling 'commit' and verify it has expected
        //    values.
        //
        // Testing:
        //    void setSorted();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS - 3" << endl
                          << "========================" << endl;
        {
            bslstl::StringRef keys[] = {
                "first",
                "second",
                "third",
                "fourth",
                "fifth"
            };
            const int numElements = sizeof(keys) / sizeof(bslstl::StringRef);
            bsl::sort(keys, keys + numElements);

            DatumMapOwningKeysBuilder builder(alloc);
            for (int i = 0; i < numElements; ++i) {
                builder.pushBack(keys[i], Datum::createInteger(i + 1));
            }
            builder.setSorted(true);
            Datum result = builder.commit();

            ASSERT(result.isMap());
            const ConstDatumMapRef ref = result.theMap();
            ASSERT(ref.size() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, 0 != ref.find(keys[i]));
            }

            ASSERT(bsl::adjacent_find(ref.data(),
                                      ref.data() + ref.size(),
                                      compareGreater)
                       == ref.data() + ref.size());

            Datum::destroy(result, alloc);
            ASSERT(0 == ta.status());
        }

        {
            bslstl::StringRef keys[] = {
                "first",
                "second",
                "third",
                "fourth",
                "fifth"
            };
            const int numElements = sizeof(keys) / sizeof(bslstl::StringRef);

            DatumMapOwningKeysBuilder builder(alloc);
            builder.setSorted(true);
            for (int i = 0; i < numElements; ++i) {
                builder.pushBack(keys[i], Datum::createInteger(i + 1));
            }
            builder.setSorted(false);
            Datum result = builder.commit();

            ASSERT(result.isMap());
            const ConstDatumMapRef ref = result.theMap();
            ASSERT(ref.size() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i,
                            ref[i] == DatumMapEntry(
                                                 keys[i],
                                                 Datum::createInteger(i + 1)));
            }

            Datum::destroy(result, alloc);
            ASSERT(0 == ta.status());
        }

    } break;
    case 6: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS - 2
        //
        // Concerns:
        //    'sortAndCommit' should sort the elements before adopting the map.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object.  Append few elements
        //    to the map.  Get a 'Datum' object by calling 'sortAndCommit' and
        //    verify it has expected values.  Also verify that the elements in
        //    the map have been sorted.
        //
        // Testing:
        //    Datum sortAndCommit();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS - 2" << endl
                          << "========================" << endl;
        DatumMapEntry values[] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(2)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::createBoolean(true)),
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::copyString("hi there", alloc)),
            DatumMapEntry(StringRef("fourth", strlen("fourth")),
                          Datum::createDouble(2.3)),
            DatumMapEntry(StringRef("fifth", strlen("fifth")),
                          Datum::createDate(bdlt::Date(2012, 11, 9)))
        };
        const int numElements = sizeof(values) / sizeof(DatumMapEntry);

        DatumMapOwningKeysBuilder builder(alloc);
        builder.append(values, numElements);
        Datum result = builder.sortAndCommit();

        ASSERT(result.isMap());
        const ConstDatumMapRef ref = result.theMap();
        ASSERT(ref.size() == numElements);
        for (int i = 0; i < numElements; ++i) {
            LOOP_ASSERT(i, 0 != ref.find(values[i].key()));
        }

        ASSERT(bsl::adjacent_find(ref.data(),
                                  ref.data() + ref.size(),
                                  compareGreater) == ref.data() + ref.size());

        Datum::destroy(result, alloc);
        ASSERT(0 == ta.status());

    } break;
    case 5: {
        // --------------------------------------------------------------------
        // TESTING DESTRUCTOR
        //
        // Concerns:
        //    The destructor should destroy the map if it is not committed.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object using the explicit
        //    constructor.  Push back few elements to the map. Destroy the
        //    'DatumMapOwningKeysBuilder' object and verify that the map has
        //    been destroyed.  Verify no memory was leaked.
        //
        // Testing:
        //    ~DatumMapOwningKeysBuilder();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING DESTRUCTOR" << endl
                          << "==================" << endl;

        const int numElements = 3;
        DatumMapEntry values[numElements] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(2)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::createBoolean(true)),
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::copyString("hi there", alloc))
        };
        const int numAllocations = ta.numAllocations();

        {
            DatumMapOwningKeysBuilder builder(alloc);
            ASSERT(numAllocations == ta.numAllocations());

            builder.pushBack(values[0].key(), values[0].value());
            ASSERT(numAllocations < ta.numAllocations());

            builder.pushBack(values[1].key(), values[1].value());
            builder.pushBack(values[2].key(), values[2].value());
        }

        ASSERT(0 == ta.status());

    } break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS - 1
        //
        // Concerns:
        //    The explicit constructor should create a
        //    'DatumMapOwningKeysBuilder' object with the zero capacity
        //    keys-capacity.  The manipulator should push back the given
        //    element and grow the map.  The destructor should not destroy the
        //    map once it is committed.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object using the explicit
        //    constructor and verify the capacity.  Push back few elements to
        //    the map and verify that the capacity and keys-capacity have
        //    increased.  Get a 'Datum' object by committing the map and verify
        //    it has expected values.  Destroy the 'DatumMapBuilder' object and
        //    verify that the map still exists.  Verify no memory was leaked.
        //
        // Testing:
        //    void pushBack(const bslstl::StringRef&, const Datum&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS - 1" << endl
                          << "========================" << endl;

        Datum result;
        const int numElements = 3;
        DatumMapEntry values[numElements] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(2)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::createBoolean(true)),
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::copyString("hi there", alloc))
        };

        {
            DatumMapOwningKeysBuilder builder(alloc);
            ASSERT(0 == builder.capacity());
            ASSERT(0 == builder.keysCapacity());

            builder.pushBack(values[0].key(), values[0].value());
            ASSERT(0 < builder.capacity());
            ASSERT(0 < builder.keysCapacity());

            builder.pushBack(values[1].key(), values[1].value());
            builder.pushBack(values[2].key(), values[2].value());

            result = builder.commit();
            ASSERT(result.isMap());
            ASSERT(result.theMap().size() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, result.theMap()[i] == values[i]);
            }
        }

        ASSERT(result.isMap());
        ASSERT(result.theMap().size() == numElements);
        for (int i = 0; i < numElements; ++i) {
            LOOP_ASSERT(i, result.theMap()[i] == values[i]);
        }
        Datum::destroy(result, alloc);
        ASSERT(0 == ta.status());

    } break;
    case 3: {
        // --------------------------------------------------------------------
        // EMPTY MAP TEST
        //
        // Concerns:
        //    The explicit constructor should create a
        //    'DatumMapOwningKeysBuilder' object with zero capacity and
        //    zero keys-capacity.  When no elements are appended and 'commit'
        //    is called, an empty map should be created.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object using the explicit
        //    constructor and verify that the capacity and the keys-capacity
        //    re 0.  Call 'commit' and verify that an empty map was created.
        //    Destroy the 'DatumMapOwningKeysBuilder' object and verify that
        //    the map still exists.
        //
        // Testing:
        //    explicit DatumMapOwningKeysBuilder(bslma::Allocator *);
        //    ~DatumMapOwningKeysBuilder();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "EMPTY MAP TEST" << endl
                          << "================" << endl;

        {
            Datum result;

            {
                DatumMapOwningKeysBuilder builder(alloc);
                ASSERT(0 == builder.capacity());
                ASSERT(0 == builder.keysCapacity());
                ASSERT(0 == ta.status());

                result = builder.commit();
                ASSERT(0 == ta.status());

                ASSERT(result.isMap());
                ASSERT(0 == result.theMap().size());
            }

            ASSERT(result.isMap());
            ASSERT(0 == result.theMap().size());
            ASSERT(0 == ta.status());
        }

    } break;
    case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND PRIMARY MANIPULATORS
        //
        // Concerns:
        //    The constructor should create a 'DatumMapOwningKeysBuilder'
        //    object with the correct capacities.  The manipulator should
        //    append the given elements and grow the map if it's size exceeds
        //    the capacity or if there is not enough space for storing the
        //    keys.  The destructor should not destroy the map once it is
        //    committed.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object with a given capacity
        //    and a given keys-capacity which is sufficient for storing all the
        //    keys.  Verify both the capacities.  Append a range of elements to
        //    that the size of the map exceeds its capacity.  Verify that the
        //    capacity has increased, but keys-capacity is still the same.  Get
        //    a 'Datum' object by committing the map and verify it has the
        //    expected values.  Destroy the 'DatumMapOwningKeysBuilder' object
        //    and verify that the map still exists.  Verify no memory was
        //    leaked.
        //
        //    Next create a 'DatumMapOwningKeysBuilder' object with a given
        //    capacity that is sufficient to store all the elements in the map
        //    and a given keys-capacity.  Verify both the capacities.  Append a
        //    range of elements to the map such that the storage for keys is
        //    not sufficient to store all the keys.  Verify that the
        //    keys-capacity has increased, but the map capacity is still the
        //    same.  Get a 'Datum' object by committing the map and verify it
        //    has the expected values.  Destroy the 'DatumMapOwningKeysBuilder'
        //    object and verify that the map still exists.  Verify no memory
        //    was leaked.
        //
        // Testing:
        //    DatumMapOwningKeysBuilder(SizeType, SizeType bslma::Allocator *);
        //    ~DatumMapOwningKeysBuilder();
        //    void append(const DatumMapEntry *, int);
        //    Datum commit();
        //    SizeType capacity() const;
        //    SizeType keysBufferCapacity() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CREATORS AND PRIMARY MANIPULATORS"
                          << endl
                          << "========================================="
                          << endl;

        {
            Datum result;
            const int numElements = 3;
            DatumMapEntry values[numElements] = {
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(2)),
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createBoolean(true)),
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("hi there", alloc))
            };

            {
                const int capacity = 2;
                int keysBufferCapacity = 0;
                for (int i = 0; i < numElements; ++i) {
                    keysBufferCapacity += values[i].key().length();
                }
                DatumMapOwningKeysBuilder builder(capacity,
                                                  keysBufferCapacity,
                                                  alloc);
                ASSERT(capacity == builder.capacity());
                ASSERT(keysBufferCapacity == builder.keysCapacity());

                builder.append(values, numElements);
                ASSERT(capacity < builder.capacity());
                ASSERT(keysBufferCapacity == builder.keysCapacity());

                result = builder.commit();
                ASSERT(result.isMap());
                ASSERT(result.theMap().size() == numElements);
                for (int i = 0; i < numElements; ++i) {
                    LOOP_ASSERT(i, result.theMap()[i] == values[i]);
                }
            }

            ASSERT(result.isMap());
            ASSERT(result.theMap().size() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, result.theMap()[i] == values[i]);
            }
            Datum::destroy(result, alloc);
            ASSERT(0 == ta.status());
        }

        {
            Datum result;
            const int numElements = 3;
            DatumMapEntry values[numElements] = {
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(2)),
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createBoolean(true)),
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("hi there", alloc))
            };

            {
                const int capacity = 3;
                int keysBufferCapacity = 10;
                DatumMapOwningKeysBuilder builder(capacity,
                                                  keysBufferCapacity,
                                                  alloc);
                ASSERT(capacity == builder.capacity());
                ASSERT(keysBufferCapacity == builder.keysCapacity());

                builder.append(values, numElements);
                ASSERT(capacity == builder.capacity());
                ASSERT(keysBufferCapacity < builder.keysCapacity());

                result = builder.commit();
                ASSERT(result.isMap());
                ASSERT(result.theMap().size() == numElements);
                for (int i = 0; i < numElements; ++i) {
                    LOOP_ASSERT(i, result.theMap()[i] == values[i]);
                }
            }

            ASSERT(result.isMap());
            ASSERT(result.theMap().size() == numElements);
            for (int i = 0; i < numElements; ++i) {
                LOOP_ASSERT(i, result.theMap()[i] == values[i]);
            }
            Datum::destroy(result, alloc);
            ASSERT(0 == ta.status());
        }

    } break;
    case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //    Exercise basic functionality before beginning testing in earnest.
        //
        // Plan:
        //    Create a 'DatumMapOwningKeysBuilder' object and verify that no
        //    memory is leaked on destruction.  Display object values
        //    frequently in verbose mode.
        //
        // Testing:
        //     This test case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            DatumMapOwningKeysBuilder builder(alloc);
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
