// bdefi_vfunc6.h               -*-C++-*-
#ifndef INCLUDED_BDEFI_VFUNC6
#define INCLUDED_BDEFI_VFUNC6

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide concrete representations for 6-arg void function objects.
//
//@CLASSES:
// bdefi_Vfunc6Null: a 6-arg void no-op functor
//
// bdefi_Vfunc6F6: a 6-arg void functor created from a 6-arg function
// bdefi_Vfunc6F7: a 6-arg void functor created from a 7-arg function
// bdefi_Vfunc6F8: a 6-arg void functor created from a 8-arg function
// bdefi_Vfunc6F9: a 6-arg void functor created from a 9-arg function
//
// bdefi_Vfunc6C6: a 6-arg void functor created from obj & 6-arg method
// bdefi_Vfunc6C7: a 6-arg void functor created from obj & 7-arg method
// bdefi_Vfunc6C8: a 6-arg void functor created from obj & 8-arg method
// bdefi_Vfunc6C9: a 6-arg void functor created from obj & 9-arg method
//
// bdefi_Vfunc6M6: a 6-arg void functor created from obj ptr & 6-arg method
// bdefi_Vfunc6M7: a 6-arg void functor created from obj ptr & 7-arg method
// bdefi_Vfunc6M8: a 6-arg void functor created from obj ptr & 8-arg method
// bdefi_Vfunc6M9: a 6-arg void functor created from obj ptr & 9-arg method
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component contains a suite of classes to implement the
// polymorphic representation of a function object (functor) whose
// function-call operator ('execute') takes six arguments and returns 'void'
// (hence the root name 'vfunc6' -- "v" for 'void' and "6" for the number of
// arguments).  Direct clients of this component choose a concrete
// implementation of the base 'bdefr_Vfunc6' representation based on the
// number of additional arguments required by the underlying procedure, and on
// the specific kind or "flavor" of function that the functor is to invoke.
// If the user does not wish to invoke any function on the functor invocation,
// then the "null functor" 'bdefi_Vfunc6Null' may be used.
//
///Language-Imposed Details
///------------------------
// For practical reasons, this component must distinguish among four types of
// functors.  The first type is a "null functor" 'bdefi_Vfunc6Null', which
// differs from the other three types in that it does not invoke any function.
//
// The other functors are distinguished by three flavors 'X' of functions they
// invoke, and also by the total number 'N' of arguments (N >= 6) that the
// function takes.  This component supplies a separate concrete implementation
// for each of the specified types.  These 12 non-null concrete
// implementations are named 'bdefi_Vfunc6XN' where 'X' is one of {'F', 'C',
// 'M'} and 'N' is in the range [6..9].
//
// The possible function flavors (and labels) are classified as:
//..
//     Label   Classification
//     -----   --------------
//       F     free function, static member function, or "free" functor object
//       C     'const' (accessor) member function
//       M     non-'const' (manipulator) member function
//..
// Note that a 'const' member function may be wrapped by a 'bdefi_Vfunc6MN'
// functor, but wrapping a non-'const' method by a 'bdefi_Vfunc6CN' will
// result in compile time error in most compiler implementations.
//
// Note also that, to accommodate the preservation of 'const', the
// 'bdefi_Vfunc6CN' functors hold an object 'Obj', supplied by a 'const'
// 'Obj&' in the constructor, while 'bdefi_Vfunc6MN' functors hold a pointer
// to 'Obj' supplied by a modifiable 'Obj*' argument in the constructor.
//
// The (N - 6) additional arguments specified at the construction of the
// functor are held by the functor, and appended, in order, to the six
// required (invocation-specific) arguments specified in each call to the
// virtual 'execute' method when the underlying function is invoked by
// 'execute'.  Any value returned from the underlying procedure is ignored.
//
// Note that the respective types of the parameters of the underlying procedure
// must correspond to the overall parameterization of the particular template
// representation class.  The virtual 'execute' method in each representation
// class defined in this component takes six arguments.  The respective types
// of the six leading parameters of the underlying procedure must match (or be
// automatically convertible from) those of the parameterized template
// representation class's virtual 'execute' method.  The respective types of
// any additional trailing parameters to the underlying procedure must match
// (or be convertible from) those specified in the particular representation
// class's constructor.
//
///Usage
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BDEFR_VFUNC6
#include <bdefr_vfunc6.h>
#endif


