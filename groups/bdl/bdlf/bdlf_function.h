// bdlf_function.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLF_FUNCTION
#define INCLUDED_BDLF_FUNCTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bdlf::Function: signature-specific function object (functor)
//
//@SEE_ALSO: bdlf_memfn, bdlf_bind, bdlcf_bindutil
//
//@DESCRIPTION: This component provides a polymorphic function object (functor)
// 'bdlf::Function' that returns an arbitrary result type and accepts up to
// fourteen arguments of arbitrary types.  A "functor" is similar to a C/C++
// function pointer, but unlike function pointers, functors can be used to
// invoke any object that can be syntactically invoked as a function.  Functors
// are generally used as callback functions or to avoid templatizing a
// function.
//
///Basic Usage
///-----------
// Before documenting the finer points of the 'bdlf_function' component, we
// offer the following examples to illustrate the various steps of using
// functors.
//
///Declaring a Functor
///- - - - - - - - - -
// The 'bdlf::Function' functor takes a single C-style function pointer
// template argument that describes the prototype of the functor.  For example,
// the declaration of a functor that returns 'void' and accepts no arguments,
// would look as follows:
//..
//  bdlf::Function< void (*)() > voidFunctor;
//..
// To declare a functor that returns 'int' and accepts two 'double' arguments:
//..
//  bdlf::Function< int (*)(double, double) > intFunctor;
//..
//
///Initializing a Functor
/// - - - - - - - - - - -
// If there exists an object 'F' for which the expression 'F(X1, ..., XN)' is
// legal, then 'F' can be represented by a functor that accepts 'N' arguments
// of types compatible with 'X1', ..., 'XN', respectively.  For example, given
// the following 'Functor' definition:
//..
//  typedef bdlf::Function<void(*)(int, const char*)> Functor;
//..
// any of the following object may be assigned to a 'Functor' object:
//..
//  void freeFunction(int, const char*);
//
//  int freeFunctionNonVoid(int, const char*);
//
//  void freeFunctionCompatibleArgs(double, const char*);
//
//  class FunctionObject {
//      // ....
//    public:
//      void operator() (int , const char*);
//  };
//
//  class FunctionObject2 {
//      // ....
//    public:
//      void operator() (int , bsl::string);
//  };
//..
//
///Invoking a Functor
/// - - - - - - - - -
// Once initialized, the functor objects can be invoked in the same way as
// regular functions:
//..
//  void usageExampleTest1()
//  {
//     Functor func;
//
//     func = &freeFunction;
//     func(100, "It works");
//
//     func = &freeFunctionNonVoid;
//     func(100, "Still works - 'int' result discarded");
//
//     func = &freeFunctionCompatibleArgs;
//     func(100, "Works too - 100 promoted to (double)100.0");
//
//     FunctionObject o;
//     func = o;
//     func(100, "This works too!!!!!");
//
//     FunctionObject2 o2;
//     func = o2;
//     func(100, "This also works - 'bsl::string' temporary created!");
//  }
//..
// Note that member function pointers cannot be directly invoked by a functor
// since for a given pointer 'memFnPtr' to a member function on an object 'o',
// the invocation syntax is 'o.*memFnPtr(...)' which is syntactically different
// from 'memFnPtr(o, ...)'.  The 'bdlf_memfn' and 'bdlf_bind' components
// provide adapters to allow member function pointers to be invoked as
// functors, either taking the object on which to invoke the member function
// as parameter, or storing a reference to it and adding it implicitly to the
// function call.
//
///Functors and Memory Allocation
///------------------------------
// 'bdlf::Function' uses and follows the 'bdema' allocator model.  However, the
// implementation exposes the details of an important memory optimization with
// specific guarantees.  It is important to expose this implementation detail
// both for testing and for its proper use in an environment where memory
// allocation is a restricted call.  We detail on the specific guarantees and
// on the rationale for exposing this implementation detail.  The rationale
// section can be skipped in a first reading.
//
///Specific Guarantees
///- - - - - - - - - -
// Functors are guaranteed *not* to trigger a memory allocation when created
// with or assigned from the following invocables:
//
//: 1 a free function pointer or pointer to a static member function,
//:
//: 2 a member function pointer together with a pointer to an object,
//:   (i.e., 'bdlf::MemFn' objects created by 'bdlf::MemFnUtil::memFn'
//:   function, as illustrated at the end of the subsection 'Initializing a
//:   functor' in the usage section below),
//:
//: 3 another object (e.g., user-defined object, or functor created by one of
//:   the 'bdefu::Vfunc*::make*' factory methods) whose size is at most
//:   'bdlf::FunctionUtil::MAX_INPLACE_OBJECT_SIZE'.
//
// Note that "size" in item number 3 refers to the return value of the 'sizeof'
// operator on the object.  Also note that the constant
// 'bdlf::FunctionUtil::MAX_INPLACE_OBJECT_SIZE' is platform- and
// implementation-dependent, but it is guaranteed to be at least as large one
// member function pointer and one additional pointer.
//
// Functors are guaranteed to *trigger* a memory allocation when constructed
// with or assigned from an invokable object which is a user-defined class of
// size larger than 'bdlf::FunctionUtil::MAX_INPLACE_OBJECT_SIZE'.
//
// Finally, once a 'bdlf::Function' object has been constructed, it is possible
// to check if it was created in-place or not, by using the 'isInplace' method.
// A compile-time check is also provided by the meta-function
// 'bdlf::FunctionUtil::isInplace'.  See the illustration of design rationale
// number two in the next section for an example of how this assertion can be
// useful.
//
///Design Rationale
/// - - - - - - - -
// There are two reasons why this component exposes the memory allocation
// algorithm:
//
//: 1 In order to enable proper testing of the 'bdema'-allocator model in
//:   components that use 'bdlf::Function' objects.
//:
//: 2 In order to avoid defining an allocator in environments where
//:   default-allocated memory should not be allowed, and yet be able to
//:   certify that the default allocator is not used.
//
// To illustrate reason number one, imagine testing a component that defines a
// class that uses a 'bslma::Allocator' and stores function objects.  According
// to the 'bdema' allocation model, this class should propagate its allocator
// to all its members.  In order to test this, typically a test allocator is
// passed to the class at construction and a default allocator guard is used to
// assert that no default allocation was performed.  If the class fails to
// implement the proper allocation model, but also uses a function object whose
// size is at most 'bdlf::FunctionUtil::MAX_INPLACE_OBJECT_SIZE', the test
// driver will fail to expose the violation of the allocation model.  In order
// to expose it, the test sequence must make sure that it creates its function
// objects from an invokable object whose size is larger than
// 'bdlf::FunctionUtil::MAX_INPLACE_OBJECT_SIZE'.
//
// To illustrate reason number two, suppose that long-term (i.e.,
// non-temporary) memory should never be allocated for reasons touching the
// environment in which the program will be run.  A run-time check can be put
// wherever a 'bdlf::Function' object is created without an allocator (perhaps
// because it would be too costly to create an allocator just for that function
// object, or perhaps because the function object has static storage and could
// not be created with an allocator), as in the following example:
//..
//  typedef bdlf::Function<int (*)(void *)>  MyCallback;
//
//  static MyCallback currentCallback = 0;
//
//  void scheduleCallback(const MyCallback& func) {
//      assert(!currentCallback);
//      currentCallback = func;
//
//      // Note: this function may run in malloc-restricted environment, so we
//      // much make sure we did not accidentally use the default allocator.
//      assert(currentCallback.isInplace());
//  }
//..
// The last line makes sure that an allocator was not used for the assignment
// of the function object.  Note that the invokable itself may have triggered a
// memory allocation for its own members, which will not be detected by this
// assert; in that case it is safer to use a default-allocator guard (see the
// 'bslma_defaultallocatorguard' component).
//
///Usage
///-----
// Basic usage examples were given in the section 'Basic usage' earlier in this
// documentation.  We complement this by an illustration of a typical use of
// 'bdlf::Function'.
//
// For small interfaces, functors can often be used in place of virtual
// functions, reducing the need for inheritance.  Take for example a class,
// 'MessageHandler' that reads data from a stream, and invokes a user-supplied
// handler for each message extracted from the stream.  Without functors, a
// inheritance hierarchy of message handler needs to be created.  However, with
// 'bdlf::Function', any function or functor that matches the required
// signature can be passed in.
//
// In the following implementation, we pass either a message handler function
// object or a free function to the constructor of 'MessageProcessor':
//..
//  class Message {
//      // ...
//  };
//
//  class MessageProcessor {
//      // This class reads data from a stream, then invokes a user-supplied
//      // handler for each message extracted from the stream.
//
//    public:
//      // TYPES
//      typedef bdlf::Function<void(*)(const Message&)> MessageHandler;
//
//    private:
//      // DATA
//      MessageHandler d_handler;
//
//      // PRIVATE MANIPULATORS
//      int extractMessage(Message *messageBuffer, bsl::istream& stream);
//
//    public:
//      // CREATORS
//      MessageProcessor(MessageHandler handler);
//
//      // MANIPULATORS
//      void processMessages(bsl::istream& stream);
//  };
//
//  // PRIVATE MANIPULATORS
//  int MessageProcessor::extractMessage(Message *messageBuffer,
//                                       bsl::istream& stream)
//  {
//      // ...
//      return 1;
//  }
//
//  // CREATORS
//  MessageProcessor::MessageProcessor(MessageHandler handler)
//  : d_handler(handler)
//  {
//  }
//
//  // MANIPULATORS
//  void MessageProcessor::processMessages(bsl::istream& stream)
//  {
//      int ret = 1;
//      do {
//          Message msg;
//          ret = extractMessage(&msg, stream);
//          if (!ret) {
//              d_handler(msg);
//          }
//      } while (!ret);
//  }
//..
// The two concrete classes implementing the 'MessageHandler' protocol can now
// provide their functionality within a function object or in free functions,
// as desired:
//..
//                       // ==================
//                       // class PrintHandler
//                       // ==================
//
//  class PrintHandler {
//
//      // DATA
//      bsl::ostream *d_stream_p;  // held, not owned
//
//    public:
//      // CREATORS
//      printHandler(bsl::ostream& stream);
//
//      // MANIPULATORS
//      void operator()(const Message &msg);
//  }
//
//  // CREATORS
//  printHandler::printHandler(bsl::ostream& stream)
//  : d_stream_p(&stream)
//  {
//  }
//
//  // MANIPULATORS
//  printHandler::operator() ((const Message& msg) {
//      (*d_stream_p) << msg << bsl::endl;
//  }
//
//                       // ====================
//                       // function freeHandler
//                       // ====================
//
//  void freeHandler(const Message &)
//  {
//     // Implementation elided.
//     // ...
//  }
//
//..
// It is now possible to store either message handler (of different types, and
// not connected by any inheritance relationship) into the polymorphic
// 'bdlf::Function' handler:
//..
//                      // ========================
//                      // function processMessages
//                      // ========================
//
//  void processMessages(bool printOnly, bsl::istream& stream)
//  {
//      MessageProcessor::MessageHandler handler;
//      if (printOnly) {
//          handler = PrintHandler(stream);
//      }
//      else {
//          handler = &processMessage;
//      }
//      MessageProcessor processor(handler);
//      processor.processMessages(stream);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>      // for swap
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {


namespace bdlf {

class Function_Rep;
template <class PROTOTYPE> struct Function_TypeList;
template <int, class RET, class ARGS> struct Function_Invoker;

struct FunctionUtil;
struct Function_RepUtil;

                             // ==================
                             // class Function_Rep
                             // ==================

class Function_Rep {
    // This is a component-private class.  Do not use.  This class provides a
    // representation for a 'Function' instance.  For detailed design
    // explanations, see the IMPLEMENTATION NOTES within the implementation
    // file.  For technical reasons, this class must be defined before
    // 'Function' (although a mere forward declaration would be all right with
    // most compilers, the Gnu compiler emits an error when trying to do
    // syntactic checking on template code even though it does not instantiate
    // template).

  public:
    // TYPES
    enum ManagerOpCode {
        // This enumeration provide values to identify operations to be
        // performed by the manager.

        e_MOVE_CONSTRUCT     = 0
      , e_COPY_CONSTRUCT     = 1
      , e_CONSTRUCT          = 2
      , e_DESTROY            = 3
      , e_IN_PLACE_DETECTION = 4
    };

    typedef bool (*Manager)(Function_Rep  *rep,
                            const void    *source,
                            ManagerOpCode  opCode);
        // 'Manager' is an alias for a pointer to a function that takes three
        // arguments: the address of a 'Function_Rep' instance 'rep', the
        // address of a 'source' (for cloning, swapping, and transferring,
        // interpreted differently according to the 'opCode'), and a value
        // 'opCode' of the 'ManagerOpcode' enumerated type,

    struct ArenaType {
        // This 'struct' defines the storage area for a functor representation.
        // Its design uses the "small-object optimization" in an attempt to
        // avoid allocations for objects which can be stored within an
        // 'ArenaType'.  In the in-place representation, the invocable, whether
        // a function pointer or function object (if it should fit in the size
        // of 'ArenaType') is stored in the buffer of 'ArenaType'.  Anything
        // bigger than 'sizeof(ArenaType)' will be stored out-of-place and its
        // address will be stored in 'd_object_p'.  Discriminating between the
        // two representations can be done by the manager with the opcode
        // 'e_IN_PLACE_DETECTION'.  As a shortcut, when the manager is 0, the
        // representation is in-place and the invocable is a function pointer
        // stored in 'd_object_p'.
        //
        // Note that union members other than 'd_object_p' are just fillers to
        // make sure that a function or member function pointer can fit without
        // allocation and that alignment is respected.  The padding after the
        // union provides additional space to allow larger function objects
        // (e.g., owning embedded arguments, such as 'Bind' objects) to be
        // constructed in-place without triggering further allocation.

        union {
            void                               (*d_func_p)(); // pointer to
                                                              // function

            void                                *d_object_p;  // pointer to
                                                              // external
                                                              // representation

            // *** inplace storage guarantees ***             // see above note
            void (bslma::Allocator::*            d_memfnPtr_p)();
            bsls::AlignmentUtil::MaxAlignedType  d_align;
        };
        char                                     d_padding[4*sizeof(void*)];
                                                              // extra padding
    };

  private:
    // DATA
    ArenaType          d_arena;       // in-place representation (if raw
                                      // function pointer or as indicated by
                                      // the manager), or pointer to external
                                      // representation

    Manager            d_manager_p;   // pointer to manager function used to
                                      // operate on function object instance
                                      // (which knows about the actual type
                                      // 'FUNC' of the function object), or 0
                                      // for raw function pointers

    bslma::Allocator  *d_allocator_p; // allocator (held, not owned)

    // FRIENDS
    friend struct FunctionUtil;
    friend struct Function_RepUtil;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Function_Rep,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // TYPES
    enum {
        // Values used as tags for dispatching, enumerated for readability.
        // Note that, although not all tags overlap, we give them all distinct
        // values.  Also note that the tag values are arbitrary.

        IS_NOT_ALLOCATOR                          = 0
      , IS_ALLOCATOR                              = 1
      , IS_FUNCTION_POINTER                       = 2
      , IS_IN_PLACE_BITWISE_COPYABLE              = 3
      , IS_OUT_OF_PLACE_BITWISE_COPYABLE          = 4
      , IS_IN_PLACE_BITWISE_MOVEABLE              = 5
      , IS_IN_PLACE                               = 6
      , IS_OUT_OF_PLACE                           = 7
      , IS_IN_PLACE_WITH_POINTER_SEMANTICS        = 8
      , IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS    = 9
      , IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS     = 10
      , IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS = 11
    };

    // CREATORS
    explicit Function_Rep(bslma::Allocator *allocator = 0);
        // Create an unset functor using the optionally specified 'allocator'
        // to supply memory.  If 'allocator' is 0, the currently installed
        // default allocator is used.

    Function_Rep(bslma::Allocator         *allocator,
                 bslmf::Tag<IS_ALLOCATOR> *,
                 bslma::Allocator         * = 0);
        // Create an unset functor representation using the specified
        // 'allocator' to supply memory.  If 'allocator' is 0, use the
        // currently installed default allocator.  The second and third are for
        // overload resolution and are unused.

    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_FUNCTION_POINTER>              *,
                 bslma::Allocator                             *allocator = 0);
    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_IN_PLACE_BITWISE_COPYABLE>     *,
                 bslma::Allocator                             *allocator = 0);
    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_OUT_OF_PLACE_BITWISE_COPYABLE> *,
                 bslma::Allocator                             *allocator = 0);
    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_IN_PLACE_BITWISE_MOVEABLE>     *,
                 bslma::Allocator                             *allocator = 0);
    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_IN_PLACE>                      *,
                 bslma::Allocator                             *allocator = 0);
    template <class FUNC>
    Function_Rep(const FUNC&                                   func,
                 bslmf::Tag<IS_OUT_OF_PLACE>                  *,
                 bslma::Allocator                             *allocator = 0);
        // Create a functor representation storing the specified 'func' object
        // of the parameterized 'FUNC' type.  Optionally specify 'allocator' to
        // supply memory.  If 'allocator' is 0, the currently installed default
        // allocator is used.  The second argument is for overload resolution
        // and is not used.

    Function_Rep(const Function_Rep&  original,
                 bslma::Allocator    *allocator = 0);
        // Create a functor representation storing the same invocable as the
        // specified 'original' function representation.  Optionally specify a
        // 'allocator' to supply memory.  If 'allocator' is 0, the currently
        // installed default allocator is used.

    ~Function_Rep();
        // Destroy this function object representation.

    // MANIPULATORS
    Function_Rep& operator=(const Function_Rep& rhs);
        // Assign to this representation the functor representation stored in
        // the specified 'rhs' representation.

    template <class FUNC>
    Function_Rep& operator=(const FUNC& func);
        // Assign to this representation the specified 'func' invocable of the
        // parameterized 'FUNC' type.

    void clear();
        // Destroy the invocable stored by this representation and reset this
        // representation to an unset (i.e., null function pointer) state.

    template <class FUNC>
    Function_Rep& load(const FUNC& func, bslma::Allocator *allocator);
        // Assign to this representation the specified 'func' invocable of the
        // parameterized 'FUNC' type, using the specified 'allocator' to supply
        // memory.
        //
        // DEPRECATED: Use method 'Function::load' instead.

    void swap(Function_Rep& other);
        // Exchange the invocable stored by this function object with that of
        // the specified 'other' modifiable function object representation.

    void transferTo(Function_Rep *target);
        // Transfer the invocable stored by this functor to the specified
        // 'target' functor.  The behavior is undefined if 'target' points to
        // this function object representation.  Note that once transferred,
        // this functor will be empty.

    // ACCESSORS
