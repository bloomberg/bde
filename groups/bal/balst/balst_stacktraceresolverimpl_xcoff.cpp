// balst_stacktraceresolverimpl_xcoff.cpp                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_xcoff.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolverimpl_xcoff_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF

#include <balst_stacktrace.h>
#include <balst_stacktraceresolver_filehelper.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bdls_filesystemutil.h>
#include <bdls_processutil.h>
#include <bdlb_string.h>

#include <bslma_default.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_vector.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

// Aix/Xcoff-specific includes:

#undef __XCOFF64__
#undef __XCOFF32__

#ifdef BSLS_PLATFORM_CPU_64_BIT
# define __XCOFF64__
#else
# define __XCOFF32__
#endif
#define __AR_BIG__

#include <ar.h>
#include <demangle.h>

#include <sys/ldr.h>
#include <xcoff.h>      // includes: filehdr.h, aouthdr.h, scnhdr.h, linenum.h,
                        // syms.h

// ============================================================================
//              Debugging trace macros: 'eprintf' and 'zprintf'
// ============================================================================

#undef  TRACES
#define TRACES 0    // debugging traces off

#if TRACES == 1
# include <stdio.h>

# define eprintf printf
# define zprintf printf

#else

# define eprintf (void)    // only called on errors
# define zprintf (void)    // called on debug output - output is very
                           // voluminous if this is turned on
#endif

