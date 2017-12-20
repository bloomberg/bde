// balst_stacktraceresolver_dwarfreader.h                             -*-C++-*-

#ifndef INCLUDED_BALST_STACKTRACERESOLVER_DWARFREADER
#define INCLUDED_BALST_STACKTRACERESOLVER_DWARFREADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mechanism for reading DWARF information from object files.
//
//@CLASSES:
//   balst::StackTraceResolver_DwarfReader: reading mechanism
//
//@SEE_ALSO: balst_stacktraceresolverimpl_elf
//           balst_stacktraceresolver_filehelper
//
//@DESCRIPTION: This component provides a class,
// 'balst::StackTraceResolver_DwarfReader', that is optimized for reading
// information from object files that are in the DWARF format.  The Elf object
// file format is used on Linux, Solaris, and HP-UX platforms, and the DWARF
// file format is used within ELF files to encode source file name and line
// number information.  The Elf format is described by documents at:
//: o 'http://en.wikipedia.org/wiki/Executable_and_Linkable_Format'
//: o 'http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf'
//: o 'http://www.sco.com/developers/gabi/latest/contents.html'
// The DWARF format is described by documents at:
//: o 'http://dwarfstd.org'
// and by the include file '/usr/include/dwarf.h'.
//
// This code is built to process version 4 of the DWARF standard, but it only
// assumes that 'dwarf.h' supports version 3.
//
// Note that this file does not include everything necessary to resolve DWARF
// information.  Most of that functionality is in
// 'balst_stacktraceresolverimpl_elf', and this component only describe a tool
// used within that effort.
//
///Usage
///-----
// This component is an implementation detail of 'balst' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALST_OBJECTFILEFORMAT
#include <balst_objectfileformat.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACERESOLVER_FILEHELPER
#include <balst_stacktraceresolver_filehelper.h>
#endif

#ifndef INCLUDED_BDLS_FILESYSTEMUTIL
#include <bdls_filesystemutil.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace balst {

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DWARF)

                    // ====================================
                    // class StackTraceResolver_DwarfReader
                    // ====================================

class StackTraceResolver_DwarfReader {
  public:
    // PUBLIC TYPES
    typedef bdls::FilesystemUtil::Offset Offset;
    typedef bsls::Types::UintPtr         UintPtr;
    typedef bsls::Types::IntPtr          IntPtr;

    struct Section {
        // Refers to one section of a segment.

        // DATA
        Offset d_offset;    // offset of the section in the file
        Offset d_size;      // size of that section in bytes

        // CREATOR
        Section();
            // Create a zero-value 'Section' object.

        // MANIPULATOR
        void reset(Offset offset = 0, Offset size = 0);
            // Reset this 'Section' object to have the specified 'offset' and
            // the specified 'size'.
    };

    // PUBLIC CONSTANTS
    enum { k_SCRATCH_BUF_LEN = 32 * 1024 - 64 };
        // length in bytes of d_buffer_p; 32K minus a little so we don't
        // waste a page

    enum Dwarf4Enums {
        // DWARF 4 flags not necessarily defined in our /usr/include/dwarf.h.

        e_DW_AT_signature              = 0x69,
        e_DW_AT_main_subprogram        = 0x6a,
        e_DW_AT_data_bit_offset        = 0x6b,
        e_DW_AT_const_expr             = 0x6c,
        e_DW_AT_enum_class             = 0x6d,
        e_DW_AT_linkage_name           = 0x6e,

        e_DW_FORM_sec_offset           = 0x17,
        e_DW_FORM_exprloc              = 0x18,
        e_DW_FORM_flag_present         = 0x19,
        e_DW_FORM_ref_sig8             = 0x20,

        e_DW_LNE_set_discriminator     = 0x04,

        e_DW_TAG_mutable_type          = 0x3e,
        e_DW_TAG_type_unit             = 0x41,
        e_DW_TAG_rvalue_reference_type = 0x42,
        e_DW_TAG_template_alias        = 0x43
    };

  private:
    // DATA
    balst::StackTraceResolver_FileHelper
                                   *d_helper_p;      // filehelper for currentt
                                                     // segment