#ifndef BSLS_PLATFORM_CMP_IBM
    template <class FUNC>
    FUNC invocable(bslmf::Tag<IS_FUNCTION_POINTER> *) const;
        // Return the function pointer of the parameterized 'FUNC' type stored
        // in this representation.

    template <class FUNC>
    FUNC& invocable(bslmf::Tag<IS_IN_PLACE_WITH_POINTER_SEMANTICS> *) const;
    template <class FUNC>
    FUNC& invocable(
                   bslmf::Tag<IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS> *) const;
        // Return a reference to the non-modifiable invocable of the
        // parameterized 'FUNC' type stored in this representation.  Note that
        // 'FUNC' is not a function pointer but otherwise has pointer semantics
        // (e.g., managed or shared pointer).

    template <class FUNC>
    FUNC *invocable(bslmf::Tag<IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS> *) const;
    template <class FUNC>
    FUNC *invocable(
                bslmf::Tag<IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS> *) const;
        // Return a pointer to the invocable of the parameterized 'FUNC' type
        // stored in this representation.  Note that 'FUNC' does *not* have
        // pointer semantics (e.g., functor instance).
#else
    // On AIX (with xlC), we resort to a workaround with the class
    // 'Function_Invoker' templated by 'FUNC' and by the tag integral
    // value, whose specializations for 'IS_FUNCTION_POINTER', etc., provide
    // the 'invocable' member function.

    template <class FUNC, int INVOCABLE_TYPE>
    friend struct Function_Invocable;
