// bslim_gtestutil.t.cpp                                              -*-C++-*-
#include <bslim_gtestutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

// Do not 'using' any namespaces.  We want to verify that everything works from
// the global namespace.

using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test consists of a series of static member functions
// that provide facilities for debugging BDE with gtest.
// ----------------------------------------------------------------------------
// [ 5] void PrintTo(const bsl::optional<TYPE>& value, ostream *stream);
// [ 4] void PrintTo(const bslstl::StringRef& value, ostream *stream);
// [ 3] void PrintTo(const bsl::wstring& value, ostream *stream);
// [ 2] void PrintTo(const bsl::string& value, ostream *stream);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q     // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P     // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_    // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_    // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_    // current Line number

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

const char        *EMPTY_STRING = "";
const wchar_t     *EMPTY_WCHAR_STRING = L"";

const bsl::string  EMPTY_EXPECTED("\"\"");
const bsl::string  EMPTY_OPTIONAL_EXPECTED("(nullopt)");

const char        *LONG_STRING  = "123456789012345678901234567890"
                                  "123456789012345678901234567890"
                                  "123456789012345678901234567890";
const wchar_t     *LONG_WCHAR_STRING  = L"123456789012345678901234567890"
                                        L"123456789012345678901234567890"
                                        L"123456789012345678901234567890";

const bsl::string  LONG_EXPECTED = bsl::string("\"")
                                 + bsl::string(LONG_STRING)
                                 + bsl::string("\"");

// Non empty string: "12345\0abcde".
// Note the null value character in the middle.

const char         NON_EMPTY_STRING[] =
                                 {49, 50, 51, 52, 53, 0, 97, 98, 99, 100, 101};
const bsl::size_t  NON_EMPTY_STRING_LENGTH =
                          sizeof(NON_EMPTY_STRING) / sizeof(*NON_EMPTY_STRING);
const char        *NON_EMPTY_STRING_BEGIN = NON_EMPTY_STRING;
const char        *NON_EMPTY_STRING_END =
                                    NON_EMPTY_STRING + NON_EMPTY_STRING_LENGTH;

const wchar_t      NON_EMPTY_WCHAR_STRING[] =
                              {49, 50, 51, 52, 53, 0, 153, 154, 155, 156, 157};
const bsl::size_t  NON_EMPTY_WCHAR_STRING_LENGTH =
              sizeof(NON_EMPTY_WCHAR_STRING) / sizeof(*NON_EMPTY_WCHAR_STRING);
const wchar_t     *NON_EMPTY_WCHAR_STRING_END =
                        NON_EMPTY_WCHAR_STRING + NON_EMPTY_WCHAR_STRING_LENGTH;

const char        *NON_EMPTY_WCHAR_EXPECTED = (4 == sizeof(wchar_t))
    //          0          153        154        155        156        157
    //         --------   --------   --------   --------   --------   --------
    ? "12345\\x00000000\\x00000099\\x0000009a\\x0000009b\\x0000009c\\x0000009d"
    : "12345\\x0000"  "\\x0099"  "\\x009a"  "\\x009b"  "\\x009c"  "\\x009d";

// ============================================================================
//           SIMULATE LEGITIMATE 'PrintTo' DECLARATIONS IN 'testing'
// ----------------------------------------------------------------------------

namespace testing {

class Hisser {
    // DATA
    unsigned d_hissLen;

  public:
    // CREATOR
    explicit
    Hisser(unsigned hissLen)
    : d_hissLen(hissLen)
    {}

    // ACCESSOR
    unsigned hissLen() const
    {
        return d_hissLen;
    }
};

void PrintTo(const Hisser& hisser, ::std::ostream *stream)
{
    *stream << '"';

    for (unsigned uu = 0; uu < hisser.hissLen(); ++uu) {
        *stream << 's';
    }

    *stream << '"';
}

namespace internal {

void PrintTo(int value, ::std::ostream *stream_p)
{
    *stream_p << value;
}

void PrintTo(const bsl::string_view& sv, ::std::ostream *stream_p)
{
    *stream_p << '"';
    for (unsigned uu = 0; uu < sv.length(); ++uu) {
        if (sv[uu]) {
            *stream_p << sv[uu];
        }
        else {
            *stream_p << "\\0";
        }
    }
    *stream_p << '"';
}

// ============================================================================
//        These functions are just propagated from 'gtest-printers.h'.
// ----------------------------------------------------------------------------

template <typename T>
class UniversalPrinter {
    public:

