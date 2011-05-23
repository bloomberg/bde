// baesu_stacktraceframe.h                                            -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#define INCLUDED_BAESU_STACKTRACEFRAME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class representing a stack frame
//
//@CLASSES:
//   baesu_StackTraceFrame: a representation of one frame of a stack trace
//
//@AUTHOR: Bill Chapman
//
//@SEE_ALSO: baesu_stacktrace
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// 'baesu_StackTraceFrame', that represents information about a single function
// call on the call stack.  A 'baesu_StackTraceFrame' stores the traits
// 'address', 'libraryFileName', 'lineNumber', 'mangledSymbolName',
// 'offsetFromSymbol', 'sourceFileName', and 'symbolName'.  Many of these
// properties which are represented by 'bsl::string's, where a value of "" is
// consider invalid for those attributes, the 'lineNumber' property has the
// value 'BAESU_INVALID_LINE_NUMBER' if it does not represet a valid line
// number, the 'offsetFromSymbol' property has the value 'BAESU_INVALID_OFFSET'
// if it is not a valid value.  All values are assigned invalid values by the
// default constructor.  Each attribute has an 'is<attribute name>Valid' method
// which returns 'true' if the attribute has a valid value and 'false'
// otherwise.  Each attribute has a gettor, which has just the name of the
// attribute, and at least one settor, which has the name
// 'set<attribute name>'.  String attributes all have two settors, one that
// takes a reference to a const string, and one which takes a 'const char *'.
// It is only possible to set attributes to valid values.
//
///Usage
///-----
//..
// Here we create two 'baesu_StackTraceFrame' objects, modify their
// properties, and compare them.  First, we create the objects 'a' and
// 'b':
//..
//  baesu_StackTraceFrame a, b;
//
//  assert(a == b);
//..
// Next, we verify all values are initialized by the constructor to
// invalid values.
//..
//  assert(!a.isAddressValid());
//  assert(!a.isLibraryFileNameValid());
//  assert(!a.isLineNumberValid());
//  assert(!a.isMangledSymbolNameValid());
//  assert(!a.isOffsetFromSymbolValid());
//  assert(!a.isSourceFileNameValid());
//  assert(!a.isSymbolNameValid());
//
//  a.setLineNumber(5);
//  assert(a.isLineNumberValid());
//
//  assert(a != b);
//
//  b.setLineNumber(5);
//  assert(b.isLineNumberValid());
//
//  assert(a == b);
//
//  a.setAddress((char *) 0x12345678);
//  b.setAddress(a.address());
//  assert(a.isAddressValid());
//  assert(b.isAddressValid());
//
//  a.setSymbolName("woof");
//  b.setSymbolName(a.symbolName());
//
//  a.setSourceFileName("woof.cpp");
//  b.setSourceFileName(a.sourceFileName());
//
//  assert(a == b);
//..
// Next, we output 'a' and it produces
// "woof at 0x12345678 source:woof.cpp:5\n".
//..
//  cout << a;
//..
// Finally, we show that identical strings in different locations are
// equal.
//..
//  char buf[100];
//  bsl::strcpy(buf, "woof");
//  b.setSymbolName(buf);
//
//  assert(a == b);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>      // bsl::size_t
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                       // ===========================
                       // class baesu_StackTraceFrame
                       // ===========================

class baesu_StackTraceFrame {
    // This class provides an unconstrained value-semantic attribute class that
    // represents a frame in a stack trace.  This class does not enforce any
    // relationship between its different attributes.  The default constructor
    // initializes all fields to invalid values.  Once assigned with the
    // appropriate 'set*' method, the value of a field is valid.  Use the
    // 'is*Valid' methods to determine validity of a field.  The traits of this
    // class are 'address', 'libraryFileName', 'lineNumber',
    // 'mangledSymbolName', 'offsetFromSymbol', 'sourceFileName', and
    // 'symbolName'.  Where you have a function 'child' called by a parent
    // function 'parent', on the stack frame of 'child' is the address within
    // 'parent' of the first instruction to be called after 'child' returns.
    // The 'address' field is this address within 'parent', and all other
    // fields in the 'baesu_StackTraceFrame' refer to the function 'parent'.

