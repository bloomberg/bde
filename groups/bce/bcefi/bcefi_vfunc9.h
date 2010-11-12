// bcefi_vfunc9.h               -*-C++-*-
#ifndef INCLUDED_BCEFI_VFUNC9
#define INCLUDED_BCEFI_VFUNC9

//@PURPOSE: Provide concrete representations for 9-arg void function objects.
//
//@CLASSES:
//
// bcefi_Vfunc9Null: a 9-arg void no-op functor
//
// bcefi_Vfunc9F9: a 9-arg void functor created from a 9-arg function
//
// bcefi_Vfunc9C9: a 9-arg void functor created from obj & 9-arg method
//
// bcefi_Vfunc9M9: a 9-arg void functor created from obj ptr & 9-arg method
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component contains a suite of classes to implement the
// polymorphic representation of a function object (functor) whose
// function-call operator ('execute') takes nine arguments and returns 'void'
// (hence the root name 'vfunc9' -- "v" for 'void' and "9" for the number of
// arguments).  Direct clients of this component choose a concrete
// implementation of the base 'bcefr_Vfunc9' representation based on the
// specific kind or "flavor" of function that the functor is to invoke.  If
// the user does not wish to invoke any function on the functor invocation,
// then the "null functor" 'bcefi_Vfunc9Null' may be used.
//
///Language-Imposed Details
///------------------------
// For practical reasons, this component must distinguish among four types of
// functors.  The first type is a "null functor" 'bcefi_Vfunc9Null', which
// differs from the other three types in that it does not invoke any function.
//
// The other functors are distinguished by three flavors 'X' of functions they
// invoke.  All functions must have nine parameters.  This component supplies
// a separate concrete implementation for each of the specified types.  These
// three non-null concrete implementations are named 'bcefi_Vfunc9X9' where
// 'X' is one of {'F', 'C', 'M'}.
//
// The possible function flavors (and labels) are classified as:
//..
//     Label   Classification
//     -----   --------------
//       F     free function, static member function, or "free" functor object
//       C     'const' (accessor) member function
//       M     non-'const' (manipulator) member function
//..
// Note that a 'const' member function may be wrapped by a 'bcefi_Vfunc9MN'
// functor, but wrapping a non-'const' method by a 'bcefi_Vfunc9CN' will
// result in compile time error in most compiler implementations.
//
// Note also that, to accommodate the preservation of 'const', the
// 'bcefi_Vfunc9C9' functors hold an object 'Obj', supplied by a 'const'
// 'Obj&' in the constructor, while 'bcefi_Vfunc9M9' functors hold a pointer
// to 'Obj' supplied by a modifiable 'Obj*' argument in the constructor.
//
// Any value returned from the underlying procedure is ignored.
//
// Note that the respective types of the parameters of the underlying procedure
// must correspond to the overall parameterization of the particular template
// representation class.  The virtual 'execute' method in each representation
// class defined in this component takes nine arguments.  The respective types
// of the nine leading parameters of the underlying procedure must match (or
// be automatically convertible from) those of the parameterized template
// representation class's virtual 'execute' method.
//
///USAGE
///-----

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEFR_VFUNC9
#include <bcefr_vfunc9.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class bcefi_Vfunc9Null
                        // ======================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
class bcefi_Vfunc9Null :
                 public bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking nine arguments,
    // returning 'void', and performing no operations.

  private:
    // not implemented
    bcefi_Vfunc9Null(
        const bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
    bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>& operator=(
        const bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>&);

  public:
    // CREATORS
    inline bcefi_Vfunc9Null(bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking nine
        // arguments, returning 'void', and performing no operation.

  private:
    inline ~bcefi_Vfunc9Null();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7, const A8& argument8,
                        const A9& argument9) const;
        // Invoke the no-op function with the specified 'argument1',
        // 'argument2', 'argument3', 'argument4', 'argument5', 'argument6',
        // 'argument7', 'argument8', and 'argument9'.
};

                        // ====================
                        // class bcefi_Vfunc9F9
                        // ====================

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
class
bcefi_Vfunc9F9 : public bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking nine arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking no additional arguments.

    F  d_f;  // function pointer or function object (functor)

  private:
    // not implemented
    bcefi_Vfunc9F9(
        const bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
    bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>& operator=(
        const bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);

  public:
    // CREATORS
    inline bcefi_Vfunc9F9(F                procedure,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking nine
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking no
        // additional arguments.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'procedure' will be
        // ignored.

  private:
    inline ~bcefi_Vfunc9F9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7, const A8& argument8,
                        const A9& argument9) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', 'argument6', 'argument7',
        // 'argument8', and 'argument9'.
};


                        // ====================
                        // class bcefi_Vfunc9C9
                        // ====================

