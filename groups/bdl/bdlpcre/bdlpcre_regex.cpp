// bdlpcre_regex.cpp                                                  -*-C++-*-
#include <bdlpcre_regex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpcre2_regex_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// This component depends on the open-source Perl Compatible Regular
// Expressions (PCRE2) library (http://www.pcre.org).
//
// The PCRE2 library used by this component was configured with UTF-8 support.
//
// The 'RegEx_MatchContext' class currently implements the following strategy
// for allocating/deallocating buffers used for pattern matching:
//:
//: o The match context for the main thread (the thread that calls
//:   'initialize') is pre-allocated when the pattern is compiled.
//:
//: o Match contexts for all other threads are allocated and deallocated within
//:   each call to 'loadMatchContext' (i.e. when invoked from other thread(s)).

#include <bslma_default.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_new.h>        // placement 'new' syntax

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
#elif defined(BSLS_PLATFORM_CPU_ARM) && defined (BSLS_PLATFORM_CPU_64_BIT)
false;
#else
true;
#endif

                        // ===================
                        // class RegEx_ImpUtil
                        // ===================

struct RegEx_ImpUtil {
    // This 'struct' provides a namespace for 'RegEx' implementation utilities.

    // TYPES
    enum {
        // Enumeration used to distinguish among results of match operations.
        k_SUCCESS                 =  0,
        k_DEPTH_LIMIT_FAILURE     =  1,
        k_JIT_STACK_LIMIT_FAILURE =  2,
        k_FAILURE                 = -1
    };

    // CLASS METHODS
    static
    void assign(bsl::pair<size_t, size_t> *value,
                const char                *subject,
                size_t                     offset,
                size_t                     length)
        // Assign a newly created pair of the specified 'offset' and
        // 'length' to the specified 'value'.  Note that the specified
        // 'subject' is unused in this overload.
    {
        (void)subject;
        *value =  bsl::make_pair(offset, length);
    }

    static
    void assign(bsl::string_view *value,
                const char       *subject,
                size_t            offset,
                size_t            length)
        // Assign a newly created 'bsl::string_view' object having the
        // specified 'subject', 'offset', and 'length' to the specified
        // 'value'.
    {
        *value = (length != 0) ? bsl::string_view(subject + offset, length)
                               : bsl::string_view();
    }
};

                        // ==========
                        // struct NOP
                        // ==========

struct NOP {
    // NOP functor for 'RegEx::match' that does not return the result of match.

    // ACCESSORS
    void operator()(const char *, PCRE2_SIZE *, unsigned int) const
    {
    }
};

                        // ===================
                        // class DataExtractor
                        // ===================

template <class DATA>
class DataExtractor {
    // This functor extracts the result of a match and assigns it to a
    // variable of the DATA (template parameter) type.

    // DATA
    DATA *d_data_p;

  public:
    // CREATORS
    explicit
    DataExtractor(DATA *dataPtr)
    : d_data_p(dataPtr)
    {
        BSLS_ASSERT(dataPtr);
    }

    // ACCESSORS
    void operator()(const char   *subject,
                    PCRE2_SIZE   *ovector,
                    unsigned int  ovectorCount) const
    {
        // Number of pairs in the output vector
        (void)ovectorCount;
        BSLS_ASSERT(1 <= ovectorCount);

        size_t offset = ovector[0];
        size_t length = ovector[1] - offset;
        RegEx_ImpUtil::assign(d_data_p, subject, offset, length);
    }
};

                        // =====================
                        // class VectorExtractor
                        // =====================

template <class VECTOR>
class VectorExtractor {
    // This functor extracts the result of a match and assigns it to a variable
    // of the VECTOR (template parameter) type.

    // DATA
    VECTOR *d_vector_p;

  public:
    // CREATORS
    explicit
    VectorExtractor(VECTOR *vectorPtr)
    : d_vector_p(vectorPtr)
    {
        BSLS_ASSERT(vectorPtr);
    }

