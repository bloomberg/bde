// bdlpcre_regex.cpp                                                  -*-C++-*-
#include <bdlpcre_regex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpcre2_regex_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// This component depends on the open-source Perl Compatible Regular
// Expressions (PCRE2) library (http://www.pcre.org).
//
// The PCRE2 library used by this component was configured with UTF8 support.

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>    // bsl::memset
#include <bsl_string.h>
#include <bsl_utility.h>    // bsl::pair
#include <bsl_vector.h>
#include <bsl_iostream.h>

extern "C" {

void *bdlpcre_malloc(size_t size, void* context)
{
    void *result = 0;

    BloombergLP::bslma::Allocator *basicAllocator =
                     reinterpret_cast<BloombergLP::bslma::Allocator*>(context);
    try {
        result = basicAllocator->allocate(size);
    } catch ( ... ) {
    }
    return result;
}

void bdlpcre_free(void* data, void* context)
{
    BloombergLP::bslma::Allocator *basicAllocator =
                     reinterpret_cast<BloombergLP::bslma::Allocator*>(context);

    basicAllocator->deallocate(data);
    return;
}

}  // close extern "C"

namespace BloombergLP {

namespace bdlpcre {

// CONSTANTS

enum {
    k_SUCCESS           =  0,
    k_DEPTHLIMITFAILURE =  1,
    k_FAILURE           = -1
};
    // Return values for this API.

                             // -----------
                             // class RegEx
                             // -----------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Int RegEx::s_depthLimit = {10000000};

// CREATORS
RegEx::RegEx(bslma::Allocator *basicAllocator)
: d_flags(0)
, d_pattern(basicAllocator)
, d_pcre2Context_p(0)
, d_matchContext_p(0)
, d_pcre2Code_p(0)
, d_depthLimit(RegEx::defaultDepthLimit())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_pcre2Context_p = pcre2_general_context_create(
                                            &bdlpcre_malloc,
                                            &bdlpcre_free,
                                            static_cast<void*>(d_allocator_p));
    BSLS_ASSERT(0 != d_pcre2Context_p);

    // Pre-create match context
    d_matchContext_p = pcre2_match_context_create(d_pcre2Context_p);
    BSLS_ASSERT(0 != d_matchContext_p);

