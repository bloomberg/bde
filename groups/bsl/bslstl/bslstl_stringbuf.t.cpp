// bslstl_stringbuf.t.cpp                                             -*-C++-*-
#include <bslstl_stringbuf.h>
#include <bslstl_string.h>
#include <bslstl_allocator.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstring>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// Testing the 'stringbuf' class is a little tricky because its implementation
// is in protected virtual methods.  First, we use a class derived from the
// 'stringbuf' so that it can access the protected members of 'stringbuf', and
// test 'stringbuf' methods through the derived class.  Second, we use public
// methods of 'stringbuf' to exercise the functionality implemented in the
// protected methods.  Those public methods are to create 'stringbuf', perform
// input from 'stringbuf' and perform output into 'stringbuf'.
// ============================================================================
// STRINGBUF:
// [ 2] stringbuf(const ALLOCATOR&)
// [ 2] stringbuf(ios_base::openmode, const ALLOCATOR&)
// [ 2] stringbuf(const string&, const ALLOCATOR&)
// [ 2] stringbuf(const string&, ios_base::openmode, const ALLOCATOR&)
// [ 3] seekoff(streamoff, ios_base::seekdir, ios_base::openmode)
// [ 4] seekpos(streempos, ios_base::openmode)
// [ 5] xsgetn(char *, streamsize)
// [ 6] underflow()
// [ 7] uflow()
// [ 8] pbackfail(int)
// [ 9] xsputn(const char *, streamsize)
// [10] overflow(int)
//-----------------------------------------------------------------------------
// [11] OUTPUT TO STRINGBUF VIA PUBLIC INTERFACE
// [12] INPUT FROM STRINGBUF VIA PUBLIC INTERFACE
// [13] INPUT/OUTPUT FROM/TO STRINGBUF VIA PUBLIC INTERFACE
// [14] USAGE EXAMPLE
// [ 1] BREATHING TEST

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

using std::printf;
using std::fflush;
using std::atoi;

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(expr)
#define ASSERT_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_PASS_RAW(expr)
#define ASSERT_SAFE_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(expr)
#define ASSERT_SAFE_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(expr)

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                printf("%s", #N ": "); dbg_print(N); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val);
    fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val);
    fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val);
    fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val);
    fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val);
    fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

