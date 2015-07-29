// bslalg_bidirectionallinklistutil.t.cpp                             -*-C++-*-

#include <bslalg_bidirectionallinklistutil.h>

#include <bslalg_bidirectionallink.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

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

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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


typedef BidirectionalLink         Link;
typedef BidirectionalLinkListUtil Obj;

// First, since 'Link' neither has a constructor nor is a POD (which would make
// aggregate initialization possible), we create a function 'makeLink' to
// assemble a link from two pointers:

Link makeLink(Link *prev, Link *next)
{
    Link result;
    result.setPreviousLink(prev);
    result.setNextLink(    next);

    return result;
}

// Then, we create a function that will, passed two links that are endpoints
// of a linked list from the specified 'first' to 'last' though the 'nextLink'
// pointers, count the number of nodes in the list including both endpoints.

int length(Link *first, Link *last)
{
    int result = 0;
    Link *p = first;
    while (p && ++result && last != p) {
        p = p->nextLink();
    }

    return result;
}

struct Bucket {
    // DATA
    Link *d_first;
    Link *d_last;

    // MANIPULATORS
    void clone(const Bucket& bucket, bslma::Allocator *alloc = 0);
        // Make a copy of the specified 'bucket' using the specified allocator
        // 'alloc'.  Note that all links are copied, with new copies being
        // allocated with the specified 'alloc'.

    void destroy(bslma::Allocator *alloc = 0);
        // Destroy the nodes in the list ranging from 'this->d_first' to
        // 'this->d_last'.  Note that if any surving nodes are pointing at
        // either of those nodes, they will be left dangling.

    // ACCESSORS
    int length() const;
        // Returns the number of nodes in the range '[ d_first, d_last ]'.  If
        // the bucket is not well-formed, and return a negative value.  This
        // should work for any bucket that is well-formed.
};

void Bucket::clone(const Bucket& bucket, bslma::Allocator *alloc)
{
    alloc = bslma::Default::allocator(alloc);

    ASSERT(Obj::isWellFormed(bucket.d_first, bucket.d_last));

    this->d_first = 0;
    for (Link *link = bucket.d_first, *prev = 0, *newLink; link;
                                     link = link->nextLink(), prev = newLink) {
        newLink = static_cast<Link *>(alloc->allocate(sizeof(Link)));
        newLink->setPreviousLink(prev);
        if (!prev) {
            this->d_first = newLink;
        }
        else {
            prev->setNextLink(newLink);
        }

        if (bucket.d_last == link) {
            newLink->setNextLink(0);
            this->d_last = newLink;
            return;                                                   // RETURN
        }
    }

    ASSERT(0 == this->d_first);
    this->d_last = 0;
}

void Bucket::destroy(bslma::Allocator *alloc)
{
    alloc = bslma::Default::allocator(alloc);

    ASSERT(Obj::isWellFormed(this->d_first, this->d_last));

    if (!this->d_first) {
        return;                                                       // RETURN
    }

    Link *link = this->d_first, *toDelete;
    do {
        toDelete = link;
        link = link->nextLink();

        alloc->deallocate(toDelete);
    } while (this->d_last != toDelete);

    this->d_first = this->d_last = 0;
}

int Bucket::length() const
{
    if (!Obj::isWellFormed(this->d_first, this->d_last)) {
        return -1;                                                    // RETURN
    }

    return ::length(d_first, d_last);
}

static
Link links[] = {
    makeLink(0,         links + 1),
    makeLink(links + 0, links + 2),
    makeLink(links + 1, links + 3),
    makeLink(links + 2, links + 4),
    makeLink(links + 3, links + 5),
    makeLink(links + 4, links + 6),
    makeLink(links + 5, 0)          };
enum { NUM_LINKS = sizeof links / sizeof *links,
       LAST_LINK = NUM_LINKS - 1 };

const Bucket linksBucket = { &links[0], &links[ NUM_LINKS - 1 ] };

struct DefaultDataRow {
    int    d_line;
    Bucket d_bucket;
    bool   d_isWellFormed;
    int    d_length;
};

Link *DEFAULT_BOGUS = (Link *) 0xBADA55E5;