#endif

    bslma::Allocator *getAllocator() const;
        // Return the address of the 'bslma::Allocator' instance used to supply
        // memory by this function object.

    bool isInplace() const;
        // Return 'true' if this representation is an inplace, and 'false'
        // otherwise.
};

                            // ===================
                            // struct FunctionUtil
                            // ===================

struct FunctionUtil {
    // This 'struct' provides a namespace for various constants and utilities
    // related to 'Function' manipulation and access.

    // TYPES
    enum {
        MAX_INPLACE_OBJECT_SIZE = sizeof(Function_Rep::ArenaType)
            // Maximum size of functor (including additional parameters) that
            // is guaranteed not to trigger a call to the allocator when
            // passing to the constructor of, or assigned to, a 'Function'
            // instance.
    };

    template <class FUNC>
    struct IsInplace {
        // Meta function to return whether a 'Function<PROTOTYPE>' object
        // created from an instance 'func' of 'FUNC' uses an inplace
        // representation or not.  Note that whether the 'Function' object is
        // created in-place or not is a property of the type 'FUNC', and does
        // *not* depend on the template argument 'PROTOTYPE' to the 'Function'.
        // All that is needed is that the type 'FUNC' offer a signature
        // compatible with 'PROTOTYPE'.  The actual type can potentially be
        // different, as illustrated by the following example:
        //..
        //  typedef double (*Prototype)(int);
        //
        //  class Quantile {
        //      // Precomputed inverse cumulative distribution function of a
        //      // normal distribution (Gaussian) with specified average and
        //      // standard deviation.  There are no known elementary
        //      // primitives, so it must be tabulated (at creation time).
        //
        //      double d_member[101];
        //    public:
        //      Quantile(double exp, double variance);
        //      double operator()(int i) { return d_member[i]; }
        //  };
        //
        //  int main() {
        //      Quantile quantile(10.0, 2.0);
        //      BSLMF_ASSERT(!FunctionUtil::IsInplace<Quantile>::VALUE);
        //      Function<Prototype> functor = quantile;
        //      assert(!func.isInplace());
        //      // ...
        //      return 0;
        //  }
        //..
        // Also see the section "Functors and Memory Allocation" in the
        // component-level documentation.

        enum {
            VALUE = sizeof(FUNC) <= FunctionUtil::MAX_INPLACE_OBJECT_SIZE
        };
    };
};

                   // =====================================
                   // struct Function_UnspecifiedBoolHelper
                   // =====================================

struct Function_UnspecifiedBoolHelper {
    // This 'struct' provides a member, 'd_member', whose pointer-to-member is
    // used to convert a 'Function' to an "unspecified boolean type".

    int d_member;
        // This data member is used solely for taking its address to return a
        // non-null pointer-to-member.  Note that the *value* of 'd_member' is
        // not used.
};

typedef int Function_UnspecifiedBoolHelper::*Function_UnspecifiedBool;
    // 'Function_UnspecifiedBool' is an alias for a pointer-to-member of the
    // 'Function_UnspecifiedBoolHelper' class.  This (opaque) type can be used
    // as an "unspecified boolean type" for converting a function object to
    // 'bool' in contexts such as 'if (fn) { ... }' without actually having a
    // conversion to 'bool' or being less-than comparable (either of which
    // would also enable undesirable implicit comparisons of function objects
    // to 'int' and less-than comparisons).

                               // ==============
                               // class Function
                               // ==============

template <class PROTOTYPE>
class Function {
    // This class provides a function object (functor) that has the same
    // signature as the parameterized 'PROTOTYPE': return a result type and
    // accept a number of up to fourteen arguments of types all specified by
    // 'PROTOTYPE'.  A "functor" is synonymous to a C/C++ function pointer, but
    // more generally than function pointers, functors can be used to invoke
    // any object ("invocable") that can be syntactically invoked as a
    // function.  Invocables include function pointers, but also user-defined
    // types that have an 'operator()'.  Functors are generally used as
    // callback functions in place of virtual functions.
    //
    // Function objects do not have value-semantics, because they are primarily
    // mechanisms used through invocation and do not have a notion of value
    // (as defined by 'operator==').

    // PRIVATE TYPES
    typedef Function_TypeList<PROTOTYPE>  Args;
        // 'Args' is an alias for the argument type list expected by an
        // invocation of this function object, as specified by the
        // parameterized 'PROTOTYPE'.

  public:
    // TYPES
    typedef typename Args::ResultType  ResultType;
        // 'ResultType' is an alias for type of the value returned by an
        // invocation of this object, as specified by the parameterized
        // 'PROTOTYPE'.

    enum {
        MAX_INPLACE_OBJECT_SIZE = FunctionUtil::MAX_INPLACE_OBJECT_SIZE
            // Maximum size of functor (including additional parameters) that
            // is guaranteed not to trigger a call to the allocator (typically
            // one function or member function pointer plus four integers).
            // Equal to 'FunctionUtil::MAX_INPLACE_OBJECT_SIZE'.
    };

  private:
    // PRIVATE TYPES
    typedef Function_Rep                                    Rep;
        // 'Rep' is an alias for 'Function_Rep', purely for brevity.

    typedef Function_Invoker<Args::ARITY, ResultType, Args> Invoker;
        // 'Invoker' is an alias for a utility that contains the 'InvokerFunc'
        // prototype, as well as concrete implementations for various 'FUNC'
        // invocables.

    typedef typename Invoker::InvokerFunc                        InvokerFunc;
        // 'InvokerFunc' is an alias for a function pointer type that returns
        // the 'ResultType', takes 'Args::ARITY' arguments of the types
        // specified by the 'Args' typelist.

    typedef typename Args::A1                                  A1;
    typedef typename Args::A2                                  A2;
    typedef typename Args::A3                                  A3;
    typedef typename Args::A4                                  A4;
    typedef typename Args::A5                                  A5;
    typedef typename Args::A6                                  A6;
    typedef typename Args::A7                                  A7;
    typedef typename Args::A8                                  A8;
    typedef typename Args::A9                                  A9;
    typedef typename Args::A10                                 A10;
    typedef typename Args::A11                                 A11;
    typedef typename Args::A12                                 A12;
    typedef typename Args::A13                                 A13;
    typedef typename Args::A14                                 A14;
        // 'AN', for 'N' from 1 up to 14, is an alias for the forwarding type
        // of the 'N'th argument expected by this function object, as specified
        // by the parameterized 'PROTOTYPE'.

    enum {
        // Values used as tags for dispatching, enumerated and repeated from
        // 'Function_Rep' for readability.

        IS_ALLOCATOR                 = Rep::IS_ALLOCATOR
      , IS_NOT_ALLOCATOR             = Rep::IS_NOT_ALLOCATOR
      , IS_FUNCTION_POINTER          = Rep::IS_FUNCTION_POINTER
      , IS_IN_PLACE_BITWISE_COPYABLE = Rep::IS_IN_PLACE_BITWISE_COPYABLE
      , IS_OUT_OF_PLACE_BITWISE_COPYABLE
                                     = Rep::IS_OUT_OF_PLACE_BITWISE_COPYABLE
      , IS_IN_PLACE_BITWISE_MOVEABLE = Rep::IS_IN_PLACE_BITWISE_MOVEABLE
      , IS_IN_PLACE                  = Rep::IS_IN_PLACE
      , IS_OUT_OF_PLACE              = Rep::IS_OUT_OF_PLACE
    };

    // DATA
    Rep         d_rep;       // representation of the functor (in-place or
                             // out-of-place)

    InvokerFunc d_invoker_p; // proxy function used to invoke this functor

    // PRIVATE CLASS METHODS
#if defined(BSLS_PLATFORM_CMP_MSVC)
    // Windows compiler cannot differentiate between the two static member
    // template overloads, even though they have different signatures due to
    // the different 'bslmf::Tag' arguments.  We resort to this alternate
    // implementation in which any 'FUNC' convertible to 'bslma::Allocator *'
    // is mapped (via 'bslmf::If') to 'bslma::Allocator *', providing an exact
    // match which is preferred to the second template.

    static InvokerFunc getInvoker(bslma::Allocator *)
#else
    template <class FUNC>
    static InvokerFunc getInvoker(bslmf::Tag<IS_ALLOCATOR> *)
#endif
        // Return 0.  Note that a 'bslma::Allocator *' is not invocable.
    {
        return 0;
    }

#if defined(BSLS_PLATFORM_CMP_MSVC)
    // We do not prefer this version of this 'getInvoker' implementation
    // because the type 'FUNC' is mentioned twice in the prototype (instead of
    // only once for the other implementation, below), leading to longer debug
    // strings (e.g., on AIX with xlC).  So we keep this version only for
    // windows.

    template <class FUNC>
    static InvokerFunc getInvoker(FUNC *)
#else
    template <class FUNC>
    static InvokerFunc getInvoker(bslmf::Tag<IS_NOT_ALLOCATOR> *)
#endif
        // Return the address of a function that can be used to invoke an
        // instance of the parameterized 'FUNC' type.  Note that a dynamic test
        // of whether the tag value is 'IS_ALLOCATOR' (and returning 0) or not
        // (and returning the value below) would return the correct result, but
        // would result in instantiating the 'Function_Invoker' in both cases,
        // leading to a compilation error for 'IS_ALLOCATOR'.
    {
        return &Invoker::template invoke<FUNC>;
    }


  private:
    // NOT IMPLEMENTED
    void operator==(const Function&) const; // = delete;
    void operator!=(const Function&) const; // = delete;
        // 'Function' objects cannot be compared for equality, as the function
        // objects they wrap might not support the equality comparison operator
        // to delegate to.  However, if these methods are not declared as
        // private, then both 'Function' objects will be implicitly converted
        // to their boolean value, and those values compared instead.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Function,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    Function();
        // Create an empty function object using the currently installed
        // default allocator to supply memory.