//=============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class StringBufTest : public bsl::stringbuf
{
public:
    StringBufTest(bsl::string const & s, bsl::ios_base::openmode mode)
        : bsl::stringbuf(s, mode)
    {}

    void assertInputPosition(int line, std::streampos inputPos)
    {
        LOOP_ASSERT(line, eback() + inputPos == gptr());
        LOOP_ASSERT(line, egptr() - eback() > inputPos);
    }

    void assertOutputPosition(int line, std::streampos outputPos)
    {
        LOOP_ASSERT(line, pbase() + outputPos == pptr());
        LOOP_ASSERT(line, epptr() - pbase() > outputPos);
    }

    void assertPositions(int line,
                         std::streampos inputPos,
                         std::streampos outputPos)
    {
        LOOP_ASSERT(line, eback() == pbase());
        LOOP_ASSERT(line, egptr() <= epptr());
        assertInputPosition(line, inputPos);
        assertOutputPosition(line, outputPos);
    }

    static void testSeekoff()
    {
        const bsl::ios_base::openmode in = bsl::ios_base::in;
        const bsl::ios_base::openmode out = bsl::ios_base::out;
        const bsl::ios_base::seekdir beg = bsl::ios_base::beg;
        const bsl::ios_base::seekdir cur = bsl::ios_base::cur;
        const bsl::ios_base::seekdir end = bsl::ios_base::end;

        struct DataDef {
            int                      line;
            bsl::ios_base::seekdir   dir;
            bsl::ios_base::openmode  mode;
        } DATA[] = {
            { L_, beg, in       },
            { L_, beg, out      },
            { L_, beg, in | out },
            { L_, cur, in       },
            { L_, cur, out      },
            { L_, end, in       },
            { L_, end, out      },
            { L_, end, in | out },
        };

        const int dataSize = sizeof DATA / sizeof *DATA;
        bsl::string initialStr("initial");

        for (int i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::ios_base::seekdir DIR = DATA[i].dir;
            bsl::ios_base::openmode MODE = DATA[i].mode;

            for (std::size_t pos = 0; pos != initialStr.size(); ++pos) {
                StringBufTest strBuf(initialStr, MODE);

                std::streamoff seekOff
                    = DIR == end ? pos - initialStr.size() : pos;

                if (DIR == cur) {
                    strBuf.pubseekoff(0, beg, MODE);
                }

                strBuf.pubseekoff(seekOff, DIR, MODE);

                if (MODE == in) {
                    strBuf.assertInputPosition(LINE, pos);
                }
                else if (MODE == out) {
                    strBuf.assertOutputPosition(LINE, pos);
                }
                else if (MODE == (in | out)) {
                    strBuf.assertPositions(LINE, pos, pos);
                }
            }
        }
    }

    static void testSeekposImp(bsl::ios_base::openmode mode)
    {
        bsl::string initialStr("initial state");

        for (std::size_t pos = 0; pos != initialStr.size(); ++pos) {
            StringBufTest strBuf(initialStr, mode);

            strBuf.pubseekpos(pos, mode);

            if (mode == bsl::ios_base::in) {
                strBuf.assertInputPosition(L_, pos);
            }
            else if (mode == bsl::ios_base::out) {
                strBuf.assertOutputPosition(L_, pos);
            }
            else if (mode == (bsl::ios_base::in | bsl::ios_base::out)) {
                strBuf.assertPositions(L_, pos, pos);
            }
        }
    }

    static void testSeekpos()
    {
        testSeekposImp(bsl::ios_base::in);
        testSeekposImp(bsl::ios_base::out);
        testSeekposImp(bsl::ios_base::in | bsl::ios_base::out);
    }

    static void testXsgetn()
    {
        bsl::string initialStr("initial state");
        const std::size_t readbufSize = 50;
        char readbuf[readbufSize];

        // ensure that the read buffer is large enough
        ASSERT(readbufSize > initialStr.size());

        for (std::streamsize n = 0;
             n != std::streamsize(initialStr.size());
             ++n)
        {
            StringBufTest strBuf(initialStr, bsl::ios_base::in);

            // reset the read buffer
            std::memset(readbuf, -1, readbufSize);

            // read some characters with 'xsgetn'
            std::streamsize readChars = strBuf.xsgetn(readbuf, n);

            // verify that characters were read successfully
            ASSERT(readChars == n);
            ASSERT(strncmp(readbuf, initialStr.c_str(), n) == 0);
        }
    }

    static void testUnderflow()
    {
        struct DataDef {
            int line;
            const char * str;
        } DATA[] = {
            { L_, "A" },
            { L_, "AA" },
            { L_, "ABA" },
            { L_, "ABCA" },
            { L_, "ABCDA" },
            { L_, "ABCDEA" },
        };

        const std::size_t dataSize = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::string STR = DATA[i].str;

            StringBufTest strBuf(STR, bsl::ios_base::in);
            for (std::streamsize pos = 0;
                 pos <= std::streamsize(STR.size());
                 ++pos)
            {
                // read a character at the specified position
                strBuf.pubseekpos(pos, bsl::ios_base::in);
                int ic = strBuf.underflow();

                if (pos == std::streamsize(STR.size())) {
                    LOOP2_ASSERT(LINE, ic,
                                 ic == std::char_traits<char>::eof());
                }
                else {
                    LOOP2_ASSERT(LINE, ic, ic == STR[pos]);
                }

                // read again to make sure the current position didn't advance
                ic = strBuf.underflow();
                if (pos == std::streamsize(STR.size())) {
                    LOOP2_ASSERT(LINE, ic,
                                 ic == std::char_traits<char>::eof());
                }
                else {
                    LOOP2_ASSERT(LINE, ic, ic == STR[pos]);
                }
            }
        }
    }

    static void testUflow()
    {
        struct DataDef {
            int line;
            const char *str;
        } DATA[] = {
            { L_, "A" },
            { L_, "AA" },
            { L_, "ABA" },
            { L_, "ABCA" },
            { L_, "ABCDA" },
            { L_, "ABCDEA" },
        };

        const std::size_t dataSize = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::string STR = DATA[i].str;

            StringBufTest strBuf(STR, bsl::ios_base::in);
            for (std::streamsize pos = 0;
                 pos <= std::streamsize(STR.size());
                 ++pos)
            {
                // read each character sequentially
                int ic = strBuf.uflow();

                if (pos == std::streamsize(STR.size())) {
                    LOOP2_ASSERT(LINE, ic,
                                 ic == std::char_traits<char>::eof());
                }
                else {
                    LOOP2_ASSERT(LINE, ic, ic == STR[pos]);
                }
            }
        }
    }

    static void testPbackfail()
    {
        struct DataDef {
            int line;
            const char *str;
        } DATA[] = {
            { L_, "A" },
            { L_, "AA" },
            { L_, "ABA" },
            { L_, "ABCA" },
            { L_, "ABCDA" },
            { L_, "ABCDEA" },
        };

        const std::size_t dataSize = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::string STR = DATA[i].str;

            StringBufTest strBuf(STR, bsl::ios_base::in);

            // trying to put back EOF gives EOF
            LOOP_ASSERT(LINE,
                        strBuf.pbackfail() == std::char_traits<char>::eof());

            // put back at the start of the buffer gives EOF
            LOOP_ASSERT(LINE, strBuf.pbackfail('0')
                                             == std::char_traits<char>::eof());

            // now read and put back characters one by one in a loop
            for (std::streamsize pos = 0;
                 pos <= std::streamsize(STR.size());
                 ++pos)
            {
                // read a character
                int ic = strBuf.uflow();

                if (ic != std::char_traits<char>::eof()) {
                    // put the character back
                    int bc = strBuf.pbackfail(ic);
                    LOOP4_ASSERT(LINE, pos, ic, bc, ic == bc);

                    // read the character again
                    ic = strBuf.uflow();
                    LOOP4_ASSERT(LINE, pos, ic, bc, ic == bc);
                }
                else {
                    // pbackfail at the end, returns the last character
                    int bc = strBuf.pbackfail();
                    LOOP3_ASSERT(LINE, pos, bc,
                                 bc != std::char_traits<char>::eof());
                }
            }
        }
    }

    static void testXsputn()
    {
        bsl::string writeStr("111");

        // writing to readonly buffer fails
        StringBufTest readonlyBuf("", bsl::ios_base::in);
        ASSERT(readonlyBuf.xsputn(writeStr.c_str(), writeStr.size()) == 0);

        struct DataDef {
            int line;
            const char *str;
        } DATA[] = {
            { L_, "A" },
            { L_, "AA" },
            { L_, "ABA" },
            { L_, "ABCA" },
            { L_, "ABCDA" },
            { L_, "ABCDEA" },
        };

        const std::size_t dataSize = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::string STR(DATA[i].str);

            for (std::size_t pos = 0; pos <= STR.size(); ++pos) {
                StringBufTest strBuf(STR, bsl::ios_base::out);

                // write characters at the specified position
                strBuf.pubseekpos(pos, bsl::ios_base::out);
                LOOP_ASSERT(LINE,
                            strBuf.xsputn(writeStr.c_str(), writeStr.size())
                                          == std::streamsize(writeStr.size()));

                // verify the result
                bsl::string copyStr(STR.begin(), STR.begin() + pos);
                copyStr.append(writeStr.begin(), writeStr.end());
                if (STR.size() > pos + writeStr.size()) {
                    copyStr.append(STR.begin() + pos + writeStr.size(),
                                   STR.end());
                }

                LOOP3_ASSERT(LINE, strBuf.str().c_str(), copyStr.c_str(),
                             strBuf.str() == copyStr);
            }
        }
    }

    static void testOverflow()
    {
        // writing to readonly buffer fails
        StringBufTest readonlyBuf("", bsl::ios_base::in);
        ASSERT(readonlyBuf.overflow('a') == std::char_traits<char>::eof());

        // writing EOF does nothing
        StringBufTest emptyBuf("", bsl::ios_base::out);
        ASSERT(emptyBuf.overflow() != std::char_traits<char>::eof());
        ASSERT(emptyBuf.str().size() == 0);

        struct DataDef {
            int line;
            const char *str;
        } DATA[] = {
            { L_, "A" },
            { L_, "AA" },
            { L_, "ABA" },
            { L_, "ABCA" },
            { L_, "ABCDA" },
            { L_, "ABCDEA" },
        };

        const std::size_t dataSize = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != dataSize; ++i) {
            int LINE = DATA[i].line;
            bsl::string STR(DATA[i].str);

            const char writeCh = '1';

            for (std::size_t pos = 0; pos <= STR.size(); ++pos) {
                StringBufTest strBuf(STR, bsl::ios_base::out);

                // write a character at the specified position
                strBuf.pubseekpos(pos, bsl::ios_base::out);
                LOOP_ASSERT(LINE, strBuf.overflow(writeCh) == writeCh);

                // verify the result
                bsl::string copyStr(STR);
                if (pos == STR.size()) {
                    copyStr.push_back(writeCh);
                }
                else {
                    copyStr[pos] = writeCh;
                }

                LOOP3_ASSERT(LINE, strBuf.str().c_str(), copyStr.c_str(),
                             strBuf.str() == copyStr);
            }
        }
    }
};

