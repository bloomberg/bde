// bdljsn_tokenizer.t.cpp                                             -*-C++-*-
#include <bdljsn_tokenizer.h>

#include <bdljsn_numberutil.h>
#include <bdljsn_stringutil.h>

#include <bdlde_utf8util.h>

#include <bdlsb_fixedmeminstreambuf.h>   // for testing only
#include <bdlsb_fixedmemoutstreambuf.h>  // for testing only
#include <bdlsb_memoutstreambuf.h>       // for testing only

#include <bsla_maybeunused.h>

#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a tokenizer for traversing a stream
// filled with JSON data and allows populating an in-memory structure with
// almost no memory allocations.  The implementation works as a finite state
// machine moving from one token to another when the 'advanceToNextToken'
// function is called.  The majority of this test driver tests that function by
// starting at a particular token, calling that function, and ensuring that
// after the advance the next token and the data value is as expected.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bdljsn::Tokenizer(bslma::Allocator *bA = 0);
// [ 2] ~bdljsn::Tokenizer();
//
// MANIPULATORS
// [10] void reset(bsl::streambuf &streamBuf);
// [13] void resetStreamBufGetPointer();
// [14] void setAllowStandAloneValues(bool value);
// [15] void setAllowHeterogenousArrays(bool value);
// [ 2] int advanceToNextToken();
// [ 3] int setAllowTrailingTopLevelComma();
//
// ACCESSORS
// [ 4] TokenType tokenType() const;
// [14] bool allowStandAloneValues() const;
// [ 3] bool allowTrailingTopLevelComma() const;
// [15] bool allowHeterogenousArrays() const;
// [ 4] int value(bslstl::StringRef *data) const;
// [18] bool utf8ErrorIsSet() const;
// [18} const char *utf8ErrorMessage(const char *) const;
// [18] void setAllowNonUtf8StringLiterals(bool);
// [18] bool allowNonUtf8StringLiterals() const;
// [19] Uint64 currentPosition() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [20] USAGE EXAMPLE

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
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#define WS "   \t       \n      \v       \f       \r       "
const bsl::size_t WSLEN = sizeof(WS) - 1;

typedef bdljsn::Tokenizer   Obj;
typedef bdlde::Utf8Util     Utf8Util;
typedef bsls::Types::IntPtr IntPtr;
typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

const char *LARGE_STRING_C_STR =
            "\"selection11selection1element1255element43123123123elementasdd52"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.51.5element4ent2tue"
            "element5-980123-980123element62012-08-18T132500.000+0000ement2tue"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemesdfnt1"
            "element2element4element5element1element1element2element4elemendt5"
            "element1element1element2element4element6LONDONLONDONelement2trdue"
            "trueelement31.51.5element4element5-980123-980123element6ent2tuesd"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7sdsdd"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+00005element4element5"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "selection11selection1element1255element43123123123elementasdd5234"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+0000980123elemefnt608"
            "2012-08-18T132500.000+0000element7element62012-08-18T132501230000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelems12ent1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000df"
            "selection11selection1element1255element43123123123elementasdd5255"
            "element6999999element7customelement8999element10255255elementsd11"
            "element12element13255255element14element1531231231233123123112323"
            "selection6arbitrarystringvalueselection7element1element1elements2"
            "element4element5element1element1element2element4element5elemenst1"
            "element1element2element4element6LONDONLONDONelement2truetruement6"
            "element31.51.5element4element5-980123-980123element6elemen123t608"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7ment6"
            "element6LONDONLONDONelement2truetrueelement31.51.5elemenst4ment68"
            "element5-980123-980123element62012-08-18T132500.000+0000element68"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelem123ent1"
            "element2element4element5element1element1element2element4edlement5"
            "element1element1element2element4element6LONDONLONDONelemendt2true"
            "trueelement31.51.5element4element5-980123-980123element6ement2tre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elemement2nt7"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+0000980123elemefnt608"
            "2012-08-18T132500.000+0000element7element62012-08-18T132501230000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelems12ent1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000df"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemensaft2"
            "selection11selection1element1255element43123123123elemensdfft5255"
            "element6999999element7customelement8999element10255255elementdf11"
            "element12element13255255element14element15312312312331231231d2dd3"
            "selection6arbitrarystringvalueselection7element1element1elemednt2"
            "element4element5element1element1element2element4element5elemdent1"
            "element1element2element4element6LONDONLONDONelement2truasdfdetrue"
            "element31.51.5element4element5-980123-980123element6ement2truetre"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000elasdffement7"
            "element6LONDONLONDONelement2truetrueelement31.51.5element4ent2tue"
            "element5-980123-980123element62012-08-18T132500.000+0000ement2tue"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemesdfnt1"
            "element2element4element5element1element1element2element4elemendt5"
            "element1element1element2element4element6LONDONLONDONelement2trdue"
            "trueelement31.51.5element4element5-980123-980123element6ent2tuesd"
            "2012-08-18T132500.000+00002012-08-18T132500.000+0000element7sdsdd"
            "element6LONDONLONDONelement2truetrueelement31.515element4element5"
            "-980123-980123element62012-08-18T132500.000+00005element4element5"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element62012-08-18Tsdf132500000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeldfsement1"
            "element2element4element5element1element1element2element4eledment5"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "element1element1element2element4element6LONDONLONDONelement2dtrue"
            "trueelement31.51.5element4element5-980123-980123element6201+0d000"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONeleme2trdue"
            "trueelement31.51.5element4element5-980123-980123element62010000ds"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LONDONLONDONelemsdfent2"
            "2012-08-18T132500.000+0000element7element6LOND12345678";

struct BreakAllocator : public bslma::TestAllocator {
    // CREATOR
    // BDE_VERIFY pragma: -FD01
    BreakAllocator() : bslma::TestAllocator() {}

    // MANIPULATOR
    // BDE_VERIFY pragma: -FD01
    virtual void *allocate(size_type size)
    {
        return bslma::TestAllocator::allocate(size);
    }
};

bsl::ostream& operator<<(bsl::ostream& stream, Obj::TokenType value)
{
#define CASE(X) case(Obj::X): stream << #X; break;

    switch (value) {
      // BDE_VERIFY pragma: -IND01
      CASE(e_BEGIN)
      CASE(e_ELEMENT_NAME)
      CASE(e_START_OBJECT)
      CASE(e_END_OBJECT)
      CASE(e_START_ARRAY)
      CASE(e_END_ARRAY)
      CASE(e_ELEMENT_VALUE)
      CASE(e_ERROR)
      default: stream << "(* UNKNOWN *)"; break;
    }
    return stream;

#undef CASE
}

void confirmStreamBufReset(bsl::streambuf     *sb,
                           int                 LINE,
                           int                 NADV,
                           int                 NAVAIL,
                           const bsl::string&  EXPECTED,
                           bool                sbEmptyInitially,
                           bool                checkUtf8,
                           bool                checkNumAvailAfterReset = true)
{
    Obj mX;  const Obj& X = mX;
    ASSERTV(LINE, X.tokenType(), Obj::e_BEGIN == X.tokenType());

    mX.reset(sb);
    mX.setAllowNonUtf8StringLiterals(!checkUtf8);

    for (int i = 0; i < NADV; ++i) {
        ASSERTV(LINE, i, 0 == mX.advanceToNextToken());
    }

    bsl::streamsize numAvail = sb->in_avail();
    if (sbEmptyInitially) {
        ASSERTV(LINE, numAvail, (0 <= numAvail && numAvail < 4) ||
                                                               -1 == numAvail);
    }

    int rc = mX.resetStreamBufGetPointer();
    ASSERTV(LINE, rc, 0 == rc);

    if (NAVAIL > 0) {
        if (checkNumAvailAfterReset) {
            numAvail = sb->in_avail();
            ASSERTV(LINE, numAvail, NAVAIL, NAVAIL == numAvail);
        }

        bsl::string actual;
        actual.resize(NAVAIL);

        bsl::streamsize rc = sb->sgetn(&actual[0], NAVAIL);
        ASSERTV(LINE, NAVAIL, rc, NAVAIL == rc);
        ASSERTV(LINE, EXPECTED, actual, EXPECTED == actual);
    }
}

const Utf8Util::ErrorStatus EIT = Utf8Util::k_END_OF_INPUT_TRUNCATION;
const Utf8Util::ErrorStatus UCO = Utf8Util::k_UNEXPECTED_CONTINUATION_OCTET;
const Utf8Util::ErrorStatus NCO = Utf8Util::k_NON_CONTINUATION_OCTET;
const Utf8Util::ErrorStatus OLE = Utf8Util::k_OVERLONG_ENCODING;
const Utf8Util::ErrorStatus IIO = Utf8Util::k_INVALID_INITIAL_OCTET;
const Utf8Util::ErrorStatus VLT = Utf8Util::k_VALUE_LARGER_THAN_0X10FFFF;
const Utf8Util::ErrorStatus SUR = Utf8Util::k_SURROGATE;

