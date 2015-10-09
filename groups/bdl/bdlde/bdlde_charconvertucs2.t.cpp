// bdlde_charconvertucs2.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlde_charconvertucs2.h>

#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bsls_stopwatch.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// Exercise boundary cases for both of the conversion mappings as well as
// handling of buffer capacity issues.
//-----------------------------------------------------------------------------
// [ 3] utf8ToUcs2
// [ 4] ucs2ToUtf8
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 2] Enumeration
// [ 5] USAGE EXAMPLE 1
// [ 6] USAGE EXAMPLE 2
// [ 7] USAGE EXAMPLE 3

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

// ============================================================================
//                           CUSTOM TEST APPARATUS
// ----------------------------------------------------------------------------

// 'ArrayPrinter(pointer, length)' simplifies printing out array values from
// LOOP_ASSERT failures.

template <class T>
struct ArrayPrinter {
    const T*   d_ptr;
    size_t     d_size;

    ArrayPrinter(const T* ptr, size_t size)
    : d_ptr(ptr)
    , d_size(size)
    {
    }
};

template <class T>
ArrayPrinter<T> makeArrayPrinter(const T* ptr, size_t size)
{
    return ArrayPrinter<T>(ptr, size);
}

template <class T>
ostream &operator <<(ostream &os, const ArrayPrinter<T> &t)
{
    os << "[";
    for(int i = 0; i < t.d_size; ++i) {
        os << " "
           << bsl::hex << bsl::showbase
           << bsl::setw(6) << bsl::setfill('0')
           << bsl::internal
           << unsigned(t.d_ptr[i]);
    }
    os << bsl::dec << " ]";
    return os;
}

// 'printStr' helper routines to print test strings at high verbosity levels.

void printStr(const char *q)
{
    const unsigned char *p = (const unsigned char*)q;
    printf("[");
    while (*p) {
        if (*p > ' ' && *p < 127) {
            printf("    '%c'",*p);
        }
        else {
            printf(" 0x%04X",*p);
        }

        ++p;
    }

    printf(" 0x00");

    printf(" ]");
}

void printStr(const unsigned short *p)
{
    printf("[");
    while (*p) {
        if (*p >= ' ' && *p < 127) {
            printf("    '%c'",*p);
        }
        else {
            printf("  0x%04X",*p);
        }

        ++p;
    }

    printf(" 0x0000");

    printf(" ]");
}

// ============================================================================
//                              USAGE EXAMPLE 1
// ----------------------------------------------------------------------------

///Usage #1: C-style interface
///---------------------------
// The following snippet of code illustrates a typical use of the
// 'bdlde::CharConvertUcs2' struct's C-style utility functions, converting a
// simple UTF-8 string to UCS-2.
//..

void testCFunction1()
{
    unsigned short buffer[256];  // arbitrary "wide-enough" size
    bsl::size_t    buffSize = sizeof buffer / sizeof *buffer;
    bsl::size_t    charsWritten;

    int retVal =
              BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
                                                             buffSize,
                                                             "Hello",
                                                             &charsWritten);

    ASSERT( 0  == retVal);
    ASSERT('H' == buffer[0]);
    ASSERT('e' == buffer[1]);
    ASSERT('l' == buffer[2]);
    ASSERT('l' == buffer[3]);
    ASSERT('o' == buffer[4]);
    ASSERT( 0  == buffer[5]);
    ASSERT( 6  == charsWritten);
}

// ============================================================================
//                              USAGE EXAMPLE 2
// ----------------------------------------------------------------------------

///..
///Usage #2: C-style round-trip
///----------------------------
// The following snippet of code illustrates another typical use of the
// 'bdlde::CharConvertUcs2' struct's C-style utility functions, converting a
// simple UTF-8 string to UCS-2, then converting the UCS-2 back and making sure
// the round-trip conversion results in the input.
///..
void testCFunction2()
{
    unsigned short buffer[256];  // arbitrary "wide-enough" size
    bsl::size_t    buffSize = sizeof buffer / sizeof *buffer;
    bsl::size_t    charsWritten;

    // "&Eacute;cole", the French word for School.  '&Eacute;' is the HTML
    // entity equivalent to Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
    int retVal =
          BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
                                                         buffSize,
                                                         "\xc3\x89" "cole",
                                                         &charsWritten);

    ASSERT( 0   == retVal);
    ASSERT(0xc9 == buffer[0]); // Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
    ASSERT('c'  == buffer[1]);
    ASSERT('o'  == buffer[2]);
    ASSERT('l'  == buffer[3]);
    ASSERT('e'  == buffer[4]);
    ASSERT( 0   == buffer[5]);
    ASSERT( 6   == charsWritten);

    char           buffer2[256];  // arbitrary "wide-enough" size
    bsl::size_t    buffer2Size  = sizeof buffer2 / sizeof *buffer2;
    bsl::size_t    bytesWritten = 0;

    // Reversing the conversion returns the original string:
    retVal =
          BloombergLP::bdlde::CharConvertUcs2::ucs2ToUtf8(buffer2,
                                                         buffer2Size,
                                                         buffer,
                                                         &charsWritten,
                                                         &bytesWritten);

    ASSERT( 0 == retVal);
    ASSERT( 0 == bsl::strcmp(buffer2, "\xc3\x89" "cole"));

    // 6 characters written, but 7 bytes, since the first character takes 2
    // octets.

    ASSERT( 6 == charsWritten);
    ASSERT( 7 == bytesWritten);
}
//..
// In this example, a UTF-8 input string is converted then passed to another
// function, which expects a UCS-2 buffer.
//
// First, we define a utility *strlen* replacement for UCS-2:
//..
int wideStrlen(const unsigned short *str)
{
    int len = 0;

    while (*str++) {
        ++len;
    }

    return len;
}
//..
// Now, some arbitrary function that calls 'wideStrlen':
//..
void functionRequiringUcs2(const unsigned short *str, bsl::size_t strLen)
{
    // Would probably do something more reasonable here.

    ASSERT(wideStrlen(str) + 1 == strLen);
}
//..
// Finally, we can take some UTF-8 as an input and call
// 'functionRequiringUcs2':
//..
void processUtf8(const char *strU8)
{
    unsigned short buffer[1024];  // some "large enough" size
    bsl::size_t    buffSize     = sizeof buffer / sizeof *buffer;
    bsl::size_t    charsWritten = 0;

    int result =
              BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
                                                             buffSize,
                                                             strU8,
                                                             &charsWritten);

    if (0 == result) {
        functionRequiringUcs2(buffer, charsWritten);
    }
}
//..
///Usage #3: C++-style interface
///---------------------------
// The following snippet of code illustrates a typical use of the
// 'bdlde::CharConvertUcs2' struct's C++-style utility functions, converting a
// simple UTF-8 string to UCS-2.
//..
void loadUCS2Hello(bsl::vector<unsigned short> *result)
{
    int retVal =
              BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(result,
                                                             "Hello");

    ASSERT( 0  == retVal);
    ASSERT('H' == (*result)[0]);
    ASSERT('e' == (*result)[1]);
    ASSERT('l' == (*result)[2]);
    ASSERT('l' == (*result)[3]);
    ASSERT('o' == (*result)[4]);
    ASSERT( 0  == (*result)[5]);
    ASSERT( 6  == result->size());
}
//..
// The following snippets of code illustrate a typical use of the
// 'bdlde::CharConvertUcs2' struct's C++-style utility functions, first
// converting from UTF-8 to UCS-2, and then converting back to make sure the
// round trip returns the same value.
//..
void checkCppRoundTrip()
{
    bsl::vector<unsigned short> result;

    // "&Eacute;cole", the French word for School.  '&Eacute;' is the HTML
    // entity corresponding to "Unicode-E WITH ACUTE, LATIN CAPITAL LETTER"
    int retVal =
          BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(&result,
                                                         "\xc3\x89" "cole");

    ASSERT( 0   == retVal);
    ASSERT(0xc9 == result[0]); // Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
    ASSERT('c'  == result[1]);
    ASSERT('o'  == result[2]);
    ASSERT('l'  == result[3]);
    ASSERT('e'  == result[4]);
    ASSERT( 0   == result[5]);
    ASSERT( 6   == result.size());

    bsl::string    result2;
    bsl::size_t    charsWritten = 0;

    // Reversing the conversion returns the original string:
    retVal =
          BloombergLP::bdlde::CharConvertUcs2::ucs2ToUtf8(&result2,
                                                         &result.front(),
                                                         &charsWritten);

    ASSERT( 0 == retVal);
    ASSERT( result2 == "\xc3\x89" "cole");

    // 6 characters written (including the null-terminator), and 6 bytes, since
    // the first character takes 2 octets and the null-terminator is not
    // counted in "length()".

    ASSERT( 6 == charsWritten);
    ASSERT( 6 == result2.length());
}
//..
// In this example, a UTF-8 input string is converted then returned.
//..
bsl::vector<unsigned short> processUtf8(const bsl::string &strU8)
{
    bsl::vector<unsigned short>   result;

    int retCode =
              BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(&result,
                                                             strU8.c_str());

    return result;
}



// Enumeration of the return codes expected from the functions being tested.
// The correctness of these values with respect to the documentation is tested
// in case 2.
enum {
    SUCCESS                 = 0,
    OK                      = 0,
    INVALID_INPUT_CHARACTER = 1,
    BADC                    = 1,
    OUTPUT_BUFFER_TOO_SMALL = 2,
    OBTS                    = 2,
    BOTH                    = 3
};


// Some useful multi-octet characters:

    // The 2 lowest 2-octet characters.
    #define U8_00080  "\xc2\x80"
    #define U8_00081  "\xc2\x81"

    // A traditional "interesting" character, 0xff.
    #define U8_000ff  "\xc3\xbf"

    // The 2 highest 2-octet characters.
    #define U8_007fe  "\xdf\xbe"
    #define U8_007ff  "\xdf\xbf"

    // The 2 lowest 3-octet characters.
    #define U8_00800  "\xe0\xa0\x80"
    #define U8_00801  "\xe0\xa0\x81"

    // The 2 highest 3-octet characters.
    #define U8_0fffe  "\xef\xbf\xbe"
    #define U8_0ffff  "\xef\xbf\xbf"

    // The 2 lowest 4-octet characters.
    #define U8_10000  "\xf0\x80\x80\x80"
    #define U8_10001  "\xf0\x80\x80\x81"

    // The 2 highest 4-octet characters.
    #define U8_10fffe "\xf7\xbf\xbf\xbe"
    #define U8_10ffff "\xf7\xbf\xbf\xbf"

// We will try all combinations of the 'PRECOMPUTED_DATA' characters up to
// 'exhaustiveSearchDepth' in length.

const int exhaustiveSearchDepth = 4; // 5 works fine on AIX, but our Sun boxes
                                     // are slower

// Precomputed conversions for state space enumeration.  These will be
// enumerated by 'buildUpAndTestStringsU8ToU2' and
// 'buildUpAndTestStringsU2ToU8'.

const struct PrecomputedData {
    const char           *d_utf8Character;
    const bsl::size_t     d_utf8CharacterLength;
    const unsigned short  d_ucs2Character;
} PRECOMPUTED_DATA[] =
{
    // valid 1-octet characters:

    { "\x1",    1, 0x0001 },
    { "\x21",   1, 0x0021 },
    { "\x7e",   1, 0x007e },
    { "\x7f",   1, 0x007f },

    // valid 2-octet characters:

    { U8_00080, 2, 0x0080 },
    { U8_00081, 2, 0x0081 },
    { U8_007fe, 2, 0x07fe },
    { U8_007ff, 2, 0x07ff },

    // valid 3-octet characters:

    { U8_00800, 3, 0x0800 },
    { U8_00801, 3, 0x0801 },
    { U8_0fffe, 3, 0xfffe },
    { U8_0ffff, 3, 0xffff },
};

bsl::size_t precomputedDataCount = sizeof PRECOMPUTED_DATA
                                 / sizeof *PRECOMPUTED_DATA;

// Utility function validating that a 'ucs2ToUtf8' conversion has the expected
// results.  The function will also test to make sure that insufficient
// 'dstCapacity' arguments for the conversion function return an
// 'OUTPUT_BUFFER_TOO_SMALL' result.

void checkForExpectedConversionResultsU2ToU8(unsigned short *input,
                                             char           *expected_output,
                                             bsl::size_t     totalOutputLength,
                                             unsigned short *characterSizes,
                                             bsl::size_t     characterCount,
                                             int             verbose,
                                             int             veryVerbose)
{
    int retVal;

    if (veryVerbose) {
        cout << "checkForExpectedConversionResultsU2ToU8("
             <<  "\n\tinput             ="
             << makeArrayPrinter(input, characterCount+1)
             << ",\n\texpected_output   =";
        printStr(expected_output);
        cout << ",\n\ttotalOutputLength ="
             << totalOutputLength
             << ",\n\tcharacterSizes    ="
             << makeArrayPrinter(characterSizes, characterCount)
             << ",\n\tcharacterCount    ="
             << characterCount
             << ");\n";
    }

    if (!totalOutputLength) {
        return;                                                       // RETURN
    }

    for(int bufSize = 0; bufSize < totalOutputLength; ++bufSize) {
        char outputBuffer[256] = { 0 };
        bsl::size_t bytesWritten = 0;
        bsl::size_t charsWritten = 0;

        retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                outputBuffer,
                bufSize,
                input,
                &charsWritten,
                &bytesWritten);

        LOOP5_ASSERT(L_, OUTPUT_BUFFER_TOO_SMALL,   retVal,
                         bufSize,                   totalOutputLength,
                         OUTPUT_BUFFER_TOO_SMALL == retVal);
        LOOP3_ASSERT(L_, charsWritten,   characterCount,
                         charsWritten <= characterCount);
        LOOP3_ASSERT(L_, bytesWritten,   totalOutputLength,
                         bytesWritten <= totalOutputLength);
    }

    char outputBuffer[256] = { 0 };
    bsl::size_t bytesWritten = 0;
    bsl::size_t charsWritten = 0;

    retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                                    outputBuffer,
                                    totalOutputLength + 1,
                                    input,
                                    &charsWritten,
                                    &bytesWritten);

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, charsWritten,   1 + characterCount,
                     charsWritten == 1 + characterCount);
    LOOP3_ASSERT(L_, bytesWritten,   1 + totalOutputLength,
                     bytesWritten == 1 + totalOutputLength);

    LOOP5_ASSERT(L_, totalOutputLength + 1,
                     bytesWritten,
                     makeArrayPrinter(outputBuffer, totalOutputLength + 1),
                     makeArrayPrinter(expected_output, bytesWritten),
                     0 == strcmp(outputBuffer, expected_output));

    bsl::string  cppOutput;
    bsl::size_t  cppCharsWritten = 0;

    retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                                    &cppOutput,
                                    input,
                                    &cppCharsWritten);

    bsl::size_t   cppBytesWritten = cppOutput.length();

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, cppCharsWritten,   1 + characterCount,
                     cppCharsWritten == 1 + characterCount);
    LOOP5_ASSERT(L_, characterCount + 1,
                     cppCharsWritten,
                     makeArrayPrinter(cppOutput.c_str(), cppBytesWritten + 1),
                     makeArrayPrinter(expected_output, charsWritten),
                     0 == memcmp(cppOutput.c_str(),
                                 expected_output,
                                 cppBytesWritten + 1));
}

// This utility function for testing 'ucs2ToUtf8' will *recursively* build up
// input strings in 'inputBuffer' and output strings in 'outputBuffer', and
// call 'checkForExpectedConversionResultsU2ToU8' to make sure that the results
// match.  'inputCursor' and 'outputCursor' point to the "current position" in
// the respective buffers where this level of the recursion will operate.  The
// recursion terminates once 'depth <= 0'.

void buildUpAndTestStringsU2ToU8(int             idx,
                                 int             depth,
                                 unsigned short *inputBuffer,
                                 char           *outputBuffer,
                                 unsigned short *characterSizes,
                                 bsl::size_t     totalOutputLength,
                                 bsl::size_t     characterCount,
                                 unsigned short *inputCursor,
                                 char           *outputCursor,
                                 int             verbose,
                                 int             veryVerbose)
{
    // Null-terminate input and expected output:

    *inputCursor  = 0;
    *outputCursor = 0;

    checkForExpectedConversionResultsU2ToU8(inputBuffer,
            outputBuffer,
            totalOutputLength,
            characterSizes,
            characterCount,
            verbose,
            veryVerbose);

    if (depth <= 0) {
        return;                                                       // RETURN
    }

    struct PrecomputedData const &d = PRECOMPUTED_DATA[idx];

    *inputCursor++         = d.d_ucs2Character;

    // Null-terminate input:

    *inputCursor           = 0;

    strcpy(outputCursor,    d.d_utf8Character);
    outputCursor         += d.d_utf8CharacterLength;
    totalOutputLength    += d.d_utf8CharacterLength;

    characterSizes[characterCount++] = d.d_utf8CharacterLength;

    for(int i = 0; i < precomputedDataCount; ++i) {
        buildUpAndTestStringsU2ToU8(i,
                                    depth - 1,
                                    inputBuffer,
                                    outputBuffer,
                                    characterSizes,
                                    totalOutputLength,
                                    characterCount,
                                    inputCursor,
                                    outputCursor,
                                    verbose,
                                    veryVerbose);
    }
}

