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
//
// To provide more sophisticated match buffer management, the match context
// helper should be extracted into external match factory component that
// encapsulates all thread logic.
// 'bdlpcre::RegEx' component currently implements the following strategy for
// allocating/deallocating buffers used for pattern matching:
//:
//: o The match context for the main thread (the thread used to call 'prepare')
//:   is pre-allocated when the pattern is compiled and re-used by 'match'
//:   method unless it is called from a different thread.
//:
//: o Match contexts for all other threads are allocated and deallocated within
//:   each call to 'match' (i.e. when the 'match' method is invoked from
//:   other thread(s)).

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>

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
    BSLS_TRY {
        result = basicAllocator->allocate(size);
    } BSLS_CATCH( ... ) {
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

namespace {

static const bool k_IS_JIT_SUPPORTED =
#if defined(BSLS_PLATFORM_CPU_SPARC) && defined (BSLS_PLATFORM_CPU_64_BIT)
false;
#else
true;
#endif

}  // close unnamed namespace

enum {
    k_SUCCESS              =  0,
    k_DEPTHLIMITFAILURE    =  1,
    k_JITSTACKLIMITFAILURE =  2,
    k_FAILURE              = -1
};
    // Return values for this API.

                        // ==================
                        // MatchContextHelper
                        // ==================

struct MatchContextHelper {
    // This 'struct' provides a namespace for a set of functions that
    // allocate and deallocate match context buffers used by 'RegEx'.
    //
  public:
    static int allocateMatchContext(RegEx_MatchContext    *matchContext,
                                    pcre2_general_context *pcre2Context,
                                    pcre2_code            *patternCode,
                                    int                    depthLimit,
                                    size_t                 jitStackSize);
        // Allocate buffers needed by PCRE2 library to do the match for the
        // specified 'pcre2Context', 'patternCode', 'depthLimit', and
        // 'jitStackSize' and load the specified 'matchContext' with the
        // pointers to the match buffers.

    static void deallocateMatchContext(RegEx_MatchContext *matchContext);
        // Deallocate the match buffers pointed by the specified
        // 'matchContext'.

    static void resetMatchContext(RegEx_MatchContext *matchContext);
        // Reset all pointers in the specified 'matchContext'.
};

                        // ------------------
                        // MatchContextHelper
                        // ------------------

int MatchContextHelper::allocateMatchContext(
                                           RegEx_MatchContext    *matchContext,
                                           pcre2_general_context *pcre2Context,
                                           pcre2_code            *patternCode,
                                           int                    depthLimit,
                                           size_t                 jitStackSize)
{
    BSLS_ASSERT(matchContext);
    BSLS_ASSERT(pcre2Context);
    BSLS_ASSERT(patternCode);

    // Match Data
    pcre2_match_data *matchData_p =
                          pcre2_match_data_create_from_pattern(patternCode, 0);

    if (0 == matchData_p) {
        return k_FAILURE;                                             // RETURN
    }

    // Match context
    pcre2_match_context *matchContext_p =
                                      pcre2_match_context_create(pcre2Context);

    if (0 == matchContext_p) {
        pcre2_match_data_free(matchData_p);
        return k_FAILURE;                                             // RETURN
    }

    pcre2_set_match_limit(matchContext_p, depthLimit);

    // Jit stack
    pcre2_jit_stack *jitStack_p = 0;
    if (jitStackSize) {
        jitStack_p =
              pcre2_jit_stack_create(jitStackSize, jitStackSize, pcre2Context);
        if (0 == jitStack_p) {
            pcre2_match_context_free(matchContext_p);
            pcre2_match_data_free(matchData_p);
            return k_FAILURE;                                         // RETURN
        }
        pcre2_jit_stack_assign(matchContext_p, 0, jitStack_p);
    }

    matchContext->d_matchData_p    = matchData_p;
    matchContext->d_matchContext_p = matchContext_p;
    matchContext->d_jitStack_p     = jitStack_p;

    return k_SUCCESS;
}

void MatchContextHelper::deallocateMatchContext(
                                              RegEx_MatchContext *matchContext)
{
    BSLS_ASSERT(matchContext);

    pcre2_match_data_free(matchContext->d_matchData_p);
    pcre2_jit_stack_free(matchContext->d_jitStack_p);
    pcre2_match_context_free(matchContext->d_matchContext_p);
}

void MatchContextHelper::resetMatchContext(RegEx_MatchContext *matchContext)
{
    BSLS_ASSERT(matchContext);

    matchContext->d_matchData_p    = 0;
    matchContext->d_matchContext_p = 0;
    matchContext->d_jitStack_p     = 0;
}

                             // -----------
                             // class RegEx
                             // -----------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Int RegEx::s_depthLimit = {10000000};

// PRIVATE ACCESSORS
int RegEx::loadMatchContext(RegEx_MatchContext *matchContext) const
{
    BSLS_ASSERT(matchContext);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        *matchContext = d_mainMatchContext;
        return k_SUCCESS;                                             // RETURN
    }

    return MatchContextHelper::allocateMatchContext(matchContext,
                                                    d_pcre2Context_p,
                                                    d_patternCode_p,
                                                    d_depthLimit,
                                                    d_jitStackSize);
}

