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
// 'RegEx_MatchContextImp' class currently implements the following strategy
// for allocating/deallocating buffers used for pattern matching:
//:
//: o The match context for the main thread (the thread that calls
//:   'initialize') is pre-allocated when the pattern is compiled.
//:
//: o Match contexts for all other threads are allocated and deallocated within
//:   each call to 'loadMatchContext' (i.e. when invoked from other thread(s)).

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

                        // =========================
                        // struct RegEx_MatchContext
                        // =========================

struct RegEx_MatchContext {
    // This is a component local POD 'struct' holds the pointers to the buffers
    // used by the PCRE2 match API.

  public:
    // DATA
    pcre2_match_context *d_matchContext_p;  // PCRE2 match context
    pcre2_match_data    *d_matchData_p;     // PCRE2 match data
    pcre2_jit_stack     *d_jitStack_p;      // PCRE2 JIT stack
};

                        // ===========================
                        // class RegEx_MatchContextImp
                        // ===========================

class RegEx_MatchContextImp {
    // This class manages opaque buffers used by PCRE2 match API.

    // PRIVATE TYPES
    typedef bslmt::ThreadUtil::Handle  ThreadHandle;
    // Alias for thread handle type.

    // DATA
    ThreadHandle           d_mainThread;        // main thread ID
    pcre2_general_context *d_pcre2Context_p;
    pcre2_code            *d_pcre2PatternCode_p;
    int                    d_depthLimit;
    size_t                 d_jitStackSize;

    RegEx_MatchContext     d_mainThreadMatchContext;

  private:
    // PRIVATE ACCESSORS
    int allocateMatchContext(RegEx_MatchContext *matchContext) const;
        // Allocate PCRE2 match data buffers and load them into the specified
        // 'matchContext'.

    void deallocateMatchContext(RegEx_MatchContext *matchContext) const;
        // Deallocate PCRE2 match data buffers pointed by the specified
        // 'matchContext'.

  public:
    // CREATORS
    RegEx_MatchContextImp();
        // Create a 'RegEx_MatchContextImp' object.

    ~RegEx_MatchContextImp();
        // Destroy this object.

    // MANIPULATORS
    int initialize(pcre2_general_context *pcre2Context,
                   pcre2_code            *patternCode,
                   int                    depthLimit,
                   size_t                 jitStackSize);
        // Initialize the object to provide data buffers for the PCRE2 match
        // API for the specified 'pcre2Context', 'patternCode, 'depthLimit',
        // and 'jitStackSize'. Return 0 on success and non-zero value if the
        // data bufferes cannot be allocated.

    void setDepthLimit(int depthLimit);
        // Change the match depth limit in the match data buffers to the
        // specified 'depthLimit'.

    // ACCESSORS
    int loadMatchContext(RegEx_MatchContext *matchContext) const;
        // Load the specified 'matchContext' with the pointers to the match
        // buffers for the current thread.  The behaviour is undefined unless
        // 'matchContext' is a valid pointer.

    void unloadMatchContext(RegEx_MatchContext *matchContext) const;
        // Unload the match buffers pointed by the specified 'matchContext'.
        // The behaviour is undefined unless 'matchContext' is a valid pointer.
};

                        // ---------------------
                        // RegEx_MatchContextImp
                        // ---------------------

// CREATORS
RegEx_MatchContextImp::RegEx_MatchContextImp()
: d_mainThread(bslmt::ThreadUtil::invalidHandle())
, d_pcre2Context_p(0)
, d_pcre2PatternCode_p(0)
, d_depthLimit(0)
, d_jitStackSize(0)
, d_mainThreadMatchContext()
{
}

RegEx_MatchContextImp::~RegEx_MatchContextImp()
{
    deallocateMatchContext(&d_mainThreadMatchContext);
}

