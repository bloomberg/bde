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
//  bsl::streambuf: C++03-compatible stringbuf class
//
//@SEE_ALSO:
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_sstream.h>' instead.
//
// This component provides 'bsl::basic_stringbuf' class template
// parameterized with a character type and two specializations: one for 'char'
// and another for 'wchar_t'.  'bsl::basic_stringbuf' implements the
// functionality of the standard class 'std::basic_streambuf' for reading and
// writing into 'bsl::basic_string' objects.  It's a 'bsl' replacement for the
// standard 'std::basic_stringbuf' class.
//
// 'bsl::basic_stringbuf' is implemented by deriving from 'std::basic_streambuf'
// and implementing the necessary protected virtual methods.  This way
// 'bsl::basic_stringbuf' customizes the behavior of 'std::basic_streambuf'
// to redirect reading and writing of characters from/into 'bsl::basic_string'.
//
// As with any stream buffer class, 'bsl::basic_stringbuf' is rarely used
// directly.  Stream buffers provide low-level unformatted input/output.  They
// are usually plugged into 'std::basic_stream' classes to provide higher-level
// formatted input/output via 'operator<<' and 'operator>>'.
// 'bsl::basic_stringbuf' is used in 'bsl::basic_stringstream' family of classes
// Users should prefer 'bsl::basic_stringstream' classes instead of directly
// using 'bsl::basic_stringbuf'.
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
// Then, we use 'bsl::stringbuf' to implement the 'toString' function.  We write
// all digits into 'bsl::stringbuf' individually using 'sputc' methods and then
// return the resulting 'bsl::string' object as follows:
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

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#define INCLUDED_BSLSTL_STRING
#endif

