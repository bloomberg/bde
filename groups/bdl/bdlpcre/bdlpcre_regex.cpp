// bdlpcre_regex.cpp                                                  -*-C++-*-
#include <bdlpcre_regex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpcre_regex_cpp,"$Id$ $CSID$")
//
///IMPLEMENTATION NOTES
///--------------------
// This component depends on the open-source Perl Compatible Regular
// Expressions (PCRE) library (http://www.pcre.org).
//
// The PCRE library can be configured with several options.  The complete list
// of options is documented at:
// http://www.pcre.org/pcre.txt (under 'PCRE BUILD-TIME OPTIONS')
//
// The PCRE library used by this component was configured with UTF8 support.
// It was built using the following commands:
//..
//  $ configure --enable-utf8
//  $ make
//..
// If successful, the library files are generated in a '.libs/' sub-directory.

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>    // bsl::memset
#include <bsl_string.h>
#include <bsl_utility.h>    // bsl::pair
#include <bsl_vector.h>

#include <pcre.h>

namespace {

void initializePcreExtra(struct BloombergLP::pcre_extra *extra, int depthLimit)
    // Initialize the 'pcre_extra' struct pointed to by the specified 'extra'
    // to specify 'depthLimit' as the maximum match limit.
{
    bsl::memset(extra, 0, sizeof(struct BloombergLP::pcre_extra));
    extra->flags       = PCRE_EXTRA_MATCH_LIMIT;
    extra->match_limit = depthLimit;
}

}  // close unnamed namespace

namespace BloombergLP {

// CONSTANTS

enum {
    BDEPCRE_SUCCESS           =  0,
    BDEPCRE_DEPTHLIMITFAILURE =  1,
    BDEPCRE_FAILURE           = -1
};
    // Return values for this API.

const int NUM_INTS_PER_CAPTURED_STRING = 3;
    // Number of integers required by PCRE for each captured string.

