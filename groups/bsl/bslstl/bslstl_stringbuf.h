// bslstl_stringbuf.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGBUF
#define INCLUDED_BSLSTL_STRINGBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the C++03 compatible stringbuf class.
//
//@CLASSES:
//  bsl::streambuf: a C++03-compatible stringbuf class
//
//@SEE_ALSO: bslstl_stringstream, bslstl_ostringstream, bslstl_istringstream
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component defines a class template 'bsl::basic_stringbuf' which
// implements a standard string buffer, providing an unformatted character
// input sequence and an unformatted character output sequence that
// may be initialized or accessed using a string value (see 27.8.2 [stringbuf]
// of the C++11 standard).  This component also defines two standard aliases,
// 'bsl::stringbuf' and 'bsl::wstringbuf', that refer to specializations of the
// 'bsl::basic_stringbuf' template for 'char' and 'wchar_t' types
// respectively.  As with any stream buffer class, 'bsl::basic_stringbuf' is
// rarely used directly.  Stream buffers provide low-level unformatted
// input/output.  They are usually plugged into 'std::basic_stream' classes to
// provide higher-level formatted input and output via 'operator<<' and
// 'operator>>'.  'bsl::basic_stringbuf' is used in 'bsl::basic_stringstream'
// family of classes and users should prefer 'bsl::basic_stringstream' classes
// instead of directly using 'bsl::basic_stringbuf'.
//
// 'bsl::basic_stringbuf' derives from 'std::basic_streambuf' and implements
// the necessary protected virtual methods.  This way 'bsl::basic_stringbuf'
// customizes the behavior of 'std::basic_streambuf' to redirect the reading
// and writing of characters to an internally maintained sequence of characters
// that can be initialized or accessed using a 'bsl::basic_string'.  Note that
// althrough the standard mandates functions that access and modify the
// buffered sequence using a 'basic_string' it does not mandate that a
// 'basic_stringbuf' internally store this buffer in a 'basic_string'; this
// implementation currently uses a 'basic_string' as its internal bufer, but
// that is subject to change without warning.
//
// The 'bsl::stringbuf' template has three parameters, 'CHAR_TYPE',
// 'CHAR_TRAITS', and 'ALLOCATOR'.  The 'CHAR_TYPE' and 'CHAR_TRAITS'
// parameters respectively define the character type for the stream buffer and
// a type providing a set of operations the stream buffer will use to
// manipulate characters of that type, which must meet the character traits
// requirements defined by the C++11 standard, 21.2 [char.traits].  The
// 'ALLOCATOR' template parameter is described in the "Memory Allocation"
// section below.
//
///Memory Allocation
///-----------------
// The type supplied as a stream buffer's 'ALLOCATOR' template parameter
// determines how that stream buffer will allocate memory.  The
// 'basic_stringbuf' template supports allocators meeting the requirements
// of the C++11 standard [17.6.3.5], in addition it supports scoped-allocators
// derived from the 'bslma::Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use 'bsl::allocator', which
// provides a C++11 standard-compatible adapter for a 'bslma::Allocator'
// object.  Note that the standard aliases 'bsl::stringbuf' and
// 'bsl::wstringbuf' both use 'bsl::allocator'.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'stringbuf' instantiation is
// 'bsl::allocator', then objects of that stream buffer type will conform to
// the standard behavior of a 'bslma'-allocator-enabled type.  Such a
// stream buffer accepts an optional 'bslma::Allocator' argument at
// construction.  If the address of a 'bslma::Allocator' object is explicitly
// supplied at construction, it will be used to supply memory for the
// stream buffer throughout its lifetime; otherwise, the stream buffer will use
// the default allocator installed at the time of the stream buffer's
// construction (see 'bslma_default').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic operations
///- - - - - - - - - - - - - -
// The following example demonstrates the use of 'bsl::stringbuf' to read and
// write character data from and to a 'bsl::string' object.
//
// Suppose we want to implement a simplified converter from 'unsigned int' to
// 'bsl::string' and back.  First, we define the prototypes of conversion
// functions:
//..
//  bsl::string toString(unsigned int from);
//  unsigned int fromString(const bsl::string& from);
//..
// Then, we use 'bsl::stringbuf' to implement the 'toString' function.  We
// write all digits into 'bsl::stringbuf' individually using 'sputc' methods
// and then return the resulting 'bsl::string' object as follows:
//..
//  #include <algorithm>
//
//  bsl::string toString(unsigned int from)
//  {
//      bsl::stringbuf out;
//
//      for (; from != 0; from /= 10) {
//          out.sputc('0' + from % 10);
//      }
//
//      bsl::string result(out.str());
//      std::reverse(result.begin(), result.end());
//      return result;
//  }
//..
// Now, we implement the 'fromString' function that converts from
// 'bsl::string' to 'unsigned int' by using 'bsl::stringbuf' to read digit
// from the string object:
//..
//  unsigned int fromString(const bsl::string& from)
//  {
//      unsigned int result = 0;
//      for (bsl::stringbuf in(from); in.in_avail(); ) {
//          result = result * 10 + (in.sbumpc() - '0');
//      }
//
//      return result;
//  }
//..
// Finally, we verify that the result of the round-trip conversion is identical
// to the original value:
//..
//  unsigned int orig = 92872498;
//  unsigned int result = fromString(toString(orig));
//  assert(orig == result);
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_sstream.h> instead of <bslstl_stringbuf.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_IOSFWD
#include <bslstl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_ISTREAM
#include <istream>
#define INCLUDED_ISTREAM
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#define INCLUDED_OSTREAM
#endif

