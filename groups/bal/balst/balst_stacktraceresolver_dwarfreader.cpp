// balst_stacktraceresolver_dwarfreader.cpp                           -*-C++-*-

#include <balst_stacktraceresolver_dwarfreader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolver_dwarfreader_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_DWARF

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_climits.h>
#include <bsl_cstdarg.h>

#include <dwarf.h>

// Implementation note: 'dwarf.h' defines no type, just a lot of enumerated
// values (and not even any enumeration types).  The organiztion of the data
// in the file is all described in prose in the 300 page spec, and often things
// are organized such that fixed-length 'structs' can't represent the data.
//
// One benefit of this is that it's not necessary for any of the include files
// in balst to include 'dwarf.h', it can just be included in the .cpp files.

// ============================================================================
//              Debugging trace macros: 'u_eprintf' and 'u_zprintf'
// ============================================================================

#undef  u_TRACES
#define u_TRACES 0  // 0 == debugging traces off, u_eprintf and u_zprintf do
                    // nothing
                    // 1 == debugging traces on, u_eprintf is like u_zprintf
                    // 2 == debugging traces on, u_eprintf prints & exits

// u_zprintf: For non-error debug traces.  Traces:  0: null function
//                                                 >0: like printf
// u_eprintf: Called when errors occur in data.  Traces: 0: null function
//                                                       1: like printf
//                                                       2: printf, then exit 1
// u_ASSERT_BAIL(expr) gentle assert.  Traces: 0: if !(expr) return -1;
//                                            >0: if !(expr) msg & return -1
//     // 'u_ASSERT_BAIL' is needed because we need asserts for the DWARF code,
//     // but it's totally inappropriate to core dump if the DWARF data is
//     // corrupt, because we can give a pretty good stack trace without the
//     // DWARF information, so if 'expr' fails, just quit the DWARF analysis
//     // and continue showing the rest of the stack trace to the user.
//
// u_P(expr): u_TRACES:  0: evaluates to 'false'.
//                      >0: output << #expr << (expr) then evaluate to false.
//    // u_P(expr) is to be used in ASSERT_BAIL, i.e.
//    // 'ASSERT_BAIL(5 == x || u_P(x))' will, if (5 != x), print out the value
//    // of x then print the assert message and return -1.
//
// u_PH(expr): like u_P(expr), except prints the value in hex.

#undef u_eprintf
#undef u_zprintf
#undef ASSERT_BAIL
#undef P
#undef PH

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
    // -1.  Note that if 'u_TRACES > 1' the 'u_eprintf' will exit 1.

// 'P()' and 'PH()' are only uused in u_ASSERT_BAIL in DWARF code, only define
// these functions in DWARF compiles, otherwise get unused messages.

