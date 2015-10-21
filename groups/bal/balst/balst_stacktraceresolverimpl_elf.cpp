// balst_stacktraceresolverimpl_elf.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_elf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolverimpl_elf_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_ELF

#include <balst_stacktraceresolver_filehelper.h>

#include <bdlb_string.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cerrno.h>
#include <bsl_cstring.h>
#include <bsl_climits.h>
#include <bsl_cstdarg.h>
#include <bsl_vector.h>

#include <elf.h>
#include <sys/types.h>    // lstat
#include <sys/stat.h>     // lstat
#include <unistd.h>

#undef BALST_DWARF
#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DWARF)
# define BALST_DWARF 1
# include <dwarf.h>
#endif

#if defined(BSLS_PLATFORM_OS_HPUX)

# include <dl.h>
# include <aCC/acxx_demangle.h>

#elif defined(BSLS_PLATFORM_OS_LINUX)

# include <cxxabi.h>
# include <dlfcn.h>
# include <execinfo.h>
# include <link.h>

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

# include <link.h>

# if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#   include <cxxabi.h>
# endif

#else

# error unrecognized ELF platform

#endif

// ============================================================================
//              Debugging trace macros: 'eprintf' and 'zprintf'
// ============================================================================

#undef  TRACES
#define TRACES 0    // 0 == debugging traces off, eprintf and zprint do nothing
                    // 1 == debugging traces on, eprintf is like zprintf
                    // 2 == debugging traces on, eprintf exits

// Checking addresses in dwarf compile units is probably redundant, but it may
// turn out that we want to re-enable it someday, which will be possible by
// setting BALST_DWARF_CHECK_ADDRESSES to 1.

#undef  BALST_DWARF_CHECK_ADDRESSES
#define BALST_DWARF_CHECK_ADDRESSES 0

// zprintf: For non-error debug traces.  Traces:  0: null function
//                                               >0: like printf
// eprintf: Called when errors occur in data.  Traces: 0: null function
//                                                     1: like printf
//                                                     2: printf, then exit 1
// ASSERT_BAIL(expr) gentle assert.  Traces: 0: if !(expr) return -1;
//                                          >0: if !(expr) message & return -1
//     // 'ASSERT_BAIL' is needed because we need asserts for the DWARF code,
//     // but it's totally inappropriate to core dump if the DWARF data is
//     // corrupt, because we can give a prety good stack trace without the
//     // DWARF information, so if 'expr' fails, just quit the DWARF analysis
//     // and continue showing the rest of the stack trace to the user.
//
// P(expr): TRACES:  0: evaluates to 'false'.
//                  >0: output << #expr << (expr) then evaluate to false.
//    // P(expr) is to be used in ASSERT_BAIL, i.e.
//    // 'ASSERT_BAIL(5 == x || P(x))' will, if (5 != x), print out the value
//    // of x then print the assert message and return -1.
//
// PH(expr): like P(expr), except prints it in hex.

#undef eprintf
#undef zprintf
#undef ASSERT_BAIL
#undef P
#undef PH

#if TRACES > 0
# include <stdio.h>

#define zprintf printf

static const char assertBailString[] = {
                "Warning: assertion (%s) failed at line %d in function %s\n" };

#define ASSERT_BAIL(expr)    do {                                             \
        if (!(expr)) {                                                        \
            eprintf(assertBailString, #expr, __LINE__, rn);                   \
            return -1;                                                        \
        }                                                                     \
    } while (false)
    // If the specified 'expr' evaluates to 'false', print a message and return
    // -1.  Note that if 'TRACES > 1' the 'eprintf' will exit 1.