template <class SeekFunc>
void testPutCharInTheMiddle(SeekFunc seekpos)
{
    bsl::stringbuf buf("abcde");

    std::streamoff res = seekpos(buf);
    ASSERT(res != -1);

    res = buf.sputc('3');
    ASSERT(res == '3');

    ASSERT(buf.str() == "ab3de");
}

template <class SeekFunc>
void testPutCharsInTheMiddle(SeekFunc seekpos)
{
    bsl::stringbuf buf("abcde");
    bsl::string what("34");

    std::streamoff res1 = seekpos(buf);
    ASSERT(res1 != -1);

    std::streamsize res2 = buf.sputn(what.data(), what.size());
    ASSERT(res2 == std::streamsize(what.size()));

    ASSERT(buf.str() == "ab34e");
}

std::streampos bufPubseekpos(bsl::stringbuf& buf)
{
    return buf.pubseekpos(2);
}

std::streampos bufPubseekoffBeg(bsl::stringbuf & buf)
{
    return buf.pubseekoff(2, std::ios_base::beg);
}

std::streampos bufPubseekoffBegOut(bsl::stringbuf & buf)
{
    return buf.pubseekoff(2, std::ios_base::beg, std::ios_base::out);
}

std::streampos bufPubseekoffBegCur(bsl::stringbuf & buf)
{
    buf.pubseekoff(0, std::ios_base::beg);
    return buf.pubseekoff(2, std::ios_base::cur, std::ios_base::out);
}

