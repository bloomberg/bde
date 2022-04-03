// bsls_bslsourcenameparserutil.t.cpp                                 -*-C++-*-
#include <bsls_bslsourcenameparserutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -MN04   // Pointer members must end in '_p'
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This test driver tests utility functions provided by the
// 'bsls::BslSourceNameParserUtil' namespace.
//-----------------------------------------------------------------------------
// [ 2] int getComponentName(p, l, srcname, kind);
// [ 3] const char *errorMessage(int errorCode);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

//=============================================================================
//                      MINIMALLY REQUIRED TEST MACROS
//-----------------------------------------------------------------------------

#define ASSERT(X) do { aSsErT(!(X), #X, __LINE__); } while(false)

#define LOOP0_ASSERT ASSERT

#define LOOP1_ASSERT(I, X) do {              \
    const bool k_ASSERT_FAILED_ = !(X);      \
    if (k_ASSERT_FAILED_) {                  \
        assertPrint("'" #I "': ", (I));      \
    }                                        \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);  \
} while(false)

#define LOOP2_ASSERT(I, J, X) do {            \
    const bool k_ASSERT_FAILED_ = !(X);       \
    if (k_ASSERT_FAILED_) {                   \
        const char *k_VAL_NAMES_[] = {        \
            "'" #I "': ",                     \
            "\t'" #J "': "                    \
        };                                    \
        assertPrint(k_VAL_NAMES_, (I), (J));  \
    }                                         \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);   \
} while(false)

#define LOOP3_ASSERT(I, J, K, X) do {              \
    const bool k_ASSERT_FAILED_ = !(X);            \
    if (k_ASSERT_FAILED_) {                        \
        const char *k_VAL_NAMES_[] = {             \
            "'" #I "': ",                          \
            "\t'" #J "': ",                        \
            "\t'" #K "': "                         \
        };                                         \
        assertPrint(k_VAL_NAMES_, (I), (J), (K));  \
    }                                              \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);        \
} while(false)

#define LOOP4_ASSERT(I, J, K, L, X) do {                \
    const bool k_ASSERT_FAILED_ = !(X);                 \
    if (k_ASSERT_FAILED_) {                             \
        const char *k_VAL_NAMES_[] = {                  \
            "'" #I "': ",                               \
            "\t'" #J "': ",                             \
            "\t'" #K "': ",                             \
            "\t'" #L "': "                              \
        };                                              \
        assertPrint(k_VAL_NAMES_, (I), (J), (K), (L));  \
    }                                                   \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);             \
} while(false)

#define LOOP5_ASSERT(I, J, K, L, M, X) do {                  \
    const bool k_ASSERT_FAILED_ = !(X);                      \
    if (k_ASSERT_FAILED_) {                                  \
        const char *k_VAL_NAMES_[] = {                       \
            "'" #I "': ",                                    \
            "\t'" #J "': ",                                  \
            "\t'" #K "': ",                                  \
            "\t'" #L "': ",                                  \
            "\t'" #M "': "                                   \
        };                                                   \
        assertPrint(k_VAL_NAMES_, (I), (J), (K), (L), (M));  \
    }                                                        \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);                  \
} while(false)

#define LOOP6_ASSERT(I, J,K, L, M, N, X) do {                     \
    const bool k_ASSERT_FAILED_ = !(X);                           \
    if (k_ASSERT_FAILED_) {                                       \
        const char *k_VAL_NAMES_[] = {                            \
            "'" #I "': ",                                         \
            "\t'" #J "': ",                                       \
            "\t'" #K "': ",                                       \
            "\t'" #L "': ",                                       \
            "\t'" #M "': ",                                       \
            "\t'" #N "': "                                        \
        };                                                        \
        assertPrint(k_VAL_NAMES_, (I), (J), (K), (L), (M), (N));  \
    }                                                             \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);                       \
} while(false)

#define LOOP7_ASSERT(I, J, K, L, M, N, O, X) do {                      \
    const bool k_ASSERT_FAILED_ = !(X);                                \
    if (k_ASSERT_FAILED_) {                                            \
        const char *k_VAL_NAMES_[] = {                                 \
            "'" #I "': ",                                              \
            "\t'" #J "': ",                                            \
            "\t'" #K "': ",                                            \
            "\t'" #L "': ",                                            \
            "\t'" #M "': ",                                            \
            "\t'" #N "': ",                                            \
            "\t'" #O "': "                                             \
        };                                                             \
        assertPrint(k_VAL_NAMES_, (I), (J), (K), (L), (M), (N), (O));  \
    }                                                                  \
    aSsErT(k_ASSERT_FAILED_, #X, __LINE__);                            \
} while(false)

#define ASSERT0 LOOP0_ASSERT
#define ASSERT1 LOOP1_ASSERT
#define ASSERT2 LOOP2_ASSERT
#define ASSERT3 LOOP3_ASSERT
#define ASSERT4 LOOP4_ASSERT
#define ASSERT5 LOOP5_ASSERT
#define ASSERT6 LOOP6_ASSERT
#define ASSERT7 LOOP7_ASSERT

#define L_ __LINE__