    // CONSTANTS
    enum {
        BAESU_INVALID_LINE_NUMBER = -1, // the 'lineNumber' attribute is
                                        // initialized to this
        BAESU_INVALID_OFFSET      = -1  // the 'offset' attribute is
                                        // initialized to this, must be cast to
                                        // a 'size_t' for comparison or
                                        // assignment, see 'invalidOffset'
    };

  private:
    // DATA
    const void *d_address_p;            // address of code within the function
                                        // referred to by this object, if
                                        // known, and 0 otherwise

    bsl::string d_libraryFileName;      // name of the executable file or
                                        // shared library, where the code
                                        // referred to by 'd_address_p'
                                        // resides, if known, and "" otherwise

    int         d_lineNumber;           // the line number in the source file
                                        // corresponding to 'd_address_p', if
                                        // known, and
                                        // 'BAESU_INVALID_LINE_NUMBER'
                                        // otherwise

    bsl::size_t d_offsetFromSymbol;     // the offset (in bytes) from the
                                        // beginning of the function into which
                                        // 'd_address_p' points and
                                        // 'd_address_p', if known, and
                                        // 'BAESU_INVALID_OFFSET' otherwise

    bsl::string d_sourceFileName;       // the name of the source file
                                        // corresponding to the code referred
                                        // to by 'd_address_p', if known, and
                                        // "" otherwise

    bsl::string d_mangledSymbolName;    // The mangled name of the routine
                                        // where 'd_address_p' resides, if
                                        // known, and "" otherwise.

    bsl::string d_symbolName;           // The name of the routine containing
                                        // the instruction pointed at by
                                        // 'd_address_p', if known, and ""
                                        // otherwise.

    bslma_Allocator
               *d_allocator_p;          // allocator (held, not owned)