// IMPLEMENTATION NOTES:
//
// The XCOFF (eXtended Common Object File Format) object file format is the
// object format for the AIX operating system and provides the formal
// definition of machine-image object and executable files.
//
// A long pdf file describing the xcoff format can be found at
//
// http://bit.ly/bJRQYu
//
// which expands to '
//
// http://publib.boulder.ibm.com/infocenter/aix/v6r1/index.jsp?topic=/
//                                     com.ibm.aix.files/doc/aixfiles/XCOFF.htm
//
// An XCOFF file contains the following parts (Note that both object and
// executable files are similar in structure):
//
// 1. A composite header consisting of:
//    a) A file header
//    b) An optional auxiliary header
//    c) Section headers, one for each of the file's raw-data sections
// 2. Raw-data sections, at most one per section header
// 3. Optional relocation information for individual raw-data sections
// 4. Optional line number information for individual raw-data sections
// 5. An optional symbol table
// 6. An optional string table, which is used for all symbol names in XCOFF64
//    and for symbol names longer than 8 bytes in XCOFF32.
//
// Not every XCOFF file contains every part. A minimal XCOFF file contains
// only the file header.  Loading an XCOFF executable file into memory creates
// the following logical segments:
//
// 1. A text segment (initialized from the .text section of the XCOFF file).
// 2. A data segment, consisting of initialized data (initialized from the
//    .data section of the XCOFF file) followed by uninitialized data
//    (initialized to 0). The length of uninitialized data is specified in the
//    .bss section header of the XCOFF file.
//
// Note that if a file is an archive, it can contain multiple archive members,
// each of which will have its own text segment and should be treated as a file
// in its own right.
//
// The following diagram shows the layout for a xcoff object file:
//
//  _________________________________________    INCLUDE FILE
//  |_______________HEADER_DATA_____________|
//  |                    |                  |
//  |    File Header     |    "filehdr.h"   |
//  |.......................................|
//  |                    |
//  |    Auxilliary Header Information      |    "aouthdr.h"
//  |_______________________________________|
//  |    ".text" section header             |    "scnhdr.h"
//  |.......................................|
//  |    ".data" section header             |        ''
//  |.......................................|
//  |    ".bss" section header              |        ''
//  |.......................................|
//  |    ".loader" section header           |        ''
//  |.......................................|
//  |    ".typchk" section header           |        ''
//  |.......................................|
//  |    ".debug" section header            |        ''
//  |.......................................|
//  |    ".except" section header           |        ''
//  |.......................................|
//  |    ".info" section header             |        ''
//  |_______________________________________|
//  |______________RAW_DATA_________________|
//  |    ".text" section data               |
//  |.......................................|
//  |    optional ".pad" section data       |
//  |        (file alignment)               |
//  |.......................................|
//  |    ".data" section data               |
//  |.......................................|
//  |    ".loader" section data             |    "loader.h"
//  |.......................................|
//  |    ".typchk" section data             |    "typchk.h"
//  |.......................................|
//  |    ".debug" section data              |    "dbug.h"
//  |.......................................|
//  |    ".except" section data             |    "exceptab.h"
//  |.......................................|
//  |    ".info" section data               |
//  |_______________________________________|
//  |____________RELOCATION_DATA____________|
//  |                                       |
//  |    ".text" section relocation data    |    "reloc.h"
//  |                                       |
//  |.......................................|
//  |                                       |
//  |    ".data" section relocation data    |        ''
//  |                                       |
//  |_______________________________________|
//  |__________LINE_NUMBER_DATA_____________|
//  |                                       |
//  |    ".text" section line numbers       |    "linenum.h"
//  |                                       |
//  |_______________________________________|
//  |________________SYMBOL_TABLE___________|
//  |                    |
//  |    ".text", ".data" and ".bss"        |    "syms.h"
//  |        section symbols                |    "storclass.h"
//  |                    |
//  |_______________________________________|
//  |________________STRING_TABLE___________|
//  |                                       |
//  |    long symbol names                  |
//  |_______________________________________|
//
//
//  The following structures, whose definitions were obtained from various
//  include files on the AIX machines and correspond to 32-bit object files,
//  are used in the implementation (the data is organized slightly differently
//  in 64-bit object files, this is handled with #ifdef's on the identifiers
//  '__XCOFF_32__' and '__XCOFF_64__').  Note that these definitions are
//  simplified, not showing those fields that this component does not access.
//  Also note that some of the identifiers that appear to be type or member
//  names are, in fact, #define's.
//
//                            // ==============
//                            // struct ld_info
//                            // ==============
//
//  struct ld_info {
//      // This 'struct' describes a loadable module in the context of either
//      // tracing a process (with the ptrace system call) or examining a core
//      // file.  Note that it does not exist in any file, but is created by
//      // the loader in memory.
//
//      unsigned int  ldinfo_next;       // offset (in bytes) from current
//                                       // entry to the next entry or 0 if
//                                       // this is the last entry
//
//      void         *ldinfo_textorg;    // effective address of the loaded
//                                       // program image, including xcoff
//                                       // headers
//
//      int           ldinfo_textsize;   // length of the loaded program image
//
//      char         *ldinfo_filename[2];// null-terminated path name
//                                       // followed by null-terminated member
//                                       // name.  The member name is "" if
//                                       // filename is not an archive.  Note
//                                       // that this is just the beginning of
//                                       // these two names, which hang over
//                                       // the end of the struct.
//  };
//
//                            // ==============
//                            // struct filehdr
//                            // ==============
//
//  typedef struct filehdr {
//      // This 'struct' defines the file header of a xcoff object file.
//
//      unsigned short f_magic;    // magic number which specifies the target
//                                 // machine on which the executable is run
//
//      _LONG32        f_symptr;   // offset (in bytes) to the symbol table
//                                 // from the start of the file
//
//      _LONG32        f_nsyms;    // number of symbol table entries.
//                                 // Negative values are reserved for future
//                                 // use and should be treated as 0.
//
//      unsigned short f_opthdr;   // size (in bytes) of the auxiliary header
//  } FILHDR;
//
// The auxiliary header contains system-dependent and implementation-dependent
// information, which is used for loading and executing a module.  Information
// in the auxiliary header minimizes how much of the file must be processed by
// the system loader at execution time.
//
// The binder generates an auxiliary header for use by the system loader.
// Auxiliary headers are not required for an object file that is not to be
// loaded.  When auxiliary headers are generated by compilers and assemblers,
// the headers are ignored by the binder.
//
//                            // ==============
//                            // struct aouthdr
//                            // ==============
//
//  typedef struct aouthdr {
//      // This 'struct' provides the auxiliary header of a xcoff file.  In
//      // general, the auxiliary header can be any size, and the length of the
//      // auxiliary in any particular object file is specified in the f_opthdr
//      // field of the xcoff header.
//
//      _ULONG32     o_text_start; // virtual address of beginning of text
//
//      short        o_sntext;     // section number for text
//  } AOUTHDR;
//
//                            // ===============
//                            // struct xcoffhdr
//                            // ===============
//
//  struct xcoffhdr {
//      // This 'struct' defines the header for an xcoff executable.
//
//      struct filehdr filehdr;  // file header
//      struct aouthdr aouthdr;  // auxiliary header
//  };
//
//                            // =============
//                            // struct scnhdr
//                            // =============
//
//  typedef struct scnhdr {
//      // This 'struct' defines the header for a section.
//
//      _LONG32        s_scnptr;    // offset from beginning of the file to
//                                  // first byte of the section
//  } SCNHDR;
//
//              AIX INDEXED ARCHIVE FORMAT
//
//      ARCHIVE File Organization:
//        _____________________________________________
//       |__________FIXED HEADER "fl_hdr"______________|
//  +--- |                                             |
//  |    |__________ARCHIVE_FILE_MEMBER_1______________|
//  +--> |                                             |
//       |    Archive Member Header "ar_hdr"           |
//  +--- |.............................................| <--+
//  |    |        Member Contents                      |    |
//  |    |_____________________________________________|    |
//  |    |________ARCHIVE_FILE_MEMBER_2________________|    |
//  +--> |                                             | ---+
//       |    Archive Member Header "ar_hdr"           |
//  +--- |.............................................| <--+
//  |    |        Member Contents                      |    |
//  |    |_____________________________________________|    |
//  |    |    .                 .            .         |    |
//  .    |    .                 .            .         |    |
//  .    |    .                 .            .         |    |
//  .    |_____________________________________________|    |
//  |    |________ARCHIVE_FILE_MEMBER_n-1______________|    |
//  +--> |                                             | ---+
//       |    Archive Member Header "ar_hdr"           |
//  +--- |.............................................| <--+
//  |    |    Member Contents                          |    |
//  |    |       (Member Table, always present)        |    |
//  |    |_____________________________________________|    |
//  |    |_____________________________________________|    |
//  |    |________ARCHIVE_FILE_MEMBER_n________________|    |
//  |    |                                             |    |
//  +--> |    Archive Member Header "ar_hdr"           | ---+
//       |.............................................|
//       |    Member Contents                          |
//       |     32-bit Global Symbol Table (if present) |
//       |     64-bit Global Symbol Table (if present) |
//       |_____________________________________________|
//
//                            // =============
//                            // struct fl_hdr
//                            // =============
//
//  typedef struct fl_hdr
//      // This 'struct' defines the file header for an archive (.ar) file.
//  {
//      char fl_magic[SAIAMAG];    //  Archive file magic string
//      char fl_fstmoff[12];       //  Offset to first archive member
//      char fl_lstmoff[12];       //  Offset to last archive member
//  } FL_HDR;
//
//                            // =============
//                            // struct ar_hdr
//                            // =============
//
//  typedef struct ar_hdr
//      // This 'struct' defines the header for an individual object file in
//      // an archive.
//  {
//      char ar_size[12];    //  file member size - decimal
//      char ar_nxtmem[12];  //  pointer to next member -  decimal
//      char ar_namlen[4];   //  file member name length - decimal
//      union
//      {
//          char ar_name[2]; //  variable length member name
//          char ar_fmag[2]; //  AIAFMAG - string to end header
//      }_ar_name;           //  and variable length name
//  } AR_HDR;
//
//                            // =============
//                            // struct SYMENT
//                            // =============
//
//  typedef struct syment {
//      // This 'struct' defines a symbol entry in an xcoff object file.
//
//      union {
//          char               n_name[SYMNMLEN];  // symbol name (if it is 8
//                                                // characters or less)
//          struct {
//              _LONG32        n_zeroes;          // If 0 this indicates that
//                                                // the symbol name is in the
//                                                // string table or .debug
//                                                // section
//
//              _LONG32        n_offset;          // offset of the symbol in
//                                                // the string table or
//                                                // .debug section if
//                                                // 0 == n_zeroes
//          } _n_n;
//          char              *_n_nptr[2];        // allows for overlaying
//      } _n;
//      _ULONG32               n_value;           // value of symbol (this
//                                                // depends on the storage
//                                                // class)
//      short                  n_scnum;           // section number
//      char                   n_sclass;          // the storage class of
//                                                // symbol (the various
//                                                // options are specified in
//                                                // storclass.h)
//      char                   n_numaux;          // number of aux. entries
//  } SYMENT;
//
//                            // =============
//                            // struct AUXENT
//                            // =============
//
//  typedef union auxent {
//      // This 'struct' defines an auxiliary symbol entry in an xcoff
//      // executable.
//
//      struct {
//          union {
//              struct {
//                  unsigned short x_lnno;      // declaration line number
//              } x_lnsz;
//          } x_misc;
//          union {
//              struct {                        // if ISFCN, tag, or .bb
//                  _LONG32        x_lnnoptr;   // a file pointer to
//                                              // the line number. The value
//                                              // is the byte offset from the
//                                              // beginning of the XCOFF
//                                              // object file.
//                  _LONG32        x_endndx;    // entry ndx past block end
//              } x_fcn;
//          } x_fcnary;
//      } x_sym;
//
//      union {
//          char               x_fname[FILNMLEN]; // source file name or
//                                                // compiler related string
//                                                // (if it is 8 chars or
//                                                // less)
//          struct {
//              _LONG32        x_zeroes;          // If 0 this indicates that
//                                                // the symbol name is in the
//                                                // string table or .debug
//                                                // section
//
//              _LONG32        x_offset;          // offset of the symbol in
//                                                // the string table or
//                                                // .debug section if
//                                                // 0 == n_zeroes
//              char           x_pad[FILNMLEN-8];
//              unsigned char  x_ftype;
//          } _x;
//      } x_file;
//
//      // CSECT auxiliary entry
//      struct {
//          _LONG32        x_scnlen;         // csect length
//          unsigned char  x_smtyp;          // symbol align and type
//                                           // bits 0 - 4 specify the log 2
//                                           // of alignment
//                                           // bits 5 - 7 specify the symbol
//                                           // type
//                                           // XTY_SD - x_scnlen contains the
//                                           //          csect length
//                                           // XTY_LD - x_scnlen contains the
//                                           //          symbol table index of
//                                           //          the containing csect
//          unsigned char  x_smclas;         // storage mapping class
//                                           // XMC_PR - specifies the program
//                                           // code.  The csect contains
//                                           // executable instructions of the
//                                           // program.
//      } x_csect;
//  } AUXENT;
//
// Explanation of the various storage classes used (from storclass.h):
//
// C_BINCL         //  marks beginning of include file
// C_EINCL         //  marks ending of include file
// C_FILE          //  provides source file-name information, source-language
//                 //  ID and CPU-version ID information, and, optionally,
//                 //  compiler-version and time-stamp information
// C_EXT           //  external symbol
// C_HIDEXT        //  un-named external symbol
// C_FCN           //  specifies the beginning or end of a function
// C_WEAKEXT       //  weak external symbol
//
// The meaning of n_value for a particular symbol (in a SYMENT object) depends
// on the storage class.  The following is a list of the meaning of n_value
// for the various storage classes:
//
// Relocatable address
//   C_EXT, C_WEAKEXT, C_HIDEXT, C_FCN
//
// Offset in file
//   C_BINCL, C_EINCL
//
// Symbol table index
//   C_FILE
//
// The following functions are used in the implementation below:
//
// int loadquery(int flags, void *buffer, unsigned int bufferLength);
//     // This function has multiple uses, but the only use in this component
//     // is calling it with 'flags == L_GETINFO', which loads the passed
//     // buffer with a linked list of variable-length 'ld_info' objects, each
//     // of which describes either the executable or one of the shared
//     // libraries.  Returns '-1' on error with 'errno' set to describe the
//     // problem.
//
// The steps used for resolution are as follows:
// 1. Read an xcoff executable / shraed library.  Shared libraries will be
//    arcihves with multiple members.  Each executable file or arhive member
//    will have one code segment.  For each segment, we iterate through the
//    'address' fields of the stack trace frames, and for those addresses, if
//    any, that match, we resolve the symbol information for that address.
// 2. Read the file header for the module and from the contained information
//    read the text section.  The text section contains a header, symbol
//    table, and strings table for that module.  The symbol table contains
//    symbols represented as SYMENT structures.  Each SYMENT is followed by
//    additional AUXENT structures that contain more information about the
//    symbol.  The functional code that is part of each symbol is represented
//    as an AUXENT (csection) following that symbol's SYMENT.
// 3. We traverse through the various SYMENTs and corresponding AUXENTs trying
//    to find the symbols that match the frames we want to populate.  We do
//    this by checking if the address contained in a frame is within the
//    csection of a symbol.
// 4. For each frame pointer populate the remaining stack frame data from the
//    AUXENT's of the matching symbol.
//
// The data that we are looking for (members of balst::StackTraceFrame) and
// where we can find it:
// 1. object file name                 : Obtained by traversing the various
//                                       object files in the executable
// 2. line number of return address    : Traverse the line number information
//                                       starting from the beginning of the
//                                       function (or sometimes, with inline
//                                       functions in include files, from the
//                                       beginning of the include file) until
//                                       finding the line number matching the
//                                       stack trace frame's address
// 3. offset from symbol               : Obtained by subtracting the address
//                                       associated with the symbol from the
//                                       matching 'address' field from the
//                                       stack trace frame
// 4. source file name                 : Find SYMENT for the include symbol by
//                                       iterating through the AUXENTs and get
//                                       file name from that SYMENT
// 5. symbol name                      : Demangling the mangled symbol
// 6. mangled symbol name              : Obtained from the corresponding
//                                       SYMENT either from its n_name data
//                                       member or looking at the symbol's
//                                       offset in the string table