#ifndef INCLUDED_STREAMBUF
#include <streambuf>
#define INCLUDED_STREAMBUF
#endif

namespace bsl {

using native_std::ios_base;

                            // =====================
                            // class basic_stringbuf
                            // =====================

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class basic_stringbuf
    : public native_std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS> {
    // This class implements a standard stream buffer providing an unformatted
    // character input sequence and an unformatted character output sequence
    // that may be initialized or accessed using a string value.

  private:
    // PRIVATE TYPES
    typedef native_std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>  BaseType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;

  public:
    // TYPES
    typedef CHAR_TYPE                       char_type;
    typedef CHAR_TRAITS                     traits_type;
    typedef ALLOCATOR                       allocator_type;
    typedef typename traits_type::int_type  int_type;
    typedef typename traits_type::off_type  off_type;
    typedef typename traits_type::pos_type  pos_type;

  private:
    // DATA
    StringType         d_str;   // internal character sequence buffer

    off_type           d_lastWrittenChar;
                                // the last available written character in
                                // 'd_str'.  Note that to enable efficient
                                // buffering, 'd_str' may be resized beyond
                                // the last written character, so
                                // 'd_str.size()' may not accurately report
                                // the current length of the character
                                // sequence available for input.  Extending
                                // the size of 'd_str' and updating 'epptr'
                                // (the end-of-output pointer) allows the
                                // parent stream type to write additional
                                // characters without 'overflow'.  However,
                                // care must be taken to refresh the cached
                                // 'd_lastWrittenChar' value as the parent
                                // stream will update the current output
                                // position 'pptr', without calling a method
                                // on this type.

    ios_base::openmode d_mode;  // stringbuf open mode ('in' or 'out')

  private:
    // NOT IMPLEMENTED
    basic_stringbuf(const basic_stringbuf&);            // = delete
    basic_stringbuf& operator=(const basic_stringbuf&); // = delete

    // PRIVATE MANIPULATORS
    pos_type updateInputPointers(char_type *currentInputPosition);
        // Update the input pointers (eback, gptr, egptr) of this string
        // buffer, assigning the beginning of the input sequence, 'eback', to
        // the address of the first character of the internal string
        // representation, 'd_str', the current position of the input sequence,
        // 'gptr', to the specified  'currentInputPosition', and the address
        // past the end of the accessible sequence, 'egptr', to the last
        // a character in 'd_ptr' ('&d_ptr[0] + d_lastWrittenChar').
        // Return the offset of the current position of the input sequence
        // from the start of the sequence.  The behavior is undefined unless
        // this buffer is in input-mode and 'currentInputPosition' is within
        // the range of accessible characters in 'd_ptr'.

    pos_type updateOutputPointers(char_type *currentOutputPosition);
        // Update the output pointers (pback, pptr, epptr) of this string
        // buffer, assigning the beginning of the output sequence, 'pback', to
        // the address of the first character of the internal string
        // representation, 'd_str', the current position of the output
        // sequence, 'pptr', to the specified 'currentOutputPosition', and the
        // address past the end of the accessible sequence, 'pptr', to the
        // last accessible character in 'd_ptr' ('&d_ptr[0] + d_ptr.size()').
        // Return the offset of the current position of the output sequence
        // from the start of the sequence.  The behavior is undefined unless
        // this buffer is in output-mode, and 'currentOutputPosition' is within
        // the range of accessible characters in 'd_ptr'.

    void updateStreamPositions(off_type inputOffset  = 0,
                               off_type outputOffset = 0);
        // Update the input and output positions of this string buffer object
        // according the the current state of the internal string
        // representation 'd_ptr'.  Optionally specify 'inputOffset' indicating
        // the current input position's offset from the beginning of the
        // sequence.  Optionally specify 'outputOffset' indicating the current
        // output position's offset from the beginning of the sequence.  If
        // this buffer is in input mode, assign the beginning of the input
        // sequence, 'eback', to the address of the first character of
        // 'd_ptr', the current input position, 'gptr', to
        // 'eback + inputoffset', and the end of the input sequence to the
        // last written character in 'd_str'
        // ('&d_ptr[0] + d_lastWrittenChar').  If this buffer is in output
        // mode, assign the beginning of the output sequence, 'pback', to the
        // address of the first character of 'd_ptr', the current output
        // position, 'pptr', to 'pback + outputOffset', and the end of the
        // output sequence to the last accessible character in 'd_str'
        // ('&d_ptr[0] + d_ptr.size()').

    bool extendInputArea();
        // Attempt to expand sequence of characters available for input
        // (i.e., update the end of input buffer position, 'egptr') to
        // incorporate additional characters that may have been written (as
        // output) to the stream.  Return 'true' if the input buffer was
        // successfully extended, and 'false' otherwise.  Note that the input
        // area as described by 'eback', 'gptr', and 'egptr' may become out of
        // sync with the characters actually available in the buffer as the
        // parent 'basic_streambuf' type may perform writes into the output
        // area (using 'pbase', 'pptr', and 'epptr') without calling any
        // methods of this object.

    // PRIVATE ACCESSORS
    pos_type streamSize() const;
        // Return the number of characters currently in the buffer.  Note this
        // may not be 'd_str.size()', as this implementation resizes 'd_str'
        // beyond the number of written characters to provide more efficient
        // buffering, and it also may not be 'd_lastWrittenChar', as that
        // value may currently be stale (as writes may have been performed
        // through the parent 'basic_streambuf' type without calling a method
        // on this object).

  protected:
    // PROTECTED MANIPULATORS
    virtual pos_type seekoff(
                off_type           offset,
                ios_base::seekdir  whence,
                ios_base::openmode modeBitMask = ios_base::in | ios_base::out);
        // Set the current input position or the current output position (or
        // both) to the specified 'offset' from the specified 'whence'
        // location.  Optionally specify 'modeBitMask' indicating whether to
        // set the current input position, output position, or both.  If
        // 'whence' is 'ios_bas::beg' set the current position to the
        // indicated 'offset' from the beginning of the stream; if 'whence' is
        // 'ios_bas::end' set the current position to the indicated 'offset'
        // from the end of the stream; and if 'whence' is 'ios_bas::cur' set
        // the current input or output position to the indicated 'offset' from
        // its current position.  If 'whence' is 'ios_bas::cur' then
        // 'modeBitMask' may be either 'ios_bas::in' or 'ios_base::out', but
        // not both.  Return the new offset from the beginning of the file on
        // success, and 'pos_type(off_type(-1))' otherwise.

    virtual pos_type seekpos(
             pos_type           absoluteOffset,
             ios_base::openmode modeBitMask = ios_base::in | ios_base::out);
        // Set the current input position or the current output position (or
        // both) to the specified 'absoluteOffset' from the beginning of the
        // stream.  Optionally specify 'modeBitMask' indicating whether
        // to set the current input position, output position, or
        // both.  Return the new offset from the beginning of the file on
        // success, and 'pos_type(off_type(-1))' otherwise.

    virtual native_std::streamsize xsgetn(
                                        char_type              *result,
                                        native_std::streamsize  numCharacters);
        // Read up to the specified 'numCharacters' from this 'stringbuf'
        // object and store them in the specified 'result' array.  Return the
        // number of characters loaded into 'result'.  Note that if fewer than
        // 'numCharacters' characters are available in the buffer, all
        // available characters are loaded into 'result'.  The behavior is
        // undefined unless 'result' refers to a contiguous sequence of
        // characters of at least 'numCharacters'.

    virtual int_type underflow();
        // Return the character at the current input position, if a character
        // is available, and 'traits_type::eof()' otherwise.   Update the end
        // of the input area, 'egptr', if additional characters are available
        // (as may occur if additional characters have been written to the
        // string buffer).  Note that this operation is similar to 'uflow',
        // but does not advance the current input position.

    virtual int_type uflow();
        // Return the character at the current input position and advance the
        // input position by 1.  If no character is available at the current
        // input position, return 'traits_type::eof()'.   Update the end of the
        // input area, 'egptr', if additional characters are available (as may
        // occur if additional characters have been written to the string
        // buffer).  Note that this operation is similar to 'underflow', but
        // advances the current input position.

    virtual int_type pbackfail(int_type character = traits_type::eof());
        // Put back the specified 'character' into the input sequence so that
        // the next character read from the input sequence will be
        // 'character'.  If 'character' is either 'traits_type::eof()' or is
        // the same as the previously read character from the input sequence,
        // then adjust the current input position, 'gptr', back one position.
        // If 'character' is neither 'traits_type::eof()' nor the previously
        // read character from the input sequence, but this string buffer was
        // opened for writing ('ios_base::out'), then adjust the input
        // sequence back one position and write 'character' to that position.
        // Return the character that was put back on success and
        // 'traits_type::eof()' if either the input position is currently at
        // the beginning of the sequence or if the previous character in the
        // input sequence is not 'character' and this buffer was not opened
        // for writing.

    virtual native_std::streamsize xsputn(
                                        const char_type        *inputString,
                                        native_std::streamsize  numCharacters);
        // Append the specified 'numCharacters' from the specified
        // 'inputString' to the output sequence starting at the current output
        // position ('pptr').  Update the current output position of this
        // string buffer to refer to the last appended character.  Return the
        // number of characters that were appended.

    virtual int_type overflow(int_type character = traits_type::eof());
        // Append the specified character 'c' to the output sequence of this
        // stream buffer at the current output position ('pptr'), and advance
        // the output position one character forward.  This operation may
        // update the end of output area ('epptr') to allow for additional
        // writes (e.g., by the base 'basic_streambuf' type) to the output
        // sequence without calling a method on this type.  Return the written
        // character on success, and 'traits_type::eof()' if 'character' is
        // 'traits_type::eof()' or this stream buffer was not opened for
        // writing.

  public:
    // TYPETRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                             basic_stringbuf,
                             BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    basic_stringbuf(const allocator_type& allocator = allocator_type());
    explicit
    basic_stringbuf(ios_base::openmode    modeBitMask,
                    const allocator_type& allocator = allocator_type());
    explicit
    basic_stringbuf(const StringType&     initialString,
                    const allocator_type& allocator = allocator_type());
    basic_stringbuf(const StringType&     initialString,
                    ios_base::openmode    modeBitMask,
                    const allocator_type& allocator = allocator_type());
        // Create a 'basic_stringbuf' object.  Optionally specify a
        // 'modeBitMask' indicating whether this buffer may be read, written
        // to, or both.  If 'modeBitMask' is not supplied this buffer is
        // created with 'ios_base::in | ios_base::out'.  Optionally specify
        // 'initialString' indicating the initial sequence of characters that
        // this buffer will access or manipulate.  If 'initialString' is not
        // supplied, the initial sequence of characters will be empty.
        // Optionally specify 'allocator' used to supply memory.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // parameterized 'ALLOCATOR' type is used.  If the template parameter
        // 'ALLOCATOR' argument is of type 'bsl::allocator' (the default) then
        // 'allocator', if supplied, shall be convertible to 'bslma::Allocator
        // *'.  If the template parameter 'ALLOCATOR' argument is of type
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.

    ~basic_stringbuf();
        // Destroy this object.

    // MANIPULATORS
    void str(const StringType& value);
        // Reset the internally buffered sequence of characters to the
        // specified 'value'.  Update the beginning and end of both the input
        // and output sequences to be the beginning and end of the updated
        // buffer, update the current input position to be the beginning of
        // the updated buffer, and the current output position to be the end
        // of the updated buffer.

    // ACCESSORS
    StringType str() const;
        // Return the currently buffered sequence of characters.
};

// STANDARD TYPEDEFS
typedef basic_stringbuf<char, char_traits<char>, allocator<char> >   stringbuf;
typedef basic_stringbuf<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >
                                                                    wstringbuf;