// PRIVATE ACCESSORS
int
RegEx_MatchContextImp::allocateMatchContext(RegEx_MatchContext *matchContext)
                                                                          const
{
    BSLS_ASSERT(matchContext);
    BSLS_ASSERT(d_pcre2Context_p);
    BSLS_ASSERT(d_pcre2PatternCode_p);

    // Match Data
    pcre2_match_data *matchData_p = pcre2_match_data_create_from_pattern(
                                                          d_pcre2PatternCode_p,
                                                          0);

    if (0 == matchData_p) {
        return k_FAILURE;                                             // RETURN
    }

    // Match context
    pcre2_match_context *matchContext_p = pcre2_match_context_create(
                                                             d_pcre2Context_p);

    if (0 == matchContext_p) {
        pcre2_match_data_free(matchData_p);
        return k_FAILURE;                                             // RETURN
    }

    pcre2_set_match_limit(matchContext_p, d_depthLimit);

    // Jit stack
    pcre2_jit_stack *jitStack_p = 0;
    if (d_jitStackSize) {
        jitStack_p =
              pcre2_jit_stack_create(d_jitStackSize,
                                     d_jitStackSize,
                                     d_pcre2Context_p);
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

void
RegEx_MatchContextImp::deallocateMatchContext(RegEx_MatchContext *matchContext)
                                                                          const
{
    BSLS_ASSERT(matchContext);

    pcre2_match_data_free(matchContext->d_matchData_p);
    pcre2_jit_stack_free(matchContext->d_jitStack_p);
    pcre2_match_context_free(matchContext->d_matchContext_p);
}

// MANIPULATORS
int RegEx_MatchContextImp::initialize(pcre2_general_context *pcre2Context,
                                      pcre2_code            *patternCode,
                                      int                    depthLimit,
                                      size_t                 jitStackSize)
{
    BSLS_ASSERT(pcre2Context);
    BSLS_ASSERT(patternCode);

    deallocateMatchContext(&d_mainThreadMatchContext);

    d_mainThread         = bslmt::ThreadUtil::self();
    d_pcre2Context_p     = pcre2Context;
    d_pcre2PatternCode_p = patternCode;
    d_depthLimit         = depthLimit;
    d_jitStackSize       = jitStackSize;

    return allocateMatchContext(&d_mainThreadMatchContext);
}

void RegEx_MatchContextImp::setDepthLimit(int depthLimit)
{
    d_depthLimit = depthLimit;
    if (d_mainThreadMatchContext.d_matchContext_p) {
        pcre2_set_match_limit(d_mainThreadMatchContext.d_matchContext_p,
                              d_depthLimit);
    }
}

// ACCESSORS
int
RegEx_MatchContextImp::loadMatchContext(RegEx_MatchContext *matchContext) const
{
    BSLS_ASSERT(matchContext);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        *matchContext = d_mainThreadMatchContext;
        return k_SUCCESS;                                             // RETURN
    }

    return allocateMatchContext(matchContext);
}

void RegEx_MatchContextImp::unloadMatchContext(
                                        RegEx_MatchContext *matchContext) const
{
    BSLS_ASSERT(matchContext);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        return;                                                       // RETURN
    }

    deallocateMatchContext(matchContext);
}

                             // -----------
                             // class RegEx
                             // -----------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Int RegEx::s_depthLimit = {10000000};

// PRIVATE ACCESSORS
int RegEx::privateMatch(const char          *subject,
                        size_t               subjectLength,
                        size_t               subjectOffset,
                        bool                 skipValidation,
                        pcre2_match_data    *matchData,
                        pcre2_match_context *matchContext) const
{
    BSLS_ASSERT(matchData);
    BSLS_ASSERT(matchContext);

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
                                          matchData,
                                          matchContext);
        } else {
            returnValue = pcre2_match(d_patternCode_p,
                                      actualSubject,
                                      subjectLength,
                                      subjectOffset,
                                      PCRE2_NO_UTF_CHECK,
                                      matchData,
                                      matchContext);
        }

    } else {
        returnValue = pcre2_match(d_patternCode_p,
                          actualSubject,
                          subjectLength,
                          subjectOffset,
                          0,
                          matchData,
                          matchContext);
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
    // Creating match context
    RegEx_MatchContextImp *matchImp = new (*d_allocator_p)
                                                       RegEx_MatchContextImp();
    d_matchContextImp.load(matchImp, d_allocator_p);

    d_pcre2Context_p = pcre2_general_context_create(
                                            &bdlpcre_malloc,
                                            &bdlpcre_free,
                                            static_cast<void*>(d_allocator_p));
    BSLS_ASSERT(0 != d_pcre2Context_p);

    // Pre-create compile context
    d_compileContext_p = pcre2_compile_context_create(d_pcre2Context_p);
    BSLS_ASSERT(0 != d_compileContext_p);
}

// MANIPULATORS
void RegEx::clear()
{
    if (isPrepared()) {
        pcre2_code_free(d_patternCode_p);
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

    if (k_SUCCESS != d_matchContextImp->initialize(d_pcre2Context_p,
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

    return k_SUCCESS;
}

int RegEx::setDepthLimit(int depthLimit)
{
    int previous = d_depthLimit;

    d_depthLimit = depthLimit;

    d_matchContextImp->setDepthLimit(d_depthLimit);

    return previous;
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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   false,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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

    if (0 != d_matchContextImp->loadMatchContext(&matchContext)) {
        return k_FAILURE;                                             // RETURN
    }

    int matchResult = privateMatch(subject,
                                   subjectLength,
                                   subjectOffset,
                                   true,
                                   matchContext.d_matchData_p,
                                   matchContext.d_matchContext_p);

    if (k_SUCCESS == matchResult) {
        extractMatchResult(matchContext.d_matchData_p, result, subject);
    }

    d_matchContextImp->unloadMatchContext(&matchContext);

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
