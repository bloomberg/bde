// bslalg_bidirectionallinklistutil.t.cpp                             -*-C++-*-

#include <bslalg_bidirectionallinklistutil.h>

#include <bslalg_bidirectionallink.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_allocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bslalg;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] void insertLinkBeforeTarget(Link *newNode, Link* target);
// [  ] void insertLinkAfterTarget(Link *newNode, Link *target);
// [  ] bool isWellFormed(Link *head, Link *tail);
// [  ] void spliceListBeforeTarget(Link *first, Link *last, Link *target);
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
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
#define lOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
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


typedef BidirectionalLink Link;
typedef BidirectionalLinkListUtil Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dm(&da);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);

        Link *DEFAULT1 = (Link *)(0xdeadbeef);
        Link *DEFAULT2 = (Link *)(0xfeedbeef);

// [  ] void insertLinkBeforeTarget(Link *newNode, Link* target);
// [  ] void insertLinkAfterTarget(Link *newNode, Link *target);
// [  ] bool isWellFormed(Link *head, Link *tail);
// [  ] void spliceListBeforeTarget(Link *first, Link *last, Link *target);
        if(veryVerbose) printf("Testing 'insertLinkBeforeTarget'"
                               " and 'unlink'\n");
        {
            if(veryVeryVerbose) printf("Create H\n");
            Link head; Link *H = &head;
            head.reset();
            ASSERTV(Obj::isWellFormed(H, H));

            // x-H-x

            Link link1; Link *L1 = &link1;
            link1.setNextLink(DEFAULT1);
            link1.setPreviousLink(DEFAULT2);

            if(veryVeryVerbose) printf("Insert L1 before H\n");
            Obj::insertLinkBeforeTarget(L1, H);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == H);
            ASSERTV(L1->previousLink() == 0);
            ASSERTV(Obj::isWellFormed(L1, H));

            // x-L1-H-x

            if(veryVeryVerbose) printf("Unlink L1\n");
            Obj::unlink(L1);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == H);
            ASSERTV(L1->previousLink() == 0);
            ASSERTV(Obj::isWellFormed(H, H));

            // x-H-x

            if(veryVeryVerbose) printf("Reinsert L1 before H\n");
            Obj::insertLinkBeforeTarget(L1, H);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == H);
            ASSERTV(L1->previousLink() == 0);
            ASSERTV(Obj::isWellFormed(L1, H));

            // x-L1-H-x

            if(veryVeryVerbose) printf("Insert L2 before H\n");
            Link link2; Link *L2 = &link2;
            link2.setNextLink(DEFAULT1);
            link2.setPreviousLink(DEFAULT2);

            Obj::insertLinkBeforeTarget(L2, H);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == H);
            ASSERTV(L2->previousLink() == L1);
            ASSERTV(Obj::isWellFormed(L2, H));
            ASSERTV(Obj::isWellFormed(L1, H));

            // x-L1-L2-H-x

            if(veryVeryVerbose) printf("Unlink L2\n");
            Obj::unlink(L2);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == H);
            ASSERTV(L2->previousLink() == L1);
            ASSERTV(Obj::isWellFormed(L1, H));

            // x-L1-H-x

            if(veryVeryVerbose) printf("Insert L2 before H\n");
            Obj::insertLinkBeforeTarget(L2, H);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == H);
            ASSERTV(L2->previousLink() == L1);
            ASSERTV(Obj::isWellFormed(L2, H));
            ASSERTV(Obj::isWellFormed(L1, H));

            // x-L1-L2-H-x

            if(veryVeryVerbose) printf("Insert L3 before L1\n");
            Link link3; Link *L3 = &link3;
            link3.setNextLink(DEFAULT1);
            link3.setPreviousLink(DEFAULT2);

            Obj::insertLinkBeforeTarget(L3, L1);
            ASSERTV(L3->nextLink() != DEFAULT1);
            ASSERTV(L3->previousLink() != DEFAULT1);
            ASSERTV(L3->nextLink() == L1);
            ASSERTV(L3->previousLink() == 0);
            ASSERTV(Obj::isWellFormed(L3, H));

            // x-L3-L1-L2-H-x

            if(veryVeryVerbose) printf("Remove all\n");
            Obj::unlink(H);
            Obj::isWellFormed(L3, L2);
            Obj::isWellFormed(L1, L2);
            Obj::isWellFormed(L2, L2);
            Obj::unlink(L2);
            Obj::isWellFormed(L3, L1);
            Obj::isWellFormed(L1, L1);
            Obj::unlink(L1);
            Obj::isWellFormed(L3, L3);
        }
        ASSERTV(dm.isTotalSame());
        ASSERTV(om.isTotalSame());

        if(veryVerbose) printf("Testing 'insertLinkAfterLink'"
                               " and 'unlink'\n");
        {
            if(veryVeryVerbose) printf("Create H\n");
            Link head; Link *H = &head;
            head.reset();
            ASSERTV(Obj::isWellFormed(H, H));

            // x-H-x

            Link link1; Link *L1 = &link1;
            link1.setNextLink(DEFAULT1);
            link1.setPreviousLink(DEFAULT2);

            if(veryVeryVerbose) printf("Insert L1 after H\n");
            Obj::insertLinkAfterTarget(L1, H);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == 0);
            ASSERTV(L1->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L1-x

            if(veryVeryVerbose) printf("Unlink L1\n");
            Obj::unlink(L1);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == 0);
            ASSERTV(L1->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, H));

            // x-H-x

            if(veryVeryVerbose) printf("Reinsert L1 after H\n");
            Obj::insertLinkAfterTarget(L1, H);
            ASSERTV(L1->nextLink() != DEFAULT1);
            ASSERTV(L1->previousLink() != DEFAULT1);
            ASSERTV(L1->nextLink() == 0);
            ASSERTV(L1->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L1-x

            if(veryVeryVerbose) printf("Insert L2 after H\n");
            Link link2; Link *L2 = &link2;
            link2.setNextLink(DEFAULT1);
            link2.setPreviousLink(DEFAULT2);

            Obj::insertLinkAfterTarget(L2, H);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == L1);
            ASSERTV(L2->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L2-L1-x

            if(veryVeryVerbose) printf("Unlink L2\n");
            Obj::unlink(L2);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == L1);
            ASSERTV(L2->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L1-x

            if(veryVeryVerbose) printf("Re Insert L2 after H\n");
            Obj::insertLinkAfterTarget(L2, H);
            ASSERTV(L2->nextLink() != DEFAULT1);
            ASSERTV(L2->previousLink() != DEFAULT1);
            ASSERTV(L2->nextLink() == L1);
            ASSERTV(L2->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L2-L1-x

            if(veryVeryVerbose) printf("Insert L3 after H\n");
            Link link3; Link *L3 = &link3;
            link3.setNextLink(DEFAULT1);
            link3.setPreviousLink(DEFAULT2);

            Obj::insertLinkAfterTarget(L3, H);
            ASSERTV(L3->nextLink() != DEFAULT1);
            ASSERTV(L3->previousLink() != DEFAULT1);
            ASSERTV(L3->nextLink() == L2);
            ASSERTV(L3->previousLink() == H);
            ASSERTV(Obj::isWellFormed(H, L1));

            // x-H-L3-L2-L1-x

            if(veryVeryVerbose) printf("Remove all\n");
            Obj::unlink(H);
            Obj::isWellFormed(L3, L1);
            Obj::isWellFormed(L1, L1);
            Obj::isWellFormed(L2, L1);
            Obj::unlink(L3);
            Obj::isWellFormed(L2, L1);
            Obj::isWellFormed(L1, L1);
            Obj::unlink(L2);
            Obj::isWellFormed(L1, L1);
        }
        ASSERTV(dm.isTotalSame());
        ASSERTV(om.isTotalSame());

        if(veryVerbose) printf("Testing 'insertLinkAfterLink'"
                               " and 'unlink'\n");
        {

            Link head1; Link *H1 = &head1;
            head1.reset();
            ASSERTV(Obj::isWellFormed(H1, H1));

            Link link11; Link link12; Link link13;  // link_listIndex_nodeIndex
            Obj::insertLinkAfterTarget(&link11, H1);
            Obj::insertLinkAfterTarget(&link12, &link11);
            Obj::insertLinkAfterTarget(&link13, &link12);
            ASSERTV(Obj::isWellFormed(H1, &link13));

            // x-H1-L11-L12-L13-x

            Link head2; Link *H2 = &head2;
            head2.reset();
            ASSERTV(Obj::isWellFormed(H2, H2));

            Link link21; Link link22; Link link23;  // link_listIndex_nodeIndex
            Obj::insertLinkAfterTarget(&link21, H2);
            Obj::insertLinkAfterTarget(&link22, &link21);
            Obj::insertLinkAfterTarget(&link23, &link22);
            ASSERTV(Obj::isWellFormed(H2, &link23));

            // x-H1-L11-L12-L13-x
            // x-H2-L21-L22-L23-x

            Obj::spliceListBeforeTarget(&link21, &link23, 0);
            ASSERTV(H2->nextLink() == 0);
            ASSERTV(Obj::isWellFormed(H2, H2));
            ASSERTV(Obj::isWellFormed(&link21, &link23));

            // x-H1-L11-L12-L13-x
            // x-H2-x
            // x-L21-L22-L23-x

            // Restore original state

            Obj::insertLinkBeforeTarget(H2, &link21);
            ASSERTV(H2->nextLink() == &link21);
            ASSERTV(Obj::isWellFormed(H2, &link23));

            // x-H1-L11-L12-L13-x
            // x-H2-L21-L22-L23-x

            Obj::spliceListBeforeTarget(&link21, &link22, &link11);
            ASSERTV(H2->nextLink() == &link23);
            ASSERTV(Obj::isWellFormed(H2, &link23));
            ASSERTV(H1->nextLink() == &link21);
            ASSERTV(link22.nextLink() == &link11);
            ASSERTV(Obj::isWellFormed(H1, &link13));

            // x-H1-L21-L22-L11-L12-L13-x
            // x-H2-L23-x

            Obj::spliceListBeforeTarget(&link23, &link23, H1);
            ASSERTV(0 == H2->nextLink());
            ASSERTV(0 == H2->previousLink());
            ASSERTV(link23.nextLink() == H1);
            ASSERTV(link23.previousLink() == 0);
            ASSERTV(H1->nextLink() == &link21);
            ASSERTV(H1->previousLink() == &link23);
            ASSERTV(Obj::isWellFormed(&link23, &link13));
            ASSERTV(Obj::isWellFormed(H2, H2));

            // x-L23-H1-L21-L22-L11-L12-L13-x
            // x-H2-x
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