void RegEx::unloadMatchContext(RegEx_MatchContext *matchContext) const
{
    BSLS_ASSERT(matchContext);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        return;                                                       // RETURN
    }

    MatchContextHelper::deallocateMatchContext(matchContext);
}

int RegEx::privateMatch(const char         *subject,
                        size_t              subjectLength,
                        size_t              subjectOffset,
                        bool                skipValidation,
                        RegEx_MatchContext *matchContext) const
{
    int result = k_SUCCESS;

    const unsigned char* actualSubject =
                reinterpret_cast<const unsigned char*>(subject ? subject : "");

    int returnValue;

    if (skipValidation) {
        if (d_flags & k_FLAG_JIT && isJitAvailable()) {
            returnValue = pcre2_jit_match(d_patternCode_p,
                                          actualSubject,
                                          subjectLength,
                                          subjectOffset,
                                          0,
                                          matchContext->d_matchData_p,
                                          matchContext->d_matchContext_p);
        } else {
            returnValue = pcre2_match(d_patternCode_p,
                                      actualSubject,
                                      subjectLength,
                                      subjectOffset,
                                      PCRE2_NO_UTF_CHECK,
                                      matchContext->d_matchData_p,
                                      matchContext->d_matchContext_p);
        }

    } else {
        returnValue = pcre2_match(d_patternCode_p,
                          actualSubject,
                          subjectLength,
                          subjectOffset,
                          0,
                          matchContext->d_matchData_p,
                          matchContext->d_matchContext_p);
    }

    if (PCRE2_ERROR_MATCHLIMIT == returnValue) {
        result = k_DEPTHLIMITFAILURE;
    } else if (PCRE2_ERROR_JIT_STACKLIMIT == returnValue) {
        result = k_JITSTACKLIMITFAILURE;
    } else if (0 > returnValue) {
        result = k_FAILURE;
    }

    return result;
}

void RegEx::extractMatchResult(pcre2_match_data          *matchData,
                               bsl::pair<size_t, size_t> *result) const
{
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);

    BSLS_ASSERT(1 <= ovectorCount);
    (void)ovectorCount;

    size_t offset = ovector[0];
    size_t length = ovector[1] - offset;
    *result = bsl::make_pair(offset, length);
}

void RegEx::extractMatchResult(pcre2_match_data  *matchData,
                               bslstl::StringRef *result,
                               const char        *subject) const
{
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);
    (void)ovectorCount;

    BSLS_ASSERT(1 <= ovectorCount);

    size_t offset = ovector[0];
    size_t length = ovector[1] - offset;
    if (length != 0) {
        result->assign(subject + offset, static_cast<int>(length));
    } else {
        result->reset();
    }
}

void RegEx::extractMatchResult(
                         pcre2_match_data                        *matchData,
                         bsl::vector<bsl::pair<size_t, size_t> > *result) const
{
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);

    result->resize(ovectorCount);

    for (size_t i = 0, j = 0; i < ovectorCount; ++i, j += 2) {
        size_t offset = ovector[j];
        size_t length = ovector[j + 1] - offset;
        (*result)[i] = bsl::make_pair(offset, length);
    }
}

void RegEx::extractMatchResult(pcre2_match_data               *matchData,
                               bsl::vector<bslstl::StringRef> *result,
                               const char                     *subject) const
{
    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);

    // Number of pairs in the output vector
    unsigned int ovectorCount = pcre2_get_ovector_count(matchData);

    result->resize(ovectorCount);

    for (size_t i = 0, j = 0; i < ovectorCount; ++i, j += 2) {
        size_t offset = ovector[j];
        size_t length = ovector[j + 1] - offset;
        if (length != 0) {
            (*result)[i] = bslstl::StringRef(subject+offset,
                                             static_cast<int>(length));
        } else {
            (*result)[i] = bslstl::StringRef();
        }
    }
}

// CLASS METHODS
bool RegEx::isJitAvailable()
{
    unsigned int result;
    (void) result;

    BSLS_ASSERT(0 <= pcre2_config(PCRE2_CONFIG_JIT, &result));
    BSLS_ASSERT(k_IS_JIT_SUPPORTED == result);

    return k_IS_JIT_SUPPORTED;
}

// CREATORS
RegEx::RegEx(bslma::Allocator *basicAllocator)
: d_flags(0)
, d_pattern(basicAllocator)
, d_pcre2Context_p(0)
, d_compileContext_p(0)
, d_patternCode_p(0)
, d_depthLimit(RegEx::defaultDepthLimit())
, d_jitStackSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_pcre2Context_p = pcre2_general_context_create(
                                            &bdlpcre_malloc,
                                            &bdlpcre_free,
                                            static_cast<void*>(d_allocator_p));
    BSLS_ASSERT(0 != d_pcre2Context_p);

    // Pre-create compile context
    d_compileContext_p = pcre2_compile_context_create(d_pcre2Context_p);
    BSLS_ASSERT(0 != d_compileContext_p);

    MatchContextHelper::resetMatchContext(&d_mainMatchContext);
}