template <class OBJ, class C, class A1, class A2, class A3,
          class A4, class A5, class A6, class A7, class A8, class A9>
class bcefi_Vfunc9C9 :
                      public bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking nine arguments and
    // returning void, that holds an object and one of that object's 'const'
    // member functions taking no additional arguments.

    OBJ d_obj; // object
    C   d_m;   // const member function pointer

  private:
    // not implemented
    bcefi_Vfunc9C9(
        const bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
    bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>& operator=(
        const bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
  public:
    // CREATORS
    inline bcefi_Vfunc9C9(const OBJ&       object,
                          C                method,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking nine
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking no additional
        // arguments.  Use the specified 'basicAllocator' to supply memory.
        // Note that any value returned from 'method' will be ignored.

  private:
    inline ~bcefi_Vfunc9C9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7, const A8& argument8,
                        const A9& argument9) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', 'argument7', 'argument8',
        // and 'argument9'.
};

                        // ====================
                        // class bcefi_Vfunc9M9
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class
bcefi_Vfunc9M9 : public bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking nine arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking no additional
    // arguments.

    OBJ *d_obj_p; // object
    M    d_m;     // member function pointer

  private:
    // not implemented
    bcefi_Vfunc9M9(
        const bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
    bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>& operator=(
        const bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>&);
  public:
    // CREATORS
    inline bcefi_Vfunc9M9(OBJ             *object,
                          M                method,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking nine
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking no additional arguments.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.

  private:
    inline ~bcefi_Vfunc9M9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7, const A8& argument8,
                        const A9& argument9) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', 'argument7', 'argument8',
        // 'argument9'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bcefi_Vfunc9Null
                        // ----------------------

template <class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>::bcefi_Vfunc9Null(
                                             bdema_Allocator *basicAllocator)
: bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>::~bcefi_Vfunc9Null()
{
}

template <class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
inline void bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7,
                                                   const A8& argument8,
                                                   const A9& argument9) const
{
}

                        // --------------------
                        // class bcefi_Vfunc9F9
                        // --------------------

template <class F, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>::bcefi_Vfunc9F9(
                                             F                procedure,
                                             bdema_Allocator *basicAllocator)
: bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(basicAllocator)
, d_f(procedure)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
inline bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>::~bcefi_Vfunc9F9()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
inline void bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7,
                                                   const A8& argument8,
                                                   const A9& argument9) const
{
    d_f(argument1, argument2, argument3, argument4, argument5,
        argument6, argument7, argument8, argument9);
}


                        // --------------------
                        // class bcefi_Vfunc9C9
                        // --------------------

template <class OBJ, class C, class A1, class A2, class A3, class A4,
          class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>::bcefi_Vfunc9C9(
                                             const OBJ&       object,
                                             C                method,
                                             bdema_Allocator *basicAllocator)
: bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(basicAllocator)
, d_obj(object)
, d_m(method)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>::~bcefi_Vfunc9C9()
{
}


template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline
void bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7,
                                                   const A8& argument8,
                                                   const A9& argument9) const
{
    (d_obj.*d_m)(argument1, argument2, argument3, argument4, argument5,
                 argument6, argument7, argument8, argument9);
}


                        // --------------------
                        // class bcefi_Vfunc9M9
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>::bcefi_Vfunc9M9(
                                             OBJ             *object,
                                             M                method,
                                             bdema_Allocator *basicAllocator)
: bcefr_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(basicAllocator)
, d_obj_p(object)
, d_m(method)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline
bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>::~bcefi_Vfunc9M9()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline
void bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7,
                                                   const A8& argument8,
                                                   const A9& argument9) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4, argument5,
                    argument6, argument7, argument8, argument9);
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