    // ACCESSORS
    void operator()(const char   *subject,
                    PCRE2_SIZE   *ovector,
                    unsigned int  ovectorCount) const
    {
        d_vector_p->resize(ovectorCount);

        for (unsigned int i = 0, j = 0; i < ovectorCount; ++i, j += 2) {
            DataExtractor<typename VECTOR::value_type> extractor(
                                                            &(*d_vector_p)[i]);
            extractor(subject, ovector + j, 1);
        }
    }
};

}  // close unnamed namespace


                        // =============================
                        // struct RegEx_MatchContextData
                        // =============================

struct RegEx_MatchContextData {
    // This is a component-local POD 'struct' that holds the pointers to the
    // buffers used by the PCRE2 match API.

  public:
    // DATA
    pcre2_match_context *d_matchContext_p;  // PCRE2 match context
    pcre2_match_data    *d_matchData_p;     // PCRE2 match data
    pcre2_jit_stack     *d_jitStack_p;      // PCRE2 JIT stack
};

                        // ========================
                        // class RegEx_MatchContext
                        // ========================

class RegEx_MatchContext {
    // This class manages opaque buffers used by PCRE2 match API.

    // PRIVATE TYPES
    typedef bslmt::ThreadUtil::Handle  ThreadHandle;
    // Alias for thread handle type.

    // DATA
    pcre2_general_context  *d_pcre2Context_p;       // PCRE2 general context
    pcre2_code             *d_pcre2PatternCode_p;   // PCRE2 compiled pattern
    int                     d_depthLimit;           // match depth limit
    size_t                  d_jitStackSize;         // JIT stack size
    ThreadHandle            d_mainThread;           // main thread ID
    RegEx_MatchContextData  d_mainThreadMatchData;  // main thread match ctx

  private:
    // PRIVATE ACCESSORS
    int allocateMatchContext(RegEx_MatchContextData *matchContextData) const;
        // Allocate PCRE2 match data buffers and load them into the specified
        // 'matchContextData'.

    void deallocateMatchContext(RegEx_MatchContextData *matchContextData)
                                                                         const;
        // Deallocate PCRE2 match data buffers pointed to by the specified
        // 'matchContextData'.

  public:
    // CREATORS
    RegEx_MatchContext();
        // Create a 'RegEx_MatchContext' object.

    ~RegEx_MatchContext();
        // Destroy this object.

    // MANIPULATORS
    int initialize(pcre2_general_context *pcre2Context,
                   pcre2_code            *patternCode,
                   int                    depthLimit,
                   size_t                 jitStackSize);
        // Initialize the object to provide data buffers for the PCRE2 match
        // API for the specified 'pcre2Context', 'patternCode, 'depthLimit',
        // and 'jitStackSize'. Return 0 on success and non-zero value if the
        // data buffers cannot be allocated.

    void setDepthLimit(int depthLimit);
        // Change the match depth limit in the match data buffers to the
        // specified 'depthLimit'.

    // ACCESSORS
    int acquireMatchContext(RegEx_MatchContextData *matchContextData) const;
        // Acquire the match data buffers for the current thread and load the
        // specified 'matchContextData' with the pointers to the match data
        // buffers.  The behavior is undefined unless 'matchContextData' is a
        // valid pointer.

    void releaseMatchContext(RegEx_MatchContextData *matchContextData) const;
        // Release the match data buffers pointed to by the specified
        // 'matchContextData'.  The behavior is undefined unless
        // 'matchContextData' is a valid pointer.
};

                        // ------------------
                        // RegEx_MatchContext
                        // ------------------

// CREATORS
RegEx_MatchContext::RegEx_MatchContext()
: d_pcre2Context_p(0)
, d_pcre2PatternCode_p(0)
, d_depthLimit(0)
, d_jitStackSize(0)
, d_mainThread(bslmt::ThreadUtil::invalidHandle())
, d_mainThreadMatchData()
{
}

RegEx_MatchContext::~RegEx_MatchContext()
{
    deallocateMatchContext(&d_mainThreadMatchData);
}