static bool warnPrint(const char                       *name,
                      BloombergLP::bsls::Types::Uint64  value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'P'.  Return 'false'.
{
    u_zprintf("%s = %llu\n", name, value);

    return false;
}

static bool warnPrintHex(const char                        *expr,
                         BloombergLP::bsls::Types::Uint64   value)
    // Print out the specified 'value' and the specified 'name', which is the
    // name of the expression have value 'value'.  This function is intended
    // only to be called by the macro 'PH'.  Return 'false'.
{
    u_zprintf("%s = 0x%llx\n", expr, value);

    return false;
}

#define u_P(expr)     warnPrint(   #expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // decimal and return 'false'.

#define u_PH(expr)    warnPrintHex(#expr, (expr))
    // Print '<source code for 'expr'> = <value of expr' with the value in
    // hex and return 'false'.

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

#define u_P(expr)            (false)
    // Ignore 'expr' and return 'false'.
#define u_PH(expr)           (false)
    // Ignore 'expr' and return 'false'.

#endif

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
# define u_ASSERT_BAIL_SAFE(expr) u_ASSERT_BAIL(expr)
    // Do u_ASSERT_BAIL
#else
# define u_ASSERT_BAIL_SAFE(expr)
    // Do nothing.
#endif

namespace BloombergLP {
namespace balst {

                // ---------------------------------------
                // StackTraceResolver_DwarfReader::Section
                // ---------------------------------------

// CREATOR
StackTraceResolver_DwarfReader::Section::Section()
: d_offset(0)
, d_size(0)
{
}

// MANIPULATOR
void StackTraceResolver_DwarfReader::Section::reset(Offset offset,
                                                    Offset size)
{
    if (offset < 0 || size < 0) {
        u_TRACES && u_eprintf("reset: negative offset or size\n");
        offset = 0;
        size   = 0;
    }
    else {
        d_offset = offset;
        d_size   = size;
    }
}

                    // ------------------------------
                    // StackTraceResolver_DwarfReader
                    // ------------------------------

// PRIVATE MANIPULATORS
int StackTraceResolver_DwarfReader::reload(bsl::size_t numBytes)
{
    static const char rn[] = { "Reader::reload:" };    (void) rn;

    u_ASSERT_BAIL(static_cast<Offset>(numBytes) <= d_endOffset - offset());

    BSLS_ASSERT_SAFE(d_buffer_p);
    u_ASSERT_BAIL_SAFE(numBytes <= k_SCRATCH_BUF_LEN);
    u_ASSERT_BAIL_SAFE(d_readPtr <= d_endPtr);

    d_offset += d_readPtr - d_buffer_p;
    u_ASSERT_BAIL_SAFE(d_offset <= d_endOffset);

    unsigned lengthToRead = static_cast<unsigned>(
              bsl::min<Offset>(d_endOffset - d_offset, k_SCRATCH_BUF_LEN));
    u_ASSERT_BAIL(lengthToRead >= numBytes || u_P(lengthToRead) ||
                                                                u_P(numBytes));
    d_helper_p->readExact(d_buffer_p, lengthToRead, d_offset);

    d_readPtr = d_buffer_p;
    d_endPtr  = d_readPtr + lengthToRead;

    return 0;
}

// CLASS METHODS
const char *StackTraceResolver_DwarfReader::stringForAt(unsigned id)
{
    static const char rn[] = { "dwarfStringForAt:" };

#undef CASE
#define CASE(x)    case x: return #x

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
      CASE(e_DW_AT_signature);
      CASE(e_DW_AT_main_subprogram);
      CASE(e_DW_AT_data_bit_offset);
      CASE(e_DW_AT_const_expr);
      CASE(e_DW_AT_enum_class);
      CASE(e_DW_AT_linkage_name);
      default: {
        u_eprintf("%s unrecognized 'DW_AT_? value = 0x%x\n", rn, id);

        return "DW_AT_????";                                          // RETURN
      }
    }
#undef CASE
}

const char *StackTraceResolver_DwarfReader::stringForForm(
                                                                   unsigned id)
{
    static const char rn[] = { "dwarfStringForForm:" };

#undef CASE
#define CASE(x)    case x: return #x

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
        u_eprintf("%s unrecognized 'DW_FORM_?' value = 0x%x\n", rn, id);

        return "DW_FORM_????";                                        // RETURN
      }
    }
#undef CASE
}

const char *StackTraceResolver_DwarfReader::stringForInlineState(
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
        u_eprintf("%s unrecognized 'DW_INL_?' value = 0x%x\n", rn,
                                                                  inlineState);

        return "DW_INL_????";                                         // RETURN
      }
    }
#undef CASE
}

const char *StackTraceResolver_DwarfReader::stringForLNE(unsigned id)
{
    static const char rn[] = { "dwarfStringForLNE:" };

#undef CASE
#define CASE(x)    case x: return #x

    switch (id) {
      CASE(DW_LNE_end_sequence);
      CASE(DW_LNE_set_address);
      CASE(DW_LNE_define_file);
      CASE(e_DW_LNE_set_discriminator);
      default: {
        u_eprintf("%s unrecognized 'DW_LNE_?' value = %u\n", rn, id);

        return "DW_LNE_????";                                         // RETURN
      }
    }
#undef CASE
}