    pcre2_set_match_limit(d_matchContext_p, d_depthLimit);
}

// MANIPULATORS
void RegEx::clear()
{
    if (isPrepared()) {
        pcre2_code_free(d_pcre2Code_p);
        d_pcre2Code_p = 0;
        d_flags       = 0;
        d_pattern.clear();
    }
}

int RegEx::prepare(bsl::string *errorMessage,
                   size_t      *errorOffset,
                   const char  *pattern,
                   int          flags)
{
    BSLS_ASSERT(pattern);

    // Free resources currently used by this object, if any, and put the object
    // into the "unprepared" state.

    clear();

    d_flags        = flags;
    d_pattern      = pattern;

    // Compile the new pattern.

    int    errorCodeFromPcre2;
    size_t errorOffsetFromPcre2;

    pcre2_compile_context *compileContext = pcre2_compile_context_create(
                                                             d_pcre2Context_p);

    if (0 == compileContext) {
        return k_FAILURE;                                             // RETURN
    }

    pcre2_code *pcre2Code = pcre2_compile(
                               reinterpret_cast<const unsigned char*>(pattern),
                               PCRE2_ZERO_TERMINATED,
                               flags,
                               &errorCodeFromPcre2,
                               &errorOffsetFromPcre2,
                               compileContext);

    pcre2_compile_context_free(compileContext);

    if (0 == pcre2Code) {
        if (errorMessage) {
            unsigned char errorBuffer[256];

            int result = pcre2_get_error_message(errorCodeFromPcre2,
                                                 errorBuffer,
                                                 256);
            if (result > 0) {
                errorMessage->assign(
                                reinterpret_cast<const char*>(&errorBuffer[0]),
                                result);
            } else {
                errorMessage->assign("");
            }
        }

        if (errorOffset) {
            *errorOffset = errorOffsetFromPcre2;
        }

        return k_FAILURE;                                             // RETURN
    }

    // Set the data members and set the object to the "prepared" state.
    d_flags        = flags;
    d_pattern      = pattern;
    d_pcre2Code_p  = pcre2Code;

    return k_SUCCESS;
}

// ACCESSORS
int RegEx::match(const char *subject,
                 size_t      subjectLength,
                 size_t      subjectOffset) const
{
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    int result = k_SUCCESS;

    pcre2_match_data *matchData = pcre2_match_data_create_from_pattern(
                                                             d_pcre2Code_p, 0);

    if (0 == matchData) {
        return k_FAILURE;                                             // RETURN
    }

    const unsigned char* actualSubject =
                reinterpret_cast<const unsigned char*>(subject ? subject : "");

    int returnValue = pcre2_match(d_pcre2Code_p,
                                  actualSubject,
                                  subjectLength,
                                  subjectOffset,
                                  0,
                                  matchData,
                                  d_matchContext_p);

    if (PCRE2_ERROR_MATCHLIMIT == returnValue) {
        result = k_DEPTHLIMITFAILURE;
    } else if (0 > returnValue) {
        result = k_FAILURE;
    }

    pcre2_match_data_free(matchData);

    return result;
}

int RegEx::match(bsl::pair<size_t, size_t> *result,
                 const char                *subject,
                 size_t                     subjectLength,
                 size_t                     subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    pcre2_match_data *matchData = pcre2_match_data_create_from_pattern(
                                                            d_pcre2Code_p, 0);

    if (0 == matchData) {
        return k_FAILURE;                                             // RETURN
    }

    const unsigned char* actualSubject =
                reinterpret_cast<const unsigned char*>(subject ? subject : "");

    int returnValue = pcre2_match(d_pcre2Code_p,
                                  actualSubject,
                                  subjectLength,
                                  subjectOffset,
                                  0,
                                  matchData,
                                  d_matchContext_p);

    if (PCRE2_ERROR_MATCHLIMIT == returnValue) {
        pcre2_match_data_free(matchData);
        return k_DEPTHLIMITFAILURE;                                   // RETURN
    } else if (0 > returnValue) {
        pcre2_match_data_free(matchData);
        return k_FAILURE;                                             // RETURN
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);

    BSLS_ASSERT(1 <= ovectorCount);

    size_t offset = ovector[0];
    size_t length = ovector[1] - offset;
    *result = bsl::make_pair(offset, length);

    pcre2_match_data_free(matchData);

    return k_SUCCESS;
}

int
RegEx::match(bsl::vector<bsl::pair<size_t, size_t> > *result,
             const char                              *subject,
             size_t                                   subjectLength,
             size_t                                   subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    pcre2_match_data *matchData = pcre2_match_data_create_from_pattern(
                                                            d_pcre2Code_p, 0);

    if (0 == matchData) {
        return k_FAILURE;                                             // RETURN
    }

    const unsigned char* actualSubject =
                reinterpret_cast<const unsigned char*>(subject ? subject : "");

    int returnValue = pcre2_match(d_pcre2Code_p,
                                  actualSubject,
                                  subjectLength,
                                  subjectOffset,
                                  0,
                                  matchData,
                                  d_matchContext_p);

    if (PCRE2_ERROR_MATCHLIMIT == returnValue) {
        pcre2_match_data_free(matchData);
        return k_DEPTHLIMITFAILURE;                                   // RETURN
    } else if (0 > returnValue) {
        pcre2_match_data_free(matchData);
        return k_FAILURE;                                             // RETURN
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);

    result->resize(ovectorCount);

    for (size_t i = 0, j = 0; i < ovectorCount; ++i, j += 2) {
        size_t offset = ovector[j];
        size_t length = ovector[j + 1] - offset;
        (*result)[i] = bsl::make_pair(offset, length);
    }

    pcre2_match_data_free(matchData);

    return k_SUCCESS;
}

int RegEx::numSubpatterns() const
{
    BSLS_ASSERT(isPrepared());

    int numSubpatterns;
    int returnValue = pcre2_pattern_info(d_pcre2Code_p,
                                         PCRE2_INFO_CAPTURECOUNT,
                                         &numSubpatterns);
    (void)returnValue;

    BSLS_ASSERT(0 == returnValue);

    return numSubpatterns;
}

int RegEx::subpatternIndex(const char *name) const
{
    BSLS_ASSERT(isPrepared());

    const int index = pcre2_substring_number_from_name(
                                 d_pcre2Code_p,
                                 reinterpret_cast<const unsigned char*>(name));

    return 0 < index && index <= numSubpatterns() ? index : -1;
}

}  // close package namespace

}  // close enterprise namespace

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
