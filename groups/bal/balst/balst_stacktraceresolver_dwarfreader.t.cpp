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

#include <bsl_algorithm.h>
#include <bsl_limits.h>
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
// The component under test describes a mechanism, the "dwarf reader", used for
// reading DWARF data from files.  DWARF stores information in certain formats,
// and the dwarf reader is able to interpret and translate that information.
//
// Most of these tests involve writing data to a file, either with
// 'bdls_FilesystemUtil::write', or with 'write*' methods defined in this file,
// and then reading them out again.  The test files are created within a test
// directory.  The test directory lives in the local directory the test driver
// is run in, and it's name includes the number of the test case and the
// process id of the task, eliminating the likelihood that the test directory
// name will match that of another test case and reducing the likelihood that
// it will match that of a previous test run.  At the end of the test, the test
// directory is recursively destroyed.  After the test directory is created,
// the current context is changed to within the test directory, and then the
// names of plain test files used within the tests can be simple since the test
// directory name guards against the chance of collision.
//
// The only test case that doesn't involve creating a file and reading from it
// again is TC 10, which translates forms to their string names.
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

namespace {
namespace u {

template <class TYPE>
int sz(const TYPE&)
    // Effectively 'sizeof', except it returns an 'int' rather than a
    // 'bsl::size_t'.
{
    return static_cast<int>(sizeof(TYPE));
}

template <class TYPE>
void setToMax(TYPE *value)
    // Set the specified '*value' to the maximum value it is capable of
    // representing.
{
    *value = bsl::numeric_limits<TYPE>::max();
}

template <class TYPE>
void setToMin(TYPE *value)
    // Set the specified '*value' to the minimum value it is capable of
    // representing.
{
    *value = bsl::numeric_limits<TYPE>::min();
}

static
void writeByte(FD fd, unsigned char u)
    // Write the specified byte 'u' to the specified file descriptor 'fd'.
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeShort(FD fd, unsigned short u)
    // Write the specified 'unsigned short' 'u' to the specified file
    // descriptor 'fd'.
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeUnsigned(FD fd, unsigned u)
    // Write the specified 'unsigned' 'u' to the specified file descriptor
    // 'fd'.
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeUint64(FD fd, Uint64 u)
    // Write the specified 'Uint64' 'u' to the specified file descriptor 'fd'.
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeInt64(FD fd, Int64 u)
    // Write the specified 'Int64' 'u' to the specified file descriptor 'fd'.
{
    int rc = FU::write(fd, &u, sz(u));
    ASSERT(sz(u) == rc);
}

static
void writeGarbage(FD fd, unsigned numBytes)
    // Write the specified 'numBytes' bytes of garbage to the specified file
    // descriptor 'fd'.
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
    // Write the specified 'value' according to the variable-length signed
    // integer 'LEB128' format to the specified file descriptor 'fd'.
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
    // Write the specified 'value' according to the variable-length unsigned
    // integer 'ULEB128' format to the specified file descriptor 'fd'.
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
    // Return the current offset in the file of the specified open file
    // descriptor 'fd'.
{
    return FU::seek(fd, 0, FU::e_SEEK_FROM_CURRENT);
}

}  // close namespace u
}  // close unnamed namespace

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

    // Remove any leftover temporary directories created by this test driver
    // over 24 hours ago.  Note that we try to clean up the temporary directory
    // at the end of this test driver, but about 10% of the time we can't, due
    // to stubborn nfs 'gremlin' files that appear in the directory.

    bsl::system("rm -rf `find . -mtime +0 -a"
                        " -name 'tmp.stacktraceresolver_dwarfreader.case_*'`");

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

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ENUM NAME METHODS
        //
        // Concerns:
        //   That the enum printing methods yield reasonable names.
        //: 1 That the enum printing methods yield strings beginning with the
        //:   appropriate prefix for the type of enum.
        //: 2 That no two valid enum values result in the same string value for
        //:   a given type of enum.
        //
        // Plan:
        //: o For each enum type:
        //:   1 Traverse the valid values of each enum type.  (Invalid values
        //:     are skipped, because they result in an exit if 'u_TRACES' is
        //:     enabled in the imp file).
        //:   2 Call the 'stringFor*' method appropriate for the enum type.
        //:   3 Check that the string yielded begins with the appropriate
        //:     prefix. (C-1)
        //:   4 Accumulate all the strings into a set, confirming that the size
        //:     of the set incremented with each string and therefore all the
        //:     strings are unique.  (C-2)
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

            const bsl::string& s = Obj::stringForAt(ii);
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

            if (ii < Obj::e_DW_TAG_type_unit &&
                                            Obj::e_DW_TAG_mutable_type != ii) {
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
        //: 1 That 'skipForm' works for those forms not supported by
        //:   'readOffsetFromForm'.
        //
        // Plan:
        //: 1 Write values to file, storing the offset after each value is
        //:   written to a vector.
        //: 2 Go back and skip over them, verifying that the cursor positions
        //:   match the offsets in the vector.  (C-1)
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

        u::writeUnsigned(fd, 0);             // initial length (will overwrite
                                             // later

        offsets.push_back(u::getOffset(fd));

        u::writeByte(fd, 4);                    // Address size

        offsets.push_back(u::getOffset(fd));
        offsets.push_back(u::getOffset(fd));    // e_DW_FORM_flag_present

        for (int ii = 0; ii <= 56; ii += 8) {    // DW_FORM_ref_udata
            u::writeULEB128(fd, 1ULL << ii);
            offsets.push_back(u::getOffset(fd));
        }

        const char str[] = { "woof woof woof\n" };    // DW_FORM_string
        ASSERT(u::sz(str) == FU::write(fd, str, u::sz(str)));

        offsets.push_back(u::getOffset(fd));

        u::writeUnsigned(fd, 100);                 // DW_FORM_strp

        offsets.push_back(u::getOffset(fd));

        u::writeUnsigned(fd, 0xf0000);             // DW_FORM_addr 4

        offsets.push_back(u::getOffset(fd));

        u::writeUnsigned(fd, 0xf000f);             // DW_FORM_ref_addr 4

        offsets.push_back(u::getOffset(fd));

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        u::writeUint64(fd, 0xfULL << 32);          // DW_FORM_addr 8

        offsets.push_back(u::getOffset(fd));

        u::writeUint64(fd, 0xffULL << 32);         // DW_FORM_ref_addr 8

        offsets.push_back(u::getOffset(fd));
#endif

        u::writeByte(fd, 20);                          // DW_FORM_block1
        u::writeGarbage(fd, 20);

        offsets.push_back(u::getOffset(fd));

        u::writeShort(fd, 1000);                       // DW_FORM_block2
        u::writeGarbage(fd, 1000);

        offsets.push_back(u::getOffset(fd));

        u::writeUnsigned(fd, 1001);                    // DW_FORM_block4
        u::writeGarbage(fd, 1001);

        offsets.push_back(u::getOffset(fd));

        u::writeULEB128(fd, 1002);                     // DW_FORM_block
        u::writeGarbage(fd, 1002);

        offsets.push_back(u::getOffset(fd));

        u::writeULEB128(fd, 1003);                     // e_DW_FORM_exprloc
        u::writeGarbage(fd, 1003);

        offsets.push_back(u::getOffset(fd));

        ASSERT(0 == FU::seek(fd, 0, FU::e_SEEK_FROM_BEGINNING));

        u::writeUnsigned(fd, static_cast<unsigned>(offsets.back() - 4));
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
        //: 1 That 'readOffset', which reads an offset of a specified number
        //:   of bytes, and assigns it, without sign-extension, to an 8-byte
        //:   'Offset', works..
        //: 2 That 'readOffsetFromForm' works, for all supported values of
        //:   integral 'form'.
        //: 3 That 'skipForm' works, for all supported values of integral
        //:   'form'.  Note that 'skipForm' also supports some string forms,
        //:   which shall be tested in a later test.
        //
        // Plan:
        //: 1 Write values to a file, appropriate to be read by different
        //:   forms.
        //: 2 Read them back using 'readOffset' and 'readOffsetFromForm', and
        //:   verify the values read were correct.  (C-1) (C-2)
        //: 3 Skip over the values with 'skipForm' and verify the cursor skips
        //:   to the right position.  (C-3)
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

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset charOffset = u::getOffset(fd);

        for (unsigned uu = 0; uu < 256; ++uu) {
            unsigned char u = static_cast<unsigned char>(uu);

            u::writeByte(fd, u);
        }
        unsigned char maxChar, minChar;
        u::setToMax(&maxChar);
        u::setToMin(&minChar);

        u::writeByte(fd, maxChar);
        u::writeByte(fd, minChar);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset shortOffset = u::getOffset(fd);

        for (unsigned uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            unsigned short u = static_cast<unsigned short>(uu);

            u::writeShort(fd, u);
        }
        unsigned short maxShort, minShort;
        u::setToMax(&maxShort);
        u::setToMin(&minShort);

        u::writeShort(fd, maxShort);
        u::writeShort(fd, minShort);

        const Offset initialLengthOffset = u::getOffset(fd);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset unsignedOffset = u::getOffset(fd);

        for (Uint64 uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            unsigned u = static_cast<unsigned>(uu);

            u::writeUnsigned(fd, u);
        }
        unsigned maxUnsigned, minUnsigned;
        u::setToMax(&maxUnsigned);
        u::setToMin(&minUnsigned);

        u::writeUnsigned(fd, maxUnsigned);
        u::writeUnsigned(fd, minUnsigned);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset offsetOffset = u::getOffset(fd);

        Offset maxOffset, minOffset;
        u::setToMax(&maxOffset);
        u::setToMin(&minOffset);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                u::writeInt64(fd, o);
            }
        }
        u::writeInt64(fd, maxOffset);
        u::writeInt64(fd, minOffset);

        const Offset indirectOffset = u::getOffset(fd);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset leb128Offset = u::getOffset(fd);

        ASSERT(indirectOffset ==
                      FU::seek(fd, indirectOffset, FU::e_SEEK_FROM_BEGINNING));

        const Int64 indirectData[] = { 0, 1, 2, 1000, 10001, 100 * 1000,
                200 * 1000, 100ULL << 32, 200ULL << 32, 300ULL << 32,
                -400LL * (1LL << 32) };
        BSLMF_ASSERT(11 == sizeof indirectData / sizeof *indirectData);

        u::writeULEB128(fd, DW_FORM_data1);
        u::writeByte(fd, static_cast<unsigned char>(indirectData[0]));
        u::writeULEB128(fd, DW_FORM_ref1);
        u::writeByte(fd, static_cast<unsigned char>(indirectData[1]));
        u::writeULEB128(fd, DW_FORM_flag);
        u::writeByte(fd, static_cast<unsigned char>(indirectData[2]));
        u::writeULEB128(fd, DW_FORM_data2);
        u::writeShort(fd, static_cast<unsigned short>(indirectData[3]));
        u::writeULEB128(fd, DW_FORM_ref2);
        u::writeShort(fd, static_cast<unsigned short>(indirectData[4]));
        u::writeULEB128(fd, DW_FORM_data4);
        u::writeUnsigned(fd, static_cast<unsigned>(indirectData[5]));
        u::writeULEB128(fd, DW_FORM_ref4);
        u::writeUnsigned(fd, static_cast<unsigned>(indirectData[6]));
        u::writeULEB128(fd, DW_FORM_data8);
        u::writeUint64(fd, indirectData[7]);
        u::writeULEB128(fd, DW_FORM_ref8);
        u::writeUint64(fd, indirectData[8]);
        u::writeULEB128(fd, DW_FORM_udata);
        u::writeULEB128(fd, indirectData[9]);
        u::writeULEB128(fd, DW_FORM_sdata);
        u::writeLEB128(fd, indirectData[10]);

        const Offset indirectEnd = u::getOffset(fd);

        ASSERT(indirectEnd < leb128Offset);

        ASSERT(leb128Offset ==
                        FU::seek(fd, leb128Offset, FU::e_SEEK_FROM_BEGINNING));

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                u::writeLEB128(fd, o);
            }
        }
        u::writeLEB128(fd, maxOffset);
        u::writeLEB128(fd, minOffset);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset uleb128Offset = u::getOffset(fd);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                u::writeULEB128(fd, o);
            }
        }
        u::writeULEB128(fd, maxOffset);
        u::writeULEB128(fd, minOffset);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        Offset endPos = u::getOffset(fd);

        const Offset initialLength = endPos - initialLengthOffset - 4;

        ASSERT(initialLengthOffset ==
                 FU::seek(fd, initialLengthOffset, FU::e_SEEK_FROM_BEGINNING));

        u::writeUnsigned(fd, static_cast<unsigned>(initialLength));

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

        u::writeUnsigned(fd, 0xffffffff);
        u::writeUint64(fd, static_cast<unsigned>(initialLength - 8));

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
        //: 1 Test that 'readULEB128' can accurately read variable-length
        //:   signed integral types, for signed integral types of size 1
        //:   through 8 bytes.
        //: 2 Test that 'skipULEB128' can skip over ULEB128 objects of all
        //:   sizes.
        //
        // Plan:
        //: 1 Write, in the test driver, 'writeULEB128' that will write an
        //:   'Int64' to a file in its minimal ULEB128 form.
        //: 2 Create a table of 'Int64' values couple with the anticipated
        //:   lengths of those values when written in ULEB128 form.
        //: 3 Open a file and iterate through the table, writing the values to
        //:   the file in sequence.
        //: 4 Create a 'Section' enveloping the whole file.
        //: 5 Create a dwarf reader 'mX' on that 'Section'.
        //: 6 Iterate through the original table.  For each iteration:
        //:   o Calculate 'expectedOffset', the expected offset after reading
        //:     the next ULEB128 in the file.
        //:   o For each size in bytes at or above the length of the ULEB128
        //:     written, read the ULEB128, verify the value read was correct,
        //:     and skip back to the beginning of the ULEB128. (C-1)
        //:   o Call 'skipULEB128' and verify the cursor is correctly
        //:     positioned. (C-2)
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
            { 0xfffffff, 4 }, { 0x8000000, 4 }, { 0x3ffffffffULL, 5 },
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

            offset = u::getOffset(fd);

            if (veryVerbose) cout << "VALUE: " << VALUE << ", LENGTH: "
                                                             << LENGTH << endl;

            u::writeULEB128(fd, VALUE);

            const Offset actualLength = u::getOffset(fd) - offset;

            ASSERTV(VALUE, LENGTH, actualLength, LENGTH == actualLength);
        }

        offset = u::getOffset(fd);

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
        //: 1 Test that 'readLEB128' can accurate read variable-length signed
        //:   integral types, for signed integral types of size 1 through 8
        //:   bytes.
        //: 2 That 'skipULEB128' will skip any LEB128 number.
        //
        // Plan:
        //: 1 Write, in the test driver, 'writeLEB128' that will write an
        //:   'Int64' to a file in its minimal LEB128 form.
        //: 2 Create a table of 'Int64' values couple with the anticipated
        //:   lengths of those values when written in LEB128 form.
        //: 3 Open a file and iterate through the table, writing the values to
        //:   the file in sequence.
        //: 4 Create a 'Section' enveloping the whole file.
        //: 5 Create a dwarf reader 'mX' on that 'Section'.
        //: 6 Iterate through the original table.  For each iteration:
        //:   o Calculate 'expectedOffset', the expected offset after reading
        //:     the next LEB128 in the file.
        //:   o For each size in bytes at or above the length of the LEB128
        //:     written, read the LEB128, verify the value read was correct,
        //:     and skip back to the beginning of the LEB128. (C-1)
        //:   o Call 'skipULEB128' and verify the cursor is correctly
        //:     positioned. (C-2)
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
            { 0x8000000, 5 }, { 0x3ffffffffLL, 5 }, { -0x3ffffffffLL, 5 },
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

            offset = u::getOffset(fd);

            if (veryVerbose) cout << ii << "VALUE: " << VALUE << ", LENGTH: "
                                                             << LENGTH << endl;

            u::writeLEB128(fd, VALUE);

            const Offset actualLength = u::getOffset(fd) - offset;

            ASSERTV(VALUE, LENGTH, actualLength, LENGTH == actualLength);
        }

        offset = u::getOffset(fd);

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
        //: 1 'readString' works.
        //: 2 'readStringAt' works.
        //: 3 'readStringFromForm' works.
        //:   A When 'form' is 'DW_FORM_string'.
        //:   B When 'form' is 'DW_FORM_strp'.
        //: 4 'skipString' works.
        //
        // Plan:
        //: 1 Create a file with two sections, a 'str' section and an 'info'
        //:   section.  The 'str' section will contain our target string.  The
        //:   info section will contain an 'initialLength' and short and long
        //:   offsets of the target string.
        //: 2 Initialize two dwarf readers, one to access the 'str' section and
        //:   one to access the 'info' section.
        //: 3 Call 'readStringAt' on the 'str' reader to read the string and
        //:   verify the result. (C-2)
        //: 4 Skip the 'str' reader to the offset of the beginning of hte
        //:   strng.
        //: 5 Call 'readString' on the 'str' reader to read the string and
        //:   verify the result. (C-1)
        //: 6 Skip the 'str' reader to the beginning of the string.
        //: 7 Call 'readStringFromForm' on the 'str' reader with a form of
        //:   'DW_FORM_string' which should have the same effect as calling
        //:   'readString', and verify the result. (C-3-A)
        //: 8 Skip the 'info' reader to the offset of the 4-byte initial length
        //:   of the 'info' section, and call 'readInitialLength', and confirm
        //:   that 'offsetSize' is 4 bytes.
        //: 9 Skip the 'info' reader to the offset in the 'info' section of the
        //:   4-byte offset of string in the 'str' section.
        //: 10 Call 'readStringFromForm' on the 'info' reader passing it the
        //:    'string' reader and 'DW_FORM_strp' and verify the result.
        //:    (C-3-B)
        //: 11 Seek back to the initial length of the 'info' section and
        //:    overwrite it with an 8-byte initial length.
        //: 12 Re-initalize the 'info' reader, skip to the initial length,
        //:    call 'readInitialLength', and verify 'offsetSize' is 8 bytes.
        //: 13 Skip the 'info' reader to the offset in the 'info' section of
        //:    the 8 byte offset of the string in the 'str' section.
        //: 14 Call 'readStringFromForm' on the 'info' reader passing it the
        //:    'string' reader and 'DW_FORM_strp' and verify the result.
        //:    (C-3-B)
        //: 15 Skip the 'str' reader ot the beginning of the string.
        //: 16 Call 'skipString' on the 'str' reader and verify the cursor is
        //:    now positioned after the string. (C-4)
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

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset strOff = u::getOffset(fd);

        {
            int rc = FU::write(fd, str, u::sz(str));
            ASSERT(u::sz(str) == rc);
        }

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset endStrSec = u::getOffset(fd);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset initLengthOff = u::getOffset(fd);

        u::writeUnsigned(fd, 0);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset shortOffOff = u::getOffset(fd);

        u::writeUnsigned(fd, static_cast<unsigned>(strOff));

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset longOffOff = u::getOffset(fd);

        u::writeInt64(fd, strOff);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        const Offset endFile = u::getOffset(fd);

        (void) FU::seek(fd, initLengthOff, FU::e_SEEK_FROM_BEGINNING);

        u::writeUnsigned(fd,
                           static_cast<unsigned>(endFile - initLengthOff - 4));

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

        u::writeUnsigned(fd, 0xffffffff);
        u::writeInt64(fd, endFile - initLengthOff - 12);

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

        ASSERT(strReader.offset() == strOff + u::sz(str));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST 'readInitialLength' and 'readSectionOffset'
        //
        // Concerns:
        //: 1 That 'readInitialLength' correctly reads the intial length.
        //: 2 That 'readInitialLength' correctly sets the offset size.
        //: 3 That 'offsetSize' correctly indicates the offset size.
        //: 4 That 'readSectionOffset' correctly reads section offsets, and
        //:   reads them of the appropriate size.
        //: 5 That 'readSectionOffset' fails if called prior to
        //:   'readInitialLength'.
        //
        // Plan:
        //: 1 Write an initial length (which will eventually be over written)
        //:   to a file, followed by offsets of 'sizeof(unsigned)' followed by
        //:   offsets of 'sizeof(Offset)'.
        //: 2 Attempt to read a section offset prior to reading the initial
        //:   length and verify that it fails without changing the offset.
        //:   (C-5)
        //: 3 Read the initial length, which will set the offset size to
        //:   'sizeof(unsigned)'.  Verify the length is as expected. (C-1)
        //: 4 Verify that 'offsetSize()' is as expected. (C-2) (C-3)
        //: 5 Read 256 offsets that will all be 4 bytes long, and confirm they
        //:   are correct. (C-4)
        //: 6 Seek back and rewrite the initial length to set the offset size
        //:   to 'sizeof(Offset)'.
        //: 7 Seek back and read the initial length.  Verify the length is as
        //:   expected. (C-1)
        //: 8 Verify the offset size is now 'sizeof(Offset)'. (C-2) (C-3)
        //: 9 Read 256 section offsets of size 'sizeof(Offset)', verify they
        //:   are as expected. (C-4)
        //
        // Testing:
        //   readInitialLength(Offset *dst);
        //   readSectionOffset(Offset *dst);
        //   offsetSize();
        // --------------------------------------------------------------------

        if (verbose) cout <<
                          "TEST 'readInitialLength' and 'readSectionOffset'\n"
                          "================================================\n";

        const char * const fn = "readAddress.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        u::writeGarbage(fd,  0x111);

        u::writeUnsigned(fd, 0);

        u::writeGarbage(fd,  0x111);
        const Offset unsignedStart = u::getOffset(fd);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                u::writeUnsigned(fd, uu);
            }
        }

        u::writeGarbage(fd,  0x111);

        const Offset offsetStart = u::getOffset(fd);

        for (Offset uu = 0; uu < INT_MAX; uu += (1LL << 56)) {
            u::writeInt64(fd, uu);
        }

        const Offset endData = u::getOffset(fd);

        u::writeGarbage(fd,  0x111);

        (void) FU::seek(fd, 0x111, FU::e_SEEK_FROM_BEGINNING);

        Offset length =
                     static_cast<unsigned>(endData - 0x111 - sizeof(unsigned));
        ASSERT(length < 0xffffffff);

        u::writeUnsigned(fd, static_cast<unsigned>(length));

        FH helper(fn);

        Obj mX;    const Obj& X = mX;
        char buffer[Obj::k_SCRATCH_BUF_LEN];
        Section sec;
        sec.reset(0x111, INT_MAX - 0x111);
        int rc = mX.init(&helper, buffer, sec, INT_MAX);
        ASSERT(0 == rc);

        Offset iLen, x;

        if (verbose) cout << "Attempt read section offset prior to init len\n";

        rc = mX.readSectionOffset(&x);
        ASSERT(0 != rc);                       // fails
        ASSERT(X.offset() == sec.d_offset);    // reader's offset unchanged

        if (verbose) cout << "Test 32 bit initial length\n";

        rc = mX.readInitialLength(&iLen);
        ASSERT(0 == rc);
        ASSERT(length == iLen);
        ASSERT(X.offset() == 0x111 + static_cast<int>(sizeof(unsigned)));
        ASSERT(static_cast<int>(sizeof(unsigned)) == X.offsetSize());

        rc = mX.skipTo(unsignedStart);
        ASSERT(0 == rc);

        for (Offset ii = 0; ii < INT_MAX; ii += (1 << 24)) {
            rc = mX.readSectionOffset(&x);
            ASSERTV(rc, ii, x, 0 == rc && ii == x);
        }

        if (verbose) cout << "Test 64 bit initial length\n";

        (void) FU::seek(fd, 0x111, FU::e_SEEK_FROM_BEGINNING);

        u::writeUnsigned(fd, 0xffffffff);
        u::writeUint64(fd, length);

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
        ASSERT(static_cast<int>(sizeof(Uint64)) == X.offsetSize());

        rc = mX.skipTo(offsetStart);
        ASSERT(0 == rc);

        for (Offset ii = 0; ii < INT_MAX; ii += (1LL << 56)) {
            rc = mX.readSectionOffset(&x);
            ASSERTV(rc, ii, x, 0 == rc && ii == x);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST 'readAddressSize', 'readAddress', 'addressSize'
        //
        // Concerns:
        //: 1 That 'readAddress', passed a form, reads an address with the # of
        //:   bytes indicated on the form.
        //: 2 That 'readAddressSize' reads a single byte address size, and
        //:   o That 'addressSize' then returns the size read.
        //:   o That 'readAddress', when not passed a form, reads an address of
        //:     the size read by the preceding 'readAddressSize' call.
        //
        // Plan:
        //: 1 Write a file with a 1-byte address field (which will be
        //:   overwritten later with a different value) followed by address
        //:   values of varying sizes, 256 values for each size.
        //: 2 Call 'readAddress' with various forms to read all the addresses
        //:   of different sizes and verify the values were correct. (C-1)
        //: 3 Skip back to the address size and read it with 'readAddressSize',
        //:   verify with 'addressSize' that it's 'sizeof(int)'. (C-2-1)
        //: 4 Skip forward to where addresses of that size are, and read all
        //:   the addresses of that size, verifying the values. (C-2-2)
        //: 5 Overwrite the address size with 'sizeof(UintPtr)'.
        //: 6 Skip back to the address size and read it with 'readAddressSize',
        //:   verify with 'addressSize' that it's 'sizeof(UintPtr)'. (C-2-1)
        //: 7 Skip forward to where addresses of that size are, and read all
        //:   the addresses of that size, verifying the values. (C-2-2)
        //
        // Testing:
        //   readAddress(UintPtr *dst, unsigned form);
        //   readAddressSize();
        //   readAddress(UintPtr *dst);
        //   addressSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout <<
                      "TEST 'readAddressSize', 'readAddress', 'addressSize'\n"
                      "====================================================\n";

        if (verbose) cout << "First, test form-based address read\n";

        const char * const fn = "readAddress.bin";

        FD fd = FU::open(fn, FU::e_CREATE, FU::e_READ_WRITE);
        ASSERT(FU::k_INVALID_FD != fd);

        enum { k_GARBAGE_LENGTH = 1111 };

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < 256; ++uu) {
            u::writeByte(fd, static_cast<unsigned char>(uu));
        }

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < (1 << 16); uu += 256) {
            u::writeShort(fd, static_cast<unsigned short>(uu));
        }

        u::writeGarbage(fd, k_GARBAGE_LENGTH);
        const Offset unsignedStart = u::getOffset(fd);

        {
            bool firstTime = true;
            for (unsigned uu = 0; firstTime || 0 != uu; uu += (1 << 24)) {
                firstTime = false;
                u::writeUnsigned(fd, uu);
            }
        }

        Offset uint64Start = -1;
        if (8 == sizeof(void *)) {
            u::writeGarbage(fd, k_GARBAGE_LENGTH);

            uint64Start = u::getOffset(fd);

            {
                bool firstTime = true;
                for (Uint64 uu = 0; firstTime || 0 != uu; uu += (1ULL << 56)) {
                    firstTime = false;
                    u::writeUint64(fd, uu);
                }
            }
        }

        Offset endPos = u::getOffset(fd);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

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

        u::writeByte(fd, static_cast<unsigned char>(sizeof(unsigned)));

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

        u::writeByte(fd, static_cast<unsigned char>(sizeof(Uint64)));

        rc = FU::close(fd);
        ASSERT(0 == rc);

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
        //: 1 That 'readValue' returns 0 when it succeeds.
        //: 2 That 'readValue' properly reads values from the file, for a
        //    variety of destination types.
        //: 3 That an attempt to read past the end of section will fail with
        //:   no change of state.
        //
        // Plan:
        //: 1 Write values of different types to a file.
        //: 2 Read them out and verify that the return value is 0 and that the
        //:   read values match with the written values.  (C-1) (C-2)
        //: 3 Attempt to read a byte past the end of section, observe that it
        //:   returns a failure return code with no change of state.  (C-3)
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

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < 256; ++uu) {
            unsigned char u = static_cast<unsigned char>(uu);

            u::writeByte(fd, u);
        }
        unsigned char maxChar, minChar;
        u::setToMax(&maxChar);
        u::setToMin(&minChar);

        u::writeByte(fd, maxChar);
        u::writeByte(fd, minChar);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        for (unsigned uu = 0; uu < (1 << 16); uu += (1 << 8)) {
            unsigned short u = static_cast<unsigned short>(uu);

            u::writeShort(fd, u);
        }
        unsigned short maxShort, minShort;
        u::setToMax(&maxShort);
        u::setToMin(&minShort);

        u::writeShort(fd, maxShort);
        u::writeShort(fd, minShort);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        for (Uint64 uu = 0; uu < (1LL << 32); uu += (1 << 24)) {
            unsigned u = static_cast<unsigned>(uu);

            u::writeUnsigned(fd, u);
        }
        unsigned maxUnsigned, minUnsigned;
        u::setToMax(&maxUnsigned);
        u::setToMin(&minUnsigned);

        u::writeUnsigned(fd, maxUnsigned);
        u::writeUnsigned(fd, minUnsigned);

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        Offset maxOffset, minOffset;
        u::setToMax(&maxOffset);
        u::setToMin(&minOffset);

        {
            bool firstTime = true;
            for (Offset o = 0; firstTime || 0 != o; o += (1ULL << 56)) {
                firstTime = false;
                u::writeInt64(fd, o);
            }
        }
        u::writeInt64(fd, maxOffset);
        u::writeInt64(fd, minOffset);

        Offset endPos = u::getOffset(fd);

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

        // Attempt to read past end of section, should fail

        {
            char x = 12;
            rc = mX.readValue(&x);
            ASSERT(0 != rc);    // fails
            ASSERT(12 == x);    // unchanged
            ASSERTV(X.offset(), endPos, X.offset() == endPos);    // unchanged
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST init, skipBytes, skipTo, atEndOfSection, offset
        //
        // Concerns:
        //: 1 That 'init' checks its args properly and correctly returns a
        //:   status to indicate whether it could be properly initialized.
        //: 2 That the navigation functions can navigate properly within a
        //:   file (note that nothing is ever read, all we do here is seeks).
        //
        // Plan:
        //: 1 Initialize 'init' with a variety of inputs and observe its return
        //:   code to ensure that it checks its args properly.  (C-1)
        //: 2 Advance to the end of the section using 'skipTo' and 'skipBytes',
        //:   observing the offset with 'offset()' and 'atEndOfSection'.  (C-2)
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

        const struct {
            int    d_line;
            bool   d_succeed;
            Offset d_offset;
            Offset d_size;
            Offset d_fileSize;
        } DATA[] = {
            { L_, 1,   0, 100, 100 },
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

        u::writeGarbage(fd, k_GARBAGE_LENGTH);

        int rc = FU::close(fd);
        ASSERT(0 == rc);

        FH helper(fn);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE     = DATA[ti].d_line;
            const bool   SUCCEED  = DATA[ti].d_succeed;
            const Offset OFFSET   = DATA[ti].d_offset;
            const Offset SIZE     = DATA[ti].d_size;
            const Offset FILESIZE = DATA[ti].d_fileSize;

            char buffer[Obj::k_SCRATCH_BUF_LEN];
            Obj mX;    const Obj& X = mX;

            // The following shouldn't work on an uninitialized object.

            ASSERT(0 != mX.skipTo(0));
            ASSERT(0 >  X.offset());
            ASSERT(0 != mX.skipBytes(10));

            // Try twice without disabling.

            Section sec;
            sec.reset(OFFSET, SIZE);

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            // Now disable.

            mX.disable();

            // The following shouldn't work on a disabled object.

            ASSERT(0 != mX.skipTo(0));
            ASSERT(0 >  X.offset());
            ASSERT(0 != mX.skipBytes(10));

            // Try init after previously disabling.

            rc = mX.init(&helper, buffer, sec, FILESIZE);
            ASSERTV(LINE, SUCCEED, rc, OFFSET, SIZE, FILESIZE,
                                                         SUCCEED == (0 == rc));

            if (0 == rc) {
                ASSERT(X.offset() == sec.d_offset);
                ASSERT(!X.atEndOfSection());

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
                    ASSERT(!X.atEndOfSection());
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
