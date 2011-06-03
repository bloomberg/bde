// baesu_stacktraceframe.h                                            -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#define INCLUDED_BAESU_STACKTRACEFRAME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class describing an execution stack frame
//
//@CLASSES:
//   baesu_StackTraceFrame: a description of one frame of an execution stack
//
//@AUTHOR: Bill Chapman (bchapman2), Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: baesu_stacktrace
//
//@DESCRIPTION: This component provides a single, simply-constrained
// (value-semantic) attribute class, 'baesu_StackTraceFrame', that describes
// the execution stack frame of a function call.
//
///Attributes
///----------
//..
//  Name               Type          Default     Constraint
//  -----------------  -----------   ----------  ----------
//  address            const void *  0           none
//  libraryFileName    bsl::string   ""          none
//  lineNumber         int           -1          'lineNumber >= -1'
//  mangledSymbolName  bsl::string   ""          none
//  offsetFromSymbol   bsl::size_t   (size_t)-1  none
//  sourceFileName     bsl::string   ""          none
//  symbolName         bsl::string   ""          none
//..
//: o address: the return address in the "parent" function (the calling
//:   function) on return from the "child" function (the called function).
//:
//: o libraryFileName: the executable file name or shared library file name
//:
//: o lineNumber: the source line number in the parent function corresponding
//:   to call to the child function.
//:
//: o mangledSymbolName: mangled symbol name of the parent function.  In C code
//:   the "mangled" name matches the symbol name.
//:
//: o offsetFromSymbol: offset from the start of the parent function to the
//:   call to the child function.
//:
//: o sourceFileName: source file of the parent function.
//:
//: o symbolName: unmangled symbol name of the parent function.  In C code
//:   symbol name matches the mangled symbol name.
//
///Unknown Values
///--------------
// For each attribute, a particular value is reserved to designate that the
// attribute value is "unknown".  Default constructed objects are created with
// the designated "unknown" value for each attribute.
//
// This class provides a suite of non-'static', boolean methods,
// 'is<attributeName>Known', which return 'true' if the object attribute named
// by the method does *not* contain the designated "unknown" value of that
// attribute.
//
///Usage
///-----
// In this example, we create two 'baesu_StackTraceFrame' objects, modify their
// properties, and compare them.  First, we create the objects 'a' and 'b':
//..
//  baesu_StackTraceFrame a, b;
//
//  assert(a == b);
//..
// Next, we verify all values are initialized by the constructor to invalid
// values.
//..
//  assert(!a.isAddressKnown());
//  assert(!a.isLibraryFileNameKnown());
//  assert(!a.isLineNumberKnown());
//  assert(!a.isMangledSymbolNameKnown());
//  assert(!a.isOffsetFromSymbolKnown());
//  assert(!a.isSourceFileNameKnown());
//  assert(!a.isSymbolNameKnown());
//
//  a.setLineNumber(5);
//  assert(a.isLineNumberKnown());
//
//  assert(a != b);
//
//  b.setLineNumber(5);
//  assert(b.isLineNumberKnown());
//
//  assert(a == b);
//
//  a.setAddress((char *) 0x12345678);
//  b.setAddress(a.address());
//  assert(a.isAddressKnown());
//  assert(b.isAddressKnown());
//
//  a.setSymbolName("woof");
//  b.setSymbolName(a.symbolName());
//
//  a.setSourceFileName("woof.cpp");
//  b.setSourceFileName(a.sourceFileName());
//
//  assert(a == b);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
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
    // This simply constrained (value-semantic) attribute class describes a the
    // call frame created on the execution stack when a "parent" function calls
    // a "child" function.  See the Attributes section under @DESCRIPTION in
    // the component-level documentation.  Note that the class invariants are
    // identically the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  private:
    // DATA
    const void      *d_address;            // return address in the parent
                                           // on completion of the child
                                           // function, if known, and 0
                                           // otherwise

    bsl::string      d_libraryFileName;    // name of the executable file or
                                           // shared library, where the code
                                           // referred to by 'd_address'
                                           // resides, if known, and ""
                                           // otherwise

    int              d_lineNumber;         // line number in the source file
                                           // corresponding to 'd_address',
                                           // if known, and 0 otherwise

    bsl::string      d_mangledSymbolName;  // mangled name of the parent
                                           // function, if known, and ""
                                           // otherwise

    bsl::size_t      d_offsetFromSymbol;  // offset (in bytes) of 'd_address'
                                          // from the start of the parent
                                          // function, if known, and
                                          // (bsl::size_t)-1 otherwise.

    bsl::string      d_sourceFileName;    // name of the source file of the
                                          // parent function, if known, and ""
                                          // otherwise

    bsl::string      d_symbolName;        // name of the parent function, if
                                          // known, and "" otherwise

    bslma_Allocator *d_allocator_p;       // allocator used to supply memory
                                          // (held, not owned)

  public:
    BSLALG_DECLARE_NESTED_TRAITS(baesu_StackTraceFrame,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    baesu_StackTraceFrame(bslma_Allocator *basicAllocator = 0);
        // Create a 'baesu_StackTraceFrame' object having the (default)
        // attribute values:
        //: o 'address            == 0'
        //: o 'libraryFileName    == ""'
        //: o 'lineNumber         == -1'
        //: o 'mangledSymbolName  == ""'
        //: o 'offsetFromSymbol   == (size_t)-1'
        //: o 'sourceFileName     == ""'
        //: o 'symbolName         == ""'
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baesu_StackTraceFrame(const void             *address,
                          const bdeut_StringRef&  libraryFileName,
                          int                     lineNumber,
                          const bdeut_StringRef&  mangledSymbolName,
                          bsl::size_t             offsetFromSymbol,
                          const bdeut_StringRef&  sourceFileName,
                          const bdeut_StringRef&  symbolName,
                          bslma_Allocator        *basicAllocator = 0);
        // Create a local time descriptor object having the specified
        // 'address', 'libraryFileName', 'lineNumber', 'mangledSymbolName',
        // 'offsetFromSymbol', 'sourceFileName', and 'symbolName' attribute
        // values.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '-1 <= lineNumber'.

    baesu_StackTraceFrame(const baesu_StackTraceFrame&  original,
                          bslma_Allocator              *basicAllocator = 0);
        // Create a 'baesu_StackTraceFrame' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baesu_StackTraceFrame();
        // Destroy this object.
#endif

    // MANIPULATORS
    baesu_StackTraceFrame& operator=(const baesu_StackTraceFrame& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setAddress(const void *value);
        // Set the address attribute of this object to the specified 'value'.
        // Note that the value 0 indicates that 'address' is not known.

    void setLibraryFileName(const bdeut_StringRef& value);
        // Set the libraryFileName attribute of this object to the specified
        // 'value'.  Note that the empty string indicates the 'libraryFileName'
        // is not known.

    void setLineNumber(int value);
        // Set the lineNumber attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '-1 <= value'.  Note that
        // the value -1 indicates the 'lineNumber' is not known.

    void setMangledSymbolName(const bdeut_StringRef& value);
        // Set the mangledSymbolName attribute of this object to the specified
        // 'value'.  Note that the empty string indicates that the
        // 'mangledSymbolName' is not known.

    void setOffsetFromSymbol(bsl::size_t value);
        // Set the offsetFromSymbol attribute of this object to the specified
        // 'value'.  Note that the value '(bsl::size_t)-1' indicates that
        // 'offsetFromSymbol' is not known.

    void setSourceFileName(const bdeut_StringRef& value);
        // Set the sourceFileName attribute of this object to the specified
        // 'value'.  Note that the empty string indicates that 'sourceFileName'
        // is not known.

    void setSymbolName(const bdeut_StringRef& value);
        // Set the symbolName attribute of this object to the specified
        // 'value'.  Note that the empty string indicates that 'symbolName' is
        // not known.

                        // Aspects

    void swap(baesu_StackTraceFrame& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bool isAddressKnown() const;
        // Return 'true' if the 'setAddress' attribute of this object is not
        // the "unknown" value for that attribute, and 'false' otherwise.

    bool isLibraryFileNameKnown() const;
        // Return 'true' if the 'libraryFileName' attribute of this object is
        // not the "unknown" value for that attribute, and 'false' otherwise.

    bool isLineNumberKnown() const;
        // Return 'true' if the 'lineNumber' attribute of this object is not
        // the "unknown" value for that attribute, and 'false' otherwise.

    bool isMangledSymbolNameKnown() const;
        // Return 'true' if the 'mangledSymbolName' attribute of this object is
        // not the "unknown" value for that attribute, and 'false' otherwise.

    bool isOffsetFromSymbolKnown() const;
        // Return 'true' if the 'offsetFromSymbol' attribute of this object is
        // not the "unknown" value for that attribute, and 'false' otherwise.

    bool isSourceFileNameKnown() const;
        // Return 'true' if the 'sourceFileName' attribute of this object is
        // not the "unknown" value for that attribute, and 'false' otherwise.

    bool isSymbolNameKnown() const;
        // Return 'true' if the 'symbolName' attribute of this object is not
        // the "unknown" value for that attribute, and 'false' otherwise.

    const void *address() const;
        // Return the value of 'address' attribute of this object.  Note that 0
        // indicates that the 'address' is not known.

    const bsl::string& libraryFileName() const;
        // Return a reference providing non-modifiable access to the
        // 'libraryFileName' attribute of this object.  Note the empty string
        // indicates that the 'libraryFileName' is not known.

    int lineNumber() const;
        // Return the value of the 'lineNumber' attribute of this object.  Note
        // that 0 indicates that 'lineNumber' is not known.

    const bsl::string& mangledSymbolName() const;
        // Return a reference providing non-modifiable access to the
        // 'mangledSymbolName' attribute of this object.  Note that the empty
        // string indicates that 'mangledSymbolName' is not known.

    bsl::size_t offsetFromSymbol() const;
        // Return the value of the 'offsetFromSymbol' attribute of this object.
        // Note that '(bsl::size_t)-1' indicates that 'lineNumber' is not
        // known.

    const bsl::string& sourceFileName() const;
        // Return a reference providing non-modifiable access to the
        // 'sourceFileName' attribute of this object.  Note that the empty
        // string indicates that 'sourceFileName' is not known.

    const bsl::string& symbolName() const;
        // Return a reference providing non-modifiable access to the
        // 'symbolName' attribute of this object.  Note that the empty string
        // indicates that 'symbolName' is not known.

                        // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_LocalTimeDescriptor' objects
    // have the same value if the corresponding values of their 'address',
    // 'libraryFileName', 'lineNumber', 'mangledSymbolName',
    // 'offsetFromSymbol', 'sourceFileName', and 'symbolName' attributes are
    // the same.

bool operator!=(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_LocalTimeDescriptor'
    // objects do not have the same value if the corresponding values of values
    // of their 'address', 'libraryFileName', 'lineNumber',
    // 'mangledSymbolName', 'offsetFromSymbol', 'sourceFileName', or
    // 'symbolName' attributes are the not same.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baesu_StackTraceFrame& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)' with the attribute names elided.

// FREE FUNCTIONS
void swap(baesu_StackTraceFrame& a, baesu_StackTraceFrame& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ===========================================================================
//                       INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ---------------------------
                       // class baesu_StackTraceFrame
                       // ---------------------------

// CREATORS
inline
baesu_StackTraceFrame::baesu_StackTraceFrame(bslma_Allocator *basicAllocator)
: d_address(0)
, d_libraryFileName(basicAllocator)
, d_lineNumber(-1)
, d_mangledSymbolName(basicAllocator)
, d_offsetFromSymbol((bsl::size_t)-1)
, d_sourceFileName(basicAllocator)
, d_symbolName(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
baesu_StackTraceFrame::baesu_StackTraceFrame(
                                   const void             *address,
                                   const bdeut_StringRef&  libraryFileName,
                                   int                     lineNumber,
                                   const bdeut_StringRef&  mangledSymbolName,
                                   bsl::size_t             offsetFromSymbol,
                                   const bdeut_StringRef&  sourceFileName,
                                   const bdeut_StringRef&  symbolName,
                                   bslma_Allocator        *basicAllocator)
: d_address(address)
, d_libraryFileName(libraryFileName.begin(),
                    libraryFileName.end(),
                    basicAllocator)
, d_lineNumber(lineNumber)
, d_mangledSymbolName(mangledSymbolName.begin(),
                      mangledSymbolName.end(),
                      basicAllocator)
, d_offsetFromSymbol(offsetFromSymbol)
, d_sourceFileName(sourceFileName.begin(),
                   sourceFileName.end(),
                   basicAllocator)
, d_symbolName(symbolName.begin(), symbolName.end(), basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT_SAFE(libraryFileName.isBound());
    BSLS_ASSERT_SAFE(-1 <= lineNumber);
    BSLS_ASSERT_SAFE(mangledSymbolName.isBound());
    BSLS_ASSERT_SAFE(sourceFileName.isBound());
    BSLS_ASSERT_SAFE(symbolName.isBound());
}


inline
baesu_StackTraceFrame::baesu_StackTraceFrame(
                                  const baesu_StackTraceFrame&  original,
                                  bslma_Allocator              *basicAllocator)
: d_address(original.d_address)
, d_libraryFileName(original.d_libraryFileName, basicAllocator)
, d_lineNumber(original.d_lineNumber)
, d_mangledSymbolName(original.d_mangledSymbolName, basicAllocator)
, d_offsetFromSymbol(original.d_offsetFromSymbol)
, d_sourceFileName(original.d_sourceFileName, basicAllocator)
, d_symbolName(original.d_symbolName, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baesu_StackTraceFrame::~baesu_StackTraceFrame()
{
    BSLS_ASSERT_SAFE(d_lineNumber >= -1);
}
#endif

// MANIPULATORS
inline
baesu_StackTraceFrame& baesu_StackTraceFrame::operator=(
                                            const baesu_StackTraceFrame& rhs)
{
    if (rhs == *this) {
        return *this;                                                 // RETURN
    }

    baesu_StackTraceFrame(rhs, d_allocator_p).swap(*this);
    return *this;
}

inline
void baesu_StackTraceFrame::setAddress(const void *value)
{
    d_address = value;
}

inline
void baesu_StackTraceFrame::setLibraryFileName(const bdeut_StringRef& value)
{
    BSLS_ASSERT_SAFE(value.isBound());

    d_libraryFileName.assign(value.begin(), value.end());
}

inline
void baesu_StackTraceFrame::setLineNumber(int value)
{
    BSLS_ASSERT_SAFE(value >= -1);

    d_lineNumber = value;
}

inline
void baesu_StackTraceFrame::setMangledSymbolName(const bdeut_StringRef& value)
{
    BSLS_ASSERT_SAFE(value.isBound());

    d_mangledSymbolName.assign(value.begin(), value.end());
}


inline
void baesu_StackTraceFrame::setOffsetFromSymbol(bsl::size_t value)
{
    d_offsetFromSymbol = value;
}

inline
void baesu_StackTraceFrame::setSourceFileName(const bdeut_StringRef& value)
{
    BSLS_ASSERT_SAFE(value.isBound());

    d_sourceFileName.assign(value.begin(), value.end());
}

inline
void baesu_StackTraceFrame::setSymbolName(const bdeut_StringRef& value)
{
    BSLS_ASSERT_SAFE(value.isBound());

    d_symbolName.assign(value.begin(), value.end());
}

inline
void baesu_StackTraceFrame::swap(baesu_StackTraceFrame& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bsl::swap(d_address,           other.d_address);
    bsl::swap(d_libraryFileName,   other.d_libraryFileName);
    bsl::swap(d_lineNumber,        other.d_lineNumber);
    bsl::swap(d_mangledSymbolName, other.d_mangledSymbolName);
    bsl::swap(d_offsetFromSymbol,  other.d_offsetFromSymbol);
    bsl::swap(d_sourceFileName,    other.d_sourceFileName);
    bsl::swap(d_symbolName,        other.d_symbolName);
}

// ACCESSORS
inline
bslma_Allocator *baesu_StackTraceFrame::allocator() const
{
    return d_allocator_p;
}

inline
const void *baesu_StackTraceFrame::address() const
{
    return d_address;
}

inline
bool baesu_StackTraceFrame::isAddressKnown() const
{
    return 0 != d_address;
}

inline
bool baesu_StackTraceFrame::isLibraryFileNameKnown() const
{
    return !d_libraryFileName.empty();
}

inline
bool baesu_StackTraceFrame::isLineNumberKnown() const
{
    return d_lineNumber > 0;
}

inline
bool baesu_StackTraceFrame::isMangledSymbolNameKnown() const
{
    return !d_mangledSymbolName.empty();
}

inline
bool baesu_StackTraceFrame::isOffsetFromSymbolKnown() const
{
    return (bsl::size_t)-1 != d_offsetFromSymbol;
}

inline
bool baesu_StackTraceFrame::isSourceFileNameKnown() const
{
    return !d_sourceFileName.empty();
}

inline
bool baesu_StackTraceFrame::isSymbolNameKnown() const
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
    return lhs.address()           == rhs.address()
        && lhs.libraryFileName()   == rhs.libraryFileName()
        && lhs.lineNumber()        == rhs.lineNumber()
        && lhs.mangledSymbolName() == rhs.mangledSymbolName()
        && lhs.offsetFromSymbol()  == rhs.offsetFromSymbol()
        && lhs.sourceFileName()    == rhs.sourceFileName()
        && lhs.symbolName()        == rhs.symbolName();
}

inline
bool operator!=(const baesu_StackTraceFrame& lhs,
                const baesu_StackTraceFrame& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
inline
void swap(baesu_StackTraceFrame& a, baesu_StackTraceFrame& b)
{
    a.swap(b);
}

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
