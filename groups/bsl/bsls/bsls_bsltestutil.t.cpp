// bsls_bsltestutil.t.cpp                                             -*-C++-*-

#include <bsls_bsltestutil.h>

#include <fcntl.h>
#include <float.h>     // FLT_MAX, etc.
#include <limits.h>    // PATH_MAX on linux, CHAR_BIT
#include <stddef.h>    // ptrdiff_t
#include <stdio.h>
#include <stdlib.h>    // abort
#include <string.h>    // strnlen
#include <sys/types.h> // struct stat: required on Sun and Windows only
#include <sys/stat.h>  // struct stat: required on Sun and Windows only

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# include <windows.h>  // MAX_PATH
# include <io.h>       // _dup2
#else
# include <unistd.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
// The component under test implements a single utility struct and 11 macros
// that call methods of the utility struct.  All of the methods write output to
// stdout and do nothing more.  Therefore, there are no Primary Manipulators or
// Basic Accessors to test.  All of the tests fall under the category of Print
// Operations.
//
// The macros provided by the component under test mirror the standard test
// macros normally used in test drivers.  The intention is that the standard
// test macros should be implemented as aliases of the 'BSLS_BSLTESTUTIL_*'
// macros, as illustrated in the usage example.  As a result, the identifiers
// normally used in a test driver conflict with the identifiers used in the
// usage example.  Therefore, this test driver avoids the standard test macros
// (and support functions), and uses the following instead:
//
// STANDARD              BSLS_BSLTESTUTIL.T.CPP
// --------              ----------------------
// 'LOOP_ASSERT'         'ANNOTATED_ASSERT'
// 'LOOP2_ASSERT'        'ANNOTATED2_ASSERT'
// 'LOOP3_ASSERT'        'ANNOTATED3_ASSERT'
// 'LOOP4_ASSERT'        'ANNOTATED4_ASSERT'
// 'LOOP5_ASSERT'        not used
// 'LOOP6_ASSERT'        not used
// 'Q'                   not used
// 'P'                   'PRINT'
// 'P_'                  'PRINT_'
// 'T_'                  'TAB_'
// 'L_'                  not used
// 'void aSsErT()'       'void realaSsErT()'
// 'int testStatus'      'int realTestStatus'
//
// Note that the 'ANNOTATED*_ASSERT' macros are only rough equivalents of the
// standard 'LOOP*_ASSERT' macros.  Because 'std::cout' is not available in
// this test driver, there is no way to automatically format values based on
// type, at least not without re-implementing the component under test in the
// test driver.  Therefore the 'ANNOTATED*_ASSERT' macros take as arguments
// pairs of identifiers and 'printf'-style format strings.  This is the reason
// why they have been given completely different names from the standard
// macros.
//
// The main difficulty with writing the test driver is capturing the output of
// the methods under test so that it can be checked for accuracy.  In addition,
// error messages and other output produced by the test driver itself must
// still appear on 'stdout' for compatibility with the standard build and
// testing scripts.  For this purpose, a support class named 'OutputRedirector'
// is provided.  'OutputRedirector' will redirect 'stdout' to a temporary file
// and verify that the contents of the temporary file match the character
// buffers provided by the user.
//
// Global Concerns:
//: o The test driver can reliably capture 'stdout' and play back captured
//:   output.
//: o The test driver's own output is written to the calling environment's
//:   'stdout'.
//
// ----------------------------------------------------------------------------
// STANDARD LOOP ASSERT MACROS
// [ 8] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
//
// STANDARD OUTPUT MACROS
// [ 7] BSLS_BSLTESTUTIL_Q(X)
// [ 6] BSLS_BSLTESTUTIL_P(X)
// [ 6] BSLS_BSLTESTUTIL_P_(X)
// [ 5] BSLS_BSLTESTUTIL_L_
// [ 5] BSLS_BSLTESTUTIL_T_
//
// PRINTF FORMAT SPECIFIER MACROS
// [ 9] BSLS_BSLTESTUTIL_FORMAT_ZU
// [ 9] BSLS_BSLTESTUTIL_FORMAT_TD
// [ 9] BSLS_BSLTESTUTIL_FORMAT_I64
// [ 9] BSLS_BSLTESTUTIL_FORMAT_U64
//
// UNDERLYING IMPLEMENTATION AND EXTENSIBILITY INTERFACE
// [ 3] void debugprint(bool v)
// [ 3] void debugprint(char v)
// [ 3] void debugprint(signed char v)
// [ 3] void debugprint(unsigned char v)
// [ 3] void debugprint(short v)
// [ 3] void debugprint(unsigned short v)
// [ 3] void debugprint(int v)
// [ 3] void debugprint(unsigned int v)
// [ 3] void debugprint(long v)
// [ 3] void debugprint(unsigned long v)
// [ 3] void debugprint(long long v)
// [ 3] void debugprint(unsigned long long v)
// [ 3] void debugprint(float v)
// [ 3] void debugprint(double v)
// [ 3] void debugprint(long double v)
// [ 3] void debugprint(const char *v)
// [ 3] void debugprint(char *v)
// [ 3] void debugprint(const void *v)
// [ 4] static void printStringNoFlush(const char *s);
// [ 4] static void printTab();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS: class 'OutputRedirector'
// [ 9] USAGE EXAMPLE
// [-1] CONCERN: 'printf' stack corruption test works
// ----------------------------------------------------------------------------

// ============================================================================
//                VARIATIONS ON STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// In order to accommodate the use of the identifiers 'testStatus' and 'aSsErT'
// in the usage example, the rest of the test driver uses the identifiers
// 'realTestStatus' and 'realaSsErT' instead.
//
// Additionally, in order to allow capturing the output of the
// 'BSLS_BSLTESTUTIL_*' macros, the standard macros output to 'stderr' instead
// of 'stdout'.

static int realTestStatus = 0;

static void realaSsErT(bool b, const char *s, int i)
{
    if (b) {
        fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (realTestStatus >= 0 && realTestStatus <= 100) ++realTestStatus;
    }
}

// The standard 'ASSERT' macro definition is deferred until after the usage
// example code
// #define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

// ============================================================================
//               VARIATIONS ON STANDARD BDE LOOP_ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// The standard BDE 'LOOP_ASSERT' test macros rely on overloads of 'operator<<'
// ostream to provide a uniform interface for all types.  The purpose of
// 'bsls_bsltestutil' is precisely to provide a similar overloading mechanism
// for 'printf'-based output.  In order to use the standard macros in this test
// driver, we would have to define a test apparatus of equivalent complexity to
// the class we are testing.  Therefore, instead of the standard test macros we
// define alternate macros that take pairs of variables and 'printf'-style
// format strings instead of just variables alone.  For convenience, all of the
// macros are defined in terms of a free function named 'printDatum' that
// prints a single variable's identifier and value separated by a user-supplied
// string.

#define ANNOTATED_ASSERT(I,IF,X) do {                                         \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\n",(I));                \
                    realaSsErT(1, #X, __LINE__);}                             \
    } while (0)

