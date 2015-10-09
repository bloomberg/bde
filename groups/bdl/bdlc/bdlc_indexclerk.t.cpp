// bdlc_indexclerk.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlc_indexclerk.h>

#include <bslim_testutil.h>

#include <bslx_outstreamfunctions.h>            // for testing only
#include <bslx_testoutstream.h>                 // for testing only
#include <bslx_testinstream.h>                  // for testing only
#include <bslx_testinstreamexception.h>         // for testing only

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstdio.h>      // sprintf
#include <bsl_cstdlib.h>     // atoi
#include <bsl_cstring.h>     // strcmp

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a value-semantic, non-templated
// container type 'bdlc::IndexClerk' (and an associated index iterator,
// 'bdlc::IndexClerkIter') that manages (and provides sequential, read-only
// access to) a collection of reusable indices.  An index-clerk object issues
// integer indices on request, beginning with '0, 1, 2, ...'.  Indices that
// are no longer needed may be "decommissioned" (i.e., returned for reuse).
//
// Our primary (logical) concern is that every 'bdlc::IndexClerk' issues a
// proper sequence of indices:
//  - Indices that are returned are re-issued before issuing any new ones.
//  - That indices are returned in LIFO order (even though it's not specified).
//
// Since the functionality of 'bdlc::IndexClerk' is implemented almost entirely
// in terms of an 'bsl::vector' (to manage the stack of decommissioned
// indices), we want to focus our testing effort on making sure that this
// underlying container is used properly:
//  - Memory allocators are propagated correctly at construction.
//  - 'bdlc::IndexClerkIter' operate correctly on the underlying 'bsl::vector'.
//  - 'bdex' streaming is implemented correctly (in an exception-neutral way).
//
// The white-box state of a 'bdlc::IndexClerk' is represented by the next new
// index to be issued and a stack of returned indices.  We have chosen our
// *primary* *manipulators* to be
//
//  o int getIndex();
//  o void putIndex(int index);
//
// which together, in conjunction with the default constructor,
//
//  o bdlc::IndexClerk(bslma::Allocator *ba = 0);
//
// are sufficient to attain any achievable state.
//
// We have also identified the following set of *direct* *accessors* (i.e.,
// the maximal set of accessors that have "direct" contact with the physical
// state of the object):
//
//  o bdlc::IndexClerkIter begin() const;
//  o bdlc::IndexClerkIter end() const;
//  o int numCommissionedIndices() const;
//  o int numDecommissionedIndices() const;
//  o int nextNewIndex() const;
//  o class bdlc::IndexClerkIter;
//
// Note that the class 'bdlc::IndexClerkIter' is also a member of this set
// because it provides *direct* access to the internal stack of
// "decommissioned" indices.
//
// As a convenient testing aid, we will create a primitive generator function
// 'gg' that will be implemented in terms of the primary manipulators (above)
// to parse a string from a simple test language and correspondingly bring an
// unmodified 'bdlc::IndexClerk' to one of a suitable subset of arbitrary
// values sufficient for thorough testing.
//
// Finally, we will make heavy use of the 'bslma::TestAllocator' to ensure that
// (1) object memory is not leaked (i.e., is returned upon object destruction),
// (2) any local memory comes from (and is returned to) the default allocator
//     within the scope of a single method invocation,
// (3) no global memory is allocated during any test case.
//
// Note that the test-case ordering (below) reflects the standard pattern for
// testing (fully) value-semantic types that require manipulation after default
// construction to achieve every attainable state required for testing
// purposes.
// ----------------------------------------------------------------------------
//
//                          // ----------
//                          // IndexClerk
//                          // ----------
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] bdlc::IndexClerk(bslma::Allocator *ba = 0);
// [ 8] bdlc::IndexClerk(const IndexClerk&, bslma::Allocator *ba = 0);
//          // The use of default allocator is tested indirectly through return
//          // by value from 'g'.
// [ 2] ~bdlc::IndexClerk();
//
// MANIPULATORS
// [ 9] bdlc::IndexClerk& operator=(const bdlc::IndexClerk& rhs);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 2] int getIndex();
// [ 2] void putIndex(int index);
// [11] void removeAll();
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 4] bdlc::IndexClerkIter begin() const;
// [ 4] bdlc::IndexClerkIter end() const;
// [13] bool isInUse(int index) const;
// [ 4] int numCommissionedIndices() const;
// [ 4] int numDecommissionedIndices() const;
// [ 4] int nextNewIndex() const;
// [12] bsl::ostream& print(bsl::ostream& stream, int lvl, int sp) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const IndexClerk& l, const IndexClerk& r);
// [ 6] bool operator!=(const IndexClerk& l, const IndexClerk& r);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const IndexClerk&);
//
//                         // --------------
//                         // IndexClerkIter
//                         // --------------
//
// CREATORS
// [ 4] bdlc::IndexClerkIter();
// [14] bdlc::IndexClerkIter(const int *index);
// [14] bdlc::IndexClerkIter(const bdlc::IndexClerkIter& original);
//
// MANIPULATORS
// [ 4] bdlc::IndexClerkIter& operator=(const bdlc::IndexClerkIter& rhs);
// [ 4] bdlc::IndexClerkIter& operator++();
// [14] bdlc::IndexClerkIter& operator--();
//
// ACCESSORS
// [ 4] int operator*() const;
//
// FREE OPERATORS
// [ 4] bool operator==(const IndexClerkIter&, const IndexClerkIter&);
// [ 4] bool operator!=(const IndexClerkIter&, const IndexClerkIter&);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] int ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] bdlc::IndexClerk& gg(bdlc::IndexClerk *object, const char *spec);
// [ 8] bdlc::IndexClerk   g(const char *spec);
// [ 7] bdlc::IndexClerk(const bdlc::IndexClerk&, bslma::Allocator *ba);
// [  ] CONCERN: Object memory is never leaked by the index clerk.
// [  ] CONCERN: Any function-local memory comes from the default allocator.

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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlc::IndexClerk     Obj;
typedef bdlc::IndexClerkIter Iter;

// ============================================================================
//            GENERATOR FUNCTIONS 'g', 'gg' and 'ggg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.
//
///LANGUAGE SPECIFICATION:
///-----------------------
// The a valid 'spec' string has two parts separated by a comma (',')
// delimiter:
//..
//  <SPEC> := ^[0-9]*,[0-9]+$
//..
// Decimal digit characters ('0'..'9') on the left of delimiter corresponds to
// the decommissioned indices and must be unique.  The unsigned number (also
// consisting of decimal digits) on the, right taken as a whole, corresponds
// to the next new index.  No other characters (including whitespace) are
// valid.
//
// Note that object's state is undefined unless:
//  (0) The input object is in the initial state (i.e., nextNewIndex() == 0).
//  (1) The string consists of only the characters in the set ['0'..'9', ','].
//  (2) there is exactly one comma (',') delimiter.
//  (3) There are no duplicate digits on the left of the delimiter.
//  (4) There is at least one digit on the right of the delimiter.
//  (5) The maximum digit to the left of the delimiter is less than the number
//      on the right, which is itself at most 8 digits (including any leading
//      0).
//
///LIMITATIONS:
///------------
// This generator function assumes that the only indices from 0 to 9 inclusive
// will be decommissioned for testing purposes.
//
///EXAMPLES
///--------
//  Spec           Decommissioned               Next
//  String         Indices                      New Index
//  -----------    --------------               ---------
//  ",0"                                                0
//  "301,7"        1, 0, 3                              7
//  "87,12"        7, 8                                12
//  ",354"                                            354
//  "543210,6"     0, 1, 2, 3, 4, 5                     6
// ----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int vF = 1)
    // Configure the specified (initially empty) 'object' according to the
    // specified 'spec', using only the primary manipulator functions
    // 'getIndex' and 'putIndex'.  Optionally specify a zero 'vF' (verbose
    // Flag) to suppress 'spec' syntax error messages.  Return the index of
    // the first invalid character, and a negative value otherwise.  Note that
    // this function is used to implement the primitive generator 'gg' as well
    // as allow for direct verification of syntax error detection within this
    // test driver.  Note that decommissioned indices are "pushed" onto the
    // underlying stack from right to left such that the left most index in
    // the spec is the first to be returned on a 'getIndex' operation.
{
    int maxDecommIndex   = -1;
    int numDecommIndices =  0;

    const char VALID[]   = "0123456789,";   // the only valid spec characters
    const char DELIM     = ',';

    if (object->nextNewIndex() != 0) {
        if (vF) {
            cout << "Error: supplied object not in initial state."  << endl;
        }
        return 0;                                                     // RETURN
    }

    const int specLength = strlen(spec);
    const int firstInvalid = strspn(spec, VALID);

    if (specLength != firstInvalid) {
        const int pos = firstInvalid;
        if (vF) {
            cout << "Error at position " << pos << ": "
                 << "illegal character '" << spec[firstInvalid] << "'."
                 << endl;
        }
        return pos;                                                   // RETURN
    }

    // Parse any leading decommissioned indices.

    bool digitInUseFlag[10] = { }; // Initialize each element to 'false'.

    int i = 0;
    for (; i < specLength; ++i) {
        if (DELIM == spec[i])  {
            break;
        }

        int d = spec[i] - '0';  // Note that spec[i] must be a decimal digit.

        if (digitInUseFlag[d]) {
            const int pos = i;
            if (vF) {
                cout << "Error at position " << pos << ": "
                 << "duplicate decommissioned index'" << d << "'." << endl;
            }
            return pos;                                               // RETURN
        }
        digitInUseFlag[d] = true;

        if (d > maxDecommIndex) {
            maxDecommIndex = d;
        }

        ++numDecommIndices;
    }

    if (i >= specLength) {
        const int pos = specLength;
        if (vF) {
            cout << "Error at position " << pos << ": "
                 << "missing delimiter." << endl;
        }
        return pos;                                                   // RETURN
    }

    const int nextNewIndexPos = i + 1;

    const char *const nextNewIndexAddr = spec + nextNewIndexPos;

    if (const char *p = strchr(nextNewIndexAddr, DELIM)) {
        const int pos = p - spec;
        if (vF) {
            cout << "Error at position " << pos << ": "
                 << "encountered second delimiter." << endl;
        }
        return pos;                                                   // RETURN
    }

    // The remainder of the string consists of only decimal digits.

    const int nextNewIndexLength = specLength - i - 1;
    ASSERT(nextNewIndexLength >= 0)

    if (0 == nextNewIndexLength) {
        const int pos = specLength;
        if (vF) {
            cout << "Error at position " << pos << ": "
                 << "missing decimal digit after delimiter." << endl;
        }
        return pos;                                                   // RETURN
    }

    if (nextNewIndexLength > 8) {
        const int pos = nextNewIndexPos + 8;
        if (vF) {
            cout << "Error at position " << pos << ": "
                 << "next new index exceeds 8 digits." << endl;
        }
        return pos;                                                   // RETURN
    }

    const int nextNewIndex = atoi(spec + i + 1);

    if (nextNewIndex <= maxDecommIndex) {
        const int pos = nextNewIndexPos;
        if (vF) {
            cout << "Error at position " << pos << ": nextNewIndex, "
                 << nextNewIndex << " <= " << "maxDecommissionedIndex, "
                 << maxDecommIndex << "." << endl;
        }
        return pos;                                                   // RETURN
    }

    // The parsed 'spec' string is valid: Set the state of the object.

    for (int i = 0; i < nextNewIndex; ++i) {
        ASSERT(i == object->getIndex());
    }

    for (int i = numDecommIndices - 1; i >= 0; --i) {
        object->putIndex(spec[i] - '0');
    }

    return -1; // SUCCESS (no syntax error)
}

