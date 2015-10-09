// balst_stacktrace.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACE
#define INCLUDED_BALST_STACKTRACE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description of a function-call stack.
//
//@CLASSES:
//  balst::StackTrace: a description of a function-call stack
//
//@SEE_ALSO: balst_stacktraceframe, balst_stacktraceutil,
//           balst_stacktraceprintutil, bdlma_heapbypassallocator
//
//@DESCRIPTION: This component provides a (value-semantic) container class,
// 'balst::StackTrace', that is used to describe a function-call stack.  A
// stack-trace object contains a sequence of 'balst::StackTraceFrame' objects.
// By default, a 'balst::StackTrace' object is supplied memory by an owned
// 'bdlma::HeapBypassAllocator' object, though the client may specify another
// allocator at construction to be used in its place.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Configuring a Stack-Trace Value
/// - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to create a 'balst::StackTrace' object,
// and then to both modify and access its value.
//
// First, we set up a test allocator as default allocator.  A
// 'balst::StackTrace' object, by default, gets all its memory from an owned
// 'bdlma::HeapBypassAllocator' object.  To demonstrate this default behavior
// we start by setting the default allocator to a test allocator so we can
// verify later that it was unused:
//..
//  bslma::TestAllocator         da;
//  bslma::DefaultAllocatorGuard guard(&da);
//..
// Then, we create a stack-trace object.  Note that when we don't specify an
// allocator, the default allocator is not used -- rather, a heap-bypass
// allocator owned by the stack-trace object is used.  The heap-bypass
// allocator is recommended because this component is often used to obtain
// debug information in situations where an error has occurred, and the
// possibility of heap corruption can't be ruled out.  The heap-bypass
// allocator obtains its memory directly from virtual memory rather than going
// through the heap, avoiding potential complications due to heap corruption.
//..
//  balst::StackTrace stackTrace;
//  assert(0 == stackTrace.length());
//..
// Next, we 'resize' the stack-trace object to contain two default-constructed
// frames, and take references to each of the two new frames:
//..
//  stackTrace.resize(2);
//  assert(2 == stackTrace.length());
//  balst::StackTraceFrame& frame0 = stackTrace[0];
//  balst::StackTraceFrame& frame1 = stackTrace[1];
//..
// Then, we set the values of the fields of the two new frames.
//..
//  frame0.setAddress((void *) 0x12ab);
//  frame0.setLibraryFileName("/a/b/c/balst_stacktrace.t.dbg_exc_mt");
//  frame0.setLineNumber(5);
//  frame0.setOffsetFromSymbol(116);
//  frame0.setSourceFileName("/a/b/c/sourceFile.cpp");
//  frame0.setMangledSymbolName("_woof_1a");
//  frame0.setSymbolName("woof");
//
//  frame1.setAddress((void *) 0x34cd);
//  frame1.setLibraryFileName("/lib/libd.a");
//  frame1.setLineNumber(15);
//  frame1.setOffsetFromSymbol(228);
//  frame1.setSourceFileName("/a/b/c/secondSourceFile.cpp");
//  frame1.setMangledSymbolName("_arf_1a");
//  frame1.setSymbolName("arf");
//..
// Next, we verify the frames have the values we expect:
//..
//  assert((void *) 0x12ab               == frame0.address());
//  assert("/a/b/c/balst_stacktrace.t.dbg_exc_mt"
//                                       == frame0.libraryFileName());
//  assert(  5                           == frame0.lineNumber());
//  assert(116                           == frame0.offsetFromSymbol());
//  assert("/a/b/c/sourceFile.cpp"       == frame0.sourceFileName());
//  assert("_woof_1a"                    == frame0.mangledSymbolName());
//  assert("woof"                        == frame0.symbolName());
//
//  assert((void *) 0x34cd               == frame1.address());
//  assert("/lib/libd.a"                 == frame1.libraryFileName());
//  assert( 15                           == frame1.lineNumber());
//  assert(228                           == frame1.offsetFromSymbol());
//  assert("/a/b/c/secondSourceFile.cpp" == frame1.sourceFileName());
//  assert("_arf_1a"                     == frame1.mangledSymbolName());
//  assert("arf"                         == frame1.symbolName());
//..
// Next, we output the stack-trace object:
//..
//  stackTrace.print(cout, 1, 2);
//..
// Finally, we observe the default allocator was never used.
//..
//  assert(0 == da.numAllocations());
//..
// The above usage produces the following output:
//..
//  [
//    [
//      address = 0x12ab
//      library file name = "/a/b/c/balst_stacktrace.t.dbg_exc_mt"
//      line number = 5
//      mangled symbol name = "_woof_1a"
//      offset from symbol = 116
//      source file name = "/a/b/c/sourceFile.cpp"
//      symbol name = "woof"
//    ]
//    [
//      address = 0x34cd
//      library file name = "/lib/libd.a"
//      line number = 15
//      mangled symbol name = "_arf_1a"
//      offset from symbol = 228
//      source file name = "/a/b/c/secondSourceFile.cpp"
//      symbol name = "arf"
//    ]
//  ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACEFRAME
#include <balst_stacktraceframe.h>
#endif

