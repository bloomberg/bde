// bdlma_aligningallocator.t.cpp                                      -*-C++-*-

#include <bdlma_aligningallocator.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>        // atoi
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// 'bdlma::AligningAllocator' is a wrapper to place around another allocator
// that, if the wrapped allocator guarantees natural or maximum aligning, will
// guarantee a minimum alignment specified at construction.  The primary
// concern is that alignment is guaranteed.  To test this, we needed an
// underlying allocator that provided natural alignment, for which we chose the
// buffered sequential allocator.  In the first test case, we simply verified
// that the buffered sequential allocator provided no better than natural
// alignment, so we could re-use that framework in test case 2 and thus verify
// that our new component was achieving better alignment of that in spite of of
// the underlying allocator not providing it.  In test case 3, we verified that
// the 'deallocate' function was indeed freeing memory.
//-----------------------------------------------------------------------------
// [4] deallocate(void *address);
// [3] AligningAllocator(size_type alignment, Allocator *allocator);
// [3] allocate(bsls::Types::size_type size);
// [2] Test machinery
// [1] bdlma_bufferedsequentialallocator -- Test Framework
//-----------------------------------------------------------------------------
// [5] USAGE EXAMPLE
//=============================================================================

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
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

                        // ============================
                        // class AlignmentTestAllocator
                        // ============================

class AlignmentTestAllocator : public bslma::Allocator {
    // This 'class' provides a mechanism to wrap another allocator and return,
    // in sequence, blocks with alignments that cover useful sample alignment
    // offsets.

    // DATA
    bsls::Types::size_type  d_alignment;        // alignment passed at
                                                // construction

    bslma::Allocator       *d_allocator_p;      // allocator passed at
                                                // construction

    int                     d_counter;          // counter to track how to
                                                // misallocate next allocation

    int                     d_samples;          // number of boundary values
                                                // to sample if available
  public:
    // CREATORS
     AlignmentTestAllocator(bsls::Types::size_type  alignment,
                            bslma::Allocator       *allocator);
        // Create an "AlignmentTestAllocator' with the specified 'alignment'
        // and 'allocator'.

    // MANIPULATORS
    void *allocate(bsls::Types::size_type size) BSLS_KEYWORD_OVERRIDE;

    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS
    int numSamples();
};

AlignmentTestAllocator::AlignmentTestAllocator(
    bsls::Types::size_type  alignment,
    bslma::Allocator       *allocator)
: d_alignment(alignment)
, d_allocator_p(allocator)
, d_counter(0)
, d_samples(8)
{
}

void *AlignmentTestAllocator::allocate(bsls::Types::size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    if (d_alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT) {
        size = (size + d_alignment) & ~(d_alignment-1);
        return d_allocator_p->allocate(size);                         // RETURN
    }

    bsls::Types::size_type allocSize = size + 3 * d_alignment;

    void *underlying = d_allocator_p->allocate(allocSize);

    bsls::Types::size_type offset =
        bsls::AlignmentUtil::calculateAlignmentOffset(
            underlying, static_cast<int>(d_alignment)) + d_alignment;

    int                    adjustmentCounter = ((d_counter++) % d_samples)
                                                               - (d_samples/2);
    bsls::Types::size_type adjustment        =
        (adjustmentCounter * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT)
                                                                 % d_alignment;
    offset += adjustment;

    BSLS_ASSERT( offset + size <= allocSize);
    BSLS_ASSERT( offset >= sizeof(void*));

    void *ret = reinterpret_cast<void *>(
        reinterpret_cast<unsigned char *>(underlying) + offset);

    void **header = reinterpret_cast<void **>(
        reinterpret_cast<unsigned char *>(ret) - sizeof(void*));
    *header = underlying;
    return ret;
}

void AlignmentTestAllocator::deallocate(void *address)
{
    if (d_alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT) {
        d_allocator_p->deallocate(address);
    }
    else {
        if (address != 0) {
            void **header = reinterpret_cast<void**>(
                reinterpret_cast<unsigned char *>(address) - sizeof(void*));
            d_allocator_p->deallocate(*header);
        }
    }
}

int AlignmentTestAllocator::numSamples()
{
    return d_samples;
}


//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::AligningAllocator'
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we are dealing with an externally supplied library function that
// creates a linked list of null-terminated strings, and we want to use a
// default-constructed buffered sequential allocator for memory allocation.
//
// First, the externally supplied library defines the 'struct' describing a
// node in the linked list:
//..
struct Node {
    // This 'struct' describes one node in a linked list containing
    // strings.

    Node *d_next;
    char  d_string[1];