std::streampos bufPubseekoffEnd(bsl::stringbuf & buf)
{
    return buf.pubseekoff(-3, std::ios_base::end);
}

}  // close unnamed namespace

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Operations
///- - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::stringbuf' to read and
// write character data from and to a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from 'unsigned int' to
// 'bsl::string' and back.  First, we define the prototypes of two conversion
// functions:
//..
//  bsl::string  toString(unsigned int from);
//  unsigned int fromString(const bsl::string& from);
//..
// Then, we use 'bsl::stringbuf' to implement the 'toString' function.  We
// write all digits into 'bsl::stringbuf' individually using 'sputc' methods
// and then return the resulting 'bsl::string' object:
//..
//  #include <algorithm>
//
    bsl::string toString(unsigned int from)
    {
        bsl::stringbuf out;

        for (; from != 0; from /= 10) {
            out.sputc(char('0' + from % 10));
        }

        bsl::string result(out.str());
        std::reverse(result.begin(), result.end());
        return result;
    }
//..
// Now, we implement the 'fromString' function that converts from
// 'bsl::string' to 'unsigned int' by using 'bsl::stringbuf' to read individual
// digits from the string object:
//..
    unsigned int fromString(const bsl::string& from)
    {
        unsigned int result = 0;

        for (bsl::stringbuf in(from); in.in_avail(); ) {
            result = result * 10 + (in.sbumpc() - '0');
        }

        return result;
    }
//..

}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    int test = argc > 1 ? atoi(argv[1]) : 0;

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

    bslma::TestAllocator *globalAllocator_p,
                         *defaultAllocator_p,
                         *objectAllocator_p;

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;
    (void) objectAllocator_p;

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;
    (void) defaultAllocator_p;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                          veryVeryVeryVerbose);
    bslma::Allocator *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;
    (void) globalAllocator_p;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Usage example compiles and executes without failures.
        //
        // Plan:
        //: 1 Copy and paste the usage example from the component header into
        //:   the test driver and replace 'assert' with 'ASSERT'.
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we verify that the result of the round-trip conversion is identical
// to the original value:
//..
    unsigned int orig   = 92872498;
    unsigned int result = fromString(toString(orig));
//
    ASSERT(orig == result);
