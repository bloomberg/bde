// baesu_stacktrace.h                                                 -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACE
#define INCLUDED_BAESU_STACKTRACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a description of a function call stack
//
//@CLASSES:
//  baesu_StackTrace: description of a function call stack
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: baesu_stacktraceframe, baesu_stacktraceutil,
//           baesu_stacktraceprintutil, bdema_heapbypassallocator
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// class, 'baesu_StackTrace', that is used to describe a function call-stack.
// A stack trace object contains a sequence of 'baesu_StackTraceFrame' objects.
// By default, a 'baesu_StackTrace' object is supplied memory by an owned
// instance of 'bdema_HeapBypassAllocator', though the client may specify
// another allocator to be used in its place at construction.
//
///Usage
///-----
// In the following example we demonstrate how to create a 'baesu_stacktrace'
// object, then both modify and access its value.
//
// A 'baesu_StackTrace' object, by default, gets its memory from an owned
// instance of 'bdema_HeapBypassAllocator'.  To demonstrate this we start by
// setting the default allocator to a test allocator so we can verify that it
// is unused.
//..
//  bslma_TestAllocator defaultAllocator;
//  bslma_DefaultAllocatorGuard guard(&defaultAllocator);
//..
// Next, we create a stack trace object.  Note that when we don't specify an
// allocator (recommended), the default allocator is not used -- rather, a heap
// bypass allocator owned by the stack trace object is used.
//..
//  baesu_StackTrace stackTrace;
//  assert(0 == stackTrace.length());
//..
// Then we create two stack trace frames in the stack trace object and obtain
// references to each of the two new frames.
//..
//  stackTrace.resize(2);
//  assert(2 == stackTrace.length());
//  baesu_StackTraceFrame& mF0 = stackTrace[0];
//  baesu_StackTraceFrame& mF1 = stackTrace[1];
//..
// Next we set values to the two frames.
//..
//  mF0.setAddress((void *) 0x12ab);
//  mF0.setLibraryFileName("/a/b/c/baesu_stacktrace.t.dbg_exc_mt");
//  mF0.setLineNumber(5);
//  mF0.setOffsetFromSymbol(116);
//  mF0.setSourceFileName("/a/b/c/sourceFile.cpp");
//  mF0.setMangledSymbolName("_woof_1a");
//  mF0.setSymbolName("woof");
//
//  mF1.setAddress((void *) 0x34cd);
//  mF1.setLibraryFileName("/lib/libd.a");
//  mF1.setLineNumber(15);
//  mF1.setOffsetFromSymbol(228);
//  mF1.setSourceFileName("/a/b/c/secondSourceFile.cpp");
//  mF1.setMangledSymbolName("_arf_1a");
//  mF1.setSymbolName("arf");
//..
// Next we print the stack trace object.
//..
//  stackTrace.print(cout, 1, 2);
//..
// Finally, we observe the default allocator was never used
//..
//  assert(0 == defaultAllocator.numAllocations());
//..
///Usage Output
///------------
// The above usage produces the following output:
//..
//  [
//    [
//      address = 0x12ab
//      library file name = "/a/b/c/baesu_stacktrace.t.dbg_exc_mt"
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#include <baesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BDEMA_HEAPBYPASSALLOCATOR
#include <bdema_heapbypassallocator.h>
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                          // ======================
                          // class baesu_StackTrace
                          // ======================

class baesu_StackTrace {
    // This unconstrained (value-semantic) class describes a function call
    // stack, represented as a sequence randomly-accesible
    // 'baesu_StackTraceFrame' objects, each of which represents one function
    // call on the stack.  Note that if no allocator is supplied at
    // construction (recommended), an owned instance of
    // 'bdema_HeapBypassAllocator' is used to supply memory.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    bdema_HeapBypassAllocator    d_hbpAlloc;     // only used if no alloc
                                                 // specified at construction.
                                                 // Note this must be declared
                                                 // and constructed prior to
                                                 // 'd_frames'.

    bsl::vector<baesu_StackTraceFrame>
                                 d_frames;       // sequence of stack trace
                                                 // frames

