// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_hash.h>
#include <bslstl_equalto.h>
#include <bslstl_pair.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>

#include <bslalg_bidirectionallink.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_stdtestallocator.h>

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//  Chief concerns are that operator() is callable by const-objects, accepts
//  const-references to arguments of the user-specified type, calls the
//  correctly overloaded operator, has the standard-mandated typedefs (but not
//  the base class, not required by C++11 spec) and is both bitwise movable
//  and bitwise copyable.
//  As QoI the class should be an empty class, and no operation should allocate
//  or consume memory, unless the user-suppled overloaded 'operator==' does so.
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

bool verbose;             
bool veryVerbose;         
bool veryVeryVerbose;    
bool veryVeryVeryVerbose; 

typedef bslalg::HashTableImpUtil ImpUtil;
typedef bslalg::BidirectionalLink Link;
typedef bsltf::StdTestAllocator<int> StlTestIntAllocator;
typedef ::bsl::hash<int> TestIntHash;
typedef ::bsl::equal_to<int> TestIntEqual;

namespace bsl {

template <class FIRST, class SECOND>
inline void debugprint(const bsl::pair<FIRST, SECOND>& p) 
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

// map-specific print function.
template <class KEY_POLICY, class HASHER, class EQUAL, class ALLOCATOR>
void debugprint(const bslstl::HashTable<KEY_POLICY, HASHER, EQUAL, ALLOCATOR>& 
                                                                            t)
{
    if (t.isEmpty()) {
        printf("<empty>");
    }
    else {

        for (Link *it = t.begin(); it != t.end(); ++it) {
            const typename KEY_POLICY::KeyType& key = 
                                           ImpUtil::extractKey<KEY_POLICY>(it);
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                       bsltf::TemplateTestFacility::getIdentifier(key)));
        }
    }
    fflush(stdout);
}

} // close namespace bsl
                       
template<class KEY, class MAPPED>
struct TestMapKeyPolicy
{
    typedef KEY KeyType;
    typedef MAPPED MappedType;
    typedef bsl::pair<const KEY, MAPPED> ValueType;

    static const KEY& extractKey(const ValueType& value) {
        return value.first;
    }
    
    static const KEY& extractValue(const ValueType& value) {
        return value.second;
    }
};
                       // =========================
                       // class CharToPairConverter
                       // =========================

template <class KEY, class VALUE>
class CharToPairConverter {
    // Convert a 'char' value to a 'bsl::pair' of the parameterized 'KEY' and
    // 'VALUE' type.

  public:
    std::pair<const KEY, VALUE> operator()(char value)
    {
        // Use different values for 'KEY' and 'VALUE'

        return bsl::pair<const KEY, VALUE> (
                bsltf::TemplateTestFacility::create<KEY>(value),
                bsltf::TemplateTestFacility::create<VALUE>(value - 'A' + '0'));
    }
};

template <class KEY_POLICY,
          class HASHER, 
          class EQUAL, 
          class ALLOCATOR>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'map'
    // container.  The parameterized 'KEY', 'COMP' and 'ALLOC' specifies the
    // value type, comparator type and allocator type respectively.  Each
    // "testCase*" method test a specific aspect of 'map<KEY, VALUE, COMP, ALLOC>'.
    // Every test cases should be invoked with various parameterized type to
    // fully test the container.

  private:
    // TYPES
    typedef bslstl::HashTable<KEY_POLICY, HASHER, EQUAL, ALLOCATOR> Obj;
        // Type under testing.

    typedef typename Obj::SizeType   SizeType;
    typedef typename Obj::KeyType     KeyType;
    typedef typename Obj::ValueType   ValueType;
        // Shorthands

    typedef bsltf::TestValuesArray<ValueType,
                          CharToPairConverter<KeyType, ValueType> > TestValues;

  public:
    typedef bsltf::StdTestAllocator<ValueType> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'map<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

  public:
    // TEST CASES
        static void testCase1(KeyType *testKeys, typename KEY_POLICY::MappedType *testValues, size_t n);
};
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

