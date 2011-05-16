// bdesu_stacktraceframe.h                                            -*-C++-*-
#ifndef INCLUDED_BDESU_STACKTRACEFRAME
#define INCLUDED_BDESU_STACKTRACEFRAME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class representing a stack frame
//
//@CLASSES:
//   bdesu_StackTraceFrame: a representation of one frame of a stack trace
//
//@AUTHOR: Bill Chapman
//
//@SEE_ALSO: bdesu_stacktrace
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// 'bdesu_StackTraceFrame', that represents a single information about a single
// function call on the call stack.  A 'bdesu_StackTraceFrame' stores the
// traits 'address', 'libraryFileName', 'lineNumber', 'mangledSymbolName',
// 'offsetFromSymbol', 'sourceFileName', and 'symbolName'.  All of these
// properties which are represented by 'const char *' pointers can be 0, the
// 'lineNumber' property has the value 'INVALID_LINE_NUMBER' if it does not
// represet a valid line number, the 'offsetFromSymbol' property has the value
// 'INVALID_OFFSET' if it is not a valid value.  All values are assigned
// invalid values by the default constructor, after which, once the
// corresponding 'set' routine has been called, they are valid.
//
///Usage
///-----
//..
// Here we create two 'bdesu_StackTraceFrame' objects, modify their
// properties, and compare them.  First, we create the objects 'a' and
// 'b':
//..
//  bdesu_StackTraceFrame a, b;
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>      // bsl::size_t
#endif

namespace BloombergLP {

                       // ===========================
                       // class bdesu_StackTraceFrame
                       // ===========================

class bdesu_StackTraceFrame {
    // This class provides a value-semantic attribute class that represents a
    // frame in a stack trace.  This class does not enforce any relationship
    // between its different properties.  'bdesu_StackTraceFrame' does not
    // manage the lifetime of any of the objects to which it refers.  The copy
    // constructor, assignment operator, and destructor are provided by the
    // compiler by default.  The default constructor initializes all fields to
    // invalid values.  Once assigned with the 'set*' methods, the value of a
    // field is valid.  Use the 'is*Valid' methods to determine validity of a
    // field.  The traits of this class are 'address', 'libraryFileName',
    // 'lineNumber', 'mangledSymbolName', 'offsetFromSymbol', 'sourceFileName',
    // and 'symbolName'.  Where you have a function 'child' called by a parent
    // function 'parent', on the stack frame of 'child' is the address within
    // 'parent' of the first instruction to be called after 'child' returns.
    // The 'address' field is this address within 'parent', and all other
    // fields in the 'bdesu_StackTraceFrame' refer to the function 'parent'.

    // CONSTANTS
    enum {
        INVALID_LINE_NUMBER = -1,  // the 'lineNumber' attribute is initialized
                                   // to this
        INVALID_OFFSET      = -1   // the 'offset' attribute is initialized to
                                   // this, must be cast to a 'size_t' for
                                   // comparison or assignment, see
                                   // 'invalidOffset'
    };

  private:
    // DATA
    const void *d_address_p;            // address of code within the function
                                        // referred to by this object

    const char *d_libraryFileName_p;    // executable file or shared library,
                                        // if known, where the code referred to
                                        // by 'd_address_p' resides, and 0
                                        // otherwise

    int         d_lineNumber;           // the line number in the source file
                                        // corresponding to 'd_address_p', if
                                        // known, and 'INVALID_LINE_NUMBER'
                                        // otherwise

    bsl::size_t d_offsetFromSymbol;     // the offset (in bytes) from the
                                        // beginning of the function into which
                                        // 'd_address_p' points and
                                        // 'd_address_p', if known, and
                                        // 'INVALID_OFFSET' otherwise.

    const char *d_sourceFileName_p;     // the name of the source file
                                        // corresponding to the code referred
                                        // to by 'd_address_p', if known, and 0
                                        // otherwise

    const char *d_mangledSymbolName_p;  // the mangled name of the routine
                                        // where 'd_address_p' resides, if
                                        // known, and 0 otherwise

    const char *d_symbolName_p;         // the name of the routine containing
                                        // the instruction pointed at by
                                        // 'd_address_p'; may be either
                                        // mangled or demangled, or 0 if
                                        // unknown.  Note that if
                                        // 'd_mangledSymbolName_p' has a valid
                                        // value, so should 'd_symbolName_p'.

