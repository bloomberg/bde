// bslstl_hashtable_array.t.cpp  -*-C++-*-
#include <bslstl_hashtable_array.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstddef>

using namespace BloombergLP;

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::size_t;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
// [ 2]  HashTable_Array(bslma::Allocator *);
// [ 3]  HashTable_Array(std::size_t size, bslma_Allocator *);
// [ 2]  ~HashTable_Array();       
// MANIPULATORS
// [ 2]  void clearAndResize(int );
// [ 2]  TYPE& operator[](int );
// [ 4]  iterator begin();
// [ 4]  iterator end();
// ACCESSORS
// [ 2]  int size() const;
// [ 2]  const TYPE& operator[](int ) const;
// [ 4]  const_iterator begin() const;
// [ 4]  const_iterator end() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHINGTEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl;
                                              // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;
                                              // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << std::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef std::size_t size_t;

//=============================================================================
//                  GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class ClassNoAlloc {
    // DATA
    size_t d_x;

  public:
    enum {
        INITIAL_VALUE   = 0xa1a1,
        DESTROYED_VALUE = 0xb2b2
    };

    ClassNoAlloc() : d_x(INITIAL_VALUE) {}
     ~ClassNoAlloc() { d_x = DESTROYED_VALUE; }
    size_t& value() { return d_x; }
    const size_t& value() const { return d_x; }
};

bool operator==(const ClassNoAlloc& lhs, const ClassNoAlloc& rhs)
{
    return lhs.value() == rhs.value();
}

typedef bslstl::HashTable_Array<ClassNoAlloc> NoAllocArray;

class ClassAlloc {

    // DATA
    size_t d_x;

    // NOT IMPLEMANTED
    ClassAlloc(const ClassAlloc& );
    ClassAlloc& operator=(const ClassAlloc& );

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ClassAlloc, 
                                 bslalg_TypeTraitUsesBslmaAllocator);

    enum {
        INVALID_CONSTRUCTOR = 0xdddd,
        INITIAL_VALUE       = 0xc1c1,
        DESTROYED_VALUE     = 0xd2d2
    };

    ClassAlloc() : d_x(INVALID_CONSTRUCTOR) {}
    ClassAlloc(bslma_Allocator *) : d_x(INITIAL_VALUE) {}
     ~ClassAlloc() { d_x = DESTROYED_VALUE; }
    size_t& value() { return d_x; }
    const size_t& value() const { return d_x; }
};
typedef bslstl::HashTable_Array<ClassAlloc> AllocArray;

typedef bslstl::HashTable_Array<int> IntArray;
//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test <<  endl;;

    bslma_TestAllocator testAllocator("ta", veryVeryVerbose);
    bslma_TestAllocator defaultAllocator("da", veryVeryVerbose);
    bslma_Allocator *Z = &testAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);
    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        