#define ANNOTATED2_ASSERT(I,IF,J,JF,X) do {                                   \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));                \
                    printDatum(stderr, #J,": ",(JF),"\n",(J));                \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define ANNOTATED3_ASSERT(I,IF,J,JF,K,KF,X) do {                              \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));                \
                    printDatum(stderr, #J,": ",(JF),"\t",(J));                \
                    printDatum(stderr, #K,": ",(KF),"\n",(K));                \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define ANNOTATED4_ASSERT(I,IF,J,JF,K,KF,L,LF,X) do {                         \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));                \
                    printDatum(stderr, #J,": ",(JF),"\t",(J));                \
                    printDatum(stderr, #K,": ",(KF),"\t",(K));                \
                    printDatum(stderr, #L,": ",(LF),"\n",(L));                \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define PRINT(X,XF) do {                                                      \
        printDatum(stderr,#X," = ",(XF),"\n",(X));                            \
    } while(0)

#define PRINT_(X,XF) do {                                                     \
        printDatum(stderr,#X," = ",(XF),", ",(X));                            \
    } while(0)

#define TAB_ do {                                                             \
        fprintf(stderr, "\t");                                                \
    } while(0)

#define TZU  BSLS_BSLTESTUTIL_FORMAT_ZU
#define TTD  BSLS_BSLTESTUTIL_FORMAT_TD
#define TI64 BSLS_BSLTESTUTIL_FORMAT_I64
#define TU64 BSLS_BSLTESTUTIL_FORMAT_U64

// ============================================================================
//                             USAGE EXAMPLE CODE
// ----------------------------------------------------------------------------
//
// Usage example code assumes that 'BSLS_BSLTESTUTIL_*' macros have been
// renamed to replace the standard test macros.  In order to simplify the rest
// of the test driver, the standard macros are redefined after 'main', and the
// usage example case just calls a function, 'executeUsageExample'.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
    namespace bslabc {

    struct BslExampleUtil {
        // This utility class provides sample functionality to demonstrate how
        // a test driver might be written validating its only method.

        static int fortyTwo();
            // Return the integer value '42'.
    };

    inline
    int BslExampleUtil::fortyTwo()
    {
        return 42;
    }

    }  // close namespace bslabc
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro:
//..
    // ========================================================================
    //                       STANDARD BDE ASSERT TEST MACRO
    // ------------------------------------------------------------------------
    static int testStatus = 0;

    static void aSsErT(bool b, const char *s, int i)
    {
        if (b) {
            printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
            if (testStatus >= 0 && testStatus <= 100) ++testStatus;
        }
    }

    # define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//..
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
    // ========================================================================
    //                       STANDARD BDE TEST DRIVER MACROS
    // ------------------------------------------------------------------------
    #define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
    #define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
    #define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
    #define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
    #define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
    #define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

    #define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
    #define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
    #define P_  BSLS_BSLTESTUTIL_P_  // 'P(X)' without '\n'.
    #define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
    #define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in a test driver.
//..
    void testFortyTwo(bool verbose)
    {
        const int value = bslabc::BslExampleUtil::fortyTwo();
        if (verbose) P(value);
        LOOP_ASSERT(value, 42 == value);
    }
//..
// Finally, when 'testMyTypeSetValue' is called from a test case in verbose
// mode we observe the console output:
//..
//  value = 42
//..
///Example 2: Adding Support For A New User-Defined Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a new user-defined type, 'MyType':
//..
    namespace xyza {

    class MyType {
        // This elided class provides a type intended to show how the macros in
        // 'bsls_bsltestutil' can be extended to support a new user-defined
        // type.

      private:
        // DATA
        int d_value;  // the value of MyType

        // ...

      public:
        // CREATORS

        // ...

        explicit MyType(int value);
            // Create a 'MyType' object with 'd_value' set to the specified
            // 'value'.

        // ACCESSORS

        // ...

        int value() const;
            // Return the value of 'd_value'.

        // ...
    };

    // ...

    MyType::MyType(int value)
    : d_value(value)
    {
    }

    // ...

    int MyType::value() const
    {
        return d_value;
    }
//..
// Then, in the same namespace in which 'MyType' is defined, we define a
// function 'debugprint' that prints the value of a 'MyType' object to the
// console.  (In this case, we will simply print a string literal for
// simplicity):
//..
    void debugprint(const MyType& obj)
    {
        printf("MyType<%d>", obj.value());
    }

    }  // close namespace xyza
//..
// Notice that 'debugprint' is defined inside the namespace 'MyNamespace'.
// This is required in order to allow the compiler to find this overload of
// debugprint by argument-dependent lookup.
//
// Now, using the (standard) abbreviated macro names previously defined, we
// write a test function for the 'MyType' constructor, to be called from a test
// case in a test driver.
//..
    void testMyTypeSetValue(bool verbose)
    {
        xyza::MyType obj(9);
        if (verbose) P(obj);
        LOOP_ASSERT(obj.value(), obj.value() == 9);
    }
//..
// Finally, when 'testMyTypeSetValue' is called from a test case in verbose
// mode we observe the console output:
//..
//  obj = MyType<9>
//..
///Example 3: Printing Unusual Types with 'printf'
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing a test driver that needs to print out the contents of
// a complex data structure in 'veryVeryVerbose' mode.  The complex data
// structure contains, among other values, an array of block sizes, expressed
// as 'size_t'.  It would be very cumbersome, and visually confusing, to print
// each member of the array with either the 'P_' or 'Q_' standard output
// macros, so we elect to print out the array as a single string, following the
// pattern of '[ A, B, C, D, E, ... ]'.  This could be easily accomplished with
// multiple calls to 'printf', except that 'printf' has no cross-platform
// standard formatting string for 'size_t'.  We can use the
// 'BSLS_BSLTESTUTIL_FORMAT_ZU' macro to resolve the appropriate format string
// for us on each platform.
//
// First, we write a component to test, which provides an a utility that
// operates on a list of memory blocks.  Each block is a structure containing a
// base address, a block size, and a pointer to the next block in the list.
//..
    namespace xyza {
    struct Block {
        // DATA
        char   *d_address;
        size_t  d_size;
        Block  *d_next;

        // ...
    };

    class BlockList {
        // ...

        // DATA
        Block *d_head;

        // ...

      public:
        // CREATORS
        BlockList();
            // Create an empty 'BlockList'.

        ~BlockList();
            // Destroy this object.

        // MANIPULATORS

        Block *begin();
        Block *end();

        void addBlock(size_t size);
            // Add a block of the specified 'size' to is 'BlockList'.
        // ...

        // ACCESSORS
        int length();
            // Return the number of blocks in this 'BlockList'.

        // ...
    };

    }  // close namespace xyza
//..
// Then, we write a test driver for this component.
//..
    // ...

    // ========================================================================
    //                       STANDARD BDE TEST DRIVER MACROS
    // ------------------------------------------------------------------------

    // ...
//..
// Here, after defining the standard BDE test macros, we define a macro, 'ZU'
// for the platform-specific 'printf' format string for 'size_t':
//..
    // ========================================================================
    //                          PRINTF FORMAT MACROS
    // ------------------------------------------------------------------------
    #define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
//..
// Note that, we could use 'BSLS_BSLTESTUTIL_FORMAT_ZU' as is, but it is more
// convenient to define 'ZU' locally as an abbreviation.
//
// Next, we write the test apparatus for the test driver, which includes a
// support function that prints the list of blocks in a 'BlockList' in a
// visually succinct form:
//..
    void printBlockList(xyza::BlockList &list)
    {
        xyza::Block *blockPtr = list.begin();

        printf("{\n");
        while (blockPtr != list.end()) {
//..
// Here, we use 'ZU' as the format specifier for the 'size_t' in the 'printf'
// invocation. 'ZU' is the appropriate format specifier for 'size_t' on each
// supported platform.
//..
            printf("\t{ address: %p,\tsize: " ZU " }",
                   blockPtr->d_address,
                   blockPtr->d_size);
            blockPtr = blockPtr->d_next;

            if (blockPtr) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        printf("}\n");
    }
//..
// Note that because we are looping through a number of blocks, formatting the
// output directly with 'printf' produces more readable output than we would
// get from callling the standard output macros.
//
// Calling 'printf' directly will yield output similar to:
//..
// {
//     { address: 0x012345600,    size: 32 },
//     ...
// }
//..
// while the standard output macros would have produced:
//..
// {
//     { blockPtr->d_address = 0x012345600,    blockPtr->d_size: 32 },
//     ...
// }
//..
// Now, we write a test function for one of our test cases, which provides a
// detailed trace of 'BlockList' contents:
//..
    void testBlockListConstruction(bool veryVeryVerbose)
    {
        // ...

        {
            xyza::BlockList bl;

            bl.addBlock(42);
            bl.addBlock(19);
            bl.addBlock(1024);

            if (veryVeryVerbose) {
                printBlockList(bl);
            }

            ASSERT(3 == bl.length());

            // ...
        }

        // ...
    }
//..
// Finally, when 'testBlockListConstruction' is called from a test case in
// 'veryVeryVerbose' mode, we observe console output similar to:
//..
//  {
//      { address: 0x012345600,    size: 42 },
//      { address: 0x012345610,    size: 19 },
//      { address: 0x012345620,    size: 1024 }
//  }
//..

namespace xyza {
    const int MAX_BLOCKS = 10;

    Block blockListStorage[MAX_BLOCKS];
}  // close namespace xyza

xyza::BlockList::BlockList()
  : d_head(0)
{
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        blockListStorage[i].d_next = 0;
    }
}

xyza::BlockList::~BlockList()
{
}

xyza::Block *xyza::BlockList::begin()
{
    return d_head;
}

xyza::Block *xyza::BlockList::end()
{
    return 0;
}

void xyza::BlockList::addBlock(size_t size)
{
    char  *dummyAddress = (char *) 0x012345600;
    Block *blockPtr     = 0;
    int len = length();

    if (len == 0) {
        blockPtr = blockListStorage;
        d_head = blockPtr;
    } else if (len < MAX_BLOCKS) {
        blockPtr = blockListStorage + len;
        blockListStorage[len - 1].d_next = blockPtr;
        dummyAddress += 0x10 * len;
    }

    if (blockPtr) {
        blockPtr->d_address = dummyAddress;
        blockPtr->d_size    = size;
        blockPtr->d_next    = 0;
    }
}

int xyza::BlockList::length()
{
    Block  *blockPtr     = d_head;
    int length = 0;

    while (blockPtr) {
        ++length;
        blockPtr = blockPtr->d_next;
    }

    return length;
}

// ============================================================================
//                    CLEANUP STANDARD TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#undef ASSERT

#undef LOOP_ASSERT
#undef LOOP2_ASSERT
#undef LOOP3_ASSERT
#undef LOOP4_ASSERT
#undef LOOP5_ASSERT
#undef LOOP6_ASSERT

#undef Q
#undef P
#undef P_
#undef T_
#undef L_

#undef ZU

#define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { FORMAT_STRING_SIZE = 256 }; // Size of temporary format string buffers
                                   // used for output formatting

enum {
    // Enumeration used to store sizes for the buffers used by the output
    // redirection apparatus.

    OUTPUT_BUFFER_SIZE = 4096,

#ifdef BSLS_PLATFORM_OS_WINDOWS
    PATH_BUFFER_SIZE   = MAX_PATH
#elif defined(BSLS_PLATFORM_OS_HPUX)
    PATH_BUFFER_SIZE   = L_tmpnam
#else
    PATH_BUFFER_SIZE   = PATH_MAX
#endif
};

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose;

// ============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS
# define snprintf _snprintf
#endif

template <class ITYPE>
int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               const ITYPE& valueI);
    // Print the specified 'identifierI' identifier name and specified value
    // 'valueI' of (parameter template) type 'ITYPE' to the specified
    // 'outStream', and separated by the specified 'connector' string and
    // followed by the specified 'suffix' string, using the specified 'formatI'
    // format string to format 'valueI' according to the rules of 'printf'.  As
    // a special case, null 'char' strings are output as '(null)'.

bool tempFileName(char *result);
    // Create a temporary file and store its name in the user-supplied buffer
    // at the address pointed to by the specified 'result'.  Return 'true' if
    // the temporary file was successfully created, and 'false' otherwise.  The
    // behavior is undefined unless the buffer pointed to by the specified
    // 'result' is at least 'PATH_BUFFER_SIZE' bytes long.

int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               char        *valueI)
{
    char tempFormatString[FORMAT_STRING_SIZE];

    int charsWritten = snprintf(tempFormatString,
                                FORMAT_STRING_SIZE,
                                "%%s%%s%s%%s",
                                formatI);
    ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);

    if (valueI) {
        return fprintf(outStream,
                       tempFormatString,
                       identifierI,
                       connector,
                       valueI,
                       suffix);                                       // RETURN
    } else {
        return fprintf(outStream,
                       tempFormatString,
                       identifierI,
                       connector,
                       "(null)",
                       suffix);                                       // RETURN
    }
}

int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               const char  *valueI)
{
    char tempFormatString[FORMAT_STRING_SIZE];

    int charsWritten = snprintf(tempFormatString,
                                FORMAT_STRING_SIZE,
                                "%%s%%s%s%%s",
                                formatI);
    ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);

    if (valueI) {
        return fprintf(outStream,
                       tempFormatString,
                       identifierI,
                       connector,
                       valueI,
                       suffix);                                       // RETURN
    } else {
        return fprintf(outStream,
                       tempFormatString,
                       identifierI,
                       connector,
                       "(null)",
                       suffix);                                       // RETURN
    }
}

template <class ITYPE>
int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               const ITYPE& valueI)
{
    char tempFormatString[FORMAT_STRING_SIZE];

    int charsWritten = snprintf(tempFormatString,
                                FORMAT_STRING_SIZE,
                                "%%s%%s%s%%s",
                                formatI);
    ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
    return fprintf(outStream,
                   tempFormatString,
                   identifierI,
                   connector,
                   valueI,
                   suffix);
}

bool tempFileName(char *result)
{
    ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[MAX_PATH];
    if (! GetTempPath(MAX_PATH, tmpPathBuf) ||
        ! GetTempFileName(tmpPathBuf, "bsls", 0, result)) {
        return false;                                                 // RETURN
    }
#elif defined(BSLS_PLATFORM_OS_HPUX)
    if(! tempnam(result, "bsls")) {
        return false;
    }
#else
    char *fn = tempnam(0, "bsls");
    if (fn) {
        strncpy(result, fn, PATH_BUFFER_SIZE);
        result[PATH_BUFFER_SIZE - 1] = '\0';
        free(fn);
    } else {
        return false;                                                 // RETURN
    }
#endif

    if (veryVerbose) printf("\tUse '%s' as a base filename.\n", result);

    ASSERT('\0' != result[0]); // result not empty

    return true;
}

void dummyVoidFunction()
{
    return;
}

int dummyIntFunction()
{
    return 0;
}

double dummyDoubleFunction()
{
    return 0.0;
}

// ============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

class OutputRedirector {
    // This class provides a facility for redirecting 'stdout' to a temporary
    // file, retrieving output from the temporary file and comparing the output
    // to user-supplied character buffers.  An 'OutputRedirector' object can
    // exist in one of two states, un-redirected or redirected.  In the
    // un-redirected state, the process' 'stdout' and 'stderr' are connected to
    // their normal targets.  In the redirected state, the process' 'stdout' is
    // connected to a temporary file, and the process' 'stderr' is connected to
    // the original target of 'stdout'.  The redirected state of an
    // 'OutputRedirector' object can be tested by calling 'isRedirected'.  An
    // 'OutputRedirector' object has the concept of a scratch buffer, where
    // output captured from the process' 'stdout' stream is stored when the
    // 'OutputRedirector' object is in the redirected state.  Throughout this
    // class, the term "captured output" refers to data that has been written
    // to the 'stdout' stream and is waiting to be loaded into the scratch
    // buffer.  Each time the 'load' method is called, the scratch buffer is
    // truncated, and the captured output is moved into the scratch buffer.
    // When this is done, there is no longer any captured output.

  private:
    // DATA
    char d_fileName[PATH_BUFFER_SIZE];        // Name of temporary capture file

    char d_outputBuffer[OUTPUT_BUFFER_SIZE];  // Scratch buffer for holding
                                              // captured output

    bool d_isRedirectedFlag;                  // Has 'stdout' been redirected

    bool d_isFileCreatedFlag;                 // Has a temp file been created

    bool d_isOutputReadyFlag;                 // Has output been read from
                                              // temp file

    long d_outputSize;                        // Size of output loaded into
                                              // 'd_outputBuffer'

    struct stat d_originalStdoutStat;         // Status information for
                                              // 'stdout' just before
                                              // redirection.

    static int redirectStream(FILE *from, FILE *to);
        // Redirect the specified stream 'from' to the specified stream 'to',
        // returning 0 for success and a negative value on failure.

    void cleanup();
        // Close 'stdout', if redirected, and delete the temporary output
        // capture file.

  private:
    // NOT IMPLEMENTED
    OutputRedirector(const OutputRedirector&);
    OutputRedirector& operator=(const OutputRedirector&);

  public:
    // CREATORS
    explicit OutputRedirector();
        // Create an 'OutputRedirector' in an un-redirected state, and
        // an empty scratch buffer.

    ~OutputRedirector();
        // Destroy this 'OutputRedirector' object.  If the object is in a
        // redirected state, 'stdout' will be closed and the temporary file to
        // which 'stdout' was redirected will be deleted.

    // MANIPULATORS
    void redirect();
        // Redirect 'stdout' to a temp file, and stderr to the original
        // 'stdout', putting this 'OutputRedirector' object into the
        // 'redirected' state.  The temp file to which 'stdout' is redirected
        // will be created the first time 'redirect' is called, and will be
        // deleted when this object is destroyed.  Subsequent calls to
        // 'redirect' will have no effect on 'stdout' and 'stderr'.  If
        // 'redirect' fails to redirect either 'stdout' or 'stderr' it will end
        // the program by calling 'std::abort'.

    void reset();
        // Reset the scratch buffer to empty.  The behavior is undefined unless
        // 'redirect' has been previously been called successfully.

    bool load();
        // Read captured output into the scratch buffer.  Return 'true' if all
        // captured output was successfully loaded, and 'false' otherwise.
        // Note that captured output is allowed to have zero length.  The
        // behavior is undefined unless 'redirect' has been previously been
        // called successfully.

    // ACCESSORS
    bool isRedirected();
        // Return 'true' if 'stdout' and 'stderr' have been successfully
        // redirected, and 'false' otherwise.

    bool isOutputReady();
        // Return 'true' if captured output been loaded into the scratch
        // buffer, and 'false' otherwise.

    char *getOutput();
        // Return the address of the scratch buffer.  This method is only used
        // for error reporting and to test the correctness of
        // 'OutputRedirector'.

    size_t outputSize();
        // Return the number of bytes currently loaded into the scratch buffer.

    int compare(const char *expected, size_t expectedLength);
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The length of the 'expected' buffer is supplied in the
        // specified 'expectedLength'.  Return 0 if the 'expected' buffer has
        // the same length and contents as the scratch buffer, and non-zero
        // otherwise.  Note that the 'expected' buffer is allowed to contain
        // embedded nulls.  The behavior is undefined unless
        // 'redirect' has been previously been called successfully.