// PRIVATE ACCESSORS
int
RegEx_MatchContext::allocateMatchContext(
                                RegEx_MatchContextData *matchContextData) const
{
    BSLS_ASSERT(matchContextData);
    BSLS_ASSERT(d_pcre2Context_p);
    BSLS_ASSERT(d_pcre2PatternCode_p);

    // Match Data
    pcre2_match_data *matchData_p = pcre2_match_data_create_from_pattern(
                                                          d_pcre2PatternCode_p,
                                                          0);

    if (0 == matchData_p) {
        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
    }

    // Match context
    pcre2_match_context *matchContext_p = pcre2_match_context_create(
                                                             d_pcre2Context_p);

    if (0 == matchContext_p) {
        pcre2_match_data_free(matchData_p);
        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
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
            return RegEx_ImpUtil::k_FAILURE;                          // RETURN
        }
        pcre2_jit_stack_assign(matchContext_p, 0, jitStack_p);
    }

    matchContextData->d_matchData_p    = matchData_p;
    matchContextData->d_matchContext_p = matchContext_p;
    matchContextData->d_jitStack_p     = jitStack_p;

    return RegEx_ImpUtil::k_SUCCESS;
}

void
RegEx_MatchContext::deallocateMatchContext(
                                RegEx_MatchContextData *matchContextData) const
{
    BSLS_ASSERT(matchContextData);

    pcre2_match_data_free(matchContextData->d_matchData_p);
    pcre2_jit_stack_free(matchContextData->d_jitStack_p);
    pcre2_match_context_free(matchContextData->d_matchContext_p);
}

// MANIPULATORS
int RegEx_MatchContext::initialize(pcre2_general_context *pcre2Context,
                                   pcre2_code            *patternCode,
                                   int                    depthLimit,
                                   size_t                 jitStackSize)
{
    BSLS_ASSERT(pcre2Context);
    BSLS_ASSERT(patternCode);

    deallocateMatchContext(&d_mainThreadMatchData);

    d_mainThread         = bslmt::ThreadUtil::self();
    d_pcre2Context_p     = pcre2Context;
    d_pcre2PatternCode_p = patternCode;
    d_depthLimit         = depthLimit;
    d_jitStackSize       = jitStackSize;

    return allocateMatchContext(&d_mainThreadMatchData);
}

void RegEx_MatchContext::setDepthLimit(int depthLimit)
{
    d_depthLimit = depthLimit;
    if (d_mainThreadMatchData.d_matchContext_p) {
        pcre2_set_match_limit(d_mainThreadMatchData.d_matchContext_p,
                              d_depthLimit);
    }
}

// ACCESSORS
int
RegEx_MatchContext::acquireMatchContext(
                                RegEx_MatchContextData *matchContextData) const
{
    BSLS_ASSERT(matchContextData);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        *matchContextData = d_mainThreadMatchData;
        return RegEx_ImpUtil::k_SUCCESS;                              // RETURN
    }

    return allocateMatchContext(matchContextData);
}

void RegEx_MatchContext::releaseMatchContext(
                                RegEx_MatchContextData *matchContextData) const
{
    BSLS_ASSERT(matchContextData);

    if (bslmt::ThreadUtil::isEqual(d_mainThread, bslmt::ThreadUtil::self())) {
        return;                                                       // RETURN
    }

    deallocateMatchContext(matchContextData);
}

                             // -----------
                             // class RegEx
                             // -----------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Int RegEx::s_depthLimit = {10000000};

// CONSTANTS
const size_t RegEx::k_INVALID_OFFSET = ~(size_t)0;