///Usage
///-----
// In the following example we demonstrate how to use a 'DynamicTagDriver_Array'
// to create an in place array of 'bdema_ManagedPtr' objects.  Note that
// 'bdema_ManagedPtr' does not provide a standard assignment or copy
// constructor method, so it cannot be used cleanly with a stl-vectory.
//
// We start by declaring an array of managed pointer's to 'bdet_Date' objects,
// of length 10.
//..
//  DynamicTagDriver_Array<bdema_ManagedPtr<bdet_Date> > dateArray(10);
//..
// Each element of the created array should be a default constructed
// 'bdema_ManagedPtr<bdet_Date>' object -- i.e., a null managed pointer:
//..
//  ASSERT(10 == dateArray.size());
//  for (int i = 0;  i < dateArray.size(); ++i) {
//      ASSERT(!dateArray[i]);
//  }
//..
// We can now allocate each element in the managed pointer array:
//..
//  bslma_TestAllocator allocator;
//  for (int i = 0;  i < dateArray.size(); ++i) {
//      dateArray[i].load(new (allocator) bdet_Date(), &allocator);
//      ASSERT(bdet_Date() == *dateArray[i]);
//  }
//  ASSERT(0 < allocator.numBytesInUse());
//..
// Finally we can use 'clearAndResize' to deallocate our array of managed
// pointers, and create a new array.  In this case we supply 'cleanAndResize'
// a new size of 0, so the array should be empty after the method returns:
//..
//  dateArray.clearAndResize(0);
//  ASSERT(0 == allocator.numBytesInUse());
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS:
        //   We want to verify the constructor and the primary manipulators.
        //
        // Plan:  
        //   Using one test type taking a 'bslma_Allocator' and one that
        //   does not, create an 'Array', size it to various lengths, and test
        //   the elements of an size of the array.
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl 
                          << "Testing Iterator" << endl
                          << "================" << endl;

        {
            if (veryVerbose) {
                cout << "\tTest with type without allocator" << endl;
            }
            
            NoAllocArray mX(Z); const NoAllocArray& X = mX;

            ASSERT(X.begin()  == X.end());
            ASSERT(mX.begin() == mX.end());
            ASSERT(X.begin()  == mX.begin());
            ASSERT(mX.end()   == X.end());

            for (size_t SIZE = 10; SIZE <= 30; SIZE+=10) {
                mX.clear();
                mX.resize(SIZE);
                ASSERT(SIZE == X.size());

                ASSERT(X.begin()  == mX.begin());
                ASSERT(mX.end()   == X.end());
                
                NoAllocArray::iterator       it = mX.begin();
                NoAllocArray::const_iterator IT = X.begin();
                for (size_t i = 0; i < SIZE; ++i) {
                    ASSERT(it < mX.end());
                    ASSERT(IT < X.end());
                    ASSERT(ClassNoAlloc::INITIAL_VALUE == it->value());
                    ASSERT(ClassNoAlloc::INITIAL_VALUE == IT->value());
                    ASSERT(ClassNoAlloc::INITIAL_VALUE == X[i].value());
                    ASSERT(&*it == &X[i]);
                    ASSERT(&*IT == &X[i]);

                    it->value() = i;

                    ASSERT(i == it->value());
                    ASSERT(i == IT->value());
                    ASSERT(i == X[i].value());
                    ASSERT(&*it == &X[i]);
                    ASSERT(&*IT == &X[i]);
                    ++it;
                    ++IT;
                }
                ASSERT(it == IT);
                ASSERT(it == mX.end());
                ASSERT(IT == X.end());
            }
            mX.clear();
            ASSERT(X.begin()  == X.end());
            ASSERT(mX.begin() == mX.end());
            ASSERT(X.begin()  == mX.begin());
            ASSERT(mX.end()   == X.end());
        }
        {
       //     if (veryVerbose) {
       //         cout << "\tTest with type with allocator" << endl;
       //     }
       //     
       //     AllocArray mX(Z); const AllocArray& X = mX;

       //     ASSERT(X.begin()  == X.end());
       //     ASSERT(mX.begin() == mX.end());
       //     ASSERT(X.begin()  == mX.begin());
       //     ASSERT(mX.end()   == X.end());

       //     for (size_t SIZE = 10; SIZE <= 30; SIZE+=10) {
       //         mX.clear();
       //         mX.resize(SIZE);
       //         ASSERT(SIZE == X.size());

       //         ASSERT(X.begin()  == mX.begin());
       //         ASSERT(mX.end()   == X.end());
       //         
       //         AllocArray::iterator       it = mX.begin();
       //         AllocArray::const_iterator IT = X.begin();
       //         for (size_t i = 0; i < SIZE; ++i) {
       //             ASSERT(it < mX.end());
       //             ASSERT(IT < X.end());
       //             ASSERT(ClassAlloc::INVALID_CONSTRUCTOR != it->value());
       //             ASSERT(ClassAlloc::INVALID_CONSTRUCTOR != IT->value());
       //             ASSERT(ClassAlloc::INVALID_CONSTRUCTOR != X[i].value());
       //             ASSERT(ClassAlloc::INITIAL_VALUE == it->value());
       //             ASSERT(ClassAlloc::INITIAL_VALUE == IT->value());
       //             ASSERT(ClassAlloc::INITIAL_VALUE == X[i].value());
       //             ASSERT(&*it == &X[i]);
       //             ASSERT(&*IT == &X[i]);

       //             it->value() = i;

       //             ASSERT(i == it->value());
       //             ASSERT(i == IT->value());
       //             ASSERT(i == X[i].value());
       //             ASSERT(&*it == &X[i]);
       //             ASSERT(&*IT == &X[i]);

       //             ++it;
       //             ++IT;
       //         }
       //         ASSERT(it == IT);
       //         ASSERT(it == mX.end());
       //         ASSERT(IT == X.end());
       //     }
       //     mX.clear();
       //     ASSERT(X.begin()  == X.end());
       //     ASSERT(mX.begin() == mX.end());
       //     ASSERT(X.begin()  == mX.begin());
       //     ASSERT(mX.end()   == X.end());
        }            
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATIVE CONSTRUCTOR:
        //   We want to verify the alternative constructor.
        //
        // Plan:  
        //   Using one test type taking a 'bslma_Allocator' and one that
        //   does not, create an 'Array' objects of various sizes, and test
        //   the elements and size of the created arrays.
        //
        // Testing:
        //   DynamicTagDriver_Array(int size, bslma_Allocator *);
        // --------------------------------------------------------------------
        if (verbose) cout << endl 
                          << "Testing Alternative Contructor" << endl
                          << "==============================" << endl;

        {
            if (veryVerbose) {
                cout << "\tTest with type without allocator" << endl;
            }
                       
            for (size_t i = 10; i <= 30; i += 10) {
                NoAllocArray mX(i, Z); const NoAllocArray& X = mX;
                ASSERT(1 == testAllocator.numBlocksInUse());
                ASSERT(i == X.size());
                for (size_t j = 0; j < X.size(); ++j) {
                    ASSERT(ClassNoAlloc::INITIAL_VALUE == X[j].value());
                    mX[j].value() = j;
                    ASSERT(j == X[j].value());
                }
                for (size_t j = 0; j < X.size(); ++j) {
                    ASSERT(j == X[j].value());
                }
            }
            ASSERT(0 == testAllocator.numBlocksInUse());
        }            
        {
            if (veryVerbose) {
                cout << "\tTest with type with allocator" << endl;
            }
                     
         //   for (size_t i = 10; i <= 30; i += 10) {
         //       AllocArray mX(i, Z); const AllocArray& X = mX;
         //       ASSERT(1 == testAllocator.numBlocksInUse());
         //       ASSERT(i == X.size());
         //       for (size_t j = 0; j < X.size(); ++j) {
         //           ASSERT(ClassAlloc::INVALID_CONSTRUCTOR != X[j].value());
         //           ASSERT(ClassAlloc::INITIAL_VALUE       == X[j].value());
         //           mX[j].value() = j;
         //           ASSERT(j == X[j].value());
         //       }
         //       for (size_t j = 0; j < X.size(); ++j) {
         //           ASSERT(j == X[j].value());
         //       }
         //  }
            ASSERT(0 == testAllocator.numBlocksInUse());
        }            
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //   We want to verify the constructor and the primary manipulators.
        //
        // Plan:  
        //   Using one test type taking a 'bslma_Allocator' and one that
        //   does not, create an 'Array', size it to various lengths, and test
        //   the elements of an size of the array.
        //
        // Testing:
        //   DynamicTagDriver_Array(bslma_Allocator *basicAllocator = 0);
        //   void clearAndResize(int size);
        //   TYPE& operator[](int index);
        //   int size() const;
        //   const TYPE& operator[](int index) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl 
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        {
            if (veryVerbose) {
                cout << "\tTest with type without allocator" << endl;
            }
            
            NoAllocArray mX(Z); const NoAllocArray& X = mX;
            ASSERT(0 == testAllocator.numBlocksInUse());
            ASSERT(0 == X.size());

            mX.clear();
            ASSERT(0 == testAllocator.numBlocksInUse());
            ASSERT(0 == X.size());
            
            for (size_t i = 10; i <= 30; i += 10) {
                mX.clear();
                mX.resize(i);
                ASSERT(1 == testAllocator.numBlocksInUse());
                ASSERT(i == X.size());
                for (size_t j = 0; j < X.size(); ++j) {
                    ASSERT(ClassNoAlloc::INITIAL_VALUE == X[j].value());
                    mX[j].value() = j;
                    ASSERT(j == X[j].value());
                }
                for (size_t j = 0; j < X.size(); ++j) {
                    ASSERT(j == X[j].value());
                }

                NoAllocArray mY(X, Z); 
                NoAllocArray::iterator itX = mX.begin();
                NoAllocArray::iterator itY = mY.begin();

                while (itX != mY.end() && itX != mX.end()) {
                    LOOP2_ASSERT(itY->value(), itX->value(), *itY == *itX);
                    ++itY; 
                    ++itX;
                }
                ASSERT(itY == mY.end());
                ASSERT(itX == mX.end());
            }
            mX.clear();
            ASSERT(0 == testAllocator.numBlocksInUse());
            ASSERT(0 == X.size());

            mX.resize(1);
            ASSERT(1 == testAllocator.numBlocksInUse());
            ASSERT(1 == X.size());
        }            
        ASSERT(0 == testAllocator.numBlocksInUse());

        {
        //    if (veryVerbose) {
        //        cout << "\tTest with type with allocator" << endl;
        //    }
        //    
        //    AllocArray mX(Z); const AllocArray& X = mX;
        //    ASSERT(0 == testAllocator.numBlocksInUse());
        //    ASSERT(0 == X.size());

        //    mX.clear();
        //    ASSERT(0 == testAllocator.numBlocksInUse());
        //    ASSERT(0 == X.size());
        //    
        //    for (size_t i = 10; i <= 30; i += 10) {
        //        mX.resize(i);
        //        ASSERT(1 == testAllocator.numBlocksInUse());
        //        ASSERT(i == X.size());
        //        for (size_t j = 0; j < X.size(); ++j) {
        //            ASSERT(ClassAlloc::INVALID_CONSTRUCTOR != X[j].value());
        //            ASSERT(ClassAlloc::INITIAL_VALUE       == X[j].value());
        //            mX[j].value() = j;
        //            ASSERT(j == X[j].value());
        //        }
        //        for (size_t j = 0; j < X.size(); ++j) {
        //            ASSERT(j == X[j].value());
        //        }
        //    }
        //    mX.clear();
        //    ASSERT(0 == testAllocator.numBlocksInUse());
        //    ASSERT(0 == X.size());

        //    mX.resize(1);
        //    ASSERT(1 == testAllocator.numBlocksInUse());
        //    ASSERT(1 == X.size());
        }            
        ASSERT(0 == testAllocator.numBlocksInUse());                
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            bslma_TestAllocator ta;
            {
                IntArray mX(&ta); const IntArray& X = mX;

                ASSERT(0         == X.size());
                ASSERT(X.begin() == X.end());
                ASSERT(0         == ta.numBytesInUse());
                
                mX.resize(10);
                
                ASSERT(10                   == X.size());
                ASSERT(X.begin() + X.size() == X.end());
                ASSERT(0                    <  ta.numBytesInUse());
                ASSERT(1                    == ta.numBlocksInUse());
                ASSERT(1                    == ta.numBlocksTotal());
                ASSERT(0                    == ta.numDeallocation());
                
                IntArray::iterator       it = mX.begin();
                IntArray::const_iterator IT = X.begin();
                
                for (int i = 0; i < 10; ++i) {
                    ++it; ++IT;
                }
                ASSERT(it == mX.end());
                ASSERT(IT == mX.end());
                
                mX.resize(5);
                
                ASSERT(5                    == X.size());
                ASSERT(X.begin() + X.size() == X.end());
                LOOP_ASSERT(ta.numBytesInUse(), 0 <  ta.numBytesInUse());
                LOOP_ASSERT(ta.numBlocksInUse(), 1 == ta.numBlocksInUse());
                LOOP_ASSERT(ta.numBlocksTotal(), 1 == ta.numBlocksTotal());
                LOOP_ASSERT(ta.numDeallocation(), 0 == ta.numDeallocation());
                
                it = mX.begin();
                IT = X.begin();
                
                for (size_t i = 0; i < 5; ++i) {
                    ASSERT(it     == IT);
                    ASSERT(&mX[i] == it);
                    ASSERT(&X[i]  == IT);
                    ++it; ++IT;
                }
                ASSERT(it == mX.end());
                ASSERT(IT == mX.end());
                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());
            LOOP_ASSERT(ta.numBytesInUse(),   0 == ta.numBytesInUse());
            LOOP_ASSERT(ta.numDeallocation(), 1 == ta.numDeallocation());
        }
        {
       //     bslma_TestAllocator ta;
       //     {
       //         StrArray mX(&ta); const StrArray& X = mX;

       //         ASSERT(0         == X.size());
       //         ASSERT(X.begin() == X.end());
       //         ASSERT(0         == ta.numBytesInUse());
       //         
       //         mX.resize(10);
       //         
       //         ASSERT(10                   == X.size());
       //         ASSERT(X.begin() + X.size() == X.end());
       //         ASSERT(0                    <  ta.numBytesInUse());
       //         ASSERT(0                    <  ta.numBlocksInUse());
       //         ASSERT(0                    <  ta.numBlocksTotal());
       //         ASSERT(0                    == ta.numDeallocation());
       //         
       //         StrArray::iterator       it = mX.begin();
       //         StrArray::const_iterator IT = X.begin();
       //         
       //         for (size_t i = 0; i < 10; ++i) {
       //             ASSERT(*it == bsl::string());
       //             ASSERT(*IT == bsl::string());
       //             ASSERT(it     == IT);
       //             ASSERT(&mX[i] == it);
       //             ASSERT(&X[i]  == IT);
       //             ++it; ++IT;
       //         }
       //         ASSERT(it == mX.end());
       //         ASSERT(IT == mX.end());
       //         
       //         mX.clearAndResize(5);
       //         
       //         ASSERT(5                    == X.size());
       //         ASSERT(X.begin() + X.size() == X.end());
       //         ASSERT(0                    <  ta.numBytesInUse());
       //         ASSERT(0                    <  ta.numBlocksInUse());
       //         ASSERT(0                    <  ta.numBlocksTotal());
       //         ASSERT(0                    <  ta.numDeallocation());
       //         
       //         it = mX.begin();
       //         IT = X.begin();
       //         
       //         for (size_t i = 0; i < 5; ++i) {
       //             ++it; ++IT;
       //         }
       //         ASSERT(it == mX.end());
       //         ASSERT(IT == mX.end());
       //         ASSERT(0 == defaultAllocator.numBytesInUse());
       //     }
       //     ASSERT(0 == defaultAllocator.numBytesInUse());
       //     ASSERT(0 == ta.numBytesInUse());
        }        
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