#ifndef INCLUDED_BDLMA_HEAPBYPASSALLOCATOR
#include <bdlma_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
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
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balst {
                              // ================
                              // class StackTrace
                              // ================

class StackTrace {
    // This value-semantic class describes a function-call stack, represented
    // as a sequence of randomly accesible 'StackTraceFrame' objects,
    // each of which represents one function call on the stack.  Note that if
    // no allocator is supplied at construction, an owned
    // 'bdlma::HeapBypassAllocator' object is used to supply memory.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    bdlma::HeapBypassAllocator    d_hbpAlloc;  // Used if no allocator is
                                              // supplied at construction.
                                              // Note this member must be
                                              // declared and constructed prior
                                              // to 'd_frames'.

    bsl::vector<StackTraceFrame>
                                 d_frames;    // sequence of stack-trace frames

    friend bool operator==(const StackTrace&,
                           const StackTrace&);

  public:
    BSLALG_DECLARE_NESTED_TRAITS2(StackTrace,
                                  bslalg::TypeTraitUsesBslmaAllocator,
                                  bslalg::TypeTraitBitwiseMoveable);

    // CREATORS
    explicit
    StackTrace(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'StackTrace' object (having a length of 0).
        // Optionally specify 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, then an owned heap-bypass allocator object is
        // used.  Note that the heap-bypass allocator is used by default to
        // avoid heap allocation in situations where the heap may have been
        // corrupted.

    StackTrace(const StackTrace&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create a 'StackTrace' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, then an owned
        // heap-bypass allocator object is used.  Note that the heap-bypass
        // allocator is used by default to avoid heap allocation in situations
        // where the heap may have been corrupted.

    //! ~StackTrace() = default;
        // Destroy this object.

    // MANIPULATORS
    StackTrace& operator=(const StackTrace& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    StackTraceFrame& operator[](int index);
        // Return a reference providing modifiable access to the stack-trace
        // frame at the specified 'index'.  The behavior is undefined unless
        // '0 <= index < length()'.

    void append(const StackTraceFrame& value);
        // Append to this sequence the specified 'value'.

    void removeAll();
        // Remove all stack-trace frames from this object.  After this
        // operation, the 'length()' method will return 0.

    void resize(int newLength);
        // Add default constructed stack-trace frames to, or remove stack-trace
        // frames from, the end of this stack-trace object such that, after the
        // operation, 'length() == newLength'.  Stack trace frames whose
        // indices are in the range '0 <= index < min(length, newLength)' will
        // be unchanged.  The behavior is undefined unless '0 <= newLength'.

                        // Aspects

    void swap(StackTrace& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const StackTraceFrame& operator[](int index) const;
        // Return a reference providing non-modifiable access to the
        // stack-trace frame at the specified 'index'.  The behavior is
        // undefined unless '0 <= index < length()'.

    int length() const;
        // Return the number of stack-trace frames contained in this object.

                        // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the owned
        // heap-bypass allocator is used.

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
bool operator==(const StackTrace& lhs, const StackTrace& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'StackTrace' objects have the
    // same value if they have the save length, and each of their corresponding
    // stack-trace frames have the same value.

bool operator!=(const StackTrace& lhs, const StackTrace& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'StackTrace' objects do
    // not have the same value if they do not have the same length, or any of
    // their corresponding stack-trace frames do not have the same value.


bsl::ostream& operator<<(bsl::ostream&           stream,
                         const StackTrace& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)', but with the attribute names elided.

// FREE FUNCTIONS
void swap(StackTrace& a, StackTrace& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ----------------
                              // class StackTrace
                              // ----------------

                            // -----------------
                            // Level 0 Functions
                            // -----------------

// ACCESSORS
inline
bslma::Allocator *StackTrace::allocator() const
{
    return d_frames.get_allocator().mechanism();
}

// CREATORS
inline
StackTrace::StackTrace(bslma::Allocator *basicAllocator)
: d_hbpAlloc()
, d_frames(basicAllocator ? basicAllocator : &d_hbpAlloc)
{
}

inline
StackTrace::StackTrace(const StackTrace&  original,
                       bslma::Allocator        *basicAllocator)
: d_hbpAlloc()
, d_frames(original.d_frames,
           basicAllocator ? basicAllocator : &d_hbpAlloc)
{
}

// MANIPULATORS
inline
StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
    d_frames = rhs.d_frames;

    return *this;
}

inline
StackTraceFrame& StackTrace::operator[](int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    BSLS_ASSERT_SAFE(index < length());

    return d_frames[index];
}

inline
void StackTrace::append(const StackTraceFrame& value)
{
    d_frames.push_back(value);
}

inline
void StackTrace::removeAll()
{
    d_frames.clear();
}

inline
void StackTrace::resize(int newLength)
{
    BSLS_ASSERT_SAFE(newLength >= 0);

    d_frames.resize(newLength);
}

inline
void StackTrace::swap(StackTrace& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_frames.swap(other.d_frames);
}

// ACCESSORS
inline
const StackTraceFrame& StackTrace::operator[](int index) const
{
    BSLS_ASSERT_SAFE(index >= 0);
    BSLS_ASSERT_SAFE(index < length());

    return d_frames[index];
}

inline
int StackTrace::length() const
{
    return (int) d_frames.size();
}

// FREE FUNCTIONS
inline
void swap(StackTrace& a, StackTrace& b)
{
    a.swap(b);
}

// FREE OPERATORS
inline
bool operator==(const StackTrace& lhs, const StackTrace& rhs)
{
    return lhs.d_frames == rhs.d_frames;
}

inline
bool operator!=(const StackTrace& lhs,
                const StackTrace& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&     stream,
                         const StackTrace& object)
{
    object.print(stream, 0, -1);

    return stream;
}

}  // close package namespace

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