    template <class FUNC_OR_ALLOC>
    Function(const FUNC_OR_ALLOC& funcOrAlloc);                // IMPLICIT
        // Create either a function object having the value of the specified
        // 'funcOrAlloc' function object, or an uninitialized function object
        // having 'funcOrAlloc' as an allocator if 'FUNC_OR_ALLOC' is
        // convertible to 'bslma::Allocator*'.  For example, if 'myAllocator'
        // belongs to a class that is derived from 'bslma::Allocator', the
        // 'Function(&myAllocator)' will construct an uninitialized function
        // object using the specified 'myAllocator'.  The behavior is undefined
        // if 'funcOrAlloc' is a null function pointer.
        //
        // Note that this creator takes the place of the two constructors
        // below:
        //..
        // Function(bslma::Allocator *allocator);
        // template <class FUNC> Function(const FUNC& func);
        //..
        // Unfortunately, if we had instead provided the above two
        // constructors, then invocation of 'Function(&myAllocator)' would
        // improperly match the second constructor because the template
        // argument is always a better match than a derived-to-base conversion.

    template <class FUNC>
    Function(const FUNC& func, bslma::Allocator *allocator);
        // Create a function object having the value of the specified 'func'
        // object using the specified 'allocator' to supply memory.

    Function(const Function<PROTOTYPE>&  original,
             bslma::Allocator           *allocator = 0);
        // Create a function object storing a copy of the same invocable as the
        // specified 'original' object, using the optionally specified
        // 'allocator' to supply memory.  If 'allocator' is 0, the currently
        // installed default allocator is used.

    // MANIPULATORS
    Function<PROTOTYPE>&
    operator=(const Function<PROTOTYPE>& rhs);
        // Assign to this object the invocable of the specified 'rhs' and
        // return a reference to this modifiable function object.

    template <class FUNC>
    Function<PROTOTYPE>& operator=(const FUNC& func);
        // Assign to this function object the specified 'func' invocable object
        // of the parameterized 'FUNC' type, and return a reference to this
        // modifiable function object.

    void clear();
        // Reset this function object to an empty state.

    template <class FUNC>
    Function<PROTOTYPE> &load(const FUNC &func, bslma::Allocator *allocator);
        // Assign to this function object the specified 'func' object of the
        // parameterized 'FUNC' type, using the specified 'allocator' to supply
        // memory.  If 'allocator' is 0, continue to use the same allocator as
        // before this call.  Return a reference to this modifiable function
        // object.  The behavior is undefined if 'func' is a null function
        // pointer.  Note the deviation from the standard 'bdema' allocator
        // usage when 'allocator' is 0.
        //
        // DEPRECATED: Create with 'allocator' and simply assign 'func'
        // instead.

    void swap(Function<PROTOTYPE>& other);
        // Exchange the invocable stored by this function object with that
        // stored by the specified 'other' function object.

    void transferTo(Function<PROTOTYPE> *target);
        // Transfer the invocable stored by this function object to the
        // specified 'target' function object.  The behavior is undefined if
        // 'target' points to this function object.  Note that once
        // transferred, this function object will be empty.

    // ACCESSORS
    ResultType operator()(void) const;
        // Invoke this function object with zero arguments and return the
        // result or 'void' if this function object does not return a result.
        // If the 'PROTOTYPE' of this function does not accept zero arguments,
        // then compiler diagnostic will be emitted.  Note that the behavior is
        // undefined if this function object is empty.

    ResultType operator()(A1 a1) const;
        // Invoke this function object with the single specified argument 'a1'
        // as defined by 'PROTOTYPE' and return the result or 'void' if this
        // function object does not return a result.  If 'PROTOTYPE' does not
        // accept a single argument, then compiler diagnostic will be emitted.
        // Note that the behavior is undefined if this function object is
        // empty.

    ResultType operator()(A1 a1, A2 a2) const;
        // Invoke this function object with the two specified arguments 'a1'
        // and 'a2' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept two arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3) const;
        // Invoke this function object with the three specified arguments 'a1'
        // up to 'a3' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept three arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4) const;
        // Invoke this function object with the four specified arguments 'a1'
        // up to 'a4' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept four arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const;
        // Invoke this function object with the five specified arguments 'a1'
        // up to 'a5' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept five arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const;
        // Invoke this function object with the six specified arguments 'a1' up
        // to 'a6' as defined by 'PROTOTYPE' and return the result or 'void' if
        // this function object does not return a result.  If 'PROTOTYPE' does
        // not accept six arguments, then compiler diagnostic will be emitted.
        // Note that the behavior is undefined if this function object is
        // empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7) const;
        // Invoke this function object with the seven specified arguments 'a1'
        // up to 'a7' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept seven arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7, A8 a8) const;
        // Invoke this function object with the eight specified arguments 'a1'
        // up to 'a8' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept eight arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                          A7 a7, A8 a8, A9 a9) const;
        // Invoke this function object with the nine specified arguments 'a1'
        // up to 'a9' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept nine arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4, A5 a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10) const;
        // Invoke this function object with the ten specified arguments 'a1' up
        // to 'a10' as defined by 'PROTOTYPE' and return the result or 'void'
        // if this function object does not return a result.  If 'PROTOTYPE'
        // does not accept ten arguments, then compiler diagnostic will be
        // emitted.  Note that the behavior is undefined if this function
        // object is empty.

    ResultType operator()(A1 a1, A2 a2, A3 a3, A4  a4,  A5  a5, A6 a6,
                          A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) const;
        // Invoke this function object with the eleven specified arguments 'a1'
        // up to 'a11' as defined by 'PROTOTYPE' and return the result or
        // 'void' if this function object does not return a result.  If
        // 'PROTOTYPE' does not accept eleven arguments, then compiler
        // diagnostic will be emitted.  Note that the behavior is undefined if
        // this function object is empty.

    ResultType operator()(A1  a1, A2 a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7, A8 a8, A9 a9, A10 a10, A11 a11,
                          A12 a12) const;
        // Invoke this function object with the twelve specified arguments 'a1'
        // up to 'a12' as defined by 'PROTOTYPE' and return the result or
        // 'void' if this function object does not return a result.  If
        // 'PROTOTYPE' does not accept twelve arguments, then compiler
        // diagnostic will be emitted.  Note that the behavior is undefined if
        // this function object is empty.

    ResultType operator()(A1  a1, A2 a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12,
                          A13 a13) const;
        // Invoke this function object with the thirteen specified arguments
        // 'a1' up to 'a13' as defined by 'PROTOTYPE' and return the result or
        // 'void' if this function object does not return a result.  If
        // 'PROTOTYPE' does not accept thirteen arguments, then compiler
        // diagnostic will be emitted.  Note that the behavior is undefined if
        // this function object is empty.

    ResultType operator()(A1  a1,  A2  a2, A3 a3, A4  a4,  A5  a5,  A6  a6,
                          A7  a7,  A8  a8, A9 a9, A10 a10, A11 a11, A12 a12,
                          A13 a13, A14 a14) const;
        // Invoke this function object with the fourteen specified arguments
        // 'a1' up to 'a14' as defined by 'PROTOTYPE' and return the result or
        // 'void' if this function object does not return a result.  If
        // 'PROTOTYPE' does not accept fourteen arguments, then compiler
        // diagnostic will be emitted.  Note that the behavior is undefined if
        // this function object is empty.

    operator Function_UnspecifiedBool() const;
        // Return a value of the "unspecified bool" that evaluates to 'false'
        // if this function object is empty, and 'true' otherwise.  Note that
        // this conversion operator allows a function object to be used within
        // a conditional context (e.g., within an 'if' or 'while' statement),
        // but does *not* allow function objects to be compared (e.g., via '<'
        // or '>').

    bslma::Allocator *getAllocator() const;
        // Return the address of the 'bslma::Allocator' instance used to supply
        // memory by this function object.

    bool isInplace() const;
        // Returns whether this instance uses an inplace representation or not.
        // See the section 'Function objects and allocation' in the
        // component-level documentation.  Note that this function returns
        // 'true' for an empty function object.
};

// FREE FUNCTIONS
template <class PROTOTYPE>
void swap(Function<PROTOTYPE>& a, Function<PROTOTYPE>& b);
    // Swap the values of the specified 'a' and 'b' objects.

}  // close package namespace

// ---- Anything below this line is implementation specific.  Do not use.  ----

#ifdef BSLS_PLATFORM_CMP_IBM

                      // ===============================
                      // struct bdlf::Function_Invocable
                      // ===============================


namespace bdlf {template <class FUNC, int INVOCABLE_TYPE>
struct Function_Invocable;

// SPECIALIZATIONS
template <class FUNC>
struct Function_Invocable<FUNC,
                               Function_Rep::IS_FUNCTION_POINTER>
                                                   : public Function_Rep {
    // ACCESSORS
    FUNC invocable() const
    {
        return *(const FUNC *)&d_arena.d_func_p;
    }
};

template <class FUNC>
struct Function_Invocable<FUNC,
                         Function_Rep::IS_IN_PLACE_WITH_POINTER_SEMANTICS>
                                                   : public Function_Rep {
    // ACCESSORS
    FUNC& invocable() const
    {
        return *const_cast<FUNC *>((const FUNC *)&d_arena);
    }
};

template <class FUNC>
struct Function_Invocable<FUNC,
                     Function_Rep::IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS>
                                                   : public Function_Rep {
    // ACCESSORS
    FUNC& invocable() const
    {
        return *(FUNC *)d_arena.d_object_p;
    }
};

template <class FUNC>
struct Function_Invocable<FUNC,
                      Function_Rep::IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS>
                                                   : public Function_Rep {
    // ACCESSORS
    FUNC *invocable() const
    {
        return const_cast<FUNC *>((const FUNC *)&d_arena);
    }
};

template <class FUNC>
struct Function_Invocable<FUNC,
                  Function_Rep::IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS>
                                                   : public Function_Rep {
    // ACCESSORS
    FUNC *invocable() const
    {
        return (FUNC *)d_arena.d_object_p;
    }
};
}  // close package namespace

#endif