const char *StackTraceResolver_DwarfReader::stringForLNS(unsigned id)
{
    static const char rn[] = { "dwarfStringForLNS:" };

#undef CASE
#define CASE(x)    case x: return #x

    switch (id) {
      CASE(DW_LNS_copy);
      CASE(DW_LNS_advance_pc);
      CASE(DW_LNS_advance_line);
      CASE(DW_LNS_set_file);
      CASE(DW_LNS_set_column);
      CASE(DW_LNS_negate_stmt);
      CASE(DW_LNS_set_basic_block);
      CASE(DW_LNS_const_add_pc);
      CASE(DW_LNS_fixed_advance_pc);
      CASE(DW_LNS_set_prologue_end);
      CASE(DW_LNS_set_epilogue_begin);
      CASE(DW_LNS_set_isa);
      default: {
        u_eprintf("%s unrecognized 'DW_LNS_?' value = %u\n", rn, id);

        return "DW_LNS_????";                                         // RETURN
      }
    }
#undef CASE
}

const char *StackTraceResolver_DwarfReader::stringForTag(unsigned tag)
{
    static const char rn[] = { "dwarfStringForTag:" };

#undef CASE
#define CASE(x)    case x: return #x

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
      CASE(e_DW_TAG_mutable_type);
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
            u_eprintf("%s unrecognized 'DW_TAG_?' value = 0x%x\n", rn, tag);
        }
        else {
            u_eprintf("%s unrecognized user-defined 'DW_TAG_?' value = 0x%x\n",
                                                                      rn, tag);
        }

        return "DW_TAG_????";                                         // RETURN
      }
    }
#undef CASE
}

// CREATORS
StackTraceResolver_DwarfReader::StackTraceResolver_DwarfReader()
{
    typedef bsls::Types::Uint64 Uint64;

    BSLMF_ASSERT(sizeof(Uint64) == sizeof(Offset));
    BSLMF_ASSERT(sizeof(UintPtr) == sizeof(void *));
    BSLMF_ASSERT(static_cast<Offset>(-1) < 0);

    disable();
}

// MANIPULATORS
void StackTraceResolver_DwarfReader::disable()
{
    d_helper_p    = 0;
    d_buffer_p    = 0;
    d_offset      = -1;
    d_beginOffset = -1;
    d_endOffset   = -1;
    d_readPtr     = 0;
    d_endPtr      = 0;
    d_offsetSize  = -1;
    d_addressSize = -1;
}

int StackTraceResolver_DwarfReader::init(
                                StackTraceResolver_FileHelper *fileHelper,
                                char                          *buffer,
                                const Section&                 section,
                                Offset                         libraryFileSize)
{
    static const char rn[] = { "Reader::init:" };    (void) rn;

    u_ASSERT_BAIL_SAFE(buffer);
    u_ASSERT_BAIL(section.d_offset >= 0);
    u_ASSERT_BAIL(section.d_size   > 0);
    u_ASSERT_BAIL(0 < section.d_offset + section.d_size);
    u_ASSERT_BAIL(    section.d_offset + section.d_size <= libraryFileSize);

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

int StackTraceResolver_DwarfReader::readAddress(UintPtr *dst)
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
        u_ASSERT_BAIL_SAFE(sizeof(UintPtr) == d_addressSize);

        rc = readValue(dst);
    }

    u_ASSERT_BAIL(0 == rc && "readValue failed");
    return 0;
}