    // CLASS METHODS
    static
    bsl::size_t sizeNeededForString(const char *string)
        // Return the size in bytes needed to store a 'Node' containing a
        // copy of the specified 'string'.
    {
        Node node;

        return sizeof(node.d_next) + bsl::strlen(string) + 1;
    }
};
//..
// Then, the externally-supplied library defines the function that will create
// the linked list of nodes from a null-terminated array of pointers to
// C-strings:
//..
void externalPopulateStringList(Node             **head,
                                const char       **stringArray,
                                bslma::Allocator  *allocator)
    // Create a linked list of strings beginning with the specified '*head'
    // containing the null-terminated strings from the null-terminated
    // 'stringArray'.  Use the specified 'allocator' to supply memory.
{
    *head = 0;
    const char *string;
    for (int ii = 0; 0 != (string = stringArray[ii]); ++ii) {
        Node *newNode = static_cast<Node *>(allocator->allocate(
                                   Node::sizeNeededForString(string)));
        ASSERT(newNode != 0);

        bsl::strcpy(newNode->d_string, string);

        newNode->d_next = *head;
        *head = newNode;
    }
}
//..
// Next, our example function will begin with the externally-supplied buffered
// sequential allocator that we are to use:
//..
void printFromNodes(bsl::ostream *out) {
    enum { k_BUFFER_SIZE = 4 * 1024 };
    char                               buffer4k[k_BUFFER_SIZE];
    bdlma::BufferedSequentialAllocator bsAlloc(buffer4k, k_BUFFER_SIZE);
//..
// There is a problem here, in that the nodes must be aligned by
// 'sizeof(Node *)', but our buffered sequential allocator, like most BDE
// allocators, has a "natural alignment" strategy (see {'bsls_alignment'}),
// meaning that it infers the required alignment from the size of the
// allocation requested.  This would normally give us properly aligned memory
// if we were allocating by 'sizeof(Node)', but our 'Node' objects are variable
// length, which will mislead the allocator to sometimes align the new segments
// by less than 'sizeof(Node *)'.
//
// Then, we solve this problem by using an aligning allocator to wrap the
// buffered sequential allocator, ensuring that the memory will still come from
// the buffered sequential allocator, but nonetheless be aligned to the
// alignment requirement of 'Node'.
//..
    enum { k_ALIGNMENT = bsls::AlignmentFromType<Node>::VALUE };
    bdlma::AligningAllocator aligningAllocator(k_ALIGNMENT, &bsAlloc);
//..
// Next, we define a null-terminated array of strings we would like to store in
// the list:
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
        "The best time to make friends is before you need them.\n",
        0  };
//..
// Now, we call the function to put the strings into a linked list, passing it
// the aligning allocator:
//..
    Node *head = 0;
    externalPopulateStringList(&head, strings, &aligningAllocator);
//..
// Finally, we traverse the list and print out the strings, verifying that the
// nodes are properly aligned:
//..
    for (const Node *node = head; node; node = node->d_next) {
        ASSERT(0 == (reinterpret_cast<bsl::size_t>(node) &
                     (k_ALIGNMENT - 1)));
        if (out) {
            *out << node->d_string;
        }
    }
}
//..

}  // close namespace USAGE_EXAMPLE