// PRIVATE MANIPULATORS
int RegEx::prepareImp(char       *errorMessage,
                      size_t      errorMessageLength,
                      size_t     *errorOffset,
                      const char *pattern,
                      int         flags,
                      size_t      jitStackSize)
{
    BSLS_ASSERT(errorMessage);
    BSLS_ASSERT(errorOffset);
    BSLS_ASSERT(pattern);

    const int VALID_FLAGS = k_FLAG_CASELESS
                          | k_FLAG_DOTMATCHESALL
                          | k_FLAG_MULTILINE
                          | k_FLAG_UTF8
                          | k_FLAG_JIT
                          | k_FLAG_DUPNAMES;
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
    pcreFlags |= flags & k_FLAG_DUPNAMES      ? PCRE2_DUPNAMES  : 0;

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
        int result = pcre2_get_error_message(
                               errorCodeFromPcre2,
                               reinterpret_cast<unsigned char *>(errorMessage),
                               errorMessageLength);

        if (result < 0 && PCRE2_ERROR_NOMEMORY != result) {
            *errorMessage = 0;
        }

        *errorOffset = errorOffsetFromPcre2;

        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
    }

    if (flags & k_FLAG_JIT && isJitAvailable()) {
        if (0 != pcre2_jit_compile(patternCode, PCRE2_JIT_COMPLETE)) {
            pcre2_code_free(patternCode);
            bsl::strncpy(errorMessage,
                         "JIT compilation failed.",
                         errorMessageLength);

            *errorOffset = 0;

            return RegEx_ImpUtil::k_FAILURE;                          // RETURN
        }
    }

    if (RegEx_ImpUtil::k_SUCCESS != d_matchContext->initialize(
                                                              d_pcre2Context_p,
                                                              patternCode,
                                                              d_depthLimit,
                                                              d_jitStackSize))
    {
        pcre2_code_free(patternCode);
        bsl::strncpy(errorMessage,
                     "Unable to create match contexts.",
                     errorMessageLength);

        *errorOffset = 0;

        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
    }

    // Set the data members and set the object to the "prepared" state.

    d_patternCode_p = patternCode;

    return RegEx_ImpUtil::k_SUCCESS;
}

// PRIVATE ACCESSORS
template <class RESULT_EXTRACTOR>
int RegEx::matchImp(const RESULT_EXTRACTOR&  extractor,
                    const char              *subject,
                    size_t                   subjectLength,
                    size_t                   subjectOffset,
                    bool                     skipUTF8Validation) const
{
    BSLS_ASSERT(subject || 0 == subjectLength);
    BSLS_ASSERT(subjectOffset <= subjectLength);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContextData matchContextData;

    if (0 != d_matchContext->acquireMatchContext(&matchContextData)) {
        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
    }

    const unsigned char *actualSubject =
                reinterpret_cast<const unsigned char*>(subject ? subject : "");

    int rc = RegEx_ImpUtil::k_SUCCESS;
    int rcPcre2;

    if (skipUTF8Validation || !(d_flags & k_FLAG_UTF8)) {
        if (d_flags & k_FLAG_JIT && isJitAvailable()) {
            rcPcre2 = pcre2_jit_match(d_patternCode_p,
                                      actualSubject,
                                      subjectLength,
                                      subjectOffset,
                                      0,
                                      matchContextData.d_matchData_p,
                                      matchContextData.d_matchContext_p);
        } else {
            rcPcre2 = pcre2_match(    d_patternCode_p,
                                      actualSubject,
                                      subjectLength,
                                      subjectOffset,
                                      PCRE2_NO_UTF_CHECK,
                                      matchContextData.d_matchData_p,
                                      matchContextData.d_matchContext_p);
        }
    } else {
        rcPcre2 = pcre2_match(        d_patternCode_p,
                                      actualSubject,
                                      subjectLength,
                                      subjectOffset,
                                      0,
                                      matchContextData.d_matchData_p,
                                      matchContextData.d_matchContext_p);
    }

    if (PCRE2_ERROR_MATCHLIMIT == rcPcre2) {
        rc = RegEx_ImpUtil::k_DEPTH_LIMIT_FAILURE;
    } else if (PCRE2_ERROR_JIT_STACKLIMIT == rcPcre2) {
        rc = RegEx_ImpUtil::k_JIT_STACK_LIMIT_FAILURE;
    } else if (0 > rcPcre2) {
        rc = RegEx_ImpUtil::k_FAILURE;
    }

    if (RegEx_ImpUtil::k_SUCCESS == rc) {
        extractor(subject,
                  pcre2_get_ovector_pointer(matchContextData.d_matchData_p),
                  pcre2_get_ovector_count(matchContextData.d_matchData_p));
    }

    d_matchContext->releaseMatchContext(&matchContextData);

    return rc;
}

