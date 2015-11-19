// bdlma_aligningallocator.t.cpp                                      -*-C++-*-

#include <bdlma_aligningallocator.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>

#include <bsl_cstdlib.h>        // atoi
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
// [3] deallocate(void *address);
// [2] AligningAllocator(size_type alignment, Allocator *alloctor);
// [2] allocate(size_type size);
// [1] bdlma_bufferedsequentialallocator -- Test Framework
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Suppose we want to store a linked list of null-terminated strings in
// dynamically allocated memory, and we want to use a buffered sequential
// allocator for allocation.
// First, we define the nodes of the list -- each node containing a pointer to
// the next element, with the string, which may have any length, immediately
// following the pointer.
//..
    struct Node {
        Node *d_next;
        char  d_string[1];
    };
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    enum { k_BUFFER_SIZE = 64 << 10 };    // 64K
    static char buffer[k_BUFFER_SIZE];
    bdlma::BufferedSequentialAllocator bsa(buffer, k_BUFFER_SIZE);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 That the usage example compiles and works, to demonstrate the
        //:   usage of this component.
        //
        // Plan:
        //: 2 Code and test the usage example, then propagate it to the
        //:   include file.
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

// Note that on a 32-bit platform, the pointer must be 4-byte aligned.  We
// could configurre the buffered sequential allocator for max alignment, but
// this would result in an 8-byte alignment, wasting memory.
//
// Instead, we configure the buffered sequential allocator for natural
// alignment (the default), and use an aligning allocator to wrap it, ensuring
// that the memory will be aligned to 'sizeof(Node *)'.
//
// Then, we define our allocators. we configure the buffered sequential
// allocator for natural alignment (the default), and use an aligning allocator
// to wrap it, ensuring that the memory will be aligned to accomodate 'Node'.
//..
    enum { k_ALIGNMENT = bsls::AlignmentFromType<Node>::VALUE };

    static char                        buffer[4 * 1024];
    bdlma::BufferedSequentialAllocator bsa(buffer, k_BUFFER_SIZE);
    bdlma::AligningAllocator           aa(k_ALIGNMENT, &bsa);
//..
// Next, we define some strings we could like to store in the list:
//..
    const char *strings[] = {
        "A zinger is not a rebuttal.\n",
        "Humor is mankind's greatest blessing.\n",
        "As usual, the facts don't care about our feelings.\n",
        "Criticism is the only antidote to error.\n",
        "If you can't annoy somebody, there is little point in writing.\n",
        "Maybe all one can do is hope to end up with the right regrets.\n",
        "People may hear your words, but they feel your attitude.\n",
        "Imagination is a poor substitute for experience.\n",
        "We wanted a labor force, but human beings came.\n",
        "The reward of a thing well done is to have done it.\n",
        "Chance fights ever on the side of the prudent.\n",
        "The best time to make friends is before you need them.\n" };
    enum { k_NUM_STRINGS = sizeof strings / sizeof *strings };
//..
// Now, we store the strings in the list, verifying that they are properly
// aligned.
//..
    Node *head = 0;
    for (int ii = 0; ii < k_NUM_STRINGS; ++ii) {
        const char *str = strings[ii];
        bsl::size_t len = bsl::strlen(str);
        Node *newNode = static_cast<Node *>(aa.allocate(
                                                    sizeof(Node *) + len + 1));
        ASSERT(0 ==
                 (reinterpret_cast<bsl::size_t>(newNode) & (k_ALIGNMENT - 1)));

        bsl::strcpy(newNode->d_string, str);

        newNode->d_next = head;
        head = newNode;
    }
//..
// Finally, we traverse our linked list and print out the strings:
//..
    for (const Node *node = head; node; node = node->d_next) {
if (veryVerbose) {
        cout << node->d_string;
}
    }
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //
        // Concern:
        //: 1 That deallocate properly frees memory.
        //
        // Plan:
        //: 1 Repeat the test framework from the previous cases, except using
        //:   bslma::TestAllocator as the underlying allocator, and free all
        //:   the memory with 'deallocate'.  When the test allocator is
        //:   destroyed, it will abort if any memory wasn't freed.
        //
        // Testing:
        //   deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << "DEALLOCATE TEST\n"
                             "===============\n";

        bslma::TestAllocator ta;

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

        for (bsl::size_t align = 2; align <= k_MAX_ALIGN; align *= 2) {
            bsl::vector<void *> segments(&ta);
            bdlma::AligningAllocator aa(align, &ta);

            for (bsl::size_t size = align / 2; size < 4 * align;
                                                               size += align) {
                for (int ii = 0; ii < 4; ++ii) {
                    segments.push_back(aa.allocate(size));
                }
            }

            for (unsigned int ii = 0; ii < segments.size(); ++ii) {
                aa.deallocate(segments[ii]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIGNING ALLOCATOR ALIGNMENT TEST
        //
        // Concern:
        //: 1 That the aligning allocator enforces alignment.
        //
        // Plan:
        //: 1 Repeat the same framework used in case 1, except using a
        //:   'bdlma::AligingAllocator' around the buffered sequential
        //:   allocatoor, and verify that the allocated memory is appropriately
        //:   aligned.
        //
        // Testing
        //   AligningAllocator(size_type alignment, Allocator *alloctor);
        //   allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << "ALIGNING ALLOCATOR ALIGNMENT TEST\n"
                             "=================================\n";

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

        for (bsl::size_t align = 2; align <= k_MAX_ALIGN; align *= 2) {
            bdlma::AligningAllocator aa(align, &bsa);
            bsl::size_t alignBits = 0;

            for (bsl::size_t size = align / 2; size < 4 * align;
                                                               size += align) {
                for (int ii = 0; ii < 4; ++ii) {
                    void *segment = aa.allocate(size);

                    alignBits |= reinterpret_cast<bsl::size_t>(segment);
                }
            }

            ASSERT(0 == (alignBits & (align - 1)));
            ASSERT(0 == (alignBits & (align / 2)));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // UNDERLYING ALLOCATOR TEST
        //   We need to make sure that 'bdlma::BufferedSequentialAllocator',
        //   which we will use as the underlying allocator, does not
        //   automatically align memory by default (which would render our
        //   later test useless).
        //
        // Concerns:
        //: 1 'bdlma::BufferedSequentialAllocator' is capable of allocating
        //:    poorly aligned memory.
        //
        // Plan:
        //: 1 Construct a 'bdlma::BufferedSequentialAllocator' configured for
        //:   natural alignment and verify that we can make it allocate memory
        //:   that is poorly aligned.
        //
        // Testing:
        //   bdlma_bufferedsequentialallocator -- Test Framework
        // --------------------------------------------------------------------

        if (verbose) cout << "UNDERLYING ALLOCATOR TEST\n"
                             "=========================\n";

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

        for (bsl::size_t align = 2; align <= k_MAX_ALIGN; align *= 2) {
            bsl::size_t alignBits = 0;

            for (bsl::size_t size = align / 2; size < 4 * align;
                                                               size += align) {
                for (int ii = 0; ii < 4; ++ii) {
                    void *segment = bsa.allocate(size);

                    alignBits |= reinterpret_cast<bsl::size_t>(segment);
                }
            }

            ASSERT(alignBits & (align - 1));    // memory wasn't aligned
            ASSERT(alignBits & (align / 2));    // memory wasn't aligned at all
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
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