int StackTraceResolver_DwarfReader::readAddress(UintPtr *dst, unsigned form)
{
    static const char rn[] = { "Reader::readAddress:" };    (void) rn;

    int rc;

    unsigned addressSize;
    switch (form) {
      case DW_FORM_addr: {
        u_ASSERT_BAIL_SAFE(d_addressSize > 0);
        addressSize = d_addressSize;
      } break;
      case DW_FORM_data1: { addressSize = 1; } break;
      case DW_FORM_data2: { addressSize = 2; } break;
      case DW_FORM_data4: { addressSize = 4; } break;
      case DW_FORM_data8: {
        u_ASSERT_BAIL(8 == sizeof(*dst));
        addressSize = 8;
      } break;
      default: {
        u_ASSERT_BAIL((0 && "unrecognized form") || u_PH(form));
      } break;
    }

    rc = needBytes(addressSize);
    u_ASSERT_BAIL(0 == rc && "needBytes failed");

    *dst = 0;
    char *dstPtr = reinterpret_cast<char *>(dst);
#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
    dstPtr += sizeof(*dst) - addressSize;
#endif
    bsl::memcpy(dstPtr, d_readPtr, addressSize);
    d_readPtr += addressSize;

    return 0;
}

int StackTraceResolver_DwarfReader::readAddressSize()
{
    static const char rn[] = { "Reader::readAddressSize:" };    (void) rn;

    unsigned char u;
    int rc = readValue(&u);
    u_ASSERT_BAIL(0 == rc && "read address size failed");

    if (sizeof(unsigned) == sizeof(UintPtr)) {
        u_ASSERT_BAIL(sizeof(unsigned) == u || u_P(u));
    }
    else {
        u_ASSERT_BAIL(sizeof(UintPtr) == u || sizeof(unsigned) == u || u_P(u));
    }
    d_addressSize = u;

    return 0;
}

int StackTraceResolver_DwarfReader::readInitialLength(Offset *dst)
{
    static const char rn[] = { "Reader::readIniitialLength:" };    (void) rn;

    int rc = readOffset(dst, sizeof(unsigned));
    u_ASSERT_BAIL(0 == rc);

    if (0xffffffff == *dst) {
        rc = readValue(dst);
        u_ASSERT_BAIL(0 == rc);
        u_ASSERT_BAIL(0 <= *dst && "negative initial length");
        d_offsetSize = static_cast<int>(sizeof(*dst));
    }
    else {
        u_ASSERT_BAIL((*dst < 0xfffffff0 && "illegal preNum") || u_PH(*dst));
        d_offsetSize = static_cast<int>(sizeof(unsigned));
    }

    u_ASSERT_BAIL(offset() + *dst <= d_endOffset);

    return 0;
}

int StackTraceResolver_DwarfReader::readOffset(Offset      *dst,
                                               bsl::size_t  offsetSize)
{
    static const char rn[] = { "Reader::readOffset:" };    (void) rn;

    enum { k_SIZE_OF_OFFSET_TYPE = sizeof(Offset) };

    int rc;

    u_ASSERT_BAIL(offsetSize > 0);
    u_ASSERT_BAIL(offsetSize <= sizeof(*dst));
    u_ASSERT_BAIL(0 == (offsetSize & (offsetSize - 1)));          // power of 2

    rc = needBytes(offsetSize);
    u_ASSERT_BAIL(0 == rc && "needBytes failed");

    *dst = 0;
    char *dstPtr = reinterpret_cast<char *>(dst);
#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
    dstPtr += k_SIZE_OF_OFFSET_TYPE - offsetSize;
#endif
    bsl::memcpy(dstPtr, d_readPtr, offsetSize);
    d_readPtr += offsetSize;

    return 0;
}