namespace bdlf {
                          // =======================
                          // struct Function_RepUtil
                          // =======================

struct Function_RepUtil {
    // This implementation-private 'struct' provides a namespace for functions
    // managing 'Function_Rep' instances that store an invocable of a
    // parameterized 'FUNC' type.  Note that, for a given 'FUNC' type, only a
    // *single* manager (out of the possible five below) will be instantiated,
    // typically the one most optimized for the given 'FUNC' type.  Also note
    // that while managers are usually parameterized by the 'FUNC' type, in the
    // case of bitwise-copyable objects, it is enough to parameterize by
    // 'sizeof(FUNC)' since all such type can be manipulated (using bitwise
    // copy) in the same way as 'char [sizeof(FUNC)]'.  Finally, note that
    // there is no advantage to treat bitwise-moveable out-of-place
    // representations any differently that for non-bitwise-moveable types (the
    // only operation that would benefit, 'e_MOVE_CONSTRUCT', is already
    // reduced to a simple pointer copy).

    // TYPES
    typedef Function_Rep::ManagerOpCode  OpCode;

    // CLASS METHODS
    template <int FUNC_SIZE>
    static bool inplaceBitwiseCopyableManager(
                                          Function_Rep                *rep,
                                          const void                  *source,
                                          Function_Rep::ManagerOpCode  opCode);
        // Manage the in-place representation of the parameterized 'FUNC_SIZE'
        // at the specified 'rep' address (the actual type does not matter),
        // using the data at the specified 'source' address, performing the
        // operation with the specified 'opCode' and optimizing
        // 'e_MOVE_CONSTRUCT' and 'e_COPY_CONSTRUCT' using bitwise copy.  Note
        // that 'source' is interpreted either as 'const char[FUNC_SIZE]' or
        // 'Function_Rep *' according to the 'opCode'.

    template <int FUNC_SIZE>
    static bool outofplaceBitwiseCopyableManager(
                                          Function_Rep                *rep,
                                          const void                  *source,
                                          Function_Rep::ManagerOpCode  opCode);
        // Manage the out-of-place representation of the parameterized
        // 'FUNC_SIZE' at the specified 'rep' address (the actual type does not
        // matter), using the data at the specified 'source' address,
        // performing the operation with the specified 'opCode' and optimizing
        // 'e_MOVE_CONSTRUCT' and 'e_COPY_CONSTRUCT' using bitwise copy.  Note
        // that 'source' is interpreted either as 'const char[FUNC_SIZE]' or
        // 'Function_Rep *' according to the 'opCode'.

    template <class FUNC>
    static bool inplaceBitwiseMoveableManager(
                                          Function_Rep                *rep,
                                          const void                  *source,
                                          Function_Rep::ManagerOpCode  opCode);
        // Manage the in-place representation of the parameterized 'FUNC'
        // instance at the specified 'rep' address, using the data at the
        // specified 'source' address, performing the operation with the
        // specified 'opCode' and optimizing 'e_MOVE_CONSTRUCT' (but not
        // 'e_COPY_CONSTRUCT') using bitwise copy.  Note that 'source' is
        // interpreted either as 'const FUNC *' or 'Function_Rep *' according
        // to the 'opCode'.

    template <class FUNC>
    static bool inplaceManager(Function_Rep                *rep,
                               const void                  *source,
                               Function_Rep::ManagerOpCode  opCode);
        // Manage the in-place representation of the parameterized 'FUNC'
        // instance at the specified 'rep' address, using the data at the
        // specified 'source' address, performing the operation with the
        // specified 'opCode'.  Note that 'source' is interpreted either as
        // 'const FUNC *' or 'Function_Rep *' according to the 'opCode'.

    template <class FUNC>
    static bool outofplaceManager(Function_Rep                *rep,
                                  const void                  *source,
                                  Function_Rep::ManagerOpCode  opCode);
        // Manage the out-of-place representation of the parameterized 'FUNC'
        // instance at the specified 'rep' address, using the data at the
        // specified 'source' address, performing the operation with the
        // specified 'opCode'.  Note that 'source' is interpreted either as
        // 'const FUNC *' or 'Function_Rep *' according to the 'opCode'.
};

                          // ========================
                          // struct Function_TypeList
                          // ========================

template <class PROTOTYPE>
struct Function_TypeList {
    // This implementation-private class provides the type definition for the
    // up-to-14 parameters.

    // TYPES
    typedef
        typename bslmf::FunctionPointerTraits<PROTOTYPE>::ResultType
                                                                    ResultType;
        // 'ResultType' is an alias for the type returned by an invocable
        // conforming to the parameterized 'PROTOTYPE'.

    typedef
        typename bslmf::FunctionPointerTraits<PROTOTYPE>::ArgumentList Args;
        // 'Args' is an alias for the type list of arguments expected by an
        // invocable conforming to the parameterized 'PROTOTYPE'.

    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<1, Args>::TypeOrDefault>::Type  A1;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<2, Args>::TypeOrDefault>::Type  A2;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<3, Args>::TypeOrDefault>::Type  A3;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<4, Args>::TypeOrDefault>::Type  A4;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<5, Args>::TypeOrDefault>::Type  A5;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<6, Args>::TypeOrDefault>::Type  A6;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<7, Args>::TypeOrDefault>::Type  A7;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<8, Args>::TypeOrDefault>::Type  A8;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<9, Args>::TypeOrDefault>::Type  A9;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<10, Args>::TypeOrDefault>::Type A10;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<11, Args>::TypeOrDefault>::Type A11;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<12, Args>::TypeOrDefault>::Type A12;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<13, Args>::TypeOrDefault>::Type A13;
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<14, Args>::TypeOrDefault>::Type A14;
        // 'AN', with 'N' from 1 to 14, is an alias for the forwarding type of
        // the 'N'th argument expected by an invocable conforming to the
        // parameterized 'PROTOTYPE'.

    enum { ARITY = Args::LENGTH };
};
}  // close package namespace

                       // =============================
                       // struct bdlf::Function_Invoker
                       // =============================

#define BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)                               \
    enum {                                                                    \
        IS_IN_PLACE           = bdlf::FunctionUtil::IsInplace<FUNC>::VALUE    \
      , HAS_POINTER_SEMANTICS = bslalg::HasTrait<FUNC,                        \
                                 bslalg::TypeTraitHasPointerSemantics>::VALUE \
      , INVOKER_TAG           = bslmf::IsFunctionPointer<FUNC>::VALUE         \
                              ? (int)bdlf::Function_Rep::IS_FUNCTION_POINTER  \
                              : IS_IN_PLACE ? (HAS_POINTER_SEMANTICS          \
         ? (int)bdlf::Function_Rep::IS_IN_PLACE_WITH_POINTER_SEMANTICS        \
         : (int)bdlf::Function_Rep::IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS)    \
                                            : (HAS_POINTER_SEMANTICS          \
         ? (int)bdlf::Function_Rep::IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS    \
         : (int)bdlf::Function_Rep::IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS)\
    };
    // This private local macro is used to define an 'INVOKER_TAG' enumerated
    // value which is equal to one of the following values depending on the
    // parameterized 'FUNC' argument (the name is self-explanatory):
    //..
    //  IS_FUNCTION_POINTER
    //  IS_IN_PLACE_WITH_POINTER_SEMANTICS
    //  IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS
    //  IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS
    //  IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS
    //..

#ifndef BSLS_PLATFORM_CMP_IBM

#define BDLF_FUNCTION_INVOKER(FUNC, rep) \
                           (rep->invocable<FUNC>((bslmf::Tag<INVOKER_TAG> *)0))
    // This private local macro expands to a call to 'rep->invocable<FUNC>'
    // with an argument of type 'bslmf::Tag<INVOKER_TAG> *', thus enabling
    // overload resolution to select the correct implementation of
    // 'bdlf::Function_Rep::invocable'.  The behavior is undefined unless the
    // 'BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)' macro has been expanded in the
    // same scope prior to using this expression.
#else

#define BDLF_FUNCTION_INVOKER(FUNC, rep) \
      (((const bdlf::Function_Invocable<FUNC, INVOKER_TAG> *)rep)->invocable())
    // This private local macro expands to a call to 'repImpl->invocable',
    // where 'repImpl' is 'rep' reinterpreted as the 'bdlf::Function_Invocable'
    // object corresponding to the 'FUNC' invocable type with its appropriate
    // traits.  The behavior is undefined unless the
    // 'BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)' macro has been expanded in the
    // same scope prior to using this expression.

#endif

namespace bdlf {
// SPECIALIZATIONS
template <class RET, class ARGS>
struct Function_Invoker<0, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept zero arguments.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const*);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))();
    }
};

template <class ARGS>
struct Function_Invoker<0, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept zero arguments.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const*);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))();
    }
};

template <class RET, class ARGS>
struct Function_Invoker<1, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept a single argument of type 'A1'.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *, typename ARGS::A1);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep, typename ARGS::A1 p1)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1);
    }
};

template <class ARGS>
struct Function_Invoker<1, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept a single argument of type 'A1'.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *, typename ARGS::A1);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep, typename ARGS::A1 p1)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<2, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept two arguments of type 'A1', 'A2'
    // respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2);
    }
};

template <class ARGS>
struct Function_Invoker<2, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept two arguments of type 'A1',
    // 'A2' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<3, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept three arguments of type 'A1',
    // 'A2', 'A3' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3);
    }
};

template <class ARGS>
struct Function_Invoker<3, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept three arguments of type 'A1',
    // 'A2', 'A3' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<4, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept four arguments of type 'A1',
    // 'A2', 'A3', 'A4' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4);
    }
};

template <class ARGS>
struct Function_Invoker<4, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept four arguments of type 'A1',
    // 'A2', 'A3', 'A4' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<5, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept five arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4, p5);
    }
};

template <class ARGS>
struct Function_Invoker<5, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept five arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4, p5);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<6, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept six arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4, p5, p6);
    }
};

template <class ARGS>
struct Function_Invoker<6, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept six arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4, p5, p6);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<7, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept seven arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', typename 'A7' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                                   p1, p2, p3, p4, p5, p6, p7);
    }
};