  public:
    BSLALG_DECLARE_NESTED_TRAITS(baesu_StackTraceFrame,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static
    bsl::size_t invalidOffset();
        // The value of an offset before it is set.

    // CREATORS
    explicit
    baesu_StackTraceFrame(bslma_Allocator *allocator = 0);
        // Create a 'baesu_StackTraceFrame' object with all members set to
        // invalid values: string pointer values default to "", the
        // 'lineNumber' value to 'BAESU_INVALID_LINE_NUMBER', and the
        // 'offsetFromSymbol' value to 'BAESU_INVALID_OFFSET'.

    baesu_StackTraceFrame(const baesu_StackTraceFrame&  original,
                          bslma_Allocator              *allocator = 0);
        // Create a 'baesu_StackTraceFrame' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ~baesu_StackTraceFrame();
        // compiler generated

    // MANIPULATORS
    baesu_StackTraceFrame& operator=(const baesu_StackTraceFrame& other);
        // Copy all value fields from 'other' to this stack trace frame object.

    void setAddress(const void *address);
        // Set the address attribute of this object to the specified 'address'.
        // The bahavior is undefined if 'address == 0'.

    void setLibraryFileName(const char         *libraryFileNameValue);
    void setLibraryFileName(const bsl::string&  libraryFileNameValue);
        // Set the libraryFileName attribute of this object to the specified
        // 'libraryFileNameValue'.  The behavior is undefined if
        // 'libraryFileNameValue' is 0 or "".

    void setLineNumber(int lineNumberValue);
        // Set the lineNumber attribute of this object to the specified
        // 'lineNumberValue'.  The behavior is undefined if
        // 'lineNumberValue < 0'.

    void setMangledSymbolName(const char         *mangledSymbolNameValue);
    void setMangledSymbolName(const bsl::string&  mangledSymbolNameValue);
        // Set the mangledSymbolName attribute of this object to the specified
        // 'mangledSymbolNameValue'.  The behavior is undefined if
        // 'mangledSymbolNameValue' is 0 or "".

    void setOffsetFromSymbol(bsl::size_t offsetFromSymbolValue);
        // Set the offsetFromSymbol attribute of this object to the specified
        // 'offsetFromSymbolValue'.  The behavior is undefined if
        // 'offsetFromSymbolValue == invalidOffset()'.

    void setSourceFileName(const char         *sourceFileNameValue);
    void setSourceFileName(const bsl::string&  sourceFileNameValue);
        // Set the sourceFileName attribute of this object to the specified
        // 'sourceFileNameValue'.  The behavior is undefined if
        // 'sourceFileNameValue' is 0 or "".

    void setSymbolName(const char         *symbolNameValue);
    void setSymbolName(const bsl::string&  symbolNameValue);
        // Set the symbolName attribute of this object to the specified
        // 'symbolNameValue'.  The behavior is undefined if 'symbolNameValue'
        // is 0 or "".

    bslma_Allocator *allocator();
        // Return the allocator used by this object.

    // ACCESSORS
    bool isAddressValid() const;
        // 'true' if 'setAddress' has been called and 'false' otherwise.

    bool isLibraryFileNameValid() const;
        // 'true' if 'setLibraryFileName' has been called and 'false'
        // otherwise.

    bool isLineNumberValid() const;
        // 'true' if 'setLineNumber' has been called and 'false' otherwise.

    bool isMangledSymbolNameValid() const;
        // 'true' if 'setMangledSymbolName' has been called and 'false'
        // otherwise.

    bool isOffsetFromSymbolValid() const;
        // 'true' if 'setOffsetFromSymbol' has been called and 'false'
        // otherwise.

    bool isSourceFileNameValid() const;
        // 'true' if 'sourceFileName' has been called and 'false' otherwise.

    bool isSymbolNameValid() const;
        // 'true' if 'symbolName' has been called and 'false' otherwise.

    const void *address() const;
        // Return the 'address' referred to by this object or 0 if it has not
        // been set.  It is the pointer from the stack that points at the first
        // instruction to be executed when another subroutine that the
        // subroutine referred to by this object has called returns.

    const bsl::string& libraryFileName() const;
        // Return the name of the program file or shared library referred to by
        // this object or "" if it has not been set.

    int lineNumber() const;
        // Return the line number in the source file of the statement
        // corresponding to 'address'.

    const bsl::string& mangledSymbolName() const;
        // Return a the mangled name of the function corresponding to this
        // object or "" if it has not been set.

    bsl::size_t offsetFromSymbol() const;
        // Return the offset of 'address' from the address of the beginning of
        // the function represented by 'symbolName'.

    const bsl::string& sourceFileName() const;
        // Return the name of the source file containing the function to which
        // this object refers or "" if it has not been set.

    const bsl::string& symbolName() const;
        // Return a the name of the function to which this object refers, or ""
        // if it has not been set.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change
        // without notice.
};

// FREE OPERATORS
bool operator==(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs);
    // Return 'true' if all corresponding attributes of the specified 'lhs' and
    // 'rhs' stack trace frame objects have the same value, and 'false'
    // otherwise.

bool operator!=(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs);
    // Return 'true' if any corresponding attributes of the specified 'lhs' and
    // 'rhs' stack trace frame objects do not have the same value, and 'false'
    // otherwise.    // equal strings are considered equal.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baesu_StackTraceFrame& frame);
    // Print the contents of 'frame' to the specified 'stream' in a plain text
    // format, on one line, omitting the mangled symbol name.  Fields that are
    // invalid are either not printed or printed as "--unknown--".  Return a
    // reference to the specified 'stream'.  Note that this function does not
    // call 'print' and is intended to be used to print a single line in a
    // stack trace.  Also note that this human-readable format is not fully
    // specified and can change without notice

// ===========================================================================
//                       INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ---------------------------
                       // class baesu_StackTraceFrame
                       // ---------------------------

// CLASS METHODS
inline
bsl::size_t baesu_StackTraceFrame::invalidOffset()
{
    return (bsl::size_t) BAESU_INVALID_OFFSET;
}

// CREATORS
inline
baesu_StackTraceFrame::baesu_StackTraceFrame(bslma_Allocator *allocator)
: d_address_p(0)
, d_libraryFileName(bslma_Default::allocator(allocator))
, d_lineNumber(BAESU_INVALID_LINE_NUMBER)
, d_offsetFromSymbol(invalidOffset())
, d_sourceFileName(bslma_Default::allocator(allocator))
, d_mangledSymbolName(bslma_Default::allocator(allocator))
, d_symbolName(bslma_Default::allocator(allocator))
, d_allocator_p(bslma_Default::allocator(allocator))
{
}

inline
baesu_StackTraceFrame::baesu_StackTraceFrame(
                                       const baesu_StackTraceFrame&  original,
                                       bslma_Allocator              *allocator)
: d_address_p(        original.d_address_p)
, d_libraryFileName(  original.d_libraryFileName,
                      bslma_Default::allocator(allocator))
, d_lineNumber(       original.d_lineNumber)
, d_offsetFromSymbol( original.d_offsetFromSymbol)
, d_sourceFileName(   original.d_sourceFileName,
                      bslma_Default::allocator(allocator))
, d_mangledSymbolName(original.d_mangledSymbolName,
                      bslma_Default::allocator(allocator))
, d_symbolName(       original.d_symbolName,
                      bslma_Default::allocator(allocator))
, d_allocator_p(      bslma_Default::allocator(allocator))
{
}

// MANIPULATORS
inline
baesu_StackTraceFrame& baesu_StackTraceFrame::operator=(
                                            const baesu_StackTraceFrame& other)
{
    d_address_p         = other.d_address_p;
    d_libraryFileName   = other.d_libraryFileName;
    d_lineNumber        = other.d_lineNumber;
    d_offsetFromSymbol  = other.d_offsetFromSymbol;
    d_sourceFileName    = other.d_sourceFileName;
    d_mangledSymbolName = other.d_mangledSymbolName;
    d_symbolName        = other.d_symbolName;

    return *this;
}

inline
void baesu_StackTraceFrame::setAddress(const void *addressValue)
{
    BSLS_ASSERT_SAFE(addressValue);

    d_address_p = addressValue;
}

inline
void baesu_StackTraceFrame::setLibraryFileName(
                                              const char *libraryFileNameValue)
{
    BSLS_ASSERT_SAFE( libraryFileNameValue);
    BSLS_ASSERT_SAFE(*libraryFileNameValue);

    d_libraryFileName = libraryFileNameValue;
}

inline
void baesu_StackTraceFrame::setLibraryFileName(
                                       const bsl::string& libraryFileNameValue)
{
    BSLS_ASSERT_SAFE(!libraryFileNameValue.empty());

    d_libraryFileName = libraryFileNameValue;
}

inline
void baesu_StackTraceFrame::setLineNumber(int lineNumberValue)
{
    BSLS_ASSERT_SAFE(lineNumberValue > 0);

    d_lineNumber = lineNumberValue;
}

inline
void baesu_StackTraceFrame::setMangledSymbolName(
                                            const char *mangledSymbolNameValue)
{
    BSLS_ASSERT_SAFE( mangledSymbolNameValue);
    BSLS_ASSERT_SAFE(*mangledSymbolNameValue);

    d_mangledSymbolName = mangledSymbolNameValue;
}

inline
void baesu_StackTraceFrame::setMangledSymbolName(
                                     const bsl::string& mangledSymbolNameValue)
{
    BSLS_ASSERT_SAFE(!mangledSymbolNameValue.empty());

    d_mangledSymbolName = mangledSymbolNameValue;
}

inline
void baesu_StackTraceFrame::setOffsetFromSymbol(
                                             bsl::size_t offsetFromSymbolValue)
{
    BSLS_ASSERT_SAFE(invalidValue() != offsetFromSymbolValue);

    d_offsetFromSymbol = offsetFromSymbolValue;
}

inline
void baesu_StackTraceFrame::setSourceFileName(const char *sourceFileNameValue)
{
    BSLS_ASSERT_SAFE( sourceFileNameValue);
    BSLS_ASSERT_SAFE(*sourceFileNameValue);

    d_sourceFileName = sourceFileNameValue;
}

inline
void baesu_StackTraceFrame::setSourceFileName(
                                        const bsl::string& sourceFileNameValue)
{
    BSLS_ASSERT_SAFE(!sourceFileNameValue.empty());

    d_sourceFileName = sourceFileNameValue;
}

inline
void baesu_StackTraceFrame::setSymbolName(const char *symbolNameValue)
{
    BSLS_ASSERT_SAFE( symbolNameValue);
    BSLS_ASSERT_SAFE(*symbolNameValue);

    d_symbolName = symbolNameValue;
}

inline
void baesu_StackTraceFrame::setSymbolName(const bsl::string& symbolNameValue)
{
    BSLS_ASSERT_SAFE(!symbolNameValue.empty());

    d_symbolName = symbolNameValue;
}

inline
bslma_Allocator *baesu_StackTraceFrame::allocator()
{
    return d_allocator_p;
}

// ACCESSORS
inline
const void *baesu_StackTraceFrame::address() const
{
    return d_address_p;
}

inline
bool baesu_StackTraceFrame::isAddressValid() const
{
    return 0 != d_address_p;
}

inline
bool baesu_StackTraceFrame::isLibraryFileNameValid() const
{
    return !d_libraryFileName.empty();
}

inline
bool baesu_StackTraceFrame::isLineNumberValid() const
{
    return d_lineNumber > 0;
}

inline
bool baesu_StackTraceFrame::isMangledSymbolNameValid() const
{
    return !d_mangledSymbolName.empty();
}

inline
bool baesu_StackTraceFrame::isOffsetFromSymbolValid() const
{
    return invalidOffset() != d_offsetFromSymbol;
}

inline
bool baesu_StackTraceFrame::isSourceFileNameValid() const
{
    return !d_sourceFileName.empty();
}

inline
bool baesu_StackTraceFrame::isSymbolNameValid() const
{
    return !d_symbolName.empty();
}

inline
const bsl::string& baesu_StackTraceFrame::libraryFileName() const
{
    return d_libraryFileName;
}

inline
int baesu_StackTraceFrame::lineNumber() const
{
    return d_lineNumber;
}

inline
bsl::size_t baesu_StackTraceFrame::offsetFromSymbol() const
{
    return d_offsetFromSymbol;
}

inline
const bsl::string& baesu_StackTraceFrame::mangledSymbolName() const
{
    return d_mangledSymbolName;
}

inline
const bsl::string& baesu_StackTraceFrame::sourceFileName() const
{
    return d_sourceFileName;
}

inline
const bsl::string& baesu_StackTraceFrame::symbolName() const
{
    return d_symbolName;
}

// FREE OPERATORS
inline
bool operator==(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs)
{
    return lhs.address()           == rhs.address()           &&
           lhs.libraryFileName()   == rhs.libraryFileName()   &&
           lhs.lineNumber()        == rhs.lineNumber()        &&
           lhs.mangledSymbolName() == rhs.mangledSymbolName() &&
           lhs.offsetFromSymbol()  == rhs.offsetFromSymbol()  &&
           lhs.sourceFileName()    == rhs.sourceFileName()    &&
           lhs.symbolName()        == rhs.symbolName();
}

inline
bool operator!=(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