namespace BloombergLP {

                        // ======================
                        // class bdefi_Vfunc6Null
                        // ======================

template <class A1, class A2, class A3, class A4, class A5, class A6>
class bdefi_Vfunc6Null : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments,
    // returning 'void', and performing no operations.

    // NOT IMPLEMENTED
    bdefi_Vfunc6Null(const bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>&);
    bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>& operator=(
                     const bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6Null();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the no-op function with the specified 'argument1',
        // 'argument2', 'argument3', 'argument4', 'argument5', and 'argument6'.

  public:
    // CREATORS
    bdefi_Vfunc6Null(bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments, returning 'void', and performing no operation.
};

                        // ====================
                        // class bdefi_Vfunc6F6
                        // ====================

template <class F, class A1, class A2, class A3, class A4, class A5, class A6>
class bdefi_Vfunc6F6 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking no additional arguments.

    // DATA
    bslalg_ConstructorProxy<F> d_f;  // function pointer (or functor)

    // NOT IMPLEMENTED
    bdefi_Vfunc6F6(const bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>&);
    bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>& operator=(
                   const bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6F6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', and 'argument6' (and no
        // additional arguments).

  public:
    // CREATORS
    bdefi_Vfunc6F6(F procedure, bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking no
        // additional arguments.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'procedure' will be
        // ignored.
};

                        // ====================
                        // class bdefi_Vfunc6F7
                        // ====================

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class D1>
class bdefi_Vfunc6F7 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking one additional trailing
    // argument, and that argument's corresponding value.

    // DATA
    bslalg_ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg_ConstructorProxy<D1> d_d1;  // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6F7(const bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>&);
    bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>& operator=(
                   const bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6F7();
        // Destroy this functor.  Note that this destructor can be only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', and 'argument6' along with
        // the trailing argument value specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6F7(F                procedure,
                   const D1&        embeddedArg1,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // one additional trailing argument, and this corresponding argument's
        // specified 'embeddedArg1' value.  Use the specified 'basicAllocator'
        // to supply memory.  Note that any value returned from 'procedure'
        // will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6F8
                        // ====================

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2>
class bdefi_Vfunc6F8 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking three additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg_ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg_ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg_ConstructorProxy<D2> d_d2;  // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6F8(const bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>&);
    bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>& operator=(
                   const bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6F8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', and 'argument6' along with
        // the sequence of two argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6F8(F                procedure,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // two additional trailing arguments, and the corresponding arguments'
        // specified 'embeddedArg1' and 'embeddedArg2' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6F9
                        // ====================

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
class bdefi_Vfunc6F9 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking three additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg_ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg_ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg_ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg_ConstructorProxy<D3> d_d3;  // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6F9(
                 const bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);
    bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>& operator=(
                 const bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6F9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', and 'argument6' along with
        // the sequence of three argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6F9(F                procedure,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   const D3&        embeddedArg3,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // three additional trailing arguments and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2, and
        // 'embeddedArg3' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'procedure' will
        // be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6C6
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6>
class bdefi_Vfunc6C6 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning void, that holds an object and one of that object's 'const'
    // member functions taking no additional arguments.

    // DATA
    bslalg_ConstructorProxy<OBJ> d_obj;  // object
    C                            d_m;    // const member function pointer

    // NOT IMPLEMENTED
    bdefi_Vfunc6C6(const bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>&);
    bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>& operator=(
                   const bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6C6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' (and no additional
        // arguments).

  public:
    // CREATORS
    bdefi_Vfunc6C6(const OBJ&       object,
                   C                method,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking no additional
        // arguments.  Use the specified 'basicAllocator' to supply memory.
        // Note that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6C7
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
class bdefi_Vfunc6C7 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking one additional trailing argument, and the
    // argument's corresponding value.

    // DATA
    bslalg_ConstructorProxy<OBJ> d_obj;  // object
    C                            d_m;    // const member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;   // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6C7(const bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>&);
    bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>& operator=(
                   const bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6C7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the trailing
        // argument value specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6C7(const OBJ&       object,
                   C                method,
                   const D1&        embeddedArg1,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking one additional
        // trailing argument, and the corresponding argument's specified
        // 'embeddedArg1' value.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6C8
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
class bdefi_Vfunc6C8 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking three additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg_ConstructorProxy<OBJ> d_obj;  // object
    C                            d_m;    // const member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg_ConstructorProxy<D2>  d_d2;   // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6C8(
                const bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>&);
    bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>& operator=(
                const bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6C8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the sequence
        // of two trailing argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6C8(const OBJ&       object,
                   C                method,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking three additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1' and 'embeddedArg2' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6C9
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
class bdefi_Vfunc6C9 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking three additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg_ConstructorProxy<OBJ> d_obj;  // object
    C                            d_m;    // const member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg_ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg_ConstructorProxy<D3>  d_d3;   // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6C9(
            const bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);
    bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>& operator=(
            const bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6C9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the sequence
        // of three trailing argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6C9(const OBJ&       object,
                   C                method,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   const D3&        embeddedArg3,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking three additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', and 'embeddedArg3' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6M6
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6>
class bdefi_Vfunc6M6 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking no additional
    // arguments.

    // DATA
    OBJ *d_obj_p;  // object
    M    d_m;      // member function pointer

    // NOT IMPLEMENTED
    bdefi_Vfunc6M6(const bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>&);
    bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>& operator=(
                   const bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6M6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' (and no additional
        // arguments).

  public:
    // CREATORS
    bdefi_Vfunc6M6(OBJ             *object,
                   M                method,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking no additional arguments.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6M7
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
class bdefi_Vfunc6M7 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking one
    // additional trailing argument, and that argument's corresponding value.

    // DATA
    OBJ                         *d_obj_p;  // object
    M                            d_m;      // member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;     // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6M7(const bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>&);
    bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>& operator=(
                   const bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6M7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the additional
        // trailing argument value specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6M7(OBJ             *object,
                   M                method,
                   const D1&        embeddedArg1,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking one additional trailing argument, and the
        // corresponding argument's specified 'embeddedArg1' value.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc6M8
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
class bdefi_Vfunc6M8 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking three
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                         *d_obj_p;  // object
    M                            d_m;      // member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg_ConstructorProxy<D2>  d_d2;     // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6M8(
                const bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>&);
    bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>& operator=(
                const bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6M8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument1, const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the sequence
        // of two argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6M8(OBJ             *object,
                   M                method,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking three additional trailing arguments, and the
        // corresponding arguments' specified 'embeddedArg1' and
        // 'embeddedArg2' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};
                        // ====================
                        // class bdefi_Vfunc6M9
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
class bdefi_Vfunc6M9 : public bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking six arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking three
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                         *d_obj_p;  // object
    M                            d_m;      // member function pointer
    bslalg_ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg_ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg_ConstructorProxy<D3>  d_d3;     // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc6M9(
            const bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);
    bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>& operator=(
            const bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc6M9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute(const A1& argument,  const A2& argument2,
                 const A3& argument3, const A4& argument4,
                 const A5& argument5, const A6& argument6) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', and 'argument6' along with the sequence
        // of three argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc6M9(OBJ             *object,
                   M                method,
                   const D1&        embeddedArg1,
                   const D2&        embeddedArg2,
                   const D3&        embeddedArg3,
                   bslma_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking six
        // arguments and returning void, using the specified object address
        // and the specified ('const' or non-'const') 'method' of 'object'
        // taking three additional trailing arguments, and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2', and
        // 'embeddedArg3' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bdefi_Vfunc6Null
                        // ----------------------

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>::bdefi_Vfunc6Null(
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>::~bdefi_Vfunc6Null()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
void bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>::execute(const A1&,
                                                       const A2&,
                                                       const A3&,
                                                       const A4&,
                                                       const A5&,
                                                       const A6&) const
{
}

                        // --------------------
                        // class bdefi_Vfunc6F6
                        // --------------------

template <class F, class A1, class A2, class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>::bdefi_Vfunc6F6(
                                             F                procedure,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_f(procedure, basicAllocator)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>::~bdefi_Vfunc6F6()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6>
inline
void bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const F& f = d_f.object();  // for readability
    f(argument1, argument2, argument3, argument4, argument5, argument6);
}

                        // --------------------
                        // class bdefi_Vfunc6F7
                        // --------------------

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class D1>
inline
bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>::bdefi_Vfunc6F7(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class D1>
inline
bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>::~bdefi_Vfunc6F7()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class D1>
inline
void bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const F& f = d_f.object();  // for readability
    f(argument1, argument2, argument3, argument4,
      argument5, argument6, d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc6F8
                        // --------------------

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2>
inline
bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>::bdefi_Vfunc6F8(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2>
inline
bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>::~bdefi_Vfunc6F8()
{
}

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2>
inline
void bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const F& f = d_f.object();  // for readability
    f(argument1, argument2, argument3, argument4,
      argument5, argument6, d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc6F9
                        // --------------------

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>::bdefi_Vfunc6F9(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             const D3&        embeddedArg3,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>::~bdefi_Vfunc6F9()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
void bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const F& f = d_f.object();  // for readability
    f(argument1, argument2, argument3, argument4,
      argument5, argument6, d_d1.object(), d_d2.object(), d_d3.object());
}

                        // --------------------
                        // class bdefi_Vfunc6C6
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>::bdefi_Vfunc6C6(
                                             const OBJ&       object,
                                             C                method,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>::~bdefi_Vfunc6C6()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
void bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(argument1, argument2, argument3, argument4, argument5,
               argument6);
}

                        // --------------------
                        // class bdefi_Vfunc6C7
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>::bdefi_Vfunc6C7(
                                             const OBJ&       object,
                                             C                method,
                                             const D1&        embeddedArg1,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>::~bdefi_Vfunc6C7()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
void bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(argument1, argument2, argument3, argument4,
               argument5, argument6, d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc6C8
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>::bdefi_Vfunc6C8(
                                             const OBJ&       object,
                                             C                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>::~bdefi_Vfunc6C8()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
void bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(argument1, argument2, argument3, argument4,
               argument5, argument6, d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc6C9
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2, class D3>
inline
bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>::bdefi_Vfunc6C9(
                                             const OBJ&       object,
                                             C                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             const D3&        embeddedArg3,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2, class D3>
inline
bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>::~bdefi_Vfunc6C9()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1, class D2, class D3>
inline
void bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(argument1, argument2, argument3, argument4, argument5,
               argument6, d_d1.object(), d_d2.object(), d_d3.object());
}

                        // --------------------
                        // class bdefi_Vfunc6M6
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>::bdefi_Vfunc6M6(
                                             OBJ             *object,
                                             M                method,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj_p(object)
, d_m(method)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>::~bdefi_Vfunc6M6()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6>
inline
void bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4, argument5,
                    argument6);
}

                        // --------------------
                        // class bdefi_Vfunc6M7
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>::bdefi_Vfunc6M7(
                                             OBJ             *object,
                                             M                method,
                                             const D1&        embeddedArg1,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>::~bdefi_Vfunc6M7()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class D1>
inline
void bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4,
                    argument5, argument6, d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc6M8
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>::bdefi_Vfunc6M8(
                                             OBJ             *object,
                                             M                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>::~bdefi_Vfunc6M8()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2>
inline
void bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4,
                    argument5, argument6, d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc6M9
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>::bdefi_Vfunc6M9(
                                             OBJ             *object,
                                             M                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             const D3&        embeddedArg3,
                                             bslma_Allocator *basicAllocator)
: bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>::~bdefi_Vfunc6M9()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6,
          class D1, class D2, class D3>
inline
void bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4, argument5,
                    argument6, d_d1.object(), d_d2.object(), d_d3.object());
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