template <class ARGS>
struct Function_Invoker<7, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept seven arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', typename 'A7' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(p1, p2, p3, p4, p5, p6, p7);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<8, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept eight arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                           p1, p2, p3, p4, p5, p6, p7, p8);
    }
};

template <class ARGS>
struct Function_Invoker<8, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept eight arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                           p1, p2, p3, p4, p5, p6, p7, p8);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<9, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept nine arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                       p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }
};

template <class ARGS>
struct Function_Invoker<9, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept nine arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                       p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<10, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept ten arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9,
                               typename ARGS::A10);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9,
                      typename ARGS::A10  p10)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                  p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }
};

template <class ARGS>
struct Function_Invoker<10, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept ten arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9,
                                typename ARGS::A10);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9,
                       typename ARGS::A10  p10)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                  p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<11, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept eleven arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11'
    // respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9,
                               typename ARGS::A10,
                               typename ARGS::A11);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9,
                      typename ARGS::A10  p10,
                      typename ARGS::A11  p11)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                 p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }
};

template <class ARGS>
struct Function_Invoker<11, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept eleven arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11'
    // respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9,
                                typename ARGS::A10,
                                typename ARGS::A11);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9,
                       typename ARGS::A10  p10,
                       typename ARGS::A11  p11)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                                 p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<12, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accept twelve arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11', 'A12'
    // respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9,
                               typename ARGS::A10,
                               typename ARGS::A11,
                               typename ARGS::A12);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9,
                      typename ARGS::A10  p10,
                      typename ARGS::A11  p11,
                      typename ARGS::A12  p12)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                        p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    }
};

template <class ARGS>
struct Function_Invoker<12, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accept twelve arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11', 'A12'
    // respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9,
                                typename ARGS::A10,
                                typename ARGS::A11,
                                typename ARGS::A12);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9,
                       typename ARGS::A10  p10,
                       typename ARGS::A11  p11,
                       typename ARGS::A12  p12)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                        p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<13, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'RET' and accepts thirteen arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11', 'A12',
    // 'A13' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9,
                               typename ARGS::A10,
                               typename ARGS::A11,
                               typename ARGS::A12,
                               typename ARGS::A13);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9,
                      typename ARGS::A10  p10,
                      typename ARGS::A11  p11,
                      typename ARGS::A12  p12,
                      typename ARGS::A13  p13)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                   p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    }
};

template <class ARGS>
struct Function_Invoker<13, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return 'void' and accepts thirteen arguments of type
    // 'A1', 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11',
    // 'A12', 'A13' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9,
                                typename ARGS::A10,
                                typename ARGS::A11,
                                typename ARGS::A12,
                                typename ARGS::A13);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9,
                       typename ARGS::A10  p10,
                       typename ARGS::A11  p11,
                       typename ARGS::A12  p12,
                       typename ARGS::A13  p13)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                       p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    }
};

template <class RET, class ARGS>
struct Function_Invoker<14, RET, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return RET and accept fourteen arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11', 'A12',
    // 'A13', 'A14' respectively.

    // TYPES
    typedef RET (*InvokerFunc)(Function_Rep const *,
                               typename ARGS::A1,
                               typename ARGS::A2,
                               typename ARGS::A3,
                               typename ARGS::A4,
                               typename ARGS::A5,
                               typename ARGS::A6,
                               typename ARGS::A7,
                               typename ARGS::A8,
                               typename ARGS::A9,
                               typename ARGS::A10,
                               typename ARGS::A11,
                               typename ARGS::A12,
                               typename ARGS::A13,
                               typename ARGS::A14);

    // CLASS METHODS
    template <class FUNC>
    static RET invoke(Function_Rep const *rep,
                      typename ARGS::A1   p1,
                      typename ARGS::A2   p2,
                      typename ARGS::A3   p3,
                      typename ARGS::A4   p4,
                      typename ARGS::A5   p5,
                      typename ARGS::A6   p6,
                      typename ARGS::A7   p7,
                      typename ARGS::A8   p8,
                      typename ARGS::A9   p9,
                      typename ARGS::A10  p10,
                      typename ARGS::A11  p11,
                      typename ARGS::A12  p12,
                      typename ARGS::A13  p13,
                      typename ARGS::A14  p14)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        return (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                  p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
    }
};

template <class ARGS>
struct Function_Invoker<14, void, ARGS> {
    // This implementation-private class defines an invoker specialized for
    // functions that return void and accept fourteen arguments of type 'A1',
    // 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8', 'A9', 'A10', 'A11', 'A12',
    // 'A13', 'A14' respectively.

    // TYPES
    typedef void (*InvokerFunc)(Function_Rep const *,
                                typename ARGS::A1,
                                typename ARGS::A2,
                                typename ARGS::A3,
                                typename ARGS::A4,
                                typename ARGS::A5,
                                typename ARGS::A6,
                                typename ARGS::A7,
                                typename ARGS::A8,
                                typename ARGS::A9,
                                typename ARGS::A10,
                                typename ARGS::A11,
                                typename ARGS::A12,
                                typename ARGS::A13,
                                typename ARGS::A14);

    // CLASS METHODS
    template <class FUNC>
    static void invoke(Function_Rep const *rep,
                       typename ARGS::A1   p1,
                       typename ARGS::A2   p2,
                       typename ARGS::A3   p3,
                       typename ARGS::A4   p4,
                       typename ARGS::A5   p5,
                       typename ARGS::A6   p6,
                       typename ARGS::A7   p7,
                       typename ARGS::A8   p8,
                       typename ARGS::A9   p9,
                       typename ARGS::A10  p10,
                       typename ARGS::A11  p11,
                       typename ARGS::A12  p12,
                       typename ARGS::A13  p13,
                       typename ARGS::A14  p14)
    {
        BDLF_FUNCTION_DECLARE_INVOKER_TAG(FUNC)
        (*BDLF_FUNCTION_INVOKER(FUNC, rep))(
                  p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
    }
};
}  // close package namespace

#undef BDLF_FUNCTION_DECLARE_INVOKER_TAG
#undef BDLF_FUNCTION_INVOKER

namespace bdlf {
// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // class Function
                               // --------------

// CREATORS
template <class PROTOTYPE>
inline
Function<PROTOTYPE>::Function()
: d_rep(0)
, d_invoker_p(0)
{
}

template <class PROTOTYPE>
template <class FUNC_OR_ALLOC>
inline
Function<PROTOTYPE>::Function(const FUNC_OR_ALLOC& funcOrAlloc)
: d_rep(funcOrAlloc,
        (bslmf::Tag<bslmf::IsConvertible<FUNC_OR_ALLOC,
                                       bslma::Allocator *>::VALUE
                                             ? IS_ALLOCATOR
                 : bslmf::IsFunctionPointer<FUNC_OR_ALLOC>::VALUE
                                             ? IS_FUNCTION_POINTER
                 : FunctionUtil::IsInplace<FUNC_OR_ALLOC>::VALUE ?
                       (bslalg::HasTrait<FUNC_OR_ALLOC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_COPYABLE
                      : bslalg::HasTrait<FUNC_OR_ALLOC,
                                       bslalg::TypeTraitBitwiseMoveable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_MOVEABLE
                                            : IS_IN_PLACE)
                 : bslalg::HasTrait<FUNC_OR_ALLOC,
                                   bslalg::TypeTraitBitwiseCopyable>::VALUE
                                             ? IS_OUT_OF_PLACE_BITWISE_COPYABLE
                                             : IS_OUT_OF_PLACE> *)0,
        (bslma::Allocator *)0)
#if defined(BSLS_PLATFORM_CMP_MSVC)
, d_invoker_p(getInvoker((typename bslmf::If<bslmf::IsConvertible<
                                                    FUNC_OR_ALLOC,
                                                    bslma::Allocator *>::VALUE,
                                            bslma::Allocator,
                                            FUNC_OR_ALLOC>::Type *)0))
#else
, d_invoker_p(getInvoker<FUNC_OR_ALLOC>((bslmf::Tag<bslmf::IsConvertible<
                                                     FUNC_OR_ALLOC,
                                                     bslma::Allocator *>::VALUE
                                                      ? IS_ALLOCATOR
                                                      : IS_NOT_ALLOCATOR> *)0))
#endif
{
}

template <class PROTOTYPE>
template <class FUNC>
inline
Function<PROTOTYPE>::Function(const FUNC& func, bslma::Allocator *allocator)
: d_rep(func,
        (bslmf::Tag<bslmf::IsFunctionPointer<FUNC>::VALUE
                                             ? IS_FUNCTION_POINTER
                 : FunctionUtil::IsInplace<FUNC>::VALUE ?
                       (bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_COPYABLE
                      : bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseMoveable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_MOVEABLE
                                            : IS_IN_PLACE)
                 : bslalg::HasTrait<FUNC,
                                    bslalg::TypeTraitBitwiseCopyable>::VALUE
                                             ? IS_OUT_OF_PLACE_BITWISE_COPYABLE
                                             : IS_OUT_OF_PLACE> *)0,
        allocator)
#if defined(BSLS_PLATFORM_CMP_MSVC)
, d_invoker_p(getInvoker((FUNC *)0))
#else
, d_invoker_p(getInvoker<FUNC>((bslmf::Tag<IS_NOT_ALLOCATOR>*)0))
#endif
{
}

template <class PROTOTYPE>
inline
Function<PROTOTYPE>::Function(const Function<PROTOTYPE>&  original,
                              bslma::Allocator           *allocator)
: d_rep(original.d_rep, allocator)
, d_invoker_p(original.d_invoker_p)
{
}

// MANIPULATORS
template <class PROTOTYPE>
inline
Function<PROTOTYPE>&
Function<PROTOTYPE>::operator=(const Function<PROTOTYPE>& rhs)
{
    d_rep       = rhs.d_rep;
    d_invoker_p = rhs.d_invoker_p;
    return *this;
}

template <class PROTOTYPE>
template <class FUNC>
inline
Function<PROTOTYPE>& Function<PROTOTYPE>::operator=(const FUNC& func)
{
    d_rep       = func;
#if defined(BSLS_PLATFORM_CMP_MSVC)
    d_invoker_p = getInvoker((FUNC *)0);
#else
    d_invoker_p = getInvoker<FUNC>((bslmf::Tag<IS_NOT_ALLOCATOR>*)0);
#endif
    return *this;
}

template <class PROTOTYPE>
template <class FUNC>
inline
Function<PROTOTYPE>& Function<PROTOTYPE>::load(const FUNC&       func,
                                               bslma::Allocator *allocator)
{
    d_rep.load(func, allocator);
#if defined(BSLS_PLATFORM_CMP_MSVC)
    d_invoker_p = getInvoker((FUNC *)0);
#else
    d_invoker_p = getInvoker<FUNC>((bslmf::Tag<IS_NOT_ALLOCATOR>*)0);
#endif
    return *this;
}

template <class PROTOTYPE>
inline
void Function<PROTOTYPE>::swap(Function<PROTOTYPE>& other)
{
    d_rep.swap(other.d_rep);
    bslalg::SwapUtil::swap(&d_invoker_p, &other.d_invoker_p);
}

template <class PROTOTYPE>
inline
void Function<PROTOTYPE>::transferTo(Function<PROTOTYPE> *target)
{
    BSLS_ASSERT_SAFE(target != this);

    d_rep.transferTo(&target->d_rep);
    target->d_invoker_p = d_invoker_p;
    d_invoker_p         = 0;
}

template <class PROTOTYPE>
inline
void Function<PROTOTYPE>::clear()
{
    d_rep.clear();
    d_invoker_p = 0;
}

// ACCESSORS
template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(void) const
{
    return (*this->d_invoker_p)(&this->d_rep);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3, a4);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3, a4, a5);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4,A5 a5, A6 a6) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3, a4, a5, a6);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                                A7 a7) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3, a4, a5, a6, a7);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                                A7 a7, A8 a8) const
{
    return (*this->d_invoker_p)(&this->d_rep, a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6,
                                A7 a7, A8 a8, A9 a9) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                                a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1 a1, A2 a2, A3 a3, A4  a4, A5 a5, A6 a6,
                                A7 a7, A8 a8, A9 a9, A10 a10) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);

}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1  a1, A2 a2, A3 a3, A4 a4, A5  a5,
                                A6  a6, A7 a7, A8 a8, A9 a9, A10 a10,
                                A11 a11) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);

}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1  a1,  A2  a2, A3 a3, A4 a4, A5  a5,
                                A6  a6,  A7  a7, A8 a8, A9 a9, A10 a10,
                                A11 a11, A12 a12) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                            a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);

}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1  a1,  A2  a2,  A3  a3, A4 a4, A5  a5,
                                A6  a6,  A7  a7,  A8  a8, A9 a9, A10 a10,
                                A11 a11, A12 a12, A13 a13) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                       a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}

