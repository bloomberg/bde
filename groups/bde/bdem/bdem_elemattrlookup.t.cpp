// bdem_elemattrlookup.t.cpp                  -*-C++-*-

#include <bdem_elemattrlookup.h>

#include <bslma_testallocator.h>
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

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
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following function tests the functionality of one of the bdem types:
//..
    void testType(bdem_ElemType::Type objType, const void* otherObj) {
        bslma_TestAllocator testAlloc;
//..
// First, we look up the descriptor for 'objType' in the 'bdem_ElemAttrLookup'
// lookup table:
//..
        const bdem_Descriptor *desc =
            bdem_ElemAttrLookup::lookupTable()[objType];
//..
// Now, we use the size and constructor members of the descriptor to construct
// an unset object of 'objType' and test that it is unset.
//..
        void *obj = testAlloc.allocate(desc->d_size);
        desc->unsetConstruct(obj, bdem_AggregateOption::BDEM_PASS_THROUGH,
                             &testAlloc);
        ASSERT(desc->isUnset(obj));
//..
// Copy 'otherObj' to 'obj' and verify the copy.
//..
        desc->assign(obj, otherObj);
        ASSERT(desc->areEqual(obj, otherObj));
//..
// Then destroy and deallocate the object:
//..
        desc->destroy(obj);
        testAlloc.deallocate(obj);
    }
//..
// The main program tests string and vector-of-double.  It also tests the VOID
// type.
//..
    int usageExample1() {
        bsl::string obj1("Hello");
        bsl::vector<double> obj2;
        obj2.push_back(-1.2);
        obj2.push_back(5.8e4);

        testType(bdem_ElemType::BDEM_STRING, &obj1);
        testType(bdem_ElemType::BDEM_DOUBLE_ARRAY, &obj2);
        testType(bdem_ElemType::BDEM_VOID, 0);

        return 0;
    }
//..

//=============================================================================
//                  SUPPORT TYPES
//-----------------------------------------------------------------------------

// The Sun compiler doesn't work with explicit template instantiation,
// so we must resort creating instances.  Fortunately, these templates have
// only one member each, which we can reference and thus force instantiation.
typedef bdem_ElemStreamInAttrLookup<bdex_TestInStream> TestInLookup;
typedef bdem_ElemStreamOutAttrLookup<bdex_TestOutStream> TestOutLookup;

const bdem_DescriptorStreamIn<bdex_TestInStream>* inLookup =
                 TestInLookup::lookupTable();
const bdem_DescriptorStreamOut<bdex_TestOutStream>* outLookup =
               TestOutLookup::lookupTable();

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
//    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4; // globalVeryVeryVerbose =
                                       //                      veryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // Usage example
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample1();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is provided to facilitate development.  Note that this
        //   breathing test exercises basic functionality, but tests nothing.
        //   Note also that testing for 'const'-correctness of copy
        //   construction and assuring compile-time errors for re-binding
        //   (assignment) are negative tests that currently must be performed
        //   manually by selectively uncommenting code.
        //
        // Plan:
        //   Do what ever is necessary to bring this component to a level
        //   where it can be tested thoroughly.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Bruit-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;
        {

            const bdem_DescriptorStreamIn<bdex_TestInStream>* inLookup =
              bdem_ElemStreamInAttrLookup<bdex_TestInStream>::lookupTable();
            ASSERT(inLookup);  // use
            const bdem_DescriptorStreamOut<bdex_TestOutStream>* outLookup =
              bdem_ElemStreamOutAttrLookup<bdex_TestOutStream>::lookupTable();

            int a = 0x12345678;
            bdex_TestOutStream strm;

            outLookup[(int)bdem_ElemType::BDEM_INT].streamOut(&a, strm, 1, 0);

            if (verbose) {
                bsl::cout << "a = [" << strm.length() << ']';
                for (int i = 0; i < strm.length(); ++i) {
                    bsl::cout << bsl::hex
                              << ' '
                              << ((unsigned) strm.data()[i] & 0xffU);
                }
                bsl::cout << bsl::endl;
            }

            static const unsigned char expectedOut[5] =
                                                {0xe6, 0x12, 0x34, 0x56, 0x78};

            ASSERT(strm.length() == sizeof(expectedOut));
            ASSERT(0 == bsl::memcmp(strm.data(),
                                    expectedOut,
                                    sizeof(expectedOut)));

        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error: non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