                      // =========================
                      // struct StringbufContainer
                      // =========================

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
class StringbufContainer {
    // This class is intended to enable the implementation of string-stream
    // types by providing a trivial type containing a 'basic_stringbuf' that
    // is suitable as a (private) base class for a string-stream.  Inheriting
    // from 'StringbufContainer' allows the string-stream to ensure that the
    // contained 'basic_stringbuf' is initialized before initializing other
    // base-classes or data memenbers, without potentially overriding virtual
    // methods in the 'basic_stringbuf' type.  Note that implementations of
    // string-stream types must pass the address of a string-buffer to their
    // public base-class (e.g., 'basic_stream') and so the string-stream must
    // ensure (using private inheritence) that the string-buffer is
    // initialized before constructing the public base-class, however if a
    // string-stream  implementation were to directly inherit from
    // 'basic_streambuf', then virtual methods defined in that string-stream
    // (e.g., 'undeflow') might incorrectly override those in the
    // basic_stringbuf' implementation.

  private:
    // TYPES
    typedef basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StreamBufType;
    typedef bsl::basic_string<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR> StringType;

    // DATA
    StreamBufType d_bufObj;  // contained 'basic_stringbuf'

  public:
    // CREATORS
    explicit
    StringbufContainer(const ALLOCATOR& allocator)
    : d_bufObj(allocator)
    {}