    friend bsl::ostream& operator<<(bsl::ostream&,
                                    const bdesu_StackTraceFrame&);

  public:
    // CLASS METHODS
    static
    bsl::size_t invalidOffset();
        // The value of an offset before it is set.

    // CREATORS
    bdesu_StackTraceFrame();
        // Create a 'bdesu_StackTraceFrame' object with all members set to
        // invalid values: string pointer values default to 0, the 'lineNumber'
        // value to 'INVALID_LINE_NUMBER', and the 'offsetFromSymbol' value to
        // 'INVALID_OFFSET'.

    // bdesu_StackTraceFrame(const bdesu_StackTraceFrame&);
        // compiler generated

    // bdesu_StackTraceFrame& operator=(const bdesu_StackTraceFrame&);
        // compiler generated

    // ~bdesu_StackTraceFrame();
        // compiler generated

    // MANIPULATORS
    void setAddress(const void *address);
        // Set the address attribute of this object to the specified 'address'.
        // The bahavior is undefined if 'address == 0'.

    void setLibraryFileName(const char *libraryFileNameValue);
        // Set the libraryFileName attribute of this object to the specified
        // 'libraryFileNameValue'.  The behavior is undefined if
        // 'libraryFileNameValud == 0'.  Note that if
        // '0 == *libraryFileNameValue', this method will have no effect.

    void setLineNumber(int lineNumberValue);
        // Set the lineNumber attribute of this object to the specified
        // 'lineNumberValue'.  The behavior is undefined if
        // 'lineNumberValue < 0'.

    void setMangledSymbolName(const char *mangledSymbolNameValue);
        // Set the mangledSymbolName attribute of this object to the specified
        // 'mangledSymbolNameValue'.  The behavior is undefined if
        // 'mangledSymbolNameValue == 0'.  Note that if
        // '0 == *mangledSymbolNameValue', this method will have no effect.

    void setOffsetFromSymbol(bsl::size_t offsetFromSymbolValue);
        // Set the offsetFromSymbol attribute of this object to the specified
        // 'offsetFromSymbolValue'.  The behavior is undefined if
        // 'offsetFromSymbolValue < 0'.

    void setSourceFileName(const char *sourceFileNameValue);
        // Set the sourceFileName attribute of this object to the specified
        // 'sourceFileNameValue'.  The behavior is undefined if

    void setSymbolName(const char *symbolNameValue);
        // Set the symbolName attribute of this object to the specified
        // 'symbolNameValue'.  The behavior is undefined if
        // 'symbolNameValue == 0'.  Note that if '0 == *symbolNameValue', this
        // method will have no effect.

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

    const char *libraryFileName() const;
        // Return the name of the program file or shared library referred to by
        // this object or 0 if it has not been set.

    int lineNumber() const;
        // Return the line number in the source file of the statement
        // corresponding to 'address'.

    const char *mangledSymbolName() const;
        // Return a the mangled name of the function corresponding to this
        // object or 0 if it has not been set.

    bsl::size_t offsetFromSymbol() const;
        // Return the offset of 'address' from the address of the beginning of
        // the function represented by 'symbolName'.

    const char *sourceFileName() const;
        // Return the name of the source file containing the function to which
        // this object refers or 0 if it has not been set.