    // Note: we deliberately don't call this PrintTo(), as that name
    // conflicts with ::testing::internal::PrintTo in the body of the
    // function.

    static void Print(const T& value, ::std::ostream* os) {
        // By default, ::testing::internal::PrintTo() is used for printing
        // the value.
        //
        // Thanks to Koenig look-up, if T is a class and has its own
        // PrintTo() function defined in its namespace, that function will
        // be visible here.  Since it is more specific than the generic ones
        // in ::testing::internal, it will be picked by the compiler in the
        // following statement - exactly what we want.

        PrintTo(value, os);
    }
};

template <typename T>
void UniversalPrint(const T& value, ::std::ostream* os) {
    // A workarond for the bug in VC++ 7.1 that prevents us from instantiating
    // UniversalPrinter with T directly.

    typedef T T1;
    UniversalPrinter<T1>::Print(value, os);
}

}  // close namespace internal
}  // close namespace testing

// ============================================================================
//           SIMULATE LEGITIMATE 'PrintTo' DECLARATIONS IN 'testingB'
// ----------------------------------------------------------------------------

namespace testingB {

class Hisser {
    // DATA
    unsigned d_hissLen;

  public:
    // CREATOR
    explicit
    Hisser(unsigned hissLen)
    : d_hissLen(hissLen)
    {}

    // ACCESSOR
    unsigned hissLen() const
    {
        return d_hissLen;
    }
};

void PrintTo(const Hisser& hisser, ::std::ostream *stream)
{
    *stream << '"';

    for (unsigned uu = 0; uu < hisser.hissLen(); ++uu) {
        *stream << 's';
    }

    *stream << '"';
}

}  // close namespace testingB

// ============================================================================
//            SIMULATE COMPETING 'PrintTo' DECLARATIONS IN GTEST
// ----------------------------------------------------------------------------

template <typename T>
void PrintTo(const T& , ::std::ostream* )
{
    ASSERT(0);
}

void PrintTo(unsigned char, ::std::ostream* )
{
    ASSERT(0);
}

