// bdld_datumutil.t.cpp                                               -*-C++-*-
#include <bdld_datumutil.h>

#include <bdld_datum.h>
#include <bdld_datummaker.h>

#include <bdlb_literalutil.h>
#include <bdlb_chartype.h>

#include <bdldfp_decimal.h>

#include <bdlma_localsequentialallocator.h>
#include <bdlma_localbufferedobject.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>
#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>

#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_unordered_set.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a utility operating on `bldd::Datum` objects.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [2] ostream& typedPrint(object, outputStream, level, spacesPerLevel)
// [3] ostream& safeTypedPrint(object, outStream, level, spacesPerLevel)
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [4] USAGE EXAMPLE
// [-1] FUZZ TESTING PRINT ROUTINES
// ----------------------------------------------------------------------------
// FUZZ TESTING
// [ *] int LLVMFuzzerTestOneInput(const uint8_t *, size_t);

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                             TYPE DEFINITIONS
//-----------------------------------------------------------------------------

typedef bdld::DatumUtil Util;

//=============================================================================
//                              PRINT HELPERS
//-----------------------------------------------------------------------------

/// Simple wrapper type to print a string quoted and escaped so printouts
/// are easy to visually compare.
struct QuotedEscapedStringPrinterRef {

    // DATA
    const bsl::string_view d_str;

    // CREATORS
    QuotedEscapedStringPrinterRef(const bsl::string_view& str)
    : d_str(str)
    {
    }
};

inline
bsl::ostream&
operator<<(bsl::ostream& os, const QuotedEscapedStringPrinterRef& obj)
{
    bdlma::LocalBufferedObject<bsl::string, 128> quotedAndEscaped;

    typedef bdlb::LiteralUtil LitUtil;
    LitUtil::createQuotedEscapedCString(&*quotedAndEscaped, obj.d_str);

    return os << *quotedAndEscaped;
}

//=============================================================================
//                           VERIFICATION HELPERS
//-----------------------------------------------------------------------------

/// Return `true`, if the specified `safeResult` matches the specified
/// `typedResult`, where both results are from the printing of the same non-
/// self-referential data structures using `safeTypedPrint` and `typedPrint`
/// respectively.  The function returns `false` in case the two strings do
/// not match.  Since this is a test function it will follow a wide
/// contract with what normally would be a `BSLS_ASSERT` just be test
/// failures.  Passing an empty string for either arguments will return in
/// such a test failure.
///
/// The safe-typed-print output matches a typed-print output when the data
/// types, values, and indentations printed are the same, but the safe-print
/// also contain an at sign '@' and hexadecimal identifier after the type
/// for each possibly self-referential type, such as arrays, maps, and
/// int-maps.  This method ignores the value of such identifiers except that
/// it verifies that they are valid hexadecimal numbers, and that they do
/// not repeat.  In case either of those errors a test failure will be
/// generated (test `ASSERT`).
bool safeAndNormalMatches(const bsl::string_view& safeResult,
                          const bsl::string_view& typedResult)
{
    ASSERT(!safeResult.empty());
    ASSERT(!typedResult.empty());
    if (safeResult.empty() || typedResult.empty()) {
        return false;                                                 // RETURN
    }

    using bsl::string_view;
    bsl::unordered_set<string_view> seenIDs;

    typedef string_view::size_type SizeT;

    const SizeT k_NPOS = string_view::npos;
    SizeT       safePos  = 0;
    SizeT       typedPos = 0;
    for (;;) {
        // Find the next '@'
        const SizeT atPos = safeResult.find('@', safePos);

        // Compare up to the next '@', or the end of the string if not found
        const SizeT sliceLen = atPos != k_NPOS ? atPos - safePos : k_NPOS;
        const string_view  safeSlice =  safeResult.substr( safePos, sliceLen);
        const string_view typedSlice = typedResult.substr(typedPos, sliceLen);
        if (safeSlice != typedSlice) {
            ASSERTV(safeSlice, typedSlice, safeSlice == typedSlice);
            return false;                                             // RETURN
        }

        // If there was no '@', we are done
        if (k_NPOS == atPos) {
            return true;                                              // RETURN
        }

        // If there *was* an '@', we need to verify/skip the hex digits

        // Let's move beyond the slices we have just compared
        typedPos += sliceLen;
        safePos = atPos + 1;

        // Skip 0x prefix is present
        const SizeT hexPos = safeResult.substr(safePos).starts_with("0x")
                           ? safePos + 2
                           : safePos;

        // Verify that we have hex digits
        const SizeT nextSafePos = safeResult.find_first_not_of(
                                                bdlb::CharType::stringXdigit(),
                                                hexPos);

        // There *must* follow a value (array, map, or int-map)
        ASSERT(nextSafePos != k_NPOS);
        if (nextSafePos == k_NPOS) {
            // Assume `safeResult` ended prematurely
            return false;                                             // RETURN
        }

        const SizeT hexIdLength = nextSafePos - safePos;

        const string_view id = safeResult.substr(safePos, hexIdLength);

        const bool alreadySeenThisID = !seenIDs.emplace(id).second;
        ASSERT(!alreadySeenThisID);

        safePos = nextSafePos;
    }
}

