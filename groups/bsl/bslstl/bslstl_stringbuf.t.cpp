// bslstl_stringbuf.t.cpp                                             -*-C++-*-
#include <bslstl_stringbuf.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_stdallocator.h>
#include <bslma_stdtestallocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bsltf_stdstatefulallocator.h>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

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
// [ 2] stringbuf(const SALLOCSTR&, const ALLOCATOR&)
// [ 2] stringbuf(const SALLOCSTR&, ios_base::openmode, const ALLOCATOR&)
// [ 4] stringbuf(stringbuf&&)
// [17] stringbuf(stringbuf&&, const ALLOCATOR&);
// [ 4] operator=(stringbuf&&)
// [ 5] seekoff(streamoff, ios_base::seekdir, ios_base::openmode)
// [ 6] seekpos(streempos, ios_base::openmode)
// [ 7] xsgetn(char *, streamsize)
// [ 8] underflow()
// [ 9] uflow()
// [10] pbackfail(int)
// [11] xsputn(const char *, streamsize)
// [12] overflow(int)
// [ 3] allocator_type get_allocator() const;
// [19] void str(const StringType& value);
// [19] void str(BloombergLP::bslmf::MovableRef<StringType> value);
// [19] void str(const basic_string<CHAR, TRAITS, SALLOC>& value);
// [ 3] StringType str() const;
// [ 3] StringType str() &&;
// [ 3] StringType str(const SALLOC&);
// [ 3] ViewType view() const;
// [18] void swap(basic_stringbuf& other);
//
// FREE FUNCTIONS
// [18] void swap(basic_stringbuf& a, basic_stringbuf& b);
//-----------------------------------------------------------------------------
// [13] OUTPUT TO STRINGBUF VIA PUBLIC INTERFACE
// [14] INPUT FROM STRINGBUF VIA PUBLIC INTERFACE
// [15] INPUT/OUTPUT FROM/TO STRINGBUF VIA PUBLIC INTERFACE
// [20] USAGE EXAMPLE
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
const char VF = 'F';
const char VG = 'G';
const char VH = 'H';
const char VI = 'I';
const char VJ = 'J';
const char VK = 'K';
const char VL = 'L';
    // All test types have character value type.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef std::ios_base       IosBase;
typedef IosBase::openmode   Mode;

// Define the length of a 'bsl::string' value long enough to ensure dynamic
// memory allocation.

const int LENGTH_OF_SUFFICIENTLY_LONG_STRING =
#ifdef BSLS_PLATFORM_CPU_32_BIT
                                               33;
#else                 // 64_BIT
                                               65;
#endif

BSLMF_ASSERT(LENGTH_OF_SUFFICIENTLY_LONG_STRING >
                                        static_cast<int>(sizeof(bsl::string)));

struct StrlenDataRow {
    int  d_line;    // source line number
    int  d_length;  // string length
    char d_mem;     // expected allocation: 'Y', 'N', '?'
};

static
const StrlenDataRow STRLEN_DATA[] =
{
    //LINE  LENGTH                              MEM
    //----  ----------------------------------  ---
    { L_,   0,                                  'N'   },
    { L_,   1,                                  'N'   },
    { L_,   LENGTH_OF_SUFFICIENTLY_LONG_STRING, 'Y'   }
};

const int NUM_STRLEN_DATA = sizeof STRLEN_DATA / sizeof *STRLEN_DATA;

//=============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace {

template <class STRING>
void loadString(STRING *value, int length)
    // Load into the specified 'value' a character string having the specified
    // 'length'.  The behavior is undefined unless 'length >= 0'.
{
    value->resize(length);

    for (int i = 0; i < length; ++i) {
        (*value)[i] =
                     static_cast<typename STRING::value_type>('a' + (i % 26));
    }
}

template <class StringT>
bool stringCouldBeMovedFrom(const StringT&                          s,
                            const typename StringT::allocator_type& otherAlloc)
    // Return 'true' if the specified string 's' could be moved to another
    // string that uses the specified allocator 'otherAlloc', and 'false'
    // otherwise.
{
    return s.size() >= LENGTH_OF_SUFFICIENTLY_LONG_STRING &&
           s.get_allocator() == otherAlloc;
}

template <class StringT>
bool stringWasMovedFrom(const StringT& s)
    // Return 'true' if the specified string 's' was possibly moved from, and
    // 'false' otherwise.
{
    return 0 == s.size();
}

template <class TYPE>
class StringBufTest : public bsl::basic_stringbuf<TYPE>
{
    // PRIVATE TYPES
    typedef bsl::basic_stringbuf<TYPE> Base;  // parent class alias
  public:
    // CLASS METHODS
    static int getValues(const TYPE **values)
    {
        bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

        const int NUM_VALUES = 12;
        static const TYPE initValues[NUM_VALUES] = { // avoid 'DEFAULT_VALUE'
            TYPE(VA),                                // and
            TYPE(VB),                                // 'UNINITIALIZED_VALUE'.
            TYPE(VC),
            TYPE(VD),
            TYPE(VE),
            TYPE(VF),
            TYPE(VG),
            TYPE(VH),
            TYPE(VI),
            TYPE(VJ),
            TYPE(VK),
            TYPE(VL)
        };

        *values = initValues;
        return NUM_VALUES;
    }

    static int populateString(bsl::basic_string<TYPE> *object,
                              const char              *spec,
                              int                      verboseFlag = 1)
    {
        const TYPE *VALUES;
        getValues(&VALUES);
        enum { SUCCESS = -1 };

        for (int i = 0; spec[i]; ++i) {
            if ('A' <= spec[i] && spec[i] <= 'L') {
                object->push_back(VALUES[spec[i] - 'A']);
            }
            else if ('~' == spec[i]) {
                object->clear();
            }
            else {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" "
                           "at position %d.\n", spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
        }
        return SUCCESS;
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
            memset(readbuf, -1, readbufSize);

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

    static void testStrAccessor()
    {
        typedef bsl::basic_string<TYPE,
                                  bsl::char_traits<TYPE>,
                                  bsl::allocator<TYPE> >    StringType;
        typedef bsl::basic_string_view<TYPE,
                                  bsl::char_traits<TYPE> >  ViewType;
        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     bsl::allocator<TYPE> > Obj;

        static const struct {
            int         d_line;     // line
            const char *d_spec_p;   // spec of the initial string
        } DATA[] = {
            //LINE SPEC
            //---- ---------------------------------------------------------
            { L_,  ""                                                        },
            { L_,  "A"                                                       },
            { L_,  "AA"                                                      },
            { L_,  "ABA"                                                     },
            { L_,  "ABCA"                                                    },
            { L_,  "ABCDA"                                                   },
            { L_,  "ABCDEA"                                                  },
            { L_,  "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFG" },
        };
        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;
        typedef std::allocator<TYPE> OtherAllocator;
        OtherAllocator otherAlloc;

        for (std::size_t i = 0; i != NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec_p;

            StringType initialString;
            populateString(&initialString, SPEC);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default");
                bslma::TestAllocator fa("footprint");
                bslma::TestAllocator sa("supplied");

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(initialString);
                  } break;
                  case 'b': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(initialString, objAllocatorPtr);
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"Bad allocator config.");
                  } break;
                }

                Obj&                   mX = *objPtr;
                const Obj&             X  = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;

                // Verify the object's 'get_allocator' accessor.

                ASSERTV(LINE, CONFIG, &oa, X.get_allocator().mechanism(),
                        &oa == X.get_allocator().mechanism());

