// balst_stacktraceframe.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACEFRAME
#define INCLUDED_BALST_STACKTRACEFRAME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class describing an execution stack frame.
//
//@CLASSES:
//   balst::StackTraceFrame: a description of one frame of an execution stack
//
//@SEE_ALSO: balst_stacktrace
//
//@DESCRIPTION: This component provides a single, simply-constrained
// (value-semantic) attribute class, 'balst::StackTraceFrame', that describes
// a stack frame from the execution stack of a function call.  Additional
// methods are provided to indicate whether a given attribute is considered
// "unknown".
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
//: o address: the return address in the *parent* (calling) function
//:   on return from the *child* (called) function.
//:
//: o libraryFileName: the executable or shared-library file name
//:   containing the parent function.
//:
//: o lineNumber: the source line number in the parent function corresponding,
//:   corresponding to a call to the child function.
//:
//: o mangledSymbolName: mangled symbol name of the parent function (in C code,
//:   the "mangled" name matches the symbol name.)
//:
//: o offsetFromSymbol: offset from the start of the parent function to the
//:   call to the child function.
//:
//: o sourceFileName: Name of the source file of the parent function.
//:
//: o symbolName: unmangled symbol name of the parent function (in C, code
//:   symbol name matches the mangled symbol name.)
//
///Unknown Values
///--------------
// For each attribute, a particular value is reserved to designate that the
// attribute value is "unknown".  Default constructed objects are created with
// the designated "unknown" value for each attribute.
//
///Supplementary Methods
///---------------------
// In addition to the usual setters and getters, the 'balst::StackTraceFrame'
// attribute class provides also provides a suite of non-'static',
// (boolean-valued) *predicate* methods, of the form 'is<attributeName>Known'.
// Each of these return 'true' if the object attribute named by the method does
// *not* contain the designated "unknown" value for that attribute, and 'false'
// otherwise.
//
///Usage
///-----
// In this example, we create two 'balst::StackTraceFrame' objects, modify
// their properties, and compare them.  First, we create the objects 'a' and
// 'b':
//..
//  balst::StackTraceFrame a, b;
//  assert(a == b);
//..
// Then, we verify all values are initialized by the constructor to "unknown"
// values:
//..
//  assert(false == a.isAddressKnown());
//  assert(false == a.isLibraryFileNameKnown());
//  assert(false == a.isLineNumberKnown());
//  assert(false == a.isMangledSymbolNameKnown());
//  assert(false == a.isOffsetFromSymbolKnown());
//  assert(false == a.isSourceFileNameKnown());
//  assert(false == a.isSymbolNameKnown());
//..
// Next, we assign a value to the 'lineNumber' attribute of 'a' and verify:
//..
//  a.setLineNumber(5);
//  assert(true == a.isLineNumberKnown());
//  assert(5    == a.lineNumber());
//  assert(a    != b);
//..
// Next, make the same change to 'b' and thereby restore it's equality to 'a':
//..
//  b.setLineNumber(5);
//  assert(true == b.isLineNumberKnown());
//  assert(5    == b.lineNumber());
//  assert(a    == b);
//..
// Next, we update the 'address' attribute of 'a' and use the 'address'
// accessor method to obtain the new value for the update of 'b':
//..
//  a.setAddress((char *) 0x12345678);
//  assert(a != b);
//
//  b.setAddress(a.address());
//  assert(true                == a.isAddressKnown());
//  assert(true                == b.isAddressKnown());
//  assert((char *) 0x12345678 == a.address());
//  assert((char *) 0x12345678 == b.address());
//  assert(a.address()         == b.address());
//  assert(a                   == b);
//..
// Finally, we exercise this sequence of operations for two other attributes,
// 'symbolName' and 'sourceFileName':
//..
//  a.setSymbolName("woof");
//  assert(a    != b);
//
//  b.setSymbolName(a.symbolName());
//  assert(true == a.isSymbolNameKnown());
//  assert(true == b.isSymbolNameKnown());
//  assert(0    == bsl::strcmp("woof", a.symbolName().c_str()));
//  assert(0    == bsl::strcmp("woof", b.symbolName().c_str()));
//  assert(a    == b);
//
//  a.setSourceFileName("woof.cpp");
//  assert(a != b);
//  b.setSourceFileName(a.sourceFileName());
//  assert(a.isSourceFileNameKnown());
//  assert(b.isSourceFileNameKnown());
//  assert(0 == bsl::strcmp("woof.cpp", a.sourceFileName().c_str()));
//  assert(0 == bsl::strcmp("woof.cpp", b.sourceFileName().c_str()));
//  assert(a == b);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>    // bsl::swap
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