int StackTraceResolver_DwarfReader::readOffsetFromForm(Offset   *dst,
                                                       unsigned  form)
{
    static const char rn[] = { "Reader::readOffsetFromForm:" };    (void) rn;

    int rc;

    switch (form) {
      case e_DW_FORM_flag_present: {
        ; // do nothing
      } break;
      case DW_FORM_flag:
      case DW_FORM_data1:
      case DW_FORM_ref1: {
        rc = readOffset(dst, 1);
        u_ASSERT_BAIL((0 == rc && "trouble reading 1 byte value") ||
                                                                   u_PH(form));
      } break;
      case DW_FORM_data2:
      case DW_FORM_ref2: {
        rc = readOffset(dst, 2);
        u_ASSERT_BAIL((0 == rc && "trouble reading 2 byte value") ||
                                                                   u_PH(form));
      } break;
      case DW_FORM_data4:
      case DW_FORM_ref4: {
        rc = readOffset(dst, 4);
        u_ASSERT_BAIL((0 == rc && "trouble reading 4 byte value") ||
                                                                   u_PH(form));
      } break;
      case DW_FORM_data8:
      case DW_FORM_ref8: {
        rc = readOffset(dst, 8);
        u_ASSERT_BAIL((0 == rc && "trouble reading 8 byte value") ||
                                                                   u_PH(form));
      } break;
      case e_DW_FORM_sec_offset: {
        rc = readSectionOffset(dst);
        u_ASSERT_BAIL(0 == rc && "failure to read section offset");
      } break;
      case DW_FORM_sdata: {
        rc = readLEB128(dst);
        u_ASSERT_BAIL(0 == rc && "failure to read sdata");
      } break;
      case DW_FORM_udata: {
        rc = readULEB128(dst);
        u_ASSERT_BAIL(0 == rc && "failure to read udata");
      } break;
      case DW_FORM_indirect: {
        unsigned iForm;
        rc = readULEB128(&iForm);
        u_ASSERT_BAIL(0 == rc && "trouble reading indirect form");
        u_ASSERT_BAIL(DW_FORM_indirect != iForm);
        rc = readOffsetFromForm(dst, iForm);
        u_ASSERT_BAIL((0 == rc && "trouble recursing on indirect offset") ||
                                                                  u_PH(iForm));
      } break;
      default: {
        u_ASSERT_BAIL((0 && "strange form ") || u_PH(form));
      }
    }

    return 0;
}

int StackTraceResolver_DwarfReader::readSectionOffset(Offset *dst)
{
    static const char rn[] = { "Reader::readSectionOffset:" };    (void) rn;

    enum { k_SIZE_OF_OFFSET_TYPE   = sizeof(Offset),
           k_SIZE_OF_UNSIGNED_TYPE = sizeof(unsigned),
#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
           k_DELTA = k_SIZE_OF_OFFSET_TYPE - k_SIZE_OF_UNSIGNED_TYPE
#else
           k_DELTA = 0
#endif
         };

    int rc;

    if (k_SIZE_OF_OFFSET_TYPE == d_offsetSize) {
        rc = readValue(dst);
        u_ASSERT_BAIL(0 == rc && "readValue of section offset failed");

        return 0;                                                     // RETURN
    }

    u_ASSERT_BAIL_SAFE(0 < d_offsetSize ||
                           "readSectionOffset with uninitialized offset size");
    u_ASSERT_BAIL(k_SIZE_OF_UNSIGNED_TYPE == d_offsetSize);

    rc = needBytes(k_SIZE_OF_UNSIGNED_TYPE);
    u_ASSERT_BAIL(0 == rc && "needBytes failed");

    *dst = 0;
    bsl::memcpy(reinterpret_cast<char *>(dst) + k_DELTA,
                d_readPtr,
                k_SIZE_OF_UNSIGNED_TYPE);
    d_readPtr += k_SIZE_OF_UNSIGNED_TYPE;

    return 0;
}