                const StringType RESULT_S = X.str();
                const ViewType   RESULT_V = X.view();

                ASSERTV(LINE, CONFIG, initialString == RESULT_S);
                ASSERTV(LINE,
                        CONFIG,
                        &da,
                        RESULT_S.get_allocator().mechanism(),
                        &da == RESULT_S.get_allocator());
                ASSERTV(LINE, CONFIG, initialString == RESULT_V);

#ifndef BSLS_PLATFORM_CMP_SUN
        // These calls are not supported on SunOS, because the std::allocator
        // there does not support rebind.  This limitation can be lifted once
        // we fully support C++20, where rebind is removed, and always goes
        // through 'allocator_traits'.  See {DRQS 168075157} and
        // https://github.com/bloomberg/bde/pull/268
                    // test 'str(otherAllocator)'
                const bsl::basic_string<
                          TYPE,
                          bsl::char_traits<TYPE>,
                          OtherAllocator> otherAllocString = X.str(otherAlloc);

                ASSERTV(LINE, CONFIG, X.view()           == otherAllocString);
                ASSERTV(otherAllocString.get_allocator() == otherAlloc);
#endif

                fa.deleteObject(objPtr);
            }
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        {
            // test 'str() &&'
            for (std::size_t i = 0; i != NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;

                StringType initialString;
                populateString(&initialString, SPEC);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default");
                    bslma::TestAllocator fa("footprint");
                    bslma::TestAllocator sa("supplied");

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(initialString);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(initialString, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }

                    Obj&                   mX = *objPtr;
                    const Obj&              X  = mX;

                    ASSERTV(LINE, CONFIG, initialString == X.str());
                    ASSERTV(LINE, CONFIG, initialString == X.view());

                    const StringType movedStr = std::move(mX).str();
                    ASSERTV(LINE, CONFIG, initialString == movedStr);
                    ASSERTV(LINE, CONFIG, X.view().empty());

                    fa.deleteObject(objPtr);
                }
            }
        }
#endif

        {
            // test 'str(StringType)'
            bslma::TestAllocator ta("test allocator");

            static const struct {
                int  d_line;
                Mode d_mode;
            } OPENMODE_DATA[] =
            {
                { L_, IosBase::in                                 },
                { L_, IosBase::out                                },
                { L_, IosBase::in  | IosBase::out                 },
                { L_, IosBase::ate                                },
                { L_, IosBase::in  | IosBase::ate                 },
                { L_, IosBase::out | IosBase::ate                 },
                { L_, IosBase::in  | IosBase::out | IosBase::ate  }
            };
            const int NUM_OPENMODE_DATA = sizeof  OPENMODE_DATA /
                                          sizeof *OPENMODE_DATA;

            for (int i = 0; i != NUM_OPENMODE_DATA; ++i) {
                const int  LINE    = OPENMODE_DATA[i].d_line;
                const Mode MODE    = OPENMODE_DATA[i].d_mode;
                const char SPEC1[] = "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKL";
                const char SPEC2[] = "ABCDEFGHIJKLABCDEF";

                StringType initialString1;
                populateString(&initialString1, SPEC1);

                StringBufTest mX(initialString1, MODE);

                if (MODE & IosBase::out) {
                    ASSERTV(LINE, mX.pbase() && mX.epptr());
                    if (MODE & IosBase::in) {
                        ASSERTV(LINE, mX.eback() && mX.egptr());
                    }
                    else {
                        ASSERTV(LINE, (0 == mX.eback()) &&
                                      (0 == mX.egptr()));
                    }
                    StringType STR1(mX.pbase(), mX.epptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR1);

                    StringType initialString2;
                    populateString(&initialString2, SPEC2);

                    mX.str(initialString2);

                    StringType STR2(mX.pbase(), mX.epptr(), &ta);
                    ASSERTV(LINE, mX.str() != STR2);
                    ASSERTV(LINE, mX.str() == initialString2);
                    ASSERTV(LINE, mX.view() == initialString2);
                }
                else if (MODE & IosBase::in) {
                    ASSERTV(LINE, mX.eback(), mX.eback() && mX.egptr());
                    StringType STR(mX.eback(), mX.egptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR);
                    ASSERTV(LINE, mX.view() == STR);
                }
                else {
                    ASSERTV(LINE, mX.str().empty());
                    ASSERTV(LINE, mX.view().empty());
                }
            }
        }

        {
            // test 'str(MoveableRef<String>)'
            bslma::TestAllocator ta("test allocator");

            typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

            static const struct {
                int  d_line;
                Mode d_mode;
            } OPENMODE_DATA[] =
            {
                { L_, IosBase::in                                 },
                { L_, IosBase::out                                },
                { L_, IosBase::in  | IosBase::out                 },
                { L_, IosBase::ate                                },
                { L_, IosBase::in  | IosBase::ate                 },
                { L_, IosBase::out | IosBase::ate                 },
                { L_, IosBase::in  | IosBase::out | IosBase::ate  }
            };
            const int NUM_OPENMODE_DATA = sizeof  OPENMODE_DATA /
                                          sizeof *OPENMODE_DATA;

            for (int i = 0; i != NUM_OPENMODE_DATA; ++i) {
                const int  LINE    = OPENMODE_DATA[i].d_line;
                const Mode MODE    = OPENMODE_DATA[i].d_mode;
                const char SPEC1[] = "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKL";
                const char SPEC2[] = "ABCDEFGHIJKLABCDEF";

                StringType initialString1;
                populateString(&initialString1, SPEC1);

                StringBufTest mX(initialString1, MODE);

                if (MODE & IosBase::out) {
                    ASSERTV(LINE, mX.pbase() && mX.epptr());
                    if (MODE & IosBase::in) {
                        ASSERTV(LINE, mX.eback() && mX.egptr());
                    }
                    else {
                        ASSERTV(LINE, (0 == mX.eback()) &&
                                      (0 == mX.egptr()));
                    }
                    StringType STR1(mX.pbase(), mX.epptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR1);

                    StringType initialString2;
                    populateString(&initialString2, SPEC2);
                    StringType sourceString = initialString2;
                    mX.str(MoveUtil::move(sourceString));

                    ASSERTV(LINE, mX.str() != STR1);
                    // test strings that will be changed by a move
                    if (initialString2.size() >=
                                          LENGTH_OF_SUFFICIENTLY_LONG_STRING) {
                        ASSERT(sourceString.empty());
                    }
                    ASSERTV(LINE, mX.str() == initialString2);
                    ASSERTV(LINE, mX.view() == initialString2);
                }
                else if (MODE & IosBase::in) {
                    ASSERTV(LINE, mX.eback(), mX.eback() && mX.egptr());
                    StringType STR(mX.eback(), mX.egptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR);
                    ASSERTV(LINE, mX.view() == STR);
                }
                else {
                    ASSERTV(LINE, mX.str().empty());
                    ASSERTV(LINE, mX.view().empty());
                }
            }
        }