template <class PROTOTYPE>
inline
typename Function<PROTOTYPE>::ResultType
Function<PROTOTYPE>::operator()(A1  a1,  A2  a2,  A3  a3,  A4  a4,
                                A5  a5,  A6  a6,  A7  a7,  A8  a8,
                                A9  a9,  A10 a10, A11 a11, A12 a12,
                                A13 a13, A14 a14) const
{
    return (*this->d_invoker_p)(&this->d_rep,
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

template <class PROTOTYPE>
inline
Function<PROTOTYPE>::operator Function_UnspecifiedBool() const
{
    return this->d_invoker_p
         ? &Function_UnspecifiedBoolHelper::d_member
         : 0;
}

template <class PROTOTYPE>
inline
bslma::Allocator *Function<PROTOTYPE>::getAllocator() const
{
    return d_rep.getAllocator();
}

template <class PROTOTYPE>
inline
bool Function<PROTOTYPE>::isInplace() const
{
    return d_rep.isInplace();
}

// FREE FUNCTIONS
template <class PROTOTYPE>
inline
void swap(Function<PROTOTYPE>& a, Function<PROTOTYPE>& b)
{
    a.swap(b);
}

                            // -------------------
                            // struct Function_Rep
                            // -------------------

// CREATORS
inline
Function_Rep::Function_Rep(bslma::Allocator *allocator)
: d_manager_p(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    d_arena.d_func_p = 0;
}

inline
Function_Rep::Function_Rep(bslma::Allocator         *allocator,
                           bslmf::Tag<IS_ALLOCATOR> *,
                           bslma::Allocator         *)
: d_manager_p(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    d_arena.d_func_p = 0;
}

template <class FUNC>
inline
Function_Rep::Function_Rep(const FUNC&                      func,
                           bslmf::Tag<IS_FUNCTION_POINTER> *,
                           bslma::Allocator                *allocator)
: d_manager_p(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    d_arena.d_func_p = (void (*)())func;
}

template <class FUNC>
inline
Function_Rep::Function_Rep(const FUNC&                               func,
                           bslmf::Tag<IS_IN_PLACE_BITWISE_COPYABLE> *,
                           bslma::Allocator                         *allocator)
: d_manager_p(
           &Function_RepUtil::inplaceBitwiseCopyableManager<sizeof(FUNC)>)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    bsl::memcpy((void *)&this->d_arena, (const void *)&func, sizeof(FUNC));
}

template <class FUNC>
inline
Function_Rep::Function_Rep(
                       const FUNC&                                   func,
                       bslmf::Tag<IS_OUT_OF_PLACE_BITWISE_COPYABLE> *,
                       bslma::Allocator                             *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    Function_RepUtil::outofplaceBitwiseCopyableManager<sizeof(FUNC)>(
                                                           this,
                                                           (const void *)&func,
                                                           e_CONSTRUCT);

    // Initialized *after* the manager call, in case the allocator throws.
    // See "Notes" in the component implementation file (.cpp).

    d_manager_p =
        &Function_RepUtil::outofplaceBitwiseCopyableManager<sizeof(FUNC)>;
}

template <class FUNC>
inline
Function_Rep::Function_Rep(
                           const FUNC&                               func,
                           bslmf::Tag<IS_IN_PLACE_BITWISE_MOVEABLE> *,
                           bslma::Allocator                         *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    Function_RepUtil::inplaceBitwiseMoveableManager<FUNC>(this,
                                                          (const void *)&func,
                                                          e_CONSTRUCT);

    // Initialized *after* the manager call, in case the ctor throws.  See
    // "Notes" in the component implementation file (.cpp).

    d_manager_p = &Function_RepUtil::inplaceBitwiseMoveableManager<FUNC>;
}

template <class FUNC>
inline
Function_Rep::Function_Rep(const FUNC&              func,
                           bslmf::Tag<IS_IN_PLACE> *,
                           bslma::Allocator        *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    Function_RepUtil::inplaceManager<FUNC>(this,
                                           (const void *)&func,
                                           e_CONSTRUCT);

    // Initialized *after* the manager call, in case the ctor throws.  See
    // "Notes" in the component implementation file (.cpp).

    d_manager_p = &Function_RepUtil::inplaceManager<FUNC>;
}

template <class FUNC>
inline
Function_Rep::Function_Rep(const FUNC&                  func,
                           bslmf::Tag<IS_OUT_OF_PLACE> *,
                           bslma::Allocator            *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    Function_RepUtil::outofplaceManager<FUNC>(this,
                                              (const void *)&func,
                                              e_CONSTRUCT);

    // Initialized *after* the manager call, in case the ctor throws.  See
    // "Notes" in the component implementation file (.cpp).

    d_manager_p = &Function_RepUtil::outofplaceManager<FUNC>;
}

// MANIPULATORS
template <class FUNC>
Function_Rep& Function_Rep::operator=(const FUNC& func)
{
    enum {
        CREATION_TAG = bslmf::IsFunctionPointer<FUNC>::VALUE
                                             ? IS_FUNCTION_POINTER
                     : FunctionUtil::IsInplace<FUNC>::VALUE ?
                       (bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_COPYABLE
                                            : IS_IN_PLACE)
                     : bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                             ? IS_OUT_OF_PLACE_BITWISE_COPYABLE
                                             : IS_OUT_OF_PLACE
    };

    Function_Rep(func, (bslmf::Tag<CREATION_TAG> *)0, d_allocator_p)
        .swap(*this);
    return *this;
}

template <class FUNC>
Function_Rep& Function_Rep::load(const FUNC& func, bslma::Allocator *allocator)
{
    // DEPRECATED
    //
    // This method's implementation remains not exception-safe for two
    // reasons:
    // - it is not possible to change the object's allocator with the copy-swap
    //   idiom (swap doesn't affect the allocator)
    // - it requires some custom exception-safety logic which may not be worth
    //   doing considering that this method is deprecated.

    this->~Function_Rep();
    d_arena.d_func_p = 0;

    enum {
        CREATION_TAG = bslmf::IsFunctionPointer<FUNC>::VALUE
                                             ? IS_FUNCTION_POINTER
                     : FunctionUtil::IsInplace<FUNC>::VALUE ?
                        bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                            ? IS_IN_PLACE_BITWISE_COPYABLE
                                            : IS_IN_PLACE
                     : bslalg::HasTrait<FUNC,
                                       bslalg::TypeTraitBitwiseCopyable>::VALUE
                                             ? IS_OUT_OF_PLACE_BITWISE_COPYABLE
                                             : IS_OUT_OF_PLACE
    };

    new(this) Function_Rep(func, (bslmf::Tag<CREATION_TAG> *)0, allocator);

    return *this;
}
}  // close package namespace

// ACCESSORS
#ifndef BSLS_PLATFORM_CMP_IBM

