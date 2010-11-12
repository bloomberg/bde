// bdefi_vfunc7.h               -*-C++-*-
#ifndef INCLUDED_BDEFI_VFUNC7
#define INCLUDED_BDEFI_VFUNC7

//@PURPOSE: Provide concrete representations for 7-arg void function objects.
//
//@CLASSES:
//
// bdefi_Vfunc7Null: a 7-arg void no-op functor
//
// bdefi_Vfunc7F7: a 7-arg void functor created from a 7-arg function
// bdefi_Vfunc7F8: a 7-arg void functor created from a 8-arg function
// bdefi_Vfunc7F9: a 7-arg void functor created from a 9-arg function
//
// bdefi_Vfunc7C7: a 7-arg void functor created from obj & 7-arg method
// bdefi_Vfunc7C8: a 7-arg void functor created from obj & 8-arg method
// bdefi_Vfunc7C9: a 7-arg void functor created from obj & 9-arg method
//
// bdefi_Vfunc7M7: a 7-arg void functor created from obj ptr & 7-arg method
// bdefi_Vfunc7M8: a 7-arg void functor created from obj ptr & 8-arg method
// bdefi_Vfunc7M9: a 7-arg void functor created from obj ptr & 9-arg method
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component contains a suite of classes to implement the
// polymorphic representation of a function object (functor) whose
// function-call operator ('execute') takes seven arguments and returns 'void'
// (hence the root name 'vfunc7' -- "v" for 'void' and "7" for the number of
// arguments).  Direct clients of this component choose a concrete
// implementation of the base 'bdefr_Vfunc7' representation based on the
// number of additional arguments required by the underlying procedure, and on
// the specific kind or "flavor" of function that the functor is to invoke.
// If the user does not wish to invoke any function on the functor invocation,
// then the "null functor" 'bdefi_Vfunc7Null' may be used.
//
///Language-Imposed Details
///------------------------
// For practical reasons, this component must distinguish among four types of
// functors.  The first type is a "null functor" 'bdefi_Vfunc7Null', which
// differs from the other three types in that it does not invoke any function.
//
// The other functors are distinguished by three flavors 'X' of functions they
// invoke, and also by the total number 'N' of arguments (N >= 7) that the
// function takes.  This component supplies a separate concrete implementation
// for each of the specified types.  These 9 non-null concrete
// implementations are named 'bdefi_Vfunc7XN' where 'X' is one of {'F', 'C',
// 'M'} and 'N' is in the range [7..9].
//
// The possible function flavors (and labels) are classified as:
//..
//     Label   Classification
//     -----   --------------
//       F     free function, static member function, or "free" functor object
//       C     'const' (accessor) member function
//       M     non-'const' (manipulator) member function
//..
// Note that a 'const' member function may be wrapped by a 'bdefi_Vfunc7MN'
// functor, but wrapping a non-'const' method by a 'bdefi_Vfunc7CN' will
// result in compile time error in most compiler implementations.
//
// Note also that, to accommodate the preservation of 'const', the
// 'bdefi_Vfunc7CN' functors hold an object 'Obj', supplied by a 'const'
// 'Obj&' in the constructor, while 'bdefi_Vfunc7MN' functors hold a pointer
// to 'Obj' supplied by a modifiable 'Obj*' argument in the constructor.
//
// The (N - 7) additional arguments specified at the construction of the
// functor are held by the functor, and appended, in order, to the seven
// required (invokation-specific) arguments specified in each call to the
// virtual 'execute' method when the underlying function is invoked by
// 'execute'.  Any value returned from the underlying procedure is ignored.
//
// Note that the respective types of the parameters of the underlying procedure
// must correspond to the overall parameterization of the particular template
// representation class.  The virtual 'execute' method in each representation
// class defined in this component takes seven arguments.  The respective
// types of the seven leading parameters of the underlying procedure must
// match (or be automatically convertible from) those of the parameterized
// template representation class's virtual 'execute' method.  The respective
// types of any additional trailing parameters to the underlying procedure
// must match (or be convertible from) those specified in the particular
// representation class's constructor.
//
///USAGE
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEFR_VFUNC7
#include <bdefr_vfunc7.h>
#endif

namespace BloombergLP {