//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    enum { k_BUFFER_SIZE = 64 << 10 };    // 64K

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example compiles and works, to demonstrate the
        //:   usage of this component.
        //
        // Plan:
        //: 1 Code and test the usage example, then propagate it to the include
        //:   file.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        USAGE_EXAMPLE::printFromNodes(veryVerbose ? &bsl::cout : 0);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //
        // Concerns:
        //: 1 That deallocate properly frees memory.
        //:
        //: 2 That the memory allocated is of at least the requested size.
        //
        // Plan:
        //: 1 Repeat the test framework from the previous cases, except using
        //:   bslma::TestAllocator as the underlying allocator, writing data to
        //:   the entirety of the returned buffer, and free all the memory with
        //:   'deallocate'.  When the test allocator is destroyed, it will
        //:   abort if any memory wasn't freed, or if there was an overrun of
        //:   the buffers it had allocated.
        //
        // Testing:
        //   deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << "DEALLOCATE TEST\n"
                             "===============\n";

        bslma::TestAllocator ta;

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT << 8 };

        for (bsl::size_t align = 1; align <= k_MAX_ALIGN; align *= 2) {
            AlignmentTestAllocator   ata(align, &ta);
            bsl::vector<void *>      segments(&ta);
            bdlma::AligningAllocator aa(align, &ata);

            for (bsl::size_t size = 0; size < 2 * align; ++size) {
                for (int ii = 0; ii < ata.numSamples(); ++ii) {
                    void * segment = aa.allocate(size);

                    ASSERT(0 == size || segment);

                    // write to entire buffer (allowing 'ta' to hopefully catch
                    // if we did not actually get sufficient memory allocated).
                    bsl::memset(segment, 17, size);

                    segments.push_back(segment);
                }
            }

            for (unsigned int ii = 0; ii < segments.size(); ++ii) {
                aa.deallocate(segments[ii]);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALIGNING ALLOCATOR ALIGNMENT TEST
        //
        // Concerns:
        //: 1 That the aligning allocator enforces alignment.
        //
        // Plan:
        //: 1 Repeat the same framework used in case 1, except using a
        //:   'bdlma::AligningAllocator' around the buffered sequential
        //:   allocator, and verify that the allocated memory is appropriately
        //:   aligned.
        //
        // Testing:
        //   AligningAllocator(size_type alignment, Allocator *allocator);
        //   allocate(bsls::Types::size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << "ALIGNING ALLOCATOR ALIGNMENT TEST\n"
                             "=================================\n";

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT << 8 };

        static char                        buffer[k_BUFFER_SIZE];
        bdlma::BufferedSequentialAllocator bsa(buffer, k_BUFFER_SIZE);

        for (bsl::size_t align = 1; align <= k_MAX_ALIGN; align *= 2) {
            bdlma::AligningAllocator aa(align, &bsa);
            bsl::size_t              alignBits = 0;

            for (bsl::size_t size = 0; size < 2 * align; ++size) {
                for (int ii = 0; ii < 4; ++ii) {
                    void *segment = aa.allocate(size);

                    ASSERT(0 == size || segment);
                    alignBits |= reinterpret_cast<bsl::size_t>(segment);

                    bsls::Types::size_type offset =
                        bsls::AlignmentUtil::calculateAlignmentOffset(
                                             segment, static_cast<int>(align));
                    ASSERT(0 == offset);

                    aa.deallocate(segment);
                }
            }

            ASSERT(0 == (alignBits & (align - 1)));
            ASSERT(0 == (alignBits & (align / 2)));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //   We need to verify that our test allocator gives us varying
        //   misalignments.
        //
        // Concerns:
        //: 1 'AlignmentTestAllocator' needs to return allocations with the
        //:   full range of possible misalignments.
        //
        // Plan:
        //: 1 Track the alignments returned and validate they they are all
        //:   covered.
        //
        // Testing:
        //   AlignmentTestAllocator
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST MACHINERY\n"
                             "==============\n";

        bslma::TestAllocator ta;

        enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT << 8 };

        for (bsl::size_t align = 1; align <= k_MAX_ALIGN; align *= 2) {
            AlignmentTestAllocator   ata(align, &ta);

            bsl::size_t minAlign = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
            if (minAlign > align) {
                minAlign = align;
            }

            ASSERT(0 == (align % minAlign));

            bsl::unordered_set<bsls::Types::size_type> seenAlignments;

            for (int ii = 0; ii < ata.numSamples(); ++ii) {
                void * segment = ata.allocate(1);
                ASSERT(0 != segment);

                bsls::Types::size_type offset =
                    bsls::AlignmentUtil::calculateAlignmentOffset(
                        segment,
                        static_cast<int>(align));

                seenAlignments.insert(offset);

                // write to the segment to attempt to catch if this isn't
                // really safely allocated space
                bsl::memset(segment, 17, 1);

                ata.deallocate(segment);
            }

            bsl::size_t maxNumAlignments = align / minAlign;
            if (maxNumAlignments <=
                                  static_cast<bsl::size_t>(ata.numSamples())) {
                ASSERT(seenAlignments.size() == maxNumAlignments);
            }
            else {
                ASSERT(seenAlignments.size() >=
                                   static_cast<bsl::size_t>(ata.numSamples()));
                ASSERT(seenAlignments.size() >= 8);
            }

            if (veryVerbose) {
                bsl::cout << "\tAlignment: " << align
                          << " produced offsets:" << seenAlignments.size()
                          << "\n";
            }
            if (veryVeryVerbose) {
                for (bsl::unordered_set<bsls::Types::size_type>::const_iterator
                         it = seenAlignments.begin(),
                         itend = seenAlignments.end();
                     it != itend;
                     ++it) {
                    bsl::cout << "\t\t" << *it << "\n";
                }
            }

            ASSERT(seenAlignments.find(0) != seenAlignments.end());
            ASSERT(seenAlignments.find(
                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT % align)
                                                      != seenAlignments.end());
            ASSERT(seenAlignments.find(
                       -bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT % align)
                                                      != seenAlignments.end());

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

        static char                        buffer[k_BUFFER_SIZE];
        bdlma::BufferedSequentialAllocator bsa(buffer, k_BUFFER_SIZE);

        for (bsl::size_t align = 2; align <= k_MAX_ALIGN; align *= 2) {
            bsl::size_t alignBits = 0;

            for (bsl::size_t size = 0; size < 2 * align; ++size) {
                for (int ii = 0; ii < 4; ++ii) {
                    void *segment = bsa.allocate(size);
                    ASSERT(0 == size || segment);

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
// Copyright 2016 Bloomberg Finance L.P.
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