namespace balst {
                           // =====================
                           // class StackTraceFrame
                           // =====================

class StackTraceFrame {
    // This simply constrained (value-semantic) attribute class describes a
    // call frame created on the execution stack when a *parent* function calls
    // a *child* function.  Supplementary predicate methods indicate whether a
    // given attribute value is considered "unknown".  See the Attributes
    // section under @DESCRIPTION in the component-level documentation.  Note
    // that the class invariants are identically the constraints on the
    // individual attributes.
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
                                           // if known, and -1 otherwise

    bsl::string      d_mangledSymbolName;  // mangled name of the parent
                                           // function, if known, and ""
                                           // otherwise

    bsl::size_t      d_offsetFromSymbol;   // offset (in bytes) of 'd_address'
                                           // from the start of the parent
                                           // function, if known, and
                                           // '(bsl::size_t)-1' otherwise.

    bsl::string      d_sourceFileName;     // name of the source file of the
                                           // parent function, if known, and ""
                                           // otherwise

    bsl::string      d_symbolName;         // name of the parent function, if
                                           // known, and "" otherwise

  public:
    BSLALG_DECLARE_NESTED_TRAITS(StackTraceFrame,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    StackTraceFrame(bslma::Allocator *basicAllocator = 0);
        // Create a 'StackTraceFrame' object having the (default)
        // attribute values:
        //..
        //  address            == 0
        //  libraryFileName    == ""
        //  lineNumber         == -1
        //  mangledSymbolName  == ""
        //  offsetFromSymbol   == (size_t)-1
        //  sourceFileName     == ""
        //  symbolName         == ""
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    StackTraceFrame(const void               *address,
                    const bslstl::StringRef&  libraryFileName,
                    int                       lineNumber,
                    const bslstl::StringRef&  mangledSymbolName,
                    bsl::size_t               offsetFromSymbol,
                    const bslstl::StringRef&  sourceFileName,
                    const bslstl::StringRef&  symbolName,
                    bslma::Allocator         *basicAllocator = 0);
        // Create a local time descriptor object having the specified
        // 'address', 'libraryFileName', 'lineNumber', 'mangledSymbolName',
        // 'offsetFromSymbol', 'sourceFileName', and 'symbolName' attribute
        // values.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '-1 <= lineNumber'.

    StackTraceFrame(const StackTraceFrame&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a 'StackTraceFrame' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~StackTraceFrame();
        // Destroy this object.

    // MANIPULATORS
    StackTraceFrame& operator=(const StackTraceFrame& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setAddress(const void *value);
        // Set the 'address' attribute of this object to the specified 'value'.
        // Note that the value '(void *)0)' indicates that 'address' is
        // "unknown".

    void setLibraryFileName(const bslstl::StringRef& value);
        // Set the 'libraryFileName' attribute of this object to the specified
        // 'value'.  Note that the empty string indicates the 'libraryFileName'
        // is "unknown".

    void setLineNumber(int value);
        // Set the 'lineNumber' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '-1 <= value'.  Note that
        // the value -1 indicates the 'lineNumber' is "unknown".

    void setMangledSymbolName(const bslstl::StringRef& value);
        // Set the 'mangledSymbolName' attribute of this object to the
        // specified 'value'.  Note that the empty string indicates that the
        // 'mangledSymbolName' is "unknown".

    void setOffsetFromSymbol(bsl::size_t value);
        // Set the 'offsetFromSymbol' attribute of this object to the specified
        // 'value'.  Note that the value '(bsl::size_t)-1' indicates that
        // 'offsetFromSymbol' is "unknown".

    void setSourceFileName(const bslstl::StringRef& value);
        // Set the 'sourceFileName' attribute of this object to the specified
        // 'value'.  Note that the empty string indicates that 'sourceFileName'
        // is "unknown".

    void setSymbolName(const bslstl::StringRef& value);
        // Set the 'symbolName' attribute of this object to the specified
        // 'value'.  Note that the empty string indicates that 'symbolName' is
        // "unknown".

                        // Aspects

    void swap(StackTraceFrame& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const void *address() const;
        // Return the value of 'address' attribute of this object.  Note that
        // '(void *)0' indicates that the 'address' is "unknown".

    const bsl::string& libraryFileName() const;
        // Return a reference providing non-modifiable access to the
        // 'libraryFileName' attribute of this object.  Note the empty string
        // indicates that the 'libraryFileName' is "unknown".

    int lineNumber() const;
        // Return the value of the 'lineNumber' attribute of this object.  Note
        // that -1 indicates that 'lineNumber' is "unknown".

    const bsl::string& mangledSymbolName() const;
        // Return a reference providing non-modifiable access to the
        // 'mangledSymbolName' attribute of this object.  Note that the empty
        // string indicates that 'mangledSymbolName' is "unknown".

    bsl::size_t offsetFromSymbol() const;
        // Return the value of the 'offsetFromSymbol' attribute of this object.
        // Note that '(bsl::size_t)-1' indicates that 'lineNumber' is not
        // known.

    const bsl::string& sourceFileName() const;
        // Return a reference providing non-modifiable access to the
        // 'sourceFileName' attribute of this object.  Note that the empty
        // string indicates that 'sourceFileName' is "unknown".

    const bsl::string& symbolName() const;
        // Return a reference providing non-modifiable access to the
        // 'symbolName' attribute of this object.  Note that the empty string
        // indicates that 'symbolName' is "unknown".

                        // Predicates

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

                        // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

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
bool operator==(const StackTraceFrame& lhs, const StackTraceFrame& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'StackTraceFrame' objects
    // have the same value if the corresponding values of their 'address',
    // 'libraryFileName', 'lineNumber', 'mangledSymbolName',
    // 'offsetFromSymbol', 'sourceFileName', and 'symbolName' attributes are
    // the same.

bool operator!=(const StackTraceFrame& lhs, const StackTraceFrame& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'StackTraceFrame'
    // objects do not have the same value if the corresponding values
    // of their 'address', 'libraryFileName', 'lineNumber',
    // 'mangledSymbolName', 'offsetFromSymbol', 'sourceFileName', or
    // 'symbolName' attributes are the not same.

bsl::ostream& operator<<(bsl::ostream& stream, const StackTraceFrame& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)', but with the attribute names elided.

// FREE FUNCTIONS
void swap(StackTraceFrame& a, StackTraceFrame& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class StackTraceFrame
                           // ---------------------

// CREATORS
inline
StackTraceFrame::StackTraceFrame(bslma::Allocator *basicAllocator)
: d_address(0)
, d_libraryFileName(basicAllocator)
, d_lineNumber(-1)
, d_mangledSymbolName(basicAllocator)
, d_offsetFromSymbol((bsl::size_t)-1)
, d_sourceFileName(basicAllocator)
, d_symbolName(basicAllocator)
{
}

inline
StackTraceFrame::StackTraceFrame(const void               *address,
                                 const bslstl::StringRef&  libraryFileName,
                                 int                       lineNumber,
                                 const bslstl::StringRef&  mangledSymbolName,
                                 bsl::size_t               offsetFromSymbol,
                                 const bslstl::StringRef&  sourceFileName,
                                 const bslstl::StringRef&  symbolName,
                                 bslma::Allocator         *basicAllocator)
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
{
    BSLS_ASSERT_SAFE(0 != libraryFileName.data());
    BSLS_ASSERT_SAFE(-1 <= lineNumber);
    BSLS_ASSERT_SAFE(0 != mangledSymbolName.data());
    BSLS_ASSERT_SAFE(0 != sourceFileName.data());
    BSLS_ASSERT_SAFE(0 != symbolName.data());
}

inline
StackTraceFrame::StackTraceFrame(const StackTraceFrame&  original,
                                 bslma::Allocator       *basicAllocator)
: d_address(original.d_address)
, d_libraryFileName(original.d_libraryFileName, basicAllocator)
, d_lineNumber(original.d_lineNumber)
, d_mangledSymbolName(original.d_mangledSymbolName, basicAllocator)
, d_offsetFromSymbol(original.d_offsetFromSymbol)
, d_sourceFileName(original.d_sourceFileName, basicAllocator)
, d_symbolName(original.d_symbolName, basicAllocator)
{
}

inline
StackTraceFrame::~StackTraceFrame()
{
    BSLS_ASSERT_SAFE(-1 <= d_lineNumber);
}

// MANIPULATORS
inline
StackTraceFrame& StackTraceFrame::operator=(const StackTraceFrame& rhs)
{
    if (rhs == *this) {
        return *this;                                                 // RETURN
    }

    bslma::Allocator *allocator_p = d_symbolName.get_allocator().mechanism();
    StackTraceFrame(rhs, allocator_p).swap(*this);
    return *this;
}

inline
void StackTraceFrame::setAddress(const void *value)
{
    d_address = value;
}

inline
void StackTraceFrame::setLibraryFileName(const bslstl::StringRef& value)
{
    BSLS_ASSERT_SAFE(0 != value.data());

    d_libraryFileName.assign(value.begin(), value.end());
}

inline
void StackTraceFrame::setLineNumber(int value)
{
    BSLS_ASSERT_SAFE(-1 <= value);

    d_lineNumber = value;
}

inline
void StackTraceFrame::setMangledSymbolName(const bslstl::StringRef& value)
{
    BSLS_ASSERT_SAFE(0 != value.data());

    d_mangledSymbolName.assign(value.begin(), value.end());
}

inline
void StackTraceFrame::setOffsetFromSymbol(bsl::size_t value)
{
    d_offsetFromSymbol = value;
}

inline
void StackTraceFrame::setSourceFileName(const bslstl::StringRef& value)
{
    BSLS_ASSERT_SAFE(0 != value.data());

    d_sourceFileName.assign(value.begin(), value.end());
}

inline
void StackTraceFrame::setSymbolName(const bslstl::StringRef& value)
{
    BSLS_ASSERT_SAFE(0 != value.data());

    d_symbolName.assign(value.begin(), value.end());
}

inline
void StackTraceFrame::swap(StackTraceFrame& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    using bsl::swap;
    swap(d_address,           other.d_address);
    swap(d_libraryFileName,   other.d_libraryFileName);
    swap(d_lineNumber,        other.d_lineNumber);
    swap(d_mangledSymbolName, other.d_mangledSymbolName);
    swap(d_offsetFromSymbol,  other.d_offsetFromSymbol);
    swap(d_sourceFileName,    other.d_sourceFileName);
    swap(d_symbolName,        other.d_symbolName);
}

// ACCESSORS
inline
const void *StackTraceFrame::address() const
{
    return d_address;
}

inline
const bsl::string& StackTraceFrame::libraryFileName() const
{
    return d_libraryFileName;
}

inline
int StackTraceFrame::lineNumber() const
{
    return d_lineNumber;
}

inline
bsl::size_t StackTraceFrame::offsetFromSymbol() const
{
    return d_offsetFromSymbol;
}

inline
const bsl::string& StackTraceFrame::mangledSymbolName() const
{
    return d_mangledSymbolName;
}

inline
const bsl::string& StackTraceFrame::sourceFileName() const
{
    return d_sourceFileName;
}

inline
const bsl::string& StackTraceFrame::symbolName() const
{
    return d_symbolName;
}

                        // Predicates
inline
bool StackTraceFrame::isAddressKnown() const
{
    return 0 != d_address;
}

inline
bool StackTraceFrame::isLibraryFileNameKnown() const
{
    return !d_libraryFileName.empty();
}

inline
bool StackTraceFrame::isLineNumberKnown() const
{
    return d_lineNumber > 0;
}

inline
bool StackTraceFrame::isMangledSymbolNameKnown() const
{
    return !d_mangledSymbolName.empty();
}

inline
bool StackTraceFrame::isOffsetFromSymbolKnown() const
{
    return (bsl::size_t)-1 != d_offsetFromSymbol;
}

inline
bool StackTraceFrame::isSourceFileNameKnown() const
{
    return !d_sourceFileName.empty();
}

inline
bool StackTraceFrame::isSymbolNameKnown() const
{
    return !d_symbolName.empty();
}

                        // Aspects

inline
bslma::Allocator *StackTraceFrame::allocator() const
{
    return d_symbolName.get_allocator().mechanism();
}

// FREE FUNCTIONS
inline
void swap(StackTraceFrame& a, StackTraceFrame& b)
{
    a.swap(b);
}

}  // close package namespace

// FREE OPERATORS
inline
bool balst::operator==(const StackTraceFrame& lhs, const StackTraceFrame& rhs)
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
bool balst::operator!=(const StackTraceFrame& lhs, const StackTraceFrame& rhs)
{
    return lhs.address()           != rhs.address()
        || lhs.libraryFileName()   != rhs.libraryFileName()
        || lhs.lineNumber()        != rhs.lineNumber()
        || lhs.mangledSymbolName() != rhs.mangledSymbolName()
        || lhs.offsetFromSymbol()  != rhs.offsetFromSymbol()
        || lhs.sourceFileName()    != rhs.sourceFileName()
        || lhs.symbolName()        != rhs.symbolName();
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