int StackTraceResolver_DwarfReader::readString(bsl::string *dst)
{
    static const char rn[] = { "Reader::readString:" };    (void) rn;
    int rc;

    const char *pc = d_readPtr;
    while (pc < d_endPtr && *pc) {
        ++pc;
    }

    if (pc >= d_endPtr) {
        u_ASSERT_BAIL(pc == d_endPtr);

        rc = needBytes(1 + (d_endPtr - d_readPtr));
        u_ASSERT_BAIL(0 == rc);

        pc = d_readPtr;
        while (pc < d_endPtr && *pc) {
            ++pc;
        }
    }

    if (pc >= d_endPtr) {
        u_ASSERT_BAIL(pc == d_endPtr);

        u_ASSERT_BAIL(pc - d_readPtr < k_SCRATCH_BUF_LEN &&
                                           "string longer than buffer length");
        u_ASSERT_BAIL(d_offset + (pc - d_readPtr) < d_endOffset &&
                                          "string longer than section length");
        u_ASSERT_BAIL(0 && "unknown error");
    }

    u_ASSERT_BAIL(!*pc && "unknown error");

    bsl::size_t len = pc - d_readPtr;
    if (dst) {
        dst->assign(d_readPtr, len);
    }
    d_readPtr += len + 1;

    return 0;
}