                        // ======================
                        // class bdefi_Vfunc7Null
                        // ======================

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class bdefi_Vfunc7Null : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments,
    // returning 'void', and performing no operations.

  private:
    // not implemented
    bdefi_Vfunc7Null(const bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>&);
    bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>& operator=(
        const bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>&);

  public:
    // CREATORS
    inline bdefi_Vfunc7Null(bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments, returning 'void', and performing no operation.

  private:
    inline ~bdefi_Vfunc7Null();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the no-op function with the specified 'argument1',
        // 'argument2', 'argument3', 'argument4', 'argument5', 'argument6',
        // and 'argument7'.
};

                        // ====================
                        // class bdefi_Vfunc7F7
                        // ====================

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
class bdefi_Vfunc7F7 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking no additional arguments.

    F  d_f;  // function pointer or function object (functor)

  private:
    // not implemented
    bdefi_Vfunc7F7(const bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>&);
    bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>& operator=(
        const bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>&);

  public:
    // CREATORS
    inline bdefi_Vfunc7F7(F                procedure,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking no
        // additional arguments.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'procedure' will be
        // ignored.

  private:
    inline ~bdefi_Vfunc7F7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', 'argument6', and 'argument7'
        // (and no additional arguments).
};

                        // ====================
                        // class bdefi_Vfunc7F8
                        // ====================

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class D1>
class bdefi_Vfunc7F8 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking one additional trailing
    // argument, and that argument's corresponding value.

    F  d_f;  // function pointer or function object (functor)
    D1 d_d1; // first embedded argument