namespace BloombergLP {

// ============================================================================
//        Static Definitions and Definitions in the Unnamed Namespace
// ============================================================================

namespace {

namespace local {

enum {
    SCRATCH_BUF_LEN = (32 * 1024) - 64, // length in bytes of 'd_scratchBuf_p'.
                                        // Make less than a power of 2 to avoid
                                        // wasting a page
    SYMBOL_BUF_LEN  = SCRATCH_BUF_LEN   // length in bytes of 'd_symbolBuf_p'
};

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Xcoff>
                                                            StackTraceResolver;

typedef bsls::Types::UintPtr UintPtr;

}  // close namespace local

}  // close unnamed namespace

template <typename TYPE>
inline static
local::UintPtr parseNumber(const TYPE& text)
    // Parse the specified 'text' object as a char representation of a decimal
    // number and return that number, stopping at the first character which is
    // not a decimal digit, or at the end 'text'.  Return the number.  The
    // behavior is undefined unless the number fits into an UintPtr value.
{
    local::UintPtr result = 0;
    const char *p = (const char *) &text;
    const char *end = (const char *) &text + sizeof(text);
    for (; p < end && '0' <= *p && *p <= '9'; ++p) {
        result = result * 10 + (*p - '0');
    }

    return result;
}

 // ===========================================================================
 // struct balst::StackTraceResolverImpl<balst::ObjectFileFormat::Xcoff>::
 //                                                                     AuxInfo
 //               == struct local::StackTraceResolver::AuxInfo
 // ===========================================================================

bslmt::QLock local::StackTraceResolver::s_demangleQLock =
                                                       BSLMT_QLOCK_INITIALIZER;

struct local::StackTraceResolver::AuxInfo {
    // Objects of this type exist in the array 'd_auxInfo' in class
    // local::StackTraceResolver in a 1-1 correspondence with the elements of
    // the 'd_segFramePtrs_p' and 'd_segAddresses_p' arrays in that class.
    // Note that 'SYMENT' and 'AUXENT' are types defined as part of the Xcoff
    // standard.

    SYMENT  d_symEnt;              // symbol table entry

    SYMENT  d_sourceSymEnt;        // symbol table entry corresponding to
                                   // sourcefile.  If the line number is
                                   // known & symbol is in an include file, the
                                   // means of obtaining the source file name
                                   // is here

    AUXENT  d_sourceAuxEnt;        // if the means of obtaining the source file
                                   // is not in d_sourceSymEnt, it is here

    UintPtr d_includesStartIndex;  // start of range of symbol table
                                   // entries corresponding to include
                                   // files

    UintPtr d_includesEndIndex;    // end of above

    UintPtr d_functionStartIndex;  // start of range of indexes for
                                   // function we are looking up

    UintPtr d_functionEndIndex;    // end of range of indexes for
                                   // function we are looking up

    UintPtr d_lineNumberOffset;    // offset into line number table
                                   // corresponding to this symbol

    int     d_lineNumberBase;      // amount to adust the line number if
                                   // it's relative

    bool    d_symEntValid;         // whether 'd_symEnt' is valid

    bool    d_sourceSymEntValid;   // whether 'd_sourceSymEnt' is valid
};

struct local::StackTraceResolver::LoadAuxInfosInfo {
    // This 'struct' contains pointers to variables in the routine
    // 'loadSymbols' with the same names (minus the 'd_' prefix.  This struct
    // is only used for communicating between 'loadSymbols' and 'loadAuxInfos'.
    // 'LoadSymbols' calls 'loadAuxInfos' frequently, so it is important these
    // calls be fast.  There are many args to be passed, and on 64 bit, each
    // arg is at least 8 bytes, passing that much information on the stack that
    // frequently would be quite slow.  Passing a pointer to this struct of
    // pointers is very fast, especially considering that, in most calls, none
    // of the fields in this struct will be accessed.  Note that all these
    // pointers only have to be initialized once per call to 'loadSymbols',
    // which is called once per call to 'resolveSegment'.

    UintPtr  *d_symIndex;            // pointer to 'symIndex'
    UintPtr  *d_funcEndIndex;        // pointer to 'funcEndIndex'
    UintPtr  *d_lineNumberOffset;    // pointer to 'lineNumberOffset'
    UintPtr  *d_sourceEndIndex;      // pointer to 'sourceEndIndex'
    SYMENT  **d_symEnt;              // pointer to 'symEnt'
    UintPtr  *d_includesStartIndex;  // pointer to 'includesStartIndex'
    UintPtr  *d_includesEndIndex;    // pointer to 'includesEndIndex'
    SYMENT   *d_savedSourceSymEnt;   // pointer to 'savedSourceSymEnt'
    AUXENT   *d_savedSourceAuxEnt;   // pointer to 'savedSourceAuxEnt'
};

