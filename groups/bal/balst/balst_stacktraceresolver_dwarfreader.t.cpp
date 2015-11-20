// balst_stacktraceresolver_dwarfreader.t.cpp                         -*-C++-*-

#include <balst_stacktraceresolver_dwarfreader.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_DWARF

#include <balst_objectfileformat.h>
#include <balst_stacktraceresolver_filehelper.h>

#include <bdls_filesystemutil.h>

#include <bslim_testutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <sys/types.h>
#include <unistd.h>

#include <dwarf.h>

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
// [10] stringForAt(unsigned);
// [10] stringForForm(unsigned);
// [10] stringForLNE(unsigned);
// [10] stringForLNS(unsigned);
// [10] stringForInlineState(unsigned);
// [10] stringForTag(unsigned);
// [ 9] skipForm(unsigned);    (partial)
// [ 8] readOffset(Offset *, bsl::size_t);
// [ 8] readOffsetFromForm(Offset *, unsigned);
// [ 8] skipForm(unsigned);    (partial)
// [ 7] readULEB128(TYPE *dst);
// [ 6] readLEB128(TYPE *dst);
// [ 6] skipULEB128();
// [ 5] readString(bsl::string *);
// [ 5] readStringAt(bsl::string *, Offset);
// [ 5] readStringFromForm(bsl::string *, Obj *, unsigned);
// [ 5] skipString();
// [ 4] readInitialLength(Offset *dst);
// [ 4] readSectionOffset(Offset *dst);
// [ 4] offset();
// [ 4] offsetSize();
// [ 3] readAddress(UintPtr *dst, unsigned form);
// [ 3] readAddressSize();
// [ 3] readAddress(UintPtr *dst);
// [ 2] readValue(TYPE *);
// [ 1] Obj();
// [ 1] init(FH *, char *, const Section&, Offset);
// [ 1] skipBytes(Offset);
// [ 1] skipTo(Offset);
// [ 1] atEndOfSection();
// [ 1] offset();
//-----------------------------------------------------------------------------

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

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// Negative testing is not really appropriate in this component, because the
// functions are all designed to return non-zero if they fail, rather than
// aborting.

// ============================================================================
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                 GLOBAL HELPER TYPES & CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::StackTraceResolver_DwarfReader Obj;
typedef balst::StackTraceResolver_FileHelper  FH;
typedef Obj::Section                          Section;
typedef Obj::Offset                           Offset;
typedef Obj::UintPtr                          UintPtr;
typedef bsls::Types::Uint64                   Uint64;
typedef bsls::Types::Int64                    Int64;
typedef bdls::FilesystemUtil                  FU;
typedef FU::FileDescriptor                    FD;

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template <class TYPE>
int sz(const TYPE&)
{
    return static_cast<int>(sizeof(TYPE));
}

template <class TYPE>
void setToMax(TYPE *value)
{
    *value = bsl::numeric_limits<TYPE>::max();
}

template <class TYPE>
void setToMin(TYPE *value)
{
    *value = bsl::numeric_limits<TYPE>::min();
}

static
void writeByte(FD fd, unsigned char u)
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeShort(FD fd, unsigned short u)
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeUnsigned(FD fd, unsigned u)
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeUint64(FD fd, Uint64 u)
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeInt64(FD fd, Int64 u)
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeGarbage(FD fd, unsigned numBytes)
{
    const char data[] = { "~`!@#$%^&*()_-+={}[]:;<>,.?/|\\"
                          "abcdefghijklmnopqrstuvwxyz0123456789" };
    const unsigned dataLen = static_cast<unsigned>(sizeof(data) - 1);

    while (0 < numBytes) {
        const unsigned toWrite = bsl::min(numBytes, dataLen);
        unsigned rc = FU::write(fd, data, toWrite);
        ASSERT(toWrite == rc);

        numBytes -= toWrite;
    }
}

static
void writeLEB128(FD fd, Int64 value)
{
    BSLMF_ASSERT((static_cast<Offset>(-1) >> 7) == -1);

    const char extBit = static_cast<char>(0x80);
    signed char byte;
    do {
        byte = static_cast<char>(value & 0x7f);
        value >>= 7;
        if ((0 == value && !(byte & 0x40)) || (-1 == value && (byte & 0x40))) {
            ;   // No more bytes are necessary after this one, don't set
                // 'extBit'.
        }
        else {
            byte |= extBit;
        }

        ASSERT(1 == FU::write(fd, &byte, 1));
    } while (extBit & byte);
}

static
void writeULEB128(FD fd, Uint64 value)
{
    const char extBit = static_cast<char>(0x80);
    char byte;
    do {
        byte = static_cast<char>(value & 0x7f);
        value >>= 7;
        if (0 != value) {
            byte |= extBit;
        }

        ASSERT(1 == FU::write(fd, &byte, 1));
    } while (extBit & byte);
}

static
Offset getOffset(FD fd)
{
    return FU::seek(fd, 0, FU::e_SEEK_FROM_CURRENT);
}