void PrintTo(signed char , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(char , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(bool , ::std::ostream* )
{
    ASSERT(0);
}

void PrintTo(wchar_t , ::std::ostream* )
{
    ASSERT(0);
}

void PrintTo(const char* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(char* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const signed char* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(signed char* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const unsigned char* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(unsigned char* , ::std::ostream* )
{
    ASSERT(0);
}

void PrintTo(const wchar_t* , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(wchar_t* , ::std::ostream* )
{
    ASSERT(0);
}

class string;
inline
void PrintTo(const ::string& , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const ::std::string& , ::std::ostream* )
{
    ASSERT(0);
}

class wstring;
inline
void PrintTo(const ::wstring& , ::std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const ::std::wstring& , ::std::ostream* )
{
    ASSERT(0);
}

bsl::string expStringViewPrint(const bsl::string_view& sv)
{
    bsl::string ret = "\"";
    bsl::size_t numZeroes = bsl::count(sv.begin(), sv.end(), '\0');

    ret.reserve(sv.length() + 2 + numZeroes);

    for (unsigned uu = 0; uu < sv.length(); ++uu) {
        if ('\0' == sv[uu]) {
            ret += "\\0";
        }
        else {
            ret += sv[uu];
        }
    }
    ret += '"';

    return ret;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = (argc > 1) ? bsl::atoi(argv[1]) : 1;
    bool             verbose = (argc > 2); (void)verbose;
    bool         veryVerbose = (argc > 3); (void)veryVerbose;
    bool     veryVeryVerbose = (argc > 4); (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = (argc > 5); (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    BloombergLP::bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 7 : {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Demonstrate the usage of this component.
        //
        // Plan:
        //: 1 Using 'PrintTo' to output a string.
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Usage
///-----
// Suppose we have a string 'str' that we want to output:
//..
    bsl::string str =
                   "No matter where you go, There you are! -- Buckaroo Banzai";
//..
// Call 'PrintTo', passing the string and a pointer to a 'bsl::ostream':
//..
    PrintTo(str, &cout);
    cout << endl;
//..
// Which results in the string being streamed to standard output, surrounded by
// double quotes:
//..
//  "No matter where you go, There you are! -- Buckaroo Banzai"
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ADL WITHIN 'PrintTo(optional<TYPE>, ...)'
        //
        // Concern:
        //: 1 That, if type and a 'PrintTo' for it are defined in 'testing' or
        //:   another namespace, ADL will properly dispatch the call to
        //:   'UniversalPrint' within 'PrintTo(optional<TYPE>, ...)'.
        //
        // Plan:
        //: 1 Define the 'class' 'Hisser' in namespace 'testing' declare an
        //:   'optional' object containing one.
        //:
        //: 2 Stream the 'optional' to an 'ostringstream' and verify that the
        //:   output is as expected.
        //:
        //: 3 Define the 'class' 'Hisser' in namespace 'testingB' declare an
        //:   'optional' object containing one.
        //:
        //: 4 Stream the 'optional' to an 'ostringstream' and verify that the
        //:   output is as expected.
        //:
        //: 5 Declare an 'optional' containing an 'int'.
        //:
        //: 6 Stream the 'optional' to an 'ostringstream' and verify that the
        //:   output is as expected.
        //
        // Testing:
        //   void PrintTo(const bsl::optional<TYPE>& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "TESTING ADL WITHIN 'PrintTo(optional<TYPE>, ...)'\n"
                        "=================================================\n";

        if (verbose) cout << "TYPE in 'testing'\n";
        {
            const testing::Hisser hisser(4);
            const bsl::optional<testing::Hisser> opt(hisser);

            bsl::ostringstream oss;

            PrintTo(opt, &oss);

            ASSERTV(oss.str(), "(\"ssss\")" == oss.str());
        }

        if (verbose) cout << "TYPE in namespace other than 'testing'\n";
        {
            const testingB::Hisser hisserB(7);
            const bsl::optional<testingB::Hisser> optB(hisserB);

            bsl::ostringstream oss;

            PrintTo(optB, &oss);

            ASSERTV(oss.str(), "(\"sssssss\")" == oss.str());
        }

        if (verbose) cout << "Fundamental TYPE, PrintTo in 'testing'\n";
        {
            const bsl::optional<int> optI(207);

            bsl::ostringstream oss;

            PrintTo(optI, &oss);

            ASSERTV(oss.str(), "(207)" == oss.str());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'PrintTo' FOR 'bsl::optional'
        //
        // Concerns:
        //: 1 The 'PrintTo' correctly handles empty 'bsl::optional' objects.
        //:
        //: 2 The 'PrintTo' correctly writes any non-empty 'bsl::optional'
        //:   object's value to the stream.
        //
        // Plan:
        //: 1 Create several 'bsl::optional' objects, having different value
        //:   types and values.  Print them to the stream using 'PrintTo'
        //:   function and verify the result.  (C-1..2)
        //
        // Testing:
        //   void PrintTo(const bsl::optional<TYPE>& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'PrintTo' FOR 'bsl::optional'"
                          << "\n=====================================\n";

        if (veryVerbose) cout << "Testing default-constructed 'bsl::optional'."
                              << endl;
        {
            bsl::optional<int>                     mXI;
            const bsl::optional<int>&              XI = mXI;
            bsl::optional<bsl::string>             mXS;
            const bsl::optional<bsl::string>&      XS = mXS;
            bsl::optional<bsl::string_view>        mXSV;
            const bsl::optional<bsl::string_view>& XSV = mXSV;

            bsl::ostringstream ossI;
            bsl::ostringstream ossS;
            bsl::ostringstream ossSV;

            bsl::PrintTo(XI,  &ossI );
            bsl::PrintTo(XS,  &ossS );
            bsl::PrintTo(XSV, &ossSV);

            ASSERTV(ossI.good());
            ASSERTV(ossS.good());
            ASSERTV(ossSV.good());

            ASSERTV(ossI.str(),  EMPTY_OPTIONAL_EXPECTED == ossI.str() );
            ASSERTV(ossS.str(),  EMPTY_OPTIONAL_EXPECTED == ossS.str() );
            ASSERTV(ossSV.str(), EMPTY_OPTIONAL_EXPECTED == ossSV.str());
        }

        if (veryVerbose) cout << "Testing 'bsl::optional<int>'." << endl;
        {
            bsl::optional<int>        mXI1(-1);
            bsl::optional<int>        mXI2( 0);
            bsl::optional<int>        mXI3( 1);
            const bsl::optional<int>& XI1 = mXI1;
            const bsl::optional<int>& XI2 = mXI2;
            const bsl::optional<int>& XI3 = mXI3;

            const bsl::string EXPECTED1("(-1)");
            const bsl::string EXPECTED2("(0)");
            const bsl::string EXPECTED3("(1)");

            bsl::ostringstream ossI1;
            bsl::ostringstream ossI2;
            bsl::ostringstream ossI3;

            bsl::PrintTo(XI1, &ossI1);
            bsl::PrintTo(XI2, &ossI2);
            bsl::PrintTo(XI3, &ossI3);

            ASSERTV(ossI1.good());
            ASSERTV(ossI2.good());
            ASSERTV(ossI3.good());

            ASSERTV(ossI1.str(), EXPECTED1 == ossI1.str());
            ASSERTV(ossI2.str(), EXPECTED2 == ossI2.str());
            ASSERTV(ossI3.str(), EXPECTED3 == ossI3.str());
        }

        if (veryVerbose) cout << "Testing 'bsl::optional<bsl::string>'."
                              << endl;
        {
            bsl::optional<bsl::string>        mXS1("");
            bsl::optional<bsl::string>        mXS2("1");
            bsl::optional<bsl::string>        mXS3(bsl::in_place,
                                                   NON_EMPTY_STRING,
                                                   NON_EMPTY_STRING_LENGTH);

            const bsl::optional<bsl::string>& XS1 = mXS1;
            const bsl::optional<bsl::string>& XS2 = mXS2;
            const bsl::optional<bsl::string>& XS3 = mXS3;

            const bsl::string EXPECTED1("(\"\")");
            const bsl::string EXPECTED2("(\"1\")");
            const bsl::string EXPECTED3 = bsl::string("(\"")
                                        + bsl::string(NON_EMPTY_STRING,
                                                      NON_EMPTY_STRING_LENGTH)
                                        + bsl::string("\")");

            bsl::ostringstream ossS1;
            bsl::ostringstream ossS2;
            bsl::ostringstream ossS3;

            bsl::PrintTo(XS1, &ossS1);
            bsl::PrintTo(XS2, &ossS2);
            bsl::PrintTo(XS3, &ossS3);

            ASSERTV(ossS1.good());
            ASSERTV(ossS2.good());
            ASSERTV(ossS3.good());

            ASSERTV(ossS1.str(), EXPECTED1 == ossS1.str());
            ASSERTV(ossS2.str(), EXPECTED2 == ossS2.str());
            ASSERTV(ossS3.str(), EXPECTED3 == ossS3.str());
        }

        if (veryVerbose) cout << "Testing 'bsl::optional<bsl::string_view>'."
                              << endl;
        {
            bsl::optional<bsl::string_view> mXSV1("");
            bsl::optional<bsl::string_view> mXSV2("1");
            const bsl::string_view SV3(NON_EMPTY_STRING,
                                       NON_EMPTY_STRING_LENGTH);
            bsl::optional<bsl::string_view> mXSV3(SV3);

            const bsl::optional<bsl::string_view>& XSV1 = mXSV1;
            const bsl::optional<bsl::string_view>& XSV2 = mXSV2;
            const bsl::optional<bsl::string_view>& XSV3 = mXSV3;

            const bsl::string EXPECTED1("(\"\")");
            const bsl::string EXPECTED2("(\"1\")");
            const bsl::string EXPECTED3 = "(" + expStringViewPrint(SV3) + ")";

            bsl::ostringstream ossSV1;
            bsl::ostringstream ossSV2;
            bsl::ostringstream ossSV3;

            bsl::PrintTo(XSV1, &ossSV1);
            bsl::PrintTo(XSV2, &ossSV2);
            bsl::PrintTo(XSV3, &ossSV3);

            ASSERTV(ossSV1.good());
            ASSERTV(ossSV2.good());
            ASSERTV(ossSV3.good());

            ASSERTV(ossSV1.str(), EXPECTED1 == ossSV1.str());
            ASSERTV(ossSV2.str(), EXPECTED2 == ossSV2.str());
            ASSERTV(ossSV3.str(), EXPECTED3, EXPECTED3 == ossSV3.str());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'PrintTo' FOR 'StringRef'
        //
        // Concerns:
        //: 1 The 'PrintTo' correctly writes an empty 'StringRef' object's
        //:   value to the stream.
        //:
        //: 2 The 'PrintTo' correctly writes 'StringRef' object's value
        //:   containing embedded null character to the stream.
        //:
        //: 3 The 'PrintTo' correctly writes any non-empty 'StringRef' object's
        //:   value to the stream.
        //
        // Plan:
        //: 1 Create several 'StringRef' objects, having different values.
        //:   Print them to the stream using 'PrintTo' function and verify the
        //:   result.  (C-1..3)
        //
        // Testing:
        //   void PrintTo(const bslstl::StringRef& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'PrintTo' FOR 'StringRef'"
                          << "\n=================================\n";

        typedef BloombergLP::bslstl::StringRef StringRef;

        if (veryVerbose) cout << "Testing default-constructed 'StringRef'."
                              << endl;
        {
            StringRef        mDES;
            const StringRef& DES = mDES;

            bsl::ostringstream ossDES;

            bsl::PrintTo(DES, &ossDES);

            ASSERTV(ossDES.good());
            ASSERTV(ossDES.str(), EMPTY_EXPECTED == ossDES.str());
        }

        if (veryVerbose) cout << "Testing empty 'StringRef'." << endl;
        {
            StringRef        mES(EMPTY_STRING);
            const StringRef& ES = mES;

            bsl::ostringstream ossES;

            bsl::PrintTo(ES, &ossES);

            ASSERTV(ossES.good());
            ASSERTV(ossES.str(), EMPTY_EXPECTED == ossES.str());
        }

        if (veryVerbose) cout << "Testing non-empty 'StringRef'." << endl;
        {
            for (const char *begin = NON_EMPTY_STRING;
                 begin != NON_EMPTY_STRING_END;
                 ++begin) {
                for (const char *end = begin;
                     end != NON_EMPTY_STRING_END + 1;
                     ++end) {
                    StringRef        mNES(begin, end);
                    const StringRef& NES = mNES;

                    bsl::ostringstream ossNES;
                    const bsl::string  EXPECTED_NES = bsl::string("\"")
                                                    + bsl::string(begin, end)
                                                    + bsl::string("\"");
                    bsl::PrintTo(NES, &ossNES);

                    ASSERTV(ossNES.good());
                    ASSERTV(EXPECTED_NES, ossNES.str(),
                            EXPECTED_NES == ossNES.str());
                }
            }
        }

        if (veryVerbose) cout << "Testing long 'StringRef'." << endl;
        {
            StringRef        mLS(LONG_STRING);
            const StringRef& LS = mLS;

            bsl::ostringstream ossLS;

            bsl::PrintTo(LS, &ossLS);

            ASSERTV(ossLS.good());
            ASSERTV(ossLS.str(), LONG_EXPECTED == ossLS.str());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'PrintTo' FOR 'wstring'
        //
        // Concerns:
        //: 1 The 'PrintTo' correctly writes an empty 'wstring' object's value
        //:   to the stream.
        //:
        //: 2 The 'PrintTo' correctly writes 'wstring' object's value
        //:   containing embedded null character to the stream.
        //:
        //: 3 The 'PrintTo' correctly writes any non-empty 'wstring' object's
        //:   value to the stream.
        //
        // Plan:
        //: 1 Create several 'wstring' objects, having different values.  Print
        //:   them to the stream using 'PrintTo' function and verify the
        //:   result.  (C-1..3)
        //
        // Testing:
        //   void PrintTo(const bsl::wstring& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'PrintTo' FOR 'wstring'"
                          << "\n===============================\n";

        if (veryVerbose) cout << "Testing default-constructed 'bsl::wstring'."
                              << endl;
        {
            bsl::wstring        mDES;
            const bsl::wstring& DES = mDES;

            bsl::ostringstream ossDES;

            bsl::PrintTo(DES, &ossDES);

            ASSERTV(ossDES.good());
            ASSERTV(ossDES.str(), EMPTY_EXPECTED == ossDES.str());
        }

        if (veryVerbose) cout << "Testing empty 'bsl::wstring'." << endl;
        {
            bsl::wstring        mES(EMPTY_WCHAR_STRING);
            const bsl::wstring& ES = mES;

            bsl::ostringstream ossES;

            bsl::PrintTo(ES, &ossES);

            ASSERTV(ossES.good());
            ASSERTV(ossES.str(), EMPTY_EXPECTED == ossES.str());
        }

        if (veryVerbose) cout << "Testing non-empty 'bsl::wstring'." << endl;
        {
            const int      EXTENDED_SYMBOL_SHIFT = sizeof(wchar_t) * 2;
            const char    *expectedBegin         = NON_EMPTY_WCHAR_EXPECTED;
            const wchar_t *begin                 = NON_EMPTY_WCHAR_STRING;

            while (begin != NON_EMPTY_WCHAR_STRING_END) {
                const char    *expectedEnd = expectedBegin;
                const wchar_t *end         = begin;

                while(end != NON_EMPTY_WCHAR_STRING_END) {
                    bsl::wstring        mNES(begin, end);
                    const bsl::wstring& NES = mNES;

                    bsl::ostringstream ossNES;
                    const bsl::string  EXPECTED_NES =
                                        bsl::string("\"")
                                      + bsl::string(expectedBegin, expectedEnd)
                                      + bsl::string("\"");

                    bsl::PrintTo(NES, &ossNES);

                    ASSERTV(ossNES.good());
                    ASSERTV(EXPECTED_NES, ossNES.str(),
                            EXPECTED_NES == ossNES.str());

                    ++end;
                    if (end != NON_EMPTY_WCHAR_STRING_END) {
                        // adjust the expected result for the next iteration

                        if (*(end - 1) > 0 && *(end - 1) < 128) {
                            // '1' -> '1'

                            expectedEnd += 1;
                        } else {
                            // '0' -> "\x00000000" || '0' -> "\x0000"

                            expectedEnd += (2 + EXTENDED_SYMBOL_SHIFT);
                        }
                    }
                }

                ++begin;
                if (begin != NON_EMPTY_WCHAR_STRING_END) {
                    // adjust the expected result for the next iteration

                    if (*(begin - 1) > 0 && *(begin - 1) < 128) {
                        // '1' -> '1'

                        expectedBegin += 1;
                    } else {
                        // '0' -> "\x00000000" || '0' -> "\x0000"

                        expectedBegin += (2 + EXTENDED_SYMBOL_SHIFT);
                    }
                }
            }
        }

        if (veryVerbose) cout << "Testing long 'bsl::wstring'." << endl;
        {
            bsl::wstring        mLS(LONG_WCHAR_STRING);
            const bsl::wstring& LS = mLS;

            bsl::ostringstream ossLS;

            bsl::PrintTo(LS, &ossLS);

            ASSERTV(ossLS.good());
            ASSERTV(ossLS.str(), LONG_EXPECTED == ossLS.str());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'PrintTo' FOR 'string'
        //
        // Concerns:
        //: 1 The 'PrintTo' correctly writes an empty 'string' object's value
        //:   to the stream.
        //:
        //: 2 The 'PrintTo' correctly writes 'string' object's value containing
        //:   embedded null character to the stream.
        //:
        //: 3 The 'PrintTo' correctly writes any non-empty 'string' object's
        //:   value to the stream.
        //
        // Plan:
        //: 1 Create several 'string' objects, having different values.  Print
        //:   them to the stream using 'PrintTo' function and verify the
        //:   result.  (C-1..3)
        //
        // Testing:
        //   void PrintTo(const bsl::string& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'PrintTo' FOR 'string'"
                          << "\n==============================\n";

        if (veryVerbose) cout << "Testing default-constructed 'bsl::string'."
                              << endl;
        {
            bsl::string        mDES;
            const bsl::string& DES = mDES;

            bsl::ostringstream ossDES;

            bsl::PrintTo(DES, &ossDES);

            ASSERTV(ossDES.good());
            ASSERTV(ossDES.str(), EMPTY_EXPECTED == ossDES.str());
        }

        if (veryVerbose) cout << "Testing empty 'bsl::string'." << endl;
        {
            bsl::string        mES(EMPTY_STRING);
            const bsl::string& ES = mES;

            bsl::ostringstream ossES;

            bsl::PrintTo(ES, &ossES);

            ASSERTV(ossES.good());
            ASSERTV(ossES.str(), EMPTY_EXPECTED == ossES.str());
        }

        if (veryVerbose) cout << "Testing non-empty 'bsl::string'." << endl;
        {
            for (const char *begin = NON_EMPTY_STRING;
                 begin != NON_EMPTY_STRING_END;
                 ++begin) {
                for (const char *end = begin;
                     end != NON_EMPTY_STRING_END + 1;
                     ++end) {
                    bsl::string        mNES(begin, end);
                    const bsl::string& NES = mNES;

                    bsl::ostringstream ossNES;
                    const bsl::string  EXPECTED_NES = bsl::string("\"")
                                                    + bsl::string(begin, end)
                                                    + bsl::string("\"");
                    bsl::PrintTo(NES, &ossNES);

                    ASSERTV(ossNES.good());
                    ASSERTV(EXPECTED_NES, ossNES.str(),
                            EXPECTED_NES == ossNES.str());
                }
            }
        }

        if (veryVerbose) cout << "Testing long 'bsl::string'." << endl;
        {
            bsl::string        mLS(LONG_STRING);
            const bsl::string& LS = mLS;

            bsl::ostringstream ossLS;

            bsl::PrintTo(LS, &ossLS);

            ASSERTV(ossLS.good());
            ASSERTV(ossLS.str(), LONG_EXPECTED == ossLS.str());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 That the function can stream a string.
        //:
        //: 2 If the stream is invalid, the function will not modify the
        //:   stream.
        //
        // Plan:
        //: 1 Stream a couple of strings to an 'ostringtream' and observe the
        //:   result.
        //:
        //: 2 Set the state of the 'ostringstream' to invalid, call the
        //:   function again, and observe that the stream has not been
        //:   modified.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        bsl::ostringstream oss(&ta);
        bsl::string s("hello, world.\n", &ta);

        PrintTo(s, &oss);
        ASSERT(oss.good());
        bsl::string expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        s.assign("The rain in Spain falls mainly in the plain.\n");
        PrintTo(s, &oss);
        ASSERT(oss.good());
        expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        oss << "woof";
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(oss.good());
        oss.setstate(bsl::ios_base::failbit);
        ASSERT(!oss.good());
        PrintTo(s, &oss);
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(!oss.good());

        oss.clear();
        oss.str("");
        bsl::wstring ws(&ta);
        ws.push_back(wchar_t(0x80));
        ws.push_back(wchar_t(0xabcd));
        ws.push_back('m');
        ws.push_back('e');
        ws.push_back('o');
        ws.push_back('w');
        ws.push_back(wchar_t(0xf1d9));
        ws.push_back(wchar_t(7));        // bell (non-printable ascii)

        PrintTo(ws, &oss);

        ASSERTV(sizeof(wchar_t), 2 == sizeof(wchar_t) || 4 == sizeof(wchar_t));

        const char *exp = 2 == sizeof(wchar_t)
                      ? "\"\\x0080\\xabcdmeow\\xf1d9\\x0007\""
                      : "\"\\x00000080\\x0000abcdmeow\\x0000f1d9\\x00000007\"";
        ASSERTV(oss.str(), exp, oss.str() == exp);

        if (verbose) { P_(sizeof(wchar_t));    P(oss.str()); }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