  public:
    BSLALG_DECLARE_NESTED_TRAITS2(baesu_StackTrace,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    explicit
    baesu_StackTrace(bslma_Allocator *basicAllocator = 0);
        // Create a 'baesu_StackTrace' object of 0 length.  Optionally specify
        // 'basicAllocator'.  If 'basicAllocator' is not specified, than an
        // owned instance of the heap-bypass allocator is used.  Note that the
        // heap bypass allocator is used by default to avoid heap allocation in
        // case the heap has been corrupted.

    baesu_StackTrace(const baesu_StackTrace&  original,
                     bslma_Allocator         *allocator = 0);
        // Create a 'baesu_StackTrace' object having the same value as the
        // specified 'original' object.  Optionally specify 'basicAllocator'.
        // If 'basicAllocator' is not specified, than an owned instance of the
        // heap-bypass allocator is used.  Note that the heap bypass allocator
        // is used by default to avoid heap allocation in case the heap has
        // been corrupted.

    // ~baesu_StackTrace();
        // Destroy this object.  Note that this destructor is
        // compiler-generated.

    // MANIPULATORS
    baesu_StackTrace& operator=(const baesu_StackTrace& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    baesu_StackTraceFrame& operator[](int index);
        // Return a reference providing modifiable access to the stack trace
        // frame at the specified 'index' in this object.  The behavior is
        // undefined unless '0 <= index < length()'.

    void removeAll();
        // Remove all stack trace frames from this object.  After this
        // operation, 'length' will return 0.

    void resize(int newLength);
        // Add default constructed stack trace frames to or remove stack trace
        // frames from the end of this stack trace object such that, after the
        // operation, 'length() == newLength'.  Stack trace frames whose
        // indices are in the range '0 <= index < min(length, newLength) will
        // be unchanged.  The behavior is undefined unless '0 <= newLength'.

    void swap(baesu_StackTrace& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const baesu_StackTraceFrame& operator[](int index) const;
        // Return a reference providing non-modifiable access to the stack
        // trace frame at the specified 'index' in this object.  The behavior
        // is undefined unless '0 <= index < length()'.

    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the owned heap
        // bypass allocator is used.

    int length() const;
        // Return the number of stack trace frames contained in this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baesu_StackTrace' objects have the
    // same value if they have the save length, and all of their corresponding
    // stack trace frames have the same value.

bool operator!=(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baesu_StackTrace' objects do
    // not have the same value if they do not have the same length, or any of
    // the stack trace frames contained in 'lhs' is not the same as the
    // corresponding stack trace frame in 'rhs'.

bsl::ostream& operator<<(bsl::ostream&           stream,
                         const baesu_StackTrace& object);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified
    // and can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)' with the attribute names
    // elided.

// FREE FUNCTIONS
void swap(baesu_StackTrace& a, baesu_StackTrace& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // ----------------------
                         // class baesu_StackTrace
                         // ----------------------

// CREATORS
inline
baesu_StackTrace::baesu_StackTrace(bslma_Allocator *basicAllocator)
: d_hbpAlloc()
, d_frames(basicAllocator ? basicAllocator : &d_hbpAlloc)
{
}

inline
baesu_StackTrace::baesu_StackTrace(const baesu_StackTrace&  original,
                                   bslma_Allocator         *basicAllocator)
: d_hbpAlloc()
, d_frames(original.d_frames,
           basicAllocator ? basicAllocator : &d_hbpAlloc)
{
}

// MANIPULATORS
inline
baesu_StackTrace& baesu_StackTrace::operator=(const baesu_StackTrace& rhs)
{
    d_frames = rhs.d_frames;

    return *this;
}

inline
baesu_StackTraceFrame& baesu_StackTrace::operator[](int index)
{
    BSLS_ASSERT_SAFE(index >= 0);
    BSLS_ASSERT_SAFE(index < length());

    return d_frames[index];
}

inline
void baesu_StackTrace::removeAll()
{
    d_frames.clear();
}

inline
void baesu_StackTrace::resize(int newLength)
{
    BSLS_ASSERT_SAFE(newLength >= 0);

    d_frames.resize(newLength);
}

inline
void baesu_StackTrace::swap(baesu_StackTrace& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_frames.swap(other.d_frames);
}

// ACCESSORS
inline
const baesu_StackTraceFrame& baesu_StackTrace::operator[](int index) const
{
    BSLS_ASSERT_SAFE(index >= 0);
    BSLS_ASSERT_SAFE(index < length());

    return d_frames[index];
}

inline
bslma_Allocator *baesu_StackTrace::allocator() const
{
    return d_frames.get_allocator().mechanism();
}

inline
int baesu_StackTrace::length() const
{
    return d_frames.size();
}

// FREE OPERATORS
inline
bool operator!=(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const baesu_StackTrace& object)
{
    object.print(stream, 0, -1);

    return stream;
}

// FREE FUNCTIONS
inline
void swap(baesu_StackTrace& a, baesu_StackTrace& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