#ifndef BSLS_PLATFORM_CMP_SUN
        // These calls are not supported on SunOS, because the std::allocator
        // there does not support rebind.  This limitation can be lifted once
        // we fully support C++20, where rebind is removed, and always goes
        // through 'allocator_traits'.  See {DRQS 168075157} and
        // https://github.com/bloomberg/bde/pull/268
        {
            // test 'str(String with different Allocator)'
            bslma::TestAllocator ta("test allocator");

            static const struct {
                int  d_line;
                Mode d_mode;
            } OPENMODE_DATA[] =
            {
                { L_, IosBase::in                                 },
                { L_, IosBase::out                                },
                { L_, IosBase::in  | IosBase::out                 },
                { L_, IosBase::ate                                },
                { L_, IosBase::in  | IosBase::ate                 },
                { L_, IosBase::out | IosBase::ate                 },
                { L_, IosBase::in  | IosBase::out | IosBase::ate  }
            };
            const int NUM_OPENMODE_DATA = sizeof  OPENMODE_DATA /
                                          sizeof *OPENMODE_DATA;

            for (int i = 0; i != NUM_OPENMODE_DATA; ++i) {
                const int  LINE    = OPENMODE_DATA[i].d_line;
                const Mode MODE    = OPENMODE_DATA[i].d_mode;
                const char SPEC1[] = "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKL";
                const char SPEC2[] = "ABCDEFGHIJKLABCDEF";

                StringType initialString1;
                populateString(&initialString1, SPEC1);

                StringBufTest mX(initialString1, MODE);

                if (MODE & IosBase::out) {
                    ASSERTV(LINE, mX.pbase() && mX.epptr());
                    if (MODE & IosBase::in) {
                        ASSERTV(LINE, mX.eback() && mX.egptr());
                    }
                    else {
                        ASSERTV(LINE, (0 == mX.eback()) &&
                                      (0 == mX.egptr()));
                    }
                    StringType STR1(mX.pbase(), mX.epptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR1);

                    StringType initialString2;
                    populateString(&initialString2, SPEC2);

                    bsl::basic_string<TYPE,
                          bsl::char_traits<TYPE>,
                          OtherAllocator>
                                       otherAllocString(initialString2.data(),
                                                        initialString2.size());

                    mX.str(otherAllocString);

                    StringType STR2(mX.pbase(), mX.epptr(), &ta);
                    ASSERTV(LINE, mX.str() != STR2);
                    ASSERTV(LINE, mX.str() == initialString2);
                    ASSERTV(LINE, mX.view() == otherAllocString);
                    ASSERTV(LINE, mX.view() == initialString2);
                }
                else if (MODE & IosBase::in) {
                    ASSERTV(LINE, mX.eback(), mX.eback() && mX.egptr());
                    StringType STR(mX.eback(), mX.egptr(), &ta);
                    ASSERTV(LINE, mX.str() == STR);
                    ASSERTV(LINE, mX.view() == STR);
                }
                else {
                    ASSERTV(LINE, mX.str().empty());
                    ASSERTV(LINE, mX.view().empty());
                }
            }
        }