    int compare(const char *expected);
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The 'expected' buffer is assumed to be a NTBS, and and its
        // length is taken to be the string length of the NTBS.  Return 0 if
        // the 'expected' buffer has the same length and contents as the
        // scratch buffer, and non-zero otherwise.  The behavior is undefined
        // unless 'redirect' has been previously been called successfully.

    const struct stat& originalStdoutStat();
        // Return a reference to the status information for 'stdout' collected
        // just before redirection.  This method is used only to test the
        // correctness of 'OutputRedirector'.
};

OutputRedirector::OutputRedirector()
: d_isRedirectedFlag(false)
, d_isFileCreatedFlag(false)
, d_isOutputReadyFlag(false)
, d_outputSize(0L)
{
    d_fileName[0] = '\0';
    memset(&d_originalStdoutStat, 0, sizeof(struct stat));
}

OutputRedirector::~OutputRedirector()
{
    cleanup();
}

int OutputRedirector::redirectStream(FILE *from, FILE *to)
{
    ASSERT(from);
    ASSERT(to);

    // The canonical way to redirect 'stderr' to 'stdout' is
    // 'ASSERT(freopen("/dev/stdout", "w", stderr));', but we use dup2
    // instead of 'freopen', because 'freopen' fails on AIX with errno
    // 13 'Permission denied' when redirecting stderr.

#if defined(BSLS_PLATFORM_OS_AIX)
    int redirected = dup2(fileno(from), fileno(to));
    return redirected == fileno(to) ? 0 : -1;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return _dup2(_fileno(from), _fileno(to));
#else
    return (stderr == freopen("/dev/stdout", "w", stderr)) ? 0 : -1;
#endif
}

void OutputRedirector::cleanup()
{
    if (d_isRedirectedFlag) {
        fclose(stdout);
    }

    if (d_isFileCreatedFlag) {
        unlink(d_fileName);
    }
}

void OutputRedirector::redirect()
{
    if (d_isRedirectedFlag) {

        // Do not redirect anything if we have already redirected.

        if (veryVerbose) {
            fprintf(stdout,
                    "Warning " __FILE__ "(%d): Output already redirected\n",
                    __LINE__);
        }

        return;                                                       // RETURN
    }

    // Retain information about original 'stdout' file descriptor for use in
    // later tests.

    int originalStdoutFD = fileno(stdout);
    ASSERT(-1 != originalStdoutFD);
    ASSERT(0 == fstat(originalStdoutFD, &d_originalStdoutStat));


    if (0 != redirectStream(stderr, stdout)) {
        // Redirect 'stderr' to 'stdout;.

        // We want 'stderr' to point to 'stdout', so we have to redirect it
        // before we change the meaning of 'stdout'.

        if (veryVerbose) {

            // Note that we print this error message on 'stdout' instead of
            // 'stderr', because 'stdout' has not been redirected.

            fprintf(stdout,
                    "Error " __FILE__ "(%d): Failed to redirect stderr\n",
                    __LINE__);
        }
        abort();
    }

    if (! tempFileName(d_fileName)) {

        // Get temp file name

        if (veryVerbose) {

            // Note that we print this error message on 'stdout' instead of
            // 'stderr', because 'stdout' has not been redirected.

            fprintf(stdout,
                    "Error "
                    __FILE__
                    "(%d): Failed to get temp file name for stdout capture\n",
                    __LINE__);
        }
        abort();
    }

    if (! freopen(d_fileName, "w+", stdout)) {

        // Redirect 'stdout'

        if (veryVerbose) {

            // Note that we print this error message on 'stderr', because we
            // have just redirected 'stdout' to the capture file.

            PRINT(d_fileName, "%s");
            fprintf(stderr,
                    "Error " __FILE__ "(%d): Failed to redirect stdout\n",
                    __LINE__);
        }
        cleanup();
        abort();
    }

    // 'stderr' and 'stdout' have been successfully redirected.

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    if (-1 == _setmode(_fileno(stdout), _O_BINARY)) {
        ASSERT(0 == "Failed to set stdout to binary mode.");
        cleanup();
        abort();
    }
#endif

    d_isFileCreatedFlag = true;
    d_isRedirectedFlag = true;

    if (EOF == fflush(stdout)) {

        // Not flushing 'stdout' is not a fatal condition, so we print out a
        // warning, but do not abort.

        if (veryVerbose) {

            // Note that we print this error message on 'stderr', because we
            // have just redirected 'stdout' to the capture file.

            perror("Error message: ");
            fprintf(stderr,
                    "Warning " __FILE__ "(%d): Error flushing stdout\n",
                    __LINE__);
        }
    }
}

void OutputRedirector::reset()
{
    ASSERT(d_isRedirectedFlag);

    d_outputSize = 0L;
    d_isOutputReadyFlag = false;
    d_outputBuffer[0] = '\0';
    rewind(stdout);
}

bool OutputRedirector::load()
{
    ASSERT(d_isRedirectedFlag);
    ASSERT(!ferror(stdout));

    d_outputSize = ftell(stdout);

    if (d_outputSize + 1 > OUTPUT_BUFFER_SIZE) {

        // Refuse to load output if it will not all fit in the scratch buffer.

        if (veryVerbose) {
            PRINT(d_outputSize, "%ld");
            fprintf(stderr,
                    "Error "
                        __FILE__
                        "(%d): Captured output exceeds read buffer size\n",
                    __LINE__);
        }
        d_outputSize = 0L;
        return false;                                                 // RETURN
    }

    rewind(stdout);

    long charsRead = fread(d_outputBuffer, sizeof(char), d_outputSize, stdout);

    if (d_outputSize != charsRead) {

        // We failed to read all output from the capture file.

        if (veryVerbose) {
            PRINT_(d_outputSize, "%ld"); PRINT(charsRead, "%ld");
            if (ferror(stdout)) {

                // We encountered a file error, not 'EOF'.

                perror("\tError message: ");
                clearerr(stdout);
            }
            fprintf(stderr,
                    "Error "
                        __FILE__
                        "(%d): Could not read all captured output\n",
                    __LINE__);
        }

        if(charsRead < 0) {
            d_outputBuffer[0] = '\0';
        } else if(charsRead >= d_outputSize) {
            // This case should never happen.  This assignment is safe because
            // the total buffer size is enough to hold 'd_outputSize' + 1.
            d_outputBuffer[d_outputSize] = '\0';
        } else {
            d_outputBuffer[charsRead] = '\0';
        }
            // ...to ensure that direct inspection of buffer does not overflow

        return false;                                                 // RETURN
    } else {

        // We have read all output from the capture file.

        d_outputBuffer[d_outputSize] = '\0';

            // ...to ensure that direct inspection of buffer does not overflow
    }

    d_isOutputReadyFlag = true;

    return true;
}

bool OutputRedirector::isRedirected()
{
    return d_isRedirectedFlag;
}

bool OutputRedirector::isOutputReady()
{
    return d_isOutputReadyFlag;
}

char *OutputRedirector::getOutput()
{
    return d_outputBuffer;
}

size_t OutputRedirector::outputSize()
{
    return static_cast<size_t>(d_outputSize);
}

int OutputRedirector::compare(const char *expected)
{
    ASSERT(expected);

    return compare(expected, strlen(expected));
}

int OutputRedirector::compare(const char *expected, size_t expectedLength)
{
    ASSERT(d_isRedirectedFlag);
    ASSERT(expected || ! expectedLength);

    if (!d_isOutputReadyFlag) {
        if (veryVerbose) {
            PRINT(expected, "'%s'");
            fprintf(stderr,
                    "Error " __FILE__ "(%d): No captured output available\n",
                    __LINE__);
        }
        return -1;                                                    // RETURN
    }

    // Use 'memcmp' instead of 'strncmp' to compare 'd_outputBuffer' to
    // 'expected', because 'expected' is allowed to contain embedded nulls.

    return d_outputSize != static_cast<long>(expectedLength) ||
           memcmp(d_outputBuffer, expected, expectedLength);
}

const struct stat& OutputRedirector::originalStdoutStat()
{
    return d_originalStdoutStat;
}

namespace xyzb {

class TestType {
    // Provide a user-defined type to use in testing 'debugprint'.

  private:
    // DATA
    int d_value;  // the value of this 'TestType' object

  public:
    // CREATORS
    explicit TestType(int value);
        // Create a 'TestType' object with 'd_value' set to the specified
        // 'value'.

    // ACCESSORS
    int value() const;
};

TestType::TestType(int value)
: d_value(value)
{
}

int TestType::value() const
{
    return d_value;
}

void debugprint(const TestType& obj)
    // Print the string representation of the specified 'TestType' object to
    // stdout.  The representation of a 'TestType' object is the string
    // "TestType<n>", where "n" is the value of the 'TestType' object.
{
    printf("TestType<%d>", obj.value());
}


class BrokenTestType {
    // Provide a user-defined type to use in testing 'debugprint'.  This class
    // does not allow copying, assignment, or the address-of operator, so that
    // it can be used to demonstrate that none of these are needed for the
    // basic operation of 'bsls_bsltestutil' on user-defined types.

  private:
    // NOT IMPLEMENTED
    BrokenTestType(const BrokenTestType&);
    void operator=(const BrokenTestType&);
    void operator&() const;
    void operator,(const BrokenTestType&) const;

    // DATA
    int d_value;  // the value of this 'BrokenTestType' object

  public:
    // CREATORS
    explicit BrokenTestType(int value);
        // Create a 'BrokenTestType' object with 'd_value' set to the specified
        // 'value'.

    // ACCESSORS
    int value() const;
};

BrokenTestType::BrokenTestType(int value)
: d_value(value)
{
}

int BrokenTestType::value() const
{
    return d_value;
}

void debugprint(const BrokenTestType& obj)
    // Print the string representation of the specified 'BrokenTestType' object
    // to stdout.  The representation of a 'BrokenTestType' object is the
    // string "BrokenTestType<n>", where "n" is the value of the
    // 'BrokenTestType' object.
{
    printf("BrokenTestType<%d>", obj.value());
}

}  // close namespace xyzb

template <class TEST_TYPE>
struct DataRow {
    int         d_line;              // line number

    TEST_TYPE   d_input;             // value that was printed to 'stdout'

    const char *d_expectedOutput_p;  // expected output string, or '0' if
                                     // expected output should be generated
                                     // with 'printf'

    const char *d_description_p;     // description of this test case
};

// ============================================================================
//                       TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

struct TestDriver {
    enum {
        BUFFER_SIZE     = 1024, // size of the buffer used to store captured
                                // output

        LOOP_ITERATIONS = 10    // number of iterations to use when testing
                                // loop assert macros
    };

    static char s_expectedOutput[BUFFER_SIZE];  // scratch area for assembling
                                                // model output that will be
                                                // compared to real output
                                                // captured from 'stdout'

    // TEST CASES

    static void testCase8(OutputRedirector *output);
        // Test 'BSLS_BSLTESTUTIL_LOOP*_ASSERT' macros.

    template <class TEST_TYPE, size_t NUM_DATA>
    static void testCase3(OutputRedirector           *output,
                          const DataRow<TEST_TYPE>(&  DATA)[NUM_DATA],
                          const char                 *formatString);
        // Test 'debugprint'.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

char TestDriver::s_expectedOutput[TestDriver::BUFFER_SIZE];