#define P_(X) \
    do { fputs("'" #X "' = ", stdout); printTestingValue((X)); } while(false);

#define P(X) do { P_(X); fputc('\n', stdout); } while(false)

//=============================================================================
//           MINIMALLY REQUIRED TEST PRINTERS FOR FUNDAMENTAL TYPES
//-----------------------------------------------------------------------------

namespace {
static inline void printTestingValue(int v)
    // Print the specified 'v' to 'stdout'.
{
    printf("%d", v);
}

static inline void printTestingValue(unsigned v)
    // Print the specified 'v' to 'stdout'.
{
    printf("%u", v);
}

#if defined(__clang__)
// We do not use '<bsls_platform.h> here so this component can be used if
// necessary in any other component (including '<bsls_platform>' or components
// depending on it).
//
// clang ignores the indication that we don't care if (function) names are used
// or not, it still emits a different kind of warning:
//..
// function 'functionName' is not needed and will not be emitted
//..
// We disable that warning for this test driver, as it has no value.
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif

#ifdef BDE_VERIFY
#pragma bde_verify push
#pragma bde_verify -CC01   // C-style cast is used
#endif
static inline void printTestingValue(unsigned long v)
    // Print the specified 'v' to 'stdout'.
{
    // these functions may be unused depending on what type 'size_t' is
    (void)(void(*)(unsigned long))& printTestingValue;

    printf("%lu", v);
}

static inline void printTestingValue(unsigned long long v)
    // Print the specified 'v' to 'stdout'.
{
    // these functions may be unused depending on what type 'size_t' is
    (void)(void(*)(unsigned long long))& printTestingValue;

    printf("%llu", v);
}
#ifdef BDE_VERIFY
#pragma bde_verify pop
#endif

static inline void printTestingValue(const char* text)
    // Print the specified 'text' to 'stdout'.
{
    fputc('"', stdout);
    fputs(text, stdout);
    fputc('"', stdout);
}


//=============================================================================
//                            PRINTERS WITH NAMES
//-----------------------------------------------------------------------------

template <class T1>
void assertPrint(const char *name, const T1& v1)
    // Print the specified 'name' followed by the specified 'v1' to 'stdout'.
{
    fputs(name, stdout);
    printTestingValue(v1);
    fputc('\n', stdout);
}

template <class T1, class T2>
void assertPrint(const char* (&names)[2], const T1& v1, const T2& v2)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', and 'v2' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputc('\n', stdout);
}

template <class T1, class T2, class T3>
void assertPrint(const char* (&names)[3],
                 const T1&     v1,
                 const T2&     v2,
                 const T3&     v3)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', 'v2', and 'v3' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputs(names[2], stdout);
    printTestingValue(v3);
    fputc('\n', stdout);
}

template <class T1, class T2, class T3, class T4>
void assertPrint(const char* (&names)[4],
                 const T1&     v1,
                 const T2&     v2,
                 const T3&     v3,
                 const T4&     v4)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', 'v2', 'v3', and 'v4' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputs(names[2], stdout);
    printTestingValue(v3);
    fputs(names[3], stdout);
    printTestingValue(v4);
    fputc('\n', stdout);
}

template <class T1, class T2, class T3, class T4, class T5>
void assertPrint(const char* (&names)[5],
                 const T1&     v1,
                 const T2&     v2,
                 const T3&     v3,
                 const T4&     v4,
                 const T5&     v5)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', 'v2', 'v3', 'v4', and 'v5' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputs(names[2], stdout);
    printTestingValue(v3);
    fputs(names[3], stdout);
    printTestingValue(v4);
    fputs(names[4], stdout);
    printTestingValue(v5);
    fputc('\n', stdout);
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
void assertPrint(const char* (&names)[6],
                 const T1&     v1,
                 const T2&     v2,
                 const T3&     v3,
                 const T4&     v4,
                 const T5&     v5,
                 const T6&     v6)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', 'v2', 'v3', 'v4', 'v5', and 'v6' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputs(names[2], stdout);
    printTestingValue(v3);
    fputs(names[3], stdout);
    printTestingValue(v4);
    fputs(names[4], stdout);
    printTestingValue(v5);
    fputs(names[5], stdout);
    printTestingValue(v6);
    fputc('\n', stdout);
}

template <class T1, class T2, class T3, class T4, class T5, class T6,
          class T7>
void assertPrint(const char* (&names)[7],
                 const T1&     v1,
                 const T2&     v2,
                 const T3&     v3,
                 const T4&     v4,
                 const T5&     v5,
                 const T6&     v6,
                 const T7&     v7)
    // Print the specified 'names' followed by their corresponding specified
    // 'v1', 'v2', 'v3', 'v4', 'v5', 'v6', and 'v7' values to 'stdout'.
{
    fputs(names[0], stdout);
    printTestingValue(v1);
    fputs(names[1], stdout);
    printTestingValue(v2);
    fputs(names[2], stdout);
    printTestingValue(v3);
    fputs(names[3], stdout);
    printTestingValue(v4);
    fputs(names[4], stdout);
    printTestingValue(v5);
    fputs(names[5], stdout);
    printTestingValue(v6);
    fputs(names[6], stdout);
    printTestingValue(v7);
    fputc('\n', stdout);
}

}  // close unnamed namespace

//=============================================================================
//                           TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bsls::BslSourceNameParserUtil Util;

//=============================================================================
//                     GLOBAL VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;

//=============================================================================
//                      GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace {
                              // ===============
                              // class SubString
                              // ===============

class SubString {
    // A not null terminated reference string type whose value is defined by a
    // pointer into another string and the number of characters it contains.
    // This simple type is used to be able easily compare, for equality,
    // substrings with C (null-terminated) strings and to easily print
    // substrings when using the 'ASSERTV' and 'P' test facilities.  Note that
    // this is a very simple type, not a template, and it uses C facilities for
    // printing and comparison so it can be used anywhere in 'bsl'.  Also note
    // that the unlike the standard string view this type does not support
    // embedded null characters!

  private:
    // DATA
    const char *d_begin_p;
    size_t      d_length;

  public:
    // CREATORS
    explicit SubString(const char *begin, size_t length);
        // Create a new 'SubString' object using the specified 'begin' and
        // 'length'.  The behavior is undefined if any of the 'length' count of
        // character starting at 'begin' is the null character.

    template <size_t LENGTH_WITH_NULL_TERMINATOR>
    explicit
    SubString(const char (&literal)[LENGTH_WITH_NULL_TERMINATOR]);
        // Create a new 'SubString' object from the specified C string
        // 'literal', not including its closing null character.

    // ACCESSORS
    const char *begin() const;
        // Return a pointer to the first character of the substring of this
        // object.

    size_t length() const;
        // Return the number of characters in the substring of this object.
};

                              // ---------------
                              // class SubString
                              // ---------------

// CREATORS
SubString::SubString(const char *begin, size_t length)
: d_begin_p(begin)
, d_length(length)
{
}