    StringbufContainer(ios_base::openmode modeBitMask,
                       const ALLOCATOR&   allocator)
    : d_bufObj(modeBitMask, allocator)
    {}

    StringbufContainer(const StringType& initialString,
                       const ALLOCATOR&  allocator)
    : d_bufObj(initialString, allocator)
    {}

    StringbufContainer(const StringType&  initialString,
                       ios_base::openmode modeBitMask,
                       const ALLOCATOR&   allocator)
    : d_bufObj(initialString, modeBitMask, allocator)
    {}

    // ACCESSORS
    StreamBufType *rdbuf() const
    {
        return const_cast<StreamBufType *>(&d_bufObj);
    }
};

// ==========================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                            // ---------------------
                            // class basic_stringbuf
                            // ---------------------

// PRIVATE MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
                           updateInputPointers(char_type *currentInputPosition)
{
    BSLS_ASSERT(d_mode & ios_base::in);
    BSLS_ASSERT(&d_str[0] <= currentInputPosition);
    BSLS_ASSERT(currentInputPosition <= &d_str[0] + d_lastWrittenChar);

    char_type *dataPtr = &d_str[0];

    this->setg(dataPtr, currentInputPosition, dataPtr + d_lastWrittenChar);
    return pos_type(currentInputPosition - dataPtr);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
                         updateOutputPointers(char_type *currentOutputPosition)
{
    BSLS_ASSERT(d_mode & ios_base::out);
    BSLS_ASSERT(&d_str[0] <= currentOutputPosition);
    BSLS_ASSERT(currentOutputPosition < &d_str[0] + d_str.size());

    char_type          *dataPtr  = &d_str[0];
    native_std::size_t  dataSize =  d_str.size();

    pos_type outputPos(currentOutputPosition - dataPtr);
    this->setp(dataPtr, dataPtr + dataSize);
    this->pbump(outputPos);
    return outputPos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    updateStreamPositions(off_type inputOffset, off_type outputOffset)
{
    // Extend the internal buffer to the full capacity of the string, to allow
    // us to use the full capacity for buffering output.

    d_str.resize(d_str.capacity());
    char_type *dataPtr = &d_str[0];

    if (d_mode & ios_base::in) {
        // Update the input position.

        this->setg(dataPtr,
                   dataPtr + inputOffset,
                   dataPtr + d_lastWrittenChar);
    }

    if (d_mode & ios_base::out) {
        // Update the output position.

        native_std::size_t dataSize = d_str.size();
        this->setp(dataPtr, dataPtr + dataSize);
        if (outputOffset) {
            this->pbump(outputOffset);
        }
    }
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
bool basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::extendInputArea()
{
    // Try to extend into written buffer.

    if (d_mode & ios_base::out && this->pptr() > this->egptr()) {
        off_type currentOutputCharacter = this->pptr() - this->pbase();
        d_lastWrittenChar = native_std::max(d_lastWrittenChar,
                                            currentOutputCharacter);

        updateInputPointers(this->gptr());
        return true;                                                  // RETURN
    }

    return false;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::streamSize() const
{
    pos_type size = native_std::max<off_type>(
                              d_lastWrittenChar, this->pptr() - this->pbase());

    BSLS_ASSERT(size <= d_str.size());

    return size;
}

// PROTECTED MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::seekoff(
                                               off_type           offset,
                                               ios_base::seekdir  whence,
                                               ios_base::openmode modeBitMask)
{

    // If 'whence' is 'ios_base::cur' (the current position), 'modeBitMask'
    // may not be both input and output mode.

    if ((modeBitMask & (ios_base::in | ios_base::out))
                    == (ios_base::in | ios_base::out)
        && !(whence == ios_base::beg || whence == ios_base::end))
    {
        return pos_type(off_type(-1));                                // RETURN
    }

    pos_type newPos(off_type(-1));

    // Set the current input position.

    if ((modeBitMask & ios_base::in) && (d_mode & ios_base::in)) {
        char_type *inputPtr = 0;

        switch (whence) {
          case ios_base::beg:
            inputPtr = this->eback() + offset;
            break;
          case ios_base::cur:
            inputPtr = this->gptr() + offset;
            break;
          case ios_base::end:
            inputPtr = this->eback() + streamSize() + offset;
            break;
          default:
            BSLS_ASSERT(false && "invalid seekdir argument");
        }

        if (inputPtr < this->eback()
         || inputPtr > this->eback() + streamSize())
        {
            // 'inputPtr' is outside the valid range of the string buffer.

            return pos_type(off_type(-1));                            // RETURN
        }

        newPos = updateInputPointers(inputPtr);
    }

    // Set the current output position.

    if ((modeBitMask & ios_base::out) && (d_mode & ios_base::out)) {
        char_type *outputPtr = 0;

        switch (whence) {
          case ios_base::beg:
            outputPtr = this->pbase() + offset;
            break;
          case ios_base::cur:
            outputPtr = this->pptr() + offset;
            break;
          case ios_base::end:
            outputPtr = this->pbase() + streamSize() + offset;
            break;
          default:
            BSLS_ASSERT(false && "invalid seekdir argument");
        }

        if (outputPtr < this->pbase()
         || outputPtr > this->pbase() + streamSize())
        {
            // 'inputPtr' is outside the valid range of the string buffer.

            return pos_type(off_type(-1));                            // RETURN
        }

        newPos = updateOutputPointers(outputPtr);
    }

    return newPos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::seekpos(
                                             pos_type           absoluteOffset,
                                             ios_base::openmode modeBitMask)
{
    return basic_stringbuf::seekoff(off_type(absoluteOffset),
                                    ios_base::beg,
                                    modeBitMask);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
native_std::streamsize
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::xsgetn(
                                          char_type              *result,
                                          native_std::streamsize numCharacters)
{
    if (this->gptr() != this->egptr()) {
        // There are characters available in this buffer.

        native_std::streamsize available(this->egptr() - this->gptr());
        native_std::streamsize readChars(native_std::min(available,
                                                         numCharacters));

        traits_type::copy(result, this->gptr(), readChars);
        this->gbump(readChars);

        return readChars;                                             // RETURN
    }

    if (extendInputArea()) {
        // Additional characters may become available for reading when the
        // input area is extended to account for any characters newly written
        // to the output sequence.

        return this->basic_stringbuf::xsgetn(result, numCharacters);  // RETURN
    }

    return 0;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::underflow()
{
    if (this->gptr() != this->egptr()) {
        // There are characters available in this buffer.

        return traits_type::to_int_type(*this->gptr());               // RETURN
    }

    if (extendInputArea()) {
        // Additional characters may become available after the input area is
        // extended.

        return this->basic_stringbuf::underflow();                    // RETURN
    }

    return traits_type::eof();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::uflow()
{
    if (this->gptr() != this->egptr()) {
        // There are characters available in this buffer.

        int_type c = traits_type::to_int_type(*this->gptr());
        this->gbump(1);
        return c;                                                     // RETURN
    }

    if (extendInputArea()) {
        // Additional characters may become available after the input area is
        // extended.

        return this->basic_stringbuf::uflow();                        // RETURN
    }

    return traits_type::eof();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pbackfail(
                                                            int_type character)
{
    if (this->gptr() == this->eback()) {
        // The current position is at the start of the buffer, cannot push
        // back a character.

        return traits_type::eof();                                    // RETURN
    }

    if (traits_type::eq_int_type(character, traits_type::eof())
        || traits_type::eq_int_type(
                character,
                traits_type::to_int_type(*(this->gptr() - 1))))
    {
        // If 'character' is 'eof' or the previous input character,
        // simply move the current position back 1.

        this->gbump(-1);
        return traits_type::to_int_type(*this->gptr());               // RETURN
    }

    if (d_mode & ios_base::out) {
        // In output mode, if 'character' is not the previous input character,
        // overwrite the previous input character.

        this->gbump(-1);
        *this->gptr() = traits_type::to_char_type(character);
        return character;                                             // RETURN
    }

    return traits_type::eof();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
native_std::streamsize
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::xsputn(
                                         const char_type        *inputString,
                                         native_std::streamsize  numCharacters)
{
    if ((d_mode & ios_base::out) == 0) {
        return 0;                                                     // RETURN
    }

    // Compute the space required.

    native_std::streamsize spaceLeft(
            d_str.data() + d_str.size() - this->pptr());
    native_std::ptrdiff_t toOverwrite = native_std::min(spaceLeft,
                                                        numCharacters);

    // Append the portion of 'inputString' that can be written without
    // resizing 'd_ptr'.

    traits_type::copy(this->pptr(), inputString, toOverwrite);

    off_type inputOffset(this->gptr() - this->eback());

    if (numCharacters == toOverwrite) {
        // If all of 'inputString' has been written, just update the stream
        // positions.

        off_type newhigh = numCharacters + this->pptr() - this->pbase();
        d_lastWrittenChar = native_std::max(d_lastWrittenChar, newhigh);

        updateStreamPositions(inputOffset, newhigh);
    }
    else {
        // If some characters remain to be written, append them to 'd_str'
        // (resizing 'd_str' in the process'.

        d_str.append(inputString + toOverwrite, inputString + numCharacters);

        // Update the last written character cache, and the input stream
        // positions.

        d_lastWrittenChar = d_str.size();
        updateStreamPositions(inputOffset, d_lastWrittenChar);
    }

    return numCharacters;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::overflow(
                                                            int_type character)
{
    if ((d_mode & ios_base::out) == 0) {
        return traits_type::eof();                                    // RETURN
    }

    if (traits_type::eq_int_type(character, traits_type::eof())) {
        // Nothing to write, so just return success.

        return traits_type::not_eof(character);                       // RETURN
    }

    char_type ch = traits_type::to_char_type(character);
    if (this->pptr() != this->epptr()) {
        // Additional space is available in 'd_str', so no need to resize the
        // buffer.

        *this->pptr() = ch;
        this->pbump(1);

        d_lastWrittenChar = native_std::max<off_type>(
                              d_lastWrittenChar, this->pptr() - this->pbase());
    }
    else {
        // Store the input offset so it can be used to restore the input and
        // output positions after the next resize.

        off_type inputOffset(this->gptr() - this->eback());

        // Append the character, and expand the buffer.

        d_str.push_back(ch);

        // Update the input sequence, restoring the current input position
        // fron 'inputOffset', and updating the output sequence to reflect the
        // newly resized buffer.

        d_lastWrittenChar = d_str.size();
        updateStreamPositions(inputOffset, d_lastWrittenChar);
    }

    return character;
}

// CREATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const allocator_type& allocator)
: BaseType()
, d_str(allocator)
, d_lastWrittenChar(0)
, d_mode(ios_base::in | ios_base::out)
{
    updateStreamPositions();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(ios_base::openmode    modeBitMask,
                    const allocator_type& allocator)
: BaseType()
, d_str(allocator)
, d_lastWrittenChar(0)
, d_mode(modeBitMask)
{
    updateStreamPositions();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const StringType&     initialString,
                    const allocator_type& allocator)
: BaseType()
, d_str(initialString, allocator)
, d_lastWrittenChar(initialString.size())
, d_mode(ios_base::in | ios_base::out)
{
    updateStreamPositions();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const StringType&     initialString,
                    ios_base::openmode    modeBitMask,
                    const allocator_type& allocator)
: BaseType()
, d_str(initialString, allocator)
, d_lastWrittenChar(initialString.size())
, d_mode(modeBitMask)
{
    updateStreamPositions(
        0,
        (d_mode & ios_base::ate) != 0 ? d_lastWrittenChar : 0);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    ~basic_stringbuf()
{
    if (d_mode & ios_base::in) {
        BSLS_ASSERT(this->eback() == d_str.data());
        BSLS_ASSERT(this->egptr() <= d_str.data() + d_str.size());
        BSLS_ASSERT(this->eback() <= this->gptr());
        BSLS_ASSERT(this->gptr() <= this->egptr());
    }

    if (d_mode & ios_base::out) {
        BSLS_ASSERT(this->pbase() == d_str.data());
        BSLS_ASSERT(this->epptr() == d_str.data() + d_str.size());
        BSLS_ASSERT(this->pbase() <= this->pptr());
        BSLS_ASSERT(this->pptr() <= this->epptr());
    }
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
                                                       const StringType& value)
{
    d_str = value;
    d_lastWrittenChar = d_str.size();
    updateStreamPositions(0, d_lastWrittenChar);
}

// ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return StringType(d_str.begin(), d_str.begin() + streamSize());
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