// MANIPULATORS
void RegEx::clear()
{
    if (isPrepared()) {
        pcre2_code_free(d_patternCode_p);
        MatchContextHelper::deallocateMatchContext(&d_mainMatchContext);
        d_patternCode_p = 0;
        d_flags         = 0;
        d_jitStackSize  = 0;
        d_pattern.clear();
    }
}

int RegEx::prepare(bsl::string *errorMessage,
                   size_t      *errorOffset,
                   const char  *pattern,
                   int          flags,
                   size_t       jitStackSize)
{
    BSLS_ASSERT(pattern);

    const int VALID_FLAGS = k_FLAG_CASELESS
                          | k_FLAG_DOTMATCHESALL
                          | k_FLAG_MULTILINE
                          | k_FLAG_UTF8
                          | k_FLAG_JIT;
    (void) VALID_FLAGS;

    BSLS_ASSERT(0 == (flags & ~VALID_FLAGS));

    // Free resources currently used by this object, if any, and put the object
    // into the "unprepared" state.

    clear();

    d_pattern      = pattern;
    d_flags        = flags;

    d_jitStackSize = (flags & k_FLAG_JIT && isJitAvailable()) ? jitStackSize
                                                              : 0;

    unsigned int pcreFlags = 0;
    pcreFlags |= flags & k_FLAG_CASELESS      ? PCRE2_CASELESS  : 0;
    pcreFlags |= flags & k_FLAG_DOTMATCHESALL ? PCRE2_DOTALL    : 0;
    pcreFlags |= flags & k_FLAG_MULTILINE     ? PCRE2_MULTILINE : 0;
    pcreFlags |= flags & k_FLAG_UTF8          ? PCRE2_UTF       : 0;

    // Compile the new pattern.

    int    errorCodeFromPcre2;
    size_t errorOffsetFromPcre2;

    pcre2_code *patternCode = pcre2_compile(
                               reinterpret_cast<const unsigned char*>(pattern),
                               PCRE2_ZERO_TERMINATED,
                               pcreFlags,
                               &errorCodeFromPcre2,
                               &errorOffsetFromPcre2,
                               d_compileContext_p);

    if (0 == patternCode) {
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

    if (flags & k_FLAG_JIT && isJitAvailable()) {
        if (0 != pcre2_jit_compile(patternCode, PCRE2_JIT_COMPLETE)) {
            pcre2_code_free(patternCode);
            if (errorMessage) {
                errorMessage->assign("JIT compilation failed.");
            }
            if (errorOffset) {
                *errorOffset = 0;
            }
            return k_FAILURE;                                         // RETURN
        }
    }

    if (k_SUCCESS != MatchContextHelper::allocateMatchContext(
                                                           &d_mainMatchContext,
                                                            d_pcre2Context_p,
                                                            patternCode,
                                                            d_depthLimit,
                                                            d_jitStackSize)) {
        pcre2_code_free(patternCode);
        if (errorMessage) {
            errorMessage->assign("Unable to create match contexts.");
        }
        if (errorOffset) {
            *errorOffset = 0;
        }
        return k_FAILURE;                                             // RETURN
    }

    // Set the data members and set the object to the "prepared" state.

    d_patternCode_p = patternCode;
    d_mainThread    = bslmt::ThreadUtil::self();

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

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   false,
                                   &matchContext);

    unloadMatchContext(&matchContext);

    return matchResult;
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

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::match(bslstl::StringRef *result,
                 const char        *subject,
                 size_t             subjectLength,
                 size_t             subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::match(bsl::vector<bsl::pair<size_t, size_t> > *result,
                 const char                              *subject,
                 size_t                                   subjectLength,
                 size_t                                   subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::match(bsl::vector<bslstl::StringRef> *result,
                 const char                     *subject,
                 size_t                          subjectLength,
                 size_t                          subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::matchRaw(const char *subject,
                    size_t      subjectLength,
                    size_t      subjectOffset) const
{
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::matchRaw(bsl::pair<size_t, size_t> *result,
                    const char                *subject,
                    size_t                     subjectLength,
                    size_t                     subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::matchRaw(bslstl::StringRef *result,
                    const char        *subject,
                    size_t             subjectLength,
                    size_t             subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::matchRaw(
                  bsl::vector<bsl::pair<size_t, size_t> > *result,
                  const char                              *subject,
                  size_t                                   subjectLength,
                  size_t                                   subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::matchRaw(bsl::vector<bslstl::StringRef> *result,
                    const char                     *subject,
                    size_t                          subjectLength,
                    size_t                          subjectOffset) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContext matchContext;

    if (0 != loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   &matchContext);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    unloadMatchContext(&matchContext);

    return matchResult;
}

int RegEx::numSubpatterns() const
{
    BSLS_ASSERT(isPrepared());

    int numSubpatterns;
    int returnValue = pcre2_pattern_info(d_patternCode_p,
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
                                 d_patternCode_p,
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