                                 // ----------
                                 // TEST CASES
                                 // ----------

void TestDriver::testCase8(OutputRedirector *output)
{
    // ------------------------------------------------------------------------
    // TESTING BSLS_BSLTESTUTIL_LOOP*_ASSERT MACROS
    //
    // Concerns:
    //: 1 Macros do not call 'aSsErT' and emit no output when the assertion is
    //:   'true'.
    //:
    //: 2 Macros call 'aSsErT' and emit output each time the assertion is
    //:   'false'.
    //:
    //: 3 Macros emit properly formatted output for each loop variable
    //:   supplied.
    //
    // Plan:
    //: 1 Loop through an arbitrary number of iterations, calling one of the
    //:   loop assert macros with distinct values for each loop variable and an
    //:   assertion that evaluates to 'true'.  Confirm that the value of
    //:   'testStatus' does not change, and that no output is captured by the
    //:   output redirection apparatus.  (C-1)
    //: 2 Loop through an arbitrary number of iterations, calling one of the
    //:   loop assert macros with distinct values for each loop variable and an
    //:   assertion that evaluates to 'false'.  Confirm that 'testStatus'
    //:   increments each time the loop assert macro is called, and that the
    //:   expected error output is captured by the output redirection
    //:   apparatus.  Note that using distinct values for each loop variable
    //:   allows us to detect omissions, repetitions or mis-ordering of the
    //:   loop assert macro's arguments.  Also note that we test the loop
    //:   assert macro with only one set of variable types, since we test
    //:   separately in test case 3 the ability of the underlying apparatus to
    //:   identify and correctly format each primitive type. (C-2,3)
    //
    // Testing:
    //   BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
    //   BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
    //   BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
    //   BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
    //   BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
    //   BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
    // ------------------------------------------------------------------------

    // [ 8] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP_ASSERT"
                    "\n------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP_ASSERT(I, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(),
                             "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP_ASSERT(I, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput,
                    "%s",
                    output->getOutput(),
                    "%s",
                    0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP2_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\tJ: %d\nError %s(%d): idx > LOOP_ITERATIONS"
                         "    (failed)\n",
                     I,
                     J,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP3_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP4_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d",
                              idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP5_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(I, J, K, L, M,
                                          idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     M,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP6_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;
            const int N = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N,
                                          idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;
            const int N = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(s_expectedOutput,
                     BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\tN: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     M,
                     N,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(s_expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(s_expectedOutput));
        }
        testStatus = 0;
    }
}

template <class TEST_TYPE, size_t NUM_DATA>
void TestDriver::testCase3(OutputRedirector                   *output,
                                   const DataRow<TEST_TYPE>(&  DATA)[NUM_DATA],
                                   const char                 *formatString)
{
    // ------------------------------------------------------------------------
    // FORMATTED OUTPUT TEST
    //   Ensure that the 'debugprint' formatted output methods write values to
    //   'stdout' in the expected form
    //
    // Concerns:
    //: 1 The 'debugprint' method writes the value to 'stdout'.
    //:
    //: 2 The 'debugprint' method writes the value in the intended format.
    //:
    //
    // Plan:
    //: 1 Using the table-driven technique:  (C-1, 2)
    //:
    //:   For each data row in the supplied DATA table, reset the output
    //:   redirector, invoke the method under test with the second parameter
    //:   set to the 'd_input' member of the row, then compare the contents of
    //:   the redirector with the expected form.  If the 'd_expectedOutput_p'
    //:   member of the 'DATA' table is non-null, then the null-terminated byte
    //:   string it points to is used as the expected form for that row.  If
    //:   the 'd_expectedOutput_p' member is null, the expected form is
    //:   generated by formatting the 'd_input' member with the specified
    //:   'formatString'.
    //
    // Testing:
    //   void debugprint(bool v)
    //   void debugprint(char v)
    //   void debugprint(signed char v)
    //   void debugprint(unsigned char v)
    //   void debugprint(short v)
    //   void debugprint(unsigned short v)
    //   void debugprint(int v)
    //   void debugprint(unsigned int v)
    //   void debugprint(long v)
    //   void debugprint(unsigned long v)
    //   void debugprint(long long v)
    //   void debugprint(unsigned long long v)
    //   void debugprint(float v)
    //   void debugprint(double v)
    //   void debugprint(long double v)
    //   void debugprint(const char *v)
    //   void debugprint(char *v)
    //   void debugprint(const void *v)
    // ------------------------------------------------------------------------

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE =       DATA[ti].d_line;
        //ARB: Should INPUT not be capitalized, because it is not necessarily
        // constant?
        TEST_TYPE INPUT =      DATA[ti].d_input;
        const char *EXPECTED = DATA[ti].d_expectedOutput_p;

        int expectedSize = 0;

        if (!EXPECTED) {
            expectedSize = snprintf(s_expectedOutput,
                                    BUFFER_SIZE,
                                    formatString,
                                    INPUT);
            ASSERT(expectedSize >= 0 &&
                   expectedSize < BUFFER_SIZE);
            EXPECTED = s_expectedOutput;
        } else {
            expectedSize = static_cast<int>(strlen(EXPECTED));
        }

        if (veryVerbose) {
            PRINT_(ti, "%d");
            PRINT_(INPUT, formatString);
            PRINT(EXPECTED, "%s");
        }

        output->reset();
        bsls::debugprint(INPUT);

        ANNOTATED2_ASSERT(LINE, "%d", INPUT, formatString, output->load());
        ANNOTATED4_ASSERT(LINE, "%d",
                          INPUT, formatString,
                          EXPECTED, "%s",
                          output->getOutput(), "%s",
                          0 == output->compare(EXPECTED, expectedSize));
    }
}

// Test case -1 checks the strategy used in test case 9 to detect stack
// corruption by intentionally providing incorrect format strings to 'printf'.
// This generates warnings that must be suppressed with a GCC pragma.  Some
// versions of the gcc compiler do not allow pragmas inside function bodies
// (i.e. inside 'main'), so the test has been factored out into a separate
// function.

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#endif

void checkStackCorruptionTest()
{
    int           TARGET_ONES   = ~((int) 0);
    unsigned char SENTINEL_ONES = ~((unsigned char) 0);

    struct {
        int           target;
        unsigned char sentinel;
        long long     prophylactic;
    } data;
    data.target       = TARGET_ONES;
    data.sentinel     = SENTINEL_ONES;
    data.prophylactic = 0;

    const char *INPUT = "0";

    // Simulate test case 9, with a format specifier too small for 'int'.

    ASSERT(data.target   == TARGET_ONES);
    ASSERT(data.sentinel == SENTINEL_ONES);

    // The format specifier mismatches the actual variable type.
    // The warning of '-Wformat' type is caused by such attempt.
    // Suppressing the warning as the format mismatch is a part of the
    // test logic.

    sscanf(INPUT, "%c", &data.target);

    // Test case 9 asserts 'data.target == 0', so here we assert the
    // opposite.

    ASSERT(data.target   != 0);
    ASSERT(data.sentinel == SENTINEL_ONES);

    // Simulate test case 9, with a format specifier too large for 'int'.

    data.target       = TARGET_ONES;
    data.sentinel     = SENTINEL_ONES;

    ASSERT(data.target   == TARGET_ONES);
    ASSERT(data.sentinel == SENTINEL_ONES);

    // The format specifier mismatches the actual variable type.
    // The warning of '-Wformat' type is caused by such attempt.
    // Suppressing the warning as the format mismatch is a part of the
    // test logic.

    sscanf(INPUT, "%lld", &data.target);

    // Test case 9 asserts 'data.sentinel == SENTINEL_ONES', so here we
    // assert the opposite.

    ASSERT(data.target   == 0);
    ASSERT(data.sentinel != SENTINEL_ONES);
}

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    fprintf(stderr, "TEST " __FILE__ " CASE %d\n", test);

    // Capture 'stdout', and send 'stderr' to 'stdout', unless we are running
    // the usage example.
    OutputRedirector output;
    if (test != 10 && test != 0) {
        output.redirect();
    }

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate the usage example from the header into the test
        //    driver, remove leading comment characters,E  and replace 'assert'
        //    with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING USAGE EXAMPLE"
                    "\n--------------------\n");
        }

        // The actual usage example code is encapsulated in three free
        // functions, 'testFortyTwo', 'testMyType', and
        // 'testBlockListConstruction' so that it can be relocated to the
        // section of the source file where the standard test macros have been
        // defined in terms of the macros supplied by the component under
        // test.

        testFortyTwo(verbose);
        testMyTypeSetValue(verbose);
        testBlockListConstruction(veryVeryVerbose);

        // None of the usage examples generates a failure, so 'testStatus',
        // defined as part of Example 1, should still be 0.

        ASSERT(testStatus == 0);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PRINTF SUPPORT
        //
        // Concerns:
        //: 1 'printf' format strings constructed with the formatting macros
        //:   produce output with the approriate sign.
        //:
        //: 2 'printf' statements using the formatting macros consume the
        //:   correct number of bytes when reading each optional argument.
        //
        // Plan:
        //: 1 Print out values that do and do not have the highest bit set, and
        //:   confirm that the output presents those values with the correct
        //:   signed-ness for the type being printed.  (C-1)
        //:
        //: 2 Use 'sscanf' to assign a value to a variable, using the format
        //:   specifier under test, and confirm that all bytes in the variable
        //:   are changed by the assignment, and that subsequent memory
        //:   locations are not changed by the assignment.  (C-2)
        //
        // Testing:
        //   BSLS_BSLTESTUTIL_FORMAT_ZU
        //   BSLS_BSLTESTUTIL_FORMAT_TD
        //   BSLS_BSLTESTUTIL_FORMAT_I64
        //   BSLS_BSLTESTUTIL_FORMAT_U64
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING PRINTF SUPPORT"
                    "\n----------------------\n");
        }

        if (verbose) fprintf(stderr, "\nTesting for Correct Output\n");

#ifdef BSLS_PLATFORM_CPU_64_BIT
        const char *SIGNED_EXPECTED   = "<-1> <0> <1> <-9223372036854775808>";
        const char *UNSIGNED_EXPECTED = "<0> <1> <9223372036854775808>";
#else
        const char *SIGNED_EXPECTED   = "<-1> <0> <1> <-2147483648>";
        const char *UNSIGNED_EXPECTED = "<0> <1> <2147483648>";
