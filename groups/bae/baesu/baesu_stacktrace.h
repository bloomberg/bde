// baesu_stacktrace.h                                                 -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACE
#define INCLUDED_BAESU_STACKTRACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an object that can represent a stack trace
//
//@CLASSES:
//  baesu_StackTrace: value of a stack trace, container of stack trace frames
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: baesu_stacktraceframe, baesu_stacktraceutil,
//           baesu_stacktraceprintutil
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// class, 'baesu_StackTrace', that is used to represent a stack trace.  This
// class contains a sequence of 'baesu_StackTraceFrame' objects and a memory
// allocator that is used by default, though the client may specify another
// allocator to be used in its place at construction.
//
///Usage
///-----

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#include <baesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BDEMA_HEAPBYPASSALLOCATOR
#include <bdema_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
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
    // This unconstrained (value-semantic) class describes a stack trace,
    // represented as a sequence randomly-accesible 'baesu_StackTraceFrame'
    // objects, each of which represents one function call on the stack.
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
                                 d_frames;
    bslma_Allocator             *d_allocator_p;  // held, now owned

  public:
    BSLALG_DECLARE_NESTED_TRAITS2(baesu_StackTrace,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    explicit
    baesu_StackTrace(bslma_Allocator *basicAllocator = 0);
        // Create a 'baesu_StackTrace' object of 0 length.  If 'allocator' is
        // specfied, the underlying 'd_frames' vector will use that value, if
        // not, 'd_frames' will be initialized using the heap bypass allocator
        // contained in this object.

    baesu_StackTrace(const baesu_StackTrace&  original,
                     bslma_Allocator         *allocator = 0);
        // Create a 'baesu_StackTrace' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // ~baesu_StackTrace();
        // Compiler-generated

    // MANIPULATORS
    baesu_StackTrace& operator=(const baesu_StackTrace& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    baesu_StackTraceFrame& operator[](int index);
        // Return a reference to the stack trace frame corresponding to the
        // specified 'index'.  The behavior is undefined unless
        // '0 <= index < length()'.

    void removeAll();
        // Remove all stack trace frames from this object and free their
        // memory, after which 'length()' will be 0.

    void resize(int newLength);
        // Add or remove stack trace frames to or from the end of this stack
        // trace object such that 'length() == newLength'.  Stack trace frames
        // whose indices are in the range
        // '0 <= index < min(oldLength, newLength)', where 'oldLength' was the
        // length of this object prior to the operation, will be unchanged.

    void swap(baesu_StackTrace& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const baesu_StackTraceFrame& operator[](int index) const;
        // Return a reference to the const stack trace frame corresponding to
        // the specified 'index'.  The behavior is undefined unless
        // '0 <= index < length()'.

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
    // value, and 'false' otherwise.  Two 'baesu_StackTrace' objects
    // have the same value if all of their corresponding stack trace frames
    // have the same value.

bool operator!=(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baesu_StackTrace' objects do
    // not have the same value if any of the stack trace frames contained in
    // 'lhs' is not the same as the corresponding stack trace frame in 'rhs'.

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
, d_frames(basicAllocator ? basicAllocator
                          : &d_hbpAlloc)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &d_hbpAlloc)
{
}

inline
baesu_StackTrace::baesu_StackTrace(const baesu_StackTrace&  original,
                                   bslma_Allocator         *basicAllocator)
: d_hbpAlloc()
, d_frames(original.d_frames,
           basicAllocator ? basicAllocator
                          : &d_hbpAlloc)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &d_hbpAlloc)
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
    return d_frames[index];
}

inline
bslma_Allocator *baesu_StackTrace::allocator() const
{
    return d_allocator_p;
}

inline
int baesu_StackTrace::length() const
{
    return d_frames.size();
}

// FREE OPERATORS
inline
bool operator==(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;
    }

    for (int i = 0;  i < lhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

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