    const char *symbolName() const;
        // Return a the name of the function to which this object refers, or 0
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
bool operator==(const bdesu_StackTraceFrame& lhs,
                const bdesu_StackTraceFrame& rhs);
    // Return 'true' if all corresponding attributes of the specified 'lhs' and
    // 'rhs' stack trace frame objects have the same value, and 'false'
    // otherwise.  Note that two 'const char *'s pointing to different but
    // equal strings are considered equal.

bool operator!=(const bdesu_StackTraceFrame& lhs,
                const bdesu_StackTraceFrame& rhs);
    // Return 'true' if any corresponding attributes of the specified 'lhs' and
    // 'rhs' stack trace frame objects do not have the same value, and 'false'
    // otherwise.  Note that two 'const char *'s pointing to different but
    // equal strings are considered equal.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdesu_StackTraceFrame& frame);
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
                       // class bdesu_StackTraceFrame
                       // ---------------------------

// CLASS METHODS
inline
bsl::size_t bdesu_StackTraceFrame::invalidOffset()
{
    return (bsl::size_t) INVALID_OFFSET;
}

// CREATORS
inline
bdesu_StackTraceFrame::bdesu_StackTraceFrame()
: d_address_p(0)
, d_libraryFileName_p(0)
, d_lineNumber(INVALID_LINE_NUMBER)
, d_offsetFromSymbol(invalidOffset())
, d_sourceFileName_p(0)
, d_mangledSymbolName_p(0)
, d_symbolName_p(0)
{
}

// MANIPULATORS
inline
void bdesu_StackTraceFrame::setAddress(const void *addressValue)
{
    d_address_p = addressValue;
}

inline
void bdesu_StackTraceFrame::setLibraryFileName(
                                              const char *libraryFileNameValue)
{
    d_libraryFileName_p = libraryFileNameValue && *libraryFileNameValue
                        ? libraryFileNameValue
                        : 0;
}

inline
void bdesu_StackTraceFrame::setLineNumber(int lineNumberValue)
{
    d_lineNumber = lineNumberValue;
}

inline
void bdesu_StackTraceFrame::setMangledSymbolName(
                                            const char *mangledSymbolNameValue)
{
    d_mangledSymbolName_p = mangledSymbolNameValue && *mangledSymbolNameValue
                          ? mangledSymbolNameValue
                          : 0;
}

inline
void bdesu_StackTraceFrame::setOffsetFromSymbol(
                                             bsl::size_t offsetFromSymbolValue)
{
    d_offsetFromSymbol = offsetFromSymbolValue;
}

inline
void bdesu_StackTraceFrame::setSourceFileName(const char *sourceFileNameValue)
{
    d_sourceFileName_p = sourceFileNameValue && *sourceFileNameValue
                       ? sourceFileNameValue
                       : 0;
}

inline
void bdesu_StackTraceFrame::setSymbolName(const char *symbolNameValue)
{
    d_symbolName_p = symbolNameValue && *symbolNameValue
                   ? symbolNameValue
                   : 0;
}

// ACCESSORS
inline
const void *bdesu_StackTraceFrame::address() const
{
    return d_address_p;
}

inline
bool bdesu_StackTraceFrame::isAddressValid() const
{
    return 0 != d_address_p;
}

inline
bool bdesu_StackTraceFrame::isLibraryFileNameValid() const
{
    return 0 != d_libraryFileName_p;
}

inline
bool bdesu_StackTraceFrame::isLineNumberValid() const
{
    return d_lineNumber > 0;
}

inline
bool bdesu_StackTraceFrame::isMangledSymbolNameValid() const
{
    return 0 != d_mangledSymbolName_p && 0 != *d_mangledSymbolName_p;
}

inline
bool bdesu_StackTraceFrame::isOffsetFromSymbolValid() const
{
    return invalidOffset() != d_offsetFromSymbol;
}

inline
bool bdesu_StackTraceFrame::isSourceFileNameValid() const
{
    return 0 != d_sourceFileName_p && 0 != *d_sourceFileName_p;
}

inline
bool bdesu_StackTraceFrame::isSymbolNameValid() const
{
    return 0 != d_symbolName_p && 0 != *d_symbolName_p;
}

inline
const char *bdesu_StackTraceFrame::libraryFileName() const
{
    return d_libraryFileName_p;
}

inline
int bdesu_StackTraceFrame::lineNumber() const
{
    return d_lineNumber;
}

inline
bsl::size_t bdesu_StackTraceFrame::offsetFromSymbol() const
{
    return d_offsetFromSymbol;
}

inline
const char *bdesu_StackTraceFrame::mangledSymbolName() const
{
    return d_mangledSymbolName_p;
}

inline
const char *bdesu_StackTraceFrame::sourceFileName() const
{
    return d_sourceFileName_p;
}

inline
const char *bdesu_StackTraceFrame::symbolName() const
{
    return d_symbolName_p;
}

// FREE OPERATORS
inline
bool operator!=(const bdesu_StackTraceFrame& lhs,
                const bdesu_StackTraceFrame& rhs)
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