#endif
        const char *SIGNED_64_EXPECTED   =
                                         "<-1> <0> <1> <-9223372036854775808>";
        const char *UNSIGNED_64_EXPECTED = "<0> <1> <9223372036854775808>";

        if (verbose) fprintf(stderr, "\tTesting 'size_t'\n");
        {
            size_t zero = 0;
            size_t one = 1;
            size_t largeValue = 1;
            largeValue <<= (sizeof(size_t) * CHAR_BIT) - 1;

            output.reset();
            printf("<" TZU "> <" TZU "> <" TZU ">", zero, one, largeValue);
            ASSERT(output.load());
            ANNOTATED2_ASSERT(UNSIGNED_EXPECTED, "%s",
                              output.getOutput(), "%s",
                              0 == output.compare(UNSIGNED_EXPECTED));
        }

        if (verbose) fprintf(stderr, "\tTesting 'ptrdiff_t'\n");
        {
            ptrdiff_t minusOne = -1;
            ptrdiff_t zero = 0;
            ptrdiff_t one = 1;
            ptrdiff_t largeValue = 1;
            largeValue <<= (sizeof(ptrdiff_t) * CHAR_BIT) - 1;

            output.reset();
            printf("<" TTD "> <" TTD "> <" TTD "> <" TTD ">",
                                              minusOne, zero, one, largeValue);
            ASSERT(output.load());
            ANNOTATED2_ASSERT(SIGNED_EXPECTED, "%s",
                              output.getOutput(), "%s",
                              0 == output.compare(SIGNED_EXPECTED));
        }

        if (verbose) fprintf(stderr, "\tTesting 'Int64'\n");
        {
            long long minusOne = -1;
            long long zero = 0;
            long long one = 1;
            long long largeValue = 1;
            largeValue <<= (sizeof(long long) * CHAR_BIT) - 1;

            output.reset();
            printf("<" TI64 "> <" TI64 "> <" TI64 "> <" TI64 ">",
                                              minusOne, zero, one, largeValue);
            ASSERT(output.load());
            ANNOTATED2_ASSERT(SIGNED_64_EXPECTED, "%s",
                              output.getOutput(), "%s",
                              0 == output.compare(SIGNED_64_EXPECTED));
        }

        if (verbose) fprintf(stderr, "\tTesting 'Uint64'\n");
        {
            unsigned long long zero = 0;
            unsigned long long one = 1;
            unsigned long long largeValue = 1;
            largeValue <<= (sizeof(unsigned long long) * CHAR_BIT) - 1;

            output.reset();
            printf("<" TU64 "> <" TU64 "> <" TU64 ">", zero, one, largeValue);
            ASSERT(output.load());
            ANNOTATED2_ASSERT(UNSIGNED_64_EXPECTED, "%s",
                              output.getOutput(), "%s",
                              0 == output.compare(UNSIGNED_64_EXPECTED));
        }

        if (verbose) fprintf(stderr, "\nTesting for Stack Corruption\n");

        if (verbose) fprintf(stderr, "\tTesting 'size_t'\n");
        {
            const size_t        TARGET_ONES   = ~((size_t) 0);
            const unsigned char SENTINEL_ONES = ~((unsigned char) 0);

            struct {
                size_t        target;
                unsigned char sentinel;
                long long     prophylactic;
            } data;
            data.target       = TARGET_ONES;
            data.sentinel     = SENTINEL_ONES;
            data.prophylactic = 0;

            const char *INPUT = "0";

            ASSERT(data.target   == TARGET_ONES);
            ASSERT(data.sentinel == SENTINEL_ONES);

            sscanf(INPUT, TZU, &data.target);

            ASSERT(data.target   ==  0);
            ASSERT(data.sentinel == SENTINEL_ONES);
        }

        if (verbose) fprintf(stderr, "\tTesting 'ptrdiff_t'\n");
        {
            const ptrdiff_t     TARGET_ONES   = ~((ptrdiff_t) 0);
            const unsigned char SENTINEL_ONES = ~((unsigned char) 0);

            struct {
                ptrdiff_t     target;
                unsigned char sentinel;
                long long     prophylactic;
            } data;
            data.target       = TARGET_ONES;
            data.sentinel     = SENTINEL_ONES;
            data.prophylactic = 0;

            const char *INPUT = "0";

            ASSERT(data.target   == TARGET_ONES);
            ASSERT(data.sentinel == SENTINEL_ONES);

            sscanf(INPUT, TTD, &data.target);

            ASSERT(data.target   ==  0);
            ASSERT(data.sentinel == SENTINEL_ONES);
        }

        if (verbose) fprintf(stderr, "\tTesting 'Int64' (equivalent)\n");
        {
            const long long     TARGET_ONES   = ~((long long) 0);
            const unsigned char SENTINEL_ONES = ~((unsigned char) 0);

            struct {
                long long     target;
                unsigned char sentinel;
                long long     prophylactic;
            } data;
            data.target       = TARGET_ONES;
            data.sentinel     = SENTINEL_ONES;
            data.prophylactic = 0;

            const char *INPUT = "0";

            ASSERT(data.target   == TARGET_ONES);
            ASSERT(data.sentinel == SENTINEL_ONES);

            sscanf(INPUT, TI64, &data.target);

            ASSERT(data.target   ==  0);
            ASSERT(data.sentinel == SENTINEL_ONES);
        }

        if (verbose) fprintf(stderr, "\tTesting 'Uint64' (equivalent)\n");
        {
            const unsigned long long TARGET_ONES   = ~((unsigned long long) 0);
            const unsigned char      SENTINEL_ONES = ~((unsigned char) 0);

            struct {
                unsigned long long target;
                unsigned char      sentinel;
                long long          prophylactic;
            } data;
            data.target       = TARGET_ONES;
            data.sentinel     = SENTINEL_ONES;
            data.prophylactic = 0;

            const char *INPUT = "0";

            ASSERT(data.target   == TARGET_ONES);
            ASSERT(data.sentinel == SENTINEL_ONES);

            sscanf(INPUT, TU64, &data.target);

            ASSERT(data.target   ==  0);
            ASSERT(data.sentinel == SENTINEL_ONES);
        }

      } break;
      case 8: {
        // ------------------------------------------------------------------
        // TESTING BSLS_BSLTESTUTIL_LOOP*_ASSERT MACROS
        //
        // Testing:
        //   BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
        //   BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
        //   BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
        //   BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
        //   BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
        //   BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
        // ------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING LOOP ASSERT MACROS"
                    "\n--------------------------\n");
        }

        TestDriver::testCase8(&output);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // IDENTIFIER OUTPUT MACRO TEST (standard 'Q' macro)
        //
        // Concerns:
        //: 1 Identifier output macro emits output.
        //:
        //: 2 Output emitted is in correct format for the standard Q() macro,
        //:   i.e. '<| [macroargument] |>', where '[macroargument]' is the
        //:   tokenization of the text supplied as argument to the macro.
        //
        // Plan:
        //: 1 Call 'BSLS_BSLTESTUTIL_Q' with a series of arbitrary identifiers
        //:   containing single, and multiple tokens, with an without initial,
        //:   final, and repeated whitespace and compare the captured output to
        //:   a model string. (C-1,2)

        //
        // Testing:
        //   BSLS_BSLTESTUTIL_Q(X)
        // --------------------------------------------------------------------

        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_Q"
                        "\n--------------------------\n");
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(sample);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| sample |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(embedded white   space);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| embedded white space |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(   initial whitespace);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| initial whitespace |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(final whitespace   );
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| final whitespace |>\n"));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VALUE OUTPUT MACRO TEST (standard 'P' and 'P_' macros)
        //
        // Concerns:
        //: 1 Value output macros emit output.
        //:
        //: 2 Output emitted is in correct format for the standard 'P' and 'P_'
        //:   macros, i.e. 'identifier = value' (with following newline in the
        //:   case of 'BSLS_BSLTESTUTIL_P') where 'identifier' is the name of
        //:   the argument supplied to the macro, and 'value' is the value of
        //:   that argument.  Note that we are not concerned here with the
        //:   exact formatting of the 'value' portion for all types, as that
        //:   will be tested in test case 3.
        //
        // Plan:
        //: 1 Call the value output macros on a variable of known value, and
        //:   confirm that the captured output is in the correct format.  Note
        //:   that it is only necessary to conduct this test once with a single
        //:   variable type, because the underlying type-differentiation and
        //:   formatting mechanisms are tested in test case 3. (C-1,2)
        //
        // Testing:
        //   BSLS_BSLTESTUTIL_P(X)
        //   BSLS_BSLTESTUTIL_P_(X)
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING VALUE OUTPUT MACROS"
                    "\n---------------------------\n");
        }

        // [ 6] BSLS_BSLTESTUTIL_P(X)
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_P"
                        "\n--------------------------\n");
            }

            const int INPUT = 42;

            output.reset();
            BSLS_BSLTESTUTIL_P(INPUT);
            ASSERT(output.load());
            ANNOTATED_ASSERT(output.getOutput(), "%s",
                             0 == output.compare("INPUT = 42\n"));
        }

        // [ 6] BSLS_BSLTESTUTIL_P_(X)
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_P_"
                        "\n---------------------------\n");
            }

            const int INPUT = 42;

            output.reset();
            BSLS_BSLTESTUTIL_P_(INPUT);
            ASSERT(output.load());
            ANNOTATED_ASSERT(output.getOutput(), "%s",
                             0 == output.compare("INPUT = 42, "));
        }

     } break;
      case 5: {
        // --------------------------------------------------------------------
        // STATIC MACRO TEST (standard 'L_' and 'T_' macros)
        //
        // Concerns:
        //: 1 Line number macro has the correct value.
        //:
        //: 2 Tab output macro emits output.
        //:
        //: 3 Tab output macro output emitted is in correct format, i.e. is a
        //:   single tab character.
        //
        // Plan:
        //: 1 Compare the value of the line number macro to '__LINE__'. (C-1)
        //: 2 Call the tab output macro, and confirm that the captured output
        //:   is in the correct format.  (C-2,3)
        //
        // Testing:
        //   BSLS_BSLTESTUTIL_L_
        //   BSLS_BSLTESTUTIL_T_
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING STATIC MACROS"
                    "\n---------------------\n");
        }

        // [ 5] BSLS_BSLTESTUTIL_L_
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_L_"
                        "\n---------------------------\n");
            }

            // Line spacing is significant, as it assures BSLS_BSLTESTUTIL_L_
            // is not a simple sequence.
            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);


            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);
            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);



            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);

            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);

        }

        // [ 5] BSLS_BSLTESTUTIL_T_
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_T_"
                        "\n---------------------------\n");
            }

            output.reset();
            BSLS_BSLTESTUTIL_T_
            ASSERT(output.load());
            ASSERT(0 == output.compare("\t"));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // UNFORMATTED OUTPUT TEST
        //
        // Concerns:
        //: 1 Unformatted output methods emit output.
        //:
        //: 2 Output emitted is in correct format
        //:
        //:   For printStringNoFlush, the correct format is identical to the
        //:   input string.
        //:
        //:   For printTab, the correct format is a single tab character.
        //:
        //: 3 'printStringNoFlush' does not flush output between calls
        //
        // Plan:
        //: 1 Using the table-driven technique, call 'printStringNoFlush' with
        //:   a variety of arguments, and check that the captured output is
        //:   correct.  (C-1,2)
        //:
        //: 2 Call 'printStringNoFlush' twice in succession, and check that the
        //:   captured output matches the concatenation of the input strings,
        //:   with no embedded nulls or newlines.  (C-3)
        //:
        //: 3 Call 'printTab' and check that the captured output is correct.
        //:   (C-1,2)
        //
        // Testing:
        //   static void printStringNoFlush(const char *s);
        //   static void printTab();
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING UNFORMATTED OUPUT METHODS"
                    "\n---------------------------------\n");
        }

        // [ 4] static void printStringNoFlush(const char *s);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING printStringNoFlush"
                        "\n--------------------------\n");
            }

            static const DataRow<const char *> DATA[] =
            {
                //LINE       INPUT    OUTPUT    DESC
                //---------- -----    ------    ----

                { __LINE__, "",       "",       "empty string" },
                { __LINE__, "a",      "a",      "non-empty string" },
                { __LINE__, "a\nb",   "a\nb",   "embedded newline" },
                { __LINE__, "a\r\nb", "a\r\nb", "embedded <CRLF>" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const char *INPUT =    DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                if (veryVerbose) {
                    PRINT_(ti, "%d"); PRINT(INPUT, "'%s'");
                }

                output.reset();
                bsls::BslTestUtil::printStringNoFlush(INPUT);
                ANNOTATED2_ASSERT(LINE, "%d", INPUT, "%s", output.load());
                ANNOTATED4_ASSERT(LINE, "%d",
                                  INPUT, "%s",
                                  EXPECTED, "%s",
                                  output.getOutput(), "%s",
                                  0 == output.compare(EXPECTED));
            }

            output.reset();
            bsls::BslTestUtil::printStringNoFlush("abc");
            bsls::BslTestUtil::printStringNoFlush("def");
            ASSERT(output.load());
            ASSERT(0 == output.compare("abcdef"));
        }

        // [ 4] static void printTab();
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING printTab"
                        "\n----------------\n");
            }

            output.reset();
            bsls::BslTestUtil::printTab();
            ASSERT(output.load());
            ASSERT(0 == output.compare("\t"));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FORMATTED OUTPUT TEST
        //   Ensure that the 'debugprint' formatted output methods write values
        //   to 'stdout' in the expected form
        //
        // Concerns:
        //: 1 The 'debugprint' method writes the value to 'stdout'.
        //:
        //: 2 The 'debugprint' method writes the value in the intended format.
        //:
        //: 3 The appropriate overload is called unambiguously for each
        //:   fundamental data type, and for any pointer type.
        //:
        //: 4 The appropriate overload is called unambiguously for
        //:   const/volatile qualified arguments.
        //
        // Plan:
        //: 1 Using the table-driven technique:  (C-1, 2)
        //:
        //:   For each overload of debugprint, choose a number of significant
        //:   values for the second parameter, and check that each is written
        //:   to 'stdout' in the intended format.  The actual checking is
        //:   delegated to 'TestDriver::testCase3'.  Note that in the data
        //:   table supplied to 'TestDriver::testCase3', the 'OUTPUT' column
        //:   may be set to null, in which case the intended format is taken to
        //:   be the 'INPUT' column formatted by 'printf' using the last
        //:   argument to 'TestDriver::testCase3'.
        //:
        //: 2 For fundamental types, much coverage of concern 3 is covered by
        //:   checking for correct output in plan 1.  Beyond this, concern 3
        //:   devolves for fundamental types into testing the compiler.
        //:   Continuing the same methodology as plan 1, there are a few corner
        //:   cases that can additionally be checked: (C-3)
        //:   o That decayed arrays devolve to pointers handled by the 'const
        //:     void *' overload.
        //:   o That pointers to arbitrary types are handled by the 'const void
        //:     *' overload.
        //:
        //: 3 All of the tests from plan 1 are repeated with 'const',
        //:   'volatile', and 'const volatile' input data. (C-4)
        //
        // Testing:
        //   void debugprint(bool v)
        //   void debugprint(char v)
        //   void debugprint(signed char v)
        //   void debugprint(unsigned char v)
        //   void debugprint(short v)
        //   void debugprint(unsigned short v)
        //   void debugprint(int v)
        //   void debugprint(unsigned int v)
        //   void debugprint(long v)
        //   void debugprint(unsigned long v)
        //   void debugprint(long long v)
        //   void debugprint(unsigned long long v)
        //   void debugprint(float v)
        //   void debugprint(double v)
        //   void debugprint(long double v)
        //   void debugprint(const char *v)
        //   void debugprint(char *v)
        //   void debugprint(const void *v)
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING FORMATTED OUTPUT METHODS"
                    "\n--------------------------------\n");
        }

        // void debugprint(bool b);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR bool"
                        "\n---------------------------\n");
            }

            static const DataRow<bool> DATA[] =
            {
                //LINE       INPUT  OUTPUT   DESC
                //---------- -----  ------   ----

                { __LINE__,  true,  "true",  "true" },
                { __LINE__,  false, "false", "false" },
            };
            TestDriver::testCase3<bool>(&output, DATA, "%d");
        }

        // void debugprint(char c);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR char"
                        "\n---------------------------\n");
            }

            static const DataRow<char> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  '\0',     0,     "\\0" },
                { __LINE__,  CHAR_MIN, 0,     "CHAR_MIN" },
                { __LINE__,  CHAR_MAX, 0,     "CHAR_MAX" },
                { __LINE__,  '\x01',   0,     "\\x01" },
                { __LINE__,  '\t',     0,     "tab character" },
                { __LINE__,  '\n',     0,     "newline character" },
                { __LINE__,  '\r',     0,     "carriage return character" },
                { __LINE__,  'A',      0,     "positive signed character" },
                { __LINE__,  '\xcc',   0,     "negative signed character" },
            };
            TestDriver::testCase3<char>(&output, DATA, "'%c'");
        }

        // void debugprint(signed char v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR signed char"
                        "\n----------------------------------\n");
            }

            static const DataRow<signed char> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  '\0',      0,     "null character" },
                { __LINE__,  '\x01',    0,     "\\x01" },
                { __LINE__,  SCHAR_MAX, 0,     "SCHAR_MAX" },
                { __LINE__,  SCHAR_MIN, 0,     "SCHAR_MIN" },
                { __LINE__,  '\x50',    0,     "positive signed character" },
                { __LINE__,  '\xcc',    0,     "negative signed character" },
            };
            TestDriver::testCase3<signed char>(&output,
                                               DATA,
                                               "%hhd");
        }

        // void debugprint(unsigned char v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR unsigned char"
                        "\n------------------------------------\n");
            }

            static const DataRow<unsigned char> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  '\0',      0,     "null character" },
                { __LINE__,  '\x01',    0,     "\\x01" },
                { __LINE__,  UCHAR_MAX, 0,     "UCHAR_MAX" },
                { __LINE__,  '\x50',    0,     "positive signed character" },
                { __LINE__,  (unsigned char) '\xcc',
                                        0,     "negative signed character" },
            };
            TestDriver::testCase3<unsigned char>(&output,
                                                 DATA,
                                                 "%hhu");
        }

        // void debugprint(short v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR short"
                        "\n----------------------------\n");
            }

            static const DataRow<short> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  -1,       0,     "-1" },
                { __LINE__,  SHRT_MAX, 0,     "SHRT_MAX" },
                { __LINE__,  SHRT_MIN, 0,     "SHRT_MIN" },
            };
            TestDriver::testCase3<short>(&output, DATA, "%hd");
        }

        // void debugprint(unsigned short v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR unsigned short"
                        "\n-------------------------------------\n");
            }

            static const DataRow<unsigned short> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0,         0,     "0" },
                { __LINE__,  1,         0,     "1" },
                { __LINE__,  USHRT_MAX, 0,     "USHRT_MAX" },
            };
            TestDriver::testCase3<unsigned short>(&output,
                                                  DATA,
                                                  "%hu");
        }

        // void debugprint(int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR int"
                        "\n--------------------------\n");
            }

            static const DataRow<int> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  0,       0,     "0" },
                { __LINE__,  1,       0,     "1" },
                { __LINE__,  -1,      0,     "-1" },
                { __LINE__,  INT_MAX, 0,     "INT_MAX" },
                { __LINE__,  INT_MIN, 0,     "INT_MIN" },
            };
            TestDriver::testCase3<int>(&output, DATA, "%d");
        }

        // void debugprint(unsigned int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR unsigned int"
                        "\n-----------------------------------\n");
            }

            static const DataRow<unsigned int> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  UINT_MAX, 0,     "UINT_MAX" },
            };
            TestDriver::testCase3<unsigned int>(&output, DATA, "%u");
        }

        // void debugprint(long v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR long"
                        "\n---------------------------\n");
            }

            static const DataRow<long> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0L,       0,     "0L" },
                { __LINE__,  1L,       0,     "1L" },
                { __LINE__,  -1L,      0,     "-1L" },
                { __LINE__,  LONG_MAX, 0,     "LONG_MAX" },
                { __LINE__,  LONG_MIN, 0,     "LONG_MIN" },
            };
            TestDriver::testCase3<long>(&output, DATA, "%ld");
        }

        // void debugprint(unsigned long v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR unsigned long"
                        "\n------------------------------------\n");
            }

            static const DataRow<unsigned long> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0UL,       0,     "0UL" },
                { __LINE__,  1UL,       0,     "1UL" },
                { __LINE__,  ULONG_MAX, 0,     "ULONG_MAX" },
            };
            TestDriver::testCase3<unsigned long>(&output,
                                                 DATA,
                                                 "%lu");
        }

        // void debugprint(long long v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR long long"
                        "\n--------------------------------\n");
            }

            static const DataRow<long long> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0LL,       0,     "0LL" },
                { __LINE__,  1LL,       0,     "1LL" },
                { __LINE__,  -1LL,      0,     "-1LL" },
                { __LINE__,  LLONG_MAX, 0,     "LLONG_MAX" },
                { __LINE__,  LLONG_MIN, 0,     "LLONG_MIN" },
            };
            TestDriver::testCase3<long long>(&output, DATA, "%lld");
        }

        // void debugprint(unsigned long long v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR unsigned long long"
                        "\n-----------------------------------------\n");
            }

            static const DataRow<unsigned long long> DATA[] =
            {
                //LINE       INPUT       OUTPUT DESC
                //---------- -----       ------ ----

                { __LINE__,  0ULL,       0,     "0ULL" },
                { __LINE__,  1ULL,       0,     "1ULL" },
                { __LINE__,  ULLONG_MAX, 0,     "ULLONG_MAX" },
            };
            TestDriver::testCase3<unsigned long long>(&output,
                                                      DATA,
                                                      "%llu");
        }

        // void debugprint(float v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR float"
                        "\n----------------------------\n");
            }

            static const DataRow<float> DATA[] =
            {
                //LINE       INPUT        OUTPUT DESC
                //---------- -----        ------ ----

                { __LINE__,  0.0F,        0,     "0.0F" },
                { __LINE__,  1.0F,        0,     "1.0F" },
                { __LINE__,  -1.0F,       0,     "-1.0F" },
                { __LINE__,  1.0F / 3.0F, 0,     "(float) 1/3" },
                { __LINE__,  FLT_MAX,     0,     "FLT_MAX" },
                { __LINE__,  FLT_MIN,     0,     "FLT_MIN" },
            };
            TestDriver::testCase3<float>(&output, DATA, "'%f'");
        }

        // void debugprint(double v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR double"
                        "\n-----------------------------\n");
            }

            static const DataRow<double> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0.0,       0,     "0.0" },
                { __LINE__,  1.0,       0,     "1.0" },
                { __LINE__,  -1.0,      0,     "-1.0" },
                { __LINE__,  1.0 / 3.0, 0,     "(double) 1/3" },
                { __LINE__,  DBL_MAX,   0,     "DBL_MAX" },
                { __LINE__,  DBL_MIN,   0,     "DBL_MIN" },
            };
            TestDriver::testCase3<double>(&output, DATA, "'%g'");
        }

        // void debugprint(long double v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR long double"
                        "\n----------------------------------\n");
            }

            static const DataRow<long double> DATA[] =
            {
                //LINE       INPUT        OUTPUT DESC
                //---------- -----        ------ ----

                { __LINE__,  0.0L,        0,     "0.0L" },
                { __LINE__,  1.0L,        0,     "1.0L" },
                { __LINE__,  -1.0L,       0,     "-1.0L" },
                { __LINE__,  1.0L / 3.0L, 0,     "(long double) 1/3" },
                { __LINE__,  LDBL_MAX,    0,     "LDBL_MAX" },
                { __LINE__,  LDBL_MIN,    0,     "LDBL_MIN" },
            };
            TestDriver::testCase3<long double>(&output, DATA, "'%Lg'");
        }

        // void debugprint(char *str);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR char *"
                        "\n-----------------------------\n");
            }

            static const DataRow<char *> DATA[] =
            {
                //LINE       INPUT                         OUTPUT          DESC
                //---------- -----                         ------          ----

                { __LINE__,  static_cast<char *>(0),       "(null)",
                                                              "null string"  },
                { __LINE__,  const_cast<char *>(""),       0,
                                                              "empty string" },
                { __LINE__,  const_cast<char *>("a"),      0,
                                                          "non-empty string" },
                { __LINE__,  const_cast<char *>("a\nb"),   0,
                                              "string with embedded newline" },
                { __LINE__,  const_cast<char *>("a\r\nb"), 0,
                                               "string with embedded <CRLF>" },
            };
            TestDriver::testCase3<char *>(&output, DATA, "\"%s\"");
        }

        // void debugprint(const char *str);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const char *"
                        "\n-----------------------------------\n");
            }

            static const DataRow<const char *> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  static_cast<const char *>(0),
                                    "(null)", "null string"  },
                { __LINE__,  "",       0,     "empty string" },
                { __LINE__,  "a",      0,     "non-empty string" },
                { __LINE__,  "a\nb",   0,     "string with embedded newline" },
                { __LINE__,  "a\r\nb", 0,     "string with embedded <CRLF>" },
            };
            TestDriver::testCase3<const char *>(&output,
                                                DATA,
                                                "\"%s\"");
        }

        // void debugprint(void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR void *"
                        "\n-----------------------------\n");
            }

            static const DataRow<void *> DATA[] =
            {
                //LINE       INPUT             OUTPUT DESC
                //---------- -----             ------ ----

                { __LINE__,  static_cast<void *>(0),
                                               0,     "NULL pointer" },
                { __LINE__,  static_cast<void *>(&output),
                                               0,     "valid address" },
            };
            TestDriver::testCase3<void *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const void *"
                        "\n-----------------------------------\n");
            }

            static const DataRow<const void *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  static_cast<const void *>(0),
                                                     0,     "NULL pointer" },
                { __LINE__,  static_cast<const void *>(&output),
                                                     0,     "valid address" },
            };
            TestDriver::testCase3<const void *>(&output, DATA, "%p");
        }

        // CONCERN 4: const


        // void debugprint(char c);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const char"
                        "\n---------------------------------\n");
            }

            static const DataRow<const char> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  '\0',     0,     "\\0" },
                { __LINE__,  CHAR_MIN, 0,     "CHAR_MIN" },
                { __LINE__,  CHAR_MAX, 0,     "CHAR_MAX" },
                { __LINE__,  '\x01',   0,     "\\x01" },
                { __LINE__,  '\t',     0,     "tab character" },
                { __LINE__,  '\n',     0,     "newline character" },
                { __LINE__,  '\r',     0,     "carriage return character" },
                { __LINE__,  'A',      0,     "positive signed character" },
                { __LINE__,  '\xcc',   0,     "negative signed character" },
            };
            TestDriver::testCase3<const char>(&output, DATA, "'%c'");
        }

        // void debugprint(int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const int"
                        "\n--------------------------------\n");
            }

            static const DataRow<const int> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  0,       0,     "0" },
                { __LINE__,  1,       0,     "1" },
                { __LINE__,  -1,      0,     "-1" },
                { __LINE__,  INT_MAX, 0,     "INT_MAX" },
                { __LINE__,  INT_MIN, 0,     "INT_MIN" },
            };
            TestDriver::testCase3<const int>(&output, DATA, "%d");
        }

        // void debugprint(unsigned int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const unsigned int"
                        "\n-----------------------------------------\n");
            }

            static const DataRow<const unsigned int> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  UINT_MAX, 0,     "UINT_MAX" },
            };
            TestDriver::testCase3<const unsigned int>(&output, DATA, "%u");
        }

        // void debugprint(double v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const double"
                        "\n-----------------------------------\n");
            }

            static const DataRow<const double> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0.0,       0,     "0.0" },
                { __LINE__,  1.0,       0,     "1.0" },
                { __LINE__,  -1.0,      0,     "-1.0" },
                { __LINE__,  1.0 / 3.0, 0,     "(double) 1/3" },
                { __LINE__,  DBL_MAX,   0,     "DBL_MAX" },
                { __LINE__,  DBL_MIN,   0,     "DBL_MIN" },
            };
            TestDriver::testCase3<const double>(&output, DATA, "'%g'");
        }

        // CONCERN 4: volatile

        // void debugprint(char c);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile char"
                        "\n------------------------------------\n");
            }

            static const DataRow<volatile char> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  '\0',     0,     "\\0" },
                { __LINE__,  CHAR_MIN, 0,     "CHAR_MIN" },
                { __LINE__,  CHAR_MAX, 0,     "CHAR_MAX" },
                { __LINE__,  '\x01',   0,     "\\x01" },
                { __LINE__,  '\t',     0,     "tab character" },
                { __LINE__,  '\n',     0,     "newline character" },
                { __LINE__,  '\r',     0,     "carriage return character" },
                { __LINE__,  'A',      0,     "positive signed character" },
                { __LINE__,  '\xcc',   0,     "negative signed character" },
            };
            TestDriver::testCase3<volatile char>(&output, DATA, "'%c'");
        }

        // void debugprint(int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile int"
                        "\n-----------------------------------\n");
            }

            static const DataRow<volatile int> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  0,       0,     "0" },
                { __LINE__,  1,       0,     "1" },
                { __LINE__,  -1,      0,     "-1" },
                { __LINE__,  INT_MAX, 0,     "INT_MAX" },
                { __LINE__,  INT_MIN, 0,     "INT_MIN" },
            };
            TestDriver::testCase3<volatile int>(&output, DATA, "%d");
        }

        // void debugprint(unsigned int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile unsigned int"
                        "\n--------------------------------------------\n");
            }

            static const DataRow<volatile unsigned int> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  UINT_MAX, 0,     "UINT_MAX" },
            };
            TestDriver::testCase3<volatile unsigned int>(&output, DATA, "%u");
        }

        // void debugprint(double v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile double"
                        "\n--------------------------------------\n");
            }

            static const DataRow<volatile double> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0.0,       0,     "0.0" },
                { __LINE__,  1.0,       0,     "1.0" },
                { __LINE__,  -1.0,      0,     "-1.0" },
                { __LINE__,  1.0 / 3.0, 0,     "(double) 1/3" },
                { __LINE__,  DBL_MAX,   0,     "DBL_MAX" },
                { __LINE__,  DBL_MIN,   0,     "DBL_MIN" },
            };
            TestDriver::testCase3<volatile double>(&output, DATA, "'%g'");
        }

        // void debugprint(const char *str);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile char *"
                        "\n--------------------------------------\n");
            }

            static const DataRow<volatile char *> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  static_cast<volatile char *>(0),
                                    "(null)", "null string"  },
                { __LINE__,  const_cast<volatile char *>(""),
                                       0,     "empty string" },
                { __LINE__,  const_cast<volatile char *>("a"),
                                       0,     "non-empty string" },
                { __LINE__,  const_cast<volatile char *>("a\nb"),
                                       0,     "string with embedded newline" },
                { __LINE__,  const_cast<volatile char *>("a\r\nb"),
                                       0,     "string with embedded <CRLF>" },
            };
            TestDriver::testCase3<volatile char *>(&output,
                                                DATA,
                                                "\"%s\"");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile void *"
                        "\n--------------------------------------\n");
            }

            static const DataRow<volatile void *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  static_cast<volatile void *>(0),
                                                     0,     "NULL pointer" },
                { __LINE__,  static_cast<volatile void *>(&output),
                                                     0,     "valid address" },
            };
            TestDriver::testCase3<volatile void *>(&output, DATA, "%p");
        }

        // CONCERN 4: const volatile

        // void debugprint(char c);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile char"
                        "\n------------------------------------------\n");
            }

            static const DataRow<const volatile char> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  '\0',     0,     "\\0" },
                { __LINE__,  CHAR_MIN, 0,     "CHAR_MIN" },
                { __LINE__,  CHAR_MAX, 0,     "CHAR_MAX" },
                { __LINE__,  '\x01',   0,     "\\x01" },
                { __LINE__,  '\t',     0,     "tab character" },
                { __LINE__,  '\n',     0,     "newline character" },
                { __LINE__,  '\r',     0,     "carriage return character" },
                { __LINE__,  'A',      0,     "positive signed character" },
                { __LINE__,  '\xcc',   0,     "negative signed character" },
            };
            TestDriver::testCase3<const volatile char>(&output, DATA, "'%c'");
        }

        // void debugprint(int v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile int"
                        "\n-----------------------------------------\n");
            }

            static const DataRow<const volatile int> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  0,       0,     "0" },
                { __LINE__,  1,       0,     "1" },
                { __LINE__,  -1,      0,     "-1" },
                { __LINE__,  INT_MAX, 0,     "INT_MAX" },
                { __LINE__,  INT_MIN, 0,     "INT_MIN" },
            };
            TestDriver::testCase3<const volatile int>(&output, DATA, "%d");
        }

        // void debugprint(unsigned int v);
        {
            if (verbose) {
                fprintf(stderr,
                    "\nTESTING debugprint FOR const volatile unsigned int"
                    "\n---------------------------------------------------\n");
            }

            static const DataRow<const volatile unsigned int> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  UINT_MAX, 0,     "UINT_MAX" },
            };
            TestDriver::testCase3<const volatile unsigned int>(&output,
                                                               DATA,
                                                               "%u");
        }

        // void debugprint(double v);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile double"
                        "\n--------------------------------------------\n");
            }

            static const DataRow<const volatile double> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0.0,       0,     "0.0" },
                { __LINE__,  1.0,       0,     "1.0" },
                { __LINE__,  -1.0,      0,     "-1.0" },
                { __LINE__,  1.0 / 3.0, 0,     "(double) 1/3" },
                { __LINE__,  DBL_MAX,   0,     "DBL_MAX" },
                { __LINE__,  DBL_MIN,   0,     "DBL_MIN" },
            };
            TestDriver::testCase3<const volatile double>(&output,
                                                         DATA,
                                                         "'%g'");
        }

        // void debugprint(const char *str);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile char *"
                        "\n--------------------------------------------\n");
            }

            static const DataRow<const volatile char *> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  static_cast<const volatile char *>(0),
                                    "(null)", "null string"  },
                { __LINE__,  "",       0,     "empty string" },
                { __LINE__,  "a",      0,     "non-empty string" },
                { __LINE__,  "a\nb",   0,     "string with embedded newline" },
                { __LINE__,  "a\r\nb", 0,     "string with embedded <CRLF>" },
            };
            TestDriver::testCase3<const volatile char *>(&output,
                                                DATA,
                                                "\"%s\"");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile void *"
                        "\n--------------------------------------------\n");
            }

            static const DataRow<const volatile void *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  static_cast<const volatile void *>(0),
                                                     0,     "NULL pointer" },
                { __LINE__,  static_cast<const volatile void *>(&output),
                                                     0,     "valid address" },
            };
            TestDriver::testCase3<const volatile void *>(&output, DATA, "%p");
        }

        // CONCERN 3

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR decayed int array"
                        "\n----------------------------------------\n");
            }

            int a[3] = { 1, 2, 3 };

            // N.B. empty non-dynamic arrays are not allowed in conforming
            // compilers, and empty dynamic arrays are just pointers from the
            // outset.  So we only test non-empty arrays for array decay.
            static const DataRow<int *> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  a,       0,     "non-empty int array" },
            };
            TestDriver::testCase3<int *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR decayed char array"
                        "\n-----------------------------------------\n");
            }

            char a[4] = { 'a', 'b', 'c', '\0' };

            // N.B. empty non-dynamic arrays are not allowed in conforming
            // compilers, and empty dynamic arrays are just pointers from the
            // outset.  So we only test non-empty arrays for array decay.
            static const DataRow<char *> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  a,       0,     "non-empty char array" },
            };
            TestDriver::testCase3<char *>(&output, DATA, "\"%s\"");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR int *"
                        "\n----------------------------\n");
            }

            int a = 1;

            static const DataRow<int *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "int pointer" },
            };
            TestDriver::testCase3<int *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const int *"
                        "\n----------------------------------\n");
            }

            const int a = 1;

            static const DataRow<const int *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "int pointer" },
            };
            TestDriver::testCase3<const int *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile int *"
                        "\n-------------------------------------\n");
            }

            volatile int a = 1;

            static const DataRow<volatile int *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "int pointer" },
            };
            TestDriver::testCase3<volatile int *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const volatile int *"
                        "\n-------------------------------------------\n");
            }

            const volatile int a = 1;

            static const DataRow<const volatile int *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "int pointer" },
            };
            TestDriver::testCase3<const volatile int *>(&output, DATA, "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR TestType *"
                        "\n---------------------------------\n");
            }

            xyzb::TestType a(1);

            static const DataRow<xyzb::TestType *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "TestType pointer" },
            };
            TestDriver::testCase3<xyzb::TestType *>(&output, DATA, "%p");
        }

        // User-defined types

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR const TestType *"
                        "\n---------------------------------------\n");
            }

            const xyzb::TestType a(1);

            static const DataRow<const xyzb::TestType *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "TestType pointer" },
            };
            TestDriver::testCase3<const xyzb::TestType *>(&output,
                                                          DATA,
                                                          "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugprint FOR volatile TestType *"
                        "\n------------------------------------------\n");
            }

            volatile xyzb::TestType a(1);

            static const DataRow<volatile xyzb::TestType *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "TestType pointer" },
            };
            TestDriver::testCase3<volatile xyzb::TestType *>(&output,
                                                             DATA,
                                                             "%p");
        }

        // void debugprint(const void *p);
        {
            if (verbose) {
                fprintf(stderr,
                       "\nTESTING debugprint FOR const volatile TestType *"
                       "\n------------------------------------------------\n");
            }

            const volatile xyzb::TestType a(1);

            static const DataRow<const volatile xyzb::TestType *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  &a,       0,     "TestType pointer" },
            };

            TestDriver::testCase3<const volatile xyzb::TestType *>(&output,
                                                                   DATA,
                                                                   "%p");
        }

        // Function pointers
        // void debugprint<RESULT>(RESULT (*v)());
        {
            if (verbose) {
                fprintf(stderr,
                       "\nTESTING debugprint FOR RESULT (*v)()"
                       "\n------------------------------------\n");
            }

            const char *format = "%p";

            // Normal function pointers
            {
                static const DataRow<void (*)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyVoidFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<void (*)()>(&output,
                                                  DATA,
                                                  format);
            }

            {
                static const DataRow<int (*)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyIntFunction,  0, "function pointer" },
                };

                TestDriver::testCase3<int (*)()>(&output,
                                                 DATA,
                                                 format);
            }

            {
                static const DataRow<double (*)()> DATA[] =
                {
                    //LINE       INPUT           OUTPUT  DESC
                    //---------- -----           ------  ----

                    { __LINE__,  dummyDoubleFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<double (*)()>(&output,
                                                    DATA,
                                                    format);
            }

            // Const function pointers
            {
                static const DataRow<void (* const)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyVoidFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<void (* const)()>(&output,
                                                  DATA,
                                                  format);
            }

            {
                static const DataRow<int (* const)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyIntFunction,  0, "function pointer" },
                };

                TestDriver::testCase3<int (* const)()>(&output,
                                                 DATA,
                                                 format);
            }

            {
                static const DataRow<double (* const)()> DATA[] =
                {
                    //LINE       INPUT           OUTPUT  DESC
                    //---------- -----           ------  ----

                    { __LINE__,  dummyDoubleFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<double (* const)()>(&output,
                                                    DATA,
                                                    format);
            }

            // volatile function pointers
            {
                static const DataRow<void (* volatile)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyVoidFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<void (* volatile)()>(&output,
                                                  DATA,
                                                  format);
            }

            {
                static const DataRow<int (* volatile)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyIntFunction,  0, "function pointer" },
                };

                TestDriver::testCase3<int (* volatile)()>(&output,
                                                 DATA,
                                                 format);
            }

            {
                static const DataRow<double (* volatile)()> DATA[] =
                {
                    //LINE       INPUT           OUTPUT  DESC
                    //---------- -----           ------  ----

                    { __LINE__,  dummyDoubleFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<double (* volatile)()>(&output,
                                                    DATA,
                                                    format);
            }

            // Const volatile function pointers
            {
                static const DataRow<void (* const volatile)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyVoidFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<void (* const volatile)()>(&output,
                                                  DATA,
                                                  format);
            }

            {
                static const DataRow<int (* const volatile)()> DATA[] =
                {
                    //LINE       INPUT         OUTPUT  DESC
                    //---------- -----         ------  ----

                    { __LINE__,  dummyIntFunction,  0, "function pointer" },
                };

                TestDriver::testCase3<int (* const volatile)()>(&output,
                                                 DATA,
                                                 format);
            }

            {
                static const DataRow<double (* const volatile)()> DATA[] =
                {
                    //LINE       INPUT           OUTPUT  DESC
                    //---------- -----           ------  ----

                    { __LINE__,  dummyDoubleFunction, 0, "function pointer" },
                };

                TestDriver::testCase3<double (* const volatile)()>(&output,
                                                    DATA,
                                                    format);
            }
        }


      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS TEST
        //
        // Concerns:
        //:  1 Output is redirected
        //:
        //:  2 Captured output is readable
        //:
        //:  3 'load' works
        //:
        //:  4 'reset' works
        //:
        //:  5 'compare' works
        //:
        //:  6 Incorrect output is correctly diagnosed
        //:
        //:  7 Embedded newlines work
        //:
        //:  8 Empty output works
        //:
        //:  9 Embedded nulls work
        //:
        //: 10 Filesystem-dependent control sequences work
        //:
        //: 11 stderr points to original target of 'stdout'
        //
        // Plan:
        //:  1 Confirm that 'ftell(stdout)' succeeds.  This demonstrates that
        //:    'stdout' is a seekable file. (C-1)
        //:
        //:  2 Write a string to 'stdout', confirm that 'stdout's seek position
        //:    has changed, read back the contents of 'stdout' and compare them
        //:    to the original string.  (C-2)
        //:
        //:  3 Write a string to 'stdout'.  Confirm that
        //:    'OutputRedirector::load' changes the contents of the output
        //:    buffer and that it changes the result of
        //:    'OutputRedirector::isOutputReady' from 'false' to 'true'.
        //:    Confirm that the contents of the output buffer match the
        //:    original string.  (C-3)
        //:
        //:  4 Write a string to 'stdout' and load it with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::reset' rewinds 'stdout', changes the output
        //:    of 'OutputRedirector::isOutputReady' from 'true' to 'false' and
        //:    sets the length of the output buffer to 0.  (C-4)
        //:
        //:  5 Write a string to 'stdout' and read it back with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::compare' gives the correct results when the
        //:    captured output is compared with the following data:  (C-5)
        //:
        //:        Data                           Comparison Result
        //:    ------------                   -------------------------
        //:    input string                           true
        //:    input string with appended data        false
        //:    input string truncated                 false
        //:    string different from input:
        //:    at beginning                           false
        //:    at end                                 false
        //:    elsewhere                              false
        //:
        //:  6 Confirm that 'load' fails when there is more data in 'stdout'
        //:    than can be fit in the capture buffer.  Confirm that 'compare'
        //:    fails if 'load' has not been first called to read data into the
        //:    capture buffer.  (C-6)
        //:
        //:  7 Confirm that strings containing embedded newlines are correctly
        //:    captured and correctly identified by 'compare'.  (C-7)
        //:
        //:  8 Write an empty string to 'stdout'.  Confirm that it can be
        //:    correctly loaded and compared with the original.  (C-8)
        //:
        //:  9 Write a series of strings to 'stdout', containing '\0' at the
        //:    beginning, end or interior of the string.  Confirm that the
        //:    captured output can be correctly loaded and compared with the
        //:    original input.  (C-9)
        //:
        //: 10 Write a series of strings to 'stdout' containing '^D' and
        //:    '<CRLF>' and confirm that these strings are correctly captured
        //:    and loaded.  (C-10)
        //:
        //: 11 Use 'fstat' to find out the device and inode of the current
        //:    (post-redirection) 'stderr'.  Compare these values to the device
        //:    and inode of 'stdout' before redirection.  (C-11)
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING TEST APPARATUS"
                    "\n----------------------\n");
        }

        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT REDIRECTION"
                        "\n--------------------------\n");
            }

            // 1 Output is redirected
            ASSERT(-1 != ftell(stdout));
        }

        {
            // 2 Captured output is readable
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE"
                        "\n----------------------\n");
            }

            enum { TEST_STRING_SIZE = 15 };
            const char *testString = "This is output";
            char buffer[TEST_STRING_SIZE];

            ASSERT(TEST_STRING_SIZE == strlen(testString) + 1);

            rewind(stdout);
            long initialStdoutPosition = ftell(stdout);
            ASSERT(0 == initialStdoutPosition);
            printf("%s", testString);
            long finalStdoutPosition = ftell(stdout);
            ASSERT(-1 != finalStdoutPosition);
            ASSERT(finalStdoutPosition > initialStdoutPosition);
            long outputSize = finalStdoutPosition - initialStdoutPosition;
            ASSERT(outputSize + 1 == TEST_STRING_SIZE);
            rewind(stdout);
            size_t bytesWritten =
                fread(buffer, sizeof(char), outputSize, stdout);
            ASSERT(static_cast<long>(bytesWritten) == outputSize);
            buffer[TEST_STRING_SIZE - 1] = '\0';
            ASSERT(0 == strcmp(testString, buffer));
        }

        {
            // 3 'load' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE LOAD"
                        "\n---------------------------\n");
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stdout);
            printf("%s", testString);
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(false == output.isOutputReady());
            ASSERT(0 == output.outputSize());
            ASSERT(output.load());
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(true == output.isOutputReady());
            ASSERT(testStringLength == output.outputSize());
            ASSERT(0 == memcmp(testString,
                               output.getOutput(),
                               output.outputSize()));
        }

        {
            // 4 'reset' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE RESET"
                        "\n----------------------------\n");
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stdout);
            printf("%s", testString);
            output.load();
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(true == output.isOutputReady());
            ASSERT(testStringLength == output.outputSize());
            output.reset();
            ASSERT(0 == ftell(stdout));
            ASSERT(true != output.isOutputReady());
            ASSERT(0 == output.outputSize());
        }

        {
            // 5 'compare' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE VERIFICATION"
                        "\n-----------------------------------\n");
            }

            const char *testString            = "This is output";
            const char *longString            = "This is outputA";
            const char *shortString           = "This is outpu";
            const char *differentStartString  = "Xhis is output";
            const char *differentEndString    = "This is outpuy";
            const char *differentMiddleString = "This iz output";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(!!strcmp(testString, testString) ==
                       !!output.compare(testString));

            ASSERT(!!strcmp(testString, shortString) ==
                       !!output.compare(shortString));
            ASSERT(!!strcmp(testString, longString) ==
                       !!output.compare(longString));
            ASSERT(!!strcmp(testString, differentStartString) ==
                       !!output.compare(differentStartString));
            ASSERT(!!strcmp(testString, differentEndString) ==
                       !!output.compare(differentEndString));
            ASSERT(!!strcmp(testString, differentMiddleString) ==
                       !!output.compare(differentMiddleString));
        }

        {
            // 6 Incorrect output is correctly diagnosed
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE ERROR"
                        "\n----------------------------\n");
            }

            // Reset verbosity levels to suppress expected error output
            const bool tempVeryVerbose = veryVerbose;
            const bool tempVeryVeryVerbose = veryVeryVerbose;
            veryVerbose = false;
            veryVeryVerbose = false;

            const char *testString = "This is good output";

            output.reset();
            size_t stringLength = strlen(testString);
            for (int idx = 0; idx * stringLength < OUTPUT_BUFFER_SIZE; ++idx) {
                printf("%s", testString);
            }
            printf("%s", testString);
            ASSERT(!output.load());

            output.reset();
            printf("%s", testString);
            ASSERT(0 != output.compare(testString));

            output.reset();
            ASSERT(0 != output.compare("", 0));

            veryVerbose = tempVeryVerbose;
            veryVeryVerbose = tempVeryVeryVerbose;
        }

        {
            // 7 Embedded newlines work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMBEDDED NEWLINES"
                        "\n-------------------------\n");
            }

            const char *testString = "This has an\nembedded newline";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString));

            const char *twoNewlineTestString =
                "This has two\nembedded newlines\n";

            output.reset();
            printf("%s", twoNewlineTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(twoNewlineTestString));
        }

        {
            // 8 Empty output works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMPTY OUTPUT"
                        "\n--------------------\n");
            }

            const char *testString = "";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString));
        }

        {
            // 9 Embedded nulls work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMBEDDED NULLS"
                        "\n----------------------\n");
            }

            const char *testString1 = "abc\0def";
            const char *testString2 = "\0def";
            const char *testString3 = "abc\0";

            output.reset();
            fwrite(testString1, sizeof(char), 7, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString1, 7));

            output.reset();
            fwrite(testString2, sizeof(char), 4, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString2, 4));

            output.reset();
            fwrite(testString3, sizeof(char), 4, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString3, 4));
        }

        {
            // 10 Filesystem-dependent control sequences work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING CONTROL SEQUENCES"
                        "\n-------------------------\n");
            }

            const char *crnlTestString = "ab\r\ncd";
            const char *ctrlDTestString = "ab" "\x04" "cd";

            output.reset();
            printf("%s", crnlTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(crnlTestString));

            output.reset();
            printf("%s", ctrlDTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(ctrlDTestString));
        }

        {
            //: 11 stderr points to original target of stdout
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING STDERR REDIRECTION"
                        "\n--------------------------\n");
            }

            int newStderrFD = fileno(stderr);
            ASSERT(-1 != newStderrFD);
            struct stat stderrStat;
            stderrStat.st_dev = output.originalStdoutStat().st_dev;
            stderrStat.st_rdev = output.originalStdoutStat().st_rdev;
            ASSERT(-1 != fstat(newStderrFD, &stderrStat));
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
            // st_dev and st_rdev are not stable on Windows
            ASSERT(stderrStat.st_dev == output.originalStdoutStat().st_dev);
            ASSERT(stderrStat.st_rdev == output.originalStdoutStat().st_rdev);