//..

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING INPUT/OUTPUT FROM/TO STRINGBUF VIA PUBLIC INTERFACE
        //
        // Concerns:
        //: 1 'str' setter correctly updates both the input and output stream
        //    positions.
        //
        // Plan:
        //: 1 Ensure that successive calls to 'sbumpc' returns the expected
        //:   characters, based on an initial string provided at construction.
        //: 2 Seek the output position to the end of the string, and then
        //:   replace the old string with a new, shorter string.
        //: 3 Ensure that successive calls to 'sbumpc' now returns the expected
        //:   characters, based on the new string.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INPUT/OUTPUT FROM/TO STRINGBUF"
                            "\n======================================\n");

        if (veryVerbose) printf("\ttesting sbumpc after seekoff and str\n");
        {
            bsl::stringbuf buf("abc");

            int res = buf.sbumpc();
            ASSERT(res == 'a');

            res = buf.sbumpc();
            ASSERT(res == 'b');

            res = buf.sbumpc();
            ASSERT(res == 'c');

            res = buf.sbumpc();
            ASSERT(res == EOF);

            buf.pubseekoff(0, bsl::ios_base::end, bsl::ios_base::out);
            buf.str("1");

            res = buf.sbumpc();
            ASSERT(res == '1');

            res = buf.sbumpc();
            ASSERT(res == EOF);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INPUT FROM STRINGBUF VIA PUBLIC INTERFACE
        //
        // Concerns:
        //: 1 'in_avail' returns the correct number of available characters
        //:   for read from 'stringbuf'.
        //: 2 'snextc' advances the input position and then reads the
        //:   character at that position from 'stringbuf'.
        //: 3 'sbumpc' reads the character at the current input position and
        //:   then advances the input position.
        //: 4 'sgetc' reads the character at the current input position.
        //: 5 'sgetn' reads the specified number of characters from the
        //:   'stringbuf' and advances the input position to the number of
        //:   read characters.
        //: 6 'sungetc' puts the previously read character back into the
        //:   'stringbuf'.
        //: 7 'sputbackc' puts the specified character back into the
        //:   'stringbuf'.
        //
        // Plan:
        //: 1 Call 'in_avail' on an empty 'stringbuf', 'stringbuf' with the
        //:   initial string, 'stringbuf' with the initial string but with
        //:   input pointer at the end, and verify that the returned number of
        //:   available characters for read is correct.
        //: 2 Read a character from 'stringbuf' with 'snextc' and verify that
        //:   the character is correct and the 'stringbuf' input position is
        //:   updated accordingly.
        //: 3 Read a character from 'stringbuf' with 'sbumpc' and verify that
        //:   the character is correct and the 'stringbuf' input position is
        //:   updated accordingly.
        //: 4 Read a character from 'stringbuf' with 'sgetc' and verify that
        //:   the character is correct and the 'stringbuf' input position is
        //:   updated accordingly.
        //: 5 Read a character from 'stringbuf' with 'sgetc' after resetting
        //:   the state of 'stringbuf' with 'str' and verify that the
        //:   character is correct and the 'stringbuf' input position is
        //:   updated accordingly.
        //: 6 Read characters from 'stringbuf' with 'sgetn' and verify that
        //:   the character is correct and the 'stringbuf' input position is
        //:   updated accordingly.
        //: 7 Read a character from 'stringbuf', then use 'sungetc' to put a
        //:   previously read character back into 'stringbuf' and verify that
        //:   the character gets put back correctly.
        //: 8 Read a character from 'stringbuf', then use 'sputbackc' to put a
        //:   a character back into 'stringbuf' and verify that the character
        //:   gets put back correctly.
        //: 9 Use 'sputbackc' to put a character back into 'stringbuf' in a
        //:   way that invokes 'pbackfail' and verify that the character gets
        //:   put back correctly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INPUT FROM STRINGBUF"
                            "\n============================\n");

        if (veryVerbose) printf("\ttesting xsgetn\n");

        StringBufTest::testXsgetn();

        if (veryVerbose) printf("\ttesting underflow\n");

        StringBufTest::testUnderflow();

        if (veryVerbose) printf("\ttesting uflow\n");

        StringBufTest::testUflow();

        if (veryVerbose) printf("\ttesting pbackfail\n");

        StringBufTest::testPbackfail();

        if (veryVerbose) printf("\ttesting overflow\n");

        StringBufTest::testOverflow();

        if (veryVerbose) printf("\ttesting xsputn\n");

        StringBufTest::testXsputn();

        if (veryVerbose) printf("\ttesting in_avail\n");

        {
            bsl::stringbuf buf1;
            std::streamsize res1 = buf1.in_avail();
            ASSERT(res1 == 0);

            bsl::string str("abc");
            bsl::stringbuf buf2(str);
            std::streamsize res2 = buf2.in_avail();
            ASSERT(res2 == std::streamsize(str.size()));

            bsl::stringbuf buf3(str);
            buf3.pubseekpos(3);
            std::streamsize res3 = buf3.in_avail();
            ASSERT(res3 == 0);
        }

        if (veryVerbose) printf("\ttesting snextc\n");

        {
            bsl::stringbuf buf1;
            int res1 = buf1.snextc();
            ASSERT(res1 == EOF);

            bsl::stringbuf buf2("ab");
            int res2 = buf2.snextc();
            ASSERT(res2 == 'b');

            res2 = buf2.snextc();
            ASSERT(res2 == EOF);
        }

        if (veryVerbose) printf("\ttesting sbumpc\n");

        {
            bsl::stringbuf buf1;
            int res1 = buf1.sbumpc();
            ASSERT(res1 == EOF);

            bsl::stringbuf buf2("ab");

            int res2 = buf2.sbumpc();
            ASSERT(res2 == 'a');

            res2 = buf2.sbumpc();
            ASSERT(res2 == 'b');

            res2 = buf2.sbumpc();
            ASSERT(res2 == EOF);
        }

        if (veryVerbose) printf("\ttesting sgetc\n");

        {
            bsl::stringbuf buf1;
            int res1 = buf1.sgetc();
            ASSERT(res1 == EOF);

            bsl::stringbuf buf2("abc");
            int res2 = buf2.sgetc();
            ASSERT(res2 == 'a');

            buf2.pubseekpos(2);
            res2 = buf2.sgetc();
            ASSERT(res2 == 'c');

            buf2.sbumpc();
            res2 = buf2.sgetc();
            ASSERT(res2 == EOF);
        }

        if (veryVerbose) printf("\ttesting sgetc after str\n");

        {
            bsl::stringbuf buf("abc");
            buf.sgetc();

            buf.str("123");
            int res = buf.sgetc();
            ASSERT(res == '1');
        }

        if (veryVerbose) printf("\ttesting sgetn\n");

        {
            std::size_t const read_buf_size = 3;
            char read_buf[read_buf_size];

            bsl::stringbuf buf1;
            std::streamsize res1 = buf1.sgetn(read_buf, read_buf_size);
            ASSERT(res1 == 0);

            bsl::stringbuf buf2("abc");
            std::streamsize res2 = buf2.sgetn(read_buf, read_buf_size);
            ASSERT(res2 == std::streamsize(read_buf_size));
            ASSERT(std::strncmp(read_buf, "abc", 3) == 0);

            bsl::stringbuf buf3("de");
            std::streamsize res3 = buf3.sgetn(read_buf, read_buf_size);
            ASSERT(res3 == 2);
            ASSERT(std::strncmp(read_buf, "dec", 3) == 0);
                // 'c' left from the previous sgetn
        }

        if (veryVerbose) printf("\ttesting sungetc\n");

        {
            bsl::stringbuf buf1;
            int res1 = buf1.sungetc();
            ASSERT(res1 == EOF);

            bsl::stringbuf buf2("ab");
            buf2.sbumpc();
            int res2 = buf2.sungetc();
            ASSERT(res2 == 'a');

            res2 = buf2.sbumpc();
            ASSERT(res2 == 'a');

            bsl::stringbuf buf3("abc");
            buf3.pubseekpos(3);
            int res3 = buf3.sungetc();
            ASSERT(res3 == 'c');
        }

        if (veryVerbose) printf("\ttesting sputbackc\n");

        {
            bsl::stringbuf buf("abc");
            int c = buf.sbumpc();
            ASSERT(c == 'a');

            int res = buf.sputbackc(char(c));
            ASSERT(res == c);

            c = buf.sbumpc();
            ASSERT(c == res);

            buf.pubseekpos(3);
            res = buf.sputbackc('c');
            ASSERT(res == 'c');

            c = buf.sbumpc();
            ASSERT(c == res);
        }

        if (veryVerbose) printf("\ttesting sputbackc with pbackfail\n");

        {
            bsl::stringbuf bufEmpty;
            int resEmpty = bufEmpty.sputbackc('a');
            ASSERT(resEmpty == EOF);

            bsl::stringbuf buf("abc");
            int c = buf.sbumpc();
            ASSERT(c == 'a');

            int res = buf.sputbackc('1');
            ASSERT(res == '1');

            c = buf.sbumpc();
            ASSERT(c == res);

            buf.pubseekpos(3);
            res = buf.sputbackc('3');
            ASSERT(res == '3');

            c = buf.sbumpc();
            ASSERT(c == res);

            bsl::stringbuf bufReadonly("abc", std::ios_base::in);
            bufReadonly.sbumpc();

            int resReadonly = bufReadonly.sputbackc('1');
            ASSERT(resReadonly == EOF);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT TO STRINGBUF VIA PUBLIC INTERFACE
        //
        // Concerns:
        //: 1 'sputc' writes the specified character at the current output
        //:   position and advances the output position.
        //: 2 'sputn' writes the specified characters at the current output
        //:   position and advances the output position to the number of
        //:   characters written.
        //: 3 'sputc' fails to write the character into a readonly input
        //:   'stringbuf'.
        //: 4 'sputn' fails to write characters into a readonly input
        //:   'stringbuf'.
        //
        // Plan:
        //: 1 Use 'sputc' to output a character and verify that it's written
        //:   correctly by using the 'stringbuf::str' method.
        //: 2 Use 'sputn' to output characters and verify that it's written
        //:   correctly by using the 'stringbuf::str' method.
        //: 3 Change 'stringbuf' position with 'pubseekpos' and 'pubseekoff'
        //:   and use 'sputc' to insert a character in the middle and verify
        //:   that it's written correctly by using the 'stringbuf::str' method.
        //: 4 Change 'stringbuf' position with 'pubseekpos' and 'pubseekoff'
        //:   and use 'sputn' to insert characters in the middle and verify
        //:   that it's written correctly by using the 'stringbuf::str' method.
        //: 5 Append a character to 'stringbuf' that already has some output
        //:   with 'sputc'.
        //: 6 Append characters to 'stringbuf' that already has some output
        //:   with 'sputn'.
        //: 7 Output a character into a readonly (std::ios_base::in)
        //:   'stringbuf' with 'sputc' and verify that the output fails and
        //:   the state of 'stringbuf' doesn't change.
        //: 8 Output characters into a readonly (std::ios_base::in)
        //:   'stringbuf' with 'sputn' and verify that the output fails and
        //:   the state of 'stringbuf' doesn't change.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OUTPUT TO STRINGBUF"
                            "\n===========================\n");

        if (veryVerbose) printf("\ttesting sputc\n");

        {
            bsl::stringbuf buf;
            int res = buf.sputc('a');

            ASSERT(res == 'a');
            ASSERT(buf.str() == "a");
        }

        if (veryVerbose) printf("\ttesting sputn\n");

        {
            bsl::stringbuf buf;
            bsl::string what("abcde");
            std::streamsize res = buf.sputn(what.data(), what.size());

            ASSERT(res == std::streamsize(what.size()));
            ASSERT(buf.str() == what);
        }

        if (veryVerbose) printf("\ttesting sputc with seekpos/seekoff\n");

        testPutCharInTheMiddle(bufPubseekpos);
        testPutCharInTheMiddle(bufPubseekoffBeg);
        testPutCharInTheMiddle(bufPubseekoffBegOut);
        testPutCharInTheMiddle(bufPubseekoffBegCur);
        testPutCharInTheMiddle(bufPubseekoffEnd);

        testPutCharsInTheMiddle(bufPubseekpos);
        testPutCharsInTheMiddle(bufPubseekoffBeg);
        testPutCharsInTheMiddle(bufPubseekoffBegOut);
        testPutCharsInTheMiddle(bufPubseekoffBegCur);
        testPutCharsInTheMiddle(bufPubseekoffEnd);

        if (veryVerbose) printf("\ttesting sputc/sputn with append\n");

        {
            bsl::stringbuf buf("abcde");
            bsl::string what("123");
            buf.pubseekpos(3);
            std::streamsize res = buf.sputn(what.data(), what.size());

            ASSERT(res == std::streamsize(what.size()));
            ASSERT(buf.str() == "abc123");
        }

        {
            bsl::stringbuf buf;
            buf.sputc('1');
            ASSERT(buf.str() == "1");

            buf.str("another");

            int res = buf.sputc('s');
            ASSERT(res == 's');
            ASSERT(buf.str() == "snother");
        }

        if (veryVerbose)
            printf("\ttesting sputc/sputn to readonly stringbuf\n");

        {
            bsl::stringbuf buf(std::ios_base::in);

            std::streamsize res = buf.sputc('1');
            ASSERT(res == EOF);
        }

        {
            bsl::stringbuf buf(std::ios_base::in);

            std::streamsize res = buf.sputn("1", 1);
            ASSERT(res == 0);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING OVERFLOW FUNCTION
        //
        // Concerns:
        //: 1 'overflow' writes the specified character into an output
        //:   'stringbuf' object.
        //: 2 'overflow' fails to perform the write into an input 'stringbuf'
        //:   object.
        //
        // Plan:
        //: 1 Create an input 'stringbuf' object and use 'overflow' to write
        //:   a character into it.  Verify that 'overflow' with an input
        //:   'stringbuf' fails.
        //: 2 Create an output 'stringbuf' with an initial string, change
        //:   the current output position to point in the middle of the
        //:   'stringbuf'.  Then use 'overflow' to write a character into the
        //:   'stringbuf' and verify that the final state of the 'stringbuf'
        //:   obtained using the 'str' method matches the expected result.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OVERFLOW FUNCTION"
                            "\n=========================\n");

        StringBufTest::testOverflow();

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING XSPUTN FUNCTION
        //
        // Concerns:
        //: 1 'xsputn' writes the specified number of characters into an output
        //:   'stringbuf' object and returned the correct number of written
        //:   characters.
        //: 2 'xsputn' fails to perform the write into an input 'stringbuf'
        //:   object.
        //
        // Plan:
        //: 1 Create an input 'stringbuf' object and use 'xsputn' to write
        //:   characters into it.  Verify that 'xsputn' with an input
        //:   'stringbuf' fails.
        //: 2 Create an output 'stringbuf' with an initial string, change
        //:   the current output position to point in the middle of the
        //:   'stringbuf'.  Then use 'xsputn' to write characters into the
        //:   'stringbuf' and verify that the final state of the 'stringbuf'
        //:   obtained using the 'str' method matches the expected result.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING XSPUTN FUNCTION"
                            "\n=======================\n");

        StringBufTest::testXsputn();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PBACKFAIL FUNCTION
        //
        // Concerns:
        //: 1 Putting back 'EOF' results in error ('pbackfail' returns 'EOF').
        //: 2 Putting back any character at the beginning of the 'stringbuf'
        //:   results in error.
        //: 3 Putting back a character that was previously read from a
        //:   'stringbuf' object results in that character reinserted into
        //:   'stringbuf' and the current input position of 'stringbuf' updated
        //:   to point to the previous character.
        //
        // Plan:
        //: 1 Create a 'stringbuf' object with an initial string.
        //: 2 Use 'pbackfail' to put back 'EOF' and verify that it results
        //:   in error.
        //: 3 Use 'pbackfail' to put back some character at the beginning of
        //:   'stringbuf' and verify that it results in error.
        //: 4 Walk through the 'stringbuf' reading each character one by one
        //:   with 'uflow' and then reinserting it back with 'pbackfail'
        //:   and verifying that 'pbackfail' undoes the effect of 'uflow', i.e.
        //:   it puts the character back into 'stringbuf' and sets the current
        //:   input position to point to that character.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PBACKFAIL FUNCTION"
                            "\n==========================\n");

        StringBufTest::testPbackfail();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING UFLOW FUNCTION
        //
        // Concerns:
        //: 1 'uflow' reads a character from an input 'stringbuf' and advances
        //:   the current input pointer.
        //
        // Plans:
        //: 1 Create an input 'stringbuf' object with some initial string.
        //: 2 For a range of positions from 0 to the size of the 'stringbuf'
        //:   object, change the current input position of the 'stringbuf',
        //:   and then read a character from that position using the
        //:   'uflow' method.
        //: 3 Verify that the character read with 'uflow' is the same as
        //:   the one from the initial string at that position.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING UFLOW FUNCTION"
                            "\n======================\n");

        StringBufTest::testUflow();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING UNDERFLOW FUNCTION
        //
        // Concerns:
        //: 1 'underflow' reads a character from an input 'stringbuf' without
        //:   advancing the current input pointer.
        //
        // Plans:
        //: 1 Create an input 'stringbuf' object with some initial string.
        //: 2 For a range of positions from 0 to the size of the 'stringbuf'
        //:   object, change the current input position of the 'stringbuf',
        //:   and then read a character from that position using the
        //:   'underflow' method.
        //: 3 Verify that the character read with 'underflow' is the same as
        //:   the one from the initial string at that position.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING UNDERFLOW FUNCTION"
                            "\n==========================\n");

        StringBufTest::testUnderflow();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING XSGETN FUNCTION
        //
        // Concerns:
        //: 1 'xsgetn' reads the requested number of characters from the input
        //:   'strinbuf' object.
        //
        // Plans:
        //: 1 Create an input 'stringbuf' object with an initial string.
        //: 2 Read characters from the 'stringbuf' object of various length.
        //: 3 Verify the characters read with 'xsgetn' against the initial
        //:   'stringbuf' string object.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING XSGETN FUNCTION"
                            "\n=======================\n");

        StringBufTest::testXsgetn();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SEEKPOS FUNCTION
        //
        // Concerns:
        //: 1 'seekpos' can change either input, output or both 'stringbuf'
        //:   position of the appropriate (input or output) 'stringbuf' object
        //:   in the range of valid positions.
        //
        // Plan:
        //: 1 Create a 'stringbuf' object of either input, output or
        //:   input/output type with an initial string of varying length.
        //: 2 Change input, output or both positions in the stringbuf for
        //:   the range of positions from 0 to the end of the 'stringbuf'.
        //: 3 Verify that the position was changed correctly using 'gptr'
        //:   and 'pptr' functions.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SEEKPOS FUNCTION"
                            "\n========================\n");

        StringBufTest::testSeekpos();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SEEKOFF FUNCTION
        //
        // Concerns:
        //: 1 'seekoff' can change either input, output or both stringbuf
        //:   position starting from the beginning of the buffer.
        //: 2 'seekoff' can change either input, output or both stringbuf
        //:   position starting from the end of the buffer.
        //: 3 'seekoff' can change both input and output stringbuf
        //:   positions starting from the current position into the buffer.
        //: 4 'seekoff' can change the stringbuf position for either input,
        //:    output or input/output stringbuf.
        //
        // Plan:
        //: 1 Create a 'stringbuf' object of either input, output or
        //:   input/output type with an initial string of varying length.
        //: 2 Change input, output or both positions in the stringbuf for
        //:   the range of positions from 0 to the end of the 'stringbuf'.
        //: 3 Verify that the position was changed correctly using 'gptr'
        //:   and 'pptr' functions.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SEEKOFF FUNCTION"
                            "\n========================\n");

        StringBufTest::testSeekoff();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1. stringbuf is creatable with the default constructor
        //: 2. stringbuf is creatable with constructor taking the input/output
        //:    mode
        //: 3. stringbuf is creatable with constructor taking the initial
        //:    string
        //: 4. stringbuf is creatable with constructors taking an allocator
        //
        // Plan:
        //: 1. create stringbuf object with the default constructor
        //: 2. create stringbuf objects with constructors taking the
        //:    input/output mode
        //: 3. create stringbuf object with constructor taking the initial
        //:    string
        //: 4. create stringbuf objects with constructors taking an allocator
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CREATORS"
                            "\n================\n");

        if (veryVerbose) printf("\tdefault stringbuf constructor\n");

        {
            bsl::stringbuf buf;
        }

        if (veryVerbose)
            printf("\tstringbuf constructors with input/output mode\n");

        {
            bsl::stringbuf in_buf(std::ios_base::in);
            bsl::stringbuf out_buf(std::ios_base::out);
            bsl::stringbuf inout_buf;
            bsl::stringbuf inout_buf2(std::ios_base::in | std::ios_base::out);
        }

        if (veryVerbose)
            printf("\tstringbuf constructor with initial string\n");

        {
            bsl::stringbuf buf(bsl::string("something"));
            ASSERT(buf.str() == "something");
        }

        if (veryVerbose)
            printf("\tstringbuf constructors with allocator\n");

        {
            bsl::stringbuf buf1(std::ios_base::in, bsl::allocator<char>());
            bsl::stringbuf buf2(std::ios_base::in,
                                bslma::Default::allocator());
            bsl::stringbuf buf3(bsl::string("something"),
                                std::ios_base::in,
                                bslma::Default::allocator());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise the basic functionality.
        //
        // Plan:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
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