// ============================================================================
//                            GLOBAL COMPILE TIME ASSERTS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(sizeof(Offset) == sizeof(Uint64));

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bsl::string origWorkingDirectory;
    ASSERT(0 == FU::getWorkingDirectory(&origWorkingDirectory));

    bsl::string tmpWorkingDir;
    {
        // Must not call 'tempFileName' here, because 'tempFileName' would
        // create a plain file with the result name, and the attempt to create
        // the directory would fail.

        char host[80];
        ASSERT(0 == ::gethostname(host, sizeof(host)));

        bsl::ostringstream oss;
        oss << "tmp.stacktraceresolver_dwarfreader.case_" << test << '.' <<
                                                     host << '.' << ::getpid();
        tmpWorkingDir = oss.str();
    }
    if (veryVerbose) P(tmpWorkingDir);

    if (FU::exists(tmpWorkingDir)) {
        // Sometimes the cleanup at the end of this program is unable to clean
        // up files, so we might encounter leftovers from a previous run, but
        // these can usually be deleted if sufficient time has elapsed.  If
        // we're not able to clean it up now, old files may prevent the test
        // case we're running this time from working.  So we want this assert
        // to fail to give the tester a 'heads-up' as to what went wrong.

        LOOP_ASSERT(tmpWorkingDir, 0 == FU::remove(tmpWorkingDir, true));
    }
    ASSERT(0 == FU::createDirectories(  tmpWorkingDir, true));
    ASSERT(0 == FU::setWorkingDirectory(tmpWorkingDir));

    switch (test) {
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ENUM NAME METHODS
        //
        // Concerns:
        //   That the enum name methods print reasonable names for all the
        //   the enums they are meant to.
        //
        // Plan:
        //   Call static methods for all possible value, store resulting
        //   strings in a set to detect duplicates, verify that strings
        //   returned don't contain '?'.
        //
        // Testing:
        //   stringForAt(unsigned);
        //   stringForForm(unsigned);
        //   stringForLNE(unsigned);
        //   stringForLNS(unsigned);
        //   stringForInlineState(unsigned);
        //   stringForTag(unsigned);
        // --------------------------------------------------------------------

        bsl::set<bsl::string> ss;

        for (unsigned ii = 1, jj = 0; ii <= Obj::e_DW_AT_linkage_name; ++ii) {
            if ((DW_AT_name < ii && ii < DW_AT_ordering) || 0xe == ii ||
                        0x1f == ii || 0x23 == ii || 0x24 == ii || 0x26 == ii ||
                        0x28 == ii || 0x29 == ii || 0x2b == ii || 0x2d == ii ||
                                                                  0x30 == ii) {
                continue;
            }

            bsl::string s = Obj::stringForAt(ii);
            ASSERTV(ii, s, bsl::string::npos == s.find('?'));

            if (ii < Obj::e_DW_AT_signature) {
                ASSERT(s.substr(0, 6) == "DW_AT_");
            }
            else {
                ASSERT(s.substr(0, 8) == "e_DW_AT_");
            }

            ss.insert(s);
            ++jj;
            ASSERTV(s, ss.size(), jj, ss.size() == jj);
        }
        ss.clear();

        for (unsigned ii = 1, jj = 0; ii <= Obj::e_DW_FORM_ref_sig8; ++ii) {
            if (2 == ii || (Obj::e_DW_FORM_flag_present < ii &&
                                               ii < Obj::e_DW_FORM_ref_sig8)) {
                continue;
            }

            bsl::string s = Obj::stringForForm(ii);
            ASSERTV(ii, s, bsl::string::npos == s.find('?'));

            if (ii < Obj::e_DW_FORM_sec_offset) {
                ASSERT(s.substr(0, 8) == "DW_FORM_");
            }
            else {
                ASSERT(s.substr(0, 10) == "e_DW_FORM_");
            }

            ss.insert(s);
            ++jj;
            ASSERTV(ii, ss.size() == jj);
        }
        ss.clear();

        for (unsigned ii = 1; ii <= DW_INL_declared_inlined; ++ii) {
            bsl::string s = Obj::stringForInlineState(ii);

            ASSERT(s.substr(0, 7) == "DW_INL_");

            ss.insert(s);
            ASSERT(ss.size() == ii);
        }
        ss.clear();

        for (unsigned ii = 1; ii <= Obj::e_DW_LNE_set_discriminator; ++ii) {
            bsl::string s = Obj::stringForLNE(ii);
            ASSERTV(ii, s, bsl::string::npos == s.find('?'));

            if (ii < Obj::e_DW_LNE_set_discriminator) {
                ASSERT(s.substr(0, 7) == "DW_LNE_");
            }
            else {
                ASSERT(s.substr(0, 9) == "e_DW_LNE_");
            }

            ss.insert(s);
            ASSERT(ss.size() == ii);
        }
        ss.clear();

        for (unsigned ii = 1; ii <= DW_LNS_set_isa; ++ii) {
            bsl::string s = Obj::stringForLNS(ii);
            ASSERTV(ii, s, bsl::string::npos == s.find('?'));

            ASSERT(s.substr(0, 7) == "DW_LNS_");

            ss.insert(s);
            ASSERT(ss.size() == ii);
        }
        ss.clear();

        for (unsigned ii = 1, jj = 0; ii <= Obj::e_DW_TAG_template_alias;
                                                                        ++ii) {
            if ((DW_TAG_formal_parameter < ii &&
                                ii < DW_TAG_imported_declaration) || 9 == ii ||
                                        0xc == ii || 0xe == ii || 0x14 == ii) {
                continue;
            }

            bsl::string s = Obj::stringForTag(ii);
            ASSERTV(ii, s, bsl::string::npos == s.find('?'));

            if (ii < Obj::e_DW_TAG_type_unit) {
                ASSERT(s.substr(0, 7) == "DW_TAG_");
            }
            else {
                ASSERT(s.substr(0, 9) == "e_DW_TAG_");
            }

            ss.insert(s);
            ++jj;
            ASSERTV(ii, ss.size() == jj);
        }
        ss.clear();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING OTHER SKIPFORM
        //
        // Concerns:
        //   That 'skipForm' works for those forms not supported by
        //   'readOffsetFromForm'.
        //
        // Plan:
        //   Write data to file, and skip over it, carefully ensuring that
        //   it skips by the right amount.
        //
        // Testing:
        //   skipForm(unsigned);    (partial)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING OTHER SKIPFORM\n"
                             "======================\n";

        bsl::vector<Offset> offsets;

        const char *fn = "skipForm_test.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        writeUnsigned(fd, 0);                // initial length (will overwrite
                                             // later

        offsets.push_back(getOffset(fd));

        writeByte(fd, 4);                    // Address size

        offsets.push_back(getOffset(fd));
        offsets.push_back(getOffset(fd));    // e_DW_FORM_flag_present

        for (int ii = 0; ii <= 56; ii += 8) {    // DW_FORM_ref_udata
            writeULEB128(fd, 1ULL << ii);
            offsets.push_back(getOffset(fd));
        }

        const char str[] = { "woof woof woof\n" };    // DW_FORM_string
        ASSERT(sz(str) == FU::write(fd, str, sz(str)));

        offsets.push_back(getOffset(fd));

        writeUnsigned(fd, 100);                 // DW_FORM_strp

        offsets.push_back(getOffset(fd));

        writeUnsigned(fd, 0xf0000);             // DW_FORM_addr 4

        offsets.push_back(getOffset(fd));

        writeUnsigned(fd, 0xf000f);             // DW_FORM_ref_addr 4

        offsets.push_back(getOffset(fd));

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        writeUint64(fd, 0xfULL << 32);          // DW_FORM_addr 8

        offsets.push_back(getOffset(fd));

        writeUint64(fd, 0xffULL << 32);         // DW_FORM_ref_addr 8

        offsets.push_back(getOffset(fd));
#endif

        writeByte(fd, 20);                          // DW_FORM_block1
        writeGarbage(fd, 20);

        offsets.push_back(getOffset(fd));

        writeShort(fd, 1000);                       // DW_FORM_block2
        writeGarbage(fd, 1000);

        offsets.push_back(getOffset(fd));

        writeUnsigned(fd, 1001);                    // DW_FORM_block4
        writeGarbage(fd, 1001);

        offsets.push_back(getOffset(fd));

        writeULEB128(fd, 1002);                     // DW_FORM_block
        writeGarbage(fd, 1002);

        offsets.push_back(getOffset(fd));

        writeULEB128(fd, 1003);                     // e_DW_FORM_exprloc
        writeGarbage(fd, 1003);

        offsets.push_back(getOffset(fd));

        ASSERT(0 == FU::seek(fd, 0, FU::e_SEEK_FROM_BEGINNING));

        writeUnsigned(fd, static_cast<unsigned>(offsets.back() - 4));
                                                    // Overwrite initial length

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(0, 0xffffffff);
        rc = mX.init(&helper, buffer, sec, 0xffffffff);
        ASSERT(0 == rc);

        rc = mX.setEndOffset(offsets.back());

        unsigned jj = 0;

        Offset x;
        rc = mX.readInitialLength(&x);
        ASSERT(0 == rc);
        ASSERT(offsets.back() - 4 == x);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.readAddressSize();
        ASSERT(0 == rc);
        ASSERT(4 == X.addressSize());

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(Obj::e_DW_FORM_flag_present);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        for (int ii = 0; ii <= 56; ii += 8) {    // DW_FORM_ref_udata
            rc = mX.skipForm(DW_FORM_ref_udata);
            ASSERT(0 == rc);
            ASSERT(offsets[jj++] == X.offset());
        }

        rc = mX.skipForm(DW_FORM_string);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_strp);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_addr);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_ref_addr);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        rc = mX.setAddressSize(8);
        ASSERT(0 == rc);
        ASSERT(8 == X.addressSize());

        rc = mX.skipForm(DW_FORM_addr);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_ref_addr);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());