#if 0 // comment until used, avoid "never called" warnings.
static bool warnPrint(const char *name, const void *value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'PH'.  Return 'false'.
{
    zprintf("%s = %p\n", name, value);

    return false;
}
#endif
#if defined(BALST_DWARF)
// 'P()' and 'PH()' are only uused in ASSERT_BAIL in DWARF code, only define
// these functions in DWARF compiles, otherwise get unused messages.

static bool warnPrint(const char                       *name,
                      BloombergLP::bsls::Types::Uint64  value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'P'.  Return 'false'.
{
    zprintf("%s = %llu\n", name, value);

    return false;
}

static bool warnPrintHex(const char                        *expr,
                         BloombergLP::bsls::Types::Uint64   value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'PH'.  Return 'false'.
{
    zprintf("%s = 0x%llx\n", expr, value);

    return false;
}

#define P(expr)     warnPrint(   #expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // decimal and return 'false'.

#define PH(expr)    warnPrintHex(#expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // hex and return 'false'.
#endif

#if 1 == TRACES
# define eprintf printf
#else
static
int eprintf(const char *format, ...)
    // Do with the arguments, including the specified 'format', exactly what
    // 'printf' would do, then exit.
{
    va_list ap;
    va_start(ap, format);

    vprintf(format, ap);

    va_end(ap);

    exit(1);    // If traces are enabled, there are so many traces that the
                // only way to draw attention to an error is to exit.  Core
                // files would just waste disk space -- we can always put a
                // breakpoint here and repeat.
    return 0;
}
#endif

#else

static inline
int eprintf(const char *, ...)
    // do nothing
{
    return 0;
}

static inline
int zprintf(const char *, ...)
    // do nothing
{
    return 0;
}

#define ASSERT_BAIL(expr) do {                                                \
        if (!(expr)) {                                                        \
            return -1;                                                        \
        }                                                                     \
    } while (false)
    // If the specified 'expr' evaluates to 'false', return -1, otherwise do
    // nothing.

#if defined(BALST_DWARF)
#define P(expr)            (false)
    // Ignore 'expr' and return 'false'.
#define PH(expr)           (false)
    // Ignore 'expr' and return 'false'.
#endif

#endif

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
# define ASSERT_BAIL_SAFE(expr) ASSERT_BAIL(expr)
    // Do ASSERT_BAIL
#else
# define ASSERT_BAIL_SAFE(expr)
    // Do nothing.
#endif

// Rohan's:
// IMPLEMENTATION NOTES:
//
// All of the following 'struct' definitions are specific to Sun Solaris and
// are derived from '/usr/include/sys/elf.h'.  Note that we use 32-bit
// 'struct's for explanation.
//
// Each ELF object file or executable starts with an ELF header that specifies
// how many segments are provided in the file.  The ELF header looks as
// follows:
//..
// typedef struct {
//     unsigned char e_ident[EI_NIDENT];    //  ident bytes
//     Elf32_Half    e_type;                //  file type
//     Elf32_Half    e_machine;             //  target machine
//     Elf32_Word    e_version;             //  file version
//     Elf32_Addr    e_entry;               //  start address
//     Elf32_Off     e_phoff;               //  program header file offset
//     Elf32_Off     e_shoff;               //  section header file offset
//     Elf32_Word    e_flags;               //  file flags
//     Elf32_Half    e_ehsize;              //  sizeof ehdr
//     Elf32_Half    e_phentsize;           //  sizeof phdr
//     Elf32_Half    e_phnum;               //  number of program headers
//     Elf32_Half    e_shentsize;           //  sizeof section header
//     Elf32_Half    e_shnum;               //  number of section headers
//     Elf32_Half    e_shstrndx;            //  shdr string index
// } Elf32_Ehdr;
//..
// Each segment is described by a program header that is an array of
// structures, each describing a segment or other information the system needs
// to prepare the program for execution, and typically looks as follows:
//..
// typedef struct {
//     Elf32_Word p_type;                   //  entry type
//     Elf32_Off  p_offset;                 //  file offset
//     Elf32_Addr p_vaddr;                  //  virtual address
//     Elf32_Addr p_paddr;                  //  physical address
//     Elf32_Word p_filesz;                 //  file size
//     Elf32_Word p_memsz;                  //  memory size
//     Elf32_Word p_flags;                  //  entry flags
//     Elf32_Word p_align;                  //  memory/file alignment
// } Elf32_Phdr;
//..
// An object file segment contains one or more sections.  The string table
// provides the names of the various sections corresponding to the integral
// 'sh_name'.  An elf file will typically contain sections such as '.text',
// '.data', '.bss' etc.
//..
// typedef struct {
//     Elf32_Word sh_name;                  //  section name
//     Elf32_Word sh_type;                  //  SHT_...
//     Elf32_Word sh_flags;                 //  SHF_...
//     Elf32_Addr sh_addr;                  //  virtual address
//     Elf32_Off  sh_offset;                //  file offset
//     Elf32_Word sh_size;                  //  section size
//     Elf32_Word sh_link;                  //  misc info
//     Elf32_Word sh_info;                  //  misc info
//     Elf32_Word sh_addralign;             //  memory alignment
//     Elf32_Word sh_entsize;               //  entry size if table
// } Elf32_Shdr;
//
// typedef struct
// {
//     Elf32_Word    st_name;               //  Symbol name (string tbl index)
//     Elf32_Addr    st_value;              //  Symbol value
//     Elf32_Word    st_size;               //  Symbol size
//     unsigned char st_info;               //  Symbol type and binding
//     unsigned char st_other;              //  Symbol visibility
//     Elf32_Section st_shndx;              //  Section index - 16-bit
// } Elf32_Sym;
//..
// Below we explain the strategies to resolve symbols on the various platforms
// that we support.
//
// Solaris:
// ----------------------------------------------------------------------------
//
// The _DYNAMIC symbol references a _dynamic structure that refers to the
// linked symbols:
//..
// typedef struct {
//     Elf32_Sword d_tag;                   //  how to interpret value
//     union {
//         Elf32_Word d_val;
//         Elf32_Addr d_ptr;
//         Elf32_Off  d_off;
//     } d_un;
// } Elf32_Dyn;
//..
// Tag values
//..
// #define    DT_NULL      0                //  last entry in list
// #define    DT_DEBUG    21                //  pointer to r_debug structure
//
// struct r_debug {
//     int            r_version;            //  debugging info version no.
//     Link_map      *r_map;                //  address of link_map
//     unsigned long  r_brk;                //  address of update routine
//     r_state_e      r_state;
//     unsigned long  r_ldbase;             //  base addr of ld.so
//     Link_map      *r_ldsomap;            //  address of ld.so.1's link map
//     rd_event_e     r_rdevent;            //  debug event
//     rd_flags_e     r_flags;              //  misc flags.
// };
//..
// The link_map is a chain of loaded object.
//..
// struct link_map {
//
//     unsigned long  l_addr;               // address at which object is
//                                          // mapped
//     char          *l_name;               //  full name of loaded object
// #ifdef _LP64
//     Elf64_Dyn     *l_ld;                 //  dynamic structure of object
// #else
//     Elf32_Dyn     *l_ld;                 //  dynamic structure of object
// #endif
//     Link_map     *l_next;                //  next link object
//     Link_map     *l_prev;                //  previous link object
//     char         *l_refname;             //  filters reference name
// };
//..
// Linux:
// ----------------------------------------------------------------------------
//..
// int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info,
//                                      size_t               size,
//                                      void                *data),
//                     void *data);
//     // Walk through the list of an application's shared objects and invoke
//     // the specified 'callback' (taking the specified 'info' object of
//     // the specified 'size' and specifying the user supplied 'data') using
//     // the specified 'data' to be passed to 'callback'.
//
// struct dl_phdr_info
// {
//     ElfW(Addr)        dlpi_addr;
//     const char       *dlpi_name;
//     const ElfW(Phdr) *dlpi_phdr;
//     ElfW(Half)        dlpi_phnum;
//
//     // Note: the next two members were introduced after the first
//     // version of this structure was available.  Check the SIZE
//     // argument passed to the dl_iterate_phdr() callback to determine
//     // whether or not they are provided.
//
//     //  Incremented when a new object may have been added.
//     unsigned long long int dlpi_adds;
//
//     //  Incremented when an object may have been removed.
//     unsigned long long int dlpi_subs;
// };
//..
// HPUX:
// ----------------------------------------------------------------------------
//..
// struct shl_descriptor {
//     unsigned long  tstart;                   // start address of the shared
//                                              // library text segment
//
//     unsigned long  tend;                     // end address of the shared
//                                              // library text segment
//
//     unsigned long  dstart;
//     unsigned long  dend;
//     void          *ltptr;
//     shl_t          handle;
//     char           filename[MAXPATHLEN + 1];
//     void          *initializer;
//     unsigned long  ref_count;
//     unsigned long  reserved3;
//     unsigned long  reserved2;
//     unsigned long  reserved1;
//     unsigned long  reserved0;
// };
//
// int shl_get_r(int index, struct shl_descriptor *desc);
//     // Load into the specified 'desc' information about the loaded library
//     // at the specified 'index'.  For libraries loaded implicitly (at
//     // startup time), 'index' is the ordinal number of the library as it
//     // appeared on the command line.  Return 0 on success and a non-zero
//     // value otherwise.  Note that an 'index' value of 0 refers to the main
//     // program itself and -1 refers to the dynamic loader.
//..

// ----------------------------------------------------------------------------
// Bill's:
//
// Note that ELF and DWARF are separate formats.  ELF has a certain amount of
// information, including library file names, symbol names, and basenames of
// source file names for static symbols.  Dwarf contains full path source file
// names for all symbols and line number information.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ELF IMPLEMENTATION NOTES:
//
// The following 'struct' definitions describing the Elf format are modified
// from those found in the .h files (mostly 'elf.h').  The following
// transformations have been done on the definitions found in the .h file --
// typedefs to fundamental types have been resolved (some that are equivalent
// to 'bsls::Types::UintPtr' have been translated to 'UintPtr') and the names
// given for the structs are the names of typedefs to them in the namespace
// 'local' within this file.  Significantly, data members not used in this
// source file are ommitted.
//
// Each ELF object file or executable starts with an ELF header that specifies
// how many segments are provided in the file.  The ELF header looks as
// follows:
//..
// typedef struct {
//     unsigned char e_ident[EI_NIDENT];    //  ident bytes
//     UintPtr       e_phoff;               //  program header file offset
//     UintPtr       e_shoff;               //  section header file offset
//     short         e_shentsize;           //  sizeof section header
//     short         e_shnum;               //  number of section headers
//     short         e_shstrndx;            //  shdr string index
// } local::ElfHeader;
//..
// Each segment is described by a program header that is an array of
// structures, each describing a segment or other information the system needs
// to prepare the program for execution, and typically looks as follows:
//..
// typedef struct {
//     unsigned int p_type;                   //  entry type
//     UintPtr      p_offset;                 //  file offset
//     unsigned int p_vaddr;                  //  virtual address
//     unsigned int p_memsz;                  //  memory size
// } local::ElfProgramHeader;
//..
// An object file segment contains one or more sections.  The string table
// provides the names of the various sections corresponding to the integral
// 'sh_name'.  An elf file will typically contain sections such as '.text',
// '.data', '.bss' etc.
//..
// typedef struct {
//     unsigned int sh_name;                  //  section name
//     unsigned int sh_type;                  //  SHT_...
//     UintPtr      sh_offset;                //  file offset
//     unsigned int sh_size;                  //  section size
// } local::ElfSectionHeader;
//
// typedef struct
// {
//     unsigned int  st_name;               //  Symbol name (string tbl index)
//     UintPtr       st_value;              //  Symbol value
//     unsigned int  st_size;               //  Symbol size
//     unsigned char st_info;               //  Symbol type and binding
// } local::ElfSymbol;
//..
// ----------------------------------------------------------------------------
// The above definitions describe the data within one file.  However, if the
// executable is dynamically linked, that usually being the case, multiple
// files must be traversed.  Unfortunately, no one strategy for traversing the
// files works for more than one platform -- so for the 3 platform currently
// supported, Solaris, Linux, and HPUX, we have a 3 custom strategies.
//
// Solaris:
// ----------------------------------------------------------------------------
//
// The link_map is a node in a chain, each representing a loaded object.
//..
// struct link_map {
//     unsigned long        l_addr;         // address at which object is
//                                          // mapped
//     char                *l_name;         // full name of loaded object
//     local::ElfDynamic   *l_ld;           // dynamic structure of object
//     Link_map            *l_next;         // next link object
// };
//
// struct r_debug {
//     link_map      *r_map;                //  address of link_map
// };
//
// typedef struct {
//     int             d_tag;                   //  how to interpret value
//     union {
//         // Note other interpretations of this union are no used, so they
//         // are omitted here.
//
//         UintPtr     d_ptr;           // really a pointer of type 'r_debug *'
//     } d_un;
// } local::ElfDynamic;
//
// Tag values
//
// #define    DT_NULL      0                //  last entry in list
// #define    DT_DEBUG    21                //  pointer to 'r_debug' structure
//..
// The '_DYNAMIC' symbol is the address of the beginning of an array of objects
// of type 'local::ElfDynamic', one of which contains a pointer to the
// 'r_debug' object, which contains a pointer to the linked list of 'link_map'
// objects, one of which exists for each executable or shared library.
//
// Linux:
// ----------------------------------------------------------------------------
//..
// int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info,
//                                      size_t               size,
//                                      void                *data),
//     // Walk through the list of an application's shared objects and invoke
//     // the specified 'callback' (taking the specified 'info' object of
//     // the specified 'size' and specifying the user supplied 'data') using
//     // the specified 'data' to be passed to 'callback'.
//
// struct dl_phdr_info
// {
//     UintPtr                           dlpi_addr;    // base address
//     const char                       *dlpi_name;    // lib name
//     const local::ElfProgramHeader    *dlpi_phdr;    // array of program
//                                                     // headers
//     short                             dlpi_phnum;   // base address
// };
//..
// HPUX:
// ----------------------------------------------------------------------------
//..
// struct shl_descriptor {
//     unsigned long  tstart;                   // start address of the shared
//                                              // library text segment
//
//     unsigned long  tend;                     // end address of the shared
//                                              // library text segment
//
//     char           filename[MAXPATHLEN + 1];
// };
//
// int shl_get_r(int index, struct shl_descriptor *desc);
//     // Load into the specified 'desc' information about the loaded library
//     // at the specified 'index'.  For libraries loaded implicitly (at
//     // startup time), 'index' is the ordinal number of the library as it
//     // appeared on the command line.  Return 0 on success and a non-zero
//     // value otherwise.  Note that an 'index' value of 0 refers to the main
//     // program itself and -1 refers to the dynamic loader.
//..
// DWARF: Bill's Notes
// ----------------------------------------------------------------------------
// The DWARF document is at: http://dwarfstd.org/doc/Dwarf3.pdf
// and various enums are provided in the file '/usr/include/dwarf.h'.  Note
// that the include file was not up to date with the pdf or with the data
// encountered in the executables.  Thus some symbols had to be added below
// in the form 'local::e_DW_*' because the corresponding 'DW_*' was described
// by the pdf and was needed, but was not in the include file.
//
// In general, the spec is 255 pages long and not very well organized.  It
// sometimes only vaguely implies necessary assumptions, in other cases it
// utterly fails to mention things (see 'mysteryZero' below).  In addition, it
// sometimes describes multiple ways to encode information, only one of which
// was encountered in the test cases, so we have had to code for multiple
// possibilities yet are only able to test one of them.  Given this, we face a
// lot of uncertainty when decoding DWARF information.  We are only reading
// DWARF information to get line numbers and source file names.  ELF gives
// source file names, but only in the instance of static identifiers, and then
// gives only the base name, while DWARF gives the full path of all source file
// names.  Unfortunately, with inline functions, even when great care has been
// taken to ensure they are not called inline, DWARF erroneously gives the
// source file name as the name of the file from which they are called.
//
// So the DWARF code was written on the assumption that we already have a
// pretty good stack trace based on ELF information, and if, while decoding
// DWARF information, we encounter something unexpected or strange, we should
// just abandon decoding the DWARF inforation (or decoding a subset of the
// DWARF information) and continue delivering the rest of the information we
// have.  A core dump would be highly inappropriate.  We use 'ASSERT_BAIL' to
// check things, which just returns a failure code if the check fails, and
// depending on 'TRACES', may print out a message and possibly exit.  'TRACES'
// will always be 0 in production, in which case 'ASSERT_BAIL' will just return
// -1 without printing any error messages or exiting.
//..
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The DWARF information is in 6 sections:
//
// .debug_abbrev:      Contains sequences of enum's dictating how the
//                     information in the .debug_info section is to be
//                     interpreted
// .debug_aranges:     Specifies addresses ranges, and for each range, the
//                     offset of the compilation unit for addresss within those
//                     ranges in the .debug_info section.
// .debug_info:        Various information about the compilation, including the
//                     source file directory and name and the offset in the
//                     .debug_line section of the line number information.
// .debug_line:        Line number information.
// .debug_ranges:      When one compilation unit (described in the .debug_info
//                     and .debug_abbrev sections) applies to multiple address
//                     ranges, they refer to a sequence of ranges in the
//                     .debug_ranges section.  This section is not traversed
//                     unless 'BALST_DWARF_CHECK_ADDRESSES' is set.
// .debug_str          Section where null-terminated strings are stored,
//                     referred to by offsets that reside in the .debug_info
//                     section that are the offset of the beginning of a
//                     string from the beginning of the .debug_str section.
//..
// We start with the .debug_aranges section, which contains address ranges to
// be compared with stack pointers.  If a stack pointer matches a range, the
// range indicates an offset in the .debug_info section where the compilation
// unit's information is contained.
//
// The graphic "Figure 48" from the DWARF doc, page 188, in Appendix D.1, was
// particularly enlightening about how to read the description of a compilation
// unit in the .debug_info and .debug_abbrev sections.  Things were illustrated
// there that had not been spelt out formally in this doc.
//
// The compilation unit section of the .debug_info starts out with a header
// that indicates the offset of the information in the .debug_abbrev section
// corresponding to the compilation unit.
//
// The .debug_abbrev section contains pairs of enums that indicate how the
// information in the .debug_info that follows the compilation unit header is
// to be interpreted.  The .debug_abbrev information for the compile unit will
// start with a 'tag' which should be 'DW_TAG_compilation_unit' or
// 'DW_TAG_partial_unit', followed by a boolean byte to indicate whether the
// tag has any children.  We are not interested in the children byte.
//
// Following the '(tag, hasChildren)' pair is a null-terminated sequence of
// '(attr, form)' pairs where the 'attr' is an enum of type 'DW_AT_*' and the
// form is an enum of the type 'DW_FORM_*'.  There are many 'DW_AT_*'s defined,
// but only a few of them will appear in the compile unit section.  The 'attr'
// specifies the attribute, which is the kind of information being stored, and
// the 'form' specifies the way in which the information is stored.  For
// example, an 'attr' or 'DW_AT_name' indicates that the information is a
// string indicating the base name of the source file.  In that case, the form
// can be of two types -- a 'DW_FORM_string', in which case the name is a null
// terminated string store in the .debug_info section, or a 'DW_FORM_strp',
// which indicates the .debug_info section contains a section offset into
// the .debug_str section where the null terminated string resides.
//
// One piece of information we are interested in is the offset in .debug_line
// where our line number information is located.  This will be found where
// 'DW_AT_stmt_list == attr', and we use 'form' to interpret how to read that
// offset.

static inline
long long ll(long long value)
    // Coerce the specied 'value' to a long long.
{
    return value;
}

static inline
long l(long value)
    // Coerce the specified 'value' to a long.
{
    return value;
}

namespace BloombergLP {

namespace {

enum { e_TRACES = TRACES };

namespace local {

typedef bdls::FilesystemUtil::Offset Offset;
typedef bsls::Types::Uint64          Uint64;
typedef bsls::Types::UintPtr         UintPtr;
typedef bsls::Types::IntPtr          IntPtr;

static const Offset  maxOffset = LLONG_MAX;
static const UintPtr minusOne  = ~static_cast<UintPtr>(0);

BSLMF_ASSERT(sizeof(Uint64) == sizeof(Offset));
BSLMF_ASSERT(sizeof(UintPtr) == sizeof(void *));
BSLMF_ASSERT(static_cast<Offset>(-1) < 0);
BSLMF_ASSERT(maxOffset > 0);
BSLMF_ASSERT(maxOffset > INT_MAX);
BSLMF_ASSERT(static_cast<Offset>(static_cast<Uint64>(maxOffset) + 1) < 0);

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Elf>
                                                            StackTraceResolver;

                            // --------------------------
                            // Run-Time Platform Switches
                            // --------------------------

#if defined(BSLS_PLATFORM_OS_LINUX)
enum { e_IS_LINUX = 1 };
#else
enum { e_IS_LINUX = 0 };
#endif

#if   defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
enum { e_IS_BIG_ENDIAN = 1,
       e_IS_LITTLE_ENDIAN = 0 };
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
enum { e_IS_BIG_ENDIAN = 0,
       e_IS_LITTLE_ENDIAN = 1 };
#else
# error endianness is undefined
#endif

                                   // ---------
                                   // Constants
                                   // ---------

enum {
    k_SCRATCH_BUF_LEN = 32 * 1024 - 64     // length in bytes of
                                           // d_scratchBuf?_p, 32K minus a
                                           // little so we don't waste a page
};

                                // ---------------
                                // local Elf Types
                                // ---------------

#undef   SPLICE
#ifdef BSLS_PLATFORM_CPU_64_BIT
# define SPLICE(suffix)  Elf64_ ## suffix
#else
# define SPLICE(suffix)  Elf32_ ## suffix
#endif

// The following types are part of the ELF standard, and describe structs that
// occur in the executable file / shared libraries.

typedef SPLICE(Dyn)     ElfDynamic;        // The expression '&_DYNAMIC' is a
                                           // 'void *' pointer to a an array of
                                           // 'struct's of this type, used to
                                           // find the link map on Solaris.

typedef SPLICE(Ehdr)    ElfHeader;         // The elf header is a standard
                                           // header at the start of any ELF
                                           // file

typedef SPLICE(Phdr)    ElfProgramHeader;  // Program headers are obtained from
                                           // the link map.  We use them to
                                           // find code segments.

typedef SPLICE(Shdr)    ElfSectionHeader;  // Section headers are located from
                                           // the U::ElfHeader, they tell us
                                           // where the sections containing
                                           // symbols and strings are

typedef SPLICE(Sym)     ElfSymbol;         // Describes one symbol in the
                                           // symbol table.
#undef SPLICE

struct Section {
    // Refers to one section of a segment.

    // DATA
    Offset d_offset;    // offset of the section in the file
    Offset d_size;      // size of that section in bytes

    // CREATOR
    Section()
    : d_offset(0)
    , d_size(0)
        // Create a zero-value 'Section' object.
    {
    }

    // MANIPULATOR
    void reset(Offset offset = 0, Offset size = 0)
        // Reset this 'Section' object to have the specified 'offset' and the
        // specified 'size'.
    {
        if (offset < 0 || size < 0) {
            e_TRACES && eprintf("reset: negative offset or size\n");
            offset = 0;
            size   = 0;
        }

        d_offset = offset;
        d_size   = size;
    }
};

static int cleanupString(bsl::string *str, bslma::Allocator *alloc)
    // Eliminate all instances of "/./" from the specified '*str', and also as
    // many instances of "/../" as possible.  Do not resolve symlinks.  Use the
    // specified 'alloc' for memory allocation of temporaries.  Return 0 unless
    // "*str" does not represent an acceptable full path for a source file, and
    // a non-zero value otherwise.
{
    static const char rn[] = { "cleanupString:" };

    const bsl::size_t npos = bsl::string::npos;

    if (str->empty() || '/' != (*str)[0]) {
        return -1;                                                    // RETURN
    }

    // First eliminate all "/./" sequences.

    bsl::size_t pos;
    while (npos != (pos = str->find("/./"))) {
        str->erase(pos, 2);
    }

    bsl::string preDir(alloc);

    // Next, eliminate as many instances of  "/../" as possible.

    while (npos != (pos = str->find("/../"))) {
        // zprintf("%s found '/../' in %s\n", rn, str->c_str());

        ASSERT_BAIL(pos >= 1);

        // Abandon cleanup unless the path prior to '/../' is not a symlink and
        // is a directory.  Note that if the path is "/a/b/c/../d.cpp" and "c"
        // is a symlink to "/e/f", the correct cleanup would be "/e/d.cpp" and
        // "/a/b/d.cpp" would be completely wrong.  By default, stack traces
        // only display the leaf name of the path, so it's not worth the effort
        // to dereference symlinks, just clean up in the obvious case when
        // we're sure we're not introducing inaccuracies.

        preDir.assign(str->begin(), str->begin() + pos);
        struct stat s;
        int rc = ::lstat(preDir.c_str(), &s);
        if (0 != rc || (s.st_mode & S_IFLNK) || !S_ISDIR(s.st_mode)) {
            // It's possible the file is not there or is not a directory, which
            // could mean that we are not running on the machine the code was
            // compiled on, which will frequently happen and is not worth
            // returning an error code over.

            e_TRACES && zprintf(
                          "%s difficulty cleaning up, but not an error\n", rn);

            return 0;                                                 // RETURN
        }

        bsl::size_t rpos = str->rfind('/', pos - 1);
        ASSERT_BAIL(npos != rpos && ".. below root directory");
        ASSERT_BAIL(rpos < pos - 1 && '/' == (*str)[rpos]);

        str->erase(rpos, pos + 3 - rpos);

        // zprintf("%s optimized to %s\n", rn, str->c_str());
    }

    return 0;
}

#ifdef BALST_DWARF
enum {
    // DWARF 4 flags not necessarily defined in our dwarf.h.

    e_DW_TAG_type_unit             = 0x41,
    e_DW_TAG_rvalue_reference_type = 0x42,
    e_DW_TAG_template_alias        = 0x43,

    e_DW_AT_main_subprogram        = 0x6a,

    e_DW_FORM_sec_offset           = 0x17,
    e_DW_FORM_exprloc              = 0x18,
    e_DW_FORM_flag_present         = 0x19,
    e_DW_FORM_ref_sig8             = 0x20
};

class Reader {
    // DATA
    balst::StackTraceResolver_FileHelper
                                   *d_helper_p;      // filehelper for currentt
                                                     // segment
    char                           *d_buffer_p;      // buffer.
                                                     // k_SCRATCH_BUF_LEN long
    bdls::FilesystemUtil::Offset    d_offset;        // offset last read from
    bdls::FilesystemUtil::Offset    d_beginOffset;   // beg of current section
    bdls::FilesystemUtil::Offset    d_endOffset;     // end of current section
    const char                     *d_readPtr;       // current place to read
                                                     // from
    const char                     *d_endPtr;        // end of what's in buffer
    int                             d_offsetSize;    // offset size determined
                                                     // by 'readInitalLength'
    int                             d_addressSize;   // address read by
                                                     // 'getAddress' or set by
                                                     // 'setAddressSize'.


  private:
    // NOT IMPLEMENTED
    Reader(const Reader&);
    Reader& operator=(const Reader&);

  private:
    // PRIVATE MANIPULATORS
    int needBytes(bsl::size_t numBytes);
        // Determine if we are able to read the specified 'numBytes' from
        // 'd_buffer', and call 'reload' to reload the butter if necessary,
        // and return an error if it 'reload' fails.  Return 0 on success and
        // a non-zero value otherwise.

    int reload(bsl::size_t numBytes);
        // Reload the buffer to accomodate a read of at least the specified
        // 'numBytes'.  If possible, read up to the end of the section or the
        // size of the buffer, whichever is shorter.  Return 0 on success, and
        // a non-zero value otherwise.

  public:
    // CREATOR
    Reader();
        // Create a 'Reader' object in a null state.

    // MANIPULATORS
    void disable();
        // Disable this object for further use.

    int init(balst::StackTraceResolver_FileHelper *fileHelper,
             char                                 *buffer,
             const Section&                        section,
             Offset                                libraryFileSize);
        // Initialize this 'Reader' object using the specified 'fileHelper' and
        // the specified 'buffer', to operate on the specified 'section', where
        // the specified 'libraryFileSize is the size of the library or
        // executable file.

    int readAddress(UintPtr *dst);
        // Read to the specified 'dst'.  This function will fail if
        // 'd_addressZie' has not been initialized by 'readAddressSize' or
        // 'setAddressSize'.  Return 0 on success and a non-zero value
        // otherwise.

    int readAddress(UintPtr *dst, unsigned form);
        // Read to the specified 'dst' according to the specified 'form'.
        // Return 0 on success and a non-zero value otherwise.

    int readAddressSize();
        // Read the address size from a single unsigned byte, check it, and
        // assign 'd_addressSize' to it.  Return 0 on success and a non-zero
        // value otherwise.  It is an error if address size is not equal to the
        // size of an unsigned int or of a 'void *'.

    int readInitialLength(Offset *dst);
        // Read the initial length of the object according to the DWARF doc to
        // the specified '*dst'.  Read it first as a 4 byte value, if the value
        // is below 0xfffffff0, then that means section offsets are to be read
        // as 4 byte values from now on.  If the value is 0xffffffff, read the
        // next 8 bytes into '*dst' and that indicates section offsets are to
        // be 8 bytes from now on.  Initialize 'd_offsetSize' accordingly.
        // Values in the range '[0xfffffff0, 0xffffffff)' are illegal for that
        // first 4 bytes.  Return 0 on success and a non-zero value otherwise.

    template <class TYPE>
    int readLEB128(TYPE *dst);
        // Read a signed variable-length number into the specified '*dst'.
        // Return 0 on success and a non-zero value otherwise.

    template <class TYPE>
    int readULEB128(TYPE *dst);
        // Read an unsigned variable-length number into the specified '*dst'.
        // Return 0 on success and a non-zero value otherwise.

    int readOffset(Offset      *dst,
                   bsl::size_t  offsetSize);
        // Read to the specified '*dst', where the specified 'offsetSize' is
        // the number of bytes to store the offset.  Return 0 on success and a
        // non-zero value otherwise.

    int readOffsetFromForm(Offset   *dst,
                           unsigned  form);
        // Read to the specified '*dst' according to the specified 'form',
        // where 'form' is a DWARF enum of the 'DW_FORM_*' category.  Return 0
        // on success and a non-zero value otherwise.

    int readSectionOffset(Offset *dst);
        // Read to the specified offset '*dst' according to 'd_offsetSize'.
        // Return 0 on success and a non-zero value otherwise.

    int readString(bsl::string *dst = 0);
        // Read a null-terminated string to the specified '*dst'.  If no 'dst'
        // is specified, skip over the string without copying it anywhere.
        // This function will fail if the string length is greater than
        // 'k_SCRATCH_BUFFER_LEN - 1'.

    int readStringAt(bsl::string *dst, Offset offset);
        // Read a null terminated string to the specified '*dst' from the
        // specified 'offset' plus 'd_beginOffset'.  Note that, unlike most of
        // the other 'read' functions, this one is intended for random access
        // so does not read a full buffer ahead, instead reading a fairly
        // minimal amount of data near the specified location.

    int readStringFromForm(bsl::string *dst,
                           Reader *stringReader,
                           unsigned form);
        // Read to the specified string either from the current reader (if the
        // specified 'form' is 'DW_AT_string') or read an offset from the
        // current reader, then use that to read the string from the specified
        // '*stringReader' (if 'form' is 'DW_AT_strp').  Return 0 on success
        // and a non-zero value otherwise.

    template <class TYPE>
    int readValue(TYPE *dst);
        // Read a value into the specified '*dst', assuming that it is
        // represented by sizeof(*dst) bytes.

    void setAddressSize(const Reader& other)
        // Copy the 'd_addressSize' of this reader from the specified reader
        // 'other'.
    {
        BSLS_ASSERT_SAFE(0 < other.d_addressSize);

        d_addressSize = other.d_addressSize;
    }

    int skipBytes(Offset bytes);
        // Skip forward over the specified 'bytes' without reading them.

    int skipString();
        // Skip over a null terminated string without copying it anywhere.

    int skipForm(unsigned form);
        // Skip over data according to the specified 'form', which is an enum
        // of type 'DW_FORM_*' or 'e_DW_FORM_*'.

    int skipTo(Offset offset);
        // Skip to the specified 'offset', which must be in the section
        // associated with this reader.  Return 0 on success and a non-zero
        // value otherwise.

    // ACCESSORS
    int addressSize() const;
        // Return the address size field.

    bool atEndOfSection() const;
        // Return 'true' if the reader has reached the end of the section and
        // 'false' otherwise.

    Offset offset() const;
        // Return the current offset taking the 'd_reader' position into
        // account.
};

// PRIVATE MANIPULATORS
inline
int Reader::needBytes(bsl::size_t numBytes)
{
    static const char rn[] = { "Reader::needBytes:" };    (void) rn;

    IntPtr diff = d_endPtr - d_readPtr;

    if (diff < static_cast<IntPtr>(numBytes)) {
        BSLS_ASSERT_SAFE(0 <= diff);

        return reload(numBytes);                                      // RETURN
    }

    return 0;
}

int Reader::reload(bsl::size_t numBytes)
{
    static const char rn[] = { "Reader::reload:" };    (void) rn;

    BSLS_ASSERT_SAFE(d_buffer_p);
    ASSERT_BAIL_SAFE(numBytes <= k_SCRATCH_BUF_LEN);
    ASSERT_BAIL_SAFE(static_cast<Offset>(numBytes) <= d_endOffset - offset());
    ASSERT_BAIL_SAFE(d_readPtr <= d_endPtr);

    d_offset += d_readPtr - d_buffer_p;
    ASSERT_BAIL_SAFE(d_offset <= d_endOffset);

    unsigned lengthToRead = static_cast<unsigned>(
              bsl::min<Offset>(d_endOffset - d_offset, k_SCRATCH_BUF_LEN));
    ASSERT_BAIL(lengthToRead >= numBytes || P(lengthToRead) ||
                                                              P(numBytes));
    d_helper_p->readExact(d_buffer_p, lengthToRead, d_offset);

    d_readPtr = d_buffer_p;
    d_endPtr  = d_readPtr + lengthToRead;

    return 0;
}

// CREATOR
Reader::Reader()
{
    disable();
}

// MANIPULATORS
void Reader::disable()
{
    d_helper_p    = 0;
    d_buffer_p    = 0;
    d_offset      = local::minusOne;
    d_beginOffset = local::minusOne;
    d_endOffset   = local::minusOne;
    d_readPtr     = 0;
    d_endPtr      = 0;
    d_offsetSize  = -1;
    d_addressSize = -1;
}

int Reader::init(balst::StackTraceResolver_FileHelper *fileHelper,
                 char                                 *buffer,
                 const Section&                        section,
                 Offset                                libraryFileSize)
{
    static const char rn[] = { "Reader::init:" };    (void) rn;

    ASSERT_BAIL_SAFE(buffer);
    ASSERT_BAIL(section.d_offset >= 0);
    ASSERT_BAIL(section.d_size   > 0);
    ASSERT_BAIL(0 < section.d_offset + section.d_size);
    ASSERT_BAIL(    section.d_offset + section.d_size <= libraryFileSize);

    d_helper_p    = fileHelper;
    d_buffer_p    = buffer;
    d_offset      = section.d_offset;
    d_beginOffset = section.d_offset;
    d_endOffset   = section.d_offset + section.d_size;
    d_readPtr     = buffer;
    d_endPtr      = buffer;
    d_offsetSize  = -1;
    d_addressSize = -1;

    return 0;
}

inline
int Reader::readAddress(UintPtr *dst)
{
    static const char rn[] = { "Reader::readAddress:" };    (void) rn;

    int rc;

    if (sizeof(unsigned) != sizeof(UintPtr) &&
                                       sizeof(unsigned) == d_addressSize) {
        unsigned u;
        rc = readValue(&u);
        *dst = u;
    }
    else {
        ASSERT_BAIL_SAFE(sizeof(UintPtr) == d_addressSize);

        rc = readValue(dst);
    }

    ASSERT_BAIL(0 == rc && "readValue failed");
    return 0;
}

int Reader::readAddress(UintPtr *dst, unsigned form)
{
    static const char rn[] = { "Reader::readAddress:" };    (void) rn;

    int rc;

    unsigned addressSize;
    switch (form) {
      case DW_FORM_addr: {
        ASSERT_BAIL_SAFE(d_addressSize > 0);
        addressSize = d_addressSize;
      } break;
      case DW_FORM_data1: { addressSize = 1; } break;
      case DW_FORM_data2: { addressSize = 2; } break;
      case DW_FORM_data4: { addressSize = 4; } break;
      case DW_FORM_data8: {
        ASSERT_BAIL(8 == sizeof(*dst));
        addressSize = 8;
      } break;
      default: {
        ASSERT_BAIL((0 && "unrecognized form") || PH(form));
      } break;
    }

    rc = needBytes(addressSize);
    ASSERT_BAIL(0 == rc && "needBytes failed");

    *dst = 0;
    char *dstPtr = reinterpret_cast<char *>(dst);
    dstPtr += e_IS_BIG_ENDIAN ? sizeof(*dst) - addressSize : 0;
    bsl::memcpy(dstPtr, d_readPtr, addressSize);
    d_readPtr += addressSize;

    return 0;
}

int Reader::readAddressSize()
{
    static const char rn[] = { "Reader::readAddressSize:" };    (void) rn;

    unsigned char u;
    int rc = readValue(&u);
    ASSERT_BAIL(0 == rc && "read address size failed");

    if (sizeof(unsigned) == sizeof(UintPtr)) {
        ASSERT_BAIL(sizeof(unsigned) == u || P(u));
    }
    else {
        ASSERT_BAIL(sizeof(UintPtr) == u || sizeof(unsigned) == u || P(u));
    }
    d_addressSize = u;

    // zprintf("%s addressSize: %u\n", rn, u);

    return 0;
}

int Reader::readInitialLength(Offset *dst)
    // Read the initial length to '*dst', and set '*isShortInitialLength' to
    // reflect whether that initial length was short or long.
{
    static const char rn[] = { "Reader::readIniitialLength:" };    (void) rn;

    int rc = readOffset(dst, sizeof(unsigned));
    ASSERT_BAIL(0 == rc);

    if (0xffffffff == *dst) {
        rc = readValue(dst);
        ASSERT_BAIL(0 == rc);
        ASSERT_BAIL(0 <= *dst && "negative initial length");
        d_offsetSize = static_cast<int>(sizeof(*dst));
    }
    else {
        ASSERT_BAIL((*dst < 0xfffffff0 && "illegal preNum") || PH(*dst));
        d_offsetSize = static_cast<int>(sizeof(unsigned));
    }

    ASSERT_BAIL(offset() + *dst <= d_endOffset);

    return 0;
}

template <class TYPE>
int Reader::readLEB128(TYPE *dst)                              // DWARF doc 7.6
{
    static const char rn[] = { "Reader::readLEB128:" };    (void) rn;

    int rc;

    *dst = 0;

    unsigned char u = 0x80;

    enum { k_MAX_SHIFT = sizeof(*dst) * 8 };

    unsigned shift = 0;
    for (; (0x80 & u); shift += 7) {
        ASSERT_BAIL(shift < k_MAX_SHIFT);

        rc = readValue(&u);
        ASSERT_BAIL(0 == rc);

        *dst |= static_cast<TYPE>(u) << shift;
    }

    if (static_cast<TYPE>(-1) < 0) {
        // signed type, extend sign

        const TYPE negFlag = static_cast<TYPE>(0x40) << shift;
        if (negFlag & *dst) {
            *dst |= ~(negFlag - 1);
        }
    }

    return 0;
}

template <class TYPE>
int Reader::readULEB128(TYPE *dst)                             // DWARF doc 7.6
{
    static const char rn[] = { "Reader::readULEB128:" };    (void) rn;

    int rc;

    *dst = 0;

    unsigned char u = 0x80;

    enum { k_MAX_SHIFT = sizeof(*dst) * 8 };

    unsigned shift = 0;
    for (; (0x80 & u); shift += 7) {
        ASSERT_BAIL(shift < k_MAX_SHIFT);

        rc = readValue(&u);
        ASSERT_BAIL(0 == rc);

        *dst |= static_cast<TYPE>(u) << shift;
    }

    return 0;
}

int Reader::readOffset(Offset      *dst,
                       bsl::size_t  offsetSize)
{
    static const char rn[] = { "Reader::readOffset:" };    (void) rn;

    enum { k_SIZE_OF_OFFSET_TYPE = sizeof(Offset) };

    int rc;

    if (offsetSize <= sizeof(*dst)) {
        ASSERT_BAIL(offsetSize > 0);

        rc = needBytes(offsetSize);
        ASSERT_BAIL(0 == rc && "needBytes failed");

        *dst = 0;
        char *dstPtr = reinterpret_cast<char *>(dst);
        dstPtr += e_IS_BIG_ENDIAN ? k_SIZE_OF_OFFSET_TYPE - offsetSize : 0;
        bsl::memcpy(dstPtr, d_readPtr, offsetSize);
        d_readPtr += offsetSize;
        return 0;                                                     // RETURN
    }
    else if (0xffff == offsetSize) {
        return readULEB128(dst);                                      // RETURN
    }

    // error

    ASSERT_BAIL(0 || P(offsetSize));

    return -1;
}

int Reader::readOffsetFromForm(Offset   *dst,
                               unsigned  form)
{
    static const char rn[] = { "Reader::readOffsetFromForm:" };    (void) rn;

    int rc;

    switch (form) {
      case local::e_DW_FORM_flag_present: {
        ; // do nothing
      } break;
      case DW_FORM_flag:
      case DW_FORM_data1:
      case DW_FORM_ref1: {
        rc = readOffset(dst, 1);
        ASSERT_BAIL((0 == rc && "trouble reading 1 byte value") || PH(form));
      } break;
      case DW_FORM_data2:
      case DW_FORM_ref2: {
        rc = readOffset(dst, 2);
        ASSERT_BAIL((0 == rc && "trouble reading 2 byte value") || PH(form));
      } break;
      case DW_FORM_data4:
      case DW_FORM_ref4: {
        rc = readOffset(dst, 4);
        ASSERT_BAIL((0 == rc && "trouble reading 4 byte value") || PH(form));
      } break;
      case DW_FORM_data8:
      case DW_FORM_ref8: {
        rc = readOffset(dst, 8);
        ASSERT_BAIL((0 == rc && "trouble reading 8 byte value") || PH(form));
      } break;
      case e_DW_FORM_sec_offset: {
        rc = readSectionOffset(dst);
        ASSERT_BAIL(0 == rc && "failure to read section offset");
      } break;
      case DW_FORM_sdata: {
        rc = readLEB128(dst);
        ASSERT_BAIL(0 == rc && "failure to read sdata");
      } break;
      case DW_FORM_udata: {
        rc = readULEB128(dst);
        ASSERT_BAIL(0 == rc && "failure to read udata");
      } break;
      case DW_FORM_indirect: {
        unsigned iForm;
        rc = readULEB128(&iForm);
        ASSERT_BAIL((0 == rc && "trouble reading indirect form") || PH(iForm));
        ASSERT_BAIL(DW_FORM_indirect != iForm);
        rc = readOffsetFromForm(dst, iForm);
        ASSERT_BAIL((0 == rc && "trouble recursing on indirect offset") ||
                                                                    PH(iForm));
      } break;
      default: {
        ASSERT_BAIL((0 && "strange form ") || P(form));
      }
    }

    return 0;
}

int Reader::readSectionOffset(Offset *dst)
{
    static const char rn[] = { "Reader::readSectionOffset:" };    (void) rn;

    enum { k_SIZE_OF_OFFSET_TYPE   = sizeof(Offset),
           k_SIZE_OF_UNSIGNED_TYPE = sizeof(unsigned),
           k_DELTA = e_IS_BIG_ENDIAN
                     ? k_SIZE_OF_OFFSET_TYPE - k_SIZE_OF_UNSIGNED_TYPE
                     : 0
         };

    int rc;

    if (k_SIZE_OF_OFFSET_TYPE == d_offsetSize) {
        rc = readValue(dst);
        ASSERT_BAIL(0 == rc && "readValue of section offset failed");

        return 0;                                                     // RETURN
    }

    ASSERT_BAIL_SAFE(0 < d_offsetSize ||
                           "readSectionOffset with uninitialized offset size");
    ASSERT_BAIL(k_SIZE_OF_UNSIGNED_TYPE == d_offsetSize);

    rc = needBytes(k_SIZE_OF_UNSIGNED_TYPE);
    ASSERT_BAIL(0 == rc && "needBytes failed");

    *dst = 0;
    bsl::memcpy(reinterpret_cast<char *>(dst) + k_DELTA,
                d_readPtr,
                k_SIZE_OF_UNSIGNED_TYPE);
    d_readPtr += k_SIZE_OF_UNSIGNED_TYPE;

    return 0;
}

int Reader::readString(bsl::string *dst)
{
    static const char rn[] = { "Reader::readString:" };    (void) rn;
    int rc;

    const char *pc = d_readPtr;
    while (pc < d_endPtr && *pc) {
        ++pc;
    }

    if (pc >= d_endPtr) {
        ASSERT_BAIL(pc == d_endPtr);

        rc = needBytes(1 + (d_endPtr - d_readPtr));
        ASSERT_BAIL(0 == rc);

        pc = d_readPtr;
        while (pc < d_endPtr && *pc) {
            ++pc;
        }
    }

    if (pc >= d_endPtr) {
        ASSERT_BAIL(pc == d_endPtr);

        ASSERT_BAIL(pc - d_readPtr < local::k_SCRATCH_BUF_LEN &&
                                           "string longer than buffer length");
        ASSERT_BAIL(d_offset + (pc - d_readPtr) < d_endOffset &&
                                          "string longer than section length");
        ASSERT_BAIL(0 && "unknown error");
    }

    ASSERT_BAIL(!*pc && "unknown error");

    bsl::size_t len = pc - d_readPtr;
    if (dst) {
        dst->assign(d_readPtr, len);
    }
    d_readPtr += len + 1;

    return 0;
}

int Reader::readStringAt(bsl::string *dst, Offset offset)
{
    static const char rn[] = { "Reader::readStringAt:" };    (void) rn;

    ASSERT_BAIL(0 <= offset);
    offset += d_beginOffset;
    ASSERT_BAIL(0 <= offset);
    ASSERT_BAIL(offset < d_endOffset || PH(offset) || PH(d_endOffset));

    enum { k_START_LEN = 256 };

    bsl::size_t maxReadLen = static_cast<bsl::size_t>(
                               bsl::min<Offset>(d_endOffset - offset,
                                                local::k_SCRATCH_BUF_LEN - 1));

    bsl::size_t stringLen;
    for (bsl::size_t readLen = k_START_LEN; true; readLen *= 4) {
        if (readLen > maxReadLen) {
            readLen = maxReadLen;
        }

        bsl::size_t bytes = d_helper_p->readBytes(d_buffer_p, readLen, offset);
        ASSERT_BAIL(0 < bytes);

        BSLS_ASSERT(bytes <= readLen);
        d_buffer_p[bytes] = 0;
        stringLen = bsl::strlen(d_buffer_p);
        if (stringLen < bytes || bytes < readLen || maxReadLen == readLen) {
            break;
        }
    }

    dst->assign(d_buffer_p, stringLen);

    return 0;
}

int Reader::readStringFromForm(bsl::string *dst,
                               Reader      *stringReader,
                               unsigned     form)
{
    static const char rn[] = { "Reader::readString:" };    (void) rn;

    int rc;

    if      (DW_FORM_string == form) {
        rc = readString(dst);
        ASSERT_BAIL(0 == rc);
    }
    else if (DW_FORM_strp   == form) {
        Offset offset;
        rc = readSectionOffset(&offset);
        ASSERT_BAIL(0 == rc);
        rc = stringReader->readStringAt(dst, offset);
        ASSERT_BAIL(0 == rc);
    }
    else {
        ASSERT_BAIL((0 && "unrecognized baseName form") || PH(form));
    }

    return 0;
}

template <class TYPE>
inline
int Reader::readValue(TYPE *dst)
{
    static const char rn[] = { "Reader::readValue:" };    (void) rn;

    int rc = needBytes(sizeof(*dst));
    ASSERT_BAIL(0 == rc && "needBytes failed");

    bsl::memcpy(dst, d_readPtr, sizeof(*dst));
    d_readPtr += sizeof(*dst);

    return 0;
}

inline
int Reader::skipBytes(Offset bytes)
{
    static const char rn[] = { "Reader::skipBytes:" };    (void) rn;

    BSLS_ASSERT_SAFE(bytes >= 0);

    if (static_cast<IntPtr>(bytes) > d_endPtr - d_readPtr) {
        Offset off = offset();

        ASSERT_BAIL(off         >= d_beginOffset);
        ASSERT_BAIL(off + bytes <= d_endOffset);

        // By setting 'd_readPtr == d_endPtr' we guarantee that the next read
        // will trigger a reload.

        d_offset  += bytes + (d_readPtr - d_buffer_p);
        d_readPtr =  d_buffer_p;
        d_endPtr  =  d_readPtr;
    }
    else {
        d_readPtr += bytes;
    }

    return 0;
}

int Reader::skipForm(unsigned form)
{
    static const char rn[] = { "Reader::skipForm:" };    (void) rn;

    using namespace local;

    int    rc;
    Offset toSkip = -1;

    switch (form) {
      case e_DW_FORM_flag_present: {
        ;    // Do nothing
      } break;
      case DW_FORM_flag:
      case DW_FORM_data1:
      case DW_FORM_ref1: {
        toSkip = 1;
      } break;
      case DW_FORM_data2:
      case DW_FORM_ref2: {
        toSkip = 2;
      } break;
      case DW_FORM_data4:
      case DW_FORM_ref4: {
        toSkip = 4;
      } break;
      case DW_FORM_data8:
      case DW_FORM_ref8:
      case e_DW_FORM_ref_sig8: {
        toSkip = 8;
      } break;
      case DW_FORM_sdata:
      case DW_FORM_udata:
      case DW_FORM_ref_udata: {
        enum { k_MAX_SHIFT = sizeof(Offset) * 8 - 1 };
        unsigned shift = 0;
        do {
            ASSERT_BAIL(shift <= k_MAX_SHIFT || PH(form));
            shift += 7;

            rc = needBytes(1);
            ASSERT_BAIL(0 == rc || PH(form));
        } while (*d_readPtr++ | 0x80);
      } break;
      case DW_FORM_string: {
        do {
            rc = needBytes(1);
            ASSERT_BAIL(0 == rc);
        } while (*d_readPtr++);
      } break;
      case DW_FORM_strp:
      case e_DW_FORM_sec_offset: {
        ASSERT_BAIL_SAFE(d_offsetSize > 0);
        toSkip = d_offsetSize;
      } break;
      case DW_FORM_addr:
      case DW_FORM_ref_addr: {
        ASSERT_BAIL_SAFE(d_addressSize > 0);
        toSkip = d_addressSize;
      } break;
      case DW_FORM_indirect: {
        unsigned iForm;
        rc = readULEB128(&iForm);
        ASSERT_BAIL((0 == rc && "trouble skipping indirect form") ||
                                                                    PH(iForm));
        ASSERT_BAIL(DW_FORM_indirect != iForm);
        rc = skipForm(iForm);
        ASSERT_BAIL((0 == rc &&
                "trouble recursing skipping on indirect offset") || PH(iForm));
      } break;
      case DW_FORM_block1: {
        unsigned char len;
        rc = readValue(&len);
        ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block2: {
        unsigned short len;
        rc = readValue(&len);
        ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block4: {
        unsigned len;
        rc = readValue(&len);
        ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block:
      case e_DW_FORM_exprloc: {
        rc = readULEB128(&toSkip);
        ASSERT_BAIL(0 == rc);
      } break;
      default: {
        ASSERT_BAIL((0 && "unrecognized form") || PH(form));
      }
    }

    if (toSkip > 0) {
        // zprintf("%s skipping forward %lld\n", rn, ll(toSkip));

        rc = skipBytes(toSkip);
        ASSERT_BAIL(0 == rc);
    }
    else {
        // zprintf("%s not skipping\n", rn);

        ASSERT_BAIL(-1 == toSkip);
    }

    return 0;
}

inline
int Reader::skipString()
{
    static const char rn[] = { "Reader::skipString" };    (void) rn;

    do {
        int rc = needBytes(1);
        ASSERT_BAIL(0 == rc);
    } while (*d_readPtr++);

    return 0;
}

int Reader::skipTo(Offset dstOffset)
{
    static const char rn[] = { "Reader::skipTo:" };    (void) rn;

    Offset diff = dstOffset - this->offset();
    if (diff > d_endPtr - d_readPtr || diff < d_buffer_p - d_readPtr) {
        ASSERT_BAIL(dstOffset >= d_beginOffset);
        ASSERT_BAIL(dstOffset <= d_endOffset);

        // By setting 'd_readPtr == d_endPtr' we will automatically make the
        // buffer be refilled next time we read any data.

        d_offset  = dstOffset;
        d_readPtr = d_buffer_p;
        d_endPtr  = d_readPtr;
    }
    else {
        ASSERT_BAIL_SAFE(static_cast<IntPtr>(diff) == diff);

        d_readPtr += static_cast<IntPtr>(diff);
    }

    return 0;
}

// ACCESSORS
inline
int Reader::addressSize() const
{
    return d_addressSize;
}

inline
bool Reader::atEndOfSection() const
{
    return d_readPtr == d_endPtr &&
                              d_endOffset - d_offset == d_readPtr - d_buffer_p;
}

inline
Offset Reader::offset() const
{
    return d_offset + (d_readPtr - d_buffer_p);
}
#endif

                                    // --------
                                    // FrameRec
                                    // --------

class FrameRec {
    // A struct consisting of the things we want stored associated with a given
    // frame.  We put these into a vector and sort them for O(log n) lookup of
    // frames by address.  (The code was previously always doing an exhaustive
    // search of all the addresses).

    // DATA
    const void             *d_address;
    balst::StackTraceFrame *d_frame_p;
#ifdef BALST_DWARF
    Offset                  d_compileUnitOffset;
    Offset                  d_lineNumberOffset;
    bsl::string             d_compileUnitDir;
    bsl::string             d_compileUnitFileName;
#endif
    bool                    d_isSymbolResolved;

  public:
    BSLALG_DECLARE_NESTED_TRAITS(FrameRec,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    FrameRec(const void             *address,
             balst::StackTraceFrame *stackTraceFrame,
             bslma::Allocator       *allocator)
    : d_address(address)
    , d_frame_p(stackTraceFrame)
#ifdef BALST_DWARF
    , d_compileUnitOffset(maxOffset)
    , d_lineNumberOffset( maxOffset)
    , d_compileUnitDir(allocator)
    , d_compileUnitFileName(allocator)
#endif
    , d_isSymbolResolved(false)
        // Create a 'FrameRec' referring to the specified 'address' and the
        // specified 'framePtr'.
    {
    }

    FrameRec(const FrameRec&   original,
             bslma::Allocator *allocator)
    : d_address(original.d_address)
    , d_frame_p(original.d_frame_p)
#ifdef BALST_DWARF
    , d_compileUnitOffset(original.d_compileUnitOffset)
    , d_lineNumberOffset( original.d_lineNumberOffset)
    , d_compileUnitDir(     original.d_compileUnitDir,      allocator)
    , d_compileUnitFileName(original.d_compileUnitFileName, allocator)
#endif
    , d_isSymbolResolved(original.d_isSymbolResolved)
        // Create a 'FrameRec' that uses the specified 'allocator' and is a
        // copy of the specified 'original'.
    {
    }

    // ~FrameRec() = default;

    // MANIPULATORS
    // inline operator=(const FrameRec&) = default;

    void setAddress(const void *value)
        // Set tthe 'address' field to the specified 'value'.
    {
        d_address = value;
    }

    void setAddress(UintPtr value)
        // Set tthe 'address' field to the specified 'value'.
    {
        d_address = reinterpret_cast<const void *>(value);
    }

#ifdef BALST_DWARF
    void setCompileUnitDir(const bsl::string& value)
        // The the compile unit directory to the specified 'value'.
    {
        d_compileUnitDir = value;
    }

    void setCompileUnitFileName(const bsl::string& value)
        // The the compile unit file name to the specified 'value'.
    {
        d_compileUnitFileName = value;
    }

    void setCompileUnitOffset(const Offset value)
        // Set tthe compilation unit offset field to the specified 'value'.
    {
        d_compileUnitOffset = value;
    }

    void setLineNumberOffset(Offset value)
        // Set tthe line number offset field to the specified 'value'.
    {
        d_lineNumberOffset = value;
    }
#endif

    void setSymbolResolved()
        // Set this frame as being done.
    {
        d_isSymbolResolved = true;
    }

    // ACCESSORS
    bool operator<(const FrameRec& rhs) const
        // Return 'true' if the address field of this object is less than the
        // address field of 'rhs'.
    {
        return d_address < rhs.d_address;
    }

    const void *address() const
        // Return the 'address' field from this object.
    {
        return d_address;
    }

#ifdef BALST_DWARF
    const bsl::string& compileUnitDir() const
        // Return the compile unit directory name.
    {
        return d_compileUnitDir;
    }

    const bsl::string& compileUnitFileName() const
        // Return the compile unit file name.
    {
        return d_compileUnitFileName;
    }

    Offset compileUnitOffset() const
        // Return the compile unit offset field.
    {
        return d_compileUnitOffset;
    }

    Offset lineNumberOffset() const
        // Return the line number offset field.
    {
        return d_lineNumberOffset;
    }
#endif

    balst::StackTraceFrame& frame() const
        // Return a reference to the modifiable 'frame' referenced by this
        // object.  Note that though this is a 'const' method, modifiable
        // access to the frame is provided.
    {
        return *d_frame_p;
    }

    bool isSymbolResolved() const
        // Return 'true' if this frame is done and 'false' otherwise.
    {
        return d_isSymbolResolved;
    }
};

typedef bsl::vector<local::FrameRec> FrameRecVec;     // Vector of 'FrameRec's.
typedef FrameRecVec::iterator        FrameRecVecIt;   // Iterator of
                                                      // 'FrameRecVec'.

struct AddressRange {
    UintPtr d_address;
    UintPtr d_size;

    // ACCESSORS
    bool contains(const void *address) const
        // Return 'true' if this address range contains the specified 'address'
        // and 'false' otherwise.
    {
        UintPtr a = reinterpret_cast<UintPtr>(address);
        return d_address <= a && a < d_address + d_size;
    }

    bool contains(UintPtr address) const
        // Return 'true' if this address range contains the specified 'address'
        // and 'false' otherwise.
    {
        return d_address <= address && address < d_address + d_size;
    }

    bool overlaps(const AddressRange& other) const
        // Return 'true' if this address range overlaps the specified
        // AddressRange 'other' and 'false' otherwise.
    {
        return d_address <= other.d_address
               ? d_address + d_size > other.d_address
               : other.d_address + other.d_size > d_address;
    }
};

}  // close namespace local

typedef bsl::size_t size_t;

}  // close unnamed namespace

                              // ----------------
                              // static functions
                              // ----------------

static
int checkElfHeader(local::ElfHeader *elfHeader)
    // Return 0 if the magic numbers in the specified 'elfHeader' are correct
    // and a non-zero value otherwise.
{
    if   (ELFMAG0 != elfHeader->e_ident[EI_MAG0]
       || ELFMAG1 != elfHeader->e_ident[EI_MAG1]
       || ELFMAG2 != elfHeader->e_ident[EI_MAG2]
       || ELFMAG3 != elfHeader->e_ident[EI_MAG3]) {
        return -1;                                                    // RETURN
    }

    // this code can only read native-endian ELF files

    if ((local::e_IS_BIG_ENDIAN ? ELFDATA2MSB : ELFDATA2LSB) !=
                                                 elfHeader->e_ident[EI_DATA]) {
        return -1;                                                    // RETURN
    }

    // this code can only read native-sized ELF files

    if ((sizeof(void *) == 4 ? ELFCLASS32 : ELFCLASS64) !=
                                                elfHeader->e_ident[EI_CLASS]) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                    // ------------------------------------
                    // local::StackTraceResolver::HiddenRec
                    // ------------------------------------

struct local::StackTraceResolver::HiddenRec {
    // This 'struct' contains all fields of this resolver that are local to the
    // current segment.  The resolver iterates over multiple segments,
    // resolving symbols within one at a time.

    // TYPES
    typedef local::UintPtr
                   UintPtr;             // 32 bit unsigned on 32 bit, 64 bit
                                        // unsigned on 64 bit, usually used for
                                        // absolute offsets into a file and
                                        // some pointers
    typedef local::Offset
                   Offset;              // 8 bit signed file offset

    // DATA
    StackTraceResolver_FileHelper
                  *d_helper_p;          // file helper associated with current
                                        // segment

    StackTrace    *d_stackTrace_p;      // the stack trace we are resolving

    local::AddressRange
                   d_addressRange;      // address range of the current segment

    local::FrameRecVec
                   d_frameRecs;         // Vector of address frame pairs for
                                        // fast lookup of addresses.  Not local
                                        // to current segment -- initialized
                                        // once per resolve.

    local::FrameRecVecIt                // This begin, end pair indicates the
                   d_frameRecsBegin;    // range of frame records that pertain
    local::FrameRecVecIt                // to the current segment.
                   d_frameRecsEnd;

    UintPtr        d_adjustment;        // adjustment between addresses
                                        // expressed in object file and actual
                                        // addresses in memory for current
                                        // segment

    local::Section d_symTableSec;       // symbol table section (symbol table
                                        // does not contain symbol names, just
                                        // offsets into string table) from the
                                        // beginning of the executable or
                                        // library file

    local::Section d_stringTableSec;    // string table offset from the
                                        // beginning of the file

#ifdef BALST_DWARF
    local::Section d_abbrevSec;         // .debug_abbrev section
    local::Reader  d_abbrevReader;      // reader for that section

    local::Section d_arangesSec;        // .debug_aranges section
    local::Reader  d_arangesReader;     // reader for that section

    local::Section d_infoSec;           // .debug_info section
    local::Reader  d_infoReader;        // reader for that section

    local::Section d_lineSec;           // .debug_line section
    local::Reader  d_lineReader;        // reader for that section

    local::Section d_rangesSec;         // .debug_ranges section
    local::Reader  d_rangesReader;      // reader for that section

    local::Section d_strSec;            // .debug_str section
    local::Reader  d_strReader;         // reader for that section
#endif

    Offset         d_libraryFileSize;   // size of the current library or
                                        // executable file

    char          *d_scratchBufA_p;     // crratch buffer A (from resolver)

    char          *d_scratchBufB_p;     // scratch buffer B (from resolver)

    char          *d_scratchBufC_p;     // scratch buffer C (from resolver)

    char          *d_scratchBufD_p;     // scratch buffer D (from resolver)

    int            d_numTotalUnmatched; // Total number of unmatched frames
                                        // remaining in this resolve.

    bool           d_isMainExecutable;  // 'true' if in main executable
                                        // segment, as opposed to a shared
                                        // library
    bslma::Allocator
                  *d_allocator_p;       // The resolver's heap bypass
                                        // allocator.

  private:
    // NOT IMPLEMENTED
    HiddenRec(const HiddenRec&);
    HiddenRec& operator=(const HiddenRec&);

  public:
    // CLASS METHODS
#ifdef BALST_DWARF
    static
    const char *dwarfStringForAt(unsigned id);
        // Return the string equivalent of the specified 'DW_AT_*' 'id'.

    static
    const char *dwarfStringForForm(unsigned id);
        // Return the string equivalent of the specified 'DW_FORM_*' 'id'.

#if 0
    static
    const char *dwarfStringForInlineState(unsigned inlineState);
        // Return the string equivalent of the specified 'DW_INL_*'
        // 'inlineState'.

    static
    const char *dwarfStringForTag(unsigned tag);
        // Return the string equivalent of the specified 'DW_TAG_*' 'tag'.
#endif
#endif

    // CREATORS
    explicit
    HiddenRec(local::StackTraceResolver *resolver);
        // Create this 'Seg' object, initialize 'd_numFrames' to 'numFrames',
        // and initialize all other fields to 0.

    // MANIPULATORS
#ifdef BALST_DWARF
# if BALST_DWARF_CHECK_ADDRESSES
    int dwarfCheckRanges(bool          *isMatch,
                         UintPtr        addressToMatch,
                         UintPtr        baseAddress,
                         Offset         offset);
        // Read a ranges section and determine if an address matches.
# endif

    int dwarfReadAll();
        // Read the DWARF information.

    int dwarfReadAranges();
        // Read the .debug_aranges section.  Return the number of frames
        // matched.

    int dwarfReadCompileOrPartialUnit(local::FrameRec *frameRec);
        // Read a compile or partial unit, assuming that 'd_infoReader' is
        // positioned right after the tag & children info, and 'd_abbrevReader'
        // is positioned right after the tag index, and at the first attribute.

    int dwarfReadDebugInfo();
        // Read the .debug_info and .debug_abbrev sections.

    int dwarfReadDebugInfoFrameRec(local::FrameRec *frameRec);
        // Read the dwarf info for a single compilation unit for a single
        // frame.  Return
        //..
        //: o rc <  0: failure
        //: o rc == 0: successfully parsed, but no match
        //: o rc == 1: successfully parsed, matched address & line number info
        //..

#if 0
    int dwarfReadForeignFunctionFileIdx(local::FrameRec *frameRec);
        // Read children of the current compile unit and store the information
        // found in the specified '*frameRec'.
        //
        // Called by 'dwarfReadCompileOrPartialUnit' to traverse children,
        // looking for the declaration of the subroutine to find out if the
        // subroutine is declared in a separate file (i.e. if it's inline or a
        // template function).  This function assumes that 'd_abbrevReader' and
        // 'd_infoReader' are positioned right after reading the compile unit.
#endif
#endif

    void reset();
        // Zero numerous fields.

    // ACCESSORS
    int frameIndex(const balst::StackTraceFrame& fr) const;
        // Give the stack trace index of the specified 'fr'.
};

// CLASS METHODS
#ifdef BALST_DWARF
const char *local::StackTraceResolver::HiddenRec::dwarfStringForAt(unsigned id)
{
    static const char rn[] = { "dwarfStringForAt:" };

#undef CASE
#define CASE(x)    case x: return #x

    using namespace local;    // pick up 'e_*' id's

    switch (id) {
      CASE(DW_AT_sibling);
      CASE(DW_AT_location);
      CASE(DW_AT_name);
      CASE(DW_AT_ordering);
      CASE(DW_AT_subscr_data);
      CASE(DW_AT_byte_size);
      CASE(DW_AT_bit_offset);
      CASE(DW_AT_bit_size);
      CASE(DW_AT_element_list);
      CASE(DW_AT_stmt_list);
      CASE(DW_AT_low_pc);
      CASE(DW_AT_high_pc);
      CASE(DW_AT_language);
      CASE(DW_AT_member);
      CASE(DW_AT_discr);
      CASE(DW_AT_discr_value);
      CASE(DW_AT_visibility);
      CASE(DW_AT_import);
      CASE(DW_AT_string_length);
      CASE(DW_AT_common_reference);
      CASE(DW_AT_comp_dir);
      CASE(DW_AT_const_value);
      CASE(DW_AT_containing_type);
      CASE(DW_AT_default_value);
      CASE(DW_AT_inline);
      CASE(DW_AT_is_optional);
      CASE(DW_AT_lower_bound);
      CASE(DW_AT_producer);
      CASE(DW_AT_prototyped);
      CASE(DW_AT_return_addr);
      CASE(DW_AT_start_scope);
      CASE(DW_AT_bit_stride);
      CASE(DW_AT_upper_bound);
      CASE(DW_AT_abstract_origin);
      CASE(DW_AT_accessibility);
      CASE(DW_AT_address_class);
      CASE(DW_AT_artificial);
      CASE(DW_AT_base_types);
      CASE(DW_AT_calling_convention);
      CASE(DW_AT_count);
      CASE(DW_AT_data_member_location);
      CASE(DW_AT_decl_column);
      CASE(DW_AT_decl_file);
      CASE(DW_AT_decl_line);
      CASE(DW_AT_declaration);
      CASE(DW_AT_discr_list);
      CASE(DW_AT_encoding);
      CASE(DW_AT_external);
      CASE(DW_AT_frame_base);
      CASE(DW_AT_friend);
      CASE(DW_AT_identifier_case);
      CASE(DW_AT_macro_info);
      CASE(DW_AT_namelist_item);
      CASE(DW_AT_priority);
      CASE(DW_AT_segment);
      CASE(DW_AT_specification);
      CASE(DW_AT_static_link);
      CASE(DW_AT_type);
      CASE(DW_AT_use_location);
      CASE(DW_AT_variable_parameter);
      CASE(DW_AT_virtuality);
      CASE(DW_AT_vtable_elem_location);
      CASE(DW_AT_allocated);
      CASE(DW_AT_associated);
      CASE(DW_AT_data_location);
      CASE(DW_AT_byte_stride);
      CASE(DW_AT_entry_pc);
      CASE(DW_AT_use_UTF8);
      CASE(DW_AT_extension);
      CASE(DW_AT_ranges);
      CASE(DW_AT_trampoline);
      CASE(DW_AT_call_column);
      CASE(DW_AT_call_file);
      CASE(DW_AT_call_line);
      CASE(DW_AT_description);
      CASE(DW_AT_binary_scale);
      CASE(DW_AT_decimal_scale);
      CASE(DW_AT_small);
      CASE(DW_AT_decimal_sign);
      CASE(DW_AT_digit_count);
      CASE(DW_AT_picture_string);
      CASE(DW_AT_mutable);
      CASE(DW_AT_threads_scaled);
      CASE(DW_AT_explicit);
      CASE(DW_AT_object_pointer);
      CASE(DW_AT_endianity);
      CASE(DW_AT_elemental);
      CASE(DW_AT_pure);
      CASE(DW_AT_recursive);
      CASE(e_DW_AT_main_subprogram);
      default: {
        eprintf("%s unrecognized 'DW_AT_? value = 0x%x\n", rn, id);

        return "DW_AT_????";                                          // RETURN
      }
    }
#undef CASE
}

const char *local::StackTraceResolver::HiddenRec::dwarfStringForForm(
                                                                   unsigned id)
{
    static const char rn[] = { "dwarfStringForForm:" };

#undef CASE
#define CASE(x)    case x: return #x

    using namespace local;    // pick up 'e_*' id's

    switch (id) {
      CASE(DW_FORM_addr);
      CASE(DW_FORM_block2);
      CASE(DW_FORM_block4);
      CASE(DW_FORM_data2);
      CASE(DW_FORM_data4);
      CASE(DW_FORM_data8);
      CASE(DW_FORM_string);
      CASE(DW_FORM_block);
      CASE(DW_FORM_block1);
      CASE(DW_FORM_data1);
      CASE(DW_FORM_flag);
      CASE(DW_FORM_sdata);
      CASE(DW_FORM_strp);
      CASE(DW_FORM_udata);
      CASE(DW_FORM_ref_addr);
      CASE(DW_FORM_ref1);
      CASE(DW_FORM_ref2);
      CASE(DW_FORM_ref4);
      CASE(DW_FORM_ref8);
      CASE(DW_FORM_ref_udata);
      CASE(DW_FORM_indirect);
      CASE(e_DW_FORM_sec_offset);
      CASE(e_DW_FORM_exprloc);
      CASE(e_DW_FORM_flag_present);
      CASE(e_DW_FORM_ref_sig8);
      default: {
        eprintf("%s unrecognized 'DW_FORM_?' value = 0x%x\n", rn, id);

        return "DW_FORM_????";                                        // RETURN
      }
    }
#undef CASE
}

#if 0
const char *local::StackTraceResolver::HiddenRec::dwarfStringForInlineState(
                                                          unsigned inlineState)
{
    static const char rn[] = { "dwarfStringForInlineState:" };

#undef CASE
#define CASE(x)    case x: return #x

    switch (inlineState) {
      CASE(DW_INL_not_inlined);
      CASE(DW_INL_inlined);
      CASE(DW_INL_declared_not_inlined);
      CASE(DW_INL_declared_inlined);
      default: {
        eprintf("%s unrecognized 'DW_INL_?' value = 0x%x\n", rn, inlineState);

        return "DW_INL_????";                                         // RETURN
      }
    }
#undef CASE
}
#endif

#if 0
const char *local::StackTraceResolver::HiddenRec::dwarfStringForTag(
                                                                  unsigned tag)
{
    static const char rn[] = { "dwarfStringForTag:" };

#undef CASE
#define CASE(x)    case x: return #x

    using namespace local;    // pick up 'e_DW_TAG_*'

    switch (tag) {
      CASE(DW_TAG_array_type);
      CASE(DW_TAG_class_type);
      CASE(DW_TAG_entry_point);
      CASE(DW_TAG_enumeration_type);
      CASE(DW_TAG_formal_parameter);
      CASE(DW_TAG_imported_declaration);
      CASE(DW_TAG_label);
      CASE(DW_TAG_lexical_block);
      CASE(DW_TAG_member);
      CASE(DW_TAG_pointer_type);
      CASE(DW_TAG_reference_type);
      CASE(DW_TAG_compile_unit);
      CASE(DW_TAG_string_type);
      CASE(DW_TAG_structure_type);
      CASE(DW_TAG_subroutine_type);
      CASE(DW_TAG_typedef);
      CASE(DW_TAG_union_type);
      CASE(DW_TAG_unspecified_parameters);
      CASE(DW_TAG_variant);
      CASE(DW_TAG_common_block);
      CASE(DW_TAG_common_inclusion);
      CASE(DW_TAG_inheritance);
      CASE(DW_TAG_inlined_subroutine);
      CASE(DW_TAG_module);
      CASE(DW_TAG_ptr_to_member_type);
      CASE(DW_TAG_set_type);
      CASE(DW_TAG_subrange_type);
      CASE(DW_TAG_with_stmt);
      CASE(DW_TAG_access_declaration);
      CASE(DW_TAG_base_type);
      CASE(DW_TAG_catch_block);
      CASE(DW_TAG_const_type);
      CASE(DW_TAG_constant);
      CASE(DW_TAG_enumerator);
      CASE(DW_TAG_file_type);
      CASE(DW_TAG_friend);
      CASE(DW_TAG_namelist);
      CASE(DW_TAG_namelist_item);
      CASE(DW_TAG_packed_type);
      CASE(DW_TAG_subprogram);
      CASE(DW_TAG_template_type_parameter);
      CASE(DW_TAG_template_value_parameter);
      CASE(DW_TAG_thrown_type);
      CASE(DW_TAG_try_block);
      CASE(DW_TAG_variant_part);
      CASE(DW_TAG_variable);
      CASE(DW_TAG_volatile_type);
      CASE(DW_TAG_dwarf_procedure);
      CASE(DW_TAG_restrict_type);
      CASE(DW_TAG_interface_type);
      CASE(DW_TAG_namespace);
      CASE(DW_TAG_imported_module);
      CASE(DW_TAG_unspecified_type);
      CASE(DW_TAG_partial_unit);
      CASE(DW_TAG_imported_unit);
      CASE(DW_TAG_mutable_type);
      CASE(DW_TAG_condition);
      CASE(DW_TAG_shared_type);
      CASE(e_DW_TAG_type_unit);
      CASE(e_DW_TAG_rvalue_reference_type);
      CASE(e_DW_TAG_template_alias);

      CASE(DW_TAG_lo_user);

      // The values between 'DW_TAG_lo_user' and 'DW_TAG_hi_user' are not
      // important, if we port somewhere where they aren't in 'dwarf.h',
      // those entries can be removed.

      CASE(DW_TAG_MIPS_loop);
      CASE(DW_TAG_format_label);
      CASE(DW_TAG_function_template);
      CASE(DW_TAG_class_template);

      CASE(DW_TAG_hi_user);
      default: {
        if (tag < DW_TAG_lo_user || tag > DW_TAG_hi_user) {
            eprintf("%s unrecognized 'DW_TAG_?' value = 0x%x\n", rn, tag);
        }
        else {
            eprintf("%s unrecognized user-defined 'DW_TAG_?' value = 0x%x\n",
                                                                      rn, tag);
        }

        return "DW_TAG_????";                                         // RETURN
      }
    }
#undef CASE
}
#endif

#endif

// CREATORS
local::StackTraceResolver::HiddenRec::HiddenRec(
                                           local::StackTraceResolver *resolver)
: d_helper_p(0)
, d_stackTrace_p(resolver->d_stackTrace_p)
, d_frameRecs(&resolver->d_hbpAlloc)
, d_frameRecsBegin()
, d_frameRecsEnd()
, d_adjustment(0)
, d_symTableSec()
, d_stringTableSec()
#ifdef BALST_DWARF
, d_abbrevSec()
, d_arangesSec()
, d_infoSec()
, d_lineSec()
, d_rangesSec()
#endif
, d_scratchBufA_p(resolver->d_scratchBufA_p)
, d_scratchBufB_p(resolver->d_scratchBufB_p)
, d_scratchBufC_p(resolver->d_scratchBufC_p)
, d_scratchBufD_p(resolver->d_scratchBufD_p)
, d_numTotalUnmatched(resolver->d_stackTrace_p->length())
, d_isMainExecutable(0)
, d_allocator_p(&resolver->d_hbpAlloc)
{
    d_frameRecs.reserve(d_numTotalUnmatched);
    for (int ii = 0; ii < d_numTotalUnmatched; ++ii) {
        balst::StackTraceFrame& frame = (*resolver->d_stackTrace_p)[ii];
        d_frameRecs.push_back(local::FrameRec(frame.address(),
                                              &frame,
                                              &resolver->d_hbpAlloc));
    }
    bsl::sort(d_frameRecs.begin(), d_frameRecs.end());
}

// MANIPULATORS
#ifdef BALST_DWARF
int local::StackTraceResolver::HiddenRec::dwarfReadAll()
{
    static const char rn[] = { "dwarfReadAll" };    (void) rn;

    if (0 == d_arangesSec.d_offset || 0 == d_infoSec.d_offset) {
        e_TRACES && zprintf("%s Not enough information to find file names"
                                                    " or line numbers.\n", rn);
        return -1;                                                    // RETURN
    }

    int rc = dwarfReadAranges();  // Get the locations of the compile unit info
    ASSERT_BAIL(rc >= 0 && ".debug_aranges failed");
    if (0 == rc) {
        e_TRACES && zprintf(".debug_aranges did not match\n");
        return -1;                                                    // RETURN
    }

    rc = dwarfReadDebugInfo();   // Get the source filenames and the location
                                 // of the line number info, from .debug_info.
    ASSERT_BAIL(0 == rc && "dwarfReadDebugInfo failed");

#if 0
    if (0 == d_lineSec.d_offset) {
        e_TRACES && zprintf("%s Line number info not found.\n", rn);
        return -1;
    }

    rc = dwarfReadLineInfo();        // Get the line numbers, from .debug_line.
    if (rc) {
        e_TRACES && zprintf("%s .debug_line failed\n", rn);
        return -1;
    }
#endif
    return 0;
}

#if BALST_DWARF_CHECK_ADDRESSES
// This code has been tested and works.  For the time being, it is deemed
// unnecessary, if we assume that .debug_aranges pointed us at exactly the
// right compile unit.  If it turns out that it sometimes points us at a
// compile unit followed by partial compile units and we need to match
// addresses to find the right one, we will want to ressurect this code.

int local::StackTraceResolver::HiddenRec::dwarfCheckRanges(
                                                 bool          *isMatch,
                                                 UintPtr        addressToMatch,
                                                 UintPtr        baseAddress,
                                                 Offset         offset)
{
    static const char rn[] = { "dwarfCheckRanges:" };
    int rc;

    *isMatch = false;

    rc = d_rangesReader.skipTo(d_rangesSec.d_offset + offset);
    ASSERT_BAIL(0 == rc);

    bool firstTime = true;
    while (true) {
        UintPtr loAddress, hiAddress;
        rc = d_rangesReader.readAddress(&loAddress);
        ASSERT_BAIL(0 == rc && "read loAddress failed");
        rc = d_rangesReader.readAddress(&hiAddress);
        ASSERT_BAIL(0 == rc && "read hiAddress failed");

        if (0 == loAddress && 0 == hiAddress) {
            e_TRACES && zprintf("%s done - no match found\n", rn);

            return 0;                                                 // RETURN
        }

        if (firstTime) {
            firstTime = false;

            const UintPtr baseIndicator =
                      static_cast<int>(sizeof(UintPtr)) ==
                                                   d_rangesReader.addressSize()
                      ? local::minusOne
                      : 0xffffffffUL;

            if (baseIndicator == loAddress) {
                baseAddress = hiAddress;
            }
            else {
                if (local::minusOne == baseAddress) {
                    baseAddress = d_adjustment;
                }

                e_TRACES && zprintf("%s base address not in .debug_ranges,"
                                     " loAddress: %llx, baseAddress: 0x%llx\n",
                                           rn, ll(loAddress), ll(baseAddress));
            }
            if (addressToMatch < baseAddress) {
                e_TRACES && zprintf("%s address to match 0x%llx below base"
                                              " address 0x%llx -- not a bug\n",
                                      rn, ll(addressToMatch), ll(baseAddress));

                return 0;                                             // RETURN
            }
            addressToMatch -= baseAddress;
            if (baseIndicator == loAddress) {
                continue;
            }
        }

        ASSERT_BAIL_SAFE(loAddress <= hiAddress);

        if (loAddress <= addressToMatch && addressToMatch < hiAddress) {
            e_TRACES && zprintf("%s .debug_ranges matched address 0x%llx in"
                                                         " [0x%llx, 0x%llx)\n",
                                          rn, ll(addressToMatch + baseAddress),
                     ll(loAddress + baseAddress), ll(hiAddress + baseAddress));
            *isMatch = true;
            return 0;                                                 // RETURN
        }
    }
}
#endif

int local::StackTraceResolver::HiddenRec::dwarfReadAranges()
{
    static const char rn[] = { "dwarfReadAranges:" };    (void) rn;

    // Buffer use: this function usee only scratch buf A.

    int rc;

    const local::FrameRecVecIt end = d_frameRecsEnd;
    const int toMatch = static_cast<int>(end - d_frameRecsBegin);
    BSLS_ASSERT(toMatch > 0);       // otherwise we should not have been called
    int matched = 0;

    e_TRACES && zprintf("%s starting, toMatch=%d\n", rn, toMatch);

    rc = d_arangesReader.init(d_helper_p, d_scratchBufA_p, d_arangesSec,
                                                            d_libraryFileSize);
    ASSERT_BAIL(0 == rc);

    local::AddressRange addressRange, prevRange = { 0, 0 }; (void) prevRange;
    local::FrameRec dummyFrameRec(0, 0, d_allocator_p);

    while (!d_arangesReader.atEndOfSection()) {
        Offset      rangeLength;
        int rc = d_arangesReader.readInitialLength(&rangeLength);
        ASSERT_BAIL(0 == rc && "read initial length failed");

        Offset endOffset = d_arangesReader.offset() + rangeLength;
        // 'endOffset has already been checked to fit within the section.

        unsigned short version;
        rc = d_arangesReader.readValue(&version);
        ASSERT_BAIL(0 == rc && "read version failed");
        ASSERT_BAIL(2 == version || P(version));

        Offset debugInfoOffset;
        rc = d_arangesReader.readSectionOffset(&debugInfoOffset);
        ASSERT_BAIL(0 == rc);
        ASSERT_BAIL(0 <= debugInfoOffset || PH(debugInfoOffset));
        ASSERT_BAIL(     debugInfoOffset < d_infoSec.d_size ||
                                                          PH(debugInfoOffset));

        rc = d_arangesReader.readAddressSize();
        ASSERT_BAIL(0 == rc);

        unsigned char segmentSize;
        rc = d_arangesReader.readValue(&segmentSize);
        ASSERT_BAIL(0 == rc && "read segment size failed");
        ASSERT_BAIL(0 == segmentSize);

        // The 'mysteryZero' field is not in the spec, but everything didn't
        // start working until I skipped those 4 bytes.

        unsigned mysteryZero;
        rc = d_arangesReader.readValue(&mysteryZero);
        ASSERT_BAIL(0 == rc);
        if (0 != mysteryZero) {
            eprintf("%s mysteryZero = %u\n", rn, mysteryZero);

            // Don't return, we aren't sure those 4 bytes are supposed to be
            // zero.
        }

        // e_TRACES && zprintf("%s Starting section %g pairs long.\n", rn,
        //                    (double) (endOffset - d_arangesReader.offset()) /
        //                                (2 * d_arangesReader.addressSize()));

        bool foundZeroes = false;
        while (d_arangesReader.offset() < endOffset) {
            if (sizeof(UintPtr) == sizeof(unsigned)) {
                rc =  d_arangesReader.readValue(&addressRange);
            }
            else {
                rc =  d_arangesReader.readAddress(&addressRange.d_address);
                rc |= d_arangesReader.readAddress(&addressRange.d_size);
            }
            ASSERT_BAIL(0 == rc);

            if (0 == addressRange.d_address && 0 == addressRange.d_size) {
                if (d_arangesReader.offset() != endOffset) {
                    eprintf("%s terminating 0's %s range end\n", rn,
                        d_arangesReader.offset() < endOffset ? "reached before"
                                                             : "overlap");
                    rc = d_arangesReader.skipTo(endOffset);
                    ASSERT_BAIL(0 == rc);
                }

                foundZeroes = true;
                break;
            }

            addressRange.d_address += d_adjustment;    // This was not
                                                       // mentioned in the doc.

            if (!d_addressRange.contains(addressRange.d_address)) {
                // Sometimes the address ranges are just garbage.

                e_TRACES && zprintf("Garbage address range [0x%lx, 0x%lx)\n",
                            l(addressRange.d_address), l(addressRange.d_size));
                continue;
            }

            ASSERT_BAIL_SAFE((!prevRange.overlaps(addressRange) &&
                                                            "range overlap") ||
                       PH(prevRange.   d_address) || PH(prevRange.   d_size) ||
                       PH(addressRange.d_address) || PH(addressRange.d_size));
            prevRange = addressRange;
            // zprintf("%s range:[0x%lx, 0x%lx)\n",
            //                                   rn, l(addressRange.d_address),
            //                                       l(addressRange.d_size));

            dummyFrameRec.setAddress(addressRange.d_address);
            local::FrameRecVecIt begin =
                        bsl::lower_bound(d_frameRecsBegin, end, dummyFrameRec);
            for (local::FrameRecVecIt it = begin; it < end &&
                                  addressRange.contains(it->address()); ++it) {
                const bool isRedundant =
                                   local::maxOffset != it->compileUnitOffset();
                e_TRACES && zprintf("%s%s range (0x%lx, 0x%lx) matches"
                                     " frame %d, address: %p, offset 0x%llx\n",
                                           rn, isRedundant ? " redundant" : "",
                             l(addressRange.d_address), l(addressRange.d_size),
                                                       frameIndex(it->frame()),
                                           it->address(), ll(debugInfoOffset));
                if (isRedundant) {
                    continue;
                }

                it->setCompileUnitOffset(debugInfoOffset);
                if (toMatch == ++matched) {
                    e_TRACES && zprintf(
                                     "%s last frame in segment matched\n", rn);

                    d_arangesReader.disable();
                    return matched;                                   // RETURN
                }
            }
        }

        ASSERT_BAIL(foundZeroes);
    }

    zprintf("%s failed to complete -- %d frames unmatched.\n",
                                                        rn, toMatch - matched);

    d_arangesReader.disable();
    return matched;
}

int local::StackTraceResolver::HiddenRec::dwarfReadCompileOrPartialUnit(
                                                     local::FrameRec *frameRec)
{
    static const char rn[] = { "dwarfReadCompileOrPartialUnit:" };

    int rc;

    enum ObtainedFlags {
        k_OBTAINED_ADDRESS_MATCH = 0x1,
        k_OBTAINED_DIR_NAME      = 0x2,
        k_OBTAINED_BASE_NAME     = 0x4,
        k_OBTAINED_LINE_OFFSET   = 0x8,

        k_OBTAINED_SOURCE_NAME   = k_OBTAINED_DIR_NAME | k_OBTAINED_BASE_NAME,
        k_OBTAINED_ALL           = 0xf };

    int obtained = BALST_DWARF_CHECK_ADDRESSES ? 0
                                               : k_OBTAINED_ADDRESS_MATCH;

#if BALST_DWARF_CHECK_ADDRESSES
    const UintPtr addressToMatch =
                                reinterpret_cast<UintPtr>(frameRec->address());
#endif

    const int index = frameIndex(frameRec->frame()); (void) index;

    UintPtr loPtr = local::minusOne, hiPtr = 0;
    bsl::string baseName(d_allocator_p), dirName(d_allocator_p);
    dirName.reserve(200);
    Offset lineNumberInfoOffset;

    do {
        unsigned attr;
        rc = d_abbrevReader.readULEB128(&attr);
        ASSERT_BAIL(0 == rc);

        unsigned form;
        rc = d_abbrevReader.readULEB128(&form);
        ASSERT_BAIL(0 == rc);

        if (0 == attr) {
            ASSERT_BAIL(0 == form);
            break;
        }

        e_TRACES && zprintf("%s %s %s\n", rn, dwarfStringForAt(attr),
                                                     dwarfStringForForm(form));

        switch (attr) {
          case DW_AT_low_pc: {                            // DWARF doc 3.1.1.1
            ASSERT_BAIL(local::minusOne == loPtr);

            rc = d_infoReader.readAddress(&loPtr, form);
            ASSERT_BAIL(0 == rc);

#if BALST_DWARF_CHECK_ADDRESSES
            if (DW_FORM_addr != form) {
                // this was not in the doc

                loPtr += d_adjustment;
            }

            e_TRACES && zprintf("%s loPtr: 0x%llx\n", rn, ll(loPtr));

            if (0 != hiPtr) {
                if (loPtr <= addressToMatch && addressToMatch < hiPtr) {
                    e_TRACES && zprintf("%s loHiMatch on lo\n", rn);
                    obtained |= k_OBTAINED_ADDRESS_MATCH;
                }
                else {
                    e_TRACES && zprintf("%s loHi failed to match on lo\n", rn);
                }
            }
#endif
          } break;
          case DW_AT_high_pc: {                           // DWARF doc 3.1.1.1
            ASSERT_BAIL(0 == hiPtr);

            rc = d_infoReader.readAddress(&hiPtr, form);
            ASSERT_BAIL(0 == rc);

#if BALST_DWARF_CHECK_ADDRESSES
            if (DW_FORM_addr != form) {
                // this was not in the doc, just guessing

                hiPtr += local::minusOne != loPtr ? loPtr : d_adjustment;
            }

            e_TRACES && zprintf("%s hiPtr: 0x%llx\n", rn, ll(hiPtr));

            if (local::minusOne != loPtr) {
                if (loPtr <= addressToMatch && addressToMatch < hiPtr) {
                    e_TRACES && zprintf("%s loHiMatch on hi\n", rn);
                    obtained |= k_OBTAINED_ADDRESS_MATCH;
                }
                else {
                    e_TRACES && zprintf("%s loHi failed to match on hi\n", rn);
                }
            }
#endif
          } break;
          case DW_AT_ranges: {                            // DWARF doc 3.1.1.1
            Offset rangesOffset;
            rc = d_infoReader.readOffsetFromForm(&rangesOffset, form);
            ASSERT_BAIL(0 == rc && "trouble reading ranges offset");
            ASSERT_BAIL(rangesOffset < d_rangesSec.d_size);
#if BALST_DWARF_CHECK_ADDRESSES
            bool isMatch;
            rc = dwarfCheckRanges(
                                &isMatch,
                                reinterpret_cast<UintPtr>(frameRec->address()),
                                loPtr,
                                rangesOffset);
            ASSERT_BAIL(0 == rc && "dwarfCheckRanges failed");

            if (isMatch) {
                e_TRACES && zprintf("%s ranges match\n", rn);
                obtained |= k_OBTAINED_ADDRESS_MATCH;
            }
#endif
          } break;
          case DW_AT_name: {                              // DWARF doc 3.1.1.2
            ASSERT_BAIL(0 == (obtained & k_OBTAINED_BASE_NAME));
            ASSERT_BAIL(baseName.empty());

            rc = d_infoReader.readStringFromForm(
                                                &baseName, &d_strReader, form);
            ASSERT_BAIL(0 == rc);

            if (!baseName.empty()) {
                e_TRACES && zprintf("%s baseName \"%s\" found\n",
                                                         rn, baseName.c_str());

                obtained |= k_OBTAINED_BASE_NAME |
                              ('/' == baseName.c_str()[0] ? k_OBTAINED_DIR_NAME
                                                          : 0);
            }
          } break;
          case DW_AT_stmt_list: {                         // DWARF doc 3.1.1.4
            ASSERT_BAIL(0 == (obtained & k_OBTAINED_LINE_OFFSET));

            rc = d_infoReader.readOffsetFromForm(&lineNumberInfoOffset, form);
            ASSERT_BAIL(0 == rc && "trouble reading line offset");

            obtained |= k_OBTAINED_LINE_OFFSET;
          } break;
          case DW_AT_comp_dir: {                          // DWARF doc 3.1.1.6
            rc = d_infoReader.readStringFromForm(&dirName, &d_strReader, form);
            ASSERT_BAIL(0 == rc);

            if (!dirName.empty()) {
                e_TRACES && zprintf("%s dirName \"%s\" found\n",
                                                          rn, dirName.c_str());

                obtained |= k_OBTAINED_DIR_NAME;
            }
          } break;
          case DW_AT_language:                            // DWARF doc 3.1.1.3
          case DW_AT_macro_info:                          // DWARF doc 3.1.1.5
          case DW_AT_producer:                            // DWARF doc 3.1.1.7
          case DW_AT_identifier_case:                     // DWARF doc 3.1.1.8
          case DW_AT_base_types:                          // DWARF doc 3.1.1.9
          case DW_AT_use_UTF8:                            // DWARF doc 3.1.1.10
          case local::e_DW_AT_main_subprogram:            // DWARF doc 3.1.1.11
          case DW_AT_description:
          case DW_AT_segment: {
            rc = d_infoReader.skipForm(form);
            ASSERT_BAIL((0 == rc && "problem skipping") || PH(attr) ||
                                                                     PH(form));
          } break;
          default: {
            ASSERT_BAIL((0 && "compile unit: unrecognized attribute")
                                                                  || PH(attr));
          }
        }

        // If 'e_TRACES' is set, continue until we hit the terminating '0's to
        // verify that our code is handling everything in compile units.  If
        // 'e_TRACES' is not set, quit once we have the info we want.
    } while (e_TRACES || k_OBTAINED_ALL != obtained);

    if (e_TRACES) {
        zprintf("%s (attr, form) loop terminated, all%s obtained\n", rn,
                (k_OBTAINED_ALL == obtained) ? "" : " not");
        zprintf("%s base name %s, dir name %s, line # offset %s\n", rn,
                !baseName.empty()                   ? "found" : "not found",
                !dirName .empty()                   ? "found" : "not found",
                (obtained | k_OBTAINED_LINE_OFFSET) ? "found" : "not found");
    }

    ASSERT_BAIL((k_OBTAINED_ADDRESS_MATCH & obtained) || P(index));
    ASSERT_BAIL((k_OBTAINED_LINE_OFFSET & obtained) || P(index));
    ASSERT_BAIL(!baseName.empty());
    ASSERT_BAIL(!dirName.empty());

    (void) local::cleanupString(&dirName,  d_allocator_p);
    frameRec->setCompileUnitDir(     dirName);
    frameRec->setCompileUnitFileName(baseName);

    bsl::string sfn(d_allocator_p);
    sfn.reserve(dirName.length() + 1 + baseName.length());
    if ('/' == baseName.c_str()[0]) {
        // 'baseName' is a full path, ignore dirName.

        e_TRACES && zprintf("Source file name: base name \"%s\" is"
                                         " full path\n", baseName.c_str());

        sfn = baseName;
    }
    else {
        if (e_TRACES && baseName.empty()) {
            zprintf("Source file name: dir name, no base name\n");
        }

        sfn = dirName;

        if (!sfn.empty() && '/' != sfn[dirName.length() - 1]) {
            sfn += '/';
        }

        sfn += baseName;
    }


    if (frameRec->frame().isSourceFileNameKnown()) {
        e_TRACES && zprintf("%s source file name found twice for frame %d,"
                    " expected if static.  Old sfn: %s, new sfn: %s\n", rn,
                             index, frameRec->frame().sourceFileName().c_str(),
                                                                  sfn.c_str());
    }
    else {
        e_TRACES && zprintf("%s source file name found for frame %d sfn: %s\n",
                                                       rn, index, sfn.c_str());
        rc = local::cleanupString(&sfn, d_allocator_p);
        if (0 == rc) {
            frameRec->frame().setSourceFileName(sfn);
        }
    }

    frameRec->setLineNumberOffset(lineNumberInfoOffset);

    return 0;
}

int local::StackTraceResolver::HiddenRec::dwarfReadDebugInfo()
{
    static const char rn[] = { "dwarfDebugInfo:" };    (void) rn;

    const local::FrameRecVecIt end = d_frameRecsEnd;
    for (local::FrameRecVecIt it = d_frameRecsBegin, prev = end; it < end;
                                                             prev = it, ++it) {
        // Because the 'FrameRec's are sorted by address, those referring to
        // the same compilation unit are going to be adjacent.

#if !BALST_DWARF_CHECK_ADDRESSES || 0 == TRACES
        // Disable this shortcut if checking addressses with traces on so we
        // can observe trace matching for each symbol.

        if (end != prev &&
                        prev->compileUnitOffset() == it->compileUnitOffset()) {
            e_TRACES && zprintf("%s frames %d and %d are from the same"
                                                     " compilation unit\n", rn,
                           frameIndex(prev->frame()), frameIndex(it->frame()));

            if (!it->frame().isSourceFileNameKnown()) {
                it->frame().setSourceFileName(prev->frame().sourceFileName());
            }
            it->setCompileUnitDir(        prev->compileUnitDir());
            it->setCompileUnitFileName(   prev->compileUnitFileName());
            it->setLineNumberOffset(      prev->lineNumberOffset());
            continue;
        }
#else
        (void) prev;
#endif

        int rc = dwarfReadDebugInfoFrameRec(&*it);
        if (0 != rc) {
            // The fact that we failed on one doesn't mean we'll fail on the
            // others -- keep going.

            e_TRACES && zprintf("%s dwarfReadDebugInfoFrameRec failed on"
                                   " frame %d\n", rn, frameIndex(it->frame()));
        }
    }

    return 0;
}

int local::StackTraceResolver::HiddenRec::dwarfReadDebugInfoFrameRec(
                                                     local::FrameRec *frameRec)
{
    static const char rn[] = { "dwarfDebugInfoFrameRec:" };    (void) rn;

    int rc;
    const int index = frameIndex(frameRec->frame()); (void) index;

    if (local::maxOffset == frameRec->compileUnitOffset()) {
        e_TRACES && zprintf("%s no compile unit offset for frame %d,"
                                                " can't proceed\n", rn, index);
        return -1;                                                    // RETURN
    }

    ASSERT_BAIL(0 < d_infoSec  .d_size);
    ASSERT_BAIL(0 < d_abbrevSec.d_size);
    ASSERT_BAIL(0 < d_rangesSec.d_size);
    ASSERT_BAIL(0 < d_strSec   .d_size);

    e_TRACES && zprintf("%s reading frame %d, symbol: %s\n", rn,
                                index, frameRec->frame().symbolName().c_str());

    rc = d_infoReader.init(  d_helper_p, d_scratchBufA_p, d_infoSec,
                                                            d_libraryFileSize);
    ASSERT_BAIL(0 == rc);
    rc = d_abbrevReader.init(d_helper_p, d_scratchBufB_p, d_abbrevSec,
                                                            d_libraryFileSize);
    ASSERT_BAIL(0 == rc);
    rc = d_rangesReader.init(d_helper_p, d_scratchBufC_p, d_rangesSec,
                                                            d_libraryFileSize);
    ASSERT_BAIL(0 == rc);
    rc = d_strReader.init(   d_helper_p, d_scratchBufD_p, d_strSec,
                                                            d_libraryFileSize);
    ASSERT_BAIL(0 == rc);

    rc = d_infoReader.skipTo(
                           d_infoSec.d_offset + frameRec->compileUnitOffset());
    ASSERT_BAIL(0 == rc && "skipTo failed");

    Offset compileUnitLength;
    rc = d_infoReader.readInitialLength(&compileUnitLength);
    ASSERT_BAIL(0 == rc);

    {
        unsigned short version;
        rc = d_infoReader.readValue(&version);
        ASSERT_BAIL(0 == rc && "read version failed");
        ASSERT_BAIL(4 == version || P(version));
    }

    // This is the compilation unit headers as outlined in 7.5.1.1

    Offset abbrevOffset;
    rc = d_infoReader.readSectionOffset(&abbrevOffset);
    ASSERT_BAIL(0 == rc && "read abbrev offset failed");

    rc = d_abbrevReader.skipTo(d_abbrevSec.d_offset + abbrevOffset);
    ASSERT_BAIL(0 == rc);

    rc = d_infoReader.readAddressSize();
    ASSERT_BAIL(0 == rc && "read address size failed");

    d_rangesReader.setAddressSize(d_infoReader);

    {
        Offset readTagIdx;

        // These tag indexes were barely, vaguely mentioned by the doc, with
        // some implication that they'd be '1' before the first tag we
        // encounter.  If they turn out not to be '1' in production it's
        // certainly not worth abandoning DWARF decoding over.

        rc = d_infoReader.readULEB128(&readTagIdx);
        ASSERT_BAIL(0 == rc);
        (1 != readTagIdx && e_TRACES) && eprintf( // we don't care much
                  "%s strange .debug_info tag idx %llx\n", rn, ll(readTagIdx));

        rc = d_abbrevReader.readULEB128(&readTagIdx);
        ASSERT_BAIL(0 == rc);
        (1 != readTagIdx && e_TRACES) && eprintf( // we don't care much
                "%s strange .debug_abbrev tag idx %llx\n", rn, ll(readTagIdx));
    }

    unsigned tag;
    rc = d_abbrevReader.readULEB128(&tag);
    ASSERT_BAIL(0 == rc);

    ASSERT_BAIL(DW_TAG_compile_unit == tag || DW_TAG_partial_unit == tag ||
                                                                      PH(tag));

    BSLMF_ASSERT(0 == DW_CHILDREN_no && 1 == DW_CHILDREN_yes);
    unsigned hasChildren;
    rc = d_abbrevReader.readULEB128(&hasChildren);
    ASSERT_BAIL(0 == rc);
    ASSERT_BAIL(hasChildren <= 1);    // other than that, we don't care

    rc = dwarfReadCompileOrPartialUnit(frameRec);
    ASSERT_BAIL(0 == rc);

    d_infoReader.  disable();
    d_abbrevReader.disable();
    d_rangesReader.disable();
    d_strReader.   disable();

    return 0;
}

#if 0
// When I started this, I misunderstood that 'DW_AT_decl_file' would give a
// file name, when in fact it gives the index into a table of file names in
// the line number information header.  Apparently, we'll get the file name
// for free when we parse the line number inforation, so it isn't worth the
// trouble to get this function compiling and debugged.

int local::StackTraceResolver::HiddenRec::dwarfReadForeignFunction(
                                                     local::FrameRec *frameRec)
{
    static const char rn[] = { "dwarfReadForeignFunction:" };    (void) rn;

    StackTraceFrame& frame = frameRef->frame();
    unsigned hasChildren = 0;
    bsl::string linkageName(d_allocator_p);

    int depth = 0;
    for (int ii = 2;; ++ii) {
        depth += hasChildren;

        unsigned tagIdx;
        {
            rc = d_infoReader.readULEB128(&tagIdx);
            ASSERT_BAIL(0 == rc);

            unsigned abbrevIdx;
            rc = d_abbrevReader.readULEB128(&abbrevIdx);
            ASSERT_BAIL(0 == rc);

            ASSERT_BAIL(tagIdx == abbrevIdx);

            if (0 == tagIdx) {
                if (depth-- <= 0) {
                    e_TRACES && eprintf("%s completed child traversal, no"
                                                " routine name match found\n");

                    return 0;
                }
            }

            // We aren't sure what to expect from the tagIdx -- it's only
            // mentioned in one example in the doc, and it's a tiny example
            // that doesn't cover a lot of cases.

            e_TRACES && ii != tagIdx && eprintf("%s unexpected tagIdx %u\n",
                                                                       tagIdx);
        }

        unsigned tag;
        rc = d_abbrevReader.readULEB128(&tag);
        ASSERT_BAIL(0 == rc);

        BSLMF_ASSERT(0 == DW_CHILDREN_no && 1 == DW_CHILDREN_yes);
        rc = d_abbrevReader.readULEB128(&hasChildren);
        ASSERT_BAIL(0 == rc);
        ASSERT_BAIL(hasChildren <= 1);

        e_TRACES && zprintf("%s tag: %s children: %u tagIdx: %u\n", rn,
                                  dwarfStringForTag(tag), hasChildren, tagIdx);

        if (DW_TAG_subprogram == tag) {
            declFile.clear();
            linkageName.clear();
            Offset declFile = local::maxOffset;
            Offset declLine = local::maxOffset;
            Offset lineNumberInfoOffset = local::maxOffset;
            unsigned char inlineState = 0xff;
            bool knownMatch = false, knownMisMatch = false;

            bool foundZeroes = false;
            for (;;) {
                unsigned attr;
                rc = d_abbrevReader.readULEB128(&attr);
                ASSERT_BAIL(0 == rc && "reading attr");

                unsigned form;
                rc = d_abbrevReader.readULEB128(&form);
                ASSERT_BAIL(0 == rc && "reading form");

                if (0 == attr) {
                    ASSERT_BAIL(0 == form);
                    foundZeroes = true;
                    break;
                }

                zprintf("%s%s attr: %s form: %s\n", rn,
                             knownMisMatch ? " skipping" : "",
                             dwarfStringForAt(attr), dwarfStringForFrom(form));

                if (knownMisMatch) {
                    rc = d_infoReader.skipForm(form);
                    ASSERT_BAIL(0 == rc);

                    continue;
                }

                switch (attr) {
                  case DW_AT_decl_file: {
                    ASSERT_BAIL(local::maxOffset == declFile);

                    rc = d_infoReader.readOffsetFromForm(&declFile, form);
                    ASSERT_BAIL(0 == rc);
                    ASSERT_BAIL(0 <= declFile);
                    ASSERT_BAIL(     declFile < INT_MAX);

                    e_TRACES && zprintf("%s declFile %llu found\n",
                                                             rn, ll(declFile));
                  } break;
                  case DW_AT_decl_line: {
                    ASSERT_BAIL(local::maxOffset == declLine);

                    d_infoReader.readOffsetFromForm(&declLine, form);
                    ASSERT_BAIL(0 < declLine);
                    ASSERT_BAIL(    declLine < INT_MAX);
                  } break;
                  case DW_AT_inline: {
                    ASSERT_BAIL(DW_FORM_udata = form || DW_FORM_data1 = form);

                    rc = d_infoReader.readULEB128(&inlineState);
                    ASSERT_BAIL(0 == rc);
                    ASSERT_BAIL(inlineState < 4);
                  } break;
                  case DW_AT_linkage_name: {
                    ASSERT_BAIL(linkageName.empty());
                    ASSERT_BAIL(!knownMatch && !knownMisMatch);

                    rc = d_infoReader.readStringFromForm(&linkageName,
                                                         &d_strReader,
                                                         form);
                    ASSERT_BAIL(0 == rc);

                    if (linkageName == frame.mangledSymbolName()) {
                        knownMatch = true;
                    }
                    else {
                        knownMisMatch = true;
                    }

                    e_TRACES && zprintf("%s routine %s\n", rn, knownMatch
                                                               ? "match!!!!!"
                                                               : "misMatch");
                  } break;
                  case DW_AT_stmt_list: {    // This is not in the doc, but it
                                             // would be *VERY* useful if it
                                             // turns out to be there.
                    rc = d_infoReader.readOffsetFromForm(&lineNumberOffset,
                                                         form);
                    ASSERT_BAIL(0 == rc);

                    e_TRACES && zprintf("%s found statement list\n", rn);
                  } break;
                  default: {
                    rc = d_infoReader.skipForm(form);
                    ASSERT_BAIL(0 == rc || PH(attr) || PH(form));
                  }
                }
            }

            BSLS_ASSERT(foundZeroes);

            if (knownMatch) {
                if (declFile.empty()) {
                    // No declFile statement, probebly means the decl file is
                    // the compile unit file.

                    return 0;
                }

                if (0 == declFile) {
                    // The routine is in the compile unit file, we already have
                    // the right file name and the line number info should turn
                    // out correct when we look it up.

                    return 0;                                         // RETURN
                }

                zprintf("%s found foreign function %s in file %llu,"
                                          " line %llu, inlineState 0x%s\n", rn,
                        frame.symbolName().c_str(),
                        ll(declFile),
                        ll(declLine),
                        0xff == inlineState
                                    ? "0xff"
                                    : dwarfStringForInlineState(inlineState));

                frameRec.setDeclFile(static_cast<unsigned>(declFile));
                frame.setSourceFileName(declFile);

                ASSERT_BAIL(-1 == frame.lineNumber());

                if (local::maxOffset != lineNumberInfoOffset) {
                    frameRec->setLineNumberOffset(lineNumberInfoOffset);
                }
                else if (local::maxOffset != declLine) {
                    // Set the line number to the function declaration line,
                    // better than nothing.

                    frame.setLineNumber(static_cast<int>(declLine));

                    // At the time of this writing, we don't know whether the
                    // line number info in the compile unit will yield the
                    // correct line number for a function in a file other than
                    // the compile unit file.
                    //
                    // frameRec->setLineNumberOffset(local::maxoffset);
                }

                return 0;                                             // RETURN
            }
        }
        else {
            // It's not a subprogram tag.  Just go through what we find.

            bool foundZeroes = false;
            for (;;) {
                unsigned attr;
                rc = d_abbrevReader.readULEB128(&attr);
                ASSERT_BAIL(0 == rc && "reading attr");

                unsigned form;
                rc = d_abbrevReader.readULEB128(&form);
                ASSERT_BAIL(0 == rc && "reading form");

                if (0 == attr) {
                    ASSERT_BAIL(0 == form);
                    foundZeroes = true;
                    break;
                }

                e_TRACES && zprintf("%s skipping child: attr: %s form: %s\n",
                         rn, dwarfStringForAt(attr), dwarfStringForForm(form));

                rc = d_infoReader.skipForm(form);
                ASSERT_BAIL(0 == rc);
            }

            ASSERT_BAIL(foundZeroes);
        }
    }

    ASSERT_BAIL(0 && "unreachable statement");
}
#endif // 0
#endif // BALST_DWARF

void local::StackTraceResolver::HiddenRec::reset()
{
    // Note that 'd_frameRecs' and 'd_numTotalUnmatched' are not to be cleared
    // or reinitialized, they have a lifetime of the length of the resolve.

    d_helper_p           = 0;
    d_frameRecsBegin     = local::FrameRecVecIt();
    d_frameRecsEnd       = local::FrameRecVecIt();
    d_adjustment         = 0;
    d_symTableSec.reset();
    d_stringTableSec.reset();
#ifdef BALST_DWARF
    d_abbrevSec. reset();
    d_arangesSec.reset();
    d_infoSec.   reset();
    d_lineSec.   reset();
    d_rangesSec. reset();
    d_strSec.    reset();

    d_abbrevReader. disable();
    d_arangesReader.disable();
    d_infoReader.   disable();
    d_lineReader.   disable();
    d_rangesReader. disable();
    d_strReader.    disable();
#endif
}

int local::StackTraceResolver::HiddenRec::frameIndex(
                                     const balst::StackTraceFrame& frame) const
{
    return static_cast<int>(&frame - &(*d_stackTrace_p)[0]);
}

     // -----------------------------------------------------------------
     // class balst::StackTraceResolverImpl<balst::ObjectFileFormat::Elf>
     //                 == class U::StackTraceResolver
     // -----------------------------------------------------------------

// PRIVATE CREATORS
local::StackTraceResolver::StackTraceResolverImpl(
                                    balst::StackTrace *stackTrace,
                                    bool               demanglingPreferredFlag)
: d_hbpAlloc()
, d_stackTrace_p(stackTrace)
, d_scratchBufA_p(static_cast<char *>(
                                d_hbpAlloc.allocate(local::k_SCRATCH_BUF_LEN)))
, d_scratchBufB_p(static_cast<char *>(
                                d_hbpAlloc.allocate(local::k_SCRATCH_BUF_LEN)))
, d_scratchBufC_p(static_cast<char *>(
                                d_hbpAlloc.allocate(local::k_SCRATCH_BUF_LEN)))
, d_scratchBufD_p(static_cast<char *>(
                                d_hbpAlloc.allocate(local::k_SCRATCH_BUF_LEN)))
, d_hidden(*(new (d_hbpAlloc) HiddenRec(this)))    // must be after scratch
                                                   // buffers
, d_demangle(demanglingPreferredFlag)
{
}

local::StackTraceResolver::~StackTraceResolverImpl()
{
}

// PRIVATE MANIPULATORS
int local::StackTraceResolver::loadSymbols(int matched)
{
    char *symbolBuf = d_scratchBufA_p;
    char *stringBuf = d_scratchBufB_p;

    const int           symSize = static_cast<int>(sizeof(local::ElfSymbol));
    const local::Offset maxSymbolsPerPass = local::k_SCRATCH_BUF_LEN / symSize;
    const local::Offset numSyms = d_hidden.d_symTableSec.d_size / symSize;
    local::Offset       sourceFileNameOffset = local::maxOffset;

    UintPtr            numSymsThisTime;
    for (local::Offset symIndex = 0; symIndex < numSyms;
                                                 symIndex += numSymsThisTime) {
        numSymsThisTime = static_cast<UintPtr>(
                              bsl::min(numSyms - symIndex, maxSymbolsPerPass));

        const local::Offset offsetToRead = d_hidden.d_symTableSec.d_offset +
                                                            symIndex * symSize;
        int          rc = d_hidden.d_helper_p->readExact(
                                                     symbolBuf,
                                                     numSymsThisTime * symSize,
                                                     offsetToRead);
        if (rc) {
            eprintf("failed to read %lu symbols from offset %llu, errno %d\n",
                    l(numSymsThisTime),
                    ll(offsetToRead),
                    errno);
            return -1;                                                // RETURN
        }

        const local::ElfSymbol *symBufStart = static_cast<local::ElfSymbol *>(
                                               static_cast<void *>(symbolBuf));
        const local::ElfSymbol *symBufEnd   = symBufStart + numSymsThisTime;
        for (const local::ElfSymbol *sym = symBufStart; sym < symBufEnd;
                                                                       ++sym) {
            switch (ELF32_ST_TYPE(sym->st_info)) {
              case STT_FILE: {
                sourceFileNameOffset = sym->st_name;
              } break;
              case STT_FUNC: {
                if (SHN_UNDEF != sym->st_shndx) {
                    const void *symbolAddress = reinterpret_cast<const void *>(
                                        sym->st_value + d_hidden.d_adjustment);
                    const void *endSymbolAddress =
                                     static_cast<const char *>(symbolAddress) +
                                                                  sym->st_size;
                    const local::FrameRecVecIt begin =
                                bsl::lower_bound(d_hidden.d_frameRecsBegin,
                                                 d_hidden.d_frameRecsEnd,
                                                 local::FrameRec(symbolAddress,
                                                                 0,
                                                                 &d_hbpAlloc));
                    const local::FrameRecVecIt end =
                             bsl::lower_bound(d_hidden.d_frameRecsBegin,
                                              d_hidden.d_frameRecsEnd,
                                              local::FrameRec(endSymbolAddress,
                                                              0,
                                                              &d_hbpAlloc));
                    for (local::FrameRecVecIt it = begin; it < end; ++it) {
                        if (it->isSymbolResolved()) {
                            continue;
                        }

                        balst::StackTraceFrame& frame = it->frame();

                        frame.setOffsetFromSymbol(
                                  static_cast<const char *>(it->address())
                                   - static_cast<const char *>(symbolAddress));

                        // in ELF, filename information is only accurate
                        // for statics in the main executable

                        if (d_hidden.d_isMainExecutable
                           && STB_LOCAL == ELF32_ST_BIND(sym->st_info)
                           && local::maxOffset != sourceFileNameOffset) {
                            frame.setSourceFileName(
                                      d_hidden.d_helper_p->loadString(
                                           d_hidden.d_stringTableSec.d_offset +
                                                          sourceFileNameOffset,
                                           stringBuf,
                                           local::k_SCRATCH_BUF_LEN,
                                           &d_hbpAlloc));
                        }

                        frame.setMangledSymbolName(
                                  d_hidden.d_helper_p->loadString(
                                           d_hidden.d_stringTableSec.d_offset +
                                                                  sym->st_name,
                                           stringBuf,
                                           local::k_SCRATCH_BUF_LEN,
                                           &d_hbpAlloc));
                        if (frame.isMangledSymbolNameKnown()) {
                            setFrameSymbolName(&frame);

                            it->setSymbolResolved();

                            e_TRACES && zprintf(
                                    "Resolved symbol %s, frame %d, [%p, %p)\n",
                                    frame.symbolName().c_str(),
                                    d_hidden.frameIndex(frame),
                                    symbolAddress,
                                    endSymbolAddress);

                            if (0 == --matched) {
                                e_TRACES && zprintf(
                                            "Last symbol in segment loaded\n");

                                return 0;                             // RETURN
                            }
                        }
                        else {
                            e_TRACES && zprintf("Null symbol found for %p\n",
                                                                it->address());
                        }
                    }
                }
              }  break;
            }
        }
    }

    return 0;
}

int local::StackTraceResolver::processLoadedImage(
                                                 const char *fileName,
                                                 const void *programHeaders,
                                                 int         numProgramHeaders,
                                                 void       *textSegPtr,
                                                 void       *baseAddress)
    // Note this must be public so 'linkMapCallBack' can call it on Solaris.
    // Also note that it assumes that both scratch buffers are available for
    // writing.
{
    static const char rn[] = { "processLoadedImage" };    (void) rn;

    BSLS_ASSERT(!textSegPtr || !baseAddress);

    // Scratch buffers: some platforms use A to read the link, then
    // 'resolveSegment' will trash both A and B.

    d_hidden.reset();

#if defined(BSLS_PLATFORM_OS_HPUX)
    const char *name = fileName;
#else
    const char *name = 0;
    if (fileName && fileName[0]) {
        if (local::e_IS_LINUX) {
            d_hidden.d_isMainExecutable = false;
        }

        name = fileName;
    }
    else {
        if (local::e_IS_LINUX) {
            d_hidden.d_isMainExecutable = true;
        }
        else {
            ASSERT_BAIL(d_hidden.d_isMainExecutable);
        }

        // On Solaris and Linux, 'fileName' is sometimes null for the main
        // executable file, but those platforms have a standard virtual symlink
        // that points to the executable file name.

        const int numChars = static_cast<int>(
                                           readlink("/proc/self/exe",
                                                    d_scratchBufA_p,
                                                    local::k_SCRATCH_BUF_LEN));
        if (numChars > 0) {
            ASSERT_BAIL(numChars < local::k_SCRATCH_BUF_LEN);
            d_scratchBufA_p[numChars] = 0;
            name = d_scratchBufA_p;
        }
        else {
            e_TRACES && zprintf("readlink of /proc/self/exe failed\n");

            return -1;                                                // RETURN
        }
    }
#endif
    name = bdlb::String::copy(name, &d_hbpAlloc);   // so we can trash the
                                                    // scratch buffers later

    e_TRACES && zprintf("processing loaded image: fn:\"%s\", name:\"%s\""
                                         " main:%d numHdrs:%d unmatched:%ld\n",
                        fileName ? fileName : "(null)", name ? name : "(null)",
                                 static_cast<int>(d_hidden.d_isMainExecutable),
                            numProgramHeaders, l(d_hidden.d_frameRecs.size()));

    balst::StackTraceResolver_FileHelper helper(name);
    d_hidden.d_helper_p = &helper;

    for (int i = 0; i < numProgramHeaders; ++i) {
        const local::ElfProgramHeader *ph =
              static_cast<const local::ElfProgramHeader *>(programHeaders) + i;
        // if (ph->p_type == PT_LOAD && ph->p_offset == 0) {

        if    (PT_LOAD == ph->p_type) {
            // on Linux, textSegPtr will be 0, on Solaris && HPUX, baseAddress
            // will be 0.  We will always have 1 of the two, and since they
            // differ by ph->p_vaddr, we can always calculate the one we don't
            // have.

            if (textSegPtr) {
                BSLS_ASSERT(0 == baseAddress);

                // calculating baseAddress from textSegPtr

                baseAddress = static_cast<char *>(textSegPtr) - ph->p_vaddr;
            }
            else {
                // or the other way around

                textSegPtr = static_cast<char *>(baseAddress) + ph->p_vaddr;
            }

            // 'resolveSegment' trashes scratch buffers A and B

            int rc = resolveSegment(baseAddress,    // base address
                                    textSegPtr,     // seg ptr
                                    ph->p_memsz,    // seg size
                                    name);          // file name
            if (rc) {
                return -1;                                            // RETURN
            }

            return 0;                                                 // RETURN
        }
    }

    return -1;
}

int local::StackTraceResolver::resolveSegment(void       *segmentBaseAddress,
                                              void       *segmentPtr,
                                              UintPtr     segmentSize,
                                              const char *libraryFileName)
{
    int rc;

    // Scratch Buffers: beginning: 'sec' is A
    // Then 'loadSymbols' trashes both A and B
    // Then 'readDwarfAll' trasnes both A and B

    d_hidden.d_addressRange.d_address = reinterpret_cast<UintPtr>(segmentPtr);
    d_hidden.d_addressRange.d_size    = segmentSize;

    const char *sp = static_cast<char *>(segmentPtr);
    const char *se = sp + segmentSize;

    d_hidden.d_frameRecsBegin = bsl::lower_bound(d_hidden.d_frameRecs.begin(),
                                                 d_hidden.d_frameRecs.end(),
                                                 local::FrameRec(sp,
                                                                 0,
                                                                 &d_hbpAlloc));
    d_hidden.d_frameRecsEnd   = bsl::lower_bound(d_hidden.d_frameRecs.begin(),
                                                 d_hidden.d_frameRecs.end(),
                                                 local::FrameRec(se,
                                                                 0,
                                                                 &d_hbpAlloc));

    int matched = static_cast<int>(
                          d_hidden.d_frameRecsEnd - d_hidden.d_frameRecsBegin);
    BSLS_ASSERT(0 <= matched);
    BSLS_ASSERT(matched <= d_stackTrace_p->length());

    e_TRACES && zprintf(
                       "ResolveSegment lfn=%s\nba=%p sp=%p se=%p matched=%d\n",
                         libraryFileName, segmentBaseAddress, sp, se, matched);

    if (0 == matched) {
        e_TRACES && zprintf(
                         "0 addresses match in library %s\n", libraryFileName);

        return 0;                                                     // RETURN
    }

    d_hidden.d_libraryFileSize = bdls::FilesystemUtil::getFileSize(
                                                              libraryFileName);

    bsl::string libName(libraryFileName, &d_hbpAlloc);
    int cleanupRc = local::cleanupString(&libName, &d_hbpAlloc);

    local::FrameRecVecIt it, end  = d_hidden.d_frameRecsEnd;
    for (it = d_hidden.d_frameRecsBegin; it < end; ++it) {
        e_TRACES && zprintf("address %p MATCH\n", it->address());
        it->frame().setLibraryFileName(0 == cleanupRc ? libName.c_str()
                                                      : libraryFileName);
    }

    // read the elf header

    local::ElfHeader elfHeader;
    rc = d_hidden.d_helper_p->readExact(&elfHeader,
                                        sizeof(local::ElfHeader),
                                        0);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    if (0 != checkElfHeader(&elfHeader)) {
        return -1;                                                    // RETURN
    }

    d_hidden.d_adjustment = reinterpret_cast<UintPtr>(segmentBaseAddress);

    // find the section headers we're interested in, that is, .symtab and
    // .strtab, or, if the file was stripped, .dynsym and .dynstr

    local::ElfSectionHeader symTabHdr, strTabHdr, dynSymHdr, dynStrHdr;
    bsl::memset(&symTabHdr, 0, sizeof(local::ElfSectionHeader));
    bsl::memset(&strTabHdr, 0, sizeof(local::ElfSectionHeader));
    bsl::memset(&dynSymHdr, 0, sizeof(local::ElfSectionHeader));
    bsl::memset(&dynStrHdr, 0, sizeof(local::ElfSectionHeader));

    // Possible speedup: read all the section headers at once instead of one at
    // a time.

    int     numSections = elfHeader.e_shnum;
    UintPtr sectionHeaderSize = elfHeader.e_shentsize;
    UintPtr sectionHeaderOffset = elfHeader.e_shoff;
    if (local::k_SCRATCH_BUF_LEN < sectionHeaderSize) {
        return -1;                                                    // RETURN
    }
    local::ElfSectionHeader *sec = static_cast<local::ElfSectionHeader *>(
                                         static_cast<void *>(d_scratchBufA_p));

    // read the string table that is used for section names

    int     stringSectionIndex = elfHeader.e_shstrndx;
    UintPtr stringSectionHeaderOffset =
                  sectionHeaderOffset + stringSectionIndex * sectionHeaderSize;
    if (0 != d_hidden.d_helper_p->readExact(sec,
                                            sectionHeaderSize,
                                            stringSectionHeaderOffset)) {
        return -1;                                                    // RETURN
    }
    UintPtr headerStringsOffset = sec->sh_offset;

    for (int i = 0; i < numSections; ++i) {
        if (0 != d_hidden.d_helper_p->readExact(sec,
                                                sectionHeaderSize,
                                                sectionHeaderOffset +
                                                i * sectionHeaderSize)) {
            return -1;                                                // RETURN
        }
        char sectionName[16];
        if (0 !=  d_hidden.d_helper_p->readExact(sectionName,
                                                 sizeof(sectionName),
                                                 headerStringsOffset +
                                                               sec->sh_name)) {
            return -1;                                                // RETURN
        }

        // zprintf("Section: type:%d name:%s\n", sec->sh_type, sectionName);

        switch (sec->sh_type) {
          case SHT_STRTAB: {
            if      (!bsl::strcmp(sectionName, ".strtab")) {
                strTabHdr = *sec;
            }
            else if (!bsl::strcmp(sectionName, ".dynstr")) {
                dynStrHdr = *sec;
            }
          } break;
          case SHT_SYMTAB: {
            if      (!bsl::strcmp(sectionName, ".symtab")) {
                symTabHdr = *sec;
            }
          } break;
          case SHT_DYNSYM: {
            if      (!bsl::strcmp(sectionName, ".dynsym")) {
                dynSymHdr = *sec;
            }
          } break;
#ifdef BALST_DWARF
          case SHT_PROGBITS: {
            if ('d' != sectionName[1]) {
                ; // do nothing
            }
            else if (!bsl::strcmp(sectionName, ".debug_abbrev")) {
                d_hidden.d_abbrevSec. reset(sec->sh_offset, sec->sh_size);
            }
            else if (!bsl::strcmp(sectionName, ".debug_aranges")) {
                d_hidden.d_arangesSec.reset(sec->sh_offset, sec->sh_size);
            }
            else if (!bsl::strcmp(sectionName, ".debug_info")) {
                d_hidden.d_infoSec.   reset(sec->sh_offset, sec->sh_size);
            }
            else if (!bsl::strcmp(sectionName, ".debug_line")) {
                d_hidden.d_lineSec.   reset(sec->sh_offset, sec->sh_size);
            }
            else if (!bsl::strcmp(sectionName, ".debug_ranges")) {
                d_hidden.d_rangesSec. reset(sec->sh_offset, sec->sh_size);
            }
            else if (!bsl::strcmp(sectionName, ".debug_str")) {
                d_hidden.d_strSec.    reset(sec->sh_offset, sec->sh_size);
            }
          } break;
#endif
        }
    }

    e_TRACES && zprintf("symtab:(0x%llx, 0x%llx), strtab:(0x%llx, 0x%llx)\n",
            ll(symTabHdr.sh_offset), ll(symTabHdr.sh_size),
            ll(strTabHdr.sh_offset), ll(strTabHdr.sh_size));
    e_TRACES && zprintf("dynsym:(0x%llx, %llu), dynstr:(0x%llx, %llu)\n",
            ll(dynSymHdr.sh_offset), ll(dynSymHdr.sh_size),
            ll(dynStrHdr.sh_offset), ll(dynStrHdr.sh_size));

    if (0 != strTabHdr.sh_size && 0 != symTabHdr.sh_size) {
        // use the full symbol table if it is available

        d_hidden.d_symTableSec.   reset(symTabHdr.sh_offset,
                                        symTabHdr.sh_size);
        d_hidden.d_stringTableSec.reset(strTabHdr.sh_offset,
                                        strTabHdr.sh_size);
    }
    else if (0 != dynSymHdr.sh_size && 0 != dynStrHdr.sh_size) {
        // otherwise use the dynamic symbol table

        d_hidden.d_symTableSec.   reset(dynSymHdr.sh_offset,
                                        dynSymHdr.sh_size);
        d_hidden.d_stringTableSec.reset(dynStrHdr.sh_offset,
                                        dynStrHdr.sh_size);
    }
    else {
        // otherwise fail

        return -1;                                                    // RETURN
    }

    e_TRACES && zprintf(
                   "Sym table:(0x%llx, 0x%llx) string table:(0x%llx 0x%llx)\n",
            ll(d_hidden.d_symTableSec.d_offset),
            ll(d_hidden.d_symTableSec.d_size),
            ll(d_hidden.d_stringTableSec.d_offset),
            ll(d_hidden.d_stringTableSec.d_size));

#ifdef BALST_DWARF
    e_TRACES && zprintf("abbrev:(0x%llx, 0x%llx) aranges:(0x%llx, 0x%llx)"
            " info:(0x%llx 0x%llx) line::(0x%llx 0x%llx)"
            " ranges:(0x%llx, 0x%llx) str:(0x%llx, 0x%llx)\n",
            ll(d_hidden.d_abbrevSec.d_offset),
            ll(d_hidden.d_abbrevSec.d_size),
            ll(d_hidden.d_arangesSec.d_offset),
            ll(d_hidden.d_arangesSec.d_size),
            ll(d_hidden.d_infoSec.d_offset),
            ll(d_hidden.d_infoSec.d_size),
            ll(d_hidden.d_lineSec.d_offset),
            ll(d_hidden.d_lineSec.d_size),
            ll(d_hidden.d_rangesSec.d_offset),
            ll(d_hidden.d_rangesSec.d_size),
            ll(d_hidden.d_strSec.d_offset),
            ll(d_hidden.d_strSec.d_size));
#endif

    // Note that 'loadSymbols' trashes scratchBufA and scratchBufB.

    rc = loadSymbols(matched);
    if (rc) {
        eprintf("loadSymbols failed\n");
        return -1;                                                    // RETURN
    }

    // we return 'rc' at the end.

#ifdef BALST_DWARF
    // Note that 'readDwarfAll' trashes scratchBufA and scratchBufB.

    rc = d_hidden.dwarfReadAll();
    if (rc) {
        e_TRACES && zprintf("readDwarf failed\n");
    }
#endif

    if (e_TRACES && 0 == (d_hidden.d_numTotalUnmatched -= matched)) {
        zprintf("Last address in stack trace matched\n");
    }

    return 0;
}

// PRIVATE ACCESSORS
void local::StackTraceResolver::setFrameSymbolName(
                                           balst::StackTraceFrame *frame) const
{
#if !defined(BSLS_PLATFORM_OS_SOLARIS)                                        \
 || defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    // Linux or Sun g++ or HPUX

    int   status = -1;
    char *demangledSymbol = 0;
    if (d_demangle) {
        // note the demangler uses 'malloc' to allocate its result

#if defined(BSLS_PLATFORM_OS_HPUX)
        demangledSymbol = __cxa_demangle(frame->mangledSymbolName().c_str(),
                                         0,
                                         0,
                                         &status);
#else
        demangledSymbol = abi::__cxa_demangle(
                                            frame->mangledSymbolName().c_str(),
                                            0,
                                            0,
                                            &status);
#endif
    }
    if (0 == status) {
        e_TRACES && zprintf("Demangled to: %s\n", demangledSymbol);
        frame->setSymbolName(demangledSymbol);
    }
    else {
        e_TRACES && zprintf("Did not demangle: status: %d\n", status);
        frame->setSymbolName(frame->mangledSymbolName());
    }
    if (demangledSymbol) {
        bsl::free(demangledSymbol);
    }
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)                                         \
 && !(defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
    // Sun CC only -- no demangling is available

    frame->setSymbolName(frame->mangledSymbolName());
#endif
}

#if defined(BSLS_PLATFORM_OS_LINUX)
// Linux could use the same method as Solaris, but we would need a special case
// for statically linked apps.  Instead of that we're going to use the
// 'dl_iterate_phdr' function, which works for static and dynamic apps (you get
// called back once if the app is static).

extern "C" {

static
int linkmapCallback(struct dl_phdr_info *info,
                    size_t               size,
                    void                *data)
    // This routine is called once for the executable file, and once for every
    // shared library that is loaded.  The specified 'info' contains a pointer
    // to the relevant information in the link map.  The specified 'size' is
    // unused, the specified 'data' is a pointer to the elf resolver.  Return 0
    // on success and a non-zero value otherwise.
{
    (void) size;

    local::StackTraceResolver *resolver =
                           reinterpret_cast<local::StackTraceResolver *>(data);

    // If we have completed resolving, there is no way to signal the caller to
    // stop iterating through the shared libs, but we aren't allowed to throw
    // and the caller ignores and propagates the return value we pass to it.
    // So just return without doing any work once resolving is done.

    if (0 == resolver->numUnmatchedFrames()) {
        return 0;                                                     // RETURN
    }

    // here the base address is known and text segment loading address is
    // unknown

    int rc = resolver->processLoadedImage(
                                    info->dlpi_name,
                                    info->dlpi_phdr,
                                    info->dlpi_phnum,
                                    0,
                                    reinterpret_cast<void *>(info->dlpi_addr));
    if (rc) {
        return -1;                                                    // RETURN
    }

    return 0;
}

}  // extern "C"

#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)

// modern Solaris applications are NEVER statically linked, so we always
// have a link_map.

extern "C" void *_DYNAMIC;    // global pointer that leads to the link map

#endif

// CLASS METHODS
int local::StackTraceResolver::resolve(
                                    balst::StackTrace *stackTrace,
                                    bool               demanglingPreferredFlag)
{
    static const char rn[] = { "resolve" };    (void) rn;

#if defined(BSLS_PLATFORM_OS_HPUX)

    int rc;

    local::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);

    // The HPUX compiler, 'aCC', doesn't accept the -Bstatic option, suggesting
    // we are never statically linked on HPUX, so 'shl_get_r' should always
    // work.

    shl_descriptor desc;
    bsl::memset(&desc, 0, sizeof(shl_descriptor));

    // 'programHeaders' will point to a segment of memory we will allocate and
    // reallocated to the needed size indicated by the 'ElfHeader's we
    // encounter.  The max is the number of program headers that will fit in
    // the allcoated segment.

    local::ElfProgramHeader *programHeaders = 0;
    int maxNumProgramHeaders = 0;

    local::ElfHeader elfHeader;
    for (int i = -1;
                0 < resolver.numUnmatchedFrames() && -1 != shl_get_r(i, &desc);
                                                                         ++i) {
        int numProgramHeaders = 0;

        {
            // this block limits the lifetime of 'helper' below

            e_TRACES && zprintf("(%d) %s 0x%lx-0x%lx\n",
                    i,
                    desc.filename && desc.fileName[0] ? desc.fileName :"(null)"
                    desc.tstart,
                    desc.tend);

            // index 0 is for the main executable

            resolver.d_hidden.d_isMainExecutable = (0 == i);

            // note this will be opened twice, here and in 'processLoadedImage'

            balst::StackTraceResolver_FileHelper helper(desc.filename);

            rc = helper.readExact(&elfHeader, sizeof(elfHeader), 0);
            ASSERT_BAIL(0 == rc);

            numProgramHeaders = elfHeader.e_phnum;
            if (numProgramHeaders > maxNumProgramHeaders) {
                programHeaders = static_cast<local::ElfProgramHeader *>(
                       resolver.d_hbpAlloc.allocate(
                         numProgramHeaders * sizeof(local::ElfProgramHeader)));
                maxNumProgramHeaders = numProgramHeaders;
            }

            rc = helper.readExact(
                           programHeaders,
                           numProgramHeaders * sizeof(local::ElfProgramHeader),
                           elfHeader.e_phoff);
            ASSERT_BAIL(0 == rc);
        }

        rc = resolver.processLoadedImage(
                                   desc.filename,
                                   programHeaders,
                                   numProgramHeaders,
                                   static_cast<void *>(desc.tstart),
                                   0);
        ASSERT_BAIL(0 == rc);
    }

#elif defined(BSLS_PLATFORM_OS_LINUX)

    local::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);

    // 'dl_iterate_phdr' will iterate over all loaded files, the executable and
    // all the .so's.  It doesn't exist on Solaris.

    dl_iterate_phdr(&linkmapCallback,
                    &resolver);

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    local::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);

    struct link_map *linkMap = 0;

#if 0
    // This method of getting the linkMap was deemed less desirable because it
    // calls 'malloc'.

    dlinfo(RTLD_SELF, RTLD_DI_LINKMAP, &linkMap);
    if (0 == linkMap) {
        return -1;                                                    // RETURN
    }
#else
    // This method was adopted as superior to the above (commented out) method.

    local::ElfDynamic *dynamic = reinterpret_cast<local::ElfDynamic *>(
                                                                    &_DYNAMIC);
    ASSERT_BAIL(dynamic);

    for (; true; ++dynamic) {
        // DT_NULL means we reached then end of list without finding the link
        // map

        ASSERT_BAIL(DT_NULL != dynamic->d_tag);

        if (DT_DEBUG == dynamic->d_tag) {
            r_debug *rdb = reinterpret_cast<r_debug *>(dynamic->d_un.d_ptr);
            ASSERT_BAIL(0 != rdb);

            linkMap = rdb->r_map;
            break;
        }
    }
#endif

    for (int i = 0; 0 < resolver.numUnmatchedFrames() && linkMap;
                                              ++i, linkMap = linkMap->l_next) {
        local::ElfHeader *elfHeader = reinterpret_cast<local::ElfHeader *>(
                                                              linkMap->l_addr);

        if (0 != checkElfHeader(elfHeader)) {
            return -1;
        }

        local::ElfProgramHeader *programHeaders =
            static_cast<local::ElfProgramHeader *>(static_cast<void *>(
                reinterpret_cast<char *>(elfHeader) + elfHeader->e_phoff));
        int numProgramHeaders = elfHeader->e_phnum;

        resolver.d_hidden.d_isMainExecutable = (0 == i);

        // Here the text segment address is known, not base address.  On this
        // platform, but not necessarily on other platforms, the text segment
        // begins with the Elf Header.

        int rc = resolver.processLoadedImage(linkMap->l_name,
                                             programHeaders,
                                             numProgramHeaders,
                                             static_cast<void *>(elfHeader),
                                             0);
        if (rc) {
            return -1;                                                // RETURN
        }
    }

#else
# error unrecognized platform
#endif

    return 0;
}

// PUBLIC ACCESSOR
int local::StackTraceResolver::numUnmatchedFrames() const
{
    return static_cast<int>(d_hidden.d_numTotalUnmatched);
}

}  // close enterprise namespace

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