static const struct Utf8Data {
    int         d_lineNum;    // source line number

    const char *d_utf8_p;     // UTF-8 input string

    int         d_status;
                              // number of UTF-8 code points (or "ErrorStatus'
                              // if invalid.  Note that all the 'ErrorStatus'
                              // enums are -ve.

    int         d_errOffset;  // byte offset to first invalid sequence;
                              // -1 if valid
} UTF8_DATA[] = {
    { L_, "",                               0, -1   },
    { L_, "Hello",                          5, -1   },
    { L_, "\x80",                         UCO,  0   },
    { L_, "\x85p",                        UCO,  0   },
    { L_, "a\x85",                        UCO,  1   },
    { L_, "\x90",                         UCO,  0   },
    { L_, " \x91",                        UCO,  1   },
    { L_, "\x9a",                         UCO,  0   },
    { L_, "\xf0",                         EIT,  0   },                        \
    { L_, "\xf0\x80",                     EIT,  0   },
    { L_, "\xf0\x80\x80",                 EIT,  0   },
    { L_, "\xf0 ",                        NCO,  0   },
    { L_, "\xf0\x80 ",                    NCO,  0   },
    { L_, "\xf0\x80\x80 ",                NCO,  0   },
    { L_, "\xe0\x80",                     EIT,  0   },
    { L_, "\xe0",                         EIT,  0   },
    { L_, "\xe0\x80 ",                    NCO,  0   },
    { L_, "\xe0 ",                        NCO,  0   },
    { L_, "\xf8\xaf\xaf\xaf",             IIO,  0   },
    { L_, "\xf8\x80\x80\x80",             IIO,  0   },
    { L_, "\xf8",                         IIO,  0   },
    { L_, "\xf9",                         IIO,  0   },
    { L_, "\xf0\x80\x80\x80",             OLE,  0   },
    { L_, "\xf0\x8a\xaa\xaa",             OLE,  0   },
    { L_, "\xf0\x8f\xbf\xbf",             OLE,  0   },    // max OLE
    { L_, "\xf0\x90\x80\x80",               1, -1   },    // min legal
    { L_, "\xf1\x80\x80\x80",               1, -1   },    // norm legal
    { L_, "\xf1\xaa\xaa\xaa",               1, -1   },    // norm legal
    { L_, "\xf4\x8f\xbf\xbf",               1, -1   },    // max legal
    { L_, "\xf4\x90\x80\x80",             VLT,  0   },    // min VLT
    { L_, "\xf4\x90\xbf\xbf",             VLT,  0   },    //     VLT
    { L_, "\xf4\xa0\x80\x80",             VLT,  0   },    //     VLT
    { L_, "\xf7\xbf\xbf\xbf",             VLT,  0   },    // max VLT

    { L_, "\xe0\x80\x80",                 OLE,  0   },
    { L_, "\xe0\x9a\xaa",                 OLE,  0   },
    { L_, "\xe0\x9f\xbf",                 OLE,  0   },    // max OLE
    { L_, "\xe0\xa0\x80",                   1, -1   },    // min legal
    { L_, "\xef\xbf\xbf",                   1, -1   },    // max

    { L_, "\xc0\x80",                     OLE,  0   },
    { L_, "\xc0\xaa",                     OLE,  0   },
    { L_, "\xc0\xbf",                     OLE,  0   },
    { L_, "\xc1\x81",                     OLE,  0   },
    { L_, "\xc1\xbf",                     OLE,  0   },    // max OLE
    { L_, "\xc2\x80",                       1, -1   },    // min legal
    { L_, "\xd0\xb0",                       1, -1   },
    { L_, "\xdf\xbf",                       1, -1   },    // max

    { L_, "\xc2",                         EIT,  0   },
    { L_, " \xc2",                        EIT,  1   },
    { L_, "\xc2 ",                        NCO,  0   },
    { L_, "\xc2\xc2",                     NCO,  0   },
    { L_, "\xc2\xef",                     NCO,  0   },

    { L_, "\xef",                         EIT,  0   },
    { L_, " \xef",                        EIT,  1   },
    { L_, "\xef ",                        NCO,  0   },
    { L_, "\xef\xef ",                    NCO,  0   },
    { L_, "\xef \xef",                    NCO,  0   },

    { L_, "\xef\xbf",                     EIT,  0   },
    { L_, "\xef\xbf",                     EIT,  0   },
    { L_, " \xef\xbf@",                   NCO,  1   },
    { L_, " \xef\xbf@",                   NCO,  1   },
    { L_, "\xef\xbf ",                    NCO,  0   },
    { L_, "\xef\xbf ",                    NCO,  0   },
    { L_, "\xef\xbf\xef",                 NCO,  0   },

    { L_, "\xed\xa0\x80",                 SUR,  0   },
    { L_, "\xed\xb0\x85 ",                SUR,  0   },
    { L_, "\xed\xbf\xbf",                 SUR,  0   },

    { L_, LARGE_STRING_C_STR + 1,        8177, -1   },

    { L_, "123456\xe0\x80\x80",           OLE,  6   },
    { L_, "1234567\xe0\x80\x80",          OLE,  7   },
    { L_, "12345678\xe0\x80\x80",         OLE,  8   },
    { L_, "12345678A\xe0\x80\x80",        OLE,  9   },
    { L_, "12345678AB\xe0\x80\x80",       OLE, 10   },
    { L_, "12345678ABC\xe0\x80\x80",      OLE, 11   },

    { L_, "1\xef\xbf\xbf",                  2, -1   },
    { L_, "12\xef\xbf\xbf",                 3, -1   },
    { L_, "123\xef\xbf\xbf",                4, -1   },
    { L_, "1234\xef\xbf\xbf",               5, -1   },
    { L_, "12345\xef\xbf\xbf",              6, -1   },
    { L_, "123456\xef\xbf\xbf",             7, -1   },
    { L_, "1234567\xef\xbf\xbf",            8, -1   },
    { L_, "12345678\xef\xbf\xbf",           9, -1   },
    { L_, "12345678A\xef\xbf\xbf",         10, -1   },
    { L_, "12345678AB\xef\xbf\xbf",        11, -1   },
    { L_, "12345678ABC\xef\xbf\xbf",       12, -1   },

    { L_, "1\xf1\xaa\xaa\xaa",              2, -1   },
    { L_, "12\xf1\xaa\xaa\xaa",             3, -1   },
    { L_, "123\xf1\xaa\xaa\xaa",            4, -1   },
    { L_, "1234\xf1\xaa\xaa\xaa",           5, -1   },
    { L_, "12345\xf1\xaa\xaa\xaa",          6, -1   },
    { L_, "123456\xf1\xaa\xaa\xaa",         7, -1   },
    { L_, "1234567\xf1\xaa\xaa\xaa",        8, -1   },
    { L_, "12345678\xf1\xaa\xaa\xaa",       9, -1   },
    { L_, "12345678A\xf1\xaa\xaa\xaa",     10, -1   },
    { L_, "12345678AB\xf1\xaa\xaa\xaa",    11, -1   },
    { L_, "12345678ABC\xf1\xaa\xaa\xaa",   12, -1   },
    { L_, "12345678ABCD\xf1\xaa\xaa\xaa",  13, -1   },
    { L_, "12345678ABCDE\xf1\xaa\xaa\xaa", 14, -1   },

    { L_, "1\xf4\xa0\x80\x80",            VLT,  1   },
    { L_, "12\xf4\xa0\x80\x80",           VLT,  2   },
    { L_, "123\xf4\xa0\x80\x80",          VLT,  3   },
    { L_, "1234\xf4\xa0\x80\x80",         VLT,  4   },
    { L_, "12345\xf4\xa0\x80\x80",        VLT,  5   },
    { L_, "123456\xf4\xa0\x80\x80",       VLT,  6   },
    { L_, "1234567\xf4\xa0\x80\x80",      VLT,  7   },
    { L_, "12345678\xf4\xa0\x80\x80",     VLT,  8   },
    { L_, "12345678A\xf4\xa0\x80\x80",    VLT,  9   },
    { L_, "12345678AB\xf4\xa0\x80\x80",   VLT, 10   },
    { L_, "12345678ABC\xf4\xa0\x80\x80",  VLT, 11   },
    { L_, "12345678ABCD\xf4\xa0\x80\x80", VLT, 12   },
    { L_, "12345678ABCDE\xf4\xa0\x80\x80",VLT, 13   },
};
enum { k_NUM_UTF8_DATA = sizeof UTF8_DATA / sizeof *UTF8_DATA };

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Extracting JSON data into an object
///----------------------------------------------
// For this example, we will use 'bdljsn::Tokenizer' to read each node in a
// JSON document and populate a simple 'Employee' object.
//
// First, we will define the JSON data that the tokenizer will traverse over:
//..
    const char *INPUT = "    {\n"
                        "        \"street\" : \"Lexington Ave\",\n"
                        "        \"state\" : \"New York\",\n"
                        "        \"zipcode\" : \"10022-1331\",\n"
                        "        \"floorCount\" : 55\n"
                        "    }";
//..
// Next, we will construct populate a 'streambuf' with this data:
//..
    bdlsb::FixedMemInStreamBuf isb(INPUT, bsl::strlen(INPUT));
//..
// Then, we will create a 'bdljsn::Tokenizer' object and associate the above
// streambuf with it:
//..
    bdljsn::Tokenizer tokenizer;
    tokenizer.reset(&isb);
//..
// Next, we will create an address record type and object.
//..
    struct Address {
        bsl::string d_street;
        bsl::string d_state;
        bsl::string d_zipcode;
        int         d_floorCount;
    } address = { "", "", "", 0 };
//..
// Then, we will traverse the JSON data one node at a time:
//..
    // Read '{'

    int rc = tokenizer.advanceToNextToken();
    ASSERT(!rc);

    bdljsn::Tokenizer::TokenType token = tokenizer.tokenType();
    ASSERT(bdljsn::Tokenizer::e_START_OBJECT == token);

    rc = tokenizer.advanceToNextToken();
    ASSERT(!rc);
    token = tokenizer.tokenType();

    // Continue reading elements till '}' is encountered

    while (bdljsn::Tokenizer::e_END_OBJECT != token) {
        ASSERT(bdljsn::Tokenizer::e_ELEMENT_NAME == token);

        // Read element name

        bslstl::StringRef nodeValue;
        rc = tokenizer.value(&nodeValue);
        ASSERT(!rc);

        bsl::string elementName = nodeValue;

        // Read element value

        int rc = tokenizer.advanceToNextToken();
        ASSERT(!rc);

        token = tokenizer.tokenType();
        ASSERT(bdljsn::Tokenizer::e_ELEMENT_VALUE == token);

        rc = tokenizer.value(&nodeValue);
        ASSERT(!rc);

        // Extract the simple type with the data

        if (elementName == "street") {
            rc = bdljsn::StringUtil::readString(&address.d_street,
                    nodeValue);
            ASSERT(!rc);
        }
        else if (elementName == "state") {
            rc = bdljsn::StringUtil::readString(&address.d_state,
                    nodeValue);
            ASSERT(!rc);
        }
        else if (elementName == "zipcode") {
            rc = bdljsn::StringUtil::readString(&address.d_zipcode, nodeValue);
            ASSERT(!rc);
        }
        else if (elementName == "floorCount") {
            rc = bdljsn::NumberUtil::asInteger(&address.d_floorCount,
                                               nodeValue);
            ASSERT(!rc);
        }

        rc = tokenizer.advanceToNextToken();
        ASSERT(!rc);
        token = tokenizer.tokenType();
    }
//..
// Finally, we will verify that the 'address' aggregate has the correct values:
//..
    ASSERT("Lexington Ave" == address.d_street);
    ASSERT("New York"      == address.d_state);
    ASSERT("10022-1331"    == address.d_zipcode);
    ASSERT(55              == address.d_floorCount);