Obj& gg(Obj *object, const char *spec)
    // Return a reference to the specified 'object' with its value adjusted
    // according to the specified 'spec'.  The behavior is undefined unless
    // 'object' and 'spec' are non-zero, and 'spec' is a valid specification.
{
    ASSERT(object);
    ASSERT(spec);
    ASSERT(0 > ggg(object, spec));
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object configured using the specified 'spec',
    // using the default 'allocator' to supply memory.
{
    Obj object;
    return gg(&object, spec);
}

// ============================================================================
//        Additional Functionality Needed to Complete Usage Test Case
// ----------------------------------------------------------------------------

#if defined(BDE_BUILD_TARGET_SAFE)
const bool safe = true;
#else
const bool safe = false;
#endif

class Security {
    // This object contains an identifier which is used to verify that it had
    // been properly added to a vector in the usage example.

    // DATA
    int d_id;  // security identifier

  public:
    // CREATORS
    Security(int id = -1) : d_id(id) { }
        // Create a security object.  Optionally specify 'id' to assign this
        // security an identifier.

    // ACCESSORS
    int getId() const { return d_id; }
        // Return this security's identifier.
};

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
// A 'bdlc::IndexClerk' is commonly used in conjunction with an array to enable
// machine-address-independent referencing.  Rather than dynamically allocating
// an object and holding its address, the object is stored in the array at the
// next position dispensed by its associated 'bdlc::IndexClerk', and that index
// becomes an identifier (Id) for the new object.  Instead of destroying an
// unneeded object, its Id is merely returned to the clerk.
//
// Care must be taken to ensure that objects "created" at reused indices (i.e.,
// indices below the current length of the array) *replace* (the value of) an
// existing object in the array while objects created at new indices (i.e.,
// indices at the current length) are *appended* to the array.
//
// For example, suppose we have a security class object.  To add and remove
// security values from a security array/clerk pair, you might write the
// following two functions:
//..
    int addSecurity(bsl::vector<Security> *securityArray,
                    bdlc::IndexClerk      *securityClerk,
                    const Security&        newSecurity)
        // Add a copy of the specified 'newSecurity' to the specified
        // 'securityArray' at the index dispensed by the specified
        // 'securityClerk'.  Also update the 'securityClerk', and return the id
        // (in 'securityArray') for the newly added security.
    {
        BSLS_ASSERT(securityArray);
        BSLS_ASSERT(securityClerk);

        int id = securityClerk->getIndex();

        if (id < (int) securityArray->size()) {
            (*securityArray)[id] = newSecurity;
        }
        else {
            securityArray->push_back(newSecurity);
        }

        return id;
    }

    void removeSecurity(bsl::vector<Security> *securityArray,
                        bdlc::IndexClerk      *securityClerk,
                        int                    securityId)
        // Remove the security object identified by the specified 'securityId'
        // from the specified 'securityArray', and update the specified
        // 'securityClerk' (making 'securityId' available for reuse).  The
        // behavior is undefined unless 'securityId' refers to an active
        // security in 'securityArray' dispensed by 'securityClerk'.
    {
        BSLS_ASSERT(securityArray);
        BSLS_ASSERT(securityClerk);

        BSLS_ASSERT(0                             <= securityId);
        BSLS_ASSERT(securityClerk->nextNewIndex() >  securityId);
        BSLS_ASSERT((int) securityArray->size()   >  securityId);

        // Note that the 'isInUse' function (below) runs in linear time.

        BSLS_ASSERT_SAFE(securityClerk->isInUse(securityId));

        (*securityArray)[securityId] = Security();  // optional
        securityClerk->putIndex(securityId);
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Allocator *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // zero is always the leading case
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Simply invoke the functions 'addSecurity' and 'removeSecurity' to
        //   ensure the code compiles.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        bsl::vector<Security> securityArray;
        bdlc::IndexClerk        indexClerk;

        int id0 = addSecurity(&securityArray, &indexClerk, Security(0));
        ASSERT(0 == id0);
        ASSERT(0 == securityArray[id0].getId());

        int id1 = addSecurity(&securityArray, &indexClerk, Security(1));
        ASSERT(1 == id1);
        ASSERT(0 == securityArray[id0].getId());
        ASSERT(1 == securityArray[id1].getId());

        int id2 = addSecurity(&securityArray, &indexClerk, Security(2));
        ASSERT(2 == id2);
        ASSERT(0 == securityArray[id0].getId());
        ASSERT(1 == securityArray[id1].getId());
        ASSERT(2 == securityArray[id2].getId());

        removeSecurity(&securityArray, &indexClerk, id1);
        ASSERT( 0 == securityArray[id0].getId());
        ASSERT(-1 == securityArray[id1].getId());
        ASSERT( 2 == securityArray[id2].getId());

        id1 = addSecurity(&securityArray, &indexClerk, Security(3));
        ASSERT(0 == securityArray[id0].getId());
        ASSERT(3 == securityArray[id1].getId());
        ASSERT(2 == securityArray[id2].getId());

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // ITERATOR TEST:
        //   The methods of 'bdlc::IndexClerkIter' not tested under the basic
        //   accessors test should be tested in this method.
        //
        // Concerns:
        //   1. That 'operator--' can be invoked, moving the
        //      'bdlc::IndexClerkIter' to point to a previous index.
        //   2. That 'operator--' returns a reference to the iterator.
        //   3. That a valid 'bdlc::IndexClerkIter' can be constructed from an
        //      integer pointer.
        //   4. That a valid 'bdlc::IndexClerkIter' can be constructed from
        //      another 'bdlc::IndexClerkIter'.
        //
        // Plan:
        //   For concern 1, we first create a 'bdlc::IndexClerk' with several
        //   decommissioned indices.  Then, using 'operator*', we can verify
        //   that the index pointed to by the iterator is properly modified
        //   after invoking 'operator--'.
        //
        //   For concern 2, we first create an array of integers.  Then, we
        //   construct a 'bdlc::IndexClerkIter' using the state constructor.
        //   To verify validity of the iterator, we invoke the tested
        //   'operator++' and 'operator--', and invoke 'operator*' to compare
        //   against the expected value.
        //
        //   For concern 3, we first create a 'bdlc::IndexClerk'.  We then
        //   obtain two iterators by invoking 'begin'.  We then copy construct
        //   a third iterator from the second iterator.  Iterate through
        //   the list of decommissioned index to verify the copy constructed
        //   iterator is valid.  Further assert that the iterator used as a
        //   parameter for copy construction is not modified by comparing it
        //   with the first (unused) iterator.
        //
        // Testing:
        //  [13] bdlc::IndexClerkIter& operator--();
        //  [13] bdlc::IndexClerkIter(const int *index);
        //  [13] bdlc::IndexClerkIter(const bdlc::IndexClerkIter& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ITERATOR TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting the 'operator--()' method." << endl;
        {
            bdlc::IndexClerk idxClerk(g("135,9"), &objectAllocator);

            bdlc::IndexClerkIter  itr     = idxClerk.end();
            bdlc::IndexClerkIter *address = &itr;

            ASSERT(address == &--itr);
            if (veryVerbose) { T_ P(*itr) }
            ASSERT(5 == *itr);      ASSERT(idxClerk.begin() != itr);

            ASSERT(address == &--itr);
            if (veryVerbose) { T_ P(*itr) }
            ASSERT(3 == *itr);      ASSERT(idxClerk.begin() != itr);

            ASSERT(address == &--itr);
            if (veryVerbose) { T_ P(*itr) }
            ASSERT(1 == *itr);      ASSERT(idxClerk.begin() == itr);
        }

        if (verbose) cout << "\nTesting the state constructor." << endl;
        {
            static const int INDICES[5] = { 0, 1, 2, 3, 4 };

            bdlc::IndexClerkIter itr(&INDICES[3]);

            if (veryVerbose) { T_ P(*itr) }
            ++itr;                  ASSERT(1 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            ++itr;                  ASSERT(0 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            --itr;                  ASSERT(1 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            --itr;                  ASSERT(2 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            --itr;                  ASSERT(3 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            --itr;                  ASSERT(4 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            ++itr;                  ASSERT(3 == *itr);

            if (veryVerbose) { T_ P(*itr) }
            ++itr;                  ASSERT(2 == *itr);
        }

        if (verbose) cout << "\nTesting the copy constructor." << endl;
        {
            bdlc::IndexClerk idxClerk(g("135,9"), &objectAllocator);

            bdlc::IndexClerkIter itrA = idxClerk.begin();
            bdlc::IndexClerkIter itrB = idxClerk.begin();

            bdlc::IndexClerkIter itrC(itrA);

            if (veryVerbose) { T_ P_(*itrA) P_(*itrB) P(*itrC) }
            ASSERT(itrB == itrC);   ASSERT(itrA == itrC);
            ASSERT(itrA == itrB);   ASSERT(1    == *itrC);

            ++itrC;
            if (veryVerbose) { T_ P_(*itrA) P_(*itrB) P(*itrC) }
            ASSERT(itrB != itrC);   ASSERT(itrA != itrC);
            ASSERT(itrA == itrB);   ASSERT(3    == *itrC);

            ++itrC;
            if (veryVerbose) { T_ P_(*itrA) P_(*itrB) P(*itrC) }
            ASSERT(itrB != itrC);   ASSERT(itrA != itrC);
            ASSERT(itrA == itrB);   ASSERT(5    == *itrC);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'isInUse' TEST:
        //
        // Concerns:
        //   That 'isInUse' properly return 'false' for indices not in use and
        //   'true' for indices in use.
        //
        // Plan:
        //   First create a 'bdlc::IndexClerk' using the generator function
        //   'g'.  Then using the loop-based technique, invoke 'putIndex' to
        //   return indices to the index clerk, and verify that 'isInUse'
        //   return 'false' for the indices returned and 'true' otherwise.
        //
        // Testing:
        //   bool isInUse(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'isInUse' TEST" << endl
                                  << "==============" << endl;


        if (verbose) cout << "\nTesting the 'isInUse' method." << endl;
        {

            const int TESTSIZE = 9;
            char buf[12];  // 1 for ',', 1 for '\0', 10 for digits.
            sprintf(&buf[1], "%d", TESTSIZE);
            buf[0] = ',';

            if (veryVerbose) { T_ P(buf) }

            Obj mX(g(buf), &objectAllocator);   const Obj& X = mX;

            for (int i = 0; i < TESTSIZE; ++i) {
                if (veryVerbose) { T_ P(i) }
                mX.putIndex(i);

                for (int j = 0; j <= i; ++j) {
                    if (veryVerbose) { T_ T_ P(j) }
                    LOOP2_ASSERT(i, j, false == X.isInUse(j));
                }
                for (int j = i + 1; j < TESTSIZE; ++j) {
                    if (veryVerbose) { T_ T_ P(j) }
                    LOOP2_ASSERT(i, j, true  == X.isInUse(j));
                }
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
#if defined(BSLS_PLATFORM_CMP_IBM)
        // For some odd reason IBM is the only compiler that uses the default
        // allocator with the generator function here.  Other compilers seem to
        // have elided this.
        ASSERT(0 != defaultAllocator.numBlocksTotal());
#else
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
#endif
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // 'print' TEST:
        //
        // Concerns:
        //   That 'print' method should print to the specified 'stream' with
        //   the appropriate indentation and new line characters.
        //
        // Plan:
        //   Using the table driven technique, enumerate a sequence of test
        //   vectors, each containing the line number, a specification and an
        //   expected output.  For each test vector, construct an independent
        //   object 'mX' and configure it using the tested generator function
        //   'gg'.  Create an 'ostringstream' object and use 'print' to stream
        //   a constant reference to 'mX'.  Finally, compare the contents of
        //   the stream object with the expected output.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream, int lvl, int sp) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'print' TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting the 'print' method." << endl;
        {
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
    //LINE  SPEC    INDENT      SPACES  EXPECTED
    //----  ----    ----------  ------  ------------------------------------

    // No decommissioned indices.
    { L_,   ",1",      0,       -2,     "[ (1) ]"                            },
    { L_,   ",1",      0,       -1,     "[ (1) ]"                            },
    { L_,   ",1",      0,        0,     "[" NL "(1)" NL "]" NL               },
    { L_,   ",1",      0,        1,     "[" NL " (1)" NL "]" NL              },
    { L_,   ",1",      0,        2,     "[" NL "  (1)" NL "]" NL             },
    { L_,   ",1",      1,        1,     " [" NL "  (1)" NL " ]" NL           },
    { L_,   ",1",      1,        2,     "  [" NL "    (1)" NL "  ]" NL       },
    { L_,   ",1",      2,        1,     "  [" NL "   (1)" NL "  ]" NL        },
    { L_,   ",1",      2,        2,     "    [" NL "      (1)" NL "    ]" NL },
    { L_,   ",1",     -1,        1,     "[" NL "  (1)" NL " ]" NL            },
    { L_,   ",1",     -1,        2,     "[" NL "    (1)" NL "  ]" NL         },

    // Have decommissioned indices.
    { L_,   "135,6",   0,       -2,     "[ 1 3 5 (6) ]"                      },
    { L_,   "135,6",   0,       -1,     "[ 1 3 5 (6) ]"                      },
    { L_,   "135,6",   0,        0,     "[" NL "135(6)" NL "]" NL            },
    { L_,   "135,6",   0,        1,     "[" NL " 1 3 5 (6)" NL "]" NL        },
    { L_,   "135,6",   0,        2,     "[" NL "  1  3  5  (6)" NL "]" NL    },
    { L_,   "135,6",   1,        1,     " [" NL "  1  3  5  (6)" NL " ]" NL  },
    { L_,   "135,6",   1,        2,
                               "  [" NL "    1    3    5    (6)" NL "  ]" NL },
    { L_,   "135,6",   2,        1,
                                   "  [" NL "   1   3   5   (6)" NL "  ]" NL },
    { L_,   "135,6",   2,        2,
                   "    [" NL "      1      3      5      (6)" NL "    ]" NL },
    { L_,   "135,6",  -1,        1,     "[" NL "  1  3  5  (6)" NL " ]"  NL },
    { L_,   "135,6",  -1,        2,
                                 "[" NL "    1    3    5    (6)" NL "  ]" NL },
            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int BUF_SIZE = 1000;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE = DATA[i].d_lineNum;
                const char     *SPEC = DATA[i].d_spec;
                const int     INDENT = DATA[i].d_indent;
                const int     SPACES = DATA[i].d_spaces;
                const char *EXPECTED = DATA[i].d_fmt_p;

                Obj mX(&objectAllocator);          const Obj& X = mX;
                gg(&mX, SPEC);

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P_(INDENT) P_(SPACES) P_(X)
                    P(EXPECTED)
                }

                char buf[BUF_SIZE];
                memset(buf, 0, sizeof(buf));
                ostringstream outbuf(bsl::string(buf, BUF_SIZE));

                bsls::Types::Int64 numBlocksTotal =
                                             defaultAllocator.numBlocksTotal();
                X.print(outbuf, INDENT, SPACES);
                ASSERT(numBlocksTotal == defaultAllocator.numBlocksTotal());

                LOOP3_ASSERT(LINE,
                             EXPECTED,
                             outbuf.str(),
                             0 == bsl::strcmp(EXPECTED, outbuf.str().c_str()));
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'removeAll' TEST:
        //
        // Concerns:
        //   1. That invoking 'removeAll' should reset the 'bdlc::IndexClerk'
        //      to its default constructed state.
        //   2. That invoking 'removeAll' on default constructed
        //      'bdlc::IndexClerk' should have no effect.
        //
        // Plan:
        //   For concern 1, using the array-driven technique, construct a
        //   representative set of specs S for objects having substantial and
        //   varied differences in value.  For each spec in S, generate two
        //   identical 'bdlc::IndexClerk' objects using the generator function
        //   'g'.  Invoke 'removeAll' on the first object and verify that the
        //   object is reset to its default constructed state.
        //
        //   For concern 2, default construct three objects, W, X and Y.  Using
        //   W as a control object, invoke 'removeAll' on Y.  Verify that all
        //   three objects are the same.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'removeAll' TEST" << endl
                                  << "================" << endl;

        if (verbose) cout << "\nTesting 'removeAll' on configured objects."
                          << endl;

        static const char *SPECS[] = {
            // Empty decommissioned indices.
            ",1", ",2", ",3", ",4", ",5", ",6", ",7", ",8", ",9"

            // Next new index: 1
            ,",1", "0,1"

            // Next new index: 2
            ,",2", "0,2", "1,2", "01,2", "10,2"

            // Next new index: 3
            ,",3",    "0,3",  "1,3",  "2,3",   "01,3",  "02,3",  "12,3"
            ,"10,3",  "20,3", "21,3", "012,3", "021,3", "120,3", "102,3"
            ,"201,3", "210,3"

            // Large next new indices.
            ,"135,246", "246,135", "1234,5678", "8765,4321"
            ,"13579,12345678", "86420,87654321"

            // Same generated sequence of decommissioned indices, but different
            // next new index.
            ,"321,4", "3214,5", "32145,6"

            // Almost identical and long sequence of decommissioned indices.
            ,"0123456789,9998", "0123456789,9999", "012345678,9999"
            ,"012345679,9999",  "1023456789,9999", "0123456798,9999"
        };
        const int NUM_SPEC = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *SPEC = SPECS[i];

            const Obj W(&objectAllocator);  // control

            Obj mX(&objectAllocator);       const Obj& X = mX;
            gg(&mX, SPEC);

            Obj mY(&objectAllocator);       const Obj& Y = mY;
            gg(&mY, SPEC);

            if (veryVerbose) { T_ T_ P_(SPEC) P_(W) P_(X) P(Y) }

            LOOP2_ASSERT(W, X, W != X);      LOOP2_ASSERT(Y, X, Y == X);
            LOOP2_ASSERT(W, Y, W != Y);

            mX.removeAll();

            LOOP2_ASSERT(W, X, W == X);      LOOP2_ASSERT(Y, X, Y != X);
            LOOP2_ASSERT(W, Y, W != Y);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting 'removeAll' on default constructed "
                             "objects." << endl;
        {
            const Obj W(&objectAllocator);  // control
            Obj mX(&objectAllocator);       const Obj& X = mX;
            Obj mY(&objectAllocator);       const Obj& Y = mY;

            if (veryVerbose) { T_ P_(W) P_(X) P(Y) }
            LOOP2_ASSERT(W, X, W == X);      LOOP2_ASSERT(Y, X, Y == X);
            LOOP2_ASSERT(W, Y, W == Y);

            mX.removeAll();

            LOOP2_ASSERT(W, X, W == X);      LOOP2_ASSERT(Y, X, Y == X);
            LOOP2_ASSERT(W, Y, W == Y);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   We need to probe the member functions 'bdexStreamIn' and
        //   'bdexStreamOut' in the manner of a "breathing test" to verify
        //   basic functionality, then we need to thoroughly test that
        //   functionality using the available bdex stream functions,
        //   which forward appropriate calls to the member functions.  We also
        //   want to step through the sequence of possible stream states
        //   (valid, empty, invalid, incomplete, and corrupted), appropriately
        //   selecting data sets as described below.  In all cases, we need to
        //   confirm exception neutrality using the specially instrumented
        //   'bslx::TestInStream' and a pair of standard macros,
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //   'bslx::TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'bdexStreamOut' and
        //     'bdexStreamIn' methods.
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty stream, and then an invalid stream.  Verify after
        //     each attempt that the object is unchanged and that the stream is
        //     invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects that
        //     are either successfully modified or left entirely unmodified,
        //     and that the stream became invalid immediately after the first
        //     incomplete read.  Finally ensure that each object streamed into
        //     is in some valid state by assigning it a distinct new value and
        //     testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of
        //     a typical valid (control) stream and verify that it can be
        //     streamed in successfully.  A corrupted 'bdlc::IndexClerk' is an
        //     index clerk that has a next new index less than 0, has any
        //     decommissioned indices greater than the next new index, has
        //     negative decommissioned indices, or duplicate decommissioned
        //     indices.  Also check for bad version numbers.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        // Testing:
        //   static int maxSupportedBdexVersion();
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING STREAMING FUNCTIONALITY" << endl
                                  << "===============================" << endl;

        {

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Abbreviations used for streaming tests

        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20130612;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Scalar and array object values for various stream tests

        const Obj VA(g("012,3"), &objectAllocator);
        const Obj VB(g("20,3"), &objectAllocator);
        const Obj VC(g("12345,9"), &objectAllocator);
        const Obj VD(g("05827,9"), &objectAllocator);
        const Obj VE(g("012876,9"), &objectAllocator);
        const Obj VF(g("012876,300"), &objectAllocator);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        const int VERSION = Obj::maxSupportedBdexVersion(VERSION_SELECTOR);
        if (verbose) cout << "\nTesting 'bdexStreamOut' and (valid) "
                          << "'bdexStreamIn' functionality." << endl;
        {
            // Testing 'bdexStreamOut' and 'bdexStreamIn' directly.
            const Obj X(VC, &objectAllocator);
            Out out(1);
            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                         ASSERT(!in.isEmpty());

            Obj t(VA, &objectAllocator);        ASSERT(X != t);
            t.bdexStreamIn(in, VERSION);        ASSERT(X == t);
            ASSERT(in);                         ASSERT(in.isEmpty());
        }

        if (verbose) cout << "\nThorough test of bdex stream functions."
                          << endl;
        {
            // Testing '<<' and '>>' operators thoroughly.
            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) { T_ P(i) }

                const Obj X(VALUES[i], &objectAllocator);
                Out out(1);
                bslx::OutStreamFunctions::bdexStreamOut(out, X, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    if (veryVerbose) { T_ T_ P(j) }

                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j], &objectAllocator);

                    LOOP2_ASSERT(i, j, (X == t) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                      in.reset();
                      LOOP2_ASSERT(i, j, (X == t) == (i == j));
                      bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            // Testing empty and invalid streams.
            Out out(1);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) { T_ P(i) }

                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i], &objectAllocator);
                Obj t(X, &objectAllocator);
                LOOP_ASSERT(i, X == t);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                  bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, X == t);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "\tOn incomplete (but otherwise valid) data."
                          << endl;
        {
            bslma::Allocator *Z = &objectAllocator;  // for brevity

            // Testing incomplete (but otherwise valid) data.
            const Obj W1(VA, Z), X1(VB, Z), Y1(VC, Z);
            const Obj W2(VB, Z), X2(VC, Z), Y2(VD, Z);
            const Obj W3(VC, Z), X3(VD, Z), Y3(VE, Z);

            Out out(1);
            bslx::OutStreamFunctions::bdexStreamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bslx::OutStreamFunctions::bdexStreamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bslx::OutStreamFunctions::bdexStreamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const    OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                if (veryVerbose) { T_ P(i) }

                In in(OD, i);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  LOOP_ASSERT(i, in);
                  LOOP_ASSERT(i, !i == in.isEmpty());
                  Obj t1(W1, Z), t2(W2, Z), t3(W3, Z);

                  if (i < LOD1) {
                      bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                      LOOP_ASSERT(i, !in);
                      // Necessary because state of 't1' can be modified.  Only
                      // when '0 == i' will the state of 't1' be untouched.
                      if (0 == i) {
                          LOOP_ASSERT(i, W1 == t1);
                      }
                      // Verify that we still have a valid 'bdlc::IndexClerk'.
                      else {
                          Iter itr = t1.begin();
                          for (int j = 0; j < t1.numDecommissionedIndices();
                                                                  ++j, ++itr) {
                              if (veryVerbose) { T_ T_ P(j) }
                              LOOP2_ASSERT(i, j, t1.nextNewIndex() > *itr);
                          }
                          LOOP_ASSERT(i, t1.end() == itr);
                      }
                      bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                      bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  }
                  else if (i < LOD2) {
                      bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                      LOOP_ASSERT(i, !in);
                      if (LOD1 == i) {
                          LOOP_ASSERT(i, X1 == t2);
                      }
                      else {
                          Iter itr = t2.begin();
                          for (int j = 0; j < t2.numDecommissionedIndices();
                                                                  ++j, ++itr) {
                              if (veryVerbose) { T_ T_ P(j) }
                              LOOP2_ASSERT(i, j, t2.nextNewIndex() > *itr);
                          }
                          LOOP_ASSERT(i, t2.end() == itr);
                      }
                      bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  }
                  else {
                      bslx::InStreamFunctions::bdexStreamIn(in, t1, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X1 == t1);
                      bslx::InStreamFunctions::bdexStreamIn(in, t2, VERSION);
                      LOOP_ASSERT(i, in);     LOOP_ASSERT(i, X2 == t2);
                      bslx::InStreamFunctions::bdexStreamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);
                      if (LOD2 == i) {
                          LOOP_ASSERT(i, W3 == t3);
                      }
                      else {
                          Iter itr = t3.begin();
                          for (int j = 0; j < t3.numDecommissionedIndices();
                                                                  ++j, ++itr) {
                              if (veryVerbose) { T_ T_ P(j) }
                              LOOP2_ASSERT(i, j, t3.nextNewIndex() > *itr);
                          }
                          LOOP_ASSERT(i, t3.end() == itr);
                      }
                  }
                              LOOP_ASSERT(i, Y1 != t1);
                  t1 = Y1;    LOOP_ASSERT(i, Y1 == t1);

                              LOOP_ASSERT(i, Y2 != t2);
                  t2 = Y2;    LOOP_ASSERT(i, Y2 == t2);

                              LOOP_ASSERT(i, Y3 != t3);
                  t3 = Y3;    LOOP_ASSERT(i, Y3 == t3);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // There are three types of corrupted data:
        // 1.  Next new index less than 0.
        // 2.  Any of the decommissioned indices is greater than the next new
        //     index.
        // 3.  Bad version numbers.

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W(&objectAllocator);              // default value
        const Obj X(g("012,3"), &objectAllocator);  // original (control) value
        const Obj Y(g("135,6"), &objectAllocator);  // new (streamed-out) value

        if (veryVerbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out(1);
            Y.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);

            In in(OD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(in);
            ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
        }

        if (veryVerbose) cout << "\t\tNext new index less than 0." << endl;
        {
            const int id = -1;  // too small
            bsl::vector<int> stack; stack.push_back(1);

            Out out(1);
            out.putInt32(id);
            bslx::OutStreamFunctions::bdexStreamOut(out, stack, VERSION);
            const char *const PD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t);

            In in(PD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t);
        }

        if (veryVerbose) cout << "Decommissioned indices greater than the "
                                 "next new index." << endl;
        {
            const int id = 3;
            // Pushing in '3' makes this an invalid 'bdlc::IndexClerk'.
            bsl::vector<int> stack; stack.push_back(2);
            stack.push_back(3); stack.push_back(0);

            Out out(1);
            out.putInt32(id);
            bslx::OutStreamFunctions::bdexStreamOut(out, stack, VERSION);
            const char *const PD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t);

            In in(PD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t);
        }

        if (veryVerbose) cout << "Negative decommissioned indices" << endl;
        {
            const int id = 4;
            // Pushing in '-3' makes this an invalid 'bdlc::IndexClerk'.
            bsl::vector<int> stack; stack.push_back(2);
            stack.push_back(-3); stack.push_back(0);

            Out out(1);
            out.putInt32(id);
            bslx::OutStreamFunctions::bdexStreamOut(out, stack, VERSION);
            const char *const PD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t);

            In in(PD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t);
        }

        if (veryVerbose) cout << "Duplicate decommissioned indices" << endl;
        {
            const int id = 4;
            // Pushing in '2' twice makes this an invalid 'bdlc::IndexClerk'.
            bsl::vector<int> stack; stack.push_back(2);
            stack.push_back(2); stack.push_back(0);

            Out out(1);
            out.putInt32(id);
            bslx::OutStreamFunctions::bdexStreamOut(out, stack, VERSION);
            const char *const PD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t);

            In in(PD, LOD); ASSERT(in);
            bslx::InStreamFunctions::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t);
        }

        if (veryVerbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0;  // too small ('version' must be >= 1)

            Out out(1);
            Y.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);

            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char version = 5;  // too large (current version is 1)

            Out out(1);
            Y.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X, &objectAllocator);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);

            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            bslx::InStreamFunctions::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            // test 'maxSupportedBdexVersion()'
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X(&objectAllocator);
            ASSERT(1 == X.maxSupportedBdexVersion(VERSION_SELECTOR));
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
        }

        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
        ASSERT(0 != defaultAllocator.numBlocksTotal());  // generator function
                                                         // uses default
                                                         // allocator when
                                                         // return by value
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial
        //   value, and without affecting the rhs operand.  Also, any object
        //   must be assignable to itself (with no effect on value).
        //
        // Plan:
        //   Using the array-driven technique, construct a representative set
        //   of specs S for objects having substantial differences in value.
        //   Construct and initialize all combinations (u, v) in the cross
        //   product S X S, copy construct (with an allocator) a control w from
        //   v, assign v to u, and assert that 'w == u' and 'w == v'.  Then
        //   test aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that 'w == u'.
        //
        // Testing:
        //   bdlc::IndexClerk& operator=(const bdlc::IndexClerk& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = v." << endl;

        static const char *SPECS[] = {
            // Empty decommissioned indices.
            ",0", ",1", ",2", ",3", ",4"

            // Next new index: 1
            ,",1", "0,1"

            // Next new index: 2
            ,",2", "0,2", "1,2", "01,2", "10,2"

            // Next new index: 3
            ,",3",    "0,3",  "1,3",  "2,3"
            ,"10,3",  "20,3", "21,3", "012,3", "021,3", "120,3"

            // Large next new indices.
            ,"135,246", "246,135", "1234,567", "8765,432"
            ,"13579,1234", "86420,876"

            // Same generated sequence of decommissioned indices, but different
            // next new index.
            ,"321,4", "3214,5", "32145,6"

            // Almost identical and long sequence of decommissioned indices.
            ,"0123456789,998", "0123456789,999", "012345678,999"
            ,"012345679,999",  "1023456789,999", "0123456798,999"
        };
        const int NUM_SPEC = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *V_SPEC = SPECS[i];

            Obj mV(&objectAllocator);           const Obj& V = mV;
            gg(&mV, V_SPEC);

            if (veryVerbose) { T_ P_(V_SPEC) P(V) }

            for (int j = 0; j < NUM_SPEC; ++j) {
                const char *U_SPEC = SPECS[j];

                Obj mU(&objectAllocator);       const Obj& U = mU;
                gg(&mU, U_SPEC);

                Obj mW(V, &objectAllocator);    const Obj& W = mW;  // control

                if (veryVerbose) { T_ T_ P_(U_SPEC) P_(U) P(W) }

                mU = V;

                LOOP2_ASSERT(W, U, W == U);
                LOOP2_ASSERT(W, V, W == V);
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting assignment u = u (Aliasing)." << endl;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *SPEC = SPECS[i];

            Obj mU(&objectAllocator);           const Obj& U = mU;
            gg(&mU, SPEC);

            Obj mW(U, &objectAllocator);        const Obj& W = mW;  // control

            if (veryVerbose) { T_ P(SPEC) P_(U) P(W) }

            mU = U;

            LOOP2_ASSERT(W, U, W == U);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION AND COPY CTOR:
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need only
        //   to verify that the arguments are properly forwarded and that 'g'
        //   returns the new object by value.
        //
        // Plan:
        //   Using the array-driven technique, enumerate a representative set
        //   of specifications, compare the object returned (by value) from the
        //   generator function, 'g(SPEC)' with the value of a newly
        //   constructed object 'mX' configured using 'gg(&mX, SPEC)'.  The
        //   test also ensures that 'g' returns a distinct object by comparing
        //   the memory addresses.  Note that returning the object by value
        //   implicitly tests copy construction.
        //
        // Testing:
        //   bdlc::IndexClerk g(const char *spec);
        //   bdlc::IndexClerk(const IndexClerk&, bslma::Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING GENERATOR FUNCTION AND COPY CTOR" << endl
                         << "========================================" << endl;

        if (verbose) cout <<
           "\nCompare values produced by 'g', 'gg' on various inputs." << endl;

        static const char *SPECS[] = {
            // depth 0 (no decommissioned indices)
            ",0", ",1", ",2", ",3", ",4", ",5", ",6", ",7", ",8", ",9"

            // depth 1
            ,"0,1", "1,2", "2,3", "3,4", "4,5", "5,6", "6,7", "7,8", "8,9"

            // depth 2
            ,"01,2", "12,3", "23,4", "34,5", "45,6", "56,7", "67,8", "78,9"

            // depth 3
            ,"012,3", "123,4", "234,5", "345,6", "456,7", "567,8", "678,9"

            // depth 5
            ,"13579,2468", "02468,1357"

            // depth 10  (max depth for 'gg' function.
            ,"0123456789,12345", "9876543210,98760"
        };
        const int NUM_SPEC = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *SPEC = SPECS[i];

            Obj mX(&objectAllocator);
            const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { T_ P_(SPEC) P(X); }

            LOOP2_ASSERT(X, g(SPEC), X == g(SPEC));
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
        ASSERT(0 != defaultAllocator.numBlocksTotal());  // Generator function
                                                         // uses default
                                                         // allocator for
                                                         // return by value.
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *SPEC = "123,4";

            ASSERT(sizeof(Obj) == sizeof g(SPEC));  // compile-time fact

            Obj mX(&objectAllocator);               // runtime tests
            Obj& r1 = gg(&mX, SPEC);
            const Obj& r2 = g(SPEC);
            const Obj& r3 = g(SPEC);
            ASSERT(&mX == &r1);
            ASSERT(&r3 != &r2);
            ASSERT(&mX != &r2);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == defaultAllocator.numBlocksInUse());
        ASSERT(0 != defaultAllocator.numBlocksTotal());  // Generator function
                                                         // uses default
                                                         // allocator for
                                                         // return by value.
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING EXTENDED COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed (with an allocator)
        //   such that the new object has the same value without affecting the
        //   original.
        //
        // Plan:
        //   Using the array-driven technique, construct a representative set
        //   of specs S for objects having substantial and varied differences
        //   in value.  For each element in S, construct and initialize two
        //   identically-valued objects, W and X, using the 'gg' function.
        //   Then copy construct (with an allocator) an object Y from X, and
        //   use the equality operator to assert that X and Y each have the
        //   same value as W.
        //
        // Testing:
        //   bdlc::IndexClerk(const bdlc::IndexClerk&, bslma::Allocator *ba);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING EXTENDED COPY CONSTRUCTOR" << endl
                         << "=================================" << endl;

        if (verbose) cout <<
            "\nCopy construct various different values." << endl;

        static const char *SPECS[] = {
            // depth 0 (no decommissioned indices)
            ",0", ",1", ",2", ",3", ",4", ",5", ",6", ",7", ",8", ",9"

            // depth 1
            ,"0,1", "1,2", "2,3", "3,4", "4,5", "5,6", "6,7", "7,8", "8,9"

            // depth 2
            ,"01,2", "12,3", "23,4", "34,5", "45,6", "56,7", "67,8", "78,9"

            // depth 3
            ,"012,3", "123,4", "234,5", "345,6", "456,7", "567,8", "678,9"

            // depth 5
            ,"13579,2468", "02468,1357"

            // depth 10  (max depth for 'gg' function.
            ,"0123456789,12345", "9876543210,98760"
        };
        const int NUM_SPEC = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *SPEC = SPECS[i];

            Obj mW(&objectAllocator);           const Obj& W = mW;
            gg(&mW, SPEC);

            Obj mX(&objectAllocator);           const Obj& X = mX;
            gg(&mX, SPEC);

            if (veryVerbose) { T_ P_(SPEC) P_(W) P(X) }

            Obj mY(X, &objectAllocator);        const Obj& Y = mY;

            LOOP2_ASSERT(W, X, W == X);
            LOOP2_ASSERT(W, Y, W == Y);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //   That 'operator==' returns 'false' for objects that are very
        //   similar but still represent different values, and returns 'true'
        //   for objects that represent the same value.  Likewise, we want to
        //   make sure that 'operator!=' returns 'true' for objects that are
        //   very similar but still have different values, and return
        //   'true' for objects that have the same value.
        //
        //   In particular, that objects that would produce the same sequence,
        //   but have different values for next new index do not compare equal.
        //
        // Plan:
        //   Using the array-driven technique, construct a set of specs
        //   containing similar but different object VALUES.  Then loop through
        //   the cross product of the test data.  For each tuple, generate two
        //   objects 'U' and 'V' using the previously tested 'gg' function.
        //   Use the '==' and '!=' operators and check their return value for
        //   correctness.
        //
        // Testing:
        //   bool operator==(const IndexClerk& l, const IndexClerk& r);
        //   bool operator!=(const IndexClerk& l, const IndexClerk& r);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING EQUALITY OPERATORS" << endl
                                  << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u,v) in S X S." << endl;

        static const char *SPECS[] = {
            // Next new index: 0
            ",0"

            // Next new index: 1
            ,",1", "0,1"

            // Next new index: 2
            ,",2", "0,2", "1,2", "01,2", "10,2"

            // Next new index: 3
            ,",3",    "0,3",  "1,3",  "2,3",   "01,3",  "02,3",  "12,3"
            ,"10,3",  "20,3", "21,3", "012,3", "021,3", "120,3", "102,3"
            ,"201,3", "210,3"

            // Same generated sequence of decommissioned indices, but different
            // next new index.
            ,"321,4", "3214,5", "32145,6"

            // Almost identical sequence of decommissioned indices.
            ,"0123456789,9998", "0123456789,9999", "012345678,9999"
            ,"012345679,9999",  "1023456789,9999", "0123456798,9999"
        };
        const int NUM_SPEC = sizeof SPECS / sizeof *SPECS;

        for (int i = 0; i < NUM_SPEC; ++i) {
            const char *U_SPEC = SPECS[i];

            Obj mU(&objectAllocator);           const Obj& U = mU;
            gg(&mU, U_SPEC);

            if (veryVerbose) { T_ P_(U_SPEC) P(U) }

            for (int j = 0; j < NUM_SPEC; ++j) {
                const char *V_SPEC = SPECS[j];

                Obj mV(&objectAllocator);       const Obj& V = mV;
                gg(&mV, V_SPEC);

                if (veryVerbose) { T_ T_ P_(V_SPEC) P(V) }

                bool isSame = (i == j);
                LOOP2_ASSERT(U, V,  isSame == (U == V));
                LOOP2_ASSERT(U, V, !isSame == (U != V));
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   That the value of the object is written to the stream in the
        //   expected format (where the left most decommissioned index is
        //   dispensed first) on a single line with no trailing newline:
        //   ..
        //     [ decommissioned indices (next new index) ]
        //   ..
        //
        // Plan:
        //   Using the table driven technique, enumerate a sequence of test
        //   vectors, with each vector having a specification and an expected
        //   output.  For each test vector, construct an independent object
        //   'mX' and configure it using the generator function 'gg'.  Create
        //   an 'ostringstream' object, and use 'operator<<' to stream a
        //   constant reference to 'mX'.  Finally, compare the contents of the
        //   'ostringstream' object with the expected output.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream& output, const IndexClerk&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT (<<) OPERATOR" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' using table" << endl;

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // specification string
            const char *d_expected;  // expected output
        } DATA[] = {
            //LINE         SPEC            EXPECTED
            //----         ---------       -----------
            { L_,          ",0",           "[ (0) ]"          },  // depth 0

            { L_,          ",1",           "[ (1) ]"          },  // depth 1
            { L_,          "0,1",          "[ 0 (1) ]"        },

            { L_,          "0,2",          "[ 0 (2) ]"        },  // depth 2
            { L_,          ",2",           "[ (2) ]"          },
            { L_,          "0,2",          "[ 0 (2) ]"        },
            { L_,          "1,2",          "[ 1 (2) ]"        },
            { L_,          "01,2",         "[ 0 1 (2) ]"      },
            { L_,          "10,2",         "[ 1 0 (2) ]"      },

            { L_,          "135,6",        "[ 1 3 5 (6) ]"    },  // others
            { L_,          "153,12",       "[ 1 5 3 (12) ]"   },
            { L_,          "315,345",      "[ 3 1 5 (345) ]"  },
            { L_,          "531,6789",     "[ 5 3 1 (6789) ]" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int BUF_SIZE = 1000;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE     = DATA[ti].d_lineNum;
            const char *SPEC     = DATA[ti].d_spec;
            const char *EXPECTED = DATA[ti].d_expected;

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

            Obj mX(&objectAllocator);           const Obj& X = mX;
            gg(&mX, SPEC);

            char buf[BUF_SIZE];
            memset(buf, 0, sizeof(buf));
            ostringstream outbuf(bsl::string(buf, BUF_SIZE));

            bsls::Types::Int64 numBlocksTotal =
                                             defaultAllocator.numBlocksTotal();
            outbuf << X;
            ASSERT(numBlocksTotal == defaultAllocator.numBlocksTotal());

            if (veryVeryVerbose) { T_ T_ P_(EXPECTED) P(outbuf.str()) }
            LOOP3_ASSERT(LINE,
                         EXPECTED,
                         buf,
                         0 == bsl::strcmp(EXPECTED, outbuf.str().c_str()));
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        // Concerns:
        //   That, for each unique object value, the basic accessors return the
        //   correct attribute values.
        //
        // Plan:
        //   Using the table-driven technique, enumerate a sequence of
        //   specifications (in increasing order of next new index), and verify
        //   that each of the basic accessors returns the corresponding values.
        //
        //   Then, using the loop-based technique, configure the object
        //   with different values and verify each of the basic accessors
        //   returns the correct value.
        //
        // Testing:
        //   bdlc::IndexClerkIter begin() const;
        //   bdlc::IndexClerkIter end() const;
        //   int numCommissionedIndices() const;
        //   int numDecommissionedIndices() const;
        //   int nextNewIndex() const;
        //   bdlc::IndexClerkIter();
        //   bdlc::IndexClerkIter& operator++();
        //   int operator*() const;
        //   operator==(const IndexClerkIter&, const IndexClerkIter&);
        //   operator!=(const IndexClerkIter&, const IndexClerkIter&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\nTesting basic accessors using table." << endl;

        static const struct {
            int         d_lineNum;         // source line number
            const char *d_spec;            // specification string
            int         d_next;            // next new index
            int         d_numDecom;        // number of returned indices
            int         d_decomArray[10];  // decommissioned indices
        } DATA[] = {
            //LINE  SPEC       NEXT  NUMDECOM  DECOMARRAY
            //----  ---------  ----  --------  ----------
            { L_,   ",0",        0,     0,     { -1 }      },  // depth 0

            { L_,   ",1",        1,     0,     { -1 }      },  // depth 1
            { L_,   "0,1",       1,     1,     { 0 }       },

            { L_,   ",2",        2,     0,     { -1 }      },  // depth 2
            { L_,   "0,2",       2,     1,     { 0 }       },
            { L_,   "1,2",       2,     1,     { 1 }       },
            { L_,   "01,2",      2,     2,     { 0, 1 }    },
            { L_,   "10,2",      2,     2,     { 1, 0 }    },

            { L_,   ",3",        3,     0,     { -1 }      },  // depth 3
            { L_,   "0,3",       3,     1,     { 0 }       },
            { L_,   "1,3",       3,     1,     { 1 }       },
            { L_,   "2,3",       3,     1,     { 2 }       },
            { L_,   "01,3",      3,     2,     { 0, 1 }    },
            { L_,   "02,3",      3,     2,     { 0, 2 }    },
            { L_,   "12,3",      3,     2,     { 1, 2 }    },
            { L_,   "10,3",      3,     2,     { 1, 0 }    },
            { L_,   "20,3",      3,     2,     { 2, 0 }    },
            { L_,   "21,3",      3,     2,     { 2, 1 }    },
            { L_,   "012,3",     3,     3,     { 0, 1, 2 } },
            { L_,   "021,3",     3,     3,     { 0, 2, 1 } },
            { L_,   "120,3",     3,     3,     { 1, 2, 0 } },
            { L_,   "102,3",     3,     3,     { 1, 0, 2 } },
            { L_,   "201,3",     3,     3,     { 2, 0, 1 } },
            { L_,   "210,3",     3,     3,     { 2, 1, 0 } },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE       = DATA[ti].d_lineNum;
            const char *SPEC       = DATA[ti].d_spec;
            const int   NEXT       = DATA[ti].d_next;
            const int   NUMDECOM   = DATA[ti].d_numDecom;
            const int  *DECOMARRAY = DATA[ti].d_decomArray;

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P_(NEXT) P(NUMDECOM)
            }

            Obj mX(&objectAllocator);               const Obj& X = mX;
            gg(&mX, SPEC);

            LOOP_ASSERT(LINE, NEXT     == X.nextNewIndex());
            LOOP_ASSERT(LINE, NUMDECOM == X.numDecommissionedIndices());

            Iter it = X.begin();
            if (0 == NUMDECOM) {
                LOOP_ASSERT(LINE, X.end() == it);
            }
            else {
                LOOP_ASSERT(LINE, X.end() != it);
            }

            for (int j = 0; j < NUMDECOM; ++j) {
                if (veryVerbose) { T_ T_ P_(j) P_(DECOMARRAY[j]) P(*it) }
                LOOP2_ASSERT(LINE, j, X.end()       !=  it);
                LOOP2_ASSERT(LINE, j, DECOMARRAY[j] == *it);
                ++it;
            }
            LOOP_ASSERT(LINE, X.end() == it);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nLoop-based testing on basic accessors."
                          << endl;
        {
            const int MAX_TRIALS = 100;

            for (int sz = 0; sz < MAX_TRIALS; ++sz) {
                if (veryVerbose) { T_ P(sz) }

                Obj mX(&objectAllocator);           const Obj& X = mX;

                for (int i = 0; i < sz; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    mX.getIndex();
                    LOOP_ASSERT(i, i + 1 == X.nextNewIndex());
                }
                LOOP_ASSERT(sz, sz == X.nextNewIndex());

                for (int i = 0; i < sz; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }
                    LOOP2_ASSERT(sz, i, sz - i== X.numCommissionedIndices());
                    LOOP2_ASSERT(sz, i,      i== X.numDecommissionedIndices());

                    mX.putIndex(i);
                    Iter it = X.begin();
                    LOOP2_ASSERT(sz, i, X.end() != it);
                    for (int j = i; j >= 0; --j) {
                        if (veryVerbose) { T_ T_ T_ P(j) }
                        LOOP3_ASSERT(sz, i, j, X.end() !=  it);
                        LOOP3_ASSERT(sz, i, j, j       == *it);
                        ++it;
                    }
                    LOOP2_ASSERT(sz, i, X.end() == it);
                }
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION:
        //
        // Concerns:
        //   1a. That valid generator syntax produces expected results.
        //    b. That 'gg' returns a reference to the specified 'object'.
        //
        //   2a. That invalid syntax is detected and reported.
        //    b. That 'ggg' returns the index of the first invalid character in
        //       the specified 'spec'.
        //
        // Plan:
        //   Using the table-driven technique, use the primitive generator
        //   function 'gg' to set the state of a newly created object for
        //   a variety of different specs (see table).  Assert that the 'gg'
        //   function always returns a valid reference to the object, and
        //   verify the object's state (using basic accessors).
        //
        //   Also using the table-driven technique, test the 'ggg' function
        //   using invalid 'spec' values (see second table) to ensure that
        //   illegal specifications are caught, and the index corresponding
        //   to the first offending character is returned.
        //
        // Testing:
        //   bdlc::IndexClerk& gg(bdlc::IndexClerk *object, const char *spec);
        //   int ggg(bdlc::IndexClerk *object, const char *spec, int vF = 1);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING PRIMITIVE GENERATOR FUNCTION" << endl
                         << "====================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec;            // specification string
                int         d_next;            // next new index
                int         d_numDecom;        // number of returned indices
                int         d_decomArray[10];  // decommissioned indices
            } DATA[] = {
            //LINE   SPEC             NEXT    NUMDECOM    DECOMARRAY
            //----   --------------   ----    --------    -----------------

            // No decommissioned indices.
            { L_,          ",0",        0,         0,    { -1 }              },
            { L_,          ",1",        1,         0,    { -1 }              },
            { L_,          ",2",        2,         0,    { -1 }              },
            { L_,          ",3",        3,         0,    { -1 }              },

            // All indices returned.
            { L_,          ",0",        0,         0,    { -1 }              },
            { L_,         "0,1",        1,         1,    { 0 }               },
            { L_,        "01,2",        2,         2,    { 0, 1 }            },
            { L_,       "012,3",        3,         3,    { 0, 1, 2 }         },
            { L_,      "0123,4",        4,         4,    { 0, 1, 2, 3 }      },
            { L_,"0123456789,10",      10,        10,    { 0, 1, 2, 3, 4,
                                                              5, 6, 7, 8, 9 }},
            // All indices returned in reverse order.
            { L_,        "10,2",        2,         2,    { 1, 0 }            },
            { L_,       "210,3",        3,         3,    { 2, 1, 0 }         },
            { L_,      "3210,4",        4,         4,    { 3, 2, 1, 0 }      },
            { L_,     "43210,5",        5,         5,    { 4, 3, 2, 1, 0 }   },
            { L_,    "543210,6",        6,         6,    { 5, 4, 3, 2, 1, 0 }},
            { L_,"9876543210,10",      10,        10,    { 9, 8, 7, 6, 5,
                                                              4, 3, 2, 1, 0 }},
            // All indices returned in arbitrary order.
            { L_,     "02143,5",        5,         5,    { 0, 2, 1, 4, 3 }   },
            { L_,     "13204,5",        5,         5,    { 1, 3, 2, 0, 4 }   },
            { L_,     "41032,5",        5,         5,    { 4, 1, 0, 3, 2 }   },

            // Partial indices returned.
            { L_,        "01,9",        9,         2,    { 0, 1 }            },
            { L_,        "04,8",        8,         2,    { 0, 4 }            },
            { L_,       "123,7",        7,         3,    { 1, 2, 3 }         },
            { L_,       "431,6",        6,         3,    { 4, 3, 1 }         },
            { L_,      "3014,5",        5,         4,    { 3, 0, 1, 4 }      },

            // Leading 0 digit(s).
            { L_,          ",00",       0,         0,    { -1 }              },
            { L_,        "01,05",       5,         2,    { 0, 1 }            },
            { L_,       "031,06",       6,         3,    { 0, 3, 1 }         },
            { L_,      "0321,007",      7,         4,    { 0, 3, 2, 1 }      },

            // Large next new indices.
            { L_,       "79,30",       30,         2,    { 7, 9 }            },
            { L_,      "79,400",      400,         2,    { 7, 9 }            },
            { L_,     "79,5000",     5000,         2,    { 7, 9 }            },
            { L_,    "79,60000",    60000,         2,    { 7, 9 }            },
            { L_,   "79,700000",   700000,         2,    { 7, 9 }            },
            { L_,  "79,8000000",  8000000,         2,    { 7, 9 }            },
            { L_, "79,99999999", 99999999,         2,    { 7, 9 }            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_lineNum;
                const char *SPEC       = DATA[ti].d_spec;
                const int   NEXT       = DATA[ti].d_next;
                const int   NUMDECOM   = DATA[ti].d_numDecom;
                const int  *DECOMARRAY = DATA[ti].d_decomArray;

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P_(NEXT) P(NUMDECOM)
                }

                Obj mX(&objectAllocator);           const Obj& X = mX;

                if (veryVeryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(LINE, &mX == &gg(&mX, SPEC));

                LOOP_ASSERT(LINE, NEXT      == X.nextNewIndex());
                LOOP_ASSERT(LINE, NUMDECOM  == X.numDecommissionedIndices());

                Iter it = X.begin();
                if (0 == NUMDECOM) {
                    LOOP_ASSERT(LINE, X.end() == it);
                }
                else {
                    LOOP_ASSERT(LINE, X.end() != it);
                }

                for (int j = 0; j < NUMDECOM; ++j) {
                    if (veryVerbose) {
                        T_ T_ P_(j) P_(DECOMARRAY[j]) P(*it)
                    }
                    LOOP2_ASSERT(LINE, j, X.end() != it);
                    LOOP2_ASSERT(LINE, j, DECOMARRAY[j] == *it);
                    ++it;
                }
                LOOP_ASSERT(LINE, X.end() == it);
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // specification string
                int         d_ret;      // return value
            } DATA[] = {
                //LINE  SPEC             RET
                //----  ----             ---

                // illegal character
                { L_,   " ",              0     },
                { L_,   "0+",             1     },
                { L_,   "-0",             0     },
                { L_,   "01a",            2     },
                { L_,   "0.2",            1     },
                { L_,   "A12",            0     },

                // missing delim
                { L_,   "",               0     },
                { L_,   "123",            3     },

                // too many delimiters
                { L_,   ",0,",            2     },
                { L_,   ",1,",            2     },
                { L_,   ",10,2",          3     },
                { L_,   ",1,0,2",         2     },
                { L_,   ",,1",            1     },  // (missing next new index)
                { L_,   "0,,1",           2     },  // (missing next new index)

                // missing next new index
                { L_,   ",",              1     },
                { L_,   "0,",             2     },

                // incorrect next new index (first character not a digit)
                { L_,   ",a",             1     },
                { L_,   "0,+1",           2     },
                { L_,   "01,-2",          3     },

                // illegal trailing text
                { L_,   ",1a",            2     },
                { L_,   "0,2+",           3     },
                { L_,   "0,12-",          4     },

                // next new index <= returned index
                { L_,   "0,0",            2     },
                { L_,   "1,0",            2     },
                { L_,   "01,1",           3     },
                { L_,   "01,0",           3     },
                { L_,   "10,1",           3     },
                { L_,   "10,0",           3     },

                // duplicate returned index
                { L_,   "0023,4",         1     },
                { L_,   "0103,4",         2     },
                { L_,   "0121,4",         3     },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_spec;
                const int   RET  = DATA[ti].d_ret;

                if (veryVerbose) {
                    T_ P_(LINE) P_(SPEC) P(RET)
                }

                Obj mX(&objectAllocator);

                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP4_ASSERT(LINE, SPEC, RET, result, RET == result);
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //
        // Concerns:
        //   1. The default constructor creates a 'bdlc::IndexClerk' that will
        //      issues index 0 as the next index, and has no decommissioned
        //      indices.
        //   2. 'getIndex' returns the next newest index.
        //   3. 'putIndex' returns the commissioned index to the index clerk.
        //   4. Memory is provided by only the supplied allocator, or if none
        //      is supplied, from only the default allocator.
        //
        // Plan:
        //   a. Using brute force, enumerate a sequence of independent tests,
        //      ordered by increasing next new index value of
        //      'bdlc::IndexClerk' object:
        //
        //                Decommissioned  Next new
        //                indices         index
        //                -------------   ---------
        //                [                (0) ]  // depth 0
        //
        //                [                (1) ]  // depth 1
        //                [ 0              (1) ]
        //
        //                [                (2) ]  // depth 2
        //                [ 0              (2) ]
        //                [ 1              (2) ]
        //                [ 0 1            (2) ]
        //                [ 1 0            (2) ]
        //
        //                [                (3) ]  // depth 3
        //                [ 0              (3) ]
        //                [ 1              (3) ]
        //                [ 2              (3) ]
        //                [ 0 1            (3) ]
        //                [ 0 2            (3) ]
        //                [ 1 0            (3) ]
        //                [ 1 2            (3) ]
        //                [ 2 0            (3) ]
        //                [ 2 1            (3) ]
        //                [ 0 1 2          (3) ]
        //                [ 0 2 1          (3) ]
        //                [ 1 0 2          (3) ]
        //                [ 1 2 0          (3) ]
        //                [ 2 0 1          (3) ]
        //                [ 2 1 0          (3) ]
        //                  ^
        //                  |_ first decommissioned index to reuse
        //
        //   b. In each test, use the default constructor to create an
        //      empty object with the test allocator, and then use the primary
        //      manipulator functions 'getIndex' and 'putIndex' to modify the
        //      value of the object to the required state.
        //
        //   c. Verify the correctness of the above functions using the basic
        //      accessors 'numCommissionedIndices', 'numDecommissionIndices',
        //      'nextNewIndex', and the iterator, 'bdlc::IndexClerkIter'.
        //
        //   d. The destructor is exercised on each configuration as the object
        //      under test leaves scope.
        //
        //   e. After each block, assert that there is no outstanding memory
        //      usage from the object allocator, and no usage of either the
        //      default or global allocators.
        //
        //   f. Then, using the loop-based technique, interleave 'getIndex' and
        //      'putIndex' calls, and verify that the value returned by
        //      'getIndex' and also (using the basic accessors) that the
        //      internal state of the 'bdlc::IndexClerk' is correct.
        //
        //   g. Finally, default construct a 'bdlc::IndexClerk', invoke
        //      'getIndex' and 'putIndex' on the index clerk, then verify that
        //      memory is supplied by only the default allocator.
        //
        // Testing:
        //   bdlc::IndexClerk(bslma::Allocator *ba);
        //   ~bdlc::IndexClerk();
        //   int getIndex(void);
        //   void putIndex(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATORS" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            // mC = [ (0) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(0 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting 'getIndex' and 'putIndex'." << endl;

        if (verbose) cout << "\tTesting when nextNewIndex == 1." << endl;
        {
            // mC = [ (1) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(1 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(0 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // mC = [ 0 (1) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(1 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\tTesting when nextNewIndex == 2." << endl;
        {
            // mC = [ (2) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(2 == C.nextNewIndex());
            ASSERT(2 == C.numCommissionedIndices());
            ASSERT(0 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // mC = [ 0 (2) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(2 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 (2) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(2 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 1 (2) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            mC.putIndex(1);
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(2 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 0 (2) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            mC.putIndex(0);
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(2 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\tTesting when nextNewIndex == 3." << endl;
        {
            // c = [ (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(3 == C.numCommissionedIndices());
            ASSERT(0 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() == it);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(2 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(2 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 2 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(2);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(2 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 1 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(1);
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 2 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(2);
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 0 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(0);
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 2 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(2);
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 2 0 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(0);
            mC.putIndex(2);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 2 1 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(1);
            mC.putIndex(2);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(1 == C.numCommissionedIndices());
            ASSERT(2 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 1 2 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(2);
            mC.putIndex(1);
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 0 2 1 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(1);
            mC.putIndex(2);
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 0 2 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(2);
            mC.putIndex(0);
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 1 2 0 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(0);
            mC.putIndex(2);
            mC.putIndex(1);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 2 0 1 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(1);
            mC.putIndex(0);
            mC.putIndex(2);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        {
            // c = [ 2 1 0 (3) ]
            Obj mC(&objectAllocator);           const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(2 == mC.getIndex());
            mC.putIndex(0);
            mC.putIndex(1);
            mC.putIndex(2);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(3 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(3 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(2 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(1 == *it); ++it;             ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(2 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
            ASSERT(0 == mC.getIndex());
            ASSERT(3 == mC.getIndex());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout <<
            "\nLoop-based test on 'getIndex' and 'putIndex'." << endl;
        {

            // Now, using the loop-based technique, construct a
            // 'bdlc::IndexClerk' in each iteration and put it in a state where
            // the value returned by 'nextNewIndex' equals to the loop index.
            // This defines the baseline state of the 'bdlc::IndexClerk' in
            // that iteration.

            const int MAX_TRIALS = 10;

            const int CO_PRIME   = 11;  // this number has to be coprime with
                                        // 'MAX_TRAILS' to properly execute the
                                        // last inner loop below.

            for (int size = 0; size < MAX_TRIALS; ++size) {
                if (veryVerbose) { T_ P(size) }

                Obj mC(&objectAllocator);       const Obj& C = mC;

                // Set the state of 'bdlc::IndexClerk' to the baseline state by
                // constantly calling 'getIndex' until the value returned by
                // 'nextNewIndex' equals to the loop index.

                if (veryVerbose) { T_ T_ cout << "Baseline:" << endl; }

                for (int i = 0; i < size; ++i) {
                    if (veryVerbose) { T_ T_ T_ P(i) }

                    LOOP2_ASSERT(size, i, 0 == C.numDecommissionedIndices());
                    LOOP2_ASSERT(size, i, i == C.numCommissionedIndices());
                    LOOP2_ASSERT(size, i, i == C.nextNewIndex());

                    LOOP2_ASSERT(size, i, i == mC.getIndex());
                }

                LOOP_ASSERT(size, size == C.nextNewIndex());

                // Finally, interleave calls to the 'putIndex' and 'getIndex()'
                // methods.  To ensure that indices are not returned
                // sequentially, we use 'i * CO_PRIME % size' as the next index
                // to return.  Furthermore, to ensure that the same index is
                // not placed back into the index clerk, we have to use a
                // number that is coprime to 'size'.  The easiest way to
                // achieve this is to use the next prime number greater than
                // 'MAX_TRAILS'.

                if (veryVerbose) {
                    T_ T_
                    cout << "Interleave 'putIndex' and 'getIndex' method calls"
                         << endl;
                }

                for (int i = 0; i < size; ++i) {
                    int idx = i * CO_PRIME % size;

                    if (veryVerbose) { T_ T_ T_ P_(i) P_(idx) P(mC) }

                    LOOP_ASSERT(size,size - i == C.numCommissionedIndices());
                    LOOP_ASSERT(size,       i == C.numDecommissionedIndices());

                    mC.putIndex(idx);
                    LOOP2_ASSERT(size, i, idx == mC.getIndex());
                    mC.putIndex(idx);
                }
            }
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting default 'bslma::Allocator' argument."
                          << endl;

        int currentTestAllocatorUsage = objectAllocator.numBlocksTotal();
        {
            Obj mC;                             const Obj& C = mC;
            ASSERT(0 == mC.getIndex());
            mC.putIndex(0);

            if (veryVerbose) { T_ T_ P(C) }
            ASSERT(1 == C.nextNewIndex());
            ASSERT(0 == C.numCommissionedIndices());
            ASSERT(1 == C.numDecommissionedIndices());

            Iter it = C.begin();                ASSERT(C.end() != it);
            ASSERT(0 == *it); ++it;             ASSERT(C.end() == it);

            ASSERT(0 == mC.getIndex());
            ASSERT(1 == mC.getIndex());
        }
        ASSERT(currentTestAllocatorUsage == objectAllocator.numBlocksTotal());
        ASSERT(0                         == defaultAllocator.numBlocksInUse());
        ASSERT(0                         != defaultAllocator.numBlocksTotal());
        ASSERT(0                         == globalAllocator.numBlocksTotal());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'update' and 'reset'.
        //
        // Plan:
        //   Create three test objects using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [8, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'vA', 'vB' and 'vC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but as yet "Unperturbed" default object value.
        //
        //    1. Create an object x1 (default ctor) { x1:U              }
        //    2. Create an object x2 (copy of x1)   { x1:U  x2:U        }
        //    3. Set X1 using 'getIndex'            { x1:vA x2:U        }
        //    4. Create an object x3 (copy of x1)   { x1:vA x2:U  x3:vA }
        //    5. Set X3 using 'getIndex'            { x1:vA x2:U  x3:vB }
        //    6. Change x1 using 'removeAll'        { x1:U  x2:U  x3:vB }
        //    7. Change x1 using 'getIndex'         { x1:vA x2:U  x3:vB }
        //    8. Assign x2 = x1                     { x1:vA x2:vA x3:vB }
        //    9. Assign x2 = x3                     { x1:vA x2:vB x3:vB }
        //   10. Assign x1 = x1 (aliasing)          { x1:vA x2:vB x3:vB }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor)."
                          << "\t\t{ x1:U              }" << endl;

        Obj mX1(&objectAllocator);                   const Obj& X1 = mX1;
        if (verbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;

        ASSERT(0 == X1.nextNewIndex());
        ASSERT(0 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1).  "
                          << "\t\t{ x1:U  x2:U        }" << endl;

        Obj mX2(X1, &objectAllocator);               const Obj& X2 = mX2;
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;

        ASSERT(0 == X2.nextNewIndex());
        ASSERT(0 == X2.numCommissionedIndices());
        ASSERT(0 == X2.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                          << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set X1 using 'getIndex'.           "
                             "\t\t{ x1:vA x2:U        }" << endl;

        int idx = mX1.getIndex();
        if (verbose) { T_ P_(idx) P(X1) }
        ASSERT(0 == idx);

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.nextNewIndex());
        ASSERT(1 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                          << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x3 (copy of x1)   "
                             "\t\t{ x1:vA x2:U  x3:vA }" << endl;

        Obj mX3(X1, &objectAllocator);               const Obj& X3 = mX3;
        if (verbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(1 == X1.nextNewIndex());
        ASSERT(1 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X3 == X1));        ASSERT(0 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set X3 using 'getIndex'            "
                             "\t\t{ x1:vA x2:U  x3:vB }" << endl;

        idx = mX3.getIndex();
        if (verbose) { T_ P_(idx) P(X3) }
        ASSERT(1 == idx);

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(2 == X3.nextNewIndex());
        ASSERT(2 == X3.numCommissionedIndices());
        ASSERT(0 == X3.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'removeAll'        "
                             "\t\t{ x1:U  x2:U  x3:vB }" << endl;

        mX1.removeAll();
        if (verbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(0 == X1.nextNewIndex());
        ASSERT(0 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 using 'getIndex'         "
                             "\t\t{ x1:vA x2:U  x3:vB }" << endl;

        idx = mX1.getIndex();
        if (verbose) { T_ P_(idx) P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.nextNewIndex());
        ASSERT(1 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1.                    "
                             "\t\t{ x1:vA x2:vA x3:vB }" << endl;

        mX2 = mX1;
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(1 == X2.nextNewIndex());
        ASSERT(1 == X2.numCommissionedIndices());
        ASSERT(0 == X2.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3.                    "
                             "\t\t{ x1:vA x2:vB x3:vB }" << endl;

        mX2 = mX3;
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;

        ASSERT(2 == X2.nextNewIndex());
        ASSERT(2 == X2.numCommissionedIndices());
        ASSERT(0 == X2.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x2 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x1 = x1.                    "
                             "\t\t{ x1:vA x2:vB x3:vB }" << endl;

        mX1 = mX1;
        if (verbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        ASSERT(1 == X1.nextNewIndex());
        ASSERT(1 == X1.numCommissionedIndices());
        ASSERT(0 == X1.numDecommissionedIndices());

        if (verbose) cout << "\tb. Try equality operators: "
                             "x1 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));

        }
        ASSERT(0 == objectAllocator.numBlocksInUse());
        ASSERT(safe || 0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

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