#endif

        rc = mX.skipForm(DW_FORM_block1);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_block2);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_block4);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(DW_FORM_block);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        rc = mX.skipForm(Obj::e_DW_FORM_exprloc);
        ASSERT(0 == rc);

        ASSERT(offsets[jj++] == X.offset());

        ASSERT(offsets.size() == jj);
        ASSERT(X.atEndOfSection());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VARIOUS READOFFSET AND SOME SKIPFORM
        //
        // Concerns:
        //   That 'readOffset' and 'readOffsetFromForm' work properly, along
        //   with 'skipForm' for those forms supported by 'readOffsetFromForm'.
        //
        // Plan:
        //   Write offsets of various lengths to a file, then read them back.
        //
        // Testing:
        //   readOffset(Offset *, bsl::size_t);
        //   readOffsetFromForm(Offset *, unsigned);
        //   skipForm(unsigned);    (partial)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING VARIOUS READOFFSET\n"
                             "==========================\n";

        const char fn[] = { "readOffset_test.bin" };

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        if (verbose) cout << "Writing Data\n";

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset charOffset = getOffset(fd);

        for (unsigned uu = 0; uu < 256; ++uu) {
            unsigned char u = static_cast<unsigned char>(uu);

            writeByte(fd, u);
        }
        unsigned char maxChar, minChar;
        setToMax(&maxChar);
        setToMin(&minChar);

        writeByte(fd, maxChar);
        writeByte(fd, minChar);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset shortOffset = getOffset(fd);

        for (unsigned uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            unsigned short u = static_cast<unsigned short>(uu);

            writeShort(fd, u);
        }
        unsigned short maxShort, minShort;
        setToMax(&maxShort);
        setToMin(&minShort);

        writeShort(fd, maxShort);
        writeShort(fd, minShort);

        const Offset initialLengthOffset = getOffset(fd);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset unsignedOffset = getOffset(fd);

        for (Uint64 uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            unsigned u = static_cast<unsigned>(uu);

            writeUnsigned(fd, u);
        }
        unsigned maxUnsigned, minUnsigned;
        setToMax(&maxUnsigned);
        setToMin(&minUnsigned);

        writeUnsigned(fd, maxUnsigned);
        writeUnsigned(fd, minUnsigned);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset offsetOffset = getOffset(fd);

        Offset maxOffset, minOffset;
        setToMax(&maxOffset);
        setToMin(&minOffset);

        ASSERT(Obj::s_maxOffset == maxOffset);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                writeInt64(fd, o);
            }
        }
        writeInt64(fd, maxOffset);
        writeInt64(fd, minOffset);

        const Offset indirectOffset = getOffset(fd);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset leb128Offset = getOffset(fd);

        ASSERT(indirectOffset ==
                      FU::seek(fd, indirectOffset, FU::e_SEEK_FROM_BEGINNING));

        const Int64 indirectData[] = { 0, 1, 2, 1000, 10001, 100 * 1000,
                200 * 1000, 100ULL << 32, 200ULL << 32, 300ULL << 32,
                -400LL << 32 };
        BSLMF_ASSERT(11 == sizeof indirectData / sizeof *indirectData);

        writeULEB128(fd, DW_FORM_data1);
        writeByte(fd, static_cast<unsigned char>(indirectData[0]));
        writeULEB128(fd, DW_FORM_ref1);
        writeByte(fd, static_cast<unsigned char>(indirectData[1]));
        writeULEB128(fd, DW_FORM_flag);
        writeByte(fd, static_cast<unsigned char>(indirectData[2]));
        writeULEB128(fd, DW_FORM_data2);
        writeShort(fd, static_cast<unsigned short>(indirectData[3]));
        writeULEB128(fd, DW_FORM_ref2);
        writeShort(fd, static_cast<unsigned short>(indirectData[4]));
        writeULEB128(fd, DW_FORM_data4);
        writeUnsigned(fd, static_cast<unsigned>(indirectData[5]));
        writeULEB128(fd, DW_FORM_ref4);
        writeUnsigned(fd, static_cast<unsigned>(indirectData[6]));
        writeULEB128(fd, DW_FORM_data8);
        writeUint64(fd, indirectData[7]);
        writeULEB128(fd, DW_FORM_ref8);
        writeUint64(fd, indirectData[8]);
        writeULEB128(fd, DW_FORM_udata);
        writeULEB128(fd, indirectData[9]);
        writeULEB128(fd, DW_FORM_sdata);
        writeLEB128(fd, indirectData[10]);

        const Offset indirectEnd = getOffset(fd);

        ASSERT(indirectEnd < leb128Offset);

        ASSERT(leb128Offset ==
                        FU::seek(fd, leb128Offset, FU::e_SEEK_FROM_BEGINNING));

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                writeLEB128(fd, o);
            }
        }
        writeLEB128(fd, maxOffset);
        writeLEB128(fd, minOffset);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset uleb128Offset = getOffset(fd);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                writeULEB128(fd, o);
            }
        }
        writeULEB128(fd, maxOffset);
        writeULEB128(fd, minOffset);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        Offset endPos = getOffset(fd);

        const Offset initialLength = endPos - initialLengthOffset - 4;

        ASSERT(initialLengthOffset ==
                 FU::seek(fd, initialLengthOffset, FU::e_SEEK_FROM_BEGINNING));

        writeUnsigned(fd, static_cast<unsigned>(initialLength));

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(k_GARBAGE_LENGTH, endPos - k_GARBAGE_LENGTH);
        int rc = mX.init(&helper, buffer, sec, endPos);
        ASSERT(0 == rc);

        if (verbose) cout << "Testing 'readOffset'\n";

        ASSERT(charOffset == X.offset());

        Offset x;
        for (int uu = 0; uu < 256; ++uu) {
            rc = mX.readOffset(&x, 1);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffset(&x, 1);
        ASSERT(0 == rc);
        ASSERT((int) maxChar == x);
        rc = mX.readOffset(&x, 1);
        ASSERT(0 == rc);
        ASSERT((int) minChar == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        for (int uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            rc = mX.readOffset(&x, 2);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffset(&x, 2);
        ASSERT(0 == rc);
        ASSERT((int) maxShort == x);
        rc = mX.readOffset(&x, 2);
        ASSERT(0 == rc);
        ASSERT((int) minShort == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(unsignedOffset == X.offset());

        for (Offset uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            rc = mX.readOffset(&x, 4);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffset(&x, 4);
        ASSERT(0 == rc);
        ASSERT((Offset) maxUnsigned == x);
        rc = mX.readOffset(&x, 4);
        ASSERT(0 == rc);
        ASSERT((Offset) minUnsigned == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(offsetOffset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffset(&x, 8);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffset(&x, 8);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffset(&x, 8);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(leb128Offset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readLEB128(&x);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readLEB128(&x);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readLEB128(&x);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(uleb128Offset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readULEB128(&x);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readULEB128(&x);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readULEB128(&x);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        ASSERT(endPos == X.offset());
        ASSERT(X.atEndOfSection());

        if (verbose) cout << "Testing 'readOffsetFromForm'\n";

        rc = mX.skipTo(initialLengthOffset);
        ASSERT(0 == rc);

        rc = mX.readInitialLength(&x);
        ASSERT(0 == rc);
        ASSERT(initialLength == x);

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 256; ++uu) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_data1);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_data1);
        ASSERT(0 == rc);
        ASSERT((int) maxChar == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_data1);
        ASSERT(0 == rc);
        ASSERT((int) minChar == x);

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 256; ++uu) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_flag);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_flag);
        ASSERT(0 == rc);
        ASSERT((int) maxChar == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_flag);
        ASSERT(0 == rc);
        ASSERT((int) minChar == x);

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 256; ++uu) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_ref1);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref1);
        ASSERT(0 == rc);
        ASSERT((int) maxChar == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref1);
        ASSERT(0 == rc);
        ASSERT((int) minChar == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        for (int uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_data2);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_data2);
        ASSERT(0 == rc);
        ASSERT((int) maxShort == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_data2);
        ASSERT(0 == rc);
        ASSERT((int) minShort == x);

        rc = mX.skipTo(shortOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_ref2);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref2);
        ASSERT(0 == rc);
        ASSERT((int) maxShort == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref2);
        ASSERT(0 == rc);
        ASSERT((int) minShort == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(unsignedOffset == X.offset());

        for (Offset uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_data4);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_data4);
        ASSERT(0 == rc);
        ASSERT((Offset) maxUnsigned == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_data4);
        ASSERT(0 == rc);
        ASSERT((Offset) minUnsigned == x);

        rc = mX.skipTo(unsignedOffset);
        ASSERT(0 == rc);

        for (Offset uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_ref4);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref4);
        ASSERT(0 == rc);
        ASSERT((Offset) maxUnsigned == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref4);
        ASSERT(0 == rc);
        ASSERT((Offset) minUnsigned == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(offsetOffset == X.offset());

        rc = mX.skipTo(unsignedOffset);
        ASSERT(0 == rc);

        for (int ii = 0; ii < 258; ++ii) {
            rc = mX.skipForm(Obj::e_DW_FORM_sec_offset);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(offsetOffset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffsetFromForm(&x, DW_FORM_data8);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_data8);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_data8);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipTo(offsetOffset);
        ASSERT(0 == rc);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffsetFromForm(&x, DW_FORM_ref8);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref8);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_ref8);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(leb128Offset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffsetFromForm(&x, DW_FORM_sdata);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_sdata);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_sdata);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(uleb128Offset == X.offset());

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffsetFromForm(&x, DW_FORM_udata);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffsetFromForm(&x, DW_FORM_udata);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffsetFromForm(&x, DW_FORM_udata);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        ASSERT(endPos == X.offset());
        ASSERT(X.atEndOfSection());

        rc = mX.skipTo(unsignedOffset);
        ASSERT(0 == rc);

        for (Offset uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
            ASSERT(0 == rc);
            ASSERT(uu == x);
        }
        rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
        ASSERT(0 == rc);
        ASSERT((Offset) maxUnsigned == x);
        rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
        ASSERT(0 == rc);
        ASSERT((Offset) minUnsigned == x);

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        ASSERT(offsetOffset == X.offset());

        ASSERT(initialLengthOffset ==
                 FU::seek(fd, initialLengthOffset, FU::e_SEEK_FROM_BEGINNING));

        writeUnsigned(fd, 0xffffffff);
        writeUint64(fd, static_cast<unsigned>(initialLength - 8));

        mX.disable();

        rc = mX.init(&helper, buffer, sec, endPos);
        ASSERT(0 == rc);

        rc = mX.skipTo(initialLengthOffset);
        ASSERT(0 == rc);

        rc = mX.readInitialLength(&x);
        ASSERT(0 == rc);
        ASSERT(initialLength - 8 == x);

        rc = mX.skipTo(offsetOffset);
        ASSERT(0 == rc);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
                ASSERT(0 == rc);
                ASSERT(o == x);
            }
        }
        rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
        ASSERT(0 == rc);
        ASSERT((Offset) maxOffset == x);
        rc = mX.readOffsetFromForm(&x, Obj::e_DW_FORM_sec_offset);
        ASSERT(0 == rc);
        ASSERT((Offset) minOffset == x);

        rc = mX.skipTo(indirectOffset);
        ASSERT(0 == rc);

        for (int ii = 0; ii < 11; ++ii) {
            rc = mX.readOffsetFromForm(&x, DW_FORM_indirect);
            ASSERT(0 == rc);
            ASSERTV(ii, indirectData[ii], x, indirectData[ii] == x);
        }

        ASSERT(indirectEnd == X.offset());

        if (verbose) cout << "Testing various 'skipForm'\n";

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 258; ++uu) {
            rc = mX.skipForm(DW_FORM_data1);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 258; ++uu) {
            rc = mX.skipForm(DW_FORM_ref1);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        rc = mX.skipTo(charOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < 258; ++uu) {
            rc = mX.skipForm(DW_FORM_flag);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        rc = mX.skipForm(Obj::e_DW_FORM_flag_present);
        ASSERT(0 == rc);
        ASSERT(shortOffset == X.offset());

        for (int uu = 0; uu < (1 << 16) + (2 << 8); uu += (1 << 8)) {
            rc = mX.skipForm(DW_FORM_data2);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(unsignedOffset == X.offset());

        rc = mX.skipTo(shortOffset);
        ASSERT(0 == rc);

        for (int uu = 0; uu < (1 << 16) + (2 << 8); uu += (1 << 8)) {
            rc = mX.skipForm(DW_FORM_ref2);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(unsignedOffset == X.offset());

        for (Offset uu = 0; uu < (1LL << 32) + (2 << 24); uu += (1 << 24)) {
            rc = mX.skipForm(DW_FORM_data4);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(offsetOffset == X.offset());

        rc = mX.skipTo(unsignedOffset);
        ASSERT(0 == rc);

        for (Offset uu = 0; uu < (1LL << 32) + (2 << 24); uu += (1 << 24)) {
            rc = mX.skipForm(DW_FORM_data4);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(offsetOffset == X.offset());

        for (int ii = 0; ii < 258; ++ii) {
            rc = mX.skipForm(DW_FORM_data8);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(leb128Offset == X.offset());

        rc = mX.skipTo(offsetOffset);
        ASSERT(0 == rc);

        for (int ii = 0; ii < 258; ++ii) {
            rc = mX.skipForm(DW_FORM_ref8);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(leb128Offset == X.offset());

        for (int ii = 0; ii < 258; ++ii) {
            rc = mX.skipForm(DW_FORM_sdata);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);
        ASSERT(uleb128Offset == X.offset());

        for (int ii = 0; ii < 258; ++ii) {
            rc = mX.skipForm(DW_FORM_udata);
            ASSERT(0 == rc);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        ASSERT(endPos == X.offset());
        ASSERT(X.atEndOfSection());

        rc = mX.skipTo(indirectOffset);
        ASSERT(0 == rc);

        for (int ii = 1; ii <= 11; ++ii) {
            rc = mX.skipForm(DW_FORM_indirect);
            ASSERT(0 == rc);
        }

        ASSERT(indirectEnd == X.offset());

        rc = FU::close(fd);
        ASSERT(0 == rc);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST READULEB128
        //
        // Concerns:
        //   Test that 'readULEB128' can accurate read variable-length unsigned
        //   integral types.
        //
        // Plan:
        //   Construct a table of signed values, along with the length in
        //   bytes to store each value as a signed LEB128.  Write the values
        //   to a file, then read them back and observe the values and the
        //   number of bytes are correct.
        //
        // Testing:
        //   readULEB128(TYPE *dst);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST READULEB128\n"
                             "================\n";

        struct {
            Uint64 d_value;
            int    d_length;
        } DATA[] = {
            { 0, 1 }, { 1, 1 }, { 10, 1 }, { 0x7f, 1 },
            { 0x80, 2 }, { 0xfff, 2 }, { 0x3fff, 2 },
            { 0x7fff, 3 }, { 0x1fffff, 3 }, { 0x200000, 4 },
            { 0xfffffff, 4 }, { 0x8000000, 4 }, { 0x3ffffffff, 5 },
            { 0x400000000ULL, 5 }, { 0x7ffffffffULL, 5 },
            { 0x800000000ULL, 6 }, { 0xfffffffffULL, 6 },
            { 0x7ffffffffffULL, 7 }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        const char *fn = "readULEB128.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        cout << std::hex;

        Offset offset = 0;
        for (int ii = 0; ii < k_NUM_DATA; ++ii) {
            const Uint64 VALUE  = DATA[ii].d_value;
            const int    LENGTH = DATA[ii].d_length;

            offset = getOffset(fd);

            if (veryVerbose) cout << "VALUE: " << VALUE << ", LENGTH: "
                                                             << LENGTH << endl;

            writeULEB128(fd, VALUE);

            const Offset actualLength = getOffset(fd) - offset;

            ASSERTV(VALUE, LENGTH, actualLength, LENGTH == actualLength);
        }

        offset = getOffset(fd);

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);
        Section sec;
        sec.reset(0, offset);
        char buffer[Obj::k_SCRATCH_BUF_LEN];

        Obj mX;    const Obj& X = mX;
        rc = mX.init(&helper, buffer, sec, offset);

        for (int ii = 0; ii < k_NUM_DATA; ++ii) {
            const Uint64 VALUE  = DATA[ii].d_value;
            const int    LENGTH = DATA[ii].d_length;

            offset = X.offset();

            const Offset expectedOffset = offset + LENGTH;

            if (LENGTH <= 1) {
                unsigned char uc;
                rc = mX.readULEB128(&uc);
                ASSERT(0 == rc);
                ASSERTV(VALUE, uc, VALUE == uc);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            if (LENGTH <= 2) {
                unsigned short us;
                rc = mX.readULEB128(&us);
                ASSERT(0 == rc);
                ASSERTV(VALUE, us, VALUE == us);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            if (LENGTH <= 4) {
                unsigned int v;
                rc = mX.readULEB128(&v);
                ASSERT(0 == rc);
                ASSERTV(VALUE, v, VALUE == v);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            {
                Offset v;
                rc = mX.readULEB128(&v);
                ASSERT(0 == rc);
                ASSERTV(VALUE, v, static_cast<Offset>(VALUE) == v);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            rc = mX.skipULEB128();
            ASSERT(0 == rc);
            ASSERT(expectedOffset == X.offset());

            rc = mX.skipTo(expectedOffset);
            ASSERT(0 == rc);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST READLEB128
        //
        // Concerns:
        //   Test that 'readLEB128' can accurate read variable-length signed
        //   integral types.
        //
        // Plan:
        //   Construct a table of signed values, along with the length in
        //   bytes to store each value as a signed LEB128.  Write the values
        //   to a file, then read them back and observe the values and the
        //   number of bytes are correct.
        //
        // Testing:
        //   readLEB128(TYPE *dst);
        //   skipULEB128();
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST READLEB128\n"
                             "===============\n";

        struct {
            Int64 d_value;
            int   d_length;
        } DATA[] = {
            { 0, 1 }, { 1, 1 }, { -1, 1 }, { 10, 1 }, { -10, 1 }, { 0x3f, 1 },
            { -0x3f, 1 }, { 0x40, 2 }, { -0x40, 1 }, { 0xfff, 2 },
            { -0xfff, 2 }, { 0x1fff, 2 }, { -0x1fff, 2 }, { -0x2000, 2 },
            { 0x7fff, 3}, { -0x7fff, 3}, { 0xfffff, 3 }, { -0xfffff, 3 },
            { -0x100000, 3 }, { 0x200000, 4 }, { -0x200001, 4 },
            { 0x7ffffff, 4 }, { -0x7ffffff, 4 }, { -0x8000000, 4 },
            { 0x8000000, 5 }, { 0x3ffffffff, 5 }, { -0x3ffffffff, 5 },
            { -0x400000000LL, 5 }, { 0x400000000LL, 6 }, { -0x400000001LL, 6 }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        const char *fn = "readULEB128.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        cout << std::hex;

        Offset offset = 0;
        for (int ii = 0; ii < k_NUM_DATA; ++ii) {
            const Int64 VALUE  = DATA[ii].d_value;
            const int   LENGTH = DATA[ii].d_length;

            offset = getOffset(fd);

            if (veryVerbose) cout << ii << "VALUE: " << VALUE << ", LENGTH: "
                                                             << LENGTH << endl;

            writeLEB128(fd, VALUE);

            const Offset actualLength = getOffset(fd) - offset;

            ASSERTV(VALUE, LENGTH, actualLength, LENGTH == actualLength);
        }

        offset = getOffset(fd);

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);
        Section sec;
        sec.reset(0, offset);
        char buffer[Obj::k_SCRATCH_BUF_LEN];

        Obj mX;    const Obj& X = mX;
        rc = mX.init(&helper, buffer, sec, offset);

        for (int ii = 0; ii < k_NUM_DATA; ++ii) {
            const Int64 VALUE  = DATA[ii].d_value;
            const int   LENGTH = DATA[ii].d_length;

            offset = X.offset();

            const Offset expectedOffset = offset + LENGTH;

            if (LENGTH <= 1) {
                signed char sc;
                rc = mX.readLEB128(&sc);
                ASSERT(0 == rc);
                ASSERTV(VALUE, (unsigned) sc, VALUE == sc);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            if (LENGTH <= 2) {
                short ss;
                rc = mX.readLEB128(&ss);
                ASSERT(0 == rc);
                ASSERTV(VALUE, ss, VALUE == ss);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            if (LENGTH <= 4) {
                int v;
                rc = mX.readLEB128(&v);
                ASSERT(0 == rc);
                ASSERTV(VALUE, v, VALUE == v);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            {
                Offset v;
                rc = mX.readLEB128(&v);
                ASSERT(0 == rc);
                ASSERTV(VALUE, v, VALUE == v);
                ASSERT(expectedOffset == X.offset());

                mX.skipTo(offset);
            }

            rc = mX.skipULEB128();
            ASSERT(0 == rc);
            ASSERT(expectedOffset == X.offset());

            rc = mX.skipTo(expectedOffset);
            ASSERT(0 == rc);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST STRING FUNCTIONS
        //
        // Concerns:
        //   All the methods that read strings work.
        //
        // Plan:
        //   Create a file with two sections, a 'str' section and an 'info'
        //   section.  The 'str' section will contain our target string.  The
        //   info section will contain an 'initialLength' and short and long
        //   offsets of the target string.  Call all permutations of the
        //   string reading methods.
        //
        // Testing:
        //   readString(bsl::string *);
        //   readStringAt(bsl::string *, Offset);
        //   readStringFromForm(bsl::string *, Obj *, unsigned);
        //   skipString();
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST STRING FUNCTIONS\n"
                             "=====================\n";

        const char *fn = "readstring.bin";
        const char str[] = { "Always debate to the best interpretation of"
                                                " your opponent's argument." };

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset strOff = getOffset(fd);

        {
            int rc = FU::write(fd, str, sz(str));
            ASSERT(sz(str) == rc);
        }

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset endStrSec = getOffset(fd);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset initLengthOff = getOffset(fd);

        writeUnsigned(fd, 0);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset shortOffOff = getOffset(fd);

        writeUnsigned(fd, static_cast<unsigned>(strOff));

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset longOffOff = getOffset(fd);

        writeInt64(fd, strOff);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset endFile = getOffset(fd);

        (void) FU::seek(fd, initLengthOff, FU::e_SEEK_FROM_BEGINNING);

        writeUnsigned(fd, static_cast<unsigned>(endFile - initLengthOff - 4));

        Section strSec, infoSec;
        strSec. reset(0, endStrSec);
        infoSec.reset(   endStrSec, endFile - endStrSec);

        FH helper(fn);

        Obj strReader;
        char strBuffer[Obj::k_SCRATCH_BUF_LEN];
        int rc = strReader.init(&helper, strBuffer, strSec, endFile);
        ASSERT(0 == rc);

        Obj infoReader;
        char infoBuffer[Obj::k_SCRATCH_BUF_LEN];
        rc = infoReader.init(&helper, infoBuffer, infoSec, endFile);
        ASSERT(0 == rc);

        if (verbose) cout << "Test 'readStringAt'\n";

        bsl::string result = "woof";
        rc = strReader.readStringAt(&result, strOff);
        ASSERT(0 == rc);
        ASSERT(str == result);

        if (verbose) cout << "Test 'readString'\n";

        result = "woof";

        rc = strReader.skipTo(0);
        ASSERT(0 == rc);
        rc = strReader.skipTo(strOff);
        ASSERT(0 == rc);

        rc = strReader.readString(&result);
        ASSERT(0 == rc);
        ASSERT(str == result);

        if (verbose) cout << "Test 'readStringFromForm' DW_FORM_string\n";

        result = "woof";

        rc = strReader.skipTo(0);
        ASSERT(0 == rc);
        rc = strReader.skipTo(strOff);
        ASSERT(0 == rc);

        rc = strReader.readStringFromForm(&result,
                                          &infoReader,
                                          DW_FORM_string);
        ASSERT(0 == rc);
        ASSERT(str == result);

        if (verbose) cout << "Test 'readStringFromForm' short DW_AT_strp\n";

        result = "woof";

        rc = infoReader.skipTo(initLengthOff);
        ASSERT(0 == rc);

        Offset initLen;

        rc = infoReader.readInitialLength(&initLen);
        ASSERT(0 == rc);
        ASSERT(4 == infoReader.offsetSize());

        rc = infoReader.skipTo(shortOffOff);
        ASSERT(0 == rc);

        rc = infoReader.readStringFromForm(&result,
                                           &strReader,
                                           DW_FORM_strp);
        ASSERT(0 == rc);
        ASSERT(str == result);

        if (verbose) cout << "Test 'readStringFromForm' long DW_AT_strp\n";

        (void) FU::seek(fd, initLengthOff, FU::e_SEEK_FROM_BEGINNING);

        writeUnsigned(fd, 0xffffffff);
        writeInt64(fd, endFile - initLengthOff - 12);

        rc = FU::close(fd);
        ASSERT(0 == rc);

        result = "woof";

        rc = infoReader.init(&helper, infoBuffer, infoSec, endFile);
        ASSERT(0 == rc);

        rc = infoReader.skipTo(initLengthOff);
        ASSERT(0 == rc);

        rc = infoReader.readInitialLength(&initLen);
        ASSERT(0 == rc);
        ASSERT(8 == infoReader.offsetSize());

        rc = infoReader.skipTo(longOffOff);
        ASSERT(0 == rc);

        rc = infoReader.readStringFromForm(&result,
                                           &strReader,
                                           DW_FORM_strp);
        ASSERT(0 == rc);
        ASSERT(str == result);

        if (verbose) cout << "Test 'skipString'\n";

        rc = strReader.skipTo(strOff);
        ASSERT(0 == rc);

        rc = strReader.skipString();
        ASSERT(0 == rc);

        ASSERT(strReader.offset() == strOff + sz(str));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST 'readInitialLength' and 'readSectionOffset'
        //
        // Concerns:
        //   That 'readInitialLength' correctly reads the intial length and
        //   sets the offset size.
        //
        // Plan:
        //   Set initial lengths of both types in a file, read them, and
        //   observe 'offset' and reading offsets.
        //
        // Testing:
        //   readInitialLength(Offset *dst);
        //   readSectionOffset(Offset *dst);
        //   offset();
        //   offsetSize();
        // --------------------------------------------------------------------

        if (verbose) cout <<
                          "TEST 'readInitialLength' and 'readSectionOffset'\n"
                          "================================================\n";

        const char * const fn = "readAddress.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        writeGarbage(fd,  0x111);

        writeUnsigned(fd, 0);

        writeGarbage(fd,  0x111);
        const Offset unsignedStart = getOffset(fd);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                writeUnsigned(fd, uu);
            }
        }

        writeGarbage(fd,  0x111);

        const Offset offsetStart = getOffset(fd);

        for (Offset uu = 0; uu < INT_MAX; uu += (1 << 24)) {
            writeInt64(fd, uu);
        }

        const Offset endData = getOffset(fd);

        writeGarbage(fd,  0x111);

        (void) FU::seek(fd, 0x111, FU::e_SEEK_FROM_BEGINNING);

        Offset length =
                     static_cast<unsigned>(endData - 0x111 - sizeof(unsigned));
        ASSERT(length < 0xffffffff);

        writeUnsigned(fd, static_cast<unsigned>(length));

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(0x111, INT_MAX - 0x111);
        int rc = mX.init(&helper, buffer, sec, INT_MAX);
        ASSERT(0 == rc);

        Offset iLen;

        if (verbose) cout << "Test 32 bit initial length\n";

        rc = mX.readInitialLength(&iLen);
        ASSERT(0 == rc);
        ASSERT(length == iLen);
        ASSERT(X.offset() == 0x111 + static_cast<int>(sizeof(unsigned)));
        ASSERT(static_cast<int>(sizeof(unsigned)) == X.offsetSize());

        rc = mX.skipTo(unsignedStart);
        ASSERT(0 == rc);

        Offset x;
        for (Offset ii = 0; ii < INT_MAX; ii += (1 << 24)) {
            rc = mX.readSectionOffset(&x);
            ASSERTV(rc, ii, x, 0 == rc && ii == x);
        }

        if (verbose) cout << "Test 64 bit initial length\n";

        (void) FU::seek(fd, 0x111, FU::e_SEEK_FROM_BEGINNING);

        writeUnsigned(fd, 0xffffffff);
        writeUint64(fd, length);

        rc = FU::close(fd);
        ASSERT(0 == rc);

        rc = mX.init(&helper, buffer, sec, INT_MAX);
        ASSERT(0 == rc);

        rc = mX.skipTo(0x111);
        ASSERT(0 == rc);

        iLen = 0;
        rc = mX.readInitialLength(&iLen);
        ASSERT(0 == rc);
        ASSERT(length == iLen);
        ASSERT(X.offset() == 0x111 + 12);

        rc = mX.skipTo(offsetStart);
        ASSERT(0 == rc);

        for (Offset ii = 0; ii < INT_MAX; ii += (1 << 24)) {
            rc = mX.readSectionOffset(&x);
            ASSERTV(rc, ii, x, 0 == rc && ii == x);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST 'readAddressSize', 'readAddress', 'addressSize'
        //
        // Concerns:
        //   That the class successfully reads addresses through the 3 funcions
        //   for that purpose, and that it sets 'addressSize' appropriately.
        //
        // Plan:
        //   Write addresses to a file in different formats, then use the
        //   class to read them back
        //
        // Testing:
        //   readAddress(UintPtr *dst, unsigned form);
        //   readAddressSize();
        //   readAddress(UintPtr *dst);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                      "TEST 'readAddressSize', 'readAddress', 'addressSize'\n"
                      "====================================================\n";

        if (verbose) cout << "First, test form-based address read\n";

        const char * const fn = "readAddress.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < 256; ++uu) {
            writeByte(fd, static_cast<unsigned char>(uu));
        }

        writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < (1 << 16); uu += 256) {
            writeShort(fd, static_cast<unsigned short>(uu));
        }

        writeGarbage(fd, k_GARBAGE_LENGTH);
        const Offset unsignedStart = getOffset(fd);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                writeUnsigned(fd, uu);
            }
        }

        Offset uint64Start = -1;
        if (8 == sizeof(void *)) {
            writeGarbage(fd, k_GARBAGE_LENGTH);

            uint64Start = getOffset(fd);

            {
                bool firstTime = true;
                for (Uint64 uu = 0; firstTime || 0 != uu; uu += (1ULL << 56)) {
                    firstTime = false;
                    writeUint64(fd, uu);
                }
            }
        }

        Offset endPos = getOffset(fd);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(k_GARBAGE_LENGTH, endPos - k_GARBAGE_LENGTH);
        int rc = mX.init(&helper, buffer, sec, endPos + k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        UintPtr x;

        for (unsigned uu = 0; uu < 256; ++uu) {
            rc = mX.readAddress(&x, DW_FORM_data1);
            ASSERT(0 == rc && uu == x);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        for (unsigned uu = 0; uu < (1 << 16); uu += 256) {
            rc = mX.readAddress(&x, DW_FORM_data2);
            ASSERT(0 == rc && uu == x);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        ASSERT(X.offset() == unsignedStart);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                rc = mX.readAddress(&x, DW_FORM_data4);
                ASSERT(0 == rc && uu == x);
            }
        }

        if (8 == sizeof(void *)) {
            rc = mX.skipBytes(k_GARBAGE_LENGTH);
            ASSERT(0 == rc);

            ASSERT(X.offset() == uint64Start);

            {
                bool firstTime = true;
                for (Uint64 uu = 0; firstTime || 0 != uu; uu += (1ULL << 56)) {
                    firstTime = false;
                    rc = mX.readAddress(&x, DW_FORM_data8);
                    ASSERT(0 == rc && uu == x);
                }
            }
        }

        ASSERT(X.offset() == endPos);
        ASSERT(X.atEndOfSection());

        if (verbose) cout <<
                   "Now test 'readAddressSize' and 'readAddress(UintPtr *)'\n";

        (void) FU::seek(fd, k_GARBAGE_LENGTH, FU::e_SEEK_FROM_BEGINNING);

        writeByte(fd, static_cast<unsigned char>(sizeof(unsigned)));

        rc = mX.init(&helper, buffer, sec, endPos + k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        rc = mX.readAddressSize();
        ASSERT(0 == rc);
        ASSERT(sizeof(unsigned) == X.addressSize());

        mX.skipTo(unsignedStart);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                rc = mX.readAddress(&x);
                ASSERT(0 == rc && uu == x);
            }
        }

        if (sizeof(unsigned) == sizeof(UintPtr)) {
            ASSERT(X.atEndOfSection());
            break;
        }

        (void) FU::seek(fd, k_GARBAGE_LENGTH, FU::e_SEEK_FROM_BEGINNING);

        writeByte(fd, static_cast<unsigned char>(sizeof(Uint64)));

        rc = mX.init(&helper, buffer, sec, endPos + k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        rc = mX.readAddressSize();
        ASSERT(0 == rc);
        ASSERT(sizeof(Uint64) == X.addressSize());

        rc = mX.skipTo(uint64Start);
        ASSERT(0 == rc);

        {
            bool firstTime = true;
            for (Uint64 uu = 0; firstTime || 0 != uu; uu += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readAddress(&x);
                ASSERT(0 == rc && uu == x);
            }
        }

        ASSERT(X.atEndOfSection());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST 'readValue'
        //
        // Concerns:
        //   That the template member function 'readValue' works with a variety
        //   of destination types.
        //
        // Plan:
        //   Write values to a file, then read them out.
        //
        // Testing:
        //   readValue(TYPE *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST 'readValue'\n"
                             "================\n";

        const char fn[] = { "readValue_test.bin" };

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < 256; ++uu) {
            unsigned char u = static_cast<unsigned char>(uu);

            writeByte(fd, u);
        }
        unsigned char maxChar, minChar;
        setToMax(&maxChar);
        setToMin(&minChar);

        writeByte(fd, maxChar);
        writeByte(fd, minChar);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            unsigned short u = static_cast<unsigned short>(uu);

            writeShort(fd, u);
        }
        unsigned short maxShort, minShort;
        setToMax(&maxShort);
        setToMin(&minShort);

        writeShort(fd, maxShort);
        writeShort(fd, minShort);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        for (Uint64 uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            unsigned u = static_cast<unsigned>(uu);

            writeUnsigned(fd, u);
        }
        unsigned maxUnsigned, minUnsigned;
        setToMax(&maxUnsigned);
        setToMin(&minUnsigned);

        writeUnsigned(fd, maxUnsigned);
        writeUnsigned(fd, minUnsigned);

        writeGarbage(fd, k_GARBAGE_LENGTH);

        Offset maxOffset, minOffset;
        setToMax(&maxOffset);
        setToMin(&minOffset);

        ASSERT(Obj::s_maxOffset == maxOffset);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                writeInt64(fd, o);
            }
        }
        writeInt64(fd, maxOffset);
        writeInt64(fd, minOffset);

        Offset endPos = getOffset(fd);

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(k_GARBAGE_LENGTH, endPos - k_GARBAGE_LENGTH);
        rc = mX.init(&helper, buffer, sec, endPos);
        ASSERT(0 == rc);

        {
            unsigned char x;
            for (unsigned uu = 0; uu < 256; ++uu) {
                rc = mX.readValue(&x);
                ASSERT(0 == rc);
                ASSERT(uu == x);
            }
            rc = mX.readValue(&x);
            ASSERT(0 == rc && maxChar == x);
            rc = mX.readValue(&x);
            ASSERT(0 == rc && minChar == x);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        {
            unsigned short x;
            for (unsigned uu = 0; uu < (1 << 16); uu += (1 << 8)) {
                rc = mX.readValue(&x);
                ASSERT(0 == rc);
                ASSERT(uu == x);
            }
            rc = mX.readValue(&x);
            ASSERT(0 == rc && maxShort == x);
            rc = mX.readValue(&x);
            ASSERT(0 == rc && minShort == x);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        {
            unsigned x;
            for (Uint64 uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
                rc = mX.readValue(&x);
                ASSERT(0 == rc && uu == x);
            }
            rc = mX.readValue(&x);
            ASSERT(0 == rc && maxUnsigned == x);
            rc = mX.readValue(&x);
            ASSERT(0 == rc && minUnsigned == x);
        }

        rc = mX.skipBytes(k_GARBAGE_LENGTH);
        ASSERT(0 == rc);

        {
            Offset x;
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                rc = mX.readValue(&x);
                ASSERT(0 == rc && o == x);
            }
            rc = mX.readValue(&x);
            ASSERT(0 == rc && maxOffset == x);
            rc = mX.readValue(&x);
            ASSERT(0 == rc && minOffset == x);
        }

        ASSERT(X.atEndOfSection());
        ASSERT(X.offset() == endPos);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST init, skipBytes, skipTo, atEndOfSection, offset
        //
        // Concerns:
        //   That 'init', 'skipBytes', 'skipTo', 'atEndOfSection', and 'offset'
        //   all function correction.
        //
        // Plan:
        //: 1 Initialize 'init' with a variety of inputs and observe its return
        //:   code to ensure that it's check its args properly.
        //: 2 Advance to the end of the section using 'skipTo' and 'skipBytes',
        //:   observing the offset with 'offset()'.
        //
        // Testing:
        //   Obj();
        //   init(FH *, char *, const Section&, Offset);
        //   skipBytes(Offset);
        //   skipTo(Offset);
        //   atEndOfSection();
        //   offset();
        // --------------------------------------------------------------------

        if (verbose) cout <<
                      "TEST init, skipBytes, skipTo, atEndOfSection, offset\n"
                      "====================================================\n";

        struct {
            int    d_line;
            bool   d_succeed;
            Offset d_offset;
            Offset d_size;
            Offset d_fileSize;
        } DATA[] = {
            { L_, 1, 0, 100, 100 },
            { L_, 1, 100, 100, 200 },
            { L_, 1, 100, 100, 400 },

            // negative section offset

            { L_, 0, -1, 100, 100 },
            { L_, 0, -1, 100, 200 },
            { L_, 0, -1, 100, 400 },

            // negative size

            { L_, 0, 0, -1, 100 },
            { L_, 0, 100, -1, 200 },
            { L_, 0, 100, -1, 400 },

            // libraryFileSize too short

            { L_, 0, 0, 100, 99 },
            { L_, 0, 100, 100, 199 },
            { L_, 0, 100, 10, 60 },

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const char *fn = "testInit.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        writeGarbage(fd, k_GARBAGE_LENGTH);

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);

        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Obj mX;    const Obj& X = mX;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE     = DATA[ti].d_line;
            const bool   SUCCEED  = DATA[ti].d_succeed;
            const Offset OFFSET   = DATA[ti].d_offset;
            const Offset SIZE     = DATA[ti].d_size;
            const Offset FILESIZE = DATA[ti].d_fileSize;

            Section sec;
            sec.reset(OFFSET, SIZE);

            // Try twice without disabling.

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            // Try after previously disabling.

            mX.disable();

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            if (0 == rc) {
                ASSERT(X.offset() == sec.d_offset);

                rc = mX.skipTo(sec.d_offset + sec.d_size + 1);
                ASSERT(0 != rc);    // Should fail
                ASSERT(X.offset() == sec.d_offset);

                rc = mX.skipTo(sec.d_offset - 1);
                ASSERT(0 != rc);    // Should fail
                ASSERT(X.offset() == sec.d_offset);

                bool skipType = false;
                for (Offset ii = sec.d_offset, jj = 1;
                           ii < sec.d_offset + sec.d_size; jj *= 2, ii += jj) {
                    if (skipType) {
                        rc = mX.skipTo(ii);
                        ASSERT(0 == rc);
                    }
                    else {
                        rc = mX.skipBytes(ii - X.offset());
                        ASSERT(0 == rc);
                    }
                    skipType = !skipType;

                    ASSERT(X.offset() == ii);
                }

                Offset offset = X.offset();
                Offset toSkip = sec.d_offset + sec.d_size - offset;

                rc = mX.skipBytes(toSkip + 1);
                ASSERT(0 != rc);    // should fail
                ASSERT(X.offset() == offset);

                rc = mX.skipBytes(toSkip);
                ASSERT(0 == rc);
                ASSERT(X.atEndOfSection());
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == FU::setWorkingDirectory(origWorkingDirectory));
    LOOP_ASSERT(tmpWorkingDir, FU::exists(tmpWorkingDir));

    // Sometimes this delete won't work because of '.nfs*' gremlin files that
    // mysteriously get created in the directory.  Leave the directory behind
    // and move on.  Also remove twice, because sometimes the first 'remove'
    // 'sorta' fails -- it returns a negative status after successfully killing
    // the gremlin file.  Worst case, leave the file there to be cleaned up in
    // a sweep later.

    FU::remove(tmpWorkingDir, true);
    FU::remove(tmpWorkingDir, true);

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

#else

int main()
{
    return -1;
}

#endif

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