                             // -------------------
                             // class bdlpcre::RegEx
                             // -------------------

// CLASS DATA

bsls::AtomicOperations::AtomicTypes::Int
        bdlpcre::RegEx::s_depthLimit = {10000000}; // from pcre's config.h

namespace bdlpcre {                                                  // MATCH_LIMIT value

// CREATORS

RegEx::RegEx(bslma::Allocator *basicAllocator)
: d_flags(0)
, d_pattern(basicAllocator)
, d_pcre_p(0)
, d_depthLimit(RegEx::defaultDepthLimit())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS

void RegEx::clear()
{
    if (isPrepared()) {
        d_allocator_p->deallocate(d_pcre_p);
        d_pcre_p = 0;
        d_flags  = 0;
        d_pattern.clear();
    }
}

int RegEx::prepare(bsl::string *errorMessage,
                           int         *errorOffset,
                           const char  *pattern,
                           int          flags)
{
    BSLS_ASSERT(pattern);

    // Free resources currently used by this object, if any, and put the object
    // into the "unprepared" state.

    clear();

    // Set these data members here, before compiling the pattern.  In case of
    // an exception (assigning to 'd_pattern' or compiling the pattern), the
    // object will remain in the "unprepared" state.

    d_flags   = flags;
    d_pattern = pattern;

    // Compile the new pattern.

    const char *errorMessageFromPcre = 0;
    int         errorOffsetFromPcre;

    pcre *pcreObject = pcre_compile(pattern,
                                    flags,
                                    &errorMessageFromPcre,
                                    &errorOffsetFromPcre,
                                    0,         // use PCRE's default char table
                                    d_allocator_p);

    if (0 == pcreObject) {
        if (errorMessage) {
            BSLS_ASSERT(errorMessageFromPcre);
            *errorMessage = errorMessageFromPcre;
        }

        if (errorOffset) {
            *errorOffset = errorOffsetFromPcre;
        }

        return BDEPCRE_FAILURE;                                       // RETURN
    }

    // Set object to the "prepared" state.

    d_pcre_p  = reinterpret_cast<Pcre*>(pcreObject);

    return BDEPCRE_SUCCESS;
}

// ACCESSORS

int RegEx::match(const char *subject,
                         int         subjectLength,
                         int         subjectOffset) const
{
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(0 <= subjectLength);
    BSLS_ASSERT(0 <= subjectOffset);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    struct pcre_extra extra;
    initializePcreExtra(&extra, d_depthLimit);

    int returnValue = pcre_exec(reinterpret_cast<pcre*>(d_pcre_p),
                                &extra,
                                subject ? subject : "",
                                subjectLength,
                                subjectOffset,
                                0,
                                0,
                                0,
                                d_allocator_p);

    if (PCRE_ERROR_MATCHLIMIT == returnValue) {
        return BDEPCRE_DEPTHLIMITFAILURE;                             // RETURN
    }
    else if (0 > returnValue) {
        return BDEPCRE_FAILURE;                                       // RETURN
    }

    return BDEPCRE_SUCCESS;
}

int RegEx::match(bsl::pair<int, int> *result,
                         const char          *subject,
                         int                  subjectLength,
                         int                  subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(0 <= subjectLength);
    BSLS_ASSERT(0 <= subjectOffset);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    int outputVector[NUM_INTS_PER_CAPTURED_STRING];

    struct pcre_extra extra;
    initializePcreExtra(&extra, d_depthLimit);

    int returnValue = pcre_exec(reinterpret_cast<pcre*>(d_pcre_p),
                                &extra,
                                subject ? subject : "",
                                subjectLength,
                                subjectOffset,
                                0,
                                outputVector,
                                NUM_INTS_PER_CAPTURED_STRING,
                                d_allocator_p);

    if (PCRE_ERROR_MATCHLIMIT == returnValue) {
        return BDEPCRE_DEPTHLIMITFAILURE;                             // RETURN
    }
    else if (0 > returnValue) {
        return BDEPCRE_FAILURE;                                       // RETURN
    }

    int offset = outputVector[0];
    int length = outputVector[1] - offset;
    *result = bsl::make_pair(offset, length);

    return BDEPCRE_SUCCESS;
}

int
RegEx::match(bsl::vector<bsl::pair<int, int> > *result,
                     const char                        *subject,
                     int                                subjectLength,
                     int                                subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(0 <= subjectLength);
    BSLS_ASSERT(0 <= subjectOffset);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    const int len = numSubpatterns() + 1;

    int *outputVector = (int*)d_allocator_p->allocate(
                             sizeof(int) * len * NUM_INTS_PER_CAPTURED_STRING);
    BSLS_ASSERT(outputVector);

    // Let a proctor manage 'outputVector' (to ensure exception safety).

    bslma::DeallocatorProctor<bslma::Allocator> proctor(outputVector,
                                                        d_allocator_p);

    struct pcre_extra extra;
    initializePcreExtra(&extra, d_depthLimit);

    int returnValue = pcre_exec(reinterpret_cast<pcre*>(d_pcre_p),
                                &extra,
                                subject ? subject : "",
                                subjectLength,
                                subjectOffset,
                                0,
                                outputVector,
                                len * NUM_INTS_PER_CAPTURED_STRING,
                                d_allocator_p);

    if (PCRE_ERROR_MATCHLIMIT == returnValue) {
        return BDEPCRE_DEPTHLIMITFAILURE;                             // RETURN
    }
    else if (0 > returnValue) {
        return BDEPCRE_FAILURE;                                       // RETURN
    }

    result->resize(len);

    for (int i = 0, j = 0; i < len; ++i, j += 2) {
        int offset = outputVector[j];
        int length = outputVector[j + 1] - offset;
        (*result)[i] = bsl::make_pair(offset, length);
    }

    return BDEPCRE_SUCCESS;
}

int RegEx::numSubpatterns() const
{
    BSLS_ASSERT(isPrepared());

    int numSubpatterns;
    int returnValue = pcre_fullinfo(reinterpret_cast<pcre*>(d_pcre_p),
                                    0,
                                    PCRE_INFO_CAPTURECOUNT,
                                    &numSubpatterns);
    (void)returnValue;

    BSLS_ASSERT(0 == returnValue);

    return numSubpatterns;
}

int RegEx::subpatternIndex(const char *name) const
{
    BSLS_ASSERT(isPrepared());

    const int index = pcre_get_stringnumber(reinterpret_cast<pcre*>(d_pcre_p),
                                            name);

    return 0 < index && index <= numSubpatterns() ? index : -1;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