    char                           *d_buffer_p;      // buffer.
                                                     // k_SCRATCH_BUF_LEN long

    Offset                          d_offset;        // offset last read from

    Offset                          d_beginOffset;   // beg of current section

    Offset                          d_endOffset;     // end of current section

    const char                     *d_readPtr;       // current place to read
                                                     // from (in the buffer)

    const char                     *d_endPtr;        // end of what's in buffer

    int                             d_offsetSize;    // offset size determined
                                                     // by 'readInitalLength'

    int                             d_addressSize;   // address read by
                                                     // 'getAddress' or set by
                                                     // 'setAddressSize'.

  private:
    // NOT IMPLEMENTED
    StackTraceResolver_DwarfReader(const StackTraceResolver_DwarfReader&);
    StackTraceResolver_DwarfReader& operator=(
                                   const StackTraceResolver_DwarfReader&);

  private:
    // PRIVATE MANIPULATORS
    int needBytes(bsl::size_t numBytes);
        // Determine if we are able to read the specified 'numBytes' from
        // 'd_buffer'.  If not, 'reload' is used to attempt to accomodate this
        // 'needBytes' request.  Return 0 if we are able to read 'numBytes'
        // from 'd_buffer' after this invocation of 'needBytes', and a non-zero
        // value otherwise.

    int reload(bsl::size_t numBytes);
        // Reload the buffer to accomodate a read of at least the specified
        // 'numBytes'.  If possible, read up to the end of the section or the
        // size of the buffer, whichever is shorter.  Return 0 on success, and
        // a non-zero value otherwise.

  public:
    // CLASS METHODS
    static
    const char *stringForAt(unsigned id);
        // Return the string equivalent of the specified 'DW_AT_*' 'id'.

    static
    const char *stringForForm(unsigned id);
        // Return the string equivalent of the specified 'DW_FORM_*' 'id'.

    static
    const char *stringForInlineState(unsigned inlineState);
        // Return the string equivalent of the specified 'DW_INL_*'
        // 'inlineState'.

    static
    const char *stringForLNE(unsigned id);
        // Return the string equivalent of the specified 'DW_LNE_*' 'id'.

    static
    const char *stringForLNS(unsigned id);
        // Return the string equivalent of the specified 'DW_LNS_*' 'id'.

    static
    const char *stringForTag(unsigned tag);
        // Return the string equivalent of the specified 'DW_TAG_*' 'tag'.

    // CREATORS
    StackTraceResolver_DwarfReader();
        // Create a 'Reader' object in a null state.

    // ~StackTraceResolver_DwarfReader() = default;

    // MANIPULATORS
    void disable();
        // Disable this object for further use.

    int init(balst::StackTraceResolver_FileHelper *fileHelper,
             char                                 *buffer,
             const Section&                        section,
             Offset                                libraryFileSize);
        // Initialize this 'Reader' object using the specified 'fileHelper' and
        // the specified 'buffer', to operate on the specified 'section', where
        // the specified 'libraryFileSize' is the size of the library or
        // executable file.  'buffer' is assumed to be at least
        // 'k_SCRATCH_BUF_LEN' long.

    int readAddress(UintPtr *dst);
        // Read to the specified 'dst'.  This function will fail if
        // 'd_addressSize' has not been initialized by 'readAddressSize' or
        // 'setAddressSize'.  Return 0 on success and a non-zero value
        // otherwise.

    int readAddress(UintPtr *dst, unsigned form);
        // Read to the specified 'dst' according to the specified 'form'.
        // Return 0 on success and a non-zero value otherwise.

    int readAddressSize();
        // Read the address size from a single unsigned byte, check it, and
        // assign 'd_addressSize' to it.  Return 0 on success and a non-zero
        // value otherwise.  It is an error if address size is not equal to the
        // size of an 'unsigned int' or of a 'void *'.