#endif
    }

    static void testStrManipulator()
    {
        typedef bsl::basic_string<TYPE,
                                  bsl::char_traits<TYPE>,
                                  bsl::allocator<TYPE> >    StringType;
        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     bsl::allocator<TYPE> > Obj;

        static const struct {
            int         d_line;     // line
            const char *d_spec_p;   // spec of the initial string
        } DATA[] = {
            //LINE SPEC
            //---- ---------------------------------------------------------
            { L_,  ""                                                        },
            { L_,  "A"                                                       },
            { L_,  "AA"                                                      },
            { L_,  "ABA"                                                     },
            { L_,  "ABCA"                                                    },
            { L_,  "ABCDA"                                                   },
            { L_,  "ABCDEA"                                                  },
            { L_,  "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFG" },
        };
        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec_p;

            StringType initialString;
            populateString(&initialString, SPEC);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default");
                bslma::TestAllocator fa("footprint");
                bslma::TestAllocator sa("supplied");

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(initialString);
                  } break;
                  case 'b': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(initialString, objAllocatorPtr);
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"Bad allocator config.");
                  } break;
                }

                Obj&                   mX = *objPtr;
                const Obj&             X  = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;

                // Verify the object's 'get_allocator' accessor.

                ASSERTV(LINE, CONFIG, &oa, X.get_allocator().mechanism(),
                        &oa == X.get_allocator().mechanism());

                for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
                    const int LENGTH_TJ = STRLEN_DATA[tj].d_length;

                    StringType mT(&da);  const StringType& T = mT;
                    loadString(&mT, LENGTH_TJ);

                    mX.str(T);
                    ASSERT(X.str() == T);
                    ASSERT(X.view() == T);
                }

                for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
                    const int LENGTH_TJ = STRLEN_DATA[tj].d_length;

                    StringType mT(&oa);  StringType& T = mT;
                    StringType oT(&oa);
                    loadString(&mT, LENGTH_TJ);
                    loadString(&oT, LENGTH_TJ);

                    mX.str(BloombergLP::bslmf::MovableRefUtil::move(T));
                    ASSERT(X.str() == oT);
                    ASSERT(X.view() == oT);
                    // test strings that will be changed by a move
                    if (LENGTH_TJ >= LENGTH_OF_SUFFICIENTLY_LONG_STRING) {
                        ASSERT(T != oT);
                    }
                }

                fa.deleteObject(objPtr);
            }
        }
    }

    static void testGetAllocator()
    {
        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     bsl::allocator<TYPE> > Obj;


        bslma::TestAllocator         da("default");
        bslma::TestAllocator         sa("supplied");
        bslma::DefaultAllocatorGuard dag(&da);

        Obj        mXD;
        const Obj& XD  = mXD;

        {
            bslma::TestAllocator         tda("temporary");
            bslma::DefaultAllocatorGuard tdag(&tda);

            ASSERTV(&da, XD.get_allocator().mechanism(),
                    &da == XD.get_allocator().mechanism());
        }

        Obj        mXS(&sa);
        const Obj& XS  = mXS;

        ASSERTV(&sa, XS.get_allocator().mechanism(),
                &sa == XS.get_allocator().mechanism());
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    static void testSwapMethod(int verbose)
    {
        typedef bsl::basic_string<TYPE,
                          bsl::char_traits<TYPE>,
                          bsl::allocator<TYPE> >    StringType;

        static const struct {
            int          d_line;    // line
            const char  *d_spec_p;  // spec for the initial string
            std::size_t  d_length;  // length of the spec

        } DATA[] = {
            //LINE SPEC                                               LENGTH
            //---- -------------------------------------------------- ------
            { L_, "A",                                                1     },
            { L_, "AB",                                               2     },
            { L_, "ABC",                                              3     },
            { L_, "ABCA",                                             4     },
            { L_, "ABCDA",                                            5     },
            { L_, "ABCDEA",                                           6     },
            { L_, "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKL", 48    }
        };

        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != NUM_DATA; ++i) {
            const int          LINE1   = DATA[i].d_line;
            const char        *SPEC1   = DATA[i].d_spec_p;
            const std::size_t  LENGTH1 = DATA[i].d_length;

            StringType  initialString1;
            populateString(&initialString1, SPEC1);

            for (std::size_t j = 0; j != NUM_DATA; ++j) {
                const int          LINE2   = DATA[j].d_line;
                const char        *SPEC2   = DATA[j].d_spec_p;
                const std::size_t  LENGTH2 = DATA[j].d_length;

                StringType  initialString2;
                populateString(&initialString2, SPEC2);

                for (std::size_t k = 0; k < LENGTH1; ++k) {
                    const std::size_t POS1 = k;
                    for (std::size_t l = 0; l < LENGTH2; ++l) {
                        const std::size_t POS2 = l;
                        if (verbose) { P_(SPEC1)P_(SPEC2) P_(POS1) P(POS2); }

                        StringBufTest sb1(initialString1, std::ios_base::in );
                        StringBufTest sb2(initialString2, std::ios_base::out);

                        sb1.seekpos(POS1, std::ios_base::in );
                        sb2.seekpos(POS2, std::ios_base::out);

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString1 == sb1.str());
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString2 == sb2.str());

                        sb1.swap(sb2);

                        // Check buffer replacement.

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString2 == sb1.str());
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString1 == sb2.str());

                        // Check mode replacement.  Writing to read-only buffer
                        // reading from write-only buffer should fail.

                        TYPE result = '0';
                        TYPE input  = '1';
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                0 == sb1.xsgetn(&result, 1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                0 == sb2.xsputn(&input,  1));

                        // Check current positions.

                        sb1.assertOutputPosition(LINE1, POS2);
                        sb2.assertInputPosition( LINE2, POS1);

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb1.xsputn(&input,  1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb2.xsgetn(&result, 1));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                input           == sb1.str()[POS2]);
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                sb2.str()[POS1] == result         );

                        // Check beginning positions.

                        sb1.seekpos(0, std::ios_base::out);
                        sb2.seekpos(0, std::ios_base::in );

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb1.xsputn(&input,  1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb2.xsgetn(&result, 1));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                input        == sb1.str()[0]);
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                sb2.str()[0] == result      );

                        // Check end positions.

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                static_cast<int>(LENGTH2) == sb1.seekpos(
                                                          LENGTH2,
                                                          std::ios_base::out));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                static_cast<int>(LENGTH1) == sb2.seekpos(
                                                           LENGTH1,
                                                           std::ios_base::in));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                -1 == sb1.seekpos(LENGTH2 + 1,
                                                  std::ios_base::out));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                -1 == sb2.seekpos(LENGTH1 + 1,
                                                  std::ios_base::in));
                    }
                }
            }
        }
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    static void testSwapFunction(int verbose)
    {
        typedef bsl::basic_string<TYPE,
                          bsl::char_traits<TYPE>,
                          bsl::allocator<TYPE> >    StringType;

        static const struct {
            int          d_line;    // line
            const char  *d_spec_p;  // spec for the initial string
            std::size_t  d_length;  // length of the spec

        } DATA[] = {
            //LINE SPEC                                               LENGTH
            //---- -------------------------------------------------- ------
            { L_, "A",                                                1     },
            { L_, "AB",                                               2     },
            { L_, "ABC",                                              3     },
            { L_, "ABCA",                                             4     },
            { L_, "ABCDA",                                            5     },
            { L_, "ABCDEA",                                           6     },
            { L_, "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKL", 48    }
        };

        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != NUM_DATA; ++i) {
            const int          LINE1   = DATA[i].d_line;
            const char        *SPEC1   = DATA[i].d_spec_p;
            const std::size_t  LENGTH1 = DATA[i].d_length;

            StringType  initialString1;
            populateString(&initialString1, SPEC1);

            for (std::size_t j = 0; j != NUM_DATA; ++j) {
                const int          LINE2   = DATA[j].d_line;
                const char        *SPEC2   = DATA[j].d_spec_p;
                const std::size_t  LENGTH2 = DATA[j].d_length;

                StringType  initialString2;
                populateString(&initialString2, SPEC2);

                for (std::size_t k = 0; k < LENGTH1; ++k) {
                    const std::size_t POS1 = k;
                    for (std::size_t l = 0; l < LENGTH2; ++l) {
                        const std::size_t POS2 = l;
                        if (verbose) { P_(SPEC1)P_(SPEC2)P_(POS1) P(POS2); }

                        StringBufTest sb1(initialString1, std::ios_base::in );
                        StringBufTest sb2(initialString2, std::ios_base::out);

                        sb1.seekpos(POS1, std::ios_base::in );
                        sb2.seekpos(POS2, std::ios_base::out);

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString1 == sb1.str());
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString2 == sb2.str());

                        bsl::swap(sb1, sb2);

                        // Check buffer replacement.

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString2 == sb1.str());
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                initialString1 == sb2.str());

                        // Check mode replacement.  Writing to read-only buffer
                        // reading from write-only buffer should fail.

                        TYPE result = '0';
                        TYPE input  = '1';
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                0 == sb1.xsgetn(&result, 1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                0 == sb2.xsputn(&input,  1));

                        // Check current positions.

                        sb1.assertOutputPosition(LINE1, POS2);
                        sb2.assertInputPosition( LINE2, POS1);

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb1.xsputn(&input,  1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb2.xsgetn(&result, 1));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                input           == sb1.str()[POS2]);
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                sb2.str()[POS1] == result         );

                        // Check beginning positions.

                        sb1.seekpos(0, std::ios_base::out);
                        sb2.seekpos(0, std::ios_base::in );

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb1.xsputn(&input,  1));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                1 == sb2.xsgetn(&result, 1));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                input        == sb1.str()[0]);
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                sb2.str()[0] == result      );

                        // Check end positions.

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                static_cast<int>(LENGTH2) == sb1.seekpos(
                                                          LENGTH2,
                                                          std::ios_base::out));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                static_cast<int>(LENGTH1) == sb2.seekpos(
                                                           LENGTH1,
                                                           std::ios_base::in));

                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                -1 == sb1.seekpos(LENGTH2 + 1,
                                                  std::ios_base::out));
                        ASSERTV(LINE1, LINE2, POS1, POS2,
                                -1 == sb2.seekpos(LENGTH1 + 1,
                                                  std::ios_base::in));
                    }
                }
            }
        }
    }

    static void testMoveAssigment(int verbose)
    {
        typedef bsl::basic_string<TYPE,
                          bsl::char_traits<TYPE>,
                          bsl::allocator<TYPE> >             StringT;

        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     bsl::allocator<TYPE> >  Obj;

        static
        const Mode OPENMODE_DATA[] =
        {
            IosBase::in,
            IosBase::out,
            IosBase::in  | IosBase::out,
            IosBase::ate,
            IosBase::in  | IosBase::ate,
            IosBase::out | IosBase::ate,
            IosBase::in  | IosBase::out | IosBase::ate
        };
        const int NUM_OPENMODE_DATA = sizeof OPENMODE_DATA /
                                      sizeof *OPENMODE_DATA;

        static const struct {
            int         d_line;    // line
            const char *d_str;     // initial string
            int         d_length;  // string length
        } DATA[] = {
            //LINE SPEC                                       LENGTH
            //---- -----------------------------------------  ------
            { L_,  ""                                        ,  0   },
            { L_,  "ABCDE"                                   ,  5   },
            { L_,  "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCD", 40   },
        };
        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator ta("Test Allocator");

        for (int ti = 0; ti != NUM_OPENMODE_DATA; ++ti) {
            const Mode MODE_TO = OPENMODE_DATA[ti];

            for (int tj = 0; tj != NUM_OPENMODE_DATA; ++tj) {
                const Mode MODE_FROM   = OPENMODE_DATA[tj];
                const bool FROM_IN_OUT = (MODE_FROM & IosBase::in ) ||
                                         (MODE_FROM & IosBase::out);

                for (int tk = 0; tk != NUM_DATA; ++tk) {
                    const int   LINE_TO   = DATA[tk].d_line;
                    const char *SPEC_TO   = DATA[tk].d_str;
                    const int   LENGTH_TO = DATA[tk].d_length;
                    StringT     STR_TO;

                    (void)LENGTH_TO;

                    populateString(&STR_TO, SPEC_TO);

                    for (int tl = 0; tl != NUM_DATA; ++tl) {
                        const int   LINE_FROM   = DATA[tl].d_line;
                        const char *SPEC_FROM   = DATA[tl].d_str;
                        const int   LENGTH_FROM = DATA[tl].d_length;
                        StringT     STR_FROM;

                        (void)LENGTH_FROM;

                        populateString(&STR_FROM, SPEC_FROM);

                        Obj sbTo  (STR_TO,   MODE_TO,   &ta);
                        Obj sbFrom(STR_FROM, MODE_FROM, &ta);

                        if (verbose) {
                            P_(MODE_TO)   T_ P(SPEC_TO);
                            P_(MODE_FROM)    P(SPEC_FROM);
                        }

                        sbTo = std::move(sbFrom);

                        if (FROM_IN_OUT) {
                            ASSERTV(LINE_TO, LINE_FROM,
                                    sbTo.str() == STR_FROM);
                        }
                        else {
                            ASSERTV(LINE_TO, LINE_FROM,
                                    sbTo.str().empty());
                        }
                    }
                }
            }
        }
    }