// *Break* a copy of the input, manipulating the bits to make each character in
// turn , and validating the reported 'numCharsWritten' and output string.

struct PerturbationDesc {
    unsigned char   d_octetToConvertTo;
    bool            d_isNewValid;
    unsigned short  d_newCharacter;
    int             d_extraInvalidBefore;
    int             d_extraInvalidAfter;
};

// This utility function perturbs a single octet in 'input' and checks that
// only the effects specified in the 'perturb' description occur.

void testSingleOctetPerturbation(const char             *input,
                                 bsl::size_t             perturbationPos,
                                 bsl::size_t             perturbationChar,
                                 const unsigned short   *origExpectedOutput,
                                 bsl::size_t             totalInputLength,
                                 unsigned short         *characterSizes,
                                 bsl::size_t             characterCount,
                                 const PerturbationDesc &perturb,
                                 int                     verbose,
                                 int                     veryVerbose)
{
    char           inputBuffer[256];

    strcpy(inputBuffer, input);

    inputBuffer[perturbationPos] = perturb.d_octetToConvertTo;

    int before = perturb.d_extraInvalidBefore;
    int after  = perturb.d_extraInvalidAfter;
    int pos    = perturbationChar;

    // Increment characterCount to account for additional error characters
    // before and after 'pos' and for the null terminator.

    characterCount += before + after + 1;

    unsigned short outputBuffer[256] = { 0 };
    bsl::size_t charsWritten = 0;

    // Make sure conversions where 'outputBuffer' is too small result in the
    // correct errors AND a null-terminated output
    int retVal = bdlde::CharConvertUcs2::utf8ToUcs2(outputBuffer,
                                                   characterCount - 1,
                                                   inputBuffer,
                                                   &charsWritten);

    LOOP3_ASSERT( L_, OBTS,   retVal,
                      OBTS == retVal || BOTH == retVal );

    LOOP3_ASSERT( L_, 0,   outputBuffer[characterCount-2],
                      0 == outputBuffer[characterCount-2]);

    retVal = bdlde::CharConvertUcs2::utf8ToUcs2(outputBuffer,
                                               characterCount,
                                               inputBuffer,
                                               &charsWritten);

    if (veryVerbose) {
        cout << "PERTURBING:"
             << "\n\tinputBuffer       =";
        printStr(inputBuffer);
        cout << "\n\toutputBuffer      =" << makeArrayPrinter(outputBuffer,
                                                              characterCount)
             << "\n\tperturbation      = { '"
             <<            perturb.d_octetToConvertTo
             << "', '" << (perturb.d_isNewValid?"Y":"N")
             << "', '" <<  perturb.d_newCharacter
             << "', "  <<  perturb.d_extraInvalidBefore
             << "', "  <<  perturb.d_extraInvalidAfter
             << "}"
             << "\n\torigInput         =";
        printStr(input);
        cout << "\n\torigExpectedOutput="
             << makeArrayPrinter(origExpectedOutput, characterCount)
             << endl;
    }

    if (perturb.d_isNewValid && !before && !after) {
        LOOP3_ASSERT( L_, SUCCESS,   retVal,
                          SUCCESS == retVal );
    }
    else {
        LOOP3_ASSERT( L_, INVALID_INPUT_CHARACTER,   retVal,
                          INVALID_INPUT_CHARACTER == retVal );
    }

    ASSERT ( charsWritten == characterCount );

    // Adjust the position in the output of the character we changed by adding
    // 'before'.

    pos += before;

    for (int i = 0; i < characterCount; ++i) {
        if (i < pos - before) {
            LOOP3_ASSERT(i, outputBuffer[i],   origExpectedOutput[i],
                            outputBuffer[i] == origExpectedOutput[i]);
        }
        else if (before && i <  pos && i >= pos - before) {
            // We have introduced 'before' new '?'(s) before 'pos'.

            LOOP3_ASSERT(i, outputBuffer[i],   '?',
                            outputBuffer[i] == '?');
        }
        else if (i == pos) {
            // This is the perturbed character position.

            LOOP3_ASSERT(i, outputBuffer[i],   perturb.d_newCharacter,
                            outputBuffer[i] == perturb.d_newCharacter);
        }
        else if (after && i >  pos && i <= pos + after) {
            // We have introduced 'after' new '?'(s) after 'pos'

            LOOP3_ASSERT(i, outputBuffer[i],   '?',
                            outputBuffer[i] == '?');
        }
        else {
            // we're beyond 'pos + after':

            int posInOrig = i - before - after;
            LOOP4_ASSERT(i, posInOrig,
                            outputBuffer[i],   origExpectedOutput[posInOrig],
                            outputBuffer[i] == origExpectedOutput[posInOrig]);
        }
    }
}


// This utility function perturbs each octet of each UTF-8 character in 'input'
// into each possible alternative character class, making sure that the correct
// errors are detected.

void perturbUtf8AndCheckConversionFailures(const char     *input,
                                           unsigned short *expected_output,
                                           bsl::size_t     totalInputLength,
                                           unsigned short *characterSizes,
                                           bsl::size_t     characterCount,
                                           int             verbose,
                                           int             veryVerbose)
{
    int retVal;

    if (veryVerbose) {
        cout << "perturbUtf8AndCheckConversionFailures("
             <<  "\n\tinput             =";
        printStr(input);
        cout << ",\n\texpected_output   ="
             << makeArrayPrinter(expected_output, characterCount+1)
             << ",\n\ttotalInputLength  ="
             << totalInputLength
             << ",\n\tcharacterSizes    ="
             << makeArrayPrinter(characterSizes, characterCount)
             << ",\n\tcharacterCount    ="
             << characterCount
             << ");\n";
    }

    if (!totalInputLength) {
        return;                                                       // RETURN
    }

    // The perturbations we can apply to each UTF-8 input character will depend
    // on its number of octets.  Depending on what permutation we apply to each
    // octet in a character, some number of previously valid characters will
    // become '?' error indicators or new valid characters, and additional
    // error indicators may be created either before, after, or both before and
    // after the perturbed octet.  For example, changing the middle octet of a
    // 3-octet character to a '!' character changes the result from some valid
    // character to an error, then a '!' character, then another error.
    //
    //    +----------+-----------+-------------------+----------------------+
    //    | CharLen  | Which     | Convert to        |       # of chars     |
    //    |          | Octet     |                   | becoming   | extra ? |
    //    |          |           |                   |            | bef/aft |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 1-octet: | Octet 1   | illegal 1-octet   |          ? |   0   0 |
    //    |          |           | 2-octet octet 1   |          ? |   0   0 |
    //    |          |           | 3-octet octet 1   |          ? |   0   0 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    +----------+-----------+-------------------+------------+---------+
    //    | 2-octet: | Octet 1   | illegal 1-octet   |          ? |   0   1 |
    //    |          |           | '!' 1-octet char  |          ! |   0   1 |
    //    |          |           | 3-octet octet 1   |          ? |   0   0 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   1 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 2-octet: | Octet 2   | illegal 1-octet   |          ? |   1   0 |
    //    |          |           | '!' 1-octet char  |          ! |   1   0 |
    //    |          |           | 2-octet octet 1   |          ? |   1   0 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 1   | illegal 1-octet   |          ? |   0   2 |
    //    |          |           | '!' 1-octet char  |          ! |   0   2 |
    //    |          |           | 4-octet octet 1   |          ? |   0   0 |
    //    |          |           | "extra octet"     |          ? |   0   2 |
    //    |          |           | 2-octet octet 1   | New valid  |   0   1 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 2   | illegal 1-octet   |          ? |   1   1 |
    //    |          |           | '!' 1-octet char  |          ! |   1   1 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    |          |           | 2-octet octet 1   | New valid  |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+
    //    | 3-octet: | Octet 3   | illegal 1-octet   |          ? |   1   0 |
    //    |          |           | '!' 1-octet char  |          ! |   1   0 |
    //    |          |           | 2-octet octet 1   |          ? |   1   0 |
    //    |          |           | 3-octet octet 1   |          ? |   1   0 |
    //    |          |           | 4-octet octet 1   |          ? |   1   0 |
    //    +----------+-----------+-------------------+------------+---------+

    for (int currentChar = 0; currentChar < characterCount; ++currentChar) {
        int currentCharStart = 0;
        for (int i=0; i < currentChar; ++i) {
            currentCharStart += characterSizes[i];
        }

        switch(characterSizes[currentChar]) {
          case 1: {
            // perturbing 1-octet character

            static const PerturbationDesc oneOctetCharOctetOne[] = {
                { 0xff, false, '?', 0, 0 },  // Illegal 1-octet
                { 0xc2, false, '?', 0, 0 },  // 2-octet char 1
                { 0xef, false, '?', 0, 0 },  // 3-octet char 1
                { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                { 0x80, false, '?', 0, 0 },  // "extra" octet
            };
            bsl::size_t testCount = sizeof oneOctetCharOctetOne /
                                    sizeof *oneOctetCharOctetOne;

            for (int i = 0; i < testCount; ++i) {
                testSingleOctetPerturbation(input,
                                            currentCharStart,
                                            currentChar,
                                            expected_output,
                                            totalInputLength,
                                            characterSizes,
                                            characterCount,
                                            oneOctetCharOctetOne[i],
                                            verbose,
                                            veryVerbose);
            }
          } break;

          case 2: {
            // perturbing 2-octet character, octet 1

            {
                static const PerturbationDesc twoOctetCharOctetOne[] = {
                    { 0xff, false, '?', 0, 1 },  // Illegal 1-octet
                    {  '!',  true, '!', 0, 1 },  // '!' 1-octet char
                    { 0xef, false, '?', 0, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                    { 0x80, false, '?', 0, 1 },  // "extra" octet
                };
                static const
                bsl::size_t testCount = sizeof twoOctetCharOctetOne
                                      / sizeof *twoOctetCharOctetOne;

                for (int i = 0; i < testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCharStart,
                                                currentChar,
                                                expected_output,
                                                totalInputLength,
                                                characterSizes,
                                                characterCount,
                                                twoOctetCharOctetOne[i],
                                                verbose,
                                                veryVerbose);
                }
            }

            // perturbing 2-octet character, octet 2

            {
                static const PerturbationDesc twoOctetCharOctetTwo[] = {
                    { 0xff, false, '?', 1, 0 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 0 },  // '!' 1-octet char
                    { 0xc2, false, '?', 1, 0 },  // 2-octet char 1
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof twoOctetCharOctetTwo
                                      / sizeof *twoOctetCharOctetTwo;

                for (int i = 0; i < testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCharStart + 1,
                                                currentChar,
                                                expected_output,
                                                totalInputLength,
                                                characterSizes,
                                                characterCount,
                                                twoOctetCharOctetTwo[i],
                                                verbose,
                                                veryVerbose);
                }
            }
          } break;

          case 3: {
            // perturbing 3-octet character, octet 1

            {
                static const PerturbationDesc threeOctetCharOctetOne[] = {
                    { 0xff, false, '?', 0, 2 },  // Illegal 1-octet
                    {  '!',  true, '!', 0, 2 },  // '!' 1-octet char
                    { 0xf0, false, '?', 0, 0 },  // 4-octet char 1
                    { 0x80, false, '?', 0, 2 },  // "extra" octet
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetOne /
                                        sizeof *threeOctetCharOctetOne;

                for (int i = 0; i < testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCharStart,
                                                currentChar,
                                                expected_output,
                                                totalInputLength,
                                                characterSizes,
                                                characterCount,
                                                threeOctetCharOctetOne[i],
                                                verbose,
                                                veryVerbose);
                }

                // Changing this byte to a "2-octet char 1" can't be
                // data-driven since we must compute the resulting character.

                const unsigned short newChar = ((0xc2 & 0x1f) << 6 )
                                          | (input[currentCharStart+1] & 0x3f);
                const PerturbationDesc perturb =
                                             { 0xc2,  true, newChar, 0, 1 };

                testSingleOctetPerturbation(input,
                                            currentCharStart,
                                            currentChar,
                                            expected_output,
                                            totalInputLength,
                                            characterSizes,
                                            characterCount,
                                            perturb,
                                            verbose,
                                            veryVerbose);
            }

            // perturbing 3-octet character, octet 2
            {
                static const PerturbationDesc threeOctetCharOctetTwo[] = {
                    { 0xff, false, '?', 1, 1 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 1 },  // '!' 1-octet char
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetTwo /
                                        sizeof *threeOctetCharOctetTwo;

                for (int i = 0; i < testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCharStart + 1,
                                                currentChar,
                                                expected_output,
                                                totalInputLength,
                                                characterSizes,
                                                characterCount,
                                                threeOctetCharOctetTwo[i],
                                                verbose,
                                                veryVerbose);
                }

                // Changing this byte to a "2-octet char 1" can't be
                // data-driven since we must compute the resulting character.
                const
                unsigned short newChar = ((0xc2 & 0x1f) << 6 )
                                       | (input[currentCharStart+2] & 0x3f);
                const
                PerturbationDesc perturb = { 0xc2,  true, newChar, 1, 0 };

                testSingleOctetPerturbation(input,
                                            currentCharStart + 1,
                                            currentChar,
                                            expected_output,
                                            totalInputLength,
                                            characterSizes,
                                            characterCount,
                                            perturb,
                                            verbose,
                                            veryVerbose);
            }

            // perturbing 3-octet character, octet 3

            {
                static const PerturbationDesc threeOctetCharOctetThree[] = {
                    { 0xff, false, '?', 1, 0 },  // Illegal 1-octet
                    {  '!',  true, '!', 1, 0 },  // '!' 1-octet char
                    { 0xc2, false, '?', 1, 0 },  // 2-octet char 1
                    { 0xef, false, '?', 1, 0 },  // 3-octet char 1
                    { 0xf0, false, '?', 1, 0 },  // 4-octet char 1
                };
                static const
                bsl::size_t testCount = sizeof threeOctetCharOctetThree
                                      / sizeof *threeOctetCharOctetThree;

                for (int i = 0; i < testCount; ++i) {
                    testSingleOctetPerturbation(input,
                                                currentCharStart + 2,
                                                currentChar,
                                                expected_output,
                                                totalInputLength,
                                                characterSizes,
                                                characterCount,
                                                threeOctetCharOctetThree[i],
                                                verbose,
                                                veryVerbose);
                }
            }
          } break;

          default:// not perturbing 4-octet characters
          break;
        }
    }
}

// Utility function validating that a 'utf8ToUcs2' conversion has the expected
// results.  The function will also test to make sure that insufficient
// 'dstCapacity' arguments for the conversion function return an
// 'OUTPUT_BUFFER_TOO_SMALL' result.

void checkForExpectedConversionResultsU8ToU2(const char     *input,
                                             unsigned short *expected_output,
                                             bsl::size_t     totalInputLength,
                                             unsigned short *characterSizes,
                                             bsl::size_t     characterCount,
                                             int             verbose,
                                             int             veryVerbose)
{
    int retVal;

    if (veryVerbose) {
        cout << "checkForExpectedConversionResultsU8ToU2("
             <<  "\n\tinput             =";
        printStr(input);
        cout << ",\n\texpected_output   ="
             << makeArrayPrinter(expected_output, characterCount+1)
             << ",\n\ttotalInputLength  ="
             << totalInputLength
             << ",\n\tcharacterSizes    ="
             << makeArrayPrinter(characterSizes, characterCount)
             << ",\n\tcharacterCount    ="
             << characterCount
             << ");\n";
    }

    if (!totalInputLength) {
        return;                                                       // RETURN
    }

    perturbUtf8AndCheckConversionFailures(input,
                                          expected_output,
                                          totalInputLength,
                                          characterSizes,
                                          characterCount,
                                          verbose,
                                          veryVerbose);

    for(int bufSize = 0; bufSize < characterCount; ++bufSize) {
        unsigned short outputBuffer[256] = { 0 };
        bsl::size_t charsWritten = 0;

        retVal = bdlde::CharConvertUcs2::utf8ToUcs2(
                outputBuffer,
                bufSize,
                input,
                &charsWritten);

        LOOP5_ASSERT(L_, OUTPUT_BUFFER_TOO_SMALL,   retVal,
                         bufSize,                   characterCount,
                         OUTPUT_BUFFER_TOO_SMALL == retVal);
        LOOP3_ASSERT(L_, charsWritten,   bufSize,
                         charsWritten == bufSize);
    }

    unsigned short outputBuffer[256] = { 0 };
    bsl::size_t charsWritten = 0;

    retVal = bdlde::CharConvertUcs2::utf8ToUcs2(outputBuffer,
                                               characterCount + 1,
                                               input,
                                               &charsWritten);

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, charsWritten,   1 + characterCount,
                     charsWritten == 1 + characterCount);

    LOOP5_ASSERT(L_, characterCount + 1,
                     charsWritten,
                     makeArrayPrinter(outputBuffer,    characterCount + 1),
                     makeArrayPrinter(expected_output, charsWritten),
                     0 == memcmp(outputBuffer,
                                 expected_output,
                                 charsWritten * sizeof *outputBuffer));

    bsl::vector<unsigned short> cppOutput;
    retVal = bdlde::CharConvertUcs2::utf8ToUcs2(&cppOutput,
                                               input);
    bsl::size_t cppCharsWritten = cppOutput.size();
    bsl::size_t resultBytes = cppCharsWritten * sizeof(short);

    LOOP3_ASSERT(L_, SUCCESS,   retVal,
                     SUCCESS == retVal);
    LOOP3_ASSERT(L_, cppCharsWritten,   1 + characterCount,
                     cppCharsWritten == 1 + characterCount);
    LOOP5_ASSERT(L_, characterCount + 1,
                     cppCharsWritten,
                     makeArrayPrinter(&(cppOutput[0]), characterCount + 1),
                     makeArrayPrinter(expected_output, charsWritten),
                     0 == memcmp(&(cppOutput[0]),
                                 expected_output,
                                 resultBytes));
}