//..
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'currentPosition'
        //
        // Concerns:
        //: 1 In both valid and error states, the currentPosition is
        //:   correctly returned.
        //:
        // Plan:
        //: 1 Test various length valid json strings, making sure the
        //:   currentPosition is correctly reported at each step, taking
        //:   varying amounts of whitespace and token lengths into account.
        //:
        //: 2 Test various length invalid json strings, making sure the
        //:   currentPosition is correctly reported at each step and after
        //:   the failure, taking varying amounts of whitespace and token
        //:   lengths into account.
        //
        // Testing:
        //   Uint64 currentPosition() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING currentPosition" << "\n"
                 << "=======================" << endl;

        typedef int TOffsetList[20];

        struct TokenSequence {
            int         d_lineNum;
            bsl::string d_string;
            int         d_offsetCount;
            TOffsetList d_offsets;
        };

        const TokenSequence TOKEN_OFFSET_TESTS[] = {
             //L                            STRING  COUNT            OFFSETS
             //== ================================  =====  =================
            { L_,                               WS,     1,              { 0} }
          , { L_,                            "   ",     1,              { 0} }

          , { L_,                           "null",     2,           { 0, 4} }
          , { L_,                       "  null  ",     2,           { 0, 6} }
          , { L_,                      WS "null  ",     2,           { 0,
                                                                  WSLEN + 4} }

          , { L_,                           "true",     2,           { 0, 4} }
          , { L_,                       "  true  ",     2,           { 0, 6} }
          , { L_,                      WS "true  ",     2,           { 0,
                                                                  WSLEN + 4} }
          , { L_,                          "false",     2,           { 0, 5} }
          , { L_,                       "  false ",     2,           { 0, 7} }
          , { L_,                      WS "false ",     2,           { 0,
                                                                  WSLEN + 5} }

          , { L_,                           "-123",     2,           { 0, 4} }
          , { L_,                       "  -123  ",     2,           { 0, 6} }
          , { L_,                           "1234",     2,           { 0, 4} }
          , { L_,                       "  1234  ",     2,           { 0, 6} }
          , { L_,                          "1.0e1",     2,           { 0, 5} }
          , { L_,                       "  1.0e1 ",     2,           { 0, 7} }

          , { L_,                           "\"\"",     2,           { 0, 2} }
          , { L_,                       "  \"\"  ",     2,           { 0, 4} }
          , { L_,                          "\"a\"",     2,           { 0, 3} }
          , { L_,                       "  \"a\" ",     2,           { 0, 5} }

          , { L_,                             "[]",     3,         {0, 1, 2} }
          , { L_,                            " []",     3,         {0, 2, 3} }
          , { L_,                            "[],",     3,         {0, 1, 2} }
          , { L_,                      "[]   ,   ",     3,         {0, 1, 2} }
          , { L_,                      "[] , ,   ",     3,         {0, 1, 2} }
          , { L_,                           " [ ]",     3,         {0, 2, 4} }
          , { L_,                          "[ 1 ]",     4,      {0, 1, 3, 5} }
          , { L_,                         " [ 1 ]",     4,      {0, 2, 4, 6} }
          , { L_,                      "[ 1 , 2 ]",     5,   {0, 1, 3, 7, 9} }
          , { L_,                     " [ 1 , 2 ]",     5,  {0, 2, 4, 8, 10} }
          , { L_,                     "[ 1 , 2 ] ",     5,   {0, 1, 3, 7, 9} }
          , { L_,                    " [ 1 , 2 ] ",     5,  {0, 2, 4, 8, 10} }
          , { L_,               "[1, 2 ,3  ,  4 ]",     7, {0, 1, 2, 5, 8,
                                                                     14, 16} }
          , { L_,        " [1, 2 ,3  ,  4 " WS "]",     7, {0, 2, 3, 6, 9,
                                                             15, WSLEN + 17} }

          , { L_,                           "[[]]",     5,   {0, 1, 2, 3, 4} }
          , { L_,                     "[[" WS "]]",     5,   {0, 1, 2,
                                                       WSLEN + 3, WSLEN + 4} }
          , { L_,                         "[ [] ]",     5,   {0, 1, 3, 4, 6} }
          , { L_,                        " [ [] ]",     5,   {0, 2, 4, 5, 7} }
          , { L_,                        " [ {} ]",     5,   {0, 2, 4, 5, 7} }
          , { L_,                        "[[ 1 ]]",     6, {0, 1, 2, 4, 6,
                                                                          7} }
          , { L_,                  "[ [ 1 , 2 ] ]",     7, {0, 1, 3, 5, 9,
                                                                     11, 13} }

          , { L_,                             "{}",     3,         {0, 1, 2} }
          , { L_,                            " {}",     3,         {0, 2, 3} }
          , { L_,                            "{},",     3,         {0, 1, 2} }
          , { L_,                      "{}    ,  ",     3,         {0, 1, 2} }
          , { L_,                      "{} ,  ,  ",     3,         {0, 1, 2} }
          , { L_,                           " { }",     3,         {0, 2, 4} }
          , { L_,                   "{\"a\":null}",     5,  {0, 1, 4, 9, 10} }
          , { L_,                 "{\"a\":[null]}",     7, {0, 1, 4, 6, 10,
                                                                     11, 12} }
        };

        const int NUM_TOT = sizeof(TOKEN_OFFSET_TESTS) /
                            sizeof(TOKEN_OFFSET_TESTS[0]);
        for (int ti = 0; ti < NUM_TOT; ++ti) {
            const TokenSequence& DATA         = TOKEN_OFFSET_TESTS[ti];
            const int            LINE         = DATA.d_lineNum;
            const bsl::string&   STR          = DATA.d_string;
            const int            OFFSET_COUNT = DATA.d_offsetCount;
            const TOffsetList&   OFFSETS      = DATA.d_offsets;

            if (veryVerbose) {
                P_(ti);
                P_(LINE);
                P_(STR);
                P_(OFFSET_COUNT);
                cout << " OFFSETS = [";
                for (int i = 0; i < OFFSET_COUNT; ++i) {
                    cout << " " << OFFSETS[i];
                }
                cout << " ]" << endl;
            }

            for (int topLevelComma = 0; topLevelComma < 2; ++topLevelComma) {
                bsl::istringstream iss(STR);

                Obj        mX;
                const Obj& X = mX;
                mX.reset(iss.rdbuf());

                mX.setAllowTrailingTopLevelComma(bool(topLevelComma));

                ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
                ASSERTV(X.currentPosition(), 0 == X.currentPosition());
                ASSERTV(X.readOffset(), 0 == X.readOffset());

                int currOffsetIndex = 0;

                do {
                    const bsl::size_t CURR_OFFSET = OFFSETS[currOffsetIndex];
                    if (veryVeryVerbose) {
                        T_;
                        P_(LINE);
                        P_(CURR_OFFSET);
                        P_(topLevelComma);
                        P_(X.currentPosition());
                        P(X.tokenType());
                    }

                    ASSERTV(LINE, currOffsetIndex < OFFSET_COUNT);
                    ASSERTV(LINE,
                            currOffsetIndex,
                            OFFSET_COUNT,
                            X.currentPosition(),
                            CURR_OFFSET,
                            X.currentPosition() == CURR_OFFSET);

                    ++currOffsetIndex;
                } while (0 == mX.advanceToNextToken());

                ASSERTV(currOffsetIndex,
                        OFFSET_COUNT,
                        currOffsetIndex == OFFSET_COUNT);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING UTF-8
        //
        // Concerns:
        //: 1 That the tokenizer can accurately detect and report invalid
        //:   UTF-8.
        //:   o The tokenizer accurately identifies the type of error.
        //:
        //:   o The tokenizer accurately identifies the offset of the error.
        //:
        //: 2 That the tokenizer in UTF-8 mode can work on strings that fill up
        //:   more than one buffer, or that exactly fill up a buffer.
        //:
        //: 3 That the tokenizer will work on strings ending with a quad octet
        //:   UTF-8 sequence that exactly fills up the buffer.
        //:
        // Plan:
        //: 1 Create large table, 'UTF8_DATA' contain valid and invalid UTF-8
        //:   sequences, including a very large valid sequence that nearly
        //:   fills up the buffer.
        //:
        //: 2 Iterate through the table, visiting only valid UTF-8 strings.
        //:   o Nest a loop iterating through the same table, again visiting
        //:     only valid UTF-8 strings.
        //:
        //:   o Splice the strings from the inner and outer loops together,
        //:     surrounded by double quotes.
        //:
        //:   o Initialize a stringstream to the spliced string.
        //:
        //:   o Reset the tokenizer to the stringstream's 'streambuf'.
        //:
        //:   o Call 'advanceToNextToken' and observe:
        //:     1 It returns 0.
        //:
        //:     2 The tokenizer does not report a UTF-8 error.
        //:
        //:     3 Call 'value' on the tokenizer and observe the result matches
        //:       the spliced string.
        //:
        //:   o Finish the nested loop and start another nested loop, this time
        //:     iterating through invalid UTF-8 sequences.
        //:
        //:   o Create a string that is '"' + the valid string + the invalid
        //:     string.
        //:
        //:   o Initialize a stringstream to the spliced string.
        //:
        //:   o Reset the tokenizer to the stringstream's 'streambuf'.
        //:
        //:   o Call 'advanceToNextToken' and observe:
        //:     1 It does not return 0.
        //:
        //:     2 The tokenizer reports a UTF-8 error.
        //:
        //:     3 The tokenizer correctly reports the type of the error.
        //:
        //:     4 The tokenizer correctly reports the offset of the beginning
        //:       of the invalid UTF-8 code point.
        //:
        //:   o If the type of error was end of input truncation, append a '"'
        //:     to the string, initialize the stringstream to it, reset the
        //:     tokenizer to the 'streambuf', and repeat the above steps,
        //:     except this time expecting a non-continuation octet error.
        //
        // Testing:
        //   bool utf8ErrorIsSet() const;
        //   const char *utf8ErrorMessage(const char *) const;
        //   void setAllowNonUtf8StringLiterals(bool);
        //   bool allowNonUtf8StringLiterals() const;
        // --------------------------------------------------------------------

        bslma::TestAllocator sa;
        bsl::string          str(&sa);
        bsl::istringstream   iss(&sa);

        for (int ti = 0; ti < k_NUM_UTF8_DATA; ++ti) {
            const Utf8Data&   idata   = UTF8_DATA[ti];
            const int         ILINE   = idata.d_lineNum;
            const char       *IUTF8   = idata.d_utf8_p;
            const int         ISTATUS = idata.d_status;
            const int         IERROFF = idata.d_errOffset;

            ASSERT((0 <= ISTATUS) == (-1 == IERROFF));

            if (ISTATUS < 0) continue;

            // IUTF8 is valid UTF-8

            str.clear();
            str += '"';
            str += IUTF8;

            if (veryVerbose) { cout << "  ";    P(str); }

            const bsl::size_t ILEN = str.length();

            {
                int         sts;
                const char *end;
                IntPtr      numCodePoints =
                    Utf8Util::advanceIfValid(&sts, &end, str.c_str(), INT_MAX);

                ASSERT(0 <= numCodePoints);
                ASSERTV(ILINE, numCodePoints, ISTATUS + 1 == numCodePoints);
            }

            for (int tj = 0; tj < k_NUM_UTF8_DATA; ++tj) {
                const Utf8Data&    jdata   = UTF8_DATA[tj];
                const int          JLINE   = jdata.d_lineNum;
                const char        *JUTF8   = jdata.d_utf8_p;
                const int          JSTATUS = jdata.d_status;

                if (JSTATUS < 0) continue;

                str.resize(ILEN);
                str += JUTF8;
                str += '"';

                iss.str(str);

                Obj mX;  const Obj& X = mX;
                ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
                ASSERTV(X.allowStandAloneValues(), X.allowStandAloneValues());
                ASSERTV(X.allowNonUtf8StringLiterals(),
                                               X.allowNonUtf8StringLiterals());
                ASSERT(!X.readStatus());

                mX.reset(iss.rdbuf());
                mX.setAllowNonUtf8StringLiterals(false);

                ASSERTV(X.allowNonUtf8StringLiterals(),
                                              !X.allowNonUtf8StringLiterals());

                ASSERTV(ILINE, JLINE, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::e_ELEMENT_VALUE == X.tokenType());
                ASSERT(!X.readStatus());

                bslstl::StringRef sr;
                ASSERT(0 == X.value(&sr));
                ASSERTV(str, sr, str == sr);

                Uint64 sOff = iss.rdbuf()->pubseekoff(0,
                                                      bsl::ios_base::cur,
                                                      bsl::ios_base::in);
                ASSERT(str.length() == sOff);

                ASSERTV(ILINE, JLINE, 0 != mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::e_ERROR == X.tokenType());
                ASSERTV(bdljsn::Tokenizer::k_EOF == X.readStatus());
            }

            for (int tj = 0; tj < k_NUM_UTF8_DATA; ++tj) {
                const Utf8Data&    jdata   = UTF8_DATA[tj];
                const int          JLINE   = jdata.d_lineNum;
                const char        *JUTF8   = jdata.d_utf8_p;
                int                JSTATUS = jdata.d_status;
                const int          JERROFF = jdata.d_errOffset;

                if (0 <= JSTATUS) continue;

                const bsl::size_t  ERROFF  = JERROFF + ILEN;

                str.resize(ILEN);
                str += JUTF8;

                char                        errOffStr[256];
                bdlsb::FixedMemOutStreamBuf sb(errOffStr, sizeof(errOffStr));
                bsl::ostream                ostr(&sb);

                ostr << "UTF-8 error " << Utf8Util::toAscii(JSTATUS) <<
                                          " at offset " << ERROFF << bsl::ends;
                ASSERT(bsl::strlen(errOffStr) < sizeof(errOffStr));

                iss.str(str);

                Obj mX;  const Obj& X = mX;
                ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
                ASSERTV(X.allowStandAloneValues(), X.allowStandAloneValues());

                mX.reset(iss.rdbuf());
                mX.setAllowNonUtf8StringLiterals(false);

                ASSERTV(ILINE, JLINE, 0 != mX.advanceToNextToken());

                if (veryVeryVerbose) { cout << "    ";    P(str); }

                ASSERT(ERROFF  == X.readOffset());
                ASSERT(JSTATUS == X.readStatus());

                ASSERTV(ILINE, JLINE, 0 != mX.advanceToNextToken());

                ASSERT(ERROFF  == X.readOffset());
                ASSERT(JSTATUS == X.readStatus());
                Uint64 sOff = iss.rdbuf()->pubseekoff(0,
                                                      bsl::ios_base::cur,
                                                      bsl::ios_base::in);
                ASSERT(ERROFF == sOff);

                if (EIT == JSTATUS) {
                    // Now expect 'NCO' (Non Continuation Octet)

                    str += '"';
                    iss.str(str);

                    mX.reset(iss.rdbuf());
                    mX.setAllowNonUtf8StringLiterals(false);
                    ASSERTV(ILINE, JLINE, 0 != mX.advanceToNextToken());

                    ASSERT(ERROFF  == X.readOffset());
                    ASSERT(NCO == X.readStatus());

                    ASSERTV(ILINE, JLINE, 0 != mX.advanceToNextToken());

                    ASSERT(ERROFF  == X.readOffset());
                    ASSERT(NCO == X.readStatus());

                    Uint64 sOff = iss.rdbuf()->pubseekoff(0,
                                                          bsl::ios_base::cur,
                                                          bsl::ios_base::in);
                    ASSERT(ERROFF == sOff);
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING that arrays of heterogeneous types are handled correctly
        //
        // Concerns:
        //: 1 In an array, all nested types are allowed, in any order.
        //:   (DRQS 146756621)
        //:
        // Plan:
        //: 1 Exhaustively test all possible combinations of 1, 2, and 3-length
        //:   arrays containing an empty sub-array, empty sub-hash, string,
        //:   number, or bool in each of the positions.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING THAT ARRAYS OF HETEROGENEOUS TYPES ARE HANDLED "
                    "CORRECTLY" << "\n"
                 << "======================================================="
                    "=========" << endl;

        bsl::string values[] = {
            "{}"
          , "[]"
          , "1.234"
          , "\"str\""
          , "true"
        };

        enum { NUM_VALUES = sizeof(values) / sizeof(values[0]) };

        const bsl::string        COMMA = ", ";
        bsl::vector<bsl::string> candidates;
        candidates.reserve(NUM_VALUES * NUM_VALUES * NUM_VALUES * NUM_VALUES *
                           NUM_VALUES);

        enum { k_LENGTH_MOD = 6 };
        for (int ti = 0; ti < 2 * k_LENGTH_MOD; ++ti) {
            const bsl::size_t LENGTH     = ti % k_LENGTH_MOD;
            const bool        CHECK_UTF8 = k_LENGTH_MOD <= ti;

            bsl::string       separator  = "";

            candidates.clear();
            candidates.push_back("[ ");

            for (bsl::size_t i = 0; i < LENGTH; ++i) {
                bsl::vector<bsl::string> inputs = candidates;
                candidates.clear();

                for (bsl::size_t j = 0; j < inputs.size(); ++j) {
                    for (bsl::size_t k = 0; k < NUM_VALUES; ++k) {
                        candidates.push_back(inputs[j] + separator +
                                             values[k]);
                    }
                }

                separator = COMMA;
            }

            for (bsl::size_t i = 0; i < candidates.size(); ++i) {
                candidates[i] += " ]";

                if (veryVerbose) {
                    P_(i) P(candidates[i])
                }

                bsl::istringstream iss(candidates[i]);

                Obj mX;  const Obj& X = mX;
                ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());


                mX.reset(iss.rdbuf());
                mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

                bsl::size_t item_count = 0;

                while (0 == mX.advanceToNextToken()) {
                    switch (X.tokenType()) {
                      case Obj::e_ELEMENT_VALUE:
                      case Obj::e_END_OBJECT:
                      case Obj::e_END_ARRAY:
                        ++item_count;
                        break;
                      default:
                        break;
                    }
                }

                // '+ 1' for the closing outer array.
                ASSERTV(i,
                        candidates[i],
                        item_count,
                        LENGTH + 1,
                        LENGTH + 1 == item_count);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING that truncated data is handled correctly
        //
        // Concerns:
        //: 1 A stream of data that is truncated at any point -- within an
        //:   element name or value -- is correctly handled.
        //:
        //: 2 After advancing past the truncated data the tokenizer reflects
        //:   that it is in an error state.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text with an 'X' at the expected final
        //:   location, the number of 'advanceToNextToken' calls to be made,
        //:   the expected token type and data value.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'is', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'is' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the specified number of
        //:     times.
        //:
        //:   4 Confirm that the token type and value is as expected.
        //:
        //:   5 The next 'advanceToNextToken' should result in an error and
        //:     the state of 'mX' should also be reflected as the error state.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING THAT TRUNCATED DATA IS HANDLED CORRECTLY" << "\n"
                 << "================================================" << "\n";

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_numPreAdvances;
            Obj::TokenType  d_expTokenType;
            const char     *d_value_p;
        } DATA[] = {
            {L_,
             "",  // empty string
             0,
             Obj::e_BEGIN,
             0},
            {L_,
             "{",  // with only open brace
             1,
             Obj::e_START_OBJECT,
             0},
            {L_,
             "{\"",  // with only one quote char
             1,
             Obj::e_START_OBJECT,
             0},
            {L_,
             "{\"name",  // with incomplete element name
             1,
             Obj::e_START_OBJECT,
             0},
            {L_,
             "{\"name\"",  // with complete element name but missing value
             2,
             Obj::e_ELEMENT_NAME,
             "name"},
            {L_,
             "{\"name\":",  // with complete element name but missing value
             2,
             Obj::e_ELEMENT_NAME,
             "name"},
            {L_,
             "{\"name\":1.2",  // with complete element name and value but
                               // missing closing brace
             3,
             Obj::e_ELEMENT_VALUE,
             "1.2"},
            {L_,
             "{\"name\":1.2,",  // with complete element name and value but
                                // spurious comma
             3,
             Obj::e_ELEMENT_VALUE,
             "1.2"},
            {L_,
             "{\"n\":1.2,\"",  // with incomplete element name
             3,
             Obj::e_ELEMENT_VALUE,
             "1.2"},
            {L_,
             "{\"n\":1.2,\"t",  // with incomplete element name
             3,
             Obj::e_ELEMENT_VALUE,
             "1.2"},
            {L_,
             "{\"n\":1.2,\"too",  // with incomplete element name
             3,
             Obj::e_ELEMENT_VALUE,
             "1.2"},
            {L_,
             "{\"n\":1,\"t\"",  // with incomplete element name
             4,
             Obj::e_ELEMENT_NAME,
             "t"},
            {L_,
             "{\"n\":1,\"t\":",  // with complete element name but missing
                                 // value
             4,
             Obj::e_ELEMENT_NAME,
             "t"},
            {L_,
             "{\"n\":1,\"t\":\"2\"",  // with complete element name and value
                                      // but missing closing brace
             5,
             Obj::e_ELEMENT_VALUE,
             "\"2\""},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&           data        = DATA[ti % NUM_DATA];
            const int             LINE        = data.d_line;
            const string          TEXT        = data.d_text_p;
            const int             NUM_PREADVS = data.d_numPreAdvances;
            const Obj::TokenType  EXP_TOKEN   = data.d_expTokenType;
            const char           *EXP_VALUE   = data.d_value_p;
            const bool            CHECK_UTF8  = NUM_DATA <= ti;

            bsl::istringstream is(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(NUM_PREADVS)
            }

            Obj        mX;
            const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(is.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            // NUM_PREADVS advances should be successful.

            for (int i = 0; i < NUM_PREADVS; ++i) {
                int rc = mX.advanceToNextToken();
                ASSERTV(LINE, CHECK_UTF8, rc, !rc);
            }

            // Confirm the state after the pre-advances.

            ASSERTV(
                LINE, EXP_TOKEN, mX.tokenType(), EXP_TOKEN == mX.tokenType());
            if (EXP_VALUE) {
                bslstl::StringRef nodeValue;
                int               rc = mX.value(&nodeValue);
                ASSERTV(LINE, rc, !rc);

                bsl::string strValue = nodeValue;
                ASSERTV(LINE, strValue, EXP_VALUE, strValue == EXP_VALUE);
            }

            // The next advance should fail.

            int rc = mX.advanceToNextToken();
            ASSERTV(LINE, rc, rc);

            // Confirm that the error state is accurately reflected.

            ASSERTV(LINE, mX.tokenType(), Obj::e_ERROR == mX.tokenType());
            if (EXP_VALUE) {
                bslstl::StringRef nodeValue;
                int               rc = mX.value(&nodeValue);
                ASSERTV(LINE, rc, rc);

                bsl::string strValue = nodeValue;
                ASSERTV(LINE, strValue, strValue == "");
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'setAllowHeterogenousArrays' and 'allowHeterogenousArrays'
        //
        // Concerns:
        //: 1 'allowHeterogenousArrays' returns 'true' by default.
        //:
        //: 2 'setAllowHeterogenousArrays' method sets the
        //:   'allowHeterogenousArrays' option to the specified value.
        //:
        //: 3 'allowHeterogenousArrays' method returns the correct value of the
        //:   'allowHeterogenousArrays' option.
        //:
        //: 4 If 'allowHeterogenousArrays' option is 'false' then only JSON
        //:   arrays that have homogeneous values are accepted.  Note that
        //:   homogeneous implies that the values are all simple types (number
        //:   or string) or all arrays or all objects.
        //:
        //: 5 If 'allowHeterogenousArrays' option is 'true' then arrays of
        //:   heterogeneous values are accepted.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the value of the
        //:   'allowHeterogenousArrays' option, the expected token type after
        //:   invoking 'advanceToNextToken', and the expected value.
        //:
        //: 2 For each row in the table, construct a 'Tokenizer', 'mX',
        //:   with the values in that row.
        //:
        //: 3 Confirm that the 'allowHeterogenousArrays' manipulator and
        //:   accessor functions work as expected.
        //:
        //: 4 Confirm that the if 'allowHeterogenousArrays' value is 'true'
        //:   then arrays of heterogeneous values are tokenized correctly.
        //
        // Testing:
        //   void setAllowHeterogenousArrays(bool value);
        //   bool allowHeterogenousArrays() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'allowHeterogenousArrays' OPTION" << endl
                 << "========================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_numAdvances;
            bool            d_allowHeterogenousArrays;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
        } DATA[] = {
            // {
            //     L_,
            //     "[1,\"Hello\"]",
            //     2,
            //     false,
            //     true,
            //     Obj::e_ELEMENT_VALUE,
            // },
            // {
            //     L_,
            //     "[1,\"Hello\"]",
            //     2,
            //     true,
            //     true,
            //     Obj::e_ELEMENT_VALUE,
            // },
            // {
            //     L_,
            //     "[[],1]",
            //     3,
            //     false,
            //     false,
            //     Obj::e_ERROR,
            // },
            {
                L_,
                "[[],1]",
                3,
                true,
                true,
                Obj::e_ELEMENT_VALUE,
            },
            // {
            //     L_,
            //     "[[], \"Hello\"]",
            //     4,
            //     false,
            //     false,
            //     Obj::e_ERROR,
            // },
            // {
            //     L_,
            //     "[1,{}]",
            //     2,
            //     false,
            //     false,
            //     Obj::e_ERROR,
            // },
            // {
            //     L_,
            //     "[1,{}]",
            //     2,
            //     true,
            //     true,
            //     Obj::e_START_OBJECT,
            // },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            NUM_ADV    = data.d_numAdvances;
            const bool           ALLOW_HETEROGENOUS_ARRAYS
                                            = data.d_allowHeterogenousArrays;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(IS_VALID)
                P(EXP_TOKEN)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
            ASSERTV(X.allowHeterogenousArrays(),
                    true == X.allowHeterogenousArrays());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            mX.setAllowHeterogenousArrays(ALLOW_HETEROGENOUS_ARRAYS);
            ASSERTV(X.allowHeterogenousArrays(), ALLOW_HETEROGENOUS_ARRAYS,
                    ALLOW_HETEROGENOUS_ARRAYS == X.allowHeterogenousArrays());

            for (int i = 0; i < NUM_ADV; ++i) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'setAllowStandAloneValues' and 'allowStandAloneValues'
        //
        // Concerns:
        //: 1 'allowStandAloneValues' returns 'true' by default.
        //:
        //: 2 'setAllowStandAloneValues' method sets the
        //:   'allowStandAloneValues' option to the specified value.
        //:
        //: 3 'allowStandAloneValues' method returns the correct value of the
        //:   'allowStandAloneValues' option.
        //:
        //: 4 If 'allowStandAloneValues' option is 'false' then only JSON
        //:   objects and arrays are accepted as top-level elements.
        //:
        //: 5 If 'allowStandAloneValues' option is 'true' then JSON objects,
        //:   arrays, and values are accepted as top-level elements.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the value of the
        //:   'allowStandAloneValues' option, the expected token type after
        //:   invoking 'advanceToNextToken', and the expected value.
        //:
        //: 2 For each row in the table, construct a 'bdljsn::Tokenizer', 'mX',
        //:   with the values in that row.
        //:
        //: 3 Confirm that the 'allowStandAloneValues' setter and getter
        //:   functions works as expected.
        //:
        //: 4 Confirm that the if 'allowStandAloneValues' value is 'true' then
        //:   stand-alone values are correctly tokenized else only object and
        //:   array values are tokenized.
        //
        // Testing:
        //   void setAllowStandAloneValues(bool value);
        //   bool allowStandAloneValues() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'allowStandAloneValues' OPTION" << endl
                          << "======================================" << endl;

#define DT                                                                    \
     "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\""

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            bool            d_allowStandAloneValues;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            const char     *d_value_p;
        } DATA[] = {
            {
                L_,
                "12",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "12",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "12"
            },
            {
                L_,
                "true",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "true",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "true"
            },
            {
                L_,
                "\"true\"",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "\"true\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"true\""
            },
            {
                L_,
                "Michael",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "Michael",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "Michael"
            },
            {
                L_,
                "\"Michael\"",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "\"Michael\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"Michael\""
            },
            {
                L_,
                "\"Mic\\\"hael\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"Mic\\\"hael\""
            },
            {
                L_,
                "\"Mic\\\"hael\\\"\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"Mic\\\"hael\\\"\""
            },
            {
                L_,
                "\"Mic\\\"hael\\\\\\\"\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"Mic\\\"hael\\\\\\\"\""
            },
            {
                L_,
                "\"Mic\\\"\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"Mic\\\"\""
            },
            {
                L_,
                "null",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "null",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "null"
            },
            {
                L_,
                "\"null\"",
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                "\"null\"",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "\"null\""
            },
            {
                L_,
                "Michael",
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                "Michael"
            },
            {
                L_,
                "{ \"name\" : \"Michael\" }",
                true,
                true,
                Obj::e_START_OBJECT,
                "{"
            },
            {
                L_,
                "{ \"name\" : \"Michael\" }",
                false,
                true,
                Obj::e_START_OBJECT,
                "{"
            },
            {
                L_,
                "[ \"12\" ]",
                true,
                true,
                Obj::e_START_ARRAY,
                "["
            },
            {
                L_,
                "[ \"12\" ]",
                false,
                true,
                Obj::e_START_ARRAY,
                "["
            },
            {
                L_,
                DT,
                false,
                false,
                Obj::e_ERROR,
                ""
            },
            {
                L_,
                DT,
                true,
                true,
                Obj::e_ELEMENT_VALUE,
                DT
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const bool           ALLOW_STAND_ALONE_VALUES
                                            = data.d_allowStandAloneValues;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(IS_VALID)
                P(EXP_TOKEN) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
            ASSERTV(X.allowStandAloneValues(),
                    true == X.allowStandAloneValues());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            mX.setAllowStandAloneValues(ALLOW_STAND_ALONE_VALUES);
            ASSERTV(X.allowStandAloneValues(), ALLOW_STAND_ALONE_VALUES,
                    ALLOW_STAND_ALONE_VALUES == X.allowStandAloneValues());

            if (IS_VALID) {
                ASSERTV(LINE, CHECK_UTF8, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, CHECK_UTF8, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (Obj::e_ELEMENT_VALUE == EXP_TOKEN) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
#undef DT
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'resetStreamBufGetPointer'
        //
        // Concerns:
        //: 1 'resetStreamBufGetPointer' correctly resets the offset of the
        //:   underlying 'streambuf' to the next character after a token.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text with an 'X' at the expected final
        //:   location, the number of 'advanceToNextToken' calls to be made,
        //:   and the number of available characters in the 'streambuf' after
        //:   the function call.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Confirm that no characters are available in the 'streambuf'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the specified number of
        //:     times.
        //:
        //:   4 Confirm that the value of that token is as expected.
        //
        // Testing:
        //   int resetStreamBufGetPointer();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RESETTING STREAMBUF GET POINTER" << "\n"
                          << "=======================================" << endl;

// Define data block of 1400 bytes

#define DATA_TEXT                                                             \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
     "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_numAdvances;
            int             d_numAvail;
        } DATA[] = {
        //line    value            nAdv      nAvail
        //----    -----            ----      ------

        { L_,   "",                  0,           0   },
        { L_,   "{}",                2,           0   },

        { L_,   "{} ",               2,           1   },
        { L_,   "{}\n",              2,           1   },
        { L_,   "{}X",               2,           1   },

        { L_,   "{}\n ",             2,           2   },
        { L_,   "{}X ",              2,           2   },
        { L_,   "{} X",              2,           2   },

        { L_,   "{}          ",      2,          10   },
        { L_,   "{}      1234",      2,          10   },
        { L_,   "{}1234567890",      2,          10   },

        {
          L_,
          "{ \"x\" : \"" DATA_TEXT "X\"}",
          4,
          0
        },
        {
          L_,
          "{ \"x\" : \"" DATA_TEXT " X\"} ",
          4,
          1
        },
        {
          L_,
          "{ \"x\" : \"X\"}   " DATA_TEXT "\"} ",
          4,
          1406
        },
        {
          L_,
          "{ \"x\" : \"" DATA_TEXT "X\"} X",
          4,
          2
        },
        {
          L_,
          "{ \"x\" : \"" DATA_TEXT "X\"}X\n",
          4,
          2
        },
        {
          L_,
          "{ \"x\" : \" X\" }  \n" DATA_TEXT "X\"}X\n",
          4,
          1408
        },
        {
          L_,
          "{ \"x\" : \"" DATA_TEXT "X\"}1234567890",
          4,
          10
        },
        {
          L_,
          "{ \"x\" : \"" DATA_TEXT DATA_TEXT "X\"}1234567890",
          4,
          10
        },
        {
          L_,
          "{ \"x\" : \"X\" }   " DATA_TEXT DATA_TEXT DATA_TEXT "\"}1234567890",
          4,
          4215
        },

        // Total data one less than 8K

        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\"}1234",
          4,
          8177
        },

        // Total data equal to 8K

        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\"}12345",
          4,
          8178
        },

        // Total data one greater than 8K

        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\"}123456",
          4,
          8179
        },

        // Total data a lot greater than 8K

        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\"}123456",
          4,
          8319
        },
        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT,
          4,
          14003
        },
        {
          L_,
          "{ \"x\" : \"X\" }ABC"
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT
          DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT DATA_TEXT,
          4,
          28003
        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int BUFSIZE = 8192;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            NADV       = data.d_numAdvances;
            const int            NAVAIL     = data.d_numAvail;
            const bsl::string    EXPECTED(TEXT.end() - NAVAIL, TEXT.end());
            const bool           EMPTY      = TEXT.size() < BUFSIZE;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            if (veryVerbose) {
                P_(LINE) P_(TEXT)  P_(TEXT.size())  P_(NADV) P(NAVAIL)
            }

            // Try different 'streambuf' types

            // default 'bsl::streambuf' implementation
            {
                bsl::ostringstream os;
                os << TEXT;

                bsl::istringstream iss(os.str());

                bsl::streambuf *sb = iss.rdbuf();

                confirmStreamBufReset(sb,
                                      LINE,
                                      NADV,
                                      NAVAIL,
                                      EXPECTED,
                                      EMPTY,
                                      CHECK_UTF8);
            }

            // 'bdlsb::FixedMemInStreamBuf'
            {
                bdlsb::MemOutStreamBuf osb;
                bsl::ostream           os(&osb);

                os << TEXT;

                bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

                confirmStreamBufReset(&isb,
                                      LINE,
                                      NADV,
                                      NAVAIL,
                                      EXPECTED,
                                      EMPTY,
                                      CHECK_UTF8);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING that strings with escaped quotes are handled correctly
        //
        // Concerns:
        //: 1 Values having escaped quotes are handled correctly.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text and expected output.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' till an ELEMENT_VALUE
        //:     token is reached.
        //:
        //:   4 Confirm that the value of that token is as expected.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING THAT STRINGS WITH ESCAPED QUOTES "
                          << "ARE HANDLED CORRECTLY" << "\n"
                          << "========================================="
                          << "=====================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_value_p;
        } DATA[] = {
        //line    value
        //----    -----

          { L_,   "\"\""                    },

          { L_,   "\"\\\"\""                },

          { L_,   "\"\\\"\\\"\""            },

          { L_,   "\"\\\\\""                },

          { L_,   "\"\\\\\\\\\""            },

          { L_,   "\"\\\\\\\"\\\\\""        },

          { L_,   "\"\\\\\\\"\\\\\\\"\""    },

          { L_,   "\"\\\"\""                },
          { L_,   "\" \\\"\""               },
          { L_,   "\"\\\" \""               },
          { L_,   "\" \\\" \""              },

          { L_,   "\"\\\"\""                },
          { L_,   "\"A\\\"\""               },
          { L_,   "\"\\\"A\""               },
          { L_,   "\"A\\\"B\""              },

          { L_,   "\"A \\\" B\""            },

          { L_,   "\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\""     },

          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
          {
            L_,
            "\"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\\"\\\"\"",
          },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::ostringstream os;
            os << "{\"n\":" << EXP_VALUE << "}";

            bsl::istringstream iss(os.str());

            if (veryVerbose) {
                P(LINE) P(EXP_VALUE)
            }

            bslma::TestAllocator ta;
            Obj                  mX(&ta); const Obj& X = mX;

            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            int rc;
            do {
                rc = mX.advanceToNextToken();
            } while (!rc && Obj::e_ELEMENT_VALUE != X.tokenType());

            ASSERTV(LINE, rc, 0 == rc);

            bslstl::StringRef value;
            ASSERTV(LINE, 0 == X.value(&value));
            ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING that large values (greater than 8K) are handled correctly
        //
        // Concerns:
        //: 1 Values of larger sizes are handled correctly.
        //:
        //: 2 Only values larger than 1K result in an allocation.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text and whether external memory is
        //:   allocated.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text and
        //:     including the opening brace and name.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     to get to a value token.
        //:
        //:   4 Confirm that the value of that token is as expected.
        //:
        //:   5 Verify that memory is allocated when expected.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING THAT LARGE VALUES (GREATER THAN 8k) "
                          << "ARE HANDLED CORRECTLY" << "\n"
                          << "============================================"
                          << "=====================" << endl;

        const bsl::string LARGE_STRING(LARGE_STRING_C_STR);

        enum Allocs { e_FALSE, e_TRUE, e_TRUE_IF_UTF8 };

        const struct Data {
            int               d_line;
            const bsl::string d_suffixText;
            Allocs            d_allocatesMemory;
        } DATA[] = {

        //Line   Suffix Text                             Allocates memory flag
        //----   -----------                             ---------------------

#if defined(BSLS_PLATFORM_CPU_32_BIT)

        // 32-bit

        {   L_,  "12345678A\"",                           e_FALSE            },
        {   L_,  "12345678AB\"",                          e_TRUE_IF_UTF8     },
        {   L_,  "12345678ABCDE\"",                       e_TRUE             },
        {   L_,  "12345678ABCDEF\"",                      e_TRUE             },
        {   L_,  "12345678ABCDE12345678901234567890\"",   e_TRUE             },

#elif defined(BSLS_PLATFORM_CPU_64_BIT)
        // 64-bit

        {   L_,  "ABC\"",                                 e_FALSE            },
        {   L_,  "ABCD\"",                                e_FALSE            },

#if !defined(BSLS_PLATFORM_CMP_AIX)
        // AIX

        {   L_,  "12345678A\"",                           e_FALSE            },
        {   L_,  "12345678AB\"",                          e_TRUE_IF_UTF8     },
        {   L_,  "12345678ABCDE\"",                       e_TRUE             },
#endif

        {   L_,  "ABCDE12345678901234567890\"",           e_TRUE             },

#endif

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&  data       = DATA[ti % NUM_DATA];
            const int    LINE       = data.d_line;
            const string SUFFIX     = data.d_suffixText;
            const string TEXT       = LARGE_STRING + SUFFIX;
            const bool   CHECK_UTF8 = NUM_DATA <= ti;
            const bool   ALLOC      = e_FALSE == data.d_allocatesMemory
                                    ? false
                                    : e_TRUE  == data.d_allocatesMemory
                                    ? true
                                    : CHECK_UTF8;

            if (veryVerbose) {
                P(LINE) P(TEXT) P(TEXT.size()) P(ALLOC)
            }

            bsl::ostringstream os;
            os << "{\"n\":" << TEXT << "}";

            bsl::istringstream is(os.str());

            bslma::TestAllocator tta;
            BreakAllocator        ba;
            bslma::TestAllocator& ta = ALLOC ? tta : ba;

            Obj mX(&ta);  const Obj& X = mX;
            mX.reset(is.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::e_START_OBJECT  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::e_ELEMENT_NAME  == X.tokenType());

            ASSERTV(LINE, 0                   == mX.advanceToNextToken());
            ASSERTV(Obj::e_ELEMENT_VALUE == X.tokenType());

            bslstl::StringRef sr;
            ASSERTV(0                      == X.value(&sr));
            ASSERTV(LINE, TEXT, sr, TEXT == sr);

            if (ALLOC) {
                ASSERTV(LINE, TEXT.size(), ta.numBlocksTotal() > 0);
            }
            else {
                ASSERTV(LINE, 0 == ta.numBlocksTotal());
            }

            if (veryVerbose) {
                P(ta.numBlocksTotal()) P(ta.numBytesInUse())
            }
        }

        for (int tj = 0; tj < 2; ++tj) {
            const bool CHECK_UTF8 = tj;

            bsl::ostringstream os;
            os << "{\"Sequence\":{";
            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                const string SUFFIX = DATA[ti].d_suffixText;
                const string TEXT   = LARGE_STRING + SUFFIX;

                os << "\"" << 'a' << "\":" << TEXT << ",";
            }

            bsl::ostringstream ds;

            const string S = LARGE_STRING + DATA[NUM_DATA - 1].d_suffixText;
            const string T(S.begin() + 1, S.end() - 1);
            const string VERY_LARGE_STRING = "\"" + T + T + T + T + "\"";

            for (int ti = 0; ti < NUM_DATA - 1; ++ ti) {
                os << "\"" << 'a' << "\":" << VERY_LARGE_STRING << ",";
            }

            os << "\"" << 'a' << "\":" << VERY_LARGE_STRING << "}";

            bsl::istringstream is(os.str());

            if (veryVerbose) {
                P(os.str())
            }

            Obj mX;  const Obj& X = mX;
            mX.reset(is.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::e_START_OBJECT  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::e_ELEMENT_NAME  == X.tokenType());

            ASSERTV(0                         == mX.advanceToNextToken());
            ASSERTV(Obj::e_START_OBJECT  == X.tokenType());

            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                const int    LINE   = DATA[ti].d_line;
                const string SUFFIX = DATA[ti].d_suffixText;
                const string TEXT   = LARGE_STRING + SUFFIX;

                ASSERTV(LINE, 0                    == mX.advanceToNextToken());
                ASSERTV(LINE, Obj::e_ELEMENT_NAME  == X.tokenType());

                ASSERTV(LINE, 0                    == mX.advanceToNextToken());
                ASSERTV(LINE, Obj::e_ELEMENT_VALUE == X.tokenType());

                bslstl::StringRef data;
                ASSERTV(LINE, 0                == X.value(&data));
                ASSERTV(LINE, TEXT, data, TEXT == data);
            }

            for (int ti = 0; ti < NUM_DATA; ++ ti) {
                ASSERTV(0                         == mX.advanceToNextToken());
                ASSERTV(Obj::e_ELEMENT_NAME  == X.tokenType());

                ASSERTV(0                         == mX.advanceToNextToken());
                ASSERTV(Obj::e_ELEMENT_VALUE == X.tokenType());

                bslstl::StringRef data;
                ASSERTV(0                                 == X.value(&data));
                ASSERTV(VERY_LARGE_STRING, data, VERY_LARGE_STRING == data);
            }

            ASSERTV(0                       == mX.advanceToNextToken());
            ASSERTV(Obj::e_END_OBJECT  == X.tokenType());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'reset'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset'" << endl
                          << "===============" << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_END_ARRAY'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_ARRAY    -> END_ARRAY                          '[' -> ']'
        //:
        //:   2 VALUE (number) -> END_ARRAY                        VALUE -> ']'
        //:
        //:   3 VALUE (string) -> END_ARRAY                          '"' -> ']'
        //:
        //:   4 END_OBJECT     -> END_ARRAY                          '}' -> ']'
        //:
        //:   5 END_ARRAY      -> END_ARRAY                          ']' -> ']'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')         -> END_ARRAY                   '"' -> ']'
        //:
        //:   2 NAME (with ':')       -> END_ARRAY                   ':' -> ']'
        //:
        //:   3 NAME (with ',')       -> END_ARRAY           NAME -> ',' -> ']'
        //:
        //:   4 VALUE (with ',')      -> END_ARRAY          VALUE -> ',' -> ']'
        //:
        //:   5 START_OBJECT          -> END_ARRAY                   '{' -> ']'
        //:
        //:   6 END_OBJECT (with ',') -> END_ARRAY            '}' -> ',' -> ']'
        //:
        //:   7 END_ARRAY (with ',')  -> END_ARRAY            ']' -> ',' -> ']'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "TESTING 'advanceToNextToken' TO 'e_END_ARRAY'" << endl
                 << "=============================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '[' - ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                              "]",
                3,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS               "]" WS,
                3,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                     "]",
                3,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "["
                WS                            "]" WS,
                3,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            // value (integer) -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1, 2"
                                     "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            // value (string) -> ']'
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                4,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS                           "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\","
                WS                 "\"Ryan\""
                WS                           "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS
                WS                                  "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS
                WS                                  "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\""  WS
                WS                                           "]" WS,
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS
                WS                                                         "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS ","
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS
                WS                                                         "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            // '}' -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\""
                                                ":"
                                                  "1.500012"
                                "}"
                              "]",
                7,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                7,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\""
                                           ":"
                                             "\"CompanyName\""
                                "},"
                                "{"
                                  "\"bid_price\""
                                                ":"
                                                  "1.500012"
                                "}"
                               "]",
                11,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                11,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]" WS,
                11,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            // ']' -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "]"
                              "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "]"
                WS               "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                  "\"bid_price\""
                                "]"
                              "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                   "\"bid_price\""
                WS                 "]"
                WS               "]",
                5,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "],"
                                "["
                                "]"
                              "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "],"
                WS                 "["
                WS                 "]"
                WS               "]",
                6,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                  "123"
                                "],"
                                "["
                                  "456"
                                "]"
                              "]",
                8,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                   "123"
                WS                 "],"
                WS                 "["
                WS                   "456"
                WS                 "]"
                WS               "]",
                8,
                true,
                Obj::e_END_ARRAY,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ","
                             "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ","
                WS              "]",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      ","
                                        "]",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS                       ","
                WS                         "]",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "{"
                               "]",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "{"
                WS                "]",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "1,"
                             "]",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "1,"
                WS              "]",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "{"
                               "},"
                             "]",
                5,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "{"
                WS                "},"
                WS              "]",
                5,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "["
                               "],"
                             "]",
                5,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "["
                WS                "],"
                WS              "]",
                5,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(LINE, i, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_START_ARRAY'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 NAME             -> START_ARRAY                      ':' -> '['
        //:
        //:   2 START_ARRAY      -> START_ARRAY                      '[' -> '['
        //:
        //:   3 END_ARRAY        -> START_ARRAY               ']' -> ',' -> '['
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')    -> START_ARRAY                      '"' -> '['
        //:
        //:   2 VALUE (with ',') -> START_ARRAY             VALUE -> ',' -> '['
        //:
        //:   3 START_OBJECT     -> START_ARRAY                      '{' -> '['
        //:
        //:   4 END_OBJECT       -> START_ARRAY                      '}' -> '['
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO 'e_NAME'" << endl
                      << "========================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (array), i.e. name -> '['
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "[",
                2,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "[" WS,
                2,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "[" WS,
                2,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "[" WS,
                2,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },

            // '[' -> '[' (array of arrays)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "[",
                3,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[",
                3,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[" WS,
                3,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },

            // '[' -> '[' (array of arrays)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "["
                                "],"
                                "[",
                5,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "["
                WS                 "],"
                WS                 "[",
                5,
                true,
                Obj::e_START_ARRAY,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"price\""
                            "[",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "[",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "1.500012,"
                                        "[",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "1.500012,"
                WS                         "[",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "[",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "[",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                              "},"
                              "[",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS               "},"
                WS               "[",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_END_OBJECT'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_OBJECT     -> END_OBJECT                       '{' -> '}'
        //:
        //:   2 VALUE (number)   -> END_OBJECT                     VALUE -> '}'
        //:
        //:   3 VALUE (string)   -> END_OBJECT                       '"' -> '}'
        //:
        //:   4 START_OBJECT     -> END_OBJECT                '[' -> '{' -> '}'
        //:
        //:   5 START_OBJECT     -> END_OBJECT                '{' -> '{' -> '}'
        //:
        //:   6 END_OBJECT       -> END_OBJECT         '{' -> '{' -> '}' -> '}'
        //:
        //:   7 END_OBJECT       -> END_OBJECT  '{' -> '{' -> '{' -> '}' -> '}'
        //:
        //:   8 END_ARRAY        -> END_OBJECT                '[' -> ']' -> '}'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME             -> END_OBJECT                    '"'  -> '}'
        //:
        //:   2 NAME (with ':')  -> END_OBJECT                    ':'  -> '}'
        //:
        //:   3 VALUE (with ',') -> END_OBJECT                  VALUE  -> '}'
        //:
        //:   4 START_ARRAY      -> END_OBJECT                  '['    -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "TESTING 'advanceToNextToken' TO 'e_END_OBJECT'" << endl
                << "==============================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> '}'
            {
                L_,
                "{"
                "}",
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                "}",
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                   "}",
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS
                "}",
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                "}" WS,
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{\n"
                WS "}",
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS "}"
                WS,
                1,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // value (integer) -> '}'
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // value (string) -> '}'
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                3,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}",
                4,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}",
                4,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}" WS,
                4,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}",
                5,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}",
                5,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS,
                5,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '}' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '{' -> '}' -> '}' -> '}
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}"
                "}",
                7,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}"
                WS "}",
                7,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}"
                WS "}",
                7,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{"
                                "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}" WS,
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]"
                "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]"
                WS "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "}"
                              "]"
                "}",
                8,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                8,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                 "}"
                               "]"
                "}",
                10,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                12,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // '[' -> ']' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "["
                                                "]"
                              "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "["
                WS                                 "]"
                WS               "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "["
                WS                                 "]" WS
                WS               "}",
                6,
                true,
                Obj::e_END_OBJECT,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ","
                             "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ","
                WS              "}",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      ","
                                        "}",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS                       ","
                WS                         "}",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "}",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "}",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_VALUE'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 NAME           -> VALUE (number)                   ':' -> VALUE
        //:
        //:   2 NAME           -> VALUE (string)                   ':' -> VALUE
        //:
        //:   3 START_ARRAY    -> VALUE (number)                   '[' -> VALUE
        //:
        //:   4 START_ARRAY    -> VALUE (string)                   '[' -> VALUE
        //:
        //:   5 VALUE (number) -> VALUE (number)                 VALUE -> VALUE
        //:
        //:   6 VALUE (string) -> VALUE (string)                 VALUE -> VALUE
        //:
        //:   7 VALUE (number) -> VALUE (string)                 VALUE -> VALUE
        //:
        //:   8 VALUE (string) -> VALUE (number)                 VALUE -> VALUE
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 VALUE (no ,)   -> VALUE                         VALUE  -> VALUE
        //:
        //:   2 END_OBJECT     -> VALUE                           '}'  -> VALUE
        //:
        //:   3 END_ARRAY      -> VALUE                           ']'  -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "TESTING 'advanceToNextToken' TO 'e_VALUE'" << endl
                     << "=========================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (integer)
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // name -> value (string)
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS " Doe" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                        "\"John " WS " Doe\""
                WS                                            "}" WS,
                2,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John " WS " Doe\""
            },

            // '[' -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // '[' -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                "["
                    "\"John\""
                    "]",
                1,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                "["
                    "\"\nJohn\""
                    "]",
                1,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"\nJohn\""
            },
            {
                L_,
                "["
                    "\" John\""
                    "]",
                1,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\" John\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                3,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "Doe" WS "\""
            },

            // value (integer) -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "0"
            },

            // value (string) -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\","
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "\"Smith\"" WS
                WS                                 "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS
                WS                                  "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "Smith" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "Ryan" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS
                WS                                                         "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "Black" WS "Jack" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS ","
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS
                WS                                                         "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"" WS "New" WS "Deal" WS "\""
            },

            //..
            // value (integer) -> value (string)
            // value (string)  -> value (integer)
            //..
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "12345,"
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "12345,"
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"Smith\","
                                "12345"
                                          "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Ryan\","
                                "12345"
                                          "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "12345,"
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "12345,"
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "12345"     WS
                WS                                 "]",
                4,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "12345"     WS
                WS                                  "]",
                5,
                true,
                Obj::e_ELEMENT_VALUE,
                true,
                "12345"
            },

            // Error
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                             " "
                             "12345",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\""
                WS              " "
                WS              "12345",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "12345"
                             " "
                             "\"John\"",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "12345"
                WS              " "
                WS              "\"John\"",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "["
                               "\"John\""
                               " "
                               "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""
                WS                " "
                WS                "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "{"
                                  "}"
                                    "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "{"
                WS                   "}"
                WS                     "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                  "]"
                                    "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                   "]"
                WS                     "12345",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                    "1"
                                    " "
                                    "2"
                                  "]",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                     "1"
                WS                     " "
                WS                     "2"
                WS                   "]",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= 2;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_NAME'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 START_OBJECT   -> NAME                               '{' -> '"'
        //:
        //:   2 END_OBJECT     -> NAME          ':' -> '{' -> '}' -> ',' -> '"'
        //:
        //:   3 END_ARRAY      -> NAME          ':' -> '[' -> ']' -> ',' -> '"'
        //:
        //:   4 VALUE (number) -> NAME               ':' -> VALUE -> ',' -> '"'
        //:
        //:   5 VALUE (string) -> NAME               ':' -> VALUE -> ',' -> '"'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 END_OBJECT (no ',') -> NAME                         '}'  -> '"'
        //:
        //:   2 END_ARRAY (no ',')  -> NAME                         ']'  -> '"'
        //:
        //:   3 NAME                -> NAME                         '"'  -> '"'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO 'e_NAME'" << endl
                      << "========================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> name
            {
                L_,
                "{"
                 "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{\"element name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "element name"
            },
            {
                L_,
                WS "{\"element " WS " name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"" WS,
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "element " WS " name"
            },

            // value (number) -> '"'
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "1.500012,"
                  "\"name\"",
                3,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "1.500012,"
                WS   "\"name\"",
                3,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\","
                  "\"bid_price\"",
                3,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "bid_price"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "\"John\","
                WS   "\"bid_price\"",
                3,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "bid_price"
            },

            // '}' -> '"'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"bid_price\":1.500012"
                              "},"
                  "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"bid_price\":1.500012"
                WS               "},"
                WS   "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"name\":\"CompanyName\","
                                "\"bid_price\":1.500012"
                              "},"
                  "\"date\"",
                8,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"name\":\"CompanyName\","
                WS                 "\"bid_price\":1.500012"
                WS               "},"
                WS   "\"date\"",
                8,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },

            // ']' -> '"'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                              "],"
                  "\"date\"",
                5,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS               "],"
                WS   "\"date\"",
                5,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                              "],"
                  "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345"
                WS               "],"
                WS   "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                              "],"
                  "\"date\"",
                5,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS               "],"
                WS   "\"date\"",
                5,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                              "],"
                  "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\","
                WS                 "\"Smith\""
                WS               "],"
                WS   "\"date\"",
                6,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "date"
            },

            // Error
            {
                L_,
                "{"
                  "\"bid_price\""
                                " "
                                  "\"value\"",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 " "
                WS                   "\"value\"",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "{"
                                  "}"
                                    "\"value\"",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "{"
                WS                   "}"
                WS                     "\"value\"",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"bid_price\""
                                ":"
                                  "["
                                  "]"
                                    "\"value\"",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"bid_price\""
                WS                 ":"
                WS                   "["
                WS                   "]"
                WS                     "\"value\"",
                4,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO 'e_START_OBJECT'
        //
        // Concerns:
        //: 1 The following transitions are correctly handled:
        //:
        //:   1 BEGIN          -> START_OBJECT                     BEGIN -> '{'
        //:
        //:   2 NAME           -> START_OBJECT                       ':' -> '{'
        //:
        //:   3 START_ARRAY    -> START_OBJECT                       '[' -> '{'
        //:
        //:   4 END_OBJECT     -> START_OBJECT  '[' -> '{' -> '}' -> ',' -> '{'
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 NAME (no ':')  -> START_OBJECT                      '"'  -> '{'
        //:
        //:   2 START_OBJECT   -> START_OBJECT                      '{'  -> '{'
        //:
        //:   3 END_ARRAY      -> START_OBJECT                      ']'  -> '{'
        //:
        //:   4 VALUE          -> START_OBJECT                     VALUE -> '{'
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        //   TokenType tokenType() const;
        //   int value(bslstl::StringRef *data) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "TESTING 'advanceToNextToken' TO 'e_START_OBJECT'" << endl
              << "================================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // BEGIN -> '{'
            {
                L_,
                "{",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS,
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{" WS,
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\n\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name\n"
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{"
                WS "\"name\"",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                WS "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{"
                 "\"name\"" WS,
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"" WS,
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS  "\"name\"",
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\"",
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                0,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS "\" name\"" WS,
                1,
                true,
                Obj::e_ELEMENT_NAME,
                true,
                " name"
            },

            // name -> value (object), i.e. name -> '{'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{",
                2,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{" WS,
                2,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "{" WS,
                2,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock" WS "price\""
                WS                        ":"
                WS                          "{" WS,
                2,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },

            // '[' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{",
                3,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{",
                3,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{" WS,
                3,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },

            // '}' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{",
                5,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{",
                5,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                7,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\":1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                7,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                9,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\","
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                9,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\","
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                9,
                true,
                Obj::e_START_OBJECT,
                false,
                ""
            },

            // Error
            {
                L_,
                "{"
                  "\"price\""
                            "{",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "{" WS,
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "{" WS,
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "\"John\""
                                      "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\","
                                    "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\","
                WS                     "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                             "12345"
                                   "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "12345"
                WS                    "{",
                3,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            "}"
                              "{",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "}"
                WS               "{" WS,
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            "]"
                              "{",
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             "]"
                WS               "{" WS,
                2,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(),
                        Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                    if (value != EXP_VALUE) {
                        cout << "value=::" << value << "::"
                             << ", EXP_VALUE=::" << EXP_VALUE << "::"
                             << ", tokenType=::" << X.tokenType()
                             << endl;
                    }
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'setAllowTrailingTopLevelComma'
        //
        // Concerns:
        //: 1 Top-level trailing commas are allowed by default.
        //:
        //: 2 Passing 'false' to 'setAllowTrailingTopLevelComma' disallows
        //:   them.
        //:
        //: 3 The accessor 'allowTrailingTopLevelComma' reports a value
        //:   consistent with the expected value and the demonstrated behavior.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of otherwise-valid input text with trailing
        //:   commas, and make sure the result of calling 'advanceToNextToken'
        //:   to completion matches the 'setAllowTrailingTopLevelComma' value.
        //
        // Testing:
        //   int setAllowTrailingTopLevelComma();
        //   bool allowTrailingTopLevelComma() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "TESTING 'setAllowTrailingTopLevelComma'" << endl
                    << "=======================================" << endl;

        const struct Data {
            int         d_lineNum;
            bsl::string d_text;
        } JSON_DATA[] = {
            // null
            {
                L_,
                "null,"
            },
            {
                L_,
                " null  ,   "
            },
            // Boolean values
            {
                L_,
                "true,"
            },
            {
                L_,
                "false,"
            },
            {
                L_,
                " true  ,   "
            },
            {
                L_,
                " false  ,   "
            },
            // Numerical values
            {
                L_,
                "1,"
            },
            {
                L_,
                " 1  ,   "
            },
            // String values
            {
                L_,
                "\"a\","
            },
            {
                L_,
                " \"a\"  ,   "
            },
            // Array values
            {
                L_,
                "[],"
            },
            {
                L_,
                " []  ,   "
            },
            {
                L_,
                "[1,2,3],"
            },
            {
                L_,
                " [  1 ,   2 , 3     ]  ,   "
            },
            {
                L_,
                " [ [ 1 ] ,   [2 , 3  ]     ]  ,   "
            },
            {
                L_,
                " [ { \"a\":[ 1 ] ,   \"b  \"   : [2 , 3  ] }   ]  ,   "
            },
            // Object values
            {
                L_,
                "{},"
            },
            {
                L_,
                " {}  ,   "
            },
            {
                L_,
                "{\"a\":1},"
            },
            {
                L_,
                "{\"a\":1, \"b\":{}},"
            },
            {
                L_,
                "{\"a\":[1, 2], \"b\":[{}, 2, 4]},"
            },
        };
        const int NUM_JSON_DATA = sizeof(JSON_DATA) / sizeof(JSON_DATA[0]);

        for (int ti = 0; ti < NUM_JSON_DATA; ++ti) {
            const Data&        DATA = JSON_DATA[ti];
            const int          LINE = DATA.d_lineNum;
            const bsl::string& TEXT = DATA.d_text;

            if (veryVerbose) {
                P_(ti);
                P_(LINE);
                P(TEXT);
            }

            for (int topLevelComma = 0; topLevelComma < 2; ++topLevelComma) {
                bool tlcFlag = bool(topLevelComma);

                if (veryVeryVerbose) {
                    T_;
                    P(tlcFlag);
                }

                bsl::istringstream iss(TEXT);

                Obj        mX;
                const Obj& X = mX;
                mX.reset(iss.rdbuf());

                mX.setAllowTrailingTopLevelComma(tlcFlag);
                ASSERT(tlcFlag == X.allowTrailingTopLevelComma());

                ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
                ASSERTV(X.currentPosition(), 0 == X.currentPosition());
                ASSERTV(X.readOffset(), 0 == X.readOffset());

                while (0 == mX.advanceToNextToken()) {
                }

                ASSERTV(LINE,
                        TEXT,
                        X.readStatus(),
                        tlcFlag,
                        tlcFlag == bool(X.readStatus()));
            }
        }
       } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' FIRST CHARACTER
        //
        // Concerns:
        //: 1 The first character is always '{' or '['
        //:
        //: 2 The return code is 0 on success and non-zero on failure.
        //
        // Errors:
        //: 1 The following transitions return an error:
        //:
        //:   1 WHITESPACE ONLY                                  " \t\n\v\f\r"
        //:
        //:   2 BEGIN -> START_ARRAY                             BEGIN -> '['
        //:
        //:   3 BEGIN -> END_ARRAY                               BEGIN -> ']'
        //:
        //:   4 BEGIN -> END_OBJECT                              BEGIN -> '}'
        //:
        //:   5 BEGIN -> '"'                                     BEGIN -> '"'
        //:
        //:   6 BEGIN -> ','                                     BEGIN -> ','
        //:
        //:   7 BEGIN -> ':'                                     BEGIN -> ':'
        //:
        //:   8 BEGIN -> VALUE                                   BEGIN -> VALUE
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   rows consisting of input text, the number of times to invoke
        //:   'advanceToNextToken', the result of an additional invocation of
        //:   'advanceToNextToken', and the expected token and value, if
        //:   applicable, after that invocation.
        //:
        //: 2 For each row in the table of P-1:
        //:
        //:   1 Create an 'bsl::istringstream', 'iss', with the input text.
        //:
        //:   2 Create a 'bdljsn::Tokenizer' object, mX, and associate the
        //:     'bsl::streambuf' of 'iss' with 'mX'.
        //:
        //:   3 Invoke 'advanceToNextToken' on 'mX' the number of times
        //:     specified in that row.
        //:
        //:   4 Invoke 'advanceToNextToken' one more time and record the
        //:     return value, the token type, and the value of that token.
        //:
        //:   5 Confirm that the return code is 0 on success and non-zero
        //:     otherwise.  Also confirm that the token type is as expected.
        //:     Finally, if that token is expected to have a value, then the
        //:     value of that token is as expected.
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "TESTING 'advanceToNextToken' FIRST CHARACTER" << endl
                    << "============================================" << endl;

        const struct Data {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {
            // Whitespace only
            {
                L_,
                "",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "          ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "   \t       ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n          ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "      \n    ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n\t          ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "    \t  \n    ",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                " \t\n\v\f\r",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS,
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },

            // Error - First character
            {
                L_,
                "]",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "]",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                "}",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "}",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },

            {
                L_,
                "\"",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "\"",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                ",",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ",",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                ":",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ":",
                0,
                false,
                Obj::e_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < 2 * NUM_DATA; ++ ti) {
            const Data&          data       = DATA[ti % NUM_DATA];
            const int            LINE       = data.d_line;
            const string         TEXT       = data.d_text_p;
            const int            PRE_MOVES  = data.d_preMoves;
            const bool           IS_VALID   = data.d_validFlag;
            const Obj::TokenType EXP_TOKEN  = data.d_expTokenType;
            const bool           HAS_VALUE  = data.d_hasValue;
            const string         EXP_VALUE  = data.d_value_p;
            const bool           CHECK_UTF8 = NUM_DATA <= ti;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());

            mX.reset(iss.rdbuf());
            mX.setAllowNonUtf8StringLiterals(!CHECK_UTF8);

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::e_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX;  const Obj& X = mX;
        ASSERTV(X.tokenType(), Obj::e_BEGIN == X.tokenType());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global/default allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