#endif

    static void testSwapNegative()
    {
        typedef bsltf::StdStatefulAllocator<
                                       TYPE,
                                       true,  // ON_CONTAINER_COPY_CONSTRUCTION
                                       true,  // ON_CONTAINER_COPY_ASSIGNMENT
                                       true,  // ON_CONTAINER_SWAP
                                       true>  // ON_CONTAINER_MOVE_ASSIGNMENT
                                       PropagatingStdAlloc;

        typedef bsltf::StdStatefulAllocator<
                                      TYPE,
                                      true,   // ON_CONTAINER_COPY_CONSTRUCTION
                                      true,   // ON_CONTAINER_COPY_ASSIGNMENT
                                      false,  // ON_CONTAINER_SWAP
                                      true>   // ON_CONTAINER_MOVE_ASSIGNMENT
                                      NonPropagatingStdAlloc;

        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     PropagatingStdAlloc>    PropagatingObj;
        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     NonPropagatingStdAlloc> NonPropagatingObj;

        bsls::AssertTestHandlerGuard guard;

        bslma::TestAllocator   ta1;
        bslma::TestAllocator   ta2;
        PropagatingStdAlloc    pa1(&ta1);
        PropagatingStdAlloc    pa2(&ta2);
        NonPropagatingStdAlloc npa1(&ta1);
        NonPropagatingStdAlloc npa2(&ta2);

        PropagatingObj    poEqualAlloc1(pa1);
        PropagatingObj    poEqualAlloc2(pa1);
        PropagatingObj    poNonEqualAlloc(pa2);

        NonPropagatingObj npoEqualAlloc1(npa1);
        NonPropagatingObj npoEqualAlloc2(npa1);
        NonPropagatingObj npoNonEqualAlloc(npa2);

        ASSERT_PASS(poEqualAlloc1.swap(poEqualAlloc2  ));
        ASSERT_PASS(poEqualAlloc1.swap(poNonEqualAlloc));

        ASSERT_PASS(npoEqualAlloc1.swap(npoEqualAlloc2  ));
        ASSERT_FAIL(npoEqualAlloc1.swap(npoNonEqualAlloc));
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE

    static void testMoveCtorWithAllocator(int verbose)
    {
        typedef bsl::basic_string<TYPE,
                                  bsl::char_traits<TYPE>,
                                  bsl::allocator<TYPE> >       StringType;
        typedef bsl::basic_stringbuf<TYPE,
                                     bsl::char_traits<TYPE>,
                                     bsl::allocator<TYPE> >    Obj;

        static const struct {
            int         d_line;     // line
            const char *d_spec_p;   // spec of the initial string
        } DATA[] = {
            //LINE SPEC
            //---- ---------------------------------------------------------
            { L_,  ""                                                        },
            { L_,  "A"                                                       },
            { L_,  "AA"                                                      },
            { L_,  "ABA"                                                     },
            { L_,  "ABCA"                                                    },
            { L_,  "ABCDA"                                                   },
            { L_,  "ABCDEA"                                                  },
            { L_,  "ABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFGHIJKLABCDEFG" },
        };
        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i != NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec_p;

            StringType initialString;
            populateString(&initialString, SPEC);

            const std::size_t LENGTH = initialString.length();

            bslma::TestAllocator da("default");
            bslma::TestAllocator oa("object");
            bslma::TestAllocator sa("supplied");

            bslma::DefaultAllocatorGuard dag(&da);

            for (std::size_t j = 0; j < LENGTH; ++j) {
                const std::size_t POS = j;
                if (verbose) { P_(SPEC) P(POS) };

                Obj        mXI(initialString, std::ios_base::in );
                const Obj& XI = mXI;
                Obj        mXO(initialString, std::ios_base::out);
                const Obj& XO = mXO;

                mXI.pubseekpos(POS, std::ios_base::in );
                mXO.pubseekpos(POS, std::ios_base::out);

                ASSERTV(LINE, POS, initialString == XI.str());
                ASSERTV(LINE, POS, initialString == XO.str());

                Obj        mYI(std::move(mXI), &sa);
                const Obj& YI = mYI;
                Obj        mYO(std::move(mXO), &sa);
                const Obj& YO = mYO;

                // Check allocator replacement.

                ASSERTV(LINE, POS, &sa == YI.get_allocator().mechanism());
                ASSERTV(LINE, POS, &sa == YO.get_allocator().mechanism());

                // Check buffer replacement.

                ASSERTV(LINE, POS, initialString == YI.str());
                ASSERTV(LINE, POS, initialString == YO.str());

                // Check mode replacement.  Writing to read-only buffer and
                // reading from write-only buffer should fail.

                TYPE result = '0';
                TYPE input  = '1';

                ASSERTV(LINE, POS, 0 == mYI.sputn(&input,  1));
                ASSERTV(LINE, POS, 0 == mYO.sgetn(&result, 1));

                // Check current positions.

                ASSERTV(LINE, POS, 1 == mYI.sgetn(&result, 1));
                ASSERTV(LINE, POS, 1 == mYO.sputn(&input, 1));

                ASSERTV(LINE, POS, YI.str()[POS] == result);
                ASSERTV(LINE, POS, input == YO.str()[POS]);

                // Check beginning positions.

                mYI.pubseekpos(0, std::ios_base::in);
                mYO.pubseekpos(0, std::ios_base::out);

                ASSERTV(LINE, POS, 1 == mYI.sgetn(&result, 1));
                ASSERTV(LINE, POS, 1 == mYO.sputn(&input, 1));

                ASSERTV(LINE, POS, YI.str()[0] == result);
                ASSERTV(LINE, POS, input == mYO.str()[0]);

                // Check end positions.

                ASSERTV(LINE, POS,
                        static_cast<int>(LENGTH) ==
                                   mYI.pubseekpos(LENGTH, std::ios_base::in ));
                ASSERTV(LINE, POS,
                        static_cast<int>(LENGTH) ==
                                   mYO.pubseekpos(LENGTH, std::ios_base::out));

                ASSERTV(LINE, POS,
                        -1 == mYI.pubseekpos(LENGTH + 1, std::ios_base::in ));
                ASSERTV(LINE, POS,
                        -1 == mYO.pubseekpos(LENGTH + 1, std::ios_base::out));
            }
        }
    }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE

    // CREATORS
    StringBufTest(bsl::basic_string<TYPE> const & s,
                  bsl::ios_base::openmode         mode)
        : bsl::basic_stringbuf<TYPE>(s, mode)
    {}

    // ACCESSORS
    void assertInputPosition(int line, std::streampos inputPos)
    {
        ASSERTV(line, inputPos - std::streampos(0) ==
                    static_cast<std::streampos>(Base::gptr() - Base::eback()));
        ASSERTV(line, Base::egptr() - Base::eback() > inputPos);
    }

    void assertOutputPosition(int line, std::streampos outputPos)
    {
        ASSERTV(line, outputPos - std::streampos(0) ==
                    static_cast<std::streampos>(Base::pptr() - Base::pbase()));
        ASSERTV(line, Base::epptr() - Base::pbase() > outputPos);
    }

    void assertPositions(int line,
                         std::streampos inputPos,
                         std::streampos outputPos)
    {
        LOOP_ASSERT(line, Base::eback() == Base::pbase());
        LOOP_ASSERT(line, Base::egptr() <= Base::epptr());
        assertInputPosition(line, inputPos);
        assertOutputPosition(line, outputPos);
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
      case 20: {
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
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'str' MANIPULATOR
        //
        // Concerns:
        //: 1 Setting the contents of the internal string works as expected.
        //
        // Plan:
        //: 1 Set the contents via the 'str' manipulator, and then verify that
        //:   the correct contents are available.
        //:
        //: 2 Set the contents with a 'MovableRef', and verify that the correct
        //:   contents are available, and that the source was moved from, when
        //:   appropriate.
        //
        // Testing:
        //   void str(const StringType& value);
        //   void str(BloombergLP::bslmf::MovableRef<StringType> value);
        //   void str(const basic_string<CHAR, TRAITS, SALLOC>& value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'str' MANIPULATOR"
                            "\n=========================\n");

        StringBufTest<char   >::testStrManipulator();
        StringBufTest<wchar_t>::testStrManipulator();

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 The 'swap' method exchanges internal strings.
        //:
        //: 2 The 'swap' method exchanges internal pointers.
        //:
        //: 3 The 'swap' method exchanges modes.
        //:
        //: 4 Swap free function works the same way as the 'swap' method.
        //:
        //: 5 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create two objects of the 'StringBufTest' that allows to call
        //:   'bslstl::basic_stringbuf' protected methods.
        //:
        //: 2 Swap these objects using class method and
        //:
        //:   1 Using 'str()' method verify that the internal strings have
        //:     been exchanged.  (C-1)
        //:
        //:   2 Using protected methods 'xsgetn()' for output 'streambuf' and
        //:     'xsputn()' for input 'streambuf' verify that modes of the
        //:     objects have been replaced.  (C-3)
        //:
        //:   3 Using protected methods 'seekpos()', 'xsgetn()' and
        //:     'xsputn()' verify that internal pointers of the objects have
        //:     been replaced.  (C-2)
        //:
        //: 3 Create another two objects, swap them using free function and
        //:   repeat the steps from P-2.  (C-4)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones.  (C-5)
        //
        // Testing:
        //   void swap(basic_stringbuf& other);
        //   void swap(basic_stringbuf& a, basic_stringbuf& b);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING SWAP"
                            "\n============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) printf("\tTesting method.\n");
        StringBufTest<char   >::testSwapMethod(verbose);
        StringBufTest<wchar_t>::testSwapMethod(verbose);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        if (verbose) printf("\tTesting free function.\n");
        StringBufTest<char   >::testSwapFunction(verbose);
        StringBufTest<wchar_t>::testSwapFunction(verbose);
#endif
        if (verbose) printf("\tNegative Testing.\n");
        StringBufTest<char   >::testSwapNegative();
        StringBufTest<wchar_t>::testSwapNegative();
#else
    if (verbose) {
        printf("\tThis functionality is not supported.\n");
    }
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING MOVE CTOR WITH ALLOCATOR
        //
        // Concerns:

        //
        // Plan:

        // Testing:
        //   stringbuf(stringbuf&&, const ALLOCATOR&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CTOR WITH ALLOCATOR"
                            "\n================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        StringBufTest<char   >::testMoveCtorWithAllocator(verbose);
        StringBufTest<wchar_t>::testMoveCtorWithAllocator(verbose);
#endif
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING MODIFYING BUFFER POINTERS VIA BASE CLASS INTERFACE
        //
        // Concerns:
        //: 1 The class functions correctly even when the base class functions
        //:   'setg' and 'setp' are used to set buffer pointers to 0.
        //
        // Plan:
        //: 1 Ensure that the object's destructor validation works with
        //:   pointers equal to 0.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MODIFYING BUFFER POINTERS VIA BASE"
                            "\n======================================\n");

        class DerivedStringBuf
            : private bsl::stringbuf {
          public:
            DerivedStringBuf()
            : bsl::stringbuf()
            {
            }
            DerivedStringBuf(const bsl::string &initialString)
            : bsl::stringbuf(initialString)
            {
            }

            void setg(char_type *gbeg, char_type *gcurr, char_type *gend)
            {
                bsl::stringbuf::setg(gbeg, gcurr, gend);
            }
            void setp(char_type *pbeg, char_type *pend)
            {
                bsl::stringbuf::setp(pbeg, pend);
            }
        };

        {
            DerivedStringBuf buf;
            buf.setg(0, 0, 0);
        }
        {
            DerivedStringBuf buf("abc");
            buf.setg(0, 0, 0);
        }
        {
            DerivedStringBuf buf;
            buf.setp(0, 0);
        }
        {
            DerivedStringBuf buf("abc");
            buf.setp(0, 0);
        }
        {
            DerivedStringBuf buf;
            buf.setg(0, 0, 0);
            buf.setp(0, 0);
        }
        {
            DerivedStringBuf buf("abc");
            buf.setg(0, 0, 0);
            buf.setp(0, 0);
        }
      } break;
      case 15: {
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
      case 14: {
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

        StringBufTest<char>::testXsgetn();

        if (veryVerbose) printf("\ttesting underflow\n");

        StringBufTest<char>::testUnderflow();

        if (veryVerbose) printf("\ttesting uflow\n");

        StringBufTest<char>::testUflow();

        if (veryVerbose) printf("\ttesting pbackfail\n");

        StringBufTest<char>::testPbackfail();

        if (veryVerbose) printf("\ttesting overflow\n");

        StringBufTest<char>::testOverflow();

        if (veryVerbose) printf("\ttesting xsputn\n");

        StringBufTest<char>::testXsputn();

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
            ASSERT(strncmp(read_buf, "abc", 3) == 0);

            bsl::stringbuf buf3("de");
            std::streamsize res3 = buf3.sgetn(read_buf, read_buf_size);
            ASSERT(res3 == 2);
            ASSERT(strncmp(read_buf, "dec", 3) == 0);
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
      case 13: {
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
      case 12: {
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
        //
        // Testing:
        //   overflow(int)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OVERFLOW FUNCTION"
                            "\n=========================\n");

        StringBufTest<char>::testOverflow();

      } break;
      case 11: {
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

        StringBufTest<char>::testXsputn();

      } break;
      case 10: {
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

        StringBufTest<char>::testPbackfail();

      } break;
      case 9: {
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

        StringBufTest<char>::testUflow();

      } break;
      case 8: {
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

        StringBufTest<char>::testUnderflow();

      } break;
      case 7: {
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

        StringBufTest<char>::testXsgetn();

      } break;
      case 6: {
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

        StringBufTest<char>::testSeekpos();

      } break;
      case 5: {
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

        StringBufTest<char>::testSeekoff();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1. move assignment moves
        //: 2. move assignment properly updates the internal pointers of the
        //:    moved-from object if the internal string changes
        //: 3. move assignment properly updates the internal pointers of the
        //:    moved-to object if the internal string has the small string
        //:    optimization (pointers point into the object)
        //: 4. move assignment objects having different open modes works as
        //:    expected
        //
        // Plan:
        //: 1. create a stringbuf object with both long (allocating) and short
        //:    strings.  Move assign other stringbufs from them.
        //:    1. Verify that all accessible members have the moved-from
        //:       values.
        //:    2. Verify that the string has not changed.
        //:    3. Verify that the read position has not changed.
        //:    4. Verify that the long string has been moved-from by checking
        //:       that the original stream now reports an empty 'str()'.  Note
        //:       that this test requires the inner string to be longer than
        //:       the short string optimization.
        //:    5. Verify that the short string has been non-destructively
        //:       moved-from by checking that the original stream still reports
        //:       the original 'str()'.  Note that this test requires the
        //:       string to employ the short string optimization (otherwise the
        //:       move is destructive).
        //:    6. Verify that the write position has not changed.
        //: 2. Define a set of short and long strings (u) and a set of the open
        //:    mode values (v).  For every item in the cross-product of these
        //:    two sets (u, v), verify that after moving the string associated
        //:    with the moved-to string buffer equals to the string associated
        //:    with the moved-from string buffer obtained before moving.  (C-4)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        if (veryVerbose)
            printf("\tstringbuf move assignment with a long string\n");

        {
            bslma::TestAllocator ta("Test Allocator", veryVeryVeryVerbose);
            static const char ls[] = "something very very very very very long";
            bsl::stringbuf sb1(ls,
                               std::ios_base::in | std::ios_base::out,
                               &ta);

            ASSERTV(ls[0], (char)sb1.snextc(), ls[0] == sb1.sbumpc());
            ASSERTV(ls[1], (char)sb1.snextc(), ls[1] == sb1.sgetc());

            bsl::stringbuf sb2(&ta);
            sb2 = std::move(sb1);

            ASSERT(ls == sb2.str());
            ASSERT(sb1.str().empty());
            ASSERT(bsl::stringbuf::traits_type::eof() == sb1.sgetc());

            for (const char *p = ls + 1; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
            ASSERTV(sb2.sgetc(), (char)sb2.sgetc(),
                    bsl::stringbuf::traits_type::eof() == sb2.sgetc());

            // Test output

            static const char   s[] = "a string";
            static const size_t n = sizeof(s) / sizeof(s[0]) - 1;
            ASSERT(n == sb2.sputn(s, n));
            ASSERT(sb2.str() == "a stringg very very very very very long");
            ASSERT(0 == sb2.pubseekpos(0, std::ios_base::in));
            ASSERT(sb1.str().empty());
            for (const char *p = s; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
        }

        if (veryVerbose)
            printf("\tstringbuf move assignment with a short string\n");

        {  // With a short string move copies the string
            bslma::TestAllocator ta("Test Allocator", veryVeryVeryVerbose);
            static const char sm[] = "small";
            bsl::stringbuf sb1(sm,
                               std::ios_base::in | std::ios_base::out,
                               &ta);

            bsl::stringbuf sb2(&ta);
            sb2 = std::move(sb1);

            ASSERT(sm == sb2.str());
            ASSERT(sm == sb1.str());

            // Test output

            static const char   s[] = "short";
            static const size_t n = sizeof(s) / sizeof(s[0]) - 1;
            ASSERT(n == sb2.sputn(s, n));
            ASSERT(s == sb2.str());
            ASSERT(0 == sb2.pubseekpos(0, std::ios_base::in));
            ASSERT(sm == sb1.str());
            for (const char *p = s; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
        }

        StringBufTest<char   >::testMoveAssigment(verbose);
        StringBufTest<wchar_t>::testMoveAssigment(verbose);
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The 'get_allocator' method returns the allocator specified at
        //:   construction, and that is the default allocator at the time of
        //:   object's construction if none was specified at construction.
        //:
        //: 2 The 'str' method returns a copy of the buffered sequence of
        //:   characters that uses the default allocator to supply memory.
        //:
        //: 3 If string buffer object was created with
        //:   'modeBitMask & ios_base::out' being nonzero then 'str' method
        //:   returns a copy of the buffered sequence in the range
        //:   '[pbase(), high_mark)', where 'high_mark' represents the position
        //:   one past the highest initialized character in the buffer.
        //:
        //: 4 If string buffer object was created only in input mode then 'str'
        //:   method returns a copy of the buffered sequence in the range
        //:   '[eback(), egptr())'.
        //:
        //: 5 If string buffer object was created in neither output nor input
        //:   mode then 'str' method returns a zero length string.
        //
        // Plan:
        //: 1 Create an object without passing an allocator reference, setup
        //:   temporary default allocator and verify that 'get_allocator'
        //:   returns a copy of the default allocator at the time of object's
        //:   construction.
        //:
        //: 2 Create an object specifying the allocator and verify that
        //:   'get_allocator' returns a copy of the supplied allocator.  (C-1)
        //:
        //: 3 Specify a set S of initial strings with substantial and varied
        //:   differences, ordered by increasing length, to be used in the
        //:   following tests.
        //:
        //: 4 Execute a loop that creates an object with each value in S but
        //:   invokes the constructor differently in each iteration:
        //:   (a) without passing an allocator,
        //:   (b) passing the address of a test allocator distinct from the
        //:       default.
        //:
        //: 5 For each iteration from P-4:
        //:
        //:   1 Using 'str' method obtain a copy of the object's buffered
        //:     sequence of characters.
        //:
        //:   2 Verify the value of the obtained string.
        //:
        //:   3 Using the 'get_allocator()' method verify that the expected
        //:     allocator is assigned to the obtained string.  (C-2)
        //:
        //: 6 Create a table of distinct open mode value (in, out, ate)
        //:   combination.
        //:
        //: 7 For each item in the table in P-6:
        //:
        //:   1 Create an object specifying the open mode and a distinct string
        //:     value.
        //:
        //:   2 If the object was created only in input mode, ensure that 'str'
        //:     returns a string equal to a copy of the buffered sequence in
        //:     the range '[eback(), egptr()).
        //:
        //:   3 If the object was created with 'modeBitMask & ios_base::out'
        //:     being nonzero, ensure that 'str' returns a string equal to a
        //:     copy of the buffered sequence in the range
        //:     '[eback(), high_mark)'.
        //:
        //:   4 If the object was created in neither input or output mode,
        //:     ensure 'str' returns an empty string.
        //
        // Testing:
        //   allocator_type get_allocator() const;
        //   StringType str() const;
        //   StringType str() &&;
        //   StringType str(const SALLOC&);
        //   ViewType view() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        if (verbose) printf("\tTesting 'get_allocator().'\n");

        StringBufTest<char   >::testGetAllocator();
        StringBufTest<wchar_t>::testGetAllocator();

        if (verbose) printf("\tTesting 'str()'.\n");

        StringBufTest<char   >::testStrAccessor();
        StringBufTest<wchar_t>::testStrAccessor();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1. stringbuf is creatable with the default constructor
        //: 2. stringbuf is creatable with constructor taking the input/output
        //:    mode
        //: 3. stringbuf is creatable with constructor taking the initial
        //:    string
        //: 4. stringbuf is creatable with constructors taking an allocator
        //: 5. move constructor moves
        //: 6. move constructor properly updates the internal pointers of the
        //:    moved-from object if the internal string changes
        //: 7. move constructor properly updates the internal pointers of the
        //:    moved-to object if the internal string has the small string
        //:    optimization (pointers point into the object)
        //
        // Plan:
        //: 1. create stringbuf object with the default constructor
        //: 2. create stringbuf objects with constructors taking the
        //:    input/output mode
        //: 3. create stringbuf object with constructor taking the initial
        //:    string
        //: 4. create stringbuf objects with constructors taking an allocator
        //: 5. create a stringbuf object with both long (allocating) and short
        //:    strings.  Move construct other stringbufs from them.
        //:    1. Verify that all accessible members have the moved-from
        //:       values.
        //:    2. Verify that the string has not changed.
        //:    3. Verify that the read position has not changed.
        //:    4. Verify that the long string has been moved-from by checking
        //:       that the original stream now reports an empty 'str()'.  Note
        //:       that this test requires the inner string to be longer than
        //:       the short string optimization.
        //:    5. Verify that the short string has been non-destructively
        //:       moved-from by checking that the original stream still reports
        //:       the original 'str()'.  Note that this test requires the
        //:       string to employ the short string optimization (otherwise the
        //:       move is destructive).
        //:    6. Verify that the write position has not changed.
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

            typedef bslmf::MovableRefUtil MoveUtil;
            bslma::TestAllocator          ta1("Test Allocator #1");
            bslma::TestAllocator          ta2("Test Allocator #2");
            const bsl::ios_base::openmode mode = std::ios_base::in;

            for (int tj = 0; tj < NUM_STRLEN_DATA; ++tj) {
                const int LENGTH_TJ = STRLEN_DATA[tj].d_length;

                // string used for comparisons
                bsl::string oT;  bsl::string& O = oT;
                loadString(&oT, LENGTH_TJ);

                // strings used for constructions
                bsl::string str1(&ta1);
                bsl::string str2(&ta2);
                ASSERT(str1.get_allocator() != str2.get_allocator());

                // test bsl::stringbuf(MovableRef)
                {
                    loadString(&str1, LENGTH_TJ);
                    bsl::stringbuf buf1(MoveUtil::move(str1));
                    ASSERT(buf1.str() == O);
                    ASSERT(buf1.get_allocator() == str1.get_allocator());
                    if (stringCouldBeMovedFrom(str1, &ta1)) {
                        ASSERT(stringWasMovedFrom(str1));
                    }
                }

                // test bsl::stringbuf(MovableRef, mode)
                {
                    loadString(&str1, LENGTH_TJ);
                    bsl::stringbuf buf1(MoveUtil::move(str1), mode);
                    ASSERT(buf1.str() == O);
                    ASSERT(buf1.get_allocator() == str1.get_allocator());
                    if (stringCouldBeMovedFrom(str1, &ta1)) {
                        ASSERT(stringWasMovedFrom(str1));
                    }
                }
                // test bsl::stringbuf(MovableRef, allocator)
                {
                    loadString(&str1, LENGTH_TJ);
                    loadString(&str2, LENGTH_TJ);
                    bsl::stringbuf buf1(MoveUtil::move(str1), &ta1);
                    bsl::stringbuf buf2(MoveUtil::move(str2), &ta1); // diff
                    ASSERT(buf1.str() == O);
                    ASSERT(buf1.get_allocator() == str1.get_allocator());
                    if (stringCouldBeMovedFrom(str1, &ta1)) {
                        ASSERT(stringWasMovedFrom(str1));
                    }

                    ASSERT(buf2.str() == O);
                    ASSERT(buf2.get_allocator() == bsl::allocator<char>(&ta1));
                    ASSERT(0 == LENGTH_TJ || !stringWasMovedFrom(str2));
                }

                // test bsl::stringbuf(MovableRef, mode, allocator)
                {
                    loadString(&str1, LENGTH_TJ);
                    loadString(&str2, LENGTH_TJ);
                    bsl::stringbuf buf1(MoveUtil::move(str1), mode, &ta1);
                    bsl::stringbuf buf2(MoveUtil::move(str2), mode, &ta1);
                    ASSERT(buf1.str() == O);
                    ASSERT(buf1.get_allocator() == str1.get_allocator());
                    if (stringCouldBeMovedFrom(str1, &ta1)) {
                        ASSERT(stringWasMovedFrom(str1));
                    }

                    ASSERT(buf2.str() == O);
                    ASSERT(buf2.get_allocator() == bsl::allocator<char>(&ta1));
                    ASSERT(0 == LENGTH_TJ || !stringWasMovedFrom(str2));
                }


            }
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

            bsl::basic_string<char,
                              bsl::char_traits<char>,
                              bslma::StdTestAllocator<char> > s1("something");

            bsl::stringbuf buf4(s1);
            ASSERT(buf4.str() == "something");

            bsl::stringbuf buf5(s1, bsl::allocator<char>());
            ASSERT(buf5.str() == "something");

            bsl::stringbuf buf6(s1, std::ios_base::in);
            ASSERT(buf6.str() == "something");

            bsl::stringbuf buf7(s1, std::ios_base::in, bsl::allocator<char>());
            ASSERT(buf7.str() == "something");
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
        if (veryVerbose)
            printf("\tstringbuf move construction with a long string\n");

        {
            bslma::TestAllocator ta("Test Allocator", veryVeryVeryVerbose);
            static const char ls[] = "something very very very very very long";
            bsl::stringbuf sb1(ls,
                               std::ios_base::in | std::ios_base::out,
                               &ta);

            ASSERTV(ls[0], (char)sb1.snextc(), ls[0] == sb1.sbumpc());
            ASSERTV(ls[1], (char)sb1.snextc(), ls[1] == sb1.sgetc());

            bsl::stringbuf sb2(std::move(sb1));

            ASSERT(ls == sb2.str());
            ASSERT(sb1.str().empty());
            ASSERT(bsl::stringbuf::traits_type::eof() == sb1.sgetc());

            for (const char *p = ls + 1; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
            ASSERT(bsl::stringbuf::traits_type::eof() == sb2.sgetc());

            // Test output

            static const char   s[] = "a string";
            static const size_t n = sizeof(s) / sizeof(s[0]) - 1;
            ASSERT(n == sb2.sputn(s, n));
            ASSERT(sb2.str() == "a stringg very very very very very long");
            ASSERT(0 == sb2.pubseekpos(0, std::ios_base::in));
            ASSERT(sb1.str().empty());
            for (const char *p = s; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
        }

        if (veryVerbose)
            printf("\tstringbuf move construction with a short string\n");

        {  // With a short string move copies the string
            bslma::TestAllocator ta("Test Allocator", veryVeryVeryVerbose);
            static const char sm[] = "small";
            bsl::stringbuf sb1(sm,
                               std::ios_base::in | std::ios_base::out,
                               &ta);

            bsl::stringbuf sb2(std::move(sb1));

            ASSERT(sm == sb2.str());
            ASSERT(sm == sb1.str());

            // Test output

            static const char   s[] = "short";
            static const size_t n = sizeof(s) / sizeof(s[0]) - 1;
            ASSERT(n == sb2.sputn(s, n));
            ASSERT(s == sb2.str());
            ASSERT(0 == sb2.pubseekpos(0, std::ios_base::in));
            ASSERT(sm == sb1.str());
            for (const char *p = s; *p; ++p) {
                ASSERTV((char)sb2.snextc(), *p == sb2.sbumpc());
            }
        }
#endif
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