#ifndef INCLUDED_BSLSTL_IOSFWD
#include <bslstl_iosfwd.h>
#define INCLUDED_BSLSTL_IOSFWD
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
    : public native_std::basic_streambuf<CHAR_TYPE, CHAR_TRAITS>
    // This class implements the functionality of the 'streambuf' class
    // perform unformatted reads and writes of character data from/to
    // 'bsl::string' objects.
{
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
    StringType         d_str;   // internal string representation
    ios_base::openmode d_mode;  // stringbuf open mode ('in' or 'out')

  private:
    // PRIVATE CREATORS
    basic_stringbuf(const basic_stringbuf&);            // = delete
    basic_stringbuf& operator=(const basic_stringbuf&); // = delete

    // PRIVATE MANIPULATORS
    pos_type updateInputPointer(char_type *input_p);
        // Update the input pointers (eback, gptr, egptr) of this string buffer
        // object using the internal string buffer representation for 'eback'
        // and 'egptr' and the specified 'input_p' for 'gptr'.  The behavior is
        // undefined unless 'input_p' is within the range of the internal
        // string representation.

    pos_type updateOutputPointer(char_type *output_p);
        // Update the output pointers (pbase, pptr, epptr) of this string
        // buffer object using the internal string buffer representation for
        // 'pbase' and 'epptr' and the specified 'output_p' for 'pptr'.  The
        // behavior is undefined unless 'output_p' is within the range of the
        // internal string representation.

    void updatePointers(off_type inputOffset = 0);
        // Update the input and output pointers of this string buffer object
        // using the internal string buffer representation and set the current
        // output pointer, 'pptr', to the beginning, 'pbase', and the current
        // input pointer, 'gptr', according to the optionally specified
        // 'inputOffset' to 'eback + inputOffset'.

  protected:
    // PROTECTED MANIPULATORS
    virtual pos_type seekoff(
                      off_type off,
                      ios_base::seekdir way,
                      ios_base::openmode which = ios_base::in | ios_base::out);
        // Set the position of the current input and/or output pointers
        // depending on the optionally specified 'which' parameter, relative to
        // the specified seek direction of the 'way' parameter, and with offset
        // value of the specified 'off' parameter.  Return the new absolute
        // position value or 'pos_type(-1)' on error.

    virtual pos_type seekpos(
                      pos_type pos,
                      ios_base::openmode which = ios_base::in | ios_base::out);
        // Set the absolute position of the current input and/or output
        // pointers to the value of the specified 'pos' parameter depending on
        // the optionally specified 'which' parameter.  Return the new position
        // value or 'pos_type(-1)' on error.

    virtual native_std::streamsize xsgetn(char_type *s,
                                          native_std::streamsize n);
        // Read up to the specified 'n' characters from this 'stringbuf' object
        // and store them in the array pointed by the specified 's' argument.
        // Return the number of characters read.  Note that if less than 'n'
        // characters are available, all available characters are read.

    virtual int_type underflow();
        // Return the character at the current input position, if available,
        // and 'traits_type::eof()' otherwise.

    virtual int_type uflow();
        // Return the character at the current input position, if available,
        // and 'traits_type::eof()' otherwise.  Adjust the input position to
        // point to the next character in this 'stringbuf' input stream, if
        // available.

    virtual int_type pbackfail(int_type c = traits_type::eof());
        // Put back the specified 'c' character and adjust the input position
        // so that the next character read is 'c'.  If
        // 'c == traits_type::eof()', do nothing and return
        // 'traits_type::eof()'.  Return the character that was put back on
        // success and 'traits_type::eof()' on failure.  Note that this
        // function provides a "best effort" contract.  It may fail to put back
        // a character if doing so is prohibitively expensive.

    virtual native_std::streamsize xsputn(const char_type *s,
                                          native_std::streamsize n);
        // Write the specified 'n' characters from the array pointed by the
        // specified 's' argument into this 'stringbuf' strting starting from
        // the current output position.  Update the output position
        // accordingly.  Return the number of characters written.

    virtual int_type overflow(int_type c = traits_type::eof());
        // Write the specified character 'c' into this 'stringbuf' at the
        // current output position.  Advance the output position one character
        // forward.  Return the written character, and 'traits_type::eof()' on
        // error or if 'c == traits_type::eof()'.

  public:
    // TYPETRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
            basic_stringbuf,
            BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    basic_stringbuf(const allocator_type& alloc = allocator_type());
    explicit
    basic_stringbuf(ios_base::openmode mode,
                    const allocator_type& alloc = allocator_type());
    explicit
    basic_stringbuf(const StringType& str,
                    const allocator_type& alloc = allocator_type());
    explicit
    basic_stringbuf(const StringType& str,
                    ios_base::openmode mode,
                    const allocator_type& alloc = allocator_type());
        // Create an object of 'strinbuf' class using an optionally specified
        // 'alloc' allocator, having the specified input/output 'mode' and the
        // initial string 'str'.

    ~basic_stringbuf();
        // Destroy the 'strinbbuf' object.

    // ACCESSORS
    StringType str() const;
        // Return the string used for input/output of this 'stringbuf' object.

    // MANIPULATORS
    void str(const StringType& s);
        // Initialize this 'stringbuf' object with the specified string 's'.
        // Reset the input and output positions.
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
    updateInputPointer(char_type *input_p)
{
    char_type *data_p = &d_str[0];
    native_std::size_t dataSize = d_str.size();

    this->setg(data_p, input_p, data_p + dataSize);
    return pos_type(input_p - data_p);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    updateOutputPointer(char_type *output_p)
{
    char_type *data_p = &d_str[0];
    native_std::size_t dataSize = d_str.size();
    pos_type outputPos(output_p - data_p);

    this->setp(data_p, data_p + dataSize);
    this->pbump(outputPos);
    return outputPos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
void basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    updatePointers(off_type inputOffset)
{
    char_type *data_p = &d_str[0];
    native_std::size_t dataSize = d_str.size();

    this->setg(data_p, data_p + inputOffset, data_p + dataSize);
    this->setp(data_p, data_p + dataSize);
    this->pbump(int(dataSize));
}

// PROTECTED MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::seekoff(
                  off_type off,
                  ios_base::seekdir way,
                  ios_base::openmode which)
{
    // when setting both input and output positions,
    // allow them to be relative only to 'beg' and 'end'
    if ((which & (ios_base::in | ios_base::out))
            == (ios_base::in | ios_base::out)
        && !(way == ios_base::beg || way == ios_base::end))
    {
        return pos_type(-1);
    }

    pos_type newPos(-1);

    // set input position
    if ((which & ios_base::in) && (d_mode & ios_base::in)) {
        char_type *input_p = 0;

        switch (way) {
          case ios_base::beg:
            input_p = this->eback() + off;
            break;
          case ios_base::cur:
            input_p = this->gptr() + off;
            break;
          case ios_base::end:
            input_p = this->egptr() + off;
            break;
          default:
            BSLS_ASSERT(false && "invalid seekdir argument");
        }

        if (input_p < this->eback() || input_p > this->egptr()) {
            // out of range
            return pos_type(-1);
        }

        newPos = updateInputPointer(input_p);
    }

    // set output position
    if ((which & ios_base::out) && (d_mode & ios_base::out)) {
        char_type *output_p = 0;

        switch (way) {
          case ios_base::beg:
            output_p = this->pbase() + off;
            break;
          case ios_base::cur:
            output_p = this->pptr() + off;
            break;
          case ios_base::end:
            output_p = this->epptr() + off;
            break;
          default:
            BSLS_ASSERT(false && "invalid seekdir argument");
        }

        if (output_p < this->pbase() || output_p > this->epptr()) {
            // out of range
            return pos_type(-1);
        }

        newPos = updateOutputPointer(output_p);
    }

    return newPos;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pos_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::seekpos(
            pos_type pos,
            ios_base::openmode which)
{
    return basic_stringbuf::seekoff(off_type(pos),
                                    ios_base::beg,
                                    which);
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
native_std::streamsize
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::xsgetn(
            char_type *s,
            native_std::streamsize n)
{
    native_std::streamsize available(this->egptr() - this->gptr());
    native_std::streamsize readChars(std::min(available, n));

    traits_type::copy(s, this->gptr(), readChars);
    this->gbump(readChars);

    return readChars;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::underflow()
{
    return this->gptr() != this->egptr()
                        ?  traits_type::to_int_type(*this->gptr())
                        :  traits_type::eof();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::uflow()
{
    int_type c = traits_type::eof();

    if (this->gptr() != this->egptr()) {
        c = traits_type::to_int_type(*this->gptr());
        this->gbump(1);
    }

    return c;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::pbackfail(int_type c)
{
    if (this->gptr() == this->eback()) {
        // at the start of the buffer, cannot put back
        return traits_type::eof();
    }

    if (traits_type::eq_int_type(c, traits_type::eof())
        || traits_type::eq_int_type(c, *(this->gptr() - 1)))
    {
        // putting back eof or the previous char,
        // just adjust the input pointer
        this->gbump(-1);
        return c;
    }

    if (d_mode & ios_base::out) {
        // in out mode allow overwriting the previous char
        this->gbump(-1);
        *this->gptr() = traits_type::to_char_type(c);
        return c;
    }

    // otherwise, cannot put back
    return traits_type::eof();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
native_std::streamsize
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::xsputn(
            const char_type *s,
            native_std::streamsize n)
{
    // check if the stream is writeable
    if ((d_mode & ios_base::out) == 0) {
        return 0;
    }

    // compute required space
    native_std::streamsize spaceLeft(
            d_str.data() + d_str.size() - this->pptr());
    native_std::ptrdiff_t toOverwrite = native_std::min(spaceLeft, n);

    // overwrite
    traits_type::copy(this->pptr(), s, toOverwrite);

    // append
    off_type inputOffset(this->gptr() - this->eback());
    d_str.append(s + toOverwrite, s + n);

    // update pointer positions
    updatePointers(inputOffset);

    return n;
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::int_type
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::overflow(
            int_type c)
{
    // check if we can write to stream
    if (traits_type::eq_int_type(c, traits_type::eof())
            || (d_mode & ios_base::out) == 0)
    {
        return traits_type::eof();
    }

    char_type ch = traits_type::to_char_type(c);
    if (this->pptr() != this->epptr()) {
        // space is available, no need to expand the storage
        *this->pptr() = ch;
        this->pbump(1);
    }
    else {
        // store the input position to restore later
        off_type inputOffset(this->gptr() - this->eback());

        // expand the storage
        d_str.push_back(ch);

        // update pointer positions
        updatePointers(inputOffset);
    }

    return c;
}

// CREATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const allocator_type& alloc)
: BaseType()
, d_str(alloc)
, d_mode(ios_base::in | ios_base::out)
{
    updatePointers();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(ios_base::openmode mode,
                    const allocator_type& alloc)
: BaseType()
, d_str(alloc)
, d_mode(mode)
{
    updatePointers();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const StringType& str,
                    const allocator_type& alloc)
: BaseType()
, d_str(str, alloc)
, d_mode(ios_base::in | ios_base::out)
{
    updatePointers();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    basic_stringbuf(const StringType& str,
                    ios_base::openmode mode,
                    const allocator_type& alloc)
: BaseType()
, d_str(str, alloc)
, d_mode(mode)
{
    updatePointers();
}

template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::
    ~basic_stringbuf()
{
    BSLS_ASSERT(this->eback() == d_str.data());
    BSLS_ASSERT(this->pbase() == d_str.data());
    BSLS_ASSERT(this->egptr() == d_str.data() + d_str.size());
    BSLS_ASSERT(this->epptr() == d_str.data() + d_str.size());

    BSLS_ASSERT(this->eback() <= this->gptr());
    BSLS_ASSERT(this->gptr() <= this->egptr());
    BSLS_ASSERT(this->pbase() <= this->pptr());
    BSLS_ASSERT(this->pptr() <= this->epptr());
}

// ACCESSORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
typename basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::StringType
    basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str() const
{
    return d_str;
}

// MANIPULATORS
template <typename CHAR_TYPE, typename CHAR_TRAITS, typename ALLOCATOR>
inline
void basic_stringbuf<CHAR_TYPE, CHAR_TRAITS, ALLOCATOR>::str(
        const StringType& s)
{
    d_str = s;
    updatePointers();
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