template <class KEY_POLICY, class HASHER, class EQUAL, class ALLOCATOR>
void TestDriver<KEY_POLICY, HASHER, EQUAL, ALLOCATOR> ::testCase1(
                                                   KeyType                              *testKeys,
                                                   typename KEY_POLICY::MappedType      *testValues,
                                                   size_t       numValues)
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Execute each methods to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------


    typedef bslstl::HashTable<KEY_POLICY, HASHER, EQUAL> Obj;
    typedef typename Obj::ValueType     Value;
    typedef bsl::pair<Link *, bool>     InsertResult;

    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Default construct an empty map.\n");
    }
    {
        Obj x(HASHER(), EQUAL(), 0, &objectAllocator); const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(true == X.isEmpty());
        ASSERTV(0    <  X.maxSize());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == objectAllocator.numBytesInUse());
    }

   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

   if (veryVerbose) {
       printf("Test use of allocators.\n");
   }
   {
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       Obj o1(HASHER(), EQUAL(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
           o1.insertContiguous(Value(testKeys[i], testValues[i]));
       }
       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());
   }
   {
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       Obj o1(HASHER(), EQUAL(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
           bool isInsertedFlag = false;
           o1.insertIfMissing(&isInsertedFlag, 
                              Value(testKeys[i], 
                              testValues[i]));
           ASSERTV(isInsertedFlag, true == isInsertedFlag);
       }
       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());
   }
   {
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       if(veryVerbose) printf("Use a different allocator\n");
       Obj o1(HASHER(), EQUAL(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
           o1.findOrInsertDefault(testKeys[i]);
       }

       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());

       
       if(veryVerbose) printf("Use a different allocator\n");
       {
           bslma::TestAllocatorMonitor monitor(&objectAllocator1);
           Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
           ASSERTV(&objectAllocator2 == O2.allocator().mechanism());
           ASSERTV(monitor.isInUseSame());
           ASSERTV(monitor.isTotalSame());
           ASSERTV(0 <  objectAllocator1.numBytesInUse());
           ASSERTV(0 <  objectAllocator2.numBytesInUse());
       }
       ASSERTV(0 ==  objectAllocator2.numBytesInUse());
           
       
       if(veryVerbose) printf("Copy construct O2(O1)\n");
           
       Obj o2(O1, &objectAllocator1); const Obj& O2 = o2;
       
       ASSERTV(&objectAllocator1 == O2.allocator().mechanism());

       ASSERTV(numValues == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       if(veryVerbose) printf("Default construct O3 and swap with O1\n");
       Obj o3(HASHER(), EQUAL(), 0, &objectAllocator1); const Obj& O3 = o3;
       ASSERTV(&objectAllocator1 == O3.allocator().mechanism());

       ASSERTV(numValues == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(0         == O3.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
       o1.swap(o3);
       ASSERTV(0         == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(numValues == O3.size());
       ASSERTV(monitor1.isInUseSame());
       ASSERTV(monitor1.isTotalSame());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       if(veryVerbose) printf("swap O3 with O2\n");
       o3.swap(o2);
       ASSERTV(0         == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(numValues == O3.size());
       ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                        // on implementation
       ASSERTV(!monitor1.isTotalUp());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());
       ASSERTV(&objectAllocator1 == O2.allocator().mechanism());
       ASSERTV(&objectAllocator1 == O3.allocator().mechanism());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testKeys, testKeys + numValues);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(HASHER(), EQUAL(), 0, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            Obj z(X, &objectAllocator); const Obj& Z = z;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, 0 == X.find(testKeys[i]));

            // Test 'insert'.
            Value value(testKeys[i], testValues[i]);
            bool isInsertedFlag = false;
            Link *link = x.insertIfMissing(&isInsertedFlag, value);
            ASSERTV(0             != link);
            ASSERTV(true          == isInsertedFlag);
            ASSERTV(testKeys[i]   == ImpUtil::extractKey<KEY_POLICY>(link));
            ASSERTV(Value(testKeys[i], testValues[i]) == 
                                      ImpUtil::extractValue<KEY_POLICY>(link));

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.isEmpty());

            // Test insert duplicate key
            ASSERTV(link    == x.insertIfMissing(&isInsertedFlag, value));
            ASSERTV(false   == isInsertedFlag);;
            ASSERTV(i + 1   == X.size());

            // Test find, operator[], at.
            ASSERTV(ImpUtil::extractKey<KEY_POLICY>(link) == 
                         ImpUtil::extractKey<KEY_POLICY>(X.find(testKeys[i])));
            // ASSERTV(testValues[i] == x[testKeys[i]]);
            // ASSERTV(testValues[i] == x.at(testKeys[i]));
            // ASSERTV(testValues[i] == X.at(testKeys[i]));

            // Test findOrInsertDefault
            ASSERTV(!(X == Z));
            ASSERTV(  X != Z);
            const Value& V = ImpUtil::extractValue<KEY_POLICY>(
                                           z.findOrInsertDefault(testKeys[i]));
            ASSERTV(Value(testKeys[i], typename KEY_POLICY::MappedType()) == V);
            //z[testKeys[i]] = testValues[i];
            //ASSERTV(testValues[i] == z[testKeys[i]]);
            //ASSERTV( (X == Z));
            //ASSERTV(!(X != Z));


            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());
        // Verify sorted order of elements.

//
//        // Test iterators.
//        {
//            const_iterator cbi  = X.begin();
//            const_iterator ccbi = X.cbegin();
//            iterator       bi   = x.begin();
//
//            const_iterator last = X.begin();
//            while (cbi != X.end()) {
//                ASSERTV(cbi == ccbi);
//                ASSERTV(cbi == bi);
//
//                if (cbi != X.begin()) {
//                    ASSERTV(comparator(last->first, cbi->first));
//                }
//                last = cbi;
//                ++bi; ++ccbi; ++cbi;
//            }
//
//            ASSERTV(cbi  == X.end());
//            ASSERTV(ccbi == X.end());
//            ASSERTV(bi   == X.end());
//            --bi; --ccbi; --cbi;
//
//            reverse_iterator       ri   = x.rbegin();
//            const_reverse_iterator rci  = X.rbegin();
//            const_reverse_iterator rcci = X.crbegin();
//
//            while  (rci != X.rend()) {
//                ASSERTV(cbi == ccbi);
//                ASSERTV(cbi == bi);
//                ASSERTV(rci == rcci);
//                ASSERTV(ri->first == rcci->first);
//
//                if (rci !=  X.rbegin()) {
//                    ASSERTV(comparator(cbi->first, last->first));
//                    ASSERTV(comparator(rci->first, last->first));
//                }
//
//                last = cbi;
//                if (cbi != X.begin()) {
//                    --bi; --ccbi; --cbi;
//                }
//                ++ri; ++rcci; ++rci;
//            }
//            ASSERTV(cbi  == X.begin());
//            ASSERTV(ccbi == X.begin());
//            ASSERTV(bi   == X.begin());
//
//            ASSERTV(rci  == X.rend());
//            ASSERTV(rcci == X.rend());
//            ASSERTV(ri   == x.rend());
//        }

        // Use erase(iterator) on all the elements.
        for (size_t i = 0; i < numValues; ++i) {
            Link *it     = x.find(testKeys[i]);
            Link *nextIt = it->nextLink();

            ASSERTV(0       != it);
            ASSERTV(testKeys[i]   == ImpUtil::extractKey<KEY_POLICY>(it));
            ASSERTV(Value(testKeys[i], testValues[i]) ==
                                        ImpUtil::extractValue<KEY_POLICY>(it));
            Link *resIt = x.remove(it);
            ASSERTV(resIt == nextIt);
            
            Link *resFind = x.find(testKeys[i]);
            ASSERTV(0 == resFind);
           
            ASSERTV(numValues - i - 1 == X.size());
        }
    } while (native_std::next_permutation(testKeys,
                                          testKeys + numValues));
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


    native_std::random_shuffle(testKeys,  testKeys + numValues);
    if (veryVerbose) {
        printf("Test 'erase(const key_type&)'.\n");
    }
    {
        Obj x(HASHER(), EQUAL(), 0, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Value value(testKeys[i], testValues[i]);
            Link *result1 = x.insertContiguous(value);
            ASSERTV(0 != result1);
            Link *result2 = x.insertContiguous(value);
            ASSERTV(0 != result2);
            ASSERTV(result1 != result2);
            ASSERTV(2 * (i + 1) == X.size());
            
            Link *start;
            Link *end;
            x.findRange(&start, &end, testKeys[i]);
            ASSERTV(ImpUtil::extractKey<KEY_POLICY>(start) == testKeys[i]);
            ASSERTV(ImpUtil::extractKey<KEY_POLICY>(start->nextLink()) == 
                                                                  testKeys[i]);
            ASSERTV(start->nextLink()->nextLink() == end);
        }
            
        for (size_t i = 0; i < numValues; ++i) {
            KeyType key = ImpUtil::extractKey<KEY_POLICY>(x.elementListRoot());
            Link *resIt1 = x.remove(x.elementListRoot());
            ASSERTV(x.find(key) == resIt1);
            ASSERTV(X.size(), (2 * numValues - (2 * (i + 1) - 1)) == X.size());
            Link *resIt2 = x.remove(x.elementListRoot());
            ASSERTV( x.find(key) == 0);
            ASSERTV(X.size(), (2 * numValues - 2 * (i + 1)) == X.size());
        }
    }

    if (veryVerbose) {
        printf("Test 'equal' and 'hasher'\n");
    }
   
}