/// Return the next "@ ID" from the specified `string`, or a default
/// constructed (empty) string-view.  Start searching for the '@' sign from
/// the specified `position`.  Load, into `position` the position of the '@'
/// sign, or load `npos` if there was no '@' sign found.
///
/// For brevity this function is used both to get the next ID and the next
/// named placeholder, so it allows the ID to have any alphanumeric
/// character, not just hexadecimal digits.  That verification is done by
/// the caller.  This will work because the ID as well as the name will be
/// naturally delimited by a '[' character for arrays, and a '{' character
/// for maps.
bsl::string_view getNextID(bsl::string_view::size_type *position,
                           const bsl::string_view&      string)
{
    using bsl::string_view;
    typedef string_view::size_type SizeT;


    const SizeT startPos = *position;
    const SizeT atPos = string.find('@', startPos);
    *position = atPos;
    if (atPos == string_view::npos) {
        return string_view();                                         // RETURN
    }

    const SizeT end = string.find_first_not_of(bdlb::CharType::stringAlnum(),
                                               atPos + 1);
    ASSERT(end != string_view::npos);  // Must be a '[' or '{' there
    return string.substr(atPos + 1, end - atPos  -1);
}

/// Return `true` if the specified `str` is not empty, and all its
/// constituent characters are hexadecimal digits.  Otherwise, return
/// `false`.
bool isHexId(const bsl::string_view& str)
{
    if (str.empty()) {
        return false;                                                 // RETURN
    }

    const bsl::string_view hex = str.substr(str.starts_with("0x") ? 2 : 0);
    return hex.find_first_not_of(bdlb::CharType::stringXdigit()) ==
                                                        bsl::string_view::npos;
}