     // -------------------------------------------------------------------
     // class balst::StackTraceResolverImpl<balst::ObjectFileFormat::Xcoff>
     //                == class local::StackTraceResolver
     // -------------------------------------------------------------------

// PRIVATE CREATORS
local::StackTraceResolver::StackTraceResolverImpl(
                                              balst::StackTrace *stackTrace,
                                              bool               demangle)
: d_helper(0)
, d_stackTrace_p(stackTrace)
, d_segFramePtrs_p(0)
, d_segAddresses_p(0)
, d_segAuxInfos_p(0)
, d_numCurrAddresses(0)
, d_scratchBuf_p(0)
, d_symbolBuf_p(0)
, d_virtualToPhysicalOffset(0)
, d_archiveMemberOffset(0)
, d_archiveMemberSize(0)
, d_symTableOffset(0)
, d_stringTableOffset(0)
, d_demangle(demangle)
, d_hbpAlloc()
{
    int totalNumFrames = stackTrace->length();

    d_segAddresses_p = (const void **) allocator()->allocate(
                                              sizeof(void *) * totalNumFrames);
    memset(d_segAddresses_p, 0, sizeof(void *) * totalNumFrames);

    d_segFramePtrs_p = (balst::StackTraceFrame **)
                        allocator()->allocate(sizeof(void *) * totalNumFrames);

    d_segAuxInfos_p = (AuxInfo *) allocator()->allocate(
                                             sizeof(AuxInfo) * totalNumFrames);

    d_scratchBuf_p = (char *) allocator()->allocate(local::SCRATCH_BUF_LEN);
    d_symbolBuf_p  = (char *) allocator()->allocate(local::SYMBOL_BUF_LEN);
}

local::StackTraceResolver::~StackTraceResolverImpl()
{
}

// PRIVATE MANIPULATORS
int local::StackTraceResolver::findArchiveMember(const char *memberName)
{
    fl_hdr archiveHeader;
    if (d_helper->readExact(&archiveHeader, sizeof(archiveHeader), 0)) {
        return -1;                                                    // RETURN
    }

    // check the magic number

    if (memcmp(&archiveHeader, AIAMAGBIG, SAIAMAG)) {
        eprintf("Bad archive header magic number: 0x%llx\n",
                                     * (bsls::Types::Uint64 *) &archiveHeader);
        return -1;                                                    // RETURN
    }

    // all integer fields in the archive are represented as text

    UintPtr firstMemberOffset = parseNumber(archiveHeader.fl_fstmoff);
    UintPtr lastMemberOffset  = parseNumber(archiveHeader.fl_lstmoff);
    UintPtr curOffset         = firstMemberOffset;

    // traverse the linked list of archive members

    while (1) {
        ar_hdr memberHeader;

        // read the header, not including member file name

        if (d_helper->readExact(&memberHeader, sizeof(ar_hdr), curOffset)) {
            return -1;                                                // RETURN
        }
        bsls::Types::IntPtr nameLength = parseNumber(memberHeader.ar_namlen);
        if (nameLength >= local::SCRATCH_BUF_LEN) {
            return -1;                                                // RETURN
        }

        // now read the member name into scratch buffer

        UintPtr nameOffset =
              (char *) &memberHeader._ar_name.ar_name - (char *) &memberHeader;
        if (d_helper->readExact(d_scratchBuf_p,
                                nameLength,
                                curOffset + nameOffset)) {
            return -1;                                                // RETURN
        }
        d_scratchBuf_p[nameLength] = 0;
        if (0 == bsl::strcmp(d_scratchBuf_p, memberName)) {
            d_archiveMemberSize = parseNumber(memberHeader.ar_size);
            d_archiveMemberOffset = curOffset + sizeof(struct ar_hdr) +
                                                                    nameLength;

            // align the offset to even-byte boundary

            if (1 & d_archiveMemberOffset) {
                ++d_archiveMemberOffset;
            }
            return 0;                                                 // RETURN
        }
        zprintf("@%lx %s\n", curOffset, d_scratchBuf_p);
        if (curOffset == lastMemberOffset) {
            break;
        }
        else {
            curOffset = parseNumber(memberHeader.ar_nxtmem);
        }
    }

    return -1;
}

local::UintPtr local::StackTraceResolver::findCsectIndex(
                                                   const char *symbolAddress,
                                                   const char *csectEndAddress,
                                                   UintPtr     primarySymIndex)
{
    // This is the performance critical part of the code, at least for deep
    // backtraces.  Oleg rewrote the implementation to keep the addresses
    // together in an array rather than just referring to them through
    // 'd_segFramePtrs_p', to be nice to the cache (~2x speedup).

    // An experiment was done by Bill changing this to a binary search using an
    // STL multimap, but no speedup was observed, so the increase in code
    // complexity was deemed unjustified and the change was backed out.

    for (int i = 0; i < d_numCurrAddresses; ++i) {
        const void *a = d_segAddresses_p[i];
        if (symbolAddress <= a && a < csectEndAddress) {
            zprintf("addr=%p csectEnd=%p iaddr=%p MATCH\n",
                          symbolAddress, csectEndAddress, d_segAddresses_p[i]);

            return primarySymIndex;                                   // RETURN
        }
        else {
            zprintf("addr=%p csectEnd=%p iaddr=%p no match\n",
                                            symbolAddress, csectEndAddress, a);
        }
    }

    return (UintPtr) - 1;
}

int local::StackTraceResolver::findIncludeFile(
                                          SYMENT  *includeSymEnt,
                                          UintPtr  firstLineNumberOffset,
                                          UintPtr  lineNumberOffset,
                                          UintPtr  symStartIndex,
                                          UintPtr  symEndIndex)
{
    zprintf("Scanning %lu-%lu for includes\n", symStartIndex, symEndIndex);

    // [ symBufStartIndex, symBufEndIndex ) is the range within
    // [ symStartIndex, symEndIndex ) that is loaded into the symbol buffer.

    UintPtr symBufStartIndex = symStartIndex;
    UintPtr symBufEndIndex = symBufStartIndex;
    const UintPtr maxSymsInBuf = local::SYMBOL_BUF_LEN / SYMESZ;

    bool bincl = false;
    bool binclFirst = false;
    for (UintPtr symIndex = symStartIndex; symIndex < symEndIndex; ++symIndex){
        if (symIndex >= symBufEndIndex) {
            // read a bufferful of symbols, this always happens the first time
            // through the loop

            UintPtr numSymsToRead =
                                bsl::min(symEndIndex - symIndex, maxSymsInBuf);
            UintPtr offsetToRead = d_symTableOffset + SYMESZ * symIndex;
            zprintf("Reading %lu symbols at offset %lu\n",
                                                  numSymsToRead, offsetToRead);
            if (d_helper->readExact(d_symbolBuf_p,
                                    numSymsToRead * SYMESZ,
                                    offsetToRead)) {
                eprintf("failed to read %d symbols from offset %lu,"
                            " errno %d\n", numSymsToRead, offsetToRead, errno);
                return -1;                                            // RETURN
            }
            symBufStartIndex = symIndex;
            symBufEndIndex = symBufStartIndex + numSymsToRead;
            zprintf("Now [%lu,%lu) in buffer\n", symBufStartIndex,
                                                               symBufEndIndex);
        }
        SYMENT *symEnt = (SYMENT *) (d_symbolBuf_p +
                                       (symIndex - symBufStartIndex) * SYMESZ);
        if (C_BINCL == symEnt->n_sclass) {
            zprintf("%lu BINCL %lu (need %lu)\n", symIndex,
                                  (UintPtr) symEnt->n_value, lineNumberOffset);
            bincl = (symEnt->n_value <= lineNumberOffset);
            binclFirst = (symEnt->n_value <= firstLineNumberOffset);
        }
        else {
            if (C_EINCL == symEnt->n_sclass) {
                zprintf("%lu EINCL %lu (need %lu)\n", symIndex,
                                  (UintPtr) symEnt->n_value, lineNumberOffset);
                if (bincl && symEnt->n_value >= lineNumberOffset) {
                    *includeSymEnt = *symEnt;
                    if (binclFirst) {
                        zprintf("Found, includes first line\n");

                        return k_FOUND_INCLUDE_FILE;                  // RETURN
                    }
                    else {
                        zprintf("Found, does not include first line\n");
                        return k_FOUND_INCLUDE_FILE |
                                        k_LINE_NUMBER_IS_ABSOLUTE;    // RETURN
                    }
                }
            }
            bincl = false;
        }

        // skip any AUXENT's, we aren't interested in them

        symIndex += symEnt->n_numaux;
    }

    return 0;
}

int local::StackTraceResolver::findLineNumber(int       *outLineNumber,
                                              UintPtr   *outLineNumberOffset,
                                              UintPtr    lineBufStartOffset,
                                              const void *segAddress)
{
    // The scratch buffer is 32 K, which is equivalent to about 3200 lines of
    // code.  We don't want to read that much every time, so read in smaller
    // amounts (though we use the scratch buffer).  It is better to read about
    // 4K at a time, which is 400 lines or so.  The relative offset must be a
    // multiple of LINESZ.

    enum { SHORT_BUF_LEN = 4000 - 4000 % LINESZ };
    BSLMF_ASSERT(SHORT_BUF_LEN <= local::SCRATCH_BUF_LEN);

    UintPtr       lineBufEndOffset = lineBufStartOffset;
    UintPtr       maxScan          = d_archiveMemberSize - lineBufStartOffset;
    const UintPtr maxLineOffset    =
                               maxScan - maxScan % LINESZ + lineBufStartOffset;

    int lineNumber = -1;     // we don't know yet if this is relative or
                                // absolute
    UintPtr lineNumberOffset = lineBufStartOffset;

    zprintf("SYMBOL ADDRESS=%lu\n", segAddress);

    bool first = true;
    for (UintPtr lineOffset = lineBufStartOffset; true; lineOffset += LINESZ) {
        if (lineOffset >= lineBufEndOffset) {
            // read a bufferful of data, this always happens the first time
            // through the loop

            if (lineOffset != lineBufEndOffset) {
                return -1;                                            // RETURN
            }
            if (maxLineOffset <= lineOffset) {
                zprintf("breaking out: EOF\n");
                break;
            }
            const UintPtr readSize = bsl::min((UintPtr) SHORT_BUF_LEN,
                                              maxLineOffset - lineOffset);
            BSLS_ASSERT_SAFE(0 == readSize % LINESZ);
            int rc = d_helper->readExact(d_scratchBuf_p,
                                         readSize,
                                         d_archiveMemberOffset + lineOffset);
            if (rc) {
                return -1;                                            // RETURN
            }
            lineBufStartOffset = lineOffset;
            lineBufEndOffset += readSize;
            zprintf("lineOffset=%lu in buf [%lu-%lu] readSize=%ld\n",
                   lineOffset, lineBufStartOffset, lineBufEndOffset, readSize);
        }

        LINENO *lineno = (LINENO *) (d_scratchBuf_p + lineOffset -
                                                           lineBufStartOffset);
        if (first) {
            first = false;

            // skip the first 'LINENO', it is a special one for the beginning
            // of the function

            zprintf("at (%d) ptr=%p symNdx=%u lnno=%d sizeof(LINENO)=%d\n",
                    lineOffset,
                    lineno,
                    lineno->l_addr.l_symndx,
                    (int) lineno->l_lnno,
                    (int) sizeof(LINENO));
        }
        else {
            char *addr = (char *)
                          (lineno->l_addr.l_paddr + d_virtualToPhysicalOffset);
            zprintf("at (%d) ptr=%p paddr=%lu lnno=%d %s\n", lineOffset,
                                            lineno, addr, (int) lineno->l_lnno,
                                               addr <= segAddress ? "<" : ".");
            if (0 == lineno->l_lnno) {
                // start of next function

                zprintf("breaking out -- end of function\n");
                break;
            }
            if (addr <= segAddress) {
                // note this will usually happen many times before we return

                lineNumber = lineno->l_lnno;
                lineNumberOffset = lineOffset;
            }
            else {
                zprintf("breaking out -- line after\n");
                break;
            }
        }
    }

    zprintf("Detected lineNumber=%d, lineNumberOffset=%lu\n",
                                           lineNumber, lineNumberOffset);

    // Aix seems to start line numbers at 0, disagreeing with __LINE__, vi,
    // and emacs.  It's VERY consistently off by one, so it's easy for us
    // to correct by subtracting one here.

    *outLineNumber       = lineNumber - 1;
    *outLineNumberOffset = lineNumberOffset;

    return 0;
}

void local::StackTraceResolver::loadAuxInfos(
                                  const LoadAuxInfosInfo *info,
                                  const char             *functionBeginAddress,
                                  const char             *functionEndAddress)
{
    for (int i = 0; i < d_numCurrAddresses; ++i) {
        const void *address = d_segAddresses_p[i];

        if (functionBeginAddress <= address && address < functionEndAddress) {
            zprintf("addr=%p csectEnd=%p iaddr=%p  MATCH func %lu-%lu\n",
                    functionBeginAddress, functionEndAddress, address,
                    *info->d_symIndex, *info->d_funcEndIndex);

            d_segFramePtrs_p[i]->setOffsetFromSymbol(
                                (const char *) address - functionBeginAddress);

            AuxInfo *auxInfo = &d_segAuxInfos_p[i];

            auxInfo->d_symEnt = **info->d_symEnt;
            auxInfo->d_symEntValid = true;
            if (*info->d_symIndex < *info->d_sourceEndIndex) {
                zprintf("Func %lu-%lu, lnnop %lu sourceEnd %lu\n",
                        *info->d_symIndex, *info->d_funcEndIndex,
                        *info->d_lineNumberOffset, *info->d_sourceEndIndex);

                auxInfo->d_sourceSymEntValid  = true;
                auxInfo->d_sourceSymEnt       = *info->d_savedSourceSymEnt;
                auxInfo->d_sourceAuxEnt       = *info->d_savedSourceAuxEnt;
                auxInfo->d_includesStartIndex = *info->d_includesStartIndex;
                auxInfo->d_includesEndIndex   = *info->d_includesEndIndex;
                auxInfo->d_functionStartIndex = *info->d_symIndex;
                auxInfo->d_functionEndIndex   = *info->d_funcEndIndex;
                auxInfo->d_lineNumberOffset   = *info->d_lineNumberOffset;
                auxInfo->d_lineNumberBase     = -1;
            }
            else {
                auxInfo->d_sourceSymEntValid  = false;
            }
        }
        else {
            zprintf("addr=%p iaddr=%p no match\n",
                                    functionBeginAddress, d_segAddresses_p[i]);
        }
    }
}

int local::StackTraceResolver::loadSymbols(UintPtr numSyms,
                                           int     textSectionNum)
{
    if (0 == numSyms) {
        return 0;                                                     // RETURN
    }

    SYMENT *symEnt = 0;               // always == symPtr, just different type

    SYMENT savedSymEnt;               // place to save SYMENT in case we go on
                                      // to the next buffer

    UintPtr primarySymIndex;          // index of the current SYMENT

    int numAux = 0;                   // number of remaining AUXENTs for
                                      // current SYMENT.  If 0 we're reading
                                      // the primary symbol, otherwise we're
                                      // reading an auxiliary symbol.

    UintPtr csectIndex = -1;          // index of the current csect of interest
                                      // (if some of our addresses reside in
                                      // the csect), or -1

    const char *csectEndAddress = 0;  // end address of the csect above in
                                      // memory

    UintPtr funcEndIndex = 0;         // end index of the current function,
                                      // from the function AUXENT

    UintPtr lineNumberOffset = 0;     // the offset in the file where we are to
                                      // start looking for the line number
                                      // information

    UintPtr sourceEndIndex = 0;       // index in symbol table where current
                                      // source file ends and next source file
                                      // starts

    UintPtr includesStartIndex;
    UintPtr includesEndIndex;         // indexes in symbol of range of entries
                                      // describing include files, gradually
                                      // populated when we encounter
                                      // sourcefile.

    SYMENT savedSourceSymEnt;         // place to save SYMENT of source file

    AUXENT savedSourceAuxEnt;         // place to save AUXENT of source file

    UintPtr symIndex = 0;             // index of the current SYMENT or AUXENT

    enum { NUM_TOTAL_SYMS = local::SYMBOL_BUF_LEN / SYMESZ };
                                      // number of SYMENT's / AUXENT's that
                                      // will fit into the buffer

    const LoadAuxInfosInfo laiInfo = {
        &symIndex,
        &funcEndIndex,
        &lineNumberOffset,
        &sourceEndIndex,
        &symEnt,
        &includesStartIndex,
        &includesEndIndex,
        &savedSourceSymEnt,
        &savedSourceAuxEnt
    };

    while (numSyms > symIndex) {
        const UintPtr numSymsToRead =
                        bsl::min(numSyms - symIndex, (UintPtr) NUM_TOTAL_SYMS);

        const UintPtr symBufStartIndex = symIndex;

        const UintPtr symBufStartOffset = d_symTableOffset + SYMESZ * symIndex;
        if (d_helper->readExact(d_symbolBuf_p,
                                numSymsToRead * SYMESZ,
                                symBufStartOffset)) {
            eprintf("failed to read %d symbols from offset %lu, errno %d\n",
                                      numSymsToRead, symBufStartOffset, errno);
            return -1;                                                // RETURN
        }

        const UintPtr symBufEndIndex = symIndex + numSymsToRead;
            // last index we're processing

        zprintf("Parsing [%lu, %lu)\n", symIndex, symBufEndIndex);

        // The symbol buffer contains objects of type 'SYMENT' and 'AUXENT',
        // both of which are the same size 'SYMESZ', (not the same as
        // 'sizeof(SYMENT)' or 'sizeof(AUXENT)' because the sizeof will add
        // padding).  We go through the symbols in sequence, we will first
        // encounter a 'SYMENT' and it will then be followed by
        // 'symEnt->n_numAux' number of 'AUXENT's which have more information
        // relating to that 'SYMENT', after these 'AUXENT's will be the next
        // 'SYMENT', and so forth.

        for (; symIndex < symBufEndIndex; ++symIndex) {
            char *symPtr = d_symbolBuf_p +
                                        (symIndex - symBufStartIndex) * SYMESZ;
            if (0 == numAux) {
                // it's definitely a SYMENT

                symEnt = (SYMENT *) symPtr;
                primarySymIndex = symIndex;

                switch(symEnt->n_sclass) {
                  case C_BINCL:
                  case C_EINCL: {
                    includesEndIndex = symIndex + 1;
                  } break;
                  case C_FILE: {
                    savedSourceSymEnt = *symEnt;
                    sourceEndIndex = symEnt->n_value;
                    includesStartIndex = symIndex + symEnt->n_numaux;
                    includesEndIndex = 0;
                    numAux = symEnt->n_numaux;
                  } break;
                  case C_EXT:
                  case C_HIDEXT:
                  case C_WEAKEXT:
                  case C_FCN: {
                    // do not check the section# and skip AUXENTs, instead
                    // defer the check until function AUXENT is read and
                    // skip the whole function in case of section mismatch

                    numAux = symEnt->n_numaux;
                  } break;
                  default: {
                    // We ignore other types of symbols.  Skip all the aux
                    // entries.

                    symIndex += symEnt->n_numaux;
                  } break;
                }
            }
            else {
                // It turns out it's not a SYMENT, it's an AUXENT.  Cast it.

                AUXENT *auxEnt = (AUXENT *) symPtr;

                --numAux;
                switch (symEnt->n_sclass) {
                  case C_FILE: {
                    // Find out the source-file string type

                    int ftype = auxEnt->x_file._x.x_ftype;
                    if (XFT_FN == ftype) {
                        // ftype specifies that x_fname stores the source-file
                        // name

                        savedSourceAuxEnt = *auxEnt;
                    }
                  } break;
                  case C_FCN: {
                    // Find the source line number of the function

                    int lnno = auxEnt->x_sym.x_misc.x_lnsz.x_lnno;
                    for (int i = 0; i < d_numCurrAddresses; ++i) {
                        if (  d_segAuxInfos_p[i].d_sourceSymEntValid
                           && d_segAuxInfos_p[i].d_functionEndIndex >  symIndex
                           && d_segAuxInfos_p[i].d_functionStartIndex <symIndex
                           && -1 == d_segAuxInfos_p[i].d_lineNumberBase) {

                            // line number of the start of the function

                            d_segAuxInfos_p[i].d_lineNumberBase = lnno;
                        }
                    }
                  } break;
                  case C_EXT:
                  case C_WEAKEXT:
                  case C_HIDEXT: {
                    if (0 == numAux) {
                        // this is the last AUXENT following the primary
                        // SYMENT

                        if (XMC_PR == auxEnt->x_csect.x_smclas) {
                            // The csect specifies program code.  The csect
                            // contains the executable instructions of the
                            // program.

                            union {
                                // This variable has radically different
                                // meanings depending on the value of
                                // 'auxEnt->x_csect.x_smtyp' so it is in a
                                // union in order to give it
                                // context-appropriate names

                                UintPtr d_scnLen;               // assign to
                                                                // this

                                UintPtr d_sdEndFunctionOffset;  // bytes of
                                                                // code in the
                                                                // current
                                                                // function

                                UintPtr d_ldCsectIndex;         // index of the
                                                                // current
                                                                // csect at
                                                                // time of
                                                                // match
                            } u;
#ifdef __XCOFF64__
                            u.d_scnLen =
                                    (UintPtr) auxEnt->x_csect.x_scnlen_hi << 32
                                                 | auxEnt->x_csect.x_scnlen_lo;
#else
                            u.d_scnLen = auxEnt->x_csect.x_scnlen;
#endif
                            const char *symbolAddress =
                                          (char *) (symEnt->n_value +
                                                    d_virtualToPhysicalOffset);

                            switch (auxEnt->x_csect.x_smtyp & 0x7) {
                              case XTY_SD: {
                                // check if this csect contains any of the
                                // addresses we want

                                csectEndAddress = symbolAddress +
                                                       u.d_sdEndFunctionOffset;

                                csectIndex = findCsectIndex(symbolAddress,
                                                            csectEndAddress,
                                                            primarySymIndex);
                              } break;
                              case XTY_LD: {
                                if (csectIndex == u.d_ldCsectIndex) {
                                    loadAuxInfos(&laiInfo,
                                                 symbolAddress,
                                                 csectEndAddress);
                                }
                              } break;
                            }
                        }
                    }
                    else if (csectIndex != (UintPtr) - 1) {
#ifdef __XCOFF_32__
                        funcEndIndex = auxEnt->x_sym.x_fcnary.x_fcn.x_endndx;
#else
                        funcEndIndex = auxEnt->x_fcn.x_endndx;
#endif
                        if (symEnt->n_scnum != textSectionNum &&
                                                     funcEndIndex <= numSyms) {
                            zprintf("wrong section %d, need %d, skipping"
                                                         " to next function\n",
                                    (int) symEnt->n_scnum, textSectionNum,
                                                                 funcEndIndex);

                            // skip all symbols inside this function

                            symIndex = funcEndIndex - 1;
                        }
                        else {
                            // set lineNumberOffset, which will eventually be
                            // copied to the appropriate AuxInfo

#ifdef __XCOFF_32__
                            lineNumberOffset =
                                        auxEnt->x_sym.x_fcnary.x_fcn.x_lnnoptr;
#else
                            lineNumberOffset = auxEnt->x_fcn.x_lnnoptr;
#endif
                            zprintf("FCN lnnoptr=%lu funcend=%lu\n",
                                               lineNumberOffset, funcEndIndex);
                        }
                    }
                  } break;
                }
            }
        }

        // Save the parsing state in data members

        zprintf("Saving symEnt %p, symIndex %lu, numAux %d\n",
                                                     symEnt, symIndex, numAux);

        // We're about to dereference 'symEnt'.  It should be impossible for it
        // to be 0 at this point, but if it is better to assert & explain why
        // than just segfault.

        BSLS_ASSERT(0 != symEnt);

        // Since we're about to read in a whole new buffer, 'symEnt', which
        // points into that buffer, will be invalidated.  We may need to look
        // at that SYMENT.  So save the SYMENT to 'savedSyment' and point
        // 'symEnt' to it.

        if (symEnt != &savedSymEnt) {
            savedSymEnt = *symEnt;
            symEnt = &savedSymEnt;
        }
    }

    return 0;
}

const char *local::StackTraceResolver::getSourceName(
                                                    const AUXENT *sourceAuxEnt)
{
    return 0 == sourceAuxEnt->x_file._x.x_zeroes
           ? d_helper->loadString(d_stringTableOffset +
                                              sourceAuxEnt->x_file._x.x_offset,
                                  d_scratchBuf_p,
                                  local::SCRATCH_BUF_LEN,
                                  allocator())
           : bdlb::String::copy(sourceAuxEnt->x_file.x_fname,
                               FILNMLEN,
                               allocator());
}

const char *local::StackTraceResolver::getSymbolName(const SYMENT *symEnt)
{
    const char *srcName = 0;

#ifdef __XCOFF32__
    if (0 != symEnt->n_zeroes) {
        srcName = bdlb::String::copy(symEnt->n_name, SYMNMLEN, allocator());
    }
#endif

    if (0 == srcName) {
        srcName = d_helper->loadString(d_stringTableOffset + symEnt->n_offset,
                                       d_scratchBuf_p,
                                       local::SCRATCH_BUF_LEN,
                                       allocator());
    }

    return srcName;
}

int local::StackTraceResolver::resolveSegment(void       *segmentPtr,
                                              UintPtr     segmentSize,
                                              const char *libraryFileName,
                                              const char *displayFileName,
                                              const char *archiveMemberName)
{
    int rc;

    zprintf("ResolveSegment lfn=%s sp=%p se=%p dfn=%s amn=%s\n",
            libraryFileName, segmentPtr,
            (char *) segmentPtr + segmentSize, displayFileName,
            archiveMemberName ? archiveMemberName : "nul");

    int totalNumFrames = d_stackTrace_p->length();
    zprintf("num all addresses = %d\n", totalNumFrames);

    memset(d_segAuxInfos_p, 0, sizeof(AuxInfo) * d_numCurrAddresses);

    d_numCurrAddresses = 0;
    for (int i = 0; i < totalNumFrames; ++i) {
        const void *address = (*d_stackTrace_p)[i].address();

        // Identify the stack frames that are defined in this object file.

        if (address >= segmentPtr && address <
                                           (char *) segmentPtr + segmentSize) {
            zprintf("address %p MATCH\n", address);
            d_segFramePtrs_p[d_numCurrAddresses] = &(*d_stackTrace_p)[i];
            d_segAddresses_p[d_numCurrAddresses] = address;
            ++d_numCurrAddresses;
        }
        else {
            zprintf("address %p NO MATCH\n", address);
        }
    }
    if (0 == d_numCurrAddresses) {
        return 0;                                                     // RETURN
    }

    displayFileName = bdlb::String::copy(displayFileName, allocator());

    const UintPtr baseAddress = (UintPtr) segmentPtr;

    for (int i = 0; i < d_numCurrAddresses; ++i) {
        d_segFramePtrs_p[i]->setLibraryFileName(displayFileName);
    }

    balst::StackTraceResolver_FileHelper helper(libraryFileName);
    d_helper = &helper;

    if (archiveMemberName && archiveMemberName[0]) {
        rc = findArchiveMember(archiveMemberName);
        if (rc) {
            return rc;                                                // RETURN
        }
    }
    else {
        d_archiveMemberOffset = 0;
        d_archiveMemberSize =
                            bdls::FilesystemUtil::getFileSize(libraryFileName);
    }

    xcoffhdr header;                  // Xcoff header at the beginning of the
                                      // file or archive member

    if (d_helper->readExact(&header, sizeof(xcoffhdr), d_archiveMemberOffset)){
        return -1;                                                    // RETURN
    }

    switch(header.filehdr.f_magic) {
      case U802TOCMAGIC: {
        zprintf("32-bit XCOFF detected\n");
#ifdef __XCOFF64__
        eprintf("Expected 64 bit binary, encountered 32 bit\n");
        return -1;                                                    // RETURN
#endif
      } break;
      case U803XTOCMAGIC: {
        eprintf("discontinued 64-bit XCOFF detected\n");
        return -1;                                                    // RETURN
      } break;
      case U64_TOCMAGIC: {
        zprintf("64-bit XCOFF detected\n");
#ifdef __XCOFF32__
        eprintf("Expected 32 bit binary, encountered 64 bit\n");
        return -1;                                                    // RETURN
#endif
      } break;
      default: {
        eprintf("Unknown magic number 0%o \n", (int) header.filehdr.f_magic);
        return -1;                                                    // RETURN
      } break;
    }

    scnhdr textSectionHeader;
    int textSectionNum = header.aouthdr.o_sntext;
    UintPtr textSectionHeaderOffset = d_archiveMemberOffset + FILHSZ +
                       header.filehdr.f_opthdr + (textSectionNum - 1) * SCNHSZ;
    if (d_helper->readExact(&textSectionHeader,
                            sizeof(scnhdr),
                            textSectionHeaderOffset)) {
        return -1;                                                    // RETURN
    }

    UintPtr textSectionOffset = textSectionHeader.s_scnptr;
    if (0 == textSectionOffset) {
        eprintf("null textSectionOffset\n");
        return -1;                                                    // RETURN
    }

    // offset 0 is mapped to baseAddress
    // offset s_scnptr has virtual address header.aouthdr.o_text_start
    //                 , is mapped to segmentPtrs + s_scnptr

    d_virtualToPhysicalOffset = (UintPtr) segmentPtr + textSectionOffset -
                                                   header.aouthdr.o_text_start;
    d_symTableOffset = d_archiveMemberOffset + header.filehdr.f_symptr;
    const UintPtr numSyms = header.filehdr.f_nsyms;
    d_stringTableOffset = d_symTableOffset + numSyms * SYMESZ;
    zprintf("member offset %lu, size %lu; text sec %d, base %lx,"
            " s_scnptr %lx, o_text_start %lx, adjustment %lx; "
            " symTableOffset %lu, stringTableOffset %lu, numSyms %lu\n",
            d_archiveMemberOffset, d_archiveMemberSize, textSectionNum,
            baseAddress,
            textSectionOffset,
            (UintPtr) header.aouthdr.o_text_start,
            d_virtualToPhysicalOffset,
            d_symTableOffset, d_stringTableOffset,
            numSyms);

    rc = loadSymbols(numSyms, textSectionNum);
    if (rc) {
        return rc;                                                    // RETURN
    }

    // load all found symbol names, etc

    // Note that if the call stack contained recursion, this can wind up
    // loading the same symbol multiple times, which is no big deal.

    for (int i = 0; i < d_numCurrAddresses; ++i) {
        AuxInfo               *auxInfo = d_segAuxInfos_p + i;
        balst::StackTraceFrame *frame   = d_segFramePtrs_p[i];

        if (auxInfo->d_symEntValid) {
            const char *symbolName = getSymbolName(&auxInfo->d_symEnt);
            frame->setMangledSymbolName(symbolName);
            zprintf("Loaded symbol name: %s\n", frame->mangledSymbolName());

            bsl::auto_ptr<Name> name;
            if (d_demangle) {
                // Note that 'Demangle' is not thread safe.

                bslmt::QLockGuard guard(&s_demangleQLock);

                // Note that 'Demangle' allocates with 'new', and that
                // 'remainder' is passed as a reference to a modifiable.  Also
                // note that whoever wrote 'Demangle' didn't know how to use
                // 'const'.

                char *remainder = 0;
                name.reset(Demangle(const_cast<char *>(symbolName),
                                    remainder));
                if (name.get() && remainder && *remainder) {
                    // For some reason, Demangle may leave some trailing crud
                    // at the end of the string pointed at by 'name'.
                    // 'remainder' points to the end of the identifier and the
                    // beginning of that crud.

                    *remainder = 0;
                }
            }
            if (name.get()) {
                char *text = name->Text();
                zprintf("Demangled to %s\n", text);
                frame->setSymbolName(text);
            }
            else {
                zprintf("Did not demangle: %s\n", frame->mangledSymbolName());
                frame->setSymbolName(frame->mangledSymbolName());
            }
        }

        if (auxInfo->d_sourceSymEntValid) {
            zprintf("Symbol %d loaded source file name: ?? includes %lu"
                                               " to  %lu, lnnoptr %lu ln %d\n",
                    i,
                    auxInfo->d_includesStartIndex,
                    auxInfo->d_includesEndIndex,
                    auxInfo->d_lineNumberOffset,
                    auxInfo->d_lineNumberBase);

            int     lineNumber;
            UintPtr lineNumberOffset;
            if (-1 != auxInfo->d_lineNumberBase &&
                                            0 != auxInfo->d_lineNumberOffset) {
                rc = findLineNumber(&lineNumber,
                                    &lineNumberOffset,
                                    auxInfo->d_lineNumberOffset,
                                    d_segAddresses_p[i]);
                if (rc) {
                    return -1;                                        // RETURN
                }
                SYMENT includeSymEnt;
                rc = findIncludeFile(&includeSymEnt,
                                     auxInfo->d_lineNumberOffset,
                                     lineNumberOffset,
                                     auxInfo->d_includesStartIndex,
                                     auxInfo->d_includesEndIndex);
                if (rc < 0) {
                    return -1;                                        // RETURN
                }
                if (rc & k_FOUND_INCLUDE_FILE) {
                    frame->setSourceFileName(getSymbolName(&includeSymEnt));
                }
                else {
                    frame->setSourceFileName(
                                      getSourceName(&auxInfo->d_sourceAuxEnt));
                }
                if (rc & k_LINE_NUMBER_IS_ABSOLUTE) {
                    frame->setLineNumber(lineNumber);
                }
                else {
                    // line number is relative to start of function

                    frame->setLineNumber(
                                       auxInfo->d_lineNumberBase + lineNumber);
                }
            }
            else {
                frame->setLineNumber(-1);
                if (!auxInfo->d_sourceSymEntValid) {
                    return -1;                                        // RETURN
                }

                frame->setSourceFileName(
                                      getSymbolName(&auxInfo->d_sourceSymEnt));
            }

            // d_sourceAuxEnt might have been updated to the one pointing to
            // an include file

            zprintf("Final source file name: %s line %d\n",
                    frame->sourceFileName(),
                    frame->lineNumber());
        }
        else {
            zprintf("Symbol %d source file name invalid\n", i);
        };
    }

    // 'helper' will close its file upon destruction

    return 0;
}

// PUBLIC CLASS METHODS
int local::StackTraceResolver::resolve(balst::StackTrace *stackTrace,
                                       bool               demangle)
{
    local::StackTraceResolver resolver(stackTrace,
                                       demangle);

    enum { BUF_SIZE = (8 << 10) - 64 };
    char *ldInfoBuf = (char *) resolver.allocator()->allocate(BUF_SIZE);

    int rc = loadquery(L_GETINFO, ldInfoBuf, BUF_SIZE);
    if (-1 == rc) {
        return 1;                                                     // RETURN
    }

    ld_info *currInfo = (ld_info *) ldInfoBuf;
    bool first = true;
    while (1) {
        // Check to see if we're done and exit early.  Note there is another
        // break statement at the end of the loop when we finish the linked
        // list of segments.

        int i = stackTrace->length() - 1;
        while (i >= 0 && (*stackTrace)[i].isSymbolNameKnown()) {
            --i;
        }
        if (i < 0) {
            break;
        }

        const char *fileName = (char *) currInfo->ldinfo_filename;
        const char *displayFileName = (char *) currInfo->ldinfo_filename;
        const char *archiveMemberName = fileName + bsl::strlen(fileName) + 1;

        // AIX truncates the name of the executable (which is always the first
        // one in the list) to 32 characters.  To work around this issue, we
        // use /proc/<pid>/object/a.out pseudo-link to open the executable,
        // while still displaying the correct (but possibly truncated) name to
        // the user.

        char fileNameBuf[40];
        if (first) {
            first = false;
            snprintf(fileNameBuf,
                     sizeof(fileNameBuf),
                     "/proc/%d/object/a.out",
                     bdls::ProcessUtil::getProcessId());
            fileName = fileNameBuf;
        }

        rc = resolver.resolveSegment(
                                    currInfo->ldinfo_textorg,   // segment ptr
                                    currInfo->ldinfo_textsize,  // segment size
                                    fileName,
                                    displayFileName,
                                    archiveMemberName);
        if (rc) {
            return rc;                                                // RETURN
        }

        // advance to the next ld_info structure

        if (currInfo->ldinfo_next) {
            *(char **)&currInfo += currInfo->ldinfo_next;
        }
        else {
            break;
        }
    }

    // if a source file name is ".file", it's incorrect -- this happens with
    // shared libraries at least some of the time

    for (int i = 0; i < (int) stackTrace->length(); ++i) {
        if (!bsl::strcmp((*stackTrace)[i].sourceFileName().c_str(), ".file")){
            (*stackTrace)[i].setSourceFileName("");
        }
    }

    return 0;
}

}  // close namespace BloombergLP

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