const DefaultDataRow DEFAULT_DATA[] = {
    { L_, { 0,         0                   }, 1,  0 },
    { L_, { DEFAULT_BOGUS, 0               }, 0, -1 },
    { L_, { 0        , DEFAULT_BOGUS       }, 0, -1 },
    { L_, { &links[0], &links[0]           }, 1,  1 },
    { L_, { &links[0], &links[LAST_LINK]   }, 1,  7 },
    { L_, { &links[0], &links[1]           }, 1,  2 },
    { L_, { &links[1], &links[LAST_LINK-1] }, 1,  5 },
    { L_, { &links[1], &links[0]           }, 0, -1 },
    { L_, { &links[LAST_LINK-1], &links[1] }, 0, -1 },
    { L_, { &links[LAST_LINK], &links[0]   }, 0, -1 } };

enum { NUM_DEFAULT_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    ASSERT(Obj::isWellFormed(linksBucket.d_first, linksBucket.d_last));

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE
        // --------------------------------------------------------------------

        if (verbose) printf("Usage Example\n"
                            "=============\n");

// Next, in our 'main', we declare a 'typedef' for the component name and a
// a contanst 'invalid' garbage pointer we use when we want data to be garbage.

        typedef BidirectionalLinkListUtil Util;
        Link * const invalid = (Link *) 0XBADDEED5;

// Then, we create a linked list of links and use 'isWellFormed' to verify
// that it is well formed, and call the 'length' method we just created to
// verify its length.

        Link usageData[] = {
            makeLink(0,             &usageData[1]),
            makeLink(&usageData[0], &usageData[2]),
            makeLink(&usageData[1], &usageData[3]),
            makeLink(&usageData[2], 0            )  };

        ASSERT(Util::isWellFormed(      &usageData[0], &usageData[3]));
        ASSERT(4 == length(&usageData[0], &usageData[3]));

// Next, we create two new links 'front' and 'back', and initialize them with
// garbage:

        Link front = makeLink(invalid, invalid);
        Link back  = makeLink(invalid, invalid);

// Then, we use our component's 'insertLinkBeforeTarget' and
// 'insertLinkAfterTarget' to concatenate 'front' to the front of the list and
// 'back' to its rear:

        Util::insertLinkBeforeTarget(&front, &usageData[0]);
        Util::insertLinkAfterTarget( &back,  &usageData[3]);

// Next, We examine the new list and verify we now have a well-formed list, 2
// longer than the old list:

        ASSERT(0 == front.previousLink());
        ASSERT(0 == back .nextLink());
        ASSERT(Util::isWellFormed(          &front, &back));
        ASSERT(6 == length(&front, &back));

// Then, we use our component's 'unlink' method to remove two nodes from our
// list.  Note that the state of the removed nodes is undefined:

        Util::unlink(&usageData[1]);
        Util::unlink(&usageData[3]);

// Next, we verify that the new list is well formed and 2 elements shorter than
// it was before we removed those two nodes:

        ASSERT(Util::isWellFormed(&front, &back));
        ASSERT(4 == length(&front, &back));

// Then, we weave the two discarded nodes into a new, second list of two nodes,
// and use 'isWellFormed' and 'length' to verify it is as we expect:

        usageData[1] = makeLink(0, &usageData[3]);
        usageData[3] = makeLink(&usageData[1], 0);
        ASSERT(Util::isWellFormed(&usageData[1], &usageData[3]));
        ASSERT(2 ==        length(&usageData[1], &usageData[3]));

// Next, we use our component's 'spliceListBeforeTarget' method to remove the
// middle nodes from the longer list and append them to the end of shorter
// list.  Note that the splicing function not only adds the sequence to the new
// list, it also splices the list the sequence is removed from so that both are
// well-formed lists:

        Util::spliceListBeforeTarget(&usageData[0],
                                     &usageData[2],
                                     &usageData[3]);

// Then, we use 'isWellFormed' and 'length' to verify the state of our two
// lists:

        ASSERT(Util::isWellFormed(&usageData[1], &usageData[3]));
        ASSERT(4 ==        length(&usageData[1], &usageData[3]));

        ASSERT(Util::isWellFormed(&front, &back));
        ASSERT(2 ==        length(&front, &back));

// Next, we call 'spliceListBeforeTarget' again to join our two lists into one:

        Util::spliceListBeforeTarget(&usageData[1],
                                     &usageData[3],
                                     &back);

// Now, we use 'isWellFormed' and 'length' to verify the state of our one
// remaining list:

        ASSERT(Util::isWellFormed(&front, &back));
        ASSERT(6 ==        length(&front, &back));
        ASSERT(0 == front.previousLink());
        ASSERT(0 == back .nextLink());

// Finally, we traverse our list in both directions, examining each node to
// verify all the links and that the sequence is as expected:

        Link *p = &front;
        ASSERT(0 == p->previousLink());
        p = p->nextLink();
        ASSERT(&usageData[1] == p);
        p = p->nextLink();
        ASSERT(&usageData[0] == p);
        p = p->nextLink();
        ASSERT(&usageData[2] == p);
        p = p->nextLink();
        ASSERT(&usageData[3] == p);
        p = p->nextLink();
        ASSERT(&back         == p);
        ASSERT(0 == p->nextLink());

        p = p->previousLink();
        ASSERT(&usageData[3] == p);
        p = p->previousLink();
        ASSERT(&usageData[2] == p);
        p = p->previousLink();
        ASSERT(&usageData[0] == p);
        p = p->previousLink();
        ASSERT(&usageData[1] == p);
        p = p->previousLink();
        ASSERT(&front        == p);
        ASSERT(0 == p->previousLink());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'spliceListBeforeTarget'
        //
        // Concerns:
        //: That the 'spliceListBeforeTarget' works correctly
        //
        // Plan:
        // Test all combinations of the following:
        //: 1 Target is:
        //:   o Zero.
        //:   o The beginning of a list.
        //:   o The last node of a list.
        //: 2 Inserted is (A: a single node, B: a list of nodes)
        //:   o By itself.
        //:   o At the beginning of a list.
        //:   o At the end of a list.
        //:   o In the middle of a list.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING spliceListBeforeTarget\n"
                            "==============================\n");

        if (verbose) printf("insert single link, target == 0\n");
        {
            Link   toInsertLink = makeLink(0, 0);
            Obj::spliceListBeforeTarget(&toInsertLink,
                                        &toInsertLink,
                                        0);
            ASSERT(0 == toInsertLink.previousLink());
            ASSERT(0 == toInsertLink.nextLink());
            ASSERT(Obj::isWellFormed(&toInsertLink, &toInsertLink));
        }

        if (verbose) printf("insert full list, target == 0\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);

            Obj::spliceListBeforeTarget(toInsert.d_first,
                                        toInsert.d_last,
                                        0);
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));

            toInsert.destroy(&ta);
        }

        if (verbose) printf("insert single node from start of list,"
                            " target == 0\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link   *first  = toInsert.d_first;
            Link   *second = toInsert.d_first->nextLink();

            Obj::spliceListBeforeTarget(first,
                                        first,
                                        0);
            ASSERT(0 == first->previousLink());
            ASSERT(0 == first->nextLink());
            ASSERT(0 == second->previousLink());
            ASSERT(Obj::isWellFormed(first, first));
            ASSERT(1 == length(first, first));

            toInsert.d_first = second;
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            Obj::spliceListBeforeTarget(first,
                                        first,
                                        toInsert.d_first);
            toInsert.d_first = first;
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            toInsert.destroy(&ta);
        }

        if (verbose) printf("insert single node from end of list,"
                            " target == 0\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *last = toInsert.d_last;
            Link *prev = toInsert.d_last->previousLink();

            Obj::spliceListBeforeTarget(last, last, 0);
            ASSERT(0 == last->previousLink());
            ASSERT(0 == last->nextLink());
            ASSERT(0 == prev->nextLink());
            ASSERT(Obj::isWellFormed(last, last));
            ASSERT(1 == length(last, last));

            toInsert.d_last = prev;
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            Obj::spliceListBeforeTarget(last,
                                        last,
                                        toInsert.d_first);
            toInsert.d_first = last;
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            toInsert.destroy(&ta);
        }

        if (verbose) printf("insert single node from middle of list,"
                            " target == 0\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *middle = toInsert.d_first->nextLink()->nextLink();
            Link *prev   = middle->previousLink();
            Link *next   = middle->nextLink();

            Obj::spliceListBeforeTarget(middle, middle, 0);
            ASSERT(0 == middle->previousLink());
            ASSERT(0 == middle->nextLink());
            ASSERT(next == prev->nextLink());
            ASSERT(prev == next->previousLink());
            ASSERT(Obj::isWellFormed(middle, middle));
            ASSERT(1 == length(middle, middle));

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            Obj::spliceListBeforeTarget(middle,
                                        middle,
                                        next);
            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            toInsert.destroy(&ta);
        }

        if (verbose) printf("insert single link, target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);

            Link   toInsertLink = makeLink(0, 0);
            Obj::spliceListBeforeTarget(&toInsertLink,
                                        &toInsertLink,
                                        dest.d_first);
            ASSERT(0 == toInsertLink.previousLink());
            ASSERT(dest.d_first == toInsertLink.nextLink());
            ASSERT(Obj::isWellFormed(&toInsertLink, dest.d_last));
            ASSERT(NUM_LINKS + 1 == length(&toInsertLink, dest.d_last));

            dest.destroy(&ta);
        }

        if (verbose) printf("insert full list, target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);

            Obj::spliceListBeforeTarget(toInsert.d_first,
                                        toInsert.d_last,
                                        dest.d_first);
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(dest.d_first    == toInsert.d_last->nextLink());
            ASSERT(toInsert.d_last == dest.d_first->previousLink());
            ASSERT(0 == dest.d_last ->nextLink());
            ASSERT(Obj::isWellFormed(toInsert.d_first, dest.d_last));
            ASSERT(2 * NUM_LINKS == length(toInsert.d_first, dest.d_last));

            toInsert.d_last = dest.d_last;

            toInsert.destroy(&ta);
        }

        if (verbose) printf("insert single node from start of list,"
                            " target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link   *first  = toInsert.d_first;
            Link   *second = toInsert.d_first->nextLink();

            Obj::spliceListBeforeTarget(first,
                                        first,
                                        dest.d_first);
            ASSERT(0 == first->previousLink());
            ASSERT(dest.d_first == first->nextLink());
            ASSERT(first        == dest.d_first->previousLink());
            ASSERT(0 == second->previousLink());

            toInsert.d_first = second;
            dest.    d_first = first;

            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());
            ASSERT(0 == dest.d_first->previousLink());
            ASSERT(0 == dest.d_last ->nextLink());

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());

            toInsert.destroy(&ta);
            dest.    destroy(&ta);
        }

        if (verbose) printf("insert single node from end of list,"
                            " target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *last = toInsert.d_last;
            Link *prev = toInsert.d_last->previousLink();

            Obj::spliceListBeforeTarget(last, last, dest.d_first);
            ASSERT(0 == last->previousLink());
            ASSERT(0 == prev->nextLink());

            ASSERT(last == dest.d_first->previousLink());
            ASSERT(dest.d_first == last->nextLink());

            toInsert.d_last  = prev;
            dest.    d_first = last;

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());

            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());
            ASSERT(0 == dest    .d_first->previousLink());
            ASSERT(0 == dest    .d_last ->nextLink());

            toInsert.destroy(&ta);
            dest    .destroy(&ta);
        }

        if (verbose) printf("insert single node from middle of list,"
                            " target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *middle = toInsert.d_first->nextLink()->nextLink();
            Link *prev   = middle->previousLink();
            Link *next   = middle->nextLink();

            Obj::spliceListBeforeTarget(middle, middle, dest.d_first);
            ASSERT(0 == middle->previousLink());
            ASSERT(dest.d_first == middle->nextLink());
            ASSERT(next == prev->nextLink());
            ASSERT(prev == next->previousLink());
            ASSERT(Obj::isWellFormed(middle, dest.d_last));
            ASSERT(1 == length(middle, middle));

            dest.d_first = middle;

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());
            ASSERT(0 == dest    .d_first->previousLink());
            ASSERT(0 == dest    .d_last ->nextLink());

            toInsert.destroy(&ta);
            dest    .destroy(&ta);
        }

        if (verbose) printf("insert single link, target == end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);
            Link *destPrev = dest.d_last->previousLink();

            Link   toInsertLink = makeLink(0, 0);
            Obj::spliceListBeforeTarget(&toInsertLink,
                                        &toInsertLink,
                                        dest.d_last);
            ASSERT(destPrev      == toInsertLink.previousLink());
            ASSERT(&toInsertLink == destPrev->nextLink());
            ASSERT(dest.d_last   == toInsertLink.nextLink());
            ASSERT(&toInsertLink == dest.d_last->previousLink());
            ASSERT(Obj::isWellFormed(dest.d_first, &toInsertLink));
            ASSERT(NUM_LINKS + 1 == dest.length());

            Obj::unlink(&toInsertLink);

            dest.destroy(&ta);
        }

        if (verbose) printf("insert full list, target == end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);
            Link *destPrev = dest.d_last->previousLink();

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);

            Obj::spliceListBeforeTarget(toInsert.d_first,
                                        toInsert.d_last,
                                        dest.d_last);
            ASSERT(destPrev         == toInsert.d_first->previousLink());
            ASSERT(dest.d_last      == toInsert.d_last->nextLink());
            ASSERT(toInsert.d_last  == dest.d_last->previousLink());
            ASSERT(toInsert.d_first == destPrev->nextLink());
            ASSERT(0 == dest.d_last ->nextLink());
            ASSERT(0 == dest.d_first->previousLink());

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(    NUM_LINKS == toInsert.length());

            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(2 * NUM_LINKS == dest    .length());

            dest.destroy(&ta);
        }

        if (verbose) printf("insert single node from start of list,"
                            " target == end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);
            Link *destPrev = dest.d_last->previousLink();

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link   *first  = toInsert.d_first;
            Link   *second = toInsert.d_first->nextLink();

            Obj::spliceListBeforeTarget(first,
                                        first,
                                        dest.d_last);
            ASSERT(destPrev == first->previousLink());
            ASSERT(dest.d_last == first->nextLink());
            ASSERT(first       == dest.d_last->previousLink());
            ASSERT(first       == destPrev->nextLink());
            ASSERT(0 == second->previousLink());

            toInsert.d_first = second;

            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());
            ASSERT(0 == dest    .d_first->previousLink());
            ASSERT(0 == dest    .d_last ->nextLink());

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());

            toInsert.destroy(&ta);
            dest.    destroy(&ta);
        }

        if (verbose) printf("insert single node from end of list,"
                            " target == end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);
            Link *destPrev = dest.d_last->previousLink();

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *last = toInsert.d_last;
            Link *prev = toInsert.d_last->previousLink();

            Obj::spliceListBeforeTarget(last, last, dest.d_last);
            ASSERT(destPrev == last->previousLink());
            ASSERT(0 == prev->nextLink());
            ASSERT(dest.d_last == last->nextLink());
            ASSERT(last == dest.d_last->previousLink());

            toInsert.d_last  = prev;

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());

            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());
            ASSERT(0 == dest    .d_first->previousLink());
            ASSERT(0 == dest    .d_last ->nextLink());

            toInsert.destroy(&ta);
            dest    .destroy(&ta);
        }

        if (verbose) printf("insert single node from middle of list,"
                            " target == front of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket dest;
            dest.clone(linksBucket, &ta);
            Link *destPrev = dest.d_last->previousLink();

            Bucket toInsert;
            toInsert.clone(linksBucket, &ta);
            Link *middle = toInsert.d_first->nextLink()->nextLink();
            Link *prev   = middle->previousLink();
            Link *next   = middle->nextLink();

            Obj::spliceListBeforeTarget(middle, middle, dest.d_last);
            ASSERT(destPrev    == middle->previousLink());
            ASSERT(dest.d_last == middle->nextLink());
            ASSERT(next == prev->nextLink());
            ASSERT(prev == next->previousLink());

            ASSERT(Obj::isWellFormed(toInsert.d_first, toInsert.d_last));
            ASSERT(NUM_LINKS - 1 == toInsert.length());
            ASSERT(0 == toInsert.d_first->previousLink());
            ASSERT(0 == toInsert.d_last ->nextLink());

            ASSERT(Obj::isWellFormed(dest    .d_first, dest    .d_last));
            ASSERT(NUM_LINKS + 1 == dest    .length());
            ASSERT(0 == dest    .d_first->previousLink());
            ASSERT(0 == dest    .d_last ->nextLink());

            toInsert.destroy(&ta);
            dest    .destroy(&ta);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'insertLinkAfterTarget'
        //
        // Concerns:
        //: 1 That the function works correctly when 'target' is at the
        //:   end of the list.
        //: 2 That the function works correctly when 'target' is the only
        //:   element in a list.
        //: 3 That the function works correctly when 'target' is in the middle
        //:   of a list.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'insertLinkAfterTarget'\n"
                            "================================\n");

        Link bogusLink;
        bogusLink.setPreviousLink(DEFAULT_BOGUS);
        bogusLink.setNextLink(    DEFAULT_BOGUS);

        if (verbose) printf("at the end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link newLink = bogusLink;

            Obj::insertLinkAfterTarget(&newLink, bucket.d_last);
            ASSERTV(0             == newLink.nextLink());
            ASSERTV(bucket.d_last == newLink.previousLink());

            ASSERTV(Obj::isWellFormed(bucket.d_first, &newLink));
            ASSERTV(NUM_LINKS == bucket.length());

            bucket.destroy(&ta);
        }

        if (verbose) printf("where there's only one element in the list\n");
        {
            Link oldLink = makeLink(0, 0);
            Link newLink = bogusLink;

            Obj::insertLinkAfterTarget(&newLink, &oldLink);
            ASSERTV(0        == newLink.nextLink());
            ASSERTV(&oldLink == newLink.previousLink());

            ASSERTV(0        == oldLink.previousLink());
            ASSERTV(Obj::isWellFormed(&oldLink, &newLink));
        }

        if (verbose) printf("in the middle of the list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link *at = bucket.d_first->nextLink()->nextLink();

            Link newLink = bogusLink;

            Obj::insertLinkAfterTarget(&newLink, at);
            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS + 1 == bucket.length());

            ASSERTV(at == newLink.previousLink());

            Obj::unlink(&newLink);
            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            bucket.destroy(&ta);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Testing 'insertLinkBeforeTarget'
        //
        // Concerns:
        //: 1 That the function works correctly when '0 == target'.
        //: 2 That the function works correctly when 'target' is at the
        //:   beginning of the list.
        //: 3 That the function works correctly when 'target' is the only
        //:   element in a list.
        //: 4 That the function works correctly when 'target' is in the middle
        //:   of a list.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'insertLinkBeforeTarget'\n"
                            "================================\n");

        Link bogusLink;
        bogusLink.setPreviousLink(DEFAULT_BOGUS);
        bogusLink.setNextLink(    DEFAULT_BOGUS);

        if (verbose) printf("target == 0\n");
        {
            Link newLink = bogusLink;

            Obj::insertLinkBeforeTarget(&newLink, 0);
            ASSERT(0 == newLink.nextLink());
            ASSERT(0 == newLink.previousLink());
        }

        if (verbose) printf("at the beginning of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link newLink = bogusLink;

            Obj::insertLinkBeforeTarget(&newLink, bucket.d_first);
            ASSERTV(0              == newLink.previousLink());
            ASSERTV(bucket.d_first == newLink.nextLink());

            ASSERTV(Obj::isWellFormed(&newLink, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            bucket.destroy(&ta);
        }

        if (verbose) printf("where there's only one element in the list\n");
        {
            Link oldLink = makeLink(0, 0);
            Link newLink = bogusLink;

            Obj::insertLinkBeforeTarget(&newLink, &oldLink);
            ASSERTV(0        == newLink.previousLink());
            ASSERTV(&oldLink == newLink.nextLink());

            ASSERTV(0        == oldLink.nextLink());
            ASSERTV(Obj::isWellFormed(&newLink, &oldLink));
        }

        if (verbose) printf("in the middle of the list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link *at = bucket.d_first->nextLink()->nextLink();

            Link newLink = bogusLink;

            Obj::insertLinkBeforeTarget(&newLink, at);
            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS + 1 == bucket.length());

            ASSERTV(at == newLink.nextLink());

            Obj::unlink(&newLink);
            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            bucket.destroy(&ta);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'unlink'
        //
        // Concerns:
        //: 1 'unlink' successfully removes a node in the middle of a list.
        //: 2 'unlink' successfully removes a node at either end of the list.
        //
        // Plan:
        //: Clone 'linksBucket' and operate on cloned strings, being sure to
        //: explicitly delete the 'unlinked' nodes.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'unlink'\n"
                            "================\n");

        if (verbose) printf("unlink in middle of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link *toDelete = bucket.d_first->nextLink()->nextLink();

            Obj::unlink(toDelete);
            memset(toDelete, 'x', sizeof(Link));

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS - 1 == bucket.length());

            ta.deallocate(toDelete);
            bucket.destroy(&ta);
        }

        if (verbose) printf("unlink at start of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link *toDelete = bucket.d_first;
            bucket.d_first = toDelete->nextLink();

            Obj::unlink(toDelete);
            memset(toDelete, 'x', sizeof(Link));

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS - 1 == bucket.length());

            ta.deallocate(toDelete);
            bucket.destroy(&ta);
        }

        if (verbose) printf("unlink at end of list\n");
        {
            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(linksBucket, &ta);

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS == bucket.length());

            Link *toDelete = bucket.d_last;
            bucket.d_last = toDelete->previousLink();

            Obj::unlink(toDelete);
            memset(toDelete, 'x', sizeof(Link));

            ASSERTV(Obj::isWellFormed(bucket.d_first, bucket.d_last));
            ASSERTV(NUM_LINKS - 1 == bucket.length());

            ta.deallocate(toDelete);
            bucket.destroy(&ta);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING Bucket: 'clone', 'destroy', 'length'
        //
        // Concern:
        //: 1 That 'clone' accuratel copies a bucket, and 'destroy'
        //:   effectively destroys it.
        //: 2 That 'length' accurately measures the length of a bucket.
        //
        // Plan:
        //: 1 Call 'length' on each bucket in the test data and verify we are
        //:   getting the proper results.
        //: 2 Clone a copy of the bucket using a test allocator, verify that
        //:   'isWellFormed' and 'length' return the same values as they
        //:   returned for the original bucket.
        //: 3 Verify the ends of the list in the cloned bucket are 0.
        //: 4 Call 'destroy'.  The test allocator will inform us of any
        //:   redundant frees or leaked memory.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING isWellFormed\n"
                            "====================\n");

        enum { NUM_DATA                        = NUM_DEFAULT_DATA };
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE         = DATA[i].d_line;
            const Bucket& BUCKET   = DATA[i].d_bucket;
            Link *FIRST            = BUCKET.d_first;
            Link *LAST             = BUCKET.d_last;
            const bool WELL_FORMED = DATA[i].d_isWellFormed;
            const int  LENGTH      = DATA[i].d_length;

            LOOP_ASSERT(LINE, WELL_FORMED == Obj::isWellFormed(FIRST, LAST));
            LOOP_ASSERT(LINE, LENGTH      == BUCKET.length());

            if (!WELL_FORMED) {
                continue;
            }

            bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

            Bucket bucket;
            bucket.clone(BUCKET, &ta);

            ASSERTV(LINE, !!LENGTH == (0 != ta.numBlocksInUse()));
            ASSERTV(LINE, WELL_FORMED == Obj::isWellFormed(bucket.d_first,
                                                           bucket.d_last));
            ASSERTV(LINE, LENGTH      == bucket.length());

            if (LENGTH > 0) {
                ASSERTV(LINE, 0 == bucket.d_first->previousLink());
                ASSERTV(LINE, 0 == bucket.d_last->nextLink());
            }
            else {
                ASSERTV(LINE, 0 == bucket.d_first && 0 == bucket.d_last);
            }

            bucket.destroy(&ta);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isWellFormed'
        //
        // Concern:
        //: 1 'isWellFormed' correctly assesses the state of ranges of links.
        //
        // Plan:
        //: 1 Feed all combinations of ranges of links to 'isWellFormed
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING isWellFormed\n"
                            "====================\n");

        enum { NUM_DATA                        = NUM_DEFAULT_DATA };
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE         = DATA[i].d_line;
            const Bucket& BUCKET   = DATA[i].d_bucket;
            Link *FIRST            = BUCKET.d_first;
            Link *LAST             = BUCKET.d_last;
            const bool WELL_FORMED = DATA[i].d_isWellFormed;

            ASSERTV(LINE, WELL_FORMED == Obj::isWellFormed(FIRST, LAST));
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

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