template <size_t LENGTH_WITH_NULL_TERMINATOR>
SubString::SubString(const char(&literal)[LENGTH_WITH_NULL_TERMINATOR])
: d_begin_p(literal)
, d_length(LENGTH_WITH_NULL_TERMINATOR - 1)
{
}

// ACCESSORS
const char *SubString::begin() const
{
    return d_begin_p;
}

size_t SubString::length() const
{
    return d_length;
}


// FREE OPERATORS
inline
bool operator==(const SubString& lhs, const SubString& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to the same number
    // of characters, and that sequence of characters have equal value.  Return
    // 'false' if 'lhs' and 'rhs' refer to different number of characters or if
    // any of those characters in the sequence is different.
{
    return lhs.length() == rhs.length()
        && 0 == memcmp(lhs.begin(), rhs.begin(), rhs.length());
}

inline
bool operator==(const SubString& lhs, const char *rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to the same number
    // of characters, and that sequence of characters have equal value.  Return
    // 'false' if 'lhs' and 'rhs' refer to different number of characters or if
    // any of those characters in the sequence is different.
{
    return lhs.length() == strlen(rhs)
        && 0 == strncmp(lhs.begin(), rhs, lhs.length());
}

inline
bool operator==(const char *lhs, const SubString& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to the same number
    // of characters, and that sequence of characters have equal value.  Return
    // 'false' if 'lhs' and 'rhs' refer to different number of characters or if
    // any of those characters in the sequence is different.
{
    return rhs == lhs;
}

#ifdef BDE_VERIFY
#pragma bde_verify push
#pragma bde_verify -CC01   // C-style cast is used
#endif
inline
bool operator!=(const SubString& lhs, const SubString& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to different number
    // of characters, or any two corresponding characters in those sequences
    // differ.  Return 'false' if 'lhs' and 'rhs' refer to the same number of
    // characters and those characters are the same in the same sequence.
{
    // this function may be unused but it is necessary for completeness
    (void)(bool(*)(const SubString&, const SubString&))& operator!=;

    return !(lhs == rhs);
}

inline
bool operator!=(const SubString& lhs, const char* rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to different number
    // of characters, or any two corresponding characters in those sequences
    // differ.  Return 'false' if 'lhs' and 'rhs' refer to the same number of
    // characters and those characters are the same in the same sequence.
{
    // this function may be unused but it is necessary for completeness
    (void)(bool(*)(const SubString&, const char*))& operator!=;

    return !(lhs == rhs);
}

inline
bool operator!=(const char* lhs, const SubString& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' refer to different number
    // of characters, or any two corresponding characters in those sequences
    // differ.  Return 'false' if 'lhs' and 'rhs' refer to the same number of
    // characters and those characters are the same in the same sequence.
{
    // this function may be unused but it is necessary for completeness
    (void)(bool(*)(const char*, const SubString&))& operator!=;

    return !(rhs == lhs);
}
#ifdef BDE_VERIFY
#pragma bde_verify pop
#endif

// ASPECT FREE FUNCTIONS
static
inline
void printTestingValue(const SubString& object)
    // Print the string referred by the specified 'object' to the 'stdout'
    // stream, in quotes.
{
    printf("\"%.*s\"", static_cast<int>(object.length()), object.begin());
}

                          // =======================
                          // class SourcenameBuilder
                          // =======================

class SourcenameBuilder {
    // This non-copyable helper class is used to maintain the buffer for, and
    // assemble a source file name for testing 'getComponentName.

  private:
    // DATA
    char    d_buffer[129];

    size_t  d_namePos;
    size_t  d_extPos;

    size_t  d_cpp03SuffixPos;

  public:
    // CREATORS
    SourcenameBuilder();
        // Create an empty 'SourcenameBuilder' object.  Note that the behavior
        // is undefined unless the 'reset' method is called next to set a
        // value, or the object is destroyed.

    // MANIPULATORS
#ifdef BSLS_PLATFORM_OS_WINDOWS
    bool convertToWindowsPathOnWindows();
        // If the current source file name string in the buffer of this object
        // has non-empty UNIX style path (that ends with a '/') replace its
        // path separators with backslash to test them as a Windows path.  If
        // a replacement was made return 'true'.  Otherwise, if the path was
        // empty or not a UNIX path, return 'false'.
#endif

    bool insertCpp03Suffix();
        // Insert to the end of the component name part in the buffer of this
        // object the "_cpp03" suffix.  Return 'false' if the result would be
        // too long.

    bool reset(const char *path, const char *name, const char *exts);
        // Create, in this object's buffer, the full source file name
        // (optionally with path) from the specified 'path', 'name', and 'exts'
        // (extensions).  Note that the buffers content is overwritten.  Return
        // 'false' if the name would not fit into the available buffer.

    // ACCESSORS
    const char *sourceName() const;
        // Return a pointer to the beginning of the current source file name
        // buffer of this object.
};

                          // -----------------------
                          // class SourcenameBuilder
                          // -----------------------
// CREATORS
inline
SourcenameBuilder::SourcenameBuilder()
: d_namePos(0)
, d_extPos(0)
, d_cpp03SuffixPos(0)
{
    *d_buffer = 0;  // Make it an empty C string
}

// MANIPULATORS
#ifdef BSLS_PLATFORM_OS_WINDOWS
bool SourcenameBuilder::convertToWindowsPathOnWindows()
{
    // Do nothing for empty path, or non-UNIX path (so we don't repeat tests)
    if (0 == d_namePos || '/' != d_buffer[d_namePos - 1]) {
        return false;                                                 // RETURN
    }

    d_buffer[d_namePos - 1] = '\\';  // Update last separator before the name
    for (size_t i = d_namePos - 1; i; --i) {
        if ('/' == d_buffer[i - 1]) {
            d_buffer[i - 1] = '\\';
        }
    }
    return true;
}
#endif

bool SourcenameBuilder::insertCpp03Suffix()
{
    static const char k_SUFFIX[] = "_cpp03";
    const size_t k_LENGTH   = (sizeof k_SUFFIX) - 1;

    const size_t k_STRLEN = strlen(d_buffer);
    if (k_STRLEN + k_LENGTH > sizeof(d_buffer) - 1) {
        return false;                                                 // RETURN
    }

    // Calculate the number of characters we need to move up
    const size_t k_EXTLEN = k_STRLEN - d_extPos;

    // The suffix will start where the extensions was began before
    d_cpp03SuffixPos = d_extPos;

    // The extension moves up to make space for the suffix
    d_extPos += k_LENGTH;

    // Making space, move "up" the extension and the closing null character
    memmove(d_buffer + d_extPos, d_buffer + d_cpp03SuffixPos, k_EXTLEN + 1);

    // Copy in the suffix
    memcpy(d_buffer + d_cpp03SuffixPos, k_SUFFIX, k_LENGTH);

    return true;
}

bool SourcenameBuilder::reset(const char *path,
                              const char *name,
                              const char *exts)
{
    size_t pl = strlen(path);
    size_t nl = strlen(name);
    size_t el = strlen(exts);

    d_buffer[0] = 0;

    if (pl + nl + el + 1 > sizeof(d_buffer) - 1) {
        // Would not fit.
        return false;                                                 // RETURN
    }

    strcpy(d_buffer, path);
    strcat(d_buffer, name);
    strcat(d_buffer, exts);

    d_namePos = pl;
    d_extPos  = d_namePos + nl;

    return true;
}

// ACCESSORS
const char *SourcenameBuilder::sourceName() const
{
    return d_buffer;
}

}  // close unnamed namespace

//=============================================================================
//                          HELPER FREE FUNCTIONS
//-----------------------------------------------------------------------------

static
inline
bool isIn(const char *p, const char *buffer, size_t length)
    // Return 'true' if the specified 'p' points into the specified 'buffer' of
    // the specified 'length', and return 'false' otherwise.
{
    return p >= buffer && p < buffer + length;
}

static
inline
bool isIn(const char *s, size_t sLen, const char *buffer, size_t length)
    // Return 'true' if the character sequence starting at the specified 's',
    // and consisting of the specified 'sLen' number of characters fully
    // resides withing the specified 'buffer' of the specified 'length', and
    // 'false' otherwise.
{
    return isIn(s, buffer, length) && isIn(s + sLen, buffer, length);
}

static
inline
bool isSafe(const char *p, size_t len, const char *s1, const char *s2)
    // Determines if the string specified by 'p' and 'len' is within the
    // specified 's1' or 's2' null terminated strings, excluding the null
    // terminator character.  Used to check if it is safe to print 'p'.
{
    return isIn(p, len, s1, strlen(s1)) || isIn(p, len, s2, strlen(s2));
}

//=============================================================================
//                        TESTER FREE FUNCTIONS
//-----------------------------------------------------------------------------

static
void verifyValidSourceParse(int                 PATH_LINE,
                            int                 NAME_LINE,
                            int                 EXTS_LINE,
                            int                 CALLER_LINE,
                            const char         *TESTED_FILENAME,
                            const char         *EXPECTED_COMPONENT_NAME,
                            unsigned            EXPECTED_TYPE)
    // Execute and verify against the specified 'EXPECTED_COMPONENT_NAME' and
    // 'EXPECTED_TYPE' values the parsing of the specified valid, BDE-style
    // source 'TESTED_FILENAME' using 'bsls::AssertTest_ParseFilename'.  Use
    // the specified 'PATH_LINE', 'NAME_LINE', and 'EXTS_LINE' line numbers in
    // assertions to refer to the source code line number for test data table
    // row number source for the path, name, and extension(s) that contributed
    // to 'TESTED_FILENAME', and the specified 'CALLER_LINE' line number that
    // called this function (it is called several times with the same input but
    // input but slightly different 'TESTED_FILENAME').
{
#define ASSERTL(X)                                                            \
    ASSERT5(PATH_LINE, NAME_LINE, EXTS_LINE, CALLER_LINE, TESTED_FILENAME, X)

#define ASSERTL2(I, J, X)                                                     \
    ASSERT7(PATH_LINE, NAME_LINE, EXTS_LINE, CALLER_LINE, TESTED_FILENAME,    \
            I, J, X)

    if (veryVerbose) printf("Verifying \"%s\"\n", TESTED_FILENAME);

    static const char COMPONENT_DEFAULT[] = "????";
    const char *componentPtr = COMPONENT_DEFAULT;
    size_t      componentLen = (sizeof COMPONENT_DEFAULT) - 1;
    unsigned    srcType      = ~0u;
    const int retval = Util::getComponentName(&componentPtr,
                                              &componentLen,
                                              TESTED_FILENAME,
                                              &srcType);
    ASSERTL(0 == retval);

    ASSERTL2(EXPECTED_TYPE, srcType, EXPECTED_TYPE == srcType);

    const SubString componentName =
        isSafe(componentPtr, componentLen, TESTED_FILENAME, COMPONENT_DEFAULT)
        ? SubString(componentPtr, componentLen)
        : SubString(COMPONENT_DEFAULT);

    ASSERTL2(EXPECTED_COMPONENT_NAME,   componentName,
             EXPECTED_COMPONENT_NAME == componentName);
#undef ASSERTL2
#undef ASSERTL
}

static
void verifyInvalidSourceParse(int                 BAD_NAME_LINE,
                              int                 PATH_LINE,
                              const char         *TESTED_BAD_FILENAME,
                              int                 EXPECTED_RETURN)
    // Attempt to parse the specified 'TESTED_BAD_FILENAME' using
    // 'bsls::AssertTest_ParseFilename' and verify that the parsing result is
    // the specified 'EXPECTED_RETURN' code.  Use the specified 'BAD_NAME_LINE'
    // and 'PATH_LINE' in assertions to refer to the test data table row for
    // 'TESTED_BAD_FILENAME'.
{
#define ASSERTL2(I, J, X)                                                     \
    ASSERT5(BAD_NAME_LINE, PATH_LINE, TESTED_BAD_FILENAME, I, J, X)

    if (veryVerbose) printf("Verifying \"%s\"\n", TESTED_BAD_FILENAME);

    const char *componentPtr;
    size_t      componentLen;
    const int retval = Util::getComponentName(&componentPtr,
                                              &componentLen,
                                              TESTED_BAD_FILENAME);
    ASSERTL2(EXPECTED_RETURN, retval, EXPECTED_RETURN == retval);
#undef ASSERTL2
}


//=============================================================================
//                            USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace Usage {

void example1()
///Example 1: Determining Component Name from Source File Name
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
{
// Suppose we need to determine the component name from one of its source file
// names.  To ease understanding we use string literals for source file names.
//
// First, we declare the result variables that the parser will fill:
//..
    const char *componentStart  = 0;
    size_t      componentLength = 0;
//..
// Next, we call the parser, saving its return value:
//..
    int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                          &componentStart,
                                          &componentLength,
                                          "groups/abc/abcx/abcx_name_cpp03.h");
//..
// Now, we verify that the parsing was successful:
//..
    ASSERT(0 == returnCode);
//..
// Finally, we can verify that the expected component name is found:
//..
    ASSERT(9 == componentLength &&
           0 == memcmp("abcx_name", componentStart, 9));
//..
// Notice how the "_cpp03" suffix of the generated file has been removed.
}


void example2()
///Example 2: Determining the Type of a Source File
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to determine, in addition to the component name, what kind
// source file name do we have.  To ease understanding we use string literals
// for source file names.
{
// First, we declare the result variables that the parser will fill:
//..
    const char *componentStart  = 0;
    size_t      componentLength = 0;
    unsigned    sourceType      = ~0u;
//..
// Next, we call the parser with the first name, passing the address of the
// optional output variable after the source file name:
//..
    int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                           &componentStart,
                                           &componentLength,
                                           "groups/abc/abcx/abcx_name_cpp03.h",
                                           &sourceType);
//..
// Then, we verify that the parsing was successful, and the expected component
// name is found:
//..
    ASSERT(0 == returnCode);
    ASSERT(9 == componentLength &&
           0 == memcmp("abcx_name", componentStart, 9));
//..
// Next, we verify the determined source file type by examining the "kind",
// stored in the bits masked by 'bsls::BslSourceNameParserUtil::k_MASK_KIND',
// and the flags stored in other bits:
//..
    typedef bsls::BslSourceNameParserUtil Util;  // For brevity

    ASSERT(Util::k_HEADER == (sourceType & Util::k_MASK_KIND));

    ASSERT(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
    ASSERT(0 == (sourceType & Util::k_IS_SUBORDINATE_TEST));
    ASSERT(0 != (sourceType & Util::k_IS_CPP03_GENERATED ));
//..
// Then, we can verify a subordinate test component implementation file name.
// These names, and also headers for subordinate test components are special as
// they are not supposed to contain executable code.  They are just another
// test driver for their main component.
//..
    returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                        &componentStart,
                                        &componentLength,
                                        "groups/abc/abcx/abcx_name_test12.cpp",
                                        &sourceType);

    ASSERT(0 == returnCode);
    ASSERT(9 == componentLength &&
           0 == memcmp("abcx_name", componentStart, 9));
//..
// Note that the main component name is reported.
//..
    ASSERT(Util::k_IMPL == (sourceType & Util::k_MASK_KIND));

    ASSERT(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
    ASSERT(0 != (sourceType & Util::k_IS_SUBORDINATE_TEST));
    ASSERT(0 == (sourceType & Util::k_IS_CPP03_GENERATED ));
//..
// Now, we verify a traditional test driver file name of a subordinate test
// component:
//..
    returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                      &componentStart,
                                      &componentLength,
                                      "groups/abc/abcx/abcx_name_test12.t.cpp",
                                      &sourceType);

    ASSERT(0 == returnCode);
    ASSERT(9 == componentLength &&
           0 == memcmp("abcx_name", componentStart, 9));

    ASSERT(Util::k_TTEST == (sourceType & Util::k_MASK_KIND));

    ASSERT(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
    ASSERT(0 != (sourceType & Util::k_IS_SUBORDINATE_TEST));
    ASSERT(0 == (sourceType & Util::k_IS_CPP03_GENERATED ));
//..
// Finally, we verify a multi-file test driver source:
//..
    returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                                    &componentStart,
                                                    &componentLength,
                                                    "wxya_other_cpp03.0.g.cpp",
                                                    &sourceType);

    ASSERT(0 == returnCode);
    ASSERT(10 == componentLength &&
           0 == memcmp("wxya_other", componentStart, 10));

    ASSERT(Util::k_GTEST == (sourceType & Util::k_MASK_KIND));

    ASSERT(0 != (sourceType & Util::k_IS_MULTIFILE_TEST  ));
    ASSERT(0 == (sourceType & Util::k_IS_SUBORDINATE_TEST));
    ASSERT(0 != (sourceType & Util::k_IS_CPP03_GENERATED ));
//..
}

void example3()
///Example 3: Reporting Parsing Errors
///- - - - - - - - - - - - - - - - - -
// Suppose we need to parse source file names from an external source, and
// therefore we may need to report the reason for parsing failures for human
// readers (of log files).  To ease understanding we use string literals for
// source file names.
{
// First, we declare the result variables that the parser will fill:
//..
    const char *componentStart  = 0;
    size_t      componentLength = 0;
//..
// Next, we can call the parser with a too short file name and save the return
// value:
//..
    int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                                              &componentStart,
                                                              &componentLength,
                                                              "a.h");
//..
// Then, we verify that the parsing has failed:
//..
    ASSERT(0 != returnCode);
//..
// Next, we output a brief error message to the user if requested:
//..
    if (verbose) {
        printf("Error parsing source file name \"%s\": %s\n",
               "a.h",
               bsls::BslSourceNameParserUtil::errorMessage(returnCode));
        // Output will indicate the file name was too short (to be a BDE name)
    }
//..
// Now, we demonstrate another failing-to-parse source name and its error
// message:
//..
    returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                                            &componentStart,
                                                            &componentLength,
                                                            "abcxyz_name.hpp");
    ASSERT(0 != returnCode);
    if (verbose) {
        printf("Error parsing source file name \"%s\": %s\n",
               "abcxyz_name.hpp",
               bsls::BslSourceNameParserUtil::errorMessage(returnCode));
        // Output will indicate an unsupported extension
    }
//..
// Finally, we demonstrate the "missing test driver tag" error:
//..
    returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
                                                           &componentStart,
                                                           &componentLength,
                                                           "abcx_name..t.cpp");
    ASSERT(0 != returnCode);
    if (verbose) {
        printf("Error parsing source file name \"%s\": %s\n",
               "abcx_name..t.cpp",
               bsls::BslSourceNameParserUtil::errorMessage(returnCode));
        // Output will indicate two dots next to each other in the file name
    }
//..
}
}  // close namespace Usage

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
             verbose = argc > 2;
         veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Replace 'assert' with 'ASSERT' and silence output unless a
        //:   verbosity flag is set.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        Usage::example1();
        Usage::example2();
        Usage::example3();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ERROR MESSSAGES
        //
        // Concerns:
        //:  1 The possible error return code translate to text messages.
        //:
        //
        // Plan:
        //:  1 White box testing to verify all codes.
        //:
        // Testing:
        //   const char *errorMessage(int errorCode);
        // --------------------------------------------------------------------

        if (verbose) puts("\nERROR MESSAGES"
                          "\n==============");

        ASSERT1(Util::errorMessage(0),
                0 != strstr(Util::errorMessage(0), "success"));

        ASSERT1(Util::errorMessage(-1),
                0 != strstr(Util::errorMessage(-1), "short"));

        ASSERT1(Util::errorMessage(-2),
                0 != strstr(Util::errorMessage(-2), "extension"));

        ASSERT1(Util::errorMessage(-3),
                0 != strstr(Util::errorMessage(-3), "two"));

        ASSERT1(Util::errorMessage(-4),
                0 != strstr(Util::errorMessage(-4), "unknown"));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SOURCE FILE NAME PARSING
        //
        // Concerns:
        //:  1 Path portion, if exists, is not included in component name.
        //:
        //:  2 Path portion is not required for a successful parsing.
        //:
        //:  3 For successful parsing the package name prefix is included in
        //:    the reported component name.
        //:
        //:  4 The optional '_cpp03' suffix is removed from the component name
        //:    if present as the last suffix.
        //:
        //:  5 Normal (not subordinate or package private) component source and
        //:    test driver file names are parsed correctly, the result
        //:    indicating their type, and the correct component name.
        //:
        //:  6 Package private component source and test driver file names are
        //:    parsed correctly, the result indicating their type and the
        //:    correct component name with all private suffixes present.
        //:
        //:  7 Subordinate test component source and test driver file names are
        //:    parsed correctly, the result indicating their type and the
        //:    correct component name without the '_test[a-z0-9]*' suffix.
        //:
        //:  8 Both BDE traditional and gtest test drivers are properly
        //:    detected as test drivers, and their component names properly
        //:    reported.
        //:
        //:  9 Multi-file test driver file names are properly parsed for both
        //:    styles of test drivers (traditional and gtest), with the part
        //:    extension (e.g., ".00.t.cpp") fully removed from the reported
        //:    component name.
        //:
        //: 10 Parsing errors are reported as expected:
        //:    1 File names less than 8 character long are rejected.
        //:    2 Unknown (not '.h'. '.cpp') extensions are rejected.
        //:    3 Missing test driver part (number) ("abc..t.cpp") is rejected.
        //
        // Plan:
        //:  1 Create a set of valid component names with different prefixes:
        //:    normal package prefix, and additional single character prefix
        //:    ("a_" - adapter, "m_" - executable, "z_" -wrapper library, an so
        //:    on).  Component base names ranging from one to several
        //:    characters long.  Different suffixes: subordinate test component
        //:    suffix with and without additional characters after '_test',
        //:    package private component suffixes and combination of the two.
        //:    Do *not* use the '_cpp03' suffix, we repeat every test by adding
        //:    '_cpp03' to the end of the base name.  This table also contains
        //:    the expected component name, and a 'bool' that tells if it is a
        //:    subordinate test component name.
        //:
        //:  2 Use the following extensions: '.h', '.cpp', and test driver
        //:    extensions '.t.cpp' and '.g.cpp'.  This table will contain the
        //:    expected tag value (see 'AssertTest_ParseFilename::tag()').
        //:
        //:  3 Create a set of valid path portions, including the empty path
        //:    (while all non-empty path ending with the path delimiter)
        //:    appropriate for the target platform (Windows or Unix
        //:    style), including but not limited to empty path, absolute and
        //:    relative path, UNC path on Windows.  (Beware that programs
        //:    compiled for Windows may use forward slash as a path delimiter
        //:    so all Unix path should also be tested on Windows.)
        //:
        //:  4 In nested loops:
        //:    1 Combine the above parts into a source file name to parse
        //:
        //:    2 Parse and verify the expected result (name and tag)
        //:
        //:    3 Add '_cpp03' to the end of the component name part in
        //:      the source file name and verify that the parse result is the
        //:      same.
        //:
        //:    4 On Windows also verify all non-empty Unix path (that ends with
        //:      '/') by replacing all slash characters with backslash.
        //:
        //:  5 Create a table of malformed source file names to verify concerns
        //:    of C-16 in a loop.
        //
        // Testing:
        //   int getComponentName(p, l, srcname, kind);
        // --------------------------------------------------------------------

        if (verbose) puts("\nSOURCE FILE NAME PARSING"
                          "\n========================");

        if (verbose) puts("\nVALID FILE NAME PARSING"
                          "\n=======================");

        static const struct PathTestData {
            int         d_line;
            const char *d_path;
        } k_PATH_DATA[] = {
            //LINE Path to Source
            //---- --------------------
            { L_,  ""                   }, // no path is valid
            { L_,  "/"                  },
            { L_,  "../"                },
            { L_,  "/abc_123/"          },
            { L_,  "./abc_123/"         },
            { L_,  "//server/path/"     },  // Unix UNC
#ifdef BSLS_PLATFORM_OS_WINDOWS
            { L_,  "c:"                 },
            { L_,  "c:\\"               },
            { L_,  "c:\\abc_123\\"      },
            { L_,  "c:\\abc_123\\"      },
#endif
        };
        const size_t k_NUM_PATH = sizeof k_PATH_DATA / sizeof *k_PATH_DATA;

        static const struct NameTestData {
            int         d_line;
            const char *d_baseName;
            const char *d_expComponent;  // expected component name
            bool        d_testComponent;
        } k_NAME_DATA[] = {
            //                              Expected                 Is SubTest
            //LINE File Name Part           Component Name           Component
            //---- ----------------         ------------------------ -------
            { L_, "abcx_a",                 "abcx_a",                 false },
            { L_, "abcxy_a",                "abcxy_a",                false },
            { L_, "abcxyz_a",               "abcxyz_a",               false },

            { L_, "abcx_component",         "abcx_component",         false },
            { L_, "abcxy_component",        "abcxy_component",        false },
            { L_, "abcxyz_component",       "abcxyz_component",       false },

            // Verify additional single character suffix works
            { L_, "x_abcx_thing",           "x_abcx_thing",           false },

            // Verify package private component names
            { L_, "abcx_it_be",             "abcx_it_be",             false },
            // The following names do not *end* with the special
            // "_cpp03" or R"_test[0-9a-z]*" suffixes, therefore
            // those are part of the (arguably bad) component name.
            { L_, "abcx_it_test_be",        "abcx_it_test_be",        false },
            { L_, "abcx_it_test1_be",       "abcx_it_test1_be",       false },
            { L_, "abcx_it_cpp03_be",       "abcx_it_cpp03_be",       false },
            { L_, "abcx_it_test_cpp03_be",  "abcx_it_test_cpp03_be",  false },
            { L_, "abcx_it_cpp03_test_be",  "abcx_it_cpp03_test_be",  false },
            { L_, "abcx_it_test2_cpp03_be", "abcx_it_test2_cpp03_be", false },
            { L_, "abcx_it_cpp03_test2_be", "abcx_it_cpp03_test2_be", false },

            // Subordinate Test Components
            { L_, "abcx_biggy_test",        "abcx_biggy",             true  },
            { L_, "abcx_biggy_test1",       "abcx_biggy",             true  },
            { L_, "abcx_biggy_test42",      "abcx_biggy",             true  },
            { L_, "abcx_biggy_testzz",      "abcx_biggy",             true  },
        };
        const size_t k_NUM_NAMES = sizeof k_NAME_DATA / sizeof *k_NAME_DATA;

        static const struct ExtensionTestData {
            int         d_line;
            const char *d_extension;
            char        d_expectedType;  // if not subord. test component src.
        } k_EXT_DATA[] = {
            //LINE Extension    Type
            //---- -----------------
            { L_, ".h",         Util::k_HEADER },
            { L_, ".cpp",       Util::k_IMPL   },

            { L_, ".t.cpp",     Util::k_TTEST  },
            { L_, ".g.cpp",     Util::k_GTEST  },

            // Multi-part
            { L_, ".0.t.cpp",   Util::k_TTEST | Util::k_IS_MULTIFILE_TEST },
            { L_, ".0.g.cpp",   Util::k_GTEST | Util::k_IS_MULTIFILE_TEST},
            { L_, ".9.t.cpp",   Util::k_TTEST | Util::k_IS_MULTIFILE_TEST },

            { L_, ".00.t.cpp",  Util::k_TTEST | Util::k_IS_MULTIFILE_TEST },
            { L_, ".99.g.cpp",  Util::k_GTEST | Util::k_IS_MULTIFILE_TEST },
        };
        const size_t k_NUM_EXT_DATA = sizeof k_EXT_DATA / sizeof *k_EXT_DATA;

        // Create the builder outside of the loops so it can "remember" the
        // longest string (source file name) length and so does a lot less
        // allocations.
        SourcenameBuilder builder;

        typedef const char *CStrPtr;

        for (size_t i = 0; i < k_NUM_NAMES; ++i) {
            const NameTestData& nameData = k_NAME_DATA[i];

            const int     NAME_LINE               = nameData.d_line;
            const CStrPtr FILE_NAME               = nameData.d_baseName;
            const CStrPtr EXPECTED_COMPONENT_NAME = nameData.d_expComponent;
            const bool    IS_TEST_COMPONENT       = nameData.d_testComponent;

            for (size_t j = 0; j < k_NUM_EXT_DATA; ++j) {
                const ExtensionTestData& extData = k_EXT_DATA[j];

                const int      EXTS_LINE       = extData.d_line;
                const CStrPtr  EXTS            = extData.d_extension;
                const unsigned EXPECTED_TYPE_P = extData.d_expectedType;

                const unsigned EXPECTED_TYPE =
                                IS_TEST_COMPONENT
                                ? EXPECTED_TYPE_P | Util::k_IS_SUBORDINATE_TEST
                                : EXPECTED_TYPE_P;

                for (size_t k = 0; k < k_NUM_PATH; ++k) {
                    const PathTestData& pathData = k_PATH_DATA[k];

                    const int     PATH_LINE  = pathData.d_line;
                    const CStrPtr PATH       = pathData.d_path;

                    if (veryVerbose) {
                        P_(NAME_LINE) P(FILE_NAME);
                        P_(EXTS_LINE) P(EXTS);
                        P_(PATH_LINE) P(PATH);
                        P_(EXPECTED_COMPONENT_NAME) P_(IS_TEST_COMPONENT)
                                                              P(EXPECTED_TYPE);
                    }

#define VERIFY_SOURCE_PARSE_FROM(exp_type)                                    \
    verifyValidSourceParse(PATH_LINE,  NAME_LINE,  EXTS_LINE, L_,             \
                           builder.sourceName(),                              \
                           EXPECTED_COMPONENT_NAME,                           \
                           exp_type)

                    builder.reset(PATH, FILE_NAME, EXTS);
                    VERIFY_SOURCE_PARSE_FROM(EXPECTED_TYPE);

                    builder.insertCpp03Suffix();
                    VERIFY_SOURCE_PARSE_FROM(
                                   EXPECTED_TYPE | Util::k_IS_CPP03_GENERATED);

#ifdef BSLS_PLATFORM_OS_WINDOWS
                    if (builder.convertToWindowsPathOnWindows()) {
                        builder.reset(PATH, FILE_NAME, EXTS);
                        VERIFY_SOURCE_PARSE_FROM(EXPECTED_TYPE);

                        builder.insertCpp03Suffix();
                        VERIFY_SOURCE_PARSE_FROM(
                                   EXPECTED_TYPE | Util::k_IS_CPP03_GENERATED);
                    }
#endif
                }
            }
        }

        if (verbose) puts("\nUNPARSABLE FILE NAMES"
                          "\n=====================");
        enum {
            k_TOO_SHORT = -1,  // "gggp_c.h" -- minimum 8 characters
            k_BAD_EXT   = -2,  // not ".h", or ".cpp"
            k_NO_PART   = -3   // "pref_comp..t.cpp", two consecutive dots
        };

        static const struct BadTestData {
            int         d_line;
            const char *d_badName;
            int         d_returnCode;
        } k_BAD_DATA[] = {
            //LINE Non-parsable Name  Return Code
            //---- -----------------  -----------
            { L_,  "1",               k_TOO_SHORT },
            { L_,  "12",              k_TOO_SHORT },
            { L_,  "123",             k_TOO_SHORT },
            { L_,  "1234",            k_TOO_SHORT },
            { L_,  "12345",           k_TOO_SHORT },
            { L_,  "123456",          k_TOO_SHORT },
            { L_,  "1234567",         k_TOO_SHORT },
            { L_,  "12345678",        k_BAD_EXT   },

            // We verify the working minimum as well
            // (to ensure the test code system works)
            { L_,  "gggp_c.h",        0           },

            // Bad 1 char extensions
            { L_,  "gggp_c.a",        k_BAD_EXT   },
            { L_,  "gggp_c.c",        k_BAD_EXT   },
            { L_,  "gggp_c.s",        k_BAD_EXT   },
            { L_,  "gggp_c.9",        k_BAD_EXT   },

            // Bad 2 char extensions
            { L_,  "gggp_c.ab",       k_BAD_EXT   },
            { L_,  "gggp_c.cp",       k_BAD_EXT   },
            { L_,  "gggp_c.sa",       k_BAD_EXT   },
            { L_,  "gggp_c.93",       k_BAD_EXT   },

            // Bad 3 char extensions
            { L_,  "gggp_c.bas",      k_BAD_EXT   },
            { L_,  "gggp_c.pas",      k_BAD_EXT   },

            // Verify the other working extension(s)
            { L_,  "gggp_c.cpp",      0           },
            { L_,  "gggp_c.t.cpp",    0           },
            { L_,  "gggp_c.g.cpp",    0           },

            { L_,  "gggp_c..t.cpp",   k_NO_PART   },
            { L_,  "gggp_c..g.cpp",   k_NO_PART   },

            // Verify valid minimum part id (number)
            { L_,  "gggp_c.0.t.cpp",  0           },
            { L_,  "gggp_c.0.g.cpp",  0           },
        };
        const size_t k_NUM_BAD_DATA = sizeof k_BAD_DATA / sizeof * k_BAD_DATA;

        for (size_t i = 0; i < k_NUM_BAD_DATA; ++i) {
            const BadTestData& badData = k_BAD_DATA[i];

            const int     BAD_NAME_LINE   = badData.d_line;
            const CStrPtr BAD_NAME        = badData.d_badName;
            const int     EXPECTED_RETURN = badData.d_returnCode;

            // Verify that the existence of a valid path won't "fix" a bad name
            for (size_t k = 0; k < k_NUM_PATH; ++k) {
                const PathTestData& pathData = k_PATH_DATA[k];

                const int     PATH_LINE  = pathData.d_line;
                const CStrPtr PATH       = pathData.d_path;

                if (veryVerbose) {
                    P_(BAD_NAME_LINE) P(BAD_NAME);
                    P_(PATH_LINE) P(PATH);
                }

                builder.reset(PATH, BAD_NAME, "");
                verifyInvalidSourceParse(BAD_NAME_LINE,
                                         PATH_LINE,
                                         builder.sourceName(),
                                         EXPECTED_RETURN);
            }
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
        //: 1 Verify parsing results of a sufficiently complex source file name
        //:   with path.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");
        {
            const char SRC_NAME[] =
                "/home/user/ws/bde/groups/xyz/xyzab/xyzab_that.42.g.cpp";
            const char* SRC_END = SRC_NAME + sizeof(SRC_NAME) - 1;

            const char  *COMP_DEFAULT = "**ERROR**";
            const char *componentName = COMP_DEFAULT;
            size_t      length        = strlen(componentName);
            unsigned    srcType       = ~0;
            int         rv            = -99;
            ASSERT1(rv, 0 == (rv = Util::getComponentName(&componentName,
                                                          &length,
                                                          SRC_NAME,
                                                          &srcType)));

            const char *safeComponentName =
                (componentName > SRC_NAME && componentName < SRC_END) ||
                COMP_DEFAULT == componentName ? componentName : "???";
            ASSERT1(safeComponentName,
                    componentName > SRC_NAME && componentName < SRC_END &&
                                 0 == memcmp("xyzab_that", componentName, 10));

            ASSERT1(length,     10  == length);
            ASSERT1(srcType, 0 != (srcType & Util::k_GTEST));
            ASSERT1(srcType, 0 != (srcType & Util::k_IS_MULTIFILE_TEST));

            ASSERT1(srcType, 0 == (srcType & Util::k_IS_CPP03_GENERATED));
            ASSERT1(srcType, 0 == (srcType & Util::k_IS_SUBORDINATE_TEST));
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
// Copyright 2022 Bloomberg Finance L.P.
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