namespace bdlf {template <class FUNC>
inline
FUNC Function_Rep::invocable(bslmf::Tag<IS_FUNCTION_POINTER> *) const
{
    return *(const FUNC *)&d_arena.d_func_p;
}

template <class FUNC>
inline
FUNC& Function_Rep::invocable(
                        bslmf::Tag<IS_IN_PLACE_WITH_POINTER_SEMANTICS> *) const
{
    return *const_cast<FUNC *>((const FUNC *)&d_arena);
}

template <class FUNC>
inline
FUNC& Function_Rep::invocable(
                    bslmf::Tag<IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS> *) const
{
    return *(FUNC *)d_arena.d_object_p;
}

template <class FUNC>
inline
FUNC *Function_Rep::invocable(
                     bslmf::Tag<IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS> *) const
{
    return const_cast<FUNC *>((const FUNC *)&d_arena);
}

template <class FUNC>
inline
FUNC *Function_Rep::invocable(
                 bslmf::Tag<IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS> *) const
{
    return (FUNC *)d_arena.d_object_p;
}
}  // close package namespace
#endif

namespace bdlf {
inline
bslma::Allocator *Function_Rep::getAllocator() const
{
    return d_allocator_p;
}

inline
bool Function_Rep::isInplace() const
{
    if (!d_manager_p) {
        return true;                                                  // RETURN
    }
    return (*d_manager_p)(0, (const void *)0, e_IN_PLACE_DETECTION);
}

                          // -----------------------
                          // struct Function_RepUtil
                          // -----------------------

// CLASS METHODS
template <int FUNC_SIZE>
bool Function_RepUtil::inplaceBitwiseCopyableManager(
                                           Function_Rep                *rep,
                                           const void                  *source,
                                           Function_Rep::ManagerOpCode  opCode)
{
    // NOTE: This function is optimized for space (to minimize template bloat),
    // hence the code that can be shared, such as 'e_COPY_CONSTRUCT' and
    // 'e_CONSTRUCT', is shared via 'FALL THROUGH'.

    switch(opCode) {
      case Function_Rep::e_MOVE_CONSTRUCT:              // FALL THROUGH
      case Function_Rep::e_COPY_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // instance, and 'rep' must be empty (or a function pointer).  Note
        // that 'source' is left in a empty state but must be zero-initialized
        // by the caller upon return.

        source = &((const Function_Rep *)source)->d_arena;
      }                                                         // FALL THROUGH
      case Function_Rep::e_CONSTRUCT: {
        // 'source' is a 'FUNC *', and 'rep' must be empty (or a function
        // pointer).

        bsl::memcpy((void *)&rep->d_arena, source, FUNC_SIZE);
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_DESTROY: {
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_IN_PLACE_DETECTION: {
        return true;                                                  // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
    }
    return false;  // quell warnings
}

template <int FUNC_SIZE>
bool Function_RepUtil::outofplaceBitwiseCopyableManager(
                                           Function_Rep                *rep,
                                           const void                  *source,
                                           Function_Rep::ManagerOpCode  opCode)
{
    // NOTE: This function is optimized for space (to minimize template bloat),
    // hence the code that can be shared, such as 'e_COPY_CONSTRUCT' and
    // 'e_CONSTRUCT', is shared via 'FALL THROUGH'.

    switch(opCode) {
      case Function_Rep::e_MOVE_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // pointer, and 'rep' must be empty (or a function pointer).  Note
        // that 'source' is left in a empty state but must be zero-initialized
        // by the caller upon return.

        const Function_Rep *sourceRep = (const Function_Rep *)source;
        BSLS_ASSERT_SAFE(sourceRep->d_allocator_p == rep->d_allocator_p);

        rep->d_arena.d_object_p = sourceRep->d_arena.d_object_p;
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_COPY_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // pointer, and 'rep' must be empty (or a function pointer).

        source = ((const Function_Rep *)source)->d_arena.d_object_p;
      }                                                         // FALL THROUGH
      case Function_Rep::e_CONSTRUCT: {
        // 'source' is a 'FUNC *', and 'rep' must be empty (or a function
        // pointer).

        void *tempPtr = (void *)rep->d_allocator_p->allocate(FUNC_SIZE);
        bsl::memcpy(tempPtr, source, FUNC_SIZE);
        rep->d_arena.d_object_p = tempPtr;
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_DESTROY: {
        // 'rep' stores a 'FUNC' instance, and 'source' is irrelevant.

        rep->d_allocator_p->deallocate(rep->d_arena.d_object_p);
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_IN_PLACE_DETECTION: {
        return false;                                                 // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
    }
    return true;  // quell warnings
}

template <class FUNC>
bool Function_RepUtil::inplaceBitwiseMoveableManager(
                                           Function_Rep                *rep,
                                           const void                  *source,
                                           Function_Rep::ManagerOpCode  opCode)
{
    // NOTE: This function is optimized for space (to minimize template bloat),
    // hence the code that can be shared, such as 'e_COPY_CONSTRUCT' and
    // 'e_CONSTRUCT', is shared via 'FALL THROUGH'.

    switch(opCode) {
      case Function_Rep::e_MOVE_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // instance, and 'rep' must be empty (or a function pointer).  Note
        // that 'source' is left in a empty state but must be zero-initialized
        // by the caller upon return.

        const Function_Rep *sourceRep
            = (const Function_Rep *) source;

        BSLS_ASSERT_SAFE(sourceRep->d_allocator_p == rep->d_allocator_p);
        bsl::memcpy((FUNC *)&rep->d_arena,
                    &sourceRep->d_arena,
                    sizeof(FUNC));

        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_COPY_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // instance, and 'rep' must be empty (or a function pointer).

        source = &((const Function_Rep *)source)->d_arena;
      }                                                         // FALL THROUGH
      case Function_Rep::e_CONSTRUCT: {
        // 'source' is a 'FUNC *', and 'rep' must be empty (or a function
        // pointer).

        bslalg::ScalarPrimitives::copyConstruct((FUNC *)&rep->d_arena,
                                                *(const FUNC *)source,
                                                rep->d_allocator_p);
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_DESTROY: {
        // 'rep' stores a 'FUNC' instance, and 'source' is irrelevant.

        ((FUNC *)&rep->d_arena)->~FUNC();
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_IN_PLACE_DETECTION: {
        return true;                                                  // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
    }
    return false;  // quell warnings
}

template <class FUNC>
bool Function_RepUtil::inplaceManager(Function_Rep                *rep,
                                      const void                  *source,
                                      Function_Rep::ManagerOpCode  opCode)
{
    // NOTE: This function is optimized for space (to minimize template bloat),
    // hence the code that can be shared, such as 'e_COPY_CONSTRUCT' and
    // 'e_CONSTRUCT', is shared via 'FALL THROUGH'.

    switch(opCode) {
      case Function_Rep::e_MOVE_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // instance, and 'rep' must be empty (or a function pointer).  Note
        // that 'source' is left in a empty state but must be zero-initialized
        // by the caller upon return.

        BSLS_ASSERT_SAFE(((const Function_Rep *)source)->d_allocator_p ==
                                                           rep->d_allocator_p);

        source = &((const Function_Rep *)source)->d_arena;
        bslalg::ScalarPrimitives::copyConstruct((FUNC *)&rep->d_arena,
                                                *(const FUNC *)source,
                                                rep->d_allocator_p);

        const_cast<FUNC *>((const FUNC *)source)->~FUNC();
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_COPY_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // instance, and 'rep' must be empty (or a function pointer).

        source = &((const Function_Rep *)source)->d_arena;
      }                                                         // FALL THROUGH
      case Function_Rep::e_CONSTRUCT: {
        // 'source' is a 'FUNC *', and 'rep' must be empty (or a function
        // pointer).

        bslalg::ScalarPrimitives::copyConstruct((FUNC *)&rep->d_arena,
                                                *(const FUNC *)source,
                                                rep->d_allocator_p);
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_DESTROY: {
        // 'rep' stores a 'FUNC' instance, and 'source' is irrelevant.

        ((FUNC *)&rep->d_arena)->~FUNC();
        return true;                                                  // RETURN
      } break;
      case Function_Rep::e_IN_PLACE_DETECTION: {
        return true;                                                  // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
    }
    return false;  // quell warnings
}

template <class FUNC>
bool Function_RepUtil::outofplaceManager(Function_Rep                *rep,
                                         const void                  *source,
                                         Function_Rep::ManagerOpCode  opCode)
{
    // NOTE: This function is optimized for space (to minimize template bloat),
    // hence the code that can be shared, such as 'e_COPY_CONSTRUCT' and
    // 'e_CONSTRUCT', is shared via 'FALL THROUGH'.

    switch(opCode) {
      case Function_Rep::e_MOVE_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // pointer, and 'rep' must be empty (or a function pointer).  Note
        // that 'source' is left in a empty state but must be zero-initialized
        // by the caller upon return.

        BSLS_ASSERT_SAFE(((const Function_Rep *)source)->d_allocator_p ==
                                                           rep->d_allocator_p);

        rep->d_arena.d_object_p =
                       ((const Function_Rep *)source)->d_arena.d_object_p;
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_COPY_CONSTRUCT: {
        // 'source' is interpreted as a 'Function_Rep *' storing a 'FUNC'
        // pointer, and 'rep' must be empty (or a function pointer).

        source = ((const Function_Rep *)source)->d_arena.d_object_p;
      }                                                         // FALL THROUGH
      case Function_Rep::e_CONSTRUCT: {
        // 'source' is a 'FUNC *', and 'rep' must be empty (or a function
        // pointer).

        FUNC *tempPtr = (FUNC *)rep->d_allocator_p->allocate(sizeof(FUNC));

        bslma::DeallocatorProctor<bslma::Allocator> guard(tempPtr,
                                                          rep->d_allocator_p);

        bslalg::ScalarPrimitives::copyConstruct(tempPtr,
                                                *(const FUNC *)source,
                                                rep->d_allocator_p);
        guard.release();
        rep->d_arena.d_object_p = tempPtr;
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_DESTROY: {
        // 'rep' stores a 'FUNC' instance, and 'source' is irrelevant.

        ((FUNC *)rep->d_arena.d_object_p)->~FUNC();
        rep->d_allocator_p->deallocate(rep->d_arena.d_object_p);
        return false;                                                 // RETURN
      } break;
      case Function_Rep::e_IN_PLACE_DETECTION: {
        return false;                                                 // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
    }
    return true;  // quell warnings
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