int StackTraceResolver_DwarfReader::readStringAt(bsl::string *dst,
                                                 Offset       offset)
{
    static const char rn[] = { "Reader::readStringAt:" };    (void) rn;

    u_ASSERT_BAIL(0 <= offset);
    offset += d_beginOffset;
    u_ASSERT_BAIL(0 <= offset);
    u_ASSERT_BAIL(offset < d_endOffset || u_PH(offset) || u_PH(d_endOffset));

    enum { k_START_LEN = 256 };

    bsl::size_t maxReadLen = static_cast<bsl::size_t>(
                                      bsl::min<Offset>(d_endOffset - offset,
                                                       k_SCRATCH_BUF_LEN - 1));

    bsl::size_t stringLen;
    for (bsl::size_t readLen = k_START_LEN; true; readLen *= 4) {
        if (readLen > maxReadLen) {
            readLen = maxReadLen;
        }

        bsl::size_t bytes = d_helper_p->readBytes(d_buffer_p, readLen, offset);
        u_ASSERT_BAIL(0 < bytes);

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

int StackTraceResolver_DwarfReader::readStringFromForm(
                                  bsl::string                    *dst,
                                  StackTraceResolver_DwarfReader *stringReader,
                                  unsigned                        form)
{
    static const char rn[] = { "Reader::readString:" };    (void) rn;

    int rc;

    if      (DW_FORM_string == form) {
        rc = readString(dst);
        u_ASSERT_BAIL(0 == rc);
    }
    else if (DW_FORM_strp   == form) {
        Offset offset;
        rc = readSectionOffset(&offset);
        u_ASSERT_BAIL(0 == rc);
        rc = stringReader->readStringAt(dst, offset);
        u_ASSERT_BAIL(0 == rc);
    }
    else {
        u_ASSERT_BAIL((0 && "unrecognized baseName form") || u_PH(form));
    }

    return 0;
}

int StackTraceResolver_DwarfReader::setAddressSize(unsigned size)
{
    static const char rn[] = { "Reader::setAddressSize:" };    (void) rn;

    u_ASSERT_BAIL(sizeof(unsigned) == size || sizeof(UintPtr) == size);

    d_addressSize = size;

    return 0;
}

int StackTraceResolver_DwarfReader::setEndOffset(Offset newOffset)
{
    static const char rn[] = { "Reader::setEndOffset:" };    (void) rn;

    u_ASSERT_BAIL(offset() <= newOffset);

    Offset readLength   = d_endPtr - d_readPtr;
    Offset offsetLength = newOffset - offset();

    d_endOffset = newOffset;
    if (offsetLength < readLength) {
        d_endPtr = d_readPtr + offsetLength;
    }

    return 0;
}

int StackTraceResolver_DwarfReader::skipForm(unsigned form)
{
    // The values of the 'DW_FORM_*' identifiers are described in Figure 21 of
    // the DWARF version 4 document.  The meanings of the different values
    // 'DW_FORM_*' are described in section 7.5.4.

    static const char rn[] = { "Reader::skipForm:" };    (void) rn;

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
        rc = skipULEB128();
        u_ASSERT_BAIL(0 == rc || u_PH(form));
      } break;
      case DW_FORM_string: {
        do {
            rc = needBytes(1);
            u_ASSERT_BAIL(0 == rc);
        } while (*d_readPtr++);
      } break;
      case DW_FORM_strp:
      case e_DW_FORM_sec_offset: {
        u_ASSERT_BAIL_SAFE(d_offsetSize > 0);
        toSkip = d_offsetSize;
      } break;
      case DW_FORM_addr:
      case DW_FORM_ref_addr: {
        u_ASSERT_BAIL_SAFE(d_addressSize > 0);
        toSkip = d_addressSize;
      } break;
      case DW_FORM_indirect: {
        unsigned iForm;
        rc = readULEB128(&iForm);
        u_ASSERT_BAIL((0 == rc && "trouble skipping indirect form") ||
                                                                  u_PH(iForm));
        u_ASSERT_BAIL(DW_FORM_indirect != iForm);
        rc = skipForm(iForm);
        u_ASSERT_BAIL((0 == rc &&
              "trouble recursing skipping on indirect offset") || u_PH(iForm));
      } break;
      case DW_FORM_block1: {
        unsigned char len;
        rc = readValue(&len);
        u_ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block2: {
        unsigned short len;
        rc = readValue(&len);
        u_ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block4: {
        unsigned len;
        rc = readValue(&len);
        u_ASSERT_BAIL(0 == rc);
        toSkip = len;
      } break;
      case DW_FORM_block:
      case e_DW_FORM_exprloc: {
        rc = readULEB128(&toSkip);
        u_ASSERT_BAIL(0 == rc);
      } break;
      default: {
        u_ASSERT_BAIL((0 && "unrecognized form") || u_PH(form));
      }
    }

    if (toSkip > 0) {
        // u_zprintf("%s skipping forward %lld\n", rn, (long long) toSkip);

        rc = skipBytes(toSkip);
        u_ASSERT_BAIL(0 == rc);
    }
    else {
        // u_zprintf("%s not skipping\n", rn);

        u_ASSERT_BAIL(-1 == toSkip);
    }

    return 0;
}

int StackTraceResolver_DwarfReader::skipTo(Offset dstOffset)
{
    static const char rn[] = { "Reader::skipTo:" };    (void) rn;

    Offset diff = dstOffset - this->offset();
    if (diff > d_endPtr - d_readPtr || diff < d_buffer_p - d_readPtr) {
        u_ASSERT_BAIL(dstOffset >= d_beginOffset || u_PH(dstOffset) ||
                                                          u_PH(d_beginOffset));
        u_ASSERT_BAIL(dstOffset <= d_endOffset || u_PH(dstOffset) ||
                                                            u_PH(d_endOffset));

        // By setting 'd_readPtr == d_endPtr' we will automatically make the
        // buffer be refilled next time we read any data.

        d_offset  = dstOffset;
        d_readPtr = d_buffer_p;
        d_endPtr  = d_readPtr;
    }
    else {
        u_ASSERT_BAIL_SAFE(static_cast<IntPtr>(diff) == diff);

        d_readPtr += static_cast<IntPtr>(diff);
    }

    return 0;
}

int StackTraceResolver_DwarfReader::skipULEB128()
{
    static const char rn[] = { "Reader::skipULEB128:" };    (void) rn;

    enum { k_MAX_SHIFT = sizeof(Offset) * 8 - 1 };
    unsigned shift = 0;
    do {
        u_ASSERT_BAIL(shift <= k_MAX_SHIFT);
        shift += 7;

        int rc = needBytes(1);
        u_ASSERT_BAIL(0 == rc);
    } while (*d_readPtr++ & 0x80);

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

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