/// Return `true`, if the specified `safeResult` matches the specified
/// `expected` result, where the `expected` string has named placeholders
/// for identifiers, not their expected values.  The function returns
/// `false` in case the two strings do not match.  Since this is a test
/// function it will follow a wide contract in regards to test-result
/// arguments, but narrow contract in regards of the `expected` argument.
///
/// The safe-typed-print output matches the expected output when the data
/// types, values, indentations printed are the same, the hexadecimal, and
/// the matching hexadecimal identifiers are the same where the placeholder
/// name in the `expected` string is the same, and different for different
/// names.
bool verifySafePrintResult(const bsl::string_view& safeResult,
                           const bsl::string_view& expected)
{
    BSLS_ASSERT(!expected.empty());

    ASSERT(!safeResult.empty());
    if (safeResult.empty()) {
        return false;                                                 // RETURN
    }

    using bsl::string_view;
    bsl::unordered_set<string_view>              seenIDs;
    bsl::unordered_map<string_view, string_view> nameToID;
    typedef bsl::unordered_map<string_view, string_view>::iterator NameIter;

    typedef string_view::size_type SizeT;

    const SizeT k_NPOS = string_view::npos;
    SizeT       safePos = 0;
    SizeT       expdPos = 0;
    for (;;) {
        // Find the next '@'
        SizeT safeAtPos = safePos;
        const string_view ident = getNextID(&safeAtPos, safeResult);
        SizeT expdAtPos = expdPos;
        const string_view named = getNextID(&expdAtPos, expected);

        if ((k_NPOS == safeAtPos) != (k_NPOS == expdAtPos)) {
            return false;                                             // RETURN
        }

        // First we compare up to the '@' sign
        {
            const SizeT safeLen = k_NPOS == safeAtPos
                                          ? k_NPOS
                                          : safeAtPos - safePos;
            const SizeT expdLen = k_NPOS == expdAtPos
                                          ? k_NPOS
                                          : expdAtPos - expdPos;

            const string_view safeSlice = safeResult.substr(safePos, safeLen);
            const string_view expdSlice =   expected.substr(expdPos, expdLen);
            if (safeSlice != expdSlice) {
                ASSERTV(safeSlice, expdSlice, safeSlice == expdSlice);
                return false;                                         // RETURN
            }
        }

        // If there was no '@', we are done
        if (k_NPOS == safeAtPos) {
            return true;                                              // RETURN
        }

        // Verify that the ID is hex and not empty
        ASSERTV(ident, isHexId(ident));

        // If we have seen this ID already it must have a name, and may be loop
        const bool alreadySeenThisID = !seenIDs.emplace(ident).second;
        const bsl::pair<NameIter, bool> per = nameToID.emplace(named, ident);
        if (!per.second) {
            if (per.first->second != ident) {
                // We have seen this name before, ID must match
                const string_view storedIdForName = per.first->second;
                ASSERTV(storedIdForName, ident, storedIdForName != ident);
                return false;                                         // RETURN
            }
        }
        else {
            // If we have this ID with another name, that is an error in the
            // `safeTypedPrint` code.
            ASSERT(!alreadySeenThisID);
        }

        // If there *was* an '@', we need to skip the name/id
        safePos = safeAtPos + 1 + ident.size();
        expdPos = expdAtPos + 1 + named.size();
    }
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Showing the Difference Between an Integer and a Double Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// See `main()`.
// ```
//  Then, we create a shorthand for `bdld::Datumutil::typedPrint`:
// ```
    void printWithType(bsl::ostream& outStream, const bdld::Datum& object)
    {
        bdld::DatumUtil::typedPrint(outStream, object, 0, -1);
    }
// ```
// See `main()`.

//=============================================================================
//                              FUZZ TESTING
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, `LLVMFuzzerTestOneInput`, is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
//-----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

using bsl::uint8_t;

extern "C"
/// Use the specified `data` array of `size` bytes as input to methods of
/// `bdld::DatumUtil` and return zero.
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // --------------------------------------------------------------------
    // FUZZ TESTING PRINT ROUTINES
    //
    // Concerns:
    // 1. That `bdld::DatumUtil::typedPrint` and `safeTypedPrint` do not
    //    crash, abort, leak, or otherwise misbehave when invoked on
    //    `bdld::Datum` objects constructed from arbitrary fuzz input.
    //
    // 2. That the print routines handle all supported `Datum` types
    //    (integer, int64, double, decimal64, owning string, string
    //    reference, boolean, null, error, error-with-message, binary,
    //    array, map, int-map) without invoking undefined behavior.
    //
    // Plan:
    // 1. Consume a small opcode from the fuzz input to select which
    //    `Datum` factory to invoke.
    //
    // 2. Consume additional bytes from the fuzz input to build the
    //    `Datum` value.
    //
    // 3. Invoke `typedPrint` and `safeTypedPrint` on the resulting
    //    `Datum` into `bsl::ostringstream`s, then destroy the `Datum`
    //    with the provided allocator.
    //
    // Testing:
    //   int LLVMFuzzerTestOneInput(const uint8_t *, size_t);
    // --------------------------------------------------------------------

    if (size < 5) {
        // Need at least 1 byte for the opcode discriminator plus enough
        // bytes for the smallest payload (e.g. a 4-byte integer).
        return 0;                                                     // RETURN
    }

    bslma::TestAllocator                  ta;
    bdlma::LocalSequentialAllocator<1024> lsa(&ta);
    bdld::DatumMaker                      dm(&lsa);
    bslim::FuzzDataView                   fdv(data, size);

    // Some `Datum` variants (e.g. string references, binary data, and the
    // `dm.bin`/`createStringRef` helpers) hold a pointer into an external
    // buffer that must outlive the `Datum`.  Declare those backing buffers
    // in the enclosing function scope so they remain valid until the end
    // of this call.
    bsl::string strBuf(&lsa);
    bsl::string binBuf(&lsa);
    bsl::string msgBuf(&lsa);
    bsl::string keyBuf1(&lsa);
    bsl::string keyBuf2(&lsa);

    // Named opcodes selecting which `Datum` variant to construct from the
    // fuzz input.  Keep `k_NUM_OPCODES` last; it is used as the upper
    // bound (inclusive) passed to `consumeNumberInRange`, so the final
    // value (`e_DECIMAL64`) is the `default` case in the switch below.
    enum OpCode {
        e_INTEGER          =  0,
        e_INT64            =  1,
        e_DOUBLE           =  2,
        e_OWNING_STRING    =  3,
        e_STRING_REFERENCE =  4,
        e_BOOLEAN          =  5,
        e_NULL             =  6,
        e_ERROR            =  7,
        e_ERROR_WITH_MSG   =  8,
        e_BINARY           =  9,
        e_ARRAY            = 10,
        e_MAP              = 11,
        e_INT_MAP          = 12,
        e_DECIMAL64        = 13,
        k_NUM_OPCODES      = e_DECIMAL64
    };

    const int opType = bslim::FuzzUtil::consumeNumberInRange<int>(
                                                             &fdv,
                                                             0,
                                                             k_NUM_OPCODES);

    bdld::Datum d;
    switch (opType) {
      case e_INTEGER: {
        d = dm(bslim::FuzzUtil::consumeNumber<int>(&fdv));
      } break;
      case e_INT64: {
        d = dm(bslim::FuzzUtil::consumeNumber<long long>(&fdv));
      } break;
      case e_DOUBLE: {
        unsigned long long bits =
                    bslim::FuzzUtil::consumeNumber<unsigned long long>(&fdv);
        double val;
        bsl::memcpy(&val, &bits, sizeof(val));
        d = dm(val);
      } break;
      case e_OWNING_STRING: {
        bslim::FuzzUtil::consumeRandomLengthString(&strBuf, &fdv, 50);
        d = dm(bsl::string_view(strBuf));
      } break;
      case e_STRING_REFERENCE: {
        bslim::FuzzUtil::consumeRandomLengthString(&strBuf, &fdv, 50);
        d = bdld::Datum::createStringRef(strBuf.data(),
                                         static_cast<int>(strBuf.size()),
                                         &lsa);
      } break;
      case e_BOOLEAN: {
        d = dm(bslim::FuzzUtil::consumeBool(&fdv));
      } break;
      case e_NULL: {
        d = dm();
      } break;
      case e_ERROR: {
        d = bdld::Datum::createError(
                                 bslim::FuzzUtil::consumeNumber<int>(&fdv));
      } break;
      case e_ERROR_WITH_MSG: {
        bslim::FuzzUtil::consumeRandomLengthString(&msgBuf, &fdv, 30);
        d = bdld::Datum::createError(
                                 bslim::FuzzUtil::consumeNumber<int>(&fdv),
                                 msgBuf,
                                 &lsa);
      } break;
      case e_BINARY: {
        bslim::FuzzUtil::consumeRandomLengthString(&binBuf, &fdv, 64);
        d = dm.bin(binBuf.data(), binBuf.size());
      } break;
      case e_ARRAY: {
        d = dm.a(
              dm(bslim::FuzzUtil::consumeNumber<int>(&fdv)),
              dm(bslim::FuzzUtil::consumeBool(&fdv)),
              dm());
      } break;
      case e_MAP: {
        bslim::FuzzUtil::consumeRandomLengthString(&keyBuf1, &fdv, 16);
        bslim::FuzzUtil::consumeRandomLengthString(&keyBuf2, &fdv, 16);
        d = dm.m(
              keyBuf1.c_str(),
                  dm(bslim::FuzzUtil::consumeNumber<int>(&fdv)),
              keyBuf2.c_str(),
                  dm());
      } break;
      case e_INT_MAP: {
        d = dm.im(
              bslim::FuzzUtil::consumeNumber<int>(&fdv),
                  dm(bslim::FuzzUtil::consumeNumber<int>(&fdv)),
              bslim::FuzzUtil::consumeNumber<int>(&fdv),
                  dm());
      } break;
      case e_DECIMAL64:
      default: {
        d = dm(BDLDFP_DECIMAL_DD(1.23));
      } break;
    }

    // Exercise trivial type-inspection accessors.
    (void)d.type();
    (void)d.isNull();

    // Exercise the print routines (the actual `DatumUtil` API under test).
    bsl::ostringstream oss;
    bdld::DatumUtil::typedPrint(oss, d, 0, -1);
    (void)oss.str();

    bsl::ostringstream oss2;
    bdld::DatumUtil::safeTypedPrint(oss2, d, 0, -1);
    (void)oss2.str();

    // The `LocalSequentialAllocator` releases all memory on scope exit, so no
    // per-`Datum` `destroy` call is required here.

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test  = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;  (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: Unexpected `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(bsls::Review::failByAbort);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test)  { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, replace `assert` with
        //    `ASSERT`, and insert `if (veryVerbose)` before all output
        //    operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";
        {
///Example 1: Showing the Difference Between an Integer and a Double Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are testing a system with operations that result in `bdld::Datum`
// values.  We verify that those results are what we have expected, *including*
// that their type matches.  After a getting an unexpected value, we use normal
// printing and get the following test failure: "Expected 1, got 1".
// Obviously, the type difference is not visible.  Instead, we can use
// `bdld::DatumUtil::typedPrint` to display the type as well as the value.
//
// First, let us define two `bdld::Datum` objects that have the same value, but
// use different types to represent them:
// ```
    bdld::Datum expected = bdld::Datum::createInteger(1);
    bdld::Datum actual   = bdld::Datum::createDouble(1.);

    ASSERT(expected != actual);
// ```
// Next, we demonstrate that printing these results in the same printout:
// ```
    bsl::ostringstream os;

    os << expected;
    bsl::string expectedStr = os.str();

    os.str("");
    os.clear();
    os << actual;
    bsl::string actualStr = os.str();

    ASSERT(expectedStr == actualStr);  // "1" is equal to "1"
// ```
//  Then, we create a shorthand for `bdld::DatumUtil::typedPrint`:
// ```
//  void printWithType(ostream& outStream, const bdld::Datum& object)
//  {
//      bdld::Datumutil::typedPrint(outStream, object, 0, -1);
//  }
// ```
// The 0 `level` and -1 `spacesPerLevel` results in single-line printout
// without a trailing newline, just like the stream output operator works.
//
// Finally, we verify that now we get a different printout for the two values:
// ```
    os.str("");
    os.clear();
    printWithType(os, expected);
    expectedStr = os.str();

    os.str("");
    os.clear();
    printWithType(os, actual);
    actualStr = os.str();

    ASSERT(expectedStr != actualStr);  // "1i" is *not* equal to "1."
// ```
      }  // close scope of example 1
      {
///Example 2: Avoiding Endless Printing of Data with Cycles
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are testing a system that creates a complex data structure
// that it stores is `bdld::Datum` objects.  Suppose that such a system doesn't
// use the fail-safe `Datum` builders for optimization purposes (for example it
// stores all map entries in one big allocation), and so it may be able to
// create a self-referential data structure.
//
// It is not easy to legitimately create self-referential data structures so we
// won't even attempt it in a short example code.
//
// First, we use a `bdld::DatumMaker` with a local allocator so we can ignore
// any cleanup and allocation:
// ```
    bdlma::LocalSequentialAllocator<1024> lsa;
    bdld::DatumMaker dm(&lsa);
// ```
// Next, we create two array datum's with a Nil element each:
// ```
    bdld::Datum arr1 = dm.a(dm());
    bdld::Datum arr2 = dm.a(dm());
// ```
// Then, we circumvent the type system to initialize their single elements to
// "contain" each other:
// ```
    const_cast<bdld::Datum&>(arr1.theArray()[0]) = arr2;
    const_cast<bdld::Datum&>(arr2.theArray()[0]) = arr1;
// ```
// Finally, we use the safe printing on this trapdoor of an endless loop to
// nevertheless safely print them:
// ```
    bsl::ostringstream os;
    bdld::DatumUtil::safeTypedPrint(os, arr1);
// ```
// Were we to print the results standard out, say
// ```
    if (verbose) {
        bdld::DatumUtil::safeTypedPrint(cout, arr2);
    }
// ```
// we would see something akin to:
// ```
//  <array@000000EFE4CFF928[
//      <array@000000EFE4CFF908[
//          <array@000000EFE4CFF928[!CYCLE!]>
//      ]>
//  ]>
// ```
// Where the hexadecimal numbers identify the arrays (and maps or int-maps) so
// we can clearly see that the cycle "points" back to the top-level array.
      }  // close scope of example 2
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SAFE PRINTING
        //
        // Concerns:
        // 1. Self-referential data does not cause endless loop/recursion.
        //
        // 2. "Normal" data printing was covered by previous test case.
        //
        // 3. Identifiers match when there is a self-reference.
        //
        // Plan:
        // 1. Table based testing with lines with varying values that are self-
        //    referential, and varying `level` and `spacesPerLevel` arguments.
        //
        // 2. `Datum` values are created using a `DatumMaker` and a sequential-
        //    allocator to avoid having to deal with deallocations.
        //
        // 3. Printing is done into a string stream for comparison.
        //
        // 4. All scalars that cannot create self-references have been
        //    previously tested, this case is *only* for self-referential data
        //    structures.
        //
        // Testing:
        //   ostream& safeTypedPrint(object, outStream, level, spacesPerLevel)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSAFE TYPED PRINTING"
                             "\n===================\n";

        bdlma::LocalSequentialAllocator<1024> lsa(&ta);
        bdld::DatumMaker dm(&lsa);

        // Self-referential types cannot be easily created using `bdld`, we use
        // `const_cast` here to create the input data for the test-table lines.

        // Self-referencing array
        bdld::Datum arr1 = dm.a(dm());
        const_cast<bdld::Datum&>(arr1.theArray()[0]) = arr1;

        // Two arrays creating a loop
        bdld::Datum arr2a = dm.a(dm());
        bdld::Datum arr2b = dm.a(dm());
        const_cast<bdld::Datum&>(arr2a.theArray()[0]) = arr2b;
        const_cast<bdld::Datum&>(arr2b.theArray()[0]) = arr2a;

        // Three arrays creating a circle
        bdld::Datum arr3a = dm.a(dm());
        bdld::Datum arr3b = dm.a(dm());
        bdld::Datum arr3c = dm.a(dm());
        const_cast<bdld::Datum&>(arr3a.theArray()[0]) = arr3b;
        const_cast<bdld::Datum&>(arr3b.theArray()[0]) = arr3c;
        const_cast<bdld::Datum&>(arr3c.theArray()[0]) = arr3a;

        // Three arrays having a sub-loop
        bdld::Datum arr4a = dm.a(dm());
        bdld::Datum arr4b = dm.a(dm());
        bdld::Datum arr4c = dm.a(dm());
        const_cast<bdld::Datum&>(arr4a.theArray()[0]) = arr4b;
        const_cast<bdld::Datum&>(arr4b.theArray()[0]) = arr4c;
        const_cast<bdld::Datum&>(arr4c.theArray()[0]) = arr4b;

        // Self-referencing map
        bdld::Datum map1 = dm.m("myself", dm());
        const_cast<bdld::Datum&>(map1.theMap()[0].value()) = map1;

        // Two maps creating a loop
        bdld::Datum map2a = dm.m("vice", dm());
        bdld::Datum map2b = dm.m("versa", dm());
        const_cast<bdld::Datum&>(map2a.theMap()[0].value()) = map2b;
        const_cast<bdld::Datum&>(map2b.theMap()[0].value()) = map2a;

        // Self-referencing int-map
        bdld::Datum imap1 = dm.im(1, dm());
        const_cast<bdld::Datum&>(imap1.theIntMap()[0].value()) = imap1;

        // Two maps creating a loop
        bdld::Datum imap2a = dm.im(2, dm());
        bdld::Datum imap2b = dm.im(1, dm());
        const_cast<bdld::Datum&>(imap2a.theIntMap()[0].value()) = imap2b;
        const_cast<bdld::Datum&>(imap2b.theIntMap()[0].value()) = imap2a;

        static const struct {
            long        d_line;
            bdld::Datum d_datum;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expectedOutput;
        } TEST_DATA[] = {
            // === ARRAY ===
            { L_, arr1,   0, -1, "<array@arr1[ <array@arr1[!CYCLE!]> ]>"     },

            { L_, arr2a,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr1[!CYCLE!]> ]> ]>"        },
            { L_, arr2b,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr1[!CYCLE!]> ]> ]>"        },

            { L_, arr3a,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr3[ <array@arr1[!CYCLE!]> "
                                                                 "]> ]> ]>"  },
            { L_, arr3b,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr3[ <array@arr1[!CYCLE!]> "
                                                                 "]> ]> ]>"  },
            { L_, arr3c,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr3[ <array@arr1[!CYCLE!]> "
                                                                 "]> ]> ]>"  },

            { L_, arr4a,  0, -1,
              "<array@arr1[ <array@arr2[ <array@arr3[ <array@arr2[!CYCLE!]> "
                                                                 "]> ]> ]>"  },

            // === MAP ===
            { L_, map1,   0, -1,
              "<map@map1{ \"myself\": <map@map1{!CYCLE!}> }>"                },
            { L_, map1,   0, 4, "<map@map1{\n"
                                "    \"myself\": <map@map1{!CYCLE!}>\n"
                                "}>\n"                                       },

            { L_, map2a,   0, -1,
              "<map@map1{ \"vice\": <map@map2{ \"versa\": <map@map1{!CYCLE!}>"
                                                                   " }> }>"  },
            { L_, map2b,   0, -1,
              "<map@map1{ \"versa\": <map@map2{ \"vice\": <map@map1{!CYCLE!}>"
                                                                   " }> }>"  },
            { L_, map2a,   0, 4, "<map@map1{\n"
                                 "    \"vice\": <map@map2{\n"
                                 "        \"versa\": <map@map1{!CYCLE!}>\n"
                                 "    }>\n}>\n"                              },
            { L_, map2b,   0, 4, "<map@map1{\n"
                                 "    \"versa\": <map@map2{\n"
                                 "        \"vice\": <map@map1{!CYCLE!}>\n"
                                 "    }>\n}>\n"                              },

            // === INT-MAP ===
            { L_, imap1,   0, -1,
              "<intmap@map1{ 1: <intmap@map1{!CYCLE!}> }>"                   },
            { L_, imap1,   0, 4, "<intmap@map1{\n"
                                 "    1: <intmap@map1{!CYCLE!}>\n"
                                 "}>\n"                                      },

            { L_, imap2a,  0, -1,
              "<intmap@map1{ 2: <intmap@map2{ 1: <intmap@map1{!CYCLE!}>"
                                                                   " }> }>"  },
            { L_, imap2b,  0, -1,
              "<intmap@map1{ 1: <intmap@map2{ 2: <intmap@map1{!CYCLE!}>"
                                                                   " }> }>"  },
            { L_, imap2a,  0, 4, "<intmap@map1{\n"
                                  "    2: <intmap@map2{\n"
                                  "        1: <intmap@map1{!CYCLE!}>\n"
                                  "    }>\n}>\n"                             },
            { L_, imap2b,  0, 4, "<intmap@map1{\n"
                                 "    1: <intmap@map2{\n"
                                 "        2: <intmap@map1{!CYCLE!}>\n"
                                 "    }>\n}>\n"                              },
        };
        const size_t TEST_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA;

        for (size_t ti = 0; ti < TEST_SIZE; ++ti) {
            const long         LINE       = TEST_DATA[ti].d_line;
            const bdld::Datum  DATUM      = TEST_DATA[ti].d_datum;
            const int          LEVEL      = TEST_DATA[ti].d_level;
            const int          SP_PER_LEV = TEST_DATA[ti].d_spacesPerLevel;
            const char * const EXPECTED   = TEST_DATA[ti].d_expectedOutput;

            if (veryVeryVerbose) {
                T_ P_(LINE) P_(DATUM) P_(LEVEL) P(SP_PER_LEV);
            }

            bsl::ostringstream os;
            Util::safeTypedPrint(os, DATUM, LEVEL, SP_PER_LEV);
            const bsl::string RESULT = os.str();

            typedef QuotedEscapedStringPrinterRef QnEs;
            ASSERTV(QnEs(RESULT), QnEs(EXPECTED),
                    verifySafePrintResult(RESULT, EXPECTED));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TYPED PRINTING
        //
        // Concerns:
        // 1. Each type is distinguished as intended.
        //
        // 2. `level` and `spacesPerLevel` arguments are obeyed.
        //
        // 3. The scalar values themselves are printed properly.
        //
        // Plan:
        // 1. Table based testing with lines for each type with varying values,
        //    and varying `level` and `spacesPerLevel` arguments.
        //
        // 2. `Datum` values are created using a `DatumMaker` and a sequential-
        //    allocator to avoid having to deal with deallocations.
        //
        // 3. Printing is done into a string stream for comparison.
        //
        // Testing:
        //   ostream& typedPrint(object, outputStream, level, spacesPerLevel)
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTYPED PRINTING"
                             "\n==============\n";

        bdlma::LocalSequentialAllocator<1024> lsa(&ta);
        bdld::DatumMaker dm(&lsa);

        const unsigned char BIN_DATA[] =
                        "\xDE\xCA\xFB\xAD\0\x1F\xAD\xED\x10\xAD\xC0\xFF\xEE";
        const size_t BIN_SIZE = (sizeof BIN_DATA) - 1; // No closing null
        const char BIN_STR[] = "<binary(DECAFBAD001FADED10ADC0FFEE)>";

        static const struct {
            long        d_line;
            bdld::Datum d_datum;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expectedOutput;
        } TEST_DATA[] = {
            // === BASIC TYPES === (cannot reference other types)

            { L_, dm(),   0, -1, "<nil>"      },  // Nil: the type and value

            { L_, dm( 1), 0, -1,  "1i"        },  // Small integers
            { L_, dm(-1), 0, -1, "-1i"        },
            { L_, dm( 0), 0, -1,  "0i"        },
            { L_, dm(42), 0, -1, "42i"        },

            { L_, dm( 1ll),  0, -1,  "1L"     },  // 64-bit integers
            { L_, dm(-1ll),  0, -1, "-1L"     },
            { L_, dm( 0ll),  0, -1,  "0L"     },
            { L_, dm(42ll),  0, -1, "42L"     },

            { L_, dm( 1.  ), 0, -1,  "1."     },  // Binary floating-point
            { L_, dm(-1.  ), 0, -1, "-1."     },
            { L_, dm( 0.  ), 0, -1,  "0."     },
            { L_, dm(42.  ), 0, -1, "42."     },

            { L_, dm( 1.23), 0, -1,  "1.23"   },
            { L_, dm(-1.23), 0, -1, "-1.23"   },
            { L_, dm( 0.05), 0, -1,  "0.05"   },
            { L_, dm(42.25), 0, -1, "42.25"   },

#define MDD(X) dm(BDLDFP_DECIMAL_DD(X))
            { L_, MDD( 1.  ), 0, -1,  "1.d"   },  // Decimal floating-point
            { L_, MDD(-1.  ), 0, -1, "-1.d"   },
            { L_, MDD( 0.  ), 0, -1,  "0.d"   },
            { L_, MDD(42.  ), 0, -1, "42.d"   },

            { L_, MDD( 1.23), 0, -1,  "1.23d" },
            { L_, MDD(-1.23), 0, -1, "-1.23d" },
            { L_, MDD( 0.05), 0, -1,  "0.05d" },
            { L_, MDD(42.25), 0, -1, "42.25d" },
#undef MDD

            { L_, dm("some text\n..more"), 0, -1,"\"some text\\n..more\"" },

            { L_, dm(bdld::DatumUdt(&dm, 42)), 0, -1,
                  "<udt(type:42, ptr:" },

            { L_, dm.bin(BIN_DATA, BIN_SIZE), 0, -1, BIN_STR },

            // === COMPOUND TYPES ===

            // <| ARRAY |>
            { L_, dm.a(dm()),             0, -1, "<array[ <nil> ]>"          },
            { L_, dm.a(dm.a(dm())),       0, -1,
                                              "<array[ <array[ <nil> ]> ]>"  },
            { L_, dm.a(dm(), dm.a(dm())), 0, -1,
                                        "<array[ <nil>, <array[ <nil> ]> ]>" },

            // <| MAP |>
            { L_, dm.m("nil", dm()),              0, -1,
                                                  "<map{ \"nil\": <nil> }>"  },
            { L_, dm.m("map", dm.m("nil", dm())), 0, -1,
                                "<map{ \"map\": <map{ \"nil\": <nil> }> }>"  },
            { L_, dm.m("nil", dm(),
                       "map", dm.m("nil", dm())), 0, -1,
                "<map{ \"nil\": <nil>, \"map\": <map{ \"nil\": <nil> }> }>"  },

            // <| INT-MAP |>
            { L_, dm.im(1, dm()),          0, -1, "<intmap{ 1: <nil> }>"     },
            { L_, dm.im(2, dm.im(5, dm())), 0, -1,
                                      "<intmap{ 2: <intmap{ 5: <nil> }> }>"  },
            { L_, dm.im( 3, dm(),
                        15, dm.im(-322, dm())), 0, -1,
                        "<intmap{ 3: <nil>, 15: <intmap{ -322: <nil> }> }>"  },

            // <| ARRAY, MAP MIX |>
            { L_, dm.a(dm.m("nil", dm())), 0, -1,
                                       "<array[ <map{ \"nil\": <nil> }> ]>"  },
            { L_, dm.m("arr", dm.a(dm())), 0, -1,
                                       "<map{ \"arr\": <array[ <nil> ]> }>"  },

            // <| ARRAY, MAP, INT-MAP MIX |>
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))), 0, -1,
                "<array[ <map{ \"nil\": <intmap{ 1848: \"revolution\" }> }> ]>"
            },

            // === EXERCISE INDENTATION ===

            // Variations on basic `level` & `spacesPerLevel` behavior
            { L_, dm(), 0, 0,  "<nil>\n"     },

            { L_, dm(), 1, 0,  "<nil>\n"     },
            { L_, dm(), 1, 1,  " <nil>\n"    },
            { L_, dm(), 1, 2,  "  <nil>\n"   },
            { L_, dm(), 1, 4,  "    <nil>\n" },

            { L_, dm(), 0, 0,  "<nil>\n"     },
            { L_, dm(), 0, 1,  "<nil>\n"     },
            { L_, dm(), 0, 2,  "<nil>\n"     },
            { L_, dm(), 0, 4,  "<nil>\n"     },

            { L_, dm(), -5, 0,  "<nil>\n"    },
            { L_, dm(), -5, 1,  "<nil>\n"    },
            { L_, dm(), -5, 2,  "<nil>\n"    },
            { L_, dm(), -5, 4,  "<nil>\n"    },

            // Variations on compound `level` & `spacesPerLevel` behavior
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))),  0, 4,
                  "<array[\n"
                  "    <map{\n"
                  "        \"nil\": <intmap{\n"
                  "            1848: \"revolution\"\n"
                  "        }>\n"
                  "    }>\n"
                  "]>\n"
            },
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))),  1, 4,
                  "    <array[\n"
                  "        <map{\n"
                  "            \"nil\": <intmap{\n"
                  "                1848: \"revolution\"\n"
                  "            }>\n"
                  "        }>\n"
                  "    ]>\n"
            },
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))), -1, 4,
                  "<array[\n"
                  "        <map{\n"
                  "            \"nil\": <intmap{\n"
                  "                1848: \"revolution\"\n"
                  "            }>\n"
                  "        }>\n"
                  "    ]>\n"
            },
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))),  1, -4,
            "    <array[ <map{ \"nil\": <intmap{ 1848: \"revolution\" }> }> ]>"
            },
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))),  0, -4,
                "<array[ <map{ \"nil\": <intmap{ 1848: \"revolution\" }> }> ]>"
            },
            { L_, dm.a(dm.m("nil", dm.im(1848, "revolution"))), -1, -4,
                "<array[ <map{ \"nil\": <intmap{ 1848: \"revolution\" }> }> ]>"
            },
        };
        const size_t TEST_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA;

        for (size_t ti = 0; ti < TEST_SIZE; ++ti) {
            const long         LINE       = TEST_DATA[ti].d_line;
            const bdld::Datum  DATUM      = TEST_DATA[ti].d_datum;
            const int          LEVEL      = TEST_DATA[ti].d_level;
            const int          SP_PER_LEV = TEST_DATA[ti].d_spacesPerLevel;
            const char * const EXPECTED   = TEST_DATA[ti].d_expectedOutput;

            if (veryVeryVerbose) {
                T_ P_(LINE) P_(DATUM) P_(LEVEL) P(SP_PER_LEV);
            }

            bsl::ostringstream os;
            Util::typedPrint(os, DATUM, LEVEL, SP_PER_LEV);
            const bsl::string RESULT = os.str();

            typedef QuotedEscapedStringPrinterRef QnEs;
            if (DATUM.type() != bdld::Datum::e_USERDEFINED) {
                ASSERTV(QnEs(RESULT), QnEs(EXPECTED), RESULT == EXPECTED);
            }
            else {
                ASSERTV(QnEs(RESULT), QnEs(EXPECTED),
                       RESULT.starts_with(EXPECTED) && RESULT.ends_with(")>"));
            }

            bsl::ostringstream oss;
            Util::safeTypedPrint(oss, DATUM, LEVEL, SP_PER_LEV);
            const bsl::string SAFE_RESULT = oss.str();

            typedef QuotedEscapedStringPrinterRef QnEs;
            ASSERTV(QnEs(SAFE_RESULT), QnEs(RESULT),
                    safeAndNormalMatches(SAFE_RESULT, RESULT));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Call the utility functions to verify their existence and basics.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        bdlma::LocalSequentialAllocator<1024> lsa(&ta);
        bdld::DatumMaker dm(&lsa);

        const bdld::Datum arr = dm.a(
            dm(1.),
            dm(1),
            dm(1ll),
            dm(BDLDFP_DECIMAL_DD(1.)),
            dm("just a\tstring"),
            dm(bdld::DatumUdt(&dm, 42)),
            dm.bin(&lsa, 16),
            dm());

        const bdld::Datum imap = dm.im(
            1, dm(1.),
            2, dm(1),
            3, dm(1ll),
            4, dm(BDLDFP_DECIMAL_DD(1.)),
            5, arr,
            6, dm(),
            7, dm());

        const bdld::Datum map = dm.mok(
            "dbl", dm(1.),
            "int", dm(1),
            "i64", dm(1ll),
            "dfp", dm(BDLDFP_DECIMAL_DD(1.)),
            "str", dm("at first you fail\n"
                      "trie and trie again"),
            "udt", dm(bdld::DatumUdt(&dm, 42)),
            "bin", dm.bin(&lsa, 16),
            "arr", arr,
            "imap", imap,
            "nil1", dm(),
            "nil2", dm(),
            "nil3", dm());

        if (verbose) {
            Util::typedPrint(bsl::cout, arr , 0, -1);
            bsl::cout << "\n";
            Util::typedPrint(bsl::cout, imap, 0, -1);
            bsl::cout << "\n";
            Util::typedPrint(bsl::cout, map, 0, -1);
            bsl::cout << "\n\n";
            Util::typedPrint(bsl::cout, arr);
            bsl::cout << "\n";
            Util::typedPrint(bsl::cout, imap);
            bsl::cout << "\n";
            Util::typedPrint(bsl::cout, map);
            bsl::cout << "\n";
        }
      } break;
      default: {
        if (-1 == test) {
            // ----------------------------------------------------------------
            // FUZZ TESTING SMOKE
            //   Exercise the `LLVMFuzzerTestOneInput` routine without
            //   LibFuzzer, so it can be run as part of the normal test
            //   driver.
            //
            // Concerns:
            // 1. That `LLVMFuzzerTestOneInput` returns 0 and does not crash
            //    on a small set of hand-picked inputs (empty, and buffers
            //    of increasing size filled with a deterministic
            //    pseudo-random byte sequence).
            //
            // Plan:
            // 1. Fill a fixed-size buffer with a deterministic
            //    pseudo-random byte sequence (a linear congruential step
            //    seeded with a non-zero constant so no byte is left as
            //    the vector's default-constructed zero).
            //
            // 2. Invoke `LLVMFuzzerTestOneInput` directly with the empty
            //    input, then with increasing-length prefixes of the
            //    buffer, asserting that each call returns 0.
            //
            // Testing:
            //   int LLVMFuzzerTestOneInput(const uint8_t *, size_t);
            // ----------------------------------------------------------------
            if (verbose) cout << "\nFUZZ TESTING SMOKE"
                                 "\n==================\n";

            enum { k_BUF_LEN = 256 };
            bsl::vector<uint8_t> buf(&ta);
            buf.reserve(k_BUF_LEN);

            // Deterministic pseudo-random-looking fill; the seed and
            // multiplier are non-zero so that every byte is overwritten
            // to a value distinct from the vector's default-constructed
            // zero.
            unsigned state = 0xABCDEF01u;
            for (unsigned i = 0; i < k_BUF_LEN; ++i) {
                state = state * 1103515245u + 12345u;
                buf.push_back(static_cast<uint8_t>(state >> 16));
            }

            ASSERT(0 == LLVMFuzzerTestOneInput(0, 0));
            for (size_t len = 0; len <= k_BUF_LEN; len += 8) {
                ASSERT(0 == LLVMFuzzerTestOneInput(buf.data(), len));
            }
        }
        else {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
        }
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
