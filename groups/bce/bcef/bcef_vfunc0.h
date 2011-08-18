// bcef_vfunc0.h                -*-C++-*-
#ifndef INCLUDED_BCEF_VFUNC0
#define INCLUDED_BCEF_VFUNC0

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bcef_Vfunc0: signature-specific function object (functor)
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component implements a function object (functor) that
// returns void and takes no arguments of arbitrary type; it is invoked in a
// manner similar to that of following free function:
//..
//   void functionObject();
//..
// The functors in the 'bcef' package are implemented using the envelope-letter
// pattern (see Coplien, "Advanced C++", Sections 5.5-5.6, pp.  133-179).
// Functors are objects that implement a function-call operator whose signature
// characterizes the particular functor's type.  A valid functor will contain a
// pointer to an underlying (member or free) function and is invoked in a
// manner similar to that of free functions.
//
// Unlike a function pointer, a functor may be created with one or more
// arbitrary objects (sometimes called "user data") to be passed (typically as
// trailing arguments) to the underlying function.  By "pre-binding" particular
// arguments to the underlying function, a functor reduces the number of
// arguments a caller must supply when the functor is invoked.  In this way,
// function objects can be used to coerce functions with extra arguments of
// arbitrary type into a standard calling signature.  Even (non-'static')
// *member* functions can be encapsulated within functors and treated
// uniformly, simply by supplying the appropriate representation object at
// construction.
//
// The 'bcef_Vfunc0' functor (envelope) holds a polymorphic, reference-counted,
// dynamically-allocated representation (letter), which is loaded by the
// immediate client.  Typical users do not load the functor representation
// directly, but instead rely on a suite of template functions (provided by
// components in the higher-level 'bcefu' package) to allocate and install the
// representation appropriate to (1) the desired function-call-operator
// signature and (2) the supplied user data.  See the 'bcefu_vfunc0' component
// for more information on populating 'bcef_Vfunc0' function objects with
// specific representations.
//
// Note that because each required argument type 'T' is passed by ('const'
// 'T&'), a compiler will generate an error if the user declares a callback
// function taking an argument via a non-'const' reference parameter.  This
// serendipitous side effect is truly a feature, and specifically not a bug, as
// we use references (and occationally pointers) to pass constant arguments,
// and pointers (exclusively) to pass modifiable arguments.  This
// implementation enforces that rule.
//
///Usage
///-----

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEFR_VFUNC0
#include <bcefr_vfunc0.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

                        // =================
                        // class bcef_Vfunc0
                        // =================

namespace BloombergLP {

class bcef_Vfunc0 {
    // Provide an object that encapsulates a function or method along with zero
    // or more "extra" arguments of arbitrary type (collectively referred to
    // as "user data") to be passed as arguments to the underlying function on
    // invocation.  Clients invoke the underlying function by calling this
    // functor's function-call operator.
    //..
    //     (void) functionObject()    // functor calling signature
    //..
    // which takes no arguments and returns void.  Note that pointer (but not
    // reference) types are supported.

    bcefr_Vfunc0 *d_rep_p;  // polymorphic functor representation

    // NOT IMPLEMENTED
    bool operator==(const bcef_Vfunc0&) const;
    bool operator!=(const bcef_Vfunc0&) const;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcef_Vfunc0,
                                 bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    bcef_Vfunc0();
        // Create an unitialized functor.  The resulting functor cannot be
        // invoked, but may subsequently be assigned an invokable value.

    bcef_Vfunc0(bcefr_Vfunc0 *representation);
        // Create a functor that assumes shared ownership of the specified,
        // dynamically allocated, reference-counted representation.  If 0 is
        // specified, the functor is created in a null state (i.e., cannot be
        // invoked), but may subsequently be assigned a value.
        //
        // Note that typical clients of this class do not use this constructor
        // directly, but instead use utilities provided in higher level
        // components (see 'bcefu_vfunc0') to create an appropriate
        // representation and load it into an existing functor.

    bcef_Vfunc0(const bcef_Vfunc0& functionObject);
        // Create a copy of the specified 'functionObject'.  If
        // 'functionObject' is null, an independent null functor is created.
        // Otherwise, both functors will share ownership of the same internal
        // representation.

    ~bcef_Vfunc0();
        // If this functor holds a non-null representation, decrement the
        // reference count of that internal representation object, and, if the
        // count is now 0, destroy and deallocate that representation (using
        // the static 'deleteObject' method of 'bcefr_Vfunc0' base class).

    // MANIPULATORS
    bcef_Vfunc0& operator=(const bcef_Vfunc0& functionObject);
        // Assign the value of the specified 'functionObject' to this object.
        // If 'functionObject' is not null, both functors will share ownership
        // of the same internal representation; otherwise, both functors will
        // be null.  If this object was not initially null (and did not
        // already share ownership of the same internal representation with
        // 'functionObject'), first decrement the reference count of this
        // functor's internal representation object, and, if the count is now
        // 0, destroy and deallocate the representation (using the static
        // 'deleteObject' method of 'bcefr_Vfunc0' base class.

    // ACCESSORS
    void operator()() const;
        // Invoke this functor.  The behavior is undefined if this functor is
        // null.

    operator const void *() const;
        // Return 0 if this functor is null, and non-zero otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline bcef_Vfunc0::bcef_Vfunc0()
: d_rep_p(0)
{
}

inline bcef_Vfunc0::bcef_Vfunc0(bcefr_Vfunc0 *rep)
: d_rep_p(rep)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}

inline bcef_Vfunc0::bcef_Vfunc0(const bcef_Vfunc0& functor)
: d_rep_p(functor.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}

inline bcef_Vfunc0::~bcef_Vfunc0()
{
    if (d_rep_p && 0 == d_rep_p->decrement()) {
        bcefr_Vfunc0::deleteObject(d_rep_p);
    }
}

inline bcef_Vfunc0& bcef_Vfunc0::operator=(const bcef_Vfunc0& rhs)
{
    if (d_rep_p != rhs.d_rep_p) {
        if (d_rep_p && 0 == d_rep_p->decrement()) {
            bcefr_Vfunc0::deleteObject(d_rep_p);
        }
        d_rep_p = rhs.d_rep_p;
        if (d_rep_p) {
            d_rep_p->increment();
        }
    }
    return *this;
}

inline void bcef_Vfunc0::operator()() const
{
    d_rep_p->execute();
}

inline bcef_Vfunc0::operator const void *() const
{
    return d_rep_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