// This utility function for testing 'utf8ToUcs2' will *recursively* build up
// input strings in 'inputBuffer' and output strings in 'outputBuffer', and
// call 'checkForExpectedConversionResultsU8ToU2' to make sure that the results
// match.  'inputCursor' and 'outputCursor' point to the "current position" in
// the respective buffers where this level of the recursion will operate.  The
// recursion terminates once 'depth <= 0'.

void buildUpAndTestStringsU8ToU2(int             idx,
                                 int             depth,
                                 char           *inputBuffer,
                                 unsigned short *outputBuffer,
                                 unsigned short *characterSizes,
                                 bsl::size_t     totalOutputLength,
                                 bsl::size_t     characterCount,
                                 char           *inputCursor,
                                 unsigned short *outputCursor,
                                 int             verbose,
                                 int             veryVerbose)
{
    // Null-terminate input and expected output:

    *inputCursor  = 0;
    *outputCursor = 0;

    checkForExpectedConversionResultsU8ToU2(inputBuffer,
            outputBuffer,
            totalOutputLength,
            characterSizes,
            characterCount,
            verbose,
            veryVerbose);

    if (depth <= 0) {
        return;                                                       // RETURN
    }

    struct PrecomputedData const &d = PRECOMPUTED_DATA[idx];

    strcpy(inputCursor,      d.d_utf8Character);
    inputCursor           += d.d_utf8CharacterLength;

    *outputCursor++        = d.d_ucs2Character;
    *outputCursor          = 0;

    totalOutputLength += d.d_utf8CharacterLength;

    characterSizes[characterCount++] = d.d_utf8CharacterLength;

    for(int i = 0; i < precomputedDataCount; ++i) {
        buildUpAndTestStringsU8ToU2(i,
                                    depth - 1,
                                    inputBuffer,
                                    outputBuffer,
                                    characterSizes,
                                    totalOutputLength,
                                    characterCount,
                                    inputCursor,
                                    outputCursor,
                                    verbose,
                                    veryVerbose);
    }
}


// This utility function for performance testing 'utf8ToUcs2' and 'ucs2ToUtf8'
// will repeatedly convert a long ascii corpus to ucs2 and back again.  It
// returns 0 on success, and non-zero otherwise.