//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//    if (veryVerbose) {
//        printf("Test 'erase(const_iterator, const_iterator )'.\n");
//    }
//    {
//        for (size_t i = 0; i < numValues; ++i) {
//            for (size_t j = 0; j < numValues; ++j) {
//                Obj x(comparator, &objectAllocator); const Obj& X = x;
//                for (size_t k = 0; k < numValues; ++k) {
//                    Value value(testKeys[k], testValues[k]);
//                    InsertResult result = x.insert(value);
//                }
//
//                const_iterator a = X.find(testKeys[i]);
//                const_iterator b = X.find(testKeys[j]);
//
//                if (!comparator(testKeys[i], testKeys[j])) {
//                    native_std::swap(a, b);
//                }
//                KEY min = a->first;
//                KEY max = b->first;
//                ASSERTV(!comparator(max, min)); // min <= max
//
//                size_t numElements = bsl::distance(a, b);
//                iterator endPoint = x.erase(a, b);
//
//                ASSERTV(numValues - numElements == X.size());
//                ASSERTV(endPoint                == b);
//
//                for (size_t k = 0; k < numValues; ++k) {
//                    if (comparator(testKeys[k], min) ||
//                        !comparator(testKeys[k], max)) {
//                        ASSERTV(testKeys[k] == X.find(testKeys[k])->first);
//                    }
//                    else {
//                        ASSERTV(X.end() == X.find(testKeys[k]));
//                    }
//                }
//            }
//        }
//
//        // Test 'erase(const_iterator, const_iterator )' for end of range.
//        for (size_t i = 0; i < numValues; ++i) {
//            Obj x(comparator, &objectAllocator); const Obj& X = x;
//            for (size_t k = 0; k < numValues - 1; ++k) {
//                // Insert 1 fewer than the total number of keys.
//
//                Value value(testKeys[k], testValues[k]);
//                InsertResult result = x.insert(value);
//            }
//
//            const_iterator a = X.find(testKeys[i]);
//            const_iterator b = X.end();
//            size_t numElements = bsl::distance(a, b);
//            iterator endPoint = x.erase(a, b);
//
//            ASSERTV(numValues - numElements - 1 == X.size());
//            ASSERTV(endPoint                    == b);
//        }
//    }
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//    if (veryVerbose) {
//        printf("Test insert & map for iterator ranges.\n");
//    }
//    {
//
//        typedef bsl::pair<KEY, VALUE> NonConstValue;
//        NonConstValue *myValues = new NonConstValue[numValues];
//        for (size_t i = 0; i < numValues; ++i) {
//            myValues[i].first  = testKeys[i];
//            myValues[i].second = testValues[i];
//        }
//
//        for (size_t i = 0; i < numValues; ++i) {
//            for (size_t length = 0; length <= numValues - i; ++length) {
//                Obj x(comparator, &objectAllocator); const Obj& X = x;
//                for (size_t k = 0; k < length; ++k) {
//                    size_t index = i + k;
//                    InsertResult result = x.insert(myValues[index]);
//                }
//                Obj y(comparator, &objectAllocator); const Obj& Y = y;
//                y.insert(myValues + i, myValues + (i + length));
//
//                Obj z(myValues + i,
//                      myValues + (i + length),
//                      comparator,
//                      &objectAllocator);
//                const Obj& Z = z;
//                ASSERTV(X == Y);
//                ASSERTV(X == Z);
//            }
//        }
//        delete [] myValues;
//    }
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//    if (veryVerbose) {
//        printf("Test 'equal_range'\n");
//    }
//    {
//        Obj x(comparator, &objectAllocator); const Obj& X = x;
//        for (size_t i = 0; i < numValues; ++i) {
//            Value value(testKeys[i], testValues[i]);
//            InsertResult result = x.insert(value);
//        }
//
//        for (size_t i = 0; i < numValues; ++i) {
//            pair<iterator, iterator> result = x.equal_range(testKeys[i]);
//            pair<const_iterator, const_iterator> cresult =
//                                                  X.equal_range(testKeys[i]);
//
//            ASSERTV(cresult.first  == result.first);
//            ASSERTV(cresult.second == result.second);
//
//            ASSERTV(result.first->first == testKeys[i]);
//            ASSERTV(X.end() == result.second ||
//                   result.second->first != testKeys[i]);;
//        }
//        for (size_t i = 0; i < numValues; ++i) {
//            x.erase(testKeys[i]);
//            pair<iterator, iterator> result = x.equal_range(testKeys[i]);
//            pair<const_iterator, const_iterator> cresult =
//                                                  x.equal_range(testKeys[i]);
//
//            iterator       li = x.lower_bound(testKeys[i]);
//            const_iterator LI = X.lower_bound(testKeys[i]);
//            iterator       ui = x.upper_bound(testKeys[i]);
//            const_iterator UI = X.upper_bound(testKeys[i]);
//
//            ASSERTV(result.first   == li);
//            ASSERTV(result.second  == ui);
//            ASSERTV(cresult.first  == LI);
//            ASSERTV(cresult.second == UI);
//        }
//    }
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//    if (veryVerbose) {
//        printf("Test 'operator<', 'operator>', 'operator<=', 'operator>='.\n");
//    }
//    {
//        // Iterate over possible selections of elements to add to two
//        // containers, 'X' and 'Y' then compare the results of the comparison
//        // operators to an "oracle" result from
//        // 'bslalg::RangeCompare::lexicographical' over the same range.
//
//        for (size_t i = 0; i < numValues; ++i) {
//            for (size_t j = 0; j < numValues; ++j) {
//                for (size_t length = 0; length < numValues; ++length) {
//                    Obj x(comparator, &objectAllocator); const Obj& X = x;
//                    Obj y(comparator, &objectAllocator); const Obj& Y = y;
//                    for (size_t k = 0; k < j; ++k) {
//                        int xIndex = (i + length) % numValues;
//                        int yIndex = (j + length) % numValues;
//
//                        Value xValue(testKeys[xIndex], testValues[xIndex]);
//                        x.insert(xValue);
//                        Value yValue(testKeys[yIndex], testValues[yIndex]);
//                        y.insert(yValue);
//                    }
//
//                    int comp = bslalg::RangeCompare::lexicographical(X.begin(),
//                                                                    X.end(),
//                                                                    Y.begin(),
//                                                                    Y.end());
//                    ASSERTV((comp < 0)  == (X < Y));
//                    ASSERTV((comp > 0)  == (X > Y));
//                    ASSERTV((comp <= 0) == (X <= Y));
//                    ASSERTV((comp >= 0) == (X >= Y));
//                }
//            }
//        }
//    }
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//    native_std::sort(testKeys, testKeys + numValues, comparator);
//    if (veryVerbose) {
//        printf("Test 'key_comp' and 'value_comp'.\n");
//    }
//    {
//        Obj x(comparator, &objectAllocator); const Obj& X = x;
//        typename Obj::key_compare   keyComp   = X.key_comp();
//        typename Obj::value_compare valueComp = X.value_comp();
//        for (size_t i = 0; i < numValues - 1; ++i) {
//            ASSERTV(keyComp(testKeys[i], testKeys[i+1]));
//            ASSERTV(valueComp(Value(testKeys[i],   testValues[i]),
//                              Value(testKeys[i+1], testValues[i+1])));
//        }
//    }
//}
//#endif

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

            typedef TestMapKeyPolicy<int, int> TestMapIntKeyPolicy;
            
            TestDriver<TestMapIntKeyPolicy, 
                       TestIntHash, 
                       TestIntEqual,
                       StlTestIntAllocator >::testCase1(INT_VALUES,
                                                        INT_VALUES,
                                                        NUM_INT_VALUES);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