  private:
    // not implemented
    bdefi_Vfunc7F8(const bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>&);
    bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>& operator=(
        const bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7F8(F                procedure,
                          const D1&        embeddedArg1,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // one additional trailing argument, and this corresponding argument's
        // specified 'embeddedArg1' value.  Use the specified 'basicAllocator'
        // to supply memory.  Note that any value returned from 'procedure'
        // will be ignored.

  private:
    inline ~bdefi_Vfunc7F8();
        // Destroy this functor.  Note that this destructor can be only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', 'argument6', and 'argument7'
        // along with the trailing argument value specified at construction.
};

                        // ====================
                        // class bdefi_Vfunc7F9
                        // ====================

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1, class D2>
class bdefi_Vfunc7F9 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking three additional trailing
    // arguments, and those arguments' corresponding values.

    F  d_f;  // function pointer or function object (functor)
    D1 d_d1; // first embedded argument
    D2 d_d2; // second embedded argument

  private:
    // not implemented
    bdefi_Vfunc7F9(
        const bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
    bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>& operator=(
        const bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
  private:
    inline ~bdefi_Vfunc7F9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

  public:
    // CREATORS
    inline bdefi_Vfunc7F9(F                procedure,
                          const D1&        embeddedArg1,
                          const D2&        embeddedArg2,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // two additional trailing arguments, and the corresponding arguments'
        // specified 'embeddedArg1' and 'embeddedArg2' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'procedure' will be ignored.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the specified 'argument1', 'argument2',
        // 'argument3', 'argument4', 'argument5', 'argument6', and 'argument7'
        // along with the sequence of two argument values specified at
        // construction.
};

                        // ====================
                        // class bdefi_Vfunc7C7
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
class bdefi_Vfunc7C7 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning void, that holds an object and one of that object's 'const'
    // member functions taking no additional arguments.

    OBJ d_obj; // object
    C   d_m;   // const member function pointer

  private:
    // not implemented
    bdefi_Vfunc7C7(const bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>&);
    bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>& operator=(
        const bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7C7(const OBJ&       object,
                          C                method,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking no additional
        // arguments.  Use the specified 'basicAllocator' to supply memory.
        // Note that any value returned from 'method' will be ignored.

  private:
    inline ~bdefi_Vfunc7C7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' (and no
        // additional arguments).
};

                        // ====================
                        // class bdefi_Vfunc7C8
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
class bdefi_Vfunc7C8 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking one additional trailing argument, and the
    // argument's corresponding value.


    OBJ d_obj; // object
    C   d_m;   // const member function pointer
    D1  d_d1;  // first embedded argument

  private:
    // not implemented
    bdefi_Vfunc7C8(
        const bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>&);
    bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>& operator=(
        const bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7C8(const OBJ&       object,
                          C                method,
                          const D1&        embeddedArg1,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking one additional
        // trailing argument, and the corresponding argument's specified
        // 'embeddedArg1' value.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'method' will be ignored.

  private:
    inline ~bdefi_Vfunc7C8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' along with
        // the trailing argument value specified at construction.
};

                        // ====================
                        // class bdefi_Vfunc7C9
                        // ====================

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
class bdefi_Vfunc7C9 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking three additional trailing arguments, and the
    // arguments' corresponding values.

    OBJ d_obj; // object
    C   d_m;   // const member function pointer
    D1  d_d1;  // first embedded argument
    D2  d_d2;  // second embedded argument

  private:
    // not implemented
    bdefi_Vfunc7C9(
        const bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
    bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>& operator=(
        const bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7C9(const OBJ&       object,
                          C                method,
                          const D1&        embeddedArg1,
                          const D2&        embeddedArg2,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking three additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1' and 'embeddedArg2' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.

  private:
    inline ~bdefi_Vfunc7C9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' along with
        // the sequence of two trailing argument values specified at
        // construction.
};

                        // ====================
                        // class bdefi_Vfunc7M7
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
class bdefi_Vfunc7M7 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking no additional
    // arguments.

    OBJ *d_obj_p; // object
    M    d_m;     // member function pointer

  private:
    // not implemented
    bdefi_Vfunc7M7(const bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>&);
    bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>& operator=(
        const bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7M7(OBJ             *object,
                          M                method,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking no additional arguments.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.

  private:
    inline ~bdefi_Vfunc7M7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' (and no
        // additional arguments).
};

                        // ====================
                        // class bdefi_Vfunc7M8
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
class bdefi_Vfunc7M8 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking one
    // additional trailing argument, and that argument's corresponding value.

    OBJ *d_obj_p; // object
    M    d_m;     // member function pointer
    D1   d_d1;    // first embedded argument

  private:
    // not implemented
    bdefi_Vfunc7M8(
        const bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>&);
    bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>& operator=(
        const bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7M8(OBJ             *object,
                          M                method,
                          const D1&        embeddedArg1,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking one additional trailing argument, and the
        // corresponding argument's specified 'embeddedArg1' value.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.

  private:
    inline ~bdefi_Vfunc7M8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' along with
        // the additional trailing argument value specified at construction.
};

                        // ====================
                        // class bdefi_Vfunc7M9
                        // ====================

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
class bdefi_Vfunc7M9 : public bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7> {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking seven arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking three
    // additional trailing arguments, and those arguments' corresponding
    // values.

    OBJ *d_obj_p; // object
    M    d_m;     // member function pointer
    D1   d_d1;    // first embedded argument
    D2   d_d2;    // second embedded argument

  private:
    // not implemented
    bdefi_Vfunc7M9(
        const bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
    bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>& operator=(
        const bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>&);
  public:
    // CREATORS
    inline bdefi_Vfunc7M9(OBJ             *object,
                          M                method,
                          const D1&        embeddedArg1,
                          const D2&        embeddedArg2,
                          bdema_Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking seven
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking three additional trailing arguments, and the
        // corresponding arguments' specified 'embeddedArg1' and
        // 'embeddedArg2' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.

  private:
    inline ~bdefi_Vfunc7M9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // ACCESSORS
    inline void execute(const A1& argument1, const A2& argument2,
                        const A3& argument3, const A4& argument4,
                        const A5& argument5, const A6& argument6,
                        const A7& argument7) const;
        // Invoke the underlying member function on the embedded object pointer
        // with the specified 'argument1', 'argument2', 'argument3',
        // 'argument4', 'argument5', 'argument6', and 'argument7' along with
        // the sequence of two argument values specified at construction.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bdefi_Vfunc7Null
                        // ----------------------

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>::bdefi_Vfunc7Null(
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>::~bdefi_Vfunc7Null()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline void bdefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
}

                        // --------------------
                        // class bdefi_Vfunc7F7
                        // --------------------

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
inline
bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>::bdefi_Vfunc7F7(
                                             F                procedure,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_f(procedure)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
inline bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>::~bdefi_Vfunc7F7()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
inline void bdefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    d_f(argument1, argument2, argument3, argument4, argument5,
        argument6, argument7);
}

                        // --------------------
                        // class bdefi_Vfunc7F8
                        // --------------------

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class D1>
inline bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>::bdefi_Vfunc7F8(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_f(procedure)
, d_d1(embeddedArg1)
{
}

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class D1>
inline bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>::~bdefi_Vfunc7F8()
{
}

template <class F, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class D1>
inline void bdefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    d_f(argument1, argument2, argument3, argument4,
        argument5, argument6, argument7, d_d1);
}

                        // --------------------
                        // class bdefi_Vfunc7F9
                        // --------------------

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1, class D2>
inline bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>::bdefi_Vfunc7F9(
                                             F                procedure,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_f(procedure)
, d_d1(embeddedArg1)
, d_d2(embeddedArg2)
{
}

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1, class D2>
inline bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>::~bdefi_Vfunc7F9()
{
}

template <class F, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1, class D2>
inline
void bdefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    d_f(argument1, argument2, argument3, argument4,
        argument5, argument6, argument7, d_d1, d_d2);
}


                        // --------------------
                        // class bdefi_Vfunc7C7
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>::bdefi_Vfunc7C7(
                                             const OBJ&       object,
                                             C                method,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj(object)
, d_m(method)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>::~bdefi_Vfunc7C7()
{
}


template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline void bdefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj.*d_m)(argument1, argument2, argument3, argument4, argument5,
                 argument6, argument7);
}

                        // --------------------
                        // class bdefi_Vfunc7C8
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>::bdefi_Vfunc7C8(
                                             const OBJ&       object,
                                             C                method,
                                             const D1&        embeddedArg1,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj(object)
, d_m(method)
, d_d1(embeddedArg1)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline
bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>::~bdefi_Vfunc7C8()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline
void bdefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj.*d_m)(argument1, argument2, argument3, argument4,
                 argument5, argument6, argument7, d_d1);
}

                        // --------------------
                        // class bdefi_Vfunc7C9
                        // --------------------

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
inline
bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>::bdefi_Vfunc7C9(
                                             const OBJ&       object,
                                             C                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj(object)
, d_m(method)
, d_d1(embeddedArg1)
, d_d2(embeddedArg2)
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
inline
bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>::~bdefi_Vfunc7C9()
{
}

template <class OBJ, class C, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2> inline
void bdefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj.*d_m)(argument1, argument2, argument3, argument4,
                 argument5, argument6, argument7, d_d1, d_d2);
}


                        // --------------------
                        // class bdefi_Vfunc7M7
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>::bdefi_Vfunc7M7(
                                             OBJ             *object,
                                             M                method,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj_p(object)
, d_m(method)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>::~bdefi_Vfunc7M7()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7>
inline void bdefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4, argument5,
                    argument6, argument7);
}

                        // --------------------
                        // class bdefi_Vfunc7M8
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>::bdefi_Vfunc7M8(
                                             OBJ             *object,
                                             M                method,
                                             const D1&        embeddedArg1,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline
bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>::~bdefi_Vfunc7M8()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7, class D1>
inline
void bdefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4,
                    argument5, argument6, argument7, d_d1);
}

                        // --------------------
                        // class bdefi_Vfunc7M9
                        // --------------------

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
inline
bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>::bdefi_Vfunc7M9(
                                             OBJ             *object,
                                             M                method,
                                             const D1&        embeddedArg1,
                                             const D2&        embeddedArg2,
                                             bdema_Allocator *basicAllocator)
: bdefr_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1)
, d_d2(embeddedArg2)
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2>
inline
bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>::~bdefi_Vfunc7M9()
{
}

template <class OBJ, class M, class A1, class A2,
          class A3, class A4, class A5, class A6, class A7,
          class D1, class D2> inline
void bdefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>::execute(
                                                   const A1& argument1,
                                                   const A2& argument2,
                                                   const A3& argument3,
                                                   const A4& argument4,
                                                   const A5& argument5,
                                                   const A6& argument6,
                                                   const A7& argument7) const
{
    (d_obj_p->*d_m)(argument1, argument2, argument3, argument4,
                    argument5, argument6, argument7, d_d1, d_d2);
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