int runPlainTextPerformanceTest(void)
{
    // The first 4 chapters of _Pride and Prejudice_, by Jane Austen
    const char prideAndPrejudice[] = {
        "The Project Gutenberg EBook of Pride and Prejudice, by Jane\n"
        "Austen\n"
        "\n"
        "This eBook is for the use of anyone anywhere at no cost and\n"
        "with almost no restrictions whatsoever.  You may copy it,\n"
        "give it away or re-use it under the terms of the Project\n"
        "Gutenberg License included with this eBook or online at\n"
        "www.gutenberg.org\n"
        "\n"
        "\n"
        "Title: Pride and Prejudice\n"
        "\n"
        "Author: Jane Austen\n"
        "\n"
        "Posting Date: August 26, 2008 [EBook #1342] Release Date:\n"
        "June, 1998\n"
        "\n"
        "Language: English\n"
        "\n"
        "Character set encoding: ASCII\n"
        "\n"
        "*** START OF THIS PROJECT GUTENBERG EBOOK PRIDE AND\n"
        "PREJUDICE ***\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "Produced by Anonymous Volunteers\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "PRIDE AND PREJUDICE\n"
        "\n"
        "By Jane Austen\n"
        "\n"
        "\n"
        "\n"
        "Chapter 1\n"
        "\n"
        "\n"
        "It is a truth universally acknowledged, that a single man in\n"
        "possession of a good fortune, must be in want of a wife.\n"
        "\n"
        "However little known the feelings or views of such a man may\n"
        "be on his first entering a neighbourhood, this truth is so\n"
        "well fixed in the minds of the surrounding families, that he\n"
        "is considered the rightful property of some one or other of\n"
        "their daughters.\n"
        "\n"
        "\"My dear Mr. Bennet,\" said his lady to him one day, \"have\n"
        "you heard that Netherfield Park is let at last?\"\n"
        "\n"
        "Mr. Bennet replied that he had not.\n"
        "\n"
        "\"But it is,\" returned she; \"for Mrs. Long has just been\n"
        "here, and she told me all about it.\"\n"
        "\n"
        "Mr. Bennet made no answer.\n"
        "\n"
        "\"Do you not want to know who has taken it?\" cried his wife\n"
        "impatiently.\n"
        "\n"
        "\"_You_ want to tell me, and I have no objection to hearing\n"
        "it.\"\n"
        "\n"
        "This was invitation enough.\n"
        "\n"
        "\"Why, my dear, you must know, Mrs. Long says that\n"
        "Netherfield is taken by a young man of large fortune from\n"
        "the north of England; that he came down on Monday in a\n"
        "chaise and four to see the place, and was so much delighted\n"
        "with it, that he agreed with Mr. Morris immediately; that he\n"
        "is to take possession before Michaelmas, and some of his\n"
        "servants are to be in the house by the end of next week.\"\n"
        "\n"
        "\"What is his name?\"\n"
        "\n"
        "\"Bingley.\"\n"
        "\n"
        "\"Is he married or single?\"\n"
        "\n"
        "\"Oh! Single, my dear, to be sure! A single man of large\n"
        "fortune; four or five thousand a year. What a fine thing for\n"
        "our girls!\"\n"
        "\n"
        "\"How so? How can it affect them?\"\n"
        "\n"
        "\"My dear Mr. Bennet,\" replied his wife, \"how can you be so\n"
        "tiresome!  You must know that I am thinking of his marrying\n"
        "one of them.\"\n"
        "\n"
        "\"Is that his design in settling here?\"\n"
        "\n"
        "\"Design! Nonsense, how can you talk so! But it is very\n"
        "likely that he _may_ fall in love with one of them, and\n"
        "therefore you must visit him as soon as he comes.\"\n"
        "\n"
        "\"I see no occasion for that. You and the girls may go, or\n"
        "you may send them by themselves, which perhaps will be still\n"
        "better, for as you are as handsome as any of them, Mr.\n"
        "Bingley may like you the best of the party.\"\n"
        "\n"
        "\"My dear, you flatter me. I certainly _have_ had my share of\n"
        "beauty, but I do not pretend to be anything extraordinary\n"
        "now. When a woman has five grown-up daughters, she ought to\n"
        "give over thinking of her own beauty.\"\n"
        "\n"
        "\"In such cases, a woman has not often much beauty to think\n"
        "of.\"\n"
        "\n"
        "\"But, my dear, you must indeed go and see Mr. Bingley when\n"
        "he comes into the neighbourhood.\"\n"
        "\n"
        "\"It is more than I engage for, I assure you.\"\n"
        "\n"
        "\"But consider your daughters. Only think what an\n"
        "establishment it would be for one of them. Sir William and\n"
        "Lady Lucas are determined to go, merely on that account, for\n"
        "in general, you know, they visit no newcomers. Indeed you\n"
        "must go, for it will be impossible for _us_ to visit him if\n"
        "you do not.\"\n"
        "\n"
        "\"You are over-scrupulous, surely. I dare say Mr. Bingley\n"
        "will be very glad to see you; and I will send a few lines by\n"
        "you to assure him of my hearty consent to his marrying\n"
        "whichever he chooses of the girls; though I must throw in a\n"
        "good word for my little Lizzy.\"\n"
        "\n"
        "\"I desire you will do no such thing. Lizzy is not a bit\n"
        "better than the others; and I am sure she is not half so\n"
        "handsome as Jane, nor half so good-humoured as Lydia. But\n"
        "you are always giving _her_ the preference.\"\n"
        "\n"
        "\"They have none of them much to recommend them,\" replied he;\n"
        "\"they are all silly and ignorant like other girls; but Lizzy\n"
        "has something more of quickness than her sisters.\"\n"
        "\n"
        "\"Mr. Bennet, how _can_ you abuse your own children in such a\n"
        "way? You take delight in vexing me. You have no compassion\n"
        "for my poor nerves.\"\n"
        "\n"
        "\"You mistake me, my dear. I have a high respect for your\n"
        "nerves. They are my old friends. I have heard you mention\n"
        "them with consideration these last twenty years at least.\"\n"
        "\n"
        "\"Ah, you do not know what I suffer.\"\n"
        "\n"
        "\"But I hope you will get over it, and live to see many young\n"
        "men of four thousand a year come into the neighbourhood.\"\n"
        "\n"
        "\"It will be no use to us, if twenty such should come, since\n"
        "you will not visit them.\"\n"
        "\n"
        "\"Depend upon it, my dear, that when there are twenty, I will\n"
        "visit them all.\"\n"
        "\n"
        "Mr. Bennet was so odd a mixture of quick parts, sarcastic\n"
        "humour, reserve, and caprice, that the experience of\n"
        "three-and-twenty years had been insufficient to make his\n"
        "wife understand his character. _Her_ mind was less difficult\n"
        "to develop. She was a woman of mean understanding, little\n"
        "information, and uncertain temper. When she was\n"
        "discontented, she fancied herself nervous. The business of\n"
        "her life was to get her daughters married; its solace was\n"
        "visiting and news.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 2\n"
        "\n"
        "\n"
        "Mr. Bennet was among the earliest of those who waited on Mr.\n"
        "Bingley.  He had always intended to visit him, though to the\n"
        "last always assuring his wife that he should not go; and\n"
        "till the evening after the visit was paid she had no\n"
        "knowledge of it. It was then disclosed in the following\n"
        "manner. Observing his second daughter employed in trimming a\n"
        "hat, he suddenly addressed her with:\n"
        "\n"
        "\"I hope Mr. Bingley will like it, Lizzy.\"\n"
        "\n"
        "\"We are not in a way to know _what_ Mr. Bingley likes,\" said\n"
        "her mother resentfully, \"since we are not to visit.\"\n"
        "\n"
        "\"But you forget, mamma,\" said Elizabeth, \"that we shall meet\n"
        "him at the assemblies, and that Mrs. Long promised to\n"
        "introduce him.\"\n"
        "\n"
        "\"I do not believe Mrs. Long will do any such thing. She has\n"
        "two nieces of her own. She is a selfish, hypocritical woman,\n"
        "and I have no opinion of her.\"\n"
        "\n"
        "\"No more have I,\" said Mr. Bennet; \"and I am glad to find\n"
        "that you do not depend on her serving you.\"\n"
        "\n"
        "Mrs. Bennet deigned not to make any reply, but, unable to\n"
        "contain herself, began scolding one of her daughters.\n"
        "\n"
        "\"Don't keep coughing so, Kitty, for Heaven's sake! Have a\n"
        "little compassion on my nerves. You tear them to pieces.\"\n"
        "\n"
        "\"Kitty has no discretion in her coughs,\" said her father;\n"
        "\"she times them ill.\"\n"
        "\n"
        "\"I do not cough for my own amusement,\" replied Kitty\n"
        "fretfully. \"When is your next ball to be, Lizzy?\"\n"
        "\n"
        "\"To-morrow fortnight.\"\n"
        "\n"
        "\"Aye, so it is,\" cried her mother, \"and Mrs. Long does not\n"
        "come back till the day before; so it will be impossible for\n"
        "her to introduce him, for she will not know him herself.\"\n"
        "\n"
        "\"Then, my dear, you may have the advantage of your friend,\n"
        "and introduce Mr. Bingley to _her_.\"\n"
        "\n"
        "\"Impossible, Mr. Bennet, impossible, when I am not\n"
        "acquainted with him myself; how can you be so teasing?\"\n"
        "\n"
        "\"I honour your circumspection. A fortnight's acquaintance is\n"
        "certainly very little. One cannot know what a man really is\n"
        "by the end of a fortnight. But if _we_ do not venture\n"
        "somebody else will; and after all, Mrs. Long and her\n"
        "daughters must stand their chance; and, therefore, as she\n"
        "will think it an act of kindness, if you decline the office,\n"
        "I will take it on myself.\"\n"
        "\n"
        "The girls stared at their father. Mrs. Bennet said only,\n"
        "\"Nonsense, nonsense!\"\n"
        "\n"
        "\"What can be the meaning of that emphatic exclamation?\"\n"
        "cried he. \"Do you consider the forms of introduction, and\n"
        "the stress that is laid on them, as nonsense? I cannot quite\n"
        "agree with you _there_. What say you, Mary? For you are a\n"
        "young lady of deep reflection, I know, and read great books\n"
        "and make extracts.\"\n"
        "\n"
        "Mary wished to say something sensible, but knew not how.\n"
        "\n"
        "\"While Mary is adjusting her ideas,\" he continued, \"let us\n"
        "return to Mr.  Bingley.\"\n"
        "\n"
        "\"I am sick of Mr. Bingley,\" cried his wife.\n"
        "\n"
        "\"I am sorry to hear _that_; but why did not you tell me that\n"
        "before?  If I had known as much this morning I certainly\n"
        "would not have called on him. It is very unlucky; but as I\n"
        "have actually paid the visit, we cannot escape the\n"
        "acquaintance now.\"\n"
        "\n"
        "The astonishment of the ladies was just what he wished; that\n"
        "of Mrs.  Bennet perhaps surpassing the rest; though, when\n"
        "the first tumult of joy was over, she began to declare that\n"
        "it was what she had expected all the while.\n"
        "\n"
        "\"How good it was in you, my dear Mr. Bennet! But I knew I\n"
        "should persuade you at last. I was sure you loved your girls\n"
        "too well to neglect such an acquaintance. Well, how pleased\n"
        "I am! and it is such a good joke, too, that you should have\n"
        "gone this morning and never said a word about it till now.\"\n"
        "\n"
        "\"Now, Kitty, you may cough as much as you choose,\" said Mr.\n"
        "Bennet; and, as he spoke, he left the room, fatigued with\n"
        "the raptures of his wife.\n"
        "\n"
        "\"What an excellent father you have, girls!\" said she, when\n"
        "the door was shut. \"I do not know how you will ever make him\n"
        "amends for his kindness; or me, either, for that matter. At\n"
        "our time of life it is not so pleasant, I can tell you, to\n"
        "be making new acquaintances every day; but for your sakes,\n"
        "we would do anything. Lydia, my love, though you _are_ the\n"
        "youngest, I dare say Mr. Bingley will dance with you at the\n"
        "next ball.\"\n"
        "\n"
        "\"Oh!\" said Lydia stoutly, \"I am not afraid; for though I\n"
        "_am_ the youngest, I'm the tallest.\"\n"
        "\n"
        "The rest of the evening was spent in conjecturing how soon\n"
        "he would return Mr. Bennet's visit, and determining when\n"
        "they should ask him to dinner.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 3\n"
        "\n"
        "\n"
        "Not all that Mrs. Bennet, however, with the assistance of\n"
        "her five daughters, could ask on the subject, was sufficient\n"
        "to draw from her husband any satisfactory description of Mr.\n"
        "Bingley. They attacked him in various ways--with barefaced\n"
        "questions, ingenious suppositions, and distant surmises; but\n"
        "he eluded the skill of them all, and they were at last\n"
        "obliged to accept the second-hand intelligence of their\n"
        "neighbour, Lady Lucas. Her report was highly favourable. Sir\n"
        "William had been delighted with him. He was quite young,\n"
        "wonderfully handsome, extremely agreeable, and, to crown the\n"
        "whole, he meant to be at the next assembly with a large\n"
        "party. Nothing could be more delightful! To be fond of\n"
        "dancing was a certain step towards falling in love; and very\n"
        "lively hopes of Mr. Bingley's heart were entertained.\n"
        "\n"
        "\"If I can but see one of my daughters happily settled at\n"
        "Netherfield,\" said Mrs. Bennet to her husband, \"and all the\n"
        "others equally well married, I shall have nothing to wish\n"
        "for.\"\n"
        "\n"
        "In a few days Mr. Bingley returned Mr. Bennet's visit, and\n"
        "sat about ten minutes with him in his library. He had\n"
        "entertained hopes of being admitted to a sight of the young\n"
        "ladies, of whose beauty he had heard much; but he saw only\n"
        "the father. The ladies were somewhat more fortunate, for\n"
        "they had the advantage of ascertaining from an upper window\n"
        "that he wore a blue coat, and rode a black horse.\n"
        "\n"
        "An invitation to dinner was soon afterwards dispatched; and\n"
        "already had Mrs. Bennet planned the courses that were to do\n"
        "credit to her housekeeping, when an answer arrived which\n"
        "deferred it all. Mr.  Bingley was obliged to be in town the\n"
        "following day, and, consequently, unable to accept the\n"
        "honour of their invitation, etc.  Mrs. Bennet was quite\n"
        "disconcerted. She could not imagine what business he could\n"
        "have in town so soon after his arrival in Hertfordshire; and\n"
        "she began to fear that he might be always flying about from\n"
        "one place to another, and never settled at Netherfield as he\n"
        "ought to be. Lady Lucas quieted her fears a little by\n"
        "starting the idea of his being gone to London only to get a\n"
        "large party for the ball; and a report soon followed that\n"
        "Mr. Bingley was to bring twelve ladies and seven gentlemen\n"
        "with him to the assembly.  The girls grieved over such a\n"
        "number of ladies, but were comforted the day before the ball\n"
        "by hearing, that instead of twelve he brought only six with\n"
        "him from London--his five sisters and a cousin. And when the\n"
        "party entered the assembly room it consisted of only five\n"
        "altogether--Mr. Bingley, his two sisters, the husband of the\n"
        "eldest, and another young man.\n"
        "\n"
        "Mr. Bingley was good-looking and gentlemanlike; he had a\n"
        "pleasant countenance, and easy, unaffected manners. His\n"
        "sisters were fine women, with an air of decided fashion. His\n"
        "brother-in-law, Mr. Hurst, merely looked the gentleman; but\n"
        "his friend Mr. Darcy soon drew the attention of the room by\n"
        "his fine, tall person, handsome features, noble mien, and\n"
        "the report which was in general circulation within five\n"
        "minutes after his entrance, of his having ten thousand a\n"
        "year.  The gentlemen pronounced him to be a fine figure of a\n"
        "man, the ladies declared he was much handsomer than Mr.\n"
        "Bingley, and he was looked at with great admiration for\n"
        "about half the evening, till his manners gave a disgust\n"
        "which turned the tide of his popularity; for he was\n"
        "discovered to be proud; to be above his company, and above\n"
        "being pleased; and not all his large estate in Derbyshire\n"
        "could then save him from having a most forbidding,\n"
        "disagreeable countenance, and being unworthy to be compared\n"
        "with his friend.\n"
        "\n"
        "Mr. Bingley had soon made himself acquainted with all the\n"
        "principal people in the room; he was lively and unreserved,\n"
        "danced every dance, was angry that the ball closed so early,\n"
        "and talked of giving one himself at Netherfield. Such\n"
        "amiable qualities must speak for themselves. What a contrast\n"
        "between him and his friend! Mr. Darcy danced only once with\n"
        "Mrs. Hurst and once with Miss Bingley, declined being\n"
        "introduced to any other lady, and spent the rest of the\n"
        "evening in walking about the room, speaking occasionally to\n"
        "one of his own party.  His character was decided. He was the\n"
        "proudest, most disagreeable man in the world, and everybody\n"
        "hoped that he would never come there again.  Amongst the\n"
        "most violent against him was Mrs.  Bennet, whose dislike of\n"
        "his general behaviour was sharpened into particular\n"
        "resentment by his having slighted one of her daughters.\n"
        "\n"
        "Elizabeth Bennet had been obliged, by the scarcity of\n"
        "gentlemen, to sit down for two dances; and during part of\n"
        "that time, Mr. Darcy had been standing near enough for her\n"
        "to hear a conversation between him and Mr.  Bingley, who\n"
        "came from the dance for a few minutes, to press his friend\n"
        "to join it.\n"
        "\n"
        "\"Come, Darcy,\" said he, \"I must have you dance. I hate to\n"
        "see you standing about by yourself in this stupid manner.\n"
        "You had much better dance.\"\n"
        "\n"
        "\"I certainly shall not. You know how I detest it, unless I\n"
        "am particularly acquainted with my partner. At such an\n"
        "assembly as this it would be insupportable. Your sisters are\n"
        "engaged, and there is not another woman in the room whom it\n"
        "would not be a punishment to me to stand up with.\"\n"
        "\n"
        "\"I would not be so fastidious as you are,\" cried Mr.\n"
        "Bingley, \"for a kingdom! Upon my honour, I never met with so\n"
        "many pleasant girls in my life as I have this evening; and\n"
        "there are several of them you see uncommonly pretty.\"\n"
        "\n"
        "\"_You_ are dancing with the only handsome girl in the room,\"\n"
        "said Mr.  Darcy, looking at the eldest Miss Bennet.\n"
        "\n"
        "\"Oh! She is the most beautiful creature I ever beheld! But\n"
        "there is one of her sisters sitting down just behind you,\n"
        "who is very pretty, and I dare say very agreeable. Do let me\n"
        "ask my partner to introduce you.\"\n"
        "\n"
        "\"Which do you mean?\" and turning round he looked for a\n"
        "moment at Elizabeth, till catching her eye, he withdrew his\n"
        "own and coldly said: \"She is tolerable, but not handsome\n"
        "enough to tempt _me_; I am in no humour at present to give\n"
        "consequence to young ladies who are slighted by other men.\n"
        "You had better return to your partner and enjoy her smiles,\n"
        "for you are wasting your time with me.\"\n"
        "\n"
        "Mr. Bingley followed his advice. Mr. Darcy walked off; and\n"
        "Elizabeth remained with no very cordial feelings toward him.\n"
        "She told the story, however, with great spirit among her\n"
        "friends; for she had a lively, playful disposition, which\n"
        "delighted in anything ridiculous.\n"
        "\n"
        "The evening altogether passed off pleasantly to the whole\n"
        "family. Mrs.  Bennet had seen her eldest daughter much\n"
        "admired by the Netherfield party. Mr. Bingley had danced\n"
        "with her twice, and she had been distinguished by his\n"
        "sisters. Jane was as much gratified by this as her mother\n"
        "could be, though in a quieter way. Elizabeth felt Jane's\n"
        "pleasure. Mary had heard herself mentioned to Miss Bingley\n"
        "as the most accomplished girl in the neighbourhood; and\n"
        "Catherine and Lydia had been fortunate enough never to be\n"
        "without partners, which was all that they had yet learnt to\n"
        "care for at a ball. They returned, therefore, in good\n"
        "spirits to Longbourn, the village where they lived, and of\n"
        "which they were the principal inhabitants. They found Mr.\n"
        "Bennet still up. With a book he was regardless of time; and\n"
        "on the present occasion he had a good deal of curiosity as\n"
        "to the events of an evening which had raised such splendid\n"
        "expectations. He had rather hoped that his wife's views on\n"
        "the stranger would be disappointed; but he soon found out\n"
        "that he had a different story to hear.\n"
        "\n"
        "\"Oh! my dear Mr. Bennet,\" as she entered the room, \"we have\n"
        "had a most delightful evening, a most excellent ball. I wish\n"
        "you had been there.  Jane was so admired, nothing could be\n"
        "like it. Everybody said how well she looked; and Mr. Bingley\n"
        "thought her quite beautiful, and danced with her twice! Only\n"
        "think of _that_, my dear; he actually danced with her twice!\n"
        "and she was the only creature in the room that he asked a\n"
        "second time. First of all, he asked Miss Lucas. I was so\n"
        "vexed to see him stand up with her! But, however, he did not\n"
        "admire her at all; indeed, nobody can, you know; and he\n"
        "seemed quite struck with Jane as she was going down the\n"
        "dance. So he inquired who she was, and got introduced, and\n"
        "asked her for the two next. Then the two third he danced\n"
        "with Miss King, and the two fourth with Maria Lucas, and the\n"
        "two fifth with Jane again, and the two sixth with Lizzy, and\n"
        "the _Boulanger_--\"\n"
        "\n"
        "\"If he had had any compassion for _me_,\" cried her husband\n"
        "impatiently, \"he would not have danced half so much! For\n"
        "God's sake, say no more of his partners. O that he had\n"
        "sprained his ankle in the first place!\"\n"
        "\n"
        "\"Oh! my dear, I am quite delighted with him. He is so\n"
        "excessively handsome! And his sisters are charming women. I\n"
        "never in my life saw anything more elegant than their\n"
        "dresses. I dare say the lace upon Mrs.  Hurst's gown--\"\n"
        "\n"
        "Here she was interrupted again. Mr. Bennet protested against\n"
        "any description of finery. She was therefore obliged to seek\n"
        "another branch of the subject, and related, with much\n"
        "bitterness of spirit and some exaggeration, the shocking\n"
        "rudeness of Mr. Darcy.\n"
        "\n"
        "\"But I can assure you,\" she added, \"that Lizzy does not lose\n"
        "much by not suiting _his_ fancy; for he is a most\n"
        "disagreeable, horrid man, not at all worth pleasing. So high\n"
        "and so conceited that there was no enduring him! He walked\n"
        "here, and he walked there, fancying himself so very great!\n"
        "Not handsome enough to dance with! I wish you had been\n"
        "there, my dear, to have given him one of your set-downs. I\n"
        "quite detest the man.\"\n"
        "\n"
        "\n"
        "\n"
        "Chapter 4\n"
        "\n"
        "\n"
        "When Jane and Elizabeth were alone, the former, who had been\n"
        "cautious in her praise of Mr. Bingley before, expressed to\n"
        "her sister just how very much she admired him.\n"
        "\n"
        "\"He is just what a young man ought to be,\" said she,\n"
        "\"sensible, good-humoured, lively; and I never saw such happy\n"
        "manners!--so much ease, with such perfect good breeding!\"\n"
        "\n"
        "\"He is also handsome,\" replied Elizabeth, \"which a young man\n"
        "ought likewise to be, if he possibly can. His character is\n"
        "thereby complete.\"\n"
        "\n"
        "\"I was very much flattered by his asking me to dance a\n"
        "second time. I did not expect such a compliment.\"\n"
        "\n"
        "\"Did not you? I did for you. But that is one great\n"
        "difference between us. Compliments always take _you_ by\n"
        "surprise, and _me_ never. What could be more natural than\n"
        "his asking you again? He could not help seeing that you were\n"
        "about five times as pretty as every other woman in the room.\n"
        "No thanks to his gallantry for that. Well, he certainly is\n"
        "very agreeable, and I give you leave to like him. You have\n"
        "liked many a stupider person.\"\n"
        "\n"
        "\"Dear Lizzy!\"\n"
        "\n"
        "\"Oh! you are a great deal too apt, you know, to like people\n"
        "in general.  You never see a fault in anybody. All the world\n"
        "are good and agreeable in your eyes. I never heard you speak\n"
        "ill of a human being in your life.\"\n"
        "\n"
        "\"I would not wish to be hasty in censuring anyone; but I\n"
        "always speak what I think.\"\n"
        "\n"
        "\"I know you do; and it is _that_ which makes the wonder.\n"
        "With _your_ good sense, to be so honestly blind to the\n"
        "follies and nonsense of others! Affectation of candour is\n"
        "common enough--one meets with it everywhere. But to be\n"
        "candid without ostentation or design--to take the good of\n"
        "everybody's character and make it still better, and say\n"
        "nothing of the bad--belongs to you alone. And so you like\n"
        "this man's sisters, too, do you? Their manners are not equal\n"
        "to his.\"\n"
        "\n"
        "\"Certainly not--at first. But they are very pleasing women\n"
        "when you converse with them. Miss Bingley is to live with\n"
        "her brother, and keep his house; and I am much mistaken if\n"
        "we shall not find a very charming neighbour in her.\"\n"
        "\n"
        "Elizabeth listened in silence, but was not convinced; their\n"
        "behaviour at the assembly had not been calculated to please\n"
        "in general; and with more quickness of observation and less\n"
        "pliancy of temper than her sister, and with a judgement too\n"
        "unassailed by any attention to herself, she was very little\n"
        "disposed to approve them. They were in fact very fine\n"
        "ladies; not deficient in good humour when they were pleased,\n"
        "nor in the power of making themselves agreeable when they\n"
        "chose it, but proud and conceited. They were rather\n"
        "handsome, had been educated in one of the first private\n"
        "seminaries in town, had a fortune of twenty thousand pounds,\n"
        "were in the habit of spending more than they ought, and of\n"
        "associating with people of rank, and were therefore in every\n"
        "respect entitled to think well of themselves, and meanly of\n"
        "others. They were of a respectable family in the north of\n"
        "England; a circumstance more deeply impressed on their\n"
        "memories than that their brother's fortune and their own had\n"
        "been acquired by trade.\n"
        "\n"
        "Mr. Bingley inherited property to the amount of nearly a\n"
        "hundred thousand pounds from his father, who had intended to\n"
        "purchase an estate, but did not live to do it. Mr. Bingley\n"
        "intended it likewise, and sometimes made choice of his\n"
        "county; but as he was now provided with a good house and the\n"
        "liberty of a manor, it was doubtful to many of those who\n"
        "best knew the easiness of his temper, whether he might not\n"
        "spend the remainder of his days at Netherfield, and leave\n"
        "the next generation to purchase.\n"
        "\n"
        "His sisters were anxious for his having an estate of his\n"
        "own; but, though he was now only established as a tenant,\n"
        "Miss Bingley was by no means unwilling to preside at his\n"
        "table--nor was Mrs. Hurst, who had married a man of more\n"
        "fashion than fortune, less disposed to consider his house as\n"
        "her home when it suited her. Mr. Bingley had not been of age\n"
        "two years, when he was tempted by an accidental\n"
        "recommendation to look at Netherfield House. He did look at\n"
        "it, and into it for half-an-hour--was pleased with the\n"
        "situation and the principal rooms, satisfied with what the\n"
        "owner said in its praise, and took it immediately.\n"
        "\n"
        "Between him and Darcy there was a very steady friendship, in\n"
        "spite of great opposition of character. Bingley was endeared\n"
        "to Darcy by the easiness, openness, and ductility of his\n"
        "temper, though no disposition could offer a greater contrast\n"
        "to his own, and though with his own he never appeared\n"
        "dissatisfied. On the strength of Darcy's regard, Bingley had\n"
        "the firmest reliance, and of his judgement the highest\n"
        "opinion.  In understanding, Darcy was the superior. Bingley\n"
        "was by no means deficient, but Darcy was clever. He was at\n"
        "the same time haughty, reserved, and fastidious, and his\n"
        "manners, though well-bred, were not inviting. In that\n"
        "respect his friend had greatly the advantage. Bingley was\n"
        "sure of being liked wherever he appeared, Darcy was\n"
        "continually giving offense.\n"
        "\n"
        "The manner in which they spoke of the Meryton assembly was\n"
        "sufficiently characteristic. Bingley had never met with more\n"
        "pleasant people or prettier girls in his life; everybody had\n"
        "been most kind and attentive to him; there had been no\n"
        "formality, no stiffness; he had soon felt acquainted with\n"
        "all the room; and, as to Miss Bennet, he could not conceive\n"
        "an angel more beautiful. Darcy, on the contrary, had seen a\n"
        "collection of people in whom there was little beauty and no\n"
        "fashion, for none of whom he had felt the smallest interest,\n"
        "and from none received either attention or pleasure. Miss\n"
        "Bennet he acknowledged to be pretty, but she smiled too\n"
        "much.\n"
        "\n"
        "Mrs. Hurst and her sister allowed it to be so--but still\n"
        "they admired her and liked her, and pronounced her to be a\n"
        "sweet girl, and one whom they would not object to know more\n"
        "of. Miss Bennet was therefore established as a sweet girl,\n"
        "and their brother felt authorized by such commendation to\n"
        "think of her as he chose.\n"
        "\n"
        "\n"
        "\n"
        "Chapter 5\n"
        "\n"
        "\n"
        "Within a short walk of Longbourn lived a family with whom\n"
        "the Bennets were particularly intimate. Sir William Lucas\n"
        "had been formerly in trade in Meryton, where he had made a\n"
        "tolerable fortune, and risen to the honour of knighthood by\n"
        "an address to the king during his mayoralty.  The\n"
        "distinction had perhaps been felt too strongly. It had given\n"
        "him a disgust to his business, and to his residence in a\n"
        "small market town; and, in quitting them both, he had\n"
        "removed with his family to a house about a mile from\n"
        "Meryton, denominated from that period Lucas Lodge, where he\n"
        "could think with pleasure of his own importance, and,\n"
        "unshackled by business, occupy himself solely in being civil\n"
        "to all the world. For, though elated by his rank, it did not\n"
        "render him supercilious; on the contrary, he was all\n"
        "attention to everybody. By nature inoffensive, friendly, and\n"
        "obliging, his presentation at St.  James's had made him\n"
        "courteous.\n"
        "\n"
        "Lady Lucas was a very good kind of woman, not too clever to\n"
        "be a valuable neighbour to Mrs. Bennet. They had several\n"
        "children. The eldest of them, a sensible, intelligent young\n"
        "woman, about twenty-seven, was Elizabeth's intimate friend.\n"
        "\n"
        "That the Miss Lucases and the Miss Bennets should meet to\n"
        "talk over a ball was absolutely necessary; and the morning\n"
        "after the assembly brought the former to Longbourn to hear\n"
        "and to communicate.\n"
        "\n"
        "\"_You_ began the evening well, Charlotte,\" said Mrs. Bennet\n"
        "with civil self-command to Miss Lucas. \"_You_ were Mr.\n"
        "Bingley's first choice.\"\n"
        "\n"
        "\"Yes; but he seemed to like his second better.\"\n"
        "\n"
        "\"Oh! you mean Jane, I suppose, because he danced with her\n"
        "twice. To be sure that _did_ seem as if he admired\n"
        "her--indeed I rather believe he _did_--I heard something\n"
        "about it--but I hardly know what--something about Mr.\n"
        "Robinson.\"\n"
        "\n"
        "\"Perhaps you mean what I overheard between him and Mr.\n"
        "Robinson; did not I mention it to you? Mr. Robinson's asking\n"
        "him how he liked our Meryton assemblies, and whether he did\n"
        "not think there were a great many pretty women in the room,\n"
        "and _which_ he thought the prettiest?  and his answering\n"
        "immediately to the last question: 'Oh! the eldest Miss\n"
        "Bennet, beyond a doubt; there cannot be two opinions on that\n"
        "point.'\"\n"
        "\n"
        "\"Upon my word! Well, that is very decided indeed--that does\n"
        "seem as if--but, however, it may all come to nothing, you\n"
        "know.\"\n"
        "\n"
        "\"_My_ overhearings were more to the purpose than _yours_,\n"
        "Eliza,\" said Charlotte. \"Mr. Darcy is not so well worth\n"
        "listening to as his friend, is he?--poor Eliza!--to be only\n"
        "just _tolerable_.\"\n"
        "\n"
        "\"I beg you would not put it into Lizzy's head to be vexed by\n"
        "his ill-treatment, for he is such a disagreeable man, that\n"
        "it would be quite a misfortune to be liked by him. Mrs. Long\n"
        "told me last night that he sat close to her for half-an-hour\n"
        "without once opening his lips.\"\n"
        "\n"
        "\"Are you quite sure, ma'am?--is not there a little mistake?\"\n"
        "said Jane.  \"I certainly saw Mr. Darcy speaking to her.\"\n"
        "\n"
        "\"Aye--because she asked him at last how he liked\n"
        "Netherfield, and he could not help answering her; but she\n"
        "said he seemed quite angry at being spoke to.\"\n"
        "\n"
        "\"Miss Bingley told me,\" said Jane, \"that he never speaks\n"
        "much, unless among his intimate acquaintances. With _them_\n"
        "he is remarkably agreeable.\"\n"
        "\n"
        "\"I do not believe a word of it, my dear. If he had been so\n"
        "very agreeable, he would have talked to Mrs. Long. But I can\n"
        "guess how it was; everybody says that he is eat up with\n"
        "pride, and I dare say he had heard somehow that Mrs. Long\n"
        "does not keep a carriage, and had come to the ball in a hack\n"
        "chaise.\"\n"
        "\n"
        "\"I do not mind his not talking to Mrs. Long,\" said Miss\n"
        "Lucas, \"but I wish he had danced with Eliza.\"\n"
        "\n"
        "\"Another time, Lizzy,\" said her mother, \"I would not dance\n"
        "with _him_, if I were you.\"\n"
        "\n"
        "\"I believe, ma'am, I may safely promise you _never_ to dance\n"
        "with him.\"\n"
        "\n"
        "\"His pride,\" said Miss Lucas, \"does not offend _me_ so much\n"
        "as pride often does, because there is an excuse for it. One\n"
        "cannot wonder that so very fine a young man, with family,\n"
        "fortune, everything in his favour, should think highly of\n"
        "himself. If I may so express it, he has a _right_ to be\n"
        "proud.\"\n"
        "\n"
        "\"That is very true,\" replied Elizabeth, \"and I could easily\n"
        "forgive _his_ pride, if he had not mortified _mine_.\"\n"
        "\n"
        "\"Pride,\" observed Mary, who piqued herself upon the solidity\n"
        "of her reflections, \"is a very common failing, I believe. By\n"
        "all that I have ever read, I am convinced that it is very\n"
        "common indeed; that human nature is particularly prone to\n"
        "it, and that there are very few of us who do not cherish a\n"
        "feeling of self-complacency on the score of some quality or\n"
        "other, real or imaginary. Vanity and pride are different\n"
        "things, though the words are often used synonymously. A\n"
        "person may be proud without being vain. Pride relates more\n"
        "to our opinion of ourselves, vanity to what we would have\n"
        "others think of us.\"\n"
        "\n"
        "\"If I were as rich as Mr. Darcy,\" cried a young Lucas, who\n"
        "came with his sisters, \"I should not care how proud I was. I\n"
        "would keep a pack of foxhounds, and drink a bottle of wine a\n"
        "day.\"\n"
        "\n"
        "\"Then you would drink a great deal more than you ought,\"\n"
        "said Mrs.  Bennet; \"and if I were to see you at it, I should\n"
        "take away your bottle directly.\"\n"
        "\n"
        "The boy protested that she should not; she continued to\n"
        "declare that she would, and the argument ended only with the\n"
        "visit.\n"
        "\n"
        "\n"
        "\n"
// xlC8 has an internal compiler error if this is too long..., so we're
// skipping the rest of the book...
        "********************************************************...\n"
        "*********** rest of book skipped to avoid compile errors...\n"
        "********************************************************...\n"
        "\n"
        "\n"
        "End of the Project Gutenberg EBook of Pride and Prejudice,\n"
        "by Jane Austen\n"
        "\n"
        "*** END OF THIS PROJECT GUTENBERG EBOOK PRIDE AND PREJUDICE\n"
        "***\n"
        "\n"
        "***** This file should be named 1342.txt or 1342.zip *****\n"
        "This and all associated files of various formats will be\n"
        "found in: http://www.gutenberg.org/1/3/4/1342/\n"
        "\n"
        "Produced by Anonymous Volunteers\n"
        "\n"
        "Updated editions will replace the previous one--the old\n"
        "editions will be renamed.\n"
        "\n"
        "Creating the works from public domain print editions means\n"
        "that no one owns a United States copyright in these works,\n"
        "so the Foundation (and you!) can copy and distribute it in\n"
        "the United States without permission and without paying\n"
        "copyright royalties.  Special rules, set forth in the\n"
        "General Terms of Use part of this license, apply to copying\n"
        "and distributing Project Gutenberg-tm electronic works to\n"
        "protect the PROJECT GUTENBERG-tm concept and trademark.\n"
        "Project Gutenberg is a registered trademark, and may not be\n"
        "used if you charge for the eBooks, unless you receive\n"
        "specific permission.  If you do not charge anything for\n"
        "copies of this eBook, complying with the rules is very easy.\n"
        "You may use this eBook for nearly any purpose such as\n"
        "creation of derivative works, reports, performances and\n"
        "research.  They may be modified and printed and given\n"
        "away--you may do practically ANYTHING with public domain\n"
        "eBooks.  Redistribution is subject to the trademark license,\n"
        "especially commercial redistribution.\n"
        "\n"
        "\n"
        "\n"
        "*** START: FULL LICENSE ***\n"
        "\n"
        "THE FULL PROJECT GUTENBERG LICENSE PLEASE READ THIS BEFORE\n"
        "YOU DISTRIBUTE OR USE THIS WORK\n"
        "\n"
        "To protect the Project Gutenberg-tm mission of promoting the\n"
        "free distribution of electronic works, by using or\n"
        "distributing this work (or any other work associated in any\n"
        "way with the phrase \"Project Gutenberg\"), you agree to\n"
        "comply with all the terms of the Full Project Gutenberg-tm\n"
        "License (available with this file or online at\n"
        "http://gutenberg.org/license).\n"
        "\n"
        "\n"
        "Section 1.  General Terms of Use and Redistributing Project\n"
        "Gutenberg-tm electronic works\n"
        "\n"
        "1.A.  By reading or using any part of this Project\n"
        "Gutenberg-tm electronic work, you indicate that you have\n"
        "read, understand, agree to and accept all the terms of this\n"
        "license and intellectual property (trademark/copyright)\n"
        "agreement.  If you do not agree to abide by all the terms of\n"
        "this agreement, you must cease using and return or destroy\n"
        "all copies of Project Gutenberg-tm electronic works in your\n"
        "possession.  If you paid a fee for obtaining a copy of or\n"
        "access to a Project Gutenberg-tm electronic work and you do\n"
        "not agree to be bound by the terms of this agreement, you\n"
        "may obtain a refund from the person or entity to whom you\n"
        "paid the fee as set forth in paragraph 1.E.8.\n"
        "\n"
        "1.B.  \"Project Gutenberg\" is a registered trademark.  It may\n"
        "only be used on or associated in any way with an electronic\n"
        "work by people who agree to be bound by the terms of this\n"
        "agreement.  There are a few things that you can do with most\n"
        "Project Gutenberg-tm electronic works even without complying\n"
        "with the full terms of this agreement.  See paragraph 1.C\n"
        "below.  There are a lot of things you can do with Project\n"
        "Gutenberg-tm electronic works if you follow the terms of\n"
        "this agreement and help preserve free future access to\n"
        "Project Gutenberg-tm electronic works.  See paragraph 1.E\n"
        "below.\n"
        "\n"
        "1.C.  The Project Gutenberg Literary Archive Foundation\n"
        "(\"the Foundation\" or PGLAF), owns a compilation copyright in\n"
        "the collection of Project Gutenberg-tm electronic works.\n"
        "Nearly all the individual works in the collection are in the\n"
        "public domain in the United States.  If an individual work\n"
        "is in the public domain in the United States and you are\n"
        "located in the United States, we do not claim a right to\n"
        "prevent you from copying, distributing, performing,\n"
        "displaying or creating derivative works based on the work as\n"
        "long as all references to Project Gutenberg are removed.  Of\n"
        "course, we hope that you will support the Project\n"
        "Gutenberg-tm mission of promoting free access to electronic\n"
        "works by freely sharing Project Gutenberg-tm works in\n"
        "compliance with the terms of this agreement for keeping the\n"
        "Project Gutenberg-tm name associated with the work.  You can\n"
        "easily comply with the terms of this agreement by keeping\n"
        "this work in the same format with its attached full Project\n"
        "Gutenberg-tm License when you share it without charge with\n"
        "others.\n"
        "\n"
        "1.D.  The copyright laws of the place where you are located\n"
        "also govern what you can do with this work.  Copyright laws\n"
        "in most countries are in a constant state of change.  If you\n"
        "are outside the United States, check the laws of your\n"
        "country in addition to the terms of this agreement before\n"
        "downloading, copying, displaying, performing, distributing\n"
        "or creating derivative works based on this work or any other\n"
        "Project Gutenberg-tm work.  The Foundation makes no\n"
        "representations concerning the copyright status of any work\n"
        "in any country outside the United States.\n"
        "\n"
        "1.E.  Unless you have removed all references to Project\n"
        "Gutenberg:\n"
        "\n"
        "1.E.1.  The following sentence, with active links to, or\n"
        "other immediate access to, the full Project Gutenberg-tm\n"
        "License must appear prominently whenever any copy of a\n"
        "Project Gutenberg-tm work (any work on which the phrase\n"
        "\"Project Gutenberg\" appears, or with which the phrase\n"
        "\"Project Gutenberg\" is associated) is accessed, displayed,\n"
        "performed, viewed, copied or distributed:\n"
        "\n"
        "This eBook is for the use of anyone anywhere at no cost and\n"
        "with almost no restrictions whatsoever.  You may copy it,\n"
        "give it away or re-use it under the terms of the Project\n"
        "Gutenberg License included with this eBook or online at\n"
        "www.gutenberg.org\n"
        "\n"
        "1.E.2.  If an individual Project Gutenberg-tm electronic\n"
        "work is derived from the public domain (does not contain a\n"
        "notice indicating that it is posted with permission of the\n"
        "copyright holder), the work can be copied and distributed to\n"
        "anyone in the United States without paying any fees or\n"
        "charges.  If you are redistributing or providing access to a\n"
        "work with the phrase \"Project Gutenberg\" associated with or\n"
        "appearing on the work, you must comply either with the\n"
        "requirements of paragraphs 1.E.1 through 1.E.7 or obtain\n"
        "permission for the use of the work and the Project\n"
        "Gutenberg-tm trademark as set forth in paragraphs 1.E.8 or\n"
        "1.E.9.\n"
        "\n"
        "1.E.3.  If an individual Project Gutenberg-tm electronic\n"
        "work is posted with the permission of the copyright holder,\n"
        "your use and distribution must comply with both paragraphs\n"
        "1.E.1 through 1.E.7 and any additional terms imposed by the\n"
        "copyright holder.  Additional terms will be linked to the\n"
        "Project Gutenberg-tm License for all works posted with the\n"
        "permission of the copyright holder found at the beginning of\n"
        "this work.\n"
        "\n"
        "1.E.4.  Do not unlink or detach or remove the full Project\n"
        "Gutenberg-tm License terms from this work, or any files\n"
        "containing a part of this work or any other work associated\n"
        "with Project Gutenberg-tm.\n"
        "\n"
        "1.E.5.  Do not copy, display, perform, distribute or\n"
        "redistribute this electronic work, or any part of this\n"
        "electronic work, without prominently displaying the sentence\n"
        "set forth in paragraph 1.E.1 with active links or immediate\n"
        "access to the full terms of the Project Gutenberg-tm\n"
        "License.\n"
        "\n"
        "1.E.6.  You may convert to and distribute this work in any\n"
        "binary, compressed, marked up, nonproprietary or proprietary\n"
        "form, including any word processing or hypertext form.\n"
        "However, if you provide access to or distribute copies of a\n"
        "Project Gutenberg-tm work in a format other than \"Plain\n"
        "Vanilla ASCII\" or other format used in the official version\n"
        "posted on the official Project Gutenberg-tm web site\n"
        "(www.gutenberg.org), you must, at no additional cost, fee or\n"
        "expense to the user, provide a copy, a means of exporting a\n"
        "copy, or a means of obtaining a copy upon request, of the\n"
        "work in its original \"Plain Vanilla ASCII\" or other form.\n"
        "Any alternate format must include the full Project\n"
        "Gutenberg-tm License as specified in paragraph 1.E.1.\n"
        "\n"
        "1.E.7.  Do not charge a fee for access to, viewing,\n"
        "displaying, performing, copying or distributing any Project\n"
        "Gutenberg-tm works unless you comply with paragraph 1.E.8 or\n"
        "1.E.9.\n"
        "\n"
        "1.E.8.  You may charge a reasonable fee for copies of or\n"
        "providing access to or distributing Project Gutenberg-tm\n"
        "electronic works provided that\n"
        "\n"
        "- You pay a royalty fee of 20% of the gross profits you\n"
        "  derive from the use of Project Gutenberg-tm works\n"
        "  calculated using the method you already use to calculate\n"
        "  your applicable taxes.  The fee is owed to the owner of\n"
        "  the Project Gutenberg-tm trademark, but he has agreed to\n"
        "  donate royalties under this paragraph to the Project\n"
        "  Gutenberg Literary Archive Foundation.  Royalty payments\n"
        "  must be paid within 60 days following each date on which\n"
        "  you prepare (or are legally required to prepare) your\n"
        "  periodic tax returns.  Royalty payments should be clearly\n"
        "  marked as such and sent to the Project Gutenberg Literary\n"
        "  Archive Foundation at the address specified in Section 4,\n"
        "  \"Information about donations to the Project Gutenberg\n"
        "  Literary Archive Foundation.\"\n"
        "\n"
        "- You provide a full refund of any money paid by a user who\n"
        "  notifies you in writing (or by e-mail) within 30 days of\n"
        "  receipt that s/he does not agree to the terms of the full\n"
        "  Project Gutenberg-tm License.  You must require such a\n"
        "  user to return or destroy all copies of the works\n"
        "  possessed in a physical medium and discontinue all use of\n"
        "  and all access to other copies of Project Gutenberg-tm\n"
        "  works.\n"
        "\n"
        "- You provide, in accordance with paragraph 1.F.3, a full\n"
        "  refund of any money paid for a work or a replacement copy,\n"
        "  if a defect in the electronic work is discovered and\n"
        "      reported to you within 90 days of receipt of the work.\n"
        "\n"
        "- You comply with all other terms of this agreement for free\n"
        "  distribution of Project Gutenberg-tm works.\n"
        "\n"
        "1.E.9.  If you wish to charge a fee or distribute a Project\n"
        "Gutenberg-tm electronic work or group of works on different\n"
        "terms than are set forth in this agreement, you must obtain\n"
        "permission in writing from both the Project Gutenberg\n"
        "Literary Archive Foundation and Michael Hart, the owner of\n"
        "the Project Gutenberg-tm trademark.  Contact the Foundation\n"
        "as set forth in Section 3 below.\n"
        "\n"
        "1.F.\n"
        "\n"
        "1.F.1.  Project Gutenberg volunteers and employees expend\n"
        "considerable effort to identify, do copyright research on,\n"
        "transcribe and proofread public domain works in creating the\n"
        "Project Gutenberg-tm collection.  Despite these efforts,\n"
        "Project Gutenberg-tm electronic works, and the medium on\n"
        "which they may be stored, may contain \"Defects,\" such as,\n"
        "but not limited to, incomplete, inaccurate or corrupt data,\n"
        "transcription errors, a copyright or other intellectual\n"
        "property infringement, a defective or damaged disk or other\n"
        "medium, a computer virus, or computer codes that damage or\n"
        "cannot be read by your equipment.\n"
        "\n"
        "1.F.2.  LIMITED WARRANTY, DISCLAIMER OF DAMAGES - Except for\n"
        "the \"Right of Replacement or Refund\" described in paragraph\n"
        "1.F.3, the Project Gutenberg Literary Archive Foundation,\n"
        "the owner of the Project Gutenberg-tm trademark, and any\n"
        "other party distributing a Project Gutenberg-tm electronic\n"
        "work under this agreement, disclaim all liability to you for\n"
        "damages, costs and expenses, including legal fees.  YOU\n"
        "AGREE THAT YOU HAVE NO REMEDIES FOR NEGLIGENCE, STRICT\n"
        "LIABILITY, BREACH OF WARRANTY OR BREACH OF CONTRACT EXCEPT\n"
        "THOSE PROVIDED IN PARAGRAPH F3.  YOU AGREE THAT THE\n"
        "FOUNDATION, THE TRADEMARK OWNER, AND ANY DISTRIBUTOR UNDER\n"
        "THIS AGREEMENT WILL NOT BE LIABLE TO YOU FOR ACTUAL, DIRECT,\n"
        "INDIRECT, CONSEQUENTIAL, PUNITIVE OR INCIDENTAL DAMAGES EVEN\n"
        "IF YOU GIVE NOTICE OF THE POSSIBILITY OF SUCH DAMAGE.\n"
        "\n"
        "1.F.3.  LIMITED RIGHT OF REPLACEMENT OR REFUND - If you\n"
        "discover a defect in this electronic work within 90 days of\n"
        "receiving it, you can receive a refund of the money (if any)\n"
        "you paid for it by sending a written explanation to the\n"
        "person you received the work from.  If you received the work\n"
        "on a physical medium, you must return the medium with your\n"
        "written explanation.  The person or entity that provided you\n"
        "with the defective work may elect to provide a replacement\n"
        "copy in lieu of a refund.  If you received the work\n"
        "electronically, the person or entity providing it to you may\n"
        "choose to give you a second opportunity to receive the work\n"
        "electronically in lieu of a refund.  If the second copy is\n"
        "also defective, you may demand a refund in writing without\n"
        "further opportunities to fix the problem.\n"
        "\n"
        "1.F.4.  Except for the limited right of replacement or\n"
        "refund set forth in paragraph 1.F.3, this work is provided\n"
        "to you 'AS-IS' WITH NO OTHER WARRANTIES OF ANY KIND, EXPRESS\n"
        "OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES OF\n"
        "MERCHANTIBILITY OR FITNESS FOR ANY PURPOSE.\n"
        "\n"
        "1.F.5.  Some states do not allow disclaimers of certain\n"
        "implied warranties or the exclusion or limitation of certain\n"
        "types of damages.  If any disclaimer or limitation set forth\n"
        "in this agreement violates the law of the state applicable\n"
        "to this agreement, the agreement shall be interpreted to\n"
        "make the maximum disclaimer or limitation permitted by the\n"
        "applicable state law.  The invalidity or unenforceability of\n"
        "any provision of this agreement shall not void the remaining\n"
        "provisions.\n"
        "\n"
        "1.F.6.  INDEMNITY - You agree to indemnify and hold the\n"
        "Foundation, the trademark owner, any agent or employee of\n"
        "the Foundation, anyone providing copies of Project\n"
        "Gutenberg-tm electronic works in accordance with this\n"
        "agreement, and any volunteers associated with the\n"
        "production, promotion and distribution of Project\n"
        "Gutenberg-tm electronic works, harmless from all liability,\n"
        "costs and expenses, including legal fees, that arise\n"
        "directly or indirectly from any of the following which you\n"
        "do or cause to occur: (a) distribution of this or any\n"
        "    Project Gutenberg-tm work, (b) alteration, modification,\n"
        "    or additions or deletions to any Project Gutenberg-tm\n"
        "    work, and (c) any Defect you cause.\n"
        "\n"
        "\n"
        "Section  2.  Information about the Mission of Project\n"
        "Gutenberg-tm\n"
        "\n"
        "Project Gutenberg-tm is synonymous with the free\n"
        "distribution of electronic works in formats readable by the\n"
        "widest variety of computers including obsolete, old,\n"
        "middle-aged and new computers.  It exists because of the\n"
        "efforts of hundreds of volunteers and donations from people\n"
        "in all walks of life.\n"
        "\n"
        "Volunteers and financial support to provide volunteers with\n"
        "the assistance they need, is critical to reaching Project\n"
        "Gutenberg-tm's goals and ensuring that the Project\n"
        "Gutenberg-tm collection will remain freely available for\n"
        "generations to come.  In 2001, the Project Gutenberg\n"
        "Literary Archive Foundation was created to provide a secure\n"
        "and permanent future for Project Gutenberg-tm and future\n"
        "generations.  To learn more about the Project Gutenberg\n"
        "Literary Archive Foundation and how your efforts and\n"
        "donations can help, see Sections 3 and 4 and the Foundation\n"
        "web page at http://www.pglaf.org.\n"
        "\n"
        "\n"
        "Section 3.  Information about the Project Gutenberg Literary\n"
        "Archive Foundation\n"
        "\n"
        "The Project Gutenberg Literary Archive Foundation is a non\n"
        "profit 501(c)(3) educational corporation organized under the\n"
        "laws of the state of Mississippi and granted tax exempt\n"
        "status by the Internal Revenue Service.  The Foundation's\n"
        "EIN or federal tax identification number is 64-6221541.  Its\n"
        "501(c)(3) letter is posted at http://pglaf.org/fundraising.\n"
        "Contributions to the Project Gutenberg Literary Archive\n"
        "Foundation are tax deductible to the full extent permitted\n"
        "by U.S. federal laws and your state's laws.\n"
        "\n"
        "The Foundation's principal office is located at 4557 Melan\n"
        "Dr. S.  Fairbanks, AK, 99712., but its volunteers and\n"
        "employees are scattered throughout numerous locations.  Its\n"
        "business office is located at 809 North 1500 West, Salt Lake\n"
        "City, UT 84116, (801) 596-1887, email business@pglaf.org.\n"
        "Email contact links and up to date contact information can\n"
        "be found at the Foundation's web site and official page at\n"
        "http://pglaf.org\n"
        "\n"
        "For additional contact information: Dr. Gregory B. Newby\n"
        "Chief Executive and Director gbnewby@pglaf.org\n"
        "\n"
        "\n"
        "Section 4.  Information about Donations to the Project\n"
        "Gutenberg Literary Archive Foundation\n"
        "\n"
        "Project Gutenberg-tm depends upon and cannot survive without\n"
        "wide spread public support and donations to carry out its\n"
        "mission of increasing the number of public domain and\n"
        "licensed works that can be freely distributed in machine\n"
        "readable form accessible by the widest array of equipment\n"
        "including outdated equipment.  Many small donations ($1 to\n"
        "$5,000) are particularly important to maintaining tax exempt\n"
        "status with the IRS.\n"
        "\n"
        "The Foundation is committed to complying with the laws\n"
        "regulating charities and charitable donations in all 50\n"
        "states of the United States.  Compliance requirements are\n"
        "not uniform and it takes a considerable effort, much\n"
        "paperwork and many fees to meet and keep up with these\n"
        "requirements.  We do not solicit donations in locations\n"
        "where we have not received written confirmation of\n"
        "compliance.  To SEND DONATIONS or determine the status of\n"
        "compliance for any particular state visit http://pglaf.org\n"
        "\n"
        "While we cannot and do not solicit contributions from states\n"
        "where we have not met the solicitation requirements, we know\n"
        "of no prohibition against accepting unsolicited donations\n"
        "from donors in such states who approach us with offers to\n"
        "donate.\n"
        "\n"
        "International donations are gratefully accepted, but we\n"
        "cannot make any statements concerning tax treatment of\n"
        "donations received from outside the United States.  U.S.\n"
        "laws alone swamp our small staff.\n"
        "\n"
        "Please check the Project Gutenberg Web pages for current\n"
        "donation methods and addresses.  Donations are accepted in a\n"
        "number of other ways including checks, online payments and\n"
        "credit card donations.  To donate, please visit:\n"
        "http://pglaf.org/donate\n"
        "\n"
        "\n"
        "Section 5.  General Information About Project Gutenberg-tm\n"
        "electronic works.\n"
        "\n"
        "Professor Michael S. Hart is the originator of the Project\n"
        "Gutenberg-tm concept of a library of electronic works that\n"
        "could be freely shared with anyone.  For thirty years, he\n"
        "produced and distributed Project Gutenberg-tm eBooks with\n"
        "only a loose network of volunteer support.\n"
        "\n"
        "\n"
        "Project Gutenberg-tm eBooks are often created from several\n"
        "printed editions, all of which are confirmed as Public\n"
        "Domain in the U.S.  unless a copyright notice is included.\n"
        "Thus, we do not necessarily keep eBooks in compliance with\n"
        "any particular paper edition.\n"
        "\n"
        "\n"
        "Most people start at our Web site which has the main PG\n"
        "search facility:\n"
        "\n"
        "     http://www.gutenberg.org\n"
        "\n"
        "This Web site includes information about Project\n"
        "Gutenberg-tm, including how to make donations to the Project\n"
        "Gutenberg Literary Archive Foundation, how to help produce\n"
        "our new eBooks, and how to subscribe to our email newsletter\n"
        "to hear about new eBooks.\n"
    };

    bsl::size_t     prideLen = sizeof(prideAndPrejudice);

    unsigned short *ucs2Buffer_p = new unsigned short[prideLen];
    char           *utf8Buffer_p = new char[prideLen];
    bsl::size_t     charsWritten = 0;
    bsl::size_t     bytesWritten = 0;

    const int       iterLimit    = 1000;

    bsls::Stopwatch s;

    s.start();

    for (int i = 0; i < iterLimit; ++i) {
        ASSERT(SUCCESS == bdlde::CharConvertUcs2::utf8ToUcs2(ucs2Buffer_p,
                                                            prideLen,
                                                            prideAndPrejudice,
                                                            &charsWritten));
        ASSERT(charsWritten == prideLen);

        ASSERT(0 == bdlde::CharConvertUcs2::ucs2ToUtf8(utf8Buffer_p,
                                                      prideLen,
                                                      ucs2Buffer_p,
                                                      &charsWritten,
                                                      &bytesWritten));
        ASSERT(charsWritten == prideLen);
        ASSERT(bytesWritten == prideLen);
    }

    s.stop();

    cout << "Performance test for C-style interface , converted "
         << prideLen << " characters "
         << "back and forth " << iterLimit << " times in " << s.elapsedTime()
         << " seconds."
         << endl;

    ASSERT(0 == strcmp(utf8Buffer_p, prideAndPrejudice));

    delete [] utf8Buffer_p;
    delete [] ucs2Buffer_p;

    bsl::string                 cppPrideUtf8;
    bsl::vector<unsigned short> cppPrideUcs2;

    s.reset();
    s.start();

    for (int i = 0; i < iterLimit; ++i) {
        ASSERT(SUCCESS == bdlde::CharConvertUcs2::utf8ToUcs2(&cppPrideUcs2,
                                                           prideAndPrejudice));

        ASSERT(cppPrideUcs2.size() == prideLen);

        if (cppPrideUcs2.size() != prideLen) {
            cout << "cppPrideUcs2.size() == "
                 << cppPrideUcs2.size()
                 << ", prideLen = "
                 << prideLen
                 << endl;
        }

        ASSERT(0 == bdlde::CharConvertUcs2::ucs2ToUtf8(&cppPrideUtf8,
                                                      &(cppPrideUcs2[0]),
                                                      &charsWritten));
        ASSERT(charsWritten == prideLen);
        ASSERT(cppPrideUtf8.length() == prideLen - 1);

        if (cppPrideUtf8.length() != prideLen - 1) {
            cout << "cppPrideUtf8.length() == "
                 << cppPrideUtf8.length()
                 << ", prideLen = "
                 << prideLen
                 << endl;
        }


    }

    s.stop();

    cout << "Performance test for C++-style interface , converted "
         << prideLen << " characters "
         << "back and forth " << iterLimit << " times in " << s.elapsedTime()
         << " seconds."
         << endl;

    ASSERT(cppPrideUtf8 == prideAndPrejudice);

    return 0;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char**argv)
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use utf8ToUcs2 via the
        //   C++-style interface.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        bsl::vector<unsigned short> result;
        loadUCS2Hello(&result);
        ASSERT('H' == result[0]);
        ASSERT('e' == result[1]);
        ASSERT('l' == result[2]);
        ASSERT('l' == result[3]);
        ASSERT('o' == result[4]);
        ASSERT( 0  == result[5]);

        checkCppRoundTrip();

        bsl::string source ="\xc3\x89" "cole";
        result = processUtf8(source);
        ASSERT(0xc9 == result[0]); // Unicode-E WITH ACUTE, LATIN CAPITAL
                                   // LETTER
        ASSERT('c'  == result[1]);
        ASSERT('o'  == result[2]);
        ASSERT('l'  == result[3]);
        ASSERT('e'  == result[4]);
        ASSERT( 6   == result.size());
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use utf8ToUcs2 via the
        //   C-style interface.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        testCFunction2();
        processUtf8("");
        processUtf8("\x01\x20\x7f\xc3\xbf\xdf\xbf\xe0\xa0\x80\xef\xbf\xbf");
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use utf8ToUcs2 via the
        //   C-style interface.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        testCFunction1();
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // CONVERT UCS-2 TO UTF-8
        //   Ability to convert UCS-2 to UTF-8 correctly.
        //
        // Concerns:
        //   - That all valid and invalid inputs are handled correctly.
        //   - Make sure short buffers are treated right for 1-byte characters.
        //   - Make sure short buffers are treated right for multiple 1-byte
        //     characters.
        //   - Check handling of boundary between 1-octet and
        //     2-octet characters.
        //   - Check handling of boundary between 1-octet and 2-octet
        //     characters for correct handling of short buffers and correct
        //     conversion.
        //   - Make sure short buffers are treated right and conversion is
        //     correct for 2-byte characters.
        //   - Make sure short buffers are treated right and conversion is
        //     correct for 3-byte characters.
        //   - Make sure short buffers are treated right and conversion is
        //     correct for 3-byte characters at end of longer strings.
        //   - Make sure that 0 dstCapacity is handled correctly, whether or
        //     not '&bytesWritten' and/or '&charsWritten' are passed in.
        //   - Make sure that all permutations of 1-octet, 2-octet, and 3-octet
        //     characters are converted correctly or handled correctly for
        //     short output buffers.
        //   - Make sure the C++-style interface matches the behavior of the
        //     C-style interface for all cases where the output buffer length
        //     is sufficient.
        //
        // Plan:
        //   - Test 7-bit ASCII conversions
        //   - Test characters that convert to 2-octet utf-8
        //   - Test characters that convert to 3-octet utf-8
        //   - All tests will be repeated with insufficient output space
        //   - Use 'buildUpAndTestStringsU2ToU8' to exhaustively check
        //     combinations of 1-octet, 2-octet, and 3-octet characters for
        //     correct conversion and correct handling of insufficient
        //     'dstCapacity' arguments.
        //   - Test C++-style interface for all cases where the output buffer
        //     is sufficiently wide.
        //
        // Testing:
        //   ucs2ToUtf8
        // --------------------------------------------------------------------

        if (verbose) cout <<
           "\nTesting 'bdlde::CharConvertUcs2::ucs2ToUtf8'." << endl;
        {
            static const struct {
                int                   d_lineNum;        // source line number

                const char           *d_output_p;       // utf-8 input string

                const unsigned short  d_spec_p[256];    // expected ucs2 output

                bsl::size_t           d_bufsize;        // buffer size to claim
                                                        //

                bsl::size_t           d_expectedChar;   // expected output char
                                                        // count

                bsl::size_t           d_expectedByte;   // expected output byte
                                                        // count

                int                   d_expectedResult; // expected result
            } DATA[] = {
// v------------^
   //L#  output           input                            size #c   #b  result
   //--  -----            ------                           ---- --   --  ------
   { L_, "",              {                            0 }, 255, 1,  1,   OK },

   // Make sure short buffers are treated right for 1-byte characters.
   { L_, "",              { 0x20,                      0 },   1, 1,  1, OBTS },
   { L_, " ",             { 0x20,                      0 },   2, 2,  2,   OK },
   { L_, " ",             { 0x20,                      0 }, 255, 2,  2,   OK },

   // Make sure short buffers are treated right for multiple 1-byte characters.
   { L_, "",              { 'H', 'e', 'l', 'l', 'o',   0 },   1, 1,  1, OBTS },
   { L_, "H",             { 'H', 'e', 'l', 'l', 'o',   0 },   2, 2,  2, OBTS },
   { L_, "He",            { 'H', 'e', 'l', 'l', 'o',   0 },   3, 3,  3, OBTS },
   { L_, "Hel",           { 'H', 'e', 'l', 'l', 'o',   0 },   4, 4,  4, OBTS },
   { L_, "Hell",          { 'H', 'e', 'l', 'l', 'o',   0 },   5, 5,  5, OBTS },
   { L_, "Hello",         { 'H', 'e', 'l', 'l', 'o',   0 },   6, 6,  6,   OK },
   { L_, "Hello",         { 'H', 'e', 'l', 'l', 'o',   0 }, 255, 6,  6,   OK },

   // Check handling of boundary between 1-octet and 2-octet characters for
   // correct handling of short buffers and correct conversion.
   { L_, "",              { 0x7f,                      0 },   1, 1,  1, OBTS },
   { L_, "\x7f",          { 0x7f,                      0 }, 255, 2,  2,   OK },
   { L_, U8_00080,        { 0x80,                      0 }, 255, 2,  3,   OK },
   { L_, U8_00080 " ",    { 0x80, ' ',                 0 }, 255, 3,  4,   OK },

   { L_, U8_000ff,        { 0xff,                      0 }, 255, 2,  3,   OK },

   // Make sure short buffers are treated right and conversion is correct for
   // 2-byte characters.
   { L_, "",              { 0x07ff,                    0 },   1, 1,  1, OBTS },
   { L_, "",              { 0x07ff,                    0 },   2, 1,  1, OBTS },
   { L_, U8_007ff,        { 0x07ff,                    0 },   3, 2,  3,   OK },
   { L_, U8_007ff,        { 0x07ff,                    0 }, 255, 2,  3,   OK },

   // Make sure short buffers are treated right for 3-byte characters.
   { L_, "",              { 0x0800,                    0 },   1, 1,  1, OBTS },
   { L_, "",              { 0x0800,                    0 },   2, 1,  1, OBTS },
   { L_, "",              { 0x0800,                    0 },   3, 1,  1, OBTS },
   { L_, U8_00800,        { 0x0800,                    0 },   4, 2,  4,   OK },
   { L_, U8_000ff,        { 0xff,                      0 }, 255, 2,  3,   OK },
   { L_, U8_0ffff,        { 0xffff,                    0 }, 255, 2,  4,   OK },

   // Make sure short buffers are treated right for 3-byte characters at end of
   // longer strings.
   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                          { 0x1, 0x20, 0x7f, 0xff,
                            0x7ff, 0x800, 0xffff,      0 },  11, 7, 11, OBTS },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                          { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                            0x800, 0xffff,             0 },  12, 7, 11, OBTS },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                          { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                            0x800, 0xffff,             0 },  13, 7, 11, OBTS },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                          { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                            0x800, 0xffff,             0 },  14, 8, 14,   OK },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                          { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                            0x800, 0xffff,             0 }, 255, 8, 14,   OK },

   { L_, U8_0ffff U8_00800 U8_007ff U8_000ff "\x7f\x20\x01",
                          { 0xffff, 0x800, 0X7ff, 0Xff, 0X7f,
                            0x20, 0x1,                 0 }, 255, 8, 14,   OK },
