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

#include <balst_stacktraceresolver_dwarfreader.h>
#include <balst_stacktraceresolver_filehelper.h>

#include <bdlb_string.h>
#include <bdls_filesystemutil.h>

#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cerrno.h>
#include <bsl_cstring.h>
#include <bsl_climits.h>
#include <bsl_cstdarg.h>
#include <bsl_deque.h>
#include <bsl_vector.h>

#include <elf.h>
#include <sys/types.h>    // lstat
#include <sys/stat.h>     // lstat
#include <unistd.h>

#undef u_DWARF
#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DWARF)
# define u_DWARF 1
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
# else
#   include <dlfcn.h>
# endif

#else

# error unrecognized ELF platform

#endif

// 'u_' PREFIX:
// We have many types, static functions and macros defined in this file.  Prior
// to when we were using package namespaces, all global types and functions
// began with the package prefix, so the reader saw a type, macro, or function
// without the package prefix they knew it was probably local to the file.  Now
// that we have package prefixes, this is no longer the case, leading to
// confusion.  Hence, local definitions at file scope begin with 'u_', lending
// considerable clarity.
//
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
// } u_Elf32_Ehdr;
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
// } u_Elf32_Phdr;
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
// } u_Elf32_Shdr;
//
// typedef struct
// {
//     Elf32_Word    st_name;               //  Symbol name (string tbl index)
//     Elf32_Addr    st_value;              //  Symbol value
//     Elf32_Word    st_size;               //  Symbol size
//     unsigned char st_info;               //  Symbol type and binding
//     unsigned char st_other;              //  Symbol visibility
//     Elf32_Section st_shndx;              //  Section index - 16-bit
// } u_Elf32_Sym;
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
// } u_Elf32_Dyn;
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
// given for the structs are the names of typedefs to them in the unnamed
// namespace within this file.  Significantly, data members not used in this
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
// } u_ElfHeader;
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
// } u_ElfProgramHeader;
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
// } u_ElfSectionHeader;
//
// typedef struct
// {
//     unsigned int  st_name;               //  Symbol name (string tbl index)
//     UintPtr       st_value;              //  Symbol value
//     unsigned int  st_size;               //  Symbol size
//     unsigned char st_info;               //  Symbol type and binding
// } u_ElfSymbol;
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
//     u_ElfDynamic        *l_ld;           // dynamic structure of object
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
// } u_ElfDynamic;
//
// Tag values
//
// #define    DT_NULL      0                //  last entry in list
// #define    DT_DEBUG    21                //  pointer to 'r_debug' structure
//..
// The '_DYNAMIC' symbol is the address of the beginning of an array of objects
// of type 'u_ElfDynamic', one of which contains a pointer to the
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
//     const u_ElfProgramHeader         *dlpi_phdr;    // array of program
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
// ============================================================================
//
//                                  // -----
//                                  // DWARF
//                                  // -----
//
// The information in ELF format only tells us:
//: o function names
//: o source file names, but only in the case of file-scope static functions,
//:   and only the base names of those few source files.
//
// Line number information and full source file information, if available at
// all, is present in other formats.  On Linux, that information is in the
// DWARF format.
//
// The DWARF document is at: http://www.dwarfstd.org/Download.php and various
// enums are provided in the file '/usr/include/dwarf.h'.  Note that on the
// platform this was developed on, the include file was version 3, but the
// executables this was developed with was were DWARF version 4.  Thus some
// symbols had to be added in the enum type
// 'StackTraceResolver_DwarfReader::Dwarf4Enums'.
//
// In general, the spec is 255 pages long and not very well organized.  It
// sometimes only vaguely implies necessary assumptions, in other cases it
// utterly fails to mention things (see 'mysteryZero' below) or mentions things
// that do not actually occur in the binaries (see 'isStmt' below).  In
// addition, it sometimes describes multiple ways to encode information, only
// one of which was encountered in the test cases, so we have had to code for
// multiple possibilities yet are only able to test one of them.  Given this,
// we face a lot of uncertainty when decoding DWARF information.  We are only
// reading DWARF information to get line numbers and source file names.  ELF
// gives source file names, but only in the instance of static identifiers, and
// then gives only the base name, while DWARF gives the full path of all source
// file names.
//
// So the DWARF code was written on the assumption that we already have a
// pretty good stack trace based on ELF information, and if, while decoding
// DWARF information, we encounter something unexpected or strange, we should
// just abandon decoding the DWARF inforation (or decoding a subset of the
// DWARF information) and continue delivering the rest of the information we
// have.  A core dump would be highly inappropriate.  We use 'u_ASSERT_BAIL' to
// check things, which just returns a failure code if the check fails, and
// depending on 'u_TRACES', may print out a message and possibly exit.
// 'u_TRACES' will always be 0 in production, in which case 'u_ASSERT_BAIL'
// will just return -1 without printing any error messages or exiting.
//
// '/usr/include/dwarf.h' defines only 'enum' constant values, no 'class'es,
// 'struct's or 'union's.  The organization of the data is described only by
// prose in the spec.
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
// .debug_info:        Various information about the compilation unit,
//                     including the source file directory and name and the
//                     offset in the .debug_line section of the line number
//                     information.  Also contains information about the
//                     address range to which the compilation unit applies.
// .debug_line:        Line number and source file information.
// .debug_ranges:      When one compilation unit (described in the .debug_info
//                     and .debug_abbrev sections) applies to multiple address
//                     ranges, they refer to a sequence of ranges in the
//                     .debug_ranges section.  This section is not traversed
//                     unless 'u_DWARF_CHECK_ADDRESSES' is set.
// .debug_str          Section where null-terminated strings are stored,
//                     referred to by offsets that reside in the .debug_info
//                     section that are the offset of the beginning of a
//                     string from the beginning of the .debug_str section.
//..
//
//                              // .debug_aranges
//
// We start with the .debug_aranges section, which contains address ranges to
// be compared with stack pointers.  If a stack pointer matches a range, the
// range indicates an offset in the .debug_info section where the compilation
// unit's information is contained.
//
//                              // .debug_info
//
// The graphic "Figure 48" from the DWARF doc, page 188, in Appendix D.1, was
// particularly enlightening about how to read the description of a compilation
// unit in the .debug_info and .debug_abbrev sections.  Things were illustrated
// there that had not been spelled out formally in this doc.
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
// terminated string stored in the .debug_info section, or a 'DW_FORM_strp',
// which indicates the .debug_info section contains a section offset into
// the .debug_str section where the null terminated string resides.
//
// One piece of information we are interested in is the offset
// in .debug_line where our line number information and most of the file name
// information is located.  This will be found where 'DW_AT_stmt_list == attr',
// and we use 'form' to interpret how to read that offset.
//
//                              // .debug_line
//
// The layout of the line number information is in changer 6.2 of the DWARF
// doc.  In the .debug_line section at the specified offset is the information
// for the given compile unit.  It begins with the header, which includes,
// among other things:
//: o A table of all directories that source files (including include files)
//:   are in, though not including the directory where the compilation was
//:   run (which was in the .debug_info section).
//: o A table of all source files, and what directories they're in, (but not
//:   including the .cpp file that was compiled, which was in the .debug_info
//:   section).
//: o Initialization of some variables, including, among others:
//:   1 'lineBase'
//:   2 'lineRange'
//:   3 'opcodeBase'
//
// The line number information header is followed by unsigned byte opcodes in a
// program described in section 6.2.5 of the DWARF doc.  The opcodes describe a
// state machine, that always has
//: o A current line number, which may increase or decrease.
//: o A current address, which always increases.
//: o A current source file name.
// If '1 <= opcode < opcodeBase', 'opcode' is one of the 'DW_LNS_*' identifiers
// described in 'dwarf.h', and it's meaning and what arguments it has is
// described in the DWARF doc.  If 'opcode >= opcodeBase', then it will modify
// the value of the current line number and the current address in a complex
// formula described in the doc (and in the code).
//
// If '0 == opcode', it is followed by an extended opcode of the form
// 'DW_LNE_*' defined in dwarf.h and in the DWARF doc.  Often the 0 is followed
// by a 5 or a 9 which has no meaning and is to be skipped (and which is not
// described in the spec).
//
// Once the address is greater than the address from the stack trace, then the
// current line number is the line number to be reported for the stack frame,
// and the current file name is the source file name to be reported.
//
//            // Subset of DWARF Doc to Read to Parse DWARF for g++
//
// The DWARF 4 doc is 311 pages wrong.  A key part of tackling this task was
// determining the subset of it necessary to deliver line number and source
// file name information.
//
// To understand how to find the compilation unit based on the stack address
// from the .debug_aranges section, read chapter 6.1.2, which is not very long.
// The .debug_aranges section will tell us the offset of the compile unit
// information in the .debug_info section.
//
// To understand how to read the compilation unit, one must gather information
// from several parts of the doc.  The compilation unit will tell us 3 things:
//: o The directory in which the compilation was run.
//: o The source file that was compiled.  Note that if the address was in a
//:   function in an include file (an inline called out-of-line, or a template
//:   function), then this will not be the right source file name.
//: o The offset into the .debug_line section where the line number information
//:   for the compile unit resides.
// The compilation unit information will begin with the compile unit header,
// which is described in chapter 7.5.1.1, which is not very long.  This header
// will tell us the offset of the information in the .debug_abbrev section that
// we will want to read together with the .debug_info section.  Figure 48
// explains how the .debug_info and .debug_abbrev sections are read together to
// describe a compilation unit.  This figure mentions a few things not
// mentioned in the prose in other parts of the doc.  Chapter 3.1.1 explains
// some of how to parse the .debug_info information, explaining which 'DW_AT_*'
// identifiers will be encountered while parsing a compile unit.  Note that
// since we don't want to parse information about types or variables, the
// number of 'DW_AT_*' id's we will have to understand is a very small subset
// of the total number described in 'dwarf.h'.
//
// Once we get to the line number information in .debug_line, we simply follow
// the directions in chapter 6.2, which must be read in its entirety, but it's
// only about 15 pages.  The .debug_line entry begins with a header
// described in 6.2.4.
//
//                          // Clang Support For DWARF
//
// Doing line number information with DWARF on Linux using the clang compiler
// is problematic.  There are two ways the DWARF information can tell you
// which compile unit an address is in -- either in the .debug_aranges section,
// or in address information in the compile unit itself.  The clang compiler
// produces no .debug_aranges, and does not put the address information into
// the compile unit information. gdb is able to function anyway, examination of
// the gdb code determined that they were building huge datastructure in RAM
// based on the voluminous .debug_line information, which we don't want to do
// because in a large 32-bit executable we would exhaust the address space.  We
// also decided that, for the time being, clang is not a very important
// platform for us.  It might be possible to support clang by traversing all
// in the .debug_line information and resolving all addresses at once, but this
// might be very slow.
//
//                          // Proposed Clang Support
//
// A proposed way to provide clang support without having to use large amounts
// of memory, which has not been coded, would be to traverse all the compile
// units in the .debug_info section, and for each one go to its corresponding
// section in the .debug_line inforation, then parse the .debug line
// information in such a way to determine which ranges of addresses are
// described, and for each range, use the STL 'lower_bound' function to
// determine which of the stack addresses, if any, apply to that range.  If
// any do apply, assign the compile unit offset of the compile unit to those
// frame records.  Then go back in a later pass and use the existing Linux
// code to traverse those compile units and line number information.

// ============================================================================
//              Debugging trace macros: 'eprintf' and 'zprintf'
// ============================================================================

#undef  u_TRACES
#define u_TRACES 0  // 0 == debugging traces off, eprintf and zprint do nothing
                    // 1 == debugging traces on, eprintf is like zprintf
                    // 2 == debugging traces on, eprintf exits

// If the .debug_aranges section is present, the code guarded by
// '#ifdef u_DWARF_CHECK_ADDRESSES' is not necessary.  But plan to eventually
// port to a Solaris platform that uses DWARF, and we don't know if the
// .debug_aranges section will be present there, so keep this already tested
// and debugged code around as it may become useful.

#undef  u_DWARF_CHECK_ADDRESSES
#define u_DWARF_CHECK_ADDRESSES 0

// zprintf: For non-error debug traces.  0 == u_TRACES:  0: null function
//                                       0 <  u_TRACES:   : like printf
// eprintf: Called when errors occur in data.
//                                           0 == u_TRACES: null function
//                                           1 == u_TRACES: like printf
//                                           2 == u_TRACES: printf, then exit 1
// u_ASSERT_BAIL(expr) gentle assert.
//                               0 == TRACES: if !(expr) return -1;
//                               0 <  TRACES: if !(expr) message & return -1
//     // 'u_ASSERT_BAIL' is needed because we need asserts for the DWARF code,
//     // but it's totally inappropriate to core dump if the DWARF data is
//     // corrupt, because we can give a prety good stack trace without the
//     // DWARF information, so if 'expr' fails, just quit the DWARF analysis
//     // and continue showing the rest of the stack trace to the user.
//
// u_P(expr):
//       0 == u_TRACES: evaluates to 'false'.
//       0 <  u_TRACES: output << #expr << ':' << (expr) then evaluate to false
//    // P(expr) is to be used in u_ASSERT_BAIL, i.e.
//    // 'u_ASSERT_BAIL(5 == x || P(x))' will, if (5 != x), print out the value
//    // of x then print the assert message and return -1.
//
// u_PH(expr): like P(expr), except prints the value in hex.

#undef u_eprintf
#undef u_zprintf
#undef u_ASSERT_BAIL
#undef u_P
#undef u_PH

#if u_TRACES > 0
# include <stdio.h>

#define u_zprintf printf

static const char u_assertBailString[] = {
                "Warning: assertion (%s) failed at line %d in function %s\n" };