template <class Vector>
inline
void RegEx::namedSubpatternsImp(Vector *result) const
{
    BSLS_ASSERT(isPrepared());

    uint32_t nameCount;
    int rc = pcre2_pattern_info(d_patternCode_p,
                                PCRE2_INFO_NAMECOUNT,
                                &nameCount);
    // The 'pcre2_pattern_info' functions called here will only report an error
    // on invalid input, which would indicate a programming error in this
    // component
    BSLS_ASSERT(rc == 0);

    result->clear();

    if (nameCount == 0) {
        return;                                                       // RETURN
    }

    result->reserve(nameCount);

    uint32_t nameEntrySize;
    rc = pcre2_pattern_info(d_patternCode_p,
                            PCRE2_INFO_NAMEENTRYSIZE,
                            &nameEntrySize);
    BSLS_ASSERT(rc == 0);

    PCRE2_SPTR nameTable;
    rc = pcre2_pattern_info(d_patternCode_p,
                            PCRE2_INFO_NAMETABLE,
                            &nameTable);
    BSLS_ASSERT(rc == 0);
    (void) rc;

    for (; nameCount--; nameTable += nameEntrySize) {
#if PCRE2_CODE_UNIT_WIDTH == 8
        // In the 8-bit library, the first two bytes of each entry are the
        // number of the capturing parenthesis, most significant byte first.
        uint16_t groupNum = nameTable[0];
        groupNum = static_cast<uint16_t>((groupNum << 8) | nameTable[1]);

        // The rest of the entry is the corresponding name, zero terminated.
        const char *name = (const char *) (nameTable + 2);
#else
        // In the 16/32-bit library, the pointer points to 16/32-bit code
        // units, the first of which contains the parenthesis number.
        PCRE2_UCHAR groupNum = nameTable[0];

        // The rest of the entry is the corresponding name, zero terminated.
        PCRE2_SPTR name = nameTable + 1;
#endif
        result->push_back(typename Vector::value_type(
                                            name, static_cast<int>(groupNum)));
    }
}

template <class STRING>
int RegEx::replaceImp(STRING                  *result,
                      int                     *errorOffset,
                      const bsl::string_view&  subject,
                      const bsl::string_view&  replacement,
                      size_t                   options,
                      bool                     skipUTF8Validation) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(errorOffset);
    BSLS_ASSERT(isPrepared());

    RegEx_MatchContextData matchContextData;

    if (0 != d_matchContext->acquireMatchContext(&matchContextData)) {
        return RegEx_ImpUtil::k_FAILURE;                              // RETURN
    }

    unsigned int pcreFlags = 0;
    pcreFlags |= options & k_REPLACE_LITERAL    ? PCRE2_SUBSTITUTE_LITERAL : 0;
    pcreFlags |= options & k_REPLACE_GLOBAL      ? PCRE2_SUBSTITUTE_GLOBAL : 0;
    pcreFlags |= options & k_REPLACE_EXTENDED  ? PCRE2_SUBSTITUTE_EXTENDED : 0;
    pcreFlags |= options & k_REPLACE_UNKNOWN_UNSET
                                          ? PCRE2_SUBSTITUTE_UNKNOWN_UNSET : 0;
    pcreFlags |= options & k_REPLACE_UNSET_EMPTY
                                            ? PCRE2_SUBSTITUTE_UNSET_EMPTY : 0;
    pcreFlags |= skipUTF8Validation ? PCRE2_NO_UTF_CHECK : 0;
    pcreFlags |= PCRE2_SUBSTITUTE_OVERFLOW_LENGTH;

    int    rcPcre2;
    size_t bufferLength = static_cast<size_t>(result->length());

    do {
        if (bufferLength > result->length()) {
            result->resize(bufferLength);
        }

        rcPcre2 = pcre2_substitute(
                   d_patternCode_p,
                   reinterpret_cast<const unsigned char *>(subject.data()),
                   subject.length(),
                   0,
                   pcreFlags,
                   matchContextData.d_matchData_p,
                   matchContextData.d_matchContext_p,
                   reinterpret_cast<const unsigned char *>(replacement.data()),
                   replacement.length(),
                   reinterpret_cast<unsigned char *>(&(*result)[0]),
                   &bufferLength);

    } while (PCRE2_ERROR_NOMEMORY == rcPcre2);

    int rc = rcPcre2;

    if (rcPcre2 >= 0) {
        result->resize(bufferLength);
    } else {
        rc           = RegEx_ImpUtil::k_FAILURE;
        *errorOffset = PCRE2_UNSET == static_cast<size_t>(rcPcre2)
                       ? -1 : static_cast<int>(bufferLength);
    }

    d_matchContext->releaseMatchContext(&matchContextData);

    return rc;
}