#endif
            ASSERT(stderrStat.st_ino == output.originalStdoutStat().st_ino);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component.
        //
        // Plan:
        //   Invoke each of the "print" macros, protected by a check for
        //   'verbose' mode, and manually inspect the output to the console.
        //   Invoke each 'LOOPx_ASSERT' macro with a test that passes, and
        //   again with a test that fails only in verbose mode.  Then reset the
        //   'testStatus' count to reflect the expected number of failed
        //   'LOOPx_ASSERT's.
        //
        // Testing:
        //   BREATHING TEST
        //   BSLS_BSLTESTUTIL_P()
        //   BSLS_BSLTESTUTIL_P_()
        //   BSLS_BSLTESTUTIL_Q()
        //   BSLS_BSLTESTUTIL_T_
        //   BSLS_BSLTESTUTIL_L_
        //   BSLS_BSLTESTUTIL_LOOP_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP2_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP3_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP4_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP5_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP6_ASSERT
        // --------------------------------------------------------------------

        if (verbose) fprintf(stderr, "\nBREATHING TEST"
                                     "\n==============\n");

        const bool                b       = true;
        const char                c       = 'c';
        const signed char         sc      = -42;
        const unsigned char       uc      = 142;
        const short               ss      = -1234;
        const unsigned short      us      = 1234;
        const int                 si      = -123456;
        const unsigned int        ui      = 123456;
        const long                sl      = -7654321l;
        const unsigned long       ul      = 7654321ul;
        const long long           sll     = -5123467890ll;
        const unsigned long long  ull     = 9876543210ull;
        const float               f       = 2.78f;
        const double              d       = 3.14159268;
        const long double         ld      = 1.608e300L;
        char                      hello[] = { 'h', 'e', 'l', 'l', 'o', '\0' };
        char                     *s       = hello;
        const char                world[] = { 'w', 'o', 'r', 'l', 'd', '\0' };
        const char               *cs      = world;
        void                     *pv      = hello;
        const void               *pcv     = &si;

        // Check verbose so that we do not unexpectedly write to the console
        if (verbose) {
            BSLS_BSLTESTUTIL_P(b)
            BSLS_BSLTESTUTIL_P(c)
            BSLS_BSLTESTUTIL_P(sc)
            BSLS_BSLTESTUTIL_P(uc)
            BSLS_BSLTESTUTIL_P(ss)
            BSLS_BSLTESTUTIL_P(us)
            BSLS_BSLTESTUTIL_P(si)
            BSLS_BSLTESTUTIL_P(ui)
            BSLS_BSLTESTUTIL_P(sl)
            BSLS_BSLTESTUTIL_P(ul)
            BSLS_BSLTESTUTIL_P(sll)

            BSLS_BSLTESTUTIL_P(ull)
            BSLS_BSLTESTUTIL_P(f)
            BSLS_BSLTESTUTIL_P(d)
            BSLS_BSLTESTUTIL_P(ld)
            BSLS_BSLTESTUTIL_P(s)
            BSLS_BSLTESTUTIL_P(cs)
            BSLS_BSLTESTUTIL_P(pv)
            BSLS_BSLTESTUTIL_P(pcv)

            BSLS_BSLTESTUTIL_P_(b)
            BSLS_BSLTESTUTIL_P_(c)
            BSLS_BSLTESTUTIL_P_(sc)
            BSLS_BSLTESTUTIL_P_(uc)
            BSLS_BSLTESTUTIL_P_(ss)
            BSLS_BSLTESTUTIL_P_(us)
            BSLS_BSLTESTUTIL_P_(si)
            BSLS_BSLTESTUTIL_P_(ui)
            BSLS_BSLTESTUTIL_P_(sl)

            BSLS_BSLTESTUTIL_P_(ul)
            BSLS_BSLTESTUTIL_P_(sll)
            BSLS_BSLTESTUTIL_P_(ull)
            BSLS_BSLTESTUTIL_P_(f)
            BSLS_BSLTESTUTIL_P_(d)
            BSLS_BSLTESTUTIL_P_(ld)
            BSLS_BSLTESTUTIL_P_(s)
            BSLS_BSLTESTUTIL_P_(cs)
            BSLS_BSLTESTUTIL_P_(pv)

            BSLS_BSLTESTUTIL_P_(pcv)

            BSLS_BSLTESTUTIL_Q(BSLS_BSLTESTUTIL_L_)
            BSLS_BSLTESTUTIL_T_
            BSLS_BSLTESTUTIL_P(BSLS_BSLTESTUTIL_L_)
        }

        BSLS_BSLTESTUTIL_LOOP_ASSERT(b, true == b);
        BSLS_BSLTESTUTIL_LOOP2_ASSERT(c, sc, 'c' == c && -42 == sc);
        BSLS_BSLTESTUTIL_LOOP3_ASSERT(uc, ss, us,
                       142 == uc &&
                     -1234 == ss &&
                      1234 == us);
        BSLS_BSLTESTUTIL_LOOP4_ASSERT(si, ui, sl, ul,
                   -123456 == si &&
                    123456 == ui &&
                  -7654321 == sl &&
                   7654321 == ul);
        BSLS_BSLTESTUTIL_LOOP5_ASSERT(si, ui, sl, ul, sll,
                         -123456   == si &&
                          123456   == ui &&
                        -7654321l  == sl &&
                         7654321ul == ul &&
                     -5123467890ll == sll);
        BSLS_BSLTESTUTIL_LOOP6_ASSERT(si, ui, sl, ul, sll, ull,
                        -123456    == si  &&
                         123456    == ui  &&
                       -7654321l   == sl  &&
                        7654321ul  == ul  &&
                    -5123467890ll  == sll &&
                     9876543210ull == ull);

        ASSERT(testStatus == 0);

        if (verbose) {
            BSLS_BSLTESTUTIL_LOOP_ASSERT(verbose, !verbose);
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(verbose, c, !verbose);
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(verbose, uc, sc, !verbose);
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(verbose, f, d, ld, !verbose);
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(verbose, s, cs, pv, pcv, !verbose);
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(verbose,
                                          ss,
                                          us,
                                          sll,
                                          ull,
                                          si,
                                          !verbose);

            ASSERT(testStatus == 6);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CONCERN: 'printf' stack corruption test works
        //
        // Concerns:
        //   That the data corruption strategy used in test case 9 would detect
        //   corruption if the format specifier string under test were
        //   incorrect, i.e. that the 'data.target' and 'data.sentinel' values
        //   tested in test case 9 would actually trigger asserts.
        //
        // Plan:
        //   Use the same test strategy as the stack corruption test in test
        //   case 9, using format strings that are either smaller or larger
        //   than the type being printed.
        //
        // Testing:
        //   CONCERN: 'printf' stack corruption test works
        // --------------------------------------------------------------------
        if (verbose) fprintf(stderr,
                          "\tCONCERN: 'printf' stack corruption test works\n");

            checkStackCorruptionTest();
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        realTestStatus = -1;
      }
    }

    if (realTestStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", realTestStatus);
    }

    return realTestStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