    int readInitialLength(Offset *dst);
        // Read the initial length of the object according to the DWARF
        // specification to the specified '*dst', which is an 8-byte value.
        // Read '*dst' first as a 4 byte value, setting the high-order 4 bytes
        // to 0, and if the value is below 0xfffffff0, then that indicates that
        // section offsets are to be read as 4 byte values within the object
        // whose length is specified.  If the value is 0xffffffff, read the
        // next 8 bytes into '*dst' and that indicates that section offsets are
        // to be 8 bytes within the object whose length is specified.
        // Initialize 'd_offsetSize' accordingly.  Values in the range
        // '[0xfffffff0, 0xffffffff)' are illegal for that first 4 bytes.
        // Return 0 on success and a non-zero value otherwise.  Note that when
        // we read a 4-byte value, we do not extend sign to the high order 4
        // bytes of '*dst'.

    template <class TYPE>
    int readLEB128(TYPE *dst);
        // Read a signed, variable-length number into the specified '*dst'.
        // Return 0 on success and a non-zero value otherwise.

    template <class TYPE>
    int readULEB128(TYPE *dst);
        // Read an unsigned, variable-length number into the specified '*dst'.
        // Return 0 on success and a non-zero value otherwise.

    int readOffset(Offset      *dst,
                   bsl::size_t  offsetSize);
        // Read to the specified '*dst', where the specified 'offsetSize' is
        // the number of low-order bytes to be read into the offset, where
        // '*dst' is 8 bytes, and extra high-order bytes are to be set to 0.
        // Do not extend sign.  Return 0 on success and a non-zero value
        // otherwise.

    int readOffsetFromForm(Offset   *dst,
                           unsigned  form);
        // Read to the specified '*dst' according to the specified 'form',
        // where 'form' is a DWARF enum of the 'DW_FORM_*' category.  Return 0
        // on success and a non-zero value otherwise.

    int readSectionOffset(Offset *dst);
        // Read to the specified offset '*dst' according to 'd_offsetSize'.
        // Return 0 on success and a non-zero value otherwise.  Note that when
        // the offset read is only 4 bytes, no sign extension takes place as
        // we always expect a positive result.

    int readString(bsl::string *dst = 0);
        // Read a null-terminated string to the specified '*dst'.  If no 'dst'
        // is specified, skip over the string without copying it.  This
        // function will fail if the string length is greater than
        // 'k_SCRATCH_BUFFER_LEN - 1'.

    int readStringAt(bsl::string *dst, Offset offset);
        // Read a null terminated string to the specified '*dst' from the
        // specified 'offset' plus 'd_beginOffset'.  Note that, unlike most of
        // the other 'read' functions, this one is intended for random access
        // so does not read a full buffer ahead, instead reading a fairly
        // minimal amount of data near the specified location.

    int readStringFromForm(bsl::string                    *dst,
                           StackTraceResolver_DwarfReader *stringReader,
                           unsigned                        form);
        // Read to the specified string either from the current reader (if the
        // specified 'form' is 'DW_AT_string') or read an offset from the
        // current reader, then use that to read the string from the specified
        // '*stringReader' (if 'form' is 'DW_AT_strp').  Return 0 on success
        // and a non-zero value otherwise.

    template <class TYPE>
    int readValue(TYPE *dst);
        // Read a value into the specified '*dst', assuming that it is
        // represented by 'sizeof(*dst)' bytes.

    int setAddressSize(unsigned size);
        // Explicitly set the 'd_addressSize' of this reader to the specified
        // 'size'.  This function will fail unless the size is the
        // 'sizeof(unsigned) == size' or 'sizeof(UintPtr) == size'.

    int setEndOffset(Offset newOffset);
        // Set the end offset to the specified 'newOffset'.

    int skipBytes(Offset bytes);
        // Skip forward over the specified 'bytes' without reading them.

    int skipString();
        // Skip over a null terminated string without copying it.

    int skipForm(unsigned form);
        // Skip over data according to the specified 'form', which is an enum
        // of type 'DW_FORM_*' or 'e_DW_FORM_*'.

    int skipTo(Offset offset);
        // Skip to the specified 'offset', which must be in the section
        // associated with this reader.  Return 0 on success and a non-zero
        // value otherwise.

    int skipULEB128();
        // Skip a variable-length integer.  Note this will work for both
        // LEB128's and ULEB128's.

    // ACCESSORS
    int addressSize() const;
        // Return the address size field.

    bool atEndOfSection() const;
        // Return 'true' if the reader has reached the end of the section and
        // 'false' otherwise.