// ^------------v
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE          = DATA[i].d_lineNum;
                const char           *OUTPUT        = DATA[i].d_output_p;
                const unsigned short *SPEC          = DATA[i].d_spec_p;
                const bsl::size_t     BUFSIZE       = DATA[i].d_bufsize;
                const bsl::size_t     EXPECTEDCHARS = DATA[i].d_expectedChar;
                const bsl::size_t     EXPECTEDBYTES = DATA[i].d_expectedByte;
                const int             RETURN        = DATA[i].d_expectedResult;

                char           results[256] = { 0 };
                bsl::size_t    resultLen = sizeof results / sizeof *results;
                bsl::size_t    charsWritten = -1;
                bsl::size_t    bytesWritten = -1;

                int retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                                      results,
                                      BUFSIZE,
                                      SPEC,
                                      &charsWritten,
                                      &bytesWritten);

                LOOP_ASSERT(LINE, charsWritten == EXPECTEDCHARS);
                LOOP_ASSERT(LINE, bytesWritten == EXPECTEDBYTES);
                LOOP_ASSERT(LINE, retVal       == RETURN);
                LOOP_ASSERT(LINE, 0 == memcmp(OUTPUT,
                                              results,
                                              charsWritten));
                if (veryVerbose) {
                    printf( "%4d: SPEC     ", LINE);
                    printStr(SPEC);
                    printf("\n");
                    printf("    : EXPECTEDCHARS = %d charsWritten = %d\n",
                                  EXPECTEDCHARS,
                                  charsWritten);
                    printf("    : EXPECTEDBYTES = %d bytesWritten = %d\n",
                                  EXPECTEDBYTES,
                                  bytesWritten);
                    printf("    : RETURN        = %d retVal       = %d\n",
                                  RETURN,
                                  retVal);

                    printf("    : OUTPUT   ");
                    printStr(OUTPUT);
                    printf("\n");

                    printf("    : results  ");
                    printStr(results);
                    printf("\n");
                }

                if (OK == RETURN) {
                    bsl::string cppResult;

                    int retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                                          &cppResult,
                                          SPEC,
                                          &charsWritten);

                    LOOP_ASSERT(LINE, charsWritten       == EXPECTEDCHARS);
                    LOOP_ASSERT(LINE, cppResult.length() == EXPECTEDBYTES - 1);
                    LOOP_ASSERT(LINE, retVal             == RETURN);
                    LOOP_ASSERT(LINE, 0 == memcmp(OUTPUT,
                                                  cppResult.c_str(),
                                                  cppResult.length()));
                }

            }
        }

        // Make sure that 0 dstCapacity is handled correctly, whether or not
        // '&bytesWritten' and/or '&charsWritten' are passed in.

        {
            int retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                    (char*)0, // add cast to avoid overload ambiguity
                    0,
                    0);
            ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
            ASSERT(                      2 == retVal);

            bsl::size_t charsWritten = 1;
            bsl::size_t bytesWritten = 1;

            retVal = bdlde::CharConvertUcs2::ucs2ToUtf8(
                    0,
                    0,
                    0,
                    &charsWritten);
                    // not passing in &bytesWritten
            ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
            ASSERT(                      2 == retVal);
            ASSERT(                      0 == charsWritten);
            ASSERT(                      1 == bytesWritten);

            charsWritten = 1;
            bytesWritten = 1;
            retVal =bdlde::CharConvertUcs2::ucs2ToUtf8(
                    0,
                    0,
                    0,
                    0, // Not passing in &charsWritten
                    &bytesWritten);
            ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
            ASSERT(                      2 == retVal);
            ASSERT(                      1 == charsWritten);
            ASSERT(                      0 == bytesWritten);

            charsWritten = 1;
            bytesWritten = 1;
            retVal =bdlde::CharConvertUcs2::ucs2ToUtf8(
                    0,
                    0,
                    0,
                    &charsWritten,
                    &bytesWritten);
            ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
            ASSERT(                      2 == retVal);
            ASSERT(                      0 == charsWritten);
            ASSERT(                      0 == bytesWritten);
        }

        //   - Make sure that all permutations of 1-octet, 2-octet, and 3-octet
        //     characters are converted correctly or handled correctly for
        //     short output buffers.

        {
            for(int i = 0; i < precomputedDataCount; ++i) {
                unsigned short inputBuffer[256]    = { 0 };
                char           outputBuffer[256]   = { 0 };
                unsigned short characterSizes[256] = { 0 };
                bsl::size_t    totalOutputLength   = 0;
                bsl::size_t    characterCount      = 0;

                unsigned short *inputCursor  = inputBuffer;
                char           *outputCursor = outputBuffer;

                buildUpAndTestStringsU2ToU8(i,
                                            exhaustiveSearchDepth,
                                            inputBuffer,
                                            outputBuffer,
                                            characterSizes,
                                            totalOutputLength,
                                            characterCount,
                                            inputCursor,
                                            outputCursor,
                                            verbose,
                                            veryVerbose);
            }
        }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // CONVERT UTF-8 TO UCS-2
        //   Ability to convert UTF-8 to UCS-2 correctly.
        //
        // Concerns:
        //   - Make sure short buffers are handled correctly.
        //   - Check the boundary between 1-octet and 2-octet characters.
        //   - Check the boundary between 2-octet and 3-octet characters.
        //   - Check the maximal 3-octet character.
        //   - Make sure 4-octet characters are handled correctly (with a
        //     single error).
        //   - Make sure partial 4-octet characters are handled correctly (with
        //     a single error).
        //   - Make sure the "illegal" UTF-8 characters are handled correctly:
        //          o  The octet values C0, C1, F5 to FF never appear.
        //             (however, C0 and C1 indicate the start of overlong
        //             encodings, which this implementation accepts)
        //   - Make sure that the "illegal" UTF-8 characters are handled
        //     correctly mid-string:
        //          o  The octet values C0, C1, F5 to FF never appear.
        //             (however, C0 and C1 indicate the start of overlong
        //             encodings, which this implementation accepts)
        //   - Make sure short buffers are handled correctly for long input
        //     chars.
        //   - Make sure illegal overlong encodings are accepted.
        //   - Make sure that the "skip" logic for corrupted extended
        //     characters:
        //       - operates correctly, without losing any usable data
        //       - handles corrupted 2-octet characters
        //       - handles corrupted 2-octet character followed by a valid
        //         character
        //       - handles 3-octet characters corrupted after octet 1
        //       - handles 3-octet characters corrupted after octet 2
        //   - Make sure the C++-style interface matches the behavior of the
        //     C-style interface for all cases where the output buffer length
        //     is sufficient.
        //
        // Plan:
        //   - Test 7-bit ASCII conversions
        //   - Test 2-octet utf-8 character conversions
        //   - Test 3-octet utf-8 character conversions
        //   - Test invalid utf-8 character conversions
        //   - All tests will be repeated with insufficient output space
        //   - Use 'buildUpAndTestStringsU8ToU2' to exhaustively check
        //     combinations of 1-octet, 2-octet, and 3-octet characters for
        //     correct conversion and correct handling of insufficient
        //     'dstCapacity' arguments.
        //   - Test C++-style interface for all cases where the output buffer
        //     is sufficiently wide.
        //
        // Testing:
        //   utf8ToUcs2
        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting 'bdlde::CharConvertUcs2::utf8ToUcs2'." << endl;
        {
            static const struct {
                int                   d_lineNum;        // source line number

                const char           *d_spec_p;         // utf-8 input string

                const unsigned short  d_output_p[256];  // expected ucs2 output

                bsl::size_t           d_bufsize;        // buffer size to claim

                bsl::size_t           d_expectedCount;  // expected output char
                                                        // count

                int                   d_expectedResult; // expected result
            } DATA[] = {
// v------------^
   //L#  input             output                               size #c  result
   //--  -----             ------                               ---- --  ------
   { L_, "",               {                               0 },   0, 0, OBTS },
   { L_, "",               {                               0 },   1, 1,   OK },
   { L_, "",               {                               0 }, 255, 1,   OK },
   { L_, " ",              { 0x20,                         0 }, 255, 2,   OK },

   // Make sure short buffers are handled correctly.
   { L_, "Hello",          {                               0 },   1, 1, OBTS },
   { L_, "Hello",          { 'H',                          0 },   2, 2, OBTS },
   { L_, "Hello",          { 'H', 'e', 'l', 'l',           0 },   5, 5, OBTS },
   { L_, "Hello",          { 'H', 'e', 'l', 'l', 'o',      0 },   6, 6,   OK },
   { L_, "Hello",          { 'H', 'e', 'l', 'l', 'o',      0 }, 255, 6,   OK },

   // Check the boundary between 1-octet and 2-octet characters.
   { L_, "\x7f",           {                               0 },   1, 1, OBTS },
   { L_, "\x7f",           { 0x7f,                         0 }, 255, 2,   OK },
   { L_, U8_00080,         {                               0 },   1, 1, OBTS },
   { L_, U8_00080,         { 0x80,                         0 }, 255, 2,   OK },

   // Check the boundary between 2-octet and 3-octet characters.
   { L_, U8_007ff,         {                               0 },   1, 1, OBTS },
   { L_, U8_007ff,         { 0x07ff,                       0 }, 255, 2,   OK },
   { L_, U8_00800,         {                               0 },   1, 1, OBTS },
   { L_, U8_00800,         { 0x0800,                       0 }, 255, 2,   OK },

   // Check the maximal 3-octet character.
   { L_, U8_0ffff,         {                               0 },   1, 1, OBTS },
   { L_, U8_0ffff,         { 0xffff,                       0 }, 255, 2,   OK },

   // Make sure 4-octet characters are handled correctly (with a single error).
   { L_, U8_10000,         {                               0 },   1, 1, OBTS },
   { L_, U8_10000,         { '?',                          0 }, 255, 2, BADC },
   { L_, U8_10000 " ",     { '?', ' ',                     0 }, 255, 3, BADC },
   { L_, " " U8_10000 " ", { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, U8_10ffff,        { '?',                          0 }, 255, 2, BADC },
   { L_, U8_10ffff " ",    { '?', ' ',                     0 }, 255, 3, BADC },
   { L_, " " U8_10ffff " ",
                           { ' ', '?', ' ',                0 }, 255, 4, BADC },

   // Make sure partial 4-octet characters are handled correctly (with a single
   // error).
   { L_, "\xf0",           {                               0 },   1, 1, OBTS },
   { L_, "\xf0",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf0\x80",       { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf0\x80\x80",   { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf0 ",          { '?', ' ',                     0 }, 255, 3, BADC },
   { L_, "\xf0\x80 ",      { '?', ' ',                     0 }, 255, 3, BADC },
   { L_, "\xf0\x80\x80 ",  { '?', ' ',                     0 }, 255, 3, BADC },

   // Make sure the "illegal" UTF-8 octets are handled correctly:
   //      o  The octet values C0, C1, F5 to FF never appear.
   //         (however, C0 and C1 indicate the start of overlong
   //         encodings, which this implementation accepts)
   { L_, "\xf5",           {                               0 },   1, 1, OBTS },
   { L_, "\xf5",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf6",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf7",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf8",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xf9",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xfa",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xfb",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xfc",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xfd",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xfe",           { '?',                          0 }, 255, 2, BADC },
   { L_, "\xff",           { '?',                          0 }, 255, 2, BADC },

   // Make sure that the "illegal" UTF-8 octets are handled correctly
   // mid-string:
   //      o  The octet values C0, C1, F5 to FF never appear.
   //         (however, C0 and C1 indicate the start of overlong
   //         encodings, which this implementation accepts)
   { L_, " \xf5 ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xf6 ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xf7 ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xf8 ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xf9 ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xfa ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xfb ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xfc ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xfd ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xfe ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },
   { L_, " \xff ",         { ' ', '?', ' ',                0 }, 255, 4, BADC },

   // Make sure short buffers are handled correctly for long input chars.
   { L_, U8_00080,         {                               0 },   1, 1, OBTS },
   { L_, U8_00080,         { 0x80,                         0 },   2, 2,   OK },
   { L_, "\xc2",           { '?',                          0 },   2, 2, BADC },
   { L_, U8_00080,         { 0x80,                         0 }, 255, 2,   OK },
   { L_, U8_00080 " ",     { 0x80, ' ',                    0 }, 255, 3,   OK },
   { L_, U8_000ff,         { 0xff,                         0 },   2, 2,   OK },
   { L_, U8_000ff,         { 0xff,                         0 },   3, 2,   OK },
   { L_, U8_000ff,         { 0xff,                         0 }, 255, 2,   OK },
   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 U8_0ffff,
                           { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                             0x800, 0xffff,                0 }, 255, 8,   OK },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 "\xef",
                           { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                             0x800, '?',                   0 }, 255, 8, BADC },

   { L_, "\x01\x20\x7f" U8_000ff U8_007ff U8_00800 "\xef\xbf",
                           { 0x1, 0x20, 0x7f, 0xff, 0x7ff,
                             0x800, '?',                   0 }, 255, 8, BADC },

   { L_, U8_0ffff U8_00800 U8_007ff U8_000ff "\x7f\x20\x01",
                           { 0xffff, 0x800, 0x7ff, 0xff,
                             0x7f, 0x20,   0x1,            0 }, 255, 8,   OK },

   // Make sure illegal overlong encodings are accepted.  These characters are
   // mathematically correctly encoded, but since there are equivalent 1-octet
   // encodings, the UTF-8 standard disallows them.
   { L_, "\xc0\x81",       { 0x1,                          0 },   2, 2,   OK },
   { L_, "\xc0\xbf",       { 0x3f,                         0 },   2, 2,   OK },
   { L_, "\xc1\x81",       { 0x41,                         0 },   2, 2,   OK },
   { L_, "\xc1\xbf",       { 0x7f,                         0 },   2, 2,   OK },

   // Make sure that the "skip" logic for corrupted extended characters
   // operates correctly, without losing any usable data.

   // corrupted 2-octet character:
   { L_,   "\xc2",         {                               0 },   1, 1, OBTS },
   { L_,   "\xc2",         { '?',                          0 },   2, 2, BADC },
   { L_,   " \xc2",        { ' ',                          0 },   2, 2, BOTH },
   { L_,   "\xc2 ",        { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xc2 ",        { '?', ' ',                     0 },   3, 3, BADC },
   { L_,   "\xc2\xc2 ",    { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xc2\xc2 ",    { '?', '?',                     0 },   3, 3, BOTH },
   { L_,   "\xc2\xc2 ",    { '?', '?', ' ',                0 },   4, 4, BADC },
   { L_,   "\xc2 \xc2",    { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xc2 \xc2",    { '?', ' ',                     0 },   3, 3, BOTH },
   { L_,   "\xc2 \xc2",    { '?', ' ', '?',                0 },   4, 4, BADC },

   // corrupted 2-octet character followed by a valid character:
   { L_,   "\xc2" U8_00080,
                           { '?',                          0 },   2, 2, BOTH },

   { L_,   "\xc2" U8_00080,
                           { '?', 0x80,                    0 },   3, 3, BADC },

   // corrupted 2-octet character followed by an invalid character:
   { L_,   "\xc2\xff",
                           { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xc2\xff",
                           { '?', '?',                     0 },   3, 3, BADC },

   // 3-octet characters corrupted after octet 1:
   { L_,   "\xef",         {                               0 },   1, 1, OBTS },
   { L_,   "\xef",         { '?',                          0 },   2, 2, BADC },
   { L_,   " \xef",        { ' ',                          0 },   2, 2, BOTH },
   { L_,   "\xef ",        { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xef ",        { '?', ' ',                     0 },   3, 3, BADC },
   { L_,   "\xef\xef ",    { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xef\xef ",    { '?', '?',                     0 },   3, 3, BOTH },
   { L_,   "\xef\xef ",    { '?', '?', ' ',                0 },   4, 4, BADC },
   { L_,   "\xef \xef",    { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xef \xef",    { '?', ' ',                     0 },   3, 3, BOTH },
   { L_,   "\xef \xef",    { '?', ' ', '?',                0 },   4, 4, BADC },
   { L_,   "\xef" U8_00080,
                           { '?',                          0 },   2, 2, BOTH },

   { L_,   "\xef" U8_00080,
                           { '?', 0x80,                    0 },   3, 3, BADC },

   // 3-octet characters corrupted after octet 2:
   { L_,   "\xef\xbf",     {                               0 },   1, 1, OBTS },
   { L_,   "\xef\xbf",     { '?',                          0 },   2, 2, BADC },
   { L_,   " \xef\xbf",    { ' ',                          0 },   2, 2, BOTH },
   { L_,   " \xef\xbf",    { ' ', '?',                     0 },   3, 3, BADC },
   { L_,   "\xef\xbf ",    { '?',                          0 },   2, 2, BOTH },
   { L_,   "\xef\xbf ",    { '?', ' ',                     0 },   3, 3, BADC },
   { L_,   "\xef\xbf" U8_00080,
                           { '?',                          0 },   2, 2, BOTH },

   { L_,   "\xef\xbf" U8_00080,
                           { '?', 0x80,                    0 },   3, 3, BADC },

   { L_,   "\xef\xbf" U8_00080 " ",
                           { '?',                          0 },   2, 2, BOTH },

   { L_,   "\xef\xbf" U8_00080 " ",
                           { '?', 0x80,                    0 },   3, 3, BOTH },

   { L_,   "\xef\xbf" U8_00080 " ",
                           { '?', 0x80, ' ',               0 },   4, 4, BADC },

   { L_,   "\xef\xbf\xef\xbf ",
                           { '?', '?',                     0 },   3, 3, BOTH },

   { L_,   "\xef\xbf\xef\xbf ",
                           { '?', '?', ' ',                0 },   4, 4, BADC },

   { L_,   "\xef\xbf \xef\xbf",
                           { '?',                          0 },   2, 2, BOTH },

   { L_,   "\xef\xbf \xef\xbf",
                           { '?', ' ',                     0 },   3, 3, BOTH },

   { L_,   "\xef\xbf \xef\xbf",
                           { '?', ' ', '?',                0 },   4, 4, BADC },
// ^------------v
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE     = DATA[i].d_lineNum;
                const char           *SPEC     = DATA[i].d_spec_p;
                const unsigned short *OUTPUT   = DATA[i].d_output_p;
                const bsl::size_t     BUFSIZE  = DATA[i].d_bufsize;
                const int             EXPECTED = DATA[i].d_expectedCount;
                const int             RETURN   = DATA[i].d_expectedResult;

                unsigned short              results[256] = {0};
                bsl::size_t                 resultLen
                                            = sizeof results / sizeof *results;
                bsl::size_t                 charsWritten = -1;
                bsl::vector<unsigned short> cppResult;

                int retVal = bdlde::CharConvertUcs2::utf8ToUcs2(
                                      results,
                                      BUFSIZE,
                                      SPEC,
                                      &charsWritten);

                if (OK == RETURN || BADC == RETURN) {
                    int cppRetVal = bdlde::CharConvertUcs2::utf8ToUcs2(
                                        &cppResult,
                                        SPEC);

                    bsl::size_t cppCharsWritten = cppResult.size();
                    bsl::size_t resultBytes = cppCharsWritten
                                            * sizeof(short);

                    LOOP3_ASSERT(LINE, cppRetVal,         RETURN,
                                       cppRetVal       == RETURN);
                    LOOP3_ASSERT(LINE, cppCharsWritten,   EXPECTED,
                                       cppCharsWritten == EXPECTED);
                    if (cppCharsWritten > 0) {
                        LOOP4_ASSERT(LINE,
                                        makeArrayPrinter(OUTPUT,  EXPECTED),
                                        makeArrayPrinter(&(cppResult[0]),
                                                         cppCharsWritten),
                                        cppCharsWritten,
                                   0 == memcmp(OUTPUT,
                                               &(cppResult[0]),
                                               resultBytes));
                    }
                }

                LOOP3_ASSERT(LINE, charsWritten,   EXPECTED,
                                   charsWritten == EXPECTED);
                LOOP3_ASSERT(LINE, retVal,         RETURN,
                                   retVal       == RETURN);
                LOOP4_ASSERT(LINE,
                                    makeArrayPrinter(OUTPUT,  EXPECTED),
                                    makeArrayPrinter(results, charsWritten),
                                    charsWritten,
                               0 == memcmp(OUTPUT,
                                           results,
                                           charsWritten * sizeof(short)));
                if (veryVerbose) {
                    printf( "%4d: SPEC     ", LINE);
                    printStr(SPEC);
                    printf("\n");
                    printf("    : EXPECTED = %d, charsWritten = %d\n",
                                  EXPECTED,
                                  charsWritten);
                    printf("    : RETURN   = %d retVal        = %d\n",
                                  RETURN,
                                  retVal);

                    printf("    : OUTPUT   ");
                    printStr(OUTPUT);
                    printf("\n");

                    printf("    : results  ");
                    printStr(results);
                    printf("\n");
                }
            }

            // Concern - make sure that the 'invalidCharacterPlaceholder'
            // argument functions correctly.

            {

                static const struct {
                    int            d_lineNum;       // source line number

                    unsigned short d_placeholder;   // placeholder character to
                                                    // test

                    unsigned short d_expectedWidth; // space we expect it to
                                                    // use
                } DATA[] = {
                    { L_, '?',    1},
                    { L_, '*',    1},
                    { L_, '\0',   0},
                    { L_, 0x1234, 1}
                };

                for(int i=0; i<sizeof DATA/sizeof *DATA; ++i) {
                    const int            LINE   = DATA[i].d_lineNum;
                    const unsigned short PLACEHOLDER
                                                = DATA[i].d_placeholder;
                    const unsigned short WIDTH  = DATA[i].d_expectedWidth;

                    unsigned short results[256] = {0};
                    bsl::size_t   resultsLen
                                        = sizeof results / sizeof *results;
                    bsl::size_t   charsWritten = -1;

                    bsl::vector<unsigned short> cppResult;

                    int           expectedRetVal = INVALID_INPUT_CHARACTER;

                    int retVal = bdlde::CharConvertUcs2::utf8ToUcs2(
                            results,
                            resultsLen,
                            "\xef",
                            &charsWritten,
                            PLACEHOLDER);

                    int cppRetVal = bdlde::CharConvertUcs2::utf8ToUcs2(
                            &cppResult,
                            "\xef",
                            PLACEHOLDER);

                    bsl::size_t cppCharsWritten = cppResult.size();

                    LOOP3_ASSERT(LINE, retVal,   expectedRetVal,
                                       retVal == expectedRetVal);
                    LOOP3_ASSERT(LINE, cppRetVal,   expectedRetVal,
                                       cppRetVal == expectedRetVal);

                    LOOP3_ASSERT(LINE, PLACEHOLDER,   results[0],
                                       PLACEHOLDER == results[0]);
                    LOOP3_ASSERT(LINE, PLACEHOLDER,   cppResult[0],
                                       PLACEHOLDER == cppResult[0]);

                    if (WIDTH > 0) {
                        LOOP3_ASSERT(LINE, 0,   results[1],
                                           0 == results[1]);
                        LOOP3_ASSERT(LINE, 0,   cppResult[1],
                                           0 == cppResult[1]);
                    }
                    LOOP3_ASSERT(LINE, 1 + WIDTH,   charsWritten,
                                       1 + WIDTH == charsWritten);
                    LOOP3_ASSERT(LINE, 1 + WIDTH,   cppCharsWritten,
                                       1 + WIDTH == cppCharsWritten);
                }
            }

            // Concern - make sure that 0 dstCapacity is handled correctly,
            // whether or not '&charsWritten' is passed in.

            {
                int retVal =bdlde::CharConvertUcs2::utf8ToUcs2(
                        0,
                        0,
                        "");
                ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
                ASSERT(                      2 == retVal);

                bsl::size_t charsWritten = 1;
                retVal =bdlde::CharConvertUcs2::utf8ToUcs2(
                        0,
                        0,
                        "",
                        &charsWritten);
                ASSERT(OUTPUT_BUFFER_TOO_SMALL == retVal);
                ASSERT(                      2 == retVal);
                ASSERT(                      0 == charsWritten);
            }

            //   - Make sure that all permutations of 1-octet, 2-octet, and
            //     3-octet characters are converted correctly or handled
            //     correctly for short output buffers.

            {
                for(int i = 0; i < precomputedDataCount; ++i) {
                    char           inputBuffer[256]    = { 0 };
                    unsigned short outputBuffer[256]   = { 0 };
                    unsigned short characterSizes[256] = { 0 };
                    bsl::size_t    totalOutputLength   = 0;
                    bsl::size_t    characterCount      = 0;

                    char           *inputCursor  = inputBuffer;
                    unsigned short *outputCursor = outputBuffer;

                    buildUpAndTestStringsU8ToU2(i,
                                                exhaustiveSearchDepth,
                                                inputBuffer,
                                                outputBuffer,
                                                characterSizes,
                                                totalOutputLength,
                                                characterCount,
                                                inputCursor,
                                                outputCursor,
                                                verbose,
                                                veryVerbose);
                }
            }
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // ENUMERATION TEST
        //   Note: This test validates that our named (enumerated) return codes
        //   match the documented values
        //
        // Concerns:
        //   - That our enumerated return values match the documentation.
        //
        // Plan:
        //   Check each enumerated return value.
        //
        // Testing:
        //   Nothing
        // --------------------------------------------------------------------

        testStatus=0;

        // Check return value enumeration:

        // Return 0 on success

        ASSERT( 0 == SUCCESS );
        ASSERT( 0 == OK );

        // "1 on invalid input"

        ASSERT( 1 == INVALID_INPUT_CHARACTER );
        ASSERT( 1 == BADC );

        // "2 if 'dstCapacity' is insufficient to hold the complete conversion"

        ASSERT( 2 == OUTPUT_BUFFER_TOO_SMALL );
        ASSERT( 2 == OBTS );

        // "and 3 if both types of error occur."

        ASSERT( 3 == (OUTPUT_BUFFER_TOO_SMALL | INVALID_INPUT_CHARACTER) );
        ASSERT( 3 == BOTH );
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test exercises basic functionality, but tests nothing!
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Test a round-trip conversion, and make sure we get the original
        //  input back
        //
        // Testing:
        //   Nothing
        // --------------------------------------------------------------------

        const char *ecole="\xc3\x89" "cole";
        unsigned short ucs2_buffer[256];
        char  utf8_buffer[256];

        ASSERT(SUCCESS == BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(
                      ucs2_buffer,
                      sizeof ucs2_buffer/sizeof *ucs2_buffer,
                      ecole));

        ASSERT(SUCCESS == BloombergLP::bdlde::CharConvertUcs2::ucs2ToUtf8(
                      utf8_buffer,
                      sizeof utf8_buffer/sizeof *utf8_buffer,
                      ucs2_buffer));

        ASSERT(0 == strcmp(ecole, utf8_buffer));
      } break;

      case -1: {
          runPlainTextPerformanceTest();
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
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