// CLASS METHODS
bool RegEx::isJitAvailable()
{
    unsigned int result = 0;
    (void) result;

#if !(defined(BSLS_PLATFORM_OS_DARWIN) && defined(BSLS_PLATFORM_CPU_ARM))
    // Currently pcre2_config incorrectly reports JIT support is available for
    // Apple M1 hardware, but it currently does not work.

    BSLS_ASSERT(0 <= pcre2_config(PCRE2_CONFIG_JIT, &result));
    BSLS_ASSERT(k_IS_JIT_SUPPORTED == (0 != result));
#endif
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
    d_matchContext.load(new (*d_allocator_p) RegEx_MatchContext(),
                        d_allocator_p);

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

int RegEx::prepare(bsl::nullptr_t  errorMessage,
                   size_t         *errorOffset,
                   const char     *pattern,
                   int             flags,
                   size_t          jitStackSize)
{
    (void)errorMessage;

    char   dummy[1];
    size_t offset;

    int rc = prepareImp(&dummy[0], 0, &offset, pattern, flags, jitStackSize);
    if (rc && errorOffset) {
        *errorOffset = offset;
    }

    return rc;
}

int RegEx::setDepthLimit(int depthLimit)
{
    int previous = d_depthLimit;

    d_depthLimit = depthLimit;

    d_matchContext->setDepthLimit(d_depthLimit);

    return previous;
}

// ACCESSORS
int RegEx::match(const bsl::string_view& subject,
                 size_t                  subjectOffset) const
{
    return match(subject.data(), subject.length(), subjectOffset);
}

int RegEx::match(const char *subject,
                 size_t      subjectLength,
                 size_t      subjectOffset) const
{
    return matchImp(NOP(), subject, subjectLength, subjectOffset, false);
}

int RegEx::match(bsl::pair<size_t, size_t> *result,
                 const char                *subject,
                 size_t                     subjectLength,
                 size_t                     subjectOffset) const
{
    BSLS_ASSERT(result);

    return matchImp(DataExtractor<bsl::pair<size_t, size_t> >(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    false);
}

int RegEx::match(bsl::string_view *result,
                 const char       *subject,
                 size_t            subjectLength,
                 size_t            subjectOffset) const
{
    BSLS_ASSERT(result);

    return matchImp(DataExtractor<bsl::string_view>(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    false);
}

int RegEx::match(bsl::string_view        *result,
                 const bsl::string_view&  subject,
                 size_t                   subjectOffset) const
{
    BSLS_ASSERT(result);

    return match(result, subject.data(), subject.length(), subjectOffset);
}

int RegEx::match(bsl::vector<bsl::pair<size_t, size_t> > *result,
                 const char                              *subject,
                 size_t                                   subjectLength,
                 size_t                                   subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bsl::pair<size_t, size_t> > >
                                                                     Extractor;

    return matchImp(Extractor(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    false);
}

int RegEx::match(bsl::vector<bslstl::StringRef> *result,
                 const char                     *subject,
                 size_t                          subjectLength,
                 size_t                          subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bslstl::StringRef> > Extractor;

    return matchImp(Extractor(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    false);
}

int RegEx::match(bsl::vector<bsl::string_view> *result,
                 const bsl::string_view&        subject,
                 size_t                         subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    false);
}

int RegEx::match(std::vector<bsl::string_view> *result,
                 const bsl::string_view&        subject,
                 size_t                         subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<std::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    false);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int RegEx::match(std::pmr::vector<bsl::string_view> *result,
                const bsl::string_view&             subject,
                size_t                              subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<std::pmr::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    false);
}
#endif

int RegEx::matchRaw(const char *subject,
                    size_t      subjectLength,
                    size_t      subjectOffset) const
{
    return matchImp(NOP(), subject, subjectLength, subjectOffset, true);
}

int RegEx::matchRaw(const bsl::string_view& subject,
                    size_t                  subjectOffset) const
{
    return matchRaw(subject.data(), subject.length(), subjectOffset);
}

int RegEx::matchRaw(bsl::pair<size_t, size_t> *result,
                    const char                *subject,
                    size_t                     subjectLength,
                    size_t                     subjectOffset) const
{
    BSLS_ASSERT(result);

    return matchImp(DataExtractor<bsl::pair<size_t, size_t> >(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    true);
}

int RegEx::matchRaw(bsl::string_view *result,
                    const char       *subject,
                    size_t            subjectLength,
                    size_t            subjectOffset) const
{
    BSLS_ASSERT(result);

    return matchImp(DataExtractor<bsl::string_view>(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    true);
}

int RegEx::matchRaw(bsl::string_view        *result,
                    const bsl::string_view&  subject,
                    size_t                   subjectOffset) const
{
    BSLS_ASSERT(result);

    return matchRaw(result, subject.data(), subject.length(), subjectOffset);
}

int RegEx::matchRaw(
                  bsl::vector<bsl::pair<size_t, size_t> > *result,
                  const char                              *subject,
                  size_t                                   subjectLength,
                  size_t                                   subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bsl::pair<size_t, size_t> > >
                                                                     Extractor;

    return matchImp(Extractor(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    true);
}

int RegEx::matchRaw(
                  bsl::vector<bslstl::StringRef> *result,
                  const char                     *subject,
                  size_t                          subjectLength,
                  size_t                          subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bslstl::StringRef> > Extractor;

    return matchImp(Extractor(result),
                    subject,
                    subjectLength,
                    subjectOffset,
                    true);
}

int RegEx::matchRaw(bsl::vector<bsl::string_view> *result,
                    const bsl::string_view&        subject,
                    size_t                         subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<bsl::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    true);
}

int RegEx::matchRaw(std::vector<bsl::string_view> *result,
                    const bsl::string_view&        subject,
                    size_t                         subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<std::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    true);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int RegEx::matchRaw(std::pmr::vector<bsl::string_view> *result,
                    const bsl::string_view&             subject,
                    size_t                              subjectOffset) const
{
    BSLS_ASSERT(result);

    typedef VectorExtractor<std::pmr::vector<bsl::string_view> > Extractor;

    return matchImp(Extractor(result),
                    subject.data(),
                    subject.length(),
                    subjectOffset,
                    true);
}
#endif

int RegEx::replace(bsl::string             *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      false);
}

int RegEx::replace(std::string             *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      false);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int RegEx::replace(std::pmr::string        *result,
                   int                     *errorOffset,
                   const bsl::string_view&  subject,
                   const bsl::string_view&  replacement,
                   size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      false);
}
#endif

int RegEx::replaceRaw(bsl::string             *result,
                      int                     *errorOffset,
                      const bsl::string_view&  subject,
                      const bsl::string_view&  replacement,
                      size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      true);
}

int RegEx::replaceRaw(std::string             *result,
                      int                     *errorOffset,
                      const bsl::string_view&  subject,
                      const bsl::string_view&  replacement,
                      size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      true);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int RegEx::replaceRaw(std::pmr::string        *result,
                      int                     *errorOffset,
                      const bsl::string_view&  subject,
                      const bsl::string_view&  replacement,
                      size_t                   options) const
{
    return replaceImp(result,
                      errorOffset,
                      subject,
                      replacement,
                      options,
                      true);
}
#endif

void RegEx::namedSubpatterns(
                  bsl::vector<bsl::pair<bsl::string_view, int> > *result) const
{
    namedSubpatternsImp(result);
}

void RegEx::namedSubpatterns(
                  std::vector<std::pair<bsl::string_view, int> > *result) const
{
    namedSubpatternsImp(result);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void RegEx::namedSubpatterns(
             std::pmr::vector<std::pair<bsl::string_view, int> > *result) const
{
    namedSubpatternsImp(result);
}
#endif

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