#define u_ASSERT_BAIL(expr)    do {                                           \
        if (!(expr)) {                                                        \
            u_eprintf(u_assertBailString, #expr, __LINE__, rn);               \
            return -1;                                                        \
        }                                                                     \
    } while (false)
    // If the specified 'expr' evaluates to 'false', print a message and return
    // -1.  Note that if 'TRACES > 1' the 'eprintf' will exit 1.

#if 0 // comment until used, avoid "never called" warnings.
static bool u_warnPrint(const char *name, const void *value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'PH'.  Return 'false'.
{
    u_zprintf("%s = %p\n", name, value);

    return false;
}
#endif
#if defined(u_DWARF)
// 'P()' and 'PH()' are only used in u_ASSERT_BAIL in DWARF code, only define
// these functions in DWARF compiles, otherwise get unused messages.

static bool u_warnPrint(const char                       *name,
                        BloombergLP::bsls::Types::Uint64  value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'P'.  Return 'false'.
{
    u_zprintf("%s = %llu\n", name, value);

    return false;
}

static bool u_warnPrintHex(const char                        *expr,
                           BloombergLP::bsls::Types::Uint64   value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'PH'.  Return 'false'.
{
    u_zprintf("%s = 0x%llx\n", expr, value);

    return false;
}

#define u_P(expr)     u_warnPrint(   #expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // decimal and return 'false'.

#define u_PH(expr)    u_warnPrintHex(#expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // hex and return 'false'.
#endif

#if 1 == u_TRACES
# define u_eprintf printf
#else
static
int u_eprintf(const char *format, ...)
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
int u_eprintf(const char *, ...)
    // do nothing
{
    return 0;
}

static inline
int u_zprintf(const char *, ...)
    // do nothing
{
    return 0;
}

#define u_ASSERT_BAIL(expr) do {                                              \
        if (!(expr)) {                                                        \
            return -1;                                                        \
        }                                                                     \
    } while (false)
    // If the specified 'expr' evaluates to 'false', return -1, otherwise do
    // nothing.

#if defined(u_DWARF)
#define u_P(expr)            (false)
    // Ignore 'expr' and return 'false'.
#define u_PH(expr)           (false)
    // Ignore 'expr' and return 'false'.
#endif

#endif

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
# define u_ASSERT_BAIL_SAFE(expr) u_ASSERT_BAIL(expr)
    // Do u_ASSERT_BAIL
#else
# define u_ASSERT_BAIL_SAFE(expr)
    // Do nothing.
#endif

namespace BloombergLP {

namespace {
namespace u {

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

template <class TYPE>
static TYPE approxAbs(TYPE x)
    // Return approximately the absolute value of the specified 'x'.
{
    BSLMF_ASSERT(static_cast<TYPE>(-1) < 0);

    x = x < 0 ? -x : x;
    x = x < 0 ? ~x : x;     // in the case of, i.e., INT_MIN, ~x is close
                            // enough

    BSLS_ASSERT_SAFE(0 <= x);

    return x;
}

typedef bsls::Types::UintPtr          UintPtr;
typedef bsls::Types::Uint64           Uint64;
typedef bdls::FilesystemUtil::Offset  Offset;

#ifdef u_DWARF
typedef balst::StackTraceResolver_DwarfReader Reader;
typedef u::Reader::Section                    Section;
#endif

static const u::Offset  maxOffset = LLONG_MAX;
static const u::UintPtr minusOne  = ~static_cast<u::UintPtr>(0);

BSLMF_ASSERT(sizeof(u::Uint64) == sizeof(u::Offset));
BSLMF_ASSERT(sizeof(u::UintPtr) == sizeof(void *));
BSLMF_ASSERT(static_cast<u::Offset>(-1) < 0);
BSLMF_ASSERT(u::maxOffset > 0);
BSLMF_ASSERT(u::maxOffset > INT_MAX);
BSLMF_ASSERT(static_cast<u::Offset>(static_cast<u::Uint64>(u::maxOffset) + 1) <
                                                                            0);

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

#ifdef u_DWARF
enum Dwarf4Enums {
    // DWARF 4 flags not necessarily defined in our dwarf.h.

    e_DW_TAG_type_unit             = u::Reader::e_DW_TAG_type_unit,
    e_DW_TAG_rvalue_reference_type = u::Reader::e_DW_TAG_rvalue_reference_type,
    e_DW_TAG_template_alias        = u::Reader::e_DW_TAG_template_alias,

    e_DW_AT_signature              = u::Reader::e_DW_AT_signature,
    e_DW_AT_main_subprogram        = u::Reader::e_DW_AT_main_subprogram,
    e_DW_AT_data_bit_offset        = u::Reader::e_DW_AT_data_bit_offset,
    e_DW_AT_const_expr             = u::Reader::e_DW_AT_const_expr,
    e_DW_AT_enum_class             = u::Reader::e_DW_AT_enum_class,
    e_DW_AT_linkage_name           = u::Reader::e_DW_AT_linkage_name,

    e_DW_FORM_sec_offset           = u::Reader::e_DW_FORM_sec_offset,
    e_DW_FORM_exprloc              = u::Reader::e_DW_FORM_exprloc,
    e_DW_FORM_flag_present         = u::Reader::e_DW_FORM_flag_present,
    e_DW_FORM_ref_sig8             = u::Reader::e_DW_FORM_ref_sig8,

    e_DW_LNE_set_discriminator     = u::Reader::e_DW_LNE_set_discriminator
};
#endif

                                   // ---------
                                   // Constants
                                   // ---------

enum { k_SCRATCH_BUF_LEN = 32 * 1024 - 64 };
    // length in bytes of d_buffer_p, 32K minus a little so we don't waste a
    // page

#ifdef u_DWARF
BSLMF_ASSERT(static_cast<int>(u::k_SCRATCH_BUF_LEN) ==
                               static_cast<int>(u::Reader::k_SCRATCH_BUF_LEN));
    // u::Reader really needs the buffers passed to it to be at least
    // 'u::Reader::k_SCRATCH_BUF_LEN' long.
#endif

                                // ---------------
                                // local Elf Types
                                // ---------------

#undef   u_SPLICE
#ifdef BSLS_PLATFORM_CPU_64_BIT
# define u_SPLICE(suffix)  Elf64_ ## suffix
#else
# define u_SPLICE(suffix)  Elf32_ ## suffix
#endif

// The following types are part of the ELF standard, and describe structs that
// occur in the executable file / shared libraries.

typedef u_SPLICE(Dyn)   ElfDynamic;        // The expression '&_DYNAMIC' is a
                                           // 'void *' pointer to a an array of
                                           // 'struct's of this type, used to
                                           // find the link map on Solaris.

typedef u_SPLICE(Ehdr)  ElfHeader;         // The elf header is a standard
                                           // header at the start of any ELF
                                           // file

typedef u_SPLICE(Phdr)  ElfProgramHeader;  // Program headers are obtained from
                                           // the link map.  We use them to
                                           // find code segments.

typedef u_SPLICE(Shdr)  ElfSectionHeader;  // Section headers are located from
                                           // the U::ElfHeader, they tell us
                                           // where the sections containing
                                           // symbols and strings are

typedef u_SPLICE(Sym)   ElfSymbol;         // Describes one symbol in the
                                           // symbol table.
#undef u_SPLICE

                                    // --------
                                    // FrameRec
                                    // --------

class FrameRec {
    // A struct consisting of the things we want stored associated with a given
    // frame that will not be stored in the 'StackTraceFrame' itself.  We put
    // these into a vector and sort them by address so that later we can do
    // O(log n) lookup of frames by address.  (The code was previously always
    // doing an exhaustive search of all the addresses).

    // DATA
    const void             *d_address;            // == 'd_frame_p->address()'
    balst::StackTraceFrame *d_frame_p;            // held, not owned
#ifdef u_DWARF
    u::Offset               d_compileUnitOffset;
    u::Offset               d_lineNumberOffset;
    bsl::string             d_compileUnitDir;
    bsl::string             d_compileUnitFileName;
#endif
    int                     d_index;
    bool                    d_isSymbolResolved;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(u::FrameRec, bslma::UsesBslmaAllocator);

    // CREATORS
    FrameRec(const void             *address,
             balst::StackTraceFrame *stackTraceFrame,
             int                     index,
             bslma::Allocator       *allocator);
        // Create a 'u::FrameRec' referring to the specified 'address' and the
        // specified 'framePtr'.

    FrameRec(const u::FrameRec&  original,
             bslma::Allocator   *allocator);
        // Create a 'u::FrameRec' that uses the specified 'allocator' and is a
        // copy of the specified 'original'.

    // ~u::FrameRec() = default;

    // MANIPULATORS
    // inline operator=(const u::FrameRec&) = default;

    void setAddress(const void *value);
        // Set tthe 'address' field to the specified 'value'.

    void setAddress(u::UintPtr value);
        // Set tthe 'address' field to the specified 'value'.

#ifdef u_DWARF
    void setCompileUnitDir(const bsl::string& value);
        // Set the compile unit directory to the specified 'value'.

    void setCompileUnitFileName(const bsl::string& value);
        // Set the compile unit file name to the specified 'value'.

    void setCompileUnitOffset(const u::Offset value);
        // Set tthe compilation unit offset field to the specified 'value'.

    void setLineNumberOffset(u::Offset value);
        // Set tthe line number offset field to the specified 'value'.
#endif

    void setSymbolResolved();
        // Set this frame as being done.

    // ACCESSORS
    bool operator<(const u::FrameRec& rhs) const;
        // Return 'true' if the address field of this object is less than the
        // address field of 'rhs'.

    const void *address() const;
        // Return the 'address' field from this object.

#ifdef u_DWARF
    const bsl::string& compileUnitDir() const;
        // Return the compile unit directory name.

    const bsl::string& compileUnitFileName() const;
        // Return the compile unit file name.

    u::Offset compileUnitOffset() const;
        // Return the compile unit offset field.

    u::Offset lineNumberOffset() const;
        // Return the line number offset field.
#endif

    balst::StackTraceFrame& frame() const;
        // Return a reference to the modifiable 'frame' referenced by this
        // object.  Note that though this is a 'const' method, modifiable
        // access to the frame is provided.

    int index() const;
        // Return the index of the frame in the stack trace.

    bool isSymbolResolved() const;
        // Return 'true' if this frame is done and 'false' otherwise.
};

                                // --------------
                                // class FrameRec
                                // --------------

// CREATORS
FrameRec::FrameRec(const void             *address,
                   balst::StackTraceFrame *stackTraceFrame,
                   int                     index,
                   bslma::Allocator       *allocator)
: d_address(address)
, d_frame_p(stackTraceFrame)
#ifdef u_DWARF
, d_compileUnitOffset(u::maxOffset)
, d_lineNumberOffset( u::maxOffset)
, d_compileUnitDir(allocator)
, d_compileUnitFileName(allocator)
#endif
, d_index(index)
, d_isSymbolResolved(false)
{
    (void) allocator;    // in case not DWARF
}

FrameRec::FrameRec(const FrameRec&    original,
                   bslma::Allocator  *allocator)
: d_address(            original.d_address)
, d_frame_p(            original.d_frame_p)
#ifdef u_DWARF
, d_compileUnitOffset(  original.d_compileUnitOffset)
, d_lineNumberOffset(   original.d_lineNumberOffset)
, d_compileUnitDir(     original.d_compileUnitDir,      allocator)
, d_compileUnitFileName(original.d_compileUnitFileName, allocator)
#endif
, d_index(              original.d_index)
, d_isSymbolResolved(   original.d_isSymbolResolved)
{
    (void) allocator;    // in case not DWARF
}

// MANIPULATORS
inline
void FrameRec::setAddress(const void *value)
{
    d_address = value;
}

inline
void FrameRec::setAddress(u::UintPtr value)
{
    d_address = reinterpret_cast<const void *>(value);
}

#ifdef u_DWARF
inline
void FrameRec::setCompileUnitDir(const bsl::string& value)
{
    d_compileUnitDir = value;
}

inline
void FrameRec::setCompileUnitFileName(const bsl::string& value)
{
    d_compileUnitFileName = value;
}

inline
void FrameRec::setCompileUnitOffset(const u::Offset value)
{
    d_compileUnitOffset = value;
}

inline
void FrameRec::setLineNumberOffset(u::Offset value)
{
    d_lineNumberOffset = value;
}
#endif

inline
void FrameRec::setSymbolResolved()
{
    d_isSymbolResolved = true;
}

// ACCESSORS
inline
bool FrameRec::operator<(const FrameRec& rhs) const
{
    return d_address < rhs.d_address;
}

inline
const void *FrameRec::address() const
{
    return d_address;
}

#if u_DWARF
inline
const bsl::string& FrameRec::compileUnitDir() const
{
    return d_compileUnitDir;
}

inline
const bsl::string& FrameRec::compileUnitFileName() const
{
    return d_compileUnitFileName;
}

inline
u::Offset FrameRec::compileUnitOffset() const
{
    return d_compileUnitOffset;
}

inline
u::Offset FrameRec::lineNumberOffset() const
{
    return d_lineNumberOffset;
}
#endif

inline
balst::StackTraceFrame& FrameRec::frame() const
{
    return *d_frame_p;
}

inline
int FrameRec::index() const
{
    return d_index;
}

inline
bool FrameRec::isSymbolResolved() const
{
    return d_isSymbolResolved;
}

typedef bsl::vector<u::FrameRec>  FrameRecVec;     // Vector of 'u::FrameRec's.
typedef u::FrameRecVec::iterator  FrameRecVecIt;   // Iterator of
                                                   // 'u::FrameRecVec'.

                                // ============
                                // AddressRange
                                // ============

struct AddressRange {
    // This 'struct' specifies a range of addresses over
    // '[ d_address, d_address + d_size )'.

    u::UintPtr d_address;
    u::UintPtr d_size;

    // ACCESSORS
    bool contains(const void *address) const;
        // Return 'true' if this address range contains the specified 'address'
        // and 'false' otherwise.

    bool contains(u::UintPtr address) const;
        // Return 'true' if this address range contains the specified 'address'
        // and 'false' otherwise.

    bool overlaps(const AddressRange& other) const;
        // Return 'true' if this address range overlaps the specified
        // u_AddressRange 'other' and 'false' otherwise.  Note that if the
        // boundaries of the addresses ranges merely 'touch', that does not
        // count as an overlap.
};

                                // ------------
                                // AddressRange
                                // ------------

// ACCESSORS
bool u::AddressRange::contains(const void *address) const
{
    u::UintPtr a = reinterpret_cast<u::UintPtr>(address);
    return d_address <= a && a < d_address + d_size;
}

bool u::AddressRange::contains(u::UintPtr address) const
{
    return d_address <= address && address < d_address + d_size;
}

bool u::AddressRange::overlaps(const AddressRange& other) const
{
    return d_address <= other.d_address
           ? d_address + d_size > other.d_address
           : other.d_address + other.d_size > d_address;
}

                              // ===============
                              // class FreeGuard
                              // ===============


class FreeGuard {
    // This 'class' will manage a buffer, which was allocated by 'malloc' and
    // is returned by '__cxa_demangle'.

    // DATA
    char *d_buffer;

  public:
    // CREATORS
    explicit
    FreeGuard(char *buffer)
    : d_buffer(buffer)
        // Create a 'FreeGuard' object that manages the specified 'buffer'.
    {}

    ~FreeGuard()
        // If 'd_buffer' is non-null, free it.
    {
        if (d_buffer) {
            ::free(d_buffer);
        }
    }
};

                      // ---------------------------------------
                      // free functions in the unnamed namespace
                      // ---------------------------------------

static int cleanupString(bsl::string *str, bslma::Allocator *alloc)
    // Eliminate all instances of "/./" from the specified '*str', and also as
    // many instances of "/../" as possible.  Do not resolve symlinks.  Use the
    // specified 'alloc' for memory allocation of temporaries.  Return 0 unless
    // "*str" does not represent an acceptable full path for a source file, and
    // a non-zero value otherwise.
{
    static const char rn[] = { "u::cleanupString:" };

    const bsl::size_t npos = bsl::string::npos;

    if (str->empty() || '/' != (*str)[0] ||
                                         !bdls::FilesystemUtil::exists(*str)) {
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
        // u_zprintf("%s found '/../' in %s\n", rn, str->c_str());

        u_ASSERT_BAIL(pos >= 1);

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

            u_TRACES && u_zprintf(
                          "%s difficulty cleaning up, but not an error\n", rn);

            return 0;                                                 // RETURN
        }

        bsl::size_t rpos = str->rfind('/', pos - 1);
        u_ASSERT_BAIL(npos != rpos && ".. below root directory");
        u_ASSERT_BAIL(rpos < pos - 1 && '/' == (*str)[rpos]);

        str->erase(rpos, pos + 3 - rpos);

        // u_zprintf("%s optimized to %s\n", rn, str->c_str());
    }

    return 0;
}

#ifdef u_DWARF
static int dumpBinary(u::Reader *reader, int numRows)
    // Dump out the specified 'numRows' of 16 byte rows of the binary about to
    // be read by the specified 'reader'.  Note that this function is called
    // for debugging when we encounter unexpected things in the binary.
{
    static const char rn[] = { "u::dumpBinary:" };    (void) rn;

    if (0 == u_TRACES) {
        return -1;                                                    // RETURN
    }

    int rc;

    u::Offset o = reader->offset();
    for (int ii = 0; ii < numRows; ++ii) {
        for (int jj = 0; jj < 16; ++jj) {
            unsigned char uc;
            rc = reader->readValue(&uc);
            if (0 != rc) {
                // We probably just bumped into the end of the section.
                // Recover reasonably well.

                rc = reader->skipTo(o);
                u_ASSERT_BAIL(0 == rc);

                return -1;                                            // RETURN
            }

            u_zprintf("%s%x", (jj ? " " : ""), uc);
        }
        u_zprintf("\n");
    }
    rc = reader->skipTo(o);
    u_ASSERT_BAIL(0 == rc);

    return 0;
}
#endif

static
int checkElfHeader(u::ElfHeader *elfHeader)
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

    if ((u::e_IS_BIG_ENDIAN ? ELFDATA2MSB : ELFDATA2LSB) !=
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

}  // close namespace u
}  // close unnamed namespace

                      // --------------------------------
                      // u::StackTraceResolver::HiddenRec
                      // --------------------------------

struct u::StackTraceResolver::HiddenRec {
    // This 'struct' contains information for the stack trace resolver that is
    // hidden from the .h file.  Thus, we avoid having to expose
    // balst_stacktraceresolver_dwarfreader.h' in the .h, and we can use types
    // that are locally defined in this imp file.

    // DATA
    StackTraceResolver_FileHelper
                    *d_helper_p;           // file helper associated with
                                           // current segment

    StackTrace       *d_stackTrace_p;      // the stack trace we are resolving

    u::AddressRange   d_addressRange;      // address range of the current
                                           // segment

    u::FrameRecVec    d_frameRecs;         // Vector of address frame pairs
                                           // for fast lookup of addresses.
                                           // Not local to current segment --
                                           // initialized once per resolve, and
                                           // sorted by address.

    u::FrameRecVecIt   d_frameRecsBegin;   // range of frame records that
                                           // pertain

    u::FrameRecVecIt   d_frameRecsEnd;     // to the current segment at a
                                           // given time.

    u::UintPtr         d_adjustment;       // adjustment between addresses
                                           // expressed in object file and
                                           // actual addresses in memory for
                                           // current segment.  This value is
                                           // added to address read from the
                                           // object file.

    u::Offset          d_symTableOffset;   // symbol table section (symbol
                                           // table

    u::Offset          d_symTableSize;     // does not contain symbol names,
                                           // just offsets into string table)
                                           // from the beginning of the
                                           // executable or library file

    u::Offset          d_stringTableOffset;// string table offset from the
    u::Offset          d_stringTableSize;  // beginning of the file

#ifdef u_DWARF
    u::Section         d_abbrevSec;        // .debug_abbrev section
    u::Reader          d_abbrevReader;     // reader for that section

    u::Section         d_arangesSec;       // .debug_aranges section
    u::Reader          d_arangesReader;    // reader for that section

    u::Section         d_infoSec;          // .debug_info section
    u::Reader          d_infoReader;       // reader for that section

    u::Section         d_lineSec;          // .debug_line section
    u::Reader          d_lineReader;       // reader for that section

    u::Section         d_rangesSec;        // .debug_ranges section
    u::Reader          d_rangesReader;     // reader for that section

    u::Section         d_strSec;           // .debug_str section
    u::Reader          d_strReader;        // reader for that section
#endif

    u::Offset          d_libraryFileSize;  // size of the current library or
                                           // executable file

    char             *d_scratchBufA_p;     // crratch buffer A (from resolver)

    char             *d_scratchBufB_p;     // scratch buffer B (from resolver)
#ifdef u_DWARF
    char             *d_scratchBufC_p;     // scratch buffer C (from resolver)

    char             *d_scratchBufD_p;     // scratch buffer D (from resolver)
#endif
    int               d_numTotalUnmatched; // Total number of unmatched frames
                                           // remaining in this resolve.

    bool              d_isMainExecutable;  // 'true' if in main executable
                                           // segment, as opposed to a shared
                                           // library

    bslma::Allocator *d_allocator_p;       // The resolver's heap bypass
                                           // allocator.

  private:
    // NOT IMPLEMENTED
    HiddenRec(const HiddenRec&);
    HiddenRec& operator=(const HiddenRec&);

  public:
    // CREATORS
    explicit
    HiddenRec(u::StackTraceResolver *resolver);
        // Create this 'Seg' object, initialize 'd_numFrames' to 'numFrames',
        // and initialize all other fields to 0.

    // MANIPULATORS
#ifdef u_DWARF
# if u_DWARF_CHECK_ADDRESSES
    int dwarfCheckRanges(bool          *isMatch,
                         u::UintPtr     addressToMatch,
                         u::UintPtr     baseAddress,
                         u::Offset      offset);
        // Read a ranges section and determine if an address matches.
# endif

    int dwarfReadAll();
        // Read the DWARF information.

    int dwarfReadAranges();
        // Read the .debug_aranges section.  Return the number of frames
        // matched.

    int dwarfReadCompileOrPartialUnit(u::FrameRec *frameRec,
                                      bool        *addressMatched);
        // Read a compile or partial unit for the given 'frameRec', assuming
        // that 'd_infoReader' is positioned right after the tag & children
        // info, and 'd_abbrevReader' is positioned right after the tag index,
        // and at the first attribute.  Set the specified '*addressMatched', to
        // 'true' if the section matched the frame record and 'false'
        // otherwise, except if 'u_CHECK_ADDRESSES' is 'false', in which case
        // '*addressMatched' is always to be set to 'true'.

    int dwarfReadDebugInfo();
        // Read the .debug_info and .debug_abbrev sections.

    int dwarfReadDebugInfoFrameRec(u::FrameRec *frameRec);
        // Read the dwarf info for a single compilation unit for a single
        // frame.  Return
        //..
        //: o rc <  0: failure
        //: o rc == 0: successfully parsed, but no match
        //: o rc == 1: successfully parsed, matched address & line number info
        //..

    int dwarfReadDebugLine();
        // Read the .debug_line section.

    int dwarfReadDebugLineFrameRec(u::FrameRec *frameRec);
        // Read the .debug_line section pertaining to the specified
        // '*frameRec', and populate the source file name and line number
        // information.
#endif

    void reset();
        // Zero numerous fields.
};

// CREATORS
u::StackTraceResolver::HiddenRec::HiddenRec(u::StackTraceResolver *resolver)
: d_helper_p(0)
, d_stackTrace_p(resolver->d_stackTrace_p)
, d_frameRecs(&resolver->d_hbpAlloc)
, d_frameRecsBegin()
, d_frameRecsEnd()
, d_adjustment(0)
, d_symTableOffset(0)
, d_symTableSize(0)
, d_stringTableOffset(0)
, d_stringTableSize(0)
#ifdef u_DWARF
, d_abbrevSec()
, d_arangesSec()
, d_infoSec()
, d_lineSec()
, d_rangesSec()
#endif
, d_scratchBufA_p(resolver->d_scratchBufA_p)
, d_scratchBufB_p(resolver->d_scratchBufB_p)
#ifdef u_DWARF
, d_scratchBufC_p(resolver->d_scratchBufC_p)
, d_scratchBufD_p(resolver->d_scratchBufD_p)
#endif
, d_numTotalUnmatched(resolver->d_stackTrace_p->length())
, d_isMainExecutable(0)
, d_allocator_p(&resolver->d_hbpAlloc)
{
    d_frameRecs.reserve(d_numTotalUnmatched);
    for (int ii = 0; ii < d_numTotalUnmatched; ++ii) {
        balst::StackTraceFrame& frame = (*resolver->d_stackTrace_p)[ii];
        d_frameRecs.push_back(u::FrameRec(frame.address(),
                                          &frame,
                                          ii,
                                          &resolver->d_hbpAlloc));
    }
    bsl::sort(d_frameRecs.begin(), d_frameRecs.end());
}

// MANIPULATORS
#ifdef u_DWARF
#if u_DWARF_CHECK_ADDRESSES
// This code has been tested and works.  This is unnecessary if the
// .debug_aranges section pointed us at the right part of the .debug_info,
// but the clang compiler generates no .debug_aranges section, so we'll have
// to scan all the compile units and use this code to find which one
// matches on that platform.

int u::StackTraceResolver::HiddenRec::dwarfCheckRanges(
                                                    bool       *isMatch,
                                                    u::UintPtr  addressToMatch,
                                                    u::UintPtr  baseAddress,
                                                    u::Offset   offset)
{
    static const char rn[] = { "HiddenRec::dwarfCheckRanges:" };
    int rc;

    *isMatch = false;

    rc = d_rangesReader.skipTo(d_rangesSec.d_offset + offset);
    u_ASSERT_BAIL(0 == rc);

    bool firstTime = true;
    while (true) {
        u::UintPtr loAddress, hiAddress;
        rc = d_rangesReader.readAddress(&loAddress);
        u_ASSERT_BAIL(0 == rc && "read loAddress failed");
        rc = d_rangesReader.readAddress(&hiAddress);
        u_ASSERT_BAIL(0 == rc && "read hiAddress failed");

        if (0 == loAddress && 0 == hiAddress) {
            u_TRACES && u_zprintf("%s done - no match found\n", rn);

            return 0;                                                 // RETURN
        }

        if (firstTime) {
            firstTime = false;

            const u::UintPtr baseIndicator =
                      static_cast<int>(sizeof(u::UintPtr)) ==
                                                   d_rangesReader.addressSize()
                      ? u::minusOne
                      : 0xffffffffUL;

            if (baseIndicator == loAddress) {
                baseAddress = hiAddress;
            }
            else {
                if (u::minusOne == baseAddress) {
                    baseAddress = d_adjustment;
                }

                u_TRACES && u_zprintf("%s base address not in .debug_ranges,"
                                     " loAddress: %llx, baseAddress: 0x%llx\n",
                                     rn, u::ll(loAddress), u::ll(baseAddress));
            }
            if (addressToMatch < baseAddress) {
                u_TRACES && u_zprintf("%s address to match 0x%llx below base"
                                              " address 0x%llx -- not a bug\n",
                                rn, u::ll(addressToMatch), u::ll(baseAddress));

                return 0;                                             // RETURN
            }
            addressToMatch -= baseAddress;
            if (baseIndicator == loAddress) {
                continue;
            }
        }

        u_ASSERT_BAIL_SAFE(loAddress <= hiAddress);

        if (loAddress <= addressToMatch && addressToMatch < hiAddress) {
            u_TRACES && u_zprintf("%s .debug_ranges matched address 0x%llx in"
                                                         " [0x%llx, 0x%llx)\n",
                                       rn, u::ll(addressToMatch + baseAddress),
                                                u::ll(loAddress + baseAddress),
                                               u::ll(hiAddress + baseAddress));
            *isMatch = true;
            return 0;                                                 // RETURN
        }
    }
}
#endif

int u::StackTraceResolver::HiddenRec::dwarfReadAll()
{
    static const char rn[] = { "HiddenRec::dwarfReadAll:" };    (void) rn;

    u_TRACES && u_zprintf("%s starting\n", rn);

    if (0 == d_infoSec.d_size || 0 == d_lineSec.d_size) {
        u_TRACES && u_zprintf("%s Not enough information to find file names"
                                                    " or line numbers.\n", rn);
        return -1;                                                    // RETURN
    }

    int rc;
    if (d_arangesSec.d_size) {
        rc = dwarfReadAranges();  // Get the locations of the compile unit info
        if (rc < 0) {
            u_TRACES && u_zprintf(".debug_aranges failed");
        }
        else if (0 == rc) {
            u_TRACES && u_zprintf(".debug_aranges did not match anything");
        }
    }

    rc = dwarfReadDebugInfo();   // Get the location of the line number info,
                                 // from .debug_info.
    u_ASSERT_BAIL(0 == rc && "dwarfReadDebugInfo failed");

    rc = dwarfReadDebugLine();       // Get the line numbers, from .debug_line.
    if (rc) {
        u_TRACES && u_zprintf("%s .debug_line failed\n", rn);
        return -1;                                                    // RETURN
    }

    return 0;
}

int u::StackTraceResolver::HiddenRec::dwarfReadAranges()
{
    static const char rn[] = { "HiddenRec::dwarfReadAranges:" };    (void) rn;

    // Buffer use: this function usee only scratch buf A.

    int rc;

    const u::FrameRecVecIt end = d_frameRecsEnd;
    const int toMatch = static_cast<int>(end - d_frameRecsBegin);
    BSLS_ASSERT(toMatch > 0);       // otherwise we should not have been called
    int matched = 0;

    u_TRACES && u_zprintf("%s starting, toMatch=%d\n", rn, toMatch);

    rc = d_arangesReader.init(d_helper_p, d_scratchBufA_p, d_arangesSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);

    u::AddressRange addressRange, prevRange = { 0, 0 }; (void) prevRange;
    u::FrameRec dummyFrameRec(0, 0, 0, d_allocator_p);

    while (!d_arangesReader.atEndOfSection()) {
        u::Offset      rangeLength;
        int rc = d_arangesReader.readInitialLength(&rangeLength);
        u_ASSERT_BAIL(0 == rc && "read initial length failed");

        u::Offset endOffset = d_arangesReader.offset() + rangeLength;
        // 'endOffset has already been checked to fit within the section.

        unsigned short version;
        rc = d_arangesReader.readValue(&version);
        u_ASSERT_BAIL(0 == rc && "read version failed");
        u_ASSERT_BAIL(2 == version || u_P(version));

        u::Offset debugInfoOffset;
        rc = d_arangesReader.readSectionOffset(&debugInfoOffset);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(0 <= debugInfoOffset || u_PH(debugInfoOffset));
        u_ASSERT_BAIL(     debugInfoOffset < d_infoSec.d_size ||
                                                        u_PH(debugInfoOffset));

        rc = d_arangesReader.readAddressSize();
        u_ASSERT_BAIL(0 == rc);
        const bool isAddressSizeSizeofUintPtr =
                           sizeof(u::UintPtr) == d_arangesReader.addressSize();

        // The meaning of the word 'segment' here is different from its meaning
        // elsewhere in this file.  Here the 'segment size' is the size of a
        // segment on a segmented architecture processor.  The 8086 and 80286
        // were segmented architectures, and 'segmentSize' might have had some
        // relevance there, but since the 80386 (ca 1992) x86 architectures
        // have been able to access memory in a non-segmented manner.

        // We don't expect this code to be used on any segmented architectures,
        // in which case 'segmentSizeSize' will be 0 and the 'tuple's of
        // '(segmentSize, address, and size)' will instead be 'pair's of
        // '(address, size)' where 'sizeof(size) == sizeof(address)'.

        unsigned char segmentSizeSize;
        rc = d_arangesReader.readValue(&segmentSizeSize);
        u_ASSERT_BAIL(0 == rc && "read segment size size failed");
        u_ASSERT_BAIL(0 == segmentSizeSize);

        // According to section 2.70 of the spec, the header is padded here
        // to the point where 'd_addressReader.offset()' will be a multiple
        // of the size of a tuple.  Since '0 == segmentSize', the size of
        // a tuple is '2 * d_arangesReader.addressSize()'.

        // On Linux at least, this turns out to be wrong.  It turns out that
        // regardless of how 'd_arangesReader.offset()' is aligned, we are to
        // skip 4 bytes here.

        rc = d_arangesReader.skipTo(d_arangesReader.offset() + 4);
        u_ASSERT_BAIL(0 == rc && "skip padding failed");

        // u_TRACES && u_zprintf("%s Starting section %g pairs long.\n", rn,
        //                    (double) (endOffset - d_arangesReader.offset()) /
        //                                (2 * d_arangesReader.addressSize()));

        bool foundZeroes = false;
        while (d_arangesReader.offset() < endOffset) {
            if (isAddressSizeSizeofUintPtr) {
                rc =  d_arangesReader.readValue(&addressRange);
            }
            else {
                rc =  d_arangesReader.readAddress(&addressRange.d_address);
                rc |= d_arangesReader.readAddress(&addressRange.d_size);
            }
            u_ASSERT_BAIL(0 == rc);

            if (0 == addressRange.d_address && 0 == addressRange.d_size) {
                if (d_arangesReader.offset() != endOffset) {
                    u_eprintf("%s terminating 0's %s range end\n", rn,
                        d_arangesReader.offset() < endOffset ? "reached before"
                                                             : "overlap");
                    rc = d_arangesReader.skipTo(endOffset);
                    u_ASSERT_BAIL(0 == rc);
                }

                foundZeroes = true;
                break;
            }

            addressRange.d_address += d_adjustment;    // This was not
                                                       // mentioned in the
                                                       // spec.

            if   (!d_addressRange.contains(addressRange.d_address)
               || 0 == addressRange.d_size
               || !d_addressRange.contains(
                           addressRange.d_address + addressRange.d_size - 1)) {
                // Sometimes the address ranges are just garbage.

                u_TRACES && u_zprintf("Garbage address range (0x%lx, 0x%lx)\n",
                      u::l(addressRange.d_address), u::l(addressRange.d_size));
                continue;
            }

            if (u_TRACES && prevRange.overlaps(addressRange)) {
                u_eprintf("%s overlapping ranges (0x%lx, 0x%lx)"
                                                       " (0x%lx, 0x%lx)\n", rn,
                             u::l(prevRange.d_address), u::l(prevRange.d_size),
                      u::l(addressRange.d_address), u::l(addressRange.d_size));
            }

            prevRange = addressRange;

            // u_TRACES && u_zprintf("%s range:[0x%lx, 0x%lx)\n",
            //                                rn, u::l(addressRange.d_address),
            //                                      u::l(addressRange.d_size));

            dummyFrameRec.setAddress(addressRange.d_address);
            u::FrameRecVecIt begin =
                        bsl::lower_bound(d_frameRecsBegin, end, dummyFrameRec);
            for (u::FrameRecVecIt it = begin; it < end &&
                                  addressRange.contains(it->address()); ++it) {
                const bool isRedundant =
                                       u::maxOffset != it->compileUnitOffset();
                u_TRACES && u_zprintf("%s%s range (0x%lx, 0x%lx) matches"
                                     " frame %d, address: %p, offset 0x%llx\n",
                                           rn, isRedundant ? " redundant" : "",
                       u::l(addressRange.d_address), u::l(addressRange.d_size),
                                                                   it->index(),
                                        it->address(), u::ll(debugInfoOffset));
                if (isRedundant) {
                    continue;
                }

                it->setCompileUnitOffset(debugInfoOffset);
                if (toMatch == ++matched) {
                    u_TRACES && u_zprintf(
                                     "%s last frame in segment matched\n", rn);

                    d_arangesReader.disable();
                    return matched;                                   // RETURN
                }
            }
        }

        u_ASSERT_BAIL(foundZeroes);
    }

    u_zprintf("%s failed to complete -- %d frames unmatched.\n",
                                                        rn, toMatch - matched);

    d_arangesReader.disable();
    return matched;
}

int u::StackTraceResolver::HiddenRec::dwarfReadCompileOrPartialUnit(
                                                   u::FrameRec *frameRec,
                                                   bool        *addressMatched)
{
    static const char rn[] = { "HiddenRec::dwarfReadCompileOrPartialUnit:" };

    int rc;

    enum ObtainedFlags {
        k_OBTAINED_ADDRESS_MATCH = 0x1,
        k_OBTAINED_DIR_NAME      = 0x2,
        k_OBTAINED_BASE_NAME     = 0x4,
        k_OBTAINED_LINE_OFFSET   = 0x8,

        k_OBTAINED_SOURCE_NAME   = k_OBTAINED_DIR_NAME | k_OBTAINED_BASE_NAME,
        k_OBTAINED_ALL           = 0xf };

    int obtained = u_DWARF_CHECK_ADDRESSES ? 0
                                           : k_OBTAINED_ADDRESS_MATCH;

    *addressMatched = false;

#if u_DWARF_CHECK_ADDRESSES
    const u::UintPtr addressToMatch =
                             reinterpret_cast<u::UintPtr>(frameRec->address());
    u::UintPtr loPtr = u::minusOne, hiPtr = 0;
#endif

    const int index = frameRec->index(); (void) index;

    bsl::string baseName(d_allocator_p), dirName(d_allocator_p);
    dirName.reserve(200);
    u::Offset lineNumberInfoOffset;

    do {
        unsigned int attr;
        rc = d_abbrevReader.readULEB128(&attr);
        u_ASSERT_BAIL(0 == rc);

        unsigned int form;
        rc = d_abbrevReader.readULEB128(&form);
        u_ASSERT_BAIL(0 == rc);

        if (0 == attr) {
            u_ASSERT_BAIL(0 == form);

            u_TRACES && u_zprintf("%s 0 0 encountered, section done\n", rn);

            break;
        }

        u_TRACES && u_zprintf("%s %s %s\n", rn, u::Reader::stringForAt(attr),
                                               u::Reader::stringForForm(form));

        switch (attr) {
#if u_DWARF_CHECK_ADDRESSES
          case DW_AT_low_pc: {                            // DWARF doc 3.1.1.1
            u_ASSERT_BAIL(u::minusOne == loPtr);

            rc = d_infoReader.readAddress(&loPtr, form);
            u_ASSERT_BAIL(0 == rc);

            if (DW_FORM_addr != form) {
                // this was not in the doc

                loPtr += d_adjustment;
            }

            u_TRACES && u_zprintf("%s loPtr: 0x%llx\n", rn, u::ll(loPtr));

            if (0 != hiPtr) {
                if (loPtr <= addressToMatch && addressToMatch < hiPtr) {
                    u_TRACES && u_zprintf("%s loHiMatch on lo\n", rn);
                    obtained |= k_OBTAINED_ADDRESS_MATCH;
                }
                else {
                    u_TRACES && u_zprintf("%s loHi failed to match on lo\n",
                                                                           rn);
                }
            }
          } break;
          case DW_AT_high_pc: {                           // DWARF doc 3.1.1.1
            u_ASSERT_BAIL(0 == hiPtr);

            rc = d_infoReader.readAddress(&hiPtr, form);
            u_ASSERT_BAIL(0 == rc);

            if (DW_FORM_addr != form) {
                // this was not in the doc, just guessing

                hiPtr += u::minusOne != loPtr ? loPtr : d_adjustment;
            }

            u_TRACES && u_zprintf("%s hiPtr: 0x%llx\n", rn, u::ll(hiPtr));

            if (u::minusOne != loPtr) {
                if (loPtr <= addressToMatch && addressToMatch < hiPtr) {
                    u_TRACES && u_zprintf("%s loHiMatch on hi\n", rn);
                    obtained |= k_OBTAINED_ADDRESS_MATCH;
                }
                else {
                    u_TRACES && u_zprintf("%s loHi failed to match on hi\n",
                                                                           rn);
                }
            }
          } break;
          case DW_AT_ranges: {                            // DWARF doc 3.1.1.1
            u::Offset rangesOffset;
            rc = d_infoReader.readOffsetFromForm(&rangesOffset, form);
            u_ASSERT_BAIL(0 == rc && "trouble reading ranges offset");
            u_ASSERT_BAIL(rangesOffset < d_rangesSec.d_size);
            bool isMatch;
            rc = dwarfCheckRanges(
                             &isMatch,
                             reinterpret_cast<u::UintPtr>(frameRec->address()),
                             loPtr,
                             rangesOffset);
            u_ASSERT_BAIL(0 == rc && "dwarfCheckRanges failed");

            if (isMatch) {
                u_TRACES && u_zprintf("%s ranges match\n", rn);
                obtained |= k_OBTAINED_ADDRESS_MATCH;
            }
          } break;
#endif
          case DW_AT_name: {                              // DWARF doc 3.1.1.2
            u_ASSERT_BAIL(0 == (obtained & k_OBTAINED_BASE_NAME));
            u_ASSERT_BAIL(baseName.empty());

            rc = d_infoReader.readStringFromForm(
                                                &baseName, &d_strReader, form);
            u_ASSERT_BAIL(0 == rc);

            if (!baseName.empty()) {
                u_TRACES && u_zprintf("%s baseName \"%s\" found\n",
                                                         rn, baseName.c_str());

                obtained |= k_OBTAINED_BASE_NAME |
                              ('/' == baseName.c_str()[0] ? k_OBTAINED_DIR_NAME
                                                          : 0);
            }
          } break;
          case DW_AT_stmt_list: {                         // DWARF doc 3.1.1.4
            u_ASSERT_BAIL(0 == (obtained & k_OBTAINED_LINE_OFFSET));

            rc = d_infoReader.readOffsetFromForm(&lineNumberInfoOffset, form);
            u_ASSERT_BAIL(0 == rc && "trouble reading line offset");
            u_TRACES && u_zprintf("%s found line offset %lld\n",
                                              rn, u::ll(lineNumberInfoOffset));

            obtained |= k_OBTAINED_LINE_OFFSET;
          } break;
          case DW_AT_comp_dir: {                          // DWARF doc 3.1.1.6
            rc = d_infoReader.readStringFromForm(&dirName, &d_strReader, form);
            u_ASSERT_BAIL(0 == rc);

            if (!dirName.empty()) {
                if ('/' != dirName[dirName.length() - 1]) {
                    dirName += '/';
                }

                u_TRACES && u_zprintf("%s dirName \"%s\" found\n",
                                                          rn, dirName.c_str());

                obtained |= k_OBTAINED_DIR_NAME;
            }
          } break;
#if 0 == u_DWARF_CHECK_ADDRESSES
          case DW_AT_low_pc:                              // DWARF doc 3.1.1.1
          case DW_AT_high_pc:                             // DWARF doc 3.1.1.1
          case DW_AT_ranges:                              // DWARF doc 3.1.1.1
#endif
          case DW_AT_language:                            // DWARF doc 3.1.1.3
          case DW_AT_macro_info:                          // DWARF doc 3.1.1.5
          case DW_AT_producer:                            // DWARF doc 3.1.1.7
          case DW_AT_identifier_case:                     // DWARF doc 3.1.1.8
          case DW_AT_base_types:                          // DWARF doc 3.1.1.9
          case DW_AT_use_UTF8:                            // DWARF doc 3.1.1.10
          case u::e_DW_AT_main_subprogram:                // DWARF doc 3.1.1.11
          case DW_AT_entry_pc:                            // not in doc, but
                                                          // crops up
          case DW_AT_description:
          case DW_AT_segment: {
            rc = d_infoReader.skipForm(form);
            u_ASSERT_BAIL((0 == rc && "problem skipping") || u_PH(attr) ||
                                                                   u_PH(form));
          } break;
          default: {
            u_ASSERT_BAIL((0 && "compile unit: unrecognized attribute")
                                                                || u_PH(attr));
          }
        }

        // If 'u_TRACES' is set, continue until we hit the terminating '0's to
        // verify that our code is handling everything in compile units.  If
        // 'u_TRACES' is not set, quit once we have the info we want.
    } while (u_TRACES || k_OBTAINED_ALL != obtained);

    if (u_TRACES) {
        u_zprintf("%s (attr, form) loop terminated, all%s obtained\n", rn,
                (k_OBTAINED_ALL == obtained) ? "" : " not");
        u_zprintf("%s base name %s, dir name %s, line # offset %s\n", rn,
                !baseName.empty()                   ? "found" : "not found",
                !dirName .empty()                   ? "found" : "not found",
                (obtained | k_OBTAINED_LINE_OFFSET) ? "found" : "not found");
    }

    *addressMatched = !!(obtained & k_OBTAINED_ADDRESS_MATCH);
    if (!*addressMatched) {
        return 0;                                                     // RETURN
    }

    u_ASSERT_BAIL((k_OBTAINED_LINE_OFFSET & obtained) || u_P(index));
    u_ASSERT_BAIL(!baseName.empty());
    u_ASSERT_BAIL(!dirName.empty());

    (void) u::cleanupString(&dirName, d_allocator_p);
    frameRec->setCompileUnitDir(     dirName);
    frameRec->setCompileUnitFileName(baseName);
    frameRec->setLineNumberOffset(lineNumberInfoOffset);

    return 0;
}

int u::StackTraceResolver::HiddenRec::dwarfReadDebugInfo()
{
    static const char rn[] = { "HiddenRec::dwarfDebugInfo:" };    (void) rn;

    const u::FrameRecVecIt end = d_frameRecsEnd;
    for (u::FrameRecVecIt it = d_frameRecsBegin, prev = end; it < end;
                                                             prev = it, ++it) {
        // Because the 'u::FrameRec's are sorted by address, those referring to
        // the same compilation unit are going to be adjacent.

        if (end != prev && d_arangesSec.d_size &&
                        u::maxOffset != it->compileUnitOffset() &&
                        prev->compileUnitOffset() == it->compileUnitOffset()) {
            u_TRACES && u_zprintf("%s frames %d and %d are from the same"
                                                     " compilation unit\n", rn,
                                                   prev->index(), it->index());

            if (!it->frame().isSourceFileNameKnown()) {
                // This is the name of the file that was compiled, which might
                // not be the right file.  If reading line number information
                // is successful, it will be overwritten by the right file.

                it->frame().setSourceFileName(prev->frame().sourceFileName());
            }
            it->setCompileUnitDir(            prev->compileUnitDir());
            it->setCompileUnitFileName(       prev->compileUnitFileName());
            it->setLineNumberOffset(          prev->lineNumberOffset());
            continue;
        }

        int rc = dwarfReadDebugInfoFrameRec(&*it);
        if (0 != rc) {
            // The fact that we failed on one doesn't mean we'll fail on the
            // others -- keep going.

            u_TRACES && u_zprintf("%s dwarfReadDebugInfoFrameRec failed on"
                                               " frame %d\n", rn, it->index());
        }
    }

    return 0;
}

int u::StackTraceResolver::HiddenRec::dwarfReadDebugInfoFrameRec(
                                                         u::FrameRec *frameRec)
{
    static const char rn[] = { "HiddenRec::dwarfDebugInfoFrameRec:" };
    (void) rn;

    int rc;
    const int index = frameRec->index(); (void) index;

    u_ASSERT_BAIL(0 < d_infoSec  .d_size);
    u_ASSERT_BAIL(0 < d_abbrevSec.d_size);
    u_ASSERT_BAIL(0 < d_rangesSec.d_size);
    u_ASSERT_BAIL(0 < d_strSec   .d_size);

    u_TRACES && u_zprintf("%s reading frame %d, symbol: %s\n", rn,
                                index, frameRec->frame().symbolName().c_str());

    rc = d_infoReader.init(  d_helper_p, d_scratchBufA_p, d_infoSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);
    rc = d_abbrevReader.init(d_helper_p, d_scratchBufB_p, d_abbrevSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);
    rc = d_rangesReader.init(d_helper_p, d_scratchBufC_p, d_rangesSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);
    rc = d_strReader.init(   d_helper_p, d_scratchBufD_p, d_strSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);

    bool addressMatched;

    u::Offset nextCompileUnitOffset = d_infoSec.d_offset;
    if (d_arangesSec.d_size && u::maxOffset != frameRec->compileUnitOffset()) {
        nextCompileUnitOffset += frameRec->compileUnitOffset();
    }

    do {
        rc = d_infoReader.skipTo(nextCompileUnitOffset);
        u_ASSERT_BAIL(0 == rc && "skipTo failed");

        {
            u::Offset compileUnitLength;
            rc = d_infoReader.readInitialLength(&compileUnitLength);
            u_ASSERT_BAIL(0 == rc);

            nextCompileUnitOffset = d_infoReader.offset() + compileUnitLength;
        }

        {
            unsigned short version;
            rc = d_infoReader.readValue(&version);
            u_ASSERT_BAIL(0 == rc && "read version failed");
            u_ASSERT_BAIL((2 <= version && version <= 4) || u_P(version));
        }

        // This is the compilation unit headers as outlined in 7.5.1.1

        {
            u::Offset abbrevOffset;
            rc = d_infoReader.readSectionOffset(&abbrevOffset);
            u_ASSERT_BAIL(0 == rc && "read abbrev offset failed");

            rc = d_abbrevReader.skipTo(d_abbrevSec.d_offset + abbrevOffset);
            u_ASSERT_BAIL(0 == rc);
        }

        rc = d_infoReader.readAddressSize();
        u_ASSERT_BAIL(0 == rc && "read address size failed");

        d_rangesReader.setAddressSize(d_infoReader.addressSize());

        {
            u::Offset readTagIdx;

            // These tag indexes were barely, vaguely mentioned by the doc,
            // with some implication that they'd be '1' before the first tag we
            // encounter.  If they turn out not to be '1' in production it's
            // certainly not worth abandoning DWARF decoding over.

            rc = d_infoReader.readULEB128(&readTagIdx);
            u_ASSERT_BAIL(0 == rc);
            (1 != readTagIdx && u_TRACES) && u_eprintf( // we don't care much
                                       "%s strange .debug_info tag idx %llx\n",
                                                        rn, u::ll(readTagIdx));

            rc = d_abbrevReader.readULEB128(&readTagIdx);
            u_ASSERT_BAIL(0 == rc);
            (1 != readTagIdx && u_TRACES) && u_eprintf( // we don't care much
                                     "%s strange .debug_abbrev tag idx %llx\n",
                                                        rn, u::ll(readTagIdx));
        }

        unsigned int tag;
        rc = d_abbrevReader.readULEB128(&tag);
        u_ASSERT_BAIL(0 == rc);

        u_ASSERT_BAIL(DW_TAG_compile_unit == tag ||
                                      DW_TAG_partial_unit == tag || u_PH(tag));

        BSLMF_ASSERT(0 == DW_CHILDREN_no && 1 == DW_CHILDREN_yes);
        unsigned int hasChildren;
        rc = d_abbrevReader.readULEB128(&hasChildren);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(hasChildren <= 1);    // other than that, we don't care

        rc = dwarfReadCompileOrPartialUnit(frameRec, &addressMatched);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(!d_arangesSec.d_size || addressMatched);
    } while (!d_arangesSec.d_size && !addressMatched &&
                nextCompileUnitOffset < d_infoSec.d_offset + d_infoSec.d_size);

    if (u_TRACES && !addressMatched) {
        // Sometimes there's a frame, like the routine that calls 'main', for
        // which no debug info is available.  This will also happen if a file
        // was compiled with the debug switch on.  A disappointment, but not an
        // error.

        u_zprintf("%s failed to match address for symbol \"%s\"\n", rn,
                                       frameRec->frame().symbolName().c_str());
    }

    d_infoReader.  disable();
    d_abbrevReader.disable();
    d_rangesReader.disable();
    d_strReader.   disable();

    return 0;
}

int u::StackTraceResolver::HiddenRec::dwarfReadDebugLine()
{
    static const char rn[] = { "HiddenRec::dwarfDebugLine:" };    (void) rn;

    const u::FrameRecVecIt end = d_frameRecsEnd;
    for (u::FrameRecVecIt it = d_frameRecsBegin, prev = end; it < end;
                                                             prev = it, ++it) {
        // Because the 'u::FrameRec's are sorted by address, those referring to
        // the same address are going to be adjacent.

        if (end != prev && prev->frame().address() == it->frame().address()) {
            u_TRACES && u_zprintf("%s recursion: frames %d and %d are from"
                                            " the same compilation unit\n", rn,
                                                   prev->index(), it->index());
            // Recursion on the stack -- file name, line # info will be
            // identical.

            it->frame().setSourceFileName(prev->frame().sourceFileName());
            it->frame().setLineNumber(    prev->frame().lineNumber());
            continue;
        }

        int rc = dwarfReadDebugLineFrameRec(&*it);
        if (0 != rc) {
            // The fact that we failed on one doesn't mean we'll fail on the
            // others -- keep going.

            u_TRACES && u_zprintf("%s dwarfReadDebugLineFrameRec failed on"
                                               " frame %d\n", rn, it->index());
            d_lineReader.disable();
        }
    }

    return 0;
}

int u::StackTraceResolver::HiddenRec::dwarfReadDebugLineFrameRec(
                                                         u::FrameRec *frameRec)
{
    static const char rn[] = { "HiddenRec:dwarfDebugLineFrameRec:" };
    (void) rn;

    int rc;

    rc = d_lineReader.init(d_helper_p, d_scratchBufA_p, d_lineSec,
                                                            d_libraryFileSize);
    u_ASSERT_BAIL(0 == rc);

    u_TRACES && u_zprintf(
                         "%s Symbol: %s, frame %d, lineNumberOffset: 0x%llx\n",
                                    rn, frameRec->frame().symbolName().c_str(),
                                                             frameRec->index(),
                                          u::ll(frameRec->lineNumberOffset()));

    if (u::maxOffset == frameRec->lineNumberOffset()) {
        u_TRACES && u_zprintf("%s no line number information for frame %d,"
                                   " cannot proceed\n", rn, frameRec->index());

        return -1;                                                    // RETURN
    }

    rc = d_lineReader.skipTo(d_lineSec.d_offset +
                                                 frameRec->lineNumberOffset());
    u_ASSERT_BAIL(0 == rc);

    u::Offset debugLineLength;
    rc = d_lineReader.readInitialLength(&debugLineLength);
    u_ASSERT_BAIL(0 == rc);

    rc = d_lineReader.setEndOffset(d_lineReader.offset() + debugLineLength);
    u_ASSERT_BAIL(0 == rc);

    u::Offset endOfHeader;
    {
        unsigned short version;
        d_lineReader.readValue(&version);
        u_ASSERT_BAIL(version >= 2 || u_P(version));

        rc = d_lineReader.readSectionOffset(&endOfHeader);
        u_ASSERT_BAIL(0 == rc);

        u_TRACES && u_zprintf("%s version: %u, header len: %llu\n",
                                              rn, version, u::ll(endOfHeader));

        endOfHeader += d_lineReader.offset();
    }

    unsigned char minInstructionLength;
    rc = d_lineReader.readValue(&minInstructionLength);
    u_ASSERT_BAIL(0 == rc);

    unsigned char maxOperationsPerInsruction;
    rc = d_lineReader.readValue(&maxOperationsPerInsruction);
    u_ASSERT_BAIL(0 == rc);

    u_TRACES && u_zprintf("%s debugLineLength: %lld minInst: %u maxOp: %u\n",
                                                    rn, u::ll(debugLineLength),
                             minInstructionLength, maxOperationsPerInsruction);

    0 && u::dumpBinary(&d_lineReader, 8);

#if 0
    // In section 6.2.4.5, the DWARF docs (versions 2, 3 and 4) say this field
    // should be there, but it's not in the binary.

    {
        unsigned char isStmt;
        rc = d_lineReader.readValue(&isStmt);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(isStmt <= 1 || u_P(isStmt));
    }
#endif

    int lineBase;
    {
        signed char sc;
        rc = d_lineReader.readValue(&sc);
        u_ASSERT_BAIL(0 == rc);
        lineBase = sc;
        u_ASSERT_BAIL(lineBase <= 1);
    }

    int lineRange;
    {
        unsigned char uc;
        rc = d_lineReader.readValue(&uc);
        u_ASSERT_BAIL(0 == rc);
        lineRange = uc;
        u_ASSERT_BAIL(0 < lineRange);
    }

    unsigned char opcodeBase;
    rc = d_lineReader.readValue(&opcodeBase);
    u_ASSERT_BAIL(0 == rc);
    u_ASSERT_BAIL(opcodeBase < 64);    // Should be 10 or 13, maybe plus a few.
                                     // 64 would be absurd.

    bsl::vector<unsigned char> opcodeLengths(d_allocator_p);    // # of ULEB128
                                                                // args for std
                                                                // opcodes
    opcodeLengths.resize(opcodeBase, 0);
    for (unsigned int ii = 1; ii < opcodeBase; ++ii) {
        rc = d_lineReader.readValue(&opcodeLengths[ii]);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(opcodeLengths[ii] <= 10 || u_P(opcodeLengths[ii]));
    }

    {
        int ii = 0;    (void) ii;
#undef  CHECK_ARG_COUNT
#define CHECK_ARG_COUNT(argCount, id)                                         \
        BSLS_ASSERT_SAFE(ii++ == id);                                         \
        u_ASSERT_BAIL(id >= opcodeBase || argCount == opcodeLengths[id]);

        CHECK_ARG_COUNT(0, 0);
        CHECK_ARG_COUNT(0, DW_LNS_copy);
        CHECK_ARG_COUNT(1, DW_LNS_advance_pc);
        CHECK_ARG_COUNT(1, DW_LNS_advance_line);
        CHECK_ARG_COUNT(1, DW_LNS_set_file);
        CHECK_ARG_COUNT(1, DW_LNS_set_column);
        CHECK_ARG_COUNT(0, DW_LNS_negate_stmt);
        CHECK_ARG_COUNT(0, DW_LNS_set_basic_block);
        CHECK_ARG_COUNT(0, DW_LNS_const_add_pc);
        CHECK_ARG_COUNT(1, DW_LNS_fixed_advance_pc);
        CHECK_ARG_COUNT(0, DW_LNS_set_prologue_end);
        CHECK_ARG_COUNT(0, DW_LNS_set_epilogue_begin);
        CHECK_ARG_COUNT(1, DW_LNS_set_isa);

#undef CHECK_ARG_COUNT
    }

    const bsl::string nullString(d_allocator_p);

    bsl::deque<bsl::string> dirPaths(d_allocator_p);
    dirPaths.push_back(frameRec->compileUnitDir());
    {
        bsl::string dir(d_allocator_p);
        for (;;) {
            rc = d_lineReader.readString(&dir);
            u_ASSERT_BAIL(0 == rc);

            if (dir.empty()) {
                break;
            }

            if ('/' != dir[dir.length() - 1]) {
                dir += '/';
            }

            dirPaths.push_back(dir);
        }

        if (0 && u_TRACES) {
            for (unsigned int ii = 0; ii < dirPaths.size(); ++ii) {
                u_zprintf("%s dirPaths[%u]: %s\n",
                                                 rn, ii, dirPaths[ii].c_str());
            }
        }
    }

    bsl::deque<bsl::string>  fileNames(d_allocator_p);
    fileNames.push_back(frameRec->compileUnitFileName());
    bsl::deque<unsigned int> dirIndexes(d_allocator_p);
    dirIndexes.push_back(0);

    for (unsigned int ii = 1;; ++ii) {
        fileNames.push_back(nullString);

        BSLS_ASSERT_SAFE(ii + 1 == fileNames.size());

        rc = d_lineReader.readString(&fileNames[ii]);
        u_ASSERT_BAIL(0 == rc);

        if (fileNames[ii].empty()) {
            break;
        }


        dirIndexes.push_back(-1);

        BSLS_ASSERT_SAFE(dirIndexes.size() == fileNames.size());

        rc = d_lineReader.readULEB128(&dirIndexes[ii]);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(dirIndexes[ii] < dirPaths.size());

        if (0 && u_TRACES) {
            u_zprintf("%s fileNames[%u]: %s dirIdx %lld:\n", rn, ii,
                                 fileNames[ii].c_str(), u::ll(dirIndexes[ii]));
        }

        rc = d_lineReader.skipULEB128();    // mod time, ignored
        u_ASSERT_BAIL(0 == rc);

        rc = d_lineReader.skipULEB128();    // file length, ignored
        u_ASSERT_BAIL(0 == rc);
    }
    u_ASSERT_BAIL(2 <= fileNames.size());
    fileNames.resize(fileNames.size() - 1);    // chomp empty entry
    BSLS_ASSERT_SAFE(dirIndexes.size() == fileNames.size());

    bsl::string  definedFile(d_allocator_p);     // in case a file is defined
    unsigned int definedDirIndex;                // by the DW_LNE_define_file
                                                 // cmd, in which case
                                                 // 'fileIdx' will be -1.

    u::UintPtr    addressToMatch =
                     reinterpret_cast<u::UintPtr>(frameRec->frame().address());
    u::UintPtr    address = 0, prevAddress = 0;
    unsigned int opIndex = 0;
    int          fileIdx = 0;
    int line = 1, prevLine = 1;                 // The spec says to begin line
                                                // numbers at 0, but they come
                                                // out right this way.

    {
        u::Offset toSkip = endOfHeader - d_lineReader.offset();
        u_TRACES && u_zprintf("%s Symbol %s, %ld dirs, %ld files, skip: %lld,"
                               " addrToMatch: 0x%lx, minLength: %u, maxOps: %u"
                                 " lineBase: %d, lineRange: %d, initLen: %lld,"
                                                               " opBase: %u\n",
             rn, frameRec->frame().symbolName().c_str(), u::l(dirPaths.size()),
                   u::l(fileNames.size()), u::ll(toSkip), u::l(addressToMatch),
                              minInstructionLength, maxOperationsPerInsruction,
                                   lineBase, lineRange, u::ll(debugLineLength),
                                                                   opcodeBase);

        u_ASSERT_BAIL(0 <= toSkip);
        d_lineReader.skipBytes(toSkip);
    }

    0 && u::dumpBinary(&d_lineReader, 24);

    bool nullPrevStatement = true;
    bool endOfSequence = false;
    int statementsSinceSetFile = 100;

    for (; !d_lineReader.atEndOfSection(); ++statementsSinceSetFile) {
        unsigned char opcode;
        rc = d_lineReader.readValue(&opcode);
        u_ASSERT_BAIL(0 == rc);

        bool statement = false;

        if (opcodeBase <= opcode) {
            // special opcode           // DWARF doc 6.2.5.1

            const int          opAdjust = opcode - opcodeBase;
            const unsigned int opAdvance = opAdjust / lineRange;
            const u::UintPtr    addressAdvance = minInstructionLength *
                          ((opIndex + opAdvance) / maxOperationsPerInsruction);
            const int          lineAdvance = lineBase + opAdjust % lineRange;

            u_TRACES && u_zprintf("%s special opcode %u, opAdj: %u, opAdv: %u,"
                                            " addrAdv: %lu, lineAdv: %d\n", rn,
                                                   opcode, opAdjust, opAdvance,
                                            u::l(addressAdvance), lineAdvance);

            statement = true;

            address += addressAdvance;
            opIndex = (opIndex + opAdvance) % maxOperationsPerInsruction;
            line += lineAdvance;

            u_ASSERT_BAIL(line >= 0);
        }
        else if (0 < opcode) {
            // standard opcode          // DWARF doc 6.2.5.2

            if (opcode <= DW_LNS_set_isa) {
                u_zprintf("%s standard opcode %s\n", rn,
                                              u::Reader::stringForLNS(opcode));
            }
            else {
                u_zprintf("%s unrecognized standard opcode %u\n", rn, opcode);
            }

            switch (opcode) {
              case DW_LNS_copy: {    // 1
                statement = true;
              } break;
              case DW_LNS_advance_pc: {    // 2
                u::UintPtr opAdvance;
                rc = d_lineReader.readULEB128(&opAdvance);
                u_ASSERT_BAIL(0 == rc);

                u::UintPtr advanceBy = minInstructionLength *
                          ((opIndex + opAdvance) / maxOperationsPerInsruction);

                u_TRACES && u_zprintf("%s advance@By: %lu\n",
                                                          rn, u::l(advanceBy));

                statement = true;
                address += advanceBy;
                opIndex = static_cast<unsigned int>(
                           (opIndex + opAdvance) % maxOperationsPerInsruction);
              } break;
              case DW_LNS_advance_line: {    // 3
                int lineAdvance;

                rc = d_lineReader.readLEB128(&lineAdvance);
                u_ASSERT_BAIL(0 == rc);
                u_ASSERT_BAIL(u::approxAbs(lineAdvance) <  10 * 1000 * 1000);

                u_TRACES && u_zprintf("%s advanceLnBy: %d\n", rn, lineAdvance);

                line += lineAdvance;

                u_ASSERT_BAIL(line >= 0);
              } break;
              case DW_LNS_set_file: {    // 4
                rc = d_lineReader.readULEB128(&fileIdx);
                u_ASSERT_BAIL(0 == rc);
                u_ASSERT_BAIL(0 <= fileIdx);
                u_ASSERT_BAIL(     fileIdx <
                                           static_cast<int>(fileNames.size()));

                nullPrevStatement = true;
                statementsSinceSetFile = 0;

                u_TRACES && u_zprintf("%s set file to %u %s%s\n", rn, fileIdx,
                                         dirPaths[dirIndexes[fileIdx]].c_str(),
                                                   fileNames[fileIdx].c_str());
              } break;
              case DW_LNS_set_column: {    // 5
                rc = d_lineReader.skipULEB128();    // ignored
                u_ASSERT_BAIL(0 == rc);
              } break;
              case DW_LNS_negate_stmt: {    // 6
                ;    // no args, ignored
              } break;
              case DW_LNS_set_basic_block: {    // 7
                ;    // no args, ignored
              } break;
              case DW_LNS_const_add_pc: {    // 8
                unsigned int opAdvance = (255 - opcodeBase) / lineRange;
                address += minInstructionLength *
                          ((opIndex + opAdvance) / maxOperationsPerInsruction);
                opIndex = (opIndex + opAdvance) % maxOperationsPerInsruction;
              } break;
              case DW_LNS_fixed_advance_pc: {    // 9
                unsigned short advance;
                rc = d_lineReader.readValue(&advance);
                u_ASSERT_BAIL(0 == rc);

                address += advance;
                opIndex = 0;
              } break;
              case DW_LNS_set_prologue_end: {    // 10
                statement = true;
              } break;
              case DW_LNS_set_epilogue_begin: {    // 11
                statement = true;
              } break;
              case DW_LNS_set_isa: {    // 12
                rc = d_lineReader.skipULEB128();    // ignored
                u_ASSERT_BAIL(0 == rc);
              } break;
              default: {
                u_ASSERT_BAIL(DW_LNS_set_isa < opcode);
                u_ASSERT_BAIL(                 opcode < opcodeLengths.size());

                // use 'opcodeLengths' to skip and ignore any arguments

                for (unsigned int ii = 0; ii < opcodeLengths[opcode]; ++ii) {
                    rc = d_lineReader.skipULEB128();
                    u_ASSERT_BAIL(0 == rc);
                }
              } break;
            }
        }
        else {
            BSLS_ASSERT_SAFE(0 == opcode);

            // expect extended opcode        // DWARF doc 6.2.5.3

            rc = d_lineReader.readValue(&opcode);
            u_ASSERT_BAIL(0 == rc);

            // Sometimes it's just a 0 followed by the extended opcode,
            // sometimes there's a 9 or a 5 between the 0 and the extended
            // opcode.

#if defined(BSLS_PLATFORM_CPU_64_BIT)
            if (9 == opcode) {                // Not in spec
#else
            if (5 == opcode) {                // Not in spec
#endif
                rc = d_lineReader.readValue(&opcode);
                u_ASSERT_BAIL(0 == rc);
            }

            u_zprintf("%s extended opcode %s\n",
                                          rn, u::Reader::stringForLNE(opcode));

            switch (opcode) {
              case DW_LNE_end_sequence: {    // 1
                0 && u::dumpBinary(&d_lineReader, 8);

                // There was an extra byte there in the example, but it's not
                // described in the spec, so its function is unknown.

                unsigned char uc;
                rc = d_lineReader.readValue(&uc);
                u_ASSERT_BAIL(!u_TRACES || 0 == rc);
                u_ASSERT_BAIL(!u_TRACES || 1 == uc); // it always seems to be 1
                                                     // only complain if it's
                                                     // not if traces are on

                endOfSequence = true;
                statement = true;
              } break;
              case DW_LNE_set_address: {    // 2
                rc = d_lineReader.readValue(&address);
                u_ASSERT_BAIL(0 == rc);
                address += d_adjustment;
                opIndex = 0;
                if (statementsSinceSetFile <= 1) {
                    // Apparently, if this statement immediately follows a
                    // 'set file', we are to reset the line #, otherwise leave
                    // it alone.

                    line = prevLine = 1;
                }
                nullPrevStatement = true;
              } break;
              case DW_LNE_define_file: {    // 3
                rc = d_lineReader.readString(&definedFile);
                u_ASSERT_BAIL(0 == rc);

                rc = d_lineReader.readULEB128(&definedDirIndex);
                u_ASSERT_BAIL(0 == rc);
                u_ASSERT_BAIL(definedDirIndex < dirIndexes.size());

                rc = d_lineReader.skipULEB128();    // mod time, ignored
                u_ASSERT_BAIL(0 == rc);

                rc = d_lineReader.skipULEB128();    // file length, ignored
                u_ASSERT_BAIL(0 == rc);

                fileIdx = -1;

                line = prevLine = 1;
              } break;
              case u::e_DW_LNE_set_discriminator: {
                rc = d_lineReader.skipULEB128();    // ignored
                u_ASSERT_BAIL(0 == rc);
              } break;
              default: {
                u_zprintf("%s unrecognized extended opcode %u\n", rn, opcode);

                (void) u::dumpBinary(&d_lineReader, 8);

                u_ASSERT_BAIL(u_P(opcode) && "unrecognized extended opcode");
              }
            }
        }

        if (statement) {
            u_TRACES && u_zprintf("%s stmt: addr: 0x%lx, line: %u\n",
                                                      rn, u::l(address), line);

            if (!nullPrevStatement &&
                   addressToMatch <= address && prevAddress < addressToMatch) {

                u_TRACES && u_zprintf(
                                     "%s stmt match: @'s(0x%lx, 0x%lx, 0x%lx),"
                                                             " line(%d, %d)\n",
                                   rn, u::l(prevAddress), u::l(addressToMatch),
                                                u::l(address), prevLine, line);

                frameRec->frame().setLineNumber(line);
                if (-1 == fileIdx) {
                    if ('/' != definedFile.c_str()[0]) {
                        definedFile.insert(0, dirPaths[definedDirIndex]);
                    }
                    (void) u::cleanupString(&definedFile, d_allocator_p);
                    frameRec->frame().setSourceFileName(definedFile);
                }
                else {
                    bsl::string& sfn = fileNames[fileIdx];

                    if ('/' != sfn.c_str()[0]) {
                        bsl::string& dir = dirPaths[dirIndexes[fileIdx]];
                        if (0 != dirIndexes[fileIdx] &&
                                                       '/' != dir.c_str()[0]) {
                            dir.insert(0, dirPaths[0]);
                        }
                        sfn.insert(0, dir);
                    }

                    (void) u::cleanupString(&sfn, d_allocator_p);

                    frameRec->frame().setSourceFileName(sfn);
                }

                u_TRACES && u_zprintf("%s stmt MATCH %s %s:%d\n", rn,
                                        frameRec->frame().symbolName().c_str(),
                                    frameRec->frame().sourceFileName().c_str(),
                                               frameRec->frame().lineNumber());

                d_lineReader.disable();

                return 0;                                             // RETURN
            }


            if (endOfSequence) {
                endOfSequence = false;
                u_TRACES && u_zprintf("%s ----------------------------\n", rn);

                address = 0, prevAddress = 0;
                opIndex = 0;
                fileIdx = 0;
                line = 1, prevLine = 1;
                nullPrevStatement = true;
            }
            else {
                prevLine    = line;
                prevAddress = address;
                nullPrevStatement = false;
            }
        }
    }

    d_lineReader.disable();

    return -1;    // 'addressToMatch' not matched
}

#endif // u_DWARF

void u::StackTraceResolver::HiddenRec::reset()
{
    // Note that 'd_frameRecs' and 'd_numTotalUnmatched' are not to be cleared
    // or reinitialized, they have a lifetime of the length of the resolve.

    d_helper_p           = 0;
    d_frameRecsBegin     = u::FrameRecVecIt();
    d_frameRecsEnd       = u::FrameRecVecIt();
    d_adjustment         = 0;
    d_symTableOffset     = 0;
    d_symTableSize       = 0;
    d_stringTableOffset  = 0;
    d_stringTableSize    = 0;
#ifdef u_DWARF
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

     // -----------------------------------------------------------------
     // class balst::u::StackTraceResolverImpl<balst::ObjectFileFormat::Elf>
     //                 == class U::u::StackTraceResolver
     // -----------------------------------------------------------------

// PRIVATE CREATORS
u::StackTraceResolver::StackTraceResolverImpl(
                                    balst::StackTrace *stackTrace,
                                    bool               demanglingPreferredFlag)
: d_hbpAlloc()
, d_stackTrace_p(stackTrace)
, d_scratchBufA_p(static_cast<char *>(
                                    d_hbpAlloc.allocate(u::k_SCRATCH_BUF_LEN)))
, d_scratchBufB_p(static_cast<char *>(
                                    d_hbpAlloc.allocate(u::k_SCRATCH_BUF_LEN)))
#ifdef u_DWARF
, d_scratchBufC_p(static_cast<char *>(
                                    d_hbpAlloc.allocate(u::k_SCRATCH_BUF_LEN)))
, d_scratchBufD_p(static_cast<char *>(
                                    d_hbpAlloc.allocate(u::k_SCRATCH_BUF_LEN)))
#endif
, d_hidden(*(new (d_hbpAlloc) HiddenRec(this)))    // must be after scratch
                                                   // buffers
, d_demangle(demanglingPreferredFlag)
{
}

// PRIVATE MANIPULATORS
int u::StackTraceResolver::loadSymbols(int matched)
{
    enum { k_SYM_SIZE = sizeof(u::ElfSymbol) };

    char *symbolBuf = d_scratchBufA_p;
    char *stringBuf = d_scratchBufB_p;

    const int       maxSymbolsPerPass = u::k_SCRATCH_BUF_LEN / k_SYM_SIZE;
    const u::Offset numSyms = d_hidden.d_symTableSize / k_SYM_SIZE;
    u::Offset       sourceFileNameOffset = u::maxOffset;

    unsigned       numSymsThisTime;
    for (u::Offset symIndex = 0; symIndex < numSyms;
                                                 symIndex += numSymsThisTime) {
        numSymsThisTime = static_cast<unsigned>(
                      bsl::min<Offset>(numSyms - symIndex, maxSymbolsPerPass));

        const u::Offset offsetToRead = d_hidden.d_symTableOffset +
                                                         symIndex * k_SYM_SIZE;
        int          rc = d_hidden.d_helper_p->readExact(
                                                  symbolBuf,
                                                  numSymsThisTime * k_SYM_SIZE,
                                                  offsetToRead);
        if (rc) {
            u_eprintf(
                     "failed to read %lu symbols from offset %llu, errno %d\n",
                    u::l(numSymsThisTime),
                    u::ll(offsetToRead),
                    errno);
            return -1;                                                // RETURN
        }

        const u::ElfSymbol *symBufStart = static_cast<u::ElfSymbol *>(
                                               static_cast<void *>(symbolBuf));
        const u::ElfSymbol *symBufEnd   = symBufStart + numSymsThisTime;
        for (const u::ElfSymbol *sym = symBufStart; sym < symBufEnd; ++sym) {
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
                    const u::FrameRecVecIt begin =
                                bsl::lower_bound(d_hidden.d_frameRecsBegin,
                                                 d_hidden.d_frameRecsEnd,
                                                 u::FrameRec(symbolAddress,
                                                             0,
                                                             0,
                                                             &d_hbpAlloc));
                    const u::FrameRecVecIt end =
                             bsl::lower_bound(d_hidden.d_frameRecsBegin,
                                              d_hidden.d_frameRecsEnd,
                                              u::FrameRec(endSymbolAddress,
                                                          0,
                                                          0,
                                                          &d_hbpAlloc));
                    for (u::FrameRecVecIt it = begin; it < end; ++it) {
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
                           && u::maxOffset != sourceFileNameOffset) {
                            frame.setSourceFileName(
                                      d_hidden.d_helper_p->loadString(
                                           d_hidden.d_stringTableOffset +
                                                          sourceFileNameOffset,
                                           stringBuf,
                                           u::k_SCRATCH_BUF_LEN,
                                           &d_hbpAlloc));
                        }

                        frame.setMangledSymbolName(
                                  d_hidden.d_helper_p->loadString(
                                           d_hidden.d_stringTableOffset +
                                                                  sym->st_name,
                                           stringBuf,
                                           u::k_SCRATCH_BUF_LEN,
                                           &d_hbpAlloc));
                        if (frame.isMangledSymbolNameKnown()) {
                            setFrameSymbolName(&frame,
                                               stringBuf,
                                               u::k_SCRATCH_BUF_LEN);

                            it->setSymbolResolved();

                            u_TRACES && u_zprintf(
                                    "Resolved symbol %s, frame %d, [%p, %p)\n",
                                    frame.symbolName().c_str(),
                                    it->index(),
                                    symbolAddress,
                                    endSymbolAddress);

                            if (0 == --matched) {
                                u_TRACES && u_zprintf(
                                            "Last symbol in segment loaded\n");

                                return 0;                             // RETURN
                            }
                        }
                        else {
                            u_TRACES && u_zprintf("Null symbol found for %p\n",
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

int u::StackTraceResolver::processLoadedImage(const char *fileName,
                                              const void *programHeaders,
                                              int         numProgramHeaders,
                                              void       *textSegPtr,
                                              void       *baseAddress)
    // Note this must be public so 'linkMapCallBack' can call it on Solaris.
    // Also note that it assumes that both scratch buffers are available for
    // writing.
{
    static const char rn[] = { "Resolver::processLoadedImage" };    (void) rn;

    BSLS_ASSERT(!textSegPtr || !baseAddress);

    // Scratch buffers: some platforms use A to read the link, then
    // 'resolveSegment' will trash both A and B.

    d_hidden.reset();

#if defined(BSLS_PLATFORM_OS_HPUX)
    const char *name = fileName;
#else
    const char *name = 0;
    if (fileName && fileName[0]) {
        if (u::e_IS_LINUX) {
            d_hidden.d_isMainExecutable = false;
        }

        name = fileName;
    }
    else {
        if (u::e_IS_LINUX) {
            d_hidden.d_isMainExecutable = true;
        }
        else {
            u_ASSERT_BAIL(d_hidden.d_isMainExecutable);
        }

        // On Solaris and Linux, 'fileName' is sometimes null for the main
        // executable file, but those platforms have a standard virtual symlink
        // that points to the executable file name.

        const int numChars = static_cast<int>(readlink("/proc/self/exe",
                                                       d_scratchBufA_p,
                                                       u::k_SCRATCH_BUF_LEN));
        if (numChars > 0) {
            u_ASSERT_BAIL(numChars < u::k_SCRATCH_BUF_LEN);
            d_scratchBufA_p[numChars] = 0;
            name = d_scratchBufA_p;
        }
        else {
            u_TRACES && u_zprintf("readlink of /proc/self/exe failed\n");

            return -1;                                                // RETURN
        }
    }
#endif
    name = bdlb::String::copy(name, &d_hbpAlloc);   // so we can trash the
                                                    // scratch buffers later

    u_TRACES && u_zprintf("processing loaded image: fn:\"%s\", name:\"%s\""
                                         " main:%d numHdrs:%d unmatched:%ld\n",
                        fileName ? fileName : "(null)", name ? name : "(null)",
                                 static_cast<int>(d_hidden.d_isMainExecutable),
                         numProgramHeaders, u::l(d_hidden.d_frameRecs.size()));

    balst::StackTraceResolver_FileHelper helper(name);
    d_hidden.d_helper_p = &helper;

    for (int i = 0; i < numProgramHeaders; ++i) {
        const u::ElfProgramHeader *ph =
                  static_cast<const u::ElfProgramHeader *>(programHeaders) + i;
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

int u::StackTraceResolver::resolveSegment(void       *segmentBaseAddress,
                                          void       *segmentPtr,
                                          u::UintPtr  segmentSize,
                                          const char *libraryFileName)
{
    int rc;

    // Scratch Buffers: beginning: 'sec' is A
    // Then 'loadSymbols' trashes both A and B
    // Then 'readDwarfAll' trasnes both A and B

    d_hidden.d_addressRange.d_address =
                                      reinterpret_cast<u::UintPtr>(segmentPtr);
    d_hidden.d_addressRange.d_size    = segmentSize;

    const char *sp = static_cast<char *>(segmentPtr);
    const char *se = sp + segmentSize;

    d_hidden.d_frameRecsBegin = bsl::lower_bound(d_hidden.d_frameRecs.begin(),
                                                 d_hidden.d_frameRecs.end(),
                                                 u::FrameRec(sp,
                                                             0,
                                                             0,
                                                             &d_hbpAlloc));
    d_hidden.d_frameRecsEnd   = bsl::lower_bound(d_hidden.d_frameRecs.begin(),
                                                 d_hidden.d_frameRecs.end(),
                                                 u::FrameRec(se,
                                                             0,
                                                             0,
                                                             &d_hbpAlloc));

    int matched = static_cast<int>(
                          d_hidden.d_frameRecsEnd - d_hidden.d_frameRecsBegin);
    BSLS_ASSERT(0 <= matched);
    BSLS_ASSERT(matched <= d_stackTrace_p->length());

    u_TRACES && u_zprintf(
                       "ResolveSegment lfn=%s\nba=%p sp=%p se=%p matched=%d\n",
                         libraryFileName, segmentBaseAddress, sp, se, matched);

    if (0 == matched) {
        u_TRACES && u_zprintf(
                         "0 addresses match in library %s\n", libraryFileName);

        return 0;                                                     // RETURN
    }

    d_hidden.d_libraryFileSize = bdls::FilesystemUtil::getFileSize(
                                                              libraryFileName);

    bsl::string libName(libraryFileName, &d_hbpAlloc);
    int cleanupRc = u::cleanupString(&libName, &d_hbpAlloc);

    u::FrameRecVecIt it, end  = d_hidden.d_frameRecsEnd;
    for (it = d_hidden.d_frameRecsBegin; it < end; ++it) {
        u_TRACES && u_zprintf("address %p MATCH\n", it->address());
        it->frame().setLibraryFileName(0 == cleanupRc ? libName.c_str()
                                                      : libraryFileName);
    }

    // read the elf header

    u::ElfHeader elfHeader;
    rc = d_hidden.d_helper_p->readExact(&elfHeader,
                                        sizeof(u::ElfHeader),
                                        0);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    if (0 != u::checkElfHeader(&elfHeader)) {
        return -1;                                                    // RETURN
    }

    d_hidden.d_adjustment = reinterpret_cast<u::UintPtr>(segmentBaseAddress);

    // find the section headers we're interested in, that is, .symtab and
    // .strtab, or, if the file was stripped, .dynsym and .dynstr

    u::ElfSectionHeader symTabHdr, strTabHdr, dynSymHdr, dynStrHdr;
    bsl::memset(&symTabHdr, 0, sizeof(u::ElfSectionHeader));
    bsl::memset(&strTabHdr, 0, sizeof(u::ElfSectionHeader));
    bsl::memset(&dynSymHdr, 0, sizeof(u::ElfSectionHeader));
    bsl::memset(&dynStrHdr, 0, sizeof(u::ElfSectionHeader));

    // Possible speedup: read all the section headers at once instead of one at
    // a time.

    int        numSections = elfHeader.e_shnum;
    u::UintPtr sectionHeaderSize = elfHeader.e_shentsize;
    u::Offset  sectionHeaderOffset = elfHeader.e_shoff;
    if (u::k_SCRATCH_BUF_LEN < sectionHeaderSize) {
        return -1;                                                    // RETURN
    }
    u::ElfSectionHeader *sec = static_cast<u::ElfSectionHeader *>(
                                         static_cast<void *>(d_scratchBufA_p));

    // read the string table that is used for section names

    int       stringSectionIndex = elfHeader.e_shstrndx;
    u::Offset stringSectionHeaderOffset =
                  sectionHeaderOffset + stringSectionIndex * sectionHeaderSize;
    if (0 != d_hidden.d_helper_p->readExact(sec,
                                            sectionHeaderSize,
                                            stringSectionHeaderOffset)) {
        return -1;                                                    // RETURN
    }
    u::UintPtr headerStringsOffset = sec->sh_offset;

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

        u_zprintf("Section: type:%d name:%s\n", sec->sh_type, sectionName);

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
#ifdef u_DWARF
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

    u_TRACES && u_zprintf("symtab:(0x%llx, 0x%llx), strtab:(0x%llx, 0x%llx)\n",
            u::ll(symTabHdr.sh_offset), u::ll(symTabHdr.sh_size),
            u::ll(strTabHdr.sh_offset), u::ll(strTabHdr.sh_size));
    u_TRACES && u_zprintf("dynsym:(0x%llx, %llu), dynstr:(0x%llx, %llu)\n",
            u::ll(dynSymHdr.sh_offset), u::ll(dynSymHdr.sh_size),
            u::ll(dynStrHdr.sh_offset), u::ll(dynStrHdr.sh_size));

    if (0 != strTabHdr.sh_size && 0 != symTabHdr.sh_size) {
        // use the full symbol table if it is available

        d_hidden.d_symTableOffset    = symTabHdr.sh_offset;
        d_hidden.d_symTableSize      = symTabHdr.sh_size;
        d_hidden.d_stringTableOffset = strTabHdr.sh_offset;
        d_hidden.d_stringTableSize   = strTabHdr.sh_size;
    }
    else if (0 != dynSymHdr.sh_size && 0 != dynStrHdr.sh_size) {
        // otherwise use the dynamic symbol table

        d_hidden.d_symTableOffset    = dynSymHdr.sh_offset;
        d_hidden.d_symTableSize      = dynSymHdr.sh_size;
        d_hidden.d_stringTableOffset = dynStrHdr.sh_offset;
        d_hidden.d_stringTableSize   = dynStrHdr.sh_size;
    }
    else {
        // otherwise fail

        return -1;                                                    // RETURN
    }

    u_TRACES && u_zprintf(
                   "Sym table:(0x%llx, 0x%llx) string table:(0x%llx 0x%llx)\n",
            u::ll(d_hidden.d_symTableOffset),
            u::ll(d_hidden.d_symTableSize),
            u::ll(d_hidden.d_stringTableOffset),
            u::ll(d_hidden.d_stringTableSize));

#ifdef u_DWARF
    u_TRACES && u_zprintf("abbrev:(0x%llx, 0x%llx) aranges:(0x%llx, 0x%llx)"
            " info:(0x%llx 0x%llx) line::(0x%llx 0x%llx)"
            " ranges:(0x%llx, 0x%llx) str:(0x%llx, 0x%llx)\n",
            u::ll(d_hidden.d_abbrevSec.d_offset),
            u::ll(d_hidden.d_abbrevSec.d_size),
            u::ll(d_hidden.d_arangesSec.d_offset),
            u::ll(d_hidden.d_arangesSec.d_size),
            u::ll(d_hidden.d_infoSec.d_offset),
            u::ll(d_hidden.d_infoSec.d_size),
            u::ll(d_hidden.d_lineSec.d_offset),
            u::ll(d_hidden.d_lineSec.d_size),
            u::ll(d_hidden.d_rangesSec.d_offset),
            u::ll(d_hidden.d_rangesSec.d_size),
            u::ll(d_hidden.d_strSec.d_offset),
            u::ll(d_hidden.d_strSec.d_size));
#endif

    // Note that 'loadSymbols' trashes scratchBufA and scratchBufB.

    rc = loadSymbols(matched);
    if (rc) {
        u_eprintf("loadSymbols failed\n");
        return -1;                                                    // RETURN
    }

    // we return 'rc' at the end.

#ifdef u_DWARF
    // Note that 'readDwarfAll' trashes scratchBufs A, B, C, and D

    rc = d_hidden.dwarfReadAll();
    if (rc) {
        u_TRACES && u_zprintf("readDwarf failed\n");
    }
#endif

    if (u_TRACES && 0 == (d_hidden.d_numTotalUnmatched -= matched)) {
        u_zprintf("Last address in stack trace matched\n");
    }

    return 0;
}

// PRIVATE ACCESSORS
void u::StackTraceResolver::setFrameSymbolName(
                                       balst::StackTraceFrame *frame,
                                       char                   *buffer,
                                       bsl::size_t             bufferLen) const
{
#if !defined(BSLS_PLATFORM_OS_SOLARIS)                                        \
 || defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    // Linux or Sun g++ or HPUX

    (void) buffer;    // silence 'unused' warning
    (void) bufferLen; // silence 'unused' warning

    int          status = -1;
    if (d_demangle) {
        // note the demangler uses 'malloc' to allocate its result

#if defined(BSLS_PLATFORM_OS_HPUX)
        char *demangled = __cxa_demangle(frame->mangledSymbolName().c_str(),
                                         0,
                                         0,
                                         &status);
#else
        char *demangled = abi::__cxa_demangle(
                                            frame->mangledSymbolName().c_str(),
                                            0,
                                            0,
                                            &status);
#endif
        u::FreeGuard guard(demangled);
        frame->setSymbolName(demangled ? demangled : "");
    }
    if (0 != status || frame->symbolName().empty()) {
        u_TRACES && u_zprintf("Did not demangle: status: %d\n", status);
        frame->setSymbolName(frame->mangledSymbolName());
    }
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)                                         \
 && !(defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
    // Sun CC

    int status = -1;

    if (d_demangle) {
        typedef int (*FuncPtr)(const char *, char *, bsl::size_t);
        static FuncPtr funcPtr = 0;
        static bool    dlopenAttempted = false;

        // The dynamic lib that contains the C++ demangler on Solaris is not
        // linked in by default unless the '-ldemangle' is set, which is
        // often not the case.

        if (!dlopenAttempted) {
            dlopenAttempted = true;

            ::dlerror();    // clear 'dlerror'

            // 'dlopen' will load the shared lib.  If the lib has already been
            // loaded elsewhere in the program, this will not result in
            // multiple copies -- 'dlopoen' will just return a handle to the
            // the original copy and increase its reference count.

            void *handle = ::dlopen("libdemangle.so", RTLD_LAZY);
            if (!handle) {
                u_eprintf("bslst: demangle: 'dlopen' failed, msg: \"%s\"\n",
                                                                  ::dlerror());
            }
            else {
                // 'dlopen' was successful, now get the pointer to
                // 'cplus_demangle' within that shared lib.

                ::dlerror();    // clear 'dlerror'

                funcPtr = reinterpret_cast<FuncPtr>(
                                            ::dlsym(handle, "cplus_demangle"));
                if (!funcPtr) {
                    u_eprintf("balst: demangle: 'dlsym' failed, msg: \"%s\"\n",
                                                                  ::dlerror());
                }
            }
        }
        if (funcPtr) {
            status = (*funcPtr)(frame->mangledSymbolName().c_str(),
                                buffer,
                                bufferLen);
        }
    }

    frame->setSymbolName(0 == status ? buffer
                                     : frame->mangledSymbolName().c_str());
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
                    bsl::size_t          size,
                    void                *data)
    // This routine is called once for the executable file, and once for every
    // shared library that is loaded.  The specified 'info' contains a pointer
    // to the relevant information in the link map.  The specified 'size' is
    // unused, the specified 'data' is a pointer to the elf resolver.  Return 0
    // on success and a non-zero value otherwise.
{
    (void) size;

    u::StackTraceResolver *resolver =
                               reinterpret_cast<u::StackTraceResolver *>(data);

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
int u::StackTraceResolver::resolve(
                                    balst::StackTrace *stackTrace,
                                    bool               demanglingPreferredFlag)
{
    static const char rn[] = { "Resolver::resolve" };    (void) rn;

#if defined(BSLS_PLATFORM_OS_HPUX)

    int rc;

    u::StackTraceResolver resolver(stackTrace,
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

    u::ElfProgramHeader *programHeaders = 0;
    int maxNumProgramHeaders = 0;

    u::ElfHeader elfHeader;
    for (int i = -1;
                0 < resolver.numUnmatchedFrames() && -1 != shl_get_r(i, &desc);
                                                                         ++i) {
        int numProgramHeaders = 0;

        {
            // this block limits the lifetime of 'helper' below

            u_TRACES && u_zprintf("(%d) %s 0x%lx-0x%lx\n",
                    i,
                    desc.filename && desc.fileName[0] ? desc.fileName :"(null)"
                    desc.tstart,
                    desc.tend);

            // index 0 is for the main executable

            resolver.d_hidden.d_isMainExecutable = (0 == i);

            // note this will be opened twice, here and in 'processLoadedImage'

            balst::StackTraceResolver_FileHelper helper(desc.filename);

            rc = helper.readExact(&elfHeader, sizeof(elfHeader), 0);
            u_ASSERT_BAIL(0 == rc);

            numProgramHeaders = elfHeader.e_phnum;
            if (numProgramHeaders > maxNumProgramHeaders) {
                programHeaders = static_cast<u::ElfProgramHeader *>(
                       resolver.d_hbpAlloc.allocate(
                         numProgramHeaders * sizeof(u::ElfProgramHeader)));
                maxNumProgramHeaders = numProgramHeaders;
            }

            rc = helper.readExact(
                           programHeaders,
                           numProgramHeaders * sizeof(u::ElfProgramHeader),
                           elfHeader.e_phoff);
            u_ASSERT_BAIL(0 == rc);
        }

        rc = resolver.processLoadedImage(
                                   desc.filename,
                                   programHeaders,
                                   numProgramHeaders,
                                   static_cast<void *>(desc.tstart),
                                   0);
        u_ASSERT_BAIL(0 == rc);
    }

#elif defined(BSLS_PLATFORM_OS_LINUX)

    u::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);

    // 'dl_iterate_phdr' will iterate over all loaded files, the executable and
    // all the .so's.  It doesn't exist on Solaris.

    dl_iterate_phdr(&linkmapCallback,
                    &resolver);

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    u::StackTraceResolver resolver(stackTrace,
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

    u::ElfDynamic *dynamic = reinterpret_cast<u::ElfDynamic *>(&_DYNAMIC);
    u_ASSERT_BAIL(dynamic);

    for (; true; ++dynamic) {
        // DT_NULL means we reached then end of list without finding the link
        // map

        u_ASSERT_BAIL(DT_NULL != dynamic->d_tag);

        if (DT_DEBUG == dynamic->d_tag) {
            r_debug *rdb = reinterpret_cast<r_debug *>(dynamic->d_un.d_ptr);
            u_ASSERT_BAIL(0 != rdb);

            linkMap = rdb->r_map;
            break;
        }
    }
#endif

    for (int i = 0; 0 < resolver.numUnmatchedFrames() && linkMap;
                                              ++i, linkMap = linkMap->l_next) {
        u::ElfHeader *elfHeader = reinterpret_cast<u::ElfHeader *>(
                                                              linkMap->l_addr);

        if (0 != u::checkElfHeader(elfHeader)) {
            return -1;
        }

        u::ElfProgramHeader *programHeaders =
                               reinterpret_cast<u::ElfProgramHeader *>(
                                         linkMap->l_addr + elfHeader->e_phoff);

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
int u::StackTraceResolver::numUnmatchedFrames() const
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