    Offset offset() const;
        // Return the current offset taking the 'd_reader' position into
        // account.

    Offset offsetSize() const;
        // Return the offset length that was set by the 'readInitialLength'
        // function.
};

// PRIVATE MANIPULATORS
inline
int StackTraceResolver_DwarfReader::needBytes(bsl::size_t numBytes)
{
    IntPtr diff = d_endPtr - d_readPtr;

    if (diff < static_cast<IntPtr>(numBytes)) {
        BSLS_ASSERT_SAFE(0 <= diff);

        return reload(numBytes);                                      // RETURN
    }

    return 0;
}

template <class TYPE>
int StackTraceResolver_DwarfReader::readLEB128(TYPE *dst)      // DWARF doc 7.6
{
    BSLMF_ASSERT(static_cast<TYPE>(-1) < 0);    // 'TYPE' must be signed

    int rc;

    *dst = 0;

    unsigned char u = 0x80;

    enum { k_MAX_SHIFT = sizeof(*dst) * 8 };

    unsigned shift = -7;
    do {
        shift += 7;
        if (shift >= k_MAX_SHIFT) {
            return -1;                                                // RETURN
        }

        rc = readValue(&u);
        if (rc) {
            return -1;                                                // RETURN
        }

        *dst |= static_cast<TYPE>(static_cast<TYPE>(u & 0x7f) << shift);
    } while (0x80 & u);

    if (static_cast<TYPE>(-1) < 0) {
        // signed type, extend sign

        const TYPE negFlag = static_cast<TYPE>(
                                             static_cast<TYPE>(0x40) << shift);
        if (negFlag & *dst) {
            *dst |= static_cast<TYPE>(~(negFlag - 1));
        }
    }

    return 0;
}

template <class TYPE>
int StackTraceResolver_DwarfReader::readULEB128(TYPE *dst)     // DWARF doc 7.6
{
    int rc;

    *dst = 0;

    unsigned char u = 0x80;

    enum { k_MAX_SHIFT = sizeof(*dst) * 8 };

    unsigned shift = 0;
    for (; (0x80 & u); shift += 7) {
        if (shift >= k_MAX_SHIFT) {
            return -1;                                                // RETURN
        }

        rc = readValue(&u);
        if (0 != rc) {
            return -1;                                                // RETURN
        }

        *dst |= static_cast<TYPE>(static_cast<TYPE>(u & 0x7f) << shift);
    }

    return 0;
}

template <class TYPE>
inline
int StackTraceResolver_DwarfReader::readValue(TYPE *dst)
{
    int rc = needBytes(sizeof(*dst));
    if (rc) {
        return -1;                                                    // RETURN
    }

    bsl::memcpy(dst, d_readPtr, sizeof(*dst));
    d_readPtr += sizeof(*dst);

    return 0;
}

inline
int StackTraceResolver_DwarfReader::skipBytes(Offset bytes)
{
    BSLS_ASSERT_SAFE(bytes >= 0);

    if (bytes > d_endPtr - d_readPtr) {
        Offset off = offset();

        if (off         < d_beginOffset) {
            return -1;                                                // RETURN
        }
        if (off + bytes > d_endOffset) {
            return -1;                                                // RETURN
        }

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

inline
int StackTraceResolver_DwarfReader::skipString()
{
    do {
        int rc = needBytes(1);
        if (rc) {
            return -1;                                                // RETURN
        }
    } while (*d_readPtr++);

    return 0;
}

// ACCESSORS
inline
int StackTraceResolver_DwarfReader::addressSize() const
{
    return d_addressSize;
}

inline
bool StackTraceResolver_DwarfReader::atEndOfSection() const
{
    return d_readPtr == d_endPtr &&
                              d_endOffset - d_offset == d_readPtr - d_buffer_p;
}

inline
StackTraceResolver_DwarfReader::Offset
StackTraceResolver_DwarfReader::offset() const
{
    return d_offset + (d_readPtr - d_buffer_p);
}

inline
StackTraceResolver_DwarfReader::Offset
StackTraceResolver_DwarfReader::offsetSize() const
{
    return d_offsetSize;
}

#endif

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
